#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndParty.h"
#include "WndNotice.h"
#include "WndMessenger.h"
#include "WndBank.h"		 // 은행 / 창고
#include "WndRepairItem.h"	 // 수리창
#include "WndDebugInfo.h"    // 디버그 창 
#include "WndHelp.h"
#include "WndWebBox.h"
#include "WndVendor.h"
#include "WndCommItem.h"
#include "WndQuest.h"
#include "WndMotion.h"
#include "WndPvp.h"
#include "WndGuildVote.h"
#ifdef __IMPROVE_MAP_SYSTEM
#include "WndMapEx.h"
#else // __IMPROVE_MAP_SYSTEM
#include "WndMap.h"
#endif // __IMPROVE_MAP_SYSTEM
#include "WndBagEx.h"

#if __VER >= 8 //__CSC_VER8_2
#include "WndPartyQuick.h"
#endif //__CSC_VER8_2
#if __VER >= 13 // __HOUSING
#include "WndHousing.h"
#endif // __HOUSING

#if __VER >= 15 // __GUILD_HOUSE
#include "WndGuildHouse.h"
#endif

#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
#include "Wnd2ndPassword.h"
#endif // __2ND_PASSWORD_SYSTEM

#ifdef __NEW_WEB_BOX
#include "WndHelperWebBox.h"
#endif // __NEW_WEB_BOX

#ifdef __WIKIPEDIA_ITEMS
#include "WndWikiItems.h"
#endif //__WIKIPEDIA_ITEMS

#ifdef __AEGON_MONSTER_HUNT
#include "WndMonsterHunt.h"
#endif // __AEGON_MONSTER_HUNT

#ifdef __MODEL_VIEW
#include "WndModelView.h"
#endif //__MODEL_VIEW

#ifdef __GIFTBOX_LIST_ITEMS
#include "WndGiftbox.h"
#endif //__GIFTBOX_LIST_ITEMS

#ifdef __GUILDFINDER
#include "WndGuildFinder.h"
#endif //__GUILDFINDER

#ifdef __SYS_AUTO_SHOUT
#include "WndAutoShout.h"
#endif // __SYS_AUTO_SHOUT

#ifdef __WIKIPEDIA_MONSTER
#include "WndMonsterWiki.h"
#endif // __WIKIPEDIA_MONSTER

#ifdef __BAN_SYSTEM
#include "WndPlayerPurge.h"
#endif //__BAN_SYSTEM


#ifdef __SWITCH_EQUIP
#include "WndSwitchEquip.h"
#endif // __SWITCH_EQUIP

#ifdef __AEGON_PVP_RANKING
#include "WndAegonPvPRanking.h"
#endif // __AEGON_PVP_RANKING

#ifdef __AEGON_INGAME_SHOP
#include "WndAegonIngameShop.h"
#endif

DECLAREAPPLET( AppMain_WndNavigator    , new CWndNavigator   );
DECLAREAPPLET( AppMain_WndCharacter    , new CWndCharacter   );
DECLAREAPPLET( AppMain_WndSkill2       , new CWndSkillTreeEx );
DECLAREAPPLET( AppMain_WndEmotion      , new CWndEmotion     );
DECLAREAPPLET( AppMain_WndMotion       , new CWndMotion      );
DECLAREAPPLET( AppMain_WndTrade        , new CWndTrade       );
DECLAREAPPLET( AppMain_WndVendor       , new CWndVendor );
DECLAREAPPLET( AppMain_WndQuest        , new CWndQuest       );
DECLAREAPPLET( AppMain_WndPvp          , new CWndPvp       );
DECLAREAPPLET( AppMain_WndInventory    , new CWndInventory   );
DECLAREAPPLET( AppMain_WndWebBox       , new CWndWebBox  );
DECLAREAPPLET( AppMain_WndWebBox2      , new CWndWebBox2 );
DECLAREAPPLET( AppMain_WndParty        , new CWndParty  );
DECLAREAPPLET( AppMain_WndGuild        , new CWndGuild  );
DECLAREAPPLET( AppMain_WndCommItem     , new CWndCommItem  );
DECLAREAPPLET( AppMain_WndUpgradeBase  , new CWndUpgradeBase  );
DECLAREAPPLET( AppMain_WndPiercing     , new CWndPiercing  );
DECLAREAPPLET( AppMain_WndChat         , new CWndChat    );
#if __VER >= 11 // __CSC_VER11_4
DECLAREAPPLET( AppMain_WndMessenger    , new CWndMessengerEx );
#else //__CSC_VER11_4
DECLAREAPPLET( AppMain_WndMessenger    , new CWndMessenger );
#endif //__CSC_VER11_4
DECLAREAPPLET( AppMain_WndOptSound     , new CWndOptSound    );
DECLAREAPPLET( AppMain_WndOptWindow    , new CWndOptWindow   );
DECLAREAPPLET( AppMain_WndOptMyInfo    , new CWndOptMyInfo   );
DECLAREAPPLET( AppMain_WndOption       , new CWndTotalOption );
DECLAREAPPLET( AppMain_WndInfoNotice   , new CWndInfoNotice  );
DECLAREAPPLET( AppMain_WndHelpHelp     , new CWndHelp        );
DECLAREAPPLET( AppMain_WndHelpTip      , new CWndHelpTip     );
DECLAREAPPLET( AppMain_WndHelpFAQ      , new CWndHelpFAQ     );
DECLAREAPPLET( AppMain_WndLogOut       , new CWndLogOut      );
DECLAREAPPLET( AppMain_WndQuit         , new CWndQuit        );
DECLAREAPPLET( AppMain_WndWorld        , new CWndWorld       );
DECLAREAPPLET( AppMain_WndDebugInfo    , new CWndDebugInfo   );
DECLAREAPPLET( AppMain_WndStatus       , new CWndStatus       );
DECLAREAPPLET( AppMain_WndLogin        , new CWndLogin        );
DECLAREAPPLET( AppMain_WndSelectServer , new CWndSelectServer );
DECLAREAPPLET( AppMain_WndCreateChar   , new CWndCreateChar   );
DECLAREAPPLET( AppMain_WndSelectChar   , new CWndSelectChar   );

#if __VER >= 9  // __INSERT_MAP
#ifdef __IMPROVE_MAP_SYSTEM
DECLAREAPPLET( AppMain_WndMap   , new CWndMapEx   );
#else // __IMPROVE_MAP_SYSTEM
DECLAREAPPLET( AppMain_WndMap   , new CWndMap   );
#endif // __IMPROVE_MAP_SYSTEM
#endif
#if __VER >= 12 // __LORD
DECLAREAPPLET( AppMain_LordSkill, new CWndLordSkill  );
#endif
#if __VER >= 8 //__CSC_VER8_2
DECLAREAPPLET( AppMain_WndPartyQuick   , new CWndPartyQuick   );
#endif //__CSC_VER8_2
#if __VER >= 8 //__CSC_VER8_3
DECLAREAPPLET( AppMain_WndBuffStatus   , new CWndBuffStatus   );
#endif //__CSC_VER8_3

#ifdef __GUILDVOTE
DECLAREAPPLET( AppMain_WndGuildVote   , new CWndGuildVote  );
#endif
DECLAREAPPLET( AppMain_WndInvenRemoveItem, new CWndInvenRemoveItem );
#if __VER >= 11 // __SYS_POCKET
#ifndef __TMP_POCKET
DECLAREAPPLET( AppMain_BagEx    , new CWndBagEx   );
#endif
#endif

#if __VER >= 12 // __MOD_TUTORIAL
DECLAREAPPLET( AppMain_InfoPang    , new CWndInfoPang   );
#endif
#if __VER >= 13 // __HOUSING
DECLAREAPPLET( AppMain_Housing    , new CWndHousing   );
#endif // __HOUSING

#if __VER >= 15 // __GUILD_HOUSE
DECLAREAPPLET( AppMain_GuildHousing    , new CWndGuildHousing   );
#endif

#if __VER >= 13 // __CSC_VER13_2
DECLAREAPPLET( AppMain_Couple    , new CWndCoupleManager );
#endif //__CSC_VER13_2

#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
DECLAREAPPLET( AppMain_Wnd2ndPassword  , new CWnd2ndPassword );
#endif // __2ND_PASSWORD_SYSTEM

#ifdef __NEW_WEB_BOX
DECLAREAPPLET( AppMain_WndHelperWebBox , new CWndHelperWebBox  );
#endif // __NEW_WEB_BOX

#ifdef __WIKIPEDIA_ITEMS
DECLAREAPPLET(AppMain_WikiItems, new CWndWikiItems);
#endif //__WIKIPEDIA_ITEMS

#ifdef __AEGON_AWAKEWINDOW
DECLAREAPPLET(AppMain_WndAwakening, new CWndAwakening);
#endif //__AEGON_AWAKEWINDOW

#ifdef __AEGON_MONSTER_HUNT
DECLAREAPPLET(AppMain_WndMonsterHunt, new CWndMonsterHunt);
#endif // __AEGON_MONSTER_HUNT

#ifdef __PREMIUM
DECLAREAPPLET(AppMain_WndPremiumMenu, new CWndPremium);
#endif //__PREMIUM

#ifdef __LEEAEG_SHOP_FINDER
DECLAREAPPLET(AppMain_ShopFinder, new CWndShopFinder);
#endif //__LEEAEG_SHOP_FINDER

#ifdef __MODEL_VIEW
DECLAREAPPLET(AppMain_ModelView, new CWndModelView);
#endif //__MODEL_VIEW

#ifdef __GIFTBOX_LIST_ITEMS
DECLAREAPPLET(AppMain_GBPreview, new CWndGiftbox);
#endif //__GIFTBOX_LIST_ITEMS

#ifdef __WIKIPEDIA_TITLE
DECLAREAPPLET(AppMain_WndWikipediaTitle, new CWndHonorTitle);
#endif // __WIKIPEDIA_TITLE

#ifdef __WIKIPEDIA_JOB
DECLAREAPPLET(AppMain_WndWikipediaJob, new CWndWikipediaJob);
#endif // __WIKIPEDIA_JOB

#ifdef __NEW_CHANGE_JOB
DECLAREAPPLET(AppMain_WndNewChangeJob, new CWndNewChangeJob);
DECLAREAPPLET(AppMain_WndNewChangeJob2, new CWndNewChangeJob2);
DECLAREAPPLET(AppMain_WndNewChangeJob3, new CWndNewChangeJob3);
#endif // __NEW_CHANGE_JOB

#ifdef __WEAPON_RARITY
DECLAREAPPLET(AppMain_WndWeaponRarity, new CWndWeaponRarity);
#endif // __WEAPON_RARITY

#ifdef __SYS_PARTYFINDER
DECLAREAPPLET(AppMain_WndPartyFind, new CWndPartyFind);
#endif //__SYS_PARTYFINDER

#ifdef __ANARCHY_SYSTEM
DECLAREAPPLET(AppMain_AnarchySkill, new CWndAnarchySkill);
#endif // __ANARCHY_SYSTEM

#ifdef	__AEGON_TELEPORTER
DECLAREAPPLET(AppMain_WndTeleporter, new CWndTeleporter);
#endif

#ifdef __AEGON_LOOT_BOX
DECLAREAPPLET(AppMain_WndLootBox, new CWndLootBoxMain);
#endif // __AEGON_LOOT_BOX

#ifdef __SYS_PETFILTER
DECLAREAPPLET(AppMain_WndPetfilter, new CWndPetfilterMain);
#endif //__SYS_PETFILTER

#ifdef __GUILDFINDER
DECLAREAPPLET(AppMain_WndGuildFinder, new CWndGuildFinder);
#endif //__GUILDFINDER

#ifdef __REPORT_SYSTEM
DECLAREAPPLET(AppMain_WndReport, new CWndReport);
#endif // __REPORT_SYSTEM

#ifdef __TASKBAR_OPEN_SITE
DECLAREAPPLET(AppMain_OpenHomepage, new CWndHomepage);
DECLAREAPPLET(AppMain_OpenDiscord, new CWndDiscord);
#endif //__TASKBAR_OPEN_SITE

#ifdef __SYS_AUTO_SHOUT
DECLAREAPPLET(AppMain_WndAutoShout, new CWndAutoShout);
#endif // __SYS_AUTO_SHOUT

#ifdef __AEGON_SEASON_PASS
DECLAREAPPLET(AppMain_WndSeasonPass, new CWndSeasonPassMain);
#endif // __AEGON_SEASON_PASS

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
DECLAREAPPLET(AppMain_WndAchievementSystem, new CWndAchievementSystemMain);
#endif // __AEGON_ACHIEVEMENT_SYSTEM

#ifdef __WIKIPEDIA_MONSTER
DECLAREAPPLET(AppMain_MonsterWiki, new CWndWikipediaMonster);
#endif // __WIKIPEDIA_MONSTER

#ifdef __PLAYER_LIST
DECLAREAPPLET(AppMain_WndPlayerList, new CWndPlayerList);
#endif // __PLAYER_LIST

#ifdef __BAN_SYSTEM
DECLAREAPPLET(AppMain_WndPlayerPurge, new CWndPlayerPurge);
#endif //__BAN_SYSTEM

#ifdef __SWITCH_EQUIP
DECLAREAPPLET(AppMain_WndSwitchEquipment, new CWndSwitchEquip);
#endif // __SWITCH_EQUIP

#ifdef __AEGON_PVP_RANKING
DECLAREAPPLET(AppMain_WndPvPRankingMain, new CWndPvPRankingMain);
#endif // __AEGON_PVP_RANKING

#ifdef __PET_GLOW
DECLAREAPPLET(AppMain_WndPetGlowChange, new CWndPetGlowChange);
#endif // __PET_GLOW

#ifdef __MODS_MANAGER
DECLAREAPPLET(AppMain_WndModsManager, new CWndModsManager);
#endif // __MODS_MANAGER

#ifdef __AZURIOM_IMAGES
DECLAREAPPLET(AppMain_WndAzuriomImage, new CWndAzuriomProfileImage);
#endif

#ifdef __FL_FARM_STAT
DECLAREAPPLET(AppMain_FarmCounter, new CWndFarmCounter);
#endif //__FL_FARM_STAT

#ifdef __AEGON_INGAME_SHOP
DECLAREAPPLET(AppMain_WndIngameShopMain, new CWndIngameShopMain);
#endif

#ifdef __AEGON_TOURNAMENT
DECLAREAPPLET(AppMain_WndCreateTournament, new CWndAegonCreateTournament);
#endif

#ifdef __AEGON_F_TOOL
DECLAREAPPLET(AppMain_WndFTool, new CWndAegonFTool);
#endif

#ifdef __AEGON_MESSENGER
DECLAREAPPLET(AppMain_WndAegonMessenger, new CWndAegonMessenger);
#endif

#ifdef __AEGON_PVP_MINIGAMES
DECLAREAPPLET(AppMain_WndAegonPvPMinigamesCreate, new CWndAegonPvPGameCreate);
DECLAREAPPLET(AppMain_WndAegonPvPMinigamesFinder, new CWndAegonPvPGameFinder);
#endif

void CWndMgr::AddAllApplet()
{
#ifdef __CHANGE_HOT_KEY
	AddAppletFunc( AppMain_WndNavigator    , APP_NAVIGATOR               , _T( "WndNavigator" )    , _T( "Icon_MenuNavigator.dds" )    , GETTEXT( TID_TIP_NAVIGATOR      ),  'N', GETTEXT(TID_APP_NAVIGATOR), AUTH_GENERAL);
	AddAppletFunc( AppMain_WndStatus       , APP_STATUS1                 , _T( "WndStatus" )       , _T( "icon_MenuStatus.dds"    )    , GETTEXT( TID_TIP_STATUS         ), 'T', GETTEXT(TID_APP_STATUS), AUTH_GENERAL);

#if __VER >= 9  // __INSERT_MAP
#ifdef __IMPROVE_MAP_SYSTEM
	AddAppletFunc( AppMain_WndMap       , APP_MAP_EX              , _T( "WndMap" )       , _T( "icon_MenuWorldmap.dds"    )    , GETTEXT(TID_TIP_MAP), 'M', GETTEXT(TID_APP_MAP), AUTH_GENERAL);
#else // __IMPROVE_MAP_SYSTEM
	AddAppletFunc( AppMain_WndMap       , APP_MAP                 , _T( "WndMap" )       , _T( "icon_MenuWorldmap.dds"    )    , GETTEXT(TID_TIP_MAP), 'M', GETTEXT(TID_APP_MAP), AUTH_GENERAL);
#endif // __IMPROVE_MAP_SYSTEM
#endif

#if __VER >= 12 // __LORD
	if( ::GetLanguage() == LANG_FRE )
		AddAppletFunc( AppMain_LordSkill       , APP_LORD_SKILL             , _T( "WndLordSkill" )       , _T( "Icon_Infopang.dds"    )    , GETTEXT(TID_TIP_INFOPANG), 0, _T("Lord Skill"), AUTH_GENERAL);
	else
		AddAppletFunc( AppMain_LordSkill       , APP_LORD_SKILL             , _T( "WndLordSkill" )       , _T( "Icon_Infopang.dds"    )    , GETTEXT(TID_TIP_INFOPANG), 0, _T("Lord Skill"), AUTH_GENERAL);
#endif	// __LORD

#if __VER >= 12 // __MOD_TUTORIAL
	AddAppletFunc( AppMain_InfoPang       , APP_INFOPANG                , _T( "WndInfoPang" )       , _T( "Icon_Infopang.dds"    )    , GETTEXT(TID_TIP_INFOPANG), 0 );
#endif
#if __VER >= 13 // __HOUSING
	AddAppletFunc( AppMain_Housing       , APP_HOUSING                , _T( "WndHousing" )       , _T( "icon_MenuHousing.dds"    )    , GETTEXT(TID_GAME_HOUSING_BOX), 'Y', GETTEXT(TID_GAME_HOUSING_BOX), AUTH_GENERAL);
#endif // __HOUSING

#if __VER >= 15 // __GUILD_HOUSE
	AddAppletFunc( AppMain_GuildHousing     , APP_GH_FURNITURE_STORAGE, _T( "WndGuildHousing" )     , _T( "icon_MenuHousing.dds"    )    , GETTEXT(TID_GAME_HOUSING_BOX), 'R', GETTEXT(TID_GAME_HOUSING_BOX), AUTH_GENERAL);
#endif 
#if __VER >= 13 // __RENEW_CHARINFO
	AddAppletFunc( AppMain_WndCharacter    , APP_CHARACTER3               , _T( "WndCharacter" )    , _T( "icon_MenuCharacter.dds" )    , GETTEXT( TID_TIP_CHARACTER      ), 'H', GETTEXT(TID_APP_CHARACTER), AUTH_GENERAL);
#elif __VER >= 9 // __CSC_VER9_2
	AddAppletFunc( AppMain_WndCharacter    , APP_CHARACTER2               , _T( "WndCharacter" )    , _T( "icon_MenuCharacter.dds" )    , GETTEXT( TID_TIP_CHARACTER      ), 'H', GETTEXT(TID_APP_CHARACTER), AUTH_GENERAL);
#else //__CSC_VER9_2
	AddAppletFunc( AppMain_WndCharacter    , APP_CHARACTER               , _T( "WndCharacter" )    , _T( "icon_MenuCharacter.dds" )    , GETTEXT( TID_TIP_CHARACTER      ), 'H', GETTEXT(TID_APP_CHARACTER), AUTH_GENERAL);
#endif //__CSC_VER9_2

	AddAppletFunc( AppMain_WndInventory    , APP_INVENTORY               , _T( "WndInventory" )    , _T( "icon_MenuInventory.dds" )    , GETTEXT( TID_TIP_INVENTORY      ), 'I', GETTEXT(TID_APP_INVENTORY), AUTH_GENERAL);
	AddAppletFunc( AppMain_WndWebBox       , APP_WEBBOX                  , _T( "WebBox" )          , _T( "Icon_CitemMall.dds" )    , GETTEXT( TID_TIP_ITEMMALL       ), 0 );

#if __VER >= 10 // __CSC_VER9_1
#ifdef __NEW_SKILL_TREE
	AddAppletFunc( AppMain_WndSkill2       , APP_SKILL4                  , _T( "WndSkill"     )    , _T( "icon_MenuSkill.dds"     )    , GETTEXT( TID_TIP_SKILL          ), 'K', GETTEXT(TID_APP_SKILL), AUTH_GENERAL);
#else // __NEW_SKILL_TREE
	AddAppletFunc( AppMain_WndSkill2       , APP_SKILL3                  , _T( "WndSkill"     )    , _T( "icon_MenuSkill.dds"     )    , GETTEXT( TID_TIP_SKILL          ), 'K', GETTEXT(TID_APP_SKILL), AUTH_GENERAL);
#endif // __NEW_SKILL_TREE
#else
	AddAppletFunc( AppMain_WndSkill2       , APP_SKILL1                  , _T( "WndSkill"     )    , _T( "icon_MenuSkill.dds"     )    , GETTEXT( TID_TIP_SKILL          ), 'K', GETTEXT(TID_APP_SKILL), AUTH_GENERAL);
#endif //__CSC_VER9_1

	AddAppletFunc( AppMain_WndMotion       , APP_MOTION                  , _T( "WndMotion"    )    , _T( "icon_MenuMotion.dds"   )     , GETTEXT( TID_TIP_MOTION         ),  'O', GETTEXT(TID_APP_MOTION), AUTH_GENERAL);
	AddAppletFunc( AppMain_WndTrade        , APP_TRADE                   , _T( "WndTrade"     )    , _T( "Icon_Trade.dds"     )    , GETTEXT( TID_TIP_TRADE          ),  0  );

#if __VER >= 8 // __S8_VENDOR_REVISION
	AddAppletFunc( AppMain_WndVendor	   , APP_VENDOR_REVISION         , _T( "WndVendor" )	   , _T( "icon_MenuPrivateShop.dds" )       , GETTEXT( TID_TIP_VENDOR ), 0 );
#else // __VER >= 8 // __S8_VENDOR_REVISION
	AddAppletFunc( AppMain_WndVendor	   , APP_VENDOREX	             , _T( "WndVendor" )	   , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_VENDOR ), 0 );
#endif // __VER >= 8 // __S8_VENDOR_REVISION

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	AddAppletFunc( AppMain_WndQuest        , APP_QUEST_EX_LIST           , _T( "WndQuest"     )    , _T( "icon_MenuQuest.dds"     )    , GETTEXT( TID_TIP_QUEST          ), 'Q', GETTEXT(TID_APP_QUEST), AUTH_GENERAL);
#else // __IMPROVE_QUEST_INTERFACE
	AddAppletFunc( AppMain_WndQuest        , APP_QUEST                   , _T( "WndQuest"     )    , _T( "icon_MenuQuest.dds"     )    , GETTEXT( TID_TIP_QUEST          ), 'Q', GETTEXT(TID_APP_QUEST), AUTH_GENERAL);
#endif // __IMPROVE_QUEST_INTERFACE
	AddAppletFunc( AppMain_WndParty        , APP_PARTY                   , _T( "WndParty"     )    , _T( "icon_MenuParty.dds"    )    , GETTEXT( TID_TIP_PARTY          ),  'P', GETTEXT(TID_APP_PARTY), AUTH_GENERAL);

	AddAppletFunc( AppMain_WndGuild        , APP_GUILD                   , _T( "WndGuild"     )    , _T( "icon_MenuGuild.dds"    )    , GETTEXT( TID_TIP_COMPANY          ),  'G', GETTEXT(TID_APP_COMPANY), AUTH_GENERAL);

#ifdef __GUILDVOTE
	AddAppletFunc( AppMain_WndGuildVote    , APP_GUILD_VOTE              , _T( "WndGuildVote")     , _T( "Icon_Troupe.dds"    )    , GETTEXT( TID_TIP_COMPANY          ),  'V', GETTEXT(TID_GAME_TOOLTIP_GUILDVOTE), AUTH_GENERAL);
#endif
	
#if __VER >= 11 // __SYS_POCKET
	#ifndef __TMP_POCKET
	AddAppletFunc( AppMain_BagEx       , APP_BAG_EX                , _T( "WndBagEx" )       , _T( "icon_MenuBackpack.dds"    )    , GETTEXT(TID_APP_BAG_EX), 'B', GETTEXT(TID_APP_BAG_EX), AUTH_GENERAL);
	#endif
#endif
	AddAppletFunc( AppMain_WndCommItem    , APP_COMM_ITEM                , _T( "WndCommItem"  )    , _T( "Icon_MenuDuration.dds" )    , GETTEXT( TID_TIP_ITEMTIME   ),  'J', GETTEXT(TID_APP_ITEMTIME), AUTH_GENERAL);
	AddAppletFunc( AppMain_WndUpgradeBase    , APP_TEST                  , _T( "WndUpgradeBase" )  , _T( "Icon_Troupe.dds"    )    , GETTEXT( TID_TIP_PARTY          ),  0 );
	AddAppletFunc( AppMain_WndPiercing    , APP_PIERCING                 , _T( "WndPiercing"     ) , _T( "Icon_Troupe.dds"    )    , GETTEXT( TID_TIP_PARTY          ),  0 );
	AddAppletFunc( AppMain_WndChat         , APP_COMMUNICATION_CHAT      , _T( "WndChat"      )    , _T( "icon_MenuChatting.dds"      )    , GETTEXT( TID_TIP_COMMUNICATION_CHAT    ),  0 );
	AddAppletFunc( AppMain_WndMessenger    , APP_MESSENGER_              , _T( "WndMessenger" )    , _T( "icon_MenuMessenger.dds"   )  , GETTEXT( TID_TIP_MESSENGER ),  'E', GETTEXT(TID_APP_MESSENGER), AUTH_GENERAL);

	AddAppletFunc( AppMain_WndOptSound     , APP_OPTION_SOUND            , _T( "WndOptSound" )     , _T( "Icon_OptSound.dds" )     , GETTEXT( TID_TIP_OPTION_SOUND          ),  0  );
	AddAppletFunc( AppMain_WndOptWindow    , APP_OPTION_WINDOW           , _T( "WndOptWindow" )    , _T( "Icon_Applet.dds"      )  , GETTEXT( TID_TIP_OPTION_WINDOW         ),  0  );
	AddAppletFunc( AppMain_WndOptMyInfo    , APP_OPTION_MYINFO           , _T( "WndOptMyInfo" )    , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_OPTION_MYINFO         ),  0  );
	AddAppletFunc( AppMain_WndOption	   , APP_OPTIONEX				 , _T( "WndOption" )	   , _T( "icon_MenuOption.dds" )	   , GETTEXT( TID_APP_OPTION				),	0, GETTEXT(TID_APP_OPTION), AUTH_GENERAL);

	AddAppletFunc( AppMain_WndInfoNotice   , APP_INFO_NOTICE             , _T( "WndInfoNotice" )   , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_INFO_NOTICE           ),  0  );
	AddAppletFunc( AppMain_WndHelpHelp     , APP_HELPER_HELP             , _T( "WndHelpHelp" )     , _T( "icon_MenuHelp.dds" )   , GETTEXT( TID_TIP_HELPER_HELP           ),  0  );
	AddAppletFunc( AppMain_WndHelpTip      , APP_HELPER_TIP              , _T( "WndHelpTip" )      , _T( "Icon_HelperTip.dds" )    , GETTEXT( TID_TIP_HELPER_TIP            ),  0  );
	AddAppletFunc( AppMain_WndHelpFAQ      , APP_HELPER_FAQ              , _T( "WndHelpFAQ" )      , _T( "Icon_HelperFAQ.dds" )    , GETTEXT( TID_TIP_HELPER_FAQ            ),  0  );

	AddAppletFunc( AppMain_WndLogOut       , APP_LOGOUT                  , _T( "WndLogout"   )     , _T( "icon_MenuLogout.dds"   )     , GETTEXT( TID_TIP_LOGOUT              ),  0  );
	AddAppletFunc( AppMain_WndQuit         , APP_QUIT                    , _T( "WndQuit"      )    , _T( "icon_MenuQuit.dds"      )    , GETTEXT( TID_TIP_QUIT                  ),  0  );
//	if( GetLanguage() == LANG_TWN )
		AddAppletFunc( AppMain_WndWebBox2  , APP_WEBBOX2                 , _T( "WebBox2" )         , _T( "QOODO.dds" )             , GETTEXT( TID_TIP_QOODO	    ), 0 );

	AddAppletFunc( AppMain_WndWorld        , APP_WORLD                   , _T( "WndWorld" )        , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_WORLD     ),  0  );
	AddAppletFunc( AppMain_WndDebugInfo    , APP_DEBUGINFO               , _T( "WndDebugInfo" )    , _T( "icon_MenuDebug.dds" )       , GETTEXT( TID_TIP_DEBUGINFO ),  0  );

#ifdef __AEGON_ADVANCED_LOGIN
	AddAppletFunc(AppMain_WndLogin, APP_AEGON_LOGIN, _T("WndLogin"), _T("Icon_Login.dds"), GETTEXT(TID_TIP_APPLET), 0);
#else // __AEGON_ADVANCED_LOGIN
	AddAppletFunc(AppMain_WndLogin, APP_LOGIN, _T("WndLogin"), _T("Icon_Login.dds"), GETTEXT(TID_TIP_APPLET), 0);
#endif // __AEGON_ADVANCED_LOGIN

	AddAppletFunc( AppMain_WndSelectServer , APP_SELECT_SERVER           , _T( "WndSelectServer")  , _T( "Icon_SelectServer.dds")  , GETTEXT( TID_TIP_WORLD     ),  0  );
	AddAppletFunc( AppMain_WndCreateChar   , APP_CREATE_CHAR             , _T( "WndCreateChar" )   , _T( "Icon_CreateChar.dds" )   , GETTEXT( TID_TIP_WORLD     ),  0  );
	AddAppletFunc( AppMain_WndSelectChar   , APP_SELECT_CHAR             , _T( "WndSelectChar" )   , _T( "Icon_SelectChar.dds" )   , GETTEXT( TID_TIP_DIALOG    ),  0  );

#if __VER >= 8 //__CSC_VER8_2
	AddAppletFunc( AppMain_WndPartyQuick   , APP_PARTY_QUICK             , _T( "WndPartyQuick" )   , NULL   , GETTEXT( TID_TIP_DIALOG    ),  0  );
#endif //__CSC_VER8_2
#if __VER >= 8 //__CSC_VER8_3
	AddAppletFunc( AppMain_WndBuffStatus   , APP_BUFF_STATUS             , _T( "WndBuffStatus" )   , NULL   , GETTEXT( TID_TIP_DIALOG    ),  0  );
#endif //__CSC_VER8_3
#if __VER >= 13 // __CSC_VER13_2
	if( ::GetLanguage() == LANG_FRE )
		AddAppletFunc( AppMain_Couple   , APP_COUPLE_MAIN             , _T( "WndCoupleManager" )   , _T( "icon_MenuCouple.dds" )   , GETTEXT( TID_GAME_COUPLE ),  'X', GETTEXT(TID_GAME_COUPLE), AUTH_GENERAL);
	else
		AddAppletFunc( AppMain_Couple   , APP_COUPLE_MAIN             , _T( "WndCoupleManager" )   , _T( "icon_MenuCouple.dds" )   , GETTEXT( TID_GAME_COUPLE ),  'F', GETTEXT(TID_GAME_COUPLE), AUTH_GENERAL);
#endif //__CSC_VER13_2
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	AddAppletFunc( AppMain_Wnd2ndPassword  , APP_2ND_PASSWORD_NUMBERPAD  , _T( "Wnd2ndPassword" )   , _T( "Icon_Applet.dds" )   , GETTEXT( TID_2ND_PASSWORD_WINDOW_OPEN ),  0  );
#endif // __2ND_PASSWORD_SYSTEM

#ifdef __NEW_WEB_BOX
	AddAppletFunc( AppMain_WndHelperWebBox , APP_WEBBOX2              , _T( "HelperWebBox" )       , _T( "Icon_HelperHelp.dds" ) , GETTEXT( TID_GAME_HELPER_WEB_BOX_ICON_TOOLTIP ), 0 );
#endif // __NEW_WEB_BOX

/******************************************** TITANIUM MMO ADDITIONS ********************************************/

// Can be changed with Change Hotkey below !

#ifdef __WIKIPEDIA_ITEMS
	AddAppletFunc(AppMain_WikiItems, APP_WIKI_ITEMS, _T("WndWikiItems"), _T("icon_MenuWiki.dds"), _T("Wikipedia - Items"), VK_F12, _T("Wikipedia - Items"), AUTH_GENERAL);
#endif //__WIKIPEDIA_ITEMS

#ifdef __WIKIPEDIA_TITLE
	AddAppletFunc(AppMain_WndWikipediaTitle, APP_WIKIPEDIA_TITLE, _T("WndHonorTitle"), _T("icon_MenuWikiTitiles.dds"), _T("Wikipedia - Titles"), 0, _T("Wikipedia - Titles"), AUTH_GENERAL);
#endif // __WIKIPEDIA_TITLE

#ifdef __WIKIPEDIA_JOB
	AddAppletFunc(AppMain_WndWikipediaJob, APP_WIKIPEDIA_JOB, _T("WikipediaJob"), _T("icon_MenuWikiJob.dds"), _T("Wikipedia - Jobs"), 0, _T("Wikipedia - Jobs"), AUTH_GENERAL);
#endif // __WIKIPEDIA_JOB

#ifdef __WIKIPEDIA_MONSTER
	AddAppletFunc(AppMain_MonsterWiki, APP_WIKIPEDIA_MONSTER, _T("WndMonsterWiki"), _T("icon_MenuWikiMon.dds"), _T("Wikipedia - Monsters"), 0, _T("Wikipedia - Monsters"), AUTH_GENERAL);
#endif // __WIKIPEDIA_MONSTER

#ifdef __AEGON_TELEPORTER
	AddAppletFunc(AppMain_WndTeleporter, APP_TELEPORTER, _T("WndTeleporterSys"), _T("icon_MenuTeleport.dds"), _T("Teleporter"), 'V', _T("Teleporter"), AUTH_GENERAL);
#endif

#ifdef __AEGON_MONSTER_HUNT
	AddAppletFunc(AppMain_WndMonsterHunt, APP_MONSTER_HUNT, _T("WndMonsterHunt"), _T("icon_MenuDamagedone.dds"), _T("Monster Hunt"), 0, _T("Monster Hunt"), AUTH_GENERAL);
#endif // __AEGON_MONSTER_HUNT

#ifdef __SYS_PARTYFINDER
	AddAppletFunc(AppMain_WndPartyFind, APP_PARTY_FIND, _T("WndPartyFind"), _T("icon_MenuParty.dds"), _T("Party Finder"), 'L', _T("Party Finder"), AUTH_GENERAL);
#endif //__SYS_PARTYFINDER

#ifdef __SYS_PETFILTER
	AddAppletFunc(AppMain_WndPetfilter, APP_PETFILTER_MAIN, _T("WndPetfilter"), _T("icon_MenuPetfilter.dds"), _T("Pet Filter"), 'U', _T("Pet Filter"), AUTH_GENERAL);
#endif //__SYS_PETFILTER
	
#ifdef __AEGON_LOOT_BOX
	AddAppletFunc(AppMain_WndLootBox, APP_LOOTBOX, _T("WndLootBox"), _T("icon_MenuLootbox.png"), _T("Lootbox"), 0, _T("Lootbox"), AUTH_GENERAL);
#endif // __AEGON_LOOT_BOX

#ifdef __AEGON_SEASON_PASS
	AddAppletFunc(AppMain_WndSeasonPass, APP_SEASON_PASS, _T("WndSeasonPass"), _T("icon_MenuSP.png"), _T("Season Pass"), 0, _T("Season Pass"), AUTH_GENERAL);
#endif // __AEGON_SEASON_PASS

#ifdef __AEGON_PVP_RANKING
	AddAppletFunc(AppMain_WndPvPRankingMain, APP_PVP_RANKING, _T("WndPvPRanking"), _T("icon_MenuPvPRanking.dds"), _T("PvP Ranking"), 0, _T("PvP Ranking"), AUTH_GENERAL);
#endif // __AEGON_PVP_RANKING

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
	AddAppletFunc(AppMain_WndAchievementSystem, APP_AEGON_ACHIEVEMENT_SYSTEM, _T("WndAchievementSystem"), _T("icon_MenuAchievement.png"), _T("Achievement System"), 0, _T("Achievement System"), AUTH_GENERAL);
#endif // __AEGON_ACHIEVEMENT_SYSTEM

#ifdef __FL_FARM_STAT
	AddAppletFunc(AppMain_FarmCounter, APP_FARM_COUNTER, _T("WndFarmCounter"), _T("icon_MenuFarmCounter.dds"), _T("Farm Counter"), 0, _T("Farm Counter"), AUTH_GENERAL);
#endif //__FL_FARM_STAT

// Can NOT be changed with Change Hotkey below !

#ifdef __AEGON_AWAKEWINDOW
	AddAppletFunc(AppMain_WndAwakening, APP_AWAKENING, _T("WndAwakening"), _T("Icon_Infopang.dds"), GETTEXT(TID_GAME_AWAKENING), 0);
#endif //__AEGON_AWAKEWINDOW

#ifdef __PREMIUM
	AddAppletFunc(AppMain_WndPremiumMenu, APP_PREMIUM_MENU, _T("WndPremiumMenu"), _T("Icon_MenuPremium.png"), _T("Premium Menu"), 0);
#endif //__PREMIUM

#ifdef __LEEAEG_SHOP_FINDER
	AddAppletFunc(AppMain_ShopFinder, APP_SHOP_FINDER, _T("WndShopFinder"), _T("Icon_Infopang.dds"), _T("Shop Finder"), 0);
#endif // __LEEAEG_SHOP_FINDER

#ifdef __MODEL_VIEW
	AddAppletFunc(AppMain_ModelView, APP_MODEL_VIEW, _T("WndModelView"), _T("Icon_Infopang.dds"), _T("Model Viewer"), 0);
#endif //__MODEL_VIEW

#ifdef __GIFTBOX_LIST_ITEMS
	AddAppletFunc(AppMain_GBPreview, APP_GIFTBOX, _T("WndGiftboxPreview"), _T("Icon_Infopang.dds"), _T("Giftbox Preview"), 0);
#endif //__GIFTBOX_LIST_ITEMS

#ifdef __NEW_CHANGE_JOB
	AddAppletFunc(AppMain_WndNewChangeJob, APP_CHANGE_JOB_01, _T("WndNewChangeJob"), _T("Icon_Infopang.dds"), prj.GetText(TID_CHANGE_JOB), 0);
	AddAppletFunc(AppMain_WndNewChangeJob2, APP_CHANGE_JOB_02, _T("WndNewChangeJob2"), _T("Icon_Infopang.dds"), prj.GetText(TID_CHANGE_JOB), 0);
	AddAppletFunc(AppMain_WndNewChangeJob3, APP_CHANGE_JOB_03, _T("WndNewChangeJob3"), _T("Icon_Infopang.dds"), prj.GetText(TID_CHANGE_JOB), 0);
#endif // __NEW_CHANGE_JOB

#ifdef __WEAPON_RARITY
	AddAppletFunc(AppMain_WndWeaponRarity, APP_WEAPON_RARITY, _T("WndWeaponRarity"), _T("Icon_Infopang.dds"), prj.GetText(TID_WR_TEXT_001), 0);
#endif // __WEAPON_RARITY

#ifdef __ANARCHY_SYSTEM
	AddAppletFunc(AppMain_AnarchySkill, APP_ANARCHY_SKILL, _T("WndAnarchySkill"), _T("icon_MenuAnarchy.dds"), _T("Anarchy Buffs"), 0);
#endif // __ANARCHY_SYSTEM

#ifdef __GUILDFINDER
	AddAppletFunc(AppMain_WndGuildFinder, APP_GUILDFINDER, _T("WndGuildFinder"), _T("icon_MenuGuild.dds"), _T("Guild Finder"), 0);
#endif //__GUILDFINDER

#ifdef __REPORT_SYSTEM
	AddAppletFunc(AppMain_WndReport, APP_REPORT, _T("WndReport"), _T("icon_MenuReport.png"), ("Report Problem"), 0);
#endif // __REPORT_SYSTEM

#ifdef __TASKBAR_OPEN_SITE
	AddAppletFunc(AppMain_OpenHomepage, APP_HOMEPAGE, _T("Homepage"), _T("icon_MenuHomepage.png"), _T("Check our website!"), 0);
	AddAppletFunc(AppMain_OpenDiscord, APP_DISCORD, _T("Discord"), _T("icon_MenuDiscord.png"), _T("Join our discord server!"), 0);
#endif //__TASKBAR_OPEN_SITE

#ifdef __SYS_AUTO_SHOUT
	AddAppletFunc(AppMain_WndAutoShout, APP_AUTO_SHOUT, _T("WndAutoShout"), _T("Icon_Applet.dds"), "Auto Shout", 0);
#endif // __SYS_AUTO_SHOUT

#ifdef __PLAYER_LIST
	AddAppletFunc(AppMain_WndPlayerList, APP_PLAYER_LIST, _T("WndPlayerList"), _T("icon_MenuCharacter.dds"), _T("Player List"), 0);
#endif // __PLAYER_LIST

//#ifdef __BAN_SYSTEM
//	AddAppletFunc(AppMain_WndPlayerPurge, APP_PLAYER_PURGE, _T("WndPlayerPurge"), _T("icon_MenuCharacter.dds"), _T("Player Purge"), 0);
//#endif //__BAN_SYSTEM

#ifdef __SWITCH_EQUIP
	AddAppletFunc(AppMain_WndSwitchEquipment, APP_SWITCHEQUIP, _T("WndSwitchEquipment"), _T("Icon_Infopang.dds"), _T("Switch Equipment"), 0);
#endif // __SWITCH_EQUIP

#ifdef __MODS_MANAGER
	AddAppletFunc(AppMain_WndModsManager, APP_MODS_MANAGER, _T("WndModsManager"), _T("Icon_Infopang.dds"), _T("Mods Manager"), 0, _T("Mods Manager"), 0);
#endif // __MODS_MANAGER

#ifdef __AZURIOM_IMAGES
	AddAppletFunc(AppMain_WndAzuriomImage, APP_AZURIOM_IMAGE, _T("WndAzuriomImage"), _T("icon_MenuWebsiteImage.dds"), _T("Azuriom Image"), 0, _T("Azuriom Image"), 0);
#endif //__AZURIOM_IMAGES

#ifdef __AEGON_INGAME_SHOP
	AddAppletFunc(AppMain_WndIngameShopMain, APP_AEGON_INGAME_SHOP, _T("WndIngameShop"), _T("Icon_Menu_ItemShop.dds"), _T("Ingame Shop"), 0, _T("Ingame Shop"), AUTH_GENERAL);
#endif //__AEGON_INGAME_SHOP

#ifdef __AEGON_TOURNAMENT
	AddAppletFunc(AppMain_WndCreateTournament, APP_AEGON_CREATE_TOURNAMENT, _T("WndCreateTournament"), _T("Icon_Infopang.dds"), _T("Create Tournament"), 0, _T("Create Tournament"), AUTH_ADMINISTRATOR);
#endif

#ifdef __AEGON_F_TOOL
	AddAppletFunc(AppMain_WndFTool, APP_AEGON_FTOOL, _T("WndFTool"), _T("Icon_Menu_FTool.dds"), _T("F-Tool"), 0, _T("F-Tool"), AUTH_GENERAL);
#endif

#ifdef __AEGON_MESSENGER
	AddAppletFunc(AppMain_WndAegonMessenger, APP_AEGON_MESSENGER, _T("WndMessenger"), _T("icon_MenuChatting.dds"), _T("Messenger"), 0, _T("Messenger"), AUTH_GENERAL);
#endif

#ifdef __AEGON_PVP_MINIGAMES
	AddAppletFunc(AppMain_WndAegonPvPMinigamesCreate, APP_PVP_MINIGAME_CREATE, _T("WndMinigameCreate"), _T("Icon_Infopang.dds"), _T("MinigameCreate"), 0, _T("MinigameCreate"), AUTH_GAMEMASTER);
	AddAppletFunc(AppMain_WndAegonPvPMinigamesFinder, APP_PVP_MINIGAME_FINDER, _T("WndMinigameFinder"), _T("Icon_Infopang.dds"), _T("MinigameFinder"), 0, _T("MinigameFinder"), AUTH_GAMEMASTER);
#endif

/******************************************** TITANIUM MMO ADDITIONS ********************************************/

#else //__CHANGE_HOT_KEY
	AddAppletFunc( AppMain_WndNavigator    , APP_NAVIGATOR               , _T( "WndNavigator" )    , _T( "Icon_Navigator.dds" )    , GETTEXT( TID_TIP_NAVIGATOR      ),  'N'  );	
	AddAppletFunc( AppMain_WndStatus       , APP_STATUS1                 , _T( "WndStatus" )       , _T( "Icon_Status.dds"    )    , GETTEXT( TID_TIP_STATUS         ), 'T' );
#if __VER >= 9  // __INSERT_MAP
#ifdef __IMPROVE_MAP_SYSTEM
	AddAppletFunc( AppMain_WndMap       , APP_MAP_EX              , _T( "WndMap" )       , _T( "Icon_Applet.dds"    )    , GETTEXT(TID_TIP_MAP), 'M' );
#else // __IMPROVE_MAP_SYSTEM
	AddAppletFunc( AppMain_WndMap       , APP_MAP                 , _T( "WndMap" )       , _T( "Icon_Applet.dds"    )    , GETTEXT(TID_TIP_MAP), 'M' );
#endif // __IMPROVE_MAP_SYSTEM
#endif
#if __VER >= 12 // __LORD
	if( ::GetLanguage() == LANG_FRE )
		AddAppletFunc( AppMain_LordSkill       , APP_LORD_SKILL             , _T( "WndLordSkill" )       , _T( "Icon_Infopang.dds"    )    , GETTEXT(TID_TIP_INFOPANG), 0 );
	else
		AddAppletFunc( AppMain_LordSkill       , APP_LORD_SKILL             , _T( "WndLordSkill" )       , _T( "Icon_Infopang.dds"    )    , GETTEXT(TID_TIP_INFOPANG), 0 );
#endif	// __LORD
#if __VER >= 12 // __MOD_TUTORIAL
	AddAppletFunc( AppMain_InfoPang       , APP_INFOPANG                , _T( "WndInfoPang" )       , _T( "Icon_Infopang.dds"    )    , GETTEXT(TID_TIP_INFOPANG), 0 );
#endif
#if __VER >= 13 // __HOUSING
	AddAppletFunc( AppMain_Housing       , APP_HOUSING                , _T( "WndHousing" )       , _T( "Icon_Housing.dds"    )    , GETTEXT(TID_GAME_HOUSING_BOX), 'Y' );
#endif // __HOUSING

#if __VER >= 15 // __GUILD_HOUSE
	AddAppletFunc( AppMain_GuildHousing     , APP_GH_FURNITURE_STORAGE, _T( "WndGuildHousing" )     , _T( "Icon_Housing.dds"    )    , GETTEXT(TID_GAME_HOUSING_BOX), 'R' );
#endif 
#if __VER >= 13 // __RENEW_CHARINFO
	AddAppletFunc( AppMain_WndCharacter    , APP_CHARACTER3               , _T( "WndCharacter" )    , _T( "Icon_Character.dds" )    , GETTEXT( TID_TIP_CHARACTER      ), 'H' );
#elif __VER >= 9 // __CSC_VER9_2
	AddAppletFunc( AppMain_WndCharacter    , APP_CHARACTER2               , _T( "WndCharacter" )    , _T( "Icon_Character.dds" )    , GETTEXT( TID_TIP_CHARACTER      ), 'H' );
#else //__CSC_VER9_2
	AddAppletFunc( AppMain_WndCharacter    , APP_CHARACTER               , _T( "WndCharacter" )    , _T( "Icon_Character.dds" )    , GETTEXT( TID_TIP_CHARACTER      ), 'H' );
#endif //__CSC_VER9_2
	AddAppletFunc( AppMain_WndInventory    , APP_INVENTORY               , _T( "WndInventory" )    , _T( "Icon_Inventory.dds" )    , GETTEXT( TID_TIP_INVENTORY      ), 'I' );
	
	AddAppletFunc( AppMain_WndWebBox       , APP_WEBBOX                  , _T( "WebBox" )          , _T( "Icon_CitemMall.dds" )    , GETTEXT( TID_TIP_ITEMMALL       ), 0 );
#if __VER >= 10 // __CSC_VER9_1
#ifdef __NEW_SKILL_TREE
	AddAppletFunc( AppMain_WndSkill2       , APP_SKILL4                  , _T( "WndSkill"     )    , _T( "Icon_Skill.dds"     )    , GETTEXT( TID_TIP_SKILL          ), 'K' );
#else // __NEW_SKILL_TREE
	AddAppletFunc( AppMain_WndSkill2       , APP_SKILL3                  , _T( "WndSkill"     )    , _T( "Icon_Skill.dds"     )    , GETTEXT( TID_TIP_SKILL          ), 'K' );
#endif // __NEW_SKILL_TREE
#else
	AddAppletFunc( AppMain_WndSkill2       , APP_SKILL1                  , _T( "WndSkill"     )    , _T( "Icon_Skill.dds"     )    , GETTEXT( TID_TIP_SKILL          ), 'K' );
#endif //__CSC_VER9_1

	AddAppletFunc( AppMain_WndMotion       , APP_MOTION                  , _T( "WndMotion"    )    , _T( "Icon_Motion.dds"   )     , GETTEXT( TID_TIP_MOTION         ),  'O'  );
	AddAppletFunc( AppMain_WndTrade        , APP_TRADE                   , _T( "WndTrade"     )    , _T( "Icon_Trade.dds"     )    , GETTEXT( TID_TIP_TRADE          ),  0  );

#if __VER >= 8 // __S8_VENDOR_REVISION
	AddAppletFunc( AppMain_WndVendor	   , APP_VENDOR_REVISION         , _T( "WndVendor" )	   , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_VENDOR ), 0 );
#else // __VER >= 8 // __S8_VENDOR_REVISION
	AddAppletFunc( AppMain_WndVendor	   , APP_VENDOREX	             , _T( "WndVendor" )	   , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_VENDOR ), 0 );
#endif // __VER >= 8 // __S8_VENDOR_REVISION

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	AddAppletFunc( AppMain_WndQuest        , APP_QUEST_EX_LIST           , _T( "WndQuest"     )    , _T( "Icon_Quest.dds"     )    , GETTEXT( TID_TIP_QUEST          ), g_Neuz.Key.chQuest );
#else // __IMPROVE_QUEST_INTERFACE
	AddAppletFunc( AppMain_WndQuest        , APP_QUEST                   , _T( "WndQuest"     )    , _T( "Icon_Quest.dds"     )    , GETTEXT( TID_TIP_QUEST          ), g_Neuz.Key.chQuest );
#endif // __IMPROVE_QUEST_INTERFACE
	AddAppletFunc( AppMain_WndParty        , APP_PARTY                   , _T( "WndParty"     )    , _T( "Icon_Troupe.dds"    )    , GETTEXT( TID_TIP_PARTY          ),  'P' );

	AddAppletFunc( AppMain_WndGuild        , APP_GUILD                   , _T( "WndGuild"     )    , _T( "Icon_Troupe.dds"    )    , GETTEXT( TID_TIP_COMPANY          ),  'G' );

#ifdef __GUILDVOTE
	AddAppletFunc( AppMain_WndGuildVote    , APP_GUILD_VOTE              , _T( "WndGuildVote")     , _T( "Icon_Troupe.dds"    )    , GETTEXT( TID_TIP_COMPANY          ),  'V' );
#endif
	
#if __VER >= 11 // __SYS_POCKET
	#ifndef __TMP_POCKET
	AddAppletFunc( AppMain_BagEx       , APP_BAG_EX                , _T( "WndBagEx" )       , _T( "Icon_Inventory.dds"    )    , GETTEXT(TID_APP_BAG_EX), 'B' );
	#endif
#endif
	AddAppletFunc( AppMain_WndCommItem    , APP_COMM_ITEM                , _T( "WndCommItem"  )    , _T( "Icon_CItemTime.dds" )    , GETTEXT( TID_TIP_ITEMTIME   ),  'J' );
	AddAppletFunc( AppMain_WndUpgradeBase    , APP_TEST                  , _T( "WndUpgradeBase" )  , _T( "Icon_Troupe.dds"    )    , GETTEXT( TID_TIP_PARTY          ),  0 );
	AddAppletFunc( AppMain_WndPiercing    , APP_PIERCING                 , _T( "WndPiercing"     ) , _T( "Icon_Troupe.dds"    )    , GETTEXT( TID_TIP_PARTY          ),  0 );
	AddAppletFunc( AppMain_WndChat         , APP_COMMUNICATION_CHAT      , _T( "WndChat"      )    , _T( "Icon_Chat.dds"      )    , GETTEXT( TID_TIP_COMMUNICATION_CHAT    ),  0 );
	AddAppletFunc( AppMain_WndMessenger    , APP_MESSENGER_              , _T( "WndMessenger" )    , _T( "Icon_Messenger.dds"   )  , GETTEXT( TID_TIP_MESSENGER ),  'E'  );

	AddAppletFunc( AppMain_WndOptSound     , APP_OPTION_SOUND            , _T( "WndOptSound" )     , _T( "Icon_OptSound.dds" )     , GETTEXT( TID_TIP_OPTION_SOUND          ),  0  );
	AddAppletFunc( AppMain_WndOptWindow    , APP_OPTION_WINDOW           , _T( "WndOptWindow" )    , _T( "Icon_Applet.dds"      )  , GETTEXT( TID_TIP_OPTION_WINDOW         ),  0  );
	AddAppletFunc( AppMain_WndOptMyInfo    , APP_OPTION_MYINFO           , _T( "WndOptMyInfo" )    , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_OPTION_MYINFO         ),  0  );
	AddAppletFunc( AppMain_WndOption	   , APP_OPTIONEX				 , _T( "WndOption" )	   , _T( "Icon_Applet.dds" )	   , GETTEXT( TID_APP_OPTION				),	0  );			

	AddAppletFunc( AppMain_WndInfoNotice   , APP_INFO_NOTICE             , _T( "WndInfoNotice" )   , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_INFO_NOTICE           ),  0  );
	AddAppletFunc( AppMain_WndHelpHelp     , APP_HELPER_HELP             , _T( "WndHelpHelp" )     , _T( "Icon_HelperHelp.dds" )   , GETTEXT( TID_TIP_HELPER_HELP           ),  0  );
	AddAppletFunc( AppMain_WndHelpTip      , APP_HELPER_TIP              , _T( "WndHelpTip" )      , _T( "Icon_HelperTip.dds" )    , GETTEXT( TID_TIP_HELPER_TIP            ),  0  );
	AddAppletFunc( AppMain_WndHelpFAQ      , APP_HELPER_FAQ              , _T( "WndHelpFAQ" )      , _T( "Icon_HelperFAQ.dds" )    , GETTEXT( TID_TIP_HELPER_FAQ            ),  0  );

	AddAppletFunc( AppMain_WndLogOut       , APP_LOGOUT                  , _T( "WndLogout"   )     , _T( "Icon_Logout.dds"   )     , GETTEXT( TID_TIP_LOGOUT              ),  0  );
	AddAppletFunc( AppMain_WndQuit         , APP_QUIT                    , _T( "WndQuit"      )    , _T( "Icon_Quit.dds"      )    , GETTEXT( TID_TIP_QUIT                  ),  0  );
//	if( GetLanguage() == LANG_TWN )
		AddAppletFunc( AppMain_WndWebBox2  , APP_WEBBOX2                 , _T( "WebBox2" )         , _T( "QOODO.dds" )             , GETTEXT( TID_TIP_QOODO	    ), 0 );

	AddAppletFunc( AppMain_WndWorld        , APP_WORLD                   , _T( "WndWorld" )        , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_WORLD     ),  0  );
	AddAppletFunc( AppMain_WndDebugInfo    , APP_DEBUGINFO               , _T( "WndDebugInfo" )    , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_DEBUGINFO ),  0  );
#ifdef __AEGON_ADVANCED_LOGIN
	AddAppletFunc(AppMain_WndLogin, APP_AEGON_LOGIN, _T("WndLogin"), _T("Icon_Login.dds"), GETTEXT(TID_TIP_APPLET), 0);
#else // __AEGON_ADVANCED_LOGIN
	AddAppletFunc(AppMain_WndLogin, APP_LOGIN, _T("WndLogin"), _T("Icon_Login.dds"), GETTEXT(TID_TIP_APPLET), 0);
#endif // __AEGON_ADVANCED_LOGIN

	AddAppletFunc( AppMain_WndSelectServer , APP_SELECT_SERVER           , _T( "WndSelectServer")  , _T( "Icon_SelectServer.dds")  , GETTEXT( TID_TIP_WORLD     ),  0  );
	AddAppletFunc( AppMain_WndCreateChar   , APP_CREATE_CHAR             , _T( "WndCreateChar" )   , _T( "Icon_CreateChar.dds" )   , GETTEXT( TID_TIP_WORLD     ),  0  );
	AddAppletFunc( AppMain_WndSelectChar   , APP_SELECT_CHAR             , _T( "WndSelectChar" )   , _T( "Icon_SelectChar.dds" )   , GETTEXT( TID_TIP_DIALOG    ),  0  );

#if __VER >= 8 //__CSC_VER8_2
	AddAppletFunc( AppMain_WndPartyQuick   , APP_PARTY_QUICK             , _T( "WndPartyQuick" )   , NULL   , GETTEXT( TID_TIP_DIALOG    ),  0  );
#endif //__CSC_VER8_2
#if __VER >= 8 //__CSC_VER8_3
	AddAppletFunc( AppMain_WndBuffStatus   , APP_BUFF_STATUS             , _T( "WndBuffStatus" )   , NULL   , GETTEXT( TID_TIP_DIALOG    ),  0  );
#endif //__CSC_VER8_3
#if __VER >= 13 // __CSC_VER13_2
	if( ::GetLanguage() == LANG_FRE )
		AddAppletFunc( AppMain_Couple   , APP_COUPLE_MAIN             , _T( "WndCoupleManager" )   , _T( "Icon_Couple.dds" )   , GETTEXT( TID_GAME_COUPLE ),  'X' );
	else
		AddAppletFunc( AppMain_Couple   , APP_COUPLE_MAIN             , _T( "WndCoupleManager" )   , _T( "Icon_Couple.dds" )   , GETTEXT( TID_GAME_COUPLE ),  'F' );
#endif //__CSC_VER13_2
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	AddAppletFunc( AppMain_Wnd2ndPassword  , APP_2ND_PASSWORD_NUMBERPAD  , _T( "Wnd2ndPassword" )   , _T( "Icon_Applet.dds" )   , GETTEXT( TID_2ND_PASSWORD_WINDOW_OPEN ),  0  );
#endif // __2ND_PASSWORD_SYSTEM

#ifdef __NEW_WEB_BOX
	AddAppletFunc( AppMain_WndHelperWebBox , APP_WEBBOX2              , _T( "HelperWebBox" )       , _T( "Icon_HelperHelp.dds" ) , GETTEXT( TID_GAME_HELPER_WEB_BOX_ICON_TOOLTIP ), 0 );
#endif // __NEW_WEB_BOX
#endif //__CHANGE_HOT_KEY
}
