# Build-chain audit

`Get-BuildChain.ps1` bepaalt welke moderne Visual Studio-projectconfiguratie de binaries uit de packaging-flow kan produceren. Het script wijzigt of bouwt niets.

## Waarom deze stap nodig is

Titanium bevat parallel meerdere generaties Visual Studio-metadata. Een aanwezige `.sln` is daarom niet automatisch de juiste build entry point.

Statisch geverifieerd voor minstens `AccountServer` en `Certifier`:

- de `*71.sln`-bestanden zijn **Visual Studio solution format 8.00**;
- ze verwijzen naar legacy `.vcproj`-bestanden;
- hun solutionconfiguraties mappen `Release` naar `Release|Win32`;
- de moderne `.vcxproj`-bestanden bevatten daarnaast `Release|x64`;
- de moderne projecten schrijven x64-output naar `Source\Output\<component>\Release64\`;
- `-_Move_Compiles.bat` kopieert juist die `Release64`-outputs naar de deploymentnamen.

Daarom mogen de oude `*71.sln`-bestanden niet zonder bewijs als canonieke x64-build worden gebruikt. De volledige repositoryscan moet uitwijzen of er een moderne overkoepelende solution bestaat, of dat de zeven servers en `Neuz` afzonderlijk worden gebouwd.

## Uitvoeren

```powershell
.\tools\baseline\Get-BuildChain.ps1 `
  -RepositoryRoot . `
  -SourceRoot Source\Source `
  -Configuration Release `
  -Platform x64
```

Na integratie in de normale Fase-0-run verschijnt deze audit als stage `build-chain`.

## Gegenereerde bestanden

```text
artifacts\baseline\<run>\build-chain\
  build-chain.json
  build-chain.md
  project-outputs.csv
  deployment-map.csv
  solution-coverage.csv
```

### `project-outputs.csv`

Per moderne `.vcxproj`:

- aanwezigheid van `Release|x64`;
- `ConfigurationType`;
- toolset en Windows SDK;
- `OutDir`;
- expliciete linkeroutput;
- berekend repositorypad van de binary;
- XML-parsefouten.

### `deployment-map.csv`

Per gevonden `COPY`/`XCOPY` van een `.exe`:

- packaging-script en regelnummer;
- bron- en doelpad;
- gekoppelde moderne projectoutput;
- matchkwaliteit;
- ambiguïteit;
- solutions die het project direct of via een legacy sibling refereren;
- reproduceerbaar per-project `msbuild`-commando.

### `solution-coverage.csv`

Per solution:

- solution format;
- exacte ondersteuning voor de gevraagde configuratie;
- aantal legacy `.vcproj`- en moderne `.vcxproj`-referenties;
- dekking van de daadwerkelijk gekopieerde deploymentoutputs;
- status van de kandidaat-buildroute.

## Matchkwaliteiten

| Waarde | Betekenis |
|---|---|
| `exact-output-path` | Packagingbron en berekende projectoutput zijn exact hetzelfde pad. |
| `unique-output-filename` | Alleen de bestandsnaam matcht, maar slechts één project heeft die outputnaam. |
| `ambiguous-*` | Meer dan één project is mogelijk; handmatige bevestiging vereist. |
| `unmatched` | Geen moderne projectoutput kon aan de packagingbron worden gekoppeld. |

Alleen `exact-output-path` is sterke statische koppeling. Ook die bewijst nog niet dat dit project in de echte buildprocedure wordt gebruikt.

## Canonieke status

| Status | Interpretatie |
|---|---|
| `strong-candidate-not-runtime-confirmed` | Eén moderne solution dekt de deploymentprojecten en heeft exact de gevraagde configuratie. Een clean build en runtimehash moeten dit nog bevestigen. |
| `coverage-only-not-valid-for-requested-modern-build` | De solution dekt de projecten, maar gebruikt legacy referenties of mist de exacte x64-configuratie. |
| `partial-component-solution` | De solution bouwt slechts een deel van de deploymentketen. |
| `unrelated-to-deployment-copy-chain` | Geen koppeling met de gekopieerde server/clientoutputs. |

## Bewijsvolgorde

De buildroute is pas bewezen wanneer alle stappen kloppen:

1. `Get-BuildChain.ps1` koppelt packagingbron aan een exacte moderne projectoutput.
2. Een clean `Release|x64`-build van dat project of die solution slaagt.
3. De geproduceerde binary staat op het verwachte `Release64`-pad.
4. Packaging kopieert exact die binary.
5. `Get-RuntimeTopology.ps1` ziet het gedeployde proces.
6. De runtime-SHA-256 matcht de zojuist gebouwde/gekopieerde binary.

Zonder deze keten blijft “dit is de juiste solution” een aanname.
