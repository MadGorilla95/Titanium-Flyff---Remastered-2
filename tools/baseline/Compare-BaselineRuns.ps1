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

function Resolve-DirectoryPath {
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

function Convert-ToComparableText {
    param([object]$Value)
    if ($null -eq $Value) { return "" }
    if ($Value -is [System.Array]) { return (@($Value) -join ";") }
    return [string]$Value
}

function Add-ComparisonRow {
    param(
        [Parameter(Mandatory = $true)][System.Collections.ArrayList]$List,
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

    [void]$List.Add([pscustomobject]@{
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
        [Parameter(Mandatory = $true)][System.Collections.ArrayList]$List,
        [Parameter(Mandatory = $true)][string]$Category,
        [Parameter(Mandatory = $true)][string]$Scope,
        [Parameter(Mandatory = $true)][string]$Metric,
        [object]$ReferenceValue,
        [object]$CandidateValue,
        [double]$MinimumAbsoluteDelta,
        [double]$WarningThresholdPercent,
        [double]$RegressionThresholdPercent
    )

    $referenceNumber = Convert-ToNumber $ReferenceValue
    $candidateNumber = Convert-ToNumber $CandidateValue
    if ($null -eq $referenceNumber -or $null -eq $candidateNumber) {
        Add-ComparisonRow -List $List -Category $Category -Scope $Scope -Metric $Metric -ReferenceValue $ReferenceValue -CandidateValue $CandidateValue -Delta $null -DeltaPercent $null -Status "missing" -Reason "Metric is absent from one or both runs."
        return
    }

    $delta = $candidateNumber - $referenceNumber
    $deltaPercent = $null
    if ([math]::Abs($referenceNumber) -gt 0.0000001) {
        $deltaPercent = ($delta / [math]::Abs($referenceNumber)) * 100.0
    }

    $status = "pass"
    $reason = "No material regression."
    if ($delta -lt (-1.0 * $MinimumAbsoluteDelta)) {
        $status = "improvement"
        $reason = "Candidate is materially lower."
    }
    elseif ($delta -le $MinimumAbsoluteDelta) {
        $status = "pass"
        $reason = "Change is inside the absolute noise floor."
    }
    elseif ($null -eq $deltaPercent) {
        $status = "warning"
        $reason = "Reference is zero; percentage change is undefined and the candidate increased."
    }
    elseif ($deltaPercent -ge $RegressionThresholdPercent) {
        $status = "regression"
        $reason = "Increase exceeds the regression threshold."
    }
    elseif ($deltaPercent -ge $WarningThresholdPercent) {
        $status = "warning"
        $reason = "Increase exceeds the warning threshold."
    }

    Add-ComparisonRow -List $List -Category $Category -Scope $Scope -Metric $Metric -ReferenceValue ([math]::Round($referenceNumber, 6)) -CandidateValue ([math]::Round($candidateNumber, 6)) -Delta ([math]::Round($delta, 6)) -DeltaPercent $(if ($null -eq $deltaPercent) { $null } else { [math]::Round($deltaPercent, 3) }) -Status $status -Reason $reason
}

function Add-CountComparison {
    param(
        [Parameter(Mandatory = $true)][System.Collections.ArrayList]$List,
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
        Add-ComparisonRow -List $List -Category $Category -Scope $Scope -Metric $Metric -ReferenceValue $ReferenceValue -CandidateValue $CandidateValue -Delta $null -DeltaPercent $null -Status "missing" -Reason "Count is absent from one or both runs."
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
        $reason = if ($AnyIncreaseIsRegression) { "Any increase is a regression for this count." } else { "Count increased and needs review." }
    }

    Add-ComparisonRow -List $List -Category $Category -Scope $Scope -Metric $Metric -ReferenceValue $referenceNumber -CandidateValue $candidateNumber -Delta $delta -DeltaPercent $null -Status $status -Reason $reason
}

function Get-IndexedRows {
    param(
        [object[]]$Rows,
        [Parameter(Mandatory = $true)][string]$KeyProperty
    )

    $index = @{}
    foreach ($row in @($Rows)) {
        $key = [string](Get-PropertyValue -Object $row -Name $KeyProperty -Default "")
        if (-not [string]::IsNullOrWhiteSpace($key)) { $index[$key] = $row }
    }
    return $index
}

function Get-AggregatedProcessRows {
    param([object]$Summary)

    if ($null -eq $Summary) { return @() }
    $rows = @(Get-PropertyValue -Object $Summary -Name "processes" -Default @())
    $result = New-Object System.Collections.ArrayList

    foreach ($group in @($rows | Group-Object ProcessName)) {
        $sampleTotal = 0.0
        $weightedCpu = 0.0
        $hasAverageCpu = $false
        foreach ($row in $group.Group) {
            $sampleCount = Convert-ToNumber (Get-PropertyValue -Object $row -Name "SampleCount" -Default 0)
            $averageCpu = Convert-ToNumber (Get-PropertyValue -Object $row -Name "AverageCpuPercent")
            if ($null -ne $averageCpu -and $sampleCount -gt 0) {
                $sampleTotal += $sampleCount
                $weightedCpu += ($averageCpu * $sampleCount)
                $hasAverageCpu = $true
            }
        }

        $p95CpuValues = @($group.Group | ForEach-Object { Convert-ToNumber (Get-PropertyValue -Object $_ -Name "P95CpuPercent") } | Where-Object { $null -ne $_ })
        $privateValues = @($group.Group | ForEach-Object { Convert-ToNumber (Get-PropertyValue -Object $_ -Name "MaxPrivateMemoryMB") } | Where-Object { $null -ne $_ })
        $workingValues = @($group.Group | ForEach-Object { Convert-ToNumber (Get-PropertyValue -Object $_ -Name "MaxWorkingSetMB") } | Where-Object { $null -ne $_ })
        $slopeValues = @($group.Group | ForEach-Object { Convert-ToNumber (Get-PropertyValue -Object $_ -Name "PrivateMemorySlopeMBPerMinute") } | Where-Object { $null -ne $_ })
        $handleValues = @($group.Group | ForEach-Object { Convert-ToNumber (Get-PropertyValue -Object $_ -Name "MaxHandleCount") } | Where-Object { $null -ne $_ })
        $threadValues = @($group.Group | ForEach-Object { Convert-ToNumber (Get-PropertyValue -Object $_ -Name "MaxThreadCount") } | Where-Object { $null -ne $_ })

        [void]$result.Add([pscustomobject]@{
            ProcessName = $group.Name
            SampleCount = [int]$sampleTotal
            AverageCpuPercent = $(if ($hasAverageCpu -and $sampleTotal -gt 0) { $weightedCpu / $sampleTotal } else { $null })
            P95CpuPercent = $(if ($p95CpuValues.Count -gt 0) { ($p95CpuValues | Measure-Object -Maximum).Maximum } else { $null })
            MaxPrivateMemoryMB = $(if ($privateValues.Count -gt 0) { ($privateValues | Measure-Object -Maximum).Maximum } else { $null })
            MaxWorkingSetMB = $(if ($workingValues.Count -gt 0) { ($workingValues | Measure-Object -Maximum).Maximum } else { $null })
            PrivateMemorySlopeMBPerMinute = $(if ($slopeValues.Count -gt 0) { ($slopeValues | Measure-Object -Maximum).Maximum } else { $null })
            MaxHandleCount = $(if ($handleValues.Count -gt 0) { ($handleValues | Measure-Object -Maximum).Maximum } else { $null })
            MaxThreadCount = $(if ($threadValues.Count -gt 0) { ($threadValues | Measure-Object -Maximum).Maximum } else { $null })
        })
    }

    return @($result)
}

$referencePath = Resolve-DirectoryPath $ReferenceRun
$candidatePath = Resolve-DirectoryPath $CandidateRun
if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $referenceName = Split-Path $referencePath -Leaf
    $OutputDirectory = Join-Path $candidatePath ("comparison-to-" + $referenceName)
}
$OutputDirectory = [System.IO.Path]::GetFullPath($OutputDirectory)
[void](New-Item -ItemType Directory -Path $OutputDirectory -Force)

$results = New-Object System.Collections.ArrayList
$environmentMismatches = New-Object System.Collections.ArrayList

$referenceEnvironment = Read-JsonFile (Join-Path $referencePath "environment.json")
$candidateEnvironment = Read-JsonFile (Join-Path $candidatePath "environment.json")
if ($null -eq $referenceEnvironment -or $null -eq $candidateEnvironment) {
    [void]$environmentMismatches.Add("environment.json missing from one or both runs")
    Add-ComparisonRow -List $results -Category "environment" -Scope "run" -Metric "environment.json" -ReferenceValue ([bool]($null -ne $referenceEnvironment)) -CandidateValue ([bool]($null -ne $candidateEnvironment)) -Delta $null -DeltaPercent $null -Status "missing" -Reason "A controlled comparison requires both environment fingerprints."
}
else {
    $hardEnvironmentFields = @("computerName", "osBuild", "logicalProcessorCount")
    foreach ($field in $hardEnvironmentFields) {
        $referenceValue = Convert-ToComparableText (Get-PropertyValue -Object $referenceEnvironment -Name $field)
        $candidateValue = Convert-ToComparableText (Get-PropertyValue -Object $candidateEnvironment -Name $field)
        $same = [string]::Equals($referenceValue, $candidateValue, [System.StringComparison]::OrdinalIgnoreCase)
        if (-not $same) { [void]$environmentMismatches.Add("$field differs") }
        Add-ComparisonRow -List $results -Category "environment" -Scope "run" -Metric $field -ReferenceValue $referenceValue -CandidateValue $candidateValue -Delta $null -DeltaPercent $null -Status $(if ($same) { "pass" } else { "warning" }) -Reason $(if ($same) { "Environment field matches." } else { "Environment field differs; comparability is reduced." })
    }

    $referenceProcessors = Convert-ToComparableText (Get-PropertyValue -Object $referenceEnvironment -Name "processors")
    $candidateProcessors = Convert-ToComparableText (Get-PropertyValue -Object $candidateEnvironment -Name "processors")
    $processorsMatch = [string]::Equals($referenceProcessors, $candidateProcessors, [System.StringComparison]::OrdinalIgnoreCase)
    if (-not $processorsMatch) { [void]$environmentMismatches.Add("processor model differs") }
    Add-ComparisonRow -List $results -Category "environment" -Scope "run" -Metric "processors" -ReferenceValue $referenceProcessors -CandidateValue $candidateProcessors -Delta $null -DeltaPercent $null -Status $(if ($processorsMatch) { "pass" } else { "warning" }) -Reason $(if ($processorsMatch) { "Processor model matches." } else { "Processor model differs; performance values are not directly comparable." })

    $referenceConfigHash = Convert-ToComparableText (Get-PropertyValue -Object $referenceEnvironment -Name "configSha256")
    $candidateConfigHash = Convert-ToComparableText (Get-PropertyValue -Object $candidateEnvironment -Name "configSha256")
    $configMatches = [string]::Equals($referenceConfigHash, $candidateConfigHash, [System.StringComparison]::OrdinalIgnoreCase)
    Add-ComparisonRow -List $results -Category "environment" -Scope "run" -Metric "configSha256" -ReferenceValue $referenceConfigHash -CandidateValue $candidateConfigHash -Delta $null -DeltaPercent $null -Status $(if ($configMatches) { "pass" } else { "warning" }) -Reason $(if ($configMatches) { "Effective baseline configuration matches." } else { "Effective baseline configuration changed; verify that the workload is still equivalent." })
}

$referenceBuild = Read-JsonFile (Join-Path $referencePath "build\build-summary.json")
$candidateBuild = Read-JsonFile (Join-Path $candidatePath "build\build-summary.json")
if ($null -ne $referenceBuild -and $null -ne $candidateBuild) {
    Add-LowerIsBetterComparison -List $results -Category "build" -Scope "solution" -Metric "elapsedMilliseconds" -ReferenceValue (Get-PropertyValue $referenceBuild "elapsedMilliseconds") -CandidateValue (Get-PropertyValue $candidateBuild "elapsedMilliseconds") -MinimumAbsoluteDelta 500 -WarningThresholdPercent 10 -RegressionThresholdPercent 25
    Add-CountComparison -List $results -Category "build" -Scope "solution" -Metric "warningCount" -ReferenceValue (Get-PropertyValue $referenceBuild "warningCount") -CandidateValue (Get-PropertyValue $candidateBuild "warningCount")
    Add-CountComparison -List $results -Category "build" -Scope "solution" -Metric "errorCount" -ReferenceValue (Get-PropertyValue $referenceBuild "errorCount") -CandidateValue (Get-PropertyValue $candidateBuild "errorCount") -AnyIncreaseIsRegression

    $candidateSucceeded = [bool](Get-PropertyValue -Object $candidateBuild -Name "succeeded" -Default $false)
    Add-ComparisonRow -List $results -Category "build" -Scope "solution" -Metric "succeeded" -ReferenceValue (Get-PropertyValue $referenceBuild "succeeded") -CandidateValue $candidateSucceeded -Delta $null -DeltaPercent $null -Status $(if ($candidateSucceeded) { "pass" } else { "regression" }) -Reason $(if ($candidateSucceeded) { "Candidate build succeeded." } else { "Candidate build failed." })
}
else {
    Add-ComparisonRow -List $results -Category "build" -Scope "solution" -Metric "build-summary" -ReferenceValue ([bool]($null -ne $referenceBuild)) -CandidateValue ([bool]($null -ne $candidateBuild)) -Delta $null -DeltaPercent $null -Status "missing" -Reason "Build summary is absent from one or both runs."
}

$referenceTimings = Read-JsonFile (Join-Path $referencePath "log-timings\timing-summary.json")
$candidateTimings = Read-JsonFile (Join-Path $candidatePath "log-timings\timing-summary.json")
if ($null -ne $referenceTimings -and $null -ne $candidateTimings) {
    $referenceTimingIndex = Get-IndexedRows -Rows @(Get-PropertyValue $referenceTimings "timings" @()) -KeyProperty "Timing"
    $candidateTimingIndex = Get-IndexedRows -Rows @(Get-PropertyValue $candidateTimings "timings" @()) -KeyProperty "Timing"
    $timingNames = @($referenceTimingIndex.Keys + $candidateTimingIndex.Keys | Sort-Object -Unique)

    foreach ($name in $timingNames) {
        if (-not $referenceTimingIndex.ContainsKey($name) -or -not $candidateTimingIndex.ContainsKey($name)) {
            Add-ComparisonRow -List $results -Category "timing" -Scope $name -Metric "timing-present" -ReferenceValue $referenceTimingIndex.ContainsKey($name) -CandidateValue $candidateTimingIndex.ContainsKey($name) -Delta $null -DeltaPercent $null -Status "missing" -Reason "Timing exists in only one run."
            continue
        }

        $referenceTiming = $referenceTimingIndex[$name]
        $candidateTiming = $candidateTimingIndex[$name]
        foreach ($metric in @("P50Milliseconds", "P95Milliseconds", "MaximumMilliseconds")) {
            Add-LowerIsBetterComparison -List $results -Category "timing" -Scope $name -Metric $metric -ReferenceValue (Get-PropertyValue $referenceTiming $metric) -CandidateValue (Get-PropertyValue $candidateTiming $metric) -MinimumAbsoluteDelta 5 -WarningThresholdPercent $WarningPercent -RegressionThresholdPercent $RegressionPercent
        }

        $candidateCount = Convert-ToNumber (Get-PropertyValue $candidateTiming "Count")
        if ($null -ne $candidateCount -and $candidateCount -lt 3) {
            Add-ComparisonRow -List $results -Category "timing" -Scope $name -Metric "sample-confidence" -ReferenceValue (Get-PropertyValue $referenceTiming "Count") -CandidateValue $candidateCount -Delta $null -DeltaPercent $null -Status "warning" -Reason "Fewer than three candidate measurements; percentile confidence is weak."
        }
    }
}
else {
    Add-ComparisonRow -List $results -Category "timing" -Scope "all" -Metric "timing-summary" -ReferenceValue ([bool]($null -ne $referenceTimings)) -CandidateValue ([bool]($null -ne $candidateTimings)) -Delta $null -DeltaPercent $null -Status "missing" -Reason "Timing summary is absent from one or both runs."
}

$referenceProcessSummary = Read-JsonFile (Join-Path $referencePath "process-resources\process-summary.json")
$candidateProcessSummary = Read-JsonFile (Join-Path $candidatePath "process-resources\process-summary.json")
if ($null -ne $referenceProcessSummary -and $null -ne $candidateProcessSummary) {
    $referenceProcessIndex = Get-IndexedRows -Rows @(Get-AggregatedProcessRows $referenceProcessSummary) -KeyProperty "ProcessName"
    $candidateProcessIndex = Get-IndexedRows -Rows @(Get-AggregatedProcessRows $candidateProcessSummary) -KeyProperty "ProcessName"
    $processNames = @($referenceProcessIndex.Keys + $candidateProcessIndex.Keys | Sort-Object -Unique)

    foreach ($name in $processNames) {
        if (-not $referenceProcessIndex.ContainsKey($name) -or -not $candidateProcessIndex.ContainsKey($name)) {
            Add-ComparisonRow -List $results -Category "process" -Scope $name -Metric "process-present" -ReferenceValue $referenceProcessIndex.ContainsKey($name) -CandidateValue $candidateProcessIndex.ContainsKey($name) -Delta $null -DeltaPercent $null -Status "missing" -Reason "Process metrics exist in only one run."
            continue
        }

        $referenceProcess = $referenceProcessIndex[$name]
        $candidateProcess = $candidateProcessIndex[$name]
        Add-LowerIsBetterComparison -List $results -Category "process" -Scope $name -Metric "AverageCpuPercent" -ReferenceValue $referenceProcess.AverageCpuPercent -CandidateValue $candidateProcess.AverageCpuPercent -MinimumAbsoluteDelta 1 -WarningThresholdPercent 10 -RegressionThresholdPercent 25
        Add-LowerIsBetterComparison -List $results -Category "process" -Scope $name -Metric "P95CpuPercent" -ReferenceValue $referenceProcess.P95CpuPercent -CandidateValue $candidateProcess.P95CpuPercent -MinimumAbsoluteDelta 1 -WarningThresholdPercent 10 -RegressionThresholdPercent 25
        Add-LowerIsBetterComparison -List $results -Category "process" -Scope $name -Metric "MaxPrivateMemoryMB" -ReferenceValue $referenceProcess.MaxPrivateMemoryMB -CandidateValue $candidateProcess.MaxPrivateMemoryMB -MinimumAbsoluteDelta 1 -WarningThresholdPercent $WarningPercent -RegressionThresholdPercent $RegressionPercent
        Add-LowerIsBetterComparison -List $results -Category "process" -Scope $name -Metric "MaxWorkingSetMB" -ReferenceValue $referenceProcess.MaxWorkingSetMB -CandidateValue $candidateProcess.MaxWorkingSetMB -MinimumAbsoluteDelta 1 -WarningThresholdPercent $WarningPercent -RegressionThresholdPercent $RegressionPercent
        Add-LowerIsBetterComparison -List $results -Category "process" -Scope $name -Metric "PrivateMemorySlopeMBPerMinute" -ReferenceValue $referenceProcess.PrivateMemorySlopeMBPerMinute -CandidateValue $candidateProcess.PrivateMemorySlopeMBPerMinute -MinimumAbsoluteDelta 0.5 -WarningThresholdPercent 20 -RegressionThresholdPercent 50
        Add-LowerIsBetterComparison -List $results -Category "process" -Scope $name -Metric "MaxHandleCount" -ReferenceValue $referenceProcess.MaxHandleCount -CandidateValue $candidateProcess.MaxHandleCount -MinimumAbsoluteDelta 10 -WarningThresholdPercent 10 -RegressionThresholdPercent 25
        Add-LowerIsBetterComparison -List $results -Category "process" -Scope $name -Metric "MaxThreadCount" -ReferenceValue $referenceProcess.MaxThreadCount -CandidateValue $candidateProcess.MaxThreadCount -MinimumAbsoluteDelta 2 -WarningThresholdPercent 10 -RegressionThresholdPercent 25
    }
}
else {
    Add-ComparisonRow -List $results -Category "process" -Scope "all" -Metric "process-summary" -ReferenceValue ([bool]($null -ne $referenceProcessSummary)) -CandidateValue ([bool]($null -ne $candidateProcessSummary)) -Delta $null -DeltaPercent $null -Status "missing" -Reason "Process summary is absent from one or both runs."
}

$referenceNetwork = Read-JsonFile (Join-Path $referencePath "network\network-summary.json")
$candidateNetwork = Read-JsonFile (Join-Path $candidatePath "network\network-summary.json")
if ($null -ne $referenceNetwork -and $null -ne $candidateNetwork) {
    $referenceNetworkIndex = Get-IndexedRows -Rows @(Get-PropertyValue $referenceNetwork "metrics" @()) -KeyProperty "Metric"
    $candidateNetworkIndex = Get-IndexedRows -Rows @(Get-PropertyValue $candidateNetwork "metrics" @()) -KeyProperty "Metric"
    $riskMetrics = @(
        "interface-receive-errors",
        "interface-outbound-errors",
        "interface-output-queue",
        "tcp-retransmitted-segments/sec"
    )
    foreach ($metricName in $riskMetrics) {
        if (-not $referenceNetworkIndex.ContainsKey($metricName) -or -not $candidateNetworkIndex.ContainsKey($metricName)) {
            Add-ComparisonRow -List $results -Category "network" -Scope "system" -Metric $metricName -ReferenceValue $referenceNetworkIndex.ContainsKey($metricName) -CandidateValue $candidateNetworkIndex.ContainsKey($metricName) -Delta $null -DeltaPercent $null -Status "missing" -Reason "Network risk metric is absent from one or both runs."
            continue
        }
        Add-LowerIsBetterComparison -List $results -Category "network" -Scope "system" -Metric ("$metricName P95") -ReferenceValue (Get-PropertyValue $referenceNetworkIndex[$metricName] "P95") -CandidateValue (Get-PropertyValue $candidateNetworkIndex[$metricName] "P95") -MinimumAbsoluteDelta 0.1 -WarningThresholdPercent 20 -RegressionThresholdPercent 50
        Add-LowerIsBetterComparison -List $results -Category "network" -Scope "system" -Metric ("$metricName Maximum") -ReferenceValue (Get-PropertyValue $referenceNetworkIndex[$metricName] "Maximum") -CandidateValue (Get-PropertyValue $candidateNetworkIndex[$metricName] "Maximum") -MinimumAbsoluteDelta 0.1 -WarningThresholdPercent 20 -RegressionThresholdPercent 50
    }
}
else {
    Add-ComparisonRow -List $results -Category "network" -Scope "system" -Metric "network-summary" -ReferenceValue ([bool]($null -ne $referenceNetwork)) -CandidateValue ([bool]($null -ne $candidateNetwork)) -Delta $null -DeltaPercent $null -Status "missing" -Reason "Network summary is absent from one or both runs."
}

$referenceTopology = Read-JsonFile (Join-Path $referencePath "runtime-topology\runtime-topology.json")
$candidateTopology = Read-JsonFile (Join-Path $candidatePath "runtime-topology\runtime-topology.json")
if ($null -ne $referenceTopology -and $null -ne $candidateTopology) {
    Add-CountComparison -List $results -Category "runtime-topology" -Scope "all" -Metric "missingExpectedProcessCount" -ReferenceValue @((Get-PropertyValue $referenceTopology "missingExpectedProcessNames" @())).Count -CandidateValue @((Get-PropertyValue $candidateTopology "missingExpectedProcessNames" @())).Count -AnyIncreaseIsRegression

    $referenceTopologyIndex = Get-IndexedRows -Rows @(Get-PropertyValue $referenceTopology "processes" @()) -KeyProperty "ProcessName"
    $candidateTopologyIndex = Get-IndexedRows -Rows @(Get-PropertyValue $candidateTopology "processes" @()) -KeyProperty "ProcessName"
    foreach ($name in @($referenceTopologyIndex.Keys + $candidateTopologyIndex.Keys | Sort-Object -Unique)) {
        if ($referenceTopologyIndex.ContainsKey($name) -and $candidateTopologyIndex.ContainsKey($name)) {
            $referenceHash = Convert-ToComparableText (Get-PropertyValue $referenceTopologyIndex[$name] "Sha256")
            $candidateHash = Convert-ToComparableText (Get-PropertyValue $candidateTopologyIndex[$name] "Sha256")
            $sameHash = -not [string]::IsNullOrWhiteSpace($referenceHash) -and [string]::Equals($referenceHash, $candidateHash, [System.StringComparison]::OrdinalIgnoreCase)
            Add-ComparisonRow -List $results -Category "runtime-topology" -Scope $name -Metric "Sha256" -ReferenceValue $referenceHash -CandidateValue $candidateHash -Delta $null -DeltaPercent $null -Status $(if ($sameHash) { "pass" } else { "info" }) -Reason $(if ($sameHash) { "Runtime binary identity matches." } else { "Runtime binary identity changed or could not be compared; verify that this is the intended build." })
        }
    }
}
else {
    Add-ComparisonRow -List $results -Category "runtime-topology" -Scope "all" -Metric "runtime-topology" -ReferenceValue ([bool]($null -ne $referenceTopology)) -CandidateValue ([bool]($null -ne $candidateTopology)) -Delta $null -DeltaPercent $null -Status "missing" -Reason "Runtime topology is absent from one or both runs."
}

$environmentCompatible = [bool]($environmentMismatches.Count -eq 0)
$regressionCount = @($results | Where-Object Status -eq "regression").Count
$warningCount = @($results | Where-Object Status -eq "warning").Count
$missingCount = @($results | Where-Object Status -eq "missing").Count
$improvementCount = @($results | Where-Object Status -eq "improvement").Count

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
        comparisons = $results.Count
        regressions = $regressionCount
        warnings = $warningCount
        missing = $missingCount
        improvements = $improvementCount
    }
    comparisons = @($results)
}

$jsonPath = Join-Path $OutputDirectory "baseline-comparison.json"
$csvPath = Join-Path $OutputDirectory "baseline-comparison.csv"
$markdownPath = Join-Path $OutputDirectory "baseline-comparison.md"
$comparison | ConvertTo-Json -Depth 12 | Set-Content -LiteralPath $jsonPath -Encoding UTF8
@($results) | Export-Csv -LiteralPath $csvPath -NoTypeInformation -Encoding UTF8

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
    foreach ($item in $environmentMismatches) { [void]$markdown.AppendLine("- $item") }
    [void]$markdown.AppendLine("")
}
[void]$markdown.AppendLine("| Status | Category | Scope | Metric | Reference | Candidate | Delta % | Reason |")
[void]$markdown.AppendLine("|---|---|---|---|---:|---:|---:|---|")
foreach ($row in @($results | Sort-Object @{Expression={switch ($_.Status) { 'regression' {0}; 'incomparable' {1}; 'warning' {2}; 'missing' {3}; 'improvement' {4}; 'info' {5}; default {6} }}}, Category, Scope, Metric)) {
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
