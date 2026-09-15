# Titanium Flyff — canonical buildbaseline

**Datum:** 15 september 2026  
**Broncommit:** `1168ec257fcf28ccaaad38f1b537777678207f35`  
**Workflowrun:** `34968359925`  
**Runner:** `windows-2022`  
**Solution:** `Source/Source/all/all.sln`  
**Configuratie:** `Release|x64`

## Uitkomst

| Metric | Waarde |
|---|---:|
| MSBuild | `17.14.51.32402` |
| Duur | 24,707 s |
| Exitcode | `1` |
| Build geslaagd | **nee** |
| Door MSBuild gerapporteerde warnings | **6** |
| Door MSBuild gerapporteerde errors | **20** |
| Errorcode | `C1083` |
| Warningcode | `C4244` |

**Conclusie:** de canonieke `Release|x64`-solution is op dit moment **niet reproduceerbaar vanuit een schone GitHub-clone**. De audittooling werkte; de gamebuild zelf faalde.

Een groene workflowstatus mag daarom niet worden gelezen als “Titanium compileert”. De workflow heeft het falen bewust als baselinebewijs bewaard in plaats van de audittools daardoor onbruikbaar te verklaren.

## Primaire blocker: ontbrekende buildinputs

Alle twintig compilererrors zijn `C1083`-meldingen voor ontbrekende headers. Na aggregatie blijven vier unieke headernamen over:

| Ontbrekende header | Direct getroffen projecten | Bewijsklasse |
|---|---|---|
| `Lang.h` / `lang.h` | AccountServer, Certifier, CacheServer, LoginServer | clean-clone builderror |
| `Define.h` | DatabaseServer | clean-clone builderror |
| `defineItem.h` | WorldServer, Neuz | clean-clone builderror |
| `defineitemkind.h` | CoreServer | clean-clone builderror |

De compiler zoekt deze headers via zowel expliciete relatieve includes als project-includepaden die naar resource-directory’s wijzen. In de GitHub-clone ontbreken de verwachte resource-headerlocaties, waaronder kandidaten zoals:

- `Source/resource/`
- `Source/Source/Resource/`

De root-`.gitignore` sluit bovendien `Resource/` uit. Dat kan correct zijn voor grote runtime-assets, maar het is onverenigbaar met een reproduceerbare clean-clone build wanneer compile-time headers uit dezelfde genegeerde boom nodig zijn.

### Wat hier nog niet uit volgt

- Het is niet bewezen dat de headers verloren zijn; ze kunnen lokaal aanwezig of door een tool gegenereerd worden.
- Het is niet bewezen dat heel `Resource/` in Git hoort.
- Het is niet veilig om lege of geïmproviseerde headers te maken om de compiler kunstmatig verder te krijgen.
- Het is niet veilig om een lokale resourceboom blind te committen; die kan assets, geheimen, licentiegevoelige inhoud of gegenereerde ruis bevatten.

De juiste oplossing is eerst de **herkomst en generatieketen** van ieder vereist headerbestand vastleggen.

## Secundair signaal: x64-narrowing

MSBuild rapporteerde zes `C4244`-warnings. De zichtbare primaire signatuur is:

```text
conversion from 'SOCKET' to 'DPID', possible loss of data
```

De signatuur verscheen tijdens builds van onder meer:

- Certifier;
- LoginServer;
- CacheServer.

Dit is relevant voor de 64-bit-modernisering: een `SOCKET` is op Win64 pointer-sized, terwijl oudere `DPID`-achtige typen smaller kunnen zijn. Toch is dit nog **geen toestemming om blind te casten**. Eerst moet worden vastgesteld:

1. welk container- of pairtype de waarde bewaart;
2. of de code de socketwaarde werkelijk als ID gebruikt;
3. of truncatie bij hoge handlewaarden functioneel mogelijk is;
4. welke protocol- en lifecycletests deze wijziging moeten afdekken.

Bovendien staan in veel projectconfiguraties warnings uit. De zes zichtbare warnings zijn dus geen volledige warninginventaris.

## Besluit voor Fase 0

De buildbaseline is nu geldig als **mislukte clean-clone baseline**:

- solution en configuratie zijn concreet;
- toolchain en duur zijn vastgelegd;
- exitcode, warning- en erroraantallen zijn vastgelegd;
- de dominante blocker is herleid tot vier ontbrekende headerfamilies;
- er is nog geen productiesource aangepast.

## Vereiste vervolgstappen

1. Zoek op de lokale Titanium-machine naar de vier bevestigde prerequisites: `Lang.h`, `Define.h`, `defineItem.h` en `defineitemkind.h`.
2. Leg voor elk bestand het absolute pad, SHA-256, wijzigingsdatum en herkomst vast.
3. Bepaal of het bestand handgeschreven, gegenereerd of uit een resourcepakket afkomstig is.
4. Documenteer het commando of de tool die gegenereerde headers reproduceert.
5. Scheid compile-time headers van grote runtime-assets in `.gitignore` en repositorystructuur.
6. Herhaal dezelfde `Release|x64`-build op dezelfde toolchain.
7. Pas na een succesvolle clean build de warningbaseline gecontroleerd toe.

**Go/no-go:** no-go voor prestatieoptimalisaties of brede herschrijving; go voor herstel van de reproduceerbare buildketen.
