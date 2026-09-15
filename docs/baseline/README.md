# Fase 0 — technische baseline en audit

Deze fase verandert **geen gameplay, protocol, databaseschema of serverlogica**. Ze verzamelt eerst controleerbaar bewijs over de huidige Titanium-code, buildketen en runtime. Iedere latere optimalisatie moet tegen dezelfde baseline worden gemeten.

## Bewijsniveaus

De audit houdt drie niveaus strikt gescheiden:

1. **Statische aanwijzing** — iets staat in source, projectbestanden of launchscripts.
2. **Runtimeobservatie** — iets is tijdens een gecontroleerde test werkelijk uitgevoerd.
3. **Bevestigde oorzaak** — profiling, tracing of logging koppelt een gemeten probleem aan concrete code.

Een groot `.cpp`-bestand, een `Sleep()`-call, een stijgende memorygrafiek of een procesnaam is op zichzelf geen bewezen bottleneck.

## Huidige harde bevinding

De canonieke kandidaat is:

```text
Source/Source/all/all.sln
Release|x64
```

De clean-clone Windows-build van 15 september 2026 eindigde met:

- exitcode `1`;
- 20 door MSBuild gerapporteerde errors;
- 6 door MSBuild gerapporteerde warnings;
- vier primaire ontbrekende headerfamilies: `Lang.h`, `Define.h`, `defineItem.h` en `defineitemkind.h`.

Daarmee is bewezen dat de huidige repository **niet zelfstandig reproduceerbaar bouwt** zonder aanvullende resource-/generated headers. Zie [`BUILD-BASELINE-2026-09-15.md`](./BUILD-BASELINE-2026-09-15.md).

De groene auditworkflow betekent uitsluitend dat de audittools en self-tests slagen. Ze betekent niet dat de gamebuild slaagt.

## Snelste veilige start

Open PowerShell in de repositoryroot:

```powershell
cd "E:\Github\Titanium Flyff - Remastered"
Set-ExecutionPolicy -Scope Process Bypass
```

Maak één keer een lokale configuratie:

```powershell
.\tools\baseline\Initialize-TitaniumBaselineConfig.ps1
```

Die configuratie:

- kiest `Source/Source/all/all.sln` en `Release|x64`;
- vult de zeven verwachte x64-serverprocessen in;
- houdt build-, runtime-, netwerk- en logmetingen standaard uit;
- overschrijft een bestaande lokale configuratie niet zonder `-Force`;
- wordt door `.gitignore` uitgesloten.

Voer daarna alleen de veilige statische audit uit:

```powershell
.\tools\baseline\Run-Phase0Baseline.cmd
```

Of rechtstreeks:

```powershell
.\tools\baseline\Invoke-TitaniumBaseline.ps1 -Mode Static
```

Deze run start of stopt geen Titanium- of SQL-processen en wijzigt geen broncode.

## Uitvoermodi

### Alleen statisch bewijs

```powershell
.\tools\baseline\Run-Phase0Baseline.cmd -Mode Static
```

Verzamelt onder andere:

- project-, solution- en executable-inventaris;
- build- en packagingketen;
- startupvolgorde en vaste delays;
- warningbeleid;
- repositoryhygiëne;
- statische hotspotkandidaten.

### Buildbaseline

```powershell
.\tools\baseline\Run-Phase0Baseline.cmd -Mode Build
```

De preflight controleert eerst:

- solution en MSBuild;
- schrijfbare output;
- vrije schijfruimte;
- bekende compile-time resourceheaders;
- Git- en configuratiestatus.

Omdat de clean-clone headers momenteel ontbreken, zal de preflight de build standaard blokkeren. Om het falen doelbewust opnieuw als bewijs vast te leggen:

```powershell
.\tools\baseline\Run-Phase0Baseline.cmd `
  -Mode Build `
  -AllowPreflightErrors
```

De buildparser schrijft:

- `build-summary.json`;
- `BUILD-SUMMARY.md`;
- `build-diagnostics.csv`;
- `missing-build-inputs.csv`;
- `failed-projects.csv`;
- normale en diagnostische MSBuild-logs.

MSBuilds eigen summaryaantallen blijven gezaghebbend; template-continuatieregels worden niet langer als afzonderlijke warnings geteld.

### Runtime en netwerk

Start de server eerst via de normale Titanium-launcher en voer daarna uit:

```powershell
.\tools\baseline\Run-Phase0Baseline.cmd -Mode Runtime
```

De audit registreert onder andere:

- actieve processen en ontbrekende verwachte rollen;
- executablepad, PID, parent PID en starttijd;
- SHA-256, bestandsgrootte, versie en PE-architectuur;
- TCP-listeners, verbindingen en UDP-endpoints;
- CPU, working set, private memory, handles en threads;
- systeemnetwerkcounters en retransmits.

Een draaiend proces is nog geen ready-proces. Readiness moet afzonderlijk blijken uit poorten, dependencyhandshakes of expliciete logmarkers.

### Volledige geconfigureerde run

```powershell
.\tools\baseline\Run-Phase0Baseline.cmd -Mode Full
```

Dit activeert build, runtime en netwerk. Logtimings worden alleen meegenomen wanneer echte logpaden en geldige start-/eindmarkers zijn geconfigureerd.

## Preflight afzonderlijk uitvoeren

```powershell
.\tools\baseline\Test-BaselinePrerequisites.ps1 `
  -ConfigPath .\tools\baseline\baseline.config.local.json
```

Voor een buildgerichte controle:

```powershell
.\tools\baseline\Test-BaselinePrerequisites.ps1 `
  -ConfigPath .\tools\baseline\baseline.config.local.json `
  -RequireBuild `
  -FailOnError
```

De resultaten staan onder:

```text
artifacts/baseline/preflight/<timestamp>/
```

## Werkelijk gebruikte executables vaststellen

Gebruik drie onafhankelijke bewijsbronnen:

1. **Buildbewijs** — het project produceert een executable.
2. **Startbewijs** — launcher, batchbestand of supervisor verwijst ernaar.
3. **Runtimebewijs** — het proces wordt tijdens de normale serverstart werkelijk waargenomen.

De huidige statische keten is:

```text
Account → Certifier → Database → Core → Login → Cache → World
```

De x64-launcher gebruikt ongeveer acht seconden vaste delay vóór World wordt gestart en ongeveer negen seconden tot de pauzestatus. Dat is launcher-overhead, geen readinessmeting.

## Startup-, login- en world-loadtimings

`Measure-LogTimings.ps1` gebruikt bestaande logregels met configureerbare regexen. Er zijn bewust geen verzonnen Titanium-markers ingebouwd.

Per timing zijn nodig:

- `startPattern`;
- `endPattern`;
- optioneel `correlationRegex` met named group `correlation`;
- logpaden;
- een timestampregex met named group `timestamp`.

Zonder correlatie gebruikt de parser FIFO-pairing. Dat is alleen betrouwbaar bij een gecontroleerde single-clienttest.

## SQL-baseline

Voer de scripts onder `tools/baseline/sql/` uit met SQLCMD Mode of `sqlcmd`:

1. `01-Create-BaselineXEventSession.sql`
2. voer exact hetzelfde testscenario uit;
3. `02-Stop-BaselineXEventSession.sql`
4. `03-Read-BaselineXEventSession.sql`

De output bevat aantallen, duration, CPU, logical reads, writes en row counts. Vergelijk cold-cache en warm-cache niet als één dataset.

> Extended Events kan SQL-tekst en parameterwaarden bevatten. Bewaar ruwe `.xel`-bestanden onder `artifacts/baseline/` en controleer ze vóór delen.

## Memory en leaks

`Measure-ProcessResources.ps1` rapporteert private-memorygroei en een lineaire slope per PID. Dat is een **signaal**, geen leakbewijs. Bevestig verdachte groei met een langere soaktest en native heap-/handleprofiling.

## Baselines vergelijken

Na een referentie- en kandidaatrun:

```powershell
.\tools\baseline\Compare-BaselineRuns.ps1 `
  -BaselineRun .\artifacts\baseline\<referentie> `
  -CandidateRun .\artifacts\baseline\<kandidaat>
```

De vergelijking controleert omgevingscompatibiliteit, buildtijd, timings, CPU, memory, handles, threads, netwerkcounters, procesaanwezigheid en binaryhashes.

## Fase 0 is pas afgerond wanneer

- de clean-clone build reproduceerbaar is of alle externe/generated prerequisites exact zijn gedocumenteerd;
- de normale toolchain en buildconfiguratie vastliggen;
- actieve executables met pad en hash runtimebevestigd zijn;
- minimaal één cold run en drie identieke warm runs beschikbaar zijn;
- startup, login en world load geldige begin- en eindmarkers hebben;
- SQL-, netwerk- en resourcecounters tijdens hetzelfde scenario zijn opgenomen;
- statische hits zijn getriageerd als relevant, false positive, generated of third-party;
- het ingevulde baseline-rapport geen geheimen, IP-adressen of persoonsgegevens bevat.

Gebruik [`BASELINE-REPORT-TEMPLATE.md`](./BASELINE-REPORT-TEMPLATE.md) als beslisdocument voor Fase 1.
