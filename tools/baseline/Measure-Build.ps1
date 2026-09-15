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
        if (-not (Test-Path -LiteralPath $vswhere)) {
            continue
        }

        $result = & $vswhere `
            -latest `
            -products * `
            -requires Microsoft.Component.MSBuild `
            -find "MSBuild\**\Bin\MSBuild.exe" 2>$null |
            Select-Object -First 1

        if ($result -and (Test-Path -LiteralPath $result)) {
            return (Resolve-Path -LiteralPath $result).Path
        }
    }

    $command = Get-Command msbuild.exe -ErrorAction SilentlyContinue
    if ($null -ne $command) {
        return $command.Source
    }

    throw "MSBuild.exe was not found. Install Visual Studio Build Tools with the C++ workload or run from Developer PowerShell."
}

function Resolve-SolutionPath {
    param(
        [Parameter(Mandatory = $true)][string]$Root,
        [string]$RequestedSolution
    )

    if (-not [string]::IsNullOrWhiteSpace($RequestedSolution)) {
        $candidate = $RequestedSolution
        if (-not [IO.Path]::IsPathRooted($candidate)) {
            $candidate = Join-Path $Root $candidate
        }
        return (Resolve-Path -LiteralPath $candidate).Path
    }

    $solutions = @(
        Get-ChildItem -LiteralPath $Root -Recurse -File -Filter *.sln -ErrorAction Stop |
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
$OutputDirectory = [IO.Path]::GetFullPath($OutputDirectory)
New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null

$consoleLogPath = Join-Path $OutputDirectory "msbuild.log"
$diagnosticLogPath = Join-Path $OutputDirectory "msbuild-diagnostic.log"

$arguments = @(
    $solutionPath,
    "/nologo",
    "/m",
    "/t:Rebuild",
    "/p:Configuration=$Configuration",
    "/p:Platform=$Platform",
    "/verbosity:normal",
    "/fl",
    "/flp:LogFile=$diagnosticLogPath;Verbosity=diagnostic"
)
if ($AdditionalArguments.Count -gt 0) {
    $arguments += $AdditionalArguments
}

$versionText = (& $msbuildPath -version -nologo 2>$null | Out-String).Trim()
$outputLines = New-Object Collections.ArrayList
$stopwatch = [Diagnostics.Stopwatch]::StartNew()

Write-Host "MSBuild:       $msbuildPath"
Write-Host "Solution:      $solutionPath"
Write-Host "Configuration: $Configuration|$Platform"
Write-Host "Evidence:      $OutputDirectory"

& $msbuildPath @arguments 2>&1 |
    ForEach-Object {
        $line = $_.ToString()
        [void]$outputLines.Add($line)
        Write-Host $line
    }

$exitCode = $LASTEXITCODE
$stopwatch.Stop()
@($outputLines) | Set-Content -LiteralPath $consoleLogPath -Encoding UTF8

$conversionArguments = @{
    LogPath = $consoleLogPath
    OutputDirectory = $OutputDirectory
    ExitCode = $exitCode
    RepositoryRoot = $repositoryPath
    Solution = $solutionPath
    Configuration = $Configuration
    Platform = $Platform
    MSBuildPath = $msbuildPath
    MSBuildVersion = $versionText
    ElapsedMilliseconds = $stopwatch.Elapsed.TotalMilliseconds
}

$result = & (Join-Path $PSScriptRoot 'Convert-MSBuildLog.ps1') @conversionArguments

$invocation = [ordered]@{
    generatedAtUtc = [DateTime]::UtcNow.ToString('o')
    msbuildPath = $msbuildPath
    msbuildVersion = $versionText
    solution = $solutionPath
    configuration = $Configuration
    platform = $Platform
    arguments = @($arguments)
    consoleLogPath = $consoleLogPath
    diagnosticLogPath = $diagnosticLogPath
}
$invocation | ConvertTo-Json -Depth 8 |
    Set-Content -LiteralPath (Join-Path $OutputDirectory 'build-invocation.json') -Encoding UTF8

if (-not $result.Succeeded) {
    Write-Warning "Canonical build did not succeed. This is retained as baseline evidence; no source was changed."
    if ($result.MissingBuildInputCount -gt 0) {
        Write-Warning "$($result.MissingBuildInputCount) distinct missing build input(s) were identified. See missing-build-inputs.csv."
    }
}

$result
