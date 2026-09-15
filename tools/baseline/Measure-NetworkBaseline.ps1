[CmdletBinding()]
param(
    [string[]]$ProcessName = @(),

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

function New-MetricSummary {
    param(
        [Parameter(Mandatory = $true)][string]$Metric,
        [Parameter(Mandatory = $true)][object[]]$Rows,
        [Parameter(Mandatory = $true)][string]$Property
    )

    $values = @(
        $Rows |
            ForEach-Object { $_.$Property } |
            Where-Object { $null -ne $_ } |
            ForEach-Object { [double]$_ }
    )

    if ($values.Count -eq 0) {
        return [pscustomobject]@{
            Metric = $Metric
            Samples = 0
            Average = $null
            P50 = $null
            P95 = $null
            Maximum = $null
        }
    }

    return [pscustomobject]@{
        Metric = $Metric
        Samples = $values.Count
        Average = [math]::Round(($values | Measure-Object -Average).Average, 3)
        P50 = [math]::Round((Get-Percentile -Values $values -Percentile 0.50), 3)
        P95 = [math]::Round((Get-Percentile -Values $values -Percentile 0.95), 3)
        Maximum = [math]::Round(($values | Measure-Object -Maximum).Maximum, 3)
    }
}

$repositoryPath = (Resolve-Path -LiteralPath $RepositoryRoot).Path
if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $OutputDirectory = Join-Path $repositoryPath "artifacts\baseline\network"
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

$interfaceSamples = New-Object System.Collections.ArrayList
$tcpSamples = New-Object System.Collections.ArrayList
$connectionSamples = New-Object System.Collections.ArrayList
$warnings = New-Object System.Collections.ArrayList

$hasTcpConnectionCommand = $null -ne (Get-Command Get-NetTCPConnection -ErrorAction SilentlyContinue)
if (-not $hasTcpConnectionCommand) {
    [void]$warnings.Add("Get-NetTCPConnection is unavailable; per-process connection evidence will be omitted.")
}

$runStartUtc = [DateTime]::UtcNow
$stopwatch = [System.Diagnostics.Stopwatch]::StartNew()

Write-Host "Sampling system network counters for $DurationSeconds seconds."
if ($normalizedNames.Count -gt 0) {
    Write-Host "Filtering TCP connection evidence for: $($normalizedNames -join ', ')"
}

while ($stopwatch.Elapsed.TotalSeconds -le $DurationSeconds) {
    $timestampUtc = [DateTime]::UtcNow
    $elapsedSeconds = [math]::Round(($timestampUtc - $runStartUtc).TotalSeconds, 3)

    try {
        $interfaces = @(
            Get-CimInstance -ClassName Win32_PerfFormattedData_Tcpip_NetworkInterface -ErrorAction Stop
        )
        foreach ($interface in $interfaces) {
            [void]$interfaceSamples.Add([pscustomobject]@{
                TimestampUtc = $timestampUtc
                ElapsedSeconds = $elapsedSeconds
                InterfaceName = $interface.Name
                BytesTotalPerSecond = [double]$interface.BytesTotalPersec
                BytesReceivedPerSecond = [double]$interface.BytesReceivedPersec
                BytesSentPerSecond = [double]$interface.BytesSentPersec
                PacketsPerSecond = [double]$interface.PacketsPersec
                PacketsReceivedErrors = [double]$interface.PacketsReceivedErrors
                PacketsOutboundErrors = [double]$interface.PacketsOutboundErrors
                OutputQueueLength = [double]$interface.OutputQueueLength
                CurrentBandwidthBitsPerSecond = [double]$interface.CurrentBandwidth
            })
        }
    }
    catch {
        $message = "Network interface counters failed: $($_.Exception.Message)"
        if (-not $warnings.Contains($message)) {
            [void]$warnings.Add($message)
        }
    }

    try {
        $tcp = Get-CimInstance -ClassName Win32_PerfFormattedData_Tcpip_TCPv4 -ErrorAction Stop |
            Select-Object -First 1
        if ($null -ne $tcp) {
            [void]$tcpSamples.Add([pscustomobject]@{
                TimestampUtc = $timestampUtc
                ElapsedSeconds = $elapsedSeconds
                ConnectionsEstablished = [double]$tcp.ConnectionsEstablished
                ConnectionFailures = [double]$tcp.ConnectionFailures
                ConnectionsReset = [double]$tcp.ConnectionsReset
                SegmentsPerSecond = [double]$tcp.SegmentsPersec
                SegmentsRetransmittedPerSecond = [double]$tcp.SegmentsRetransmittedPersec
            })
        }
    }
    catch {
        $message = "TCPv4 counters failed: $($_.Exception.Message)"
        if (-not $warnings.Contains($message)) {
            [void]$warnings.Add($message)
        }
    }

    if ($hasTcpConnectionCommand) {
        try {
            $selectedProcesses = @()
            if ($normalizedNames.Count -gt 0) {
                $selectedProcesses = @(Get-Process -Name $normalizedNames -ErrorAction SilentlyContinue)
            }

            $processById = @{}
            foreach ($process in $selectedProcesses) {
                $processById[[int]$process.Id] = $process.ProcessName
            }

            if ($processById.Count -gt 0) {
                $connections = @(
                    Get-NetTCPConnection -ErrorAction Stop |
                        Where-Object { $processById.ContainsKey([int]$_.OwningProcess) }
                )

                foreach ($connection in $connections) {
                    [void]$connectionSamples.Add([pscustomobject]@{
                        TimestampUtc = $timestampUtc
                        ElapsedSeconds = $elapsedSeconds
                        ProcessName = $processById[[int]$connection.OwningProcess]
                        ProcessId = [int]$connection.OwningProcess
                        State = [string]$connection.State
                        LocalAddress = $connection.LocalAddress
                        LocalPort = $connection.LocalPort
                        RemoteAddress = $connection.RemoteAddress
                        RemotePort = $connection.RemotePort
                    })
                }
            }
        }
        catch {
            $message = "TCP connection enumeration failed: $($_.Exception.Message)"
            if (-not $warnings.Contains($message)) {
                [void]$warnings.Add($message)
            }
        }
    }

    if ($stopwatch.Elapsed.TotalSeconds -ge $DurationSeconds) {
        break
    }

    Start-Sleep -Seconds $IntervalSeconds
}

$stopwatch.Stop()

$interfaceCsvPath = Join-Path $OutputDirectory "network-interface-samples.csv"
$tcpCsvPath = Join-Path $OutputDirectory "tcpv4-samples.csv"
$connectionsCsvPath = Join-Path $OutputDirectory "tcp-connections.csv"
@($interfaceSamples) | Export-Csv -LiteralPath $interfaceCsvPath -NoTypeInformation -Encoding UTF8
@($tcpSamples) | Export-Csv -LiteralPath $tcpCsvPath -NoTypeInformation -Encoding UTF8
@($connectionSamples) | Export-Csv -LiteralPath $connectionsCsvPath -NoTypeInformation -Encoding UTF8

$metricSummary = New-Object System.Collections.ArrayList
[void]$metricSummary.Add((New-MetricSummary -Metric "interface-bytes-total/sec" -Rows @($interfaceSamples) -Property "BytesTotalPerSecond"))
[void]$metricSummary.Add((New-MetricSummary -Metric "interface-packets/sec" -Rows @($interfaceSamples) -Property "PacketsPerSecond"))
[void]$metricSummary.Add((New-MetricSummary -Metric "interface-receive-errors" -Rows @($interfaceSamples) -Property "PacketsReceivedErrors"))
[void]$metricSummary.Add((New-MetricSummary -Metric "interface-outbound-errors" -Rows @($interfaceSamples) -Property "PacketsOutboundErrors"))
[void]$metricSummary.Add((New-MetricSummary -Metric "interface-output-queue" -Rows @($interfaceSamples) -Property "OutputQueueLength"))
[void]$metricSummary.Add((New-MetricSummary -Metric "tcp-retransmitted-segments/sec" -Rows @($tcpSamples) -Property "SegmentsRetransmittedPerSecond"))
[void]$metricSummary.Add((New-MetricSummary -Metric "tcp-connections-established" -Rows @($tcpSamples) -Property "ConnectionsEstablished"))

$stateSummary = @(
    $connectionSamples |
        Group-Object ProcessName, ProcessId, State |
        ForEach-Object {
            $first = $_.Group[0]
            [pscustomobject]@{
                ProcessName = $first.ProcessName
                ProcessId = $first.ProcessId
                State = $first.State
                ObservationCount = $_.Count
            }
        } |
        Sort-Object ProcessName, ProcessId, State
)

$summaryPath = Join-Path $OutputDirectory "network-summary.json"
$summary = [ordered]@{
    generatedAtUtc = [DateTime]::UtcNow.ToString("o")
    runStartUtc = $runStartUtc.ToString("o")
    durationRequestedSeconds = $DurationSeconds
    durationActualSeconds = [math]::Round($stopwatch.Elapsed.TotalSeconds, 3)
    intervalSeconds = $IntervalSeconds
    requestedProcessNames = @($normalizedNames)
    interfaceSampleCount = $interfaceSamples.Count
    tcpSampleCount = $tcpSamples.Count
    connectionObservationCount = $connectionSamples.Count
    metrics = @($metricSummary)
    connectionStates = @($stateSummary)
    warnings = @($warnings)
    interpretation = "Counters and connection states are evidence for triage, not proof that application code caused a network bottleneck."
}
$summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $summaryPath -Encoding UTF8

[pscustomobject]@{
    OutputDirectory = $OutputDirectory
    InterfaceSamplesCsv = $interfaceCsvPath
    TcpSamplesCsv = $tcpCsvPath
    ConnectionsCsv = $connectionsCsvPath
    SummaryJson = $summaryPath
    WarningCount = $warnings.Count
}
