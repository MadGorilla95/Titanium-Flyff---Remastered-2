[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Join-Path $PSScriptRoot "..\.."),
    [string]$ConfigPath = "",
    [string[]]$AdditionalSearchRoot = @(),
    [string]$OutputDirectory = "",
    [ValidateRange(1, 10000)]
    [int]$MaxCandidatesPerInput = 100,
    [switch]$IncludeAbsoluteExternalPaths,
    [switch]$SkipFileHash,
    [switch]$SkipLegacyPreprocessedReferences
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-OptionalProperty {
    param(
        [object]$Object,
        [Parameter(Mandatory = $true)][string]$Name,
        [object]$Default = $null
    )

    if ($null -eq $Object) { return $Default }
    $property = $Object.PSObject.Properties[$Name]
    if ($null -eq $property) { return $Default }
    return $property.Value
}

function Resolve-FromBase {
    param(
        [Parameter(Mandatory = $true)][string]$Base,
        [Parameter(Mandatory = $true)][string]$Path
    )

    if ([IO.Path]::IsPathRooted($Path)) {
        return [IO.Path]::GetFullPath($Path)
    }

    return [IO.Path]::GetFullPath((Join-Path $Base $Path))
}

function Get-RelativePath {
    param(
        [Parameter(Mandatory = $true)][string]$Base,
        [Parameter(Mandatory = $true)][string]$Path
    )

    $baseFull = [IO.Path]::GetFullPath($Base).TrimEnd('\', '/') + [IO.Path]::DirectorySeparatorChar
    $pathFull = [IO.Path]::GetFullPath($Path)
    $baseUri = [Uri]$baseFull
    $pathUri = [Uri]$pathFull
    $relative = [Uri]::UnescapeDataString($baseUri.MakeRelativeUri($pathUri).ToString())
    return $relative.Replace('/', [IO.Path]::DirectorySeparatorChar)
}

function Test-IsUnderPath {
    param(
        [Parameter(Mandatory = $true)][string]$Base,
        [Parameter(Mandatory = $true)][string]$Path
    )

    $baseFull = [IO.Path]::GetFullPath($Base).TrimEnd('\', '/') + [IO.Path]::DirectorySeparatorChar
    $pathFull = [IO.Path]::GetFullPath($Path)
    return $pathFull.StartsWith($baseFull, [StringComparison]::OrdinalIgnoreCase)
}

function Convert-ToMarkdownCell {
    param([object]$Value)

    if ($null -eq $Value) { return '' }
    return ([string]$Value).Replace('|', '\|').Replace("`r", ' ').Replace("`n", ' ')
}

function Get-GitPathEvidence {
    param(
        [Parameter(Mandatory = $true)][string]$Repository,
        [Parameter(Mandatory = $true)][string]$RelativePath,
        [bool]$GitAvailable
    )

    $result = [ordered]@{
        tracked = $false
        ignored = $false
        ignoreSource = ''
        ignoreLine = $null
        ignorePattern = ''
        historyCommitCount = 0
        historyCommits = @()
    }

    if (-not $GitAvailable) {
        return [pscustomobject]$result
    }

    $gitPath = $RelativePath.Replace('\', '/')
    Push-Location $Repository
    try {
        & git ls-files --error-unmatch -- $gitPath *> $null
        $result.tracked = [bool]($LASTEXITCODE -eq 0)

        $ignoreOutput = @(& git check-ignore -v --no-index -- $gitPath 2>$null)
        if ($LASTEXITCODE -eq 0 -and $ignoreOutput.Count -gt 0) {
            $result.ignored = $true
            $first = [string]$ignoreOutput[0]
            $match = [regex]::Match($first, '^(?<source>.*):(?<line>\d+):(?<pattern>.*?)\t(?<path>.*)$')
            if ($match.Success) {
                $result.ignoreSource = $match.Groups['source'].Value
                $result.ignoreLine = [int]$match.Groups['line'].Value
                $result.ignorePattern = $match.Groups['pattern'].Value
            }
            else {
                $result.ignorePattern = $first
            }
        }

        $history = @(& git log --all --format=%H -- $gitPath 2>$null | Where-Object { $_ })
        $result.historyCommitCount = $history.Count
        $result.historyCommits = @($history | Select-Object -First 20)
    }
    finally {
        Pop-Location
    }

    return [pscustomobject]$result
}

function Add-Candidate {
    param(
        [Parameter(Mandatory = $true)][hashtable]$CandidateIndex,
        [Parameter(Mandatory = $true)][string]$RequiredInput,
        [Parameter(Mandatory = $true)][string]$FullPath,
        [Parameter(Mandatory = $true)][string]$DiscoverySource,
        [Parameter(Mandatory = $true)][string]$RootLabel,
        [Parameter(Mandatory = $true)][string]$RootPath,
        [Parameter(Mandatory = $true)][string]$Repository,
        [Parameter(Mandatory = $true)][bool]$GitAvailable,
        [Parameter(Mandatory = $true)][bool]$ExposeAbsoluteExternalPath,
        [Parameter(Mandatory = $true)][bool]$OmitHash
    )

    $resolved = [IO.Path]::GetFullPath($FullPath)
    $key = $RequiredInput.ToLowerInvariant() + '|' + $resolved.ToLowerInvariant()
    if ($CandidateIndex.ContainsKey($key)) {
        $existing = $CandidateIndex[$key]
        if (-not $existing.DiscoverySources.Contains($DiscoverySource)) {
            [void]$existing.DiscoverySources.Add($DiscoverySource)
        }
        return
    }

    $file = Get-Item -LiteralPath $resolved -ErrorAction Stop
    $underRepository = Test-IsUnderPath -Base $Repository -Path $resolved
    $displayPath = ''
    $absolutePath = $null
    $relativePath = ''
    $gitEvidence = [pscustomobject]@{
        tracked = $false
        ignored = $false
        ignoreSource = ''
        ignoreLine = $null
        ignorePattern = ''
        historyCommitCount = 0
        historyCommits = @()
    }

    if ($underRepository) {
        $relativePath = Get-RelativePath -Base $Repository -Path $resolved
        $displayPath = $relativePath
        $gitEvidence = Get-GitPathEvidence -Repository $Repository -RelativePath $relativePath -GitAvailable $GitAvailable
    }
    else {
        $rootRelative = Get-RelativePath -Base $RootPath -Path $resolved
        $displayPath = '<' + $RootLabel + '>\' + $rootRelative
        if ($ExposeAbsoluteExternalPath) {
            $absolutePath = $resolved
        }
    }

    $sha256 = ''
    if (-not $OmitHash) {
        try {
            $sha256 = (Get-FileHash -LiteralPath $resolved -Algorithm SHA256).Hash
        }
        catch {
            $sha256 = ''
        }
    }

    $sources = New-Object Collections.ArrayList
    [void]$sources.Add($DiscoverySource)

    $CandidateIndex[$key] = [pscustomobject]@{
        RequiredInput = $RequiredInput
        DisplayPath = $displayPath
        AbsolutePath = $absolutePath
        UnderRepository = $underRepository
        RepositoryRelativePath = $relativePath
        RootLabel = $RootLabel
        DiscoverySources = $sources
        SizeBytes = [int64]$file.Length
        LastWriteTimeUtc = $file.LastWriteTimeUtc.ToString('o')
        SHA256 = $sha256
        GitTracked = [bool]$gitEvidence.tracked
        GitIgnored = [bool]$gitEvidence.ignored
        GitIgnoreSource = [string]$gitEvidence.ignoreSource
        GitIgnoreLine = $gitEvidence.ignoreLine
        GitIgnorePattern = [string]$gitEvidence.ignorePattern
        GitHistoryCommitCount = [int]$gitEvidence.historyCommitCount
        GitHistoryCommits = @($gitEvidence.historyCommits)
    }
}

$repositoryPath = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$localConfig = Join-Path $PSScriptRoot 'baseline.config.local.json'
$exampleConfig = Join-Path $PSScriptRoot 'baseline.config.example.json'
if ([string]::IsNullOrWhiteSpace($ConfigPath)) {
    $ConfigPath = if (Test-Path -LiteralPath $localConfig) { $localConfig } else { $exampleConfig }
}
$configFullPath = (Resolve-Path -LiteralPath $ConfigPath).Path
$config = Get-Content -LiteralPath $configFullPath -Raw | ConvertFrom-Json

if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $OutputDirectory = Join-Path $repositoryPath 'artifacts\baseline\build-input-origins'
}
$OutputDirectory = [IO.Path]::GetFullPath($OutputDirectory)
New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null

$requiredInputs = @($config.build.requiredInputs)
if ($requiredInputs.Count -eq 0) {
    throw 'build.requiredInputs is empty. Configure the compile-time inputs that must be traced.'
}

$configuredAdditionalRoots = @(
    Get-OptionalProperty -Object $config.build -Name 'provenanceAdditionalSearchRoots' -Default @() |
        ForEach-Object { [string]$_ } |
        Where-Object { -not [string]::IsNullOrWhiteSpace($_) }
)
$allRequestedSearchRoots = @($configuredAdditionalRoots + $AdditionalSearchRoot)
$resolvedSearchRoots = New-Object Collections.ArrayList
$searchRootWarnings = New-Object Collections.ArrayList
$searchRootIndex = @{}

foreach ($requestedRoot in $allRequestedSearchRoots) {
    $candidateRoot = $requestedRoot
    if (-not [IO.Path]::IsPathRooted($candidateRoot)) {
        $candidateRoot = Join-Path $repositoryPath $candidateRoot
    }

    try {
        $resolvedRoot = (Resolve-Path -LiteralPath $candidateRoot -ErrorAction Stop).Path
        if (-not (Test-Path -LiteralPath $resolvedRoot -PathType Container)) {
            [void]$searchRootWarnings.Add("Search root is not a directory: $candidateRoot")
            continue
        }

        $key = $resolvedRoot.ToLowerInvariant()
        if (-not $searchRootIndex.ContainsKey($key)) {
            $label = 'external-root-' + ($resolvedSearchRoots.Count + 1)
            $row = [pscustomobject]@{
                Label = $label
                Path = $resolvedRoot
                UnderRepository = (Test-IsUnderPath -Base $repositoryPath -Path $resolvedRoot)
            }
            $searchRootIndex[$key] = $row
            [void]$resolvedSearchRoots.Add($row)
        }
    }
    catch {
        [void]$searchRootWarnings.Add("Search root could not be resolved: $candidateRoot ($($_.Exception.Message))")
    }
}

$gitAvailable = [bool]((Get-Command git -ErrorAction SilentlyContinue) -and (Test-Path -LiteralPath (Join-Path $repositoryPath '.git')))
$candidateIndex = @{}
$expectedPathRows = New-Object Collections.ArrayList
$referenceRows = New-Object Collections.ArrayList
$legacyRows = New-Object Collections.ArrayList
$warnings = New-Object Collections.ArrayList
foreach ($warning in $searchRootWarnings) { [void]$warnings.Add($warning) }

$sourceRoot = Resolve-FromBase -Base $repositoryPath -Path ([string]$config.sourceRoot)
$configStaticScan = Get-OptionalProperty -Object $config -Name 'staticScan' -Default $null
$configExcludedDirectories = @(
    Get-OptionalProperty -Object $configStaticScan -Name 'excludeDirectories' -Default @() |
        ForEach-Object { [string]$_ } |
        Where-Object { -not [string]::IsNullOrWhiteSpace($_) }
)
$excludedDirectoryNames = @(
    @('.git', '.vs', 'Debug', 'Release', 'x64', 'x86', 'ipch', 'Output', 'artifacts', 'node_modules') +
    $configExcludedDirectories |
        Sort-Object -Unique
)
$escapedExcludedDirectoryNames = @($excludedDirectoryNames | ForEach-Object { [regex]::Escape($_) })
$excludedPattern = '(?i)[\\/](?:' + ($escapedExcludedDirectoryNames -join '|') + ')[\\/]'
$textExtensions = @(
    '.c', '.cc', '.cpp', '.cxx', '.h', '.hh', '.hpp', '.inl', '.rc',
    '.vcxproj', '.vcproj', '.dsp', '.sln', '.props', '.targets',
    '.bat', '.cmd', '.ps1', '.ini', '.cfg', '.txt', '.xml', '.json', '.lua'
)

$textFiles = @(
    Get-ChildItem -LiteralPath $repositoryPath -Recurse -File -Force -ErrorAction SilentlyContinue |
        Where-Object {
            $_.FullName -notmatch $excludedPattern -and
            $textExtensions -contains $_.Extension.ToLowerInvariant()
        }
)

foreach ($input in $requiredInputs) {
    $inputName = [string]$input.name
    if ([string]::IsNullOrWhiteSpace($inputName)) { continue }

    $configuredCandidatePaths = @($input.candidatePaths | ForEach-Object { [string]$_ } | Where-Object { $_ })
    foreach ($configuredPath in $configuredCandidatePaths) {
        $fullExpectedPath = Resolve-FromBase -Base $repositoryPath -Path $configuredPath
        $relativeExpectedPath = Get-RelativePath -Base $repositoryPath -Path $fullExpectedPath
        $gitEvidence = Get-GitPathEvidence -Repository $repositoryPath -RelativePath $relativeExpectedPath -GitAvailable $gitAvailable
        $exists = Test-Path -LiteralPath $fullExpectedPath -PathType Leaf

        [void]$expectedPathRows.Add([pscustomobject]@{
            RequiredInput = $inputName
            ConfiguredPath = $configuredPath
            RepositoryRelativePath = $relativeExpectedPath
            Exists = [bool]$exists
            GitTracked = [bool]$gitEvidence.tracked
            GitIgnored = [bool]$gitEvidence.ignored
            GitIgnoreSource = [string]$gitEvidence.ignoreSource
            GitIgnoreLine = $gitEvidence.ignoreLine
            GitIgnorePattern = [string]$gitEvidence.ignorePattern
            GitHistoryCommitCount = [int]$gitEvidence.historyCommitCount
            GitHistoryCommits = @($gitEvidence.historyCommits)
        })

        if ($exists) {
            Add-Candidate `
                -CandidateIndex $candidateIndex `
                -RequiredInput $inputName `
                -FullPath $fullExpectedPath `
                -DiscoverySource 'configured-candidate-path' `
                -RootLabel 'repository' `
                -RootPath $repositoryPath `
                -Repository $repositoryPath `
                -GitAvailable $gitAvailable `
                -ExposeAbsoluteExternalPath ([bool]$IncludeAbsoluteExternalPaths) `
                -OmitHash ([bool]$SkipFileHash)
        }
    }

    $repositoryMatches = @(
        Get-ChildItem -LiteralPath $repositoryPath -Recurse -File -Filter $inputName -Force -ErrorAction SilentlyContinue |
            Where-Object { $_.FullName -notmatch $excludedPattern } |
            Select-Object -First $MaxCandidatesPerInput
    )
    foreach ($match in $repositoryMatches) {
        Add-Candidate `
            -CandidateIndex $candidateIndex `
            -RequiredInput $inputName `
            -FullPath $match.FullName `
            -DiscoverySource 'repository-recursive-name-match' `
            -RootLabel 'repository' `
            -RootPath $repositoryPath `
            -Repository $repositoryPath `
            -GitAvailable $gitAvailable `
            -ExposeAbsoluteExternalPath ([bool]$IncludeAbsoluteExternalPaths) `
            -OmitHash ([bool]$SkipFileHash)
    }

    foreach ($root in $resolvedSearchRoots) {
        $matches = @(
            Get-ChildItem -LiteralPath $root.Path -Recurse -File -Filter $inputName -Force -ErrorAction SilentlyContinue |
                Select-Object -First $MaxCandidatesPerInput
        )
        foreach ($match in $matches) {
            Add-Candidate `
                -CandidateIndex $candidateIndex `
                -RequiredInput $inputName `
                -FullPath $match.FullName `
                -DiscoverySource 'additional-search-root-name-match' `
                -RootLabel $root.Label `
                -RootPath $root.Path `
                -Repository $repositoryPath `
                -GitAvailable $gitAvailable `
                -ExposeAbsoluteExternalPath ([bool]$IncludeAbsoluteExternalPaths) `
                -OmitHash ([bool]$SkipFileHash)
        }
    }

    $escapedName = [regex]::Escape($inputName)
    foreach ($file in $textFiles) {
        $lineNumber = 0
        try {
            foreach ($line in Get-Content -LiteralPath $file.FullName -ErrorAction Stop) {
                $lineNumber++
                if ($line -notmatch $escapedName) { continue }

                $trimmed = $line.Trim()
                $referenceType = 'text-reference'
                if ($trimmed -match '^\s*#\s*include\s*[<"](?<include>[^>"]+)[>"]') {
                    $referenceType = 'source-include'
                }
                elseif ($file.Extension.ToLowerInvariant() -in @('.vcxproj', '.vcproj', '.dsp', '.sln', '.props', '.targets', '.bat', '.cmd', '.ps1')) {
                    $referenceType = 'build-or-script-reference'
                }

                if ($trimmed.Length -gt 500) {
                    $trimmed = $trimmed.Substring(0, 500) + '...'
                }

                [void]$referenceRows.Add([pscustomobject]@{
                    RequiredInput = $inputName
                    ReferenceType = $referenceType
                    File = Get-RelativePath -Base $repositoryPath -Path $file.FullName
                    Line = $lineNumber
                    Text = $trimmed
                })
            }
        }
        catch {
            [void]$warnings.Add("Could not scan reference file $($file.FullName): $($_.Exception.Message)")
        }
    }
}

if (-not $SkipLegacyPreprocessedReferences) {
    $componentDirectories = New-Object Collections.Generic.HashSet[string]([StringComparer]::OrdinalIgnoreCase)
    foreach ($reference in @($referenceRows | Where-Object ReferenceType -eq 'source-include')) {
        $fullReferencePath = Join-Path $repositoryPath $reference.File
        if (Test-IsUnderPath -Base $sourceRoot -Path $fullReferencePath) {
            $relativeToSource = Get-RelativePath -Base $sourceRoot -Path $fullReferencePath
            $firstSegment = ($relativeToSource -split '[\\/]')[0]
            $componentPath = Join-Path $sourceRoot $firstSegment
            if (Test-Path -LiteralPath $componentPath -PathType Container) {
                [void]$componentDirectories.Add($componentPath)
            }
        }
    }

    foreach ($componentDirectory in $componentDirectories) {
        $preprocessedFiles = @(
            Get-ChildItem -LiteralPath $componentDirectory -Recurse -File -Filter *.i -ErrorAction SilentlyContinue |
                Where-Object { $_.Length -le 50MB }
        )

        foreach ($preprocessedFile in $preprocessedFiles) {
            $lineNumber = 0
            try {
                foreach ($line in Get-Content -LiteralPath $preprocessedFile.FullName -ErrorAction Stop) {
                    $lineNumber++
                    if ($line -notmatch '^\s*#line\s+\d+\s+"(?<path>[^"]+)"') { continue }
                    $historicalPath = $Matches['path']

                    foreach ($input in $requiredInputs) {
                        $inputName = [string]$input.name
                        if ($historicalPath -notmatch ('(?i)(^|[\\/])' + [regex]::Escape($inputName) + '$')) { continue }

                        [void]$legacyRows.Add([pscustomobject]@{
                            RequiredInput = $inputName
                            EvidenceType = 'legacy-preprocessed-line-marker'
                            PreprocessedFile = Get-RelativePath -Base $repositoryPath -Path $preprocessedFile.FullName
                            PreprocessedFileSHA = $(if ($SkipFileHash) { '' } else { (Get-FileHash -LiteralPath $preprocessedFile.FullName -Algorithm SHA256).Hash })
                            Line = $lineNumber
                            HistoricalIncludePath = $historicalPath
                            Interpretation = 'Historical path evidence only. A preprocessed .i file is not an authoritative source for reconstructing the current header.'
                        })
                    }
                }
            }
            catch {
                [void]$warnings.Add("Could not scan preprocessed file $($preprocessedFile.FullName): $($_.Exception.Message)")
            }
        }
    }
}

$candidates = @($candidateIndex.Values | Sort-Object RequiredInput, DisplayPath)
$inputSummaries = New-Object Collections.ArrayList
foreach ($input in $requiredInputs) {
    $inputName = [string]$input.name
    if ([string]::IsNullOrWhiteSpace($inputName)) { continue }

    $inputCandidates = @($candidates | Where-Object RequiredInput -ieq $inputName)
    $repoCandidates = @($inputCandidates | Where-Object UnderRepository)
    $externalCandidates = @($inputCandidates | Where-Object { -not $_.UnderRepository })
    $trackedCandidates = @($inputCandidates | Where-Object GitTracked)
    $ignoredCandidates = @($inputCandidates | Where-Object GitIgnored)
    $directReferences = @($referenceRows | Where-Object RequiredInput -ieq $inputName)
    $legacyReferences = @($legacyRows | Where-Object RequiredInput -ieq $inputName)
    $expectedPaths = @($expectedPathRows | Where-Object RequiredInput -ieq $inputName)

    $status = if ($trackedCandidates.Count -gt 0) {
        'tracked-repository-candidate'
    }
    elseif ($repoCandidates.Count -gt 0) {
        'untracked-repository-candidate'
    }
    elseif ($externalCandidates.Count -gt 0) {
        'external-candidate-found'
    }
    else {
        'not-found'
    }

    [void]$inputSummaries.Add([pscustomobject]@{
        RequiredInput = $inputName
        Status = $status
        CandidateCount = $inputCandidates.Count
        RepositoryCandidateCount = $repoCandidates.Count
        ExternalCandidateCount = $externalCandidates.Count
        TrackedCandidateCount = $trackedCandidates.Count
        IgnoredCandidateCount = $ignoredCandidates.Count
        ConfiguredExpectedPathCount = $expectedPaths.Count
        IgnoredExpectedPathCount = @($expectedPaths | Where-Object GitIgnored).Count
        HistoricalPathCommitCount = [int](($expectedPaths | Measure-Object GitHistoryCommitCount -Sum).Sum)
        DirectReferenceCount = $directReferences.Count
        LegacyPreprocessedReferenceCount = $legacyReferences.Count
        RequiresManualProvenanceDecision = [bool]($status -ne 'tracked-repository-candidate')
    })
}

$summary = [ordered]@{
    schemaVersion = 1
    generatedAtUtc = [DateTime]::UtcNow.ToString('o')
    repositoryRoot = $repositoryPath
    configPath = $configFullPath
    security = [ordered]@{
        contentCopied = $false
        absoluteExternalPathsIncluded = [bool]$IncludeAbsoluteExternalPaths
        note = 'This audit records metadata and hashes only. It never copies, modifies or commits candidate headers.'
    }
    search = [ordered]@{
        repositoryScanned = $true
        additionalSearchRootCount = $resolvedSearchRoots.Count
        additionalSearchRoots = @($resolvedSearchRoots | ForEach-Object {
            [ordered]@{
                label = $_.Label
                path = $(if ($IncludeAbsoluteExternalPaths) { $_.Path } else { '<redacted>' })
                underRepository = $_.UnderRepository
            }
        })
        maxCandidatesPerInputPerRoot = $MaxCandidatesPerInput
        legacyPreprocessedReferencesScanned = [bool](-not $SkipLegacyPreprocessedReferences)
    }
    totals = [ordered]@{
        requiredInputs = $inputSummaries.Count
        foundInputs = @($inputSummaries | Where-Object Status -ne 'not-found').Count
        unresolvedInputs = @($inputSummaries | Where-Object Status -eq 'not-found').Count
        candidateFiles = $candidates.Count
        expectedPaths = $expectedPathRows.Count
        directReferences = $referenceRows.Count
        legacyPreprocessedReferences = $legacyRows.Count
        warnings = $warnings.Count
    }
    inputs = @($inputSummaries)
    candidates = @($candidates)
    expectedPaths = @($expectedPathRows)
    references = @($referenceRows)
    legacyPreprocessedReferences = @($legacyRows)
    warnings = @($warnings)
}

$jsonPath = Join-Path $OutputDirectory 'build-input-origin-audit.json'
$candidatesCsvPath = Join-Path $OutputDirectory 'build-input-candidates.csv'
$expectedCsvPath = Join-Path $OutputDirectory 'build-input-expected-paths.csv'
$referencesCsvPath = Join-Path $OutputDirectory 'build-input-references.csv'
$legacyCsvPath = Join-Path $OutputDirectory 'legacy-preprocessed-references.csv'
$markdownPath = Join-Path $OutputDirectory 'BUILD-INPUT-ORIGINS.md'

$summary | ConvertTo-Json -Depth 14 | Set-Content -LiteralPath $jsonPath -Encoding UTF8
@($candidates) |
    Select-Object RequiredInput, DisplayPath, AbsolutePath, UnderRepository, RepositoryRelativePath, RootLabel,
        @{n='DiscoverySources';e={$_.DiscoverySources -join ';'}}, SizeBytes, LastWriteTimeUtc, SHA256,
        GitTracked, GitIgnored, GitIgnoreSource, GitIgnoreLine, GitIgnorePattern, GitHistoryCommitCount,
        @{n='GitHistoryCommits';e={$_.GitHistoryCommits -join ';'}} |
    Export-Csv -LiteralPath $candidatesCsvPath -NoTypeInformation -Encoding UTF8
@($expectedPathRows) |
    Select-Object RequiredInput, ConfiguredPath, RepositoryRelativePath, Exists, GitTracked, GitIgnored,
        GitIgnoreSource, GitIgnoreLine, GitIgnorePattern, GitHistoryCommitCount,
        @{n='GitHistoryCommits';e={$_.GitHistoryCommits -join ';'}} |
    Export-Csv -LiteralPath $expectedCsvPath -NoTypeInformation -Encoding UTF8
@($referenceRows) | Export-Csv -LiteralPath $referencesCsvPath -NoTypeInformation -Encoding UTF8
@($legacyRows) | Export-Csv -LiteralPath $legacyCsvPath -NoTypeInformation -Encoding UTF8

$markdown = New-Object Text.StringBuilder
[void]$markdown.AppendLine('# Build-input provenance')
[void]$markdown.AppendLine('')
[void]$markdown.AppendLine('> Metadata only: this audit does not copy, modify or commit candidate files. External absolute paths are redacted unless explicitly requested.')
[void]$markdown.AppendLine('')
[void]$markdown.AppendLine('| Required input | Status | Candidates | Tracked | Ignored | Direct refs | Legacy `.i` refs |')
[void]$markdown.AppendLine('|---|---|---:|---:|---:|---:|---:|')
foreach ($row in $inputSummaries) {
    [void]$markdown.AppendLine("| ``$($row.RequiredInput)`` | $($row.Status) | $($row.CandidateCount) | $($row.TrackedCandidateCount) | $($row.IgnoredCandidateCount) | $($row.DirectReferenceCount) | $($row.LegacyPreprocessedReferenceCount) |")
}

if ($candidates.Count -gt 0) {
    [void]$markdown.AppendLine('')
    [void]$markdown.AppendLine('## Candidate files')
    [void]$markdown.AppendLine('')
    [void]$markdown.AppendLine('| Required input | Path | SHA-256 | Tracked | Ignored | Discovery |')
    [void]$markdown.AppendLine('|---|---|---|---:|---:|---|')
    foreach ($candidate in $candidates) {
        $shaDisplay = if ([string]::IsNullOrWhiteSpace($candidate.SHA256)) { '' } else { $candidate.SHA256 }
        [void]$markdown.AppendLine("| ``$($candidate.RequiredInput)`` | $(Convert-ToMarkdownCell $candidate.DisplayPath) | ``$shaDisplay`` | $($candidate.GitTracked) | $($candidate.GitIgnored) | $(Convert-ToMarkdownCell ($candidate.DiscoverySources -join ', ')) |")
    }
}

$ignoredExpectedPaths = @($expectedPathRows | Where-Object GitIgnored)
if ($ignoredExpectedPaths.Count -gt 0) {
    [void]$markdown.AppendLine('')
    [void]$markdown.AppendLine('## Expected paths hidden by Git ignore rules')
    [void]$markdown.AppendLine('')
    [void]$markdown.AppendLine('| Required input | Expected path | Rule source | Pattern |')
    [void]$markdown.AppendLine('|---|---|---|---|')
    foreach ($pathRow in $ignoredExpectedPaths) {
        [void]$markdown.AppendLine("| ``$($pathRow.RequiredInput)`` | $(Convert-ToMarkdownCell $pathRow.RepositoryRelativePath) | $(Convert-ToMarkdownCell $pathRow.GitIgnoreSource):$($pathRow.GitIgnoreLine) | ``$(Convert-ToMarkdownCell $pathRow.GitIgnorePattern)`` |")
    }
}

if ($legacyRows.Count -gt 0) {
    [void]$markdown.AppendLine('')
    [void]$markdown.AppendLine('## Legacy preprocessed evidence')
    [void]$markdown.AppendLine('')
    [void]$markdown.AppendLine('| Required input | `.i` file | Historical include path | Interpretation |')
    [void]$markdown.AppendLine('|---|---|---|---|')
    foreach ($legacy in $legacyRows) {
        [void]$markdown.AppendLine("| ``$($legacy.RequiredInput)`` | $(Convert-ToMarkdownCell $legacy.PreprocessedFile) | $(Convert-ToMarkdownCell $legacy.HistoricalIncludePath) | Historical path evidence only; do not reconstruct headers blindly. |")
    }
}

[void]$markdown.AppendLine('')
[void]$markdown.AppendLine('## Acceptance criteria before using a candidate')
[void]$markdown.AppendLine('')
[void]$markdown.AppendLine('1. Record origin, generator/tool version and SHA-256.')
[void]$markdown.AppendLine('2. Verify compatibility with the exact Titanium commit and `Release|x64` configuration.')
[void]$markdown.AppendLine('3. Review for secrets, licensed assets and machine-specific paths before versioning or sharing.')
[void]$markdown.AppendLine('4. Re-run the clean build and functional smoke tests; a filename match alone is insufficient.')
[void]$markdown.AppendLine('5. Never synthesize empty placeholder headers merely to advance the compiler.')
$markdown.ToString() | Set-Content -LiteralPath $markdownPath -Encoding UTF8

[pscustomobject]@{
    OutputDirectory = $OutputDirectory
    SummaryJson = $jsonPath
    CandidatesCsv = $candidatesCsvPath
    ExpectedPathsCsv = $expectedCsvPath
    ReferencesCsv = $referencesCsvPath
    LegacyReferencesCsv = $legacyCsvPath
    Markdown = $markdownPath
    RequiredInputCount = $inputSummaries.Count
    FoundInputCount = @($inputSummaries | Where-Object Status -ne 'not-found').Count
    UnresolvedInputCount = @($inputSummaries | Where-Object Status -eq 'not-found').Count
    CandidateCount = $candidates.Count
}
