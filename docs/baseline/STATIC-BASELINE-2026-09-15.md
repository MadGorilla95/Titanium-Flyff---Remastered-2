# Titanium Flyff — statische baseline

**Datum:** 15 september 2026  
**Bronrevisie:** `main@c310e1a866e578b8411acdcde3ee45c6421d1128`  
**Auditbranch:** `chore/phase-0-baseline-audit`  
**Status:** statische inventaris voltooid; runtime- en prestatiebaseline nog niet voltooid

## 1. Reikwijdte en bewijskracht

Dit document beschrijft wat zonder draaiende Titanium-omgeving controleerbaar is in de repository. Er is nog geen geldige claim mogelijk over werkelijke startup-, login-, world-load-, SQL-, netwerk- of memoryprestaties.

De bevindingen gebruiken drie bewijsklassen:

| Klasse | Betekenis |
|---|---|
| **Statisch bevestigd** | Bestand, projectinstelling, launchercommando of binary bestaat aantoonbaar in Git. |
| **Sterke statische aanwijzing** | Meerdere statische bronnen wijzen naar hetzelfde component, maar runtimegebruik is nog niet geobserveerd. |
| **Runtime onbewezen** | Werkelijke uitvoering, bereikbaarheid, readiness en impact moeten op de Titanium-machine worden gemeten. |

Een gevonden feature, groot bronbestand, `Sleep()`-aanroep of binary is geen automatisch bewijs van actief gebruik of slechte performance.

## 2. Project- en componentstructuur

Onder `Source/Source` staan meerdere categorieën door elkaar:

### Productieserverkandidaten

- `AccountServer`
- `CACHESERVER`
- `CERTIFIER`
- `CORESERVER`
- `LOGINSERVER`
- `WORLDSERVER`
- DatabaseServer-gerelateerde code onder de databasecomponenten

### Client, launchers en tools

- `Neuz`
- `NeuzNoGG`
- `Launcher`
- `PatchClient`
- `Accumulator`
- `WORLDDIALOG`
- `WINDMESSENGER`
- `atools`
- rootmap `Patcher`

### Gedeelde interne lagen

- `_Common`
- `_Database`
- `_DirectX`
- `_Network`
- `_Sound`
- `resource`
- `Script`

### Meegeleverde of legacy dependencies

Onder andere:

- `DX_SDK`
- `DirectX`
- `CrashRpt`
- `Discord`
- `Lib` en `Lib64`
- `MySQL`
- `WAPI_AIL`
- `crypto`
- `json`
- `lua`
- `zlib`
- `libcrc`
- `libcrux`

**Auditbesluit:** tellingen moeten per componentklasse worden uitgesplitst. Een globale telling van alle `.cpp`-, `Sleep()`- of binarybestanden zou anders productiecode, clientcode, tools en third-partycode vermengen.

## 3. Statische executableketen

`-_Move_Compiles.bat` koppelt de volgende `Release64`-buildoutputs aan de deploymentnamen:

| Volgorde | Buildoutput | Deploymentnaam | Statische rol |
|---:|---|---|---|
| 1 | `AccountServer.exe` | `1. Account-x64.exe` | accountserver |
| 2 | `Certifier.exe` | `2. Certifier-x64.exe` | certifier |
| 3 | `DatabaseServer.exe` | `3. Database-x64.exe` | databaseserver |
| 4 | `CoreServer.exe` | `4. Core-x64.exe` | coreserver |
| 5 | `LoginServer.exe` | `5. Login-x64.exe` | loginserver |
| 6 | `CacheServer.exe` | `6. Cache-x64.exe` | cacheserver |
| 7 | `WorldServer.exe` | `7. World-x64.exe` | worldserver |
| client | `Neuz.exe` | `Neuz-x64.exe` | gameclient |

De map `ImmortalProgram` bevat de zeven genoemde x64-serverbinaries. Dat is sterk statisch bewijs voor de beoogde deploymentketen, maar nog geen runtimebewijs.

### Vereist runtimebewijs

Tijdens een normale start moeten minimaal worden vastgelegd:

- exact procespad;
- PID;
- bestandshash en bestandsversie;
- starttijd;
- luisterende poorten;
- succesvolle afhankelijkheidsverbindingen;
- expliciete ready-marker per proces.

Zonder deze gegevens is niet bewezen dat de binaries in `ImmortalProgram` gelijk zijn aan de laatst gebouwde outputs of dat alle zeven processen werkelijk nodig zijn.

## 4. Launcher-topologie en kunstmatige wachttijd

Er bestaan afzonderlijke 32-bit- en 64-bit-startbestanden:

- `ImmortalProgram/- Server Starter.bat`
- `ImmortalProgram/- Server Starter64.bat`

Beide starten dezelfde zeven rollen in dezelfde volgorde. De x64-launcher bevat deze vaste timing:

| Start | Scriptvertraging sinds vorige start | Cumulatief vóór start |
|---|---:|---:|
| Account | 0 s | 0 s |
| Certifier | 1 s | 1 s |
| Database | 1 s | 2 s |
| Core | 2 s | 4 s |
| Login | 2 s | 6 s |
| Cache | 1 s | 7 s |
| World | 1 s | 8 s |

Na het starten van World volgt nog één seconde `timeout`. De launcher bereikt de pauzestatus daardoor na minimaal ongeveer **9 seconden**, exclusief procescreatie en schedulerafwijkingen.

### Kritische interpretatie

- De **8 seconden vóór de World-start** zijn launcher-overhead.
- De **9 seconden tot de pauzestatus** zijn geen gemeten serverstartup.
- De launchers controleren geen health endpoint, poort, dependency handshake of log-ready-marker.
- Een vaste timeout kan zowel te lang als te kort zijn.
- Startupmetingen moeten minimaal twee waarden rapporteren:
  1. orchestrator-/launcherduur;
  2. daadwerkelijke tijd tot aantoonbare readiness van elk proces en van de volledige keten.

`Get-StartupTopology.ps1` legt deze scheiding automatisch vast.

## 5. Buildconfiguratie en warningbeleid

De gecontroleerde `AccountServer.vcxproj` en `CacheServer.vcxproj` bevatten:

- `Debug|Win32`;
- `Debug|x64`;
- `Release|Win32`;
- `Release|x64`;
- `PlatformToolset` `v143`;
- Windows SDK `10.0`;
- afzonderlijke `Release64`-outputpaden voor x64.

In meerdere gecontroleerde configuratieblokken staat:

```xml
<WarningLevel>TurnOffAllWarnings</WarningLevel>
```

Dit is een **hoog auditrisico**. Het betekent niet automatisch dat de code fout is, maar wel dat de huidige build geen betrouwbare warningbaseline kan leveren zolang warnings voor relevante configuraties zijn uitgeschakeld.

### Correcte volgorde

1. Eerst per project en configuratie inventariseren welk warningbeleid werkelijk actief is.
2. Een ongewijzigde build uitvoeren en loggen.
3. Warnings gecontroleerd inschakelen zonder ze onmiddellijk als errors te behandelen.
4. Warnings classificeren op code, project, owner en risico.
5. Pas daarna beslissen welke warningniveaus naar `/WX` kunnen evolueren.

Blind `TurnOffAllWarnings` vervangen door `/W4 /WX` in de volledige legacy-source zou waarschijnlijk ruis en buildbreuk veroorzaken en is daarom geen veilige Fase-0-wijziging.

`Get-BuildPolicyAudit.ps1` genereert voortaan een matrix in `project-build-policies.csv`.

## 6. Repository-hygiëne en legacylagen

De source bevat naast moderne `.vcxproj`-bestanden ook oudere project- en IDE-formaten, waaronder:

- `.dsp`;
- `.dsw`;
- `.vcproj`;
- `.plg`;
- preprocessed `.i`-bestanden;
- backupbestanden zoals `.bak`;
- binaries en libraries in sourcepaden.

Deze bestanden zijn niet per definitie overbodig. Ze verhogen wel het risico op:

- dubbele of tegenstrijdige projectdefinities;
- analyse van gegenereerde code alsof het handgeschreven source is;
- verwarring over de canonieke build;
- grotere repository en trager zoeken/indexeren;
- onbedoelde koppeling aan verouderde binaries.

De audit verwijdert niets. Eerst moet per artifact worden bepaald of het:

- nodig is voor de actuele build;
- alleen historisch is;
- reproduceerbaar gegenereerd kan worden;
- naar release-artifacts of externe dependencies moet verhuizen.

## 7. Grote translation units

Voorlopige statische voorbeelden in `WORLDSERVER`:

| Bestand | Omvang in repository | Interpretatie |
|---|---:|---|
| `DPSrvr.cpp` | circa 609.615 bytes | reviewkandidaat |
| `User.cpp` | circa 316.009 bytes | reviewkandidaat |
| `DPDatabaseClient.cpp` | circa 147.463 bytes | reviewkandidaat; daarnaast bestaat een backupkopie |
| `DPCoreClient.cpp` | circa 108.161 bytes | reviewkandidaat |

Bestandsgrootte bewijst geen runtimebottleneck. Grote translation units kunnen wel wijzen op:

- lange compiletijden;
- sterke koppeling en lage modulecohesie;
- moeilijkere code-review;
- grotere regressieradius;
- vermenging van netwerk-, gameplay- en persistenceverantwoordelijkheden.

`large-translation-units.csv` maakt de volledige lijst reproduceerbaar. Refactoring mag pas volgen na build- en runtimebewijs.

## 8. Custom feature-signalen

De repository bevat statische signalen voor onder meer:

- achievements;
- badges/titles;
- guild finder;
- guildsystemen;
- world bosses;
- couple/marriage;
- lord/sovereign;
- minigames;
- ranking;
- pet- en dungeon/instancefunctionaliteit;
- launcher/patcher;
- Discord-integratie.

Dit is precies waarom een generieke Flyff-modernisering niet blind mag worden toegepast. Een naam, SQL-migratie of klasse bewijst echter niet dat een systeem compleet, bereikbaar of actief is. Elk feature-signaal krijgt voorlopig de status `unverified-static-signal`.

## 9. Status van de gevraagde baseline

| Onderdeel | Status | Wat nog ontbreekt |
|---|---|---|
| Projectstructuur | **statisch in kaart gebracht** | gegenereerde inventaris op de Windows-clone bewaren |
| Beoogde executableketen | **sterk statisch bewijs** | normale start observeren en hashes/paden vastleggen |
| Bestaande features | **signalen geïnventariseerd** | runtimebereikbaarheid en volledigheid testen |
| Warningbeleid | **risico bevestigd** | volledige matrix en echte buildlog |
| Build warnings/errors | **niet gemeten** | exacte solution kiezen en cold/warm builds uitvoeren |
| Launcher-overhead | **statisch bepaald** | werkelijke proces- en readinessduur meten |
| Login time | **niet gemeten** | start/eindmarker plus correlatie-ID configureren |
| World load time | **niet gemeten** | world/channelmarkers configureren |
| SQL-querybelasting | **niet gemeten** | Extended Events onder gecontroleerd scenario |
| Network bottlenecks | **niet gemeten** | counters, verbindingen en workloadcorrelatie |
| Memory/leaks | **niet gemeten** | soaktest en native profilerbewijs |
| `Sleep()`/blocking/brute-force | **scanner aanwezig** | resultaten handmatig triageren en runtime koppelen |

## 10. Huidige risicohypothesen

Deze punten verdienen onderzoek, maar zijn nog geen bevestigde bottlenecks:

1. Vaste launchervertraging verbergt dependency- en readinessproblemen.
2. Uitgeschakelde compilerwarnings kunnen defecten en 32/64-bit-migratierisico’s maskeren.
3. Parallel aanwezige oude en moderne projectbestanden kunnen de canonieke build onduidelijk maken.
4. Gegenereerde, backup- en binarybestanden in sourcepaden kunnen analyse en onderhoud vervuilen.
5. Grote WorldServer-translation-units kunnen buildtijd, koppeling en regressierisico verhogen.
6. Custom systemen vergroten het risico dat een generieke upstreamfix gedrag breekt.

## 11. Go/no-go

**No-go voor brede herschrijving of optimalisatie.**

**Go voor gecontroleerde Fase-0-metingen**, in deze volgorde:

1. branch lokaal uitchecken;
2. standaard statische audit uitvoeren;
3. exacte solution en gebruikte x64-configuratie vastleggen;
4. buildwarningbaseline verzamelen;
5. normale serverstart observeren en readinessmarkers bepalen;
6. één cold run en minimaal drie identieke warm runs uitvoeren;
7. login- en world-loadscenario serialiseren;
8. SQL, netwerk en procesresources tijdens hetzelfde scenario vastleggen;
9. statische kandidaten koppelen aan gemeten impact;
10. pas daarna één afgebakende Fase-1-hypothese kiezen.
