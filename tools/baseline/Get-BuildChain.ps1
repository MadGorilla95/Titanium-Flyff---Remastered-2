[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Join-Path $PSScriptRoot "..\.."),
    [string]$SourceRoot = "Source\Source",
    [string]$Configuration = "Release",
    [string]$Platform = "x64",
    [string]$OutputDirectory = ""
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-RelativePath([string]$Base, [string]$Path) {
    $baseFull = [IO.Path]::GetFullPath($Base).TrimEnd('\','/') + [IO.Path]::DirectorySeparatorChar
    return [Uri]::UnescapeDataString(
        ([Uri]$baseFull).MakeRelativeUri([Uri][IO.Path]::GetFullPath($Path)).ToString()
    ).Replace('/', [IO.Path]::DirectorySeparatorChar)
}

function Resolve-RepositoryPath([string]$Repository, [string]$Base, [string]$Path) {
    if ([string]::IsNullOrWhiteSpace($Path)) { return $null }
    $clean = $Path.Trim().Trim('"')
    if ($clean -match '\$\(' -or $clean -match '%[^%]+%') { return $null }
    try {
        $full = if ([IO.Path]::IsPathRooted($clean)) {
            [IO.Path]::GetFullPath($clean)
        } else {
            [IO.Path]::GetFullPath((Join-Path $Base $clean))
        }
        $repoPrefix = [IO.Path]::GetFullPath($Repository).TrimEnd('\','/') + [IO.Path]::DirectorySeparatorChar
        if (-not $full.StartsWith($repoPrefix, [StringComparison]::OrdinalIgnoreCase)) { return $null }
        return Get-RelativePath $Repository $full
    } catch { return $null }
}

function Test-TargetCondition([string]$Condition, [string]$Configuration, [string]$Platform) {
    if ([string]::IsNullOrWhiteSpace($Condition)) { return $true }
    $conditionCompact = ($Condition -replace '\s','').Replace('"',"'")
    if ($conditionCompact -notmatch [regex]::Escape('$(Configuration)') -and
        $conditionCompact -notmatch [regex]::Escape('$(Platform)')) { return $false }

    $target = "$Configuration|$Platform"
    if ($conditionCompact -match [regex]::Escape('$(Configuration)|$(Platform)')) {
        return [bool]($conditionCompact -match ("==?'" + [regex]::Escape($target) + "'"))
    }

    $configurationMatches = $conditionCompact -notmatch [regex]::Escape('$(Configuration)')
    $platformMatches = $conditionCompact -notmatch [regex]::Escape('$(Platform)')
    if ($conditionCompact -match ([regex]::Escape('$(Configuration)') + "==?'" + [regex]::Escape($Configuration) + "'")) {
        $configurationMatches = $true
    }
    if ($conditionCompact -match ([regex]::Escape('$(Platform)') + "==?'" + [regex]::Escape($Platform) + "'")) {
        $platformMatches = $true
    }
    return [bool]($configurationMatches -and $platformMatches)
}

function Get-ProjectProperty([xml]$Xml, [string]$Name, [string]$Configuration, [string]$Platform) {
    $value = ""
    foreach ($group in @($Xml.SelectNodes("//*[local-name()='PropertyGroup']"))) {
        $groupCondition = if ($group -is [Xml.XmlElement]) { $group.GetAttribute('Condition') } else { "" }
        if (-not (Test-TargetCondition $groupCondition $Configuration $Platform)) { continue }
        foreach ($node in @($group.SelectNodes("./*[local-name()='$Name']"))) {
            $nodeCondition = if ($node -is [Xml.XmlElement]) { $node.GetAttribute('Condition') } else { "" }
            if (Test-TargetCondition $nodeCondition $Configuration $Platform) {
                $candidate = $node.InnerText.Trim()
                if ($candidate) { $value = $candidate }
            }
        }
    }
    return $value
}

function Get-LinkOutput([xml]$Xml, [string]$Configuration, [string]$Platform) {
    $value = ""
    foreach ($group in @($Xml.SelectNodes("//*[local-name()='ItemDefinitionGroup']"))) {
        $condition = if ($group -is [Xml.XmlElement]) { $group.GetAttribute('Condition') } else { "" }
        if (-not (Test-TargetCondition $condition $Configuration $Platform)) { continue }
        $node = $group.SelectSingleNode("./*[local-name()='Link']/*[local-name()='OutputFile']")
        if ($null -ne $node -and $node.InnerText.Trim()) { $value = $node.InnerText.Trim() }
    }
    return $value
}

function Expand-ProjectMacros(
    [string]$Value, [string]$ProjectDirectory, [string]$ProjectName,
    [string]$Configuration, [string]$Platform, [string]$OutDir,
    [string]$TargetName, [string]$TargetExt
) {
    $expanded = $Value
    $values = [ordered]@{
        '$(ProjectDir)' = $ProjectDirectory.TrimEnd('\','/') + [IO.Path]::DirectorySeparatorChar
        '$(ProjectName)' = $ProjectName
        '$(Configuration)' = $Configuration
        '$(Platform)' = $Platform
        '$(OutDir)' = $OutDir
        '$(TargetName)' = $TargetName
        '$(TargetExt)' = $TargetExt
    }
    foreach ($key in $values.Keys) { $expanded = $expanded.Replace($key, [string]$values[$key]) }
    return $expanded
}

function Get-ModernProject([IO.FileInfo]$File, [string]$Repository, [string]$Configuration, [string]$Platform) {
    $name = [IO.Path]::GetFileNameWithoutExtension($File.Name)
    try { [xml]$xml = Get-Content -LiteralPath $File.FullName -Raw }
    catch {
        return [pscustomobject]@{
            Project=$name; ProjectPath=(Get-RelativePath $Repository $File.FullName)
            ConfigurationExists=$false; ConfigurationType=""; PlatformToolset=""; WindowsSDK=""
            OutDirRaw=""; LinkOutputRaw=""; TargetName=$name; TargetExt=""; ResolvedOutputPath=$null
            OutputFileName=""; ParseError=$_.Exception.Message
        }
    }

    $includes = @($xml.SelectNodes("//*[local-name()='ProjectConfiguration']") | ForEach-Object {
        if ($_ -is [Xml.XmlElement]) { $_.GetAttribute('Include') }
    })
    $exists = @($includes | Where-Object {
        [string]::Equals($_, "$Configuration|$Platform", [StringComparison]::OrdinalIgnoreCase)
    }).Count -gt 0

    $type = Get-ProjectProperty $xml 'ConfigurationType' $Configuration $Platform
    $toolset = Get-ProjectProperty $xml 'PlatformToolset' $Configuration $Platform
    $sdk = Get-ProjectProperty $xml 'WindowsTargetPlatformVersion' $Configuration $Platform
    $outDir = Get-ProjectProperty $xml 'OutDir' $Configuration $Platform
    $targetName = Get-ProjectProperty $xml 'TargetName' $Configuration $Platform
    $targetExt = Get-ProjectProperty $xml 'TargetExt' $Configuration $Platform
    $linkOutput = Get-LinkOutput $xml $Configuration $Platform

    if (-not $targetName) { $targetName = $name }
    if (-not $targetExt) {
        $targetExt = switch ($type) {
            'Application' { '.exe' }
            'DynamicLibrary' { '.dll' }
            'StaticLibrary' { '.lib' }
            default { '' }
        }
    }

    $expandedOutDir = Expand-ProjectMacros $outDir $File.DirectoryName $name $Configuration $Platform $outDir $targetName $targetExt
    $output = $linkOutput
    if (-not $output -and $expandedOutDir) { $output = Join-Path $expandedOutDir ($targetName + $targetExt) }
    $output = Expand-ProjectMacros $output $File.DirectoryName $name $Configuration $Platform $expandedOutDir $targetName $targetExt
    $resolved = Resolve-RepositoryPath $Repository $File.DirectoryName $output

    return [pscustomobject]@{
        Project=$name; ProjectPath=(Get-RelativePath $Repository $File.FullName)
        ConfigurationExists=[bool]$exists; ConfigurationType=$type; PlatformToolset=$toolset; WindowsSDK=$sdk
        OutDirRaw=$outDir; LinkOutputRaw=$linkOutput; TargetName=$targetName; TargetExt=$targetExt
        ResolvedOutputPath=$resolved
        OutputFileName=if($resolved){[IO.Path]::GetFileName($resolved)}else{$targetName+$targetExt}
        ParseError=$null
    }
}

function Get-Solution([IO.FileInfo]$File, [string]$Repository, [string]$Configuration, [string]$Platform) {
    $lines = @(Get-Content -LiteralPath $File.FullName)
    $format = if($lines.Count -gt 0 -and $lines[0] -match 'Format Version\s+(?<v>[0-9.]+)'){$Matches['v']}else{""}
    $projects = New-Object Collections.ArrayList
    foreach($line in $lines){
        $match=[regex]::Match($line,'^\s*Project\("[^"]+"\)\s*=\s*"(?<name>[^"]+)",\s*"(?<path>[^"]+)",\s*"(?<guid>\{[^}]+\})"')
        if(-not $match.Success){continue}
        $raw=$match.Groups['path'].Value
        $resolved=Resolve-RepositoryPath $Repository $File.DirectoryName $raw
        $modernSibling=$null
        if([IO.Path]::GetExtension($raw) -ieq '.vcproj'){
            $candidate=[IO.Path]::ChangeExtension((Join-Path $File.DirectoryName $raw),'.vcxproj')
            if(Test-Path -LiteralPath $candidate -PathType Leaf){$modernSibling=Get-RelativePath $Repository $candidate}
        }
        [void]$projects.Add([pscustomobject]@{
            ProjectName=$match.Groups['name'].Value; ProjectGuid=$match.Groups['guid'].Value.ToUpperInvariant()
            ReferencedProjectPath=$resolved; ReferencedExtension=[IO.Path]::GetExtension($raw)
            ModernSiblingPath=$modernSibling; ActiveProjectConfiguration=""; BuildEnabled=$false
            MapsToRequestedConfiguration=$false
        })
    }

    $configurations=New-Object Collections.ArrayList
    $inside=$false
    foreach($line in $lines){
        if($line -match '^\s*GlobalSection\(SolutionConfiguration(?:Platforms)?\)'){$inside=$true;continue}
        if($inside -and $line -match '^\s*EndGlobalSection'){$inside=$false;continue}
        if($inside -and $line -match '^\s*(?<name>[^=]+?)\s*='){
            $value=$Matches['name'].Trim()
            if(-not $configurations.Contains($value)){[void]$configurations.Add($value)}
        }
    }
    $preferred="$Configuration|$Platform"
    if(-not $configurations.Contains($preferred) -and $configurations.Contains($Configuration)){$preferred=$Configuration}

    foreach($project in $projects){
        $guid=[regex]::Escape($project.ProjectGuid);$solutionConfig=[regex]::Escape($preferred)
        foreach($line in $lines){
            if($line -match "^\s*$guid\.$solutionConfig\.ActiveCfg\s*=\s*(?<value>.+?)\s*$"){
                $project.ActiveProjectConfiguration=$Matches['value'].Trim()
            }
            if($line -match "^\s*$guid\.$solutionConfig\.Build\.0\s*="){$project.BuildEnabled=$true}
        }
        $project.MapsToRequestedConfiguration=[string]::Equals(
            $project.ActiveProjectConfiguration,"$Configuration|$Platform",[StringComparison]::OrdinalIgnoreCase
        )
    }

    return [pscustomobject]@{
        Solution=[IO.Path]::GetFileNameWithoutExtension($File.Name)
        SolutionPath=Get-RelativePath $Repository $File.FullName; FormatVersion=$format
        Configurations=@($configurations); PreferredSolutionConfiguration=$preferred
        HasExactRequestedConfiguration=[bool]$configurations.Contains("$Configuration|$Platform")
        LegacyProjectReferenceCount=@($projects|Where-Object ReferencedExtension -ieq '.vcproj').Count
        ModernProjectReferenceCount=@($projects|Where-Object ReferencedExtension -ieq '.vcxproj').Count
        Projects=@($projects)
    }
}

function Get-PackagingCopy([string]$Repository) {
    $rows=New-Object Collections.ArrayList
    $files=@(Get-ChildItem -LiteralPath $Repository -Recurse -File | Where-Object {
        $_.Extension -in '.bat','.cmd' -and $_.FullName -notmatch '[\\/](?:\.git|\.vs|Debug|Release|x64|x86|Output|artifacts)[\\/]'
    })
    foreach($file in $files){
        $lineNumber=0
        foreach($line in @(Get-Content -LiteralPath $file.FullName -ErrorAction SilentlyContinue)){
            $lineNumber++
            if($line -notmatch '^\s*(?:copy|xcopy)\b' -or $line -notmatch '(?i)\.exe'){continue}
            $quoted=@([regex]::Matches($line,'"(.*?)"')|ForEach-Object{$_.Groups[1].Value})
            if($quoted.Count -lt 2){continue}
            $source=$quoted[$quoted.Count-2];$destination=$quoted[$quoted.Count-1]
            [void]$rows.Add([pscustomobject]@{
                ScriptPath=Get-RelativePath $Repository $file.FullName;Line=$lineNumber
                SourceRaw=$source;DestinationRaw=$destination
                SourcePath=Resolve-RepositoryPath $Repository $file.DirectoryName $source
                DestinationPath=Resolve-RepositoryPath $Repository $file.DirectoryName $destination
                SourceFileName=[IO.Path]::GetFileName($source);DestinationFileName=[IO.Path]::GetFileName($destination)
            })
        }
    }
    return @($rows)
}

function Escape-Markdown([object]$Value) {
    if($null -eq $Value){return ""}
    return ([string]$Value).Replace('|','\|').Replace("`r",' ').Replace("`n",' ')
}

$repo=(Resolve-Path -LiteralPath $RepositoryRoot).Path
$source=if([IO.Path]::IsPathRooted($SourceRoot)){(Resolve-Path $SourceRoot).Path}else{(Resolve-Path (Join-Path $repo $SourceRoot)).Path}
if(-not $OutputDirectory){$OutputDirectory=Join-Path $repo 'artifacts\baseline\build-chain'}
$OutputDirectory=[IO.Path]::GetFullPath($OutputDirectory)
New-Item -ItemType Directory -Path $OutputDirectory -Force|Out-Null
$excluded='[\\/](?:\.git|\.vs|Debug|Release|x64|x86|ipch|Output|artifacts)[\\/]'

$projects=@(Get-ChildItem $source -Recurse -File -Filter *.vcxproj|Where-Object{$_.FullName -notmatch $excluded}|Sort-Object FullName|ForEach-Object{
    Get-ModernProject $_ $repo $Configuration $Platform
})
$solutions=@(Get-ChildItem $repo -Recurse -File -Filter *.sln|Where-Object{$_.FullName -notmatch $excluded}|Sort-Object FullName|ForEach-Object{
    Get-Solution $_ $repo $Configuration $Platform
})
$copies=@(Get-PackagingCopy $repo)

$deployments=New-Object Collections.ArrayList
foreach($copy in $copies){
    $exact=@($projects|Where-Object{$_.ResolvedOutputPath -and $copy.SourcePath -and [string]::Equals($_.ResolvedOutputPath,$copy.SourcePath,[StringComparison]::OrdinalIgnoreCase)})
    $fileMatches=@($projects|Where-Object{$_.OutputFileName -and [string]::Equals($_.OutputFileName,$copy.SourceFileName,[StringComparison]::OrdinalIgnoreCase)})
    $project=$null;$confidence='unmatched';$ambiguous=$false
    if($exact.Count -eq 1){$project=$exact[0];$confidence='exact-output-path'}
    elseif($exact.Count -gt 1){$project=$exact[0];$confidence='ambiguous-exact-output-path';$ambiguous=$true}
    elseif($fileMatches.Count -eq 1){$project=$fileMatches[0];$confidence='unique-output-filename'}
    elseif($fileMatches.Count -gt 1){$project=$fileMatches[0];$confidence='ambiguous-output-filename';$ambiguous=$true}

    $solutionMatches=New-Object Collections.ArrayList
    if($null -ne $project){
        foreach($solution in $solutions){foreach($reference in $solution.Projects){
            $direct=$reference.ReferencedProjectPath -and [string]::Equals($reference.ReferencedProjectPath,$project.ProjectPath,[StringComparison]::OrdinalIgnoreCase)
            $sibling=$reference.ModernSiblingPath -and [string]::Equals($reference.ModernSiblingPath,$project.ProjectPath,[StringComparison]::OrdinalIgnoreCase)
            if($direct -or $sibling){[void]$solutionMatches.Add([pscustomobject]@{
                SolutionPath=$solution.SolutionPath;MatchKind=if($direct){'direct-project-reference'}else{'legacy-project-modern-sibling'}
                HasExactRequestedConfiguration=$solution.HasExactRequestedConfiguration
                ActiveProjectConfiguration=$reference.ActiveProjectConfiguration;BuildEnabled=$reference.BuildEnabled
                MapsToRequestedConfiguration=$reference.MapsToRequestedConfiguration
            })}
        }}
    }

    [void]$deployments.Add([pscustomobject]@{
        ScriptPath=$copy.ScriptPath;Line=$copy.Line;SourcePath=$copy.SourcePath;DestinationPath=$copy.DestinationPath
        SourceFileName=$copy.SourceFileName;DestinationFileName=$copy.DestinationFileName
        Project=if($project){$project.Project}else{""};ProjectPath=if($project){$project.ProjectPath}else{""}
        ProjectConfigurationExists=if($project){$project.ConfigurationExists}else{$false}
        ProjectOutputPath=if($project){$project.ResolvedOutputPath}else{""}
        MatchConfidence=$confidence;Ambiguous=$ambiguous;SolutionMatches=@($solutionMatches)
        RecommendedBuildCommand=if($project){'msbuild "'+$project.ProjectPath+'" /t:Build /m /p:Configuration='+$Configuration+' /p:Platform='+$Platform}else{""}
    })
}

$mapped=@($deployments|Where-Object ProjectPath)
$uniqueMapped=@($mapped|Select-Object -ExpandProperty ProjectPath -Unique)
$coverage=New-Object Collections.ArrayList
foreach($solution in $solutions){
    $covered=New-Object Collections.ArrayList;$kinds=New-Object Collections.ArrayList
    foreach($projectPath in $uniqueMapped){foreach($reference in $solution.Projects){
        $direct=$reference.ReferencedProjectPath -and [string]::Equals($reference.ReferencedProjectPath,$projectPath,[StringComparison]::OrdinalIgnoreCase)
        $sibling=$reference.ModernSiblingPath -and [string]::Equals($reference.ModernSiblingPath,$projectPath,[StringComparison]::OrdinalIgnoreCase)
        if($direct -or $sibling){
            if(-not $covered.Contains($projectPath)){[void]$covered.Add($projectPath)}
            $kind=if($direct){'direct'}else{'legacy-sibling'};if(-not $kinds.Contains($kind)){[void]$kinds.Add($kind)}
        }
    }}
    $all=[bool]($uniqueMapped.Count -gt 0 -and $covered.Count -eq $uniqueMapped.Count)
    $status=if($all -and $solution.HasExactRequestedConfiguration -and $solution.LegacyProjectReferenceCount -eq 0){
        'strong-candidate-not-runtime-confirmed'
    }elseif($all){
        'coverage-only-not-valid-for-requested-modern-build'
    }elseif($covered.Count -gt 0){
        'partial-component-solution'
    }else{
        'unrelated-to-deployment-copy-chain'
    }
    [void]$coverage.Add([pscustomobject]@{
        Solution=$solution.Solution;SolutionPath=$solution.SolutionPath;FormatVersion=$solution.FormatVersion
        HasExactRequestedConfiguration=$solution.HasExactRequestedConfiguration
        PreferredSolutionConfiguration=$solution.PreferredSolutionConfiguration
        LegacyProjectReferenceCount=$solution.LegacyProjectReferenceCount;ModernProjectReferenceCount=$solution.ModernProjectReferenceCount
        CoveredDeploymentProjectCount=$covered.Count;CoveredDeploymentProjects=@($covered);CoverageKinds=@($kinds)
        CoversAllMappedDeployments=$all;CanonicalStatus=$status
    })
}

$strong=@($coverage|Where-Object CanonicalStatus -eq 'strong-candidate-not-runtime-confirmed')
$full=@($coverage|Where-Object CoversAllMappedDeployments)
$unmatched=@($deployments|Where-Object MatchConfidence -eq 'unmatched')
$ambiguous=@($deployments|Where-Object Ambiguous)
$conclusion=if($copies.Count -eq 0){
    'No executable packaging copies were found; the canonical packaging chain is not statically identifiable.'
}elseif($strong.Count -eq 1){
    "One solution covers all mapped deployment projects with exact $Configuration|$Platform support. It remains a static candidate until a clean build and runtime hash confirm it."
}elseif($strong.Count -gt 1){
    'Multiple modern solutions cover the deployment chain; the canonical entry point remains ambiguous.'
}elseif($full.Count -gt 0){
    "A solution covers the deployment projects but uses legacy references and/or lacks exact $Configuration|$Platform mapping; it is not a valid canonical x64 build entry point."
}elseif($uniqueMapped.Count -gt 0){
    'No single solution covers the deployment chain; static evidence points to separate component builds or an external/uncommitted build entry point.'
}else{
    'Deployment outputs could not be mapped uniquely to modern projects; the canonical build route is unproven.'
}

$result=[ordered]@{
    schemaVersion=1;generatedAtUtc=[DateTime]::UtcNow.ToString('o');repositoryRoot=$repo;sourceRoot=$source
    requestedConfiguration=$Configuration;requestedPlatform=$Platform
    interpretation=@{staticBuildChainIsRuntimeProof=$false;solutionPresenceIsCanonicalProof=$false;note='A clean build, packaging output and matching runtime SHA-256 are required for proof.'}
    totals=@{
        modernProjects=$projects.Count;solutions=$solutions.Count;packagingCopies=$copies.Count;mappedDeploymentCopies=$mapped.Count
        uniqueMappedDeploymentProjects=$uniqueMapped.Count;unmatchedDeploymentCopies=$unmatched.Count;ambiguousDeploymentCopies=$ambiguous.Count
        projectsWithoutRequestedConfiguration=@($projects|Where-Object{-not $_.ConfigurationExists}).Count
        unresolvedProjectOutputs=@($projects|Where-Object{$_.ConfigurationExists -and -not $_.ResolvedOutputPath}).Count
        fullCoverageSolutions=$full.Count;strongCanonicalCandidates=$strong.Count
    }
    canonicalConclusion=$conclusion;projects=@($projects);solutions=@($solutions);packagingCopies=@($copies)
    deployments=@($deployments);solutionCoverage=@($coverage)
}

$result|ConvertTo-Json -Depth 16|Set-Content (Join-Path $OutputDirectory 'build-chain.json') -Encoding UTF8
@($projects)|Export-Csv (Join-Path $OutputDirectory 'project-outputs.csv') -NoTypeInformation -Encoding UTF8
@($deployments)|Select-Object ScriptPath,Line,SourcePath,DestinationPath,Project,ProjectPath,ProjectConfigurationExists,ProjectOutputPath,MatchConfidence,Ambiguous,@{n='SolutionPaths';e={@($_.SolutionMatches|ForEach-Object SolutionPath)-join';'}},RecommendedBuildCommand|Export-Csv (Join-Path $OutputDirectory 'deployment-map.csv') -NoTypeInformation -Encoding UTF8
@($coverage)|Select-Object Solution,SolutionPath,FormatVersion,HasExactRequestedConfiguration,PreferredSolutionConfiguration,LegacyProjectReferenceCount,ModernProjectReferenceCount,CoveredDeploymentProjectCount,@{n='CoveredDeploymentProjects';e={$_.CoveredDeploymentProjects-join';'}},@{n='CoverageKinds';e={$_.CoverageKinds-join';'}},CoversAllMappedDeployments,CanonicalStatus|Export-Csv (Join-Path $OutputDirectory 'solution-coverage.csv') -NoTypeInformation -Encoding UTF8

$md=New-Object Text.StringBuilder
[void]$md.AppendLine('# Build-chain audit');[void]$md.AppendLine('')
[void]$md.AppendLine('> Static mapping only. A clean build, packaging output and matching runtime hash are required for proof.')
[void]$md.AppendLine('');[void]$md.AppendLine("- Requested configuration: ``$Configuration|$Platform``")
[void]$md.AppendLine("- Projects: $($projects.Count); solutions: $($solutions.Count); packaging copies: $($copies.Count)")
[void]$md.AppendLine("- Mapped: $($mapped.Count); unmatched: $($unmatched.Count); ambiguous: $($ambiguous.Count)")
[void]$md.AppendLine('');[void]$md.AppendLine('## Conclusion');[void]$md.AppendLine('');[void]$md.AppendLine($conclusion)
[void]$md.AppendLine('');[void]$md.AppendLine('## Deployment mapping');[void]$md.AppendLine('')
[void]$md.AppendLine('| Destination | Source | Project | Match | Config exists |');[void]$md.AppendLine('|---|---|---|---|---:|')
foreach($row in $deployments){[void]$md.AppendLine("| $(Escape-Markdown $row.DestinationPath) | $(Escape-Markdown $row.SourcePath) | $(Escape-Markdown $row.ProjectPath) | $($row.MatchConfidence) | $($row.ProjectConfigurationExists) |")}
[void]$md.AppendLine('');[void]$md.AppendLine('## Solution coverage');[void]$md.AppendLine('')
[void]$md.AppendLine('| Solution | Format | Exact config | Legacy refs | Modern refs | Covered | Status |');[void]$md.AppendLine('|---|---|---:|---:|---:|---:|---|')
foreach($row in $coverage){[void]$md.AppendLine("| $(Escape-Markdown $row.SolutionPath) | $($row.FormatVersion) | $($row.HasExactRequestedConfiguration) | $($row.LegacyProjectReferenceCount) | $($row.ModernProjectReferenceCount) | $($row.CoveredDeploymentProjectCount) | $($row.CanonicalStatus) |")}
$md.ToString()|Set-Content (Join-Path $OutputDirectory 'build-chain.md') -Encoding UTF8

[pscustomobject]@{
    OutputDirectory=$OutputDirectory;ModernProjects=$projects.Count;Solutions=$solutions.Count;PackagingCopies=$copies.Count
    MappedDeploymentCopies=$mapped.Count;UnmatchedDeploymentCopies=$unmatched.Count;AmbiguousDeploymentCopies=$ambiguous.Count
    StrongCanonicalCandidates=$strong.Count;CanonicalConclusion=$conclusion
}
