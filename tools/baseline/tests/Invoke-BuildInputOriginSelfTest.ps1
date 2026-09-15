[CmdletBinding()]
param()

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw "ASSERTION FAILED: $Message" }
}

$toolRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$fixtureRoot = Join-Path ([IO.Path]::GetTempPath()) ("titanium-phase0-origin-repo-" + [Guid]::NewGuid().ToString('N'))
$externalRoot = Join-Path ([IO.Path]::GetTempPath()) ("titanium-phase0-origin-external-" + [Guid]::NewGuid().ToString('N'))
$outputRoot = Join-Path $fixtureRoot 'artifacts\origin'
$configPath = Join-Path $fixtureRoot 'origin.config.json'

try {
    New-Item -ItemType Directory -Path (Join-Path $fixtureRoot 'Source\Source\AccountServer') -Force | Out-Null
    New-Item -ItemType Directory -Path (Join-Path $fixtureRoot 'Source\Resource') -Force | Out-Null
    New-Item -ItemType Directory -Path $externalRoot -Force | Out-Null

    '/Source/Resource/' | Set-Content (Join-Path $fixtureRoot '.gitignore') -Encoding ASCII
    '#define LANG_FIXTURE 1' | Set-Content (Join-Path $fixtureRoot 'Source\Resource\Lang.h') -Encoding ASCII
    '#define DEFINE_FIXTURE 1' | Set-Content (Join-Path $externalRoot 'Define.h') -Encoding ASCII

    @'
#pragma once
#include "../../Resource/Lang.h"
#include "Define.h"
'@ | Set-Content (Join-Path $fixtureRoot 'Source\Source\AccountServer\stdafx.h') -Encoding ASCII

    @'
#line 1 "..\..\Resource\Lang.h"
extern char g_szLanguage[13][16];
#line 10 "AccountServer.cpp"
'@ | Set-Content (Join-Path $fixtureRoot 'Source\Source\AccountServer\AccountServer.i') -Encoding ASCII

    @'
{
  "schemaVersion": 3,
  "repositoryRoot": ".",
  "sourceRoot": "Source/Source",
  "artifactRoot": "artifacts/baseline",
  "build": {
    "enabled": false,
    "solution": "",
    "configuration": "Release",
    "platform": "x64",
    "additionalArguments": [],
    "provenanceAdditionalSearchRoots": [],
    "provenanceMaxCandidatesPerInput": 20,
    "provenanceIncludeAbsoluteExternalPaths": false,
    "provenanceScanLegacyPreprocessedReferences": true,
    "requiredInputs": [
      {
        "name": "Lang.h",
        "candidatePaths": [
          "Source/Resource/Lang.h",
          "Source/resource/Lang.h"
        ]
      },
      {
        "name": "Define.h",
        "candidatePaths": [
          "Source/Resource/Define.h"
        ]
      }
    ]
  }
}
'@ | Set-Content $configPath -Encoding UTF8

    $git = Get-Command git -ErrorAction SilentlyContinue
    if ($null -ne $git) {
        & $git.Source -C $fixtureRoot init --quiet
        if ($LASTEXITCODE -ne 0) { throw 'Could not initialize Git fixture.' }
    }

    $result = & (Join-Path $toolRoot 'Resolve-BuildInputOrigins.ps1') `
        -RepositoryRoot $fixtureRoot `
        -ConfigPath $configPath `
        -AdditionalSearchRoot $externalRoot `
        -OutputDirectory $outputRoot

    Assert-True ($result.RequiredInputCount -eq 2) 'Expected two required inputs.'
    Assert-True ($result.FoundInputCount -eq 2) 'Expected both required inputs to have candidates.'
    Assert-True ($result.UnresolvedInputCount -eq 0) 'No required input should remain unresolved in the fixture.'
    Assert-True ($result.CandidateCount -eq 2) 'Expected one repository and one external candidate.'

    $summary = Get-Content $result.SummaryJson -Raw | ConvertFrom-Json
    $candidates = @(Import-Csv $result.CandidatesCsv)
    $expectedPaths = @(Import-Csv $result.ExpectedPathsCsv)
    $references = @(Import-Csv $result.ReferencesCsv)
    $legacy = @(Import-Csv $result.LegacyReferencesCsv)

    $langSummary = @($summary.inputs | Where-Object RequiredInput -eq 'Lang.h')[0]
    $defineSummary = @($summary.inputs | Where-Object RequiredInput -eq 'Define.h')[0]
    Assert-True ($langSummary.Status -eq 'untracked-repository-candidate') 'Lang.h should be an untracked repository candidate.'
    Assert-True ($defineSummary.Status -eq 'external-candidate-found') 'Define.h should be an external candidate.'

    $langCandidate = @($candidates | Where-Object RequiredInput -eq 'Lang.h')[0]
    $defineCandidate = @($candidates | Where-Object RequiredInput -eq 'Define.h')[0]
    Assert-True (-not [string]::IsNullOrWhiteSpace($langCandidate.SHA256)) 'Repository candidate hash is missing.'
    Assert-True (-not [string]::IsNullOrWhiteSpace($defineCandidate.SHA256)) 'External candidate hash is missing.'
    Assert-True ($defineCandidate.DisplayPath -like '<external-root-*>*') 'External candidate path was not redacted behind a root label.'
    Assert-True ([string]::IsNullOrWhiteSpace($defineCandidate.AbsolutePath)) 'Absolute external path must be omitted by default.'

    Assert-True (@($references | Where-Object { $_.RequiredInput -eq 'Lang.h' -and $_.ReferenceType -eq 'source-include' }).Count -ge 1) 'Lang.h include reference was not detected.'
    Assert-True (@($references | Where-Object { $_.RequiredInput -eq 'Define.h' -and $_.ReferenceType -eq 'source-include' }).Count -ge 1) 'Define.h include reference was not detected.'
    Assert-True (@($legacy | Where-Object { $_.RequiredInput -eq 'Lang.h' -and $_.EvidenceType -eq 'legacy-preprocessed-line-marker' }).Count -eq 1) 'Legacy Lang.h #line evidence was not detected.'

    if ($null -ne $git) {
        $langExpected = @($expectedPaths | Where-Object { $_.RequiredInput -eq 'Lang.h' -and $_.RepositoryRelativePath -match 'Source[\\/]Resource[\\/]Lang\.h' })[0]
        Assert-True ($langExpected.GitIgnored -eq 'True') 'Expected Lang.h path should be identified as ignored.'
        Assert-True ($langCandidate.GitIgnored -eq 'True') 'Existing ignored Lang.h candidate should be identified as ignored.'
    }

    Assert-True ((Test-Path $result.Markdown)) 'Markdown provenance report was not written.'
    Write-Host 'Build-input provenance self-test passed.'
}
finally {
    if (Test-Path $fixtureRoot) { Remove-Item $fixtureRoot -Recurse -Force -ErrorAction SilentlyContinue }
    if (Test-Path $externalRoot) { Remove-Item $externalRoot -Recurse -Force -ErrorAction SilentlyContinue }
}
