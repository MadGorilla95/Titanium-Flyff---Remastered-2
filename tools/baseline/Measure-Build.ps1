[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Join-Path $PSScriptRoot "..\.."),
    [string]$Solution = "",
    [string]$Configuration = "Release",
    [string]$Platform = "x64",
    [string[]]$AdditionalArguments = @(),
    [string]$OutputDirectory = ""
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Resolve-MSBuildPath {
    $vswhereCandidates = @(
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe",
        "$env:ProgramFiles\Microsoft Visual Studio\Installer\vswhere.exe"
    ) | Where-Object { -not [string]::IsNullOrWhiteSpace($_) }

    foreach ($vswhere in $vswhereCandidates) {
        if (Test-Path -LiteralPath $vswhere) {
            $result = & $vswhere -latest -products * -requires Microsoft.Component.MSBuild -find "MSBuild\**\Bin\MSBuild.exe" 2>$null |
                Select-Object -First 1
            if ($result -and (Test-Path -LiteralPath $result)) {
                return (Resolve-Path -LiteralPath $result).Path
            }
        }
    }

    $command = Get-Command msbuild.exe -ErrorAction SilentlyContinue
    if ($null -ne $command) {
        return $command.Source
    }

    throw "MSBuild.exe was not found. Install Visual Studio Build Tools or run from a Developer PowerShell."
}

function Resolve-SolutionPath {
    param(
        [Parameter(Mandatory = $true)][string]$Root,
        [string]$RequestedSolution
    )

    if (-not [string]::IsNullOrWhiteSpace($RequestedSolution)) {
        $candidate = $RequestedSolution
        if (-not [System.IO.Path]::IsPathRooted($candidate)) {
            $candidate = Join-Path $Root $candidate
        }
        return (Resolve-Path -LiteralPath $candidate).Path
    }

    $solutions = @(
        Get-ChildItem -LiteralPath $Root -Recurse -File -Filter *.sln |
            Where-Object {
                $_.FullName -notmatch '[\\/](?:\.git|\.vs|Debug|Release|x64|x86|Output|artifacts)[\\/]'
            }
    )

    if ($solutions.Count -eq 1) {
        return $solutions[0].FullName
    }

    if ($solutions.Count -eq 0) {
        throw "No .sln file was found under '$Root'. Set build.solution in the local config."
    }

    $candidateList = ($solutions | ForEach-Object { " - $($_.FullName)" }) -join [Environment]::NewLine
    throw "Multiple .sln files were found. Set build.solution explicitly:$([Environment]::NewLine)$candidateList"
}

function Get-DiagnosticClassification {
    param(
        [Parameter(Mandatory = $true)][string]$Code,
        [Parameter(Mandatory = $true)][string]$Line
    )

    if ($Code -eq "MSB8041" -or $Line -match "(?i)MFC libraries are required") {
        return "missing-visual-studio-component"
    }
    if ($Code -eq "C1083" -and $Line -match "(?i)afx(?:win)?\.h") {
        return "missing-mfc-header"
    }
    if ($Code -eq "C1083") {
        return "missing-include-or-sdk"
    }
    if ($Code -eq "LNK1104") {
        return "missing-link-input"
    }
    if ($Code -match "^C\d+$") {
        return "compiler-diagnostic"
    }
    if ($Code -match "^LNK\d+$") {
        return "linker-diagnostic"
    }
    if ($Code -match "^MSB\d+$") {
        return "build-system-diagnostic"
    }
    return "unclassified-diagnostic"
}

function Normalize-DiagnosticLine {
    param([Parameter(Mandatory = $true)][string]$Line)

    $normalized = $Line -replace '^\s*\d+>', ''
    return $normalized.Trim()
}

$repositoryPath = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$solutionPath = Resolve-SolutionPath -Root $repositoryPath -RequestedSolution $Solution
$msbuildPath = Resolve-MSBuildPath

if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $OutputDirectory = Join-Path $repositoryPath "artifacts\baseline\build"
}
$OutputDirectory = [System.IO.Path]::GetFullPath($OutputDirectory)
[void](New-Item -ItemType Directory -Path $OutputDirectory -Force)

$logPath = Join-Path $OutputDirectory "msbuild.log"
$summaryPath = Join-Path $OutputDirectory "build-summary.json"
$diagnosticsPath = Join-Path $OutputDirectory "build-diagnostics.csv"

$arguments = @(
    $solutionPath,
    "/nologo",
    "/m",
    "/t:Rebuild",
    "/p:Configuration=$Configuration",
    "/p:Platform=$Platform",
    "/verbosity:normal"
)
if ($AdditionalArguments.Count -gt 0) {
    $arguments += $AdditionalArguments
}

$versionText = (& $msbuildPath -version -nologo 2>$null | Out-String).Trim()
$stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
$outputLines = New-Object System.Collections.ArrayList

Write-Host "MSBuild: $msbuildPath"
Write-Host "Solution: $solutionPath"
Write-Host "Configuration: $Configuration / $Platform"

& $msbuildPath @arguments 2>&1 |
    ForEach-Object {
        $line = $_.ToString()
        [void]$outputLines.Add($line)
        Write-Host $line
    }

$exitCode = $LASTEXITCODE
$stopwatch.Stop()
$outputLines | Set-Content -LiteralPath $logPath -Encoding UTF8

$rawWarningLineCount = 0
$rawErrorLineCount = 0
$reportedWarningCount = $null
$reportedErrorCount = $null
$diagnostics = New-Object System.Collections.ArrayList
$seenDiagnostics = @{}
$failedProjects = New-Object System.Collections.ArrayList

foreach ($line in $outputLines) {
    if ($line -match '^\s*(?<count>\d+)\s+Warning\(s\)\s*$') {
        $reportedWarningCount = [int]$Matches.count
    }
    if ($line -match '^\s*(?<count>\d+)\s+Error\(s\)\s*$') {
        $reportedErrorCount = [int]$Matches.count
    }
    if ($line -match '(?i)Done Building Project\s+"(?<project>[^"]+)".*--\s+FAILED\.') {
        $projectPath = $Matches.project
        if (-not $failedProjects.Contains($projectPath)) {
            [void]$failedProjects.Add($projectPath)
        }
    }

    $severity = $null
    $code = $null
    if ($line -match '(?i)\bwarning\s+(?<code>[A-Z]+\d+)\s*:') {
        $severity = "warning"
        $code = $Matches.code.ToUpperInvariant()
        $rawWarningLineCount++
    }
    elseif ($line -match '(?i)\berror\s+(?<code>[A-Z]+\d+)\s*:') {
        $severity = "error"
        $code = $Matches.code.ToUpperInvariant()
        $rawErrorLineCount++
    }

    if ($null -eq $severity) { continue }

    $normalizedLine = Normalize-DiagnosticLine -Line $line
    $key = "$severity|$code|$normalizedLine"
    if ($seenDiagnostics.ContainsKey($key)) { continue }
    $seenDiagnostics[$key] = $true

    [void]$diagnostics.Add([pscustomobject]@{
        Severity = $severity
        Code = $code
        Classification = Get-DiagnosticClassification -Code $code -Line $normalizedLine
        Line = $normalizedLine
    })
}

$warningMatches = @($diagnostics | Where-Object Severity -eq "warning")
$errorMatches = @($diagnostics | Where-Object Severity -eq "error")

$warningCodes = @(
    $warningMatches |
        Group-Object Code |
        ForEach-Object {
            [pscustomobject]@{
                Code = $_.Name
                Count = $_.Count
            }
        } |
        Sort-Object Count -Descending
)

$errorCodes = @(
    $errorMatches |
        Group-Object Code |
        ForEach-Object {
            [pscustomobject]@{
                Code = $_.Name
                Count = $_.Count
            }
        } |
        Sort-Object Count -Descending
)

$failureClassifications = @(
    $errorMatches |
        Group-Object Classification |
        ForEach-Object {
            [pscustomobject]@{
                Classification = $_.Name
                Count = $_.Count
            }
        } |
        Sort-Object Count -Descending
)

$parserMatchesReportedWarnings = if ($null -eq $reportedWarningCount) {
    $null
} else {
    [bool]($warningMatches.Count -eq $reportedWarningCount)
}
$parserMatchesReportedErrors = if ($null -eq $reportedErrorCount) {
    $null
} else {
    [bool]($errorMatches.Count -eq $reportedErrorCount)
}

$summary = [ordered]@{
    schemaVersion = 2
    generatedAtUtc = [DateTime]::UtcNow.ToString("o")
    repositoryRoot = $repositoryPath
    solution = $solutionPath
    configuration = $Configuration
    platform = $Platform
    msbuildPath = $msbuildPath
    msbuildVersion = $versionText
    arguments = @($arguments)
    elapsedMilliseconds = [math]::Round($stopwatch.Elapsed.TotalMilliseconds, 3)
    exitCode = $exitCode
    succeeded = [bool]($exitCode -eq 0)
    warningCount = $warningMatches.Count
    errorCount = $errorMatches.Count
    rawWarningLineCount = $rawWarningLineCount
    rawErrorLineCount = $rawErrorLineCount
    reportedWarningCount = $reportedWarningCount
    reportedErrorCount = $reportedErrorCount
    parserMatchesReportedWarnings = $parserMatchesReportedWarnings
    parserMatchesReportedErrors = $parserMatchesReportedErrors
    warningCodes = @($warningCodes)
    errorCodes = @($errorCodes)
    failureClassifications = @($failureClassifications)
    failedProjects = @($failedProjects)
    diagnosticCount = $diagnostics.Count
    diagnosticsPath = $diagnosticsPath
    logPath = $logPath
    warningObservability = "Compiler warning counts are incomplete evidence when project configurations use /W0 or TurnOffAllWarnings."
}

@($diagnostics) | Export-Csv -LiteralPath $diagnosticsPath -NoTypeInformation -Encoding UTF8
$summary | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath $summaryPath -Encoding UTF8

if ($exitCode -ne 0) {
    Write-Warning "Build failed with exit code $exitCode. The failure is recorded as baseline evidence; no source was changed."
}

$result = [pscustomobject]@{
    SummaryPath = $summaryPath
    DiagnosticsPath = $diagnosticsPath
    LogPath = $logPath
    Succeeded = $summary.succeeded
    ExitCode = $exitCode
    ElapsedMilliseconds = $summary.elapsedMilliseconds
    WarningCount = $warningMatches.Count
    ErrorCount = $errorMatches.Count
}

# Do not leak the native MSBuild exit code into callers. Build failure is evidence in
# build-summary.json; tooling/runtime failures still throw through ErrorActionPreference.
$global:LASTEXITCODE = 0
$result
