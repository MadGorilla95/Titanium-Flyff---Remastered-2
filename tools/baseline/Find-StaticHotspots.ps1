[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Join-Path $PSScriptRoot "..\.."),
    [string]$SourceRoot = "Source\Source",
    [string]$OutputDirectory = "",
    [string[]]$ExcludeDirectories = @(
        ".git", ".vs", "Debug", "Release", "x64", "x86", "Output",
        "artifacts", "DX_SDK", "DirectX", "Lib", "Lib64", "CrashRpt",
        "third_party", "ThirdParty", "vendor"
    )
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-RelativePath {
    param(
        [Parameter(Mandatory = $true)][string]$BasePath,
        [Parameter(Mandatory = $true)][string]$Path
    )

    $baseFull = [System.IO.Path]::GetFullPath($BasePath).TrimEnd("\", "/") + [System.IO.Path]::DirectorySeparatorChar
    $pathFull = [System.IO.Path]::GetFullPath($Path)
    $baseUri = New-Object System.Uri($baseFull)
    $pathUri = New-Object System.Uri($pathFull)
    return [System.Uri]::UnescapeDataString(
        $baseUri.MakeRelativeUri($pathUri).ToString().Replace("/", [System.IO.Path]::DirectorySeparatorChar)
    )
}

function Test-IsExcluded {
    param(
        [Parameter(Mandatory = $true)][string]$RelativePath,
        [Parameter(Mandatory = $true)][string[]]$Names
    )

    foreach ($segment in @($RelativePath -split "[\\/]")) {
        if ($Names -contains $segment) {
            return $true
        }
    }
    return $false
}

$repositoryPath = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$sourcePath = $SourceRoot
if (-not [System.IO.Path]::IsPathRooted($sourcePath)) {
    $sourcePath = Join-Path $repositoryPath $sourcePath
}
$sourcePath = (Resolve-Path -LiteralPath $sourcePath).Path

if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $OutputDirectory = Join-Path $repositoryPath "artifacts\baseline\static-scan"
}
$OutputDirectory = [System.IO.Path]::GetFullPath($OutputDirectory)
[void](New-Item -ItemType Directory -Path $OutputDirectory -Force)

$extensions = @(".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".inl")
$patternDefinitions = @(
    [pscustomobject]@{
        Category = "thread-delay"
        Signature = "Sleep/SleepEx"
        Priority = "high-review"
        Pattern = '\bSleep(?:Ex)?\s*\('
        Rationale = "Delay may serialize startup or hide synchronization defects; inspect duration and call context."
    },
    [pscustomobject]@{
        Category = "blocking-wait"
        Signature = "WaitFor*Object"
        Priority = "high-review"
        Pattern = '\bWaitFor(?:Single|Multiple)Objects?(?:Ex)?\s*\('
        Rationale = "Potential blocking wait. Verify timeout, owning thread and shutdown behavior."
    },
    [pscustomobject]@{
        Category = "infinite-loop"
        Signature = "while(true)/for(;;)"
        Priority = "high-review"
        Pattern = '\bwhile\s*\(\s*(?:true|TRUE|1)\s*\)|\bfor\s*\(\s*;\s*;\s*\)'
        Rationale = "May be a valid service loop, a busy loop or a polling loop. Confirm wait/yield behavior."
    },
    [pscustomobject]@{
        Category = "synchronous-file-io"
        Signature = "Win32/CRT/stream file I/O"
        Priority = "medium-review"
        Pattern = '\b(?:CreateFileA|CreateFileW|CreateFile|ReadFile|WriteFile|fopen|fread|fwrite|ifstream|ofstream)\s*\(?'
        Rationale = "Synchronous file I/O can block a critical thread; presence alone is not a defect."
    },
    [pscustomobject]@{
        Category = "socket-io"
        Signature = "socket call"
        Priority = "medium-review"
        Pattern = '\b(?:accept|connect|recv|recvfrom|send|sendto|select|WSARecv|WSASend)\s*\('
        Rationale = "Determine whether the socket is blocking and which thread owns the call."
    },
    [pscustomobject]@{
        Category = "database-call"
        Signature = "ODBC/query execution"
        Priority = "medium-review"
        Pattern = '\b(?:SQLExecDirectA?|SQLExecDirectW|SQLExecute|SQLFetch|SQLMoreResults|ExecuteQuery|ExecQuery|ExecuteSQL)\s*\('
        Rationale = "Correlate with Extended Events before changing SQL or adding indexes."
    },
    [pscustomobject]@{
        Category = "critical-section"
        Signature = "critical section / mutex"
        Priority = "medium-review"
        Pattern = '\b(?:EnterCriticalSection|TryEnterCriticalSection|Lock|LockGuard|Mutex)\b'
        Rationale = "Review lock scope and contention only after runtime evidence."
    },
    [pscustomobject]@{
        Category = "full-scan-candidate"
        Signature = "container/list traversal"
        Priority = "low-review"
        Pattern = '\bfor\s*\([^;]*;[^;]*(?:GetSize\s*\(|\.size\s*\(|\.end\s*\()|GetHeadPosition\s*\('
        Rationale = "Potential full collection scan. Confirm cardinality, frequency and nesting."
    }
)

$sourceFiles = @(
    Get-ChildItem -LiteralPath $sourcePath -Recurse -File -Force |
        Where-Object { $extensions -contains $_.Extension.ToLowerInvariant() } |
        ForEach-Object {
            $relativeToRepo = Get-RelativePath -BasePath $repositoryPath -Path $_.FullName
            if (-not (Test-IsExcluded -RelativePath $relativeToRepo -Names $ExcludeDirectories)) {
                [pscustomobject]@{
                    File = $_
                    RelativePath = $relativeToRepo
                }
            }
        }
)

Write-Host "Scanning $($sourceFiles.Count) source files under $sourcePath"

$findings = New-Object System.Collections.ArrayList
$readFailures = New-Object System.Collections.ArrayList

foreach ($entry in $sourceFiles) {
    try {
        $lineNumber = 0
        foreach ($line in (Get-Content -LiteralPath $entry.File.FullName -ErrorAction Stop)) {
            $lineNumber++
            foreach ($definition in $patternDefinitions) {
                if ($line -match $definition.Pattern) {
                    [void]$findings.Add([pscustomobject]@{
                        Path = $entry.RelativePath
                        Line = $lineNumber
                        Category = $definition.Category
                        Signature = $definition.Signature
                        ReviewPriority = $definition.Priority
                        Preview = $line.Trim()
                        Rationale = $definition.Rationale
                        Status = "unreviewed-candidate"
                    })
                }
            }
        }
    }
    catch {
        [void]$readFailures.Add([pscustomobject]@{
            Path = $entry.RelativePath
            Error = $_.Exception.Message
        })
    }
}

$summary = @(
    $findings |
        Group-Object Category |
        ForEach-Object {
            [pscustomobject]@{
                Category = $_.Name
                Count = $_.Count
                Files = @($_.Group | Select-Object -ExpandProperty Path -Unique).Count
                Status = "candidate-count-only"
            }
        } |
        Sort-Object Count -Descending
)

$generatedAt = [DateTime]::UtcNow.ToString("o")
$result = [ordered]@{
    generatedAtUtc = $generatedAt
    repositoryRoot = $repositoryPath
    sourceRoot = $sourcePath
    filesScanned = $sourceFiles.Count
    findingCount = $findings.Count
    readFailureCount = $readFailures.Count
    warning = "Every item is a static review candidate, not a confirmed performance defect."
    summary = @($summary)
    findings = @($findings)
    readFailures = @($readFailures)
}

$jsonPath = Join-Path $OutputDirectory "static-hotspots.json"
$result | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $jsonPath -Encoding UTF8

$csvPath = Join-Path $OutputDirectory "static-hotspots.csv"
@($findings) | Export-Csv -LiteralPath $csvPath -NoTypeInformation -Encoding UTF8

$failureCsvPath = Join-Path $OutputDirectory "read-failures.csv"
@($readFailures) | Export-Csv -LiteralPath $failureCsvPath -NoTypeInformation -Encoding UTF8

$markdown = New-Object System.Text.StringBuilder
[void]$markdown.AppendLine("# Static hotspot candidates")
[void]$markdown.AppendLine("")
[void]$markdown.AppendLine("- Generated UTC: $generatedAt")
[void]$markdown.AppendLine("- Files scanned: $($sourceFiles.Count)")
[void]$markdown.AppendLine("- Candidates: $($findings.Count)")
[void]$markdown.AppendLine("- Read failures: $($readFailures.Count)")
[void]$markdown.AppendLine("")
[void]$markdown.AppendLine("> These are search hits, not proven bottlenecks. Correlate each candidate with runtime measurements before changing code.")
[void]$markdown.AppendLine("")
[void]$markdown.AppendLine("| Category | Candidates | Files | Status |")
[void]$markdown.AppendLine("|---|---:|---:|---|")
foreach ($item in $summary) {
    [void]$markdown.AppendLine("| $($item.Category) | $($item.Count) | $($item.Files) | $($item.Status) |")
}

[void]$markdown.AppendLine("")
[void]$markdown.AppendLine("## First review queue")
[void]$markdown.AppendLine("")
[void]$markdown.AppendLine("| Priority | Category | Location | Preview |")
[void]$markdown.AppendLine("|---|---|---|---|")
foreach ($item in @($findings | Sort-Object ReviewPriority, Category, Path, Line | Select-Object -First 200)) {
    $preview = (($item.Preview -replace '\|', '\|') -replace "`t", " ")
    if ($preview.Length -gt 160) {
        $preview = $preview.Substring(0, 157) + "..."
    }
    [void]$markdown.AppendLine("| $($item.ReviewPriority) | $($item.Category) | $($item.Path):$($item.Line) | ``$preview`` |")
}

$markdownPath = Join-Path $OutputDirectory "static-hotspots.md"
$markdown.ToString() | Set-Content -LiteralPath $markdownPath -Encoding UTF8

[pscustomobject]@{
    OutputDirectory = $OutputDirectory
    Json = $jsonPath
    Csv = $csvPath
    Markdown = $markdownPath
    FilesScanned = $sourceFiles.Count
    FindingCount = $findings.Count
    ReadFailureCount = $readFailures.Count
}
