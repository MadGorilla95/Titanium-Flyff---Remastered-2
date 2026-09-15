[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$ReferenceRun,
    [Parameter(Mandatory = $true)][string]$CandidateRun,
    [string]$OutputDirectory = "",
    [ValidateRange(0.0, 1000.0)][double]$WarningPercent = 5.0,
    [ValidateRange(0.0, 1000.0)][double]$RegressionPercent = 15.0,
    [switch]$AllowEnvironmentMismatch,
    [switch]$FailOnRegression
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Resolve-RunDirectory {
    param([Parameter(Mandatory = $true)][string]$Path)
    return (Resolve-Path -LiteralPath $Path -ErrorAction Stop).Path
}

function Read-JsonFile {
    param([Parameter(Mandatory = $true)][string]$Path)

    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) { return $null }
    return Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json
}

function Get-PropertyValue {
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

function Convert-ToNumber {
    param([object]$Value)

    if ($null -eq $Value -or [string]::IsNullOrWhiteSpace([string]$Value)) { return $null }
    try { return [double]$Value } catch { return $null }
}

function Convert-ToText {
    param([object]$Value)

    if ($null -eq $Value) { return "" }
    if ($Value -is [System.Array]) { return (@($Value) -join ";") }
    return [string]$Value
}

function Add-ComparisonRow {
    param(
        [System.Collections.ArrayList]$Rows,
        [Parameter(Mandatory = $true)][string]$Category,
        [Parameter(Mandatory = $true)][string]$Scope,
        [Parameter(Mandatory = $true)][string]$Metric,
        [object]$ReferenceValue,
        [object]$CandidateValue,
        [object]$Delta,
        [object]$DeltaPercent,
        [Parameter(Mandatory = $true)][string]$Status,
        [Parameter(Mandatory = $true)][string]$Reason
    )

    if ($null -eq $Rows) { throw "Rows collection is required." }

    [void]$Rows.Add([pscustomobject]@{
        Category = $Category
        Scope = $Scope
        Metric = $Metric
        Reference = $ReferenceValue
        Candidate = $CandidateValue
        Delta = $Delta
        DeltaPercent = $DeltaPercent
        Status = $Status
        Reason = $Reason
    })
}

function Add-LowerIsBetterComparison {
    param(
        [System.Collections.ArrayList]$Rows,
        [Parameter(Mandatory = $true)][string]$Category,
        [Parameter(Mandatory = $true)][string]$Scope,
        [Parameter(Mandatory = $true)][string]$Metric,
        [object]$ReferenceValue,
        [object]$CandidateValue,
        [double]$AbsoluteNoiseFloor,
        [double]$WarningThresholdPercent,
        [double]$RegressionThresholdPercent
    )

    $referenceNumber = Convert-ToNumber $ReferenceValue
    $candidateNumber = Convert-ToNumber $CandidateValue
    if ($null -eq $referenceNumber -or $null -eq $candidateNumber) {
        Add-ComparisonRow -Rows $Rows -Category $Category -Scope $Scope -Metric $Metric `
            -ReferenceValue $ReferenceValue -CandidateValue $CandidateValue `
            -Delta $null -DeltaPercent $null -Status "missing" `
            -Reason "Metric is absent from one or both runs."
        return
    }

    $delta = $candidateNumber - $referenceNumber
    $deltaPercent = $null
    if ([math]::Abs($referenceNumber) -gt 0.0000001) {
        $deltaPercent = ($delta / [math]::Abs($referenceNumber)) * 100.0
    }

    $status = "pass"
    $reason = "No material regression."

    if ($delta -lt (-1.0 * $AbsoluteNoiseFloor)) {
        $status = "improvement"
        $reason = "Candidate is materially lower."
    }
    elseif ($delta -le $AbsoluteNoiseFloor) {
        $status = "pass"
        $reason = "Change is inside the absolute noise floor."
    }
    elseif ($null -eq $deltaPercent) {
        $status = "warning"
        $reason = "Reference is zero; the candidate increased but percentage change is undefined."
    }
    elseif ($deltaPercent -ge $RegressionThresholdPercent) {
        $status = "regression"
        $reason = "Increase exceeds the regression threshold."
    }
    elseif ($deltaPercent -ge $WarningThresholdPercent) {
        $status = "warning"
        $reason = "Increase exceeds the warning threshold."
    }

    Add-ComparisonRow -Rows $Rows -Category $Category -Scope $Scope -Metric $Metric `
        -ReferenceValue ([math]::Round($referenceNumber, 6)) `
        -CandidateValue ([math]::Round($candidateNumber, 6)) `
        -Delta ([math]::Round($delta, 6)) `
        -DeltaPercent $(if ($null -eq $deltaPercent) { $null } else { [math]::Round($deltaPercent, 3) }) `
        -Status $status -Reason $reason
}

function Add-CountComparison {
    param(
        [System.Collections.ArrayList]$Rows,
        [Parameter(Mandatory = $true)][string]$Category,
        [Parameter(Mandatory = $true)][string]$Scope,
        [Parameter(Mandatory = $true)][string]$Metric,
        [object]$ReferenceValue,
        [object]$CandidateValue,
        [switch]$AnyIncreaseIsRegression
    )

    $referenceNumber = Convert-ToNumber $ReferenceValue
    $candidateNumber = Convert-ToNumber $CandidateValue
    if ($null -eq $referenceNumber -or $null -eq $candidateNumber) {
        Add-ComparisonRow -Rows $Rows -Category $Category -Scope $Scope -Metric $Metric `
            -ReferenceValue $ReferenceValue -CandidateValue $CandidateValue `
            -Delta $null -DeltaPercent $null -Status "missing" `
            -Reason "Count is absent from one or both runs."
        return
    }

    $delta = $candidateNumber - $referenceNumber
    $status = "pass"
    $reason = "Count did not increase."
    if ($delta -lt 0) {
        $status = "improvement"
        $reason = "Count decreased."
    }
    elseif ($delta -gt 0) {
        $status = if ($AnyIncreaseIsRegression) { "regression" } else { "warning" }
        $reason = if ($AnyIncreaseIsRegression) {
            "Any increase is a regression for this count."
        }
        else {
            "Count increased and requires review."
        }
    }

    Add-ComparisonRow -Rows $Rows -Category $Category -Scope $Scope -Metric $Metric `
        -ReferenceValue $referenceNumber -CandidateValue $candidateNumber `
        -Delta $delta -DeltaPercent $null -Status $status -Reason $reason
}

function New-RowIndex {
    param(
        [object[]]$Rows,
        [Parameter(Mandatory = $true)][string]$KeyProperty
    )

    $index = @{}
    foreach ($row in @($Rows)) {
        $key = [string](Get-PropertyValue -Object $row -Name $KeyProperty -Default "")
        if (-not [string]::IsNullOrWhiteSpace($key)) {
            $index[$key] = $row
        }
    }
    return $index
}

function Get-CombinedKeys {
    param([hashtable]$First, [hashtable]$Second)

    return @(
        @($First.Keys) + @($Second.Keys) |
            Sort-Object -Unique
    )
}

function Get-MaxMetric {
    param([object[]]$Rows, [Parameter(Mandatory = $true)][string]$Property)

    $values = @(
        foreach ($row in @($Rows)) {
            $value = Convert-ToNumber (Get-PropertyValue -Object $row -Name $Property)
            if ($null -ne $value) { $value }
        }
    )
    if ($values.Count -eq 0) { return $null }
    return ($values | Measure-Object -Maximum).Maximum
}

function Get-AggregatedProcessRows {
    param([object]$Summary)

    if ($null -eq $Summary) { return @() }
    $sourceRows = @(Get-PropertyValue -Object $Summary -Name "processes" -Default @())
    $result = New-Object System.Collections.ArrayList

    foreach ($group in @($sourceRows | Group-Object ProcessName)) {
        $sampleTotal = 0.0
        $weightedCpuTotal = 0.0
        $hasCpu = $false

        foreach ($row in $group.Group) {
            $samples = Convert-ToNumber (Get-PropertyValue -Object $row -Name "SampleCount" -Default 0)
            $averageCpu = Convert-ToNumber (Get-PropertyValue -Object $row -Name "AverageCpuPercent")
            if ($null -ne $averageCpu -and $samples -gt 0) {
                $sampleTotal += $samples
                $weightedCpuTotal += ($averageCpu * $samples)
                $hasCpu = $true
            }
        }

        [void]$result.Add([pscustomobject]@{
            ProcessName = $group.Name
            SampleCount = [int]$sampleTotal
            AverageCpuPercent = $(if ($hasCpu -and $sampleTotal -gt 0) { $weightedCpuTotal / $sampleTotal } else { $null })
            P95CpuPercent = Get-MaxMetric -Rows $group.Group -Property "P95CpuPercent"
            MaxPrivateMemoryMB = Get-MaxMetric -Rows $group.Group -Property "MaxPrivateMemoryMB"
            MaxWorkingSetMB = Get-MaxMetric -Rows $group.Group -Property "MaxWorkingSetMB"
            PrivateMemorySlopeMBPerMinute = Get-MaxMetric -Rows $group.Group -Property "PrivateMemorySlopeMBPerMinute"
            MaxHandleCount = Get-MaxMetric -Rows $group.Group -Property "MaxHandleCount"
            MaxThreadCount = Get-MaxMetric -Rows $group.Group -Property "MaxThreadCount"
        })
    }

    return @($result)
}

function Get-StatusRank {
    param([string]$Status)

    switch ($Status) {
        "regression" { return 0 }
        "warning" { return 1 }
        "missing" { return 2 }
        "improvement" { return 3 }
        "info" { return 4 }
        default { return 5 }
    }
}

$referencePath = Resolve-RunDirectory $ReferenceRun
$candidatePath = Resolve-RunDirectory $CandidateRun
if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $OutputDirectory = Join-Path $candidatePath ("comparison-to-" + (Split-Path $referencePath -Leaf))
}
$OutputDirectory = [System.IO.Path]::GetFullPath($OutputDirectory)
[void](New-Item -ItemType Directory -Path $OutputDirectory -Force)

$rows = New-Object System.Collections.ArrayList
$environmentMismatches = New-Object System.Collections.ArrayList

# Environment comparability
$referenceEnvironment = Read-JsonFile (Join-Path $referencePath "environment.json")
$candidateEnvironment = Read-JsonFile (Join-Path $candidatePath "environment.json")
if ($null -eq $referenceEnvironment -or $null -eq $candidateEnvironment) {
    [void]$environmentMismatches.Add("environment.json missing from one or both runs")
    Add-ComparisonRow -Rows $rows -Category "environment" -Scope "run" -Metric "environment.json" `
        -ReferenceValue ([bool]($null -ne $referenceEnvironment)) `
        -CandidateValue ([bool]($null -ne $candidateEnvironment)) `
        -Delta $null -DeltaPercent $null -Status "missing" `
        -Reason "A controlled comparison requires both environment fingerprints."
}
else {
    foreach ($field in @("computerName", "osBuild", "logicalProcessorCount")) {
        $referenceValue = Convert-ToText (Get-PropertyValue -Object $referenceEnvironment -Name $field)
        $candidateValue = Convert-ToText (Get-PropertyValue -Object $candidateEnvironment -Name $field)
        $matches = [string]::Equals($referenceValue, $candidateValue, [System.StringComparison]::OrdinalIgnoreCase)
        if (-not $matches) { [void]$environmentMismatches.Add("$field differs") }

        Add-ComparisonRow -Rows $rows -Category "environment" -Scope "run" -Metric $field `
            -ReferenceValue $referenceValue -CandidateValue $candidateValue `
            -Delta $null -DeltaPercent $null `
            -Status $(if ($matches) { "pass" } else { "warning" }) `
            -Reason $(if ($matches) { "Environment field matches." } else { "Environment field differs; comparability is reduced." })
    }

    $referenceProcessors = Convert-ToText (Get-PropertyValue -Object $referenceEnvironment -Name "processors")
    $candidateProcessors = Convert-ToText (Get-PropertyValue -Object $candidateEnvironment -Name "processors")
    $processorsMatch = [string]::Equals($referenceProcessors, $candidateProcessors, [System.StringComparison]::OrdinalIgnoreCase)
    if (-not $processorsMatch) { [void]$environmentMismatches.Add("processor model differs") }

    Add-ComparisonRow -Rows $rows -Category "environment" -Scope "run" -Metric "processors" `
        -ReferenceValue $referenceProcessors -CandidateValue $candidateProcessors `
        -Delta $null -DeltaPercent $null `
        -Status $(if ($processorsMatch) { "pass" } else { "warning" }) `
        -Reason $(if ($processorsMatch) { "Processor model matches." } else { "Processor model differs; performance values are not directly comparable." })

    $referenceConfig = Convert-ToText (Get-PropertyValue -Object $referenceEnvironment -Name "configSha256")
    $candidateConfig = Convert-ToText (Get-PropertyValue -Object $candidateEnvironment -Name "configSha256")
    $configMatches = [string]::Equals($referenceConfig, $candidateConfig, [System.StringComparison]::OrdinalIgnoreCase)

    Add-ComparisonRow -Rows $rows -Category "environment" -Scope "run" -Metric "configSha256" `
        -ReferenceValue $referenceConfig -CandidateValue $candidateConfig `
        -Delta $null -DeltaPercent $null `
        -Status $(if ($configMatches) { "pass" } else { "warning" }) `
        -Reason $(if ($configMatches) { "Effective baseline configuration matches." } else { "Effective baseline configuration changed; verify workload equivalence." })
}

# Build
$referenceBuild = Read-JsonFile (Join-Path $referencePath "build\build-summary.json")
$candidateBuild = Read-JsonFile (Join-Path $candidatePath "build\build-summary.json")
if ($null -ne $referenceBuild -and $null -ne $candidateBuild) {
    Add-LowerIsBetterComparison -Rows $rows -Category "build" -Scope "solution" -Metric "elapsedMilliseconds" `
        -ReferenceValue (Get-PropertyValue $referenceBuild "elapsedMilliseconds") `
        -CandidateValue (Get-PropertyValue $candidateBuild "elapsedMilliseconds") `
        -AbsoluteNoiseFloor 500 -WarningThresholdPercent 10 -RegressionThresholdPercent 25

    Add-CountComparison -Rows $rows -Category "build" -Scope "solution" -Metric "warningCount" `
        -ReferenceValue (Get-PropertyValue $referenceBuild "warningCount") `
        -CandidateValue (Get-PropertyValue $candidateBuild "warningCount")

    Add-CountComparison -Rows $rows -Category "build" -Scope "solution" -Metric "errorCount" `
        -ReferenceValue (Get-PropertyValue $referenceBuild "errorCount") `
        -CandidateValue (Get-PropertyValue $candidateBuild "errorCount") `
        -AnyIncreaseIsRegression

    $candidateSucceeded = [bool](Get-PropertyValue -Object $candidateBuild -Name "succeeded" -Default $false)
    Add-ComparisonRow -Rows $rows -Category "build" -Scope "solution" -Metric "succeeded" `
        -ReferenceValue (Get-PropertyValue $referenceBuild "succeeded") `
        -CandidateValue $candidateSucceeded -Delta $null -DeltaPercent $null `
        -Status $(if ($candidateSucceeded) { "pass" } else { "regression" }) `
        -Reason $(if ($candidateSucceeded) { "Candidate build succeeded." } else { "Candidate build failed." })
}
else {
    Add-ComparisonRow -Rows $rows -Category "build" -Scope "solution" -Metric "build-summary" `
        -ReferenceValue ([bool]($null -ne $referenceBuild)) `
        -CandidateValue ([bool]($null -ne $candidateBuild)) `
        -Delta $null -DeltaPercent $null -Status "missing" `
        -Reason "Build summary is absent from one or both runs."
}

# Startup, login and world-load timings
$referenceTimings = Read-JsonFile (Join-Path $referencePath "log-timings\timing-summary.json")
$candidateTimings = Read-JsonFile (Join-Path $candidatePath "log-timings\timing-summary.json")
if ($null -ne $referenceTimings -and $null -ne $candidateTimings) {
    $referenceTimingIndex = New-RowIndex -Rows @(Get-PropertyValue $referenceTimings "timings" @()) -KeyProperty "Timing"
    $candidateTimingIndex = New-RowIndex -Rows @(Get-PropertyValue $candidateTimings "timings" @()) -KeyProperty "Timing"

    foreach ($timingName in @(Get-CombinedKeys $referenceTimingIndex $candidateTimingIndex)) {
        if (-not $referenceTimingIndex.ContainsKey($timingName) -or -not $candidateTimingIndex.ContainsKey($timingName)) {
            Add-ComparisonRow -Rows $rows -Category "timing" -Scope $timingName -Metric "timing-present" `
                -ReferenceValue $referenceTimingIndex.ContainsKey($timingName) `
                -CandidateValue $candidateTimingIndex.ContainsKey($timingName) `
                -Delta $null -DeltaPercent $null -Status "missing" `
                -Reason "Timing exists in only one run."
            continue
        }

        $referenceTiming = $referenceTimingIndex[$timingName]
        $candidateTiming = $candidateTimingIndex[$timingName]
        foreach ($metric in @("P50Milliseconds", "P95Milliseconds", "MaximumMilliseconds")) {
            Add-LowerIsBetterComparison -Rows $rows -Category "timing" -Scope $timingName -Metric $metric `
                -ReferenceValue (Get-PropertyValue $referenceTiming $metric) `
                -CandidateValue (Get-PropertyValue $candidateTiming $metric) `
                -AbsoluteNoiseFloor 5 `
                -WarningThresholdPercent $WarningPercent `
                -RegressionThresholdPercent $RegressionPercent
        }

        $candidateCount = Convert-ToNumber (Get-PropertyValue $candidateTiming "Count")
        if ($null -ne $candidateCount -and $candidateCount -lt 3) {
            Add-ComparisonRow -Rows $rows -Category "timing" -Scope $timingName -Metric "sample-confidence" `
                -ReferenceValue (Get-PropertyValue $referenceTiming "Count") `
                -CandidateValue $candidateCount -Delta $null -DeltaPercent $null `
                -Status "warning" -Reason "Fewer than three candidate measurements; percentile confidence is weak."
        }
    }
}
else {
    Add-ComparisonRow -Rows $rows -Category "timing" -Scope "all" -Metric "timing-summary" `
        -ReferenceValue ([bool]($null -ne $referenceTimings)) `
        -CandidateValue ([bool]($null -ne $candidateTimings)) `
        -Delta $null -DeltaPercent $null -Status "missing" `
        -Reason "Timing summary is absent from one or both runs."
}

# Process resources
$referenceProcessSummary = Read-JsonFile (Join-Path $referencePath "process-resources\process-summary.json")
$candidateProcessSummary = Read-JsonFile (Join-Path $candidatePath "process-resources\process-summary.json")
if ($null -ne $referenceProcessSummary -and $null -ne $candidateProcessSummary) {
    $referenceProcessIndex = New-RowIndex -Rows @(Get-AggregatedProcessRows $referenceProcessSummary) -KeyProperty "ProcessName"
    $candidateProcessIndex = New-RowIndex -Rows @(Get-AggregatedProcessRows $candidateProcessSummary) -KeyProperty "ProcessName"

    foreach ($processName in @(Get-CombinedKeys $referenceProcessIndex $candidateProcessIndex)) {
        if (-not $referenceProcessIndex.ContainsKey($processName) -or -not $candidateProcessIndex.ContainsKey($processName)) {
            Add-ComparisonRow -Rows $rows -Category "process" -Scope $processName -Metric "process-present" `
                -ReferenceValue $referenceProcessIndex.ContainsKey($processName) `
                -CandidateValue $candidateProcessIndex.ContainsKey($processName) `
                -Delta $null -DeltaPercent $null -Status "missing" `
                -Reason "Process metrics exist in only one run."
            continue
        }

        $referenceProcess = $referenceProcessIndex[$processName]
        $candidateProcess = $candidateProcessIndex[$processName]
        $definitions = @(
            @{ Name = "AverageCpuPercent"; Floor = 1.0; Warning = 10.0; Regression = 25.0 },
            @{ Name = "P95CpuPercent"; Floor = 1.0; Warning = 10.0; Regression = 25.0 },
            @{ Name = "MaxPrivateMemoryMB"; Floor = 1.0; Warning = $WarningPercent; Regression = $RegressionPercent },
            @{ Name = "MaxWorkingSetMB"; Floor = 1.0; Warning = $WarningPercent; Regression = $RegressionPercent },
            @{ Name = "PrivateMemorySlopeMBPerMinute"; Floor = 0.5; Warning = 20.0; Regression = 50.0 },
            @{ Name = "MaxHandleCount"; Floor = 10.0; Warning = 10.0; Regression = 25.0 },
            @{ Name = "MaxThreadCount"; Floor = 2.0; Warning = 10.0; Regression = 25.0 }
        )

        foreach ($definition in $definitions) {
            Add-LowerIsBetterComparison -Rows $rows -Category "process" -Scope $processName -Metric $definition.Name `
                -ReferenceValue (Get-PropertyValue $referenceProcess $definition.Name) `
                -CandidateValue (Get-PropertyValue $candidateProcess $definition.Name) `
                -AbsoluteNoiseFloor ([double]$definition.Floor) `
                -WarningThresholdPercent ([double]$definition.Warning) `
                -RegressionThresholdPercent ([double]$definition.Regression)
        }
    }
}
else {
    Add-ComparisonRow -Rows $rows -Category "process" -Scope "all" -Metric "process-summary" `
        -ReferenceValue ([bool]($null -ne $referenceProcessSummary)) `
        -CandidateValue ([bool]($null -ne $candidateProcessSummary)) `
        -Delta $null -DeltaPercent $null -Status "missing" `
        -Reason "Process summary is absent from one or both runs."
}

# Network risk counters
$referenceNetwork = Read-JsonFile (Join-Path $referencePath "network\network-summary.json")
$candidateNetwork = Read-JsonFile (Join-Path $candidatePath "network\network-summary.json")
if ($null -ne $referenceNetwork -and $null -ne $candidateNetwork) {
    $referenceNetworkIndex = New-RowIndex -Rows @(Get-PropertyValue $referenceNetwork "metrics" @()) -KeyProperty "Metric"
    $candidateNetworkIndex = New-RowIndex -Rows @(Get-PropertyValue $candidateNetwork "metrics" @()) -KeyProperty "Metric"

    foreach ($metricName in @(
        "interface-receive-errors",
        "interface-outbound-errors",
        "interface-output-queue",
        "tcp-retransmitted-segments/sec"
    )) {
        if (-not $referenceNetworkIndex.ContainsKey($metricName) -or -not $candidateNetworkIndex.ContainsKey($metricName)) {
            Add-ComparisonRow -Rows $rows -Category "network" -Scope "system" -Metric $metricName `
                -ReferenceValue $referenceNetworkIndex.ContainsKey($metricName) `
                -CandidateValue $candidateNetworkIndex.ContainsKey($metricName) `
                -Delta $null -DeltaPercent $null -Status "missing" `
                -Reason "Network risk metric is absent from one or both runs."
            continue
        }

        foreach ($statistic in @("P95", "Maximum")) {
            Add-LowerIsBetterComparison -Rows $rows -Category "network" -Scope "system" -Metric "$metricName $statistic" `
                -ReferenceValue (Get-PropertyValue $referenceNetworkIndex[$metricName] $statistic) `
                -CandidateValue (Get-PropertyValue $candidateNetworkIndex[$metricName] $statistic) `
                -AbsoluteNoiseFloor 0.1 -WarningThresholdPercent 20 -RegressionThresholdPercent 50
        }
    }
}
else {
    Add-ComparisonRow -Rows $rows -Category "network" -Scope "system" -Metric "network-summary" `
        -ReferenceValue ([bool]($null -ne $referenceNetwork)) `
        -CandidateValue ([bool]($null -ne $candidateNetwork)) `
        -Delta $null -DeltaPercent $null -Status "missing" `
        -Reason "Network summary is absent from one or both runs."
}

# Runtime binary/process topology
$referenceTopology = Read-JsonFile (Join-Path $referencePath "runtime-topology\runtime-topology.json")
$candidateTopology = Read-JsonFile (Join-Path $candidatePath "runtime-topology\runtime-topology.json")
if ($null -ne $referenceTopology -and $null -ne $candidateTopology) {
    $referenceMissing = @(Get-PropertyValue $referenceTopology "missingExpectedProcessNames" @()).Count
    $candidateMissing = @(Get-PropertyValue $candidateTopology "missingExpectedProcessNames" @()).Count
    Add-CountComparison -Rows $rows -Category "runtime-topology" -Scope "all" -Metric "missingExpectedProcessCount" `
        -ReferenceValue $referenceMissing -CandidateValue $candidateMissing -AnyIncreaseIsRegression

    $referenceTopologyIndex = New-RowIndex -Rows @(Get-PropertyValue $referenceTopology "processes" @()) -KeyProperty "ProcessName"
    $candidateTopologyIndex = New-RowIndex -Rows @(Get-PropertyValue $candidateTopology "processes" @()) -KeyProperty "ProcessName"

    foreach ($processName in @(Get-CombinedKeys $referenceTopologyIndex $candidateTopologyIndex)) {
        if (-not $referenceTopologyIndex.ContainsKey($processName) -or -not $candidateTopologyIndex.ContainsKey($processName)) {
            Add-ComparisonRow -Rows $rows -Category "runtime-topology" -Scope $processName -Metric "process-present" `
                -ReferenceValue $referenceTopologyIndex.ContainsKey($processName) `
                -CandidateValue $candidateTopologyIndex.ContainsKey($processName) `
                -Delta $null -DeltaPercent $null -Status "missing" `
                -Reason "Runtime process exists in only one run."
            continue
        }

        $referenceHash = Convert-ToText (Get-PropertyValue $referenceTopologyIndex[$processName] "Sha256")
        $candidateHash = Convert-ToText (Get-PropertyValue $candidateTopologyIndex[$processName] "Sha256")
        $sameHash = -not [string]::IsNullOrWhiteSpace($referenceHash) -and
            [string]::Equals($referenceHash, $candidateHash, [System.StringComparison]::OrdinalIgnoreCase)

        Add-ComparisonRow -Rows $rows -Category "runtime-topology" -Scope $processName -Metric "Sha256" `
            -ReferenceValue $referenceHash -CandidateValue $candidateHash `
            -Delta $null -DeltaPercent $null `
            -Status $(if ($sameHash) { "pass" } else { "info" }) `
            -Reason $(if ($sameHash) { "Runtime binary identity matches." } else { "Runtime binary identity changed or could not be compared; verify intended deployment." })
    }
}
else {
    Add-ComparisonRow -Rows $rows -Category "runtime-topology" -Scope "all" -Metric "runtime-topology" `
        -ReferenceValue ([bool]($null -ne $referenceTopology)) `
        -CandidateValue ([bool]($null -ne $candidateTopology)) `
        -Delta $null -DeltaPercent $null -Status "missing" `
        -Reason "Runtime topology is absent from one or both runs."
}

$environmentCompatible = [bool]($environmentMismatches.Count -eq 0)
$regressionCount = @($rows | Where-Object Status -eq "regression").Count
$warningCount = @($rows | Where-Object Status -eq "warning").Count
$missingCount = @($rows | Where-Object Status -eq "missing").Count
$improvementCount = @($rows | Where-Object Status -eq "improvement").Count

$overallStatus = "pass"
if (-not $environmentCompatible -and -not $AllowEnvironmentMismatch) {
    $overallStatus = "incomparable"
}
elseif ($regressionCount -gt 0) {
    $overallStatus = "regression"
}
elseif (($warningCount + $missingCount) -gt 0) {
    $overallStatus = "warning"
}

$comparison = [ordered]@{
    schemaVersion = 1
    generatedAtUtc = [DateTime]::UtcNow.ToString("o")
    referenceRun = $referencePath
    candidateRun = $candidatePath
    thresholds = [ordered]@{
        warningPercent = $WarningPercent
        regressionPercent = $RegressionPercent
    }
    environmentCompatible = $environmentCompatible
    environmentMismatches = @($environmentMismatches)
    allowEnvironmentMismatch = [bool]$AllowEnvironmentMismatch
    overallStatus = $overallStatus
    counts = [ordered]@{
        comparisons = $rows.Count
        regressions = $regressionCount
        warnings = $warningCount
        missing = $missingCount
        improvements = $improvementCount
    }
    comparisons = @($rows)
}

$jsonPath = Join-Path $OutputDirectory "baseline-comparison.json"
$csvPath = Join-Path $OutputDirectory "baseline-comparison.csv"
$markdownPath = Join-Path $OutputDirectory "baseline-comparison.md"
$comparison | ConvertTo-Json -Depth 12 | Set-Content -LiteralPath $jsonPath -Encoding UTF8
@($rows) | Export-Csv -LiteralPath $csvPath -NoTypeInformation -Encoding UTF8

$markdown = New-Object System.Text.StringBuilder
[void]$markdown.AppendLine("# Baseline comparison")
[void]$markdown.AppendLine("")
[void]$markdown.AppendLine("- Reference: $referencePath")
[void]$markdown.AppendLine("- Candidate: $candidatePath")
[void]$markdown.AppendLine("- Environment compatible: $environmentCompatible")
[void]$markdown.AppendLine("- Overall status: **$overallStatus**")
[void]$markdown.AppendLine("- Regressions: $regressionCount; warnings: $warningCount; missing: $missingCount; improvements: $improvementCount")
[void]$markdown.AppendLine("")

if ($environmentMismatches.Count -gt 0) {
    [void]$markdown.AppendLine("## Environment mismatches")
    [void]$markdown.AppendLine("")
    foreach ($item in $environmentMismatches) {
        [void]$markdown.AppendLine("- $item")
    }
    [void]$markdown.AppendLine("")
}

[void]$markdown.AppendLine("| Status | Category | Scope | Metric | Reference | Candidate | Delta % | Reason |")
[void]$markdown.AppendLine("|---|---|---|---|---:|---:|---:|---|")
$orderedRows = @(
    $rows |
        Sort-Object @{ Expression = { Get-StatusRank $_.Status } }, Category, Scope, Metric
)
foreach ($row in $orderedRows) {
    $reason = ([string]$row.Reason).Replace("|", "\|").Replace("`r", " ").Replace("`n", " ")
    [void]$markdown.AppendLine("| $($row.Status) | $($row.Category) | $($row.Scope) | $($row.Metric) | $($row.Reference) | $($row.Candidate) | $($row.DeltaPercent) | $reason |")
}
$markdown.ToString() | Set-Content -LiteralPath $markdownPath -Encoding UTF8

if ($FailOnRegression -and $overallStatus -in @("regression", "incomparable")) {
    throw "Baseline comparison failed with status '$overallStatus'. See '$markdownPath'."
}

[pscustomobject]@{
    OutputDirectory = $OutputDirectory
    Json = $jsonPath
    Csv = $csvPath
    Markdown = $markdownPath
    OverallStatus = $overallStatus
    EnvironmentCompatible = $environmentCompatible
    RegressionCount = $regressionCount
    WarningCount = $warningCount
    MissingCount = $missingCount
    ImprovementCount = $improvementCount
}
