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

$warningMatches = New-Object System.Collections.ArrayList
$errorMatches = New-Object System.Collections.ArrayList

foreach ($line in $outputLines) {
    if ($line -match '(?i)\bwarning\s+(?<code>[A-Z]+\d+)\s*:') {
        [void]$warningMatches.Add([pscustomobject]@{
            Code = $Matches.code.ToUpperInvariant()
            Line = $line
        })
    }
    if ($line -match '(?i)\berror\s+(?<code>[A-Z]+\d+)\s*:') {
        [void]$errorMatches.Add([pscustomobject]@{
            Code = $Matches.code.ToUpperInvariant()
            Line = $line
        })
    }
}

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

$summary = [ordered]@{
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
    warningCodes = @($warningCodes)
    errorCodes = @($errorCodes)
    logPath = $logPath
}

$summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $summaryPath -Encoding UTF8

if ($exitCode -ne 0) {
    Write-Warning "Build failed with exit code $exitCode. The failure is recorded as baseline evidence; no source was changed."
}

[pscustomobject]@{
    SummaryPath = $summaryPath
    LogPath = $logPath
    Succeeded = $summary.succeeded
    ExitCode = $exitCode
    ElapsedMilliseconds = $summary.elapsedMilliseconds
    WarningCount = $warningMatches.Count
    ErrorCount = $errorMatches.Count
}
