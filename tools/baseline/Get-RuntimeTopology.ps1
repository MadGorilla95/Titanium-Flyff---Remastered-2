[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateNotNullOrEmpty()]
    [string[]]$ProcessName,

    [string]$RepositoryRoot = (Join-Path $PSScriptRoot "..\.."),
    [string]$OutputDirectory = "",
    [switch]$IncludeCommandLine,
    [switch]$SkipFileHash
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Add-UniqueWarning {
    param(
        [Parameter(Mandatory = $true)][System.Collections.ArrayList]$List,
        [Parameter(Mandatory = $true)][string]$Message
    )

    if (-not $List.Contains($Message)) {
        [void]$List.Add($Message)
    }
}

function Get-PeArchitecture {
    param([Parameter(Mandatory = $true)][string]$Path)

    $stream = $null
    $reader = $null
    try {
        $stream = New-Object System.IO.FileStream(
            $Path,
            [System.IO.FileMode]::Open,
            [System.IO.FileAccess]::Read,
            [System.IO.FileShare]::ReadWrite
        )
        $reader = New-Object System.IO.BinaryReader($stream)

        if ($reader.ReadUInt16() -ne 0x5A4D) { return "not-pe" }
        $stream.Position = 0x3C
        $peOffset = $reader.ReadInt32()
        if ($peOffset -lt 0 -or ($peOffset + 6) -gt $stream.Length) { return "invalid-pe" }

        $stream.Position = $peOffset
        if ($reader.ReadUInt32() -ne 0x00004550) { return "invalid-pe" }

        switch ($reader.ReadUInt16()) {
            0x014c { return "x86" }
            0x8664 { return "x64" }
            0x01c0 { return "arm" }
            0x01c4 { return "armv7" }
            0xAA64 { return "arm64" }
            default { return ("unknown-0x{0:X4}" -f $_) }
        }
    }
    catch {
        return $null
    }
    finally {
        if ($null -ne $reader) { $reader.Dispose() }
        elseif ($null -ne $stream) { $stream.Dispose() }
    }
}

function Convert-ToMarkdownCell {
    param([object]$Value)
    if ($null -eq $Value) { return "" }
    return ([string]$Value).Replace("|", "\|").Replace("`r", " ").Replace("`n", " ")
}

$repositoryPath = (Resolve-Path -LiteralPath $RepositoryRoot).Path
if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $OutputDirectory = Join-Path $repositoryPath "artifacts\baseline\runtime-topology"
}
$OutputDirectory = [System.IO.Path]::GetFullPath($OutputDirectory)
[void](New-Item -ItemType Directory -Path $OutputDirectory -Force)

$normalizedNames = @(
    $ProcessName |
        ForEach-Object {
            $name = ([string]$_).Trim()
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

$warnings = New-Object System.Collections.ArrayList
$processRows = New-Object System.Collections.ArrayList
$tcpRows = New-Object System.Collections.ArrayList
$udpRows = New-Object System.Collections.ArrayList
$allTcp = @()
$allUdp = @()

if ($null -ne (Get-Command Get-NetTCPConnection -ErrorAction SilentlyContinue)) {
    try { $allTcp = @(Get-NetTCPConnection -ErrorAction Stop) }
    catch { Add-UniqueWarning -List $warnings -Message "TCP endpoint enumeration failed: $($_.Exception.Message)" }
}
else {
    Add-UniqueWarning -List $warnings -Message "Get-NetTCPConnection is unavailable; TCP listeners and dependencies are omitted."
}

if ($null -ne (Get-Command Get-NetUDPEndpoint -ErrorAction SilentlyContinue)) {
    try { $allUdp = @(Get-NetUDPEndpoint -ErrorAction Stop) }
    catch { Add-UniqueWarning -List $warnings -Message "UDP endpoint enumeration failed: $($_.Exception.Message)" }
}
else {
    Add-UniqueWarning -List $warnings -Message "Get-NetUDPEndpoint is unavailable; UDP endpoints are omitted."
}

$observedProcesses = @(
    Get-Process -Name $normalizedNames -ErrorAction SilentlyContinue |
        Sort-Object ProcessName, Id
)

foreach ($process in $observedProcesses) {
    try {
        $process.Refresh()
        $pidValue = [int]$process.Id
        $cimProcess = $null
        try {
            $cimProcess = Get-CimInstance -ClassName Win32_Process -Filter "ProcessId = $pidValue" -ErrorAction Stop |
                Select-Object -First 1
        }
        catch {
            Add-UniqueWarning -List $warnings -Message "CIM process metadata could not be read for PID $pidValue: $($_.Exception.Message)"
        }

        $path = $null
        try { $path = $process.Path } catch {}
        if ([string]::IsNullOrWhiteSpace($path) -and $null -ne $cimProcess) {
            $path = [string]$cimProcess.ExecutablePath
        }

        $startTimeUtc = $null
        try { $startTimeUtc = $process.StartTime.ToUniversalTime() } catch {}
        $startIdentity = if ($null -eq $startTimeUtc) { "unknown" } else { [string]$startTimeUtc.Ticks }

        $parentProcessId = $null
        $parentProcessName = $null
        if ($null -ne $cimProcess -and $null -ne $cimProcess.ParentProcessId) {
            $parentProcessId = [int]$cimProcess.ParentProcessId
            try { $parentProcessName = (Get-Process -Id $parentProcessId -ErrorAction Stop).ProcessName } catch {}
        }

        $executableName = "$($process.ProcessName).exe"
        $peArchitecture = $null
        $fileSizeBytes = $null
        $lastWriteTimeUtc = $null
        $sha256 = $null
        $fileVersion = $null
        $productVersion = $null
        $companyName = $null
        $productName = $null

        if (-not [string]::IsNullOrWhiteSpace($path) -and (Test-Path -LiteralPath $path -PathType Leaf)) {
            try {
                $file = Get-Item -LiteralPath $path -ErrorAction Stop
                $executableName = $file.Name
                $fileSizeBytes = [int64]$file.Length
                $lastWriteTimeUtc = $file.LastWriteTimeUtc.ToString("o")
                $peArchitecture = Get-PeArchitecture -Path $path
                $version = [System.Diagnostics.FileVersionInfo]::GetVersionInfo($path)
                $fileVersion = $version.FileVersion
                $productVersion = $version.ProductVersion
                $companyName = $version.CompanyName
                $productName = $version.ProductName

                if (-not $SkipFileHash) {
                    try { $sha256 = (Get-FileHash -LiteralPath $path -Algorithm SHA256 -ErrorAction Stop).Hash }
                    catch { Add-UniqueWarning -List $warnings -Message "SHA-256 could not be calculated for '$path': $($_.Exception.Message)" }
                }
            }
            catch {
                Add-UniqueWarning -List $warnings -Message "Executable metadata could not be read for '$path': $($_.Exception.Message)"
            }
        }
        else {
            Add-UniqueWarning -List $warnings -Message "Executable path is unavailable for process '$($process.ProcessName)' PID $pidValue."
        }

        $processTcp = @($allTcp | Where-Object { [int]$_.OwningProcess -eq $pidValue })
        foreach ($connection in $processTcp) {
            $state = [string]$connection.State
            [void]$tcpRows.Add([pscustomobject]@{
                ProcessName = $process.ProcessName
                ProcessId = $pidValue
                State = $state
                IsListener = [bool]($state -eq "Listen")
                LocalAddress = [string]$connection.LocalAddress
                LocalPort = [int]$connection.LocalPort
                RemoteAddress = [string]$connection.RemoteAddress
                RemotePort = [int]$connection.RemotePort
            })
        }

        $processUdp = @($allUdp | Where-Object { [int]$_.OwningProcess -eq $pidValue })
        foreach ($endpoint in $processUdp) {
            [void]$udpRows.Add([pscustomobject]@{
                ProcessName = $process.ProcessName
                ProcessId = $pidValue
                LocalAddress = [string]$endpoint.LocalAddress
                LocalPort = [int]$endpoint.LocalPort
            })
        }

        $tcpListeners = @($processTcp | Where-Object { [string]$_.State -eq "Listen" })
        $tcpEstablished = @($processTcp | Where-Object { [string]$_.State -eq "Established" })

        $cpuTotalSeconds = $null
        $handleCount = $null
        $threadCount = $null
        try { $cpuTotalSeconds = [math]::Round([double]$process.CPU, 6) } catch {}
        try { $handleCount = [int]$process.HandleCount } catch {}
        try { $threadCount = [int]$process.Threads.Count } catch {}

        $commandLine = $null
        if ($IncludeCommandLine -and $null -ne $cimProcess) {
            $commandLine = [string]$cimProcess.CommandLine
        }

        [void]$processRows.Add([pscustomobject]@{
            ProcessName = $process.ProcessName
            ProcessId = $pidValue
            RuntimeInstanceId = "$($process.ProcessName):$($pidValue):$startIdentity"
            RuntimeConfirmed = $true
            ReadinessStatus = "unproven"
            StartTimeUtc = $(if ($null -eq $startTimeUtc) { $null } else { $startTimeUtc.ToString("o") })
            UptimeSeconds = $(if ($null -eq $startTimeUtc) { $null } else { [math]::Round(([DateTime]::UtcNow - $startTimeUtc).TotalSeconds, 3) })
            ParentProcessId = $parentProcessId
            ParentProcessName = $parentProcessName
            ExecutablePath = $path
            ExecutableName = $executableName
            PeArchitecture = $peArchitecture
            FileSizeBytes = $fileSizeBytes
            LastWriteTimeUtc = $lastWriteTimeUtc
            Sha256 = $sha256
            FileVersion = $fileVersion
            ProductVersion = $productVersion
            CompanyName = $companyName
            ProductName = $productName
            CpuTotalSeconds = $cpuTotalSeconds
            WorkingSetMB = [math]::Round($process.WorkingSet64 / 1MB, 3)
            PrivateMemoryMB = [math]::Round($process.PrivateMemorySize64 / 1MB, 3)
            HandleCount = $handleCount
            ThreadCount = $threadCount
            TcpListenerCount = $tcpListeners.Count
            TcpListeningPorts = @($tcpListeners | Select-Object -ExpandProperty LocalPort -Unique | Sort-Object)
            TcpEstablishedCount = $tcpEstablished.Count
            TcpRemoteEndpoints = @(
                $tcpEstablished |
                    ForEach-Object { "$($_.RemoteAddress):$($_.RemotePort)" } |
                    Sort-Object -Unique
            )
            UdpEndpointCount = $processUdp.Count
            UdpListeningPorts = @($processUdp | Select-Object -ExpandProperty LocalPort -Unique | Sort-Object)
            CommandLine = $commandLine
        })
    }
    catch {
        Add-UniqueWarning -List $warnings -Message "Process '$($process.ProcessName)' PID $($process.Id) changed or exited during capture: $($_.Exception.Message)"
    }
}

$observedNames = @($processRows | Select-Object -ExpandProperty ProcessName -Unique)
$missingExpected = @(
    foreach ($name in $normalizedNames) {
        if ($observedNames -notcontains $name) { $name }
    }
)

$duplicateProcessNames = @(
    $processRows |
        Group-Object ProcessName |
        Where-Object Count -gt 1 |
        ForEach-Object {
            [pscustomobject]@{
                ProcessName = $_.Name
                InstanceCount = $_.Count
                ProcessIds = @($_.Group | Select-Object -ExpandProperty ProcessId)
            }
        }
)

$result = [ordered]@{
    schemaVersion = 1
    generatedAtUtc = [DateTime]::UtcNow.ToString("o")
    repositoryRoot = $repositoryPath
    expectedProcessNames = @($normalizedNames)
    observedProcessNames = @($observedNames)
    missingExpectedProcessNames = @($missingExpected)
    processInstanceCount = $processRows.Count
    runtimeConfirmed = [bool]($processRows.Count -gt 0)
    allExpectedProcessesObserved = [bool]($missingExpected.Count -eq 0)
    includeCommandLine = [bool]$IncludeCommandLine
    fileHashingEnabled = [bool](-not $SkipFileHash)
    warning = "A running process confirms execution only. Readiness still requires a port, dependency handshake, health signal or explicit log marker."
    commandLineWarning = $(if ($IncludeCommandLine) { "Command lines may contain sensitive values; review output before sharing." } else { "Command-line capture disabled." })
    duplicateProcessNames = @($duplicateProcessNames)
    processes = @($processRows)
    tcpEndpoints = @($tcpRows)
    udpEndpoints = @($udpRows)
    warnings = @($warnings)
}

$jsonPath = Join-Path $OutputDirectory "runtime-topology.json"
$processCsvPath = Join-Path $OutputDirectory "runtime-processes.csv"
$tcpCsvPath = Join-Path $OutputDirectory "runtime-tcp-endpoints.csv"
$udpCsvPath = Join-Path $OutputDirectory "runtime-udp-endpoints.csv"
$markdownPath = Join-Path $OutputDirectory "runtime-topology.md"

$result | ConvertTo-Json -Depth 12 | Set-Content -LiteralPath $jsonPath -Encoding UTF8
@($processRows) |
    Select-Object ProcessName,ProcessId,RuntimeInstanceId,RuntimeConfirmed,ReadinessStatus,StartTimeUtc,UptimeSeconds,ParentProcessId,ParentProcessName,ExecutablePath,ExecutableName,PeArchitecture,FileSizeBytes,LastWriteTimeUtc,Sha256,FileVersion,ProductVersion,CpuTotalSeconds,WorkingSetMB,PrivateMemoryMB,HandleCount,ThreadCount,TcpListenerCount,@{n='TcpListeningPorts';e={$_.TcpListeningPorts -join ';'}},TcpEstablishedCount,@{n='TcpRemoteEndpoints';e={$_.TcpRemoteEndpoints -join ';'}},UdpEndpointCount,@{n='UdpListeningPorts';e={$_.UdpListeningPorts -join ';'}},CommandLine |
    Export-Csv -LiteralPath $processCsvPath -NoTypeInformation -Encoding UTF8
@($tcpRows) | Export-Csv -LiteralPath $tcpCsvPath -NoTypeInformation -Encoding UTF8
@($udpRows) | Export-Csv -LiteralPath $udpCsvPath -NoTypeInformation -Encoding UTF8

$markdown = New-Object System.Text.StringBuilder
[void]$markdown.AppendLine("# Runtime topology")
[void]$markdown.AppendLine("")
[void]$markdown.AppendLine("- Expected process names: $($normalizedNames -join ', ')")
[void]$markdown.AppendLine("- Observed instances: $($processRows.Count)")
[void]$markdown.AppendLine("- Missing expected processes: $($(if ($missingExpected.Count -eq 0) { 'none' } else { $missingExpected -join ', ' }))")
[void]$markdown.AppendLine("")
[void]$markdown.AppendLine("> Runtime observation does not prove readiness. Validate ports, dependency connections and explicit log-ready markers before timing startup.")
[void]$markdown.AppendLine("")
[void]$markdown.AppendLine("| Process | PID | Architecture | Executable | SHA-256 | TCP listen | Established | UDP | Readiness |")
[void]$markdown.AppendLine("|---|---:|---|---|---|---|---:|---|---|")
foreach ($row in $processRows) {
    $hash = ""
    if (-not [string]::IsNullOrWhiteSpace([string]$row.Sha256)) {
        $hash = if ($row.Sha256.Length -gt 16) { $row.Sha256.Substring(0, 16) + "..." } else { $row.Sha256 }
    }
    [void]$markdown.AppendLine("| $(Convert-ToMarkdownCell $row.ProcessName) | $($row.ProcessId) | $(Convert-ToMarkdownCell $row.PeArchitecture) | $(Convert-ToMarkdownCell $row.ExecutablePath) | $hash | $(Convert-ToMarkdownCell ($row.TcpListeningPorts -join ', ')) | $($row.TcpEstablishedCount) | $(Convert-ToMarkdownCell ($row.UdpListeningPorts -join ', ')) | $($row.ReadinessStatus) |")
}
$markdown.ToString() | Set-Content -LiteralPath $markdownPath -Encoding UTF8

if ($missingExpected.Count -gt 0) {
    Write-Warning "Expected processes not observed: $($missingExpected -join ', ')"
}

[pscustomobject]@{
    OutputDirectory = $OutputDirectory
    Json = $jsonPath
    ProcessesCsv = $processCsvPath
    TcpEndpointsCsv = $tcpCsvPath
    UdpEndpointsCsv = $udpCsvPath
    Markdown = $markdownPath
    ProcessInstanceCount = $processRows.Count
    MissingExpectedProcessCount = $missingExpected.Count
    WarningCount = $warnings.Count
}
