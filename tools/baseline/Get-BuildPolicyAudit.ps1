[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Join-Path $PSScriptRoot "..\.."),
    [string]$SourceRoot = "Source\Source",
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

function Get-NodeText($Parent, [string]$Name) {
    if ($null -eq $Parent) { return "" }
    $node = $Parent.SelectSingleNode("./*[local-name()='$Name']")
    if ($null -eq $node) { return "" }
    return $node.InnerText.Trim()
}

function Get-ComponentKind([string]$Path) {
    $normalized = $Path.Replace('/', '\')
    if ($normalized -match '(?i)(?:^|\\)(AccountServer|CacheServer|CACHESERVER|Certifier|CERTIFIER|CoreServer|CORESERVER|LoginServer|LOGINSERVER|WorldServer|WORLDSERVER|DatabaseServer)(?:\\|\.|$)') { return 'production-server' }
    if ($normalized -match '(?i)(?:^|\\)(Neuz|NeuzNoGG)(?:\\|\.|$)') { return 'game-client' }
    if ($normalized -match '(?i)(?:^|\\)(Launcher|PatchClient|Patcher)(?:\\|\.|$)') { return 'launcher-patcher' }
    if ($normalized -match '(?i)(?:^|\\)(_Common|_Database|_DirectX|_Network|_Sound|resource|Script)(?:\\|\.|$)') { return 'shared-internal' }
    if ($normalized -match '(?i)(?:^|\\)(CrashRpt|DX_SDK|DirectX|Discord|Lib|Lib64|MySQL|WAPI_AIL|crypto|json|libcrc|libcrux|lua|zlib)(?:\\|\.|$)') { return 'third-party-or-vendored' }
    return 'unclassified'
}

function Get-Risk([string]$WarningLevel, [string]$TreatAsError, [string]$DisabledWarnings) {
    $flags = New-Object System.Collections.ArrayList
    $severity = 'none'

    if ([string]::IsNullOrWhiteSpace($WarningLevel)) {
        [void]$flags.Add('warning-policy-unspecified')
        $severity = 'medium'
    }
    elseif ($WarningLevel -eq 'TurnOffAllWarnings') {
        [void]$flags.Add('warnings-disabled')
        $severity = 'high'
    }
    elseif ($WarningLevel -eq 'Level1') {
        [void]$flags.Add('warning-level-low')
        $severity = 'medium'
    }

    if ($TreatAsError -match '^(?i:false|0|no)$') {
        [void]$flags.Add('warnings-not-treated-as-errors')
        if ($severity -eq 'none') { $severity = 'low' }
    }

    $cleanDisabled = $DisabledWarnings -replace '%\(DisableSpecificWarnings\)', ''
    $cleanDisabled = $cleanDisabled.Trim([char[]]@(';',' '))
    if (-not [string]::IsNullOrWhiteSpace($cleanDisabled)) {
        [void]$flags.Add('specific-warnings-disabled')
        if ($severity -eq 'none') { $severity = 'low' }
    }

    return [pscustomobject]@{ Severity = $severity; Flags = @($flags) }
}

$repo = (Resolve-Path $RepositoryRoot).Path
$source = if ([IO.Path]::IsPathRooted($SourceRoot)) { $SourceRoot } else { Join-Path $repo $SourceRoot }
$source = (Resolve-Path $source).Path
if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $OutputDirectory = Join-Path $repo 'artifacts\baseline\build-policy'
}
$OutputDirectory = [IO.Path]::GetFullPath($OutputDirectory)
New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null

$excluded = '[\\/](?:\.git|\.vs|Debug|Release|x64|x86|ipch|Output|artifacts)[\\/]'
$projects = @(Get-ChildItem $source -Recurse -File -Filter *.vcxproj -ErrorAction SilentlyContinue |
    Where-Object { $_.FullName -notmatch $excluded } |
    Sort-Object FullName)

$rows = New-Object System.Collections.ArrayList
$parseErrors = New-Object System.Collections.ArrayList

foreach ($project in $projects) {
    try {
        [xml]$xml = Get-Content $project.FullName -Raw
        $projectPath = Get-RelativePath $repo $project.FullName
        $projectName = [IO.Path]::GetFileNameWithoutExtension($project.Name)
        $toolsets = @($xml.SelectNodes("//*[local-name()='PlatformToolset']") | ForEach-Object { $_.InnerText.Trim() } | Where-Object { $_ } | Sort-Object -Unique)
        $sdks = @($xml.SelectNodes("//*[local-name()='WindowsTargetPlatformVersion']") | ForEach-Object { $_.InnerText.Trim() } | Where-Object { $_ } | Sort-Object -Unique)

        $groups = @($xml.SelectNodes("//*[local-name()='ItemDefinitionGroup']"))
        if ($groups.Count -eq 0) {
            [void]$rows.Add([pscustomobject]@{
                Project=$projectName; ProjectPath=$projectPath; ComponentKind=(Get-ComponentKind $projectPath)
                ConfigurationPlatform='(no ItemDefinitionGroup)'; WarningLevel=''; TreatWarningAsError=''
                DisableSpecificWarnings=''; SDLCheck=''; EnablePREfast=''; MultiProcessorCompilation=''
                PlatformToolsets=($toolsets -join ';'); WindowsSDKs=($sdks -join ';')
                RiskSeverity='medium'; RiskFlags='warning-policy-unavailable'
            })
            continue
        }

        foreach ($group in $groups) {
            $compile = $group.SelectSingleNode("./*[local-name()='ClCompile']")
            if ($null -eq $compile) { continue }
            $condition = if ($group -is [System.Xml.XmlElement]) { $group.GetAttribute('Condition') } else { '' }
            $warningLevel = Get-NodeText $compile 'WarningLevel'
            $treatAsError = Get-NodeText $compile 'TreatWarningAsError'
            $disabledWarnings = Get-NodeText $compile 'DisableSpecificWarnings'
            $risk = Get-Risk $warningLevel $treatAsError $disabledWarnings

            [void]$rows.Add([pscustomobject]@{
                Project=$projectName; ProjectPath=$projectPath; ComponentKind=(Get-ComponentKind $projectPath)
                ConfigurationPlatform=$condition; WarningLevel=$warningLevel; TreatWarningAsError=$treatAsError
                DisableSpecificWarnings=$disabledWarnings; SDLCheck=(Get-NodeText $compile 'SDLCheck')
                EnablePREfast=(Get-NodeText $compile 'EnablePREfast')
                MultiProcessorCompilation=(Get-NodeText $compile 'MultiProcessorCompilation')
                PlatformToolsets=($toolsets -join ';'); WindowsSDKs=($sdks -join ';')
                RiskSeverity=$risk.Severity; RiskFlags=($risk.Flags -join ';')
            })
        }
    }
    catch {
        [void]$parseErrors.Add([pscustomobject]@{
            ProjectPath=(Get-RelativePath $repo $project.FullName)
            Error=$_.Exception.Message
        })
    }
}

$summaryRows = @($rows | Group-Object RiskSeverity | ForEach-Object {
    [pscustomobject]@{ Severity=$_.Name; Count=$_.Count }
} | Sort-Object Severity)

$result = [ordered]@{
    schemaVersion=1
    generatedAtUtc=[DateTime]::UtcNow.ToString('o')
    repositoryRoot=$repo
    sourceRoot=$source
    interpretation=[ordered]@{
        policyRiskIsCodeDefect=$false
        note='Disabled or weak warnings reduce audit visibility; they do not prove a runtime defect.'
    }
    totals=[ordered]@{
        projects=$projects.Count
        policyRows=$rows.Count
        highRiskPolicies=@($rows | Where-Object RiskSeverity -eq 'high').Count
        mediumRiskPolicies=@($rows | Where-Object RiskSeverity -eq 'medium').Count
        parseErrors=$parseErrors.Count
    }
    policies=@($rows)
    parseErrors=@($parseErrors)
}

$result | ConvertTo-Json -Depth 10 | Set-Content (Join-Path $OutputDirectory 'project-build-policies.json') -Encoding UTF8
@($rows) | Export-Csv (Join-Path $OutputDirectory 'project-build-policies.csv') -NoTypeInformation -Encoding UTF8
@($parseErrors) | Export-Csv (Join-Path $OutputDirectory 'project-build-policy-parse-errors.csv') -NoTypeInformation -Encoding UTF8

$md = New-Object Text.StringBuilder
[void]$md.AppendLine('# Project build-policy audit')
[void]$md.AppendLine('')
[void]$md.AppendLine('> Policy risk is an observability signal, not proof of a code defect or bottleneck.')
[void]$md.AppendLine('')
[void]$md.AppendLine("- Projects: $($projects.Count)")
[void]$md.AppendLine("- Policy rows: $($rows.Count)")
[void]$md.AppendLine("- High risk: $(@($rows | Where-Object RiskSeverity -eq 'high').Count)")
[void]$md.AppendLine("- Medium risk: $(@($rows | Where-Object RiskSeverity -eq 'medium').Count)")
[void]$md.AppendLine("- Parse errors: $($parseErrors.Count)")
[void]$md.AppendLine('')
[void]$md.AppendLine('| Project | Configuration | Warning level | Severity | Flags |')
[void]$md.AppendLine('|---|---|---|---|---|')
foreach ($row in $rows) {
    $configuration = ([string]$row.ConfigurationPlatform).Replace('|','\|')
    $flags = ([string]$row.RiskFlags).Replace('|','\|')
    [void]$md.AppendLine("| $($row.Project) | $configuration | $($row.WarningLevel) | $($row.RiskSeverity) | $flags |")
}
$md.ToString() | Set-Content (Join-Path $OutputDirectory 'project-build-policies.md') -Encoding UTF8

[pscustomobject]@{
    OutputDirectory=$OutputDirectory
    Projects=$projects.Count
    PolicyRows=$rows.Count
    HighRiskPolicies=@($rows | Where-Object RiskSeverity -eq 'high').Count
    MediumRiskPolicies=@($rows | Where-Object RiskSeverity -eq 'medium').Count
    ParseErrors=$parseErrors.Count
}
