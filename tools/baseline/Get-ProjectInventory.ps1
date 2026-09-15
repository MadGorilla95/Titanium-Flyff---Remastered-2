[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Join-Path $PSScriptRoot "..\.."),
    [string]$SourceRoot = "Source\Source",
    [string]$OutputDirectory = ""
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Relative([string]$Base,[string]$Path) {
    $b=[IO.Path]::GetFullPath($Base).TrimEnd('\','/')+[IO.Path]::DirectorySeparatorChar
    $u=[Uri]$b; $p=[Uri][IO.Path]::GetFullPath($Path)
    return [Uri]::UnescapeDataString($u.MakeRelativeUri($p).ToString()).Replace('/',[IO.Path]::DirectorySeparatorChar)
}
function Values($Xml,[string]$Name) {
    return @($Xml.SelectNodes("//*[local-name()='$Name']") | ForEach-Object {$_.InnerText.Trim()} | Where-Object {$_} | Sort-Object -Unique)
}
function Add-Sample([System.Collections.ArrayList]$List,[string]$Path) {
    if ($List.Count -lt 12 -and -not $List.Contains($Path)) {[void]$List.Add($Path)}
}

$repo=(Resolve-Path $RepositoryRoot).Path
$source=if([IO.Path]::IsPathRooted($SourceRoot)){$SourceRoot}else{Join-Path $repo $SourceRoot}
$source=(Resolve-Path $source).Path
if([string]::IsNullOrWhiteSpace($OutputDirectory)){$OutputDirectory=Join-Path $repo "artifacts\baseline\inventory"}
$OutputDirectory=[IO.Path]::GetFullPath($OutputDirectory)
New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null

$excluded='[\\/](?:\.git|\.vs|Debug|Release|x64|x86|ipch|artifacts)[\\/]'
$files=@(Get-ChildItem $repo -Recurse -File -Force | Where-Object {$_.FullName -notmatch $excluded} | ForEach-Object {
    [pscustomobject]@{File=$_; Path=Relative $repo $_.FullName}
})
$solutions=@($files | Where-Object {$_.File.Extension -ieq '.sln'} | Select-Object -ExpandProperty Path)

$projects=New-Object Collections.ArrayList
foreach($entry in @($files | Where-Object {$_.File.Extension -in '.vcxproj','.csproj'})) {
    $errorText=$null; $xml=$null
    try {[xml]$xml=Get-Content $entry.File.FullName -Raw} catch {$errorText=$_.Exception.Message}
    $types=@(); $targets=@(); $extensions=@(); $toolsets=@(); $sdks=@()
    if($xml){
        $types=@(Values $xml 'ConfigurationType')
        $targets=@(Values $xml 'TargetName')
        $extensions=@(Values $xml 'TargetExt')
        $toolsets=@(Values $xml 'PlatformToolset')
        $sdks=@(Values $xml 'WindowsTargetPlatformVersion')
    }
    $name=[IO.Path]::GetFileNameWithoutExtension($entry.File.Name)
    if($targets.Count -eq 0){$targets=@($name)}
    if($extensions.Count -eq 0 -and ($types -contains 'Application')){$extensions=@('.exe')}
    [void]$projects.Add([pscustomobject]@{
        Name=$name; Path=$entry.Path; ProjectType=$entry.File.Extension.TrimStart('.')
        ConfigurationTypes=$types; TargetNames=$targets; TargetExtensions=$extensions
        PlatformToolsets=$toolsets; WindowsTargetPlatformVersions=$sdks
        IsExecutableProject=[bool](($types -contains 'Application') -or ($extensions -contains '.exe'))
        ParseError=$errorText
    })
}

$launchers=@($files | Where-Object {$_.File.Extension -in '.bat','.cmd','.ps1','.ini','.cfg'} )
$launcherRefs=New-Object Collections.ArrayList
$exeRegex='(?i)(?<exe>[A-Za-z0-9_. -]+\.exe)'
foreach($entry in $launchers){
    try {
        $lineNo=0
        foreach($line in Get-Content $entry.File.FullName){
            $lineNo++
            foreach($m in [regex]::Matches($line,$exeRegex)){
                [void]$launcherRefs.Add([pscustomobject]@{Executable=$m.Groups['exe'].Value.Trim(); Script=$entry.Path; Line=$lineNo})
            }
        }
    } catch {}
}
$binaries=@($files | Where-Object {$_.File.Extension -ieq '.exe'} | ForEach-Object {[pscustomobject]@{Executable=$_.File.Name; Path=$_.Path}})

$candidateNames=New-Object Collections.Generic.HashSet[string]([StringComparer]::OrdinalIgnoreCase)
foreach($p in $projects | Where-Object IsExecutableProject){foreach($t in $p.TargetNames){[void]$candidateNames.Add("$t.exe")}}
foreach($r in $launcherRefs){[void]$candidateNames.Add($r.Executable)}
foreach($b in $binaries){[void]$candidateNames.Add($b.Executable)}

$processes=@(Get-Process -ErrorAction SilentlyContinue | ForEach-Object {
    $exe="$($_.ProcessName).exe"
    if($candidateNames.Contains($exe)){
        $path=$null; try{$path=$_.Path}catch{}
        [pscustomobject]@{Executable=$exe; ProcessId=$_.Id; Path=$path; StartTime=try{$_.StartTime.ToUniversalTime().ToString('o')}catch{$null}}
    }
})

$evidence=New-Object Collections.ArrayList
foreach($exe in @($candidateNames | Sort-Object)){
    $p=@($projects | Where-Object {($_.TargetNames | ForEach-Object {"$_.exe"}) -contains $exe} | Select-Object -ExpandProperty Path)
    $l=@($launcherRefs | Where-Object Executable -ieq $exe | ForEach-Object {"$($_.Script):$($_.Line)"})
    $b=@($binaries | Where-Object Executable -ieq $exe | Select-Object -ExpandProperty Path)
    $r=@($processes | Where-Object Executable -ieq $exe)
    $kinds=@(); if($p){$kinds+='project'};if($l){$kinds+='launcher'};if($b){$kinds+='binary'}
    $confidence=if($r){'runtime-confirmed'}elseif($l){'strong-static'}elseif($p -and $b){'strong-static'}elseif($p){'project-only'}else{'binary-only'}
    [void]$evidence.Add([pscustomobject]@{
        Executable=$exe; StaticEvidence=($kinds -join '+'); StaticConfidence=$confidence
        ProjectPaths=$p; LauncherReferences=$l; BinaryPaths=$b
        RuntimeConfirmed=[bool]$r; RuntimeInstances=@($r)
        Note=if($r){'Observed during this audit.'}else{'Static candidate; normal server startup must confirm usage.'}
    })
}

$features=@(
    @{Name='Guild systems';Pattern='(?i)\bguild\b'},@{Name='Housing';Pattern='(?i)housing|house[_ -]?system'},
    @{Name='Instance/Dungeon';Pattern='(?i)\binstance\b|\bdungeon\b'},@{Name='Pet systems';Pattern='(?i)\bpet\b'},
    @{Name='Achievement';Pattern='(?i)achievement'},@{Name='Ranking';Pattern='(?i)ranking|leaderboard'},
    @{Name='Cash shop';Pattern='(?i)cash.?shop|item.?mall'},@{Name='Anti-cheat';Pattern='(?i)anti.?cheat|hack.?shield'},
    @{Name='Discord';Pattern='(?i)discord'},@{Name='Launcher/Patcher';Pattern='(?i)launcher|patcher|patchclient'}
)
$textExt=@('.c','.cc','.cpp','.cxx','.h','.hh','.hpp','.inl','.cs','.rc','.ini','.txt','.xml','.json','.lua')
$featureRows=New-Object Collections.ArrayList
foreach($feature in $features){
    $samples=New-Object Collections.ArrayList; $pathCount=0; $contentCount=0
    foreach($entry in $files){
        if($entry.Path -match $feature.Pattern){$pathCount++;Add-Sample $samples $entry.Path}
        if($textExt -contains $entry.File.Extension.ToLowerInvariant() -and $entry.File.Length -le 5MB){
            try {if((Get-Content $entry.File.FullName -Raw) -match $feature.Pattern){$contentCount++;Add-Sample $samples $entry.Path}} catch {}
        }
    }
    [void]$featureRows.Add([pscustomobject]@{
        Feature=$feature.Name; SignalFound=[bool](($pathCount+$contentCount)-gt 0)
        PathMatches=$pathCount; ContentFileMatches=$contentCount; SamplePaths=@($samples)
        VerificationStatus='unverified-static-signal'
    })
}

$inventory=[ordered]@{
    generatedAtUtc=[DateTime]::UtcNow.ToString('o'); repositoryRoot=$repo
    totals=@{files=$files.Count;solutions=$solutions.Count;projects=$projects.Count;executableProjects=@($projects|Where-Object IsExecutableProject).Count;launchScripts=$launchers.Count;repositoryBinaries=$binaries.Count;executableCandidates=$evidence.Count;runtimeConfirmedExecutables=@($evidence|Where-Object RuntimeConfirmed).Count}
    solutions=$solutions; projects=@($projects); launcherReferences=@($launcherRefs)
    observedProcesses=$processes; executableEvidence=@($evidence); featureSignals=@($featureRows)
}
$inventory | ConvertTo-Json -Depth 12 | Set-Content (Join-Path $OutputDirectory 'project-inventory.json') -Encoding UTF8
@($projects) | Select-Object Name,Path,ProjectType,IsExecutableProject,@{n='ConfigurationTypes';e={$_.ConfigurationTypes-join';'}},@{n='TargetNames';e={$_.TargetNames-join';'}},@{n='TargetExtensions';e={$_.TargetExtensions-join';'}},@{n='PlatformToolsets';e={$_.PlatformToolsets-join';'}},@{n='SDKs';e={$_.WindowsTargetPlatformVersions-join';'}},ParseError | Export-Csv (Join-Path $OutputDirectory 'projects.csv') -NoTypeInformation -Encoding UTF8
@($evidence) | Select-Object Executable,StaticEvidence,StaticConfidence,@{n='ProjectPaths';e={$_.ProjectPaths-join';'}},@{n='LauncherReferences';e={$_.LauncherReferences-join';'}},@{n='BinaryPaths';e={$_.BinaryPaths-join';'}},RuntimeConfirmed,Note | Export-Csv (Join-Path $OutputDirectory 'executable-evidence.csv') -NoTypeInformation -Encoding UTF8
@($featureRows) | Select-Object Feature,SignalFound,PathMatches,ContentFileMatches,@{n='SamplePaths';e={$_.SamplePaths-join';'}},VerificationStatus | Export-Csv (Join-Path $OutputDirectory 'feature-signals.csv') -NoTypeInformation -Encoding UTF8

$md=New-Object Text.StringBuilder
[void]$md.AppendLine('# Project inventory');[void]$md.AppendLine('')
[void]$md.AppendLine('> Static evidence is not runtime proof. An executable is active only when it is observed during a normal server start.')
[void]$md.AppendLine('');[void]$md.AppendLine('| Executable | Static evidence | Confidence | Runtime observed |')
[void]$md.AppendLine('|---|---|---|---:|')
foreach($x in $evidence){[void]$md.AppendLine("| $($x.Executable) | $($x.StaticEvidence) | $($x.StaticConfidence) | $($x.RuntimeConfirmed) |")}
[void]$md.AppendLine('');[void]$md.AppendLine('| Feature | Signal | Path matches | Content-file matches | Status |')
[void]$md.AppendLine('|---|---:|---:|---:|---|')
foreach($x in $featureRows){[void]$md.AppendLine("| $($x.Feature) | $($x.SignalFound) | $($x.PathMatches) | $($x.ContentFileMatches) | $($x.VerificationStatus) |")}
$md.ToString() | Set-Content (Join-Path $OutputDirectory 'project-inventory.md') -Encoding UTF8

[pscustomobject]@{OutputDirectory=$OutputDirectory;Totals=$inventory.totals}
