[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateNotNullOrEmpty()]
    [string]$LogPath,

    [string]$OutputDirectory = "",
    [int]$ExitCode = 0,
    [string]$RepositoryRoot = "",
    [string]$Solution = "",
    [string]$Configuration = "",
    [string]$Platform = "",
    [string]$MSBuildPath = "",
    [string]$MSBuildVersion = "",
    [double]$ElapsedMilliseconds = 0
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-RelativeDisplayPath {
    param(
        [string]$Root,
        [string]$Path
    )

    if ([string]::IsNullOrWhiteSpace($Path)) {
        return ""
    }

    if ([string]::IsNullOrWhiteSpace($Root)) {
        return $Path
    }

    try {
        $rootFull = [IO.Path]::GetFullPath($Root).TrimEnd('\', '/') + [IO.Path]::DirectorySeparatorChar
        $pathFull = [IO.Path]::GetFullPath($Path)
        $rootUri = [Uri]$rootFull
        $pathUri = [Uri]$pathFull
        $relative = [Uri]::UnescapeDataString($rootUri.MakeRelativeUri($pathUri).ToString())
        return $relative.Replace('/', [IO.Path]::DirectorySeparatorChar)
    }
    catch {
        return $Path
    }
}

function Get-BuildFailureClassification {
    param(
        [string]$Severity,
        [string]$Code,
        [string]$Message,
        [string]$MissingInput
    )

    if ($Severity -eq 'warning') {
        switch -Regex ($Code) {
            '^C4244$' { return 'narrowing-conversion' }
            '^C4267$' { return 'size-or-pointer-narrowing' }
            '^C4996$' { return 'deprecated-or-unsafe-api' }
            default { return 'compiler-warning' }
        }
    }

    if (-not [string]::IsNullOrWhiteSpace($MissingInput)) {
        $baseName = [IO.Path]::GetFileName($MissingInput)
        if ($baseName -match '^(?i)(afx|atl).+\.h$') {
            return 'missing-mfc-or-atl-header'
        }

        if (
            $baseName -match '^(?i)(lang|define|defineitem|defineitemkind|errorcode)\.h$' -or
            $MissingInput -match '(?i)(^|[\\/])resource[\\/]'
        ) {
            return 'missing-generated-resource-header'
        }

        return 'missing-include-or-sdk'
    }

    switch -Regex ($Code) {
        '^LNK' { return 'linker-error' }
        '^MSB' { return 'msbuild-error' }
        default { return 'compiler-error' }
    }
}

function Test-ContinuationDiagnostic {
    param(
        [string]$Severity,
        [string]$Message
    )

    if ($Severity -ne 'warning') {
        return $false
    }

    $trimmed = $Message.Trim()
    return [bool](
        $trimmed -match '^(?i:with)$' -or
        $trimmed -eq '[' -or
        $trimmed -eq ']' -or
        $trimmed -match '^_Ty\d+\s*=' -or
        $trimmed -match '^(?i:see reference to)'
    )
}

$logFullPath = (Resolve-Path -LiteralPath $LogPath).Path
if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $OutputDirectory = Split-Path $logFullPath -Parent
}
$OutputDirectory = [IO.Path]::GetFullPath($OutputDirectory)
New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null

$lines = @(Get-Content -LiteralPath $logFullPath -ErrorAction Stop)
$reportedWarningCount = $null
$reportedErrorCount = $null

foreach ($line in $lines) {
    $warningSummary = [regex]::Match($line, '^\s*(?<count>\d+)\s+Warning\(s\)\s*$')
    if ($warningSummary.Success) {
        $reportedWarningCount = [int]$warningSummary.Groups['count'].Value
    }

    $errorSummary = [regex]::Match($line, '^\s*(?<count>\d+)\s+Error\(s\)\s*$')
    if ($errorSummary.Success) {
        $reportedErrorCount = [int]$errorSummary.Groups['count'].Value
    }
}

$diagnosticOccurrences = @{}
$directProjectFailures = New-Object Collections.Generic.HashSet[string]([StringComparer]::OrdinalIgnoreCase)
$failedBuildNodes = New-Object Collections.Generic.HashSet[string]([StringComparer]::OrdinalIgnoreCase)

foreach ($originalLine in $lines) {
    $line = [string]$originalLine

    $failedNodeMatch = [regex]::Match(
        $line,
        '(?i)Done Building Project\s+"(?<project>[^"]+)"\s+--\s+FAILED\.'
    )
    if ($failedNodeMatch.Success) {
        [void]$failedBuildNodes.Add($failedNodeMatch.Groups['project'].Value)
    }

    $normalized = [regex]::Replace($line, '^\s*\d+>', '')
    $project = ''
    $projectMatch = [regex]::Match(
        $normalized,
        '\s+\[(?<project>[A-Za-z]:\\[^\]]+\.(?:vcxproj|csproj|sln))\]\s*$'
    )
    if ($projectMatch.Success) {
        $project = $projectMatch.Groups['project'].Value
        $normalized = $normalized.Substring(0, $projectMatch.Index)
    }

    $diagnosticMatch = [regex]::Match(
        $normalized,
        '^\s*(?<file>.+?)\((?<line>\d+),(?<column>\d+)\)\s*:\s*(?<severity>warning|error)\s+(?<code>[A-Za-z]+\d+)\s*:\s*(?<message>.+?)\s*$'
    )
    if (-not $diagnosticMatch.Success) {
        continue
    }

    $severity = $diagnosticMatch.Groups['severity'].Value.ToLowerInvariant()
    $code = $diagnosticMatch.Groups['code'].Value.ToUpperInvariant()
    $message = $diagnosticMatch.Groups['message'].Value.Trim()

    if (Test-ContinuationDiagnostic -Severity $severity -Message $message) {
        continue
    }

    $sourceFile = $diagnosticMatch.Groups['file'].Value.Trim()
    $lineNumber = [int]$diagnosticMatch.Groups['line'].Value
    $columnNumber = [int]$diagnosticMatch.Groups['column'].Value
    $missingInput = ''

    $missingMatch = [regex]::Match(
        $message,
        "(?i)Cannot open include file:\s*'(?<include>[^']+)'"
    )
    if ($missingMatch.Success) {
        $missingInput = $missingMatch.Groups['include'].Value.Trim().Replace('/', '\')
    }

    $classification = Get-BuildFailureClassification `
        -Severity $severity `
        -Code $code `
        -Message $message `
        -MissingInput $missingInput

    if ($severity -eq 'error' -and -not [string]::IsNullOrWhiteSpace($project)) {
        [void]$directProjectFailures.Add($project)
    }

    $signature = @(
        $severity,
        $code,
        $sourceFile.ToLowerInvariant(),
        $lineNumber,
        $columnNumber,
        $project.ToLowerInvariant(),
        $message
    ) -join '|'

    if (-not $diagnosticOccurrences.ContainsKey($signature)) {
        $diagnosticOccurrences[$signature] = [pscustomobject]@{
            Severity = $severity
            Code = $code
            Classification = $classification
            SourceFile = $sourceFile
            SourceFileDisplay = Get-RelativeDisplayPath -Root $RepositoryRoot -Path $sourceFile
            LineNumber = $lineNumber
            ColumnNumber = $columnNumber
            Project = $project
            ProjectDisplay = Get-RelativeDisplayPath -Root $RepositoryRoot -Path $project
            MissingInput = $missingInput
            Message = $message
            RawOccurrences = 0
        }
    }

    $diagnosticOccurrences[$signature].RawOccurrences++
}

$diagnostics = @(
    $diagnosticOccurrences.Values |
        Sort-Object Severity, Code, ProjectDisplay, SourceFileDisplay, LineNumber, ColumnNumber, Message
)

$uniqueWarnings = @($diagnostics | Where-Object Severity -eq 'warning')
$uniqueErrors = @($diagnostics | Where-Object Severity -eq 'error')

$authoritativeWarningCount = if ($null -ne $reportedWarningCount) {
    $reportedWarningCount
}
else {
    $uniqueWarnings.Count
}

$authoritativeErrorCount = if ($null -ne $reportedErrorCount) {
    $reportedErrorCount
}
else {
    $uniqueErrors.Count
}

$missingInputRows = @(
    $uniqueErrors |
        Where-Object { -not [string]::IsNullOrWhiteSpace($_.MissingInput) } |
        Group-Object { [IO.Path]::GetFileName($_.MissingInput).ToLowerInvariant() } |
        ForEach-Object {
            $groupRows = @($_.Group)
            $first = $groupRows[0]
            [pscustomobject]@{
                MissingInput = $first.MissingInput
                Basename = [IO.Path]::GetFileName($first.MissingInput)
                Classification = $first.Classification
                UniqueDiagnosticCount = $groupRows.Count
                RawOccurrenceCount = [int](($groupRows | Measure-Object RawOccurrences -Sum).Sum)
                AffectedProjects = @($groupRows | ForEach-Object ProjectDisplay | Where-Object { $_ } | Sort-Object -Unique)
                AffectedSources = @($groupRows | ForEach-Object SourceFileDisplay | Where-Object { $_ } | Sort-Object -Unique)
            }
        } |
        Sort-Object Basename
)

$warningCodes = @(
    $uniqueWarnings |
        Group-Object Code |
        ForEach-Object {
            [pscustomobject]@{
                Code = $_.Name
                UniqueSignatures = $_.Count
            }
        } |
        Sort-Object -Property @{Expression='UniqueSignatures';Descending=$true}, Code
)

$errorCodes = @(
    $uniqueErrors |
        Group-Object Code |
        ForEach-Object {
            [pscustomobject]@{
                Code = $_.Name
                UniqueSignatures = $_.Count
            }
        } |
        Sort-Object -Property @{Expression='UniqueSignatures';Descending=$true}, Code
)

$failureClassifications = @(
    $uniqueErrors |
        Group-Object Classification |
        ForEach-Object {
            [pscustomobject]@{
                Classification = $_.Name
                UniqueSignatures = $_.Count
            }
        } |
        Sort-Object -Property @{Expression='UniqueSignatures';Descending=$true}, Classification
)

$allFailedNodes = @(
    @($directProjectFailures) + @($failedBuildNodes) |
        Sort-Object -Unique
)

$diagnosticsCsvPath = Join-Path $OutputDirectory 'build-diagnostics.csv'
$missingInputsCsvPath = Join-Path $OutputDirectory 'missing-build-inputs.csv'
$failedProjectsCsvPath = Join-Path $OutputDirectory 'failed-projects.csv'
$summaryJsonPath = Join-Path $OutputDirectory 'build-summary.json'
$summaryMarkdownPath = Join-Path $OutputDirectory 'BUILD-SUMMARY.md'

@($diagnostics) |
    Select-Object Severity, Code, Classification, SourceFileDisplay, LineNumber, ColumnNumber, ProjectDisplay, MissingInput, Message, RawOccurrences |
    Export-Csv -LiteralPath $diagnosticsCsvPath -NoTypeInformation -Encoding UTF8

@($missingInputRows) |
    Select-Object MissingInput, Basename, Classification, UniqueDiagnosticCount, RawOccurrenceCount,
        @{n='AffectedProjects';e={$_.AffectedProjects -join ';'}},
        @{n='AffectedSources';e={$_.AffectedSources -join ';'}} |
    Export-Csv -LiteralPath $missingInputsCsvPath -NoTypeInformation -Encoding UTF8

@($allFailedNodes | ForEach-Object {
    [pscustomobject]@{
        Project = $_
        ProjectDisplay = Get-RelativeDisplayPath -Root $RepositoryRoot -Path $_
        HasDirectDiagnostic = [bool]$directProjectFailures.Contains($_)
        BuildNodeFailed = [bool]$failedBuildNodes.Contains($_)
    }
}) | Export-Csv -LiteralPath $failedProjectsCsvPath -NoTypeInformation -Encoding UTF8

$failureReason = ''
if ($ExitCode -ne 0) {
    if ($missingInputRows.Count -gt 0) {
        $failureReason = 'missing-build-inputs'
    }
    elseif ($authoritativeErrorCount -gt 0) {
        $failureReason = 'compiler-or-linker-errors'
    }
    else {
        $failureReason = 'msbuild-nonzero-exit'
    }
}

$summary = [ordered]@{
    schemaVersion = 3
    generatedAtUtc = [DateTime]::UtcNow.ToString('o')
    repositoryRoot = $RepositoryRoot
    solution = $Solution
    configuration = $Configuration
    platform = $Platform
    msbuildPath = $MSBuildPath
    msbuildVersion = $MSBuildVersion
    elapsedMilliseconds = [math]::Round($ElapsedMilliseconds, 3)
    exitCode = $ExitCode
    succeeded = [bool]($ExitCode -eq 0 -and $authoritativeErrorCount -eq 0)
    failureReason = $failureReason
    reproducibleFromThisEnvironment = [bool]($ExitCode -eq 0 -and $authoritativeErrorCount -eq 0)
    warningCount = $authoritativeWarningCount
    errorCount = $authoritativeErrorCount
    reportedWarningCount = $reportedWarningCount
    reportedErrorCount = $reportedErrorCount
    uniqueWarningSignatures = $uniqueWarnings.Count
    uniqueErrorSignatures = $uniqueErrors.Count
    warningCodes = @($warningCodes)
    errorCodes = @($errorCodes)
    failureClassifications = @($failureClassifications)
    missingBuildInputCount = $missingInputRows.Count
    missingBuildInputs = @($missingInputRows)
    directFailureProjects = @($directProjectFailures | Sort-Object)
    failedBuildNodes = @($failedBuildNodes | Sort-Object)
    failedProjects = @($allFailedNodes)
    diagnosticCount = $diagnostics.Count
    diagnosticsPath = $diagnosticsCsvPath
    missingInputsPath = $missingInputsCsvPath
    failedProjectsPath = $failedProjectsCsvPath
    logPath = $logFullPath
    warningObservability = 'MSBuild summary counts are authoritative for this run. Warning coverage remains incomplete when project configurations use /W0 or TurnOffAllWarnings.'
}
$summary | ConvertTo-Json -Depth 12 | Set-Content -LiteralPath $summaryJsonPath -Encoding UTF8

$markdown = New-Object Text.StringBuilder
[void]$markdown.AppendLine('# Canonical build baseline')
[void]$markdown.AppendLine('')
[void]$markdown.AppendLine("- Solution: ``$Solution``")
[void]$markdown.AppendLine("- Configuration: ``$Configuration|$Platform``")
[void]$markdown.AppendLine("- MSBuild: ``$MSBuildVersion``")
[void]$markdown.AppendLine("- Exit code: **$ExitCode**")
[void]$markdown.AppendLine("- Succeeded: **$($summary.succeeded)**")
[void]$markdown.AppendLine("- Duration: $([math]::Round($ElapsedMilliseconds / 1000.0, 3)) s")
[void]$markdown.AppendLine("- Authoritative warnings: $authoritativeWarningCount")
[void]$markdown.AppendLine("- Authoritative errors: $authoritativeErrorCount")
[void]$markdown.AppendLine('')

if ($missingInputRows.Count -gt 0) {
    [void]$markdown.AppendLine('## Missing build inputs')
    [void]$markdown.AppendLine('')
    [void]$markdown.AppendLine('| Missing input | Classification | Unique diagnostics | Affected projects |')
    [void]$markdown.AppendLine('|---|---|---:|---|')
    foreach ($row in $missingInputRows) {
        $projects = ($row.AffectedProjects -join ', ').Replace('|', '\|')
        [void]$markdown.AppendLine("| ``$($row.Basename)`` | $($row.Classification) | $($row.UniqueDiagnosticCount) | $projects |")
    }
    [void]$markdown.AppendLine('')
}

if ($uniqueWarnings.Count -gt 0) {
    [void]$markdown.AppendLine('## Warning signatures')
    [void]$markdown.AppendLine('')
    [void]$markdown.AppendLine('| Code | Classification | Project | Message |')
    [void]$markdown.AppendLine('|---|---|---|---|')
    foreach ($warning in $uniqueWarnings) {
        $message = $warning.Message.Replace('|', '\|')
        [void]$markdown.AppendLine("| ``$($warning.Code)`` | $($warning.Classification) | $($warning.ProjectDisplay) | $message |")
    }
    [void]$markdown.AppendLine('')
}

[void]$markdown.AppendLine('> A green audit-tooling workflow does not mean the game solution compiled. Use the build exit code and this report as the source of truth.')
$markdown.ToString() | Set-Content -LiteralPath $summaryMarkdownPath -Encoding UTF8

[pscustomobject]@{
    SummaryPath = $summaryJsonPath
    SummaryMarkdown = $summaryMarkdownPath
    DiagnosticsPath = $diagnosticsCsvPath
    MissingInputsPath = $missingInputsCsvPath
    FailedProjectsPath = $failedProjectsCsvPath
    Succeeded = $summary.succeeded
    ExitCode = $ExitCode
    WarningCount = $authoritativeWarningCount
    ErrorCount = $authoritativeErrorCount
    MissingBuildInputCount = $missingInputRows.Count
}
