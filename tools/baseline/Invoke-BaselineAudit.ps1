[CmdletBinding()]
param(
    [string]$ConfigPath = "",
    [switch]$IncludeBuild,
    [switch]$IncludeRuntime,
    [switch]$IncludeNetwork,
    [switch]$IncludeLogTimings
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Resolve-FromBase([string]$Base, [string]$Path) {
    if ([IO.Path]::IsPathRooted($Path)) { return [IO.Path]::GetFullPath($Path) }
    return [IO.Path]::GetFullPath((Join-Path $Base $Path))
}

function Get-OptionalProperty {
    param(
        [object]$Object,
        [Parameter(Mandatory = $true)][string]$Name,
        [object]$Default = $null
    )

    if ($null -eq $Object) { return $Default }
    $property = $Object.PSObject.Properties[$Name]
    if ($null -eq $property) { return $Default }
    return $property.Value
}

function Invoke-Stage {
    param(
        [string]$Name,
        [scriptblock]$Action,
        [System.Collections.ArrayList]$Stages
    )

    $started = [DateTime]::UtcNow
    Write-Host "`n== $Name =="
    try {
        $result = & $Action
        $finished = [DateTime]::UtcNow
        [void]$Stages.Add([pscustomobject]@{
            name = $Name
            status = "completed"
            startedUtc = $started.ToString("o")
            finishedUtc = $finished.ToString("o")
            durationSeconds = [math]::Round(($finished - $started).TotalSeconds, 3)
            message = ""
            result = $result
        })
        return $result
    }
    catch {
        $finished = [DateTime]::UtcNow
        [void]$Stages.Add([pscustomobject]@{
            name = $Name
            status = "failed"
            startedUtc = $started.ToString("o")
            finishedUtc = $finished.ToString("o")
            durationSeconds = [math]::Round(($finished - $started).TotalSeconds, 3)
            message = $_.Exception.Message
            result = $null
        })
        Write-Warning "$Name failed: $($_.Exception.Message)"
        return $null
    }
}

function Add-Skipped([string]$Name, [string]$Reason, [System.Collections.ArrayList]$Stages) {
    $now = [DateTime]::UtcNow.ToString("o")
    [void]$Stages.Add([pscustomobject]@{
        name = $Name
        status = "skipped"
        startedUtc = $now
        finishedUtc = $now
        durationSeconds = 0
        message = $Reason
        result = $null
    })
}

function Get-GitState([string]$Root) {
    $git = Get-Command git -ErrorAction SilentlyContinue
    if (-not $git) { return @{ available = $false } }

    Push-Location $Root
    try {
        $status = @(& $git.Source status --porcelain 2>$null)
        return [ordered]@{
            available = $true
            commit = ((& $git.Source rev-parse HEAD 2>$null | Out-String).Trim())
            branch = ((& $git.Source rev-parse --abbrev-ref HEAD 2>$null | Out-String).Trim())
            dirty = [bool]($status.Count -gt 0)
            changedPathCount = $status.Count
            version = ((& $git.Source --version 2>$null | Out-String).Trim())
        }
    }
    finally {
        Pop-Location
    }
}

$localConfig = Join-Path $PSScriptRoot "baseline.config.local.json"
$exampleConfig = Join-Path $PSScriptRoot "baseline.config.example.json"
if ([string]::IsNullOrWhiteSpace($ConfigPath)) {
    $ConfigPath = if (Test-Path $localConfig) { $localConfig } else { $exampleConfig }
}
$configFull = (Resolve-Path $ConfigPath).Path
$config = Get-Content $configFull -Raw | ConvertFrom-Json
$configDirectory = Split-Path $configFull -Parent
$repositoryRoot = Resolve-FromBase $configDirectory ([string]$config.repositoryRoot)
$artifactRoot = Resolve-FromBase $repositoryRoot ([string]$config.artifactRoot)
$runId = [DateTime]::UtcNow.ToString("yyyyMMdd-HHmmssZ")
$runDirectory = Join-Path $artifactRoot $runId
New-Item -ItemType Directory -Path $runDirectory -Force | Out-Null
Copy-Item $configFull (Join-Path $runDirectory "effective-config.json") -Force

$os = Get-CimInstance Win32_OperatingSystem -ErrorAction SilentlyContinue
$computer = Get-CimInstance Win32_ComputerSystem -ErrorAction SilentlyContinue
$processors = @(Get-CimInstance Win32_Processor -ErrorAction SilentlyContinue | ForEach-Object Name)
$environment = [ordered]@{
    capturedAtUtc = [DateTime]::UtcNow.ToString("o")
    computerName = $env:COMPUTERNAME
    userName = $env:USERNAME
    timeZone = [TimeZoneInfo]::Local.Id
    osCaption = if ($os) { $os.Caption } else { [Environment]::OSVersion.VersionString }
    osVersion = if ($os) { $os.Version } else { [Environment]::OSVersion.VersionString }
    osBuild = if ($os) { $os.BuildNumber } else { $null }
    powershellVersion = $PSVersionTable.PSVersion.ToString()
    powershellEdition = if ($PSVersionTable.PSEdition) { $PSVersionTable.PSEdition } else { "Desktop" }
    logicalProcessorCount = [Environment]::ProcessorCount
    processors = $processors
    totalPhysicalMemoryGB = if ($computer) { [math]::Round($computer.TotalPhysicalMemory / 1GB, 3) } else { $null }
    repositoryRoot = $repositoryRoot
    configPath = $configFull
    configSha256 = (Get-FileHash $configFull -Algorithm SHA256).Hash
    git = Get-GitState $repositoryRoot
}
$environment | ConvertTo-Json -Depth 8 | Set-Content (Join-Path $runDirectory "environment.json") -Encoding UTF8

Write-Host "Titanium Phase 0 baseline"
Write-Host "Repository: $repositoryRoot"
Write-Host "Output:     $runDirectory"
Write-Host "This runner does not start or stop Flyff or SQL Server processes."

$stages = New-Object System.Collections.ArrayList

Invoke-Stage "project-inventory" {
    & (Join-Path $PSScriptRoot "Get-ProjectInventory.ps1") `
        -RepositoryRoot $repositoryRoot `
        -SourceRoot ([string]$config.sourceRoot) `
        -OutputDirectory (Join-Path $runDirectory "inventory")
} $stages | Out-Null

Invoke-Stage "startup-topology" {
    & (Join-Path $PSScriptRoot "Get-StartupTopology.ps1") `
        -RepositoryRoot $repositoryRoot `
        -OutputDirectory (Join-Path $runDirectory "startup-topology")
} $stages | Out-Null

Invoke-Stage "build-policy-audit" {
    & (Join-Path $PSScriptRoot "Get-BuildPolicyAudit.ps1") `
        -RepositoryRoot $repositoryRoot `
        -SourceRoot ([string]$config.sourceRoot) `
        -OutputDirectory (Join-Path $runDirectory "build-policy")
} $stages | Out-Null

Invoke-Stage "repository-hygiene" {
    & (Join-Path $PSScriptRoot "Get-RepositoryHygiene.ps1") `
        -RepositoryRoot $repositoryRoot `
        -SourceRoot ([string]$config.sourceRoot) `
        -OutputDirectory (Join-Path $runDirectory "repository-hygiene")
} $stages | Out-Null

Invoke-Stage "static-hotspots" {
    & (Join-Path $PSScriptRoot "Find-StaticHotspots.ps1") `
        -RepositoryRoot $repositoryRoot `
        -SourceRoot ([string]$config.sourceRoot) `
        -OutputDirectory (Join-Path $runDirectory "static-scan") `
        -ExcludeDirectories @($config.staticScan.excludeDirectories)
} $stages | Out-Null

$runBuild = [bool]$config.build.enabled -or $IncludeBuild
if ($runBuild) {
    if ([string]::IsNullOrWhiteSpace([string]$config.build.solution)) {
        Add-Skipped "build-baseline" "build.solution is empty. Set the exact solution before measuring." $stages
    }
    else {
        Invoke-Stage "build-baseline" {
            & (Join-Path $PSScriptRoot "Measure-Build.ps1") `
                -RepositoryRoot $repositoryRoot `
                -Solution ([string]$config.build.solution) `
                -Configuration ([string]$config.build.configuration) `
                -Platform ([string]$config.build.platform) `
                -AdditionalArguments @($config.build.additionalArguments) `
                -OutputDirectory (Join-Path $runDirectory "build")
        } $stages | Out-Null
    }
}
else {
    Add-Skipped "build-baseline" "Enable build.enabled or pass -IncludeBuild." $stages
}

$runtimeProcessNames = @($config.runtime.processNames | ForEach-Object { [string]$_ } | Where-Object { $_ })
$runRuntime = [bool]$config.runtime.enabled -or $IncludeRuntime
$captureRuntimeTopology = [bool](Get-OptionalProperty -Object $config.runtime -Name "captureTopology" -Default $true)
$includeRuntimeCommandLine = [bool](Get-OptionalProperty -Object $config.runtime -Name "includeCommandLine" -Default $false)
$skipRuntimeFileHash = [bool](Get-OptionalProperty -Object $config.runtime -Name "skipFileHash" -Default $false)

if ($runRuntime -and $runtimeProcessNames.Count -gt 0) {
    if ($captureRuntimeTopology) {
        Invoke-Stage "runtime-topology" {
            $parameters = @{
                ProcessName = $runtimeProcessNames
                RepositoryRoot = $repositoryRoot
                OutputDirectory = (Join-Path $runDirectory "runtime-topology")
            }
            if ($includeRuntimeCommandLine) { $parameters.IncludeCommandLine = $true }
            if ($skipRuntimeFileHash) { $parameters.SkipFileHash = $true }
            & (Join-Path $PSScriptRoot "Get-RuntimeTopology.ps1") @parameters
        } $stages | Out-Null
    }
    else {
        Add-Skipped "runtime-topology" "runtime.captureTopology is false." $stages
    }

    Invoke-Stage "process-resources" {
        & (Join-Path $PSScriptRoot "Measure-ProcessResources.ps1") `
            -ProcessName $runtimeProcessNames `
            -DurationSeconds ([int]$config.runtime.durationSeconds) `
            -IntervalSeconds ([int]$config.runtime.sampleIntervalSeconds) `
            -RepositoryRoot $repositoryRoot `
            -OutputDirectory (Join-Path $runDirectory "process-resources")
    } $stages | Out-Null
}
elseif ($runRuntime) {
    Add-Skipped "runtime-topology" "runtime.processNames is empty." $stages
    Add-Skipped "process-resources" "runtime.processNames is empty." $stages
}
else {
    Add-Skipped "runtime-topology" "Enable runtime.enabled or pass -IncludeRuntime." $stages
    Add-Skipped "process-resources" "Enable runtime.enabled or pass -IncludeRuntime." $stages
}

$networkProcessNames = @($config.network.processNames | ForEach-Object { [string]$_ } | Where-Object { $_ })
if ($networkProcessNames.Count -eq 0) { $networkProcessNames = $runtimeProcessNames }
$runNetwork = [bool]$config.network.enabled -or $IncludeNetwork
if ($runNetwork) {
    Invoke-Stage "network-baseline" {
        & (Join-Path $PSScriptRoot "Measure-NetworkBaseline.ps1") `
            -ProcessName $networkProcessNames `
            -DurationSeconds ([int]$config.network.durationSeconds) `
            -IntervalSeconds ([int]$config.network.sampleIntervalSeconds) `
            -RepositoryRoot $repositoryRoot `
            -OutputDirectory (Join-Path $runDirectory "network")
    } $stages | Out-Null
}
else {
    Add-Skipped "network-baseline" "Enable network.enabled or pass -IncludeNetwork." $stages
}

$runTimings = [bool]$config.logs.enabled -or $IncludeLogTimings
if ($runTimings) {
    Invoke-Stage "log-timings" {
        & (Join-Path $PSScriptRoot "Measure-LogTimings.ps1") `
            -ConfigPath $configFull `
            -RepositoryRoot $repositoryRoot `
            -OutputDirectory (Join-Path $runDirectory "log-timings")
    } $stages | Out-Null
}
else {
    Add-Skipped "log-timings" "Enable logs.enabled or pass -IncludeLogTimings." $stages
}

$manifest = [ordered]@{
    schemaVersion = 3
    runId = $runId
    generatedAtUtc = [DateTime]::UtcNow.ToString("o")
    runDirectory = $runDirectory
    environmentPath = (Join-Path $runDirectory "environment.json")
    runPolicy = $config.runPolicy
    comparisonPolicy = Get-OptionalProperty -Object $config -Name "comparison" -Default $null
    stages = @($stages)
}
$manifestPath = Join-Path $runDirectory "baseline-manifest.json"
$manifest | ConvertTo-Json -Depth 14 | Set-Content $manifestPath -Encoding UTF8

$summary = New-Object Text.StringBuilder
[void]$summary.AppendLine("# Titanium Phase 0 baseline run")
[void]$summary.AppendLine("")
[void]$summary.AppendLine("- Run ID: $runId")
[void]$summary.AppendLine("- Git commit: $($environment.git.commit)")
[void]$summary.AppendLine("- Git branch: $($environment.git.branch)")
[void]$summary.AppendLine("- Dirty worktree: $($environment.git.dirty)")
[void]$summary.AppendLine("- Output: $runDirectory")
[void]$summary.AppendLine("")
[void]$summary.AppendLine("| Stage | Status | Duration (s) | Message |")
[void]$summary.AppendLine("|---|---|---:|---|")
foreach ($stage in $stages) {
    $message = ([string]$stage.message) -replace "\|", "\|"
    [void]$summary.AppendLine("| $($stage.name) | $($stage.status) | $($stage.durationSeconds) | $message |")
}
[void]$summary.AppendLine("")
[void]$summary.AppendLine("> Evidence only. Compare controlled runs with the same machine, build, database/data snapshot, world configuration, client route and workload.")
[void]$summary.AppendLine("")
[void]$summary.AppendLine("Compare this run later with: ``.\tools\baseline\Compare-BaselineRuns.ps1 -ReferenceRun <baseline> -CandidateRun '$runDirectory'``")
$summaryPath = Join-Path $runDirectory "RUN-SUMMARY.md"
$summary.ToString() | Set-Content $summaryPath -Encoding UTF8

[pscustomobject]@{
    RunId = $runId
    RunDirectory = $runDirectory
    Manifest = $manifestPath
    Summary = $summaryPath
    CompletedStages = @($stages | Where-Object status -eq "completed").Count
    FailedStages = @($stages | Where-Object status -eq "failed").Count
    SkippedStages = @($stages | Where-Object status -eq "skipped").Count
}
