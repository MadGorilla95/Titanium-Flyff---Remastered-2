[CmdletBinding()]
param()

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw "ASSERTION FAILED: $Message" }
}

$toolRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$fixtureRoot = Join-Path ([IO.Path]::GetTempPath()) ("titanium-phase0-selftest-" + [Guid]::NewGuid().ToString('N'))
$inventoryOutput = Join-Path $fixtureRoot 'artifacts\inventory'
$policyOutput = Join-Path $fixtureRoot 'artifacts\build-policy'
$hygieneOutput = Join-Path $fixtureRoot 'artifacts\hygiene'
$topologyOutput = Join-Path $fixtureRoot 'artifacts\topology'
$buildChainOutput = Join-Path $fixtureRoot 'artifacts\build-chain'

try {
    New-Item -ItemType Directory -Path (Join-Path $fixtureRoot 'Source\Source\AccountServer') -Force | Out-Null
    New-Item -ItemType Directory -Path (Join-Path $fixtureRoot 'Source\Output\AccountServer\Release64') -Force | Out-Null
    New-Item -ItemType Directory -Path (Join-Path $fixtureRoot 'ImmortalProgram') -Force | Out-Null

    @'
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" ToolsVersion="15.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Release|x64"><Configuration>Release</Configuration><Platform>x64</Platform></ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <ProjectGuid>{11111111-1111-1111-1111-111111111111}</ProjectGuid>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType><PlatformToolset>v143</PlatformToolset>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <TargetName>AccountServer</TargetName><TargetExt>.exe</TargetExt><OutDir>..\..\Output\AccountServer\Release64\</OutDir>
  </PropertyGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <ClCompile><WarningLevel>TurnOffAllWarnings</WarningLevel><TreatWarningAsError>false</TreatWarningAsError></ClCompile>
    <Link><OutputFile>..\..\Output\AccountServer\Release64\AccountServer.exe</OutputFile></Link>
  </ItemDefinitionGroup>
</Project>
'@ | Set-Content (Join-Path $fixtureRoot 'Source\Source\AccountServer\AccountServer.vcxproj') -Encoding UTF8

    '<VisualStudioProject Name="AccountServer" />' |
        Set-Content (Join-Path $fixtureRoot 'Source\Source\AccountServer\AccountServer.vcproj') -Encoding UTF8

    @'
Microsoft Visual Studio Solution File, Format Version 8.00
Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "AccountServer", "AccountServer.vcproj", "{11111111-1111-1111-1111-111111111111}"
EndProject
Global
    GlobalSection(SolutionConfiguration) = preSolution
        Debug = Debug
        Release = Release
    EndGlobalSection
    GlobalSection(ProjectConfiguration) = postSolution
        {11111111-1111-1111-1111-111111111111}.Release.ActiveCfg = Release|Win32
        {11111111-1111-1111-1111-111111111111}.Release.Build.0 = Release|Win32
    EndGlobalSection
EndGlobal
'@ | Set-Content (Join-Path $fixtureRoot 'Source\Source\AccountServer\AccountServer71.sln') -Encoding UTF8

    'int main() { return 0; }' | Set-Content (Join-Path $fixtureRoot 'Source\Source\AccountServer\AccountServer.cpp') -Encoding UTF8
    'preprocessed fixture' | Set-Content (Join-Path $fixtureRoot 'Source\Source\AccountServer\AccountServer.i') -Encoding UTF8
    New-Item -ItemType File -Path (Join-Path $fixtureRoot 'Source\Output\AccountServer\Release64\AccountServer.exe') -Force | Out-Null
    New-Item -ItemType File -Path (Join-Path $fixtureRoot 'ImmortalProgram\1. Account-x64.exe') -Force | Out-Null

    @'
@echo off
COPY "Source\Output\AccountServer\Release64\AccountServer.exe" "Program\1. Account-x64.exe"
'@ | Set-Content (Join-Path $fixtureRoot '-_Move_Compiles.bat') -Encoding ASCII

    @'
@echo off
start /min "" "1. Account-x64.exe"
timeout 1 > nul
start /min "" "2. Certifier-x64.exe"
timeout /t 2 > nul
start /min "" "7. World-x64.exe"
timeout 1 > nul
pause > nul
taskkill /im "7. World-x64.exe" > nul
timeout 1 > nul
'@ | Set-Content (Join-Path $fixtureRoot 'ImmortalProgram\Server Starter64.bat') -Encoding ASCII

    & (Join-Path $toolRoot 'Get-ProjectInventory.ps1') -RepositoryRoot $fixtureRoot -SourceRoot 'Source\Source' -OutputDirectory $inventoryOutput | Out-Null
    & (Join-Path $toolRoot 'Get-BuildPolicyAudit.ps1') -RepositoryRoot $fixtureRoot -SourceRoot 'Source\Source' -OutputDirectory $policyOutput | Out-Null
    & (Join-Path $toolRoot 'Get-RepositoryHygiene.ps1') -RepositoryRoot $fixtureRoot -SourceRoot 'Source\Source' -OutputDirectory $hygieneOutput | Out-Null
    & (Join-Path $toolRoot 'Get-StartupTopology.ps1') -RepositoryRoot $fixtureRoot -OutputDirectory $topologyOutput | Out-Null
    & (Join-Path $toolRoot 'Get-BuildChain.ps1') -RepositoryRoot $fixtureRoot -SourceRoot 'Source\Source' -Configuration 'Release' -Platform 'x64' -OutputDirectory $buildChainOutput | Out-Null

    $inventory = Get-Content (Join-Path $inventoryOutput 'project-inventory.json') -Raw | ConvertFrom-Json
    $policy = Get-Content (Join-Path $policyOutput 'project-build-policies.json') -Raw | ConvertFrom-Json
    $hygiene = Get-Content (Join-Path $hygieneOutput 'repository-hygiene.json') -Raw | ConvertFrom-Json
    $topology = Get-Content (Join-Path $topologyOutput 'startup-topology.json') -Raw | ConvertFrom-Json
    $buildChain = Get-Content (Join-Path $buildChainOutput 'build-chain.json') -Raw | ConvertFrom-Json

    Assert-True ($inventory.totals.projects -eq 1) 'Expected one project.'
    Assert-True ($inventory.totals.executableProjects -eq 1) 'Expected one executable project.'
    Assert-True ($policy.totals.highRiskPolicies -ge 1) 'Warnings-disabled policy was not classified as high risk.'
    Assert-True (@($hygiene.hygiene | Where-Object category -eq 'generated-compiler-artifact-in-source').Count -ge 1) 'Generated .i artifact was not detected.'
    Assert-True (@($hygiene.components | Where-Object component -eq 'AccountServer').Count -eq 1) 'AccountServer component was not inventoried.'
    Assert-True ($topology.totals.launchersWithTopology -eq 1) 'Expected one startup topology.'
    Assert-True ($topology.totals.startCommands -eq 3) 'Expected three start commands.'

    $launcher = @($topology.launchers)[0]
    Assert-True ([double]$launcher.DelayBeforeLastStartSeconds -eq 3) 'Expected three seconds before the last process start.'
    Assert-True ([double]$launcher.StartupScriptedDelaySeconds -eq 4) 'Expected four seconds total scripted startup delay.'
    Assert-True ([bool]$launcher.UsesOnlyFixedStartupDelay) 'Fixed-delay-only risk was not detected.'
    Assert-True ($launcher.ReadinessCheckCount -eq 0) 'Unexpected readiness check detected.'

    Assert-True ($buildChain.totals.modernProjects -eq 1) 'Expected one modern project in build-chain audit.'
    Assert-True ($buildChain.totals.packagingCopies -eq 1) 'Expected one packaging copy.'
    Assert-True ($buildChain.totals.mappedDeploymentCopies -eq 1) 'Deployment copy was not mapped to the modern project.'
    Assert-True ($buildChain.totals.strongCanonicalCandidates -eq 0) 'Legacy Win32 solution must not be accepted as canonical x64 build.'
    Assert-True (@($buildChain.deployments | Where-Object MatchConfidence -eq 'exact-output-path').Count -eq 1) 'Expected exact deployment-to-project output match.'
    Assert-True (@($buildChain.solutionCoverage | Where-Object CanonicalStatus -eq 'coverage-only-not-valid-for-requested-modern-build').Count -eq 1) 'Legacy solution risk was not identified.'

    Write-Host 'Phase 0 baseline tool self-test passed.'
}
finally {
    if (Test-Path $fixtureRoot) { Remove-Item $fixtureRoot -Recurse -Force -ErrorAction SilentlyContinue }
}
