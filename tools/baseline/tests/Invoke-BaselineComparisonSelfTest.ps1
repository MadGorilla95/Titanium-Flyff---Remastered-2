[CmdletBinding()]
param()

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw "ASSERTION FAILED: $Message" }
}

$toolRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$fixtureRoot = Join-Path ([IO.Path]::GetTempPath()) ("titanium-phase0-compare-" + [Guid]::NewGuid().ToString("N"))
$referenceRun = Join-Path $fixtureRoot "reference"
$candidateRun = Join-Path $fixtureRoot "candidate"

try {
    New-Item -ItemType Directory -Path (Join-Path $referenceRun "log-timings") -Force | Out-Null
    New-Item -ItemType Directory -Path (Join-Path $candidateRun "log-timings") -Force | Out-Null

    $environment = [ordered]@{
        computerName = "PHASE0-TEST"
        osBuild = "26100"
        logicalProcessorCount = 8
        processors = @("Fixture CPU")
        configSha256 = "SAME-CONFIG"
    }
    $environment | ConvertTo-Json -Depth 4 | Set-Content (Join-Path $referenceRun "environment.json") -Encoding UTF8
    $environment | ConvertTo-Json -Depth 4 | Set-Content (Join-Path $candidateRun "environment.json") -Encoding UTF8

    [ordered]@{
        timings = @(
            [ordered]@{
                Timing = "login"
                Count = 5
                P50Milliseconds = 100
                P95Milliseconds = 120
                MaximumMilliseconds = 130
            }
        )
    } | ConvertTo-Json -Depth 6 | Set-Content (Join-Path $referenceRun "log-timings\timing-summary.json") -Encoding UTF8

    [ordered]@{
        timings = @(
            [ordered]@{
                Timing = "login"
                Count = 5
                P50Milliseconds = 130
                P95Milliseconds = 180
                MaximumMilliseconds = 190
            }
        )
    } | ConvertTo-Json -Depth 6 | Set-Content (Join-Path $candidateRun "log-timings\timing-summary.json") -Encoding UTF8

    $comparison = & (Join-Path $toolRoot "Compare-BaselineRuns.ps1") `
        -ReferenceRun $referenceRun `
        -CandidateRun $candidateRun `
        -WarningPercent 5 `
        -RegressionPercent 15

    Assert-True ($comparison.OverallStatus -eq "regression") "Expected timing regression."
    Assert-True ($comparison.EnvironmentCompatible) "Fixture environments should be compatible."
    Assert-True ($comparison.RegressionCount -ge 1) "Expected at least one regression row."
    Assert-True (Test-Path (Join-Path $comparison.OutputDirectory "baseline-comparison.json")) "Comparison JSON was not created."

    Write-Host "Phase 0 baseline comparison self-test passed."
}
finally {
    if (Test-Path $fixtureRoot) {
        Remove-Item $fixtureRoot -Recurse -Force -ErrorAction SilentlyContinue
    }
}
