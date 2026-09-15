# Titanium Flyff — baseline-rapport

> Vul dit rapport pas in na een gecontroleerde run. Bewaar ruwe meetbestanden in `artifacts/baseline/`; commit alleen resultaten die geen geheimen, accountgegevens, IP-adressen of persoonsgegevens bevatten.

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
| Visual Studio / MSBuild | |
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

### Actieve executableketen

| Volgorde | Executable | Project | Startbewijs | Runtimebewijs | Functie | Status |
|---:|---|---|---|---|---|---|
| 1 | | | | | | |

### Aanwezige maar niet-bewezen projecten

| Project/executable | Bewijs in source | Waarom mogelijk legacy | Besluit |
|---|---|---|---|
| | | | |

### Custom feature-inventaris

| Feature | Pad/codebewijs | Runtime getest | Compleet | Opmerking |
|---|---|---:|---:|---|
| | | nee | onbekend | |

## 4. Buildbaseline

| Metric | Cold build | Warm build 1 | Warm build 2 | Warm build 3 | Mediaan |
|---|---:|---:|---:|---:|---:|
| Totale duur (s) | | | | | |
| Warnings | | | | | |
| Errors | | | | | |

### Belangrijkste warnings/errors

| Code | Aantal | Project/bestand | Risico | Besluit |
|---|---:|---|---|---|
| | | | | |

## 5. Runtime-timings

### Serverstartup

| Run | Startmarker | Eindmarker | Duur (ms) | Geldig |
|---:|---|---|---:|---:|
| 1 | | | | |

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

Let op:

- vergelijk cold-cache en warm-cache niet als één dataset;
- beoordeel totaalwerk én latency per call;
- een snelle query die extreem vaak wordt aangeroepen kan belangrijker zijn dan één trage query;
- controleer parameterisatie en execution plans voordat indexen worden toegevoegd.

## 7. Procesresources

| Proces/PID | Avg CPU % | P95 CPU % | Max CPU % | Start private MB | Eind private MB | Groei MB/min | Max handles | Max threads |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| | | | | | | | | |

### Leakonderzoek

| Signaal | Reproduceerbaar | Profilerbewijs | Vermoedelijke owner | Besluit |
|---|---:|---:|---|---|
| Private memory stijgt | | | | |
| Handles stijgen | | | | |
| Threads stijgen | | | | |

## 8. Netwerkbaseline

| Metric | P50 | P95 | Max | Opmerking |
|---|---:|---:|---:|---|
| Bytes total/sec | | | | |
| TCP retransmits/sec | | | | |
| Established connections | | | | |
| SYN_SENT / CLOSE_WAIT / TIME_WAIT | | | | |
| Interface errors | | | | |

### Verdachte flows

| Proces | Local endpoint | Remote endpoint | State | Frequentie | Beoordeling |
|---|---|---|---|---:|---|
| | | | | | |

## 9. Statische audit

| Categorie | Kandidaten | Getriageerd | Relevant | False positive | Third-party |
|---|---:|---:|---:|---:|---:|
| Sleep/SleepEx | | | | | |
| Blocking waits | | | | | |
| Synchrone file-I/O | | | | | |
| Synchrone socket-I/O | | | | | |
| SQL-calls | | | | | |
| Oneindige/busy loops | | | | | |
| Brede/brute-force scans | | | | | |

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
- [ ] Minstens drie warm runs zijn beschikbaar.
- [ ] Actieve executableketen is bewezen.
- [ ] SQL- en runtime-hotspots zijn aan concrete code gekoppeld.
- [ ] Eerste wijziging heeft één meetbare hypothese.
- [ ] Rollback en regressietest zijn bepaald.

**Go / no-go:**
**Eerste wijzigingshypothese:**
**Verwachte metric:**
**Acceptatiegrens:**
**Regressiegrens:**
