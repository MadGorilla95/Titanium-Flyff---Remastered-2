[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Join-Path $PSScriptRoot "..\.."),
    [string]$Destination = (Join-Path $PSScriptRoot "baseline.config.local.json"),
    [switch]$Force
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-RelativeDirectoryPath {
    param(
        [Parameter(Mandatory = $true)][string]$BaseDirectory,
        [Parameter(Mandatory = $true)][string]$TargetDirectory
    )

    $baseFull = [IO.Path]::GetFullPath($BaseDirectory).TrimEnd('\', '/') + [IO.Path]::DirectorySeparatorChar
    $targetFull = [IO.Path]::GetFullPath($TargetDirectory).TrimEnd('\', '/') + [IO.Path]::DirectorySeparatorChar
    $baseUri = [Uri]$baseFull
    $targetUri = [Uri]$targetFull
    $relative = [Uri]::UnescapeDataString($baseUri.MakeRelativeUri($targetUri).ToString())
    $relative = $relative.TrimEnd('/')

    if ([string]::IsNullOrWhiteSpace($relative)) {
        return "."
    }

    return $relative.Replace('/', [IO.Path]::DirectorySeparatorChar)
}

$repositoryPath = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$examplePath = Join-Path $PSScriptRoot "baseline.config.example.json"
if (-not (Test-Path -LiteralPath $examplePath -PathType Leaf)) {
    throw "Example configuration was not found: $examplePath"
}

$destinationPath = $Destination
if (-not [IO.Path]::IsPathRooted($destinationPath)) {
    $destinationPath = Join-Path $repositoryPath $destinationPath
}
$destinationPath = [IO.Path]::GetFullPath($destinationPath)
$destinationDirectory = Split-Path $destinationPath -Parent
New-Item -ItemType Directory -Path $destinationDirectory -Force | Out-Null

if ((Test-Path -LiteralPath $destinationPath) -and -not $Force) {
    Write-Host "Local configuration already exists and was not overwritten: $destinationPath"
    return [pscustomobject]@{
        Created = $false
        Overwritten = $false
        Path = $destinationPath
        RepositoryRoot = $repositoryPath
    }
}

$config = Get-Content -LiteralPath $examplePath -Raw | ConvertFrom-Json
$config.repositoryRoot = Get-RelativeDirectoryPath `
    -BaseDirectory $destinationDirectory `
    -TargetDirectory $repositoryPath

$config.build.enabled = $false
$config.build.solution = "Source/Source/all/all.sln"
$config.build.configuration = "Release"
$config.build.platform = "x64"

$processNames = @(
    "1. Account-x64",
    "2. Certifier-x64",
    "3. Database-x64",
    "4. Core-x64",
    "5. Login-x64",
    "6. Cache-x64",
    "7. World-x64"
)
$config.runtime.enabled = $false
$config.runtime.processNames = $processNames
$config.network.enabled = $false
$config.network.processNames = $processNames
$config.logs.enabled = $false

$config | ConvertTo-Json -Depth 20 |
    Set-Content -LiteralPath $destinationPath -Encoding UTF8

Write-Host "Titanium baseline configuration created: $destinationPath"
Write-Host "Build target: Source/Source/all/all.sln (Release|x64)"
Write-Host "Runtime and network measurements remain disabled until explicitly requested."
Write-Host "Log timings remain disabled until real log paths and markers are configured."

[pscustomobject]@{
    Created = $true
    Overwritten = [bool]$Force
    Path = $destinationPath
    RepositoryRoot = $repositoryPath
    ProcessNames = $processNames
}
