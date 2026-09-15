[CmdletBinding()]
param(
    [ValidateSet('Static', 'Build', 'Runtime', 'Full')]
    [string]$Mode = 'Static',

    [string]$ConfigPath = "",
    [switch]$IncludeBuild,
    [switch]$IncludeRuntime,
    [switch]$IncludeNetwork,
    [switch]$IncludeLogTimings,
    [string[]]$BuildInputSearchRoot = @(),
    [switch]$IncludeAbsoluteBuildInputPaths,
    [switch]$SkipBuildInputProvenance,
    [switch]$SkipPreflight,
    [switch]$AllowPreflightErrors,
    [switch]$ForceInitializeConfig
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$localConfig = Join-Path $PSScriptRoot 'baseline.config.local.json'
if ([string]::IsNullOrWhiteSpace($ConfigPath)) {
    $ConfigPath = $localConfig
}

if ($ForceInitializeConfig -or -not (Test-Path -LiteralPath $ConfigPath -PathType Leaf)) {
    if ($ForceInitializeConfig) {
        Write-Host 'Recreating the local Titanium baseline configuration because -ForceInitializeConfig was supplied.'
    }
    else {
        Write-Host 'No local baseline configuration was found. Creating a safe Titanium configuration.'
    }

    $initialized = & (Join-Path $PSScriptRoot 'Initialize-TitaniumBaselineConfig.ps1') `
        -Destination $ConfigPath `
        -Force:$ForceInitializeConfig
    $ConfigPath = $initialized.Path
}
else {
    $ConfigPath = (Resolve-Path -LiteralPath $ConfigPath).Path
}

$config = Get-Content -LiteralPath $ConfigPath -Raw | ConvertFrom-Json

$provenanceResult = $null
if (-not $SkipBuildInputProvenance) {
    $maxCandidates = 100
    if ($config.build.PSObject.Properties['provenanceMaxCandidatesPerInput']) {
        $maxCandidates = [int]$config.build.provenanceMaxCandidatesPerInput
    }

    $includeAbsolutePaths = [bool]$IncludeAbsoluteBuildInputPaths
    if ($config.build.PSObject.Properties['provenanceIncludeAbsoluteExternalPaths']) {
        $includeAbsolutePaths = $includeAbsolutePaths -or [bool]$config.build.provenanceIncludeAbsoluteExternalPaths
    }

    $scanLegacyPreprocessed = $true
    if ($config.build.PSObject.Properties['provenanceScanLegacyPreprocessedReferences']) {
        $scanLegacyPreprocessed = [bool]$config.build.provenanceScanLegacyPreprocessedReferences
    }

    $provenanceParameters = @{
        ConfigPath = $ConfigPath
        MaxCandidatesPerInput = $maxCandidates
    }
    if ($BuildInputSearchRoot.Count -gt 0) {
        $provenanceParameters.AdditionalSearchRoot = $BuildInputSearchRoot
    }
    if ($includeAbsolutePaths) {
        $provenanceParameters.IncludeAbsoluteExternalPaths = $true
    }
    if (-not $scanLegacyPreprocessed) {
        $provenanceParameters.SkipLegacyPreprocessedReferences = $true
    }

    Write-Host ''
    Write-Host 'Resolving Titanium build-input provenance...'
    $provenanceResult = & (Join-Path $PSScriptRoot 'Resolve-BuildInputOrigins.ps1') @provenanceParameters
    Write-Host "Build inputs found: $($provenanceResult.FoundInputCount)/$($provenanceResult.RequiredInputCount)"
    Write-Host "Provenance report: $($provenanceResult.Markdown)"
}

$runBuild = [bool]$IncludeBuild
$runRuntime = [bool]$IncludeRuntime
$runNetwork = [bool]$IncludeNetwork
$runLogTimings = [bool]$IncludeLogTimings

switch ($Mode) {
    'Build' {
        $runBuild = $true
    }
    'Runtime' {
        $runRuntime = $true
        $runNetwork = $true
    }
    'Full' {
        $runBuild = $true
        $runRuntime = $true
        $runNetwork = $true

        $enabledLogTimingCount = @(
            $config.logs.timings |
                Where-Object {
                    [bool]$_.enabled -and
                    -not [string]::IsNullOrWhiteSpace([string]$_.startPattern) -and
                    -not [string]::IsNullOrWhiteSpace([string]$_.endPattern)
                }
        ).Count
        if ([bool]$config.logs.enabled -and $enabledLogTimingCount -gt 0) {
            $runLogTimings = $true
        }
    }
}

$preflightResult = $null
if (-not $SkipPreflight) {
    $preflightParameters = @{
        ConfigPath = $ConfigPath
    }
    if ($runBuild) { $preflightParameters.RequireBuild = $true }
    if ($runRuntime) { $preflightParameters.RequireRuntime = $true }
    if ($runNetwork) { $preflightParameters.RequireNetwork = $true }
    if ($runLogTimings) { $preflightParameters.RequireLogTimings = $true }

    $preflightResult = & (Join-Path $PSScriptRoot 'Test-BaselinePrerequisites.ps1') @preflightParameters
    if (-not $preflightResult.Success -and -not $AllowPreflightErrors) {
        throw "Preflight failed with $($preflightResult.ErrorCount) error(s). Resolve them or rerun with -AllowPreflightErrors to collect failure evidence intentionally. Report: $($preflightResult.MarkdownPath)"
    }
}

$auditParameters = @{
    ConfigPath = $ConfigPath
}
if ($runBuild) { $auditParameters.IncludeBuild = $true }
if ($runRuntime) { $auditParameters.IncludeRuntime = $true }
if ($runNetwork) { $auditParameters.IncludeNetwork = $true }
if ($runLogTimings) { $auditParameters.IncludeLogTimings = $true }

Write-Host ''
Write-Host 'Titanium Phase 0 execution'
Write-Host "Mode:          $Mode"
Write-Host "Configuration: $ConfigPath"
Write-Host "Build:         $runBuild"
Write-Host "Runtime:       $runRuntime"
Write-Host "Network:       $runNetwork"
Write-Host "Log timings:   $runLogTimings"
Write-Host ''

$auditResult = & (Join-Path $PSScriptRoot 'Invoke-BaselineAudit.ps1') @auditParameters

$buildSucceeded = $null
$buildSummaryPath = Join-Path $auditResult.RunDirectory 'build\build-summary.json'
if (Test-Path -LiteralPath $buildSummaryPath -PathType Leaf) {
    $buildSummary = Get-Content -LiteralPath $buildSummaryPath -Raw | ConvertFrom-Json
    $buildSucceeded = [bool]$buildSummary.succeeded
}

$overallSuccess = [bool]($auditResult.FailedStages -eq 0)
if ($runBuild -and $null -ne $buildSucceeded -and -not $buildSucceeded) {
    $overallSuccess = $false
    Write-Warning 'The audit completed, but the canonical build did not succeed. Treat the build report as baseline evidence.'
}

[pscustomobject]@{
    Success = $overallSuccess
    Mode = $Mode
    ConfigPath = $ConfigPath
    BuildInputProvenance = $provenanceResult
    Preflight = $preflightResult
    Audit = $auditResult
    RunDirectory = $auditResult.RunDirectory
    Summary = $auditResult.Summary
    BuildSummary = $(if (Test-Path -LiteralPath $buildSummaryPath) { $buildSummaryPath } else { $null })
    BuildSucceeded = $buildSucceeded
}
