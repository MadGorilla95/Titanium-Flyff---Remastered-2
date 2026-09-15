# Titanium Flyff — baseline-rapport

> Vul dit rapport pas in na een gecontroleerde run. Bewaar ruwe meetbestanden in `artifacts/baseline/`; commit alleen resultaten zonder geheimen, accountgegevens, IP-adressen of persoonsgegevens.

## 1. Identiteit van de meting

| Veld | Waarde |
|---|---|
| Datum/tijd | |
| Git-commit | |
| Git-branch | |
| Werkboom schoon | |
| Machine | |
| Windows-versie | |
| CPU / logisch aantal cores | |
| RAM | |
| Energieplan | |
| Visual Studio / MSBuild | |
| PlatformToolset / Windows SDK | |
| SQL Server-versie | |
| Buildconfiguratie / platform | |
| Database/data-snapshot | |
| World/config-snapshot | |
| Clientbuild | |
| Testeroute/scenario | |
| Run-type | cold / warm |
| Runnummer | |

## 2. Gecontroleerd testscenario

Beschrijf exact dezelfde stappen voor iedere run:

1.
2.
3.
4.

### Belastingsprofiel

| Parameter | Waarde |
|---|---|
| Gelijktijdige clients | |
| Accounts/characters | |
| World/channel | |
| Route/acties | |
| Testduur | |
| Externe services | |
| Antivirus/andere belasting | |

## 3. Project- en runtime-inventaris

### Componenten

| Component | Classificatie | Canoniek project | Actief | Owner | Opmerking |
|---|---|---|---:|---|---|
| | production-server / client / tool / shared / third-party | | | | |

### Actieve executableketen

| Volgorde | Executable | Bestandspad | SHA-256 | Project | Startbewijs | Runtimebewijs | Ready-marker | Functie |
|---:|---|---|---|---|---|---|---|---|
| 1 | | | | | | | | |

### Aanwezige maar niet-bewezen projecten

| Project/executable | Bewijs in source | Waarom mogelijk legacy | Besluit |
|---|---|---|---|
| | | | |

### Custom feature-inventaris

| Feature | Pad/codebewijs | Runtime getest | Compleet | Opmerking |
|---|---|---:|---:|---|
| | | nee | onbekend | |

## 4. Buildbaseline

### Build-policy vóór de meting

| Project | Configuratie/platform | Warning level | Warnings as errors | Disabled warnings | Toolset | SDK | Risico |
|---|---|---|---|---|---|---|---|
| | | | | | | | |

| Metric | Cold build | Warm build 1 | Warm build 2 | Warm build 3 | Mediaan |
|---|---:|---:|---:|---:|---:|
| Totale duur (s) | | | | | |
| Warnings | | | | | |
| Errors | | | | | |

### Belangrijkste warnings/errors

| Code | Aantal | Project/bestand | Configuratie | Risico | Besluit |
|---|---:|---|---|---|---|
| | | | | | |

## 5. Startup-topologie en runtime-timings

### Launcher-/orchestratortopologie

| Launcher | Architectuur | Procesvolgorde | Vaste delay vóór laatste start | Totale scripted startupdelay | Readinesscheck | Beoordeling |
|---|---|---|---:|---:|---:|---|
| | | | | | | |

### Procesreadiness

| Proces | Launch-tijd | Proces zichtbaar | Poort/listener ready | Dependency ready | Log-ready | Totale readyduur (ms) |
|---|---|---|---|---|---|---:|
| | | | | | | |

### Volledige serverstartup

| Run | Launcherduur (ms) | Tijd tot World gestart | Tijd tot volledige keten ready | Cold/warm | Geldig | Opmerking |
|---:|---:|---:|---:|---|---:|---|
| 1 | | | | | | |

> Rapporteer vaste launcherwachttijd apart. Een `timeout` is geen ready-marker.

### Login

| Run | Correlatie/client | Duur (ms) | P50 | P95 | Max | Fouten |
|---:|---|---:|---:|---:|---:|---:|
| 1 | | | | | | |

### World load

| Run | World/channel | Duur (ms) | P50 | P95 | Max | Fouten |
|---:|---|---:|---:|---:|---:|---:|
| 1 | | | | | | |

## 6. SQL-baseline

| Query/operation | Calls | Total ms | Avg ms | Max ms | CPU ms | Logical reads | Writes | Rows | Beoordeling |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---|
| | | | | | | | | | |

Controlepunten:

- vergelijk cold-cache en warm-cache niet als één dataset;
- beoordeel totaalwerk én latency per call;
- een snelle query die extreem vaak wordt aangeroepen kan belangrijker zijn dan één trage query;
- controleer parameterisatie en execution plans vóór indexwijzigingen;
- leg deadlocks, timeouts en blocking chains afzonderlijk vast.

## 7. Procesresources

| Proces/PID | Avg CPU % | P95 CPU % | Max CPU % | Start private MB | Eind private MB | Groei MB/min | Max handles | Max threads |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| | | | | | | | | |

### Leakonderzoek

| Signaal | Reproduceerbaar | Soakduur | Profilerbewijs | Vermoedelijke owner | Besluit |
|---|---:|---:|---:|---|---|
| Private memory stijgt | | | | | |
| Handles stijgen | | | | | |
| Threads stijgen | | | | | |

## 8. Netwerkbaseline

| Metric | P50 | P95 | Max | Opmerking |
|---|---:|---:|---:|---|
| Bytes total/sec | | | | |
| TCP retransmits/sec | | | | |
| Established connections | | | | |
| SYN_SENT / CLOSE_WAIT / TIME_WAIT | | | | |
| Interface errors | | | | |

### Verdachte flows

| Proces | Local endpoint | Remote endpoint | State | Frequentie | Correlatie met scenario | Beoordeling |
|---|---|---|---|---:|---|---|
| | | | | | | |

## 9. Statische audit

| Categorie | Kandidaten | Getriageerd | Relevant | False positive | Generated | Third-party |
|---|---:|---:|---:|---:|---:|---:|
| Sleep/SleepEx | | | | | | |
| Blocking waits | | | | | | |
| Synchrone file-I/O | | | | | | |
| Synchrone socket-I/O | | | | | | |
| SQL-calls | | | | | | |
| Oneindige/busy loops | | | | | | |
| Brede/brute-force scans | | | | | | |

### Repository-hygiëne

| Categorie | Aantal | Nodig voor actuele build | Actie | Bewijs |
|---|---:|---:|---|---|
| Legacy projectformaten | | | | |
| Generated compiler artifacts | | | | |
| Backupkopieën | | | | |
| Binaries in source | | | | |
| Archives in source | | | | |

### Grote translation units

| Bestand | Grootte | Buildimpact | Runtimeimpact bewezen | Module-eigenaarschap | Besluit |
|---|---:|---:|---:|---|---|
| | | | nee | | |

### Hoogste prioriteit

| Bestand:regel | Bewijs | Runtime-impact | Voorstel voor Fase 1 | Risico |
|---|---|---|---|---|
| | | | | |

## 10. Baselineconclusie

### Bevestigde bottlenecks

1.
2.
3.

### Nog onbewezen hypothesen

1.
2.
3.

### Dingen die bewust niet worden herschreven

1.
2.
3.

## 11. Besluit voor Fase 1

- [ ] Baseline is reproduceerbaar.
- [ ] Minstens één cold en drie warm runs zijn beschikbaar.
- [ ] Actieve executableketen, paden en hashes zijn bewezen.
- [ ] Readiness is per proces en voor de volledige keten meetbaar.
- [ ] Buildwarningbeleid is bekend en de warningbaseline is geldig.
- [ ] SQL-, netwerk- en runtime-hotspots zijn aan concrete code gekoppeld.
- [ ] Eerste wijziging heeft één meetbare hypothese.
- [ ] Rollback en regressietest zijn bepaald.

**Go / no-go:**  
**Eerste wijzigingshypothese:**  
**Verwachte metric:**  
**Acceptatiegrens:**  
**Regressiegrens:**
