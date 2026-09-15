[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$ConfigPath,

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

function Convert-ToTimestamp {
    param(
        [Parameter(Mandatory = $true)][string]$Value,
        [Parameter(Mandatory = $true)][string[]]$Formats
    )

    foreach ($format in $Formats) {
        $parsed = [datetime]::MinValue
        if (
            [datetime]::TryParseExact(
                $Value,
                $format,
                [System.Globalization.CultureInfo]::InvariantCulture,
                [System.Globalization.DateTimeStyles]::AllowWhiteSpaces,
                [ref]$parsed
            )
        ) {
            return $parsed
        }
    }

    $fallback = [datetime]::MinValue
    if (
        [datetime]::TryParse(
            $Value,
            [System.Globalization.CultureInfo]::InvariantCulture,
            [System.Globalization.DateTimeStyles]::AllowWhiteSpaces,
            [ref]$fallback
        )
    ) {
        return $fallback
    }

    return $null
}

function Resolve-ConfiguredFiles {
    param(
        [Parameter(Mandatory = $true)][string]$Root,
        [Parameter(Mandatory = $true)][object[]]$ConfiguredPaths
    )

    $resolved = New-Object System.Collections.ArrayList
    foreach ($configuredPath in $ConfiguredPaths) {
        $candidate = [string]$configuredPath
        if ([string]::IsNullOrWhiteSpace($candidate)) {
            continue
        }
        if (-not [System.IO.Path]::IsPathRooted($candidate)) {
            $candidate = Join-Path $Root $candidate
        }

        foreach ($item in @(Get-ChildItem -Path $candidate -File -ErrorAction SilentlyContinue)) {
            if (-not $resolved.Contains($item.FullName)) {
                [void]$resolved.Add($item.FullName)
            }
        }
    }
    return @($resolved)
}

function Get-CorrelationKey {
    param(
        [Parameter(Mandatory = $true)][string]$Line,
        [string]$CorrelationPattern
    )

    if ([string]::IsNullOrWhiteSpace($CorrelationPattern)) {
        return "__serial_fifo__"
    }

    $match = [regex]::Match($Line, $CorrelationPattern)
    if (-not $match.Success) {
        return $null
    }

    $group = $match.Groups["correlation"]
    if ($null -eq $group -or -not $group.Success -or [string]::IsNullOrWhiteSpace($group.Value)) {
        return $null
    }

    return $group.Value
}

$repositoryPath = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$configFullPath = $ConfigPath
if (-not [System.IO.Path]::IsPathRooted($configFullPath)) {
    $configFullPath = Join-Path $repositoryPath $configFullPath
}
$configFullPath = (Resolve-Path -LiteralPath $configFullPath).Path
$config = Get-Content -LiteralPath $configFullPath -Raw | ConvertFrom-Json
$logConfig = $config.logs

if ($null -eq $logConfig) {
    throw "The configuration has no 'logs' section."
}

if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $OutputDirectory = Join-Path $repositoryPath "artifacts\baseline\log-timings"
}
$OutputDirectory = [System.IO.Path]::GetFullPath($OutputDirectory)
[void](New-Item -ItemType Directory -Path $OutputDirectory -Force)

$timestampRegex = [string]$logConfig.timestampRegex
$timestampFormats = @($logConfig.timestampFormats | ForEach-Object { [string]$_ })
if ([string]::IsNullOrWhiteSpace($timestampRegex)) {
    throw "logs.timestampRegex is empty."
}
if ($timestampFormats.Count -eq 0) {
    throw "logs.timestampFormats is empty."
}

$timingDefinitions = @(
    $logConfig.timings |
        Where-Object {
            [bool]$_.enabled -and
            -not [string]::IsNullOrWhiteSpace([string]$_.name) -and
            -not [string]::IsNullOrWhiteSpace([string]$_.startPattern) -and
            -not [string]::IsNullOrWhiteSpace([string]$_.endPattern)
        } |
        ForEach-Object {
            [pscustomobject]@{
                Name = [string]$_.name
                StartPattern = [string]$_.startPattern
                EndPattern = [string]$_.endPattern
                CorrelationRegex = [string]$_.correlationRegex
            }
        }
)

if ($timingDefinitions.Count -eq 0) {
    throw "No enabled timing has both startPattern and endPattern."
}

$logFiles = @(Resolve-ConfiguredFiles -Root $repositoryPath -ConfiguredPaths @($logConfig.paths))
if ($logFiles.Count -eq 0) {
    throw "No log files matched logs.paths."
}

$measurements = New-Object System.Collections.ArrayList
$diagnostics = New-Object System.Collections.ArrayList

foreach ($logFile in $logFiles) {
    Write-Host "Parsing $logFile"

    $states = @{}
    foreach ($definition in $timingDefinitions) {
        $states[$definition.Name] = [pscustomobject]@{
            Queues = @{}
            UnpairedStarts = 0
            UnpairedEnds = 0
            MissingCorrelation = 0
            InvalidTimestamps = 0
        }
    }

    $lineNumber = 0
    foreach ($line in (Get-Content -LiteralPath $logFile -ErrorAction Stop)) {
        $lineNumber++

        $timestampMatch = [regex]::Match($line, $timestampRegex)
        if (-not $timestampMatch.Success -or -not $timestampMatch.Groups["timestamp"].Success) {
            continue
        }

        $timestampText = $timestampMatch.Groups["timestamp"].Value
        $timestamp = Convert-ToTimestamp -Value $timestampText -Formats $timestampFormats
        if ($null -eq $timestamp) {
            foreach ($definition in $timingDefinitions) {
                $states[$definition.Name].InvalidTimestamps++
            }
            continue
        }

        foreach ($definition in $timingDefinitions) {
            $state = $states[$definition.Name]
            $isStart = [regex]::IsMatch($line, $definition.StartPattern)
            $isEnd = [regex]::IsMatch($line, $definition.EndPattern)

            if (-not $isStart -and -not $isEnd) {
                continue
            }

            $correlation = Get-CorrelationKey -Line $line -CorrelationPattern $definition.CorrelationRegex
            if ($null -eq $correlation) {
                $state.MissingCorrelation++
                continue
            }

            if ($isStart) {
                if (-not $state.Queues.ContainsKey($correlation)) {
                    $state.Queues[$correlation] = New-Object System.Collections.Queue
                }
                $state.Queues[$correlation].Enqueue([pscustomobject]@{
                    Timestamp = $timestamp
                    Line = $lineNumber
                    Preview = $line.Trim()
                })
            }

            if ($isEnd) {
                if (
                    $state.Queues.ContainsKey($correlation) -and
                    $state.Queues[$correlation].Count -gt 0
                ) {
                    $start = $state.Queues[$correlation].Dequeue()
                    $durationMs = ($timestamp - $start.Timestamp).TotalMilliseconds

                    [void]$measurements.Add([pscustomobject]@{
                        Timing = $definition.Name
                        Correlation = $correlation
                        LogFile = $logFile
                        StartLine = $start.Line
                        EndLine = $lineNumber
                        StartTimestamp = $start.Timestamp
                        EndTimestamp = $timestamp
                        DurationMilliseconds = [math]::Round($durationMs, 3)
                        IsChronological = [bool]($durationMs -ge 0)
                        PairingMode = $(if ([string]::IsNullOrWhiteSpace($definition.CorrelationRegex)) { "serial-fifo" } else { "correlated-fifo" })
                    })
                }
                else {
                    $state.UnpairedEnds++
                }
            }
        }
    }

    foreach ($definition in $timingDefinitions) {
        $state = $states[$definition.Name]
        foreach ($queue in $state.Queues.Values) {
            $state.UnpairedStarts += $queue.Count
        }

        [void]$diagnostics.Add([pscustomobject]@{
            Timing = $definition.Name
            LogFile = $logFile
            UnpairedStarts = $state.UnpairedStarts
            UnpairedEnds = $state.UnpairedEnds
            MissingCorrelation = $state.MissingCorrelation
            InvalidTimestamps = $state.InvalidTimestamps
        })
    }
}

$measurementsCsvPath = Join-Path $OutputDirectory "timing-measurements.csv"
$diagnosticsCsvPath = Join-Path $OutputDirectory "timing-diagnostics.csv"
@($measurements) | Export-Csv -LiteralPath $measurementsCsvPath -NoTypeInformation -Encoding UTF8
@($diagnostics) | Export-Csv -LiteralPath $diagnosticsCsvPath -NoTypeInformation -Encoding UTF8

$summaryRows = New-Object System.Collections.ArrayList
foreach ($group in @($measurements | Where-Object { $_.IsChronological } | Group-Object Timing)) {
    $values = @($group.Group | ForEach-Object { [double]$_.DurationMilliseconds })
    [void]$summaryRows.Add([pscustomobject]@{
        Timing = $group.Name
        Count = $values.Count
        MinimumMilliseconds = [math]::Round(($values | Measure-Object -Minimum).Minimum, 3)
        AverageMilliseconds = [math]::Round(($values | Measure-Object -Average).Average, 3)
        P50Milliseconds = [math]::Round((Get-Percentile -Values $values -Percentile 0.50), 3)
        P95Milliseconds = [math]::Round((Get-Percentile -Values $values -Percentile 0.95), 3)
        MaximumMilliseconds = [math]::Round(($values | Measure-Object -Maximum).Maximum, 3)
    })
}

$summaryJsonPath = Join-Path $OutputDirectory "timing-summary.json"
$summary = [ordered]@{
    generatedAtUtc = [DateTime]::UtcNow.ToString("o")
    configPath = $configFullPath
    logFiles = @($logFiles)
    measurementCount = $measurements.Count
    validMeasurementCount = @($measurements | Where-Object { $_.IsChronological }).Count
    invalidChronologyCount = @($measurements | Where-Object { -not $_.IsChronological }).Count
    timings = @($summaryRows)
    diagnostics = @($diagnostics)
    warning = "Serial FIFO pairing is only reliable for a controlled single-client test. Use correlationRegex for concurrent logins or world loads."
}
$summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $summaryJsonPath -Encoding UTF8

[pscustomobject]@{
    OutputDirectory = $OutputDirectory
    MeasurementsCsv = $measurementsCsvPath
    DiagnosticsCsv = $diagnosticsCsvPath
    SummaryJson = $summaryJsonPath
    MeasurementCount = $measurements.Count
}
