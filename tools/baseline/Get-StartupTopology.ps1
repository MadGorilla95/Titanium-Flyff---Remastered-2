[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Join-Path $PSScriptRoot "..\.."),
    [string]$OutputDirectory = ""
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-RelativePath([string]$Base, [string]$Path) {
    $baseFull = [IO.Path]::GetFullPath($Base).TrimEnd('\','/') + [IO.Path]::DirectorySeparatorChar
    $baseUri = [Uri]$baseFull
    $pathUri = [Uri][IO.Path]::GetFullPath($Path)
    return [Uri]::UnescapeDataString($baseUri.MakeRelativeUri($pathUri).ToString()).Replace('/', [IO.Path]::DirectorySeparatorChar)
}

function Get-ExecutableToken([string]$Line) {
    $matches = [regex]::Matches(
        $Line,
        '(?i)"(?<quoted>[^"\r\n]+\.exe)"|(?<plain>(?:[A-Za-z]:\\|\.\.?\\)?[^\s"<>|&]+\.exe)'
    )
    if ($matches.Count -eq 0) { return $null }

    $match = $matches[$matches.Count - 1]
    if ($match.Groups['quoted'].Success) { return $match.Groups['quoted'].Value.Trim() }
    return $match.Groups['plain'].Value.Trim()
}

function Get-Delay([string]$Line) {
    $trimmed = $Line.Trim()

    $m = [regex]::Match($trimmed, '(?i)^timeout(?:\.exe)?(?:\s+/t)?\s+(?<seconds>\d+)\b')
    if ($m.Success) {
        return [pscustomobject]@{
            Kind = 'timeout'
            Seconds = [double]$m.Groups['seconds'].Value
            IsEstimate = $false
        }
    }

    $m = [regex]::Match($trimmed, '(?i)^ping(?:\.exe)?\b.*?\s-n\s+(?<count>\d+)\b')
    if ($m.Success) {
        $count = [int]$m.Groups['count'].Value
        return [pscustomobject]@{
            Kind = 'ping-count'
            Seconds = [double][math]::Max($count - 1, 0)
            IsEstimate = $true
        }
    }

    $m = [regex]::Match($trimmed, '(?i)\bStart-Sleep\b.*?-Milliseconds\s+(?<milliseconds>\d+)\b')
    if ($m.Success) {
        return [pscustomobject]@{
            Kind = 'start-sleep-milliseconds'
            Seconds = [double]$m.Groups['milliseconds'].Value / 1000.0
            IsEstimate = $false
        }
    }

    $m = [regex]::Match($trimmed, '(?i)\bStart-Sleep\b(?:\s+-Seconds)?\s+(?<seconds>\d+)\b')
    if ($m.Success) {
        return [pscustomobject]@{
            Kind = 'start-sleep-seconds'
            Seconds = [double]$m.Groups['seconds'].Value
            IsEstimate = $false
        }
    }

    $m = [regex]::Match($trimmed, '(?i)^choice(?:\.exe)?\b.*?\s/t\s+(?<seconds>\d+)\b')
    if ($m.Success) {
        return [pscustomobject]@{
            Kind = 'choice-timeout'
            Seconds = [double]$m.Groups['seconds'].Value
            IsEstimate = $false
        }
    }

    return $null
}

function Convert-ToMarkdownCell([object]$Value) {
    if ($null -eq $Value) { return '' }
    return ([string]$Value).Replace('|', '\|').Replace("`r", ' ').Replace("`n", ' ')
}

$repo = (Resolve-Path $RepositoryRoot).Path
if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $OutputDirectory = Join-Path $repo 'artifacts\baseline\startup-topology'
}
$OutputDirectory = [IO.Path]::GetFullPath($OutputDirectory)
New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null

$excluded = '[\\/](?:\.git|\.vs|Debug|Release|x64|x86|ipch|artifacts|node_modules)[\\/]'
$launcherFiles = @(
    Get-ChildItem $repo -Recurse -File -Force -ErrorAction SilentlyContinue |
        Where-Object {
            $_.FullName -notmatch $excluded -and
            $_.Extension.ToLowerInvariant() -in @('.bat', '.cmd')
        } |
        Sort-Object FullName
)

$launcherRows = New-Object System.Collections.ArrayList
$startRows = New-Object System.Collections.ArrayList
$stopRows = New-Object System.Collections.ArrayList
$delayRows = New-Object System.Collections.ArrayList
$readinessRows = New-Object System.Collections.ArrayList

foreach ($file in $launcherFiles) {
    $relativePath = Get-RelativePath $repo $file.FullName
    $phase = 'startup'
    $lineNumber = 0
    $startupDelay = 0.0
    $shutdownDelay = 0.0
    $delaySincePreviousStart = 0.0
    $startOrder = 0
    $stopOrder = 0
    $readinessCount = 0
    $startupDelayCount = 0
    $shutdownDelayCount = 0
    $hasPauseBoundary = $false

    foreach ($rawLine in Get-Content $file.FullName -ErrorAction Stop) {
        $lineNumber++
        $trimmed = $rawLine.Trim()
        if ($trimmed.StartsWith('@')) { $trimmed = $trimmed.Substring(1).TrimStart() }
        if ([string]::IsNullOrWhiteSpace($trimmed)) { continue }
        if ($trimmed -match '^(?i:rem)(?:\s|$)' -or $trimmed.StartsWith('::')) { continue }

        if ($trimmed -match '^(?i:pause)(?:\s|>|$)') {
            $phase = 'shutdown'
            $hasPauseBoundary = $true
            continue
        }

        if ($trimmed -match '^(?i:taskkill)(?:\.exe)?\b' -and $phase -eq 'startup') {
            $phase = 'shutdown'
        }

        $delay = Get-Delay $trimmed
        if ($null -ne $delay) {
            if ($phase -eq 'startup') {
                $startupDelay += [double]$delay.Seconds
                $delaySincePreviousStart += [double]$delay.Seconds
                $startupDelayCount++
                $cumulative = $startupDelay
            }
            else {
                $shutdownDelay += [double]$delay.Seconds
                $shutdownDelayCount++
                $cumulative = $shutdownDelay
            }

            [void]$delayRows.Add([pscustomobject]@{
                Launcher = $relativePath
                Phase = $phase
                Line = $lineNumber
                Kind = $delay.Kind
                Seconds = [math]::Round([double]$delay.Seconds, 3)
                IsEstimate = [bool]$delay.IsEstimate
                CumulativePhaseDelaySeconds = [math]::Round($cumulative, 3)
                Source = $trimmed
            })
        }

        if ($phase -eq 'startup' -and $trimmed -match '^(?i:start)(?:\.exe)?(?:\s|$)') {
            $executable = Get-ExecutableToken $trimmed
            if ($null -ne $executable) {
                $startOrder++
                [void]$startRows.Add([pscustomobject]@{
                    Launcher = $relativePath
                    Order = $startOrder
                    Line = $lineNumber
                    Executable = $executable
                    ExecutableName = [IO.Path]::GetFileName($executable)
                    DelaySincePreviousStartSeconds = [math]::Round($delaySincePreviousStart, 3)
                    CumulativeScriptDelayBeforeStartSeconds = [math]::Round($startupDelay, 3)
                    Source = $trimmed
                })
                $delaySincePreviousStart = 0.0
            }
        }

        if ($trimmed -match '^(?i:taskkill)(?:\.exe)?\b') {
            $m = [regex]::Match($trimmed, '(?i)/im\s+(?:"(?<quoted>[^"\r\n]+\.exe)"|(?<plain>[^\s>]+\.exe))')
            if ($m.Success) {
                $stopOrder++
                $executable = if ($m.Groups['quoted'].Success) { $m.Groups['quoted'].Value } else { $m.Groups['plain'].Value }
                [void]$stopRows.Add([pscustomobject]@{
                    Launcher = $relativePath
                    Order = $stopOrder
                    Line = $lineNumber
                    Executable = $executable
                    ExecutableName = [IO.Path]::GetFileName($executable)
                    Source = $trimmed
                })
            }
        }

        if ($phase -eq 'startup' -and $trimmed -match '(?i)(Test-NetConnection|Wait-Process|netstat\b.*find|findstr\b.*(?:ready|listen)|curl(?:\.exe)?\b|Invoke-WebRequest|sqlcmd\b.*-Q)') {
            $readinessCount++
            [void]$readinessRows.Add([pscustomobject]@{
                Launcher = $relativePath
                Line = $lineNumber
                Source = $trimmed
            })
        }
    }

    $launcherStarts = @($startRows | Where-Object Launcher -eq $relativePath)
    $launcherStops = @($stopRows | Where-Object Launcher -eq $relativePath)
    if ($launcherStarts.Count -eq 0 -and $launcherStops.Count -eq 0 -and ($startupDelayCount + $shutdownDelayCount) -eq 0) {
        continue
    }

    $lastStartDelay = 0.0
    if ($launcherStarts.Count -gt 0) {
        $lastStartDelay = [double]$launcherStarts[$launcherStarts.Count - 1].CumulativeScriptDelayBeforeStartSeconds
    }

    $architecture = 'unknown'
    $joinedExecutables = (($launcherStarts | Select-Object -ExpandProperty ExecutableName) -join ';')
    if ($relativePath -match '(?i)64' -or $joinedExecutables -match '(?i)(?:-x64|x64\.exe)') {
        $architecture = 'x64'
    }
    elseif ($launcherStarts.Count -gt 0) {
        $architecture = 'x86-or-unspecified'
    }

    $riskFlags = New-Object System.Collections.ArrayList
    if ($startupDelayCount -gt 0 -and $readinessCount -eq 0) { [void]$riskFlags.Add('fixed-delay-without-readiness-check') }
    if ($launcherStarts.Count -gt 0 -and $launcherStops.Count -gt 0) { [void]$riskFlags.Add('combined-start-stop-script') }
    if (-not $hasPauseBoundary -and $launcherStops.Count -gt 0) { [void]$riskFlags.Add('implicit-start-stop-boundary') }

    [void]$launcherRows.Add([pscustomobject]@{
        Launcher = $relativePath
        Architecture = $architecture
        StartCount = $launcherStarts.Count
        StopCount = $launcherStops.Count
        StartupDelayCount = $startupDelayCount
        StartupScriptedDelaySeconds = [math]::Round($startupDelay, 3)
        DelayBeforeLastStartSeconds = [math]::Round($lastStartDelay, 3)
        DelayAfterLastStartSeconds = [math]::Round(($startupDelay - $lastStartDelay), 3)
        ShutdownScriptedDelaySeconds = [math]::Round($shutdownDelay, 3)
        ReadinessCheckCount = $readinessCount
        UsesOnlyFixedStartupDelay = [bool]($startupDelayCount -gt 0 -and $readinessCount -eq 0)
        HasPauseBoundary = $hasPauseBoundary
        StartExecutables = @($launcherStarts | Select-Object -ExpandProperty ExecutableName)
        StopExecutables = @($launcherStops | Select-Object -ExpandProperty ExecutableName)
        RiskFlags = @($riskFlags)
    })
}

$topology = [ordered]@{
    schemaVersion = 1
    generatedAtUtc = [DateTime]::UtcNow.ToString('o')
    repositoryRoot = $repo
    interpretation = [ordered]@{
        scriptedDelayIsReadiness = $false
        note = 'Fixed timeout/ping delays are launcher overhead estimates. They do not prove that a process was ready, healthy, connected or fully initialized.'
    }
    totals = [ordered]@{
        launcherFilesScanned = $launcherFiles.Count
        launchersWithTopology = $launcherRows.Count
        startCommands = $startRows.Count
        stopCommands = $stopRows.Count
        delayCommands = $delayRows.Count
        readinessChecks = $readinessRows.Count
    }
    launchers = @($launcherRows)
    starts = @($startRows)
    stops = @($stopRows)
    delays = @($delayRows)
    readinessChecks = @($readinessRows)
}

$topology | ConvertTo-Json -Depth 12 | Set-Content (Join-Path $OutputDirectory 'startup-topology.json') -Encoding UTF8
@($launcherRows) | Select-Object Launcher,Architecture,StartCount,StopCount,StartupDelayCount,StartupScriptedDelaySeconds,DelayBeforeLastStartSeconds,DelayAfterLastStartSeconds,ShutdownScriptedDelaySeconds,ReadinessCheckCount,UsesOnlyFixedStartupDelay,HasPauseBoundary,@{n='StartExecutables';e={$_.StartExecutables -join ';'}},@{n='StopExecutables';e={$_.StopExecutables -join ';'}},@{n='RiskFlags';e={$_.RiskFlags -join ';'}} | Export-Csv (Join-Path $OutputDirectory 'startup-launchers.csv') -NoTypeInformation -Encoding UTF8
@($startRows) | Export-Csv (Join-Path $OutputDirectory 'startup-sequence.csv') -NoTypeInformation -Encoding UTF8
@($stopRows) | Export-Csv (Join-Path $OutputDirectory 'shutdown-sequence.csv') -NoTypeInformation -Encoding UTF8
@($delayRows) | Export-Csv (Join-Path $OutputDirectory 'scripted-delays.csv') -NoTypeInformation -Encoding UTF8
@($readinessRows) | Export-Csv (Join-Path $OutputDirectory 'readiness-checks.csv') -NoTypeInformation -Encoding UTF8

$md = New-Object Text.StringBuilder
[void]$md.AppendLine('# Startup topology')
[void]$md.AppendLine('')
[void]$md.AppendLine('> Scripted waits are launcher overhead, not proof of application readiness. Actual readiness must come from process health, ports, dependencies or explicit log markers.')
[void]$md.AppendLine('')
[void]$md.AppendLine('| Launcher | Architecture | Starts | Delay before last start (s) | Startup scripted delay (s) | Readiness checks | Risk flags |')
[void]$md.AppendLine('|---|---|---:|---:|---:|---:|---|')
foreach ($row in $launcherRows) {
    [void]$md.AppendLine("| $(Convert-ToMarkdownCell $row.Launcher) | $($row.Architecture) | $($row.StartCount) | $($row.DelayBeforeLastStartSeconds) | $($row.StartupScriptedDelaySeconds) | $($row.ReadinessCheckCount) | $(Convert-ToMarkdownCell ($row.RiskFlags -join ', ')) |")
}

foreach ($launcher in $launcherRows) {
    [void]$md.AppendLine('')
    [void]$md.AppendLine("## $(Convert-ToMarkdownCell $launcher.Launcher)")
    [void]$md.AppendLine('')
    [void]$md.AppendLine('| Order | Executable | Line | Delay since previous start (s) | Cumulative scripted delay (s) |')
    [void]$md.AppendLine('|---:|---|---:|---:|---:|')
    foreach ($start in @($startRows | Where-Object Launcher -eq $launcher.Launcher)) {
        [void]$md.AppendLine("| $($start.Order) | $(Convert-ToMarkdownCell $start.ExecutableName) | $($start.Line) | $($start.DelaySincePreviousStartSeconds) | $($start.CumulativeScriptDelayBeforeStartSeconds) |")
    }
}
$md.ToString() | Set-Content (Join-Path $OutputDirectory 'startup-topology.md') -Encoding UTF8

[pscustomobject]@{
    OutputDirectory = $OutputDirectory
    LaunchersWithTopology = $launcherRows.Count
    StartCommands = $startRows.Count
    DelayCommands = $delayRows.Count
    ReadinessChecks = $readinessRows.Count
}
