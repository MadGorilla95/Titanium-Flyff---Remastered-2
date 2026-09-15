# Fase 0 — technische baseline en audit

Deze fase verandert **geen gameplay, protocol, database-schema of serverlogica**. Ze verzamelt eerst controleerbaar bewijs over de huidige Titanium-code en runtime. Alle latere optimalisaties moeten aantoonbaar tegen dezelfde baseline worden vergeleken.

## Waarom dit nodig is

Een mapnaam of oud projectbestand bewijst niet dat een executable werkelijk wordt gebruikt. Een stijgende private-memorygrafiek bewijst evenmin automatisch een memory leak. Daarom maakt deze audit steeds onderscheid tussen:

- **statische aanwijzing** — iets staat in de broncode of projectstructuur;
- **runtime-observatie** — iets is tijdens een gecontroleerde test werkelijk uitgevoerd;
- **bevestigde oorzaak** — de observatie is met logging, profiling of tracing aan concrete code gekoppeld.

Een baseline is alleen vergelijkbaar wanneer machine, compiler, buildconfiguratie, database, data, wereldconfiguratie, clientroute en testduur gelijk blijven. Leg een cold run apart vast en gebruik voor warme tests minimaal drie runs; vergelijk bij voorkeur de mediaan en P95 in plaats van één toevallige meting.

## Wat de audit oplevert

Elke run schrijft naar `artifacts/baseline/<timestamp>/`:

- environment- en Git-fingerprint;
- inventaris van solutions, projecten, launchscripts, binaries en executable-bewijs;
- feature-signalen, zonder ze ten onrechte als volledig werkende features te bestempelen;
- statische kandidaten voor `Sleep()`, waits, blocking I/O, SQL-hotspots, oneindige loops en brede scans;
- buildduur, exitcode, warnings en errors;
- procesmetingen voor CPU, working set, private memory, handles en threads;
- netwerkcounters en TCP-verbindingstoestanden;
- timings uit bestaande logs voor startup, login en world load;
- een manifest en runsamenvatting waarmee latere resultaten kunnen worden vergeleken.

SQL-querymetingen worden bewust via SQL Server Extended Events uitgevoerd. De scripts onder `tools/baseline/sql/` wijzigen geen queries of tabellen.

## Veilig starten

Open PowerShell in de repositoryroot:

```powershell
Set-ExecutionPolicy -Scope Process Bypass
.\tools\baseline\Invoke-BaselineAudit.ps1
```

Zonder extra switches voert dit alleen de niet-invasieve inventarisatie en statische scan uit. Het script start of stopt nooit zelf Flyff-processen en maakt geen databaseverbinding.

Maak daarna een lokale configuratie:

```powershell
Copy-Item .\tools\baseline\baseline.config.example.json `
          .\tools\baseline\baseline.config.local.json
notepad .\tools\baseline\baseline.config.local.json
```

`baseline.config.local.json` wordt niet gecommit. Vul minstens de werkelijke solution, procesnamen, logpaden en timingmarkers in.

Een volledige gecontroleerde run:

```powershell
.\tools\baseline\Invoke-BaselineAudit.ps1 `
  -ConfigPath .\tools\baseline\baseline.config.local.json `
  -IncludeBuild `
  -IncludeRuntime `
  -IncludeNetwork `
  -IncludeLogTimings
```

De servers moeten voor runtime- en netwerkmetingen al via de normale Titanium-startprocedure draaien. De audit start ze niet, omdat dat anders de vraag “welke executables worden echt gebruikt?” vooraf zou invullen.

## Werkelijk gebruikte executables vaststellen

Gebruik drie onafhankelijke bewijsbronnen:

1. **Buildbewijs** — het project bouwt een `.exe`.
2. **Startbewijs** — een `.bat`, `.cmd`, launcher of supervisor verwijst naar die executable.
3. **Runtimebewijs** — het proces draait tijdens de normale serverstart.

`Get-ProjectInventory.ps1` verzamelt de eerste twee bronnen. Vul daarna in `runtime.processNames` alleen de namen in die je tijdens een normale start werkelijk ziet:

```powershell
Get-Process |
  Sort-Object ProcessName |
  Select-Object ProcessName, Id, Path
```

Een executable wordt pas als actief beschouwd wanneer runtimebewijs aanwezig is. Een project dat alleen in de source tree staat, kan legacy of ongebruikt zijn.

## Startup-, login- en world-loadtimings

`Measure-LogTimings.ps1` gebruikt bestaande logregels en configureerbare reguliere expressies. Er zijn bewust geen fictieve Titanium-logteksten ingebouwd.

Voor elke timing zijn nodig:

- `startPattern`;
- `endPattern`;
- optioneel `correlationRegex` met een named group `correlation`;
- logpaden;
- een timestampregex met named group `timestamp`.

Voor gelijktijdige logins is correlatie verplicht. Zonder correlatie gebruikt de parser FIFO-pairing; dat is alleen betrouwbaar bij een single-client, seriële test.

## SQL-baseline

Voer de SQL-scripts uit met SQLCMD Mode ingeschakeld in SQL Server Management Studio of met `sqlcmd`.

1. Pas bovenaan `01-Create-BaselineXEventSession.sql` de SQLCMD-variabelen aan.
2. Start de Extended Events-sessie.
3. Voer exact hetzelfde testscenario uit.
4. Stop de sessie met `02-Stop-BaselineXEventSession.sql`.
5. Lees de resultaten met `03-Read-BaselineXEventSession.sql`.

De output bevat onder andere aantal calls, totaal/average/max duration, CPU, logical reads, writes en row count. Houd databasecache-effecten apart: een cold-cache-run en een warm-cache-run zijn niet onderling vergelijkbaar.

> **Gegevensbeveiliging:** Extended Events kan SQL-tekst en parameterwaarden vastleggen. Behandel `.xel`-bestanden en geëxporteerde queryrapporten als gevoelige operationele data. Bewaar ze onder de genegeerde map `artifacts/baseline/` en controleer ze vóór je ze deelt.

## Memory en leaks

`Measure-ProcessResources.ps1` berekent private-memorygroei per PID. Dat is een **groeisignaal**, geen leakbewijs. Bevestig een verdachte helling met een langere soak test en een native heap-profiler. Let ook op een stijgend aantal handles of threads; die kunnen op resource leaks wijzen zonder dat private memory sterk stijgt.

## Netwerk

`Measure-NetworkBaseline.ps1` meet systeem- en interfacecounters plus TCP-states per geselecteerd proces. Het inspecteert geen payloads. Retransmits of grote wachtrijen wijzen op nader onderzoek, maar bewijzen op zichzelf niet dat de Flyff-code de oorzaak is.

## Directe scripts

```powershell
# Alleen project- en feature-inventaris
.\tools\baseline\Get-ProjectInventory.ps1

# Alleen statische hotspotkandidaten
.\tools\baseline\Find-StaticHotspots.ps1

# Buildbaseline
.\tools\baseline\Measure-Build.ps1 `
  -Solution .\Source\Source\<werkelijke-solution>.sln `
  -Configuration Release `
  -Platform x64

# Procesresources gedurende tien minuten
.\tools\baseline\Measure-ProcessResources.ps1 `
  -ProcessName WorldServer,LoginServer `
  -DurationSeconds 600 `
  -IntervalSeconds 1

# Netwerk gedurende tien minuten
.\tools\baseline\Measure-NetworkBaseline.ps1 `
  -ProcessName WorldServer,LoginServer `
  -DurationSeconds 600 `
  -IntervalSeconds 1
```

## Acceptatiecriteria voor Fase 0

Fase 0 is pas afgerond wanneer:

- de normale buildconfiguratie en toolchain vastliggen;
- alle actieve serverprocessen met runtimebewijs zijn geregistreerd;
- minstens één cold run en drie identieke warm runs zijn uitgevoerd;
- startup, login en world load meetbare begin- en eindmarkers hebben;
- SQL-, netwerk- en resourcecounters tijdens hetzelfde scenario zijn opgenomen;
- statische bevindingen zijn getriageerd als relevant, false positive of third-party;
- het ingevulde rapport onder versiebeheer staat, zonder machinegeheimen of ruwe persoonsgegevens.

Gebruik `docs/baseline/BASELINE-REPORT-TEMPLATE.md` als beslisdocument voor de overgang naar Fase 1.
