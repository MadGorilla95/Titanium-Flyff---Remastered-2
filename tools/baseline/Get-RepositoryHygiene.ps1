[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Join-Path $PSScriptRoot "..\.."),
    [string]$SourceRoot = "Source\Source",
    [string]$OutputDirectory = "",
    [long]$LargeTranslationUnitBytes = 102400
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-RelativePath([string]$Base, [string]$Path) {
    $baseFull = [IO.Path]::GetFullPath($Base).TrimEnd('\','/') + [IO.Path]::DirectorySeparatorChar
    $baseUri = [Uri]$baseFull
    $pathUri = [Uri][IO.Path]::GetFullPath($Path)
    return [Uri]::UnescapeDataString($baseUri.MakeRelativeUri($pathUri).ToString()).Replace('/', [IO.Path]::DirectorySeparatorChar)
}

function Get-ComponentKind([string]$Path) {
    $normalized = $Path.Replace('/', '\')
    if ($normalized -match '(?i)(?:^|\\)(AccountServer|CacheServer|CACHESERVER|Certifier|CERTIFIER|CoreServer|CORESERVER|LoginServer|LOGINSERVER|WorldServer|WORLDSERVER|DatabaseServer)(?:\\|\.|$)') { return 'production-server' }
    if ($normalized -match '(?i)(?:^|\\)(Neuz|NeuzNoGG)(?:\\|\.|$)') { return 'game-client' }
    if ($normalized -match '(?i)(?:^|\\)(Launcher|PatchClient|Patcher)(?:\\|\.|$)') { return 'launcher-patcher' }
    if ($normalized -match '(?i)(?:^|\\)(Accumulator|WORLDDIALOG|WINDMESSENGER|atools)(?:\\|\.|$)') { return 'development-admin-tool' }
    if ($normalized -match '(?i)(?:^|\\)(_Common|_Database|_DirectX|_Network|_Sound|resource|Script)(?:\\|\.|$)') { return 'shared-internal' }
    if ($normalized -match '(?i)(?:^|\\)(CrashRpt|DX_SDK|DirectX|Discord|Lib|Lib64|MySQL|WAPI_AIL|crypto|json|libcrc|libcrux|lua|zlib)(?:\\|\.|$)') { return 'third-party-or-vendored' }
    if ($normalized -match '(?i)(?:^|\\)ImmortalProgram(?:\\|$)') { return 'deployment-runtime' }
    return 'unclassified'
}

function Get-HygieneCategory($File, [string]$RelativePath, [string]$SourcePrefix) {
    $extension = $File.Extension.ToLowerInvariant()
    $name = $File.Name.ToLowerInvariant()
    $isInSource = $RelativePath.StartsWith($SourcePrefix, [StringComparison]::OrdinalIgnoreCase)

    if ($extension -in @('.dsp','.dsw','.vcproj','.plg','.ncb','.opt','.aps')) { return 'legacy-project-or-ide-artifact' }
    if ($isInSource -and $extension -in @('.i','.obj','.pch','.pdb','.ilk','.idb','.tlog','.sbr','.bsc')) { return 'generated-compiler-artifact-in-source' }
    if ($name -match '(?i)(\.bak$|\.backup$|~$|\bcopy\b|\bold\b)' -or $extension -in @('.bak','.backup','.orig')) { return 'backup-copy' }
    if ($isInSource -and $extension -in @('.exe','.dll','.lib','.exp')) { return 'binary-or-library-in-source' }
    if ($isInSource -and $extension -in @('.zip','.rar','.7z','.tar','.gz')) { return 'archive-in-source' }
    if ($extension -in @('.suo','.user','.sdf','.opensdf')) { return 'user-or-ide-state' }
    return ''
}

$repo = (Resolve-Path $RepositoryRoot).Path
$source = if ([IO.Path]::IsPathRooted($SourceRoot)) { $SourceRoot } else { Join-Path $repo $SourceRoot }
$source = (Resolve-Path $source).Path
if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $OutputDirectory = Join-Path $repo 'artifacts\baseline\repository-hygiene'
}
$OutputDirectory = [IO.Path]::GetFullPath($OutputDirectory)
New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null

$sourcePrefix = Get-RelativePath $repo $source
$excluded = '[\\/](?:\.git|\.vs|Debug|Release|x64|x86|ipch|Output|artifacts|node_modules)[\\/]'
$files = @(Get-ChildItem $repo -Recurse -File -Force -ErrorAction SilentlyContinue |
    Where-Object { $_.FullName -notmatch $excluded } |
    Sort-Object FullName)

$hygiene = New-Object System.Collections.ArrayList
$largeUnits = New-Object System.Collections.ArrayList
$componentMap = @{}

foreach ($file in $files) {
    $relative = Get-RelativePath $repo $file.FullName
    $kind = Get-ComponentKind $relative
    $category = Get-HygieneCategory $file $relative $sourcePrefix

    if (-not [string]::IsNullOrWhiteSpace($category)) {
        [void]$hygiene.Add([pscustomobject]@{
            Category=$category; Path=$relative; Extension=$file.Extension
            SizeBytes=$file.Length; ComponentKind=$kind
            ReviewStatus='untriaged'
        })
    }

    if ($relative.StartsWith($sourcePrefix, [StringComparison]::OrdinalIgnoreCase)) {
        $underSource = $relative.Substring($sourcePrefix.Length).TrimStart('\','/')
        $component = if ($underSource.Contains('\')) { $underSource.Split('\')[0] } elseif ($underSource.Contains('/')) { $underSource.Split('/')[0] } else { '(root)' }
        if (-not $componentMap.ContainsKey($component)) {
            $componentMap[$component] = [ordered]@{
                Component=$component; ComponentKind=(Get-ComponentKind $component)
                FileCount=0; SourceFileCount=0; ProjectFileCount=0; BinaryArtifactCount=0; TotalBytes=[long]0
            }
        }
        $entry = $componentMap[$component]
        $entry.FileCount++
        $entry.TotalBytes += [long]$file.Length
        if ($file.Extension.ToLowerInvariant() -in @('.c','.cc','.cpp','.cxx','.h','.hh','.hpp','.hxx')) { $entry.SourceFileCount++ }
        if ($file.Extension.ToLowerInvariant() -in @('.sln','.vcxproj','.vcproj','.csproj','.dsp','.dsw')) { $entry.ProjectFileCount++ }
        if ($file.Extension.ToLowerInvariant() -in @('.exe','.dll','.lib','.exp')) { $entry.BinaryArtifactCount++ }
    }

    if ($file.Extension.ToLowerInvariant() -in @('.c','.cc','.cpp','.cxx') -and $file.Length -ge $LargeTranslationUnitBytes) {
        [void]$largeUnits.Add([pscustomobject]@{
            Path=$relative; SizeBytes=$file.Length; SizeKiB=[math]::Round($file.Length/1KB,1)
            ComponentKind=$kind; RuntimeImpactConfirmed=$false
            ReviewStatus='review-candidate'
        })
    }
}

$componentRows = @($componentMap.Values | ForEach-Object { [pscustomobject]$_ } | Sort-Object Component)
$hygieneSummary = @($hygiene | Group-Object Category | ForEach-Object {
    [pscustomobject]@{
        Category=$_.Name; Count=$_.Count
        TotalBytes=[long](($_.Group | Measure-Object SizeBytes -Sum).Sum)
        SamplePaths=@($_.Group | Select-Object -First 10 -ExpandProperty Path)
    }
} | Sort-Object Category)

$result = [ordered]@{
    schemaVersion=1
    generatedAtUtc=[DateTime]::UtcNow.ToString('o')
    repositoryRoot=$repo
    sourceRoot=$source
    interpretation=[ordered]@{
        hygieneSignalIsDeletionApproval=$false
        largeTranslationUnitIsRuntimeBottleneck=$false
        note='Review and prove ownership/build/runtime relevance before deleting or refactoring anything.'
    }
    totals=[ordered]@{
        filesScanned=$files.Count
        components=$componentRows.Count
        hygieneSignals=$hygiene.Count
        largeTranslationUnits=$largeUnits.Count
    }
    components=$componentRows
    hygiene=@($hygiene)
    hygieneSummary=$hygieneSummary
    largeTranslationUnits=@($largeUnits | Sort-Object SizeBytes -Descending)
}

$result | ConvertTo-Json -Depth 12 | Set-Content (Join-Path $OutputDirectory 'repository-hygiene.json') -Encoding UTF8
@($componentRows) | Export-Csv (Join-Path $OutputDirectory 'source-components.csv') -NoTypeInformation -Encoding UTF8
@($hygiene) | Export-Csv (Join-Path $OutputDirectory 'repository-hygiene.csv') -NoTypeInformation -Encoding UTF8
@($hygieneSummary) | Select-Object Category,Count,TotalBytes,@{n='SamplePaths';e={$_.SamplePaths -join ';'}} |
    Export-Csv (Join-Path $OutputDirectory 'repository-hygiene-summary.csv') -NoTypeInformation -Encoding UTF8
@($largeUnits | Sort-Object SizeBytes -Descending) |
    Export-Csv (Join-Path $OutputDirectory 'large-translation-units.csv') -NoTypeInformation -Encoding UTF8

$md = New-Object Text.StringBuilder
[void]$md.AppendLine('# Repository hygiene audit')
[void]$md.AppendLine('')
[void]$md.AppendLine('> Signals only. Nothing in this report is automatic deletion approval or proof of a runtime bottleneck.')
[void]$md.AppendLine('')
[void]$md.AppendLine("- Files scanned: $($files.Count)")
[void]$md.AppendLine("- Components: $($componentRows.Count)")
[void]$md.AppendLine("- Hygiene signals: $($hygiene.Count)")
[void]$md.AppendLine("- Large translation units: $($largeUnits.Count)")
[void]$md.AppendLine('')
[void]$md.AppendLine('## Hygiene categories')
[void]$md.AppendLine('')
[void]$md.AppendLine('| Category | Count | Total bytes |')
[void]$md.AppendLine('|---|---:|---:|')
foreach ($row in $hygieneSummary) { [void]$md.AppendLine("| $($row.Category) | $($row.Count) | $($row.TotalBytes) |") }
[void]$md.AppendLine('')
[void]$md.AppendLine('## Largest translation units')
[void]$md.AppendLine('')
[void]$md.AppendLine('| Path | KiB | Component | Runtime impact confirmed |')
[void]$md.AppendLine('|---|---:|---|---:|')
foreach ($row in @($largeUnits | Sort-Object SizeBytes -Descending | Select-Object -First 50)) {
    $path = ([string]$row.Path).Replace('|','\|')
    [void]$md.AppendLine("| $path | $($row.SizeKiB) | $($row.ComponentKind) | $($row.RuntimeImpactConfirmed) |")
}
$md.ToString() | Set-Content (Join-Path $OutputDirectory 'repository-hygiene.md') -Encoding UTF8

[pscustomobject]@{
    OutputDirectory=$OutputDirectory
    FilesScanned=$files.Count
    Components=$componentRows.Count
    HygieneSignals=$hygiene.Count
    LargeTranslationUnits=$largeUnits.Count
}
