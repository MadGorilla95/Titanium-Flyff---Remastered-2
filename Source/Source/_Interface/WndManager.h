// WndArcane.h: interface for the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDMANAGER_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDMANAGER_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __RT_1025
#include "rtmessenger.h"
#else	// __RT_1025
#include "messenger.h"
#endif	// __RT_1025
#include "WndWorld.h"
#include "WndBank.h"
#include "WndRepairItem.h"	 // 수리창
#include "WndGuildWarPeaceConfirm.h"
#include "WndGuildWarRequest.h"
#include "WndUpgradebase.h"
#include "WndCommItem.h"
#include "WndPvp.h"

#include "WndPiercing.h"

#include "WndInvenRemoveItem.h"
#include "WndCollecting.h"

#if __VER >= 11 // __SYS_IDENTIFY
#include "WndAwakening.h"
#include "WndLvReqDown.h"
#include "WndBlessingCancel.h"
#endif

#if __VER >= 12 // __LORD
#include "WndLord.h"
#endif

#include "WndPetRes.h"

#if __VER >= 8 //__CSC_VER8_5
#include "WndSummonAngel.h"
#endif //__CSC_VER8_5

#ifdef __EVE_MINIGAME
#include "WndMiniGame.h"
#endif //__EVE_MINIGAME

#if __VER >= 9 // __CSC_VER9_1
#include "WndPetSys.h"
#endif //__CSC_VER9_1

#if __VER >= 11 // __GUILD_COMBAT_1TO1
#include "WndGuildCombat1to1.h"
#endif //__GUILD_COMBAT_1TO1

#if __VER >= 12 // __SECRET_ROOM
#include "WndSecretRoom.h"
#endif //__SECRET_ROOM
#ifdef __AZRIA_1023
#include "WndSelectCh.h"
#endif
#ifdef __HELP_BUG_FIX
#include "WndHelp.h"
#endif //__HELP_BUG_FIX

#if __VER >= 13 // __RAINBOW_RACE
#include "WndRainbowRace.h"
#endif //__RAINBOW_RACE
#if __VER >= 13 // __HOUSING
#include "WndRoomList.h"
#include "WndQuitRoom.h"
#endif // __HOUSING
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
#include "WndQuestQuickInfo.h"
#include "WndQuest.h"
#endif // __IMPROVE_QUEST_INTERFACE

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
#include "WndChattingBlockingList.h"
#endif // __YS_CHATTING_BLOCKING_SYSTEM

#ifdef __GUILD_HOUSE_MIDDLE
#include "WndHousing.h"
#endif //__GUILD_HOUSE_MIDDLE

#ifdef	__COLOSSEUM
#include "WndColosseum.h"
#endif	// __COLOSSEUM

#ifdef __AEGON_MONSTER_HUNT
#include "WndMonsterHunt.h"
#endif // __AEGON_MONSTER_HUNT

#ifdef __PREMIUM
#include "WndPremiumMenu.h"
#endif //__PREMIUM

#ifdef __LEEAEG_SHOP_FINDER
#include "WndShopFinder.h"
#endif //__LEEAEG_SHOP_FINDER

#ifdef __GIFTBOX_LIST_ITEMS
#include "WndGiftbox.h"
#endif //__GIFTBOX_LIST_ITEMS

#ifdef __SYS_CUSTOM_TITLE
#include "WndCreateOwnTitle.h"
#endif //__SYS_CUSTOM_TITLE

#ifdef __SYS_PARTYFINDER
#include "WndPartyFind.h"
#endif //__SYS_PARTYFINDER

#ifdef __AEGON_LOOT_BOX
#include "WndAegonLootBox.h"
#endif // __AEGON_LOOT_BOX

#ifdef __AEGON_TOURNAMENT
#include "WndAegonTournament.h"
#endif // __AEGON_TOURNAMENT

#ifdef __AEGON_VIEW_INV
#include "WndViewInv.h"
#endif // __AEGON_VIEW_INV

#ifdef __SYS_PETFILTER
#include "WndPetfilter.h"
#endif //__SYS_PETFILTER

#ifdef __SYS_GUILDCOLOR
#include "WndGuildColor.h"
#endif //__SYS_GUILDCOLOR

#ifdef __REPORT_SYSTEM
#include "WndReport.h"
#endif //__REPORT_SYSTEM

#ifdef __TASKBAR_OPEN_SITE
#include "WndSites.h"
#endif //__TASKBAR_OPEN_SITE

#ifdef __SYS_MONSTER_CLASH
class CWndMonsterClashRegister;
class CWndMonsterClashRegisterMember;
#endif //__SYS_MONSTER_CLASH

#ifdef __SYS_SANDBOX
#include "WndSandbox.h"
#endif //__SYS_SANDBOX

#ifdef __AEGON_SEASON_PASS
#include "WndAegonSeasonPass.h"
#endif // __AEGON_SEASON_PASS

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
#include "WndAegonAchievementSystem.h"
#endif // __AEGON_ACHIEVEMENT_SYSTEM

#ifdef __SYS_NAMECOLOR
#include "WndNameColor.h"
#endif //__SYS_NAMECOLOR

#ifdef __AEGON_LAST_ONLINE
#include "AegonLastOnlineMng.h"
#endif // __AEGON_LAST_ONLINE

#ifdef __AEGON_CRAFTING_SYSTEM
#include "WndAegonCrafting.h"
#endif // __AEGON_CRAFTING_SYSTEM

#ifdef __WEAPON_MERGE
#include "WndWeaponMerge.h"
#endif //__WEAPON_MERGE

#ifdef __WIKIPEDIA_MONSTER_DROPLIST
#include "WndDropInfo.h"
#endif //__WIKIPEDIA_MONSTER_DROPLIST

#ifdef __AEGON_ADVANCED_ANNOUNCEMENT
#include "WndAegonSystemShout.h"
#endif // __AEGON_ADVANCED_ANNOUNCEMENT

#ifdef __SWITCH_EQUIP
#include "WndSwitchEquip.h"
#endif // __SWITCH_EQUIP

#ifdef __AEGON_PVP_RANKING
#include "WndAegonPvPRanking.h"
#endif // __AEGON_PVP_RANKING

#ifdef __AEGON_THEME_SWITCHER
#include "AegonThemeSwitcherMng.h"
#endif // __AEGON_THEME_SWITCHER

#ifdef __PET_GLOW
#include "WndPetGlow.h"
#endif //__PET_GLOW

#ifdef __MODS_MANAGER
#include "WndModsManager.h"
#endif // __MODS_MANAGER

#ifdef __IGNORE_LIST_OPT
#include "WndMessenger.h"
#endif //__IGNORE_LIST_OPT

#ifdef __DPS_METER
#include "WndDpsMeter.h"
#endif //__DPS_METER

#ifdef __AEGON_EMOTE_SYSTEM
#include "AegonEmoticonMng.h"
#endif //__AEGON_EMOTE_SYSTEM

#ifdef __FL_FARM_STAT
#ifdef __WIKIPEDIA_ITEMS
#include "WndWikiItems.h"
#endif //__WIKIPEDIA_ITEMS
#endif //__FL_FARM_STAT

#ifdef __CRYSTAL
#include "WndCrystal.h"
#endif // __CRYSTAL

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
#include "WndSummonFriend.h"
#endif //__AEGON_FULLSCREEN_AND_RESIZE

#ifdef __AEGON_INGAME_SHOP
#include "WndAegonIngameShop.h"
#endif //__AEGON_INGAME_SHOP

#ifdef __ROYAL_RUMBLE
#include "WndFFA.h"
#include "WndPlayerRanking.h"
#endif //__ROYAL_RUMBLE

#ifdef __AEGON_PVP_MINIGAMES
#include "WndAegonPvPMinigames.h"
#endif

#ifdef __AEGON_IMPROVED_SHOPDESIGN
#include "AegonShopDesign.h"
#endif

#ifdef __AEGON_F_TOOL
#include "WndAegonFTool.h" 
#endif

#ifdef __AEGON_MESSENGER
#include "WndAegonMessenger.h"
#endif

#ifdef __AEGON_TELEPORTER
#include "WndAegonTeleporter.h"
#endif

#ifdef __AEGON_TELEPORTER
#include "WndAegonTeleporter.h"
#include "AegonTeleportMng.h"
#endif

typedef CMap<DWORD, DWORD, void *, void *> CMapDWordToPtr;

#define DECLAREAPPLET( AddMain_Func, AllocClass) CWndBase* AddMain_Func() { return AllocClass; }

class CWndMessage;
class CWndInstantMsg;
class CWndCommand;
class CWndMap;
class CWndPartyChangeName;
class CWndPartyConfirm;
class CWndPartyLeaveConfirm;
class CWndFriendConFirm;
class CWndDuelConfirm;

#if __VER >= 8     // 8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World
class CWndDuelResult;
#endif	// __VER >= 8  

class CWndAddFriend;
class CWndCloseExistingConnection;
class CWndMessageNote;
class CWndMessengerNote;
class CWndTextBook;
class CWndTextScroll;
class CWndTextLetter;
class CWndSelectVillage;
 
#define REG_VERSION 1

#include "WndGuild.h"
#include "WndRankGuild.h"
#include "WndRankInfo.h"
#include "WndRankWar.h"
#include "WndGuildBank.h"
#include "WndChangeName.h"

#ifdef  __GUILDFINDER
#include "WndGuildFinder.h"
#endif //__GUILDFINDER

#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
#include "WndSealChar.h"
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

#define MAX_SKILL		  16

// chat style
#define CHATSTY_GENERAL   0x0001
#define CHATSTY_WHISPER   0x0002
#define CHATSTY_SHOUT     0x0004
#define CHATSTY_PARTY     0x0008
#define CHATSTY_GUILD     0x0010

#define CHATSTY_HELP          0x0060
#define CHATSTY_HELP_CLIENT   0x0020
#define CHATSTY_HELP_CHAT     0x0040
#define CHATSTY_SYSTEM        0x0180
#define CHATSTY_SYSTEM_CLIENT 0x0080
#define CHATSTY_SYSTEM_CHAT   0x0100
#define CHATSTY_GAME          0x0600
#define CHATSTY_GAME_CLIENT   0x0200
#define CHATSTY_GAME_CHAT     0x0400

#define PS_USE_MACRO     0x00000001
#define PS_NOT_MACRO     0x00000002


typedef struct tagWNDREGINFO
{
	DWORD dwWndId;
	CRect rect;
	BOOL  bOpen;
	DWORD dwVersion;
	DWORD dwWndSize;
	DWORD dwSize;
	BYTE* lpArchive;
} WNDREGINFO,* LPWNDREGINFO;

struct AppletFunc
{
	CWndBase* (*m_pFunc)();
	DWORD m_dwIdApplet;
	LPCTSTR m_pAppletName;
	LPCTSTR m_pAppletDesc;
	LPCTSTR m_pszIconName;
	CHAR m_cHotkey;
#ifdef __CHANGE_HOT_KEY
	LPCTSTR m_pAppletText;
	DWORD   m_dwAppletAuth;
#endif // __CHANGE_HOT_KEY
};

struct ToolTipItemTextColor
{
	DWORD dwName0;
	DWORD dwName1;
	DWORD dwName2;
	DWORD dwName3;
	DWORD dwName4;
	DWORD dwName5;
#ifdef __PREMIUM
	DWORD dwName6;
#endif //__PREMIUM
#ifdef __ANARCHY_SYSTEM
	DWORD dwName7;
#endif //__ANARCHY_SYSTEM
	DWORD dwGeneral;
	DWORD dwPiercing;
	DWORD dwPlusOption;
	DWORD dwResist;
	DWORD dwResistFire;
	DWORD dwResistWater;
	DWORD dwResistEarth;
	DWORD dwResistElectricity;
	DWORD dwResistWind;
	DWORD dwResistSM;
	DWORD dwResistSM1;
	DWORD dwTime;
	DWORD dwEffective0;
	DWORD dwEffective1;
	DWORD dwEffective2;
	DWORD dwEffective3;
	DWORD dwRandomOption;
	DWORD dwEnchantOption;
	DWORD dwSetName;
	DWORD dwSetItem0;
	DWORD dwSetItem1;
	DWORD dwSetEffect;
	DWORD dwGold;
	DWORD dwCommand;
	DWORD dwNotUse;
#if __VER >= 9 // __CSC_VER9_1
	DWORD dwAddedOpt1;
	DWORD dwAddedOpt2;
	DWORD dwAddedOpt3;
	DWORD dwAddedOpt4;
	DWORD dwAddedOpt5;
	DWORD dwAddedOpt6;
	DWORD dwAddedOpt7;
	DWORD dwAddedOpt8;
	DWORD dwAddedOpt9;
#ifdef __NEW_DIAMONDS
	DWORD dwAddedOpt10;
#endif //__NEW_DIAMONDS
#endif //__CSC_VER9_1
#if __VER >= 11 // __SYS_IDENTIFY
	DWORD dwAwakening;
	DWORD dwBlessing;
	DWORD dwBlessingWarning;
#endif	// __SYS_IDENTIFY
};

enum 
{
	FIRST_TC,
	SECOND_TC,
	THIRD_TC,
	MAX_TC
};

#if __VER >= 15 // __GUILD_HOUSE
class CWndGHMainMenu;
class CWndGHUpkeep;
#endif

class CWndMgr : public CWndBase
{ 
	CString m_strChatBackup;
	CTimer m_timerDobe;
#ifdef __BAN_CHATTING_SYSTEM
	int m_nWarningCounter;
	int m_nWarning2Counter;
	CTimer m_timerWarning;
	CTimer m_timerWarning2;
	CTimer m_timerShortcutWarning;
	CTimer m_timerBanning;
	BOOL m_bShortcutCommand;

public:
	enum { BANNING_POINT = 5, BANNING_2_POINT = 2 };
	enum { WARNING_MILLISECOND = 700, WARNING_2_MILLISECOND = 1000, SHORTCUT_WARNING_MILLISECOND = 3000, BANNING_MILLISECOND = 180000 };

public:
	void SetWarningCounter( int nWarningCounter );
	void SetWarning2Counter( int nWarning2Counter );
	int GetWarningCounter( void ) const;
	int GetWarning2Counter( void ) const;
	CTimer& GetWarningTimer( void );
	CTimer& GetWarning2Timer( void );
	CTimer& GetShortcutWarningTimer( void );
	CTimer& GetBanningTimer( void );
	BOOL IsShortcutCommand( void ) const;

private:
#endif // __BAN_CHATTING_SYSTEM

public:
	CStringArray m_aChatString;
	CUIntArray   m_aChatColor ;
	CUIntArray   m_aChatStyle ;
#ifdef __RT_1025
	CRTMessenger	m_RTMessenger;
#else	//__RT_1025
	CMessenger m_Messenger;
#endif	// __RT_1025
	CMapStringToPtr m_mapMap;
	CMapStringToPtr m_mapInstantMsg;
	CMapStringToPtr m_mapMessage;
	CEditString m_ChatString;

	BOOL m_bAutoRun;
	
	DWORD m_dwSavePlayerTime;
	
	TCHAR m_szTimerChat[ 128 ];
	CTimer m_timerChat;
	
	BOOL   m_bConnect;
	CTimer m_timerMessenger;
	ToolTipItemTextColor	dwItemColor[MAX_TC];


	CPtrArray	m_awndShortCut;
	BOOL	m_bTitle        ;
	CMapDWordToPtr	m_mapAppletFunc ;
#if __VER >= 8 // __CSC_VER8_1
	list<int> m_tempWndId;
	BOOL m_clearFlag;
#endif //__CSC_VER8_1

	AppletFunc*		GetAppletFunc( DWORD dwIdApplet );
	void	AddAllApplet();
	BOOL ScreenCapture();
	BOOL SaveBitmap( LPCTSTR lpszName );
#if __VER >= 11 // __CAPTURE_JPG
	BOOL SaveJPG( LPCTSTR lpszName );
#endif
	void InitSetItemTextColor();
	void Free();
	void SetMessengerAutoState();

public:
	DWORD		   m_dwSkillTime[MAX_SKILL];
	
	CWndBeautyShop* m_pWndBeautyShop;
#if __VER >= 8 //__CSC_VER8_4
	CWndFaceShop* m_pWndFaceShop;
#ifdef __NEWYEARDAY_EVENT_COUPON
	CWndUseCouponConfirm* m_pWndUseCouponConfirm;
#endif //__NEWYEARDAY_EVENT_COUPON

#endif //__CSC_VER8_4
#if __VER >= 8 //__CSC_VER8_5
	CWndSummonAngel* m_pWndSummonAngel;
#endif //__CSC_VER8_5
	CWndDialog*    m_pWndDialog;
	CWndShop*      m_pWndShop;
	CWndBase*      m_pWndActiveDesktop;
	CWndTrade*     m_pWndTrade;
	CWndTradeGold* m_pWndTradeGold;
	CWndDropItem*  m_pWndDropItem;
	CWndConfirmTrade * m_pWndConfirmTrade;
	CWndTradeConfirm * m_pWndTradeConfirm;
	CWndQuickList* m_pWndQuickList;
	CWndCommand*   m_pWndCommand;
	CWndBank*	   m_pWndBank;
	CWndWorld*     m_pWndWorld;
#ifdef __JEFF_11
	CWndPetRes*    m_pPetRes;
#endif
#ifdef __EVE_MINIGAME
	CWndKawiBawiBoGame* m_pWndKawiBawiBoGame;
	CWndKawiBawiBoGameConfirm* m_pWndKawiBawiBoGameConfirm;
	CWndFindWordGame* m_pWndFindWordGame;
	CWndDiceGame* m_pWndDiceGame;
	CWndPuzzleGame* m_pWndPuzzleGame;
#endif //__EVE_MINIGAME
	
#if __VER >= 9 // __CSC_VER9_1
	CWndMixJewel* m_pWndSmeltMixJewel;
	CWndSmeltJewel* m_pWndSmeltJewel;
	CWndExtraction* m_pWndExtraction;
	CWndChangeWeapon* m_pWndChangeWeapon;
	CWndPetStatus* m_pWndPetStatus;

	CWndFoodConfirm* m_pWndFoodConfirm;
	CWndPetMiracle* m_pWndPetMiracle;
	CWndPetFoodMill* m_pWndPetFoodMill;
	CWndPetLifeConfirm* m_pWndPetLifeConfirm;
#if __VER >= 10 // __CSC_VER9_1 -> __LEGEND
	CWndHeroSkillUp* m_pWndHeroSkillUp;
#endif //__CSC_VER9_1 -> __LEGEND
#endif //__CSC_VER9_1
#if __VER >= 11 // __PIERCING_REMOVE
	CWndRemovePiercing* m_pWndRemovePiercing;
#endif //__PIERCING_REMOVE
#if __VER >= 12 // __CSC_VER12_4
	CWndRemoveJewel* m_pWndRemoveJewel;
#endif //__CSC_VER12_4
#if __VER >= 12 // __CSC_VER12_5
	CWndPetTransEggs* m_pWndPetTransEggs;
#endif //__CSC_VER12_5
#ifdef __TRADESYS
	CWndDialogEvent* m_pWndDialogEvent;
#endif //__TRADESYS
#if __VER >= 12 // __HEAVEN_TOWER
	CWndHeavenTower* m_pWndHeavenTower;
#endif //__HEAVEN_TOWER
#if __VER >= 8 //__Y_CHAT_SYSTEM_8
	CWndChatLog* m_pWndChatLog;
#endif //__Y_CHAT_SYSTEM_8

#if __VER >= 10 // __REMOVE_ATTRIBUTE
	CWndRemoveAttribute* m_pWndRemoveAttribute;
#endif //__REMOVE_ATTRIBUTE
	CWndPost* m_pWndPost;
	CWndQueryEquip* m_pWndQueryEquip;
	CWndSelectVillage*		m_pWndSelectVillage;
	CWndQuestItemWarning* m_pWndQuestItemWarning;
	CWndChangeName*		m_pWndChangeName;
	CWndDropConfirm*  m_pWndDropConfirm;
	CWndUpgradeBase*  m_pWndUpgradeBase;
	CWndPiercing*	  m_pWndPiercing;

#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	CWndSealChar*		m_pWndSealChar;
	CWndSealCharSelect*		m_pWndSealCharSelect;
	CWndSealCharSend*		m_pWndSealCharSend;
	CWndSealCharSet*		m_pWndSealCharSet;
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

	CWndInvenRemoveItem* m_pWndInvenRemoveItem;

	CWndCommercialElem* m_pWndCommerialElem;
	CWndRemoveElem* m_pRemoveElem;
	CWndRepairItem* m_pWndRepairItem;
	CWndConfirmBank* m_pWndConfirmBank;
	CWndBankPassword* m_pWndBankPassword;
	CWndPenaltyPK* m_pWndPenaltyPK;
	CWndRankGuild* m_pWndRankGuild;
	CWndRankInfo* m_pWndRankInfo;
	CWndRankWar* m_pWndRankWar;
	CWndGuildBank*	m_pWndGuildBank;
#ifdef __FUNNY_COIN
	CWndFunnyCoinConfirm* m_pFunnyCoinConfirm;
#endif //__FUNNY_COIN

#ifdef __MAIL_REQUESTING_BOX
	BOOL	m_bWaitRequestMail;
#endif

#if __VER >= 11 // __GUILD_BANK_LOG
	CWndGuildBankLog* m_pWndGuildBankLog;
#endif //__GUILD_BANK_LOG
	CWndGuildConfirm* m_pWndGuildConfirm;
	CWndGuildWarPeaceConfirm* m_pWndGuildWarPeaceConfirm;
	CWndGuildWarRequest* m_pWndGuildWarRequest;
	CWndGuildNickName* m_pWndGuildNickName;
	CWndGuildCombatRank_Person*   m_pWndGuildCombatRanking;
	CWndGuildCombatState*		  m_pWndGuildWarState;	
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	CWndGuildCombat1to1Offer*		  m_pWndGuildCombatOffer;
#else //__GUILD_COMBAT_1TO1
	CWndGuildCombatOffer*		  m_pWndGuildCombatOffer;
#endif //__GUILD_COMBAT_1TO1
	CWndGuildCombatSelection*	  m_pWndGuildCombatSelection;
	CWndGuildCombatJoinSelection* m_pWndGuildCombatJoinSelection;
	CWndGuildCombatBoard*		  m_pWndGuildCombatBoard;
	CGuildCombatInfoMessageBox2*  m_pWndGuildCombatInfoMessageBox2;
	CGuildCombatInfoMessageBox*   m_pWndGuildCombatInfoMessageBox;
	CWndGuildCombatResult*		  n_pWndGuildCombatResult;

#if __VER >= 11 // __GUILD_COMBAT_1TO1
	CWndGuildCombat1to1Selection*	m_pWndGuildCombat1to1Selection;
	CWndGuildCombat1to1Offer*		m_pWndGuildCombat1to1Offer;
#endif //__GUILD_COMBAT_1TO1
	CCollectingWnd*				  m_pWndCollecting;
#if __VER >= 11 // __SYS_IDENTIFY
	CWndLvReqDown*				  m_pWndLvReqDown;
	CWndBlessingCancel*			  m_pWndBlessingCancel;
	CWndAwakening*				  m_pWndAwakening;
#endif
#ifdef __MIKSIK_EQUALIZED_MODE
	CWndSelectTmpClass* m_WndSelectTmpClass;
#endif //__MIKSIK_EQUALIZED_MODE
#if __VER >= 12 // __LORD
	CWndLordTender*				m_pWndLordTender;// __AEGON_FIXES checked windows till here
	CWndLordVote*				m_pWndLordVote;
	CWndLordState*				m_pWndLordState;
	CWndLordEvent*				m_pWndLordEvent;
	CWndLordSkillConfirm*		m_pWndLordSkillConfirm;
	CWndLordInfo*				m_pWndLordInfo;
	CWndLordRPInfo*				m_pWndLordRPInfo;
	bool						m_bIsOpenLordSkill;
#endif
#if __VER >= 12 // __SECRET_ROOM
	CWndSecretRoomInfoMsgBox*		m_pWndSecretRoomMsg;
	CWndSecretRoomSelection*		m_pWndSecretRoomSelection;
	CWndSecretRoomOffer*			m_pWndSecretRoomOffer;
	CWndSecretRoomOfferState*		m_pWndSecretRoomOfferState;
	CWndSecretRoomChangeTaxRate*	m_pWndSecretRoomChangeTaxRate;
	CWndSecretRoomCheckTaxRate*		m_pWndSecretRoomCheckTaxRate;
	CWndSecretRoomBoard*			m_pWndSecretRoomBoard;
	CWndSecretRoomQuick*			m_pWndSecretRoomQuick;
#endif //__SECRET_ROOM
#if __VER >= 12 // __PET_0519
	CWndPetAwakCancel*				m_pWndPetAwakCancel;
#endif
#ifdef __AZRIA_1023
	CWndSelectCh*					m_pWndSelectCh;
#endif
#if __VER >= 13 // __RAINBOW_RACE
	CWndRainbowRaceOffer*			m_pWndRainbowRaceOffer;
	CWndRainbowRaceInfo*			m_pWndRainbowRaceInfo;
	CWndRainbowRaceRule*			m_pWndRainbowRaceRule;
	CWndRainbowRaceRanking*			m_pWndRainbowRaceRanking;
	CWndRainbowRacePrize*			m_pWndRainbowRacePrize;
	CWndRainbowRaceMiniGameButton*	m_pWndRainbowRaceMiniGameButton;
	CWndRainbowRaceMiniGame*		m_pWndRainbowRaceMiniGame;
	CWndRainbowRaceMiniGameEnd*		m_pWndRainbowRaceMiniGameEnd;
	CWndRRMiniGameKawiBawiBo*		m_pWndRRMiniGameKawiBawiBo;
	CWndRRMiniGameDice*				m_pWndRRMiniGameDice;
	CWndRRMiniGameArithmetic*		m_pWndRRMiniGameArithmetic;
	CWndRRMiniGameStopWatch*		m_pWndRRMiniGameStopWatch;
	CWndRRMiniGameTyping*			m_pWndRRMiniGameTyping;
	CWndRRMiniGameCard*				m_pWndRRMiniGameCard;
	CWndRRMiniGameLadder*			m_pWndRRMiniGameLadder;
#endif //__RAINBOW_RACE
#if __VER >= 13 // __CSC_VER13_2
	CWndCoupleMessage*				m_pWndCoupleMessage;
#endif //__CSC_VER13_2
#if __VER >= 13 // __EXT_ENCHANT
	CWndChangeAttribute*			m_pWndChangeAttribute;
#endif //__EXT_ENCHANT

	CWndReSkillWarning*			  m_pWndReSkillWarning;	
	CWndFontEdit*				  m_pWndFontEdit;

	CWndPartyChangeName* m_pWndPartyChangeName;
	CWndPartyConfirm* m_pWndPartyConfirm;
	
	CWndMessageNote* m_pWndMessageNote;
	CWndMessengerNote* m_pWndMessengerNote;

	CWndPartyLeaveConfirm * m_pWndPartyLeaveConfirm;
	CWndFriendConFirm* m_pWndFriendConfirm;
	CWndDuelConfirm* m_pWndDuelConfirm;

#if __VER >= 8     // 8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World
	CWndDuelResult* m_pWndDuelResult;
#endif	// __VER >= 8  

	CWndAddFriend* m_pWndAddFriend;
	CWndStateConfirm* m_pWndStateConfirm;
	CWndCloseExistingConnection*	m_pWndCloseExistingConnection;
	
	CWndRandomScrollConfirm*		m_pWndRandomScrollConfirm;

#ifdef __HELP_BUG_FIX
	CWndHelpInstant* m_pWndHelpInstant;
#endif //__HELP_BUG_FIX
#if __VER >= 13 // __HOUSING
	CWndRoomList*				m_pWndRoomList;
	CWndQuitRoom*				m_pWndQuitRoom;
#endif // __HOUSING
#ifdef __PET_1024
	CWndChangePetName*				m_pWndChangePetName;
#endif
#ifdef __S_SERVER_UNIFY
	BOOL							m_bAllAction;
#endif // __S_SERVER_UNIFY

#if __VER >= 14 // __SMELT_SAFETY
	CWndSmeltSafety* m_pWndSmeltSafety;
	CWndSmeltSafetyConfirm* m_pWndSmeltSafetyConfirm;
#endif //__SMELT_SAFETY

#if __VER >= 14 // __EQUIP_BIND
	CWndEquipBindConfirm* m_pWndEquipBindConfirm;
#endif // __EQUIP_BIND

#if __VER >= 14 // __RESTATE_CONFIRM
	CWndRestateConfirm* m_pWndRestateConfirm;
#endif // __RESTATE_CONFIRM

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	CWndQuestQuickInfo*				m_pWndQuestQuickInfo;
	CWndQuestDetail*				m_pWndQuestDetail;
#endif // __IMPROVE_QUEST_INTERFACE

#if __VER >= 15 // __CAMPUS
	CWndCampusInvitationConfirm*	m_pWndCampusInvitationConfirm;
	CWndCampusSeveranceConfirm*		m_pWndCampusSeveranceConfirm;
#endif // __CAMPUS

#if __VER >= 15 // __PETVIS
	CWndBuffPetStatus*				m_pWndBuffPetStatus;
	CWndConfirmVis*					m_pWndConfirmVis;
#endif

#if __VER >= 15 // __GUILD_HOUSE
	CWndGHMainMenu*					m_pWndGHMain;
	CWndGHUpkeep*					m_pWndUpkeep;
#endif

#ifdef __PROTECT_AWAKE
	CWndSelectAwakeCase*			m_pWndSelectAwakeCase;
#endif 

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
	CWndChattingBlockingList*		m_pWndChattingBlockingList;
#endif // __YS_CHATTING_BLOCKING_SYSTEM

#ifdef __GUILD_HOUSE_MIDDLE
	CWndGuildHouseBid*				m_pWndGHBid;
#endif //__GUILD_HOUSE_MIDDLE

#ifdef __NEW_ITEM_VARUNA
	CWndBarunaNPC*                  m_pWndBarunaNPC;
	CWndBarunaEnchant*              m_pWndBarunaEnchant;
	CWndBarunaExtractOffer*         m_pWndBarunaExtractOffer;
	CWndBarunaOffer*                m_pWndBarunaOffer;
	CWndBarunaSeed*                 m_pWndBarunaSeed;
	CWndBarunaOfferSeed*            m_pWndBarunaOfferSeed;

#ifdef __BARUNA_PIERCING
	CWndBarunaPiercing*             m_pWndBarunaPiercing;
#endif // __BARUNA_PIERCING
#endif // __NEW_ITEM_VARUNA

#ifdef __FASHION_COMBINE
	CWndFashionCombine*             m_pWndFashionCombine;
#endif // __FASHION_COMBINE

#ifdef __ANARCHY_SYSTEM
	CWndAnarchySkill*               m_pWndAnarchy;
	CWndAnarchySkillConfirm*        m_pWndAnarchySkillConfirm;
#endif // __ANARCHY_SYSTEM

	CTexturePack                    m_texture;
	CTexturePack                    m_texCommand;
	CTexturePack                    m_texIcon;
	CTexturePack                    m_texWnd;

	CWndMessageBox*		            m_pWndMessageBox;
	CWndMessageBoxUpper*	        m_pWndMessageBoxUpper;
	CWndQuestItemInfo*	            m_pQuestItemInfo;

#ifdef __ITEMLINK
	CWndLinkedItem*		            m_pWndLinkedItem;
#endif //__ITEMLINK

#ifdef	__COLOSSEUM
	CWndColosseumComplete*	        m_pColosseumComplete;
	CWndColosseumEnter*		        m_pColosseumEnter;
	CWndColosseumMassage*	        m_pColosseumMassage;
	CWndColosseumRankGuild*	        m_pColosseumRankGuild;
	CWndColosseumRankNomal*	        m_pColosseumRankNomal;
	CWndColosseumRetry*		        m_pColosseumRetry;
	CWndColosseumStart*		        m_pColosseumStart;
	CWndColosseumStartReady1*	    m_pColosseumStartReady1;
	CWndColosseumStartReady2*	    m_pColosseumStartReady2;
#endif // __COLOSSEUM

#ifdef __TREASUREBOX
	CWndTreasureBox*                m_pWndTreasureBox;
	CWndTreasureRecieve*            m_pWndTreasureRecieve;
#endif //__TREASUREBOX

#ifdef __AEGON_PETMISTAKE
	CWndPetMistakeCustom*           m_pWndPetMistakeCustom;
#endif // __AEGON_PETMISTAKE

#ifdef __AEGON_MONSTER_HUNT
	CWndMonsterHunt*                m_pWndMonsterHunt;
#endif // __AEGON_MONSTER_HUNT

#ifdef __LEEAEG_SHOP_FINDER
	CWndShopFinder*                 m_pWndShopFinder;
#endif //__LEEAEG_SHOP_FINDER

#ifdef __GIFTBOX_LIST_ITEMS
	CWndGiftbox*                    m_pWndGiftbox;
#endif //__GIFTBOX_LIST_ITEMS

#ifdef __AEGON_RETURN_AWAKE
	CWndReturnAwakening*            m_pWndReturnAwakening;
#endif // __AEGON_RETURN_AWAKE

#ifdef __VENDING_NPC
	CWndVendorWarning*              m_pWndVendorWarning;
#endif // __VENDING_NPC

#ifdef __WEAPON_RARITY
	CWndWeaponRarity*               m_pWndWeaponRarity;
#endif // __WEAPON_RARITY

#ifdef __SYS_CUSTOM_TITLE
	CWndCreateOwnTitle*             m_pCreateOwnTitle;
#endif // __SYS_CUSTOM_TITLE

#ifdef __SYS_PARTYFINDER
	CWndPartyFind*                  m_pWndPartyFind;
#endif // __SYS_PARTYFINDER

#ifdef __AEGON_TELEPORTER
	CWndTeleporter*                 m_pWndTeleporter;
#endif

#ifdef __AEGON_LOOT_BOX
	CWndLootBoxMain*                m_pWndLootBoxMain;
#endif // __AEGON_LOOT_BOX

#ifdef __AEGON_VIEW_INV
	CWndViewInventoryMain*          m_pWndViewInventoryMain;
	CWndViewInventoryCtrl*          m_pWndViewInventoryCtrl;
	CWndViewBag*					m_pWndViewBag;
#endif // __AEGON_VIEW_INV

#ifdef __SYS_PETFILTER
	CWndPetfilter*                  m_pWndPetfilter;
#endif //__SYS_PETFILTER

#ifdef  __GUILDFINDER
	CWndGuildFinder*                m_pWndGuildFinder;
	CWndGuildFinderReq*             m_pWndGuildFinderReq;
#endif //__GUILDFINDER

#ifdef	__REMOVE_DURATION_ITEMS
	CWndRemoveCmm*                  m_pWndRemoveCmm;
#endif	//__REMOVE_DURATION_ITEMS

#ifdef __SYS_MONSTER_CLASH
	CWndMonsterClashRegister*       pWndMonsterClashRegister;
	CWndMonsterClashRegisterMember* pWndMonsterClashRegisterMember;
#endif //__SYS_MONSTER_CLASH

#ifdef __SYS_SANDBOX
	CWndSandbox *                   m_pWndSandbox;
#endif //__SYS_SANDBOX

#ifdef __AEGON_CRAFTING_SYSTEM
	CWndCraftingMain*				m_pWndCraftingMain;
#endif // __AEGON_CRAFTING_SYSTEM

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
	int resizeWidth;
	int resizeHeight;
#endif // __AEGON_FULLSCREEN_AND_RESIZE

#ifdef __WIKIPEDIA_MONSTER_DROPLIST
	CWndDropInfo*					m_pWndDropInfo;
#endif //__WIKIPEDIA_MONSTER_DROPLIST

#ifdef __AEGON_ADVANCED_ANNOUNCEMENT
	CWndAegonSystemShout* m_pWndAegonSystemShout;
#endif // __AEGON_ADVANCED_ANNOUNCEMENT

#ifdef __SWITCH_EQUIP
	CWndSwitchEquip* m_pWndSwitchEquip;
#endif // __SWITCH_EQUIP

#ifdef __PET_GLOW
	CWndPetGlowChange* m_pWndPetGlowChange;
#endif // __PET_GLOW

#ifdef __MODS_MANAGER
	CWndModsManager* m_pWndModsManager;
#endif // __MODS_MANAGER

#ifdef __WEAPON_MERGE
	CWndWeaponMerge* m_pWndWeaponMerge;
	CWndWeaponMergeCancel* m_pWndWeaponMergeCancel;
#endif // __WEAPON_MERGE

#ifdef __LOOKCHANGE
	CWndLookChange* m_pWndLookChange;
#endif // __LOOKCHANGE

#ifdef __AEGON_INGAME_DP
	CWndIngameDPPurchaseConfirm*	m_pWndDPPurchaseConfirm;
#endif //__AEGON_INGAME_DP

#ifdef __FL_FARM_STAT
	CWndFarmCounter* m_pWndFarmCounter;
#ifdef __WIKIPEDIA_ITEMS
	CWndWikiItems* m_pWndWikipediaItems;
#endif //__WIKIPEDIA_ITEMS
#endif //__FL_FARM_STAT

#ifdef __CRYSTAL
	CWndCrystal* m_pWndCrystal;
#endif // __CRYSTAL

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
	CWndGuildWarJoinConfirm* m_pWndGuildCombatJoinConfirm;
	CWndGuildWarCancelConfirm* m_pWndGuildCombatCancelConfirm;

	CWndSecretRoomCancelConfirm* m_pWndSecretRoomCancelConfirm;

	CWndSummonParty* m_pWndSummonParty;

	CWndSummonFriendMsg* m_pWndSummonFriendConfirm;
	CWndSummonPartyMsg* m_pWndSummonPartyConfirm;
	CWndSummonFriend* m_pWndSummonFriend;
	CWndSummonPartyUse* m_pWndSummonPartyUse;

	CWndGuildName* m_pWndGuildName;
#endif


#ifdef __AEGON_TOURNAMENT
	CWndAegonTournamentScoreBoard* m_pWndAegonTournamentScoreBoard;
	CWndAegonTournament* m_pWndAegonTournament;
#endif // __AEGON_TOURNAMENT

#ifdef __ROYAL_RUMBLE
	CWndJoinPlayerSiege* m_pJoinPlayerSiege;
	CWndPlayerRanking* m_pSiegeRanking;
#endif // __ROYAL_RUMBLE

#ifdef __AEGON_PVP_MINIGAMES
	CWndAegonPvPGameCreate* m_pWndAegonPvPGameCreate;
	CWndAegonPvPGameInvitation* m_pWndAegonPvPGameInvitation;
	CWndAegonPvPGameMain* m_pWndAegonPvPGameMain;
	CWndMessageBox* m_pWndAegonPvPGameMessageBox;
	CWndAegonPvPGameFinder* m_pWndAegonPvPGameFinder;
#endif

	void OpenQuestItemInfo(CWndBase* pWndParent = NULL, CItemBase* pItemBase = NULL);
	void ChangeQuestItemInfo( CItemBase* pItemBase = NULL );

	CWndTextBook* m_pWndTextBook;
	CWndTextScroll* m_pWndTextScroll;
	CWndTextLetter* m_pWndTextLetter;
	void OpenTextBook     ( CWndBase* pWndParent = NULL, CItemBase* pItemBase = NULL);
	void OpenTextScroll   ( CWndBase* pWndParent = NULL, CItemBase* pItemBase = NULL);
	void OpenTextLetter   ( CWndBase* pWndParent = NULL, CItemBase* pItemBase = NULL);
	void ChangeTextBook   ( CItemBase* pItemBase = NULL );
	void ChangeTextScroll ( CItemBase* pItemBase = NULL );
	void ChangeTextLetter ( CItemBase* pItemBase = NULL );
	// Field
	CWndTaskBar*	m_pWndTaskBar;
	CWndTaskMenu*	m_pWndMenu;
	CWndRevival*    m_pWndRevival;
	CWndResurrectionConfirm* m_pWndResurrectionConfirm;
//	CWndRecoverChao*    m_pWndRecoverChao;
	CWndReWanted*		m_pReWanted;
	CWndWanted*			m_pWanted;
	CWndLogOutWaitting* m_pLogOutWaitting;
	CWndCommItemDlg*	m_pWndCommItemDlg;
	CWndChangeClass1*	m_pWndChangeClass1;
	CWndChangeClass2*	m_pWndChangeClass2;

	CMapDWordToPtr	m_mapWndApplet ;
	CMapDWordToPtr  m_mapWndRegInfo;

#ifdef __AEGON_FONT_SWITCH
	DWORD m_dwFontSwitched;
#endif // __AEGON_FONT_SWITCH

#ifdef __AEGON_TIME_DIFF
	int m_nTimeDiff;
#endif

#ifdef __AEGON_FIXES
	BOOL m_bSetOtherNull;
#endif

#ifdef __DPS_METER
	BOOL m_bToggledDpsMeter;
#endif

	void PutDestParam( DWORD dwDst1, DWORD dwDst2, DWORD dwAdj1, DWORD dwAdj2, CEditString &str );	
	void PutDefinedString( DWORD dwText, ... );
	void PutString( LPCTSTR lpszString, CObj* pObj = NULL, DWORD dwColor = 0xffffffff, DWORD dwChatStyle = CHATSTY_GAME, DWORD dwPStyle = 0x00000001 );
	void ParsingChat( CString string );
	void WordChange( CString& rString );

	BOOL	IsTitle()	{	return m_bTitle;	}

	void	OpenTitle( BOOL bFirstTime = FALSE );
	void	OpenField();

	BOOL	OpenTaskBar();
#ifdef __AEGON_THEME_SWITCHER
	void	RestartTaskBar();
#endif // __AEGON_THEME_SWITCHER
	BOOL	OpenMenu();

	void	ObjectExecutor( DWORD dwShortcut, DWORD dwId, DWORD dwType = 0 );
	void	ObjectExecutor( LPSHORTCUT lpShortcut );
	void	UseSkillShortCut( DWORD dwType, DWORD dwSkillIdx );
		
	// map
	CWndMap* OpenMap( LPCTSTR lpszMapFileName );
	CCollectingWnd* OpenCollecting();
	BOOL CloseCollecting();

	// massenger
	CWndMessage* GetMessage( LPCTSTR lpszFrom );
	CWndMessage* OpenMessage( LPCTSTR lpszFrom );
	BOOL	UpdateMessage( LPCTSTR pszOld, LPCTSTR pszNew );
	CWndInstantMsg* GetInstantMsg( LPCTSTR lpszFrom );
	CWndInstantMsg* OpenInstantMsg( LPCTSTR lpszFrom );

#ifdef __AEGON_MESSENGER
	CWndAegonMessenger* OpenMessenger(BOOL bMinimized = FALSE);
#endif

	// MessageBox
	BOOL	OpenCustomBox( LPCTSTR strMessage, CWndMessageBox* pWndMessageBox, CWndBase* pWndParent = NULL );
	BOOL	OpenMessageBox( LPCTSTR strMessage, UINT nType = MB_OK, CWndBase* pWndParent = NULL );
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	BOOL	OpenMessageBoxWithTitle( LPCTSTR lpszMessage, const CString& strTitle, UINT nType = MB_OK, CWndBase* pWndParent = NULL );
#endif // __IMPROVE_QUEST_INTERFACE
	BOOL    OpenMessageBoxUpper( LPCTSTR lpszMessage, UINT nType = MB_OK, BOOL bPostLogoutMsg = FALSE );
		
	void	CloseMessageBox();

	// tooltip
	void PutToolTip_Troupe( DWORD dwSkill, CPoint point, CRect* pRect );
	void PutToolTip_Skill( DWORD dwSkill, DWORD dwLevel, CPoint point, CRect* pRect, BOOL bActive = TRUE );
#ifdef __BETTER_SKILL_TOOLTIP
	void PutToolTip_Skill_Extended(DWORD dwSkill, DWORD dwLevel, CPoint point, CRect* pRect, BOOL bActive = TRUE);
#endif //__BETTER_SKILL_TOOLTIP
	void PutToolTip_Item( DWORD dwType, DWORD dwId, CPoint point, CRect* pRect, int flag = 0 );
	void PutToolTip_Item( CItemBase* pItemBase, CPoint point, CRect* pRect, int flag = 0 );
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	enum { TOOL_TIP_SWITCH_MAIN = 0, TOOL_TIP_SWITCH_SUB1 = 1, TOOL_TIP_SWITCH_SUB2 = 2 };
	void PutToolTipItemAndSubToolTip( CItemBase* pItemBase, CPoint point, CRect* pRect, int nFlag = 0 );
	void PutToolTipParts( CItemBase* pPartsItemBase, CPoint point, CRect* pRect, const int nFlag, const int nSubToolTipFlag );
	void MakeToolTipText( CItemBase* pItemBase, CEditString& strEdit, int flag = 0 );
#endif // __IMPROVE_SYSTEM_VER15
	void PutToolTip_Character( int SelectCharacter, CPoint point, CRect* pRect );

	DWORD PutItemName( CItemElem* pItemElem, CEditString* pEdit );
	void PutItemAbilityPiercing( CItemElem* pItemElem, CEditString* pEdit, DWORD dwColorBuf );
	void PutItemResist( CItemElem* pItemElem, CEditString* pEdit );
	void PutItemSpeed( CItemElem* pItemElem, CEditString* pEdit );
	void PutItemMinMax( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit );
	void PutRandomOpt( CItemElem* pItemElem, CEditString* pEdit );
#if __VER >= 11 // __SYS_IDENTIFY
#ifdef __WEAPON_MERGE
	void	PutAwakeningBlessing( CItemElem* pItemElem, CEditString* pEdit, bool bDefault = true );
#else //__WEAPON_MERGE
	void	PutAwakeningBlessing( CItemElem* pItemElem, CEditString* pEdit );
#endif //__WEAPON_MERGE
#endif	// __SYS_IDENTIFY
	void PutPiercingOpt( CItemElem* pItemElem, CEditString* pEdit );
	void PutEnchantOpt( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit, int flag );
#ifdef __WIKIPEDIA_ITEMS
	void PutSetItemOptWiki(CMover* pMover, CItemElem* pItemElem, CEditString* pEdit);
#endif //__WIKIPEDIA_ITEMS

#ifdef __VENDING_NPC
	void PutSkillTime(CItemElem* pItemElem, CEditString* pEdit, int nFlag);
	void PutCooldown(CItemElem* pItemElem, CEditString* pEdit, int nFlag);
#endif // __VENDING_NPC

	void PutSetItemOpt( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit );
	void PutBaseItemOpt( CItemElem* pItemElem, CEditString* pEdit );
	void PutMedicine( CItemElem* pItemElem, DWORD dwParam, LONG nParamVal, CEditString* pEdit );

#ifndef __VENDING_NPC
#ifdef __ANARCHY_SYSTEM
	void PutSkillTime(CItemElem* pItemElem, CEditString* pEdit, int nFlag);
	void PutCooldown(CItemElem* pItemElem, CEditString* pEdit, int nFlag);
#endif // __ANARCHY_SYSTEM
#endif // __VENDING_NPC

	void PutBaseResist( CItemElem* pItemElem, CEditString* pEdit );
	void PutItemGold( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit, int flag );
	void PutCoolTime( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit );
	void PutKeepTime( CItemElem* pItemElem, CEditString* pEdit );
#ifdef __AEGON_FIXES
	void PutCommand(CItemElem* pItemElem, CEditString* pEdit, int flag);
#else
	void PutCommand( CItemElem* pItemElem, CEditString* pEdit );
#endif
	void PutEndurance( CItemElem* pItemElem, CEditString* pEdit, int flag );
	void PutSex( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit );
	void PutJob( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit );
	void PutLevel( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit );
	void PutWeapon( CItemElem* pItemElem, CEditString* pEdit );
#if __VER >= 9 // __CSC_VER9_1
	void PutAddedOpt( CItemElem* pItemElem, CEditString* pEdit );
	void PutPetInfo( CItemElem* pItemElem, CEditString* pEdit );
	void PutPetFeedPocket( CItemElem* pItemElem, CEditString* pEdit );
#endif //__CSC_VER9_1

#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	void	PutSealChar( CItemElem* pItemElem, CEditString* pEdit );
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

#if __VER >= 15 // __PETVIS
	void PutNeededVis( CItemElem* pItemElem, CEditString* pEdit );
	void PutVisPetInfo( CItemElem* pItemElem, CEditString* pEdit );
	void PutPetKind( CItemElem* pItemElem, CEditString* pEdit );
	BOOL CheckConfirm( CItemBase* pItem );
#endif // __PETVIS

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	void PutEquipItemText( CEditString* pEdit );
#endif // __IMPROVE_SYSTEM_VER15

#ifdef __LOOKCHANGE
	void PutLookText( CItemElem *pItemElem, CEditString *pEdit );
#endif // __LOOKCHANGE

#ifdef __BLING_BLING
	void PutBlingBlingText(CItemElem* pItemElem, CEditString* pEdit);
#endif // __BLING_BLING

#ifdef __WEAPON_MERGE
	void MakeMergeWeaponText(CItemBase* pSource, CEditString& Edit);
	void PutMergeWeaponText(CItemElem* pItemElem, CEditString* pEdit);
#endif //__WEAPON_MERGE
	 
#ifdef __PROPPACK_TOOLTIP
	void CWndMgr::PutPackItemInfo(CItemElem* pItemElem, CEditString* pEdit);
#endif //__PROPPACK_TOOLTIP

#ifdef __CRYSTAL
	void PutCrystalText(CItemElem* pItemElem, CEditString* pEdit);
#endif // __CRYSTAL

	CWndMgr();
	virtual	~CWndMgr();

	void	DestroyApplet();
	void	AlighWindow( CRect rcOld, CRect rcNew );

	//virtual CItem* GetFocusItem();
	virtual	BOOL    Process();
	virtual	BOOL	Initialize(CWndBase* pWndParent = NULL);
	virtual	void	OnInitialUpdate();
	virtual	void	OnDestroyChildWnd(CWndBase* pWndChild);
	virtual	void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void	OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual	BOOL	OnCommand(UINT nID,DWORD dwMessage);
	virtual	void	OnSize(UINT nType, int cx, int cy);
	virtual BOOL    OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual	BOOL	OnEraseBkgnd(C2DRender* p2DRender);
	virtual	BOOL	OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);

	DWORD	GetAppletId( TCHAR* lpszAppletName );
	CWndBase* GetApplet( DWORD dwIdApplet );
	CWndBase* CreateApplet( DWORD dwIdApplet );
#ifdef __CHANGE_HOT_KEY
	void	AddAppletFunc(LPVOID pAppletFunc, DWORD dwIdApplet, LPCTSTR lpszKeyword, LPCTSTR pszIconName, LPCTSTR lpszAppletDesc = NULL, CHAR cHotkey = 0, LPCTSTR lpszAppletText = NULL, DWORD dwAppletAuth = NULL_ID);
#else // __CHANGE_HOT_KEY
	void	AddAppletFunc(LPVOID pAppletFunc, DWORD dwIdApplet, LPCTSTR lpszKeyword, LPCTSTR pszIconName, LPCTSTR lpszAppletDesc = NULL, CHAR cHotkey = 0);
#endif // __CHANGE_HOT_KEY

	HRESULT	RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	
//	Initialize player
	void SetPlayer( CMover* pMover );

	BOOL LoadRegInfo( LPCTSTR lpszFileName );
	BOOL SaveRegInfo( LPCTSTR lpszFileName );
	LPWNDREGINFO GetRegInfo( DWORD dwWndId );
	BOOL PutRegInfo( DWORD dwWndId, CRect rect, BOOL bOpen );
	BOOL PutRegInfo( LPWNDREGINFO lpRegInfo );
	BOOL PutRegInfo( CWndNeuz* pWndNeuz, BOOL bOpen );


	CWndBase* GetWndVendorBase( void );

#if __VER >= 8 //__CSC_VER8_1
	void ClearAllWnd();
	void RestoreWnd();
#endif //__CSC_VER8_1

#if __VER >= 12 // __CSC_VER12_4
	DWORD GetDSTStringId(int nDstParam);
	BOOL IsDstRate(int nDstParam);
#endif //__CSC_VER12_4

#ifdef __WINDOW_INTERFACE_BUG
	void CloseBoundWindow(void); // 아이템이 걸려 있거나, 아이템을 조작할 가능성이 있는 모든 창을 강제로 닫는다.
#endif // __WINDOW_INTERFACE_BUG

#ifdef __CHANGE_HOT_KEY
	void SaveHotkey();
	void LoadHotkey();
#endif // __CHANGE_HOT_KEY

	friend CWndApplet;
protected:
	void __HotKeyChange( DWORD dwId, char* pch );
	void	FormalizeChatString( CString & str );

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
public:
	void OnClientResize(int clientWidth, int clientHeight, int oldClientWidth, int oldClientHeight);
#endif // __AEGON_FULLSCREEN_AND_RESIZE

#ifdef __AEGON_THEME_SWITCHER
public:
	void OnThemeSelectionChangedTrigger(BOOL bTypeChanged);
#endif // __AEGON_THEME_SWITCHER

#ifdef __AEGON_FONT_SWITCH
	void OnAegonFontSwitch();
	void SwitchFontChildren(CWndBase* pWndBase);
#endif // __AEGON_FONT_SWITCH

#ifdef __NAVIGATOR_V19
	BOOL OpenMinimap();
	void RestartMinimap();
#endif // __NAVIGATOR_V19

#ifdef __AEGON_INGAME_DP
	CWndIngameDPPurchaseConfirm* OpenDPConfirmPurchase();
#endif

#ifdef __AEGON_INGAME_SHOP
	void PutToolTip_ShopPackage(DWORD dwPackID, CPoint point, CRect* pRect);
#endif

#ifdef __AEGON_TOURNAMENT
	void ProcessTournament();
#endif

#ifdef __AEGON_LOGOUT_CHAR_SELECT
	BOOL m_bLogoutToCharSelect;
	BOOL m_bSkipServerSelect;
	char m_szLogoutName[MAX_PLAYER];
	char m_szLogoutPw[MAX_SESSION_PWD];
#endif
};

#ifdef __VS2003
extern CPtrArray      m_wndOrder;
#endif

extern CWndMgr          g_WndMng; // 윈도 매니저 클래스 

extern void RenderEnchant( C2DRender* p2DRender, CPoint pt );
extern void RenderRadar( C2DRender* p2DRender, CPoint pt, DWORD dwValue, DWORD dwDivisor );
#define WID_MESSAGEBOX        10

#endif // !defined(AFX_WNDMANAGER_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
