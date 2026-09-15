[CmdletBinding()]
param(
    [string]$ConfigPath = "",
    [string]$OutputDirectory = "",
    [switch]$RequireBuild,
    [switch]$RequireRuntime,
    [switch]$RequireNetwork,
    [switch]$RequireLogTimings,
    [switch]$FailOnError
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Resolve-FromBase {
    param([string]$Base, [string]$Path)

    if ([IO.Path]::IsPathRooted($Path)) {
        return [IO.Path]::GetFullPath($Path)
    }

    return [IO.Path]::GetFullPath((Join-Path $Base $Path))
}

function Add-Check {
    param(
        [System.Collections.ArrayList]$Checks,
        [string]$Category,
        [string]$Name,
        [ValidateSet('pass', 'warning', 'error', 'info')][string]$Status,
        [string]$Message,
        [object]$Evidence = $null
    )

    [void]$Checks.Add([pscustomobject]@{
        Category = $Category
        Name = $Name
        Status = $Status
        Message = $Message
        Evidence = $Evidence
    })
}

function Find-MSBuildPath {
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

    return $null
}

function Test-IsAdministrator {
    try {
        $identity = [Security.Principal.WindowsIdentity]::GetCurrent()
        $principal = New-Object Security.Principal.WindowsPrincipal($identity)
        return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
    }
    catch {
        return $false
    }
}

function Convert-ToMarkdownCell {
    param([object]$Value)

    if ($null -eq $Value) {
        return ''
    }

    return ([string]$Value).Replace('|', '\|').Replace("`r", ' ').Replace("`n", ' ')
}

$localConfig = Join-Path $PSScriptRoot 'baseline.config.local.json'
$exampleConfig = Join-Path $PSScriptRoot 'baseline.config.example.json'
if ([string]::IsNullOrWhiteSpace($ConfigPath)) {
    $ConfigPath = if (Test-Path -LiteralPath $localConfig) { $localConfig } else { $exampleConfig }
}

$configFullPath = (Resolve-Path -LiteralPath $ConfigPath).Path
$configDirectory = Split-Path $configFullPath -Parent
$config = Get-Content -LiteralPath $configFullPath -Raw | ConvertFrom-Json
$repositoryRoot = Resolve-FromBase -Base $configDirectory -Path ([string]$config.repositoryRoot)
$sourceRoot = Resolve-FromBase -Base $repositoryRoot -Path ([string]$config.sourceRoot)
$artifactRoot = Resolve-FromBase -Base $repositoryRoot -Path ([string]$config.artifactRoot)

if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $runId = [DateTime]::UtcNow.ToString('yyyyMMdd-HHmmssZ')
    $OutputDirectory = Join-Path $artifactRoot (Join-Path 'preflight' $runId)
}
$OutputDirectory = [IO.Path]::GetFullPath($OutputDirectory)
New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null

$checks = New-Object Collections.ArrayList
$buildRequested = [bool]$RequireBuild -or [bool]$config.build.enabled
$runtimeRequested = [bool]$RequireRuntime -or [bool]$config.runtime.enabled
$networkRequested = [bool]$RequireNetwork -or [bool]$config.network.enabled
$logsRequested = [bool]$RequireLogTimings -or [bool]$config.logs.enabled

Add-Check $checks 'environment' 'Windows operating system' `
    $(if ($env:OS -eq 'Windows_NT') { 'pass' } else { 'error' }) `
    $(if ($env:OS -eq 'Windows_NT') { 'Windows environment detected.' } else { 'Titanium build/runtime tooling requires Windows.' }) `
    ([Environment]::OSVersion.VersionString)

$psVersion = $PSVersionTable.PSVersion
$psStatus = if ($psVersion -ge [Version]'5.1') { 'pass' } else { 'error' }
Add-Check $checks 'environment' 'PowerShell version' $psStatus `
    "PowerShell $psVersion detected; 5.1 or newer is required." `
    $psVersion.ToString()

Add-Check $checks 'configuration' 'Configuration file' 'pass' `
    'Configuration parsed successfully.' $configFullPath

$schemaVersion = if ($config.PSObject.Properties['schemaVersion']) { [int]$config.schemaVersion } else { 0 }
Add-Check $checks 'configuration' 'Schema version' `
    $(if ($schemaVersion -ge 2) { 'pass' } else { 'error' }) `
    "Configuration schema version is $schemaVersion." $schemaVersion

Add-Check $checks 'repository' 'Repository root' `
    $(if (Test-Path -LiteralPath $repositoryRoot -PathType Container) { 'pass' } else { 'error' }) `
    "Repository root: $repositoryRoot" $repositoryRoot

Add-Check $checks 'repository' 'Source root' `
    $(if (Test-Path -LiteralPath $sourceRoot -PathType Container) { 'pass' } else { 'error' }) `
    "Source root: $sourceRoot" $sourceRoot

try {
    New-Item -ItemType Directory -Path $artifactRoot -Force | Out-Null
    $probe = Join-Path $artifactRoot ('.write-test-' + [Guid]::NewGuid().ToString('N') + '.tmp')
    'phase0' | Set-Content -LiteralPath $probe -Encoding ASCII
    Remove-Item -LiteralPath $probe -Force
    Add-Check $checks 'repository' 'Artifact output is writable' 'pass' `
        "Audit evidence can be written to $artifactRoot" $artifactRoot
}
catch {
    Add-Check $checks 'repository' 'Artifact output is writable' 'error' `
        "Cannot write to ${artifactRoot}: $($_.Exception.Message)" $artifactRoot
}

try {
    $driveName = ([IO.Path]::GetPathRoot($artifactRoot)).TrimEnd('\').TrimEnd(':')
    $drive = Get-PSDrive -Name $driveName -ErrorAction Stop
    $freeGB = [math]::Round($drive.Free / 1GB, 2)
    $spaceStatus = if ($freeGB -ge 5) { 'pass' } elseif ($freeGB -ge 2) { 'warning' } else { 'error' }
    Add-Check $checks 'environment' 'Free disk space' $spaceStatus `
        "$freeGB GB free on the artifact drive." $freeGB
}
catch {
    Add-Check $checks 'environment' 'Free disk space' 'warning' `
        "Free disk space could not be determined: $($_.Exception.Message)" $null
}

$git = Get-Command git -ErrorAction SilentlyContinue
if ($null -eq $git) {
    Add-Check $checks 'repository' 'Git state' 'warning' 'Git is unavailable; commit and dirty-worktree evidence cannot be captured.' $null
}
else {
    Push-Location $repositoryRoot
    try {
        $commit = ((& $git.Source rev-parse HEAD 2>$null | Out-String).Trim())
        $branch = ((& $git.Source rev-parse --abbrev-ref HEAD 2>$null | Out-String).Trim())
        $changes = @(& $git.Source status --porcelain 2>$null)
        $gitStatus = if ($changes.Count -eq 0) { 'pass' } else { 'warning' }
        Add-Check $checks 'repository' 'Git state' $gitStatus `
            "Branch=$branch; commit=$commit; changed paths=$($changes.Count)." `
            ([ordered]@{ branch = $branch; commit = $commit; changedPathCount = $changes.Count })
    }
    finally {
        Pop-Location
    }
}

$solutionPath = ''
if (-not [string]::IsNullOrWhiteSpace([string]$config.build.solution)) {
    $solutionPath = Resolve-FromBase -Base $repositoryRoot -Path ([string]$config.build.solution)
}
$solutionExists = -not [string]::IsNullOrWhiteSpace($solutionPath) -and (Test-Path -LiteralPath $solutionPath -PathType Leaf)
$solutionStatus = if ($solutionExists) { 'pass' } elseif ($buildRequested) { 'error' } else { 'warning' }
Add-Check $checks 'build' 'Canonical solution' $solutionStatus `
    $(if ($solutionExists) { "Canonical solution found: $solutionPath" } else { "Canonical solution is missing or not configured: $solutionPath" }) `
    $solutionPath

$msbuildPath = Find-MSBuildPath
$msbuildStatus = if ($null -ne $msbuildPath) { 'pass' } elseif ($buildRequested) { 'error' } else { 'warning' }
Add-Check $checks 'build' 'MSBuild' $msbuildStatus `
    $(if ($null -ne $msbuildPath) { "MSBuild found: $msbuildPath" } else { 'MSBuild was not found.' }) `
    $msbuildPath

$requiredInputs = @()
if ($config.build.PSObject.Properties['requiredInputs']) {
    $requiredInputs = @($config.build.requiredInputs)
}

foreach ($requiredInput in $requiredInputs) {
    $name = [string]$requiredInput.name
    $matches = New-Object Collections.ArrayList

    foreach ($candidate in @($requiredInput.candidatePaths)) {
        $candidatePath = Resolve-FromBase -Base $repositoryRoot -Path ([string]$candidate)
        if (Test-Path -LiteralPath $candidatePath -PathType Leaf) {
            [void]$matches.Add($candidatePath)
        }
    }

    if ($matches.Count -eq 0 -and (Test-Path -LiteralPath (Join-Path $repositoryRoot 'Source'))) {
        foreach ($found in @(Get-ChildItem -LiteralPath (Join-Path $repositoryRoot 'Source') -Recurse -File -Filter $name -ErrorAction SilentlyContinue)) {
            if (-not $matches.Contains($found.FullName)) {
                [void]$matches.Add($found.FullName)
            }
        }
    }

    $inputStatus = if ($matches.Count -gt 0) { 'pass' } elseif ($buildRequested) { 'error' } else { 'warning' }
    $message = if ($matches.Count -gt 0) {
        "$name found at $($matches -join '; ')."
    }
    else {
        "$name was not found. The clean-clone Release|x64 baseline cannot compile without this generated/resource header."
    }

    Add-Check $checks 'build-input' $name $inputStatus $message @($matches)
}

$resourceDirectories = @(
    (Join-Path $repositoryRoot 'Source\resource'),
    (Join-Path $repositoryRoot 'Source\Source\Resource')
)
$presentResourceDirectories = @($resourceDirectories | Where-Object { Test-Path -LiteralPath $_ -PathType Container })
$resourceStatus = if ($presentResourceDirectories.Count -gt 0) { 'pass' } elseif ($buildRequested) { 'error' } else { 'warning' }
Add-Check $checks 'build-input' 'Titanium resource-header directory' $resourceStatus `
    $(if ($presentResourceDirectories.Count -gt 0) { "Resource directory found: $($presentResourceDirectories -join '; ')" } else { 'Neither Source\resource nor Source\Source\Resource exists.' }) `
    $presentResourceDirectories

$processNames = @($config.runtime.processNames | ForEach-Object { [string]$_ } | Where-Object { $_ })
if ($runtimeRequested) {
    if ($processNames.Count -eq 0) {
        Add-Check $checks 'runtime' 'Configured process names' 'error' 'runtime.processNames is empty.' $null
    }
    else {
        Add-Check $checks 'runtime' 'Configured process names' 'pass' `
            "$($processNames.Count) expected process names are configured." $processNames

        $observed = @()
        foreach ($name in $processNames) {
            $observed += @(Get-Process -Name $name -ErrorAction SilentlyContinue)
        }
        $observed = @($observed | Sort-Object Id -Unique)

        if ($observed.Count -eq 0) {
            Add-Check $checks 'runtime' 'Active Titanium processes' 'error' `
                'No configured Titanium process was observed. Start the server through its normal launcher before runtime measurement.' $null
        }
        elseif ($observed.Count -lt $processNames.Count) {
            Add-Check $checks 'runtime' 'Active Titanium processes' 'warning' `
                "$($observed.Count) process instance(s) observed for $($processNames.Count) configured roles. Missing roles must remain explicit in the baseline." `
                @($observed | Select-Object ProcessName, Id)
        }
        else {
            Add-Check $checks 'runtime' 'Active Titanium processes' 'pass' `
                "$($observed.Count) configured process instance(s) observed." `
                @($observed | Select-Object ProcessName, Id)
        }
    }
}
else {
    Add-Check $checks 'runtime' 'Runtime measurement requested' 'info' 'Runtime measurement is disabled for this run.' $false
}

if ($networkRequested) {
    $networkCommand = Get-Command Get-NetTCPConnection -ErrorAction SilentlyContinue
    Add-Check $checks 'network' 'Get-NetTCPConnection' `
        $(if ($null -ne $networkCommand) { 'pass' } else { 'error' }) `
        $(if ($null -ne $networkCommand) { 'Get-NetTCPConnection is available.' } else { 'Get-NetTCPConnection is unavailable; per-process TCP evidence cannot be captured.' }) `
        $(if ($null -ne $networkCommand) { $networkCommand.Source } else { $null })
}
else {
    Add-Check $checks 'network' 'Network measurement requested' 'info' 'Network measurement is disabled for this run.' $false
}

if ($logsRequested) {
    $enabledTimings = @(
        $config.logs.timings |
            Where-Object {
                [bool]$_.enabled -and
                -not [string]::IsNullOrWhiteSpace([string]$_.startPattern) -and
                -not [string]::IsNullOrWhiteSpace([string]$_.endPattern)
            }
    )
    if ($enabledTimings.Count -eq 0) {
        Add-Check $checks 'logs' 'Enabled timing definitions' 'error' `
            'No enabled timing has both a startPattern and endPattern.' $null
    }
    else {
        Add-Check $checks 'logs' 'Enabled timing definitions' 'pass' `
            "$($enabledTimings.Count) timing definition(s) are enabled." @($enabledTimings | ForEach-Object name)
    }

    $matchedLogFiles = New-Object Collections.ArrayList
    foreach ($configuredPath in @($config.logs.paths)) {
        $candidate = Resolve-FromBase -Base $repositoryRoot -Path ([string]$configuredPath)
        foreach ($file in @(Get-ChildItem -Path $candidate -File -ErrorAction SilentlyContinue)) {
            if (-not $matchedLogFiles.Contains($file.FullName)) {
                [void]$matchedLogFiles.Add($file.FullName)
            }
        }
    }

    Add-Check $checks 'logs' 'Log files' `
        $(if ($matchedLogFiles.Count -gt 0) { 'pass' } else { 'error' }) `
        $(if ($matchedLogFiles.Count -gt 0) { "$($matchedLogFiles.Count) log file(s) matched." } else { 'No configured log path matched a file.' }) `
        @($matchedLogFiles)
}
else {
    Add-Check $checks 'logs' 'Log timing measurement requested' 'info' 'Log timing measurement is disabled for this run.' $false
}

$sqlFiles = @(
    'tools\baseline\sql\01-Create-BaselineXEventSession.sql',
    'tools\baseline\sql\02-Stop-BaselineXEventSession.sql',
    'tools\baseline\sql\03-Read-BaselineXEventSession.sql'
)
$missingSqlFiles = @(
    $sqlFiles | Where-Object {
        -not (Test-Path -LiteralPath (Join-Path $repositoryRoot $_) -PathType Leaf)
    }
)
Add-Check $checks 'sql' 'Extended Events scripts' `
    $(if ($missingSqlFiles.Count -eq 0) { 'pass' } else { 'error' }) `
    $(if ($missingSqlFiles.Count -eq 0) { 'All SQL baseline scripts are present.' } else { "Missing SQL scripts: $($missingSqlFiles -join ', ')" }) `
    $missingSqlFiles

$administrator = Test-IsAdministrator
Add-Check $checks 'environment' 'Administrator token' `
    $(if ($administrator) { 'pass' } else { 'warning' }) `
    $(if ($administrator) { 'PowerShell is running elevated.' } else { 'PowerShell is not elevated; some process paths, command lines, hashes or network endpoints may be inaccessible.' }) `
    $administrator

$includeCommandLine = $false
if ($config.runtime.PSObject.Properties['includeCommandLine']) {
    $includeCommandLine = [bool]$config.runtime.includeCommandLine
}
if ($includeCommandLine) {
    Add-Check $checks 'security' 'Command-line capture' 'warning' `
        'runtime.includeCommandLine is enabled. Review output for credentials, tokens or connection strings before sharing.' $true
}
else {
    Add-Check $checks 'security' 'Command-line capture' 'pass' `
        'Command-line capture is disabled by default.' $false
}

$errorChecks = @($checks | Where-Object Status -eq 'error')
$warningChecks = @($checks | Where-Object Status -eq 'warning')
$passChecks = @($checks | Where-Object Status -eq 'pass')

$summary = [ordered]@{
    schemaVersion = 1
    generatedAtUtc = [DateTime]::UtcNow.ToString('o')
    configPath = $configFullPath
    repositoryRoot = $repositoryRoot
    sourceRoot = $sourceRoot
    artifactRoot = $artifactRoot
    requested = [ordered]@{
        build = $buildRequested
        runtime = $runtimeRequested
        network = $networkRequested
        logTimings = $logsRequested
    }
    success = [bool]($errorChecks.Count -eq 0)
    errorCount = $errorChecks.Count
    warningCount = $warningChecks.Count
    passCount = $passChecks.Count
    checks = @($checks)
}

$jsonPath = Join-Path $OutputDirectory 'preflight.json'
$csvPath = Join-Path $OutputDirectory 'preflight.csv'
$markdownPath = Join-Path $OutputDirectory 'PREFLIGHT.md'
$summary | ConvertTo-Json -Depth 12 | Set-Content -LiteralPath $jsonPath -Encoding UTF8
@($checks) |
    Select-Object Category, Name, Status, Message, @{n='Evidence';e={($_.Evidence | ConvertTo-Json -Compress -Depth 6)}} |
    Export-Csv -LiteralPath $csvPath -NoTypeInformation -Encoding UTF8

$markdown = New-Object Text.StringBuilder
[void]$markdown.AppendLine('# Titanium Phase 0 preflight')
[void]$markdown.AppendLine('')
[void]$markdown.AppendLine("- Success: **$($summary.success)**")
[void]$markdown.AppendLine("- Errors: $($summary.errorCount)")
[void]$markdown.AppendLine("- Warnings: $($summary.warningCount)")
[void]$markdown.AppendLine("- Config: ``$configFullPath``")
[void]$markdown.AppendLine('')
[void]$markdown.AppendLine('| Category | Check | Status | Message |')
[void]$markdown.AppendLine('|---|---|---|---|')
foreach ($check in $checks) {
    [void]$markdown.AppendLine("| $(Convert-ToMarkdownCell $check.Category) | $(Convert-ToMarkdownCell $check.Name) | **$($check.Status)** | $(Convert-ToMarkdownCell $check.Message) |")
}
$markdown.ToString() | Set-Content -LiteralPath $markdownPath -Encoding UTF8

Write-Host "Preflight: errors=$($summary.errorCount), warnings=$($summary.warningCount), output=$OutputDirectory"

$result = [pscustomobject]@{
    Success = $summary.success
    ErrorCount = $summary.errorCount
    WarningCount = $summary.warningCount
    OutputDirectory = $OutputDirectory
    JsonPath = $jsonPath
    CsvPath = $csvPath
    MarkdownPath = $markdownPath
    Checks = @($checks)
}

if ($FailOnError -and -not $result.Success) {
    throw "Phase 0 preflight failed with $($result.ErrorCount) error(s). Evidence: $OutputDirectory"
}

$result
