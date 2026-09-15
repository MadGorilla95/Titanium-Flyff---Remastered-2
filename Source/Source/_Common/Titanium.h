//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																													//	
//        ████████╗██╗████████╗ █████╗ ███╗   ██╗██╗██╗   ██╗███╗   ███╗    ███╗   ███╗███╗   ███╗ ██████╗			//
//        ╚══██╔══╝██║╚══██╔══╝██╔══██╗████╗  ██║██║██║   ██║████╗ ████║    ████╗ ████║████╗ ████║██╔═══██╗			//
//           ██║   ██║   ██║   ███████║██╔██╗ ██║██║██║   ██║██╔████╔██║    ██╔████╔██║██╔████╔██║██║   ██║			//
//           ██║   ██║   ██║   ██╔══██║██║╚██╗██║██║██║   ██║██║╚██╔╝██║    ██║╚██╔╝██║██║╚██╔╝██║██║   ██║			//
//           ██║   ██║   ██║   ██║  ██║██║ ╚████║██║╚██████╔╝██║ ╚═╝ ██║    ██║ ╚═╝ ██║██║ ╚═╝ ██║╚██████╔╝			//
//           ╚═╝   ╚═╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝ ╚═════╝ ╚═╝     ╚═╝    ╚═╝     ╚═╝╚═╝     ╚═╝ ╚═════╝			//
//																													//                                                                   
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/****************************************************** CONFIGURATION *******************************************************/

//------------------------//
#define __LOCALSERVER
//------------------------//

#ifdef __LOCALSERVER
#define NEUZ_IPSET			"127.0.0.1"						//| Server Local IP -> 127.0.0.1 / Localhost				[OK]
#define NEUZ_BHASH			""								//| Server Neuz Hash -> Empty for Development				[OK]
#define AZURIOM_WEBSITE		"127.0.0.1"						//| azuriom website											[OK]
#endif //__LOCALSERVER

#ifdef __TESTSERVER
#define NEUZ_IPSET			"161.97.150.1"					//| Testserver Public IP									[OK]
#define NEUZ_BHASH			"TitClient!123"					//| Server Neuz Hash										[OK]
#define AZURIOM_WEBSITE		"161.97.149.219"				//| azuriom website											[OK]
#endif //__TESTSERVER

#ifdef __LIVESERVER
#define NEUZ_IPSET			"161.97.150.1"					//| Server Public IP										[OK]
#define NEUZ_BHASH			""					//| Server Neuz Hash										[OK]
#define AZURIOM_WEBSITE		"161.97.150.1"				//| azuriom website											[OK]
#endif //__LIVESERVER

/*************************************************** CLIENT CONFIGURATION ***************************************************/

#define NEUZ_TITLE			"Titanium MMO"					//|	Neuz Title												[OK]
#define NEUZ_PHASH			""				//|	Password Hash > Database & Website						[OK]
#define NEUZ_MSGVR			"20100412"						//|	Neuz Msg Version > Matches Patcher						[OK]
#define __AES												//| AES Encryption for Client								[OK]
#define __DISCORD_RPC										//| Discord RPC Integration									[OK]
#define __fmod
#define __dx9c	// using 9.0c libs.
#define D3DXFX_LARGEADDRESS_HANDLE

//#define __DEBUG_NEUZ										//| Check every model, texture, terrain, icons client side.	[OK]

/**************************************************** PORT CONFIGURATION ****************************************************/

#define NEUZ_CPORT				"5400"						//| Titanium Neuz Port										[OK]
#define SERVER_CERT_PORT		23000						//| Titanium Certifier Port									[OK]
#define SERVER_LOGIN_PORT		28000						//| Titanium Login Port										[OK]

/********************************************************* VARIABLES *********************************************************/

#define __MAX_STACK				9999						//| VARIABLE: Max Item Stack								[OK]
#define __PET_SPEED				4.5f						//| VARIABLE: Pet Speed	Multiplier							[OK]
#define __TELEPORT_DELAY		5							//| VARIABLE: Teleport delay on on apps.					[OK]
#define __MAX_SPEED				200000							//| VARIABLE: Max walking speed.							[OK]
//#define MOD_MAX_SPEED			220
#define INGAMEDP_CASH_ERROR		1							//| __AEGON_INGAME_DP										[OK]
#define INGAMEDP_BUYLOOTBOX		2							//| __AEGON_INGAME_DP										[OK]
#define INGAMEDP_BUYSEASONPASS	3							//| __AEGON_INGAME_DP										[OK]
#define INGAMEDP_BUYMOTION		4							//| __AEGON_INGAME_DP										[OK]
#define INGAMEDP_BUYEMOTE		5							//| __AEGON_INGAME_DP										[OK]
#define INGAMEDP_WEBSHOP		6							//| __AEGON_INGAME_DP										[OK]

#define GAMEGUARD_LOG_STAT		15500							//| VARIABLE: Character Stat Tracking.						[OK]
#define GAMEGUARD_LOG_ISTAT		19000						//| VARIABLE: Character Individual Stat Tracking.			[OK]
#define GAMEGUARD_LOG_TSTAT		20000						//| VARIABLE: Character Total Stats Tracking.				[OK]

#define __AEGON_PICKUP_PET_RANGE	20						//| range the pickup pet walks to items						[OK]

//#define __SYS_STACKABLE_BEADS	43200						//| VARIABLE: Stackable Beads QOL in Minutes.				[OK]

#define AUTH_NINJA_MODE		AUTH_GAMEMASTER					//| auth needed for ninja mode

#define __AEGON_LOGIN_CRYPT	"w9z$C&F)J@NcRfUj"				//| Crypt Key for user logins								[OK]

#define __AEGON_OFFSET_PLAYER_NAME_Y	-10					//| Offset player name, guild name/logo, hp bar, etc		[OK]

#define __BUYBACK_PERCENT_COST	10							//| Increased buyback cost									[OK]

#define __RIDE_ACTIVATION_TIME	500							//| set ride/mount channel time	overrides spec_item			[OK]

#define __MAX_COLLECT_BY_IP		5							//| Max collectors per ip address							[OK]	

#define __AEGON_FLY_LEVEL	1								//| Level required to use board/mount						[OK]

#define __MAX_ATEX 8										//| additional textures per model							[OK]

/********************************************************* AZURIOM **********************************************************/
#ifndef __LOCALSERVER
#define __AZURIOM_IMAGES									//| Azuriom char images										[OK]
#define __AEGON_INGAME_DP_AZURIOM							//| azuriom ingame dp (extends __AEGON_INGAME_DP)			[OK]
#define __AEGON_INGAME_SHOP									//| Ingame shop reading items from azuriom db				[OK]
#endif // !__LOCALSERVER

#define AZURIOM_DB				"azuriom"					//| This is the DB you created for the CMS					[OK]
#define AZURIOM_WEBSITE_PORT	80							//| azuriom website port									[OK]
#define __AZURIOM_INGAME_SHOP								//| Ingame Webshop											[OK]

/********************************************************* SYSTEMS **********************************************************/
//#define __PLAYER_SPEED
#define	__ANGEL_CREATION_FIX								//| Angel Creation Fix										[OK]
#define __ARROW_FIX											//| Arrow Positioning Fix									[OK]
#define	__FAST_NEUZ											//| Quicker loading of world maps							[OK]
#define	__INVENTORY_168										//| 168 Slots Inventory										[OK]
#define	__NEW_EXCHANGE_V19									//| V19 Exchanging											[OK]
#define	__V21_SIEGE_BUFFS									//| Buffs upon playing Siege								[OK]
#define	__V16_FORMATS										//| V16 Props												[OK]
#define	__V19_FORMATS										//| V19 Props												[OK]
#define	__ZONE_AFFICHAGE									//| Region Text on left side of screen						[OK]
#define	__INVENTORY_SORT									//| Sorts items depending per Item-ID DESC					[OK]
#define	__TREASUREBOX										//| Treasure Box System	Fixed								[OK]

#define	__SYS_SCR_CANCELLATION								//| Scroll of Unbinding										[DEV]
#define	__SYS_SCR_PARTYPOINTFREE							//| Scroll of Party Amplification							[DEV]
#define	__SYS_SCR_PARTYSPEEDLEVELUP							//| Scroll of Party Growth									[DEV]
#define	__SYS_SYS_ARMOREFFECTCHANGE							//| Aura Scrolls											[DEV]
#define	__SYS_SYS_ITEMMODELRETURN							//| Revert Transmutation Scroll								[DEV]
#define	__V21_PREMIUM_BATTERY								//| Collect Premium Items									[DEV]
#define	__YS_CHATTING_BLOCKING_SYSTEM						//| Spam Blocking System									[DEV]

#define	__3RD_LEGEND16										//| 3rd Jobs												[OK]
#define	__COLOSSEUM						        	        //| Colosseum System										[OK]
#define	__FASHION_COMBINE				        			//| Fashion Combination System								[OK]
#define	__IMPROVE_MAP_SYSTEM			        		    //| Improved Map System										[OK]
#define	__INSTANCE_AGGRO_SYSTEM			        		    //| V17 Aggro System										[OK]
#define	__ITEMLINK						        	        //| Item Linking											[OK]
#define	__LECTURE_V17					        	    	//| V17 WndTitle Edits										[OK]
#define	__LOADING_SCREEN_UPDATE			        	    	//| New Loading Screens										[OK]

#define	__NEW_SKILL_TREE				        			//| New Skill Tree											[OK]
#define	__NEW_STACKABLE_AMPS			        			//| Stackable Amps from 3 to 5.								[OK]
#define	__NEW_WEAPON_GLOW				        			//| V17 Weapon Glows										[OK]
#define	__NO_AFTER_SKIN					        		    //| No After Skin on Textures.								[OK]
#define	__NO_SUB_LANG					        	    	//| No second language.										[OK]
#define	__UPGRADE_SUCCESS_SCROLL		        			//| Scroll IK3_ULTIMATE_UPGRADE_RATE added.					[OK]
#define	__WND_BUTTONS					        	    	//| New Window Buttons.										[OK]

/************************************************* TITANIUM MMO SYSTEMS **************************************************/

#define __TITANIUM_TASKBAR									//| Taskbar Modifications									[OK]
#define __TITANIUM_OPTIONS									//| Extended Options Menu for Titanium						[OK]
#define __TITANIUM_THEME									//| Titanium MMO Source Theme Tweaks						[OK]
#define __TITANIUM_MAPS										//| Maps													[OK]

#define __CUSTOM_HP_BAR										//| Customized HP Bar										[OK]
#define __BETTER_MODEL_CACHE								//| Better Model Caching									[OK]
#define __SYS_LOCK_ITEM										//| Lock Item System										[OK]
#define __GAMEGUARD											//| Security Systems										[OK]
#define __PREMIUM											//| Premium on SM Mode										[OK]
#define __SYS_NO_SHOP_SFX									//| Disabled SFX when opening a private shop				[OK]
#define __SYS_PET_NO_CLICK									//| Click Trough Pets										[OK]
//#define __SYS_FLYING_PETS									//| You can fly with pets enabled now!						[OK]
#define __NEW_NIGHT											//| New Day / Night System									[OK]
#define __PARTY_ASSISTANT									//| Party Assistance System									[OK]
#define __NPCVEND_STACKBUY									//| Stack Purchase system									[OK]
#define __BETTER_SKILL_TOOLTIP								//| Better Skill Tooltip System								[OK]
#define __VENDING_NPC										//| Offline Vendor											[OK]
#define __SYS_GROUND_ITEM_TEXT								//| Ground Items Text Overlay								[OK]
#define __GIFTBOX_LIST_ITEMS								//| List Gifbox Items										[OK]
#define __ANARCHY_SYSTEM									//| Anarchy System											[OK]
#define __BADGE												//| Badge System with SQL saving badges © -					[OK]
#define	__SYS_GUILDCOLOR									//|	Changes Guild Color via Scroll							[OK]
#define __SWITCH_DMG_RENDER									//| Option to Switch Damage Rendering						[OK]
#define __GUILDFINDER										//| Guild Finder with SQL									[OK]
#define __SYS_MONSTER_CLASH									//| Monster Clash System									[OK]
#define __TASKBAR_OPEN_SITE									//| Added multiple URL's in Taskbar.						[OK]
#define __SYS_SANDBOX										//| Sandbox System											[OK]
#define __SHOPS_VISITED										//| Indicator of visited vendors.							[OK]
#define	__TICKETS_DETAILS									//| Information of connected players on channels			[OK]
#define __CHECKLOAD 										//| Check Load Ticket - Requires V18 Ticket.				[OK]
#define __CHANNELSWITCHER 									//| Channel Switch Ticket - Requires V18 Ticket .			[OK]
#define __V18TICKET 										//| New Ticket Loader & Files.								[OK]
#define __ACCELOGIC											//| Jewelry Improvement										[OK]
#define __NEW_GUILD_PERM									//| New Guild Rights (Apply for Siege) on Guild Tab			[OK]
#define __DMGCORRECT										//| Tweaked min/max dmg | Removed miss | Block DMG Tweak	[OK]
#define	__TRAILS											//| Custom Trails											[OK]
#define __NO_PARTYSKILL_DELETE								//| Party Skills will not be deleted on party leader switch	[OK]
#define	__SPAM_PROTECT_SKILLS								//| Disabling skill in Safety Regions						[OK]
#define __AFK_MONITORING									//| Automatic AFK in 15 minutes								[OK] 
#define	__SYS_NAMECOLOR										//|	Changes Player Name Color via Scroll					[OK]
#define	__STATISTICS										//| Improved display of stats (H)							[OK]
#define __WEAPON_MERGE										//| Weapon Merge System										[OK]
#define	__EATPET_STATS										//| Bonuses at buff-pets									[OK]
#define __SINGLE_TASKBAR_SWAP								//| Switch between F Keys									[OK]
#define __NPC_BUFF_CLASS									//| NPC Buff of Class Skills								[OK]
#define	__WINDOW_TITLE										//| Title name in Window.									[OK]
#define	__ADDSHOPITEM										//| AddShopItem NPC function.								[OK]
#define __NO_TICKET_RESELL									//| Sell expired or active tickets for 1 penya				[OK]
#define	__ADDTEXT_COLORED									//| dwColor in AddText function								[OK]
#define __ITEM_COUNT_TOOLTIP								//| Item count in Tooltip								    [OK]
#define __STAFF_ITEM_IDANDIK_TOOLTIP						//| Display Item ID, IK1/2/3 as Tooltip					    [OK]
#define __CLIENT_LOAD_MIDDLE								//| Load Window in Middle of Screen							[OK]
#define __OPTIMIZE_LATENCY									//|	Client's latency optimized for positioning				[OK]
#define	__VENDOR_UPDATE										//| NPC Shop Update.										[OK]
#define	__CHANGE_HOT_KEY									//| Choice of keys											[OK]
#define	__NEW_COLOR_CHANGE									//| Hair color when creating the character					[OK]
#define __CHIP_PLAYERSHOP									//| Private Shops with Penya / Perin / Red Chip / Red Perin	[OK]
#define	__BETTER_POCKET										//| Larger Bags (B)											[OK]
#define	__NEW_CHANGE_JOB									//| Automatic Class Change									[OK]
#define	__DROP_LOG											//| Drop Logging on left of screen							[OK]
#define	__WEAPON_RARITY										//| Weapon Rarity											[OK]
#define	__BLING_BLING										//| Bling Bling Scroll										[OK]
#define __BACKGROUNDRENDER									//| Minimal Render for Background Task						[OK]
#define __MUTEBGSOUNDS										//| Mute Background Task (Sound/Music)						[OK]
#define __MINIMIZEFIX										//| Minimize Fix											[OK]
#define	__ALIASING											//| Reduces Aliasing										[OK]
#define	__ANISOTROPIC										//| Anisotropic Filtering									[OK]
#define __HIDE_NPC_BUBBLES									//| Hide NPC Chat Bubbles									[OK]
#define	__SYS_CUSTOM_TITLE									//| Own User Title											[OK]
#define __COMBOBOX_FIX	10									//| Fix combobox dropdown size	x= max entries shown		[OK]
#define __MOUNT												//| Mount System											[OK]
#define	__SYS_PARTYFINDER									//|	Partyfinder												[OK]
#define __SHOPS_ONLY_CONT									//| Shops only in shopping continent						[OK]
#define __HIDE_MONSTER_LEVEL								//| Option to hide monster tag in name						[OK]
#define __ALPHA_CHAT										//| Option to hide chat										[OK]
#define __STOP_EXP											//| Option to Stop EXP										[OK]
#define __WELCOME_MESSAGE									//| Welcome message OnGameJoin                              [OK]
#define __HIDE_POWER_UPS									//| Hide Powerups											[OK]
#define __ADVANCED_MAIL										//| Fast collect/delete mail								[OK]
#define __RANGE_COLLECT_ITEM								//| collect same items in a radius around the player		[OK]
#define __LOOKCHANGE										//| Lookchange												[OK]
#define __HIDE_FASHION										//| Hide Fashion System										[OK]
#define __REMOVE_DURATION_ITEMS								//| Duration - Right Click Remove Item						[OK]
#define __AUTOLOOT_PENYA									//| Penya Automatically in Inventory						[OK]
#define __10MIN_CD_REGUILD									//| 10 Minute Cooltime on Guild Rejoin						[OK]
#define	__SYS_PETFILTER										//| Petfilter System										[OK]
#define __MODEL_VIEW										//| Model Viewer Window										[OK]
#define __THREAD_UPDATE										//| Quicker Database Server									[OK]
#define	__HEAL_SHOW											//| Gain Health Display										[OK]
#define	__REPORT_SYSTEM										//| Creates .txt files (Program\Logs\Server\.Reports).		[OK]
#define	__SYS_AUTO_SHOUT									//| Ability to automatically shout in-game 					[OK]
#define	__AUTO_HELPER_THREAD								//| Helper Thread for auto power up and auto shout			[OK]
#define __TIMER												//| CD remaining instances in the teleporter				[OK]
#define __ANI_WING											//| Animated PARTS_MASK Wingmasks.							[OK]
#define __MODEL_NPC_VIEW									//| 3D Model animated in NPC Menu							[OK]
#define __BLESSINGS_TRADABLE								//| Blessings are Tradable now!								[OK]
#define __AWAKE_MASK										//| Blessings Available for Masks							[OK]
#define __TELEPORTAPP_EXCEPTIONS							//| Teleport System Exceptions								[OK]
#define __AUTO_MAX_SKILL									//| Skillpoints added automatically.						[OK]
#define __ENEMY_SYSTEM										//| Shows Enemy if player is not in same guild on Siege.	[OK]
#define	__LOOTPET_TELEPORT									//| Lootpet teleports with player							[OK]
#define __ACTIVE_ITEM_ICON_V17								//| Display active item checkmark							[OK]
#define	__MESSENGER_CTRL_EXTENTED							//| Modifications on Messenger								[OK]
#define	__PARTYLIST_MESSENGER								//| Modifications on Messenger								[OK]
#define	__PLAYER_LIST										//| Players List											[OK]
#define __BAN_SYSTEM										//| /ban Command											[OK] 
#define	__CROSSBOW						        	        //| Baruna													[OK]
#define __ITEM_COOLDOWN										//| Ingame items with a new cooldown time.					[OK]
#define __SWITCH_EQUIP										//| Switch Equipment System									[OK]
#define __SHOP_SEARCH										//| Shop Search Function									[OK]
#define __INVENTORY_SEARCH									//| Inventory Search Function								[OK]
#define __GUILD_BANK_SEARCH									//| Guild Search Function									[OK]
#define __CHAR_CREATE_ROTATE								//| You can rotate character at creation window				[OK]
#define	__SWING_CHANGE										//| Glow Change for set (items)								[OK]
#define __NEW_BUFF_FORMAT									//| New Round Buffs + Option								[OK]

/******************************************************** AEGON *********************************************************/

#define __AEGON_ADVANCED_LOGIN								//| Advanced login window with acc manager					[OK]
#define __AEGON_AWAKEWINDOW									//| Awake window											[OK]
#define __NO_CLOSE_AWAKEWINDOW								//| Awake window stays open when walking					[OK]
#define __AEGON_AWAKE_COLOR									//| Max awake color											[OK]
#define __AEGON_PETMISTAKE									//| Pet Mistake Aegon										[OK]
#define __ALLOW_BERSERK_SKILLUSE							//| Able to use other skills while berserk is active		[OK]
#define __AEGON_MONSTER_HUNT								//| Monster hunt											[OK]
#define __AEGON_PUT_JEWEL_WEAP								//| Place Weapons for jewel setting with Dbl Click			[OK]
#define __AEGON_COUNT_DIFF_IP								//| Player Count with different ip							[OK]
#define __LEEAEG_SHOP_FINDER								//| Shop Finder System for __PREMIUM menu					[OK] 
#define __AEGON_RETURN_AWAKE								//| Return the last awake									[OK]
#define __AEGON_CONVERT_PERIN								//| penya -> perin converter single/all						[OK]
#define __MOUNT_TOWN_RESTRICTION							//| Restrict the usage of certain mounts in towns			[OK]
#define __AEGON_TOWN_MAP									//| Activate closeup map of the towns						[OK]
#define __AEGON_LOOT_BOX									//| Lootbox system											[OK]
#define __AEGON_VIEW_INV									//| View Players Inventory									[OK]
#define __AEGON_INGAME_DP									//| donate point access ingame								[OK]
#define __AEGON_SEASON_PASS									//| pvp & pve season pass									[DEV] // TODO: pvp points
#define __NEW_HEAVEN_TOWER_WND								//| reworked heaven tower teleport wnd						[OK]
#define __AEGON_QOL_STUFF									//| QOL changes												[OK]
#define __AEGON_ACHIEVEMENT_SYSTEM							//| Achievement System										[OK] //TODO: add more types
#define __AEGON_TEX_LOAD									//| Improved texture loading								[OK]
#define __AEGON_2K_RES										//| 2K resolution support									[OK]
#define __AEGON_FULLSCREEN_AND_RESIZE						//| ingame window/fullscreen switch and client resize		[OK]
#define __AEGON_LAST_ONLINE									//| Last online info for guild members / friends			[OK]
#define __AEGON_INGAME_TIME									//| save players ingame time								[OK]
#define __AEGON_DUNGEON_TELEPORT							//| Dungeon teleport for admins								[OK]
#define __AEGON_CRAFTING_SYSTEM								//| Crafting system											[OK]
#define __AEGON_TELEPORT_SHORTCUT							//| Teleporter Shortcut										[OK]
#define __AEGON_MAX_BUY_CHANGE								//| amount able to purchase instead of 9999					[OK]
#define __AEGON_ADVANCED_ANNOUNCEMENT						//| announcement bg image									[OK]
#define __AEGON_HORI_SCROLLBAR								//| Horizontal scrollbar									[OK]
#define __AEGON_SCROLLBAR_STEP								//| Set the scroll step for scrollbar buttons				[OK]
#define __AEGON_SHUFFLE_DROP								//| Shuffle drop index to randomize	order					[OK]
#define __AEGON_USEFULL_FUNCTIONS							//| functions collection									[OK]
#define __AEGON_CUSTOM_FONTS								//| load custom fonts										[OK]
#define __AEGON_FIXES										//| Define to mark fixes									[OK]
#define __AEGON_PVP_RANKING									//| PVP Ranking System										[OK] //TODO: better window
#define __AEGON_PERMANENT_BUFF								//| Permanent Buff System									[OK]
#define __AEGON_PREM_MOTIONS								//| Unlockable Premium Motions								[OK]
#define __AEGON_THEME_SWITCHER								//| Theme Switch system										[OK]
#define __AEGON_APNG										//| animated png load/render								[OK]
#define __AEGON_FONT_SWITCH									//| ingame font switch without restart						[OK]
#define __AEGON_BOW_FACTOR									//| Bow Factor in Resource									[OK]
#define __AEGON_WEBSHOP_FLAG								//| Flag to mark items purchased from webshop				[OK]
#define __AEGON_EMOTE_SYSTEM								//| Extented Emote System									[OK]
#define __LEEAEG_SYSTEM_INDIVIDUAL							//| System Shout for Individual Users						[OK]
//#define __AEGON_TOURNAMENT									//| Tournament system										[DEV]	//TODO: everything
#define __AEGON_GETITEMPROP_DEBUG							//| Improved Debug for GetItemProp							[OK]
#define __INCREASE_SKILL_ID_VALUE							//| WORD -> DWORD											[OK]
#define __SMALL_CENTERED_TASKBAR							//| choose smaller taskbar when centered					[OK]
#define __AEGON_FAST_SMELT_FILL								//| Fill all 10 slots with one click 						[OK]
#define __LOG_UNKNOWN_SNAPSHOT								//|	Log brocken client snapshots							[OK]
#define __AEGON_DELETED_CHAR_CHECK							//| Check if a char got deleted when processing login		[OK]
#define __AEGON_AUTO_DEACTIVATE_PETS						//| Deactivate Pets when using board/mount					[OK]
#define __AEGON_AUTO_STATS									//| Select one stat to be automatically upped on level up	[OK]
#define __AEGON_F_TOOL										//| F-Tool													[OK]
#define __AEGON_SHOW_GUILD_JOIN_COUNTDOWN					//| Show time left to join a new guild						[OK]
#define __AEGON_RM_HERO_SKILL								//| Changes rm hero skill to fast assist/rm buff			[OK]
#define __AEGON_OPTIONAL_SKILL_SFX							//| make sfx optional to fix sfx spam (aroundbuff, etc)		[OK]
#define __AEGON_SOLO_PARTY_EXP								//| solo party leveling										[OK]
#define __AEGON_NINJA_MODE									//| serverside invisibility									[OK]
#define __AEGON_FOLLOW_HOTKEY								//| hotkey to follow										[OK]
#define __AEGON_TIME_DIFF									//| Server client time diff									[OK]
#define __AEGON_IMPROVED_QUICKSELL							//| filters for quick sell									[OK]
#define __AEGON_CLIENTSIDE_PIECE_DROPRATE					//| Client side droprate for monster wiki					[OK]
//#define __AEGON_PVP_MINIGAMES								//| PvP Minigame System										[OK]
#define __AEGON_IMPROVED_SHOPDESIGN							//| Improved version of __SHOPDESIGN (incl. vending npc)	[OK]
#define __AEGON_STAFF_MOB_IGNORE_MODE						//| Staff mode that makes mobs ignore you					[OK]
//#define __AEGON_BOSS_MECHANICS								//| Special Boss mechanics									[DEV]
#define __AEGON_MESSENGER									//| Improved Messenger										[OK]
#define __AEGON_MINIMIZE_WINDOWS							//| Minimize window button									[OK]
#define __AEGON_SPLASH_SCREEN								//| Splash screen when loading neuz							[OK]
#define __AEGON_MAIL_ITEM_SAVE								//| proper item saving with all variables					[OK]
#define __AEGON_HOTKEY_F_KEYS								//| F10-F24 for hotkeys										[OK]
#define __AEGON_WEAPON_MERGE_100							//| 100% success for 2h weapon merge						[OK]
#define __AEGON_DB_PETFILTER								//| Save petfilter in db insteaf of neuz.ini				[OK]
#define __AEGON_ALWAYS_FULLSHOUT							//| Always full shout without scroll						[OK]
#define __AEGON_TELEPORTER									//| Teleporter system										[OK]
#define __AEGON_LOGOUT_CHAR_SELECT							//| logout to char select option							[OK]
#define __AEGON_DISABLE_ANGEL_CREATION						//| Disable angel creation									[OK]
#define __AEGON_MOB_ATTACK_MOUNTED							//| Monsters attack mounted players							[OK]
#define __AEGON_MASTER_KEEPS_SKILLS							//| Master keeps skills up to 120, taskbar doesnt reset		[OK]
#define __AEGON_BAG_FREE_MOVE								//| freely move items in bag								[OK]

#if defined(__CLIENT) && (defined(__LOCALSERVER) || defined(__TESTSERVER) || defined(__LIVESERVER))
#define __CRASH_REPORT										//| CrashRpt for Neuz										[OK]
#endif

/******************************************************** LEESE *********************************************************/

#define __LEESE_NOTICKET_TELEPORT							//| Admins can teleport without the use of tickets			[OK]
#define __LEESE_VIEW_SETEFFECT								//| Shows set effect with Shift + Hover Item				[OK]
#define __LEESE_MESSENGER_TIMER								//| Timer inside messenger & shop chat © Leese				[OK]
#define __LEESE_REMOVE_LVL1									//| Disabled Level 1 text for dwLimitLevel1					[OK]
#define __LEESE_NO_EVENTITEM_RESELL							//| Disables the reselling of Event Items.					[OK]
#define __LEESE_REMOVE_BUBBLETIME							//| Removed Bubbletime from CWndStatus						[OK]
#define __LEESE_CENTER_BASE_TEXT							//| CWndBase Text Centered in Window						[OK]
#define __PLACEHOLDER_TEXT									//| Placeholder Text										[OK]
#define __WIKIPEDIA_ITEMS									//| Wikipedia (Items)										[OK]
#define	__WIKIPEDIA_TITLE									//| Wikipedia (Titles)										[OK]
#define	__WIKIPEDIA_JOB										//| Wikipedia (Classes)										[OK]
#define	__WIKIPEDIA_MONSTER									//| Wikipedia (Monster) + Admin Create						[OK]
#define __LEESE_LOADING_SPREAD								//| Spread Loadingscreens on all resolutions				[OK]
#define __LEESE_VENDING_RENDER								//| Changed positioning of vendor title bar rendering		[OK]
#define __LEESE_BADGE_HONOR_COMBINE							//| Combined Honor & Badge in one window					[OK]
#define __LEESE_RARITY_1000									//| Upgraded Rarity to 1000 instead of 100					[OK]
#define __LEESE_AROUND_BUFF									//| Around Buff command for Staff							[OK]
#define __LEESE_SM_DESC										//| SM Item Description										[OK]
#define __LEESE_WORLDSERVER_OPT								//| WorldServer optimized list								[OK]
#define __BUFF_4X_OPT										//| 14x Rendering buffs instead of 7						[OK]
#define __LEESE_PARTY_REFRESH								//| Party Skill Refresh OnJoin()							[OK]
#define __LEESE_EVENT_REWARD								//| Command to spawn Event Rewards for GM+					[OK]							
#define __LEESE_STAFF_CONNECT								//| Staff Connect Message									[OK]
#define __LEESE_RENDER_ITEMCOUNT							//| New Item Count Design									[OK]
#define __LEESE_STAFF_BOLD									//| Staff Bold Names (Multi Render)							[OK]
#define __LEESE_GUILD_LEVEL_UP								//| TextCmd_GuildStat > Level								[OK]
#define __LEESE_PARTY_LOCATION								//| Location in Party Window								[OK]
#define __LEESE_SAVE_STATUS									//| Saving WndStatus in Neuz.ini							[DEV]
#define __LEESE_SECURITY_CHECKS								//| Small Security Checks									[DEV]
#define __LEESE_STAFF_DUNGEON								//| Admin can enter dungeon without level restriction		[OK]

/******************************************************** MIKSIK *********************************************************/

#define __MIKSIK_GS_REWARD									//| Change Guildsiege reward								[OK]
#define __MIKSIK_BOSSCRIT									//| Boss critical dmg edits									[OK]
#define __MIKSIK_FLASH_WINDOW_EX							//| Flashing Neuz icons										[OK]
//#define __MIKSIK_EQUALIZED_MODE                             //| Equalized PVP 											[DEV] //Todo: fix HP Bug. When leaving Temporal zone your hp Displays 1/8932 ( Example since my test char had that )
#define	__MIKSIK_NEWSTAT									//| New stats.												[OK]
/******************************************************** DST'S *********************************************************/

#define __DST_CHANCEDROP_GOLD								//| DST_CHANCEDROP & DST_GOLDDROP							[OK]
#define __DST_COOLTIME_DUNGEON								//| DST_COOLTIME_DUNGEON_REDUCED							[OK]
#define __DST_CHANCEDROP_PIECE								//| DST_CHANCEDROP_PIECE									[OK]
#define __DST_COLLECTOR_TIME								//| DST_COLLECTOR_TIME										[OK]
#define __DST_PET_EXPERIENCE								//| DST_PET_EXPERIENCE										[OK]

/******************************************************** FIXES *********************************************************/

#define __FIX_DUPE_ITEM_CHAR								//| Players can't dupe characters after deletion			[OK]
#define __FIX_RANGER_ROLLER									//| Fixes Ranger Animation									[OK]
#define __FIX_SETFST										//| Frameskip Fix											[OK]
#define __FIX_ANIMATE_MODEL									//| Animated Model Fix										[OK]
#define __FIX_BUFFPANG_SPAM									//| Anti Buff Pang Spam										[OK]
#define __FIX_DST_HEAL										//| DST Heal Fix											[OK]
#define __FIX_NO_CHAT_CLEAR									//| Chat is not deleting text anymore.						[OK]
#define __FIX_RESET_BUFFTIME_ON_RECONNECT					//| Fixed a buff > 60 Min, on reconnection time resets.		[OK]
#define __FIX_GENDER_NPC									//| Fixed a bug on wrong serializing from Sex				[OK]
#define __FIX_BUTTON_FREEZE									//| Fixed Freezing Client on F10 and Alt key				[OK]
#define __FIX_CHARACTER_DISCONNECT							//| Character Disconnect Fix								[OK]
#define __FIX_GUILD_OVERFLOW								//| Fixed Guild Overflow									[OK]
#define __FIX_TICK											//| 500ms Delay on Equip Sound								[OK]
#define __FIX_EXPLOIT_PET									//| Fix Pet Exploit 2020									[OK]
#define __FIX_REMOVESFX										//| Removes SFX if object is invalid.						[OK]
#define __FIX_TIMER_GW										//| GW Timer Fix - CDPClient::OnGuildCombatEnterTime		[OK]
#define __FIX_TOOLTIP_INVENTORY								//| Fixed FPS drop when mouse over inventory				[OK]
#define __FIX_TEXTUREBUG									//| Texture Rendering Bugfix								[OK]

#define __ITEM_ANIMATE_FIX									//| Fixed Item Animation on Weapons.						[OK]
#define __COLLISION_FIX_PLAYER								//| Player Collision Fix									[OK]
#define __4TH_HIT_HACK										//| Fixes the 4th hit hack of a blade						[OK]
#define __CPU_UTILDOWN_060502								//| Implementation of more modern D3D Methods				[OK]
#define	__SYS_PLAYER_SYNC									//|	Fixed Stat Sync											[OK]
#define __LOGOUTFIX											//| Logout Fixed											[OK]
#define __BAGTPFIX											//| Safe Delete Bag											[OK]
#define __SLOW_WALK_FIX										//| Correction of position bugs with the walk key ("W")		[OK]
#define __BLOU_EXPERIMENTAL_FIX2							//| ITransformer::IsValidStuff								[OK]
#define __MOVE_LIKE_OFFI									//| Move like offi											[OK]
#define __REVIVAL_FIX										//| Res window doesn't appear if player isn't dead			[OK]
#define	__DROP_ITEM											//| Stop throwing away Eggs & Red Chips						[OK]
#define __ALWAYS_MODEL_LIGHT								//| Small Tweak -> Models always enlighted					[OK]
#define	__SLEEPING_FIX										//| Sleeping Fix											[OK]
#define	__INVISIBLE_FIX					        	    	//| Invisibility Fixed.										[OK]
#define	__SECURITY_FIXES				        			//| Global Security Fixes.									[OK]
#define	__WND_BUTTONS_FIX				        			//| Wnd Buttons Fixed.										[OK]
#define __JEWEL_EQUIPMENTFIX								//| Logic Jewel Equipment Fix								[OK]
#define __PSEVDORANGERFIX									//| Mini-fix for rangers									[OK]
#define __SFX_OPT_FIX										//| Rendering SFX Optimized									[OK]
#define __GH_DUPE_FIX										//| Guild House Duping Fix									[OK]
#define __POSI_BUG_FIX										//| Position Bugfix.										[OK]
#define __POSI_BUG_FIX_2									//| Position Bugfix.										[OK]
#define __JUMP_FIX_2021										//| Jumping Ani is altered by move speed of the char		[OK]

//#define __RANGE_FIX											//| Range Hack Fix										[DEV] Not tested.
#define __EEL_BUG_FIX										//| Fixed Grilled Eel Bug									[DEV]

/********************************************************* TEST **********************************************************/

#define __RENAMEPARTY										//| Party Rename System										[OK]
#define __UPGRADE_SUCCESS									//| Fail stack upgrade										[OK]
#define __LNB_ANIMATED_HATS									//| Animated Hats System									[OK]
#define __NAVIGATOR_RENDER_BUG_FIX							//| Navigator Render Bug Fixed								[OK]
#define	__QUICK_MOVE_ITEMS									//| Rightclick to sell, move, trade Items					[OK]
#define __BUYBACK											//| Buyback System											[OK]
#define __NEW_DIAMONDS										//| New Diamonds / New Diamond DST / Resource				[OK]
#define __TITANIUM_DISABLED									//| Disabled Offi Flyff Stuff								[OK]
#define __RESTRICT_ITEM_USAGE								//| Restrict Item Usage in certain worlds.					[OK]
#define __CLIENT_ONLINE_COUNT								//| Players Online count for Control + F overlay			[OK]
#define __WIKIPEDIA_MONSTER_DROPLIST						//| Droptable on Monster Wikipedia							[OK]
#define __IMPROVEGFX0112									//| Terrain Rendering Optimization							[OK]
#define __NEWCUR											//| Animated & changeable cursors							[OK]
#define __SYS_MAP_BUFFS										//| Map Buffs												[OK]
#define __PIERCE_CARD_TOOLTIPS								//| Advanced Piercing Card Tooltips							[OK]
#define __NO_PARTY_GW_JOIN									//| You cant join a party in GW map							[OK]
#define __MAPNAME_CHANGE_FIX								//| Gets the correct map name on teleporting.				[OK]
#define __SQK_MULTIALTERNATEPRICE							//| Multialternate AddShopItem Price						[DEV]		
#define __INFO_DUNGEONS										//| Information of killed monsters in dungeon				[OK]
#define	__PANG_SYSTEM										//| Replacing pang with piñata								[OK]
#define	__PET_GLOW											//| Pet Aura												[OK]
#define __MODS_MANAGER										//| Easily change modus for staff							[OK]
#define	__SYS_WORLDBOSS										//|	WorldBoss.xml - Bosses last 1 hour						[OK]
#define __PROPPACK_TOOLTIP									//| Shows PropPack Items in Tooltip							[OK]
#define __BUTTONS_V19										//| New V19 Button Loading -> Text Render over Texture		[OK]
#define __IGNORE_LIST_OPT									//| Optimized Ignore List in MessengerEx					[OK]
#define __RENDER_SMALL_POWERUPS								//| Small Powerup Rendering in Titanium Options.			[OK]
#define __DPS_METER											//| Damage Per Second meter									[OK]
#define __COMBAT_LOG										//| Log attacked monsters by player							[OK]
#define __FLYFF_INITPAGE_EXT								//| Login Video Fixed - Leese								[OK]
#define __TITANIUM_TOOLTIP_OPT								//| Optimized Titanium Tooltips								[OK]
#define __NPC_QUICK_SELL									//| Quick Sell Blues										[OK]
#define __COUPLE_EDITS										//| Couple Window Edits										[OK]
#define __WORLSERVER_MENU									//| WorldServer Menu										[OK]

#define __STACKABLE_POWERUPS								//| Power-Ups Stackable for 1 hour							[OK]
#define __SYS_GLYPHS										//| Glyphs instead of arrows								[OK]
#define	__NAVIGATOR_V19										//| V19 - Minimap											[OK]

#define __WBQT_GUILD_BUFFS									//| Guild Buffs												[DEV] // Add Requirements ?
#define __ACTION_SLOT_PAGES									//| More Action Slots										[OK]

#define __FL_SMOOTH_STATUS									//| Smooth Status Window									[OK]
#define __FL_FARM_STAT										//| Farm Counter											[OK]
#define __DEV_FREECAM										//| Freecam Mode for Administrators							[OK]
#define __BLOCK_GUILD_INVITES								//| Option to disabled guild invites						[OK]

#define __AUTO_ATTACK_NEXT_TARGET							//| Auto Attack of next target								[OK]
#define __SYS_DUNGEONANNOUNCE								//| Dungeon Announcement of Boss Spawning					[OK]
#define	__REMOVE_SKILL_BUFF									//| Remove Positive Skill Buffs								[OK]
#define __WS_STAFF_COUNT									//| Check Staff Members online								[OK]

#define	__BLOCK_STAFF_ACTIONS								//| Block Mail/Trade/Banking/Vending for Staff Member		[OK]
#define __SYS_DIA_REMOVE_CHOICE								//| Choose which diamond you want to remove					[OK]
#define __NEW_SFX_WINGS										//| New SFX Buff Items										[OK]
#define __AUTH_SHOUT										//| Add [Staff] on shouting									[OK]

#define __SYS_REBIRTH										//| Rebirth System	(Not finished)							[DEV]

#define __FL_BAG_LOOT										//| Option to loot directly into Bag (B)					[DEV]
//#define __FL_BAG_UPDATE									//| Use items in Bag (B)									[DEV] // Most items cant be used?

//#define	__ROYAL_RUMBLE			1							//|	Royal Rumble Sign-up Cost								[OK]
//#define	__RUMBLE_SPAWNFIX									//|	Fixed spawns from release version						[OK]
//#define	__RUMBLE_LOGGING									//|	Enable logging for Rumble								[OK]
//#define	__RUMBLE_REVIVE										//|	User does not get a revive window						[OK]
//#define	__RUMBLE_FREE_SIGNUP								//|	Signup costs no penya									[OK]
//#define	__RUMBLE_GIVEUP										//|	Command for players to surrender from Rumble			[DEV]

//#define __CRYSTAL											//| Crystal Weapons -> Development

/******************************************************* DISABLED ********************************************************/

//#define __SYS_AUTO_POWER_UPS								//| Possibility to use powerups automatically				[Not Implemented]

//#define	__BARUNA_PIERCING				        			//| Baruna													[Testing] //Disable?
//#define	__NEW_ITEM_VARUNA				        			//| Baruna													[Testing] //Disable?

/**************************************************** GLOBAL STRUCTS *****************************************************/
typedef struct __LOOTBOX_RESULT
{
	DWORD receiveItem;
	DWORD receiveNum;
	DWORD itemIndex;
	DWORD chance;
	BOOL validResult;


	void Init()
	{
		receiveItem = 0;
		receiveNum = 0;
		itemIndex = 0;
		chance = 0;
		validResult = FALSE;
	}
}__LOOTBOX_RESULT;

typedef struct __DP_COMMAND_INFO
{
	int command;
	DWORD dpCost;
	int nDPCommandVar1;
	__LOOTBOX_RESULT lootboxResult;
	DWORD newDPBalance;
	DWORD dwAmount;

	void Init()
	{
		command = 0;
		dpCost = 0;
		nDPCommandVar1 = 0;
		lootboxResult.Init();
		newDPBalance = 0;
		dwAmount = 1;
	}
}__DP_COMMAND_INFO;

/**************************************************** HELPER FUNCTION *****************************************************/

template<class T, typename U>
bool easy_find(T vec_or_map, U value) {
	if (std::find(vec_or_map.begin(), vec_or_map.end(), value) != vec_or_map.end())
		return true;

	return false;
}

/********************************************************** OLD **********************************************************/
//#define __ABSORB_DAMAGE_LIMIT								//|
//#define __MONSTER_FLY_CRIT								//| 
//#define __PMA_FIX_ATKMSG									//|	 
//#define __PMA_GW											//| 
//#define __REBIRTH											//| Rebirth System.
//#define __GLOW_NORMAL										//|
//#define __GUILD_JOIN_NOTIME								//| No Guild rejoin time.
//#define __NO_PET_LIFE										//| No Pet lifes.
//#define __ONLINE_PLAYER									//|
//#define __EVENT_TELEPORT_ALL								//|
//#define __UPGRADE_SPEED									//|
//#define __TORCHE_SYSTEM									//|
//#define __NEWSCROLLS										//|
//#define __FIX_AWAKE_SCROLLS								//|
//#define __GW_EXTENDED										//|
//#define __TAB_TARGET										//|
//#define __AWAKE_JEWELRY									//|
//#define __GM_LISTING										//|
//#define __DOUBLECLICK_ITEMUSE								//|
//#define __AUTO_PICKUP_SCROLL								//| 
//#define __FLOFLO_COUPLE_SCROLL							//|
//#define __MULTIPLE_ITEM_PICKER		5.0f				//| Multiple Item Pickup System.

//#define __STAFF_ICON										//| Staff Icon Rendering
//#define __SHOPS_ONLY_IN_SAINT								//| Shops can only be placed in Saint Morning.
//#define __BLOCK_STAFF_ACTIONS								//| Block trade / mail / vending for staff characters.
//#define __DAILY_GIFT										//| Daily Gift System Fixed.

//#define __CHEAT_ENGINE_BLOCK								//| Cheat Engine Block.
//#define __ANTI_PET_EXPLOIT								//| Pet Exploit Fix on Eillun.
//#define __ADMIN_AUTH										//| Second Admin Authentication.
//#define __PMA_STUNBUG_FIX									//| 
//#define __FIX_LINKINWAR									//|
//#define __FIX_COMBINE_FASHION								//|
//#define __Gridel_Fix										//|
//#define __FIX_GET_HP_MP_FP_POINT							//|
//#define __CHEAT_ENGINE									//|
//#define __FIX_RANGE_02									//|
//#define __FIX_CHEAT										//|
//#define __HACK_FIXXES										//|
