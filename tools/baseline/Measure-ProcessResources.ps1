[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateNotNullOrEmpty()]
    [string[]]$ProcessName,

    [ValidateRange(1, 86400)]
    [int]$DurationSeconds = 300,

    [ValidateRange(1, 60)]
    [int]$IntervalSeconds = 1,

    [string]$RepositoryRoot = (Join-Path $PSScriptRoot "..\.."),
    [string]$OutputDirectory = ""
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-Percentile {
    param(
        [double[]]$Values,
        [ValidateRange(0.0, 1.0)][double]$Percentile
    )

    $clean = @($Values | Where-Object { $null -ne $_ } | Sort-Object)
    if ($clean.Count -eq 0) {
        return $null
    }

    $index = [math]::Ceiling($Percentile * $clean.Count) - 1
    if ($index -lt 0) { $index = 0 }
    if ($index -ge $clean.Count) { $index = $clean.Count - 1 }
    return [double]$clean[$index]
}

function Get-LinearSlope {
    param(
        [Parameter(Mandatory = $true)][object[]]$Samples,
        [Parameter(Mandatory = $true)][string]$ValueProperty
    )

    if ($Samples.Count -lt 2) {
        return $null
    }

    $ordered = @($Samples | Sort-Object TimestampUtc)
    $start = [datetime]$ordered[0].TimestampUtc
    $points = @(
        foreach ($sample in $ordered) {
            $value = $sample.$ValueProperty
            if ($null -ne $value) {
                [pscustomobject]@{
                    X = (([datetime]$sample.TimestampUtc) - $start).TotalMinutes
                    Y = [double]$value
                }
            }
        }
    )

    if ($points.Count -lt 2) {
        return $null
    }

    $sumX = ($points | Measure-Object X -Sum).Sum
    $sumY = ($points | Measure-Object Y -Sum).Sum
    $sumXX = 0.0
    $sumXY = 0.0
    foreach ($point in $points) {
        $sumXX += ($point.X * $point.X)
        $sumXY += ($point.X * $point.Y)
    }

    $n = [double]$points.Count
    $denominator = ($n * $sumXX) - ($sumX * $sumX)
    if ([math]::Abs($denominator) -lt 0.0000001) {
        return $null
    }

    return (($n * $sumXY) - ($sumX * $sumY)) / $denominator
}

$repositoryPath = (Resolve-Path -LiteralPath $RepositoryRoot).Path
if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $OutputDirectory = Join-Path $repositoryPath "artifacts\baseline\process-resources"
}
$OutputDirectory = [System.IO.Path]::GetFullPath($OutputDirectory)
[void](New-Item -ItemType Directory -Path $OutputDirectory -Force)

$normalizedNames = @(
    $ProcessName |
        ForEach-Object {
            $name = $_.Trim()
            if ($name.EndsWith(".exe", [System.StringComparison]::OrdinalIgnoreCase)) {
                $name = $name.Substring(0, $name.Length - 4)
            }
            $name
        } |
        Where-Object { -not [string]::IsNullOrWhiteSpace($_) } |
        Sort-Object -Unique
)
if ($normalizedNames.Count -eq 0) {
    throw "At least one valid process name is required."
}

$logicalProcessorCount = [Environment]::ProcessorCount
$previous = @{}
$samples = New-Object System.Collections.ArrayList
$runStartUtc = [DateTime]::UtcNow
$stopwatch = [System.Diagnostics.Stopwatch]::StartNew()

Write-Host "Sampling processes: $($normalizedNames -join ', ')"
Write-Host "Duration: $DurationSeconds s; interval: $IntervalSeconds s"

while ($stopwatch.Elapsed.TotalSeconds -le $DurationSeconds) {
    $timestampUtc = [DateTime]::UtcNow
    $processes = @(
        Get-Process -Name $normalizedNames -ErrorAction SilentlyContinue
    )

    foreach ($process in $processes) {
        try {
            $key = [string]$process.Id
            $cpuSeconds = [double]$process.CPU
            $cpuPercent = $null

            if ($previous.ContainsKey($key)) {
                $previousSample = $previous[$key]
                $wallSeconds = ($timestampUtc - $previousSample.TimestampUtc).TotalSeconds
                if ($wallSeconds -gt 0 -and $cpuSeconds -ge $previousSample.CpuSeconds) {
                    $cpuPercent = (($cpuSeconds - $previousSample.CpuSeconds) / $wallSeconds / $logicalProcessorCount) * 100.0
                }
            }

            $previous[$key] = [pscustomobject]@{
                TimestampUtc = $timestampUtc
                CpuSeconds = $cpuSeconds
            }

            $startTimeUtc = $null
            try {
                $startTimeUtc = $process.StartTime.ToUniversalTime()
            }
            catch {
                # Some protected processes can deny StartTime access.
            }

            [void]$samples.Add([pscustomobject]@{
                TimestampUtc = $timestampUtc
                ElapsedSeconds = [math]::Round(($timestampUtc - $runStartUtc).TotalSeconds, 3)
                ProcessName = $process.ProcessName
                Id = $process.Id
                StartTimeUtc = $startTimeUtc
                CpuTotalSeconds = [math]::Round($cpuSeconds, 6)
                CpuPercent = $(if ($null -eq $cpuPercent) { $null } else { [math]::Round($cpuPercent, 3) })
                WorkingSetMB = [math]::Round($process.WorkingSet64 / 1MB, 3)
                PrivateMemoryMB = [math]::Round($process.PrivateMemorySize64 / 1MB, 3)
                VirtualMemoryMB = [math]::Round($process.VirtualMemorySize64 / 1MB, 3)
                PagedMemoryMB = [math]::Round($process.PagedMemorySize64 / 1MB, 3)
                HandleCount = $process.HandleCount
                ThreadCount = $process.Threads.Count
            })
        }
        catch {
            Write-Warning "Could not sample process PID $($process.Id): $($_.Exception.Message)"
        }
    }

    if ($stopwatch.Elapsed.TotalSeconds -ge $DurationSeconds) {
        break
    }

    Start-Sleep -Seconds $IntervalSeconds
}

$stopwatch.Stop()

$sampleCsvPath = Join-Path $OutputDirectory "process-samples.csv"
@($samples) | Export-Csv -LiteralPath $sampleCsvPath -NoTypeInformation -Encoding UTF8

$summaryRows = New-Object System.Collections.ArrayList
$groups = @($samples | Group-Object ProcessName, Id)
foreach ($group in $groups) {
    $ordered = @($group.Group | Sort-Object TimestampUtc)
    $cpuValues = @($ordered | Where-Object { $null -ne $_.CpuPercent } | ForEach-Object { [double]$_.CpuPercent })
    $privateValues = @($ordered | ForEach-Object { [double]$_.PrivateMemoryMB })
    $workingValues = @($ordered | ForEach-Object { [double]$_.WorkingSetMB })
    $handleValues = @($ordered | ForEach-Object { [double]$_.HandleCount })
    $threadValues = @($ordered | ForEach-Object { [double]$_.ThreadCount })
    $slope = Get-LinearSlope -Samples $ordered -ValueProperty "PrivateMemoryMB"

    [void]$summaryRows.Add([pscustomobject]@{
        ProcessName = $ordered[0].ProcessName
        Id = $ordered[0].Id
        SampleCount = $ordered.Count
        AverageCpuPercent = $(if ($cpuValues.Count -eq 0) { $null } else { [math]::Round(($cpuValues | Measure-Object -Average).Average, 3) })
        P95CpuPercent = $(if ($cpuValues.Count -eq 0) { $null } else { [math]::Round((Get-Percentile -Values $cpuValues -Percentile 0.95), 3) })
        MaxCpuPercent = $(if ($cpuValues.Count -eq 0) { $null } else { [math]::Round(($cpuValues | Measure-Object -Maximum).Maximum, 3) })
        StartPrivateMemoryMB = $ordered[0].PrivateMemoryMB
        EndPrivateMemoryMB = $ordered[-1].PrivateMemoryMB
        PrivateMemoryGrowthMB = [math]::Round(($ordered[-1].PrivateMemoryMB - $ordered[0].PrivateMemoryMB), 3)
        PrivateMemorySlopeMBPerMinute = $(if ($null -eq $slope) { $null } else { [math]::Round($slope, 6) })
        MaxPrivateMemoryMB = [math]::Round(($privateValues | Measure-Object -Maximum).Maximum, 3)
        MaxWorkingSetMB = [math]::Round(($workingValues | Measure-Object -Maximum).Maximum, 3)
        MaxHandleCount = [int](($handleValues | Measure-Object -Maximum).Maximum)
        MaxThreadCount = [int](($threadValues | Measure-Object -Maximum).Maximum)
        Interpretation = "Growth is a signal only; confirm leaks with a longer soak test and native heap/resource profiling."
    })
}

$summaryCsvPath = Join-Path $OutputDirectory "process-summary.csv"
@($summaryRows) | Export-Csv -LiteralPath $summaryCsvPath -NoTypeInformation -Encoding UTF8

$summaryJsonPath = Join-Path $OutputDirectory "process-summary.json"
$summaryObject = [ordered]@{
    generatedAtUtc = [DateTime]::UtcNow.ToString("o")
    runStartUtc = $runStartUtc.ToString("o")
    durationRequestedSeconds = $DurationSeconds
    durationActualSeconds = [math]::Round($stopwatch.Elapsed.TotalSeconds, 3)
    intervalSeconds = $IntervalSeconds
    logicalProcessorCount = $logicalProcessorCount
    requestedProcessNames = @($normalizedNames)
    sampleCount = $samples.Count
    processInstanceCount = $summaryRows.Count
    warning = "A positive memory slope is not proof of a leak."
    processes = @($summaryRows)
}
$summaryObject | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $summaryJsonPath -Encoding UTF8

if ($samples.Count -eq 0) {
    Write-Warning "None of the requested processes were observed during the sampling window."
}

[pscustomobject]@{
    OutputDirectory = $OutputDirectory
    SamplesCsv = $sampleCsvPath
    SummaryCsv = $summaryCsvPath
    SummaryJson = $summaryJsonPath
    SampleCount = $samples.Count
    ProcessInstanceCount = $summaryRows.Count
}
