# Titanium Flyff — provenance van ontbrekende buildinputs

**Datum:** 15 september 2026  
**Auditbranch:** `chore/phase-0-baseline-audit`  
**Status:** repositorybewijs afgerond; lokale bron/generator van de headers nog onbewezen

## 1. Waarom deze audit nodig is

De canonieke `Release|x64`-build van `Source/Source/all/all.sln` faalt met twintig `C1083`-errors. Na aggregatie blijven vier unieke ontbrekende compile-time inputs over:

- `Lang.h`;
- `Define.h`;
- `defineItem.h`;
- `defineitemkind.h`.

Een bestand met dezelfde naam uit een willekeurige Flyff-source importeren is geen veilige oplossing. Deze headers kunnen versie-, resource- en Titanium-specifieke identifiers bevatten. Een verkeerde variant kan compileren en toch items, tekst-ID's, protocolwaarden of runtimegedrag verkeerd koppelen.

## 2. Statisch bevestigd in deze repository

### Geen huidige tracked kandidaat

De volledige Git-tree bevat geen tracked bestand met een van de vier vereiste bestandsnamen. Ook de verwachte paden bestaan niet in de huidige branch.

### Geen zichtbare historie op de verwachte paden

Voor de gecontroleerde varianten onder onder meer:

```text
Source/Resource/
Source/resource/
Source/Source/Resource/
Source/Source/resource/
```

levert de zichtbare Git-historie geen eerdere commit van deze headers op. Dit sluit een bestand buiten de huidige Git-historie niet uit, maar er is geen herstelbare versie in deze repository aangetoond.

### `Resource/` wordt globaal genegeerd

De root-`.gitignore` bevat een regel voor `Resource/`. Daardoor kunnen compile-time headers lokaal aanwezig zijn terwijl Git ze niet toont. Die regel kan correct zijn voor grote client-/runtime-assets, maar ze maakt een clean-clone build niet reproduceerbaar wanneer headers uit dezelfde boom nodig zijn.

De juiste correctie is niet om heel `Resource/` blind te committen. Eerst moeten compile-time headers, gegenereerde data en grote runtime-assets worden gescheiden.

## 3. Wat de legacy `.i`-bestanden wel en niet bewijzen

`Source/Source/AccountServer/AccountServer.i` bevat historisch preprocessorpadbewijs zoals:

```text
..\..\resource\..\Resource\Lang.h
```

Dat normaliseert op Windows naar een locatie onder `Source/Resource/Lang.h`. Dit is bruikbaar om de vroegere include-topologie te begrijpen.

Het bestand heeft Git-blob-SHA:

```text
ed61351d611d1b13a222be1dfa311b61267f408c
```

Dezelfde blob-SHA komt voor bij `Source/AccountServer/AccountServer.i` in de publieke oude clean-V15-repository `yColes/FlyFF`. De inhoud is dus byte-identiek en is geen overtuigend bewijs dat dit `.i`-bestand recent uit de Titanium-custom source is gegenereerd.

Een preprocessed `.i`-bestand is bovendien geen betrouwbare reconstructiebron voor de oorspronkelijke header:

- macrodefinities en conditionele structuur zijn al geëvalueerd;
- comments, include guards en generatorcontext kunnen verdwenen zijn;
- één `.i`-bestand toont slechts één set preprocessor-defines;
- Titanium-custom identifiers kunnen ontbreken wanneer het artifact oud is;
- dezelfde gedeclareerde symbolen bewijzen niet dezelfde numerieke waarden.

**Besluit:** `.i`-bestanden worden alleen als `legacy-preprocessed-reference` geregistreerd. Er worden geen headers uit teruggegenereerd.

## 4. Geautomatiseerde lokale resolver

Gebruik:

```powershell
.\tools\baseline\Resolve-BuildInputOrigins.ps1 `
  -ConfigPath .\tools\baseline\baseline.config.local.json `
  -AdditionalSearchRoot "<map-met-mogelijke-resourcebestanden>"
```

Er mogen meerdere expliciete zoekmappen worden opgegeven:

```powershell
.\tools\baseline\Resolve-BuildInputOrigins.ps1 `
  -AdditionalSearchRoot @(
    "<eerste-map>",
    "<tweede-map>"
  )
```

De resolver scant **niet automatisch volledige schijven**. Hij doorzoekt alleen:

1. de repository;
2. de kandidaatpaden uit de lokale configuratie;
3. expliciet meegegeven zoekmappen.

Hij kopieert of wijzigt geen bestanden. Per kandidaat registreert hij:

- bestandsnaam en geredigeerd pad;
- SHA-256;
- grootte en wijzigingsdatum;
- tracked/untracked-status;
- toepasselijke `.gitignore`-regel;
- zichtbare Git-pathhistorie;
- source- en projectreferenties;
- afzonderlijk gelabeld legacy-`.i`-padbewijs.

Absolute externe paden worden standaard vervangen door labels zoals `<external-root-1>`. Gebruik `-IncludeAbsoluteExternalPaths` alleen voor strikt lokale evidence die niet gedeeld wordt.

## 5. Gegenereerde output

De resolver schrijft onder `artifacts/baseline/build-input-origins/`:

- `build-input-origin-audit.json`;
- `build-input-candidates.csv`;
- `build-input-expected-paths.csv`;
- `build-input-references.csv`;
- `legacy-preprocessed-references.csv`;
- `BUILD-INPUT-ORIGINS.md`.

Deze map staat buiten versiebeheer. Deel geen kandidaatheader of absoluut pad zonder controle op geheimen, licentievoorwaarden en persoonsgegevens.

## 6. Acceptatiecriteria voor een kandidaat

Een gevonden bestand wordt pas als bruikbare buildinput beschouwd wanneer alle volgende punten zijn aangetoond:

1. **Herkomst** — bronpakket, generator of handmatige owner is bekend.
2. **Versie** — compatibiliteit met exact deze Titanium-commit is onderbouwd.
3. **Integriteit** — SHA-256 en wijzigingsdatum zijn vastgelegd.
4. **Semantiek** — belangrijke identifiers en numerieke waarden passen bij Titanium-data en code.
5. **Reproduceerbaarheid** — de generatie- of installatiestap kan vanaf een schone clone worden herhaald.
6. **Veiligheid** — geen credentials, machinepaden of niet-deelbare assets worden gecommit.
7. **Buildbewijs** — dezelfde `Release|x64`-build gaat aantoonbaar verder of slaagt.
8. **Functioneel bewijs** — login, world load, items/resources en relevante custom systemen doorstaan smoke tests.

Een naamsmatch of succesvolle compilatie alleen is onvoldoende.

## 7. Besluit

De ontbrekende headers zijn nu geclassificeerd als **externe of gegenereerde buildprerequisites met onbewezen provenance**. De eerstvolgende technische stap is ze op de echte Titanium-machine lokaliseren en via de resolver een metadata-/hashmanifest maken. Pas daarna kan worden beslist of ze:

- gegenereerd moeten worden;
- als kleine compile-time headers gecontroleerd onder versiebeheer horen;
- uit een apart dependency/resourcepakket moeten komen;
- of door een gedocumenteerde bootstrapstap moeten worden geleverd.

**No-go:** lege placeholderheaders, willekeurige V15/V19-headers importeren of heel `Resource/` blind unignoren.
