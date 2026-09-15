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

function Invoke-Stage {
    param([string]$Name, [scriptblock]$Action, [System.Collections.ArrayList]$Stages)
    $start = [DateTime]::UtcNow
    Write-Host "`n== $Name =="
    try {
        $result = & $Action
        [void]$Stages.Add([pscustomobject]@{
            name=$Name; status="completed"; startedUtc=$start.ToString("o")
            finishedUtc=[DateTime]::UtcNow.ToString("o")
            durationSeconds=[math]::Round(([DateTime]::UtcNow-$start).TotalSeconds,3)
            message=""; result=$result
        })
        return $result
    }
    catch {
        $finish = [DateTime]::UtcNow
        [void]$Stages.Add([pscustomobject]@{
            name=$Name; status="failed"; startedUtc=$start.ToString("o")
            finishedUtc=$finish.ToString("o")
            durationSeconds=[math]::Round(($finish-$start).TotalSeconds,3)
            message=$_.Exception.Message; result=$null
        })
        Write-Warning "$Name failed: $($_.Exception.Message)"
        return $null
    }
}

function Add-Skipped([string]$Name, [string]$Reason, [System.Collections.ArrayList]$Stages) {
    $now = [DateTime]::UtcNow.ToString("o")
    [void]$Stages.Add([pscustomobject]@{
        name=$Name; status="skipped"; startedUtc=$now; finishedUtc=$now
        durationSeconds=0; message=$Reason; result=$null
    })
}

function Get-GitState([string]$Root) {
    $git = Get-Command git -ErrorAction SilentlyContinue
    if (-not $git) { return @{available=$false} }
    Push-Location $Root
    try {
        $status = @(& $git.Source status --porcelain 2>$null)
        return [ordered]@{
            available=$true
            commit=((& $git.Source rev-parse HEAD 2>$null | Out-String).Trim())
            branch=((& $git.Source rev-parse --abbrev-ref HEAD 2>$null | Out-String).Trim())
            dirty=[bool]($status.Count -gt 0)
            changedPathCount=$status.Count
            version=((& $git.Source --version 2>$null | Out-String).Trim())
        }
    }
    finally { Pop-Location }
}

$local = Join-Path $PSScriptRoot "baseline.config.local.json"
$example = Join-Path $PSScriptRoot "baseline.config.example.json"
if ([string]::IsNullOrWhiteSpace($ConfigPath)) {
    $ConfigPath = if (Test-Path $local) { $local } else { $example }
}
$configFull = (Resolve-Path $ConfigPath).Path
$config = Get-Content $configFull -Raw | ConvertFrom-Json
$configDir = Split-Path $configFull -Parent
$repoRoot = Resolve-FromBase $configDir ([string]$config.repositoryRoot)
$artifactRoot = Resolve-FromBase $repoRoot ([string]$config.artifactRoot)
$runId = [DateTime]::UtcNow.ToString("yyyyMMdd-HHmmssZ")
$runDir = Join-Path $artifactRoot $runId
New-Item -ItemType Directory -Path $runDir -Force | Out-Null
Copy-Item $configFull (Join-Path $runDir "effective-config.json") -Force

$os = Get-CimInstance Win32_OperatingSystem -ErrorAction SilentlyContinue
$computer = Get-CimInstance Win32_ComputerSystem -ErrorAction SilentlyContinue
$cpu = @(Get-CimInstance Win32_Processor -ErrorAction SilentlyContinue | ForEach-Object Name)
$environment = [ordered]@{
    capturedAtUtc=[DateTime]::UtcNow.ToString("o")
    computerName=$env:COMPUTERNAME
    userName=$env:USERNAME
    timeZone=[TimeZoneInfo]::Local.Id
    osCaption=if($os){$os.Caption}else{[Environment]::OSVersion.VersionString}
    osVersion=if($os){$os.Version}else{[Environment]::OSVersion.VersionString}
    osBuild=if($os){$os.BuildNumber}else{$null}
    powershellVersion=$PSVersionTable.PSVersion.ToString()
    powershellEdition=if($PSVersionTable.PSEdition){$PSVersionTable.PSEdition}else{"Desktop"}
    logicalProcessorCount=[Environment]::ProcessorCount
    processors=$cpu
    totalPhysicalMemoryGB=if($computer){[math]::Round($computer.TotalPhysicalMemory/1GB,3)}else{$null}
    repositoryRoot=$repoRoot
    configPath=$configFull
    configSha256=(Get-FileHash $configFull -Algorithm SHA256).Hash
    git=Get-GitState $repoRoot
}
$environment | ConvertTo-Json -Depth 8 | Set-Content (Join-Path $runDir "environment.json") -Encoding UTF8

Write-Host "Titanium Phase 0 baseline"
Write-Host "Repository: $repoRoot"
Write-Host "Output:     $runDir"
Write-Host "This runner does not start or stop Flyff or SQL Server processes."

$stages = New-Object System.Collections.ArrayList
$inventoryOut = Join-Path $runDir "inventory"
$staticOut = Join-Path $runDir "static-scan"
Invoke-Stage "project-inventory" {
    & (Join-Path $PSScriptRoot "Get-ProjectInventory.ps1") -RepositoryRoot $repoRoot -SourceRoot ([string]$config.sourceRoot) -OutputDirectory $inventoryOut
} $stages | Out-Null
Invoke-Stage "static-hotspots" {
    & (Join-Path $PSScriptRoot "Find-StaticHotspots.ps1") -RepositoryRoot $repoRoot -SourceRoot ([string]$config.sourceRoot) -OutputDirectory $staticOut -ExcludeDirectories @($config.staticScan.excludeDirectories)
} $stages | Out-Null

$doBuild = [bool]$config.build.enabled -or $IncludeBuild
if ($doBuild) {
    Invoke-Stage "build-baseline" {
        & (Join-Path $PSScriptRoot "Measure-Build.ps1") -RepositoryRoot $repoRoot -Solution ([string]$config.build.solution) -Configuration ([string]$config.build.configuration) -Platform ([string]$config.build.platform) -AdditionalArguments @($config.build.additionalArguments) -OutputDirectory (Join-Path $runDir "build")
    } $stages | Out-Null
} else { Add-Skipped "build-baseline" "Enable build.enabled or pass -IncludeBuild." $stages }

$runtimeNames = @($config.runtime.processNames | ForEach-Object {[string]$_} | Where-Object {$_})
$doRuntime = [bool]$config.runtime.enabled -or $IncludeRuntime
if ($doRuntime -and $runtimeNames.Count -gt 0) {
    Invoke-Stage "process-resources" {
        & (Join-Path $PSScriptRoot "Measure-ProcessResources.ps1") -ProcessName $runtimeNames -DurationSeconds ([int]$config.runtime.durationSeconds) -IntervalSeconds ([int]$config.runtime.sampleIntervalSeconds) -RepositoryRoot $repoRoot -OutputDirectory (Join-Path $runDir "process-resources")
    } $stages | Out-Null
} elseif ($doRuntime) { Add-Skipped "process-resources" "runtime.processNames is empty." $stages }
else { Add-Skipped "process-resources" "Enable runtime.enabled or pass -IncludeRuntime." $stages }

$networkNames = @($config.network.processNames | ForEach-Object {[string]$_} | Where-Object {$_})
if ($networkNames.Count -eq 0) { $networkNames = $runtimeNames }
$doNetwork = [bool]$config.network.enabled -or $IncludeNetwork
if ($doNetwork) {
    Invoke-Stage "network-baseline" {
        & (Join-Path $PSScriptRoot "Measure-NetworkBaseline.ps1") -ProcessName $networkNames -DurationSeconds ([int]$config.network.durationSeconds) -IntervalSeconds ([int]$config.network.sampleIntervalSeconds) -RepositoryRoot $repoRoot -OutputDirectory (Join-Path $runDir "network")
    } $stages | Out-Null
} else { Add-Skipped "network-baseline" "Enable network.enabled or pass -IncludeNetwork." $stages }

$doTimings = [bool]$config.logs.enabled -or $IncludeLogTimings
if ($doTimings) {
    Invoke-Stage "log-timings" {
        & (Join-Path $PSScriptRoot "Measure-LogTimings.ps1") -ConfigPath $configFull -RepositoryRoot $repoRoot -OutputDirectory (Join-Path $runDir "log-timings")
    } $stages | Out-Null
} else { Add-Skipped "log-timings" "Enable logs.enabled or pass -IncludeLogTimings." $stages }

$manifest = [ordered]@{
    schemaVersion=1; runId=$runId; generatedAtUtc=[DateTime]::UtcNow.ToString("o")
    runDirectory=$runDir; environmentPath=(Join-Path $runDir "environment.json")
    runPolicy=$config.runPolicy; stages=@($stages)
}
$manifestPath = Join-Path $runDir "baseline-manifest.json"
$manifest | ConvertTo-Json -Depth 14 | Set-Content $manifestPath -Encoding UTF8

$summary = New-Object Text.StringBuilder
[void]$summary.AppendLine("# Titanium Phase 0 baseline run")
[void]$summary.AppendLine("")
[void]$summary.AppendLine("- Run ID: $runId")
[void]$summary.AppendLine("- Git commit: $($environment.git.commit)")
[void]$summary.AppendLine("- Git branch: $($environment.git.branch)")
[void]$summary.AppendLine("- Dirty worktree: $($environment.git.dirty)")
[void]$summary.AppendLine("- Output: $runDir")
[void]$summary.AppendLine("")
[void]$summary.AppendLine("| Stage | Status | Duration (s) | Message |")
[void]$summary.AppendLine("|---|---|---:|---|")
foreach ($stage in $stages) {
    $message = ([string]$stage.message) -replace '\|','\|'
    [void]$summary.AppendLine("| $($stage.name) | $($stage.status) | $($stage.durationSeconds) | $message |")
}
[void]$summary.AppendLine("")
[void]$summary.AppendLine("> Evidence only. Compare controlled runs with the same machine, build, database/data snapshot, world configuration, client route and workload.")
$summaryPath = Join-Path $runDir "RUN-SUMMARY.md"
$summary.ToString() | Set-Content $summaryPath -Encoding UTF8

[pscustomobject]@{
    RunId=$runId; RunDirectory=$runDir; Manifest=$manifestPath; Summary=$summaryPath
    CompletedStages=@($stages | Where-Object status -eq "completed").Count
    FailedStages=@($stages | Where-Object status -eq "failed").Count
    SkippedStages=@($stages | Where-Object status -eq "skipped").Count
}
