# Runtimebewijs en baselinevergelijking

Deze aanvulling sluit twee gaten in Fase 0:

1. een draaiend proces moet aan een exact bestand, hash, versie, architectuur en endpointset worden gekoppeld;
2. een latere wijziging moet automatisch tegen een gekozen referentierun kunnen worden vergeleken.

## Runtime-topologie vastleggen

Start de Titanium-servers eerst via de normale procedure. Vul daarna in `baseline.config.local.json` de **werkelijke Windows-procesnamen** in, zonder aan te nemen dat de projectnaam en deploymentnaam gelijk zijn.

Voorbeeld:

```json
"runtime": {
  "enabled": true,
  "processNames": [
    "1. Account-x64",
    "2. Certifier-x64",
    "3. Database-x64",
    "4. Core-x64",
    "5. Login-x64",
    "6. Cache-x64",
    "7. World-x64"
  ],
  "captureTopology": true,
  "includeCommandLine": false,
  "skipFileHash": false,
  "sampleIntervalSeconds": 1,
  "durationSeconds": 300
}
```

Voer vervolgens uit:

```powershell
.\tools\baseline\Invoke-BaselineAudit.ps1 `
  -ConfigPath .\tools\baseline\baseline.config.local.json `
  -IncludeRuntime
```

De stage `runtime-topology` schrijft onder meer:

- PID en starttijd;
- parent process;
- exact executablepad;
- PE-architectuur (`x86`, `x64`, ARM of onbekend);
- bestandsgrootte en wijzigingsdatum;
- SHA-256;
- file- en productversion;
- TCP-listeners en actieve remote endpoints;
- UDP-endpoints;
- ontbrekende verwachte processen.

Een waargenomen proces krijgt `RuntimeConfirmed = true`, maar bewust `ReadinessStatus = unproven`. Een procesvenster of PID bewijst namelijk niet dat de server volledig geïnitialiseerd, bereikbaar en afhankelijkheidsklaar is.

### Rechtstreeks uitvoeren

```powershell
.\tools\baseline\Get-RuntimeTopology.ps1 `
  -ProcessName "1. Account-x64","2. Certifier-x64","7. World-x64"
```

`-IncludeCommandLine` staat standaard uit. Command lines kunnen wachtwoorden, tokens, databasegegevens of andere gevoelige argumenten bevatten. Schakel dit alleen doelbewust in en controleer de output vóór delen.

`-SkipFileHash` versnelt de snapshot, maar maakt het onmogelijk om exact te bewijzen welke binary draaide. Voor de formele baseline hoort hashing normaal ingeschakeld te blijven.

## Twee runs vergelijken

Kies één gecontroleerde run als referentie. Gebruik niet automatisch de snelste run; kies een representatieve baseline met geldige omgeving, workload en volledige meetstages.

```powershell
.\tools\baseline\Compare-BaselineRuns.ps1 `
  -ReferenceRun .\artifacts\baseline\20260915-120000Z `
  -CandidateRun .\artifacts\baseline\20260916-120000Z
```

Standaardgrenzen:

- waarschuwing vanaf 5% verslechtering;
- regressie vanaf 15% verslechtering;
- per metriek geldt daarnaast een absolute ruisvloer.

De vergelijking behandelt onder meer:

- machine-, OS-, CPU- en configuratiecompatibiliteit;
- buildtijd, warnings, errors en buildresultaat;
- P50, P95 en maximum voor startup/login/world-loadtimings;
- CPU, private memory, working set, memory slope, handles en threads;
- netwerkerrors, output queue en retransmits;
- ontbrekende verwachte processen;
- wijzigingen van runtime-binaryhashes.

Output:

```text
comparison-to-<referentierun>/
  baseline-comparison.json
  baseline-comparison.csv
  baseline-comparison.md
```

Mogelijke eindstatussen:

| Status | Betekenis |
|---|---|
| `pass` | Geen materiële regressie gevonden. |
| `warning` | Ruisgrens, ontbrekende meetdata of lichte verslechtering vereist beoordeling. |
| `regression` | Minstens één metriek overschrijdt de regressiegrens. |
| `incomparable` | Machine/OS/CPU-omgeving verschilt en vergelijking is niet betrouwbaar. |

Gebruik `-AllowEnvironmentMismatch` alleen wanneer een cross-machinevergelijking doelbewust informatief is. De output mag dan niet als zuivere performance-regressietest worden geïnterpreteerd.

Gebruik voor CI of een harde kwaliteitsgate:

```powershell
.\tools\baseline\Compare-BaselineRuns.ps1 `
  -ReferenceRun <baseline> `
  -CandidateRun <candidate> `
  -FailOnRegression
```

## Meetdiscipline

Een geldige vergelijking vereist minimaal:

- dezelfde machine en achtergrondbelasting;
- dezelfde compiler/toolset en buildconfiguratie;
- dezelfde database- en datasnapshot;
- dezelfde worldconfiguratie;
- dezelfde clientroute en handelingen;
- dezelfde testduur en sample-interval;
- een cold run die niet met warm runs wordt vermengd;
- minimaal drie warme runs voor percentilevergelijkingen.

De vergelijkingstool voorkomt slechte experimenten niet. Ze maakt verschillen reproduceerbaar en zichtbaar; causaliteit moet nog steeds met profiling, tracing en afgebakende wijzigingen worden bewezen.
