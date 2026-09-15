#include "stdafx.h"
#include "defineSkill.h"
#include "defineItem.h"
#ifdef __V19_FORMATS
#include "defineItemGrade.h"
#include "defineItemType.h"
#endif //__V19_FORMATS
#include "defineSound.h"
#include "defineText.h"
#include "AppDefine.h"
//#include "WndActiveDesktop.h"
#include "WndMessenger.h"
#include "WndCommand.h"
#include "WndNotice.h"
#include "WndNotice.h"
#include "WndMap.h"
#include "WndPartyConfirm.h"
#include "WndPartyChangeTroup.h"
#include "WndFriendConfirm.h"
#include "WndDuelConfirm.h"
#include "WndRegVend.h"
#include "WndText.h"
#include "party.h"
#include "DPClient.h"
#include "DPLoginClient.h"
#include "dpcertified.h"
#include "ClientMsg.h"
#include "yUtil.h"
#include "worldmap.h"
#include "eveschool.h"
extern CGuildCombat g_GuildCombatMng;
#include "post.h"
#if __VER >= 11 // __SYS_POCKET
#include "wndbagex.h"
#endif	// __SYS_POCKET

#if __VER >= 13 // __COUPLE_1117
#include "couplehelper.h"
#endif	// __COUPLE_1117

#ifdef __Y_INTERFACE_VER3
#include "WorldMap.h"
#endif //__Y_INTERFACE_VER3

#include "WndSelectVillage.h"

#if __VER >= 11 // __SYS_IDENTIFY
#include "randomoption.h"
#endif	// __SYS_IDENTIFY

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#if __VER >= 12 // __LORD
#include "definelordskill.h"
#endif
#if __VER >= 12 // __SECRET_ROOM
#include "SecretRoom.h"
#endif // __SECRET_ROOM

#ifdef __PROPPACK_TOOLTIP
#include "Project.h"
#endif //__PROPPACK_TOOLTIP

#include "webbox.h"

extern	CParty	g_Party;

extern	CDPClient	g_DPlay;	 
extern	CDPLoginClient	g_dpLoginClient;
extern	CDPCertified	g_dpCertified;

#include "dpclient.h"
extern	CDPClient	g_DPlay;

#include "guild.h"
extern	CGuildMng	g_GuildMng;
#include "guildwar.h"
extern	CGuildWarMng	g_GuildWarMng;

#if __VER >= 12 // __TAX
#include "Tax.h"
#endif // __TAX

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#include "WndGuildHouse.h"
#endif // __GUILD_HOUSE

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
#include "WndQuest.h"
#endif // __IMPROVE_QUEST_INTERFACE

#ifdef __CERTIFIER_COLLECTING_SYSTEM
#include "DPCollectClient.h"
#endif // __CERTIFIER_COLLECTING_SYSTEM

#ifdef __AEGON_THEME_SWITCHER
#include "WndVendor.h"
#endif // __AEGON_THEME_SWITCHER

extern _ERROR_STATE g_Error_State;

extern DWORD FULLSCREEN_WIDTH;
extern DWORD FULLSCREEN_HEIGHT;

#define CLIENT_WIDTH FULLSCREEN_WIDTH
#define CLIENT_HEIGHT FULLSCREEN_HEIGHT

#ifdef __AEGON_AWAKE_COLOR
#define MAX_AWAKE_COLOR D3DCOLOR_ARGB(255, 255, 0, 0)
#endif // __AEGON_AWAKE_COLOR

#ifdef __VS2003
CPtrArray      m_wndOrder;
#endif

CWndMgr          g_WndMng;


const char* GetATKSpeedString( float fSpeed )
{
	if( fSpeed < 0.035 )
		return prj.GetText(TID_GAME_VERYSLOW);  // "아주 느림"
	else if( fSpeed < 0.050 )
		return prj.GetText(TID_GAME_SLOW);		//"느림";
	else if( fSpeed < 0.070 )
		return prj.GetText(TID_GAME_NORMALS);	// "보통";
	else if( fSpeed < 0.080 )
		return prj.GetText(TID_GAME_FAST);		//"빠름";
	else if( fSpeed < 0.17 )
		return prj.GetText(TID_GAME_VERYFAST);	//"아주 빠름";
	else
		return prj.GetText(TID_GAME_FASTEST);	//"아주 빠름";
}

CTexture*	   g_pCoolTexArry[128];

CTexture*	   g_pEnchantTexArry[11];

void RenderEnchant( C2DRender* p2DRender, CPoint pt )
{
	static int i=0;
	
	CPoint ptBack = pt;
	CPoint ptBack_size( 32, 32 );

	i++;

	if( i>=11 )
		i = 0;
	
	p2DRender->RenderTextureEx( ptBack, ptBack_size, g_pEnchantTexArry[i], 255, 1.0f, 1.0f ) ;
}

// 레이더 모양을 그린다.
void RenderRadar( C2DRender* p2DRender, CPoint pt, DWORD dwValue, DWORD dwDivisor )
{
	pt.y += 1;
	pt.x += 15;
	
	CPoint pt_size( 15, 15 );
	const BYTE bAlpha = 200;
	
	//	float fPercent = (float)dwValue / (float)dwDivisor;
	//	int nDrawIndex = (fPercent * 127);
	int nDrawIndex = MulDiv( dwValue, 127, dwDivisor );
	
	CPoint ptBack = pt;
	CPoint ptBack_size( 32, 32 );
	ptBack.x -= 15;
	p2DRender->RenderFillRect( CRect( ptBack, ptBack+ptBack_size ), D3DCOLOR_ARGB( 120, 255,255,255 ) );
	
	if( nDrawIndex >= 0 && nDrawIndex < 32 )
	{
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[nDrawIndex], bAlpha, 1.1f, 1.1f );
		
		pt.y+=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[32], bAlpha, 1.1f, 1.1f );
		
		pt.x-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[64], bAlpha, 1.1f, 1.1f );
		
		pt.y-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[96], bAlpha, 1.1f, 1.1f );
	}
	else
	if( nDrawIndex >= 32 && nDrawIndex <= 63 )
	{
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[31], bAlpha, 1.1f, 1.1f );
		
		pt.y+=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[nDrawIndex], bAlpha, 1.1f, 1.1f );
		
		pt.x-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[64], bAlpha, 1.1f, 1.1f );
		
		pt.y-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[96], bAlpha, 1.1f, 1.1f );
	}
	else
	if( nDrawIndex >= 64 && nDrawIndex <= 95 )
	{
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[31], bAlpha, 1.1f, 1.1f );
		
		pt.y+=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[63], bAlpha, 1.1f, 1.1f );
		
		pt.x-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[nDrawIndex], bAlpha, 1.1f, 1.1f );
		
		pt.y-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[96], bAlpha, 1.1f, 1.1f );
	}
	else
	if( nDrawIndex >= 96 && nDrawIndex <= 128 )
	{
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[31], bAlpha, 1.1f, 1.1f );
		
		pt.y+=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[63], bAlpha, 1.1f, 1.1f );
		
		pt.x-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[95], bAlpha, 1.1f, 1.1f );
		
		pt.y-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[nDrawIndex], bAlpha, 1.1f, 1.1f );
	}	
				
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CWndMgr : 매니저 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define TASKBAR_HEIGHT 38

CWndMgr::CWndMgr()
{
	m_bTitle = FALSE;
	m_pWndActiveDesktop = NULL;
	m_pWndShop = NULL;
	m_pWndDialog = NULL;
	m_pWndRevival = NULL;
	m_pReWanted		  = NULL;
	m_pWanted		  = NULL;
	m_pWndResurrectionConfirm = NULL;
	m_pLogOutWaitting = NULL;
	m_pWndCommItemDlg = NULL;
	m_pWndChangeClass1 = NULL;
	m_pWndChangeClass2 = NULL;
	m_pWndTradeGold = NULL;
	m_pWndDropItem = NULL;
	m_pWndQuickList = NULL;
	m_pWndCommand = NULL;
	m_pWndTaskBar = NULL;
	m_pWndTrade = NULL;
	//m_pWndNotice = NULL;
	////m_pWndTip = NULL;
	//m_bTitle = TRUE;
	m_pWndBeautyShop = NULL;
#if __VER >= 8 //__CSC_VER8_4
	m_pWndFaceShop = NULL;
#ifdef __NEWYEARDAY_EVENT_COUPON
	m_pWndUseCouponConfirm = NULL;
#endif //__NEWYEARDAY_EVENT_COUPON
#endif //__CSC_VER8_4
#if __VER >= 8 //__CSC_VER8_5
	m_pWndSummonAngel = NULL;
#endif //__CSC_VER8_5
	m_pWndDropConfirm = NULL;
	m_nAlphaCount = m_nAlpha;
	m_pWndBank = NULL;
	m_pWndConfirmBank = NULL;
	m_pWndBankPassword = NULL;
	m_pWndConfirmTrade = NULL;

#ifdef __EVE_MINIGAME
	m_pWndKawiBawiBoGame = NULL;
	m_pWndKawiBawiBoGameConfirm = NULL;
	m_pWndFindWordGame = NULL;
	m_pWndDiceGame = NULL;
	m_pWndPuzzleGame = NULL;
#endif //__EVE_MINIGAME

#if __VER >= 9 // __CSC_VER9_1
	m_pWndSmeltMixJewel = NULL;
	m_pWndSmeltJewel = NULL;
	m_pWndExtraction = NULL;
	m_pWndChangeWeapon = NULL;
	m_pWndPetStatus = NULL;

#if __VER >= 15 // __PETVIS
	m_pWndBuffPetStatus = NULL;
	m_pWndConfirmVis = NULL;
#endif

//	m_pWndPetFood = NULL;
	m_pWndFoodConfirm = NULL;
	m_pWndPetMiracle = NULL;
	m_pWndPetFoodMill = NULL;
	m_pWndPetLifeConfirm = NULL;
#if __VER >= 10 // __CSC_VER9_1 -> __LEGEND
	m_pWndHeroSkillUp = NULL;
#endif //__CSC_VER9_1 -> __LEGEND
#endif //__CSC_VER9_1
#if __VER >= 11 // __PIERCING_REMOVE
	m_pWndRemovePiercing = NULL;
#endif //__PIERCING_REMOVE
#if __VER >= 12 // __CSC_VER12_4
	m_pWndRemoveJewel = NULL;
#endif //__CSC_VER12_4
#if __VER >= 12 // __CSC_VER12_5
	m_pWndPetTransEggs = NULL;
#endif //__CSC_VER12_5
#ifdef __TRADESYS
	m_pWndDialogEvent = NULL;
#endif //__TRADESYS
#if __VER >= 12 // __HEAVEN_TOWER
	m_pWndHeavenTower = NULL;
#endif //__HEAVEN_TOWER
#if __VER >= 10 // __REMOVE_ATTRIBUTE
	m_pWndRemoveAttribute = NULL;
#endif //__REMOVE_ATTRIBUTE
	m_pWndPost = NULL;
	m_pWndQueryEquip = NULL;
	m_pWndSelectVillage = NULL;
	m_pWndQuestItemWarning = NULL;
	m_pWndPenaltyPK = NULL;
	m_pWndPartyChangeName = NULL;
	m_pWndPartyConfirm = NULL;
	m_pWndFriendConfirm = NULL;
	m_pWndDuelConfirm = NULL;
#ifdef __MIKSIK_EQUALIZED_MODE
	m_WndSelectTmpClass = NULL;
#endif //__MIKSIK_EQUALIZED_MODE
#if __VER >= 8     // 8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World
	m_pWndDuelConfirm = NULL;
#endif	// __VER >= 8  

	m_pWndMessageNote = NULL;
	m_pWndMessengerNote = NULL;
	
	m_pWndRandomScrollConfirm = NULL;
	
#ifdef __S_SERVER_UNIFY
	m_bAllAction = TRUE;
#endif // __S_SERVER_UNIFY
	
//	m_pWndMessengerSearch	= NULL;
	m_pWndStateConfirm		= NULL;

	m_szTimerChat[ 0 ] = 0;
	m_timerChat.Set( 0 );
	m_bConnect = FALSE;
	m_timerMessenger.Set( MIN( 5 ) );

	for( int i = 0 ; i < MAX_SKILL ; i++ )
	{
		m_dwSkillTime[i] = 0;
	}
	
	m_pWndCollecting = NULL;
	m_pWndRankGuild = NULL;
	m_pWndRankInfo = NULL;
	m_pWndRankWar = NULL;
	m_pWndGuildBank = NULL;
#if __VER >= 11 // __GUILD_BANK_LOG
	m_pWndGuildBankLog = NULL;
#endif //__GUILD_BANK_LOG
	m_pWndGuildConfirm = NULL;
	m_pWndGuildWarPeaceConfirm = NULL;
	m_pWndGuildWarRequest = NULL;
	m_pWndGuildWarState = NULL;
	m_pWndGuildCombatRanking = NULL;

	m_pWndFontEdit = NULL;
	
	m_pWndReSkillWarning = NULL;
	m_pWndGuildCombatBoard      = NULL;	
	m_pWndGuildCombatOffer		= NULL;
	m_pWndGuildCombatSelection	= NULL;
	m_pWndGuildCombatJoinSelection = NULL;
	m_pWndGuildCombatInfoMessageBox2 = NULL;
	m_pWndGuildCombatInfoMessageBox = NULL;
	n_pWndGuildCombatResult = NULL;
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	m_pWndGuildCombat1to1Selection = NULL;
	m_pWndGuildCombat1to1Offer = NULL;
#endif //__GUILD_COMBAT_1TO1
#if __VER >= 8 //__Y_CHAT_SYSTEM_8
	m_pWndChatLog = NULL;
#endif //__Y_CHAT_SYSTEM_8

#if __VER >= 12 // __SECRET_ROOM
	m_pWndSecretRoomMsg = NULL;
	m_pWndSecretRoomSelection = NULL;
	m_pWndSecretRoomOffer = NULL;
	m_pWndSecretRoomOfferState = NULL;
	m_pWndSecretRoomChangeTaxRate = NULL;
	m_pWndSecretRoomCheckTaxRate = NULL;
	m_pWndSecretRoomBoard = NULL;
	m_pWndSecretRoomQuick = NULL;
#endif //__SECRET_ROOM

#if __VER >= 13 // __RAINBOW_RACE
	m_pWndRainbowRaceOffer = NULL;
	m_pWndRainbowRaceInfo = NULL;
	m_pWndRainbowRaceRule = NULL;
	m_pWndRainbowRaceRanking = NULL;
	m_pWndRainbowRacePrize = NULL;
	m_pWndRainbowRaceMiniGameButton = NULL;
	m_pWndRainbowRaceMiniGame = NULL;
	m_pWndRainbowRaceMiniGameEnd = NULL;
	m_pWndRRMiniGameKawiBawiBo = NULL;
	m_pWndRRMiniGameDice = NULL;
	m_pWndRRMiniGameArithmetic = NULL;
	m_pWndRRMiniGameStopWatch = NULL;
	m_pWndRRMiniGameTyping = NULL;
	m_pWndRRMiniGameCard = NULL;
	m_pWndRRMiniGameLadder = NULL;
#endif //__RAINBOW_RACE
#if __VER >= 13 // __CSC_VER13_2
	m_pWndCoupleMessage = NULL;
#endif //__CSC_VER13_2
#if __VER >= 13 // __EXT_ENCHANT
	m_pWndChangeAttribute = NULL;
#endif //__EXT_ENCHANT
#if __VER >= 12 // __LORD
	m_pWndLordState = NULL;
	m_pWndLordTender = NULL;
	m_pWndLordVote = NULL;
	m_pWndLordEvent = NULL;
	m_pWndLordSkillConfirm = NULL;
	m_pWndLordInfo		= NULL;
	m_pWndLordRPInfo	= NULL;
	m_bIsOpenLordSkill = false;
#endif
#if __VER >= 12 // __PET_0519
	m_pWndPetAwakCancel = NULL;
#endif
#ifdef __AZRIA_1023
	m_pWndSelectCh = NULL;
#endif
#if __VER >= 13 // __HOUSING
m_pWndRoomList = NULL;
m_pWndQuitRoom = NULL;
#endif // __HOUSING
#ifdef __PET_1024
m_pWndChangePetName = NULL;
#endif

	m_pWndUpgradeBase = NULL;
	m_pWndPiercing = NULL;
	m_pWndInvenRemoveItem = NULL;
	m_pWndCommerialElem = NULL;
	m_pRemoveElem = NULL;
	m_pQuestItemInfo = NULL;
	m_pWndTextBook = NULL;
	m_pWndTextScroll = NULL;
	m_pWndTextLetter = NULL;
	m_pWndMessageBox = NULL;
	m_pWndMessageBoxUpper = NULL;
	m_dwSavePlayerTime = 0;
	m_bAutoRun = FALSE;
	m_pWndChangeName			= NULL;
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	m_pWndSealChar			= NULL;
	m_pWndSealCharSelect			= NULL;
	m_pWndSealCharSend			= NULL;
	m_pWndSealCharSet			= NULL;
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
#if __VER >= 11 // __SYS_IDENTIFY
	m_pWndAwakening		= NULL;
	m_pPetRes			= NULL;
	m_pWndBlessingCancel = NULL;
#endif
#ifdef __FUNNY_COIN
	m_pFunnyCoinConfirm = NULL;
#endif //__FUNNY_COIN
#if __VER >= 14 // __SMELT_SAFETY
	m_pWndSmeltSafety = NULL;
	m_pWndSmeltSafetyConfirm = NULL;
#endif //__SMELT_SAFETY
#if __VER >= 14 // __EQUIP_BIND
	m_pWndEquipBindConfirm = NULL;
#endif // __EQUIP_BIND
#if __VER >= 14 // __RESTATE_CONFIRM
	m_pWndRestateConfirm = NULL;
#endif // __RESTATE_CONFIRM
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	m_pWndQuestQuickInfo = NULL;
	m_pWndQuestDetail = NULL;
#endif // __IMPROVE_QUEST_INTERFACE
#if __VER >= 15 // __CAMPUS
	m_pWndCampusInvitationConfirm = NULL;
	m_pWndCampusSeveranceConfirm = NULL;
#endif // __CAMPUS
#ifdef __HELP_BUG_FIX
	m_pWndHelpInstant = NULL;
#endif //__HELP_BUG_FIX
#if __VER >= 8 // __CSC_VER8_1
	m_clearFlag = FALSE;
#endif //__CSC_VER8_1
	InitSetItemTextColor();

	// 텍스쳐렌더 버그 관련 수정
	memset(g_pEnchantTexArry, 0, sizeof(CTexture*) * 11);
	memset(g_pCoolTexArry, 0, sizeof(CTexture*) * 128);

#if __VER >= 15 // __GUILD_HOUSE
	m_pWndGHMain = NULL;
	m_pWndUpkeep = NULL;
#endif

#ifdef __BAN_CHATTING_SYSTEM
	m_nWarningCounter = 0;
	m_nWarning2Counter = 0;
	m_bShortcutCommand = FALSE;
#endif // __BAN_CHATTING_SYSTEM

#ifdef __PROTECT_AWAKE
	m_pWndSelectAwakeCase = NULL;
#endif //__PROTECT_AWAKE

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
	m_pWndChattingBlockingList = NULL;
#endif // __YS_CHATTING_BLOCKING_SYSTEM

#ifdef __GUILD_HOUSE_MIDDLE
	m_pWndGHBid = NULL;
#endif //__GUILD_HOUSE_MIDDLE

#ifdef __MAIL_REQUESTING_BOX
	m_bWaitRequestMail = FALSE;
#endif //__MAIL_REQUESTING_BOX

#ifdef	__COLOSSEUM
	m_pColosseumComplete = NULL;
	m_pColosseumEnter	= NULL;
	m_pColosseumMassage	= NULL;
	m_pColosseumRankGuild	= NULL;
	m_pColosseumRankNomal	= NULL;
	m_pColosseumRetry	= NULL;
	m_pColosseumStart	= NULL;
	m_pColosseumStartReady1	= NULL;
	m_pColosseumStartReady2	= NULL;
#endif // __COLOSSEUM

#ifdef __NEW_ITEM_VARUNA
	m_pWndBarunaNPC = NULL;
	m_pWndBarunaExtractOffer = NULL;
#ifdef __BARUNA_PIERCING
	m_pWndBarunaPiercing = NULL;
#endif // __BARUNA_PIERCING
#endif // __NEW_ITEM_VARUNA

#ifdef __ITEMLINK
	m_pWndLinkedItem = NULL;
#endif //__ITEMLINK

#ifdef __TREASUREBOX
	m_pWndTreasureBox = NULL;
	m_pWndTreasureRecieve = NULL;
#endif//__TREASUREBOX

#ifdef __AEGON_PETMISTAKE
	m_pWndPetMistakeCustom = NULL;
#endif // __AEGON_PETMISTAKE

#ifdef __AEGON_MONSTER_HUNT
	m_pWndMonsterHunt = NULL;
#endif // __AEGON_MONSTER_HUNT

#ifdef __LEEAEG_SHOP_FINDER
	m_pWndShopFinder = NULL;
#endif //__LEEAEG_SHOP_FINDER

#ifdef __GIFTBOX_LIST_ITEMS
	m_pWndGiftbox = nullptr;
#endif // __GIFTBOX_LIST_ITEMS

#ifdef __AEGON_RETURN_AWAKE
	m_pWndReturnAwakening = NULL;
#endif // __AEGON_RETURN_AWAKE

#ifdef __VENDING_NPC
	m_pWndVendorWarning = NULL;
#endif // __VENDING_NPC

#ifdef __WEAPON_RARITY
	m_pWndWeaponRarity = NULL;
#endif // __WEAPON_RARITY

#ifdef __SYS_CUSTOM_TITLE
	m_pCreateOwnTitle = NULL;
#endif // __SYS_CUSTOM_TITLE

#ifdef __SYS_PARTYFINDER
	m_pWndPartyFind = NULL;
#endif // __SYS_PARTYFINDER

#ifdef __ANARCHY_SYSTEM
	m_pWndAnarchy = NULL;
	m_pWndAnarchySkillConfirm = NULL;
#endif // __ANARCHY_SYSTEM

#ifdef __AEGON_TELEPORTER
	m_pWndTeleporter = NULL;
#endif

#ifdef __AEGON_LOOT_BOX
	m_pWndLootBoxMain = NULL;
#endif // __AEGON_LOOT_BOX

#ifdef __AEGON_VIEW_INV
	m_pWndViewInventoryMain = NULL;
	m_pWndViewInventoryCtrl = NULL;
	m_pWndViewBag = NULL;
#endif // __AEGON_VIEW_INV

#ifdef __SYS_PETFILTER
	m_pWndPetfilter = NULL;
#endif //__SYS_PETFILTER

#ifdef __GUILDFINDER
	m_pWndGuildFinder = NULL;
	m_pWndGuildFinderReq = NULL;
#endif //__GUILDFINDER

#ifdef	__REMOVE_DURATION_ITEMS
	m_pWndRemoveCmm = NULL;
#endif	//__REMOVE_DURATION_ITEMS

#ifdef __SYS_MONSTER_CLASH
	pWndMonsterClashRegister = nullptr;
	pWndMonsterClashRegisterMember = nullptr;
#endif //__SYS_MONSTER_CLASH

#ifdef __SYS_SANDBOX
	m_pWndSandbox = nullptr;
#endif //__SYS_SANDBOX

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
	resizeWidth = 0;
	resizeHeight = 0;
#endif // __AEGON_FULLSCREEN_AND_RESIZE

#ifdef __AEGON_CRAFTING_SYSTEM
	m_pWndCraftingMain = NULL;
#endif // __AEGON_CRAFTING_SYSTEM

#ifdef __WIKIPEDIA_MONSTER_DROPLIST
	m_pWndDropInfo = NULL;
#endif //__WIKIPEDIA_MONSTER_DROPLIST

#ifdef __AEGON_ADVANCED_ANNOUNCEMENT
	m_pWndAegonSystemShout = NULL;
#endif // __AEGON_ADVANCED_ANNOUNCEMENT

#ifdef __SWITCH_EQUIP
	m_pWndSwitchEquip = NULL;
#endif // __SWITCH_EQUIP

#ifdef __MODS_MANAGER
	m_pWndModsManager = NULL;
#endif // __MODS_MANAGER

#ifdef __AEGON_FONT_SWITCH
	m_dwFontSwitched = 0;
#endif // __AEGON_FONT_SWITCH

#ifdef __PET_GLOW
	m_pWndPetGlowChange = NULL;
#endif // __PET_GLOW
#ifdef __WEAPON_MERGE
	m_pWndWeaponMerge = NULL;
	m_pWndWeaponMergeCancel = NULL;
#endif // __WEAPON_MERGE

#ifdef __LOOKCHANGE
	m_pWndLookChange = NULL;
#endif // __LOOKCHANGE

#ifdef __FASHION_COMBINE
	m_pWndFashionCombine = NULL;
#endif // __FASHION_COMBINE

#ifdef __AEGON_INGAME_DP
	m_pWndDPPurchaseConfirm = NULL;
#endif

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
	m_pWndGuildCombatJoinConfirm = NULL;
	m_pWndGuildCombatCancelConfirm = NULL;
	m_pWndSecretRoomCancelConfirm = NULL;
	m_pWndSummonParty = NULL;
	m_pWndSummonFriendConfirm = NULL;
	m_pWndSummonPartyConfirm = NULL;
	m_pWndSummonFriend = NULL;
	m_pWndSummonPartyUse = NULL;

	m_pWndGuildName = NULL;

	m_pWndGuildNickName = NULL;

	m_pWndTradeConfirm = NULL;
#endif

#ifdef __AEGON_TOURNAMENT
	m_pWndAegonTournamentScoreBoard = NULL;
	m_pWndAegonTournament = NULL;
#endif // __AEGON_TOURNAMENT

#ifdef __ROYAL_RUMBLE
	m_pJoinPlayerSiege = nullptr;
	m_pSiegeRanking = nullptr;
#endif // __ROYAL_RUMBLE

#ifdef __AEGON_FIXES
	m_pWndRepairItem = NULL;
	m_pWndLvReqDown = NULL;
#endif

#ifdef __AEGON_TIME_DIFF
	m_nTimeDiff = 0;
#endif

#ifdef __AEGON_FIXES
	m_bSetOtherNull = FALSE;
#endif

#ifdef __AEGON_PVP_MINIGAMES
	m_pWndAegonPvPGameCreate = NULL;
	m_pWndAegonPvPGameInvitation = NULL;
	m_pWndAegonPvPGameMain = NULL;
	m_pWndAegonPvPGameMessageBox = NULL;
	m_pWndAegonPvPGameFinder = NULL;
#endif

#ifdef __DPS_METER
	m_bToggledDpsMeter = FALSE;
#endif

#ifdef __AEGON_LOGOUT_CHAR_SELECT
	m_bSkipServerSelect = FALSE;
	m_bLogoutToCharSelect = FALSE;
	memset(m_szLogoutName, 0, MAX_PLAYER);
	memset(m_szLogoutPw, 0, MAX_SESSION_PWD);
#endif

}

void CWndMgr::InitSetItemTextColor( )
{
	
//	dwItemColor[FIRST_TC].dwName0				= D3DCOLOR_XRGB(   0,   0, 255 );			// 이름 0단계
//	dwItemColor[FIRST_TC].dwName0				= D3DCOLOR_XRGB(  12, 144, 231 );
	dwItemColor[FIRST_TC].dwName0				= D3DCOLOR_XRGB(  46, 112, 169 );
	dwItemColor[FIRST_TC].dwName1				= D3DCOLOR_XRGB(   0,  93,   0 );			// 이름 1단계
	dwItemColor[FIRST_TC].dwName2				= D3DCOLOR_XRGB( 128,  64,   0 );			// 이름 2단계
//	dwItemColor[FIRST_TC].dwName2				= D3DCOLOR_XRGB( 182,   0, 255 );			// 이름 2단계
#if __VER >= 9 // __CSC_VER9_1
	dwItemColor[FIRST_TC].dwName3				= D3DCOLOR_XRGB( 127,   0, 255 );			// 이름 3단계
#else
	dwItemColor[FIRST_TC].dwName3				= D3DCOLOR_XRGB(   0,   0, 255 );			// 이름 3단계
#endif //__CSC_VER9_1
#ifdef __NEW_ITEM_VARUNA
	dwItemColor[FIRST_TC].dwName4				= D3DCOLOR_XRGB(  75, 105, 140 );			// 이름 4단계
#else // __NEW_ITEM_VARUNA
	dwItemColor[FIRST_TC].dwName4				= D3DCOLOR_XRGB(   0,   0, 255 );			// 이름 4단계
#endif // __NEW_ITEM_VARUNA
	dwItemColor[FIRST_TC].dwName5				= D3DCOLOR_XRGB(   0,   0, 255 );			// 이름 5단계
#ifdef __PREMIUM
	dwItemColor[FIRST_TC].dwName6				= D3DCOLOR_XRGB(   189, 128, 51);			// 이름 5단계
#endif //__PREMIUM
#ifdef __ANARCHY_SYSTEM
	dwItemColor[FIRST_TC].dwName7				= D3DCOLOR_XRGB(   31, 38, 147);			// 이름 5단계
#endif //__ANARCHY_SYSTEM
	dwItemColor[FIRST_TC].dwGeneral				= D3DCOLOR_XRGB(   0,   0,   0 );			// 일반
	dwItemColor[FIRST_TC].dwPlusOption			= D3DCOLOR_XRGB(   0,   0,   0 );			// 플러스 옵션
	dwItemColor[FIRST_TC].dwPiercing			= D3DCOLOR_XRGB( 178,   0, 255 );			// 피어싱
	dwItemColor[FIRST_TC].dwResist				= D3DCOLOR_XRGB(   0,   0,   0 );			// 속성 일반
	dwItemColor[FIRST_TC].dwResistFire			= D3DCOLOR_XRGB(   0,   0,   0 );			// 속성 불
	dwItemColor[FIRST_TC].dwResistWater			= D3DCOLOR_XRGB(   0,   0,   0 );			// 속성 물
	dwItemColor[FIRST_TC].dwResistEarth			= D3DCOLOR_XRGB(   0,   0,   0 );			// 속성 땅
	dwItemColor[FIRST_TC].dwResistElectricity	= D3DCOLOR_XRGB(   0,   0,   0 );			// 속성 전기
	dwItemColor[FIRST_TC].dwResistWind			= D3DCOLOR_XRGB(   0,   0,   0 );			// 속성 바람
	dwItemColor[FIRST_TC].dwResistSM			= D3DCOLOR_XRGB(   0, 158,   0 );			// 속성 상용화 무기
	dwItemColor[FIRST_TC].dwResistSM1			= D3DCOLOR_XRGB( 255,   0,   0 );			// 속성 상용화 방어구
	dwItemColor[FIRST_TC].dwTime				= D3DCOLOR_XRGB(   0, 150,   0 );			// 시간 및 효과
	dwItemColor[FIRST_TC].dwEffective0			= D3DCOLOR_XRGB(   0,   0,   0 );			// 아이템 효능
	dwItemColor[FIRST_TC].dwEffective1			= D3DCOLOR_XRGB(   0, 255,   0 );			// 아이템 효능1
	dwItemColor[FIRST_TC].dwEffective2			= D3DCOLOR_XRGB( 255,   0,   0 );			// 아이템 효능2
	dwItemColor[FIRST_TC].dwEffective3			= D3DCOLOR_XRGB( 128, 128, 128 );			// 아이템 효능3
	dwItemColor[FIRST_TC].dwRandomOption		= D3DCOLOR_XRGB(   0,   0,   0 );			// 랜덤 옵션
	dwItemColor[FIRST_TC].dwEnchantOption		= D3DCOLOR_XRGB(   0,   0,   0 );			// 인첸트 옵션
	dwItemColor[FIRST_TC].dwSetName				= D3DCOLOR_XRGB(   0,   0,   0 );			// 세트 이름
	dwItemColor[FIRST_TC].dwSetItem0			= D3DCOLOR_XRGB( 178, 178, 178 );			// 세트 목록(비착용시)
	dwItemColor[FIRST_TC].dwSetItem1			= D3DCOLOR_XRGB(   0,   0, 204 );			// 세트 목록(착용시)
	dwItemColor[FIRST_TC].dwSetEffect			= D3DCOLOR_XRGB( 255, 157,   0 );			// 세트 효과
	dwItemColor[FIRST_TC].dwGold				= D3DCOLOR_XRGB(   0,   0,   0 );			// 가격
	dwItemColor[FIRST_TC].dwCommand				= D3DCOLOR_XRGB(   0,   0,   0 );			// 설명
	dwItemColor[FIRST_TC].dwNotUse				= D3DCOLOR_XRGB( 255,  66,   0 );			// 사용못함
#if __VER >= 9 // __CSC_VER9_1
	dwItemColor[FIRST_TC].dwAddedOpt1			= D3DCOLOR_XRGB( 127, 255,   0 );			//얼터멋 웨폰 추가 옵션 "경험치 상승"
	dwItemColor[FIRST_TC].dwAddedOpt2			= D3DCOLOR_XRGB(   0, 255, 255 );			//얼터멋 웨폰 추가 옵션 "데미지 반사"
	dwItemColor[FIRST_TC].dwAddedOpt3			= D3DCOLOR_XRGB( 140,  23,  23 );			//얼터멋 웨폰 추가 옵션 "흡혈"
	dwItemColor[FIRST_TC].dwAddedOpt4			= D3DCOLOR_XRGB( 255, 127,   0 );			//얼터멋 웨폰 추가 옵션 "사냥 데미지 증가"
	dwItemColor[FIRST_TC].dwAddedOpt5			= D3DCOLOR_XRGB( 255, 110, 199 );			//얼터멋 웨폰 추가 옵션 "PvP 데미지 증가"
	dwItemColor[FIRST_TC].dwAddedOpt6			= D3DCOLOR_XRGB( 205, 127,  50 );			//얼터멋 웨폰 추가 옵션 "힘 증가"
	dwItemColor[FIRST_TC].dwAddedOpt7			= D3DCOLOR_XRGB( 255,   0,   0 );			//얼터멋 웨폰 추가 옵션 "체력 증가"
	dwItemColor[FIRST_TC].dwAddedOpt8			= D3DCOLOR_XRGB(  50,  50, 255 );			//얼터멋 웨폰 추가 옵션 "민첩 증가"
	dwItemColor[FIRST_TC].dwAddedOpt9			= D3DCOLOR_XRGB( 147, 112, 219 );			//얼터멋 웨폰 추가 옵션 "지능 증가"
#ifdef __NEW_DIAMONDS
	dwItemColor[FIRST_TC].dwAddedOpt10			= D3DCOLOR_XRGB( 147, 112, 219 );
#endif //__NEW_DIAMONDS
#endif //__CSC_VER9_1
#if __VER >= 11 // __SYS_IDENTIFY
#if __VER >= 12 // __EXT_PIERCING
	dwItemColor[FIRST_TC].dwAwakening	= D3DCOLOR_XRGB( 0, 0, 255 );			// 아이템 각성
	dwItemColor[FIRST_TC].dwBlessing	= D3DCOLOR_XRGB( 0, 0, 255 );			// 여신의 축복
#else //__EXT_PIERCING
	dwItemColor[FIRST_TC].dwAwakening	= D3DCOLOR_XRGB( 178, 0, 255 );			// 아이템 각성
	dwItemColor[FIRST_TC].dwBlessing	= D3DCOLOR_XRGB( 178, 0, 255 );			// 여신의 축복
#endif //__EXT_PIERCING
	dwItemColor[FIRST_TC].dwBlessingWarning	= D3DCOLOR_XRGB( 255, 0, 0 );			// 여신의 축복 경고
#endif	// __SYS_IDENTIFY
	
	memcpy( &dwItemColor[SECOND_TC], &dwItemColor[FIRST_TC], sizeof( ToolTipItemTextColor ) );
	memcpy( &dwItemColor[THIRD_TC], &dwItemColor[FIRST_TC], sizeof( ToolTipItemTextColor ) );
	dwItemColor[SECOND_TC].dwName0				= D3DCOLOR_XRGB(  12, 144, 231 );
	dwItemColor[SECOND_TC].dwResistFire			= D3DCOLOR_XRGB( 230,   0,   0 );			// 속성 불
	dwItemColor[SECOND_TC].dwResistWater		= D3DCOLOR_XRGB(   0,   0, 182 );			// 속성 물
	dwItemColor[SECOND_TC].dwResistEarth		= D3DCOLOR_XRGB( 185, 118,   0 );			// 속성 땅
	dwItemColor[SECOND_TC].dwResistElectricity	= D3DCOLOR_XRGB(  21,  55, 152 );			// 속성 전기
	dwItemColor[SECOND_TC].dwResistWind			= D3DCOLOR_XRGB(   0, 217, 214 );			// 속성 바람
	dwItemColor[SECOND_TC].dwTime				= D3DCOLOR_XRGB( 146,   0,  38 );			// 시간 및 효과
	dwItemColor[SECOND_TC].dwPlusOption			= D3DCOLOR_XRGB( 199, 155,   0 );			// 플러스 옵션
	dwItemColor[SECOND_TC].dwPiercing			= D3DCOLOR_XRGB(  77,  11, 145 );			// 피어싱
	dwItemColor[SECOND_TC].dwRandomOption		= D3DCOLOR_XRGB( 199, 155,   0 );			// 랜덤 옵션
	dwItemColor[SECOND_TC].dwSetName			= D3DCOLOR_XRGB(  50, 178,   0 );			// 세트 이름
	dwItemColor[SECOND_TC].dwSetItem0			= D3DCOLOR_XRGB( 178, 178, 178 );			// 세트 목록(비착용시)
	dwItemColor[SECOND_TC].dwSetItem1			= D3DCOLOR_XRGB( 114, 186,  85 );			// 세트 목록(착용시)
	dwItemColor[SECOND_TC].dwSetEffect			= D3DCOLOR_XRGB(  50, 178,   0 );			// 세트 효과
	dwItemColor[SECOND_TC].dwCommand			= D3DCOLOR_XRGB( 199, 155,   0 );			// 설명
#if __VER >= 11 // __SYS_IDENTIFY
#if __VER >= 12 // __EXT_PIERCING
	dwItemColor[SECOND_TC].dwAwakening	= D3DCOLOR_XRGB( 0, 0, 255 );			// 아이템 각성
	dwItemColor[SECOND_TC].dwBlessing	= D3DCOLOR_XRGB( 0, 0, 255 );			// 여신의 축복
#else //__EXT_PIERCING
	dwItemColor[SECOND_TC].dwAwakening	= D3DCOLOR_XRGB( 178, 0, 255 );			// 아이템 각성
	dwItemColor[SECOND_TC].dwBlessing	= D3DCOLOR_XRGB( 178, 0, 255 );			// 여신의 축복
#endif //__EXT_PIERCING
	dwItemColor[SECOND_TC].dwBlessingWarning	= D3DCOLOR_XRGB( 255, 0, 0 );			// 여신의 축복 경고
#endif	// __SYS_IDENTIFY
}

CWndMgr::~CWndMgr()
{
#ifdef __CHANGE_HOT_KEY
	SaveHotkey();
#endif // __CHANGE_HOT_KEY
	AppletFunc* pApplet;
	DWORD dwIdApplet;
	POSITION pos = m_mapAppletFunc.GetStartPosition();
	while( pos )
	{
		m_mapAppletFunc.GetNextAssoc( pos, dwIdApplet, (void*&)pApplet );
		SAFE_DELETE( (AppletFunc*) pApplet );
	}
	Free();
	
	SAFE_DELETE( m_pWndMenu );
}

void CWndMgr::AlighWindow( CRect rcOld, CRect rcNew )
{
	for(int i = 0; i < m_wndOrder.GetSize(); i++ )
	{
		CWndBase* pWndBase = (CWndBase*) m_wndOrder.GetAt( i );
		CRect rcWnd = pWndBase->GetWindowRect( TRUE );
		CPoint point = rcWnd.TopLeft();
		if( pWndBase != this && pWndBase != m_pWndTaskBar )
		{
			if( pWndBase->m_nWinSize == WSIZE_MAX )
			{
				pWndBase->SetWndRect( rcNew );
			}
			else
			{
				if( rcWnd.left   < rcNew.left   ) point.x = rcNew.left;
				if( rcWnd.top    < rcNew.top    ) point.y = rcNew.top;
				if( rcWnd.right  > rcNew.right  ) point.x = rcNew.right - rcWnd.Width();
				if( rcWnd.bottom > rcNew.bottom ) point.y = rcNew.bottom - rcWnd.Height();

				if( rcWnd.left   == rcOld.left   ) point.x = rcNew.left;
				if( rcWnd.top    == rcOld.top    ) point.y = rcNew.top;
				if( rcWnd.right  == rcOld.right  ) point.x = rcNew.right - rcWnd.Width();
				if( rcWnd.bottom == rcOld.bottom ) point.y = rcNew.bottom - rcWnd.Height();

				pWndBase->Move( point );
			}
		}
	}
}


void CWndMgr::Free()
{
//	DestroyApplet();
//	AppletFunc* pApplet;
//	DWORD dwIdApplet;
	POSITION pos = m_mapWndRegInfo.GetStartPosition();
	LPWNDREGINFO pWndRegInfo;
	DWORD dwWndId;
	while( pos )
	{
		m_mapWndRegInfo.GetNextAssoc( pos, dwWndId, (void*&)pWndRegInfo );
		SAFE_DELETE( pWndRegInfo->lpArchive );
		SAFE_DELETE( pWndRegInfo );
	}
	m_mapWndRegInfo.RemoveAll();
	
	SAFE_DELETE(m_pWndCollecting);
#if __VER >= 11 // __SYS_IDENTIFY
	SAFE_DELETE(m_pWndAwakening);
	SAFE_DELETE(m_pWndBlessingCancel);
	SAFE_DELETE(m_pWndLvReqDown);
#endif
#ifdef __JEFF_11
	SAFE_DELETE(m_pPetRes);
#endif
	SAFE_DELETE( m_pWndTaskBar );
	SAFE_DELETE( m_pWndActiveDesktop );
	SAFE_DELETE( m_pWndDialog );
	SAFE_DELETE( m_pWndShop );
	SAFE_DELETE( m_pWndRevival );

	SAFE_DELETE(m_pReWanted);
	SAFE_DELETE(m_pWanted);
	SAFE_DELETE(m_pWndResurrectionConfirm);
	SAFE_DELETE(m_pLogOutWaitting);
	SAFE_DELETE( m_pWndCommItemDlg );
	SAFE_DELETE( m_pWndChangeClass1 );
	SAFE_DELETE( m_pWndChangeClass2 );
	
	SAFE_DELETE( m_pWndTradeGold );
	SAFE_DELETE( m_pWndQuickList );
	SAFE_DELETE( m_pWndMessageBox );
	SAFE_DELETE( m_pWndMessageBoxUpper );
	
	SAFE_DELETE( m_pWndCommand );
	SAFE_DELETE( m_pWndTrade );
	m_pWndWorld = NULL;
	SAFE_DELETE( m_pWndBeautyShop );

#if __VER >= 8 //__CSC_VER8_4
	SAFE_DELETE( m_pWndFaceShop );
#ifdef __NEWYEARDAY_EVENT_COUPON
	SAFE_DELETE( m_pWndUseCouponConfirm );
#endif //__NEWYEARDAY_EVENT_COUPON
#endif //__CSC_VER8_4
#if __VER >= 8 //__CSC_VER8_5
	SAFE_DELETE( m_pWndSummonAngel );
#endif //__CSC_VER8_5

#ifdef __EVE_MINIGAME
	SAFE_DELETE( m_pWndKawiBawiBoGame );
	SAFE_DELETE( m_pWndKawiBawiBoGameConfirm );
	SAFE_DELETE( m_pWndFindWordGame );
	SAFE_DELETE( m_pWndDiceGame );
	SAFE_DELETE( m_pWndPuzzleGame );
#endif //__EVE_MINIGAME

#if __VER >= 9 // __CSC_VER9_1
	SAFE_DELETE( m_pWndSmeltMixJewel );
	SAFE_DELETE( m_pWndSmeltJewel );
	SAFE_DELETE( m_pWndExtraction );
	SAFE_DELETE( m_pWndChangeWeapon );
	SAFE_DELETE( m_pWndPetStatus );
#if __VER >= 15 // __PETVIS
	SAFE_DELETE( m_pWndBuffPetStatus );
	SAFE_DELETE( m_pWndConfirmVis );
#endif

//	SAFE_DELETE( m_pWndPetFood );
	SAFE_DELETE( m_pWndFoodConfirm );
	SAFE_DELETE( m_pWndPetMiracle );
	SAFE_DELETE( m_pWndPetFoodMill );
	SAFE_DELETE( m_pWndPetLifeConfirm );
#if __VER >= 10 // __CSC_VER9_1 -> __LEGEND
	SAFE_DELETE( m_pWndHeroSkillUp );
#endif //__LEGEND
#endif //__CSC_VER9_1
#if __VER >= 11 // __PIERCING_REMOVE
	SAFE_DELETE( m_pWndRemovePiercing );
#endif //__PIERCING_REMOVE
#if __VER >= 12 // __CSC_VER12_4
	SAFE_DELETE( m_pWndRemoveJewel );
#endif //__CSC_VER12_4
#if __VER >= 12 // __CSC_VER12_5
	SAFE_DELETE( m_pWndPetTransEggs );
#endif //__CSC_VER12_5
#ifdef __TRADESYS
	SAFE_DELETE( m_pWndDialogEvent );
#endif //__TRADESYS
#if __VER >= 12 // __HEAVEN_TOWER
	SAFE_DELETE( m_pWndHeavenTower );
#endif //__HEAVEN_TOWER
#if __VER >= 10 // __REMOVE_ATTRIBUTE
	SAFE_DELETE( m_pWndRemoveAttribute );
#endif //__REMOVE_ATTRIBUTE
#if __VER >= 12 // __LORD
	SAFE_DELETE(m_pWndLordState);
	SAFE_DELETE(m_pWndLordTender);
	SAFE_DELETE(m_pWndLordVote);
	SAFE_DELETE(m_pWndLordEvent);
	SAFE_DELETE(m_pWndLordSkillConfirm);
	SAFE_DELETE(m_pWndLordInfo);
	SAFE_DELETE(m_pWndLordRPInfo);
#endif
#if __VER >= 13 // __HOUSING
	SAFE_DELETE(m_pWndRoomList);
	SAFE_DELETE(m_pWndQuitRoom);
#endif // __HOUSING
#ifdef __PET_1024
SAFE_DELETE(m_pWndChangePetName);
#endif
#if __VER >= 12 // __PET_0519
	SAFE_DELETE(m_pWndPetAwakCancel);
#endif
#ifdef __AZRIA_1023
	SAFE_DELETE(m_pWndSelectCh);
#endif
	SAFE_DELETE( m_pWndPost );
	SAFE_DELETE( m_pWndBank );
	SAFE_DELETE( m_pWndDropItem );
	SAFE_DELETE(m_pWndQueryEquip);
	SAFE_DELETE( m_pWndDropConfirm );

	SAFE_DELETE( m_pWndPenaltyPK );
	SAFE_DELETE( m_pWndConfirmBank );
	SAFE_DELETE( m_pWndBankPassword );
	SAFE_DELETE( m_pWndPartyChangeName );
	SAFE_DELETE( m_pWndDuelConfirm );

#if __VER >= 8     // 8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World
	SAFE_DELETE( m_pWndDuelConfirm );
#endif	// __VER >= 8  

#ifdef __HELP_BUG_FIX
	SAFE_DELETE( m_pWndHelpInstant );
#endif //__HELP_BUG_FIX

	SAFE_DELETE( m_pWndConfirmTrade );
	SAFE_DELETE( m_pWndRandomScrollConfirm );
	SAFE_DELETE( m_pWndSelectVillage );
	SAFE_DELETE( m_pWndQuestItemWarning );
	SAFE_DELETE( m_pWndPartyConfirm );
	SAFE_DELETE( m_pWndFriendConfirm );
	SAFE_DELETE( m_pWndMessageNote );
	SAFE_DELETE( m_pWndMessengerNote );
	SAFE_DELETE( m_pWndAddFriend );
	SAFE_DELETE( m_pWndStateConfirm );
#ifdef __MIKSIK_EQUALIZED_MODE
	SAFE_DELETE(m_WndSelectTmpClass);
#endif //__MIKSIK_EQUALIZED_MODE
	CString strTemp;
	CWndBase* pWndBaseTemp;
	pos = m_mapMessage.GetStartPosition();
	while( pos )
	{
		m_mapMessage.GetNextAssoc( pos, strTemp, (void*&)pWndBaseTemp );
		SAFE_DELETE( pWndBaseTemp );//	요 라인에서 뒤지니까 함 확인해보셈. -xuzhu-
	}
	
	m_mapMessage.RemoveAll();

	pos = m_mapInstantMsg.GetStartPosition();
	while( pos )
	{
		m_mapInstantMsg.GetNextAssoc( pos, strTemp, (void*&)pWndBaseTemp );
		SAFE_DELETE( pWndBaseTemp );
	}
	m_mapInstantMsg.RemoveAll();
	
	pos = m_mapMap.GetStartPosition();
	while( pos )
	{
		m_mapMap.GetNextAssoc( pos, strTemp, (void*&)pWndBaseTemp );
		SAFE_DELETE( pWndBaseTemp );
	}
	m_mapMap.RemoveAll();

	SAFE_DELETE( m_pQuestItemInfo );
	SAFE_DELETE( m_pWndTextBook );
	SAFE_DELETE( m_pWndTextScroll );
	SAFE_DELETE( m_pWndTextLetter );

	SAFE_DELETE(m_pWndRankGuild);
	SAFE_DELETE(m_pWndRankInfo);
	SAFE_DELETE(m_pWndRankWar);
	SAFE_DELETE(m_pWndGuildBank);
#if __VER >= 11 // __GUILD_BANK_LOG
	SAFE_DELETE(m_pWndGuildBankLog);
#endif //__GUILD_BANK_LOG
	SAFE_DELETE(m_pWndGuildConfirm);
	SAFE_DELETE(m_pWndGuildWarPeaceConfirm);
	SAFE_DELETE( m_pWndGuildWarRequest );
	SAFE_DELETE(m_pWndGuildCombatRanking);
	SAFE_DELETE(m_pWndGuildWarState);
	SAFE_DELETE(n_pWndGuildCombatResult);
	SAFE_DELETE(m_pWndGuildCombatBoard);
	SAFE_DELETE(m_pWndGuildCombatOffer);
	SAFE_DELETE(m_pWndGuildCombatSelection);
	SAFE_DELETE(m_pWndGuildCombatJoinSelection);
	SAFE_DELETE(m_pWndGuildCombatInfoMessageBox2);
	SAFE_DELETE(m_pWndGuildCombatInfoMessageBox);	
	SAFE_DELETE( m_pWndUpgradeBase );
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	SAFE_DELETE(m_pWndGuildCombat1to1Selection);
	SAFE_DELETE(m_pWndGuildCombat1to1Offer);
#endif //__GUILD_COMBAT_1TO1
#if __VER >= 8 //__Y_CHAT_SYSTEM_8
	SAFE_DELETE(m_pWndChatLog);
#endif //__Y_CHAT_SYSTEM_8

#if __VER >= 12 // __SECRET_ROOM
	SAFE_DELETE(m_pWndSecretRoomMsg);
	SAFE_DELETE(m_pWndSecretRoomSelection);
	SAFE_DELETE(m_pWndSecretRoomOffer);
	SAFE_DELETE(m_pWndSecretRoomOfferState);
	SAFE_DELETE(m_pWndSecretRoomChangeTaxRate);
	SAFE_DELETE(m_pWndSecretRoomCheckTaxRate);
	SAFE_DELETE(m_pWndSecretRoomBoard);
	SAFE_DELETE(m_pWndSecretRoomQuick);
#endif //__SECRET_ROOM

#if __VER >= 13 // __RAINBOW_RACE
	SAFE_DELETE(m_pWndRainbowRaceOffer);
	SAFE_DELETE(m_pWndRainbowRaceInfo);
	SAFE_DELETE(m_pWndRainbowRaceRule);
	SAFE_DELETE(m_pWndRainbowRaceRanking);
	SAFE_DELETE(m_pWndRainbowRacePrize);
	SAFE_DELETE(m_pWndRainbowRaceMiniGameButton);
	SAFE_DELETE(m_pWndRainbowRaceMiniGame);
	SAFE_DELETE(m_pWndRainbowRaceMiniGameEnd);
	SAFE_DELETE(m_pWndRRMiniGameKawiBawiBo);
	SAFE_DELETE(m_pWndRRMiniGameDice);
	SAFE_DELETE(m_pWndRRMiniGameArithmetic);
	SAFE_DELETE(m_pWndRRMiniGameStopWatch);
	SAFE_DELETE(m_pWndRRMiniGameTyping);
	SAFE_DELETE(m_pWndRRMiniGameCard);
	SAFE_DELETE(m_pWndRRMiniGameLadder);
#endif //__RAINBOW_RACE
#if __VER >= 13 // __CSC_VER13_2
	SAFE_DELETE(m_pWndCoupleMessage);
#endif //__CSC_VER13_2
#if __VER >= 13 // __EXT_ENCHANT
	SAFE_DELETE(m_pWndChangeAttribute);
#endif //__EXT_ENCHANT
	SAFE_DELETE( m_pWndPiercing );

	SAFE_DELETE(m_pWndFontEdit);

	SAFE_DELETE(m_pWndInvenRemoveItem);

	SAFE_DELETE(m_pWndReSkillWarning);
	
	SAFE_DELETE( m_pWndCommerialElem );
	SAFE_DELETE( m_pRemoveElem );

	SAFE_DELETE( m_pWndChangeName );
#ifdef __FUNNY_COIN
	SAFE_DELETE(m_pFunnyCoinConfirm);
#endif //__FUNNY_COIN
#if __VER >= 14 // __SMELT_SAFETY
	SAFE_DELETE(m_pWndSmeltSafety);
	SAFE_DELETE(m_pWndSmeltSafetyConfirm);
#endif //__SMELT_SAFETY
#if __VER >= 14 // __EQUIP_BIND
	SAFE_DELETE(m_pWndEquipBindConfirm);
#endif // __EQUIP_BIND
#if __VER >= 14 // __RESTATE_CONFIRM
	SAFE_DELETE(m_pWndRestateConfirm);
#endif // __RESTATE_CONFIRM
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	SAFE_DELETE( m_pWndQuestQuickInfo );
	SAFE_DELETE( m_pWndQuestDetail );
#endif // __IMPROVE_QUEST_INTERFACE
#if __VER >= 15 // __CAMPUS
	SAFE_DELETE( m_pWndCampusInvitationConfirm );
	SAFE_DELETE( m_pWndCampusSeveranceConfirm );
#endif // __CAMPUS
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	SAFE_DELETE( m_pWndSealChar );
	SAFE_DELETE( m_pWndSealCharSelect );
	SAFE_DELETE( m_pWndSealCharSend );
	SAFE_DELETE( m_pWndSealCharSet );
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

#if __VER >= 15 // __GUILD_HOUSE
	SAFE_DELETE( m_pWndGHMain );
	SAFE_DELETE( m_pWndUpkeep );
#endif

#ifdef __PROTECT_AWAKE
	SAFE_DELETE( m_pWndSelectAwakeCase );
#endif

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
	SAFE_DELETE( m_pWndChattingBlockingList );
#endif // __YS_CHATTING_BLOCKING_SYSTEM

#ifdef __GUILD_HOUSE_MIDDLE
	SAFE_DELETE( m_pWndGHBid );
#endif //__GUILD_HOUSE_MIDDLE

#ifdef	__COLOSSEUM
	SAFE_DELETE( m_pColosseumComplete );
	SAFE_DELETE( m_pColosseumEnter );
	SAFE_DELETE( m_pColosseumMassage );
	SAFE_DELETE( m_pColosseumRankGuild );
	SAFE_DELETE( m_pColosseumRankNomal );
	SAFE_DELETE( m_pColosseumRetry );
	SAFE_DELETE( m_pColosseumStart );
	SAFE_DELETE( m_pColosseumStartReady1 );
	SAFE_DELETE( m_pColosseumStartReady2 );
#endif	// __COLOSSEUM

#ifdef __NEW_ITEM_VARUNA
	SAFE_DELETE( m_pWndBarunaNPC );
	SAFE_DELETE( m_pWndBarunaExtractOffer );
#ifdef __BARUNA_PIERCING
	SAFE_DELETE( m_pWndBarunaPiercing );
#endif // __BARUNA_PIERCING
#endif // __NEW_ITEM_VARUNA

#ifdef __ITEMLINK
	SAFE_DELETE( m_pWndLinkedItem );
#endif //__ITEMLINK

#ifdef __TREASUREBOX
	SAFE_DELETE( m_pWndTreasureBox );
	SAFE_DELETE( m_pWndTreasureRecieve );
#endif//__TREASUREBOX

#ifdef __AEGON_PETMISTAKE
	SAFE_DELETE(m_pWndPetMistakeCustom);
#endif // __AEGON_PETMISTAKE

#ifdef __AEGON_MONSTER_HUNT
	SAFE_DELETE(m_pWndMonsterHunt);
#endif // __AEGON_MONSTER_HUNT

#ifdef __LEEAEG_SHOP_FINDER
	SAFE_DELETE(m_pWndShopFinder);
#endif //__LEEAEG_SHOP_FINDER

#ifdef __GIFTBOX_LIST_ITEMS
	SAFE_DELETE(m_pWndGiftbox);
#endif // __GIFTBOX_LIST_ITEMS

#ifdef __AEGON_RETURN_AWAKE
	SAFE_DELETE(m_pWndReturnAwakening);
#endif // __AEGON_RETURN_AWAKE

#ifdef __VENDING_NPC
	SAFE_DELETE(m_pWndVendorWarning);
#endif // __VENDING_NPC

#ifdef __WEAPON_RARITY
	SAFE_DELETE(m_pWndWeaponRarity);
#endif // __WEAPON_RARITY

#ifdef __SYS_CUSTOM_TITLE
	SAFE_DELETE(m_pCreateOwnTitle);
#endif // __SYS_CUSTOM_TITLE

#ifdef __SYS_PARTYFINDER
	SAFE_DELETE(m_pWndPartyFind);
#endif // __SYS_PARTYFINDER

#ifdef __ANARCHY_SYSTEM
	SAFE_DELETE(m_pWndAnarchy);
	SAFE_DELETE(m_pWndAnarchySkillConfirm);
#endif // __ANARCHY_SYSTEM

#ifdef __AEGON_TELEPORTER
	SAFE_DELETE(m_pWndTeleporter);
#endif

#ifdef __AEGON_LOOT_BOX
	SAFE_DELETE(m_pWndLootBoxMain = NULL);
#endif // __AEGON_LOOT_BOX

#ifdef __AEGON_VIEW_INV
	SAFE_DELETE(m_pWndViewInventoryMain);
	SAFE_DELETE(m_pWndViewInventoryCtrl);
	SAFE_DELETE(m_pWndViewBag);
#endif // __AEGON_VIEW_INV

#ifdef __SYS_PETFILTER
	SAFE_DELETE(m_pWndPetfilter);
#endif //__SYS_PETFILTER

#ifdef __GUILDFINDER
	SAFE_DELETE(m_pWndGuildFinder);
	SAFE_DELETE(m_pWndGuildFinderReq);
#endif //__GUILDFINDER

#ifdef __REMOVE_DURATION_ITEMS
	SAFE_DELETE(m_pWndRemoveCmm);
#endif	//__REMOVE_DURATION_ITEMS

#ifdef __SYS_MONSTER_CLASH
	SAFE_DELETE(pWndMonsterClashRegister);
	SAFE_DELETE(pWndMonsterClashRegisterMember);
#endif //__SYS_MONSTER_CLASH

#ifdef __SYS_SANDBOX
	SAFE_DELETE(m_pWndSandbox);
#endif //__SYS_SANDBOX

#ifdef __AEGON_CRAFTING_SYSTEM
	SAFE_DELETE(m_pWndCraftingMain);
#endif // __AEGON_CRAFTING_SYSTEM

#ifdef __WIKIPEDIA_MONSTER_DROPLIST
	SAFE_DELETE(m_pWndDropInfo);
#endif //__WIKIPEDIA_MONSTER_DROPLIST

#ifdef __AEGON_ADVANCED_ANNOUNCEMENT
	SAFE_DELETE(m_pWndAegonSystemShout);
#endif // __AEGON_ADVANCED_ANNOUNCEMENT

#ifdef __SWITCH_EQUIP
	SAFE_DELETE(m_pWndSwitchEquip);
#endif // __SWITCH_EQUIP

#ifdef __MODS_MANAGER
	SAFE_DELETE(m_pWndModsManager);
#endif // __MODS_MANAGER

#ifdef __PET_GLOW
	SAFE_DELETE(m_pWndPetGlowChange);
#endif // __PET_GLOW
#ifdef __WEAPON_MERGE
	SAFE_DELETE(m_pWndWeaponMerge);
	SAFE_DELETE(m_pWndWeaponMergeCancel);
#endif // __WEAPON_MERGE

#ifdef __LOOKCHANGE
	SAFE_DELETE(m_pWndLookChange);
#endif // __LOOKCHANGE

#ifdef __FASHION_COMBINE
	SAFE_DELETE(m_pWndFashionCombine);
#endif // __FASHION_COMBINE

#ifdef __AEGON_INGAME_DP
	SAFE_DELETE(m_pWndDPPurchaseConfirm);
#endif

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
	SAFE_DELETE(m_pWndGuildCombatJoinConfirm);
	SAFE_DELETE(m_pWndGuildCombatCancelConfirm);
	SAFE_DELETE(m_pWndSecretRoomCancelConfirm);
	SAFE_DELETE(m_pWndSummonParty);
	SAFE_DELETE(m_pWndSummonFriendConfirm);
	SAFE_DELETE(m_pWndSummonPartyConfirm);
	SAFE_DELETE(m_pWndSummonFriend);
	SAFE_DELETE(m_pWndSummonPartyUse);
	SAFE_DELETE(m_pWndGuildName);
	SAFE_DELETE(m_pWndGuildNickName);

	SAFE_DELETE(m_pWndTradeConfirm);
#endif

#ifdef __AEGON_TOURNAMENT
	SAFE_DELETE(m_pWndAegonTournamentScoreBoard);
	SAFE_DELETE(m_pWndAegonTournament);
#endif // __AEGON_TOURNAMENT

#ifdef __ROYAL_RUMBLE
	SAFE_DELETE(m_pJoinPlayerSiege);
	SAFE_DELETE(m_pSiegeRanking);
#endif // __ROYAL_RUMBLE

#ifdef __AEGON_FIXES
	SAFE_DELETE(m_pWndRepairItem);
#endif

#ifdef __AEGON_PVP_MINIGAMES
	SAFE_DELETE(m_pWndAegonPvPGameCreate);
	SAFE_DELETE(m_pWndAegonPvPGameInvitation);
	SAFE_DELETE(m_pWndAegonPvPGameMain);
	SAFE_DELETE(m_pWndAegonPvPGameMessageBox);
	SAFE_DELETE(m_pWndAegonPvPGameFinder);
#endif
}

void CWndMgr::DestroyApplet()
{
	CWndNeuz* pWndBase;
	DWORD dwIdApplet;
	POSITION pos = m_mapWndApplet.GetStartPosition();
	while( pos )
	{
		m_mapWndApplet.GetNextAssoc( pos, dwIdApplet, (void*&)pWndBase );
		// 윈도가 종료될 때 WndRect정보를 저장하지 않게 해야한다.
		// 여기서 파괴되는 것은 오픈되어 있는 것으로 간주해야되는데,
		// 내부에서 파괴되면 크로즈로 판단하기 때문에 재실행시 윈도가 열리지 않게 된다.
		//pWndBase->SetPutRegInfo( FALSE );
		// 이제 여기서 임의로 오픈되어 있다고 저장하자.
		//PutRegInfo( dwIdApplet, pWndBase->GetWindowRect( TRUE), TRUE );
		if( pWndBase->IsPutRegInfo() )
			PutRegInfo( (CWndNeuz*)pWndBase, TRUE );//dwIdApplet, pWndBase->GetWindowRect( TRUE), TRUE );
		// 이제 다 끝났다. 파괴하자.
		safe_delete( pWndBase );
	}
	// 파괴할 때 인터페이스 정보가 저장된다. 따라서 파괴 이후 save하기.
	if( m_bTitle == FALSE )
		SaveRegInfo( "regInfo.dat" );
	m_mapWndApplet.RemoveAll();
}

BOOL CWndMgr::Initialize(CWndBase* pParentWnd)
{
#ifdef __AEGON_THEME_SWITCHER
	CString strResdata = "ResData.inc";
	BOOL bDefault = TRUE;

	__AEGON_THEME_UNIT* pThemeUnit = CAegonThemeSwitcherMng::GetInstance()->GetThemeUnit(g_Option.m_nSelectedThemeID);
	if (pThemeUnit && pThemeUnit->nThemeID != 0)
	{
		strResdata = MakePath(pThemeUnit->szThemeFolder, "ResData.inc");
		bDefault = FALSE;
	}
	BOOL bSuccess = CWndBase::m_resMng.Load(strResdata.GetString());
	if(!bDefault && !bSuccess)
		CWndBase::m_resMng.Load("ResData.inc");
#else
	CWndBase::m_resMng.Load( "ResData.inc" );
#endif
	Create( WBS_NOFRAME | WBS_MANAGER | WBS_NODRAWFRAME, CRect( 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT/* - TASKBAR_HEIGHT*/ ), this, 100 );
	return TRUE;
}
void CWndMgr::OnInitialUpdate()
{
	m_timerDobe.Set( SEC( 2 ) );
#ifdef __BAN_CHATTING_SYSTEM
	m_timerWarning.Set( WARNING_MILLISECOND, TRUE );
	m_timerWarning2.Set( WARNING_2_MILLISECOND, TRUE );
	m_timerShortcutWarning.Set( SHORTCUT_WARNING_MILLISECOND, TRUE );
	m_timerBanning.Set( BANNING_MILLISECOND, TRUE );
#endif // __BAN_CHATTING_SYSTEM
	AddAllApplet();

	m_dwSavePlayerTime = 0;

	char filename[MAX_PATH];
	for( int i=0; i<128; i++ )
	{
		sprintf( filename, "Icon_CoolTime_%.3d.tga", i );
		g_pCoolTexArry[i] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice,  MakePath( DIR_ICON, filename ), COLOR_ITEM_BG );
	}

	for( int i=0; i<11; i++ )
	{
		sprintf( filename, "Icon_ImgIncAni_%.2d.tga", i );
		g_pEnchantTexArry[i] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice,  MakePath( DIR_ICON, filename ), 0xffffffff );
	}	
#ifdef __CHANGE_HOT_KEY
	LoadHotkey();
#endif // __CHANGE_HOT_KEY
}

void CWndMgr::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

}

void CWndMgr::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{

}

void CWndMgr::OnDestroyChildWnd( CWndBase* pWndChild )
{
	BOOL bFocus = TRUE;
	if( pWndChild && pWndChild->IsWndStyle( WBS_NOFOCUS ) )
		bFocus = FALSE;

#ifdef __MIKSIK_EQUALIZED_MODE
	if (m_WndSelectTmpClass == pWndChild)
	{
		SAFE_DELETE(m_WndSelectTmpClass);
		pWndChild = NULL;
	}
#endif //__MIKSIK_EQUALIZED_MODE

	if( m_pWndWorld == pWndChild )
	{
		SAFE_DELETE( m_pWndWorld );
		pWndChild = NULL;
	} else
	if( m_pWndBeautyShop == pWndChild )
	{
		SAFE_DELETE( m_pWndBeautyShop );
		pWndChild = NULL;
	}
#if __VER >= 8 //__CSC_VER8_4
	else if( m_pWndFaceShop == pWndChild )
	{
		SAFE_DELETE( m_pWndFaceShop );
//		m_pWndFaceShop = NULL;
		pWndChild = NULL;
	}
#ifdef __NEWYEARDAY_EVENT_COUPON
	else if( m_pWndUseCouponConfirm == pWndChild )
	{
		SAFE_DELETE(m_pWndUseCouponConfirm);
		pWndChild = NULL;
	}
#endif //__NEWYEARDAY_EVENT_COUPON
#endif //__CSC_VER8_4
#if __VER >= 8 //__CSC_VER8_5
	else if(m_pWndSummonAngel == pWndChild)
	{
		SAFE_DELETE( m_pWndSummonAngel );
//		m_pWndSummonAngel = NULL;
		pWndChild = NULL;
	}
#endif //__CSC_VER8_5
#ifdef __EVE_MINIGAME
	else if(m_pWndKawiBawiBoGame == pWndChild)
	{
		SAFE_DELETE( m_pWndKawiBawiBoGame );
		pWndChild = NULL;
	}
	else if(m_pWndKawiBawiBoGameConfirm == pWndChild)
	{
		SAFE_DELETE( m_pWndKawiBawiBoGameConfirm );
		pWndChild = NULL;
	}
	else if(m_pWndFindWordGame == pWndChild)
	{
		SAFE_DELETE( m_pWndFindWordGame );
		pWndChild = NULL;
	}
	else if(m_pWndDiceGame == pWndChild)
	{
		SAFE_DELETE( m_pWndDiceGame );
		pWndChild = NULL;
	}
	else if(m_pWndPuzzleGame == pWndChild)
	{
		SAFE_DELETE( m_pWndPuzzleGame );
		pWndChild = NULL;
	}	
#endif //__EVE_MINIGAME
#if __VER >= 9 // __CSC_VER9_1
	else if(m_pWndSmeltMixJewel == pWndChild)
	{
		SAFE_DELETE( m_pWndSmeltMixJewel );
		pWndChild = NULL;
	}
	else if(m_pWndSmeltJewel == pWndChild)
	{
		SAFE_DELETE( m_pWndSmeltJewel );
		pWndChild = NULL;
	}	
	else if(m_pWndExtraction == pWndChild)
	{
		SAFE_DELETE( m_pWndExtraction );
		pWndChild = NULL;
	}	
	else if(m_pWndChangeWeapon == pWndChild)
	{
		SAFE_DELETE( m_pWndChangeWeapon );
		pWndChild = NULL;
	}	
	else if(m_pWndPetStatus == pWndChild)
	{
		SAFE_DELETE( m_pWndPetStatus );
		pWndChild = NULL;
	}
//#endif
//	else if(m_pWndPetFood == pWndChild)
//	{
//		SAFE_DELETE( m_pWndPetFood );
//		pWndChild = NULL;
//	}
	else if(m_pWndFoodConfirm == pWndChild)
	{
		SAFE_DELETE( m_pWndFoodConfirm );
		pWndChild = NULL;
	}	
	else if(m_pWndPetMiracle == pWndChild)
	{
		SAFE_DELETE( m_pWndPetMiracle );
		pWndChild = NULL;
	}
	else if(m_pWndPetFoodMill == pWndChild)
	{
		SAFE_DELETE( m_pWndPetFoodMill );
		pWndChild = NULL;
	}
	else if(m_pWndPetLifeConfirm == pWndChild)
	{
		SAFE_DELETE( m_pWndPetLifeConfirm );
		pWndChild = NULL;
	}
#if __VER >= 10 //__CSC_VER9_1 -> __LEGEND
	else if(m_pWndHeroSkillUp == pWndChild)
	{
		SAFE_DELETE( m_pWndHeroSkillUp );
		pWndChild = NULL;
	}
#endif //__CSC_VER9_1 -> __LEGEND
#endif //__CSC_VER9_1
#if __VER >= 11 // __PIERCING_REMOVE
	else if(m_pWndRemovePiercing == pWndChild)
	{
		SAFE_DELETE( m_pWndRemovePiercing );
		pWndChild = NULL;
	}
#endif //__PIERCING_REMOVE
#if __VER >= 12 // __CSC_VER12_4
	else if(m_pWndRemoveJewel == pWndChild)
	{
		SAFE_DELETE( m_pWndRemoveJewel );
		pWndChild = NULL;
	}
#endif //__CSC_VER12_4
#if __VER >= 12 // __CSC_VER12_5
	else if(m_pWndPetTransEggs == pWndChild)
	{
		SAFE_DELETE( m_pWndPetTransEggs );
		pWndChild = NULL;
	}
#endif //__CSC_VER12_5
#ifdef __TRADESYS
	else if(m_pWndDialogEvent == pWndChild)
	{
		SAFE_DELETE( m_pWndDialogEvent );
		pWndChild = NULL;
	}
#endif //__TRADESYS
#if __VER >= 12 // __HEAVEN_TOWER
	else if( m_pWndHeavenTower == pWndChild )
	{
		SAFE_DELETE( m_pWndHeavenTower );
		pWndChild = NULL;
	}
#endif //__HEAVEN_TOWER
#if __VER >= 10 // __REMOVE_ATTRIBUTE
	else if(m_pWndRemoveAttribute == pWndChild)
	{
		SAFE_DELETE( m_pWndRemoveAttribute );
		pWndChild = NULL;
	}
#endif //__REMOVE_ATTRIBUTE
#if __VER >= 12 // __LORD
	else if(m_pWndLordState == pWndChild)
	{
		SAFE_DELETE(m_pWndLordState);
		pWndChild = NULL;
	}
	else if(m_pWndLordTender == pWndChild)
	{
		SAFE_DELETE(m_pWndLordTender);
		pWndChild = NULL;
	}
	else if(m_pWndLordVote == pWndChild)
	{
		SAFE_DELETE(m_pWndLordVote);
		pWndChild = NULL;
	}
	else if(m_pWndLordEvent == pWndChild)
	{
		SAFE_DELETE(m_pWndLordEvent);
		pWndChild = NULL;
	}
	else if(m_pWndLordInfo == pWndChild)
	{
		SAFE_DELETE(m_pWndLordInfo);
		pWndChild = NULL;
	}
	else if(m_pWndLordRPInfo == pWndChild)
	{
		SAFE_DELETE(m_pWndLordRPInfo);
		pWndChild = NULL;
	}
	else if(m_pWndLordSkillConfirm == pWndChild)
	{
		SAFE_DELETE(m_pWndLordSkillConfirm);
		pWndChild = NULL;
	}
#endif
#if __VER >= 13 // __HOUSING
	else if(m_pWndRoomList == pWndChild)
	{
		SAFE_DELETE(m_pWndRoomList);
		pWndChild = NULL;
	}
	else if(m_pWndQuitRoom == pWndChild)
	{
		SAFE_DELETE(m_pWndQuitRoom);
		pWndChild = NULL;
	}
#endif // __HOUSING
#ifdef __PET_1024
	else if(m_pWndChangePetName == pWndChild)
	{
		SAFE_DELETE(m_pWndChangePetName);
		pWndChild = NULL;
	}
#endif
#if __VER >= 12 // __PET_0519
	else if(m_pWndPetAwakCancel == pWndChild)
	{
		SAFE_DELETE(m_pWndPetAwakCancel);
		pWndChild = NULL;
	}
#endif
#ifdef __AZRIA_1023
	else if(m_pWndSelectCh == pWndChild)
	{
		SAFE_DELETE(m_pWndSelectCh);
		pWndChild = NULL;
	}
#endif
#ifdef __HELP_BUG_FIX
	else if(m_pWndHelpInstant == pWndChild)
	{
		SAFE_DELETE(m_pWndHelpInstant);
		pWndChild = NULL;
	}
#endif //__HELP_BUG_FIX
	else
	if( m_pWndPartyConfirm == pWndChild )
	{
		SAFE_DELETE( m_pWndPartyConfirm );
		pWndChild = NULL;
	} else
	if( m_pWndFriendConfirm == pWndChild )
	{
		SAFE_DELETE(m_pWndFriendConfirm);
		pWndChild = NULL;
	} else
	if( m_pWndConfirmTrade == pWndChild )
	{
		SAFE_DELETE(m_pWndConfirmTrade);
		pWndChild = NULL;
	} else
	if( m_pWndPost == pWndChild )
	{
		SAFE_DELETE( m_pWndPost );
		pWndChild = NULL;
	} else
	if( m_pWndQuestItemWarning == pWndChild )
	{
//		SAFE_DELETE(m_pWndQuestItemWarning);
		SAFE_DELETE(m_pWndQuestItemWarning);
		pWndChild = NULL;
	} else
	if( m_pWndPartyChangeName == pWndChild )
	{
		SAFE_DELETE( m_pWndPartyChangeName );
		pWndChild = NULL;
	} else
	if( m_pWndBank == pWndChild )
	{
		SAFE_DELETE( m_pWndBank );
		pWndChild = NULL;
		if( m_pWndTradeGold )
			m_pWndTradeGold->Destroy();
	} else
	if( m_pWndMessageNote == pWndChild )
	{
		SAFE_DELETE( m_pWndMessageNote );
		pWndChild = NULL;
	} else
	if( m_pWndMessengerNote == pWndChild )
	{
		SAFE_DELETE( m_pWndMessengerNote );
		pWndChild = NULL;
	} else
	if( m_pWndAddFriend == pWndChild )
	{
		SAFE_DELETE( m_pWndAddFriend );
		pWndChild = NULL;
	} else
	if( m_pWndStateConfirm == pWndChild )
	{
		SAFE_DELETE( m_pWndStateConfirm );
		pWndChild = NULL;
	} else
	if( m_pWndConfirmBank == pWndChild )
	{
		SAFE_DELETE( m_pWndConfirmBank );
		pWndChild = NULL;
	} else
	if( m_pWndPenaltyPK == pWndChild )
	{
		SAFE_DELETE( m_pWndPenaltyPK );
		pWndChild = NULL;
	} else
	if( m_pWndBankPassword == pWndChild )
	{
		SAFE_DELETE( m_pWndBankPassword );
		pWndChild = NULL;
	} else
	if( m_pWndMessageBox == pWndChild )
	{
		SAFE_DELETE( m_pWndMessageBox );
		pWndChild = NULL;
	} else
	if( m_pWndMessageBoxUpper == pWndChild )
	{
		SAFE_DELETE( m_pWndMessageBoxUpper );
		pWndChild = NULL;
	} else
	if( m_pWndRevival == pWndChild )
	{
		SAFE_DELETE( m_pWndRevival );
		pWndChild = NULL;
	} else
	if( m_pLogOutWaitting == pWndChild )
	{
		SAFE_DELETE( m_pLogOutWaitting );
		pWndChild = NULL;
	} else
	if( m_pWndResurrectionConfirm == pWndChild )
	{
		SAFE_DELETE( m_pWndResurrectionConfirm );
		pWndChild = NULL;
	} else
	if( m_pReWanted == pWndChild )
	{
		SAFE_DELETE( m_pReWanted );
		pWndChild = NULL;
	} else
	if( m_pWanted == pWndChild )
	{
		SAFE_DELETE( m_pWanted );
		pWndChild = NULL;
	} else
	if( m_pWndCommItemDlg == pWndChild )
	{
		SAFE_DELETE( m_pWndCommItemDlg );
		pWndChild = NULL;
	} else
	if( m_pWndChangeClass1 == pWndChild )
	{
		SAFE_DELETE( m_pWndChangeClass1 );
		pWndChild = NULL;
	}else
	if( m_pWndChangeClass2 == pWndChild )
	{
		SAFE_DELETE( m_pWndChangeClass2 );
		pWndChild = NULL;
	}else		
	if( m_pWndChangeName == pWndChild )
	{
		SAFE_DELETE( m_pWndChangeName );
		pWndChild = NULL;
	} else
	if( m_pWndDialog == pWndChild )
	{
		SAFE_DELETE( m_pWndDialog );
		pWndChild = NULL;
	} else
	if( m_pWndShop == pWndChild )
	{
		SAFE_DELETE( m_pWndShop );
		pWndChild = NULL;
	} else
	if( m_pWndTrade == pWndChild )
	{
		SAFE_DELETE( m_pWndTrade );
		pWndChild = NULL;
	} else
	if( m_pWndTradeGold == pWndChild )
	{
		SAFE_DELETE( m_pWndTradeGold );
		pWndChild = NULL;
	} else
	if( m_pWndDropItem == pWndChild )
	{
		SAFE_DELETE( m_pWndDropItem );
		pWndChild = NULL;
	} else
	if( m_pWndQuickList == pWndChild )
	{
		SAFE_DELETE( m_pWndQuickList );
		pWndChild = NULL;
	} else
	if( m_pWndCommand == pWndChild )
	{
		SAFE_DELETE( m_pWndCommand );
		pWndChild = NULL;
	} else
	if( m_pWndRankGuild == pWndChild )
	{
		SAFE_DELETE( m_pWndRankGuild );
		pWndChild = NULL;
	}else
	if( m_pWndRankInfo == pWndChild )
	{
		SAFE_DELETE( m_pWndRankInfo );
		pWndChild = NULL;
	}else
	if( m_pWndRankWar == pWndChild )
	{
		SAFE_DELETE( m_pWndRankWar );
		pWndChild = NULL;
	}else
	if( m_pWndGuildBank == pWndChild )
	{
		g_DPlay.SendCloseGuildBankWnd();
		SAFE_DELETE( m_pWndGuildBank );
		pWndChild = NULL;
		if( m_pWndTradeGold )
			m_pWndTradeGold->Destroy();
	}
#if __VER >= 11 // __GUILD_BANK_LOG
	else if( m_pWndGuildBankLog == pWndChild )
	{
		SAFE_DELETE(m_pWndGuildBankLog);
		pWndChild = NULL;
	}
#endif //__GUILD_BANK_LOG
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	else if( m_pWndSealChar == pWndChild )
	{
		SAFE_DELETE( m_pWndSealChar );
		pWndChild = NULL;
	}
	else if( m_pWndSealCharSelect == pWndChild )
	{
		SAFE_DELETE( m_pWndSealCharSelect );
		pWndChild = NULL;
	}
	else if( m_pWndSealCharSend == pWndChild )
	{
		SAFE_DELETE( m_pWndSealCharSend );
		pWndChild = NULL;
	}
	else if( m_pWndSealCharSet == pWndChild )
	{
		SAFE_DELETE( m_pWndSealCharSet );
		pWndChild = NULL;
	}
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	else if( m_pWndGuildConfirm == pWndChild )
	{
		SAFE_DELETE( m_pWndGuildConfirm );
		pWndChild = NULL;
	}else
	if( m_pWndGuildWarPeaceConfirm == pWndChild )
	{
		SAFE_DELETE( m_pWndGuildWarPeaceConfirm );
		pWndChild = NULL;
	} else
	if( m_pWndGuildWarRequest == pWndChild )
	{
		SAFE_DELETE( m_pWndGuildWarRequest );
		pWndChild = NULL;
	} else
	if( m_pWndDuelConfirm == pWndChild )
	{
		SAFE_DELETE( m_pWndDuelConfirm );
		pWndChild = NULL;
	}	
#if __VER >= 8     // 8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World
	else
	if( m_pWndDuelResult == pWndChild )
	{
		SAFE_DELETE( m_pWndDuelResult );
		pWndChild = NULL;
	}	
#endif	// __VER >= 8  
	else
	if( m_pWndRandomScrollConfirm == pWndChild )
	{
		SAFE_DELETE( m_pWndRandomScrollConfirm );
		pWndChild = NULL;
	}
	else
	if( m_pWndGuildCombatRanking == pWndChild )
	{
		SAFE_DELETE( m_pWndGuildCombatRanking );
		pWndChild = NULL;
	}
	else
	if( m_pWndGuildWarState == pWndChild )
	{
		SAFE_DELETE( m_pWndGuildWarState );
		pWndChild = NULL;
	}
	else	
	if( n_pWndGuildCombatResult == pWndChild )
	{
		SAFE_DELETE( n_pWndGuildCombatResult );
		pWndChild = NULL;
	}
	else
	if( m_pWndGuildCombatBoard == pWndChild )
	{
		SAFE_DELETE( m_pWndGuildCombatBoard );
		pWndChild = NULL;
	}
	else
	if( m_pWndGuildCombatInfoMessageBox2 == pWndChild )
	{
		SAFE_DELETE( m_pWndGuildCombatInfoMessageBox2 );
		pWndChild = NULL;
	}
	else
	if( m_pWndGuildCombatInfoMessageBox == pWndChild )
	{
		SAFE_DELETE( m_pWndGuildCombatInfoMessageBox );
		pWndChild = NULL;
	}
	else
	if( m_pWndGuildCombatOffer == pWndChild )
	{
		SAFE_DELETE( m_pWndGuildCombatOffer );
		pWndChild = NULL;
	}
	else
	if( m_pWndGuildCombatSelection == pWndChild )
	{
		SAFE_DELETE( m_pWndGuildCombatSelection );
		pWndChild = NULL;
	}
	else
	if( m_pWndGuildCombatJoinSelection == pWndChild )
	{
		SAFE_DELETE( m_pWndGuildCombatJoinSelection );
		pWndChild = NULL;
	}
	else		
	if( m_pWndUpgradeBase == pWndChild )
	{
		SAFE_DELETE( m_pWndUpgradeBase );
		pWndChild = NULL;
	}
	else		
	if( m_pWndPiercing == pWndChild )
	{
		SAFE_DELETE( m_pWndPiercing );
		pWndChild = NULL;
	}
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	else if( m_pWndGuildCombat1to1Selection == pWndChild )
	{
		SAFE_DELETE( m_pWndGuildCombat1to1Selection );
		pWndChild = NULL;
	}
	else if(m_pWndGuildCombat1to1Offer == pWndChild)
	{
		SAFE_DELETE( m_pWndGuildCombat1to1Offer );
		pWndChild = NULL;
	}
#endif //__GUILD_COMBAT_1TO1
#if __VER >= 8 //__Y_CHAT_SYSTEM_8
	else		
	if( m_pWndChatLog == pWndChild )
	{
		SAFE_DELETE( m_pWndChatLog );
		pWndChild = NULL;
	}
#endif //__Y_CHAT_SYSTEM_8
#if __VER >= 12 // __SECRET_ROOM
	else if( m_pWndSecretRoomMsg == pWndChild )
	{
		SAFE_DELETE( m_pWndSecretRoomMsg );
		pWndChild = NULL;
	}
	else if( m_pWndSecretRoomSelection == pWndChild )
	{
		SAFE_DELETE( m_pWndSecretRoomSelection );
		pWndChild = NULL;
	}
	else if(m_pWndSecretRoomOffer == pWndChild)
	{
		SAFE_DELETE( m_pWndSecretRoomOffer );
		pWndChild = NULL;
	}
	else if(m_pWndSecretRoomOfferState == pWndChild)
	{
		SAFE_DELETE( m_pWndSecretRoomOfferState );
		pWndChild = NULL;
	}
	else if(m_pWndSecretRoomChangeTaxRate == pWndChild)
	{
		SAFE_DELETE( m_pWndSecretRoomChangeTaxRate );
		pWndChild = NULL;
	}
	else if(m_pWndSecretRoomCheckTaxRate == pWndChild)
	{
		SAFE_DELETE( m_pWndSecretRoomCheckTaxRate );
		pWndChild = NULL;
	}
#endif //__SECRET_ROOM
	else
	if( m_pWndFontEdit == pWndChild )
	{
		SAFE_DELETE( m_pWndFontEdit );
		pWndChild = NULL;
	}
	else
	if( m_pWndInvenRemoveItem == pWndChild )
	{
		SAFE_DELETE( m_pWndInvenRemoveItem );
		pWndChild = NULL;
	}
	else	
	if( m_pWndReSkillWarning == pWndChild )
	{
		SAFE_DELETE( m_pWndReSkillWarning );
		pWndChild = NULL;
	}
	else
	if( m_pWndQueryEquip == pWndChild )
	{
		SAFE_DELETE( m_pWndQueryEquip );
		pWndChild = NULL;
	}
	else		
	if( m_pWndCommerialElem == pWndChild )
	{
		SAFE_DELETE( m_pWndCommerialElem );
		pWndChild = NULL;
	}
	else		
	if( m_pRemoveElem == pWndChild )
	{
		SAFE_DELETE( m_pRemoveElem );
		pWndChild = NULL;
	}
	else		
	if( m_pWndDropConfirm == pWndChild )
	{
		SAFE_DELETE( m_pWndDropConfirm );
		pWndChild = NULL;
	} 
	else
	if( m_pWndCollecting == pWndChild )
	{
		SAFE_DELETE( m_pWndCollecting );
		m_pWndCollecting = NULL;
	}
	else
	if( m_pWndSecretRoomBoard == pWndChild )
	{
		SAFE_DELETE( m_pWndSecretRoomBoard );
		m_pWndSecretRoomBoard = NULL;
	}
	else
	if( m_pWndSecretRoomQuick == pWndChild )
	{
		SAFE_DELETE( m_pWndSecretRoomQuick );
		m_pWndSecretRoomQuick = NULL;
	}
#if __VER >= 11 // __SYS_IDENTIFY
	else
	if( m_pWndLvReqDown == pWndChild )
	{
		SAFE_DELETE(m_pWndLvReqDown);
	}
	else
	if( m_pWndAwakening == pWndChild )
	{
		SAFE_DELETE(m_pWndAwakening);
	}
	else
	if( m_pWndBlessingCancel == pWndChild )
	{
		SAFE_DELETE(m_pWndBlessingCancel);
	}
#endif
#ifdef __JEFF_11
	else
	if( m_pPetRes == pWndChild )
	{
		SAFE_DELETE(m_pPetRes);
	}
#endif

#if __VER >= 13 // __RAINBOW_RACE
	if(m_pWndRainbowRaceOffer == pWndChild)
	{
		SAFE_DELETE( m_pWndRainbowRaceOffer );
		pWndChild = NULL;
	}
	else if(m_pWndRainbowRaceInfo == pWndChild)
	{
		SAFE_DELETE( m_pWndRainbowRaceInfo );
		pWndChild = NULL;
	}
	else if(m_pWndRainbowRaceRule == pWndChild)
	{
		SAFE_DELETE( m_pWndRainbowRaceRule );
		pWndChild = NULL;
	}
	else if(m_pWndRainbowRaceRanking == pWndChild)
	{
		SAFE_DELETE( m_pWndRainbowRaceRanking );
		pWndChild = NULL;
	}
	else if(m_pWndRainbowRacePrize == pWndChild)
	{
		SAFE_DELETE( m_pWndRainbowRacePrize );
		pWndChild = NULL;
	}
	else if(m_pWndRainbowRaceMiniGameButton == pWndChild)
	{
		SAFE_DELETE(m_pWndRainbowRaceMiniGameButton);
		pWndChild = NULL;
	}
	else if(m_pWndRainbowRaceMiniGame == pWndChild)
	{
		SAFE_DELETE( m_pWndRainbowRaceMiniGame );
		pWndChild = NULL;
	}
	else if(m_pWndRainbowRaceMiniGameEnd == pWndChild)
	{
		SAFE_DELETE( m_pWndRainbowRaceMiniGameEnd );
		pWndChild = NULL;
	}
	else if(m_pWndRRMiniGameKawiBawiBo == pWndChild)
	{
		SAFE_DELETE( m_pWndRRMiniGameKawiBawiBo );
		pWndChild = NULL;
	}
	else if(m_pWndRRMiniGameDice == pWndChild)
	{
		SAFE_DELETE( m_pWndRRMiniGameDice );
		pWndChild = NULL;
	}
	else if(m_pWndRRMiniGameArithmetic == pWndChild)
	{
		SAFE_DELETE( m_pWndRRMiniGameArithmetic );
		pWndChild = NULL;
	}
	else if(m_pWndRRMiniGameStopWatch == pWndChild)
	{
		SAFE_DELETE( m_pWndRRMiniGameStopWatch );
		pWndChild = NULL;
	}
	else if(m_pWndRRMiniGameTyping == pWndChild)
	{
		SAFE_DELETE( m_pWndRRMiniGameTyping );
		pWndChild = NULL;
	}
	else if(m_pWndRRMiniGameCard == pWndChild)
	{
		SAFE_DELETE( m_pWndRRMiniGameCard );
		pWndChild = NULL;
	}
	else if(m_pWndRRMiniGameLadder == pWndChild)
	{
		SAFE_DELETE( m_pWndRRMiniGameLadder );
		pWndChild = NULL;
	}
#endif //__RAINBOW_RACE
#if __VER >= 13 // __CSC_VER13_2
	else if(m_pWndCoupleMessage == pWndChild)
	{
		SAFE_DELETE( m_pWndCoupleMessage );
		pWndChild = NULL;
	}
#endif //__CSC_VER13_2
#if __VER >= 13 // __EXT_ENCHANT
	else if(m_pWndChangeAttribute == pWndChild)
	{
		SAFE_DELETE( m_pWndChangeAttribute );
		pWndChild = NULL;
	}
#endif //__EXT_ENCHANT
#ifdef __FUNNY_COIN
	else if(m_pFunnyCoinConfirm == pWndChild)
	{
		SAFE_DELETE(m_pFunnyCoinConfirm);
		pWndChild = NULL;
	}
#endif //__FUNNY_COIN
#if __VER >= 14 // __SMELT_SAFETY
	else if(m_pWndSmeltSafety == pWndChild)
	{
		SAFE_DELETE(m_pWndSmeltSafety);
		pWndChild = NULL;
	}
	else if(m_pWndSmeltSafetyConfirm == pWndChild)
	{
		SAFE_DELETE(m_pWndSmeltSafetyConfirm);
		pWndChild = NULL;
	}
#endif //__SMELT_SAFETY
#if __VER >= 14 // __EQUIP_BIND
	else if(m_pWndEquipBindConfirm == pWndChild)
	{
		SAFE_DELETE(m_pWndEquipBindConfirm);
		pWndChild = NULL;
	}
#endif // __EQUIP_BIND
#if __VER >= 14 // __RESTATE_CONFIRM
	else if(m_pWndRestateConfirm == pWndChild)
	{
		SAFE_DELETE(m_pWndRestateConfirm);
		pWndChild = NULL;
	}
#if __VER >= 15 // __PETVIS
	else if( m_pWndBuffPetStatus == pWndChild )
	{
		SAFE_DELETE( m_pWndBuffPetStatus );
		pWndChild = NULL;
	}
	else if( m_pWndConfirmVis == pWndChild )
	{
		SAFE_DELETE( m_pWndConfirmVis );
		pWndChild = NULL;
	}
#endif //#ifdef __PETVIS
#endif // __RESTATE_CONFIRM
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	else if( m_pWndQuestQuickInfo == pWndChild )
	{
		SAFE_DELETE( m_pWndQuestQuickInfo );
		pWndChild = NULL;
	}
	else if( m_pWndQuestDetail == pWndChild )
	{
		SAFE_DELETE( m_pWndQuestDetail );
		pWndChild = NULL;
	}
#endif // __IMPROVE_QUEST_INTERFACE
#if __VER >= 15 // __CAMPUS
	else if( m_pWndCampusInvitationConfirm == pWndChild )
	{
		SAFE_DELETE( m_pWndCampusInvitationConfirm );
		pWndChild = NULL;
	}
	else if( m_pWndCampusSeveranceConfirm == pWndChild )
	{
		SAFE_DELETE( m_pWndCampusSeveranceConfirm );
		pWndChild = NULL;
	}
#endif // __CAMPUS
#if __VER >= 15 // __GUILD_HOUSE
	else if( m_pWndUpkeep == pWndChild )
	{
		SAFE_DELETE( m_pWndUpkeep );
		pWndChild = NULL;
	}
#endif //__GUILD_HOUSE
#ifdef __YS_CHATTING_BLOCKING_SYSTEM
	else if( m_pWndChattingBlockingList == pWndChild )
	{
		SAFE_DELETE( m_pWndChattingBlockingList );
		pWndChild = NULL;
	}
#endif // __YS_CHATTING_BLOCKING_SYSTEM

#ifdef __GUILD_HOUSE_MIDDLE
	else if( m_pWndGHBid == pWndChild )
	{
		SAFE_DELETE( m_pWndGHBid );
		pWndChild = NULL;
	}
#endif //__GUILD_HOUSE_MIDDLE

#ifdef __ITEMLINK
	if( m_pWndLinkedItem == pWndChild )
	{
		SAFE_DELETE( m_pWndLinkedItem );
		pWndChild = NULL;
	}
#endif //__ITEMLINK

#ifdef	__COLOSSEUM
	else if(m_pColosseumComplete == pWndChild)
	{
		SAFE_DELETE( m_pColosseumComplete );
		pWndChild = NULL;
	}
	else if(m_pColosseumEnter == pWndChild)
	{
		SAFE_DELETE( m_pColosseumEnter );
		pWndChild = NULL;
	}
	else if(m_pColosseumMassage == pWndChild)
	{
		SAFE_DELETE( m_pColosseumMassage );
		pWndChild = NULL;
	}
	else if(m_pColosseumRankGuild == pWndChild)
	{
		SAFE_DELETE( m_pColosseumRankGuild );
		pWndChild = NULL;
	}
	else if(m_pColosseumRankNomal == pWndChild)
	{
		SAFE_DELETE( m_pColosseumRankNomal );
		pWndChild = NULL;
	}
	else if(m_pColosseumRetry == pWndChild)
	{
		SAFE_DELETE( m_pColosseumRetry );
		pWndChild = NULL;
	}
	else if(m_pColosseumStart == pWndChild)
	{
		SAFE_DELETE( m_pColosseumStart );
		pWndChild = NULL;
	}
	else if(m_pColosseumStartReady1 == pWndChild)
	{
		SAFE_DELETE( m_pColosseumStartReady1 );
		pWndChild = NULL;
	}
	else if(m_pColosseumStartReady2 == pWndChild)
	{
		SAFE_DELETE( m_pColosseumStartReady2 );
		pWndChild = NULL;
	}
#endif // __COLOSSEUM

#ifdef __NEW_ITEM_VARUNA
	else if( m_pWndBarunaNPC == pWndChild )
	{
		SAFE_DELETE( m_pWndBarunaNPC );
		pWndChild = NULL;
	}
	else if( m_pWndBarunaEnchant == pWndChild )
	{
		SAFE_DELETE( m_pWndBarunaEnchant );
		pWndChild = NULL;
	}
	else if( m_pWndBarunaExtractOffer == pWndChild )
	{
		SAFE_DELETE( m_pWndBarunaExtractOffer );
		pWndChild = NULL;
	}
	else if( m_pWndBarunaOffer == pWndChild )
	{
		SAFE_DELETE( m_pWndBarunaOffer );
		pWndChild = NULL;
	}
	else if( m_pWndBarunaSeed == pWndChild )
	{
		SAFE_DELETE( m_pWndBarunaSeed )
		pWndChild = NULL;
	}
	else if( m_pWndBarunaOfferSeed == pWndChild )
	{
		SAFE_DELETE( m_pWndBarunaOfferSeed )
		pWndChild = NULL;
	}
#ifdef __BARUNA_PIERCING
	else if(m_pWndBarunaPiercing == pWndChild)
	{
		SAFE_DELETE( m_pWndBarunaPiercing );
		pWndChild = NULL;
	}
#endif // __BARUNA_PIERCING
#endif // __NEW_ITEM_VARUNA

#ifdef __FASHION_COMBINE
	else if(m_pWndFashionCombine == pWndChild)
	{
		SAFE_DELETE( m_pWndFashionCombine );
		pWndChild = NULL;
	}
#endif // __FASHION_COMBINE

#ifdef __GIFTBOX_LIST_ITEMS
	else if (m_pWndGiftbox == pWndChild)
	{
		SAFE_DELETE(m_pWndGiftbox);
		pWndChild = NULL;
	}
#endif // __GIFTBOX_LIST_ITEMS

#ifdef __VENDING_NPC
	else if (m_pWndVendorWarning == pWndChild)
	{
		SAFE_DELETE(m_pWndVendorWarning);
		pWndChild = NULL;
	}
#endif // __VENDING_NPC

#ifdef __WEAPON_RARITY
	else if (m_pWndWeaponRarity == pWndChild)
	{
		SAFE_DELETE(m_pWndWeaponRarity);
		pWndChild = NULL;
	}
#endif // __WEAPON_RARITY

#ifdef __SYS_CUSTOM_TITLE
	else if (m_pCreateOwnTitle == pWndChild)
	{
		SAFE_DELETE(m_pCreateOwnTitle);
		pWndChild = NULL;
	}
#endif // __SYS_CUSTOM_TITLE

#ifdef __SYS_PARTYFINDER
	else if (m_pWndPartyFind == pWndChild)
	{
		SAFE_DELETE(m_pWndPartyFind);
		pWndChild = NULL;
	}
#endif // __SYS_PARTYFINDER

#ifdef __ANARCHY_SYSTEM
	else if (m_pWndAnarchy == pWndChild)
	{
		SAFE_DELETE(m_pWndAnarchy);
		pWndChild = NULL;
	}
	else if (m_pWndAnarchySkillConfirm == pWndChild)
	{
		SAFE_DELETE(m_pWndAnarchySkillConfirm);
		pWndChild = NULL;
	}
#endif // __ANARCHY_SYSTEM

#ifdef __AEGON_TELEPORTER
	else if (m_pWndTeleporter == pWndChild)
	{
		SAFE_DELETE(m_pWndTeleporter);
		pWndChild = NULL;
	}
#endif

#ifdef __SYS_PETFILTER
	else if (m_pWndPetfilter == pWndChild)
	{
		SAFE_DELETE(m_pWndPetfilter);
		pWndChild = NULL;
	}
#endif //__SYS_PETFILTER

#ifdef __GUILDFINDER
	else if (m_pWndGuildFinder == pWndChild)
	{
		SAFE_DELETE(m_pWndGuildFinder);
		pWndChild = NULL;
	}
	else if (m_pWndGuildFinderReq == pWndChild)
	{
		SAFE_DELETE(m_pWndGuildFinderReq);
		pWndChild = NULL;
	}
#endif //__GUILDFINDER

#ifdef	__REMOVE_DURATION_ITEMS
	else if (m_pWndRemoveCmm == pWndChild)
	{
		SAFE_DELETE(m_pWndRemoveCmm);
		pWndChild = NULL;
	}
#endif	//__REMOVE_DURATION_ITEMS

#ifdef __SYS_MONSTER_CLASH
	else if (pWndMonsterClashRegisterMember == pWndChild)
	{
		SAFE_DELETE(pWndMonsterClashRegisterMember);
		pWndChild = nullptr;
	}
	else if (pWndMonsterClashRegister == pWndChild)
	{
		SAFE_DELETE(pWndMonsterClashRegister);
		pWndChild = nullptr;
	}
#endif //__SYS_MONSTER_CLASH

#ifdef __SYS_SANDBOX
	else if (m_pWndSandbox == pWndChild)
	{
		SAFE_DELETE(m_pWndSandbox);
		pWndChild = nullptr;
	}
#endif //__SYS_SANDBOX

#ifdef __TREASUREBOX
	if( m_pWndTreasureBox == pWndChild )
	{
		SAFE_DELETE( m_pWndTreasureBox );
		pWndChild = NULL;
	}
	if( m_pWndTreasureRecieve == pWndChild )
	{
		SAFE_DELETE( m_pWndTreasureRecieve );
		pWndChild = NULL;
	}
#endif//__TREASUREBOX

#ifdef __AEGON_CRAFTING_SYSTEM
	if (m_pWndCraftingMain == pWndChild)
	{
		SAFE_DELETE(m_pWndCraftingMain);
		pWndChild = NULL;
	}
#endif // __AEGON_CRAFTING_SYSTEM

#ifdef __AEGON_ADVANCED_ANNOUNCEMENT
	else if (m_pWndAegonSystemShout == pWndChild)
	{
		SAFE_DELETE(m_pWndAegonSystemShout);
		pWndChild = NULL;
	}
#endif // __AEGON_ADVANCED_ANNOUNCEMENT

#ifdef __SWITCH_EQUIP
	else if (m_pWndSwitchEquip == pWndChild)
	{
		SAFE_DELETE(m_pWndSwitchEquip);
		pWndChild = NULL;
	}
#endif // __SWITCH_EQUIP

#ifdef __MODS_MANAGER
	else if (m_pWndModsManager == pWndChild)
	{
		SAFE_DELETE(m_pWndModsManager);
		pWndChild = NULL;
	}
#endif // __MODS_MANAGER

#ifdef __PET_GLOW
	else if (m_pWndPetGlowChange == pWndChild)
	{
		SAFE_DELETE(m_pWndPetGlowChange);
		pWndChild = NULL;
	}
#endif // __PET_GLOW

#ifdef __WEAPON_MERGE
	else if (m_pWndWeaponMerge == pWndChild)
	{
		SAFE_DELETE(m_pWndWeaponMerge);
		pWndChild = NULL;
	}
	else if (m_pWndWeaponMergeCancel == pWndChild)
	{
		SAFE_DELETE(m_pWndWeaponMergeCancel);
		pWndChild = NULL;
	}
#endif // __WEAPON_MERGE

#ifdef __LOOKCHANGE
	else if (m_pWndLookChange == pWndChild)
	{
		SAFE_DELETE(m_pWndLookChange);
		pWndChild = NULL;
	}
#endif // __LOOKCHANGE

#ifdef __FASHION_COMBINE
	else if (m_pWndFashionCombine == pWndChild)
	{
		SAFE_DELETE(m_pWndFashionCombine);
		pWndChild = NULL;
	}
#endif // __FASHION_COMBINE

#ifdef __AEGON_INGAME_DP
	else if (m_pWndDPPurchaseConfirm == pWndChild)
	{
		SAFE_DELETE(m_pWndDPPurchaseConfirm);
		pWndChild = NULL;
	}
#endif
#ifdef __AEGON_PETMISTAKE
	else if (m_pWndPetMistakeCustom == pWndChild)
	{
		SAFE_DELETE(m_pWndPetMistakeCustom);
		pWndChild = NULL;
	}
#endif // __AEGON_PETMISTAKE

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
	else if (m_pWndGuildCombatJoinConfirm == pWndChild)
	{
		SAFE_DELETE(m_pWndGuildCombatJoinConfirm);
		pWndChild = NULL;
	}
	else if (m_pWndGuildCombatCancelConfirm == pWndChild)
	{
		SAFE_DELETE(m_pWndGuildCombatCancelConfirm);
		pWndChild = NULL;
	}
	else if (m_pWndSecretRoomCancelConfirm == pWndChild)
	{
		SAFE_DELETE(m_pWndSecretRoomCancelConfirm);
		pWndChild = NULL;
	}
	else if (m_pWndSummonParty == pWndChild)
	{
		SAFE_DELETE(m_pWndSummonParty);
		pWndChild = NULL;
	}
	else if (m_pWndSummonFriendConfirm == pWndChild)
	{
		SAFE_DELETE(m_pWndSummonFriendConfirm);
		pWndChild = NULL;
	}
	else if (m_pWndSummonPartyConfirm == pWndChild)
	{
		SAFE_DELETE(m_pWndSummonPartyConfirm);
		pWndChild = NULL;
	}
	else if (m_pWndSummonFriend == pWndChild)
	{
		SAFE_DELETE(m_pWndSummonFriend);
		pWndChild = NULL;
	}
	else if (m_pWndSummonPartyUse == pWndChild)
	{
		SAFE_DELETE(m_pWndSummonPartyUse);
		pWndChild = NULL;
	}
	else if (m_pWndGuildName == pWndChild)
	{
		SAFE_DELETE(m_pWndGuildName);
		pWndChild = NULL;
	}
	else if (m_pWndGuildNickName == pWndChild)
	{
		SAFE_DELETE(m_pWndGuildNickName);
		pWndChild = NULL;
	}
	else if (m_pWndTradeConfirm == pWndChild)
	{
	SAFE_DELETE(m_pWndTradeConfirm);
	pWndChild = NULL;
	}
	
#endif
#ifdef __AEGON_TOURNAMENT
	else if (m_pWndAegonTournamentScoreBoard == pWndChild)
	{
	SAFE_DELETE(m_pWndAegonTournamentScoreBoard);
	pWndChild = NULL;
	}
	else if (m_pWndAegonTournament == pWndChild)
	{
	SAFE_DELETE(m_pWndAegonTournament);
	pWndChild = NULL;
	}
#endif

#ifdef __ROYAL_RUMBLE
	else if (m_pJoinPlayerSiege == pWndChild)
	{
		SAFE_DELETE(m_pJoinPlayerSiege);
		pWndChild = NULL;
	}
	else if (m_pSiegeRanking == pWndChild)
	{
		SAFE_DELETE(m_pSiegeRanking);
		pWndChild = NULL;
	}
#endif // __ROYAL_RUMBLE

#ifdef __AEGON_RETURN_AWAKE
	else if (m_pWndReturnAwakening == pWndChild)
	{
		SAFE_DELETE(m_pWndReturnAwakening);
		pWndChild = NULL;
	}
#endif // __AEGON_RETURN_AWAKE

#ifdef __LEEAEG_SHOP_FINDER
	else if (m_pWndShopFinder == pWndChild)
	{
		SAFE_DELETE(m_pWndShopFinder);
		pWndChild = NULL;
	}
#endif //__LEEAEG_SHOP_FINDER
#ifdef __AEGON_MONSTER_HUNT
	else if (m_pWndMonsterHunt == pWndChild)
	{
		SAFE_DELETE(m_pWndMonsterHunt);
		pWndChild = NULL;
	}
#endif // __AEGON_MONSTER_HUNT

#ifdef __AEGON_FIXES
	else if (m_pWndActiveDesktop == pWndChild)
	{
		SAFE_DELETE(m_pWndActiveDesktop);
		pWndChild = NULL;
	}
	else if (m_pWndSelectVillage == pWndChild)
	{
		SAFE_DELETE(m_pWndSelectVillage);
		pWndChild = NULL;
	}
	else if (m_pWndRepairItem == pWndChild)
	{
		SAFE_DELETE(m_pWndRepairItem);
		pWndChild = NULL;
	}
#endif

#ifdef __AEGON_VIEW_INV
	else if (m_pWndViewInventoryMain == pWndChild)
	{
		SAFE_DELETE(m_pWndViewInventoryMain);
		pWndChild = NULL;
	}
	else if (m_pWndViewInventoryCtrl == pWndChild)
	{
		SAFE_DELETE(m_pWndViewInventoryCtrl);
		pWndChild = NULL;
	}
	else if (m_pWndViewBag == pWndChild)
	{
		SAFE_DELETE(m_pWndViewBag);
		pWndChild = NULL;
	}
#endif // __AEGON_VIEW_INV
#ifdef __AEGON_PVP_MINIGAMES
	else if (m_pWndAegonPvPGameCreate == pWndChild)
	{
		SAFE_DELETE(m_pWndAegonPvPGameCreate);
		pWndChild = NULL;
	}
	else if (m_pWndAegonPvPGameInvitation == pWndChild)
	{
		SAFE_DELETE(m_pWndAegonPvPGameInvitation);
		pWndChild = NULL;
	}
	else if (m_pWndAegonPvPGameMain == pWndChild)
	{
		SAFE_DELETE(m_pWndAegonPvPGameMain);
		pWndChild = NULL;
	}
	else if (m_pWndAegonPvPGameMessageBox == pWndChild)
	{
		SAFE_DELETE(m_pWndAegonPvPGameMessageBox);
		pWndChild = NULL;
	}
	else if (m_pWndAegonPvPGameFinder == pWndChild)
	{
		SAFE_DELETE(m_pWndAegonPvPGameFinder);
		pWndChild = NULL;
	}
#endif

//#endif
	//else
	if( pWndChild )
	{
		CWndNeuz* pWndBase;
#ifndef __IMPROVE_MAP_SYSTEM
		if( pWndChild->GetWndId() == APP_MAP_EX )
		{
			CWndMap* pWndMap = (CWndMap*) pWndChild;
			if( m_mapMap.Lookup( pWndMap->m_szMapFile, (void*&) pWndMap ) == TRUE )
			{
				m_mapMap.RemoveKey( pWndMap->m_szMapFile );
				SAFE_DELETE( pWndMap );
				//return;
			}
		}
		else
#endif // __IMPROVE_MAP_SYSTEM
		if( pWndChild->GetWndId() == APP_PARTY_CONFIRM )
		{
			SAFE_DELETE( pWndChild );
			//return;
		}
		else
		if( m_mapWndApplet.Lookup( pWndChild->GetWndId(), (void*&) pWndBase ) == TRUE )
		{
			m_mapWndApplet.RemoveKey( pWndChild->GetWndId() );
			// 애플렛으로 등록된 윈도만 파괴할 때 윈도의 정보를 저장한다. 
			if( pWndBase->IsPutRegInfo() )
				PutRegInfo( pWndBase, FALSE );
			SAFE_DELETE( pWndBase );
			pWndChild = NULL;
			//return;
		}
		else
		if( pWndChild->GetWndId() == APP_MESSAGE )
		{
			CWndMessage* pWndMessage = (CWndMessage*) pWndChild;
			if( m_mapMessage.Lookup( pWndMessage->m_strPlayer, (void*&) pWndBase ) == TRUE )
			{
				if( pWndBase != pWndMessage )
					ADDERRORMSG( "APP_MESSAGE_Vampyre" );
				m_mapMessage.RemoveKey( pWndMessage->m_strPlayer );
				SAFE_DELETE( pWndBase );
				pWndMessage = NULL;
				//SAFE_DELETE( pWndMessage );
				//return;
			}
		}
		else
		if( pWndChild->GetWndId() == APP_INSTANTMSG )
		{
			CWndInstantMsg* pWndMessage = (CWndInstantMsg*) pWndChild;
			if( m_mapInstantMsg.Lookup( pWndMessage->m_strPlayer, (void*&) pWndBase ) == TRUE )
			{
				if( pWndBase != pWndMessage )
					ADDERRORMSG( "APP_INSTANTMSG_Vampyre" );
				m_mapInstantMsg.RemoveKey( pWndMessage->m_strPlayer );
				SAFE_DELETE( pWndBase );
				pWndMessage = NULL;
				//SAFE_DELETE( pWndMessage );
				//return;
			}
		}
	}
	if( m_bTitle == FALSE && bFocus )
		SetFocus();

	SAFE_DELETE( m_pQuestItemInfo );
	SAFE_DELETE( m_pWndTextBook );
	SAFE_DELETE( m_pWndTextScroll );
	SAFE_DELETE( m_pWndTextLetter );
}	
void CWndMgr::OpenTextBook( CWndBase* pWndParent , CItemBase* pItemBase )
{
	SAFE_DELETE( m_pWndTextBook );
	m_pWndTextBook = new CWndTextBook;
	m_pWndTextBook->Initialize( pWndParent, pItemBase );
}
void CWndMgr::OpenTextScroll( CWndBase* pWndParent, CItemBase* pItemBase )
{
	SAFE_DELETE( m_pWndTextScroll );
	m_pWndTextScroll = new CWndTextScroll;
	m_pWndTextScroll->Initialize( pWndParent, pItemBase );
}
void CWndMgr::OpenTextLetter( CWndBase* pWndParent, CItemBase* pItemBase )
{
	SAFE_DELETE( m_pWndTextLetter );
	m_pWndTextLetter = new CWndTextLetter;
	m_pWndTextLetter->Initialize( pWndParent, pItemBase );
}
void CWndMgr::ChangeTextBook( CItemBase* pItemBase )
{
	m_pWndTextBook->SetItemBase( pItemBase );
}
void CWndMgr::ChangeTextScroll( CItemBase* pItemBase )
{
	m_pWndTextScroll->SetItemBase( pItemBase );
}
void CWndMgr::ChangeTextLetter( CItemBase* pItemBase )
{
	m_pWndTextLetter->SetItemBase( pItemBase );
}
void CWndMgr::OpenQuestItemInfo(CWndBase* pWndParent, CItemBase *pItemBase)
{
	SAFE_DELETE(m_pQuestItemInfo);
	m_pQuestItemInfo = new CWndQuestItemInfo;
	m_pQuestItemInfo->Initialize(pWndParent, pItemBase);
}

void CWndMgr::ChangeQuestItemInfo(CItemBase * pItemBase)
{
	m_pQuestItemInfo->SetItemBase(pItemBase);
}

void CWndMgr::OpenTitle( BOOL bFirstTime )
{
	if( m_bTitle == TRUE )
		return;

	// 처음 실행하는게 아니라면 게임 중 로그아웃한 것이다. 그렇다면 현재 열려 있는 필드를 삭제한다.
	if( bFirstTime == FALSE )
	{
#ifdef __CHANGE_HOT_KEY
		SaveHotkey();
#endif // __CHANGE_HOT_KEY
		g_Option.Save( "neuz.ini" );
		DestroyApplet();
		Free();
		g_WorldMng.DestroyCurrentWorld();
		g_pPlayer = NULL;
		CMover::SetActiveObj( NULL );
		g_dpCertified.DeleteDPObject();
#ifdef __CERTIFIER_COLLECTING_SYSTEM
		DPCollectClient->DeleteDPObject();
#endif // __CERTIFIER_COLLECTING_SYSTEM
		g_dpLoginClient.DeleteDPObject();
		g_DPlay.DeleteDPObject();
		
		if( g_pBipedMesh )
		{
			g_pBipedMesh->DeleteDeviceObjects();
			SAFE_DELETE( g_pBipedMesh );
		}
		g_Party.InitParty();
		g_GuildWarMng.Clear();
		g_GuildMng.Clear();
		g_GuildCombatMng.GuildCombatClear( 99 );
		CPost::GetInstance()->Clear();
#if __VER >= 13 // __COUPLE_1117
		CCoupleHelper::Instance()->Clear();
#endif	// __COUPLE_1117
#if __VER >= 13 // __RAINBOW_RACE
		CRainbowRace::GetInstance()->m_dwRemainTime = 0;
#endif //__RAINBOW_RACE
#if __VER >= 11 // __SYS_PLAYER_DATA
		CPlayerDataCenter::GetInstance()->Clear();
#endif // __SYS_PLAYER_DATA
#if __VER >= 15 // __GUILD_HOUSE
		GuildHouse->Clear();
#endif // __GUILD_HOUSE
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		if( m_pWndQuestQuickInfo )
			SAFE_DELETE( m_pWndQuestQuickInfo );
		g_QuestTreeInfoManager.DeleteAllTreeInformation();
#endif // __IMPROVE_QUEST_INTERFACE
#ifdef __YS_CHATTING_BLOCKING_SYSTEM
		prj.m_setBlockedUserID.clear();
#endif // __YS_CHATTING_BLOCKING_SYSTEM
#ifdef __FLYFF_INITPAGE_EXT
		m_pTheme->ReadTitleWorld();
#endif //__FLYFF_INITPAGE_EXT
	}
	// 데스크탑 사이즈 세팅 
	g_WndMng.SetWndRect( CRect( 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT ) );
	m_bTitle = TRUE;

#ifdef __AEGON_ADVANCED_LOGIN
	ObjectExecutor(SHORTCUT_APPLET, APP_AEGON_LOGIN);
#else // __AEGON_ADVANCED_LOGIN
	ObjectExecutor(SHORTCUT_APPLET, APP_LOGIN);
#endif // __AEGON_ADVANCED_LOGIN


	// Delete 
	//CloseTitle();
	//CloseField();
	// Open
	//OpenLogin();
	// Flag set
	// 배경 음악  
	CWndBase::m_nAlpha = 255;
	m_bTitle = TRUE;
}
void CWndMgr::OpenField()
{
	m_bAutoRun = FALSE;
	
#if __VER >= 11 // __MOD_VENDOR
	memset(g_Neuz.m_aSavedInven, 0, sizeof(g_Neuz.m_aSavedInven));
#endif
	memset( g_bSlotSwitchAboutEquipItem, FALSE, sizeof( g_bSlotSwitchAboutEquipItem ) );
#ifdef __BAN_CHATTING_SYSTEM
	g_WndMng.SetWarningCounter( 0 );
	g_WndMng.SetWarning2Counter( 0 );
#endif // __BAN_CHATTING_SYSTEM
	if( m_bTitle == FALSE )
		return;
#ifndef __fmod
	StopMusic();
#endif
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		if( g_Neuz.m_apPlayer[i] )
			g_Neuz.m_apPlayer[i]->m_pModel = NULL; 
		SAFE_DELETE( g_Neuz.m_apPlayer[i] );
	}
	g_DialogMsg.ClearAllMessage();
	// 데스크탑 사이즈 세팅 
	SetWndRect( g_Neuz.GetDeviceRect() );
	//jectExecutor( SHORTCUT_APPLET, APP_TASKBAR );
	// Delete 
	//CloseTitle();
	//CloseField();
	// Open
	//OpenTaskBar();
	
	m_aChatString.RemoveAll();
	m_aChatColor.RemoveAll();
	m_aChatStyle.RemoveAll();

	LoadRegInfo( "regInfo.dat" );
	ObjectExecutor( SHORTCUT_APPLET, APP_COMMUNICATION_CHAT );
	ObjectExecutor( SHORTCUT_APPLET, APP_NAVIGATOR );
	ObjectExecutor( SHORTCUT_APPLET, APP_STATUS1 );
	ObjectExecutor( SHORTCUT_APPLET, APP_WORLD );
	OpenTaskBar();

#ifndef __TITANIUM_DISABLED	// Notice Window Disabled
	// 공지가 enable이라면?
	if( g_Option.m_bNotice )
		ObjectExecutor( SHORTCUT_APPLET, APP_INFO_NOTICE );
	else
	// 공지가 열지 않음으로 되어있어도 날짜가 갱신되었다면 강제 오픈
	{
		CFileStatus fileStatus;
		
		CString strFilter;
		strFilter = GetLangFileName( ::GetLanguage(), FILE_NOTICE );
		
		if( CFile::GetStatus( DIR_CLIENT +strFilter, fileStatus ) == TRUE )
		{
			if( g_Option.m_tNoticeTime != fileStatus.m_mtime.GetTime() )
			{
				g_Option.m_tNoticeTime = (time_t)( fileStatus.m_mtime.GetTime() );
				g_Option.m_bNotice = TRUE;
				ObjectExecutor( SHORTCUT_APPLET, APP_INFO_NOTICE );
			}
		}
	}
#endif //__TITANIUM_DISABLED

	CWndBase::m_nAlpha = g_Option.m_nWindowAlpha;

	//GetWndBase( APP_COMMUNICATION_MESSAGE )->SetVisible( TRUE );
	//GetApplet( APP_WORLD )->OnCommand( 10001, 0 );
//	ObjectExecutor( SHORTCUT_APPLET, APP_DIALOG             );
	//OpenWnd1();
	//OpenCharacter();
	//OpenChat();
	// Flag set
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	if( m_pWndQuestQuickInfo )
		SAFE_DELETE( m_pWndQuestQuickInfo );
	m_pWndQuestQuickInfo = new CWndQuestQuickInfo;
	m_pWndQuestQuickInfo->Initialize();
	m_pWndQuestQuickInfo->SetVisible( FALSE );
#endif // __IMPROVE_QUEST_INTERFACE

#ifdef __SYS_PETFILTER
	CWndPetfilter* pWndPetfilter = (CWndPetfilter*)g_WndMng.GetWndBase(APP_PETFILTER);
	if (pWndPetfilter != NULL)
		pWndPetfilter->SetVisible(FALSE);
#endif //__SYS_PETFILTER

	m_bTitle = FALSE;
}
BOOL CWndMgr::OnCommand(UINT nID,DWORD dwMessage)
{
	return TRUE;
}
/*
CItem* CWndMgr::GetFocusItem()
{
	if(m_pWndEquipment) return m_pWndEquipment->GetFocusItem();
	return NULL;
}
*/
void CWndMgr::CloseMessageBox()
{
	//m_pWndMessageBox->Destroy();
	SAFE_DELETE( m_pWndMessageBox );
	SAFE_DELETE( m_pWndMessageBoxUpper );
}
BOOL CWndMgr::OpenCustomBox( LPCTSTR lpszMessage, CWndMessageBox* pWndMessageBox, CWndBase* pWndParent ) 
{ 
	SAFE_DELETE( m_pWndMessageBox );
	m_pWndMessageBox = pWndMessageBox;
	pWndParent = this;

	if(pWndMessageBox->Initialize( pWndParent ) == FALSE)
	{
		SAFE_DELETE( m_pWndMessageBox );
		return TRUE; 
	}
	return FALSE;
}
BOOL CWndMgr::OpenMessageBox( LPCTSTR lpszMessage, UINT nType, CWndBase* pWndParent ) 
{ 
	SAFE_DELETE( m_pWndMessageBox );
	m_pWndMessageBox = new CWndMessageBox;
	pWndParent = this;
	if( m_pWndMessageBox->Initialize( lpszMessage, pWndParent, nType ) == FALSE) 
	{
		SAFE_DELETE( m_pWndMessageBox );
		return TRUE; 
	}
	return FALSE;
}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
BOOL CWndMgr::OpenMessageBoxWithTitle( LPCTSTR lpszMessage, const CString& strTitle, UINT nType, CWndBase* pWndParent )
{
	SAFE_DELETE( m_pWndMessageBox );
	m_pWndMessageBox = new CWndMessageBox;
	pWndParent = this;
	if( m_pWndMessageBox->Initialize( lpszMessage, pWndParent, nType ) == FALSE)
	{
		SAFE_DELETE( m_pWndMessageBox );
		return TRUE;
	}
	m_pWndMessageBox->SetTitle( strTitle );
	return FALSE;
}
#endif // __IMPROVE_QUEST_INTERFACE

BOOL CWndMgr::OpenMessageBoxUpper( LPCTSTR lpszMessage, UINT nType, BOOL bPostLogoutMsg ) 
{ 
	CWndBase* pWndParent = NULL;
	SAFE_DELETE( m_pWndMessageBoxUpper );
	m_pWndMessageBoxUpper = new CWndMessageBoxUpper;
	pWndParent = this;
	if( m_pWndMessageBoxUpper->Initialize( lpszMessage, pWndParent, nType, bPostLogoutMsg ) == FALSE) 
	{
		SAFE_DELETE( m_pWndMessageBoxUpper );
		return TRUE; 
	}
	return FALSE;
}

BOOL CWndMgr::OpenTaskBar()
{
	if (m_pWndTaskBar)
		return FALSE;
	m_pWndTaskBar = new CWndTaskBar;
	if (m_pWndTaskBar->Initialize(this) == FALSE)
	{
		SAFE_DELETE(m_pWndTaskBar);
		return FALSE;
	}
	return TRUE;
}

#ifdef __AEGON_THEME_SWITCHER
void CWndMgr::RestartTaskBar() 
{
	if (m_pWndTaskBar)
	{
		int selectedSlot = m_pWndTaskBar->m_nSlotIndex;
		m_pWndTaskBar->Destroy();
		SAFE_DELETE(m_pWndTaskBar);

		OpenTaskBar();
		g_DPlay.SendReloadTaskbar();

		m_pWndTaskBar->SetItemSlot(selectedSlot);
	}	
}
#endif // __AEGON_THEME_SWITCHER

BOOL CWndMgr::OpenMenu() 
{
	if( m_pWndMenu )
	{
		if( m_pWndMenu->IsVisible() )
			m_pWndMenu->SetVisible( FALSE );
		else
		{
			m_pWndMenu->SetVisible( TRUE );
			m_pWndMenu->SetFocus();
		}
		return FALSE;
	}
	m_pWndMenu = new CWndTaskMenu;
	if( m_pWndMenu->Initialize( this ) == FALSE ) 
	{
		SAFE_DELETE( m_pWndMenu );
		return TRUE; 
	}
	return TRUE;
}
void CWndMgr::OnSize(UINT nType, int cx, int cy)
{
	//m_pWndMessageBox->OnSize(nType,cx,cy);

	//if(m_pWndTaskBar)
	//	m_pWndTaskBar->SetWndRect(CRect(0,CLIENT_HEIGHT-TASKBAR_HEIGHT,CLIENT_WIDTH,CLIENT_HEIGHT));

	//m_pWndMenu->OnSize(nType,cx,cy);
	//m_pWnd1->OnSize(nType,cx,cy);
	//m_pWndCharacter->OnSize(nType,cx,cy);
	//m_pWndChat->OnSize(nType,cx,cy);

	//m_pWndMessanger->OnSize(nType,cx,cy);
	//m_pWndMail->OnSize(nType,cx,cy);
	//m_pWndSound->OnSize(nType,cx,cy);

	CWndBase::OnSize(nType,cx,cy);
}

HRESULT CWndMgr::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	
#ifdef __YDEBUG
	m_texture.InvalidateDeviceObjects();
	m_texCommand.InvalidateDeviceObjects();
	m_texIcon.InvalidateDeviceObjects();
	m_texWnd.InvalidateDeviceObjects();
#endif //__YDEBUG
	return S_OK;
}

HRESULT CWndMgr::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	
#ifdef __YDEBUG
	m_texture.DeleteDeviceObjects();
	m_texCommand.DeleteDeviceObjects();
	m_texIcon.DeleteDeviceObjects();
	m_texWnd.DeleteDeviceObjects();
#endif //__YDEBUG
	return S_OK;
}

HRESULT CWndMgr::RestoreDeviceObjects()
{
	//if( g_WndMng.IsTitle() )
	//	g_WndMng.SetWndRect( CRect( 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT ) );
	//else
	//	g_WndMng.SetWndRect( CRect( 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT - TASKBAR_HEIGHT ) );
	CWndBase::RestoreDeviceObjects();
	
#ifdef __YDEBUG
	m_texture.RestoreDeviceObjects(m_pApp->m_pd3dDevice);
	m_texCommand.RestoreDeviceObjects(m_pApp->m_pd3dDevice);
	m_texIcon.RestoreDeviceObjects(m_pApp->m_pd3dDevice);
	m_texWnd.RestoreDeviceObjects(m_pApp->m_pd3dDevice);
#endif //__YDEBUG
	
	return 0;
}
AppletFunc* CWndMgr::GetAppletFunc( DWORD dwIdApplet )
{
	AppletFunc* pApplet;
	if( m_mapAppletFunc.Lookup( dwIdApplet, (void*&) pApplet ) == TRUE)
		return pApplet;
	/*
	int nCount = 0;
	while( m_appletFunc[ nCount ].m_pFunc )
	{
		if( m_appletFunc[nCount].m_dwIdApplet == dwIdApplet )
			return &m_appletFunc[nCount];
		nCount++;
	}
	*/
	return NULL;
}
DWORD CWndMgr::GetAppletId( TCHAR* lpszAppletName )
{
	AppletFunc* pApplet;
	DWORD dwIdApplet;
	POSITION pos = m_mapAppletFunc.GetStartPosition();
	while( pos )
	{
		m_mapAppletFunc.GetNextAssoc( pos, dwIdApplet, (void*&)pApplet );
		if( !_tcscmp( pApplet->m_pAppletName, lpszAppletName ) )
			return pApplet->m_dwIdApplet;
	}
	/*
	int nCount = 0;
	while( m_appletFunc[ nCount ].m_pFunc )
	{
		if( !strcmp( m_appletFunc[nCount].m_pAppletName, lpszAppletName ) )
			return m_appletFunc[nCount].m_dwIdApplet;
		nCount++;
	}
	*/
	return 0xffffffff;
}
CWndBase* CWndMgr::GetApplet( DWORD dwIdApplet )
{
	CWndBase* pWndBase;
	if( m_mapWndApplet.Lookup( dwIdApplet, (void*&)pWndBase ) == FALSE )
		return NULL;
	return pWndBase;
}

void CWndMgr::__HotKeyChange(DWORD dwId, char *pch)
{
	if( g_Option.m_nInterface == 2 )
	{
		switch( dwId )
		{
		case APP_NAVIGATOR:
			*pch = 0;
			break;
		case APP_STATUS1:
			*pch = 0;
			break;
#if __VER >= 8 // __S8_VENDOR_REVISION
		case APP_VENDOR_REVISION:
#else // __VER >= 8 // __S8_VENDOR_REVISION
		case APP_VENDOREX:
#endif // __VER >= 8 // __S8_VENDOR_REVISION
			*pch = 0;
			break;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		case APP_QUEST_EX_LIST:
#else // __IMPROVE_QUEST_INTERFACE
		case APP_QUEST:
#endif // __IMPROVE_QUEST_INTERFACE
			*pch = 'L';
			break;
		case APP_MESSENGER_:
			*pch = 'T';
			break;
		}
	}	
}

#ifdef __CHANGE_HOT_KEY
void CWndMgr::AddAppletFunc(LPVOID pAppletFunc, DWORD dwIdApplet, LPCTSTR lpszAppletName, LPCTSTR pszIconName, LPCTSTR lpszAppletDesc, CHAR cHotkey, LPCTSTR lpszAppletText, DWORD dwAppletAuth)
#else // __CHANGE_HOT_KEY
void CWndMgr::AddAppletFunc(LPVOID pAppletFunc, DWORD dwIdApplet, LPCTSTR lpszAppletName, LPCTSTR pszIconName, LPCTSTR lpszAppletDesc, CHAR cHotkey)
#endif // __CHANGE_HOT_KEY
{
	AppletFunc* pApplet = new AppletFunc;
	pApplet->m_pFunc       = (CWndBase* (*)()) pAppletFunc;
	pApplet->m_dwIdApplet  = dwIdApplet ;
	pApplet->m_pAppletName = lpszAppletName;
	pApplet->m_pAppletDesc = lpszAppletDesc;
	pApplet->m_pszIconName = pszIconName; 
#ifdef __Y_INTERFACE_VER3
	__HotKeyChange( dwIdApplet, &cHotkey );
#endif //__Y_INTERFACE_VER3
	pApplet->m_cHotkey = cHotkey;
#ifdef __CHANGE_HOT_KEY
	pApplet->m_pAppletText = lpszAppletText;
	pApplet->m_dwAppletAuth = dwAppletAuth;
#endif // __CHANGE_HOT_KEY
	m_mapAppletFunc.SetAt( dwIdApplet, pApplet );
}
CWndBase* CWndMgr::CreateApplet( DWORD dwIdApplet )
{
	CWndNeuz* pWndBase = NULL;
	AppletFunc* pAppletFunc = NULL;
	if( m_mapAppletFunc.Lookup( dwIdApplet, (void*&)pAppletFunc ) == FALSE )
	{
		// Applet이 존재하지 않을 경우 생성 
		pAppletFunc = GetAppletFunc( dwIdApplet );
	}
	if( pAppletFunc )
	{
		CWndWorld* pWndWorld = (CWndWorld*)GetWndBase( APP_WORLD );
	#if __VER >= 12 // __MOD_TUTORIAL
		CWndGuideSystem* pWndGuide = NULL;
		pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
		if(pWndGuide && pWndGuide->IsVisible()) 
		{
			if(pWndGuide->m_CurrentGuide.m_nVicCondition == OPEN_WINDOW && pWndGuide->m_CurrentGuide.m_nInput == (int)dwIdApplet)
				pWndGuide->m_Condition.nOpenedWindowID = 1;
		}
	#else
		if( pWndWorld ) 
			pWndWorld->m_pWndGuideSystem->SendGuideMessage(pAppletFunc->m_dwIdApplet);
	#endif
		pWndBase = (CWndNeuz*)GetWndBase( pAppletFunc->m_dwIdApplet );
		if( pWndBase == NULL )
		{
			pWndBase = (CWndNeuz*)(*pAppletFunc->m_pFunc)();
			if( pWndBase )
			{
				if( pWndBase->Initialize( this, dwIdApplet ) == FALSE )  
				{
					SAFE_DELETE( pWndBase );
					return pWndBase;
				}
				LPWNDREGINFO pWndRegInfo;
				if( m_mapWndRegInfo.Lookup( dwIdApplet, (void*&)pWndRegInfo ) == TRUE )
				{
					if( pWndRegInfo->dwSize )
					{
						// load
						CAr ar( pWndRegInfo->lpArchive, pWndRegInfo->dwSize );
						((CWndNeuz*)pWndBase)->SerializeRegInfo( ar, pWndRegInfo->dwVersion );
					}
					if( pWndBase->IsWndStyle( WBS_THICKFRAME ) )
					{
						if( pWndRegInfo->dwWndSize == WSIZE_WINDOW )
						{
							((CWndNeuz*)pWndBase)->SetSizeWnd();
							pWndBase->SetWndRect( pWndRegInfo->rect );
						}								
						if( pWndRegInfo->dwWndSize == WSIZE_MAX )
							((CWndNeuz*)pWndBase)->SetSizeMax();
					}
					else
					{
						CRect wndRect = pWndBase->GetWindowRect();
#ifdef __AEGON_FULLSCREEN_AND_RESIZE
						if (pWndRegInfo->rect.left + wndRect.Width() > g_WndMng.resizeWidth) 
						{
							pWndRegInfo->rect.left = g_WndMng.resizeWidth - wndRect.Width();
						}
						if (pWndRegInfo->rect.top + wndRect.Height() > g_WndMng.resizeHeight) 
						{
							pWndRegInfo->rect.top = g_WndMng.resizeHeight - wndRect.Height();
						}
#endif // __AEGON_FULLSCREEN_AND_RESIZE
						pWndBase->SetWndRect( 
							CRect( 
							pWndRegInfo->rect.left, 
							pWndRegInfo->rect.top, 
							pWndRegInfo->rect.left + wndRect.Width(), 
							pWndRegInfo->rect.top + wndRect.Height() 
							) );
					}							
					//pWndBase->SetWndRect( pWndRegInfo->rect );
				}
				//LPWNDAPPLET lpApplet = m_resMng.GetAt( dwIdApplet );
				//if( lpApplet ) 
				//	pWndBase->SetToolTip( m_resMng.GetAt( dwIdApplet )->strToolTip );
				//pWndBase->SetToolTip( pAppletFunc->m_pAppletDesc );
				m_mapWndApplet.SetAt( dwIdApplet, pWndBase );
			}
		}
		else
			pWndBase->SetFocus();
	}
	return pWndBase;
}
void CWndMgr::ObjectExecutor( DWORD dwShortcut, DWORD dwId, DWORD dwType )
{
	SHORTCUT shortcut;
	ZeroMemory( &shortcut, sizeof( shortcut ) );
	shortcut.m_dwShortcut = dwShortcut;
	shortcut.m_dwId = dwId;
	shortcut.m_dwType = dwType;
	ObjectExecutor( &shortcut);
}
void CWndMgr::ObjectExecutor( LPSHORTCUT pShortcut )
{
	if( pShortcut->m_dwShortcut == SHORTCUT_APPLET )
	{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		CWndBase* pWndBase = (CWndBase*)g_WndMng.GetWndBase( APP_DIALOG_EX );
#else // __IMPROVE_QUEST_INTERFACE
		CWndBase* pWndBase = (CWndBase*)g_WndMng.GetWndBase( APP_DIALOG );
#endif // __IMPROVE_QUEST_INTERFACE
		if( pWndBase )
			return;

#if __VER >= 10 // __CSC_VER9_1
#ifdef __NEW_SKILL_TREE
		if( pShortcut->m_dwId == APP_SKILL4 )
#else // __NEW_SKILL_TREE
		if( pShortcut->m_dwId == APP_SKILL3 )
#endif // __NEW_SKILL_TREE
#else
		if( pShortcut->m_dwId == APP_SKILL1 )
#endif //__CSC_VER9_1
		{
			CWndBase* pWndBase = (CWndBase*)g_WndMng.GetWndBase( APP_QUEITMWARNING );	
			if( pWndBase )
				return;			// 스킬 확인창이면 스킬창을 닫을수가 없음.
#if __VER >= 10 // __CSC_VER9_1
#ifdef __NEW_SKILL_TREE
			CWndBase* pWndBase1 = (CWndBase*)g_WndMng.GetWndBase( APP_SKILL4 );
#else // __NEW_SKILL_TREE
			CWndBase* pWndBase1 = (CWndBase*)g_WndMng.GetWndBase( APP_SKILL3 );
#endif // __NEW_SKILL_TREE
#else
			CWndBase* pWndBase1 = (CWndBase*)g_WndMng.GetWndBase( APP_SKILL1 );
#endif //__CSC_VER9_1
			if(IsValidObj(g_pPlayer) && pWndBase1 && ((CWndSkillTreeEx*)pWndBase1)->GetCurrSkillPoint() != g_pPlayer->m_nSkillPoint )
			{
				if( pWndBase == NULL )
				{
					SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
					g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
					g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
					g_WndMng.m_pWndReSkillWarning->InitItem( TRUE );
				}
				return;
			}
		}
		
		if( pShortcut->m_dwId == APP_QUIT)
		{
			OpenCustomBox( NULL, new CWndQuit );
		}
		else
		if( pShortcut->m_dwId == APP_LOGOUT)
		{
			// 공격중이면 로그아웃 불가
			if(IsValidObj(g_pPlayer) && g_pPlayer->IsAttackMode() )
			{
				g_WndMng.PutString( prj.GetText(TID_PK_COMBAT_END),NULL,COLOR_ERROR);//, CHATSTY_SYSTEM_CHAT );
				return;
			}
			else
			{
				OpenCustomBox( NULL, new CWndLogOut );
			}
		}
#if __VER >= 8 //__CSC_VER8_2
		else if(pShortcut->m_dwId == APP_PARTY)
		{
			CWndBase* pWndParty = (CWndBase*)g_WndMng.GetApplet( APP_PARTY );
			CWndBase* pWndPartyQuick = (CWndBase*)g_WndMng.GetApplet( APP_PARTY_QUICK );
			if(pWndParty == NULL && pWndPartyQuick == NULL)
			{
				CreateApplet( APP_PARTY ); //Party창이 모두 없을 경우 APP_PARTY창 생성.
				if(g_Party.GetSizeofMember() >= 2) //PartyQuick창은 Party Member가 있을 경우만 생성.
				{
					CreateApplet( APP_PARTY_QUICK );
				}
			}
			else if(pWndParty != NULL && pWndPartyQuick != NULL) 
			{
				if(pWndParty && pWndPartyQuick)//Party창이 모두 있을 경우 PARTY창 전부 파괴.
				{
					pWndParty->Destroy();
					pWndPartyQuick->Destroy();
					SetFocus();
				}
			}
			else //Party창이 하나라도 켜져있다면 켜진 창만 파괴.
			{
				if(pWndParty != NULL)
				{
					pWndParty->Destroy();
					SetFocus();	
				}
				else if(pWndPartyQuick != NULL)
				{
					pWndPartyQuick->Destroy();
					SetFocus();	
				}
			}
		}
#endif //__CSC_VER8_2

#ifdef __AEGON_MONSTER_HUNT
		else if (pShortcut->m_dwId == APP_MONSTER_HUNT)
		{
			if (!g_WndMng.GetApplet(APP_MONSTER_HUNT))
			{
				g_WndMng.CreateApplet(APP_MONSTER_HUNT);

			}
			else
			{
				((CWndMonsterHunt*)g_WndMng.GetApplet(APP_MONSTER_HUNT))->OnDestroy();
			}
		}
#endif // __AEGON_MONSTER_HUNT

#ifdef __AEGON_LOOT_BOX
		else if (pShortcut->m_dwId == APP_LOOTBOX)
		{
			if (!g_WndMng.GetApplet(APP_LOOTBOX))
			{
				g_WndMng.CreateApplet(APP_LOOTBOX);

			}
			else
			{
				((CWndLootBoxMain*)g_WndMng.GetApplet(APP_LOOTBOX))->OnDestroy();
			}
		}
#endif // __AEGON_LOOT_BOX

		else
		{
			CWndBase* pWndBase = (CWndBase*)g_WndMng.GetApplet( pShortcut->m_dwId );
			if( pWndBase )
			{
#if __VER >= 12	// __MOD_TUTORIAL
				if(pShortcut->m_dwId == APP_INFOPANG && !pWndBase->IsVisible())
				{
					pWndBase->OnInitialUpdate();
					//pWndBase->Destroy();
					//CreateApplet( pShortcut->m_dwId );
					SetFocus();
				}
				else
				{
					pWndBase->Destroy();
					SetFocus();
				}

#else
				pWndBase->Destroy();
				SetFocus();
#endif
			}
#if __VER >= 8 //__CSC_VER8_5
			else if( pShortcut->m_dwId == APP_VENDOR_REVISION )
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_SUMMON_ANGEL );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_SUMMONANGELUSING ), NULL, prj.GetTextColor( TID_GAME_SUMMONANGELUSING ) );
				}
				else
					CreateApplet( pShortcut->m_dwId );
			}
#endif //__CSC_VER8_5
			else if( CreateApplet( pShortcut->m_dwId ) == FALSE )
			{
			}
		}
	}
	else if( pShortcut->m_dwShortcut == SHORTCUT_CHAT )
	{
		{
		//	m_timerDobe.Reset();
			CWndWorld* pWndWorld = (CWndWorld*)GetWndBase( APP_WORLD );
		#if __VER < 12 // __MOD_TUTORIAL
			if( pWndWorld ) 
				pWndWorld->m_pWndGuideSystem->SendGuideMessage(pShortcut->m_dwId);
		#endif
#ifdef __BAN_CHATTING_SYSTEM
			m_bShortcutCommand = TRUE;
#endif // __BAN_CHATTING_SYSTEM
			g_WndMng.ParsingChat( pShortcut->m_szString );
		}
	}
#ifdef __AEGON_TELEPORT_SHORTCUT
	else if (pShortcut->m_dwShortcut == SHORTCUT_TELEPORT)
	{
		g_DPlay.SendTeleportByApp(pShortcut->m_dwId);
	}
#endif // __AEGON_TELEPORT_SHORTCUT
	else if( pShortcut->m_dwShortcut == SHORTCUT_EMOTICON)
	{

		TCHAR szCmd[1024] = { 0 };
		
		_tcscat(szCmd, "/");
#ifdef __AEGON_EMOTE_SYSTEM
		if (pShortcut->m_dwType > 0) 
		{			
			_tcscat(szCmd, CAegonEmoticonMng::GetInstance()->GetCommand(pShortcut->m_dwId, pShortcut->m_dwType));
		}
		else
#endif //__AEGON_EMOTE_SYSTEM
			_tcscat(szCmd, g_DialogMsg.m_EmiticonCmd[pShortcut->m_dwId].m_szCommand);
			

		g_WndMng.ParsingChat( szCmd );
	}
	else if( pShortcut->m_dwShortcut == SHORTCUT_ITEM  )
	{
		if( CMover::GetActiveMover()->m_vtInfo.VendorIsVendor() )
			return;
#ifdef __MIKSIK_EQUALIZED_MODE
		if (g_WndMng.GetWndBase(APP_TMP_CLASS))
			return;
#endif //__MIKSIK_EQUALIZED_MODE
		CItemBase* pItemBase = g_pPlayer->GetItemId( pShortcut->m_dwId );
		if( IsUsableItem( pItemBase ) )
		{
			CCtrl* pCtrl = (CCtrl*)g_WorldMng.Get()->GetObjFocus();
			DWORD dwObjId = NULL_ID;
			if( pCtrl && pCtrl->GetType() != OT_OBJ )
				dwObjId = pCtrl->GetId();

			BOOL bAble = TRUE;
			if( pItemBase )
			{
				if( pItemBase->GetProp()->dwItemKind1 == IK1_RIDE )	// 사용한 아이템이 탈것인가.
					if( g_pPlayer->m_pActMover->IsFly() == FALSE )		// Ground상태인가?
						if( g_pPlayer->m_pActMover->m_bGround == 0 )		// 땅에 발을 안붙이고 있다면 못탄다.
							bAble = FALSE;	// 아이템사용 실패.
			}

			if( bAble )	// 성공판정 났을때만 보냄.
			{
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
				CItemElem* pItemElem = ( CItemElem* )pItemBase;
				if( pItemElem->GetProp() && ( pItemElem->GetProp()->dwFlag & IP_FLAG_EQUIP_BIND ) && !pItemElem->IsFlag( CItemElem::binds ) )
				{
					SAFE_DELETE( g_WndMng.m_pWndEquipBindConfirm )
						g_WndMng.m_pWndEquipBindConfirm = new CWndEquipBindConfirm( CWndEquipBindConfirm::EQUIP_DOUBLE_CLICK );
					g_WndMng.m_pWndEquipBindConfirm->SetInformation( pItemElem, dwObjId );
					g_WndMng.m_pWndEquipBindConfirm->Initialize( NULL );
				}
				else
				{
#endif // __IMPROVE_SYSTEM_VER15
					// 탈착 명령일 경우, nPart는 실제 장착되어 있는 부분과 일치해야 하므로 프로퍼티에서 꺼내지 않는다.
					int nPart = pItemBase->GetProp()->dwParts;
					BOOL bEquiped	= g_pPlayer->m_Inventory.IsEquip( pItemBase->m_dwObjId );
					if( bEquiped )
						nPart	= pItemBase->m_dwObjIndex - g_pPlayer->m_Inventory.m_dwIndexNum;
					
#if __VER >= 15 // __PETVIS
					//gmpbigsun: 퀵슬롯에서 실행된 경우 비스관련 캐쉬인경우 재확인창으로.. 09_1222
					if( !CheckConfirm( pItemBase ) )		//gmpbigsun: 패킷보내기전 확인등의 처리담당
						g_DPlay.SendDoUseItem( MAKELONG( 0, pItemBase->m_dwObjId ), dwObjId, nPart );
#else	//__PETVIS
						g_DPlay.SendDoUseItem( MAKELONG( 0, pItemBase->m_dwObjId ), dwObjId, nPart );
#endif // __PETVIS
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
				}
#endif // __IMPROVE_SYSTEM_VER15
			}
		}
	}
	else if( pShortcut->m_dwShortcut == SHORTCUT_SKILL )
	{
#ifdef __AEGON_FIXES
	CMover* pAvtiveMover = CMover::GetActiveMover();
	if (IsInvalidObj(pAvtiveMover) || pAvtiveMover->m_vtInfo.VendorIsVendor())
		return;
#else
		if( CMover::GetActiveMover()->m_vtInfo.VendorIsVendor() )
			return;
#endif
#ifdef __MIKSIK_EQUALIZED_MODE
		if (g_WndMng.GetWndBase(APP_TMP_CLASS))
			return;
#endif //__MIKSIK_EQUALIZED_MODE
		if( pShortcut->m_dwType == 2 )
		{
#ifdef __GAMEGUARD
			if (g_Party.m_nSizeofMember >= 2)
#else // __GAMEGUARD
			if (g_Party.m_nSizeofMember >= 2 && g_Party.m_nKindTroup == 1)
#endif // __GAMEGUARD
			{
				ItemProp* pItemProp =  prj.GetPartySkill( pShortcut->m_dwId );
				if( pItemProp )
				{
					if (g_Party.IsLeader(g_pPlayer->m_idPlayer)
#ifdef __PARTY_ASSISTANT
						|| g_pPlayer->GetAssist() == TRUE
#endif //__PARTY_ASSISTANT
						)
					{
#ifdef __GAMEGUARD
						if (g_Party.m_nKindTroup != 1)
							return;
#endif // __GAMEGUARD
	#ifndef __PARTYDEBUG
						if( int(g_Party.GetLevel() - pItemProp->dwReqDisLV) >= 0 )
	#endif // __PARTYDEBUG
						{
	#ifndef __PARTYDEBUG
							if( (g_Party.GetPoint() - pItemProp->dwExp) >= 0 )
	#endif // __PARTYDEBUG
							{
								// 스킬사용
								ItemProp* pItemProp = prj.GetPartySkill( pShortcut->m_dwId );
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
				#ifdef __BUFF_1107
								DWORD	dwSkillTimeTmp = 0;
								CWndWorld *pWndWorld = (CWndWorld *)g_WndMng.m_pWndWorld;
								for( MAPBUFF::iterator i = pWndWorld->m_buffs.m_mapBuffs.begin(); i != pWndWorld->m_buffs.m_mapBuffs.end(); ++i )
								{
									IBuff* ptr	= i->second;
									if( ptr->GetId() == pShortcut->m_dwId )	
										dwSkillTimeTmp = ptr->GetTotal();
								}
								#define II_SYS_SYS_SCR_PARTYEXPUP01_01 20296
								#define II_SYS_SYS_SCR_PARTYSKILLUP01_01 20297
								if( dwSkillTimeTmp == 0 
									|| g_pPlayer->HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01 )
									|| g_pPlayer->HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP02 )
									|| g_pPlayer->HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01_01 ) )
				#else // __BUFF_1107
								SKILLINFLUENCE* pSkilltmp;
								DWORD	dwSkillTimeTmp = 0;
								CWndWorld *pWndWorld = (CWndWorld *)g_WndMng.m_pWndWorld;
								for( int i=0; i<MAX_SKILLINFLUENCE; i++)
								{
									pSkilltmp = pWndWorld->m_partySkillState.Get(i);
									if( pSkilltmp->wID == pShortcut->m_dwId )	
										dwSkillTimeTmp = pSkilltmp->tmCount;
								}
								if( dwSkillTimeTmp == 0 
									|| g_pPlayer->m_SkillState.HasSkill( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01 )
									|| g_pPlayer->m_SkillState.HasSkill( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP02 )
									|| g_pPlayer->m_SkillState.HasSkill( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01_01 ) )
				#endif // __BUFF_1107
#else// //12차 극단유료아이템
								if( g_tmCurrent >= m_dwSkillTime[pShortcut->m_dwId] + pItemProp->dwCircleTime )
#endif // //12차 극단유료아이템
								{
									g_DPlay.SendPartySkillUse( pShortcut->m_dwId );
									m_dwSkillTime[pShortcut->m_dwId] = g_tmCurrent;
								}
								else
								{
									/*
									#define ST_CALL                            1
									#define ST_BLITZ                           2
									#define ST_RETREAT                         3
									#define ST_SCOUT                           4
									#define ST_SPHERECIRCLE                    5
									#define ST_LINKATTACK                      6
									#define ST_FORTUNECIRCLE                   7
									#define ST_STRETCHING                      8
									#define ST_GIFTBOX                         9
									*/
									if( pShortcut->m_dwId != ST_CALL    &&
										pShortcut->m_dwId != ST_BLITZ   &&
										pShortcut->m_dwId != ST_RETREAT )
									{
										CString str;
										str.Format( prj.GetText( TID_GAME_NOT_TIME ), pItemProp->szName );
										g_WndMng.PutString( (LPCTSTR)str, NULL, prj.GetTextColor( TID_GAME_NOT_TIME ) );
									}
								}
							}
	#ifndef __PARTYDEBUG
							else g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0049) ) );
	#endif // __PARTYDEBUG
						}
	#ifndef __PARTYDEBUG
						else // 렙이 모자라 스킬을 사용할수 없음
							g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0019) ) );
	#endif // __PARTYDEBUG
					}
					else // 단장이 아니다
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0047) ) );
				}
			}
#ifdef __GAMEGUARD
			else
			{
				g_WndMng.OpenMessageBox(_T(prj.GetText(TID_MMI_PARTYMASSAGEERROR02)));
				return;
			}
#endif // __GAMEGUARD
		}
		else
		{	// 숏컷이 스킬일 경우
			UseSkillShortCut( pShortcut->m_dwType, pShortcut->m_dwId );
		}
	}
#if __VER >= 12 // __LORD
	else if( pShortcut->m_dwShortcut == SHORTCUT_LORDSKILL )
	{
		// 군주스킬을 실행한다
		CCtrl* pCtrl = (CCtrl*)g_WorldMng.Get()->GetObjFocus();
		DWORD dwObjId = NULL_ID;
		CCLord* pLord									= CCLord::Instance();
		CLordSkillComponentExecutable* pComponent		= pLord->GetSkills()->GetSkill(pShortcut->m_dwId);

		switch(pShortcut->m_dwId)
		{
			case LI_SUMMON:
			case LI_TELEPORT:
				if(pComponent->GetTick() == 0)
				{
					SAFE_DELETE(m_pWndLordSkillConfirm);
					m_pWndLordSkillConfirm = new CWndLordSkillConfirm(pShortcut->m_dwId);
					m_pWndLordSkillConfirm->Initialize();
				}
				else
				{
					PutString(prj.GetText(TID_GAME_STILLNOTUSE), NULL, prj.GetTextColor(TID_GAME_STILLNOTUSE));
				}
				break;

			default:
				if(!pCtrl)
				{
					g_DPlay.SendLordSkillUse(pShortcut->m_dwId, g_pPlayer->GetName());
				}
				else if(pCtrl && pCtrl->GetType() == OT_MOVER)
				{
					if(((CMover*)pCtrl)->IsPlayer())
						g_DPlay.SendLordSkillUse(pShortcut->m_dwId, ((CMover*)pCtrl)->GetName());
				}
				break;
		};
		
	}
#endif
	else if( pShortcut->m_dwShortcut == SHORTCUT_MOTION )
	{
#ifdef __MIKSIK_EQUALIZED_MODE
		if (g_WndMng.GetWndBase(APP_TMP_CLASS))
			return;
#endif //__MIKSIK_EQUALIZED_MODE
		{
			CWndBase* pWndBase	= GetWndVendorBase();
			if( pWndBase ) // 상점창이 열린 상태에서는 모션 실행 금지
				return;
		}
		MotionProp* pMotionProp = prj.GetMotionProp( pShortcut->m_dwId );
		if( pMotionProp->szName[ 0 ] )
		{
			strcpy( m_szTimerChat, pMotionProp->szName );
			m_timerChat.Set( 1000 );
		}
		bool fBehavior	= false;
		if( pShortcut->m_dwId == MOT_BASE_PICKUP )
			g_pPlayer->DoPickupItemAround();
		else		
		if( pShortcut->m_dwId == MOT_BASE_BATTLE )
		{
			if( g_pPlayer->m_pActMover->IsStateFlag( OBJSTAF_COMBAT ) ) {
				if( g_pPlayer->SendActMsg( OBJMSG_MODE_PEACE ) == 1 ) {
					fBehavior	= true;
				}
			}
			else {
				if( g_pPlayer->SendActMsg( OBJMSG_MODE_COMBAT ) == 1 ) {
					fBehavior	= true;
				}
			}
		}
		else
		// 앉기/서기 전환
		if( pShortcut->m_dwId == MOT_BASE_SIT )
		{
			if( g_pPlayer->m_pActMover->IsStateFlag( OBJSTAF_SIT ) ) {	// 앉아있는 상태일때
				if( FALSE == g_pPlayer->m_pActMover->IsActJump() &&
					FALSE == g_pPlayer->m_pActMover->IsActAttack() && 
					FALSE == g_pPlayer->m_pActMover->IsActDamage() &&
					FALSE == g_pPlayer->m_pActMover->IsAction() &&
					!( g_pPlayer->m_dwReqFlag & REQ_USESKILL ) )
				{
					g_DPlay.SendMotion( OBJMSG_STANDUP );
				}
			}
			else {
				if( FALSE == g_pPlayer->m_pActMover->IsActJump() &&
					FALSE == g_pPlayer->m_pActMover->IsActAttack() && 
					FALSE == g_pPlayer->m_pActMover->IsActDamage() &&
					FALSE == g_pPlayer->m_pActMover->IsAction() &&
					!( g_pPlayer->m_dwReqFlag & REQ_USESKILL ) )
				{
					g_DPlay.SendMotion( OBJMSG_SITDOWN );
				}
			}
		} 
		else
		if( pShortcut->m_dwId == MOT_BASE_COLLECT )
		{
//			CObj *pFocus = g_pPlayer->GetWorld()->GetObjFocus();	// 현재 잡힌 타겟.
//			g_pPlayer->CMD_SetCollect( pFocus );
		}
		else
		if( pShortcut->m_dwId == MOT_BASE_ESCAPE )
		{
			g_DPlay.SendDoEscape();
		}
		else
		if( pShortcut->m_dwId == MOT_BASE_CHEER )
		{
			CObj *pFocus = g_pPlayer->GetWorld()->GetObjFocus();	// 현재 잡힌 타겟.

			if( pFocus && pFocus->GetType() == OT_MOVER && ((CMover*)pFocus)->IsPlayer() )
			{
				g_DPlay.SendCheering( ((CMover*)pFocus)->GetId() );
				fBehavior	= false;
			}
			else
			{
				g_WndMng.PutString( prj.GetText( TID_CHEER_NO2 ), NULL, prj.GetTextColor( TID_CHEER_NO2 ) );
			}
		}
#if __VER >= 8     // 8차 타겟클릭자동공격 이모티콘
		else
		if( pShortcut->m_dwId == MOT_BASE_AUTOATTACK )
		{
			CObj *pFocus = g_pPlayer->GetWorld()->GetObjFocus();	// 현재 잡힌 타겟.

			if(g_pPlayer->IsAttackAble(pFocus) != NULL)
			{
				if( pFocus && pFocus->GetType() == OT_MOVER )//&& ((CMover*)pFocus)->IsNPC() )플레이어에게도 쓸수있음 pk용
				{
					ItemProp *pItemProp = g_pPlayer->GetActiveHandItemProp();
					if( pItemProp && pItemProp->dwItemKind3 == IK3_WAND ) //Wand일 경우 AutoAttack을 하지 않음.
					{
						CCtrl* pFocusObj = (CCtrl*)pFocus;
						g_pPlayer->CMD_SetMagicAttack( pFocusObj->GetId(), 0 );
					}
					else
						((CWndWorld *)g_WndMng.m_pWndWorld)->m_bAutoAttack = TRUE;
				}
				else
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_NOTTARGET ), NULL, prj.GetTextColor( TID_GAME_NOTTARGET ) );
				}
			}
		}
#endif	// __VER >= 8
#if __VER >= 13 // __CSC_VER13_2
		else if( pShortcut->m_dwId == MOT_BASE_COUPLE )
		{
			CObj *pFocus = g_pPlayer->GetWorld()->GetObjFocus();	// 현재 잡힌 타겟.

			if( pFocus && pFocus->GetType() == OT_MOVER && ((CMover*)pFocus)->IsPlayer() )
			{
				// 프러포즈 보낼 메세지 창 출력
				if(g_WndMng.m_pWndCoupleMessage)
					SAFE_DELETE(g_WndMng.m_pWndCoupleMessage);

				g_WndMng.m_pWndCoupleMessage = new CWndCoupleMessage;
				if(g_WndMng.m_pWndCoupleMessage)
				{
					CString strText;
					strText.Format(prj.GetText(TID_GAME_PROPOSETO), ((CMover*)pFocus)->GetName());
					g_WndMng.m_pWndCoupleMessage->SetMessageMod(strText, CWndCoupleMessage::CM_SENDPROPOSE, pFocus);
					g_WndMng.m_pWndCoupleMessage->Initialize(NULL);
				}
			}
			else
			{
				g_WndMng.PutString( prj.GetText( TID_GAME_NOTCOUPLETARGET ), NULL, prj.GetTextColor( TID_GAME_NOTCOUPLETARGET ) );
			}
		}
#endif //__CSC_VER13_2
		else
		{
			// 기타 이모션.
			MotionProp *pProp = prj.GetMotionProp( pShortcut->m_dwId );
			if( pProp )
			{
				int nMotion = pProp->dwMotion;
				int nPlayMode = 0;
				switch( pProp->dwPlay )
				{
				case 0:	nPlayMode = ANILOOP_1PLAY;	break;
				case 1:	nPlayMode = ANILOOP_CONT;	break;
				case 2:	nPlayMode = ANILOOP_LOOP;	break;
				default: ANILOOP_1PLAY;
				}

				if( g_pPlayer->SendActMsg( OBJMSG_MOTION, nMotion, nPlayMode ) == 1 )
				{
					switch(nMotion)
					{
						case MTI_POINTWAYNORTH:
							g_pPlayer->SetAngle(180.0f);
							break;
						case MTI_POINTWAYSOUTH:
							g_pPlayer->SetAngle(0.0f);
							break;
						case MTI_POINTWAYEAST:
							g_pPlayer->SetAngle(90.0f);
							break;
						case MTI_POINTWAYWEST:
							g_pPlayer->SetAngle(270.0f);
							break;
					}
					fBehavior	= true;
				}
			}
		}
				
		if( fBehavior ) 
		{
			g_pPlayer->ClearDest();
		}
		if( fBehavior )
			g_DPlay.SendPlayerBehavior();
	}
#ifdef __SWITCH_EQUIP
	else if (pShortcut->m_dwShortcut == SHORTCUT_SWITCH_EQUIP)
	{
	if (pShortcut->m_dwId >= 0 && pShortcut->m_dwId < g_pPlayer->m_vEquipSwitchInfo.size())
	{
		if (g_pPlayer->m_Inventory.GetEmptyCount() > 2)
		{
			g_DPlay.SendUseEquipSwitch(pShortcut->m_dwId);
		}
		else
		{
			g_WndMng.PutString("You need at least 3 free item slot in your Inventory", NULL, 0xffffff00);
		}
	}
	}
#endif // __SWITCH_EQUIP
	else
	if( pShortcut->m_dwShortcut == SHORTCUT_OBJECT )
	{
		CMover* CMover = prj.GetMover( pShortcut->m_dwId );
		if( CMover )
		{
			CWndWorld* pWndWorld = (CWndWorld*) GetWndBase( APP_WORLD );
			//if( g_WorldMng()->GetObjFocus() != CMover )
			//	pWndWorld->m_bNewSelect = TRUE;
			g_WndMng.OpenMessage( pShortcut->m_szString );

			//pWndMessage->AddMessage( lpString );

			//g_WorldMng()->SetObjFocus( CMover );
			

			/*
			if( pObj->GetType() == OT_MOVER )
			{
				CMover* pMover = (CMover*) pObj;
				// 자신이라면 상태창  
				if( pMover == g_Neuz.m_pPlayer )
				{
					CreateApplet( APP_EQUIPMENT );
				}
				elseaaaaaaaaaaaaaaa
				// NPC라면 대화 혹은 전투 
				if( pMover->IsNPC() )
				{
					g_World.SetObjFocus( pMover );
					CMover* pMover = (CMover*) pObj;
					g_WndMng.CreateApplet( APP_DIALOG ); 					
					
					//CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
					//if( pWndWorld )
					//{
					//	if( pWndWorld->m_bBattle )
					//	{
						//}

				}
				else
				// 플레이어라면 쪽지 전송 
				{

				}
			}
			*/
		}
	}
	else
	if( pShortcut->m_dwShortcut == SHORTCUT_SKILLFUN )
	{
		g_WndMng.m_pWndWorld->UseSkill();
#ifdef __ACTION_SLOT_PAGES
		g_WndMng.m_pWndWorld->SetCurrentActionSlot(pShortcut->m_dwUserId);
#endif // __ACTION_SLOT_PAGES
	}

}
BOOL CWndMgr::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	return TRUE;
	//CRect rect = GetClientRect();
	//rect.DeflateRect( 5, 5);
	for( int i = 0; i < m_awndShortCut.GetSize(); i++)
	{
		if( ((CWndBase*)m_awndShortCut.GetAt( i ))->GetWndId() == pShortcut->m_dwIndex )
		{

			((CWndBase*)m_awndShortCut.GetAt( i ))->Move( CPoint( point.x - 12, point.y - 12 ) );
			return TRUE;
		}
	}
	CWndButton* pWndButton = new CWndButton;
	pWndButton->Create( pShortcut->m_szString,WBS_MENUITEM,CRect( point.x - 10, point.y - 10, point.x + 10, point.y + 10),this, pShortcut->m_dwIndex);
	pWndButton->m_shortcut.m_dwShortcut = pShortcut->m_dwShortcut;
	pWndButton->AddWndStyle( WBS_MOVE );
	m_awndShortCut.Add( pWndButton );
	pWndButton->m_strToolTip = pWndButton->GetTitle();
	return TRUE;
}
BOOL CWndMgr::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	if( message == WNM_CLICKED )
	{
	//	CWndButton* pWndButton = (CWndButton*)pLResult;
	//	ObjectExecutor( pWndButton->m_n,  nID );
	}
	return TRUE;
}

/*
BOOL CWndMgr::WndMessageBoxToTitle(CString strMessage) 
{ 
	if(m_pWndMessageBox) delete m_pWndMessageBox;
	m_pWndMessageBox = new CWndToTitleBox;
	if(m_pWndMessageBox->Initialize(strMessage,this) == FALSE) 
	{
		delete m_pWndMessageBox;
		m_pWndMessageBox = NULL;
		return TRUE; 
	}
	return FALSE;
}


*/
BOOL CWndMgr::OnEraseBkgnd( C2DRender* p2DRender )
{
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
#ifdef __GAME_GRADE_SYSTEM
	static DWORD dwTimeGameGradeScreen = g_tmCurrent + SEC( 3 );
	if( ::GetLanguage() == LANG_KOR && pWndWorld == NULL && g_tmCurrent <= dwTimeGameGradeScreen )
	{
#ifdef __AEGON_ADVANCED_LOGIN
		CWndLogin* pWndLogin = (CWndLogin*)g_WndMng.GetWndBase(APP_AEGON_LOGIN);
#else // __AEGON_ADVANCED_LOGIN
		CWndLogin* pWndLogin = (CWndLogin*)g_WndMng.GetWndBase(APP_LOGIN);
#endif // __AEGON_ADVANCED_LOGIN
		
		if( pWndLogin )
		{
			pWndLogin->SetVisible( FALSE );
			pWndLogin->EnableWindow( FALSE );
		}

		m_pTheme->RenderGameGradeScreen( &g_Neuz.m_2DRender );
	}
	else if( pWndWorld == NULL && m_bTitle )
	{
#ifdef __AEGON_ADVANCED_LOGIN
		CWndLogin* pWndLogin = (CWndLogin*)g_WndMng.GetWndBase(APP_AEGON_LOGIN);
#else // __AEGON_ADVANCED_LOGIN
		CWndLogin* pWndLogin = (CWndLogin*)g_WndMng.GetWndBase(APP_LOGIN);
#endif // __AEGON_ADVANCED_LOGIN
		
		if( pWndLogin )
		{
			pWndLogin->SetVisible( TRUE );
			pWndLogin->EnableWindow( TRUE );
		}

		if( m_pWndActiveDesktop )
		{
			m_pWndActiveDesktop->SetVisible( TRUE );
		}
		else
		{
			m_pTheme->RenderDesktop( &g_Neuz.m_2DRender );
		}
	}
#else // __GAME_GRADE_SYSTEM
	// 월드가 없거나 풀화면이 아닐 때
	if( pWndWorld == NULL && m_bTitle ) //|| pWndWorld->m_nWinSize != WSIZE_MAX )
	{
		if( m_pWndActiveDesktop )
			m_pWndActiveDesktop->SetVisible( TRUE );
		//if( m_bTitle == FALSE )
		else
			m_pTheme->RenderDesktop( &g_Neuz.m_2DRender );
	}
#endif // __GAME_GRADE_SYSTEM
	else
	if( pWndWorld )
	{
		//DWORD dwColor = CWorld::GetDiffuseColor();
		//m_pApp->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, dwColor /*D3DCOLOR_ARGB( 255, 255, 255, 255 )*/, 1.0f, 0 ) ;

		//if( pWndWorld->m_nWinSize != WSIZE_MAX )
		m_pApp->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, 0, 0, 0 ), 1.0f, 0 ) ;
		// 월드가 화면을 완전히 가리고 있을 때 
		if( m_pWndActiveDesktop )
			m_pWndActiveDesktop->SetVisible( FALSE );
	}
	else
		m_pApp->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, 0, 0, 0 ), 1.0f, 0 ) ;
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	return TRUE;
}
BOOL CWndMgr::Process()
{
	_PROFILE("CWndMgr::Process()");

#ifdef __MAIL_REQUESTING_BOX
	static DWORD dwTargetTime = g_tmCurrent + 5000;
	if( m_bWaitRequestMail && dwTargetTime < g_tmCurrent )		//메일이 있다고 받고, 메일정보를 받지 못한채 5초가 흐름.
	{
		g_DPlay.SendQueryMailBox();
		dwTargetTime = g_tmCurrent + 5000;
	}
#endif //__MAIL_REQUESTING_BOX

#ifdef __BS_CONSOLE
		_update_console( );
#endif

	CWndRegVend* pWndRegVend = (CWndRegVend*)g_WndMng.GetWndBase(APP_VENDOREX_SELL);
	CWndRegVend* pWndVendorBuy = (CWndRegVend*)g_WndMng.GetWndBase(APP_VENDOREX_BUY);
	
	if( pWndRegVend || pWndVendorBuy )
	{
		return CWndBase::Process();
	}
	
	if( g_pPlayer )
	{
#ifdef __GAMEGUARD
		if (g_pPlayer && g_pPlayer->IsDie())
		{
			if (m_pWndRevival == NULL)
			{
				m_pWndRevival = new CWndRevival;
				m_pWndRevival->Initialize(this, 0);
			}
		}
#endif // __GAMEGUARD
		if( g_bKeyTable[ VK_ESCAPE ] && m_pLogOutWaitting == NULL )
		{
			//TRACE( "Push ESCAPE \n" );
			// 부활 매뉴 
			g_bKeyTable[ VK_ESCAPE ] = 0;

			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
			CWorldMap* pWorldMap = CWorldMap::GetInstance();
			
			if( pWndWorld && pWndWorld->IsViewMap() && pWorldMap )
			{
				pWndWorld->SetViewMap(FALSE);
				pWorldMap->DeleteWorldMap();
				return CWndBase::Process();
			}
		
			if( g_pPlayer->IsDie() )
			{
				if( m_pWndRevival == NULL )
				{
					m_pWndRevival = new CWndRevival;
					m_pWndRevival->Initialize( this, 0 );
				}
			}
			else
			// 태스크 매뉴 
			if( m_pWndMenu == NULL || m_pWndMenu->IsVisible() == FALSE )
			{
				CWorld* pWorld = g_WorldMng();
				CObj* pObj = pWorld->GetObjFocus();

#if __VER >= 11 // __CSC_VER11_2
				CWndWorld* pWndWorld = g_WndMng.m_pWndWorld;
				if(pWndWorld && pWndWorld->m_pNextTargetObj)
				{
					pWndWorld->m_pNextTargetObj = NULL;
					CMover* pMover = (CMover*)pObj;
					if(pMover->IsPeaceful() == FALSE)
						pWndWorld->m_pRenderTargetObj = pObj;
				}
				else if( pObj )
#else //__CSC_VER11_2
				if( pObj )
#endif //__CSC_VER11_2
				{
					{
#if __VER >= 10 // __LEGEND
						DWORD	dwTmpTick = GetTickCount();
						int	nTmpSkillID = g_pPlayer->m_pActMover->GetCastingSKillID();
						if( g_pPlayer->m_pActMover->GetCastingEndTick() > dwTmpTick && ( nTmpSkillID == SI_KNT_HERO_DRAWING 
#ifndef __AEGON_RM_HERO_SKILL
							|| nTmpSkillID == SI_RIG_HERO_RETURN 
#endif
							) )
						{
							g_pPlayer->m_pActMover->SetCastingEndTick(0);
							g_pPlayer->m_pActMover->ClearState();				// 상태 클리어하고 다시 맞춤.
						}
#endif  //#if __VER >= 10 // __LEGEND

						pWorld->SetObjFocus( NULL );
#if __VER >= 11 // __CSC_VER11_2
						if(pWndWorld)
							pWndWorld->m_pRenderTargetObj = NULL;
#endif //__CSC_VER11_2
						pWorld->m_idObjFocusOld = NULL_ID;
						g_pPlayer->ClearDest();
#if __VER >= 11 // __CSC_VER11_2
						if(pWndWorld)
							pWndWorld->m_bAutoAttack = FALSE;
#endif //__CSC_VER11_2						
						if( g_pPlayer->m_dwFlag & MVRF_TRACKING )	// 이미 실행중이면 해제.
						{
							g_pPlayer->m_dwFlag &= (~MVRF_TRACKING);		// 추적모드해제.
							g_pPlayer->m_idTracking = NULL_ID;
						}
					}
				}
#if __VER >= 13 // __QUEST_HELPER
				else if(pWndWorld->m_bSetQuestNPCDest)
					pWndWorld->m_bSetQuestNPCDest = FALSE;
#endif //__QUEST_HELPER
				else
				{
#if __VER >= 12 // __ITEMCREATEMON_S0602
					if( g_Neuz.m_pCreateMonItem )
					{
						g_Neuz.m_pCreateMonItem = NULL;
					}
					else
					{
#ifdef __VENDING_NPC
						if (g_Neuz.m_pVendingNpcItem)
						{
							g_Neuz.m_pVendingNpcItem = NULL;
						}
						else
						{
#endif // __VENDING_NPC
#endif // __ITEMCREATEMON_S0602
					if( CWndBase::m_GlobalShortcut.IsEmpty() )
					{
						CWndBase* pWndBase = g_WndMng.GetWndBase();
						
						if( pWndBase )
						{
							LPWNDAPPLET lpWndApplet = pWndBase->GetWndApplet();
							if(pWndBase == &g_WndMng.m_pWndWorld->m_wndMenuMover || pWndBase->GetWndId() == 10)
								g_WndMng.m_pWndWorld->SetFocus();
							else if(lpWndApplet != NULL)
#ifdef __EVE_MINIGAME
							{
#if __VER >= 11 // __SYS_COLLECTING
#if __VER >= 13 // __RAINBOW_RACE
								if(lpWndApplet->dwWndId != APP_MINIGAME_KAWIBAWIBO && lpWndApplet->dwWndId != APP_MINIGAME_KAWIBAWIBO_WIN
									&& lpWndApplet->dwWndId != APP_MINIGAME_DICE && lpWndApplet->dwWndId != APP_COLLECTING
									&& lpWndApplet->dwWndId != APP_RR_MINIGAME_KAWIBAWIBO && lpWndApplet->dwWndId != APP_RR_MINIGAME_DICE
									&& lpWndApplet->dwWndId != APP_RR_MINIGAME_ARITHMETIC && lpWndApplet->dwWndId != APP_RR_MINIGAME_STOPWATCH
									&& lpWndApplet->dwWndId != APP_RR_MINIGAME_TYPING && lpWndApplet->dwWndId != APP_RR_MINIGAME_CARD
									&& lpWndApplet->dwWndId != APP_RR_MINIGAME_LADDER)
#else //__RAINBOW_RACE
								if(pWndBase->GetWndApplet()->dwWndId != APP_MINIGAME_KAWIBAWIBO && pWndBase->GetWndApplet()->dwWndId != APP_MINIGAME_KAWIBAWIBO_WIN
									&& pWndBase->GetWndApplet()->dwWndId != APP_MINIGAME_DICE && pWndBase->GetWndApplet()->dwWndId != APP_COLLECTING)
#endif //__RAINBOW_RACE
#else
								if(pWndBase->GetWndApplet()->dwWndId != APP_MINIGAME_KAWIBAWIBO && pWndBase->GetWndApplet()->dwWndId != APP_MINIGAME_KAWIBAWIBO_WIN
									&& pWndBase->GetWndApplet()->dwWndId != APP_MINIGAME_DICE )
#endif

#if __VER >= 15 // __GUILD_HOUSE
									if (pWndBase->GetWndApplet()->dwWndId != APP_GH_MAIN
#ifdef __MIKSIK_EQUALIZED_MODE
										&& pWndBase->GetWndApplet()->dwWndId != APP_TMP_CLASS
#endif //__MIKSIK_EQUALIZED_MODE
										)		
#endif //__GUILD_HOUSE
									pWndBase->Destroy();
							}
#else
								pWndBase->Destroy();
#endif
						}
						else
						{
							g_WndMng.OpenMenu();
							CRect rect = g_Neuz.GetDeviceRect();
							if( g_WndMng.m_pWndMenu )
								g_WndMng.m_pWndMenu->Move( CPoint( 0, rect.bottom - 48 - g_WndMng.m_pWndMenu->GetWndRect().Height() ) ); //48->TaskBar Height
						}
					}
#if __VER >= 12 // __ITEMCREATEMON_S0602
					}
#endif // __ITEMCREATEMON_S0602
#ifdef __VENDING_NPC
					}
#endif // __VENDING_NPC
				}
			}
#if __VER >= 12 // __SECRET_ROOM
			if(m_wndRemove.GetSize() > 0)
			{
				CWndBase* pWndBase = (CWndBase*)m_wndRemove.GetAt(0);

				if(pWndBase && pWndBase->GetWndApplet()->dwWndId == APP_SECRETROOM_GUILDMEMBER)
					pWndBase->Destroy(TRUE);
			}
#endif //__SECRET_ROOM
		}
		if( m_szTimerChat[ 0 ] && m_timerChat.IsTimeOut() )
		{
			g_DPlay.SendChat( m_szTimerChat );
			m_szTimerChat[ 0 ] = 0;
			m_timerChat.Set( 0 );
		}

		if( m_bConnect && m_timerMessenger.IsTimeOut() )
			m_timerMessenger.Set( MIN( 5 ) );
#if __VER >= 12 // __SECRET_ROOM
		if( g_pPlayer && CSecretRoomMng::GetInstance()->m_nState >= SRCONT_WAR )
		{
			if( CSecretRoomMng::GetInstance()->IsInTheSecretRoom( g_pPlayer ) )
			{
				if(m_pWndSecretRoomQuick == NULL)
				{
					//Create GuildMemberMng Window
					int nGuildIndex = CSecretRoomMng::GetInstance()->m_pSRCont->GetTenderGuild(g_pPlayer->m_idGuild);
					if(nGuildIndex != NULL_ID)
					{
						int nGuildMemberCount = CSecretRoomMng::GetInstance()->m_pSRCont->m_vecSecretRoomTender[nGuildIndex].vecLineUpMember.size();
						SAFE_DELETE(m_pWndSecretRoomQuick);
						m_pWndSecretRoomQuick = new CWndSecretRoomQuick;
						m_pWndSecretRoomQuick->SetGuildMemCount(nGuildMemberCount);

						for(int k=0; k<nGuildMemberCount; k++)
							m_pWndSecretRoomQuick->SetGuildMember(CSecretRoomMng::GetInstance()->m_pSRCont->m_vecSecretRoomTender[nGuildIndex].vecLineUpMember[k]);

						m_pWndSecretRoomQuick->Initialize(NULL);
					}
				}
			}
		}
		
		if( !CSecretRoomMng::GetInstance()->IsInTheSecretRoom( g_pPlayer ) )
		{
			if(m_pWndSecretRoomQuick)
				SAFE_DELETE(m_pWndSecretRoomQuick);
		}
#endif //__SECRET_ROOM
	}


	if( m_pLogOutWaitting ) // 종료진행중이면 테스크바 슬롯체인지 불가능
		return CWndBase::Process();


	if( m_pWndTaskBar )
	{
//		if( g_bKeyTable[ VK_BACK ] )	// 지훈씨가 빼라고 했음.
//		{
//			g_bKeyTable[ VK_BACK ] = 0;
//			g_Option.m_nInterface = !g_Option.m_nInterface;
//		}
#ifdef __Y_INTERFACE_VER3
		if( g_Option.m_nInterface == 2 )
		{
			if( g_bKeyTable[ VK_F1 ] )
			{
				g_bKeyTable[ VK_F1 ] = 0;
				m_pWndTaskBar->SetItemSlot( 0 );
			}
			if( g_bKeyTable[ VK_F2 ] )
			{
				g_bKeyTable[ VK_F2 ] = 0;
				m_pWndTaskBar->SetItemSlot( 1 );
			}
			if( g_bKeyTable[ VK_F3 ] )
			{
				g_bKeyTable[ VK_F3 ] = 0;
				m_pWndTaskBar->SetItemSlot( 2 );
			}
			if( g_bKeyTable[ VK_F4 ] )
			{
				g_bKeyTable[ VK_F4 ] = 0;
				m_pWndTaskBar->SetItemSlot( 3 );
			}
			if( g_bKeyTable[ VK_F1 ] )
			{
				g_bKeyTable[ VK_F1 ] = 0;
				ObjectExecutor( SHORTCUT_APPLET, APP_HELPER_HELP );
			}
			if( g_bKeyTable[ '0' ] )
			{
				g_bKeyTable[ '0' ] = 0;
				ScreenCapture();
			}
			if( g_bKeyTable[ VK_F9 ] )
			{
				g_bKeyTable[ VK_F9 ] = 0;
				FILEOUT( "Position.txt", "Pos x = %f, z = %f", g_pPlayer->GetPos().x, g_pPlayer->GetPos().z );
				PutString( prj.GetText(TID_ADMIN_COORDINATESAVE), NULL, prj.GetTextColor(TID_ADMIN_COORDINATESAVE) );
			}
		}
		else
#else //__Y_INTERFACE_VER3
		{
#ifdef __SINGLE_TASKBAR_SWAP
			if (g_Option.m_bTaskbarDisplay) {
				if (g_bKeyTable[VK_F1])
				{
					g_bKeyTable[VK_F1] = 0;
					m_pWndTaskBar->SetItemSlot(0);
				}
				if (g_bKeyTable[VK_F2])
				{
					g_bKeyTable[VK_F2] = 0;
					m_pWndTaskBar->SetItemSlot(1);
				}
				if (g_bKeyTable[VK_F3])
				{
					g_bKeyTable[VK_F3] = 0;
					m_pWndTaskBar->SetItemSlot(2);
				}
				if (g_bKeyTable[VK_F4])
				{
					g_bKeyTable[VK_F4] = 0;
					m_pWndTaskBar->SetItemSlot(3);
				}
				if (g_bKeyTable[VK_F5])
				{
					g_bKeyTable[VK_F5] = 0;
					m_pWndTaskBar->SetItemSlot(4);
				}
				if (g_bKeyTable[VK_F6])
				{
					g_bKeyTable[VK_F6] = 0;
					m_pWndTaskBar->SetItemSlot(5);
				}
				if (g_bKeyTable[VK_F7])
				{
					g_bKeyTable[VK_F7] = 0;
					m_pWndTaskBar->SetItemSlot(6);
				}
				if (g_bKeyTable[VK_F8])
				{
					g_bKeyTable[VK_F8] = 0;
					m_pWndTaskBar->SetItemSlot(7);
				}
				if (g_bKeyTable[VK_F1])
				{
					g_bKeyTable[VK_F1] = 0;
					ObjectExecutor(SHORTCUT_APPLET, APP_HELPER_HELP);
				}
				if (g_bKeyTable['0'])
				{
					g_bKeyTable['0'] = 0;
					ScreenCapture();
				}
				if (g_bKeyTable[VK_F9])
				{
					g_bKeyTable[VK_F9] = 0;
					FILEOUT("Position.txt", "Pos x = %f, z = %f", g_pPlayer->GetPos().x, g_pPlayer->GetPos().z);
					PutString(prj.GetText(TID_ADMIN_COORDINATESAVE), NULL, prj.GetTextColor(TID_ADMIN_COORDINATESAVE));
				}
			}
			else
			{
#endif // __SINGLE_TASKBAR_SWAP
				if( g_bKeyTable[ '1' ] )
				{
					g_bKeyTable[ '1' ] = 0;
					m_pWndTaskBar->SetItemSlot( 0 );
				}
				if( g_bKeyTable[ '2' ] )
				{
					g_bKeyTable[ '2' ] = 0;
					m_pWndTaskBar->SetItemSlot( 1 );
				}
				if( g_bKeyTable[ '3' ] )
				{
					g_bKeyTable[ '3' ] = 0;
					m_pWndTaskBar->SetItemSlot( 2 );
				}
				if( g_bKeyTable[ '4' ] )
				{
					g_bKeyTable[ '4' ] = 0;
					m_pWndTaskBar->SetItemSlot( 3 );
				}
#if __VER >= 11 // __CSC_VER11_5
				if( g_bKeyTable[ '5' ] )
				{
					g_bKeyTable[ '5' ] = 0;
					m_pWndTaskBar->SetItemSlot( 4 );
				}
				if( g_bKeyTable[ '6' ] )
				{
					g_bKeyTable[ '6' ] = 0;
					m_pWndTaskBar->SetItemSlot( 5 );
				}
				if( g_bKeyTable[ '7' ] )
				{
					g_bKeyTable[ '7' ] = 0;
					m_pWndTaskBar->SetItemSlot( 6 );
				}
				if( g_bKeyTable[ '8' ] )
				{
					g_bKeyTable[ '8' ] = 0;
					m_pWndTaskBar->SetItemSlot( 7 );
				}
#endif //__CSC_VER11_5
				if( g_bKeyTable[ '1' ] )
				{
					g_bKeyTable[ '1' ] = 0;
					ObjectExecutor( SHORTCUT_APPLET, APP_HELPER_HELP );
				}
				if( g_bKeyTable[ '0' ] )
				{
					g_bKeyTable[ '0' ] = 0;
					ScreenCapture();
				}
				if( g_bKeyTable[ '9' ] )
				{
					g_bKeyTable[ '9' ] = 0;
					FILEOUT( "Position.txt", "Pos x = %f, z = %f", g_pPlayer->GetPos().x, g_pPlayer->GetPos().z );
					PutString( prj.GetText(TID_ADMIN_COORDINATESAVE), NULL, prj.GetTextColor(TID_ADMIN_COORDINATESAVE) );
				}
#ifdef __SINGLE_TASKBAR_SWAP
			}
#endif // __SINGLE_TASKBAR_SWAP
		}
#endif //__Y_INTERFACE_VER3

#ifdef __NEW_WEB_BOX
		if( GetWndBase( APP_WEBBOX ) )
#else // __NEW_WEB_BOX
		if( GetWndBase(APP_WEBBOX) || GetWndBase(APP_WEBBOX2) )
#endif // __NEW_WEB_BOX
		{
			if( !CWebBox::GetInstance()->Get_Start() )
				CWebBox::GetInstance()->Set_Start();
		}
	}
#if __VER >= 8 //__CSC_VER8_2
	CWndBase* pWndPartyQuick = (CWndBase*)g_WndMng.GetApplet( APP_PARTY_QUICK );
	if(pWndPartyQuick)
	{
		if(g_Party.GetSizeofMember() < 2)
		{
			pWndPartyQuick->Destroy();
			SetFocus();
		}
	}		
#endif //__CSC_VER8_2

#ifdef __AEGON_FIXES
	if (m_bSetOtherNull)
	{
		if (IsValidObj(g_pPlayer))
		{
			g_pPlayer->m_vtInfo.SetOther(NULL);
			m_bSetOtherNull = FALSE;
		}
	}
#endif

	return CWndBase::Process();
}
void CWndMgr::SetPlayer( CMover* pMover )
{
	CWndBase* pWndBase;

	if( pMover != NULL )
	{
		if( pWndBase = GetApplet( APP_GAUGE ) )
		{
			pWndBase->SetTitle( pMover->GetName() );
			//( (CWndStatus*)pWndBase )->m_pModel	= prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, pMover->m_dwType, pMover->m_dwIndex, TRUE );
			//prj.m_modelMng.LoadMotion( ( (CWndStatus*)pWndBase )->m_pModel, pMover->m_dwType, pMover->m_dwIndex, 0 );
		}

#if __VER >= 10 // __CSC_VER9_1
#ifdef __NEW_SKILL_TREE
		if( pWndBase = GetApplet( APP_SKILL4 ) )
#else // __NEW_SKILL_TREE
		if( pWndBase = GetApplet( APP_SKILL3 ) )
#endif // __NEW_SKILL_TREE
		if( pWndBase = GetApplet( APP_SKILL1 ) )
#endif //__CSC_VER9_1
			( (CWndSkillTreeEx*)pWndBase )->InitItem( pMover->GetJob(), pMover->m_aJobSkill );
		
		if( pWndBase = GetApplet( APP_INVENTORY ) )
		{
			( (CWndInventory*)pWndBase )->m_dwEnchantWaitTime = 0xffffffff;	// 제련하는도중 replace하는 경우 neuz가 크래쉬 되는 현상 수정
			( (CWndInventory*)pWndBase )->m_wndItemCtrl.InitItem( &pMover->m_Inventory, APP_INVENTORY );
		}

#if __VER >= 11 // __SYS_POCKET
		if( pWndBase = GetApplet( APP_BAG_EX ) )
			( (CWndBagEx*)pWndBase )->InitItem();
#endif	// __SYS_POCKET

		g_Neuz.m_camera.SetPos( pMover->GetPos() );		//

		if( CMover::GetActiveMover() == NULL )
		{
			LPCTSTR szErr = Error( "SetPlayer : ActiveMover 없음" ); 
			ADDERRORMSG( szErr );
		}
		if( CMover::GetActiveMover()->m_pActMover == NULL )
		{
			LPCTSTR szErr = Error( "SetPlayer : ActionMover 없음" ); 
			ADDERRORMSG( szErr );
		}

		/*
		m_bLock = TRUE;
		CMover* pMoverTarget = (CMover*)g_WorldMng.Get()->GetObjFocus() ;
		if( !(pMoverTarget && CMover::GetActiveMover()->m_pActMover->IsFly()) ) {
			FLOAT fAngle = CMover::GetActiveMover()->GetAngle(); 
			m_fRotx=-fAngle;
		}
		*/
//		Error( "WorldName-%s, Targer-%s, ActMover-%s, ActionMover-%s\n"  )

		if( pMover->m_pActMover->IsFly() )
			g_Neuz.m_camera.Unlock();
		else
			g_Neuz.m_camera.Lock();

		// 월드가 가동하기 전에 프로젝션과 뷰 매트릭스를 만들어주어야 
		// 문제가 없다. CWndWorld는 매시지 처리기이기 때문에 미처 행렬들이
		// 세팅되기도 전에 우선된 매시지에서 행렬을 사용할 우려가 있다.(PickObject 따위들)
		//CWndWorld* pWndWorld = (CWndWorld*)GetWndBase( APP_WORLD );
		//pWndWorld->Projection( D3DDEVICE );
		g_Neuz.m_camera.Reset();
		g_Neuz.m_camera.Process( D3DDEVICE );
		g_Neuz.m_camera.Transform( D3DDEVICE, g_WorldMng.Get() );

#if __VER >= 15 // __GUILD_HOUSE
		//gmpbigsun: g_pPlayer가 이제 막 세팅됨, 길드하우스 
		// 클라 LandScape는 실제로 ReadWorld가 실행될때 값이 채워진다. ( 서버로 패킷수신시 업데이트 됨 ), 그전에 보이는것들에 대해 문제가 발생하므로
		// g_pPlayer가 생성될때 한번 업데이트 .
		CWorld* pWorld = g_WorldMng.Get();
		if( pWorld )
		{
			pWorld->ReadWorld( g_pPlayer->GetPos() );
			GuildHouse->ApplyEFTexture( ); //! 5년묶은 때를 벗기자 
		}
#endif

	}
	else 
	{
#if __VER >= 10 // __CSC_VER9_1
#ifdef __NEW_SKILL_TREE
		if( pWndBase = GetApplet( APP_SKILL4 ) )
#else // __NEW_SKILL_TREE
		if( pWndBase = GetApplet( APP_SKILL3 ) )
#endif // __NEW_SKILL_TREE
		if( pWndBase = GetApplet( APP_SKILL1 ) )
#endif //__CSC_VER9_1
			( (CWndSkillTreeEx*)pWndBase )->InitItem( 0, NULL );

		if( pWndBase = GetApplet( APP_GAUGE ) )
			SAFE_DELETE( ( (CWndStatus*)pWndBase )->m_pModel );

		if( pWndBase = GetApplet( APP_INVENTORY ) )
			( (CWndInventory*)pWndBase )->m_wndItemCtrl.InitItem( NULL, APP_INVENTORY );
	}
}
void CWndMgr::WordChange( CString& rString )
{
#ifdef __FILTER_0705
	CString strResult;

	CScanner s;
	s.SetProg( (LPVOID)(LPCSTR)rString );

	s.GetToken();
	BOOL bFirst	= ( *s.m_pProg == ' ' );

	while( s.tok != FINISHED )
	{
		CString str		= s.Token;
//		str.MakeLower();

		CScanner s1;
		s1.SetProg( (LPVOID)(LPCSTR)str );
		s1.GetToken();
		BOOL bSecond	= ( *s1.m_pProg == ' ' );
		BOOL bTerminate	= ( *s1.m_pProg == '\0' );

		while( s1.tok != FINISHED )
		{
			CString str1	= s1.Token;
			str1.MakeLower();

			map<string, string>::iterator it	= prj.m_mapAlphaFilter.find( (LPSTR)(LPCSTR)str1 );
			if( it != prj.m_mapAlphaFilter.end() )
			{
//				if( g_Option.m_nSlangWord == 1 ) 
				{
					TCHAR szWord[256]	= { 0,};
					memset( szWord, '*', lstrlen( str1 ) );
					strResult	+= szWord;
				}
//				else
//				{
//					strResult	+= it->second.data();
//				}
			}
			else
			{
				strResult	+= s1.Token;
			}
			if( ( bTerminate && bFirst ) ||
				( !bTerminate && bSecond ) )
				strResult	+= ' ';
//			strResult	+= ' ';
			s1.GetToken();
			bSecond	= ( *s1.m_pProg == ' ' );
			bTerminate	= ( *s1.m_pProg == '\0' );
		}
		s.GetToken();
		bFirst	= ( *s.m_pProg == ' ' );
	}

	for( map<string, string>::iterator it = prj.m_mapNonalphaFilter.begin(); it != prj.m_mapNonalphaFilter.end(); ++it )
	{
		CString str		= it->first.data();
		if( g_Option.m_nSlangWord == 1 ) 
		{
			TCHAR	szWord[256]	= { 0,};
			memset( szWord, '*', str.GetLength() );
			strResult.Replace( str, szWord );
		}
		else
		{
			strResult.Replace( str, it->second.data() );
		}
	}
	rString		= strResult;
#else	// __FILTER_0705
	for( int i = 0; i < prj.m_aWordFilter.GetSize(); i++)
	{
		LPFILTER lpFilter = prj.m_aWordFilter.GetAt( i );
		int nFind;
		if( ( nFind = rString.Find( lpFilter->m_szSrc ) ) >= 0 )
		{
			if( g_Option.m_nSlangWord == 1 ) 
			{
				TCHAR szWord[ 128 ];
				int nLength = strlen( lpFilter->m_szSrc );
				for( int i = 0; i < nLength; i++ )
					szWord[ i ] = '*';
				szWord[ i ] = 0;
				rString.Replace( lpFilter->m_szSrc,szWord );
			}
			else
			{
				rString.Replace( lpFilter->m_szSrc, lpFilter->m_szDst );
			}
		}
	}
#endif	// __FILTER_0705
}
// 챗에서 또는 매크로로 입력된 텍스트가 이 함수를 용도에 맞게 분류된다.
void CWndMgr::ParsingChat( CString string )
{
#if __VER >= 12 // __MOD_TUTORIAL
	CWndGuideSystem* pWndGuide = NULL;
	pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
	if(pWndGuide && pWndGuide->IsVisible()) pWndGuide->m_Condition.strInput.Format("%s", string); 
#endif
	string.TrimLeft( ' ' );
	if( string.IsEmpty() == TRUE )
		return;
	// 문장길이가 너무 길면 64바이트로 줄임.
	SetStrNull( string, 120 );

	if( string.Find("#l") != -1 )
	{
		int mCut = 0;
		mCut = string.Find('/');

		if( mCut != -1 )
		{
			string = string.Right( string.GetLength() - mCut );
		}
	}

	FormalizeChatString( string );

	if( string.GetLength() > 0 )
	{
		if( string[0] == '/' )
		{
			ParsingCommand( string.LockBuffer(), g_pPlayer );
			string.UnlockBuffer();
		}
		else 
		{
#ifdef __BAN_CHATTING_SYSTEM
			if( m_timerBanning.IsTimeOut() == FALSE )
			{
				CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
				if( pWndChat )
				{
					int nOriginalSecond = static_cast< int >( BANNING_MILLISECOND - static_cast< int >( m_timerBanning.GetLeftTime() ) ) / 1000;
					int nMinute = static_cast< int >( nOriginalSecond / 60 );
					int nSecond = static_cast< int >( nOriginalSecond % 60 );
					CString strMessage = _T( "" );
					// 현재 채팅 금지 페널티를 받고 있습니다. (남은 시간: %d분 %d초)
					strMessage.Format( prj.GetText( TID_GAME_ERROR_CHATTING_3 ), nMinute, nSecond );
					pWndChat->PutString( strMessage, COLOR_ERROR );
					return;
				}
			}
			else
			{
				if( string != m_strChatBackup || m_timerDobe.IsTimeOut() || g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
				{
					m_timerDobe.Reset();
					m_strChatBackup = string;
					WordChange( string );
					SetStrNull( string, 120 );
					if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
						g_DPlay.SendChat( ( LPCSTR )string );
					else
					{
						if( m_bShortcutCommand == TRUE )
						{
							if( m_timerShortcutWarning.IsTimeOut() == FALSE )
							{
								++m_nWarningCounter;
								CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
								if( pWndChat )
								{
									if( m_nWarningCounter >= BANNING_POINT )
									{
										// 과도한 채팅으로 인하여 %d분 동안 채팅 금지 페널티를 받으셨습니다.
										CString strChattingError1 = _T( "" );
										strChattingError1.Format( prj.GetText( TID_GAME_ERROR_CHATTING_2 ), BANNING_MILLISECOND / 1000 / 60 );
										pWndChat->PutString( strChattingError1, prj.GetTextColor( TID_GAME_ERROR_CHATTING_2 ) );
										m_nWarningCounter = 0;
										m_timerBanning.Reset();
									}
									else
									{
										// 연속 채팅으로 인하여 메시지가 출력되지 않았습니다.
										pWndChat->PutString( prj.GetText( TID_GAME_ERROR_CHATTING_1 ), COLOR_ERROR );
									}
								}
							}
							else
								g_DPlay.SendChat( ( LPCSTR )string );
							m_timerShortcutWarning.Reset();
						}
						else
						{
							if( m_timerWarning.IsTimeOut() == FALSE )
							{
								++m_nWarningCounter;
								CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
								if( pWndChat )
								{
									if( m_nWarningCounter >= BANNING_POINT )
									{
										// 과도한 채팅으로 인하여 %d분 동안 채팅 금지 페널티를 받으셨습니다.
										CString strChattingError1 = _T( "" );
										strChattingError1.Format( prj.GetText( TID_GAME_ERROR_CHATTING_2 ), BANNING_MILLISECOND / 1000 / 60 );
										pWndChat->PutString( strChattingError1, prj.GetTextColor( TID_GAME_ERROR_CHATTING_2 ) );
										m_nWarningCounter = 0;
										m_timerBanning.Reset();
									}
									else
									{
										// 연속 채팅으로 인하여 메시지가 출력되지 않았습니다.
										pWndChat->PutString( prj.GetText( TID_GAME_ERROR_CHATTING_1 ), COLOR_ERROR );
									}
								}
							}
							else
							{
								if( m_timerWarning2.IsTimeOut() == FALSE )
								{
									++m_nWarning2Counter;
									CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
									if( pWndChat )
									{
										if( m_nWarning2Counter >= BANNING_2_POINT )
										{
											// 과도한 채팅으로 인하여 %d분 동안 채팅 금지 페널티를 받으셨습니다.
											CString strChattingError1 = _T( "" );
											strChattingError1.Format( prj.GetText( TID_GAME_ERROR_CHATTING_2 ), BANNING_MILLISECOND / 1000 / 60 );
											pWndChat->PutString( strChattingError1, prj.GetTextColor( TID_GAME_ERROR_CHATTING_2 ) );
											m_nWarning2Counter = 0;
											m_timerBanning.Reset();
										}
										else
											g_DPlay.SendChat( ( LPCSTR )string );
									}
								}
								else
									g_DPlay.SendChat( ( LPCSTR )string );
							}
							m_timerWarning.Reset();
							m_timerWarning2.Reset();
						}
					}
				}
				else
				{
					CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
					if( pWndChat )
					{
						// 연속으로 같은 내용을 입력할 수 없습니다.
						pWndChat->PutString( prj.GetText( TID_GAME_CHATSAMETEXT ), prj.GetTextColor( TID_GAME_CHATSAMETEXT ) );
					}
				}
			}
#else // __BAN_CHATTING_SYSTEM
			// 문자중에 /가 포함되어 있으면 영자 명령일 수 있다.
			// 영자 명령의 누수를 막기 위해 /가 포함되어 있으면 전송하지 않는다.
			if( string != m_strChatBackup || m_timerDobe.IsTimeOut() || g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ))
			{
				m_timerDobe.Reset();
				m_strChatBackup = string;
				WordChange( string );
				// 문장길이가 너무 길면 64바이트로 줄임.
				SetStrNull( string, 120 );

				g_DPlay.SendChat( (LPCSTR)string );
			}
			else
			{
				CWndChat* pWndChat = (CWndChat*)g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
				if( pWndChat )
					pWndChat->PutString( prj.GetText(TID_GAME_CHATSAMETEXT), prj.GetTextColor(TID_GAME_CHATSAMETEXT) );
					//pWndChat->PutString( "연속으로 같은 내용을 입력할 수 없습니다.", 0xffffffa0 );
			}
#endif // __BAN_CHATTING_SYSTEM
#ifdef __BAN_CHATTING_SYSTEM
			m_bShortcutCommand = FALSE;
#endif // __BAN_CHATTING_SYSTEM
		}
	}
}
void CWndMgr::PutDefinedString( DWORD dwText, ... )
{
	TCHAR szBuffer[ 512 ];
	TCHAR szFormat[ 512 ];
	strcpy( szFormat, prj.GetText( dwText ) );
	va_list args;
	va_start( args, szFormat );
	int nBuf = _vsntprintf( szBuffer, 512, szFormat, args );
	va_end( args );

	DWORD dwColor	= prj.GetTextColor( dwText );
	g_WndMng.PutString( (LPCTSTR)szBuffer, NULL, dwColor );
}

void CWndMgr::PutString( LPCTSTR lpszString, CObj* pObj, DWORD dwColor, DWORD dwChatStyle, DWORD dwPStyle )
{
	CMover* pMover = (CMover*)pObj;
	CString str = lpszString;
	DWORD dwStyle = 0;
//	BOOL bChat = FALSE;

	dwPStyle = PS_USE_MACRO;
	
	// Text Message Style 
	#define TMS_CLIENT   0x00000001
	#define TMS_DIALOG   0x00000002
	#define TMS_CAPTION  0x00000004
	#define TMS_CHAT     0x00000008

	int nChannel = 0;
	switch( CWndChat::m_nChatChannel )
	{
		case CHATSTY_GENERAL: nChannel = 0; break;
		case CHATSTY_WHISPER: nChannel = 1; break;
		case CHATSTY_SHOUT  : nChannel = 2; break;
		case CHATSTY_PARTY  : nChannel = 3; break;
		case CHATSTY_GUILD  : nChannel = 4; break;
	}
	BOOL  bChatLog = TRUE;
	DWORD dwChatFilter = g_Option.m_dwChatFilter[ nChannel ];
	if( ( dwChatFilter & CHATSTY_GENERAL       ) == dwChatStyle ) 
		{ dwStyle |= TMS_CHAT; bChatLog = FALSE; } 
	if( ( dwChatFilter & CHATSTY_WHISPER       ) == dwChatStyle ) 
		{ dwStyle |= TMS_CHAT; bChatLog = FALSE; } 
	if( ( dwChatFilter & CHATSTY_SHOUT         ) == dwChatStyle ) 
		{ dwStyle |= TMS_CHAT; bChatLog = FALSE; } 
	if( ( dwChatFilter & CHATSTY_PARTY         ) == dwChatStyle ) 
		{ dwStyle |= TMS_CHAT; bChatLog = FALSE; } 
	if( ( dwChatFilter & CHATSTY_GUILD         ) == dwChatStyle ) 
		{ dwStyle |= TMS_CHAT; bChatLog = FALSE; } 

	DWORD dwChatFilterEtc = g_Option.m_dwChatFilterEtc;
	if( ( dwChatFilterEtc & CHATSTY_SYSTEM_CLIENT ) && ( dwChatStyle == CHATSTY_SYSTEM ) ) 
	{ dwStyle |= TMS_CLIENT; dwChatStyle = CHATSTY_SYSTEM_CHAT; }
	if( ( dwChatFilterEtc & CHATSTY_SYSTEM_CHAT   ) && ( dwChatStyle == CHATSTY_SYSTEM ) ) 
	{ dwStyle |= TMS_CHAT  ; dwChatStyle = CHATSTY_SYSTEM_CHAT; }
	if( ( dwChatFilterEtc & CHATSTY_HELP_CLIENT   ) && ( dwChatStyle == CHATSTY_HELP   ) ) 
	{ dwStyle |= TMS_CLIENT; dwChatStyle = CHATSTY_HELP_CHAT  ; }   
	if( ( dwChatFilterEtc & CHATSTY_HELP_CHAT     ) && ( dwChatStyle == CHATSTY_HELP   ) ) 
	{ dwStyle |= TMS_CHAT  ; dwChatStyle = CHATSTY_HELP_CHAT  ; }  
	if( ( dwChatFilterEtc & CHATSTY_GAME_CLIENT   ) && ( dwChatStyle == CHATSTY_GAME   ) ) 
	{ dwStyle |= TMS_CLIENT; dwChatStyle = CHATSTY_GAME_CHAT  ; }  
	if( ( dwChatFilterEtc & CHATSTY_GAME_CHAT     ) && ( dwChatStyle == CHATSTY_GAME   ) ) 
	{ dwStyle |= TMS_CHAT  ; dwChatStyle = CHATSTY_GAME_CHAT  ; } 
	BOOL bRecord = TRUE; // 무조건 기록. 나중에 이 변수 없애자.

	CWndChat* pWndChat = (CWndChat*)g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
	CEditString* pChatString = &m_ChatString;

	//
	//  예외 상황 처리(가능상황에서 정해진것이 여기서 무효가 될 수도 있다.)
	//
	if( pObj == NULL ) // 오브젝트가 널이면 말풍선 금지 
		dwStyle &= ~TMS_DIALOG; 
	else
	if( pObj->GetType() == OT_MOVER )
	{
		if( dwChatStyle & CHATSTY_GENERAL )
			dwStyle |= TMS_DIALOG;
		if( ( (CMover*)pObj )->IsPlayer() == FALSE ) // 플래이어가 아니라면 챗 금지 
		{
			dwStyle &= ~TMS_CHAT;
			bRecord = FALSE;
		}
		if( ( (CMover*)pObj )->m_vtInfo.IsVendorOpen() ) // 개인 상점이면 말풍선 금지 
			dwStyle &= ~TMS_DIALOG;

		// chipi_080311 - 운영자인 경우 말풍선 금지
		if( ((CMover*)pObj)->m_dwAuthorization >= AUTH_GAMEMASTER && ((CMover*)pObj)->IsMode( TRANSPARENT_MODE ) )
			dwStyle &= ~TMS_DIALOG;
	}		

	if( dwStyle & TMS_CLIENT )
		g_ClientMsg.AddMessage( lpszString, dwColor );

	CString strChat	= lpszString;
	strChat.Replace( "\n", " " );
	strChat.Replace( '\n', ' ' );

	if( bRecord )
	{
		if( m_aChatString.GetSize() > 500 )
		{
			m_aChatString.RemoveAt( 0, 250 );
			m_aChatColor.RemoveAt( 0, 250 );
			m_aChatStyle.RemoveAt( 0, 250 );
		}
		
		m_aChatString.Add( strChat );
//		m_aChatString.Add( lpszString );
		m_aChatColor.Add( dwColor );
		m_aChatStyle.Add( dwChatStyle );
	}
	if( dwStyle & TMS_CHAT )
	{
		if( pWndChat )
			pWndChat->PutString( strChat, dwColor, dwPStyle );
	}		

	if( dwStyle & TMS_CAPTION )
	{
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
		if( ::GetLanguage() != LANG_JAP )
			g_Caption1.AddCaption( lpszString, pWndWorld->m_pFontAPITitle );
		else
			g_Caption1.AddCaption( lpszString, NULL );		
	}

	if( dwStyle & TMS_DIALOG )
	{
#ifdef __HIDE_NPC_BUBBLES
		if (g_Option.m_nNPCBubbles == TRUE)
		{
			if (pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPlayer())
			{
				TCHAR* lpszChat = (TCHAR*)_tcschr(lpszString, _T(':'));
				lpszChat += sizeof(TCHAR) * 2;
				g_DialogMsg.AddMessage(pObj, lpszChat, 0xffffffff, 0, dwPStyle);
			}
		}
		else // Show All Bubbles
		{
			if (pObj->GetType() == OT_MOVER)
			{
				TCHAR* lpszChat = (TCHAR*)_tcschr(lpszString, _T(':'));
				lpszChat += sizeof(TCHAR) * 2;	// Skip ": "
				g_DialogMsg.AddMessage(pObj, lpszChat, 0xffffffff, 0, dwPStyle);
			}
		}
#else //__HIDE_NPC_BUBBLES
		if (pObj->GetType() == OT_MOVER)
		{
			TCHAR* lpszChat = (TCHAR*)_tcschr(lpszString, _T(':'));
			lpszChat += sizeof(TCHAR) * 2;	// Skip ": "
			g_DialogMsg.AddMessage(pObj, lpszChat, 0xffffffff, 0, dwPStyle);
		}
#endif //__HIDE_NPC_BUBBLES
	}

#if __VER >= 8 //__Y_CHAT_SYSTEM_8
	if( m_pWndChatLog )
	{
		if( bChatLog && !(dwStyle&TMS_DIALOG) && dwChatStyle != CHATSTY_SHOUT ) //외치기, NPC말풍선 내용 System Chat창에 들어오지 않게 막자.
			m_pWndChatLog->PutString( lpszString, dwColor, dwPStyle );
	}
#endif //__Y_CHAT_SYSTEM_8
}

void CWndMgr::PutToolTip_Skill( DWORD dwSkill, DWORD dwLevel, CPoint point, CRect* pRect, BOOL bActive )
{
#ifdef __AEGON_THEME_SWITCHER
	ItemProp* pSkillProp = prj.GetSkillProp(dwSkill);
	if (pSkillProp == NULL)
	{
		Error("CWndMgr::PutToolTip_Skill : 스킬(%d)의 프로퍼티가 업다.", dwSkill);
		return;	// property not found
	}

	DWORD dwSkillLevel = dwLevel;
#ifdef __SKILL0517
	LPSKILL pSkill = g_pPlayer->GetSkill(dwSkill);
	if (pSkill)
		dwSkillLevel = g_pPlayer->GetSkillLevel(pSkill);
#endif	// __SKILL0517

	AddSkillProp* pAddSkillProp = prj.GetAddSkillProp(pSkillProp->dwSubDefine, dwSkillLevel);
	if (pAddSkillProp == NULL)
	{
		Error("CWndMgr::PutToolTip_Skill : add스킬(%d)의 프로퍼티가 업다.", dwSkill);
		return;	// property not found
	}

	DWORD dwaf = D3DCOLOR_XRGB(199, 155, 0);
	CString str;
	CString strstr;
	CString string;
	CEditString strEdit;
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();

	if (bActive) 
	{
		string.Format("%s - Lvl %d", pSkillProp->szName, dwSkillLevel);
	}
	else 
	{
		string.Format("%s", pSkillProp->szName);
	}
	strEdit.SetParsingString(string, 0xcff2fbe6d);
	string = "";

	
	char statebuf[30] = { 0, };
	if (pAddSkillProp->nReqMp != 0xffffffff)
	{
		sprintf(statebuf, "\nMP : %d", g_pPlayer->GetReqMp(pAddSkillProp->nReqMp));
		string += statebuf;
	}
	if (pAddSkillProp->nReqFp != 0xffffffff)
	{
		sprintf(statebuf, "\nFP : %d", g_pPlayer->GetReqFp(pAddSkillProp->nReqFp));
		string += statebuf;
	}

	//	strstr.Format( prj.GetText(TID_GAME_BASESKILLLEVEL), pSkillProp->dwReqDisLV );
	//	if( g_pPlayer->GetLevel() < pSkillProp->dwReqDisLV )
	//		str.Format( "\n#cffff0000%s#nc", strstr );
	//	else
	//		str.Format( "\n%s", strstr );
	//	string += str;

	if (pSkillProp->dwReSkill1 != -1)
	{
		LPSKILL pskill = g_pPlayer->GetSkill(pSkillProp->dwReSkill1);

		if (pskill)
		{
			strstr.Format(prj.GetText(TID_GAME_SKILLDIS), pskill->GetProp()->szName, pSkillProp->dwReSkillLevel1);
			if (pskill->dwLevel < pSkillProp->dwReSkillLevel1)
				str.Format("\n%s", strstr.GetString());
			else
				str.Format("\n%s", strstr.GetString());
			string += str;
		}
	}
	if (pSkillProp->dwReSkill2 != -1)
	{
		LPSKILL pskill = g_pPlayer->GetSkill(pSkillProp->dwReSkill2);

		if (pskill)
		{
			strstr.Format(prj.GetText(TID_GAME_SKILLDIS), pskill->GetProp()->szName, pSkillProp->dwReSkillLevel2);
			if (pskill->dwLevel < pSkillProp->dwReSkillLevel2)
				str.Format("\n%s", strstr.GetString());
			else
				str.Format("\n%s", strstr.GetString());
			string += str;
		}
	}
	

	strstr.Format(prj.GetText(TID_GAME_BASESKILLLEVEL), pSkillProp->dwReqDisLV);
#ifdef __3RD_LEGEND16
	if (g_pPlayer->GetLevel() < (int)(pSkillProp->dwReqDisLV) && !g_pPlayer->IsMaster() && !g_pPlayer->IsHero() && !g_pPlayer->IsLegendHero())
#else // __3RD_LEGEND16
#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
	if (g_pPlayer->GetLevel() < (int)(pSkillProp->dwReqDisLV) && !g_pPlayer->IsMaster() && !g_pPlayer->IsHero())
#else //__LEGEND	//	9차 전승시스템	Neuz, World, Trans
	if (g_pPlayer->GetLevel() < (int)(pSkillProp->dwReqDisLV))
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans
#endif // __3RD_LEGEND16
		str.Format("\n%s", strstr.GetString());
	else
		str.Format("\n%s", strstr.GetString());
	string += str;

	if (strcmp(pSkillProp->szCommand, "="))
	{
		string += "\n";
		string += pSkillProp->szCommand;
	}

	int nSkillIdx = g_pPlayer->GetSkillIdx(dwSkill);

	if (nSkillIdx != -1)
	{
		DWORD dwDelay = g_pPlayer->GetReuseDelay(nSkillIdx);
		if (dwDelay > 0)
		{
			CTimeSpan ct((long)(dwDelay / 1000.0f));		// 남은시간을 초단위로 변환해서 넘겨줌
			str.Format(prj.GetText(TID_TOOLTIP_COOLTIME), ct.GetMinutes(), ct.GetSeconds());		// 남은시간을 분/초 형태로 출력.
			string += "\n";
			string += str;
		}
	}

	
	strEdit.AddParsingString(string, dwColorText);

	g_toolTip.PutToolTip(dwSkill, strEdit, *pRect, point, 0);
#else 
	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
	if( pSkillProp == NULL )
	{
		Error( "CWndMgr::PutToolTip_Skill : 스킬(%d)의 프로퍼티가 업다.", dwSkill );
		return;	// property not found
	}

	DWORD dwSkillLevel	= dwLevel;
#ifdef __SKILL0517
	LPSKILL pSkill		= g_pPlayer->GetSkill( dwSkill );
	if( pSkill )
		dwSkillLevel	= g_pPlayer->GetSkillLevel( pSkill );
#endif	// __SKILL0517

	AddSkillProp* pAddSkillProp		= prj.GetAddSkillProp( pSkillProp->dwSubDefine, dwSkillLevel );
	if( pAddSkillProp == NULL )
	{
		Error( "CWndMgr::PutToolTip_Skill : add스킬(%d)의 프로퍼티가 업다.", dwSkill );
		return;	// property not found
	}

	DWORD dwaf = D3DCOLOR_XRGB( 199, 155,   0 );
	CString str;
	CString strstr;
	CString string;
	if( bActive )
		string.Format( "#b#cff2fbe6d%s - Lvl %d#nb#nc", pSkillProp->szName, dwSkillLevel );
	else
		string.Format( "#b#cff2fbe6d%s#nb#nc", pSkillProp->szName );
	char statebuf[30] = {0,};
	if( pAddSkillProp->nReqMp != 0xffffffff )	
	{
		sprintf( statebuf, "\nMP : %d", g_pPlayer->GetReqMp( pAddSkillProp->nReqMp ) );
		string += statebuf;
	}
	if( pAddSkillProp->nReqFp != 0xffffffff )	
	{
		sprintf( statebuf, "\nFP : %d", g_pPlayer->GetReqFp( pAddSkillProp->nReqFp ) );
		string += statebuf;
	}
	
	
//	strstr.Format( prj.GetText(TID_GAME_BASESKILLLEVEL), pSkillProp->dwReqDisLV );
//	if( g_pPlayer->GetLevel() < pSkillProp->dwReqDisLV )
//		str.Format( "\n#cffff0000%s#nc", strstr );
//	else
//		str.Format( "\n%s", strstr );
//	string += str;

	if( pSkillProp->dwReSkill1 != -1 )
	{
		LPSKILL pskill = g_pPlayer->GetSkill(pSkillProp->dwReSkill1);

		if( pskill )
		{
			strstr.Format( prj.GetText(TID_GAME_SKILLDIS), pskill->GetProp()->szName, pSkillProp->dwReSkillLevel1 );
			if( pskill->dwLevel < pSkillProp->dwReSkillLevel1 )
				str.Format( "\n#cffff0000%s#nc", strstr );
			else
				str.Format( "\n%s#nc", strstr );
			string += str;
		}
	}
	if( pSkillProp->dwReSkill2 != -1 )
	{
		LPSKILL pskill = g_pPlayer->GetSkill(pSkillProp->dwReSkill2);
		
		if( pskill )
		{
			strstr.Format( prj.GetText(TID_GAME_SKILLDIS), pskill->GetProp()->szName, pSkillProp->dwReSkillLevel2 );
			if( pskill->dwLevel < pSkillProp->dwReSkillLevel2 )
				str.Format( "\n#cffff0000%s#nc", strstr );
			else
				str.Format( "\n%s#nc", strstr );
			string += str;
		}
	}

	strstr.Format( prj.GetText(TID_GAME_BASESKILLLEVEL), pSkillProp->dwReqDisLV );
#ifdef __3RD_LEGEND16
	if( g_pPlayer->GetLevel() < (int)( pSkillProp->dwReqDisLV ) && !g_pPlayer->IsMaster() && !g_pPlayer->IsHero() && !g_pPlayer->IsLegendHero() )
#else // __3RD_LEGEND16
#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
	if( g_pPlayer->GetLevel() < (int)( pSkillProp->dwReqDisLV ) && !g_pPlayer->IsMaster() && !g_pPlayer->IsHero() )
#else //__LEGEND	//	9차 전승시스템	Neuz, World, Trans
	if( g_pPlayer->GetLevel() < (int)( pSkillProp->dwReqDisLV ) )
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans
#endif // __3RD_LEGEND16
		str.Format( "\n#cffff0000%s#nc", strstr );
	else
		str.Format( "\n%s", strstr );
	string += str;

	if( strcmp( pSkillProp->szCommand, "=" ) )
	{
		string += "\n";
		string += pSkillProp->szCommand;
	}
	
	int nSkillIdx = g_pPlayer->GetSkillIdx( dwSkill );

	if( nSkillIdx != -1 )
	{
		DWORD dwDelay = g_pPlayer->GetReuseDelay( nSkillIdx );
		if( dwDelay > 0 )
		{
			CTimeSpan ct( (long)(dwDelay / 1000.0f) );		// 남은시간을 초단위로 변환해서 넘겨줌
			str.Format( prj.GetText(TID_TOOLTIP_COOLTIME), ct.GetMinutes(), ct.GetSeconds() );		// 남은시간을 분/초 형태로 출력.
			string += "\n";
			string += str;
		}	
	}
	
	CEditString strEdit;
	strEdit.SetParsingString( string );
	
	g_toolTip.PutToolTip( dwSkill, strEdit, *pRect, point, 0 );
#endif
}
void CWndMgr::PutToolTip_Troupe( DWORD dwSkill, CPoint point, CRect* pRect )
{
	ItemProp* pSkillProp = prj.GetPartySkill( dwSkill );
	
	if( pSkillProp == NULL )
		return;

#ifdef __AEGON_THEME_SWITCHER
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
#endif

	CString string;
	string.Format( "#b#cff2fbe6d%s#nb#nc", pSkillProp->szName );

	//string = pSkillProp->szName;
	char statebuf[30] = {0,};
	if( pSkillProp->dwReqMp != 0xffffffff )	
	{
		sprintf( statebuf, "\nMP : %d", pSkillProp->dwReqMp );
		string += statebuf;
	}
	if( pSkillProp->dwReqFp != 0xffffffff )	
	{
		sprintf( statebuf, "\nFP : %d", pSkillProp->dwReqFp );
		string += statebuf;
	}
	if( strcmp( pSkillProp->szCommand, "=" ) )
	{
		CString str;
		string += "\n";
		string += pSkillProp->szCommand;
	}
	CEditString strEdit;
#ifdef __AEGON_THEME_SWITCHER
	strEdit.SetParsingString(string, dwColorText);
#else
	strEdit.SetParsingString( string );
#endif
	
	g_toolTip.PutToolTip( dwSkill, strEdit, *pRect, point, 0 );
}
void CWndMgr::PutToolTip_Item( DWORD dwType, DWORD dwId, CPoint point, CRect* pRect, int flag )
{
#ifdef __AEGON_FIXES
	if (IsInvalidObj(g_pPlayer))
		return;
#endif
	CItemBase* pItemBase = g_pPlayer->GetItemId( dwId );
	PutToolTip_Item( pItemBase, point, pRect, flag );
}

struct DST_STRING
{
	int nDstParam;		// ex) DST_STR
	int nNameIndex;
};

static DST_STRING g_DstString[] = 
{
	DST_STR                     , TID_TOOLTIP_STR, 
	DST_DEX                     , TID_TOOLTIP_DEX,
	DST_INT                     , TID_TOOLTIP_INT,
	DST_STA                     , TID_TOOLTIP_STA,
	DST_SPEED                   , TID_TOOLTIP_SPEED,
	DST_ABILITY_MIN             , TID_TOOLTIP_ABILITYMIN,
	DST_ABILITY_MAX             , TID_TOOLTIP_ABILITYMAX,
	DST_ATTACKSPEED             , TID_TOOLTIP_ATKSPEED,
	DST_ADJDEF                  , TID_TOOLTIP_DEFENCE,
	DST_RESIST_MAGIC            , TID_TOOLTIP_DEFMAGIC, 
	DST_RESIST_ELECTRICITY      , TID_TOOLTIP_DEFELECTRICITY,
	DST_RESIST_ALL              , TID_TOOLTIP_DEFALLELEMETAL,
	DST_RESIST_FIRE             , TID_TOOLTIP_DEFFIFE,
	DST_RESIST_WIND             , TID_TOOLTIP_DEFWIND,
	DST_RESIST_WATER            , TID_TOOLTIP_DEFWATER, 
	DST_RESIST_EARTH            , TID_TOOLTIP_DEFEARTH,
	DST_HP_MAX                  , TID_TOOLTIP_MAXHP,
	DST_MP_MAX                  , TID_TOOLTIP_MAXMP,
	DST_FP_MAX                  , TID_TOOLTIP_MAXFP,
	DST_HP                      , TID_TOOLTIP_HP,
	DST_MP                      , TID_TOOLTIP_MP,
	DST_FP                      , TID_TOOLTIP_FP,
	DST_HP_RECOVERY             , TID_TOOLTIP_HPRECOVERY,
	DST_MP_RECOVERY             , TID_TOOLTIP_MPRECOVERY,
	DST_FP_RECOVERY             , TID_TOOLTIP_FPRECOVERY,
	DST_HP_RECOVERY_RATE        , TID_TOOLTIP_HPRECOVERYRATE,
	DST_MP_RECOVERY_RATE        , TID_TOOLTIP_MPRECOVERYRATE,
	DST_FP_RECOVERY_RATE        , TID_TOOLTIP_FPRECOVERYRATE,
	DST_ALL_RECOVERY			, TID_TOOLTIP_ALL_RECOVERY,		
	DST_ALL_RECOVERY_RATE		, TID_TOOLTIP_ALL_RECOVERY_RATE,
	DST_KILL_HP					, TID_TOOLTIP_KILL_HP,		
	DST_KILL_MP					, TID_TOOLTIP_KILL_MP,					
	DST_KILL_FP					, TID_TOOLTIP_KILL_FP,					
	DST_KILL_ALL				, TID_TOOLTIP_KILL_ALL,				
	DST_KILL_HP_RATE			, TID_TOOLTIP_KILL_HP_RATE,
	DST_KILL_MP_RATE			, TID_TOOLTIP_KILL_MP_RATE,			
	DST_KILL_FP_RATE			, TID_TOOLTIP_KILL_FP_RATE,			
	DST_KILL_ALL_RATE			, TID_TOOLTIP_KILL_ALL_RATE,			
	DST_ALL_DEC_RATE			, TID_TOOLTIP_ALL_DEC_RATE,
	DST_ADJ_HITRATE             , TID_TOOLTIP_HITRATE,
	DST_CHR_DMG                 , TID_TOOLTIP_CHRDMG,
	DST_CHRSTATE                , TID_TOOLTIP_CHRSTATE,
	DST_PARRY                   , TID_TOOLTIP_PARRY,
	DST_ATKPOWER_RATE           , TID_TOOLTIP_ATKPOWER,
	DST_JUMPING                 , TID_TOOLTIP_JUMPING,
	DST_BLOCK_MELEE				, TID_GAME_TOOLTIP_BLOCK,
	DST_BLOCK_RANGE				, TID_GAME_TOOLTIP_BLOCKRANGE,
	DST_STAT_ALLUP				, TID_GAME_TOOLTIPALLSTAT,
	DST_HP_MAX_RATE				, TID_TOOLTIP_DST_HP_MAX_RATE,
	DST_ADDMAGIC				, TID_GAME_TOOLTIPCONDITIONRATE,
	DST_ADJDEF_RATE				, TID_TOOLTIP_DST_ADJDEF_RATE,
	DST_HP_MAX_RATE				, TID_TOOLTIP_DST_HP_MAX_RATE,
	DST_MP_MAX_RATE				, TID_TOOLTIP_DST_MP_MAX_RATE, 
	DST_FP_MAX_RATE				, TID_TOOLTIP_DST_FP_RATE,
	DST_CHR_CHANCECRITICAL		, TID_TOOLTIP_DST_CRITICAL_RATE,
	DST_CHR_WEAEATKCHANGE		, TID_TOOLTIP_DST_CHR_WEAEATKCHANGE,
	DST_MASTRY_EARTH			, TID_TOOLTIP_DST_MASTRY_EARTH,
	DST_MASTRY_FIRE				, TID_TOOLTIP_DST_MASTRY_FIRE,
	DST_MASTRY_WATER			, TID_TOOLTIP_DST_MASTRY_WATER,
	DST_MASTRY_ELECTRICITY		, TID_TOOLTIP_DST_MASTRY_ELECTRICITY,
	DST_MASTRY_WIND				, TID_TOOLTIP_DST_MASTRY_WIND,
	DST_REFLECT_DAMAGE			, TID_TOOLTIP_DST_REFLECT_DAMAGE,
	DST_MP_DEC_RATE				, TID_TOOLTIP_DST_MP_DEC_RATE,
	DST_FP_DEC_RATE				, TID_TOOLTIP_DST_FP_DEC_RATE,
	DST_SPELL_RATE				, TID_TOOLTIP_DST_SPELL_RATE,
	DST_CAST_CRITICAL_RATE		, TID_TOOLTIP_DST_CAST_CRITICAL_RATE,
	DST_CRITICAL_BONUS			, TID_TOOLTIP_DST_CRITICAL_BONUS,
	DST_YOY_DMG,				  TID_TOOLTIP_DST_YOY_DMG,
	DST_BOW_DMG,				  TID_TOOLTIP_DST_BOW_DMG,
	DST_KNUCKLE_DMG,			  TID_TOOLTIP_DST_KNUCKLE_DMG,
	DST_SWD_DMG,				  TID_TOOLTIP_DST_SWD_DMG,
	DST_AXE_DMG,				  TID_TOOLTIP_DST_AXE_DMG,
#ifdef __NEWWPN1024
	DST_MASTRY_ALL,				  TID_TOOLTIP_DST_MASTRY_ALL,
#endif	// __NEWWPN1024
	DST_ATTACKSPEED_RATE,		  TID_TOOLTIP_ATTACKSPEED_RATE,
	DST_CHR_STEALHP,			  TID_TOOLTIP_DST_DMG_GET,
#if __VER >= 8 // __GUILDCOMBAT_85
	DST_PVP_DMG_RATE,				TID_TOOLTIP_DST_DMG_GET,			//gmpbigsun:js씨가 바꿔주면 고칠것
#endif // __VER >= 8
#if __VER >= 10 // __CSC_VER9_1
	DST_EXPERIENCE,					TID_TOOLTIP_DST_EXPERIENCE,
	DST_MELEE_STEALHP,				TID_TOOLTIP_DST_MELEE_STEALHP,
	DST_MONSTER_DMG,				TID_TOOLTIP_DST_MONSTER_DMG,
	DST_PVP_DMG,					TID_TOOLTIP_DST_PVP_DMG,
#endif //__CSC_VER9_1
#if __VER >= 9	// __PET_0410
	DST_HEAL,	TID_TOOLTIP_DST_HEAL,
#endif	// __PET_0410
	DST_ATKPOWER,	TID_TOOLTIP_ATKPOWER_VALUE,
#if __VER >= 10 // __LEGEND		//10차 전승스킬
	DST_ONEHANDMASTER_DMG,			TID_TOOLTIP_DST_ONEHANDMASTER_DMG,
	DST_TWOHANDMASTER_DMG,			TID_TOOLTIP_DST_TWOHANDMASTER_DMG,
	DST_YOYOMASTER_DMG,				TID_TOOLTIP_DST_YOYOMASTER_DMG,
	DST_BOWMASTER_DMG,				TID_TOOLTIP_DST_BOWMASTER_DMG,
	DST_KNUCKLEMASTER_DMG,			TID_TOOLTIP_DST_KNUCKLEMASTER_DMG,
	DST_HAWKEYE_RATE,				TID_TOOLTIP_DST_HAWKEYE,
	DST_RESIST_MAGIC_RATE,			TID_TOOLTIP_DEFMAGIC_RATE, 
#endif	// __LEGEND		//10차 전승스킬
#ifdef __DST_GIFTBOX
	DST_GIFTBOX,					TID_TOOLTIP_DST_GIFTBOX,
#endif // __DST_GIFTBOX
	DST_HPDMG_UP,					TID_TOOLTIP_DST_HPDMG_UP,
	DST_DEFHITRATE_DOWN,			TID_TOOLTIP_DST_DEFHITRATE_DOWN,
#if __VER >= 15 // __GUILD_HOUSE
	DST_RESTPOINT_RATE,				TID_TOOLTIP_RESTPOINT_RATE,
	DST_CHR_RANGE,					TID_GAME_TOOLTIP_ATTACKRANGE4,
	DST_STOP_MOVEMENT,				TID_GAME_TOOLTIP_MOVEMENT1,
	DST_IMMUNITY,					TID_GAME_TOOLTIP_IMMUNITY1,
#endif // __GUILD_HOUSE
#ifdef __V19_FORMATS
	DST_IGNORE_DMG_PVP, TID_MMI_ABSOLUTEBERRIER01,
	DST_TAKE_PVP_DMG_PHYSICAL_RATE, TID_TOOLTIP_TAKE_PVP_DMG_PHYSICAL_RATE,
	DST_TAKE_PVP_DMG_MAGIC_RATE, TID_TOOLTIP_TAKE_PVP_DMG_MAGIC_RATE,
	DST_TAKE_PVE_DMG_PHYSICAL_RATE, TID_TOOLTIP_TAKE_PVE_DMG_PHYSICAL_RATE,
	DST_TAKE_PVE_DMG_MAGIC_RATE, TID_TOOLTIP_TAKE_PVE_DMG_MAGIC_RATE,
	DST_DROP_ITEM_ALLGRADE_RATE, TID_TOOLTIP_DROP_ITEM_ALLGRADE_RATE,
	DST_DROP_ITEM_ONEMORE_CHANCE, TID_TOOLTIP_DROP_ITEM_ONEMORE_CHANCE,
	DST_GIVE_PVE_DMG_ELEMENT_FIRE_RATE, TID_TOOLTIP_GIVE_PVE_DMG_ELEMENT_FIRE_RATE,
	DST_GIVE_PVE_DMG_ELEMENT_WATER_RATE, TID_TOOLTIP_GIVE_PVE_DMG_ELEMENT_WATER_RATE,
	DST_GIVE_PVE_DMG_ELEMENT_ELECT_RATE, TID_TOOLTIP_GIVE_PVE_DMG_ELEMENT_ELECT_RATE,
	DST_GIVE_PVE_DMG_ELEMENT_WIND_RATE, TID_TOOLTIP_GIVE_PVE_DMG_ELEMENT_WIND_RATE,
	DST_GIVE_PVE_DMG_ELEMENT_EARTH_RATE, TID_TOOLTIP_GIVE_PVE_DMG_ELEMENT_EARTH_RATE,
	DST_ADJ_ITEM_EQUIP_LEVEL, TID_TOOLTIP_ADJ_ITEM_EQUIP_LEVEL,
	DST_SKILL_LEVELUP_ALL, TID_TOOLTIP_SKILL_LEVELUP_ALL,
	DST_GIVE_DMG_RATE_ENEMY_STUN, TID_TOOLTIP_GIVE_DMG_RATE_ENEMY_STUN,
	DST_GIVE_DMG_RATE_ENEMY_DARK, TID_TOOLTIP_GIVE_DMG_RATE_ENEMY_DARK,
	DST_GIVE_DMG_RATE_ENEMY_POISON, TID_TOOLTIP_GIVE_DMG_RATE_ENEMY_POISON,
	DST_GIVE_DMG_RATE_ENEMY_SLOW, TID_TOOLTIP_GIVE_DMG_RATE_ENEMY_SLOW,
	DST_GIVE_DMG_RATE_ENEMY_BLEEDING, TID_TOOLTIP_GIVE_DMG_RATE_ENEMY_BLEEDING,
	DST_GIVE_DMG_RATE_ENEMY_SILENT, TID_TOOLTIP_GIVE_DMG_RATE_ENEMY_SLILENT,
	DST_GIVE_DMG_RATE_ENEMY_LOOT, TID_TOOLTIP_GIVE_DMG_RATE_ENEMY_LOOT,
	DST_GIVE_DMG_RATE_ENEMY_SETSTONE, TID_TOOLTIP_GIVE_DMG_RATE_ENEMY_SETSTONE,
	DST_GIVE_DMG_RATE_ENEMY_SLEEPING, TID_TOOLTIP_GIVE_DMG_RATE_ENEMY_SLEEPING,
#endif //__V19_FORMATS

#ifdef __MIKSIK_NEWSTAT
		DST_HEALING_AMP, TID_TOOLTIP_DST_HEALING_AMP,
#endif //__MIKSIK_NEWSTAT

#ifdef __DST_CHANCEDROP_GOLD
	DST_CHANCEDROP, TID_DST_CHANCEDROP,
	DST_GOLDDROP, TID_DST_GOLDDROP,
#endif //__DST_CHANCEDROP_GOLD

#ifdef __DST_COOLTIME_DUNGEON
	DST_COOLTIME, TID_DST_COOLTIME,
#endif //__DST_COOLTIME_DUNGEON

#ifdef __DST_CHANCEDROP_PIECE
	DST_CHANCEDROP_PIECE, TID_DST_CHANCEDROP_PIECE,
#endif //__DST_CHANCEDROP_PIECE

#ifdef __DST_COLLECTOR_TIME
	DST_COLLECTOR_TIME, TID_DST_COLLECTOR_TIME,
#endif //__DST_COLLECTOR_TIME

#ifdef __DST_PET_EXPERIENCE
	DST_PET_EXPERIENCE, TID_DST_PET_EXPERIENCE,
#endif //__DST_PET_EXPERIENCE
	0, 0
};

BOOL IsDst_Rate( int nDstParam )
{
	static int nDstRate[] = {
			0, 
			0, 
			DST_ADJ_HITRATE,
			0, 
			DST_ATKPOWER_RATE,
			DST_ADJDEF_RATE,
			DST_DEFHITRATE_DOWN,
			DST_HP_MAX_RATE,
			DST_MP_MAX_RATE,		
			DST_FP_MAX_RATE,		
			DST_HP_RECOVERY_RATE,
			DST_MP_RECOVERY_RATE,
			DST_FP_RECOVERY_RATE,
			DST_CHR_CHANCECRITICAL,
			DST_MASTRY_EARTH,
			DST_MASTRY_FIRE,
			DST_MASTRY_WATER,
			DST_MASTRY_ELECTRICITY,
			DST_MASTRY_WIND,
			DST_ATTACKSPEED,
			DST_MP_DEC_RATE,
			DST_FP_DEC_RATE,
			DST_SPELL_RATE,
			DST_CAST_CRITICAL_RATE,
			DST_CRITICAL_BONUS,
#ifdef __NEWWPN1024
			DST_MASTRY_ALL,
#endif	

			DST_ALL_RECOVERY_RATE,
			DST_KILL_HP_RATE,	
			DST_KILL_MP_RATE,	
			DST_KILL_FP_RATE,
			DST_KILL_ALL_RATE,
			DST_ALL_DEC_RATE,
			DST_BLOCK_MELEE,
			DST_BLOCK_RANGE,		
			DST_ATTACKSPEED_RATE,
			DST_CHR_STEALHP,
#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
			DST_EXPERIENCE,
			DST_HAWKEYE_RATE,
			DST_RESIST_MAGIC_RATE,
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans
			DST_SPEED,
			DST_REFLECT_DAMAGE
#if __VER >= 15 // __GUILD_HOUSE
			, DST_RESTPOINT_RATE
			, DST_MONSTER_DMG
			, DST_PVP_DMG
#endif // __GUILD_HOUSE
#ifdef __V19_FORMATS
			, DST_TAKE_PVP_DMG_PHYSICAL_RATE
			, DST_TAKE_PVP_DMG_MAGIC_RATE
			, DST_TAKE_PVE_DMG_PHYSICAL_RATE
			, DST_TAKE_PVE_DMG_MAGIC_RATE
			, DST_DROP_ITEM_ALLGRADE_RATE
			, DST_GIVE_PVE_DMG_ELEMENT_FIRE_RATE
			, DST_GIVE_PVE_DMG_ELEMENT_WATER_RATE
			, DST_GIVE_PVE_DMG_ELEMENT_ELECT_RATE
			, DST_GIVE_PVE_DMG_ELEMENT_WIND_RATE
			, DST_GIVE_PVE_DMG_ELEMENT_EARTH_RATE
			, DST_GIVE_DMG_RATE_ENEMY_STUN
			, DST_GIVE_DMG_RATE_ENEMY_DARK
			, DST_GIVE_DMG_RATE_ENEMY_POISON
			, DST_GIVE_DMG_RATE_ENEMY_SLOW
			, DST_GIVE_DMG_RATE_ENEMY_BLEEDING
			, DST_GIVE_DMG_RATE_ENEMY_SILENT
			, DST_GIVE_DMG_RATE_ENEMY_LOOT
			, DST_GIVE_DMG_RATE_ENEMY_SETSTONE
			, DST_GIVE_DMG_RATE_ENEMY_SLEEPING
#endif //__V19_FORMATS

#ifdef __DST_CHANCEDROP_GOLD
			, DST_CHANCEDROP
			, DST_GOLDDROP
#endif //__DST_CHANCEDROP_GOLD

#ifdef __DST_COOLTIME_DUNGEON
			, DST_COOLTIME
#endif //__DST_COOLTIME_DUNGEON

#ifdef __DST_CHANCEDROP_PIECE
			, DST_CHANCEDROP_PIECE
#endif //__DST_CHANCEDROP_PIECE

#ifdef __DST_COLLECTOR_TIME
			, DST_COLLECTOR_TIME
#endif //__DST_COLLECTOR_TIME

#ifdef __DST_PET_EXPERIENCE
			, DST_PET_EXPERIENCE
#endif //__DST_PET_EXPERIENCE

#ifdef __MIKSIK_NEWSTAT
		, DST_HEALING_AMP
#endif //__MIKSIK_NEWSTAT
	};
	
	int n = sizeof( nDstRate ) / sizeof( nDstRate[0] );
	
	for( int i=0; i<n; ++i )
	{
		if( nDstRate[i] == nDstParam )
			return TRUE;
	}

	return FALSE;
}

// dst 파라메터이름을 스트링으로 리턴.
// ex) nDstparam = DST_STR -> "힘"
char *FindDstString( int nDstParam )
{
	for( int i = 0; ; ++i )
	{
		if( g_DstString[i].nDstParam == 0 )
			break;

		if( g_DstString[i].nDstParam == nDstParam )	// 같은걸 찾았으면 그 이름을 리턴.
			return (char*)prj.GetText(g_DstString[i].nNameIndex);
	}
	return "";
}

// 아이템 툴립 출력할것
// 랜덤 옵션 이름, 아이템 이름( 길드 망토면 길드 이름 ), (세트)
// 예 ) 곰의 코튼 슈트(세트)
DWORD CWndMgr::PutItemName( CItemElem* pItemElem, CEditString* pEdit )
{
	CString str;
	CString strTemp;
#ifdef __AEGON_THEME_SWITCHER
	DWORD dwColorHighlightRed = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_RED);

	DWORD dwColorbuf = CAegonThemeSwitcherMng::GetInstance()->GetNameColor(0);
#else // __AEGON_THEME_SWITCHER
	DWORD dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwName0;
#endif // __AEGON_THEME_SWITCHER

#if __VER >= 9 //__CSC_VER9_1
	if( pItemElem->GetProp()->dwReferStat1 == WEAPON_GENERAL )
#ifdef __AEGON_THEME_SWITCHER
		dwColorbuf = CAegonThemeSwitcherMng::GetInstance()->GetNameColor(0);
#else // __AEGON_THEME_SWITCHER
		dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwName0;
#endif // __AEGON_THEME_SWITCHER
	else if( pItemElem->GetProp()->dwReferStat1 == WEAPON_UNIQUE )
#ifdef __AEGON_THEME_SWITCHER
		dwColorbuf = CAegonThemeSwitcherMng::GetInstance()->GetNameColor(1);
#else // __AEGON_THEME_SWITCHER
		dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwName1;
#endif // __AEGON_THEME_SWITCHER
	else if( pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE )
#ifdef __AEGON_THEME_SWITCHER
		dwColorbuf = CAegonThemeSwitcherMng::GetInstance()->GetNameColor(3);
#else // __AEGON_THEME_SWITCHER
		dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwName3;
#endif // __AEGON_THEME_SWITCHER
	else if( pItemElem->GetProp()->dwReferStat1 == ARMOR_SET )
#ifdef __AEGON_THEME_SWITCHER
		dwColorbuf = CAegonThemeSwitcherMng::GetInstance()->GetNameColor(1);
#else // __AEGON_THEME_SWITCHER
		dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwName1;
#endif // __AEGON_THEME_SWITCHER
#ifdef __PREMIUM
	else if (pItemElem->GetProp()->dwItemKind3 == IK3_PREMIUM)
#ifdef __AEGON_THEME_SWITCHER
		dwColorbuf = CAegonThemeSwitcherMng::GetInstance()->GetNameColor(6);
#else // __AEGON_THEME_SWITCHER
		dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwName6;
#endif // __AEGON_THEME_SWITCHER
#endif //__PREMIUM
#ifdef __ANARCHY_SYSTEM
	else if (pItemElem->GetProp()->dwItemKind3 == IK3_ANARCHY)
#ifdef __AEGON_THEME_SWITCHER
		dwColorbuf = CAegonThemeSwitcherMng::GetInstance()->GetNameColor(7);
#else // __AEGON_THEME_SWITCHER
		dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwName7;
#endif // __AEGON_THEME_SWITCHER
#endif //__ANARCHY_SYSTEM
#ifdef __SYS_GLYPHS
	else if (pItemElem->GetProp()->dwItemKind3 == IK3_ARROW || pItemElem->GetProp()->dwID == II_SYS_GLYPH_SHARD)
		dwColorbuf = 0xff452aa3; // Purple
#endif //__SYS_GLYPHS
#ifdef __NEW_ITEM_VARUNA
	else if( pItemElem->GetProp()->dwReferStat1 >= BARUNA_D && pItemElem->GetProp()->dwReferStat1 <= BARUNA_S )
#ifdef __AEGON_THEME_SWITCHER
		dwColorbuf = CAegonThemeSwitcherMng::GetInstance()->GetNameColor(4);
#else // __AEGON_THEME_SWITCHER
		dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwName4;	
#endif // __AEGON_THEME_SWITCHER
#endif // __NEW_ITEM_VARUNA

#ifdef __PREMIUM
	switch (pItemElem->m_dwItemId)
	{
		case II_SCR_PREMIUM_01:
		case II_SCR_PREMIUM_07:
		case II_SCR_PREMIUM_15:
		case II_SCR_PREMIUM_30:
		case II_SCR_PREM_NOTUSE:
		{
#ifdef __AEGON_THEME_SWITCHER
			dwColorbuf = CAegonThemeSwitcherMng::GetInstance()->GetNameColor(6);
#else // __AEGON_THEME_SWITCHER
			dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwName6;
#endif // __AEGON_THEME_SWITCHER
		}
		break;
	}
#endif // __PREMIUM

#ifdef __ANARCHY_SYSTEM
	switch (pItemElem->m_dwItemId)
	{
		case II_ANARCHY_BUFF01:
		case II_ANARCHY_BUFF02:
		case II_ANARCHY_BUFF03:
		case II_ANARCHY_BUFF04:
		case II_ANARCHY_BUFF05:
		case II_ANARCHY_BUFF06:
		case II_ANARCHY_BUFF07:
		case II_ANARCHY_BUFF08:
		case II_ANARCHY_BUFF09:
		case II_ANARCHY_BUFF10:
		case II_ANARCHY_BUFF11:
		case II_ANARCHY_BUFF12:
		{
#ifdef __AEGON_THEME_SWITCHER
			dwColorbuf = CAegonThemeSwitcherMng::GetInstance()->GetNameColor(7);
#else // __AEGON_THEME_SWITCHER
			dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwName7;
#endif // __AEGON_THEME_SWITCHER
		}
		break;
	}
#endif // __ANARCHY_SYSTEM

#else //__CSC_VER9_1
	if( pItemElem->GetProp()->dwItemRare == 200 )
		dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwName1;
	else if( pItemElem->GetProp()->dwItemRare == 300 )
		dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwName2;
#endif //__CSC_VER9_1

#ifndef __TITANIUM_TOOLTIP_OPT
	if( pItemElem->IsFlag( CItemElem::binds ) 
		&& (pItemElem->m_dwItemId != II_GEN_MAT_ORICHALCUM01_1 && pItemElem->m_dwItemId != II_GEN_MAT_MOONSTONE_1)
#if __VER >= 14 // __EQUIP_BIND
		&& pItemElem->GetProp()->dwFlag != IP_FLAG_EQUIP_BIND 
#endif // __EQUIP_BIND
		)
		strTemp.Format( "%s ", prj.GetText( TID_GAME_TOOLTIP_ITEM_BINDS ) );
#endif //__TITANIUM_TOOLTIP_OPT

	if( pItemElem->GetProp()->dwParts != (DWORD)-1 )
	{
		PRANDOMOPTITEM pRandomOptItem	= CRandomOptItemGen::GetInstance()->GetRandomOptItem( pItemElem->GetRandomOpt() );
		if( pRandomOptItem )
			strTemp.Format( "%s ", pRandomOptItem->pszString );
	}

	BOOL bGuildCombatCloak = FALSE;

	if( pItemElem->m_dwItemId == II_ARM_S_CLO_CLO_DRAGON1 || pItemElem->m_dwItemId == II_ARM_S_CLO_CLO_DRAGON2 ||
		pItemElem->m_dwItemId == II_ARM_S_CLO_CLO_DRAGON3 || pItemElem->m_dwItemId == II_ARM_S_CLO_CLO_DRAGON4 )
		bGuildCombatCloak  = TRUE;

	if( pItemElem->GetProp()->dwItemKind3 == IK3_CLOAK && pItemElem->m_idGuild )	// 길드번호가 박혀있으면.
	{
		CGuild *pGuild = g_GuildMng.GetGuild( pItemElem->m_idGuild );
		if( pGuild && bGuildCombatCloak == FALSE )
			str.Format( prj.GetText( TID_GUILD_CLOAK ), pGuild->m_szGuild );
		else
			str.Format( "%s", pItemElem->GetProp()->szName );
	}
#if __VER >= 9 // __CSC_VER9_1
	else
	{
		str = pItemElem->GetName();
	}
	CString stredit;
	strTemp += str;

#ifdef __WEAPON_RARITY
	CString strName;
	WeaponRarity* nWeaponRarity = prj.m_nWeaponRarity.GetAt(pItemElem->GetWeaponRarity());
	if (nWeaponRarity && pItemElem->GetWeaponRarity() > 0) {
		strName.Format("[%s] %s", nWeaponRarity->m_szName, strTemp.GetString());
	}
	else {
		strName.Format("%s", strTemp.GetString());
	}
#ifdef __NEW_ITEM_VARUNA
	if (pItemElem->GetProp()->IsBaruna() || pItemElem->GetProp()->IsUltimate())
#else // __NEW_ITEM_VARUNA
	if (pItemElem->GetProp()->IsUltimate()) //Ultimate 아이콘이 들어가기 위한 여백
#endif // __NEW_ITEM_VARUNA
	{
		stredit.Format("             %s", strName.GetString());
	}
#ifdef __TITANIUM_TOOLTIP_OPT
	else if (pItemElem->IsFlag(CItemElem::expired)) {
		stredit.Format("[EXPIRED] %s", strName.GetString());
	}
#endif //__TITANIUM_TOOLTIP_OPT
	else {
		stredit.Format("%s", strName.GetString());
	}

	if (nWeaponRarity && pItemElem->GetWeaponRarity() > 0) {
		pEdit->AddString(stredit, nWeaponRarity->m_dwColor, ESSTY_BOLD);
	}
#ifdef __TITANIUM_TOOLTIP_OPT
	else if (pItemElem->IsFlag(CItemElem::expired)) {
		pEdit->AddString(stredit, dwColorHighlightRed, ESSTY_BOLD);
	}
#endif //__TITANIUM_TOOLTIP_OPT
	else {
		pEdit->AddString(stredit, dwColorbuf, ESSTY_BOLD);
	}
#else // __WEAPON_RARITY
	if (pItemElem->GetProp()->IsUltimate()) //Ultimate 아이콘이 들어가기 위한 여백
		stredit.Format("             %s", strTemp);
	else
		stredit.Format("%s", strTemp);

	pEdit->AddString(stredit, dwColorbuf, ESSTY_BOLD);
#endif // __WEAPON_RARITY
#else	// __CSC_VER9_1
	else
	{
		str.Format("%s", pItemElem->GetProp()->szName);
	}
	strTemp += str;

	pEdit->AddString(strTemp, dwColorbuf, ESSTY_BOLD);
#endif //__CSC_VER9_1
	
	return dwColorbuf;
}

void CWndMgr::PutItemAbilityPiercing( CItemElem* pItemElem, CEditString* pEdit, DWORD dwColorBuf )
{
#ifdef __WEAPON_RARITY
	WeaponRarity* nWeaponRarity = prj.m_nWeaponRarity.GetAt(pItemElem->GetWeaponRarity());
	if (nWeaponRarity && pItemElem->GetWeaponRarity() > 0)
		dwColorBuf = nWeaponRarity->m_dwColor;
#endif // __WEAPON_RARITY
	CString strTemp;
	if( pItemElem->GetAbilityOption() )
	{
		strTemp.Format( " %+d", pItemElem->GetAbilityOption() );
		pEdit->AddString( strTemp, dwColorBuf, ESSTY_BOLD );
	}
	
	int nCount = 0;
	for( int j = 0; j < pItemElem->GetPiercingSize(); j++ )
	{
		if( pItemElem->GetPiercingItem( j ) != 0 )
			nCount++;
	}
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	if( pItemElem->GetPiercingSize() > 0 && pItemElem->m_dwItemId != II_SYS_SYS_SCR_SEALCHARACTER )
#else	// __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	if( pItemElem->GetPiercingSize() > 0 )
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	{
		strTemp.Format( " (%d/%d)", nCount, pItemElem->GetPiercingSize() );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwPiercing , ESSTY_BOLD);
	}
}

void CWndMgr::PutItemResist( CItemElem* pItemElem, CEditString* pEdit )
{
	CString str;
	CString strTemp;
#ifdef __AEGON_THEME_SWITCHER
	DWORD dwResistColor = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
#else // __AEGON_THEME_SWITCHER
	DWORD dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResist;
#endif // __AEGON_THEME_SWITCHER
	str = "";
	switch( pItemElem->m_bItemResist )
	{
	case SAI79::FIRE:
		{
			str = prj.GetText(TID_UPGRADE_FIRE);
#ifdef __AEGON_THEME_SWITCHER
			dwResistColor = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
#else // __AEGON_THEME_SWITCHER
			dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResistFire;
#endif // __AEGON_THEME_SWITCHER
		}					
		break;
	case SAI79::WATER:
		{
			str = prj.GetText(TID_UPGRADE_WATER);
#ifdef __AEGON_THEME_SWITCHER
			dwResistColor = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
#else // __AEGON_THEME_SWITCHER
			dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResistWater;
#endif // __AEGON_THEME_SWITCHER
			
		}					
		break;
	case SAI79::EARTH:
		{
			str = prj.GetText(TID_UPGRADE_EARTH);
#ifdef __AEGON_THEME_SWITCHER
			dwResistColor = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
#else // __AEGON_THEME_SWITCHER
			dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResistEarth;
#endif // __AEGON_THEME_SWITCHER
			
		}
		break;
	case SAI79::ELECTRICITY:
		{
			str = prj.GetText(TID_UPGRADE_ELECTRICITY);
#ifdef __AEGON_THEME_SWITCHER
			dwResistColor = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
#else // __AEGON_THEME_SWITCHER
			dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResistElectricity;
#endif // __AEGON_THEME_SWITCHER
			
		}					
		break;
	case SAI79::WIND:
		{
			str = prj.GetText(TID_UPGRADE_WIND);
#ifdef __AEGON_THEME_SWITCHER
			dwResistColor = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
#else // __AEGON_THEME_SWITCHER
			dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResistWind;
#endif // __AEGON_THEME_SWITCHER
		
		}					
		break;
	}
	
	if( pItemElem->GetProp()->dwItemKind1 == IK1_WEAPON  || pItemElem->GetProp()->dwItemKind1 == IK1_ARMOR )
	{
		if( !str.IsEmpty() )
		{
			BOOL bBold = FALSE;
			strTemp.Format( "\n%s%+d", str, pItemElem->m_nResistAbilityOption );
			if( pItemElem->m_nResistSMItemId != 0 )
				bBold = TRUE;

			if( bBold )
				pEdit->AddString( strTemp, dwResistColor, ESSTY_BOLD );
			else
				pEdit->AddString( strTemp, dwResistColor );
		}
	}
	else
	{
		strTemp.Format( "\n%s Lv%d", str,  pItemElem->GetProp()->dwItemLV );
		pEdit->AddString( strTemp, dwResistColor );
	}
}

void CWndMgr::PutItemSpeed( CItemElem* pItemElem, CEditString* pEdit )
{
	CString strTemp;
	if( pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT || pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC )
	{	// 무기
		if( pItemElem->GetProp()->fAttackSpeed != 0xffffffff ) // 공격속도
		{
			pEdit->AddString( "\n" );
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ATTACKSPEED), GetATKSpeedString( pItemElem->GetProp()->fAttackSpeed ) );

#ifdef __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral ); 
#endif // __AEGON_THEME_SWITCHER
		}
	}
}

#if __VER >= 11 // __SYS_IDENTIFY
#ifdef __WEAPON_MERGE
void CWndMgr::PutAwakeningBlessing(CItemElem* pItemElem, CEditString* pEdit, bool bDefault /* = true */)
#else //__WEAPON_MERGE
void CWndMgr::PutAwakeningBlessing(CItemElem* pItemElem, CEditString* pEdit)
#endif //__WEAPON_MERGE
{
#ifdef __AEGON_THEME_SWITCHER
	DWORD dwColorHighlightBlue = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE);
#endif

	int nKind	= g_xRandomOptionProperty->GetRandomOptionKind( pItemElem );
	if( nKind < 0 )		// 아이템 각성, 여신의 축복 대상이 아니면,
		return;
	int nSize	= g_xRandomOptionProperty->GetRandomOptionSize( pItemElem->GetRandomOptItemId() );

#ifdef __PROTECT_AWAKE
	//각성 보호 취소된 아이템인가.. 그렇다면 줄그어진 옵션을 보여줘야 한다.
	bool bSafe = g_xRandomOptionProperty->IsCheckedSafeFlag( pItemElem->GetRandomOptItemId() );
	if( bSafe )
		nSize = g_xRandomOptionProperty->GetViewRandomOptionSize( pItemElem->GetRandomOptItemId() );

	assert( nSize >= 0 && nSize < 4 );
#endif //__PROTECT_AWAKE

	CString	str;

	BOOL bBlessing	= FALSE;
	// title
	if( nKind == CRandomOptionProperty::eAwakening )
	{
#if __VER >= 12 // __EXT_PIERCING
		if( nSize == 0 
#ifdef __WEAPON_MERGE
			&& bDefault
#endif //__WEAPON_MERGE
			)
		{
#ifdef __BARUNA_PIERCING
			if( pItemElem->GetProp()->IsBaruna() == FALSE )
			{
#endif // __BARUNA_PIERCING
				str.Format( "\n\"%s\"", prj.GetText( TID_GAME_AWAKENING ) );	// "각성할 수 있는 아이템"
#ifdef __AEGON_THEME_SWITCHER
				pEdit->AddString(str, dwColorHighlightBlue);
#else // __AEGON_THEME_SWITCHER
				pEdit->AddString(str, dwItemColor[g_Option.m_nToolTipText].dwAwakening);
#endif // __AEGON_THEME_SWITCHER
				
#ifdef __BARUNA_PIERCING
			}
#endif // __BARUNA_PIERCING
		}

#else //__EXT_PIERCING
		if( nSize == 0 )
			str.Format( "\n\"%s\"", prj.GetText( TID_GAME_AWAKENING ) );	// "각성할 수 있는 아이템"
		else
			str.Format( "\n%s", prj.GetText( TID_GAME_AWAKENING_CAPTION ) );	// 각성된 옵션
		pEdit->AddString( str, dwItemColor[g_Option.m_nToolTipText].dwAwakening );
#endif //__EXT_PIERCING
	}
#ifndef __BLESSINGS_TRADABLE
	else if (nKind == CRandomOptionProperty::eBlessing)
	{
		if (nSize > 0)
		{
			str.Format("\n%s", prj.GetText(TID_GAME_BLESSING_CAPTION));	// 축복받은 옵션
			pEdit->AddString(str, dwItemColor[g_Option.m_nToolTipText].dwBlessing);
			bBlessing = TRUE;
		}
	}
#endif // __BLESSINGS_TRADABLE
#if __VER >= 12 // __PET_0519
	else if( nKind == CRandomOptionProperty::eSystemPet || nKind == CRandomOptionProperty::eEatPet )
	{	// 시스템 펫과 먹펫의 툴팁에 각성과 관련된 내용을 추가한다
#if __VER >= 12 // __EXT_PIERCING
		if( nSize == 0 )
		{
			str.Format( "\n\"%s\"", prj.GetText( TID_GAME_AWAKENNIG_PET_00 ) );	// "각성할 수 있는 아이템"
#ifdef __AEGON_THEME_SWITCHER
			pEdit->AddString(str, dwColorHighlightBlue);
#else // __AEGON_THEME_SWITCHER
			pEdit->AddString( str, dwItemColor[g_Option.m_nToolTipText].dwAwakening );
#endif // __AEGON_THEME_SWITCHER
		}
#else //__EXT_PIERCING
		if( nSize == 0 )
			str.Format( "\n\"%s\"", prj.GetText( TID_GAME_AWAKENNIG_PET_00 ) );	// "각성할 수 있는 아이템"
		else
			str.Format( "\n%s", prj.GetText( TID_GAME_AWAKENING_CAPTION ) );	// 각성된 옵션
		pEdit->AddString( str, dwItemColor[g_Option.m_nToolTipText].dwAwakening );
#endif //__EXT_PIERCING
	}
#endif	// __PET_0519

	// option
	for( int i = 0; i < nSize; i++ )
	{
		int nDst, nAdj;
		if( !g_xRandomOptionProperty->GetParam( pItemElem->GetRandomOptItemId(), i, &nDst, &nAdj ) )
			continue;
		if( IsDst_Rate( nDst ) )
		{
#if __VER >= 12 // __EXT_PIERCING
			if( nDst == DST_ATTACKSPEED )
				str.Format( "\n%s %c%d%% ", FindDstString( nDst ), ( nAdj > 0? '+': '-' ), ::abs( nAdj / 2 / 10 ) );
			else
				str.Format( "\n%s %c%d%%", FindDstString( nDst ), ( nAdj > 0? '+': '-' ), ::abs( nAdj ) );
#else //__EXT_PIERCING
			if( nDst == DST_ATTACKSPEED )
				str.Format( "\n - %s %c%d%%", FindDstString( nDst ), ( nAdj > 0? '+': '-' ), ::abs( nAdj / 2 / 10 ) );
			else
				str.Format( "\n - %s %c%d%%", FindDstString( nDst ), ( nAdj > 0? '+': '-' ), ::abs( nAdj ) );
#endif //__EXT_PIERCING
		}
		else
		{
#if __VER >= 12 // __EXT_PIERCING
			str.Format( "\n%s %c%d", FindDstString( nDst ), ( nAdj > 0? '+': '-' ), ::abs( nAdj ) );
#else //__EXT_PIERCING
			str.Format( "\n - %s %c%d", FindDstString( nDst ), ( nAdj > 0? '+': '-' ), ::abs( nAdj ) );
#endif //__EXT_PIERCING
		}

#ifdef __AEGON_AWAKE_COLOR
		BOOL isMaxAdjParamary = g_xRandomOptionProperty->IsMaxAdjParamary(nKind, nDst, nAdj);
#endif // __AEGON_AWAKE_COLOR

		if( nKind == CRandomOptionProperty::eAwakening )
		{
			DWORD dwStyle = 0;
#ifdef __PROTECT_AWAKE
			dwStyle = ( bSafe ? ESSTY_STRIKETHROUGH : 0 );		//줄 그어버릴까?
#endif //__PROTECT_AWAKE
#ifdef __AEGON_AWAKE_COLOR
#ifdef __AEGON_THEME_SWITCHER
			pEdit->AddString(str, isMaxAdjParamary ? MAX_AWAKE_COLOR : dwColorHighlightBlue, dwStyle);
#else // __AEGON_THEME_SWITCHER
			pEdit->AddString(str, isMaxAdjParamary ? MAX_AWAKE_COLOR : dwItemColor[g_Option.m_nToolTipText].dwAwakening, dwStyle);
#endif // __AEGON_THEME_SWITCHER
#else // __AEGON_AWAKE_COLOR
			pEdit->AddString(str, dwItemColor[g_Option.m_nToolTipText].dwAwakening, dwStyle);

#endif // __AEGON_AWAKE_COLOR
		}
		else
#ifdef __AEGON_AWAKE_COLOR
#ifdef __AEGON_THEME_SWITCHER
			pEdit->AddString(str, isMaxAdjParamary ? MAX_AWAKE_COLOR : dwColorHighlightBlue);
#else
			pEdit->AddString(str, isMaxAdjParamary ? MAX_AWAKE_COLOR : dwItemColor[g_Option.m_nToolTipText].dwBlessing);
#endif
#else // __AEGON_AWAKE_COLOR
			pEdit->AddString(str, dwItemColor[g_Option.m_nToolTipText].dwBlessing);
#endif // __AEGON_AWAKE_COLOR
	}

#ifndef __BLESSINGS_TRADABLE
	if (bBlessing)
	{
		str.Format("\n%s", prj.GetText(TID_GAME_BLESSING_WARNING));
		pEdit->AddString(str, dwItemColor[g_Option.m_nToolTipText].dwBlessingWarning);
	}
#endif //__BLESSINGS_TRADABLE
}
#endif	// __SYS_IDENTIFY

#ifdef __LOOKCHANGE
void CWndMgr::PutLookText(CItemElem *pItemElem, CEditString *pEdit)
{
#ifdef __AEGON_THEME_SWITCHER
	DWORD dwColorHighlightPurple = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_PURPLE);
#endif //__AEGON_THEME_SWITCHER

	if( !pItemElem->IsChangedLook() )
		return;

	ItemProp *pLookProp = prj.GetItemProp(pItemElem->GetLook());
	if( !pLookProp )
		return;

	CString tmp;
	tmp.Format("\nTransmute Model: %s", pLookProp->szName );
#ifdef __AEGON_THEME_SWITCHER
	pEdit->AddString(tmp, dwColorHighlightPurple);
#else //__AEGON_THEME_SWITCHER
	pEdit->AddString( tmp, COLOR_ERROR );
#endif //__AEGON_THEME_SWITCHER
}
#endif // __LOOKCHANGE

#ifdef __BLING_BLING
void CWndMgr::PutBlingBlingText(CItemElem* pItemElem, CEditString* pEdit)
{
	if (pItemElem->IsFlag(CItemElem::reflect))
	{
		pEdit->AddString('\n');
#ifdef __AEGON_THEME_SWITCHER
		pEdit->AddString(prj.GetText(TID_BLING_BLING_00), CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
		pEdit->AddString(prj.GetText(TID_BLING_BLING_00), 0xFF000000);
#endif // __AEGON_THEME_SWITCHER
		pEdit->AddString(' ');
		pEdit->AddString(prj.GetText(TID_BLING_BLING_01), 0xFFFF9D00);
	}
}
#endif // __BLING_BLING

#ifdef __WEAPON_MERGE
void CWndMgr::PutMergeWeaponText(CItemElem* pItemElem, CEditString* pEdit)
{
	DWORD dwColorHighlightRed = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_RED);
	if (!pItemElem->IsMerged())
		return;

	pEdit->AddString("\n", COLOR_ERROR, ESSTY_BOLD);
	pEdit->AddString(prj.GetText(TID_MMI_TWOHANDEDWEAPONMERGE_RESULTTEXT01), COLOR_ERROR, ESSTY_BOLD);
	CString strTmp;
#ifdef __AEGON_THEME_SWITCHER
	strTmp.Format("\nATK: %d", pItemElem->GetMergedWeaponATK());
	pEdit->AddParsingString(strTmp, dwColorHighlightRed, ESSTY_BOLD);
#else
	strTmp.Format("\nATK: #b#cffff0000%d#nc#nb", pItemElem->GetMergedWeaponATK());
	pEdit->AddParsingString(strTmp);
#endif

	PIERCINGAVAIL pa;
	memset(&pa, 0, sizeof(PIERCINGAVAIL));
	pItemElem->GetMergedPiercingAvail(&pa);
	for (int i = 0; i < pa.nSize; i++)
	{
		int nDst = (int)pa.anDstParam[i];
		int nAdj = (int)pa.anAdjParam[i];

		if (IsDst_Rate(nDst))
		{
			if (nDst == DST_ATTACKSPEED)
				strTmp.Format("\n%s%+d%%", FindDstString(nDst), nAdj / 2 / 10);
			else
				strTmp.Format("\n%s%+d%%", FindDstString(nDst), nAdj);
		}
		else
		{
			strTmp.Format("\n%s+%d", FindDstString(nDst), nAdj);
		}

		pEdit->AddString(strTmp, dwItemColor[g_Option.m_nToolTipText].dwPiercing);
	}


	int nSize = g_xRandomOptionProperty->GetRandomOptionSize(pItemElem->GetMergedRandomOptItemId());
	for (int i = 0; i < nSize; i++)
	{
		int nDst, nAdj;
		if (!g_xRandomOptionProperty->GetParam(pItemElem->GetMergedRandomOptItemId(), i, &nDst, &nAdj))
			continue;
		if (IsDst_Rate(nDst))
		{
			if (nDst == DST_ATTACKSPEED)
				strTmp.Format("\n%s %c%d%% ", FindDstString(nDst), (nAdj > 0 ? '+' : '-'), ::abs(nAdj / 2 / 10));
			else
				strTmp.Format("\n%s %c%d%%", FindDstString(nDst), (nAdj > 0 ? '+' : '-'), ::abs(nAdj));
		}
		else
			strTmp.Format("\n%s %c%d", FindDstString(nDst), (nAdj > 0 ? '+' : '-'), ::abs(nAdj));
		DWORD dwStyle = 0;
#ifdef __AEGON_THEME_SWITCHER
		pEdit->AddString(strTmp, CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE), dwStyle);
#else // __AEGON_THEME_SWITCHER
		pEdit->AddString(strTmp, dwItemColor[g_Option.m_nToolTipText].dwAwakening, dwStyle);
#endif // __AEGON_THEME_SWITCHER
	}
}
#endif //__WEAPON_MERGE

#ifdef __PROPPACK_TOOLTIP
void CWndMgr::PutPackItemInfo(CItemElem* pItemElem, CEditString* pEdit)
{
	PPACKITEMELEM pPackItemElem = CPackItem::GetInstance()->Open(pItemElem->m_dwItemId);
	if (!pPackItemElem)
		return;
	if (pPackItemElem->nSize < 1)
		return;

	CString string;
	DWORD dwColorText = CAegonThemeSwitcherMng().GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE);
	string.Format("\n\nContains:\n");
	pEdit->AddParsingString(string, dwColorText, ESSTY_BOLD);

	for (int i = 0; i < pPackItemElem->nSize; i++)
	{
		CString pack;
		if (pPackItemElem->adwItem[i] == 0 || pPackItemElem->anNum[i] == 0)
		{
			continue;
		}
		if (pPackItemElem->anNum[i] > 1)
		{
			pack.Format("- %s (%ix)\n", prj.GetItemProp(pPackItemElem->adwItem[i])->szName, pPackItemElem->anNum[i]);
			pEdit->AddParsingString(pack, CAegonThemeSwitcherMng().GetInstance()->GetTextColor());
		}
		else
		{
			pack.Format("- %s\n", prj.GetItemProp(pPackItemElem->adwItem[i])->szName, pPackItemElem->anNum[i]);
			pEdit->AddParsingString(pack, CAegonThemeSwitcherMng().GetInstance()->GetTextColor());
		}
	}
}
#endif //__PROPPACK_TOOLTIP

#ifdef __CRYSTAL
void CWndMgr::PutCrystalText(CItemElem* pItemElem, CEditString* pEdit)
{
	if( pItemElem->GetCrystal() != NULL_ID )
	{
		ItemProp *pCrystal = prj.GetItemProp(pItemElem->GetCrystal());
		if( pCrystal )
		{
			CString text;
			text.Format( "Crystal : %s", pCrystal->szName );
			pEdit->AddString('\n');
			pEdit->AddString( text, 0xffff0000 );
#if __VER >= 19
			for (int i = 0; i < 6; i++)
#elif defined(__PROP_0827)
			for (int i = 0; i < 3; i++)
#else // __VER >= 19
			for (int i = 0; i < 2; i++)
#endif // __VER >= 19
			{
				if( pCrystal->dwDestParam[i] != -1 )
				{
					int nDst = (int)pCrystal->dwDestParam[i];
					int nAdj = (int)pCrystal->nAdjParamVal[i];

					CString strTemp;
					if( IsDst_Rate(nDst) )
						strTemp.Format( "\nCrystal bonus : %s%+d%%", FindDstString(nDst), nAdj );
					else
						strTemp.Format( "\nCrystal bonus : %s+%d", FindDstString(nDst), nAdj );

					pEdit->AddString(strTemp, 0xffff0000 );
				}
			}
		}
	}
}
#endif // __CRYSTAL

void CWndMgr::PutRandomOpt( CItemElem* pItemElem, CEditString* pEdit )
{
	if( pItemElem->GetProp()->dwParts == (DWORD)-1 )
		return;
	CString strTemp;
	PRANDOMOPTITEM pRandomOptItem	= CRandomOptItemGen::GetInstance()->GetRandomOptItem( pItemElem->GetRandomOpt() );
	if( pRandomOptItem ) // 2. 랜덤 옵션의 내용을 출력한다.
	{
		for( int i = 0; i < pRandomOptItem->ia.nSize; i++ )
		{
			int nDst = (int)pRandomOptItem->ia.anDstParam[i];
			int nAdj = (int)pRandomOptItem->ia.anAdjParam[i];
			
			if( IsDst_Rate(nDst) )
			{
				if( nDst == DST_ATTACKSPEED )
					strTemp.Format( "\n%s%+d%%", FindDstString(nDst), nAdj / 2 / 10 );
				else
					strTemp.Format( "\n%s%+d%%", FindDstString(nDst), nAdj );					
			}
			else
			{
				strTemp.Format( "\n%s+%d", FindDstString(nDst), nAdj );								
			}
#ifdef __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwRandomOption );
#endif // __AEGON_THEME_SWITCHER
		}
	}
}
void CWndMgr::PutPiercingOpt( CItemElem* pItemElem, CEditString* pEdit )
{
#ifdef __BARUNA_PIERCING
	if( pItemElem->GetProp()->IsBaruna())
	{
		CString strTemp;

		for( int i = 0; i < pItemElem->GetPiercingSize(); i++ )
		{
			if( pItemElem->GetPiercingItem( i ) != 0 )
			{
				DWORD dwRuneId = pItemElem->GetPiercingItem(i);				 
				ItemProp *pRuneProp = prj.GetItemProp( dwRuneId );
				if( !pRuneProp )
					return;

				ItemProp* pSkillProp = prj.GetSkillProp( pRuneProp->dwHitActiveSkillId );

				strTemp.Format( "\n%s", pSkillProp->szCommand );
			}
			pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwName4);
		}
	}
	else
#endif // __BARUNA_PIERCING
	{
		CString strTemp;
		PIERCINGAVAIL pa;
		memset( &pa, 0, sizeof(PIERCINGAVAIL) );
		pItemElem->GetPiercingAvail( &pa );
		for( int i = 0; i < pa.nSize; i++ )
		{
			int nDst = (int)pa.anDstParam[i];
			int nAdj = (int)pa.anAdjParam[i];
		
			if( IsDst_Rate(nDst) )
			{
				if( nDst == DST_ATTACKSPEED )
					strTemp.Format( "\n%s%+d%%", FindDstString( nDst ), nAdj / 2 / 10 );
				else
					strTemp.Format( "\n%s%+d%%", FindDstString( nDst ), nAdj );
			}
			else
			{
				strTemp.Format( "\n%s+%d", FindDstString( nDst ), nAdj );
			}
		
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwPiercing );
		}
	}
}

void CWndMgr::PutEnchantOpt( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit, int flag )
{
	CString str;
	CString strTemp;
	BOOL bPSetItem = FALSE;
	int nAbilityOption	= pMover->GetSetItemClient();
	if( pMover->IsActiveMover() )
	{
		if( flag == APP_INVENTORY && pMover->m_Inventory.IsEquip(pItemElem->m_dwObjId) && pMover->IsSetItemPart( pItemElem->GetProp()->dwParts )
			&& nAbilityOption > 0 )
			bPSetItem = TRUE;
	}
	else
	{
		if( flag == APP_QUERYEQUIP && pMover->IsSetItemPart( pItemElem->GetProp()->dwParts ) && nAbilityOption > 0 )
			bPSetItem = TRUE;
	}
	if( bPSetItem ) // 출력
	{	
		PSETITEMAVAIL psa = prj.GetSetItemAvail( nAbilityOption );
		if( psa ) // DST_MAX_HITRATE
		{					
			if( psa->nHitRate > 0 )
			{
				strTemp.Format( "\n%s+%d%%", FindDstString( (int)DST_ADJ_HITRATE ), (int)psa->nHitRate );
#ifdef __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwEnchantOption);
#endif // __AEGON_THEME_SWITCHER
			}
			if( psa->nBlock > 0 )
			{
				str = prj.GetText( TID_GAME_TOOLTIPBLOCKRATE );
				strTemp.Format( "\n%s+%d%%", str, (int)psa->nBlock );
#ifdef __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwEnchantOption);
#endif // __AEGON_THEME_SWITCHER
			}
			if( psa->nMaxHitPointRate > 0 )
			{
				strTemp.Format( "\n%s+%d%%", FindDstString( (int)DST_HP_MAX_RATE ), (int)psa->nMaxHitPointRate );
#ifdef __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwEnchantOption);
#endif // __AEGON_THEME_SWITCHER
			}
			if( psa->nAddMagic > 0 )
			{
				strTemp.Format( "\n%s+%d", FindDstString( (int)DST_ADDMAGIC ), (int)psa->nAddMagic );
#ifdef __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwEnchantOption);
#endif // __AEGON_THEME_SWITCHER
			}
			if( psa->nAdded > 0 )
			{
				strTemp.Format( "\n%s+%d", FindDstString( (int)DST_STAT_ALLUP ), (int)psa->nAdded );
#ifdef __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwEnchantOption);
#endif // __AEGON_THEME_SWITCHER
			}
		}
	}			
}

#ifdef __WIKIPEDIA_ITEMS
void CWndMgr::PutSetItemOptWiki(CMover* pMover, CItemElem* pItemElem, CEditString* pEdit)
{
	CString strTemp, strFull;
	CSetItem* pSetItem = CSetItemFinder::GetInstance()->GetSetItemByItemId(pItemElem->m_dwItemId);
	if (pSetItem)
	{
		ITEMAVAIL itemAvail;
		memset(&itemAvail, 0, sizeof(itemAvail));
		pSetItem->GetItemAvail(&itemAvail, pSetItem->m_nElemSize, TRUE);
		//int nEqui[MAX_SETITEM_ELEM] = { 0 };
		int nEqui[MAX_ITEMAVAIL] = { 0 };
		pSetItem->GetItemEquiped(*nEqui);
		for (int i = 0; i < itemAvail.nSize; i++)
		{
			int nDst = (int)itemAvail.anDstParam[i];
			int nAdj = (int)itemAvail.anAdjParam[i];

			if (IsDst_Rate(nDst))
			{
				if (nDst == DST_ATTACKSPEED)
					strTemp.Format("\n%s: %s% +d%%", prj.GetText(TID_TOOLTIP_SET), FindDstString(nDst), nAdj / 20);
				else
					strTemp.Format("\n%s: %s% +d%%", prj.GetText(TID_TOOLTIP_SET), FindDstString(nDst), nAdj);
			}
			else
				strTemp.Format("\n%s: %s +%d", prj.GetText(TID_TOOLTIP_SET), FindDstString(nDst), nAdj);
			strFull.Format("%s (%d/%d)", strTemp.GetString(), nEqui[i], pSetItem->m_nElemSize);
			pEdit->AddString(strFull, dwItemColor[g_Option.m_nToolTipText].dwSetEffect);
		}
	}
}
#endif //__WIKIPEDIA_ITEMS

void CWndMgr::PutSetItemOpt( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit )
{
#ifdef __AEGON_THEME_SWITCHER
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	DWORD dwColorHighlightBlue = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE);
	DWORD dwColorHighlightGrey = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_GREY);
	DWORD dwColorHighlightOrange = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_ORANGE);	
#endif
	CString strTemp;
	// 2. 세트 아이템의 구성 요소 인가?
	CSetItem* pSetItem	= CSetItemFinder::GetInstance()->GetSetItemByItemId( pItemElem->m_dwItemId );
	if( pSetItem )
	{
		// 해당 세트 아이템의 구성 요소를 모두 출력 하되 장착 된 것과 안된 것을 색으로 구별하여 출력한다.
		BOOL pbEquiped[MAX_HUMAN_PARTS];
		memset( pbEquiped, 0, sizeof(pbEquiped) );
		int nEquiped;
		pMover->GetEquipedSetItem( pSetItem->m_nId, pbEquiped, &nEquiped );
		// 3. 세트아이템 타이틀 출력
		strTemp.Format( "\n\n%s (%d/%d)", pSetItem->GetString(), pMover->GetEquipedSetItemNumber( pSetItem ), pSetItem->m_nElemSize );
//		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwSetName, ESSTY_BOLD );
#ifdef __AEGON_THEME_SWITCHER
		pEdit->AddString(strTemp, dwColorText);
#else // __AEGON_THEME_SWITCHER
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwSetName );
#endif // __AEGON_THEME_SWITCHER
		
		for( int i = 0; i < pSetItem->m_nElemSize; i++ )
		{
			ItemProp* pItemProp	= prj.GetItemProp( pSetItem->m_adwItemId[i] );
			if( pItemProp )
			{
				strTemp.Format( "\n   %s", pItemProp->szName  );
#ifdef __AEGON_THEME_SWITCHER
				if (pbEquiped[i]) // 장착되어있는 세트 아이템
					pEdit->AddString(strTemp, dwColorHighlightBlue);
				else
					pEdit->AddString(strTemp, dwColorHighlightGrey);
#else
				if( pbEquiped[i] ) // 장착되어있는 세트 아이템
					pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwSetItem1 );
				else
					pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwSetItem0 );
#endif
			}
		}

#ifdef __LEESE_VIEW_SETEFFECT
		if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
			nEquiped = pSetItem->m_nElemSize;
#endif //__LEESE_VIEW_SETEFFECT

		ITEMAVAIL itemAvail;
		memset( &itemAvail, 0, sizeof(itemAvail) );
		pSetItem->GetItemAvail( &itemAvail, nEquiped, TRUE );
		for( int i = 0; i < itemAvail.nSize; i++ )
		{
			int nDst = (int)itemAvail.anDstParam[i];
			int nAdj = (int)itemAvail.anAdjParam[i];
			
			if( IsDst_Rate(nDst) )
			{
				if( nDst == DST_ATTACKSPEED )
					strTemp.Format( "\n%s: %s% +d%%", prj.GetText(TID_TOOLTIP_SET), FindDstString( nDst ), nAdj / 2 / 10 );	
				else
					strTemp.Format( "\n%s: %s% +d%%", prj.GetText(TID_TOOLTIP_SET), FindDstString( nDst ), nAdj );	
			}
			else
			{
				strTemp.Format( "\n%s: %s +%d", prj.GetText(TID_TOOLTIP_SET), FindDstString( nDst ), nAdj );
			}
#ifdef __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, dwColorHighlightOrange);
#else
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwSetEffect );
#endif
		}
	}
}

void CWndMgr::PutItemMinMax( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit )
{
	pEdit->AddString( "\n" );
	CString strTemp;
	if( pItemElem->GetProp()->dwAbilityMin != 0xffffffff && pItemElem->GetProp()->dwAbilityMax != 0xffffffff 
		&& pItemElem->GetProp()->dwEndurance != 0xffffffff )
	{	
		DWORD dwColorMinMax = dwItemColor[g_Option.m_nToolTipText].dwEffective3;	// 회색
		float f = pMover->GetItemMultiplier( pItemElem );
		
		int nMin	= (int)( pMover->GetItemAbilityMin( pItemElem->GetProp()->dwID ) * f );
		int nMax	= (int)( pMover->GetItemAbilityMax( pItemElem->GetProp()->dwID ) * f );
		if (1.0f <= f)
		{
#ifdef __AEGON_THEME_SWITCHER
			dwColorMinMax = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();		// 검정
#else // __AEGON_THEME_SWITCHER
			dwColorMinMax = dwItemColor[g_Option.m_nToolTipText].dwEffective0;		// 검정
#endif // __AEGON_THEME_SWITCHER
		}
		else if( 0.8f <= f)
			dwColorMinMax = dwItemColor[g_Option.m_nToolTipText].dwEffective1;		// 노랑
		else if( 0.6f <= f )
			dwColorMinMax = dwItemColor[g_Option.m_nToolTipText].dwEffective2;		// 적색
		
#ifdef __WEAPON_RARITY
		WeaponRarity* nWeaponRarity = prj.m_nWeaponRarity.GetAt(pItemElem->GetWeaponRarity());
		if (nWeaponRarity && pItemElem->GetWeaponRarity() > 0)
		{
			nMin = (int)(nMin * nWeaponRarity->m_fAttackRate);
			nMax = (int)(nMax * nWeaponRarity->m_fAttackRate);
		}
#endif // __WEAPON_RARITY

		if( pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT || pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC )
		{	// 공격력
			int nOpt = 0;
			if( pItemElem->GetAbilityOption() > 0 )
			{
				int nAdd = (int)pow( (float)( pItemElem->GetAbilityOption() ), 1.5f );

				nMin += nAdd;
				nMax += nAdd;
			}
			if( nOpt )
			{
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ATTACKRANGE1) );
#ifdef __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
#endif // __AEGON_THEME_SWITCHER
				strTemp.Format( " (%d ~ %d)+%d", nMin, nMax, nOpt );
			}
			else
			{
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ATTACKRANGE2) );
#ifdef __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
#endif // __AEGON_THEME_SWITCHER
				strTemp.Format( " %d ~ %d", nMin, nMax );
			}
		}
		else
		{	// 방어력
			int nOpt = 0;			
			if( pItemElem->GetAbilityOption() > 0 )
			{
				int nAdd = (int)pow( (float)( pItemElem->GetAbilityOption() ), 1.5f );

				nMin += nAdd;
				nMax += nAdd;
			}
			if( nOpt )
			{	
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_DEFENSE_A) );
#ifdef __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
#endif // __AEGON_THEME_SWITCHER
				strTemp.Format( " (%d ~ %d)+%d", nMin, nMax, nOpt );
			}
			else
			{
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_DEFENSE_B) );
#ifdef __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
#endif // __AEGON_THEME_SWITCHER
				strTemp.Format( " %d ~ %d", nMin, nMax );
			}
		}
#ifdef __WEAPON_RARITY
		if (nWeaponRarity && pItemElem->GetWeaponRarity() > 0)
			pEdit->AddString(strTemp, nWeaponRarity->m_dwColor);
		else
			pEdit->AddString(strTemp, dwColorMinMax);
#else // __WEAPON_RARITY
		pEdit->AddString(strTemp, dwColorMinMax);
#endif // __WEAPON_RARITY
	}
}

#ifndef __VENDING_NPC
#ifdef __ANARCHY_SYSTEM
void CWndMgr::PutSkillTime(CItemElem* pItemElem, CEditString* pEdit, int nFlag)
{
	ItemProp* pItemProp = pItemElem->GetProp();
	if (pItemProp)
	{
		{
			CString str, strTemp;
			if (pItemProp->dwSkillTime != 0xffffffff && pItemProp->dwSkillTime != 999999999)
			{
				DWORD dwTime = pItemProp->dwSkillTime / 1000;
				int nRemainHour = dwTime / 60 / 60;
				int nRemainMin = dwTime / 60 - nRemainHour * 60;
				int nRemainSec = dwTime % 60;

				if (nRemainHour || nRemainMin || nRemainSec)
				{
					strTemp.Format("\nDuration: ");
					pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime);
					if (nRemainHour >= 1)
					{
						if (nRemainMin < 1 && nRemainSec < 1)
						{
							strTemp.Format("%.2d Hour(s)", nRemainHour);
						}
						else if (nRemainMin < 1)
						{
							strTemp.Format("%.2d Hour(s) %.2d Second(s)", nRemainHour, nRemainSec);
						}
						else if (nRemainSec < 1)
						{
							strTemp.Format("%.2d Hour(s) %.2d Minute(s)", nRemainHour, nRemainMin);
						}
						else
						{
							strTemp.Format("%.2d Hour(s) %.2d Minute(s)  %.2d Second(s)", nRemainHour, nRemainMin, nRemainSec);
						}
					}
					else if (nRemainMin >= 1)
						strTemp.Format("%.2d Minute(s) %.2d Second(s)", nRemainMin, nRemainSec);
					else if (nRemainSec >= 1)
						strTemp.Format("%.2d Second(s)", nRemainSec);
					pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime);
				}
			}
			else if (pItemProp->dwItemKind2 == IK2_BUFF2)
			{
				DWORD dwTime = pItemProp->dwAbilityMin;
				int nRemainHour = dwTime / 60;
				int nRemainDay = nRemainHour / 24;
				nRemainHour -= nRemainDay * 24;
				int nRemainMin = dwTime - nRemainHour * 60;

				if (nRemainDay || nRemainHour || nRemainMin)
				{
					strTemp.Format("\nDuration: ");
					pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime);
					if (nRemainDay >= 1)
						strTemp.Format("%d Day(s)", nRemainDay);
					else if (nRemainHour >= 1)
						strTemp.Format("%.2d Hour(s) %.2d Minute(s)", nRemainHour, nRemainMin);
					else if (nRemainMin >= 1)
						strTemp.Format("%.2d Month(s)", nRemainMin);
					pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime);
				}
			}
		}
	}
}


void CWndMgr::PutCooldown(CItemElem* pItemElem, CEditString* pEdit, int nFlag)
{
	ItemProp* pItemProp = pItemElem->GetProp();
	if (pItemProp)
	{
		CString str, strTemp;
		if (pItemProp->dwSkillReady != 0xffffffff)
		{
			DWORD dwTime = pItemProp->dwSkillReady / 1000;
			int dwRemainHour = dwTime / 60 / 60;
			int dwRemainMin = dwTime / 60 - dwRemainHour * 60;
			int dwRemainSec = dwTime % 60;

			strTemp.Format("\nWaiting time:");
			pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
			if (dwRemainHour >= 1)
				strTemp.Format("%.2dh %.2dm %.2ds", dwRemainHour, dwRemainMin, dwRemainSec);
			else if (dwRemainMin >= 1)
				strTemp.Format("%.2dm %.2ds", dwRemainMin, dwRemainSec);
			else if (dwRemainSec >= 1)
				strTemp.Format("%.2ds", dwRemainSec);
			pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
		}
	}
}
#endif // __ANARCHY_SYSTEM
#endif // __VENDING_NPC

void CWndMgr::PutMedicine( CItemElem* pItemElem, DWORD dwParam, LONG nParamVal, CEditString* pEdit )
{
	CString strTemp;
	if( dwParam != 0xffffffff )
	{	//  치료량
		if( nParamVal != 0xffffffff )
		{
			if( DST_MP == dwParam )
			{ // MP 치료량
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RECOVMP), nParamVal );
				pEdit->AddString( "\n" );
#ifdef __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
#endif // __AEGON_THEME_SWITCHER
			}
			else if( DST_HP == dwParam )
			{ // HP 치료량
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RECOVHP), nParamVal );
				pEdit->AddString( "\n" );
#ifdef __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
#endif // __AEGON_THEME_SWITCHER
			}
			else if( DST_FP == dwParam )
			{ // FP 치료량
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RECOVFP), nParamVal );
				pEdit->AddString( "\n" );
#ifdef __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
#endif // __AEGON_THEME_SWITCHER
			}
		}

		if( pItemElem->GetProp()->dwAbilityMin != 0xffffffff )
		{	// 최대회복량
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_MAXRECOVER), pItemElem->GetProp()->dwAbilityMin );
			pEdit->AddString( "\n" );
#ifdef __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
#endif // __AEGON_THEME_SWITCHER
		}
	}
}

#ifdef __VENDING_NPC
void CWndMgr::PutSkillTime(CItemElem* pItemElem, CEditString* pEdit, int nFlag)
{
	ItemProp* pItemProp = pItemElem->GetProp();
	if (pItemProp)
	{
		if (pItemProp->dwItemKind3 == IK3_VENDING)
		{
			CString str, strTemp;
			if (pItemElem->m_dwKeepTime)
			{
				DWORD dwTime = pItemElem->m_dwKeepTime;
				int nRemainHour = dwTime / 60 / 60;
				int nRemainDay = nRemainHour / 24;
				nRemainHour -= nRemainDay * 24;
				int nRemainMin = dwTime - nRemainHour * 60 - nRemainDay * 60 * 24;
				int nRemainSec = dwTime % 60;

				CTimeSpan span = CTimeSpan(0, 0, dwTime, 0);
				strTemp.Format("\nDuration: ");
				pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime);
				if (span.GetDays() >= 1)
					strTemp.Format("%d Day(s) %.2d Hour(s)", nRemainDay, nRemainHour);
				else if (span.GetHours() >= 1)
					strTemp.Format("%.2d Hour(s) %.2d Minute(s) %.2d Second(s)", nRemainHour, nRemainMin, nRemainSec);
				else if (span.GetMinutes() >= 1)
					strTemp.Format("%.2d Minute(s) %.2d Second(s)", nRemainMin, nRemainSec);
				else if (span.GetSeconds() >= 1)
					strTemp.Format("%.2d Second(s)", nRemainSec);
				pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime);
			}
			else
				if (pItemProp->dwSkillTime != 0xffffffff && pItemProp->dwSkillTime != 999999999 && pItemProp->dwSkillTime > 0)
				{
					DWORD dwTime = pItemProp->dwSkillTime;
					int nRemainHour = dwTime / 60;
					int nRemainDay = nRemainHour / 24;
					nRemainHour -= nRemainDay * 24;
					int nRemainMin = dwTime - nRemainHour * 60 - nRemainDay * 60 * 24;
					int nRemainSec = dwTime % 60;

					CTimeSpan span = CTimeSpan(0, 0, dwTime, 0);
					strTemp.Format("\nDuration: ");
					pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime);
					if (span.GetDays() >= 1)
						strTemp.Format("%lld Day(s)", span.GetDays());
					else if (span.GetHours() >= 1)
						strTemp.Format("%.2d Hour(s) %.2d Minute(s) %.2d Second(s)", span.GetHours(), span.GetMinutes(), span.GetSeconds());
					else if (span.GetMinutes() >= 1)
						strTemp.Format("%.2d Minute(s) %.2d Second(s)", span.GetMinutes(), span.GetSeconds());
					else if (span.GetSeconds() >= 1)
						strTemp.Format("%.2d Second(s)", span.GetSeconds());
					pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime);

				}
		}
		else
		{
			CString str, strTemp;
			if (pItemProp->dwSkillTime != 0xffffffff && pItemProp->dwSkillTime != 999999999)
			{
				DWORD dwTime = pItemProp->dwSkillTime / 1000;
				int nRemainHour = dwTime / 60 / 60;
				int nRemainMin = dwTime / 60 - nRemainHour * 60;
				int nRemainSec = dwTime % 60;

				if (nRemainHour || nRemainMin || nRemainSec)
				{
					strTemp.Format("\nDuration: ");
					pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime);
					if (nRemainHour >= 1 && nRemainMin >= 1 && nRemainSec >= 1)
						strTemp.Format("%.2d Hour(s) %.2d Minute(s) %.2d Second(s)", nRemainHour, nRemainMin, nRemainSec);
					else if (nRemainMin >= 1 && nRemainSec >= 1)
						strTemp.Format("%.2d Minute(s) %.2d Second(s)", nRemainMin, nRemainSec);

					else if (nRemainHour >= 1 && nRemainMin < 1 && nRemainSec < 1)
						strTemp.Format("%.2d Hour(s)", nRemainHour);
					else if (nRemainMin >= 1 && nRemainSec < 1)
						strTemp.Format("%.2d Minute(s)", nRemainMin);
					else if (nRemainSec >= 1)
						strTemp.Format("%.2d Second(s)", nRemainSec);
					pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime);
				}
			}
			else if (pItemProp->dwItemKind2 == IK2_BUFF2)
			{
				DWORD dwTime = pItemProp->dwAbilityMin;
				int nRemainHour = dwTime / 60;
				int nRemainDay = nRemainHour / 24;
				nRemainHour -= nRemainDay * 24;
				int nRemainMin = dwTime - nRemainHour * 60;

				if (nRemainDay || nRemainHour || nRemainMin)
				{
					strTemp.Format("\nDuration: ");
					pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime);
					if (nRemainDay >= 1)
						strTemp.Format("%d Day(s)", nRemainDay);
					else if (nRemainHour >= 1)
						strTemp.Format("%.2d Hour(s) %.2d Minute(s)", nRemainHour, nRemainMin);
					else if (nRemainMin >= 1)
						strTemp.Format("%.2d Minute(s)", nRemainMin);
					pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime);
				}
			}
		}
	}
}

void CWndMgr::PutCooldown(CItemElem* pItemElem, CEditString* pEdit, int nFlag)
{
	ItemProp* pItemProp = pItemElem->GetProp();
	if (pItemProp)
	{
		CString str, strTemp;
		if (pItemProp->dwSkillReady != 0xffffffff)
		{
			DWORD dwTime = pItemProp->dwSkillReady / 1000;
			int dwRemainHour = dwTime / 60 / 60;
			int dwRemainMin = dwTime / 60 - dwRemainHour * 60;
			int dwRemainSec = dwTime % 60;

			strTemp.Format("\nCooldown: ");
#ifdef __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
#endif // __AEGON_THEME_SWITCHER
			if (dwRemainHour >= 1)
				strTemp.Format("%.2d Hour(s) %.2d Minute(s) %.2d Second(s)", dwRemainHour, dwRemainMin, dwRemainSec);
			else if (dwRemainMin >= 1)
				strTemp.Format("%.2d Minute(s) %.2d Second(s)", dwRemainMin, dwRemainSec);
			else if (dwRemainSec >= 1)
				strTemp.Format("%.2d Second(s)", dwRemainSec);
#ifdef __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
#endif // __AEGON_THEME_SWITCHER
		}
	}
}
#endif // __VENDING_NPC

void CWndMgr::PutBaseItemOpt( CItemElem* pItemElem, CEditString* pEdit )
{
	CString str;
	CString strTemp;
#ifdef __V19_FORMATS
	for( int i = 0; i < 5; i ++ )
#else //__V19_FORMATS
#ifdef __PROP_0827
	for( int i = 0; i < 3; i ++ )
#else	// __PROP_0827
	for( int i = 0; i < 2; i ++ )
#endif	// __PROP_0827
#endif //__V19_FORMATS
	{
#ifdef __WEAPON_RARITY
		float fFactor = 1.0f;
		WeaponRarity* nWeaponRarity = prj.m_nWeaponRarity.GetAt(pItemElem->GetWeaponRarity());
		if (nWeaponRarity && pItemElem->GetWeaponRarity() > 0)
		{
			fFactor = nWeaponRarity->m_fStatsRate;
		}
#endif // __WEAPON_RARITY
		if( pItemElem->GetProp()->dwDestParam[i] != 0xffffffff 
#ifdef __AEGON_PERMANENT_BUFF
			&& pItemElem->GetProp()->dwDestParam[i] != DST_NONE
#endif // __AEGON_PERMANENT_BUFF
			)		
		{
			if( pItemElem->GetProp()->dwDestParam[i] == DST_STAT_ALLUP )
			{
#ifdef __WEAPON_RARITY
				str.Format("\n%s%+d", FindDstString(DST_STR), (int)((pItemElem->GetProp()->nAdjParamVal[i]) * fFactor));
				strTemp = str;
				str.Format("\n%s%+d", FindDstString(DST_STA), (int)((pItemElem->GetProp()->nAdjParamVal[i]) * fFactor));
				strTemp += str;
				str.Format("\n%s%+d", FindDstString(DST_DEX), (int)((pItemElem->GetProp()->nAdjParamVal[i]) * fFactor));
				strTemp += str;
				str.Format("\n%s%+d", FindDstString(DST_INT), (int)((pItemElem->GetProp()->nAdjParamVal[i]) * fFactor));
				strTemp += str;
#else // __WEAPON_RARITY
				str.Format("\n%s%+d", FindDstString(DST_STR), (int)pItemElem->GetProp()->nAdjParamVal[i]);
				strTemp = str;
				str.Format("\n%s%+d", FindDstString(DST_STA), (int)pItemElem->GetProp()->nAdjParamVal[i]);
				strTemp += str;
				str.Format("\n%s%+d", FindDstString(DST_DEX), (int)pItemElem->GetProp()->nAdjParamVal[i]);
				strTemp += str;
				str.Format("\n%s%+d", FindDstString(DST_INT), (int)pItemElem->GetProp()->nAdjParamVal[i]);
				strTemp += str;
#endif // __WEAPON_RARITY
			}
			else
			{
				int nDst	= (int)pItemElem->GetProp()->dwDestParam[i];
				if( IsDst_Rate( nDst ) )
				{

#ifdef __WEAPON_RARITY
					if (nDst == DST_ATTACKSPEED)
						strTemp.Format("\n%s%+d%%", FindDstString(nDst), (int)((pItemElem->GetProp()->nAdjParamVal[i] / 2 / 10) * fFactor));
					else
						strTemp.Format("\n%s%+d%%", FindDstString(nDst), (int)((pItemElem->GetProp()->nAdjParamVal[i]) * fFactor));
#else // __WEAPON_RARITY
					if (nDst == DST_ATTACKSPEED)
						strTemp.Format("\n%s%+d%%", FindDstString(nDst), (int)pItemElem->GetProp()->nAdjParamVal[i] / 2 / 10);
					else
						strTemp.Format("\n%s%+d%%", FindDstString(nDst), (int)pItemElem->GetProp()->nAdjParamVal[i]);
#endif // __WEAPON_RARITY
				}
				else
				{
#ifdef __WEAPON_RARITY
					strTemp.Format("\n%s%+d", FindDstString((int)pItemElem->GetProp()->dwDestParam[i]), (int)((pItemElem->GetProp()->nAdjParamVal[i]) * fFactor));
#else // __WEAPON_RARITY
					strTemp.Format("\n%s%+d", FindDstString((int)pItemElem->GetProp()->dwDestParam[i]), (int)pItemElem->GetProp()->nAdjParamVal[i]);
#endif // __WEAPON_RARITY
				}
			}
#ifdef __WEAPON_RARITY
			if (nWeaponRarity && pItemElem->GetWeaponRarity() > 0)
				pEdit->AddString(strTemp, nWeaponRarity->m_dwColor);
			else
			{
#ifdef __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
#endif // __AEGON_THEME_SWITCHER
			}
#else // __WEAPON_RARITY
			pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
#endif // __WEAPON_RARITY
		}
	}
#if __VER >= 11 // __SYS_COLLECTING
	CAccessoryProperty* pProperty	= CAccessoryProperty::GetInstance();
	if( pItemElem && pItemElem->IsAccessory() )		// 액세서리
	{
		vector<SINGLE_DST>* pDst	= pProperty->GetDst( pItemElem->m_dwItemId, pItemElem->GetAbilityOption() );
		for( int i = 0; i < (int)( pDst->size() ); i++ )
		{
			int nDst	= (*pDst)[i].nDst;
			int nAdj	= (*pDst)[i].nAdj;
			if( nDst == DST_STAT_ALLUP )
			{
				str.Format( "\n%s%+d", FindDstString( DST_STR ), nAdj );
				strTemp = str;
				str.Format( "\n%s%+d", FindDstString( DST_DEX ), nAdj );
				strTemp += str;
				str.Format( "\n%s%+d", FindDstString( DST_INT ), nAdj );
				strTemp += str;
				str.Format( "\n%s%+d", FindDstString( DST_STA ), nAdj );
				strTemp += str;
			}
			else
			{
				if( IsDst_Rate( nDst ) )
				{
					if( nDst == DST_ATTACKSPEED )
						strTemp.Format( "\n%s%+d%%", FindDstString( nDst ), nAdj / 2 / 10 );
					else
						strTemp.Format( "\n%s%+d%%", FindDstString( nDst ), nAdj );
				}
				else
				{
					strTemp.Format( "\n%s%+d", FindDstString( nDst ), nAdj );
				}
			}
#ifdef __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
#endif // __AEGON_THEME_SWITCHER
		}
	}
#endif	// __SYS_COLLECTING
}

#if __VER >= 9 // __CSC_VER9_1
void CWndMgr::PutAddedOpt( CItemElem* pItemElem, CEditString* pEdit )
{
	CString str;
	CString strTemp;
	int i;
	int nCount = 0;
	
#ifdef __NEW_DIAMONDS
	int nAddedOptDST[10];
#else //__NEW_DIAMONDS
	int nAddedOptDST[9];
#endif //__NEW_DIAMONDS
	int nAddedOpt[5];
	int nAddedValue[5];
#if __VER <	12 // __EXT_PIERCING
	DWORD dwColorbuf;
#endif //__EXT_PIERCING
	
	for(i=0; i<5; i++)
	{
		nAddedOpt[i] = -1;
		nAddedValue[i] = -1;
	}

	//DST 정의되는대로 아래 수정.
	nAddedOptDST[0] = DST_HP_MAX;
	nAddedOptDST[1] = DST_ATKPOWER;
	nAddedOptDST[2] = DST_ADJDEF;
	nAddedOptDST[3] = DST_MELEE_STEALHP;
	nAddedOptDST[4] = DST_PVP_DMG;
	nAddedOptDST[5] = DST_STR;
	nAddedOptDST[6] = DST_STA;
	nAddedOptDST[7] = DST_DEX;
	nAddedOptDST[8] = DST_INT;
#ifdef __NEW_DIAMONDS
	nAddedOptDST[9] = DST_MONSTER_DMG;
#endif //__NEW_DIAMONDS
	map<int, int> mapDst = prj.m_UltimateWeapon.GetDestParamUltimate( pItemElem );

	for( map<int, int>::iterator it=mapDst.begin(); it!=mapDst.end(); it++ )
	{
		int a = it->first;
#ifdef __NEW_DIAMONDS
		for (i = 0; i < 10; i++)
#else //__NEW_DIAMONDS
		for (i = 0; i < 9; i++)
#endif //__NEW_DIAMONDS
		{
			if( nAddedOptDST[i] == it->first )
			{
				nAddedOpt[nCount] = i;
				nAddedValue[nCount] = it->second;
				nCount++;
			}
		}
	}

	for(i=0; i<nCount; i++)
	{
		//추가 옵션을 툴팁에 추가.
		if(nAddedOpt[i] >= 0)
		{
#if __VER < 12 // __EXT_PIERCING
			switch(nAddedOpt[i]) 
			{
				case 0:
					dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwAddedOpt1;					
					break;
				case 1:
					dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwAddedOpt2;
					break;
				case 2:
					dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwAddedOpt3;
					break;
				case 3:
					dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwAddedOpt4;
					break;
				case 4:
					dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwAddedOpt5;
					break;
				case 5:
					dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwAddedOpt6;
					break;
				case 6:
					dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwAddedOpt7;
					break;
				case 7:
					dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwAddedOpt8;
					break;
				case 8:
					dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwAddedOpt9;
					break;
#ifdef __NEW_DIAMONDS
				case 9:
					dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwAddedOpt10;
					break;
#endif //__NEW_DIAMONDS
			}
#endif //__EXT_PIERCING

			if(FindDstString( nAddedOptDST[nAddedOpt[i]] ))
			{
				if(nAddedOpt[i] == 3 || nAddedOpt[i] == 4 
#ifdef __NEW_DIAMONDS
					|| nAddedOpt[i] == 9
#endif //__NEW_DIAMONDS
					)
					str.Format( "\n%s+%d%%", FindDstString( nAddedOptDST[nAddedOpt[i]] ), nAddedValue[i] );
				else
					str.Format( "\n%s+%d", FindDstString( nAddedOptDST[nAddedOpt[i]] ), nAddedValue[i] );
			}

			strTemp = str;
#if __VER >= 12 // __EXT_PIERCING
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwAddedOpt7 );
#else //__EXT_PIERCING
			pEdit->AddString( strTemp, dwColorbuf, ESSTY_BOLD );
#endif //__EXT_PIERCING
		}			
	}			
}

void CWndMgr::PutPetInfo( CItemElem* pItemElem, CEditString* pEdit )
{
	if(pItemElem == NULL || pEdit == NULL)
		return;
	
	CString strTemp;
	pEdit->Empty();
	//Name
//	strTemp.Format( "%s", pItemElem->GetProp()->szName );
	strTemp	= pItemElem->GetName();
#ifdef __AEGON_THEME_SWITCHER
	pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetNameColor(0), ESSTY_BOLD);
#else // __AEGON_THEME_SWITCHER
	pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwName0, ESSTY_BOLD );
#endif // __AEGON_THEME_SWITCHER

#if __VER >= 15 // __PETVIS
	PutPetKind( pItemElem, pEdit );
#endif

//	int nLife = pItemElem->m_pPet->GetLife();
//	if(nLife <= 0)
	if( pItemElem->IsFlag( CItemElem::expired ) )
	{
		strTemp.Format(" %s", prj.GetText(TID_GAME_PETINFO_DEAD));
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistSM1, ESSTY_BOLD );
	}

	//Level
	int nLevel = PL_EGG;
	if( pItemElem->m_pPet )
	{
		nLevel	= pItemElem->m_pPet->GetLevel();

		DWORD dwLevelText;
		switch(nLevel)
		{
			case PL_EGG:
				dwLevelText = TID_GAME_PETGRADE_E;
				break;
			case PL_D:
				dwLevelText = TID_GAME_PETGRADE_D;
				break;
			case PL_C:
				dwLevelText = TID_GAME_PETGRADE_C;
				break;
			case PL_B:
				dwLevelText = TID_GAME_PETGRADE_B;
				break;
			case PL_A:
				dwLevelText = TID_GAME_PETGRADE_A;
				break;
			case PL_S:
				dwLevelText = TID_GAME_PETGRADE_S;
				break;
		}
		strTemp.Format( "%s : %s", prj.GetText(TID_GAME_CHARACTER_02), prj.GetText(dwLevelText) );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistSM );

		//Ability value
		if(nLevel != PL_EGG)
		{
			DWORD dwDstParam;
			int nParam;
			DWORD dwTooltip;
			pItemElem->m_pPet->GetAvailDestParam(dwDstParam, nParam);
			
			switch(dwDstParam) 
			{
				case DST_STR:
					dwTooltip = TID_TOOLTIP_STR;
					break;
				case DST_DEX:
					dwTooltip = TID_TOOLTIP_DEX;
					break;
				case DST_INT:
					dwTooltip = TID_TOOLTIP_INT;
					break;
				case DST_STA:
					dwTooltip = TID_TOOLTIP_STA;
					break;
				case DST_ATKPOWER:
					dwTooltip	= TID_TOOLTIP_ATKPOWER_VALUE;
					break;
				case DST_ADJDEF:
					dwTooltip = TID_TOOLTIP_DEFENCE;
					break;
				case DST_HP_MAX:
					dwTooltip = TID_TOOLTIP_DST_HP_MAX;
					break;
			}
			strTemp.Format( "%s : %s +%d", prj.GetText(TID_GAME_ABILITY), prj.GetText(dwTooltip), nParam );
			pEdit->AddString( "\n" );
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistSM1 );
		}
		//Level History
		int nLevel = pItemElem->m_pPet->GetLevel();
		if(nLevel > PL_EGG)
		{
			pEdit->AddString( "\n" );
			pEdit->AddString( "(", D3DCOLOR_XRGB(0, 200, 255) );
			for(int i=PL_D; i<=nLevel; i++)
			{
				BYTE bLevel = pItemElem->m_pPet->GetAvailLevel(i);
				strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_LEVEL), bLevel);
				pEdit->AddString( strTemp, D3DCOLOR_XRGB(0, 127, 255) );
				if(i != nLevel)
					pEdit->AddString( "/", D3DCOLOR_XRGB(0, 200, 255) );
			}
			pEdit->AddString( ")", D3DCOLOR_XRGB(0, 200, 255) );

			//Pet Experience
			EXPINTEGER	nExpResult = pItemElem->m_pPet->GetExp() * (EXPINTEGER)10000 / pItemElem->m_pPet->GetMaxExp();
			float fExp = (float)nExpResult / 100.0f;

			if( fExp >= 99.99f )
				strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_EXP_MAX));
			else
				strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_EXP), fExp);
			pEdit->AddString( "\n" );
			pEdit->AddString( strTemp, D3DCOLOR_XRGB(120, 120, 220) );

			//Pet Energy
			int nMaxEnergy = pItemElem->m_pPet->GetMaxEnergy();
			int nEnergy = pItemElem->m_pPet->GetEnergy();
			int nLife = pItemElem->m_pPet->GetLife();
			pEdit->AddString( "\n" );
			strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_LIFE), nLife);
			pEdit->AddString( strTemp, D3DCOLOR_XRGB(255, 100, 100) );
			pEdit->AddString( "\n" );
			strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_HP), nEnergy, nMaxEnergy);
			pEdit->AddString( strTemp, D3DCOLOR_XRGB(255, 10, 10) );
		}
		else
		{
			//Pet Experience
			EXPINTEGER	nExpResult = pItemElem->m_pPet->GetExp() * (EXPINTEGER)10000 / pItemElem->m_pPet->GetMaxExp();
			float fExp = (float)nExpResult / 100.0f;

			if( fExp >= 99.99f )
				strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_EXP_MAX));
			else
				strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_EXP), fExp);
			pEdit->AddString( "\n" );
			pEdit->AddString( strTemp, D3DCOLOR_XRGB(120, 120, 220) );
		}

		//Description
		strTemp.Format( "%s", pItemElem->GetProp()->szCommand );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwPiercing );
	}
}

void CWndMgr::PutPetFeedPocket( CItemElem* pItemElem, CEditString* pEdit )
{
	CString strTemp, str;
	pEdit->Empty();

	if(pItemElem->m_dwKeepTime > 0) //유료아이템이 사용된 상태인가?
	{
		//Name
		strTemp.Format( "%s", pItemElem->GetProp()->szName );
#ifdef __AEGON_THEME_SWITCHER
		pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetNameColor(0), ESSTY_BOLD);
#else // __AEGON_THEME_SWITCHER
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwName0, ESSTY_BOLD );
#endif // __AEGON_THEME_SWITCHER

		if(g_pPlayer->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_PET_FEED_POCKET )) //활성화
		{
			strTemp.Format( " %s", prj.GetText(TID_GAME_POCKETUSING) );
			pEdit->AddString( strTemp, D3DCOLOR_XRGB( 255, 0, 0 ) );

			//사용 제한 시한
			time_t t = pItemElem->m_dwKeepTime - time_null();
			if( pItemElem->m_dwKeepTime && !pItemElem->IsFlag( CItemElem::expired ) )
			{
				if( t > 0 )
				{
					CTimeSpan time( t );
					if( time.GetDays() )
						str.Format( prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(time.GetDays()+1) );
					else if( time.GetHours() )
						str.Format( prj.GetText(TID_PK_LIMIT_HOUR), time.GetHours() );
#if __VER >= 11 // __CHIPI_071210
					else if( time.GetMinutes() )
						str.Format( prj.GetText(TID_PK_LIMIT_MINUTE), time.GetMinutes() );
					else
						str.Format( prj.GetText(TID_PK_LIMIT_SECOND), time.GetSeconds() );
#else //__CHIPI_071210
					else if( time.GetMinutes() > 10 )
						str.Format( prj.GetText(TID_PK_LIMIT_MINUTE), time.GetMinutes() );
					else
						str.Format( prj.GetText(TID_TOOLTIO_PERIODOFMIN), 10 );
#endif //__CHIPI_071210
				}
#if __VER < 11 // __CHIPI_071210
				else
				{
					str.Format( prj.GetText(TID_TOOLTIO_PERIODOFMIN), 10 );
				}
#endif //__CHIPI_071210
				strTemp = str + prj.GetText(TID_TOOLTIP_PERIOD);	
				pEdit->AddString( "\n" );
				pEdit->AddString( strTemp, D3DCOLOR_XRGB( 255, 20, 20 ) );
			}
			
			//사료 개수
			pEdit->AddString( "\n" );
			strTemp.Format( "%s %d", prj.GetText( TID_GAME_PET_FEED_COUNT ), g_pPlayer->GetItemNumForClient( II_SYS_SYS_FEED_01 ) );
			pEdit->AddString( strTemp, D3DCOLOR_XRGB( 50, 50, 205 ) ); 
		}
		else //비활성화
		{
			strTemp.Format( " %s", prj.GetText(TID_GAME_POCKET_NOTUSING) );
			pEdit->AddString( strTemp, D3DCOLOR_XRGB( 255, 0, 0 ) );

			//사용 제한 시한
			time_t t = pItemElem->m_dwKeepTime - time_null();
			if( pItemElem->m_dwKeepTime && !pItemElem->IsFlag( CItemElem::expired ) )
			{
				if( t > 0 )
				{
					CTimeSpan time( t );
					if( time.GetDays() )
						str.Format( prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(time.GetDays()+1) );
					else if( time.GetHours() )
						str.Format( prj.GetText(TID_PK_LIMIT_HOUR), time.GetHours() );
#if __VER >= 11 // __CHIPI_071210
					else if( time.GetMinutes() )
						str.Format( prj.GetText(TID_PK_LIMIT_MINUTE), time.GetMinutes() );
					else
						str.Format( prj.GetText(TID_PK_LIMIT_SECOND), time.GetSeconds() );
#else //__CHIPI_071210
					else if( time.GetMinutes() > 10 )
						str.Format( prj.GetText(TID_PK_LIMIT_MINUTE), time.GetMinutes() );
					else
						str.Format( prj.GetText(TID_TOOLTIO_PERIODOFMIN), 10 );
#endif //__CHIPI_071210
				}
#if __VER < 11 // __CHIPI_071210
				else
				{
					str.Format( prj.GetText(TID_TOOLTIO_PERIODOFMIN), 10 );
				}
#endif //__CHIPI_071210
				strTemp = str + prj.GetText(TID_TOOLTIP_PERIOD);	
				pEdit->AddString( "\n" );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime );
			}		
		}
		
		//Description
		strTemp.Format( "%s", prj.GetText(TID_GAME_PET_FEEDPOCKET) );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwPiercing );
	}
	else
	{
		//Name
		strTemp.Format( "%s", pItemElem->GetProp()->szName );
#ifdef __AEGON_THEME_SWITCHER
		pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetNameColor(0), ESSTY_BOLD);
#else // __AEGON_THEME_SWITCHER
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwName0, ESSTY_BOLD );
#endif // __AEGON_THEME_SWITCHER
		//Description
		strTemp.Format( "%s", prj.GetText(TID_GAME_PET_FEEDPOCKET_USE) );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwPiercing );				
	}
}
#endif //__CSC_VER9_1

#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
void CWndMgr::PutSealChar( CItemElem* pItemElem, CEditString* pEdit )
{
	CString str,strTemp,strTemp2,strTemp3;

	str.Format( "\n%s\n%s", prj.GetText(TID_TOOLTIP_CHARNAME), pItemElem->m_szItemText );
	strTemp = str;

	int	nJob = (int)pItemElem->m_nRepair;
	int nLevel =  (int)pItemElem->m_nRepairNumber;

	if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER )
		strTemp2.Format( "%d%s", nLevel, prj.GetText( TID_GAME_TOOLTIP_MARK_MASTER ) );
	else if( MAX_MASTER <= nJob )
#if __VER >= 15 // __HERO129_VER15 // 15차 히어로 레벨확장
		strTemp2.Format( "%d%s", nLevel, prj.GetText( TID_GAME_TOOLTIP_MARK_HERO ) );
#else // 15차 히어로 레벨확장
		strTemp2 = prj.GetText( TID_GAME_TOOLTIP_MARK_HERO_BEFORE );
#endif // 15차 히어로 레벨확장
	else 
		strTemp2.Format( "%d", nLevel );

	str.Format( "\n%s%s", prj.GetText(TID_TOOLTIP_CHARLEVEL), strTemp2 );
	strTemp += str;
	
	strTemp3.Format("%s",prj.m_aJob[nJob].szName);

	str.Format( "\n%s%s", prj.GetText(TID_TOOLTIP_CHARJOB), strTemp3 );
	strTemp += str;
	str.Format( "\n%s", prj.GetText(TID_TOOLTIP_CHARSTAT) );
	strTemp += str;

	
	str.Format( "\n%s%d", prj.GetText(TID_TOOLTIP_CHARSTA), (int)pItemElem->GetPiercingItem( 0 ) );
	strTemp += str;
	str.Format( "\n%s%d", prj.GetText(TID_TOOLTIP_CHARSTR), (int)pItemElem->GetPiercingItem( 1 ) );
	strTemp += str;
	str.Format( "\n%s%d", prj.GetText(TID_TOOLTIP_CHARDEX), (int)pItemElem->GetPiercingItem( 2 ) );
	strTemp += str;
	str.Format( "\n%s%d", prj.GetText(TID_TOOLTIP_CHARINT), (int)pItemElem->GetPiercingItem( 3 ) );
	strTemp += str;
	str.Format( "\n%s%d", prj.GetText(TID_TOOLTIP_CHARPOINT), (int)pItemElem->m_nResistAbilityOption );
	strTemp += str;

	pEdit->AddString( strTemp, D3DCOLOR_XRGB( 255, 20, 20 ) );
}
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

void CWndMgr::PutBaseResist( CItemElem* pItemElem, CEditString* pEdit )
{
	CString strTemp;
	if( pItemElem->GetProp()->nItemResistElecricity )
	{
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ELEC_RES), pItemElem->GetProp()->nItemResistElecricity );
		pEdit->AddString( "\n" );
#ifdef __AEGON_THEME_SWITCHER
		pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistElectricity );
#endif // __AEGON_THEME_SWITCHER
	}
	if( pItemElem->GetProp()->nItemResistFire )
	{
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_FIRE_RES), pItemElem->GetProp()->nItemResistFire );
		pEdit->AddString( "\n" );
#ifdef __AEGON_THEME_SWITCHER
		pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistFire );
#endif // __AEGON_THEME_SWITCHER
	}
	if( pItemElem->GetProp()->nItemResistWater )
	{
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_WATER_RES), pItemElem->GetProp()->nItemResistWater );
		pEdit->AddString( "\n" );
#ifdef __AEGON_THEME_SWITCHER
		pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistWater );
#endif // __AEGON_THEME_SWITCHER
	}
	if( pItemElem->GetProp()->nItemResistWind )
	{
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_WIND_RES), pItemElem->GetProp()->nItemResistWind );
		pEdit->AddString( "\n" );
#ifdef __AEGON_THEME_SWITCHER
		pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistWind );
#endif // __AEGON_THEME_SWITCHER
	}
	if( pItemElem->GetProp()->nItemResistEarth )
	{
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_EARTH_RES), pItemElem->GetProp()->nItemResistEarth );
		pEdit->AddString( "\n" );
#ifdef __AEGON_THEME_SWITCHER
		pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistEarth );
#endif // __AEGON_THEME_SWITCHER
	}
}

void CWndMgr::PutItemGold( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit, int flag )
{
	CString str;
	CString strTemp;
	if( flag == APP_SHOP_ ) // 가격은 나는 안나오게 하고 상점 인터페이스에서는 나오게함 42은 상점의 Invantory 개수
	{	// 상점
		if( pItemElem->GetCost() != 0xffffffff ) 
		{	// 가격
#if __VER >= 11 // __CSC_VER11_3
			CWndShop* pWndShop	= (CWndShop*)GetWndBase( APP_SHOP_ );
			LPCHARACTER lpCharacter = pWndShop->m_pMover->GetCharacter();
			int nCost = 0;
			int nBeforeTax = 0;
			if(lpCharacter)
			{
				if(lpCharacter->m_nVenderType == 0)
				{
					nCost = pItemElem->GetCost();
#ifdef __SHOP_COST_RATE
					nCost = (int)( prj.m_fShopBuyRate * nCost );
#endif // __SHOP_COST_RATE
				#if __VER >= 12 // __TAX
					nBeforeTax = nCost;
					if( CTax::GetInstance()->IsApplyTaxRate( g_pPlayer, pItemElem ) )
						nCost += ( static_cast<int>(nCost * CTax::GetInstance()->GetPurchaseTaxRate( g_pPlayer )) );
				#endif // __TAX
				}
#ifdef __VENDOR_UPDATE
				else if (lpCharacter->m_nVenderType != 0)
#else // __VENDOR_UPDATE
				else if (lpCharacter->m_nVenderType == 1)
#endif // __VENDOR_UPDATE
				{
#ifdef __SQK_MULTIALTERNATEPRICE
					nCost = pItemElem->GetCurrencyCost(lpCharacter->m_nVenderType);
#else //__SQK_MULTIALTERNATEPRICE
					nCost = pItemElem->GetChipCost();
#endif //__SQK_MULTIALTERNATEPRICE
				}
			}
#else //__CSC_VER11_3
			int nCost = pItemElem->GetCost();
#endif //__CSC_VER11_3
			nCost = (int)(nCost * prj.m_fShopCost );
#if __VER < 8 // __S8_PK
			KarmaProp* pProp = prj.GetKarmaProp( pMover->m_nSlaughter );
			if( pProp )
			{
				if( pProp->fDiscountRate != 0 )
					nCost = nCost  * pProp->fDiscountRate;
			}
#endif // __VER < 8 // __S8_PK
#if __VER >= 11 // __MA_VER11_02
			if( pItemElem->m_dwItemId == II_SYS_SYS_SCR_PERIN )
				nCost = PERIN_VALUE;
#endif //__MA_VER11_02
			
			if( nCost <= 0 )
			{
				nCost = 1;
#if __VER >= 12 // __TAX
				nBeforeTax = 1;
#endif // __TAX
			}
			
			char szbuffer[1024];
			_itoa( nCost, szbuffer, 10 );
			str = GetNumberFormatEx(szbuffer);
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_COST2), str );
#if __VER >= 11 // __CSC_VER11_3
#ifdef __VENDOR_UPDATE
			if (lpCharacter && lpCharacter->m_nVenderType != 0)
			{
				CString temp;
				temp.Format(" %s", prj.GetItemProp(lpCharacter->m_nVenderType)->szName);
				strTemp += temp;
			}
#else // __VENDOR_UPDATE
			if (lpCharacter && lpCharacter->m_nVenderType == 1)
			{
				CString temp;
				temp.Format(" %s", prj.GetText(TID_GAME_REDCHIP));
				strTemp += temp;
			}
#endif // __VENDOR_UPDATE
#endif //__CSC_VER11_3
			pEdit->AddString( "\n" );
#ifdef __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_YELLOW));
#else // __AEGON_THEME_SWITCHER
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGold );
#endif // __AEGON_THEME_SWITCHER
#if __VER >= 12 // __TAX
			if( CTax::GetInstance()->IsApplyTaxRate( g_pPlayer, pItemElem )
				&& lpCharacter->m_nVenderType == 0
				)
			{
				_itoa( nBeforeTax, szbuffer, 10 );
				str = GetNumberFormatEx(szbuffer);
				strTemp.Format( prj.GetText( TID_GAME_TAX_RATE ), str );
				pEdit->AddString( "\n" );
				pEdit->AddString( strTemp, prj.GetTextColor( TID_GAME_TAX_RATE ) );
			}
#endif // __TAX
		}
	}
	else if( flag == APP_INVENTORY )	// 인벤토리 / 뱅크 툴팁?
	{
		CWndShop* pWndShop	= (CWndShop*)GetWndBase( APP_SHOP_ );
		if( pWndShop )	// 상점을 열엇을때 파는가격도 나와야함
		{
			if( pItemElem->GetCost() != 0xffffffff )
			{	//	파는가격
				char szbuffer[1024];

#ifdef __NO_TICKET_RESELL
				DWORD dwCostTem = (pItemElem->GetCost() / 4);
				if (pItemElem->GetProp()->dwItemKind3 == IK3_TICKET && (pItemElem->m_dwKeepTime > 0 || pItemElem->IsFlag(CItemElem::expired)))
				{
					dwCostTem = 1;
				}
#ifdef __LEESE_NO_EVENTITEM_RESELL
				if (pItemElem->IsFlag(CItemElem::binds) && (pItemElem->m_dwItemId != II_GEN_MAT_ORICHALCUM01_1 && pItemElem->m_dwItemId != II_GEN_MAT_MOONSTONE_1) && pItemElem->GetProp()->dwFlag != IP_FLAG_EQUIP_BIND)
				{
					dwCostTem = 1;
				}
#endif //__LEESE_NO_EVENTITEM_RESELL
#else // __NO_TICKET_RESELL
				DWORD dwCostTem = 0;
				dwCostTem = pItemElem->GetCost() / 4;
#endif // __NO_TICKET_RESELL

#ifdef __SHOP_COST_RATE
				dwCostTem = (DWORD)( prj.m_fShopSellRate * dwCostTem );
#endif // __SHOP_COST_RATE
			#if __VER >= 12 // __TAX
				DWORD dwBeforeTax = dwCostTem;
				if( CTax::GetInstance()->IsApplyTaxRate( g_pPlayer, pItemElem ) )
					dwCostTem -= ( static_cast<DWORD>(dwCostTem * CTax::GetInstance()->GetSalesTaxRate( g_pPlayer )) );
			#endif // __TAX
#if __VER < 8 // __S8_PK
				KarmaProp* pProp = prj.GetKarmaProp( pMover->m_nSlaughter );
				if( pProp && pProp->fSellPenaltyRate != 0 )
					dwCostTem *= pProp->fSellPenaltyRate;
#endif // __VER < 8 // __S8_PK
				
				if( dwCostTem < 1 )
				{				
					dwCostTem = 1;
#if __VER >= 12 // __TAX
					dwBeforeTax = 1;
#endif // __TAX
				}

				_itoa( dwCostTem, szbuffer, 10 );
				str = GetNumberFormatEx(szbuffer);
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_SHELLCOST2), str );
				pEdit->AddString( "\n" );
#ifdef __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_YELLOW));
#else // __AEGON_THEME_SWITCHER
				pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGold);
#endif // __AEGON_THEME_SWITCHER
#if __VER >= 12 // __TAX
				if( CTax::GetInstance()->IsApplyTaxRate( g_pPlayer, pItemElem ) )
				{
					_itoa( dwBeforeTax, szbuffer, 10 );
					str = GetNumberFormatEx(szbuffer);
					strTemp.Format( prj.GetText( TID_GAME_TAX_RATE ), str );
					pEdit->AddString( "\n" );
					pEdit->AddString( strTemp, prj.GetTextColor( TID_GAME_TAX_RATE ) );	
				}
#endif // __TAX
			}
		}
	}
	else if( flag == APP_VENDOR )
	{
		if( pItemElem->m_nCost != 0xffffffff )
		{	// 파는가격
			char szbuffer[1024];
			DWORD dwCostTem = 0;
			dwCostTem = pItemElem->m_nCost;
			_itoa( dwCostTem, szbuffer, 10 );
			str = GetNumberFormatEx(szbuffer);
			
#ifdef __CHIP_PLAYERSHOP
			CString CurrencyName;
			switch (pItemElem->m_nBuyMode)
			{
				case 1: CurrencyName = "Penya"; break;
				case 2: CurrencyName = "Perin"; break;
				case 3: CurrencyName = "Red Chips"; break;
				case 4: CurrencyName = "Red Perin"; break;
				default: CurrencyName = " ";
			}

			strTemp.Format("Selling price: %s %s", str, CurrencyName.GetString());
#else //__CHIP_PLAYERSHOP
			strTemp.Format(prj.GetText(TID_GAME_TOOLTIP_SHELLCOST2), str);
#endif // __CHIP_PLAYERSHOP

			pEdit->AddString( "\n" );
#ifdef __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGold);
#endif // __AEGON_THEME_SWITCHER
		}
	}
}

void CWndMgr::PutCoolTime( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit )
{
	CString strTemp;
	DWORD dwGroup = pMover->m_cooltimeMgr.GetGroup( pItemElem->GetProp() );
	if( dwGroup )
	{
		DWORD dwCur = g_tmCurrent;
		DWORD dwEnd = pMover->m_cooltimeMgr.GetTime( dwGroup );		// 이벤트 종료 시각 
		if( dwEnd != 0 && dwEnd > dwCur )
		{
			DWORD dwBase = pMover->m_cooltimeMgr.GetBase( dwGroup );	// 이벤트 시작 시각 
			
			CTimeSpan ct( (dwEnd - dwCur + 500) / 1000 );		// 남은시간을 초단위로 변환해서 넘겨줌, +500 반올림 
			strTemp.Format( prj.GetText(TID_TOOLTIP_COOLTIME), ct.GetMinutes(), ct.GetSeconds() );		// 남은시간을 분/초 형태로 출력.
			pEdit->AddString( "\n" );
#ifdef __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
#endif // __AEGON_THEME_SWITCHER	
		}
	}
}

void CWndMgr::PutKeepTime( CItemElem* pItemElem, CEditString* pEdit )
{
	CString str, strTemp;
	if( pItemElem->GetProp()->dwCircleTime != 0xffffffff )
	{
		pEdit->AddString( "\n" );
		if( pItemElem->GetProp()->dwCircleTime == 1 )
		{
			pEdit->AddString( prj.GetText( TID_GAME_COND_USE ), dwItemColor[g_Option.m_nToolTipText].dwTime ); // 사망시 효과 적용
		}
		else
		{
			CTimeSpan ct( pItemElem->GetProp()->dwCircleTime );
			strTemp.Format( "%s: ", prj.GetText( TID_TOOLTIP_ITEMTIME ) );
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime );
			if (ct.GetDays() >= 1)
			{
				strTemp.Format("%d Day(s) %d Hour(s)", static_cast<int>(ct.GetDays()), ct.GetHours());
			}
			else if (ct.GetHours() >= 1)
			{
				strTemp.Format("%d Hr(s) %d Min(s)", ct.GetHours(), ct.GetMinutes());
			}
			else if (ct.GetMinutes() >= 1)
			{
				strTemp.Format("%d Min(s) %d Sec(s)", ct.GetMinutes(), ct.GetSeconds());
			}
			else if (ct.GetSeconds() >= 1)
			{
				strTemp.Format("%d Sec(s)", ct.GetSeconds());
			}
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime ); 
		}
	}

	time_t t	= pItemElem->m_dwKeepTime - time_null();

#if __VER >= 15 // __PETVIS
	//비스 남은 시간 표시( 인벤에 있는,, 아직 사용전이므로 서버데이터가 없다. Prop에서 가져다 쓴다 ( 단위는 분 ) 2009_11_10 
	if( t <= 0  && pItemElem->GetProp()->IsVis( ) )		// 장착된 비스만 유지시간을 받게 되고, 인벤에 위치한 비스는 Prop에서 가져다 써야 함.
	{
		pItemElem->m_dwKeepTime = pItemElem->GetProp()->dwAbilityMin;
		t = (time_t)( pItemElem->m_dwKeepTime * 60.0f );		//분단위 로 바뀜 
	}
#endif

	if( pItemElem->m_dwKeepTime && !pItemElem->IsFlag( CItemElem::expired ) )
	{
		if( t > 0 )
		{
			CTimeSpan time( t );
			if( pItemElem->GetProp()->dwItemKind3 == IK3_TICKET )
			{
#if __VER >= 11 // __CHIPI_071210
				str.Format( prj.GetText( TID_TOOLTIP_DATE ), static_cast<int>(time.GetDays()), time.GetHours(), time.GetMinutes(), time.GetSeconds() );
#else //__CHIPI_071210
				if( t > 600 )
					str.Format( prj.GetText( TID_TOOLTIP_DATE ), static_cast<int>(time.GetDays()), time.GetHours(), time.GetMinutes(), time.GetSeconds() );
				else
					str.Format( prj.GetText( TID_TOOLTIO_PERIODOFMIN ), 10 );
#endif //__CHIPI_071210
			}
			else
			{
#if __VER >= 15 // __PETVIS
				if( pItemElem->GetProp()->IsVis( ) )		// gmpbigsun : 일, 시간, 분, 초 의 텍스트를 읽어서 알맞게 조합해준다.
				{
					CString strDays, strHours, strMinutes, strSeconds;
					if( time.GetDays() )
					{
						strDays.Format( prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(time.GetDays()) );
						strHours.Format( prj.GetText( TID_PK_LIMIT_HOUR ), time.GetHours() );
						strMinutes.Format( prj.GetText(TID_PK_LIMIT_MINUTE), time.GetMinutes() );

						str = strDays + strHours + strMinutes;
					}
					else if( time.GetHours() )
					{
						strHours.Format( prj.GetText(TID_PK_LIMIT_HOUR), time.GetHours() );
						strMinutes.Format( prj.GetText(TID_PK_LIMIT_MINUTE), time.GetMinutes() );

						str = strHours + strMinutes;
					}else if( time.GetMinutes() )
					{
						strMinutes.Format( prj.GetText( TID_PK_LIMIT_MINUTE ), time.GetMinutes() );
						strSeconds.Format( prj.GetText( TID_PK_LIMIT_SECOND ), time.GetSeconds() );

						str = strMinutes + strSeconds;
					}
					else
					{
						str.Format( prj.GetText( TID_PK_LIMIT_SECOND ), time.GetSeconds() );
					}
				}
#else
				if( 0 )
				{
				}
#endif
				else if( time.GetDays() )
					str.Format( prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(time.GetDays()+1) );
				else if( time.GetHours() )
					str.Format( prj.GetText(TID_PK_LIMIT_HOUR), time.GetHours() );
#if __VER >= 11 // __CHIPI_071210
				else if( time.GetMinutes() )
					str.Format( prj.GetText(TID_PK_LIMIT_MINUTE), time.GetMinutes() );
				else
					str.Format( prj.GetText(TID_PK_LIMIT_SECOND), time.GetSeconds() );
#else //__CHIPI_071210
				else if( time.GetMinutes() > 10 )
					str.Format( prj.GetText(TID_PK_LIMIT_MINUTE), time.GetMinutes() );
				else
					str.Format( prj.GetText(TID_TOOLTIO_PERIODOFMIN), 10 );
#endif //__CHIPI_071210
			}
		}
#if __VER < 11 // __CHIPI_071210
		else
		{
			str.Format( prj.GetText(TID_TOOLTIO_PERIODOFMIN), 10 );
		}
#endif //__CHIPI_071210
		strTemp = str + prj.GetText(TID_TOOLTIP_PERIOD);	
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime );
	}
}

void CWndMgr::PutEndurance( CItemElem* pItemElem, CEditString* pEdit, int flag )
{
#if __VER < 11 // __REMOVE_ENDURANCE
	// 내구력 나는 나오게 하고 상점 인터페이스에서는 안나오게함 56은 나의 Invantory 개수
	if(flag != APP_SHOP_ )
	{	// 내구력
		if( flag != APP_QUERYEQUIP )
		{
			if( pItemElem->GetProp()->dwEndurance != 0xffffffff )
			{
				CString strTemp;
				int nEndurance = int((float)pItemElem->m_nHitPoint * 100 / (float)pItemElem->GetProp()->dwEndurance );
				int nMaxEndurance = int((float)(pItemElem->GetProp()->nMaxRepair - pItemElem->m_nRepair) * 100 / (float)pItemElem->GetProp()->nMaxRepair );
				strTemp.Format( "%s %d%% / %d%%", prj.GetText(TID_GAME_TOOLTIP_DURABILITY), nEndurance, nMaxEndurance );
				pEdit->AddString( "\n" );
				if( nEndurance != 0 )
					pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
				else
					pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwNotUse );
			}
		}
	}	
#endif // __REMOVE_ENDURANCE
}

void CWndMgr::PutSex( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit )
{
	if( pItemElem->GetProp()->dwItemSex != 0xffffffff )
	{	// 성별
		CString strTemp;
		if( pItemElem->GetProp()->dwItemSex == 0 )	// 남
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_SEXMALE) );
		else	// 여
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_SEXFEMALE) );
		pEdit->AddString( "\n" );
		if (pItemElem->GetProp()->dwItemSex == pMover->GetSex())
		{
#ifdef __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
#endif // __AEGON_THEME_SWITCHER
		}
		else
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwNotUse ); 
	}
}

void CWndMgr::PutJob( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit )
{
	if( pItemElem->GetProp()->dwItemJob != 0xffffffff )
	{	// 직업
		CString strTemp;
		switch( pItemElem->GetProp()->dwItemJob )
		{
		case JOB_VAGRANT:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_REGVANG) );
			break;
		case JOB_MERCENARY:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_REGMERSER) );
			break;
		case JOB_ACROBAT:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ACRO) );
			break;
		case JOB_ASSIST:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ASSIST) );
			break;
		case JOB_MAGICIAN:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_MAG) );
			break;
		case JOB_PUPPETEER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_PUPPET) );
			break;

		case JOB_KNIGHT:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_KNIGHT) );
			break;
		case JOB_BLADE: 
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_BLADE) );
			break;
		case JOB_JESTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_JASTER) );
			break;
		case JOB_RANGER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RANGER) );
			break;
		case JOB_RINGMASTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RINGMAS) );
			break;
		case JOB_BILLPOSTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_BILLPOS) );
			break;
		case JOB_PSYCHIKEEPER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_PSYCHIKEEPER) );
			break;
		case JOB_ELEMENTOR:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ELEMENTOR) );
			break;
		case JOB_GATEKEEPER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_GATE) );
			break;
		case JOB_DOPPLER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_DOPPLER) );
			break;
#if __VER >= 14 // __JOB_TEXT
		case JOB_KNIGHT_MASTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_KNIGHT_MASTER) );
			break;
		case JOB_BLADE_MASTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_BLADE_MASTER) );
			break;
		case JOB_JESTER_MASTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_JESTER_MASTER) );
			break;
		case JOB_RANGER_MASTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RANGER_MASTER) );
			break;
		case JOB_RINGMASTER_MASTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RINGMASTER_MASTER) );
			break;
		case JOB_BILLPOSTER_MASTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_BILLPOSTER_MASTER) );
			break;
		case JOB_PSYCHIKEEPER_MASTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_PSYCHIKEEPER_MASTER) );
			break;
		case JOB_ELEMENTOR_MASTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ELEMENTOR_MASTER) );
			break;

		case JOB_KNIGHT_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_KNIGHT_HERO) );
			break;
		case JOB_BLADE_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_BLADE_HERO) );
			break;
		case JOB_JESTER_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_JESTER_HERO) );
			break;
		case JOB_RANGER_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RANGER_HERO) );
			break;
		case JOB_RINGMASTER_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RINGMASTER_HERO) );
			break;
		case JOB_BILLPOSTER_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_BILLPOSTER_HERO) );
			break;
		case JOB_PSYCHIKEEPER_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_PSYCHIKEEPER_HERO) );
			break;
		case JOB_ELEMENTOR_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ELEMENTOR_HERO) );
			break;
#endif // __JOB_TEXT
#ifdef __3RD_LEGEND16
		case JOB_LORDTEMPLER_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_LORDTEMPLER_HERO) );
			break;
		case JOB_STORMBLADE_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_STORMBLADE_HERO) );
			break;
		case JOB_WINDLURKER_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_WINDLURKER_HERO) );
			break;
		case JOB_CRACKSHOOTER_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_CRACKSHOOTER_HERO) );
			break;
		case JOB_FLORIST_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_FLORIST_HERO) );
			break;
		case JOB_FORCEMASTER_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_FORCEMASTER_HERO) );
			break;
		case JOB_MENTALIST_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_MENTALIST_HERO) );
			break;
		case JOB_ELEMENTORLORD_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ELEMENTORLORD_HERO) );
			break;
#endif // __3RD_LEGEND16
		} // switch
		pEdit->AddString( "\n" );
		if (pMover->IsInteriorityJob(pItemElem->GetProp()->dwItemJob))
		{
#ifdef __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
#endif // __AEGON_THEME_SWITCHER
		}
		else 
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwNotUse );
	}
}

void CWndMgr::PutLevel( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit )
{
#ifdef __LEESE_REMOVE_LVL1
	if (pItemElem->GetProp()->dwLimitLevel1 != 0xffffffff && pItemElem->GetProp()->dwLimitLevel1 != 1)
#else //__LEESE_REMOVE_LVL1
	if (pItemElem->GetProp()->dwLimitLevel1 != 0xffffffff)
#endif //__LEESE_REMOVE_LVL1
	{
		CString strTemp;
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_REQLEVEL), pItemElem->GetProp()->dwLimitLevel1 );
		pEdit->AddString( "\n" );
#if __VER >= 14 // __NEW_ITEM_LIMIT_LEVEL
		if( pItemElem->IsLimitLevel( pMover ) )
#else // __NEW_ITEM_LIMIT_LEVEL
#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
	#if __VER >= 11 // __SYS_IDENTIFY
		if( pMover->GetLevel() < pItemElem->GetLimitLevel() && !pMover->IsHero() && !pMover->IsMaster() )
	#else	// __SYS_IDENTIFY
		if( pMover->GetLevel() < pItemElem->GetProp()->dwLimitLevel1 && !pMover->IsHero() && !pMover->IsMaster() )
	#endif	// __SYS_IDENTIFY
#else //__LEGEND	//	9차 전승시스템	Neuz, World, Trans
		if( pMover->GetLevel() < pItemElem->GetProp()->dwLimitLevel1 )
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans
#endif // __NEW_ITEM_LIMIT_LEVEL
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwNotUse );
		else
		{
#ifdef __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
#endif // __AEGON_THEME_SWITCHER
		}
#if __VER >= 11 // __SYS_IDENTIFY
		if( pItemElem->GetLevelDown() != 0 )
		{
			strTemp.Format( "(%d)", pItemElem->GetLevelDown() );
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwNotUse );
		}
#endif	// __SYS_IDENTIFY
	}
}

void CWndMgr::PutWeapon( CItemElem* pItemElem, CEditString* pEdit )
{
	if( pItemElem->GetProp()->dwItemKind3 != IK3_SHIELD )
	{
		if( pItemElem->GetProp()->dwHanded != 0xffffffff )
		{ // 한손무기용 / 양손무기용
			CString strTemp;
			if( HD_ONE == pItemElem->GetProp()->dwHanded )
			{
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ONEHANDWEAPON) );
			}
			else if( HD_TWO == pItemElem->GetProp()->dwHanded )
			{
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_TWOHANDWEAPON) );
			}
			pEdit->AddString( "\n" );
#ifdef __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
			pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
#endif // __AEGON_THEME_SWITCHER
		}
	}
}

#ifdef __AEGON_FIXES
void CWndMgr::PutCommand(CItemElem* pItemElem, CEditString* pEdit, int flag)
#else
void CWndMgr::PutCommand( CItemElem* pItemElem, CEditString* pEdit )
#endif
{
	CString strTemp;

#ifdef __ITEM_COUNT_TOOLTIP
	if (pItemElem->GetProp()->dwPackMax > 1)
	{
#ifdef __AEGON_FIXES
		if (flag != APP_SHOP_)
#endif
		{
			short nItemNum = pItemElem->m_nItemNum;
			strTemp.Format("\nQuantity: %d", nItemNum);
			pEdit->AddString(strTemp, 0XFF8C8C8C);
		}
	}
#endif //__ITEM_COUNT_TOOLTIP

	if( strlen( pItemElem->GetProp()->szCommand ) )
	{
		pEdit->AddString( "\n" );

#if __VER >= 8 // __S_COUPLERING01
		if( ( pItemElem->m_dwItemId == II_GEN_WARP_COUPLERING || pItemElem->m_dwItemId == II_GEN_WARP_WEDDING_BAND || pItemElem->m_dwItemId == II_GEN_WARP_COUPLERING01 )
			&& pItemElem->GetRandomOptItemId() > 0 )
#else // __VER >= 8
		if( pItemElem->m_dwItemId == II_GEN_WARP_COUPLERING && pItemElem->GetRandomOptItemId() > 0 )
#endif // __VER >= 8 
		{
			u_long idPlayer	= (u_long)( pItemElem->GetRandomOptItemId() );
#if __VER >= 11 // __SYS_PLAYER_DATA
			const char* pszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer );
#else	// __SYS_PLAYER_DATA
			LPCSTR	pszPlayer	= prj.GetPlayerString( idPlayer );
			if( pszPlayer == NULL )
				g_DPlay.SendQueryPlayerString( idPlayer, QPS_GENERIC );
#endif	// __SYS_PLAYER_DATA
			CString strDesc;
			strDesc.Format( prj.GetText( TID_ITEM_COUPLERING_DESC ), pszPlayer? pszPlayer: "" );
			strTemp.Format( prj.GetText( TID_ITEM_INFO ), strDesc );	// 설명 :
		}
		else
		{
#ifdef __PIERCE_CARD_TOOLTIPS
			ItemProp* pProp = pItemElem->GetProp();
			if (pProp->dwItemKind3 == IK3_SOCKETCARD || pProp->dwItemKind3 == IK3_SOCKETCARD2)
			{
				PPIERCINGAVAIL ptr = CPiercingAvail::GetInstance()->GetPiercingAvail(pItemElem->m_dwItemId);
				if (ptr)
				{
					CString str;
					if (pProp->dwItemKind3 == IK3_SOCKETCARD)
						str = "Armor Piercing Card.\n";
					else
						str = "Weapon Piercing Card.\n";
					for (short i = 0; i < ptr->nSize; i++)
					{
						int nDst = ptr->anDstParam[i];
						int nAdj = ptr->anAdjParam[i];
						if (IsDst_Rate(nDst))
							strTemp.Format("%s%+d%%", FindDstString(nDst), nDst == DST_ATTACKSPEED ? nAdj / 20 : nAdj);
						else
							strTemp.Format("%s%+d", FindDstString(nDst), nAdj);
						str += strTemp + ", ";
					}
					str = str.Left(str.GetLength() - 2);
					strTemp = str;
				}
			}
			else
#endif //__PIERCE_CARD_TOOLTIPS
			strTemp.Format( prj.GetText( TID_ITEM_INFO ), pItemElem->GetProp()->szCommand );	// 설명 :
		}
#ifdef __AEGON_THEME_SWITCHER
		pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
		pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwCommand);
#endif // __AEGON_THEME_SWITCHER
		
	}

/*	if( pItemElem->GetProp()->dwID == II_SYS_SYS_SCR_AMPESA || pItemElem->GetProp()->dwID == II_SYS_SYS_SCR_AMPESB
		|| pItemElem->GetProp()->dwID == II_SYS_SYS_SCR_AMPESC || pItemElem->GetProp()->dwID == II_SYS_SYS_SCR_AMPESD )
	{
		pEdit->AddString( "\n" );
		strTemp.Format( prj.GetText( TID_GAME_EXP_LEVELUSE ) );	// 레벨에 따라 중복 가능
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwCommand );
	}
*/
}

void CWndMgr::PutToolTip_Item( CItemBase* pItemBase, CPoint point, CRect* pRect, int flag )
{
#ifdef __AEGON_FIXES
	if (!pItemBase)
		return;
#endif

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	ItemProp* pItemProp = pItemBase->GetProp();
	if( !pItemProp )		//gmpbigsun
		return;
	CEditString strItem = _T( "" );
	MakeToolTipText( pItemBase, strItem, flag );
	g_toolTip.PutToolTip( pItemBase->m_dwItemId, strItem, *pRect, point, 0 );
	if( pItemProp->IsUltimate() )
		g_toolTip.SetUltimateToolTip( pItemBase );
#ifdef __NEW_ITEM_VARUNA
	if( pItemProp->IsBaruna() )
		g_toolTip.SetBarunaToolTip( pItemBase );
#endif // __NEW_ITEM_VARUNA
#else // __IMPROVE_SYSTEM_VER15
	if( pItemBase == NULL )
		return;
	ItemProp *pItemProp = pItemBase->GetProp();
	if( pItemProp == NULL )
	{
		LPCTSTR szErr = Error( "PutToolTip_Item : 프로퍼티가 널 %d", pItemBase->m_dwItemId );
		ADDERRORMSG( szErr );
		assert( 0 );
		return;
	}
	
	CMover* pMover = g_pPlayer;
	if( flag == APP_QUERYEQUIP )
	{
		CWndQueryEquip * pWndQueryEquip = (CWndQueryEquip *)g_WndMng.GetWndBase( APP_QUERYEQUIP );
		if( pWndQueryEquip )
			pMover = pWndQueryEquip->GetMover();
	}

	if( pMover == NULL )
		return;

	CItemElem* pItemElem	= (CItemElem*)pItemBase;

	CString str;
	CString strTemp;
	CString strEnter = '\n';
	CEditString strEdit;	// 전체를 저장하는 String

	DWORD dwColorBuf;
	dwColorBuf = PutItemName( pItemElem, &strEdit );
	PutItemAbilityPiercing( pItemElem, &strEdit, dwColorBuf );
#if __VER >= 14 // __EQUIP_BIND
	if( pItemElem->GetProp() && pItemElem->GetProp()->dwFlag & IP_FLAG_EQUIP_BIND )
	{
		strEdit.AddString( "\n" );
		if( pItemElem->IsFlag( CItemElem::binds ) )
			strEdit.AddString( GETTEXT( TID_TOOLTIP_EQUIPBIND_AFTER ), GETTEXTCOLOR( TID_TOOLTIP_EQUIPBIND_AFTER ) );
		else
			strEdit.AddString( GETTEXT( TID_TOOLTIP_EQUIPBIND_BEFORE ), GETTEXTCOLOR( TID_TOOLTIP_EQUIPBIND_BEFORE ) );
	}
#endif // __EQUIP_BIND
	PutWeapon( pItemElem, &strEdit );
	PutSex( pMover, pItemElem, &strEdit );
	
#ifdef __EATPET_STATS
	if (pItemElem->IsEatPet())
		PutBaseItemOpt(pItemElem, &strEdit);
#endif // __EATPET_STATS

	switch( pItemProp->dwItemKind2 )
	{
		case IK2_WEAPON_DIRECT:
		case IK2_WEAPON_MAGIC:
		case IK2_ARMORETC:
		case IK2_CLOTHETC:
		case IK2_ARMOR:
		case IK2_CLOTH:
		case IK2_BLINKWING:
		{
			PutItemMinMax( pMover, pItemElem, &strEdit );
			PutItemSpeed( pItemElem, &strEdit );
			if( pItemProp->dwItemKind3 == IK3_ELECARD )
				PutItemResist( pItemElem, &strEdit );
			else if( pItemElem->m_nResistAbilityOption && ( pItemProp->dwItemKind1 == IK1_WEAPON  || pItemProp->dwItemKind1 == IK1_ARMOR ) )
				PutItemResist( pItemElem, &strEdit );

			PutBaseResist( pItemElem, &strEdit );	// 속성 저항력
			
			PutBaseItemOpt( pItemElem, &strEdit );
#if __VER < 12 // __EXT_PIERCING
#if __VER >= 9 // __CSC_VER9_1
			if( (pItemProp->dwItemKind2 == IK2_WEAPON_DIRECT || pItemProp->dwItemKind2 == IK2_WEAPON_MAGIC) && 
				pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE )
				PutAddedOpt( pItemElem, &strEdit );
#endif //__CSC_VER9_1
			PutPiercingOpt( pItemElem, &strEdit );
#endif //__EXT_PIERCING
			PutRandomOpt( pItemElem, &strEdit );			
			PutEnchantOpt( pMover, pItemElem, &strEdit, flag );
			break;
		}
	case IK2_REFRESHER:
	case IK2_FOOD:
	case IK2_POTION:
		{
			PutMedicine( pItemElem, pItemElem->GetProp()->dwDestParam1,  pItemProp->nAdjParamVal1, &strEdit );
			PutMedicine( pItemElem, pItemElem->GetProp()->dwDestParam2,  pItemProp->nAdjParamVal2, &strEdit );
			break;
		}
	case IK2_JEWELRY:
		{
			PutBaseItemOpt( pItemElem, &strEdit );
			break;
		}
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	case IK2_SYSTEM:
		{
#if __VER >= 15 // __PETVIS
			if( pItemProp->dwItemKind3 == IK3_VIS )
			{
				PutNeededVis( pItemElem, &strEdit );
				PutBaseItemOpt( pItemElem, &strEdit );
			}
#endif
			
			if( pItemElem->m_dwItemId == II_SYS_SYS_SCR_SEALCHARACTER )
			{
				PutSealChar( pItemElem, &strEdit );
			}
			break;
		}
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

	case IK2_GMTEXT:
		{
			if( pItemProp->dwItemKind3 == IK3_TEXT_DISGUISE )
			{
				PutDestParam( pItemProp->dwDestParam[0], pItemProp->dwDestParam[1],
					pItemProp->nAdjParamVal[0], pItemProp->nAdjParamVal[1], strEdit );

			#if __VER >= 8 //__Y_FLAG_SKILL_BUFF
				if( pItemProp->dwActiveSkill != NULL_ID )
				{
					ItemProp *pSkillProp;
					AddSkillProp* pAddSkillProp;
					pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, pItemProp->dwActiveSkill, 1, "CWndMgr::PutToolTip_Item" );
					
					if( pAddSkillProp )
					{
						PutDestParam( pAddSkillProp->dwDestParam[0], pAddSkillProp->dwDestParam[1],
							pAddSkillProp->nAdjParamVal[0], pAddSkillProp->nAdjParamVal[1], strEdit );
					}
				}
			#endif //__Y_FLAG_SKILL_BUFF		
				
				
			}
			break;
		}
	default:
		{
			break;
		}
	}	// switch( pItemProp->dwItemKind2 )

#if __VER >= 8 //__Y_FLAG_SKILL_BUFF
	if( pItemElem->IsEatPet() )
	{
		if( pItemProp->dwActiveSkill != NULL_ID )
		{
			ItemProp *pSkillProp;
			AddSkillProp* pAddSkillProp;
			pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, pItemProp->dwActiveSkill, 1, "CWndMgr::PutToolTip_Item" );
			
			if( pAddSkillProp )
			{
				PutDestParam( pAddSkillProp->dwDestParam[0], pAddSkillProp->dwDestParam[1],
					pAddSkillProp->nAdjParamVal[0], pAddSkillProp->nAdjParamVal[1], strEdit );
			}
		}
	}
#endif //__Y_FLAG_SKILL_BUFF

#if __VER >= 15 // __PETVIS
	if( pItemElem->IsVisPet( ) )
		PutVisPetInfo( pItemElem, &strEdit );
#endif

	PutCoolTime( pMover, pItemElem, &strEdit );			// 쿨타임
	PutEndurance( pItemElem, &strEdit, flag );			// 내구력
	PutKeepTime( pItemElem, &strEdit );					// 사용할수 있는 시간
	PutJob( pMover, pItemElem, &strEdit );
	PutLevel( pMover, pItemElem, &strEdit );	
	PutCommand( pItemElem, &strEdit );					// 용도 
	PutItemGold( pMover, pItemElem, &strEdit, flag );	// 가격
#ifdef __LOOKCHANGE
	PutLookText(pItemElem, &strEdit);
#endif // __LOOKCHANGE
#ifdef __BLING_BLING
	PutBlingBlingText(pItemElem, &strEdit);
#endif // __BLING_BLING
#ifdef __CRYSTAL
	PutCrystalText(pItemElem, &strEdit);
#endif // __CRYSTAL
#ifdef __WIKIPEDIA_ITEMS
	if (flag == APP_WIKI_ITEMS)
		PutSetItemOptWiki(pMover, pItemElem, &strEdit);
	else
#endif //__WIKIPEDIA_ITEMS
	PutSetItemOpt( pMover, pItemElem, &strEdit );
#if __VER >= 9 // __CSC_VER9_1
	if( pItemProp->dwItemKind3 == IK3_EGG && pItemElem->m_pPet )//&& pItemElem->m_pPet->GetLevel() != PL_EGG )
		PutPetInfo( pItemElem, &strEdit );
	if( pItemProp->dwID == II_SYS_SYS_SCR_PET_FEED_POCKET ) //먹이 주머니 툴팁
		PutPetFeedPocket( pItemElem, &strEdit );
#endif //__CSC_VER9_1
#if __VER >= 12 // __EXT_PIERCING
	PutPiercingOpt( pItemElem, &strEdit );
#endif //__EXT_PIERCING
#if __VER >= 11 // __SYS_IDENTIFY
	PutAwakeningBlessing( pItemElem, &strEdit );
#endif	// __SYS_IDENTIFY
#if __VER >= 12 // __EXT_PIERCING
	if( (pItemProp->dwItemKind2 == IK2_WEAPON_DIRECT || pItemProp->dwItemKind2 == IK2_WEAPON_MAGIC) && 
		pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE )
		PutAddedOpt( pItemElem, &strEdit );
#endif //__EXT_PIERCING
	g_toolTip.PutToolTip( pItemBase->m_dwItemId, strEdit, *pRect, point, 0 );
#if __VER >= 9 // __CSC_VER9_1
	if( pItemProp->IsUltimate() )
		g_toolTip.SetUltimateToolTip(pItemBase);
#endif //__CSC_VER9_1
#ifdef __NEW_ITEM_VARUNA
	if (pItemProp->IsBaruna())
		g_toolTip.SetBarunaToolTip(pItemBase);
#endif // __NEW_ITEM_VARUNA
#endif // __IMPROVE_SYSTEM_VER15
}

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
void CWndMgr::PutToolTipItemAndSubToolTip( CItemBase* pItemBase, CPoint point, CRect* pRect, int nFlag )
{
	// 다중 툴팁 시스템이 적용되면서 툴팁 출력 전에 툴팁 스위치를 미리 세팅해서 위치를 조정
	g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_MAIN );
	PutToolTipParts( pItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_MAIN );
	ItemProp* pItemProp = pItemBase->GetProp();
	switch( pItemProp->dwItemKind2 )
	{
	case IK2_WEAPON_DIRECT:
	case IK2_WEAPON_MAGIC:
	case IK2_ARMOR:
	case IK2_ARMORETC:
	case IK2_CLOTH:
	case IK2_CLOTHETC:
	case IK2_JEWELRY:
	case IK2_RIDING:
	case IK2_CLOTHWIG:
		{
			CItemBase* pEquipItemBase = g_pPlayer->GetEquipItem( pItemProp->dwParts );
			if( pEquipItemBase )
			{
				g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
				PutToolTipParts( pEquipItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
			}
			if( pItemProp->dwParts == PARTS_RWEAPON )
			{
				CItemBase* pRWeaponItemBase = g_pPlayer->GetEquipItem( PARTS_RWEAPON );
				CItemBase* pLWeaponItemBase = g_pPlayer->GetEquipItem( PARTS_LWEAPON );
				if( pRWeaponItemBase && pLWeaponItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB2 );
					PutToolTipParts( pLWeaponItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB2 );
				}
				else if( pRWeaponItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
					PutToolTipParts( pRWeaponItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
				}
				else if( pLWeaponItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
					PutToolTipParts( pLWeaponItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
				}
			}
			else if( pItemProp->dwParts == PARTS_RING1 )
			{
				CItemBase* pRing1ItemBase = g_pPlayer->GetEquipItem( PARTS_RING1 );
				CItemBase* pRing2ItemBase = g_pPlayer->GetEquipItem( PARTS_RING2 );
				if( pRing1ItemBase && pRing2ItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB2 );
					PutToolTipParts( pRing2ItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB2 );
				}
				else if( pRing1ItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
					PutToolTipParts( pRing1ItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
				}
				else if( pRing2ItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
					PutToolTipParts( pRing2ItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
				}
			}
			else if( pItemProp->dwParts == PARTS_EARRING1 )
			{
				CItemBase* pEarring1ItemBase = g_pPlayer->GetEquipItem( PARTS_EARRING1 );
				CItemBase* pEarring2ItemBase = g_pPlayer->GetEquipItem( PARTS_EARRING2 );
				if( pEarring1ItemBase && pEarring2ItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB2 );
					PutToolTipParts( pEarring2ItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB2 );
				}
				else if( pEarring1ItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
					PutToolTipParts( pEarring1ItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
				}
				else if( pEarring2ItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
					PutToolTipParts( pEarring2ItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
				}
			}
			break;
		}
	}
}

void CWndMgr::PutToolTipParts( CItemBase* pPartsItemBase, CPoint point, CRect* pRect, const int nFlag, const int nSubToolTipFlag )
{
	if( pPartsItemBase == NULL )
		return;

	ItemProp* pPartsItemProp = pPartsItemBase->GetProp();
	CEditString strEquipItem = _T( "" );
	if( nSubToolTipFlag != 0 )
		PutEquipItemText( &strEquipItem );
	MakeToolTipText( pPartsItemBase, strEquipItem, nFlag );
	switch( nSubToolTipFlag )
	{
	case 0:
		{
			g_toolTip.PutToolTipEx( pPartsItemBase->m_dwItemId, strEquipItem, *pRect, point, 0, nSubToolTipFlag );
			if( pPartsItemProp->IsUltimate() )
				g_toolTip.SetUltimateToolTip( pPartsItemBase );
#ifdef __NEW_ITEM_VARUNA
			if( pPartsItemProp->IsBaruna() )
				g_toolTip.SetBarunaToolTip( pPartsItemBase );
#endif // __NEW_ITEM_VARUNA
			break;
		}
	case 1:
		{
			g_toolTipSub1.PutToolTipEx( pPartsItemBase->m_dwItemId, strEquipItem, *pRect, point, 0, nSubToolTipFlag );
			if( pPartsItemProp->IsUltimate() )
				g_toolTipSub1.SetUltimateToolTip( pPartsItemBase );
#ifdef __NEW_ITEM_VARUNA
			if( pPartsItemProp->IsBaruna() )
				g_toolTipSub1.SetBarunaToolTip( pPartsItemBase );
#endif // __NEW_ITEM_VARUNA
			break;
		}
	case 2:
		{
			g_toolTipSub2.PutToolTipEx( pPartsItemBase->m_dwItemId, strEquipItem, *pRect, point, 0, nSubToolTipFlag );
			if( pPartsItemProp->IsUltimate() )
				g_toolTipSub2.SetUltimateToolTip( pPartsItemBase );
#ifdef __NEW_ITEM_VARUNA
			if( pPartsItemProp->IsBaruna() )
				g_toolTipSub2.SetBarunaToolTip( pPartsItemBase );
#endif // __NEW_ITEM_VARUNA
			break;
		}
	}
}

void CWndMgr::MakeToolTipText( CItemBase* pItemBase, CEditString& strEdit, int flag )
{
	if( pItemBase == NULL )
		return;
	
	ItemProp *pItemProp = pItemBase->GetProp();
	if( pItemProp == NULL )
	{
		LPCTSTR szErr = Error( "PutToolTip_Item : 프로퍼티가 널 %d", pItemBase->m_dwItemId );
		ADDERRORMSG( szErr );
		assert( 0 );
		return;
	}
	
	CMover* pMover = g_pPlayer;
	if( flag == APP_QUERYEQUIP )
	{
		CWndQueryEquip * pWndQueryEquip = (CWndQueryEquip *)g_WndMng.GetWndBase( APP_QUERYEQUIP );
		if( pWndQueryEquip )
			pMover = pWndQueryEquip->GetMover();
	}

	if( pMover == NULL )
		return;

	CItemElem* pItemElem	= (CItemElem*)pItemBase;

	CString str = _T( "" );
	CString strTemp = _T( "" );
	CString strEnter = '\n';

	DWORD dwColorBuf;
	dwColorBuf = PutItemName( pItemElem, &strEdit );

	PutItemAbilityPiercing( pItemElem, &strEdit, dwColorBuf );

#ifdef __STAFF_ITEM_IDANDIK_TOOLTIP
	if (g_pPlayer->IsAuthHigher(AUTH_ADMINISTRATOR))
	{
		CString tmp;
		ItemProp* pProp = pItemElem->GetProp();
		tmp.Format("\nItem-ID: %d", pProp->dwID);
		strEdit.AddString(tmp, COLOR_PLACEHOLDER);
	}
#endif	//__STAFF_ITEM_IDANDIK_TOOLTIP

#ifdef __AEGON_WEBSHOP_FLAG
	if (pItemElem->IsFlag(CItemElem::webshop) && pItemElem->m_dwItemId != II_SYS_VOTECOIN)
	{
		strTemp.Format("\n[Cash Shop]");
		strEdit.AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_PURPLE), ESSTY_BOLD);
	}
#endif //__AEGON_WEBSHOP_FLAG

#ifdef __TITANIUM_TOOLTIP_OPT
	if (pItemElem->IsFlag(CItemElem::binds) && (pItemElem->m_dwItemId != II_GEN_MAT_ORICHALCUM01_1 && pItemElem->m_dwItemId != II_GEN_MAT_MOONSTONE_1) && pItemElem->GetProp()->dwFlag != IP_FLAG_EQUIP_BIND)
	{
		strTemp.Format("\n[Untradeable]");
#ifdef __AEGON_THEME_SWITCHER
		strEdit.AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor(), ESSTY_BOLD);
#else // __AEGON_THEME_SWITCHER
		strEdit.AddString(strTemp, COLOR_BLACK, ESSTY_BOLD);
#endif // __AEGON_THEME_SWITCHER
	}
#endif //__TITANIUM_TOOLTIP_OPT

#if __VER >= 15 // __PETVIS
	PutPetKind(pItemElem, &strEdit);		//gmpbigsun : 아이템 명 다음줄에 펫 종류 ( 리어, 픽업, 버프 ) 삽입 
#endif //__PETVIS

#ifdef __VENDING_NPC
	if (pItemElem->GetProp()->dwItemKind3 == IK3_VENDING)
	{
#ifdef __AEGON_THEME_SWITCHER
		strEdit.AddString("\n[Offline Vendor]", CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else // __AEGON_THEME_SWITCHER
		strEdit.AddString("\n[Offline Vendor]");
#endif // __AEGON_THEME_SWITCHER
	}
#endif //__VENDING_NPC
#if __VER >= 14 // __EQUIP_BIND
	if( pItemElem->GetProp() && pItemElem->GetProp()->dwFlag & IP_FLAG_EQUIP_BIND )
	{
		strEdit.AddString( "\n" );
		if(pItemElem->IsFlag( CItemElem::binds ) )
			strEdit.AddString( GETTEXT( TID_TOOLTIP_EQUIPBIND_AFTER ), GETTEXTCOLOR( TID_TOOLTIP_EQUIPBIND_AFTER ) );
		else
			strEdit.AddString( GETTEXT( TID_TOOLTIP_EQUIPBIND_BEFORE ), GETTEXTCOLOR( TID_TOOLTIP_EQUIPBIND_BEFORE ) );
	}
#endif // __EQUIP_BIND

#ifdef __UPGRADE_SUCCESS
	auto iter = prj.m_mapUpgradeSuccess.find(pItemElem->GetProp()->dwReferStat1);
	if (iter != prj.m_mapUpgradeSuccess.end())
	{
		const map<int, int>& upgrade = iter->second;
		auto iter2 = upgrade.find(pItemElem->GetAbilityOption());
		if (iter2 != upgrade.end() && iter2->second > 0)
		{
			CString str;
			str.Format("\nAttempts until upgrade: %d", iter2->second - pItemElem->m_nUpgradeTries);
			strEdit.AddString(str, GETTEXTCOLOR(TID_TOOLTIP_EQUIPBIND_AFTER));
		}
	}
#endif // __UPGRADE_SUCCESS

	PutWeapon( pItemElem, &strEdit );
	PutSex( pMover, pItemElem, &strEdit );
	
#ifdef __EATPET_STATS
	if (pItemElem->IsEatPet())
		PutBaseItemOpt(pItemElem, &strEdit);
#endif // __EATPET_STATS

	switch( pItemProp->dwItemKind2 )
	{
		case IK2_WEAPON_DIRECT:
		case IK2_WEAPON_MAGIC:
		case IK2_ARMORETC:
		case IK2_CLOTHETC:
		case IK2_ARMOR:
		case IK2_CLOTH:
		case IK2_BLINKWING:
		{
			PutItemMinMax( pMover, pItemElem, &strEdit );
			PutItemSpeed( pItemElem, &strEdit );
			if( pItemProp->dwItemKind3 == IK3_ELECARD )
				PutItemResist( pItemElem, &strEdit );
			else if( pItemElem->m_nResistAbilityOption && ( pItemProp->dwItemKind1 == IK1_WEAPON  || pItemProp->dwItemKind1 == IK1_ARMOR ) )
				PutItemResist( pItemElem, &strEdit );

			PutBaseResist( pItemElem, &strEdit );	// 속성 저항력
			
			PutBaseItemOpt( pItemElem, &strEdit );
#if __VER < 12 // __EXT_PIERCING
#if __VER >= 9 // __CSC_VER9_1
			if( (pItemProp->dwItemKind2 == IK2_WEAPON_DIRECT || pItemProp->dwItemKind2 == IK2_WEAPON_MAGIC) && 
				pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE )
				PutAddedOpt( pItemElem, &strEdit );
#endif //__CSC_VER9_1
			PutPiercingOpt( pItemElem, &strEdit );
#endif //__EXT_PIERCING
			PutRandomOpt( pItemElem, &strEdit );			
			PutEnchantOpt( pMover, pItemElem, &strEdit, flag );
			break;
		}
	case IK2_REFRESHER:
	case IK2_FOOD:
	case IK2_POTION:
		{
			PutMedicine( pItemElem, pItemElem->GetProp()->dwDestParam1,  pItemProp->nAdjParamVal1, &strEdit );
			PutMedicine( pItemElem, pItemElem->GetProp()->dwDestParam2,  pItemProp->nAdjParamVal2, &strEdit );
#ifdef __V19_FORMATS
			PutMedicine( pItemElem, pItemElem->GetProp()->dwDestParam3,  pItemProp->nAdjParamVal3, &strEdit );
			PutMedicine( pItemElem, pItemElem->GetProp()->dwDestParam4,  pItemProp->nAdjParamVal4, &strEdit );
			PutMedicine( pItemElem, pItemElem->GetProp()->dwDestParam5,  pItemProp->nAdjParamVal5, &strEdit );
			PutMedicine( pItemElem, pItemElem->GetProp()->dwDestParam6,  pItemProp->nAdjParamVal6, &strEdit );
#endif //__V19_FORMATS
			break;
		}
	case IK2_JEWELRY:
#ifdef __SYS_GLYPHS
	case IK2_BULLET:
#endif //__SYS_GLYPHS
		{
			PutBaseItemOpt( pItemElem, &strEdit );
			break;
		}
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	case IK2_SYSTEM:
		{
#if __VER >= 15 // __PETVIS
			if( pItemProp->dwItemKind3 == IK3_VIS )
			{
				PutNeededVis( pItemElem, &strEdit );
				PutBaseItemOpt( pItemElem, &strEdit );
			}
#endif
			
			if( pItemElem->m_dwItemId == II_SYS_SYS_SCR_SEALCHARACTER )
			{
				PutSealChar( pItemElem, &strEdit );
			}
			break;
		}
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

#ifdef __ANARCHY_SYSTEM
	case IK2_BUFF:
	case IK2_BUFF2:
		PutSkillTime(pItemElem, &strEdit, flag);
		PutCooldown(pItemElem, &strEdit, flag);
		PutBaseItemOpt(pItemElem, &strEdit);
	break;
#endif // __ANARCHY_SYSTEM

	case IK2_GMTEXT:
		{
			if( pItemProp->dwItemKind3 == IK3_TEXT_DISGUISE )
			{
				PutDestParam( pItemProp->dwDestParam[0], pItemProp->dwDestParam[1],
					pItemProp->nAdjParamVal[0], pItemProp->nAdjParamVal[1], strEdit );

			#if __VER >= 8 //__Y_FLAG_SKILL_BUFF
				if( pItemProp->dwActiveSkill != NULL_ID )
				{
					ItemProp *pSkillProp;
					AddSkillProp* pAddSkillProp;
					pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, pItemProp->dwActiveSkill, 1, "CWndMgr::PutToolTip_Item" );
					
					if( pAddSkillProp )
					{
						PutDestParam( pAddSkillProp->dwDestParam[0], pAddSkillProp->dwDestParam[1],
							pAddSkillProp->nAdjParamVal[0], pAddSkillProp->nAdjParamVal[1], strEdit );
					}
				}
			#endif //__Y_FLAG_SKILL_BUFF		
				
				
			}
			break;
		}
	default:
		{
			break;
		}
	}	// switch( pItemProp->dwItemKind2 )

#if __VER >= 8 //__Y_FLAG_SKILL_BUFF
	if( pItemElem->IsEatPet() )
	{
		if( pItemProp->dwActiveSkill != NULL_ID )
		{
			ItemProp *pSkillProp;
			AddSkillProp* pAddSkillProp;
			pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, pItemProp->dwActiveSkill, 1, "CWndMgr::PutToolTip_Item" );
			
			if( pAddSkillProp )
			{
				PutDestParam( pAddSkillProp->dwDestParam[0], pAddSkillProp->dwDestParam[1],
					pAddSkillProp->nAdjParamVal[0], pAddSkillProp->nAdjParamVal[1], strEdit );
			}
		}
	}
#endif //__Y_FLAG_SKILL_BUFF

#if __VER >= 15 // __PETVIS
	
	if( pItemElem->IsVisPet( ) )
	{
		PutVisPetInfo( pItemElem, &strEdit );
	}
#endif

#ifdef __VENDING_NPC
	if (pItemProp->dwItemKind3 == IK3_VENDING)
	{
		PutSkillTime(pItemElem, &strEdit, flag);
	}
#endif // __VENDING_NPC

	PutCoolTime( pMover, pItemElem, &strEdit );			// 쿨타임
	PutEndurance( pItemElem, &strEdit, flag );			// 내구력
	PutKeepTime( pItemElem, &strEdit );					// 사용할수 있는 시간
	PutJob( pMover, pItemElem, &strEdit );
	PutLevel( pMover, pItemElem, &strEdit );	
#ifdef __AEGON_FIXES
	PutCommand(pItemElem, &strEdit, flag);					// 용도 
#else
	PutCommand( pItemElem, &strEdit );					// 용도 
#endif
	PutItemGold( pMover, pItemElem, &strEdit, flag );	// 가격
#ifdef __LOOKCHANGE
	PutLookText(pItemElem, &strEdit);
#endif // __LOOKCHANGE
#ifdef __CRYSTAL
	PutCrystalText(pItemElem, &strEdit);
#endif // __CRYSTAL
#ifdef __BLING_BLING
	PutBlingBlingText(pItemElem, &strEdit);
#endif // __BLING_BLING
#ifdef __WIKIPEDIA_ITEMS
	if (flag == APP_WIKI_ITEMS)
		PutSetItemOptWiki(pMover, pItemElem, &strEdit);
	else
#endif //__WIKIPEDIA_ITEMS
	PutSetItemOpt( pMover, pItemElem, &strEdit );
#if __VER >= 9 // __CSC_VER9_1
	if( pItemProp->dwItemKind3 == IK3_EGG && pItemElem->m_pPet )//&& pItemElem->m_pPet->GetLevel() != PL_EGG )
		PutPetInfo( pItemElem, &strEdit );
	if( pItemProp->dwID == II_SYS_SYS_SCR_PET_FEED_POCKET ) //먹이 주머니 툴팁
		PutPetFeedPocket( pItemElem, &strEdit );
#endif //__CSC_VER9_1
#if __VER >= 12 // __EXT_PIERCING
	PutPiercingOpt( pItemElem, &strEdit );
#endif //__EXT_PIERCING
#if __VER >= 11 // __SYS_IDENTIFY
	PutAwakeningBlessing( pItemElem, &strEdit );
#endif	// __SYS_IDENTIFY
#if __VER >= 12 // __EXT_PIERCING
	if( (pItemProp->dwItemKind2 == IK2_WEAPON_DIRECT || pItemProp->dwItemKind2 == IK2_WEAPON_MAGIC) && 
		pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE )
		PutAddedOpt( pItemElem, &strEdit );
#endif //__EXT_PIERCING

#ifdef __MOUNT
	if (pItemElem->GetProp()->dwItemKind3 == IK3_MOUNT)
	{
		if (pItemElem->GetProp()->dwID > 0)
		{
			MOUNT_ANIMATION* pMountAnimation = prj.GetMountAnimation(pItemElem->GetProp()->dwID);
			if (pMountAnimation)
			{
				if (pMountAnimation->fGroundSpeed > 0.0f)
				{
					strEdit.AddString("\n");
					//strEdit.AddString(prj.GetText(TID_MOUNT_MODE_03));
					strEdit.AddString(_T("Mount Walk Speed"), CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
					strEdit.AddString(" ");

					if (pMountAnimation->fGroundSpeed <= 0.12f)
						strEdit.AddString(prj.GetText(TID_GAME_VERYSLOW), COLOR_ERROR, ESSTY_UNDERLINE);
					else if (pMountAnimation->fGroundSpeed <= 0.16f)
						strEdit.AddString(prj.GetText(TID_GAME_SLOW), 0xFFFF9900, ESSTY_UNDERLINE);
					else if (pMountAnimation->fGroundSpeed <= 0.24f)
						strEdit.AddString(prj.GetText(TID_GAME_NORMALS), 0xFFF2C900, ESSTY_UNDERLINE);
					else if (pMountAnimation->fGroundSpeed <= 0.32f)
						strEdit.AddString(prj.GetText(TID_GAME_FAST), 0xFF43E000, ESSTY_UNDERLINE);
					else if (pMountAnimation->fGroundSpeed <= 0.40f)
						strEdit.AddString(prj.GetText(TID_GAME_VERYFAST), 0xFF0094FF, ESSTY_UNDERLINE);
					else
						strEdit.AddString(prj.GetText(TID_GAME_FASTEST), COLOR_ITEM_BG, ESSTY_UNDERLINE);
				}
				if (pMountAnimation->fFlightSpeed > 0.0f)
				{
					strEdit.AddString("\n");
					//strEdit.AddString(prj.GetText(TID_MOUNT_MODE_04));
					strEdit.AddString(_T("Mount Fly Speed"), CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
					strEdit.AddString(" ");

					if (pMountAnimation->fFlightSpeed <= 0.12f)
						strEdit.AddString(prj.GetText(TID_GAME_VERYSLOW), COLOR_ERROR, ESSTY_UNDERLINE);
					else if (pMountAnimation->fFlightSpeed <= 0.16f)
						strEdit.AddString(prj.GetText(TID_GAME_SLOW), 0xFFFF9900, ESSTY_UNDERLINE);
					else if (pMountAnimation->fFlightSpeed <= 0.24f)
						strEdit.AddString(prj.GetText(TID_GAME_NORMALS), 0xFFF2C900, ESSTY_UNDERLINE);
					else if (pMountAnimation->fFlightSpeed <= 0.32f)
						strEdit.AddString(prj.GetText(TID_GAME_FAST), 0xFF43E000, ESSTY_UNDERLINE);
					else if (pMountAnimation->fFlightSpeed <= 0.40f)
						strEdit.AddString(prj.GetText(TID_GAME_VERYFAST), 0xFF0094FF, ESSTY_UNDERLINE);
					else
						strEdit.AddString(prj.GetText(TID_GAME_FASTEST), COLOR_ITEM_BG, ESSTY_UNDERLINE);

					strEdit.AddString("\n");
					//strEdit.AddString(prj.GetText(TID_MOUNT_MODE_05), 0xFF00FF90);
					strEdit.AddString(_T("This is a flying mount!"), CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
				}
			}
		}
	}
#endif // __MOUNT

#if defined (__ITEMLINK) || defined (__SYS_LOCK_ITEM)
	if (flag == APP_INVENTORY && !pMover->m_Inventory.IsEquip(pItemElem->m_dwObjId))
	{
#ifdef __ITEMLINK
		str.Format("\n[CTRL & Click to Link]");
		strEdit.AddString(str, COLOR_ITEMINFO);
#endif //__ITEMLINK
#ifdef __SYS_LOCK_ITEM
		str.Format("\n[Shift & R-Click to Lock]");
		strEdit.AddString(str, COLOR_ITEMINFO);
#endif //__SYS_LOCK_ITEM
	}
#endif //defined (__ITEMLINK) || defined (__SYS_LOCK_ITEM)

#ifdef __TITANIUM_TOOLTIP_OPT
	if (flag == APP_SHOPEX)
	{
		if (pItemElem->GetProp()->dwItemKind3 == IK3_MOUNT || pItemElem->GetProp()->dwItemKind3 == IK3_HAT || pItemElem->GetProp()->dwItemKind3 == IK3_MASK
			|| pItemElem->GetProp()->dwItemKind3 == IK3_SHOES || pItemElem->GetProp()->dwItemKind3 == IK3_CLOAK || pItemElem->GetProp()->dwItemKind3 == IK3_CLOTH
			|| pItemElem->GetProp()->dwItemKind3 == IK3_GLOVE || pItemElem->GetProp()->dwItemKind3 == IK3_SHIELD || pItemElem->GetProp()->dwItemKind3 == IK3_HELMET
			|| pItemElem->GetProp()->dwItemKind3 == IK3_SUIT || pItemElem->GetProp()->dwItemKind3 == IK3_GAUNTLET || pItemElem->GetProp()->dwItemKind3 == IK3_BOOTS
			|| pItemElem->GetProp()->dwItemKind3 == IK3_CLOAK

			|| pItemElem->GetProp()->dwItemKind3 == IK3_SWD || pItemElem->GetProp()->dwItemKind3 == IK3_AXE || pItemElem->GetProp()->dwItemKind3 == IK3_CHEERSTICK
			|| pItemElem->GetProp()->dwItemKind3 == IK3_KNUCKLEHAMMER || pItemElem->GetProp()->dwItemKind3 == IK3_WAND || pItemElem->GetProp()->dwItemKind3 == IK3_STAFF
			|| pItemElem->GetProp()->dwItemKind3 == IK3_THSWD || pItemElem->GetProp()->dwItemKind3 == IK3_THAXE || pItemElem->GetProp()->dwItemKind3 == IK3_VIRTUAL
			|| pItemElem->GetProp()->dwItemKind3 == IK3_YOYO || pItemElem->GetProp()->dwItemKind3 == IK3_BOW || pItemElem->GetProp()->dwItemKind3 == IK3_YOBO
			|| pItemElem->GetProp()->dwItemKind3 == IK3_CROSSBOW
			)
		{
			str.Format("\n[Shift & Click to View Model]");
			strEdit.AddString(str, COLOR_ITEMINFO);
		}
	}
#endif //__TITANIUM_TOOLTIP_OPT

#ifdef __WEAPON_MERGE
	PutMergeWeaponText(pItemElem, &strEdit);
#endif //__WEAPON_MERGE
#ifdef __PROPPACK_TOOLTIP
	PutPackItemInfo(pItemElem, &strEdit);
#endif //__PROPPACK_TOOLTIP
}
#endif // __IMPROVE_SYSTEM_VER15

void CWndMgr::PutToolTip_Character( int SelectCharacter, CPoint point, CRect* pRect )
{
#ifdef __AEGON_THEME_SWITCHER
	char statebuf[30] = { 0, };
	CEditString strEdit;
	CString string;
	DWORD dwTextColor = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();

	// Name
	if (g_Neuz.m_nCharacterBlock[SelectCharacter])
		//		string.Format( "#b#cff2e9200%s#nc#nb", g_Neuz.m_apPlayer[SelectCharacter]->GetName() );
		string.Format("%s", g_Neuz.m_apPlayer[SelectCharacter]->GetName());
	else
		string.Format("%s\nConnet Cut", g_Neuz.m_apPlayer[SelectCharacter]->GetName());

	strEdit.AddParsingString(string.GetString(), CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE), ESSTY_BOLD);

	// Sex
	if (g_Neuz.m_apPlayer[SelectCharacter]->GetSex())
		string.Format("\n%s", prj.GetText(TID_GAME_TOOLTIP_SEXFEMALE));
	else
		string.Format("\n%s", prj.GetText(TID_GAME_TOOLTIP_SEXMALE));
	strEdit.AddParsingString(string.GetString(), dwTextColor);

	// Level
	sprintf(statebuf, prj.GetText(TID_GAME_TOOL_EX_LEVEL), g_Neuz.m_apPlayer[SelectCharacter]->m_nLevel);
	if (g_Neuz.m_apPlayer[SelectCharacter]->IsMaster())
	{
		string.Format("\n%s %s", statebuf, prj.GetText(TID_GAME_TOOLTIP_MARK_MASTER));
	}
	else if (g_Neuz.m_apPlayer[SelectCharacter]->IsHero())
	{
		string.Format("\n%s %s", statebuf, prj.GetText(TID_GAME_TOOLTIP_MARK_HERO));
	}
	else if (g_Neuz.m_apPlayer[SelectCharacter]->IsLegendHero())
	{
		string.Format("\n%s (L)", statebuf);
	}
	else
	{
		string.Format("\n%s", statebuf);

	}
	strEdit.AddParsingString(string.GetString(), dwTextColor);

	// Str
	sprintf(statebuf, prj.GetText(TID_GAME_TOOL_EX_STR), g_Neuz.m_apPlayer[SelectCharacter]->m_nStr);
	string.Format("\n%s", statebuf);
	strEdit.AddParsingString(string.GetString(), dwTextColor);

	// Sta 
	sprintf(statebuf, prj.GetText(TID_GAME_TOOL_EX_STA), g_Neuz.m_apPlayer[SelectCharacter]->m_nSta);
	string.Format("\n%s", statebuf);
	strEdit.AddParsingString(string.GetString(), dwTextColor);

	// Dex
	sprintf(statebuf, prj.GetText(TID_GAME_TOOL_EX_DEX), g_Neuz.m_apPlayer[SelectCharacter]->m_nDex);
	string.Format("\n%s", statebuf);
	strEdit.AddParsingString(string.GetString(), dwTextColor);

	// Int
	sprintf(statebuf, prj.GetText(TID_GAME_TOOL_EX_INT), g_Neuz.m_apPlayer[SelectCharacter]->m_nInt);
	string.Format("\n%s", statebuf);
	strEdit.AddParsingString(string.GetString(), dwTextColor);

	// Job
	sprintf(statebuf, "%s: %s", prj.GetText(TID_APP_CHARACTER_JOB), prj.m_aJob[g_Neuz.m_apPlayer[SelectCharacter]->m_nJob].szName);
	string.Format("\n%s", statebuf);
	strEdit.AddParsingString(string.GetString(), dwTextColor);

	g_toolTip.PutToolTip(SelectCharacter, strEdit, *pRect, point, 3);
#else // __AEGON_THEME_SWITCHER
	CString string;
	char statebuf[30] = {0,};
	// 블록 상태( 블록상태면 빨간색 )
	if( g_Neuz.m_nCharacterBlock[SelectCharacter] )
//		string.Format( "#b#cff2e9200%s#nc#nb", g_Neuz.m_apPlayer[SelectCharacter]->GetName() );
		string.Format( "#b#cff0000ff%s#nc#nb", g_Neuz.m_apPlayer[SelectCharacter]->GetName() );
	else
		string.Format( "#b#cffff0000%s\nConnet Cut#nc#nb", g_Neuz.m_apPlayer[SelectCharacter]->GetName() );
	// 성별
	if(	g_Neuz.m_apPlayer[SelectCharacter]->GetSex() )
		sprintf( statebuf, prj.GetText(TID_GAME_TOOLTIP_SEXFEMALE) );
	else
		sprintf( statebuf, prj.GetText(TID_GAME_TOOLTIP_SEXMALE) );
	string += '\n'; string += statebuf;

	// 레벨
#if __VER >= 10 // __LEGEND
	if(g_Neuz.m_apPlayer[SelectCharacter]->IsMaster())
	{
		sprintf( statebuf, prj.GetText(TID_GAME_TOOL_EX_LEVEL), g_Neuz.m_apPlayer[SelectCharacter]->m_nLevel );
		string += '\n'; string += statebuf;
		string += " ";
		string += prj.GetText( TID_GAME_TOOLTIP_MARK_MASTER );		
	}
	else if(g_Neuz.m_apPlayer[SelectCharacter]->IsHero())
	{
		sprintf( statebuf, prj.GetText(TID_GAME_TOOL_EX_LEVEL), g_Neuz.m_apPlayer[SelectCharacter]->m_nLevel );
		string += '\n'; string += statebuf;
		string += " ";
#if __VER >= 15 // __HERO129_VER15				// 15차 히어로 레벨확장
		string += prj.GetText( TID_GAME_TOOLTIP_MARK_HERO );
#else	// 15차 히어로 레벨확장
		int nlength = string.GetLength();
		string.Delete(nlength-3, 3);
		string += prj.GetText( TID_GAME_TOOLTIP_MARK_HERO_BEFORE );
#endif	// 15차 히어로 레벨확장
	}
#ifdef __3RD_LEGEND16
	else if (g_Neuz.m_apPlayer[SelectCharacter]->IsLegendHero())
	{
		sprintf(statebuf, prj.GetText(TID_GAME_TOOL_EX_LEVEL), g_Neuz.m_apPlayer[SelectCharacter]->m_nLevel);
		string += '\n'; string += statebuf;
		string += " ";
#if __VER >= 15 // __HERO129_VER15				// 15차 히어로 레벨확장
		string += "(L)";
#else	// 15차 히어로 레벨확장
		int nlength = string.GetLength();
		string.Delete(nlength - 3, 3);
		string += prj.GetText(TID_GAME_TOOLTIP_MARK_HERO_BEFORE);
#endif	// 15차 히어로 레벨확장
	}
#endif // __3RD_LEGEND16
	else
	{
		sprintf( statebuf, prj.GetText(TID_GAME_TOOL_EX_LEVEL), g_Neuz.m_apPlayer[SelectCharacter]->m_nLevel);
		string += '\n'; string += statebuf;
	}
#else //__LEGEND
	sprintf( statebuf, prj.GetText(TID_GAME_TOOL_EX_LEVEL), g_Neuz.m_apPlayer[SelectCharacter]->m_nLevel);
	string += '\n'; string += statebuf;
#endif //__LEGEND

	// Str
	sprintf( statebuf, prj.GetText(TID_GAME_TOOL_EX_STR), g_Neuz.m_apPlayer[SelectCharacter]->m_nStr);
	string += '\n'; string += statebuf;
	
	// Sta 
	sprintf( statebuf, prj.GetText(TID_GAME_TOOL_EX_STA), g_Neuz.m_apPlayer[SelectCharacter]->m_nSta);
	string += '\n'; string += statebuf;
	
	// Dex
	sprintf( statebuf, prj.GetText(TID_GAME_TOOL_EX_DEX), g_Neuz.m_apPlayer[SelectCharacter]->m_nDex);
	string += '\n'; string += statebuf;
	
	// Int
	sprintf( statebuf, prj.GetText(TID_GAME_TOOL_EX_INT), g_Neuz.m_apPlayer[SelectCharacter]->m_nInt);
	string += '\n'; string += statebuf;
				
	// 직업
	sprintf( statebuf, "%s: %s", prj.GetText(TID_APP_CHARACTER_JOB), prj.m_aJob[ g_Neuz.m_apPlayer[SelectCharacter]->m_nJob ].szName );
	string += '\n'; string += statebuf;

	CEditString strEdit;
	strEdit.SetParsingString( string );
	g_toolTip.PutToolTip( SelectCharacter, strEdit, *pRect, point, 3 );
#endif // __AEGON_THEME_SWITCHER
}

// 숏컷으로 스킬 실행하기.
void CWndMgr::UseSkillShortCut( DWORD dwType, DWORD dwSkillIdx )
{
	LPSKILL lpSkill = g_pPlayer->GetSkill( dwType, dwSkillIdx );
	if( lpSkill )
	{
		CCtrl* pCtrl = (CCtrl*)g_WorldMng.Get()->GetObjFocus();
		DWORD dwObjId = NULL_ID;
		if( pCtrl && pCtrl->GetType() != OT_OBJ )
			dwObjId = pCtrl->GetId();

		{
			// 스킬바 사용중일때는 핫키로 스킬사용 안됨. / 타겟이 무버여야만함.
			if( ((CWndTaskBar *)g_WndMng.m_pWndTaskBar)->m_nExecute == 0 && (pCtrl == NULL || (pCtrl && pCtrl->GetType() == OT_MOVER)) )	
			{
//				if( ((CWndTaskBar *)g_WndMng.m_pWndTaskBar)->m_nExecute )	// 단축키로 스킬을 실행했을때 액션슬롯으로 실행상태였다면 그것을 취소시킴.
//					((CWndTaskBar *)g_WndMng.m_pWndTaskBar)->OnCancelSkill();
				int nResult = 0;
				
				if( g_pPlayer->IsBullet( lpSkill->GetProp() ) == FALSE )
				{
					return;
				}

				nResult = g_pPlayer->CMD_SetUseSkill( dwObjId, dwSkillIdx );

				if( nResult == 0 )
				{
					g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );
				}

				if( nResult )		// 실행할 명령을 셋팅. 이동 + 스킬사용이 합쳐진 명령.
				{
					g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );
				} else
				// 공격중이라 스킬 사용이 씹혔으면
				if( g_pPlayer->m_pActMover->IsActAttack() )
				{
					// 다음 턴때 이 스킬을 예약시켜둠.
					g_WndMng.m_pWndWorld->SetNextSkill( dwSkillIdx );
				}				
			}
				
		}
	}
}

BOOL CWndMgr::ScreenCapture()
{
	static long	bmpcnt = 0;
	char	filename[ _MAX_PATH ] = "";
	FILE	*fp;

#if __VER >= 11 // __CAPTURE_JPG
	{
		CreateDirectory( "Capture", NULL );
		while(1)
		{
			sprintf( filename, "Capture\\flyff%05d.jpg", bmpcnt );
			if( (fp = fopen(filename,"r")) == NULL ) break; else fclose(fp);
			bmpcnt ++;
		}
	}
	sprintf( filename, "Capture\\flyff%05d.jpg", bmpcnt );
	SaveJPG( filename );
	CString string;
	string.Format( prj.GetText(TID_ADMIN_CAPTUREJPG), bmpcnt++ );
	PutString( string, NULL, prj.GetTextColor(TID_ADMIN_CAPTUREJPG) );
#else
	{
		CreateDirectory( "Capture", NULL );
		while(1)
		{
			sprintf( filename, "Capture\\flyff%05d.bmp", bmpcnt );
			if( (fp = fopen(filename,"r")) == NULL ) break; else fclose(fp);
			bmpcnt ++;
		}
	}
	sprintf( filename, "Capture\\flyff%05d.bmp", bmpcnt );

	SaveBitmap( filename );
	CString string;
	string.Format( prj.GetText(TID_ADMIN_SCREENSHOTSAVE), bmpcnt++ );
	PutString( string, NULL, prj.GetTextColor(TID_ADMIN_SCREENSHOTSAVE) );
#endif
	
				
	return TRUE;
}

#if __VER >= 11 // __CAPTURE_JPG
BOOL CWndMgr::SaveJPG( LPCTSTR lpszName )
{
	IDirect3DSurface9* pDestSurface;

	CTime ctTime = CTime::GetCurrentTime();
	char lpString[260]	= { 0, };
	sprintf( lpString, prj.GetText( TID_GAME_SCREENSHOT_TIME ), ctTime.GetYear(), ctTime.GetMonth(), ctTime.GetDay(), ctTime.GetHour(), ctTime.GetMinute(), ctTime.GetSecond() );
	CRect rect = g_Neuz.GetDeviceRect();
	CSize size = m_p2DRender->m_pFont->GetTextExtent( lpString );
	g_Neuz.m_2DRender.TextOut(rect.right - size.cx - 20, rect.bottom - 70, lpString);

	m_pApp->m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pDestSurface );
	BOOL bResult = D3DXSaveSurfaceToFile(lpszName, D3DXIFF_JPG, pDestSurface, NULL, NULL );
	pDestSurface->Release();

	return bResult;
}
#endif

BOOL CWndMgr::SaveBitmap( LPCTSTR lpszName )
{
	LPBYTE  lpData = new BYTE[ g_Option.m_nResWidth * g_Option.m_nResHeight * 4 ];
	LPDWORD lpdwData = (LPDWORD) lpData;
	memset( lpData, 255, g_Option.m_nResWidth * g_Option.m_nResHeight * 4 );
	
	IDirect3DSurface9* pDestSurface;
	m_pApp->m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pDestSurface );

	D3DLOCKED_RECT lockedRect;
	CRect rectIn ( 0, 0, m_pApp->m_d3dsdBackBuffer.Width, m_pApp->m_d3dsdBackBuffer.Height );//cb.bih.biWidth, abs(cb.bih.biHeight) );
	HRESULT hr = pDestSurface->LockRect( &lockedRect, NULL, 0 ) ;
	if( hr == D3D_OK )// Lock
	{
		LPBYTE pSrcByte = (LPBYTE)lockedRect.pBits; 
		LPWORD pSrcWord = (LPWORD)lockedRect.pBits; // 16bit
		//LPDWORD pSrcDWord = (LPDWORD)lockedRect.pBits; // 32bit

		DWORD dwOffset = 0;
		DWORD dwBit = 0;
		switch( g_Neuz.m_d3dpp.BackBufferFormat )
		{
			case D3DFMT_R8G8B8: dwBit = 24; break; // 20 24-bit RGB pixel format with 8 bits per channel. 
			case D3DFMT_A8R8G8B8: dwBit = 32; break; //  21 32-bit ARGB pixel format with alpha, using 8 bits per channel. 
			case D3DFMT_X8R8G8B8: dwBit = 32; break; //  22 32-bit RGB pixel format, where 8 bits are reserved for each color. 
			case D3DFMT_R5G6B5: dwBit = 16; break; //  23 16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue. 
			case D3DFMT_X1R5G5B5: dwBit = 16; break; //  24 16-bit pixel format where 5 bits are reserved for each color. 
			case D3DFMT_A1R5G5B5: dwBit = 16; break; //  25 16-bit pixel format where 5 bits are reserved for each color and 1 bit is reserved for alpha. 
			case D3DFMT_A4R4G4B4: dwBit = 16; break; //  26 16-bit ARGB pixel format with 4 bits for each channel. 
			case D3DFMT_R3G3B2: dwBit = 8; break; //  27 8-bit RGB texture format using 3 bits for red, 3 bits for green, and 2 bits for blue. 
			case D3DFMT_A8: dwBit = 8; break; //  28 8-bit alpha only. 
			case D3DFMT_A8R3G3B2: dwBit = 16; break; //  29 16-bit ARGB texture format using 8 bits for alpha, 3 bits each for red and green, and 2 bits for blue. 
			case D3DFMT_X4R4G4B4: dwBit = 16; break; //  30 16-bit RGB pixel format using 4 bits for each color. 
			case D3DFMT_A2B10G10R10: dwBit = 32; break; //  31 32-bit pixel format using 10 bits for each color and 2 bits for alpha. 
			case D3DFMT_A8B8G8R8: dwBit = 32; break; //  32 32-bit ARGB pixel format with alpha, using 8 bits per channel. 
			case D3DFMT_X8B8G8R8: dwBit = 32; break; //  33 32-bit RGB pixel format, where 8 bits are reserved for each color. 
			case D3DFMT_G16R16: dwBit = 32; break; //  34 32-bit pixel format using 16 bits each for green and red. 
			case D3DFMT_A2R10G10B10: dwBit = 32; break; //  35 32-bit pixel format using 10 bits each for red, green, and blue, and 2 bits for alpha. 
			case D3DFMT_A16B16G16R16: dwBit = 64; break; //  36 64-bit pixel format using 16 bits for each component. 
			case D3DFMT_A8P8: dwBit = 8; break; //  40 8-bit color indexed with 8 bits of alpha. 
			case D3DFMT_P8: dwBit = 8; break; //  41 8-bit color indexed. 
			case D3DFMT_L8: dwBit = 8; break; //  50 8-bit luminance only. 
			case D3DFMT_L16: dwBit = 16; break; //  81 16-bit luminance only. 
			case D3DFMT_A8L8: dwBit = 16; break; //  51 16-bit using 8 bits each for alpha and luminance. 
			case D3DFMT_A4L4: dwBit = 8; break; //  8-bit using 4 bits each for alpha and luminance.
		}	
		if( dwBit == 16 || dwBit == 32 )
		{
			int nPitch = lockedRect.Pitch;
			if( dwBit == 32 )
			{
				for(int y = 0; y < g_Option.m_nResHeight; y++ )
				{
					for(int x = 0; x < g_Option.m_nResWidth; x++, dwOffset += 3 )
					{
						// 32 bit color code
						BYTE byData3 = pSrcByte[ y * nPitch + ( x * 4 ) + 0 ];
						BYTE byData2 = pSrcByte[ y * nPitch + ( x * 4 ) + 1 ];
						BYTE byData1 = pSrcByte[ y * nPitch + ( x * 4 ) + 2 ];
						lpData[ dwOffset + 0 ] = byData3;
						lpData[ dwOffset + 1 ] = byData2;
						lpData[ dwOffset + 2 ] = byData1;
					}
				}
			}
			else
			{
				nPitch /= 2;
				for(int y = 0; y < g_Option.m_nResHeight; y++ )
				{
					for(int x = 0; x < g_Option.m_nResWidth; x++, dwOffset += 3 )
					{
						WORD wData = pSrcWord[ y * nPitch + x ];
							
						BYTE byData1 = ( wData & 0xf800 ) >> 8;
						BYTE byData2 = ( wData & 0x07e0 ) >> 3;
						BYTE byData3 = ( wData & 0x001f ) << 3 ;
						
						lpData[ dwOffset + 0 ] = byData3;
						lpData[ dwOffset + 1 ] = byData2;
						lpData[ dwOffset + 2 ] = byData1;
					}
				}
			}
		}
		pDestSurface->UnlockRect();
	}
	
	pDestSurface->Release();
	BOOL bResult = SaveBMP( lpszName, lpData,CSize( g_Option.m_nResWidth, g_Option.m_nResHeight ) );
	SAFE_DELETE_ARRAY( lpData );
	return bResult;
}
CWndMap* CWndMgr::OpenMap( LPCTSTR lpszMapFileName )
{
	CWndMap* pWndMap = NULL;
	if( m_mapMap.Lookup( lpszMapFileName, (void*&)pWndMap ) == FALSE )
	{
		pWndMap = new CWndMap;
		strcpy( pWndMap->m_szMapFile, lpszMapFileName );
		pWndMap->Initialize();
		m_mapMap.SetAt( lpszMapFileName, pWndMap );
	}
	return pWndMap;
}

CCollectingWnd* CWndMgr::OpenCollecting()
{

	if(!m_pWndCollecting)
	{
		m_pWndCollecting = new CCollectingWnd;
		m_pWndCollecting->Initialize();
	}

	return m_pWndCollecting;
}


BOOL CWndMgr::CloseCollecting()
{

	if(m_pWndCollecting)
	{
		m_pWndCollecting->Destroy();
		SAFE_DELETE(m_pWndCollecting);
		m_pWndCollecting = NULL;
		return TRUE;
	}

	return FALSE;
}


CWndMessage* CWndMgr::GetMessage( LPCTSTR lpszFrom )
{
	CString string;
	CWndMessage* pWndMessage = NULL;
	m_mapMessage.Lookup( lpszFrom, (void*&)pWndMessage );
	return pWndMessage;
}
#ifdef __AEGON_MESSENGER
CWndAegonMessenger* CWndMgr::OpenMessenger(BOOL bMinimized)
{
	CWndAegonMessenger* pWndMessenger = (CWndAegonMessenger*)g_WndMng.GetApplet(APP_AEGON_MESSENGER);
	if (!pWndMessenger)
	{
		pWndMessenger = (CWndAegonMessenger*)g_WndMng.CreateApplet(APP_AEGON_MESSENGER);
		if (bMinimized)
		{
			pWndMessenger->OnMinimizeToggle();
			pWndMessenger->MoveTopLeft();
		}
	}
	else if (pWndMessenger->m_nMinimizedMode != __AEG_MINIMIZE_MODE_NONE && !bMinimized)
	{
		pWndMessenger->OnMinimizeToggle();
	}

	return pWndMessenger;
}
#endif
CWndMessage* CWndMgr::OpenMessage( LPCTSTR lpszFrom )
{
#ifdef __AEGON_MESSENGER
	if (IsValidObj(g_pPlayer) && strlen(lpszFrom) > 0)
	{
		CWndAegonMessenger* pWndMessenger = (CWndAegonMessenger*)g_WndMng.GetApplet(APP_AEGON_MESSENGER);
		if (!pWndMessenger)
		{
			pWndMessenger = (CWndAegonMessenger*)g_WndMng.CreateApplet(APP_AEGON_MESSENGER);
		}
		if (pWndMessenger)
		{
			if (pWndMessenger->m_nMinimizedMode != __AEG_MINIMIZE_MODE_NONE)
			{
				pWndMessenger->OnMinimizeToggle();
			}

			pWndMessenger->AddChatUnit(lpszFrom, -1);
			pWndMessenger->SwitchToChat(lpszFrom);

		}		
	}
	return NULL;
#else
	CString string;
	CWndMessage* pWndMessage = NULL;
	if( m_mapMessage.Lookup( lpszFrom, (void*&)pWndMessage ) == FALSE )
	{
		pWndMessage = new CWndMessage;
		pWndMessage->Initialize();
		m_mapMessage.SetAt( lpszFrom, pWndMessage );
		string = pWndMessage->GetTitle();
		string += " - ";
		string += lpszFrom;
		pWndMessage->SetTitle( string );
		pWndMessage->m_strPlayer = lpszFrom;
	}
	else
	{
		pWndMessage->InitSize();
	}
	return pWndMessage;
#endif
}

BOOL CWndMgr::UpdateMessage( LPCTSTR pszOld, LPCTSTR pszNew )
{
	CWndMessage* pWndMessage	= NULL;
	if( m_mapMessage.Lookup( pszOld, (void*&)pWndMessage ) )
	{
		m_mapMessage.RemoveKey( pWndMessage->m_strPlayer );
		m_mapMessage.SetAt( pszNew, pWndMessage );
		CString string	= pWndMessage->GetTitle();
		string.Replace( pszOld, pszNew );
		pWndMessage->SetTitle( string );
		pWndMessage->m_strPlayer	= pszNew;
		return TRUE;
	}
	return FALSE;
}

CWndInstantMsg* CWndMgr::GetInstantMsg( LPCTSTR lpszFrom )
{
	CString string;
	CWndInstantMsg* pWndMessage = NULL;
	m_mapInstantMsg.Lookup( lpszFrom, (void*&)pWndMessage );
	return pWndMessage;
}
CWndInstantMsg* CWndMgr::OpenInstantMsg( LPCTSTR lpszFrom )
{
	CString string;
	CWndInstantMsg* pWndMessage = NULL;
	if( m_mapInstantMsg.Lookup( lpszFrom, (void*&)pWndMessage ) == FALSE )
	{
		CWndBase* pWndFocus = GetFocusWnd();
		pWndMessage = new CWndInstantMsg;
		pWndMessage->Initialize();
		pWndFocus->SetFocus();
		pWndMessage->m_strPlayer = lpszFrom;
		int nNumber = m_mapInstantMsg.GetCount();
		CRect rcMsg = pWndMessage->GetWindowRect();
		CRect rcWnd = GetLayoutRect();

		int nNumHeight = rcWnd.Height() / rcMsg.Height();
		int nNumWidth = rcWnd.Width() / rcMsg.Width();

		int nCount = ( nNumber / nNumHeight ) + 1; // +1안해주면 화면을 벗어난 곳에 열림 
		int nCount2 = ( nNumber % nNumHeight ) + 1;

		CPoint pt( rcWnd.right - ( rcMsg.Width() * nCount ) , rcWnd.bottom - ( rcMsg.Height() * nCount2 ) );
 		pWndMessage->Move( pt );
		m_mapInstantMsg.SetAt( lpszFrom, pWndMessage );
		
	}
	return pWndMessage;
}
// 레지스트 인포를 얻는다.
LPWNDREGINFO CWndMgr::GetRegInfo( DWORD dwWndId  )
{
	LPWNDREGINFO pWndRegInfo = NULL;
	m_mapWndRegInfo.Lookup( dwWndId, (void*&)pWndRegInfo );
	return pWndRegInfo;
}		
// 컬런트 윈도가 닫힐 때 호출 됨 
BOOL CWndMgr::PutRegInfo( CWndNeuz* pWndNeuz, BOOL bOpen )
{
	LPWNDREGINFO pWndRegInfo = NULL;
	BOOL bLookup = FALSE;
	if( m_mapWndRegInfo.Lookup( pWndNeuz->GetWndId(), (void*&)pWndRegInfo ) == TRUE )
	{
		SAFE_DELETE( pWndRegInfo->lpArchive );
		pWndRegInfo->dwSize = 0;
		bLookup = TRUE;
	}
	else
		pWndRegInfo = new WNDREGINFO;
	pWndRegInfo->dwWndId = pWndNeuz->GetWndId();
	pWndRegInfo->rect = pWndNeuz->GetWindowRect( TRUE );
	pWndRegInfo->bOpen = bOpen;
	pWndRegInfo->dwWndSize = pWndNeuz->m_nWinSize;
	pWndRegInfo->dwVersion = 0;
	CAr ar;
	int nSize;
	LPBYTE lpData;
	// Write
	pWndNeuz->SerializeRegInfo( ar, pWndRegInfo->dwVersion );
	lpData = ar.GetBuffer( &nSize );
	if( nSize )
	{
		pWndRegInfo->lpArchive = new BYTE[ nSize ];
		pWndRegInfo->dwSize = nSize;
		memcpy( pWndRegInfo->lpArchive, lpData, nSize );
	}
	else
	{
		pWndRegInfo->lpArchive = NULL;
		pWndRegInfo->dwSize = 0;
	}
	if( bLookup == FALSE )
		m_mapWndRegInfo.SetAt( pWndNeuz->GetWndId(), pWndRegInfo );
	return TRUE;
}
// 로드한 레지스트리인포를 세트할때 
BOOL CWndMgr::PutRegInfo( LPWNDREGINFO lpRegInfo )
{
	LPWNDREGINFO pWndRegInfo = NULL;
	if( m_mapWndRegInfo.Lookup( lpRegInfo->dwWndId, (void*&)pWndRegInfo ) == TRUE )
	{
		// 갱신 
		SAFE_DELETE( pWndRegInfo->lpArchive );
		memcpy( pWndRegInfo, lpRegInfo, sizeof( WNDREGINFO ) );
		if( lpRegInfo->dwSize )
		{
			pWndRegInfo->lpArchive = new BYTE[ lpRegInfo->dwSize ];
			memcpy( pWndRegInfo->lpArchive, lpRegInfo->lpArchive, lpRegInfo->dwSize );
		}
		else
			pWndRegInfo->lpArchive = NULL;
		return FALSE;
	}
	pWndRegInfo = new WNDREGINFO;
	memcpy( pWndRegInfo, lpRegInfo, sizeof( WNDREGINFO ) );
	if( lpRegInfo->dwSize )
	{
		pWndRegInfo->lpArchive = new BYTE[ lpRegInfo->dwSize ];
		memcpy( pWndRegInfo->lpArchive, lpRegInfo->lpArchive, lpRegInfo->dwSize );
	}
	else
		pWndRegInfo->lpArchive = NULL;
	m_mapWndRegInfo.SetAt( lpRegInfo->dwWndId, pWndRegInfo );
	return TRUE;
}
BOOL CWndMgr::SaveRegInfo( LPCTSTR lpszFileName )
{
	CFileIO file;
	if( file.Open( lpszFileName, "wb" ) == FALSE )
		return FALSE;
	POSITION pos = m_mapWndRegInfo.GetStartPosition();
	LPWNDREGINFO pWndRegInfo = NULL;
	//CRect* pRect;
	DWORD dwWndId;
	file.PutDW( REG_VERSION );
	file.PutDW( m_mapWndRegInfo.GetCount() );
	while( pos )
	{
		m_mapWndRegInfo.GetNextAssoc( pos, dwWndId, (void*&)pWndRegInfo );
		file.Write( &pWndRegInfo->dwWndId, sizeof( pWndRegInfo->dwWndId ) );
		file.Write( &pWndRegInfo->rect, sizeof( pWndRegInfo->rect ) );
		file.Write( &pWndRegInfo->dwWndSize, sizeof( pWndRegInfo->dwWndSize ) );
		file.Write( &pWndRegInfo->bOpen, sizeof( pWndRegInfo->bOpen ) );
		file.Write( &pWndRegInfo->dwVersion, sizeof( pWndRegInfo->dwVersion ) );
		file.Write( &pWndRegInfo->dwSize, sizeof( pWndRegInfo->dwSize ) );
		if( pWndRegInfo->dwSize )
			file.Write( pWndRegInfo->lpArchive, pWndRegInfo->dwSize );
	}
	file.Close();
	// resolution 파일 만들기. 현재 사이즈로 저장
	if( file.Open( "resolution", "wb" ) == FALSE )
		return FALSE;
	file.PutDW( g_Option.m_nResWidth );
	file.PutDW( g_Option.m_nResHeight );
	file.Close();
	return TRUE;
}
BOOL CWndMgr::LoadRegInfo( LPCTSTR lpszFileName )
{
	CFileIO file;
	BOOL bResult = FALSE;
	if( file.Open( "resolution", "rb" ) )
	{
		int nWidth = file.GetDW();
		int nHeight = file.GetDW();
		file.Close();
		// 해상도가 같으면 변경이 없는 상태다. 레지스트 파일 읽기 
		if( g_Option.m_nResWidth == nWidth && g_Option.m_nResHeight == nHeight )
		{
			bResult = TRUE;
			// 사이즈 저장 파일 로드 
			if( file.Open( lpszFileName, "rb" ) == FALSE )
				return FALSE;
			DWORD dwRegVersion = file.GetDW();
			if( dwRegVersion != REG_VERSION )
				return FALSE;
			WNDREGINFO wndRegInfo;
			//CWndBase* pWndBase; 
			int nNum = file.GetDW();
			for( int i = 0; i < nNum; i++ )
			{
				file.Read( &wndRegInfo.dwWndId, sizeof( wndRegInfo.dwWndId ) );
				file.Read( &wndRegInfo.rect, sizeof( wndRegInfo.rect ) );
				file.Read( &wndRegInfo.dwWndSize, sizeof( wndRegInfo.dwWndSize ) );
				file.Read( &wndRegInfo.bOpen, sizeof( wndRegInfo.bOpen ) );
				file.Read( &wndRegInfo.dwVersion, sizeof( wndRegInfo.dwVersion ) );
				file.Read( &wndRegInfo.dwSize, sizeof( wndRegInfo.dwSize ) );
				if( wndRegInfo.dwSize )
				{
					wndRegInfo.lpArchive = new BYTE[ wndRegInfo.dwSize ];
					file.Read( wndRegInfo.lpArchive, wndRegInfo.dwSize );
				}
				else
					wndRegInfo.lpArchive = NULL;
#if __VER >= 13 // __RENEW_CHARINFO
				if(wndRegInfo.rect.left < 0 )
					wndRegInfo.rect.left = 0;
				if(wndRegInfo.rect.top < 0)
					wndRegInfo.rect.top = 0;
#endif // __RENEW_CHARINFO	
				/*if( wndRegInfo.bOpen )
				{
					pWndBase = CreateApplet( wndRegInfo.dwWndId );
					if( pWndBase )
					{
						if( wndRegInfo.dwSize )
						{
							// load
							CAr ar( wndRegInfo.lpArchive, wndRegInfo.dwSize );
							((CWndNeuz*)pWndBase)->SerializeRegInfo( ar, wndRegInfo.dwVersion );
						}
						if( pWndBase->IsWndStyle( WBS_THICKFRAME ) )
						{
							if( wndRegInfo.dwWndSize == WSIZE_WINDOW )
							{
								((CWndNeuz*)pWndBase)->SetSizeWnd();
								pWndBase->SetWndRect( wndRegInfo.rect );
							}								
							if( wndRegInfo.dwWndSize == WSIZE_MAX )
								((CWndNeuz*)pWndBase)->SetSizeMax();
						}
						else
						{
							CRect wndRect = pWndBase->GetWindowRect();	
							pWndBase->SetWndRect( 
								CRect( 
								wndRegInfo.rect.left, 
								wndRegInfo.rect.top, 
								wndRegInfo.rect.left + wndRect.Width(), 
								wndRegInfo.rect.top + wndRect.Height() 
								) );
						}							
					}
				}*/
				PutRegInfo( &wndRegInfo );//.dwWndId, wndRegInfo.rect, wndRegInfo.bOpen );
				SAFE_DELETE( wndRegInfo.lpArchive );
				//PutRegInfo( wndRegInfo.dwWndId, wndRegInfo.rect, wndRegInfo.bOpen );
			}
			file.Close();
		}
	}
	return bResult;
}

void CWndMgr::SetMessengerAutoState()
{
#ifdef __RT_1025
	if( m_bConnect && m_RTMessenger.GetState() == FRS_AUTOABSENT )
#else	// __RT_1025
	if( m_bConnect && m_Messenger.GetState() == FRS_AUTOABSENT )
#endif	// __RT_1025
	{
		g_DPlay.SendSetState( FRS_ONLINE );	
		m_timerMessenger.Set( MIN( 5 ) );
	}
}

void CWndMgr::PutDestParam( DWORD dwDst1, DWORD dwDst2, DWORD dwAdj1, DWORD dwAdj2, CEditString &str )
{
	CString strTemp;
	CString strSignAdj;
	DWORD dwHighlightColorBlue = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE);
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
	if( dwDst1 != NULL_ID && dwDst1 != 0 && dwDst1 != DST_CHRSTATE )
#else
	if( dwDst1 != NULL_ID && dwDst1 != 0 )
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
	{
		int nAdj = dwAdj1;

		if(nAdj < 0)
			strSignAdj = "";
		else
			strSignAdj = " +";

#if defined (__DST_COOLTIME_DUNGEON) || defined (__DST_COLLECTOR_TIME)
		CString strCooltime, strCollectTime;
		strCooltime = " -";
		strCollectTime = " -";
#endif //

		if( IsDst_Rate(dwDst1) )
		{
			if( dwDst1 == DST_ATTACKSPEED )
				strTemp.Format( "\n%s%s%d%%", FindDstString(dwDst1), strSignAdj, static_cast< int >( dwAdj1 ) / 2 / 10 );
#ifdef __DST_COOLTIME_DUNGEON
			else if (dwDst1 == DST_COOLTIME)
				strTemp.Format("\n%s%s%d%%", FindDstString(dwDst1), strCooltime, dwAdj1);
#endif //__DST_COOLTIME_DUNGEON
#ifdef __DST_COLLECTOR_TIME
			else if (dwDst1 == DST_COLLECTOR_TIME)
				strTemp.Format("\n%s%s%d%%", FindDstString(dwDst1), strCollectTime, dwAdj1);
#endif //__DST_COLLECTOR_TIME
			else
				strTemp.Format( "\n%s%s%d%%", FindDstString(dwDst1), strSignAdj, dwAdj1 );					
		}
		else
		{
			strTemp.Format( "\n%s%s%d", FindDstString(dwDst1), strSignAdj, dwAdj1 );								
		}
#ifdef __AEGON_THEME_SWITCHER
		str.AddString(strTemp, dwHighlightColorBlue);
#else
		str.AddString(strTemp, D3DCOLOR_XRGB( 0, 0, 255 ) );
#endif
	}

#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
	if( dwDst2 != NULL_ID && dwDst2 != 0 && dwDst2 != DST_CHRSTATE )
#else
	if( dwDst2 != NULL_ID && dwDst2 != 0 )
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
	{
		int nAdj = dwAdj2;

		if(nAdj < 0)
			strSignAdj = "";
		else
			strSignAdj = " +";

#if defined (__DST_COOLTIME_DUNGEON) || defined (__DST_COLLECTOR_TIME)
		CString strCooltime, strCollectTime;
		strCooltime = " -";
		strCollectTime = " -";
#endif //

		if( IsDst_Rate(dwDst2) )
		{
			if( dwDst2 == DST_ATTACKSPEED )
				strTemp.Format( "\n%s%s%d%%", FindDstString(dwDst2), strSignAdj, static_cast< int >( dwAdj2 ) / 2 / 10 );
#ifdef __DST_COOLTIME_DUNGEON
			else if (dwDst1 == DST_COOLTIME)
				strTemp.Format("\n%s%s%d%%", FindDstString(dwDst2), strCooltime, dwAdj1);
#endif //__DST_COOLTIME_DUNGEON
#ifdef __DST_COLLECTOR_TIME
			else if (dwDst1 == DST_COLLECTOR_TIME)
				strTemp.Format("\n%s%s%d%%", FindDstString(dwDst2), strCollectTime, dwAdj1);
#endif //__DST_COLLECTOR_TIME
			else
				strTemp.Format( "\n%s%s%d%%", FindDstString(dwDst2), strSignAdj, dwAdj2 );					
		}
		else
		{
			strTemp.Format( "\n%s%s%d", FindDstString(dwDst2), strSignAdj, dwAdj2 );								
		}
#ifdef __AEGON_THEME_SWITCHER
		str.AddString(strTemp, dwHighlightColorBlue);
#else
		str.AddString(strTemp, D3DCOLOR_XRGB( 0, 0, 255 ) );
#endif
	}	
}

#if __VER >= 15 // __PETVIS
BOOL IsEquipedVis( CItemElem* pPetItem, DWORD visIndex );
BOOL IsEquipedRequireVis( CItemElem* pPetItem, DWORD visIndex, BOOL bSelfCheck );

void CWndMgr::PutNeededVis( CItemElem* pItemElem, CEditString* pEdit )
{
	//gmpbigsun: 해당 비스를 착용하기 위해 필요한 비스 정보 출력 
	ItemProp* pItemPropVis = pItemElem->GetProp();
	assert( pItemPropVis );
		
	DWORD dwNeeds[2] = { pItemPropVis->dwReferTarget1, pItemPropVis->dwReferTarget2 };

	CString strTemp;
	DWORD color = 0xffffffff;

	BYTE byState = 0;
	CItemElem* pPetItem = g_pPlayer->GetVisPetItem( );
	if( !pPetItem )
	{
		//버프펫이 활성화가 안되어있따면, 필요비스 걍 빨간색으로 출력 
		byState = FAILED_BOTH_NEEDVIS;
	}
	else
	{
		byState = g_pPlayer->IsSatisfyNeedVis( pPetItem, pItemPropVis );
	}

	if( NULL_ID != dwNeeds[0] && 0 != dwNeeds[ 0 ] )
	{
		ItemProp* pProp = prj.GetItemProp( dwNeeds[ 0 ] );		//sun!!
		strTemp.Format( "\n%s: %s", GETTEXT( TID_GAME_BUFFPET_REQUIRE ), pProp->szName ); //필요비스

#ifdef __AEGON_THEME_SWITCHER
		color = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
#else // __AEGON_THEME_SWITCHER
		color = 0xff000000;
#endif // __AEGON_THEME_SWITCHER
		if( FAILED_BOTH_NEEDVIS == byState || FAILED_1ST_NEEDVIS == byState )
			color = COLOR_ERROR;

		pEdit->AddString( strTemp, color );
	}

	if( NULL_ID != dwNeeds[1] && 0 != dwNeeds[ 1 ] )
	{
		ItemProp* pProp = prj.GetItemProp( dwNeeds[ 1 ] );
		strTemp.Format( "\n%s: %s", GETTEXT( TID_GAME_BUFFPET_REQUIRE), pProp->szName );

#ifdef __AEGON_THEME_SWITCHER
		color = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
#else // __AEGON_THEME_SWITCHER
		color = 0xff000000;
#endif // __AEGON_THEME_SWITCHER
		if( FAILED_BOTH_NEEDVIS == byState || FAILED_2ND_NEEDVIS == byState )
			color = COLOR_ERROR;
		
		pEdit->AddString( strTemp, color );
	}

}

BOOL IsEquipedRequireVis( CItemElem* pItemElem, DWORD index, BOOL bSelfCheck );
void CWndMgr::PutVisPetInfo( CItemElem* pItemElem, CEditString* pEdit )
{
	if( !pItemElem )
		return;

	//펫의 정보 (비스들의 총합 )
	map< int, int > cTotalOpt;
	map< int, int >::iterator iter;
	CString strTemp, strTemp2;
	
	static const int MAX_PROP = 3;

	int availableSlot = pItemElem->GetPiercingSize( );
	
	// 장착된 모든 비스의 능력치 더하기 
	for( int ia = 0; ia < availableSlot; ++ia )
	{
		DWORD index = pItemElem->GetPiercingItem( ia );
		ItemProp* pProp = prj.GetItemProp( index );
		if( !pProp )
			continue;

		if( time_null() >= pItemElem->GetVisKeepTime( ia ) )
			continue;

		BOOL bOK = g_pPlayer->IsSatisfyNeedVis( pItemElem, pProp );		

		if( SUCCSESS_NEEDVIS == bOK )		//OK 활성중임.
		{
			for( int iaa = 0; iaa < MAX_PROP; ++iaa )
			{
				int nDst	= (int)pProp->dwDestParam[iaa];
				if( NULL_ID == nDst )
					continue;

				int nVal	= (int)pProp->nAdjParamVal[iaa];
				iter = cTotalOpt.find( nDst );
				if( iter == cTotalOpt.end() )
					cTotalOpt.insert( map< int, int >::value_type( nDst, nVal ) );
				else cTotalOpt[ nDst ] += nVal;
			}
		}
	}

	//전부 더해진 옵션에 대해서 출력 
	for( iter = cTotalOpt.begin(); iter != cTotalOpt.end(); ++iter )
	{
		int nDst = iter->first;
		int nVal = iter->second;
		
		if( IsDst_Rate( nDst ) )
		{
			if( nDst == DST_ATTACKSPEED )
				strTemp.Format( "\n%s%+d%%", FindDstString( nDst ), nVal / 2 / 10 );
			else
				strTemp.Format( "\n%s%+d%%", FindDstString( nDst ), nVal );
		}
		else
		{
			strTemp.Format( "\n%s%+d", FindDstString( nDst ), nVal );
		}

		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwPiercing );
	}

	//장착된 비스 이름 + 남은 시간 출력 
	for( int ib = 0; ib < MAX_VIS; ++ib )
	{
		DWORD index = pItemElem->GetPiercingItem( ib );
		if( 0 != index && NULL_ID != index )
		{
			ItemProp* pProp = prj.GetItemProp( index );
			if( !pProp )
				continue;

			DWORD dwKeepTime = pItemElem->GetVisKeepTime( ib );

			time_t t = dwKeepTime - time_null();
			CTimeSpan ct( t );

			//strTemp2.Format( prj.GetText( TID_TOOLTIP_DATE ), static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	// 지속시간 : 
			
		//	strTemp2.Format( prj.GetText( TID_TOOLTIP_DATE ), static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	// 지속시간 : 
			LONG nDay	= (LONG)( ct.GetDays() );
			LONG nHour	= ct.GetHours();
			LONG nMin	= ct.GetMinutes();
			LONG nSec	= ct.GetSeconds();

			CString strDays, strHours, strMinutes, strSeconds;
			if( nDay > 0 )
			{
				strDays.Format( prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(nDay) );
				strHours.Format( prj.GetText( TID_PK_LIMIT_HOUR ), nHour );
				strMinutes.Format( prj.GetText(TID_PK_LIMIT_MINUTE), nMin );

				strTemp2 = strDays + strHours + strMinutes;
			}
			else if( nHour > 0 )
			{
				strHours.Format( prj.GetText(TID_PK_LIMIT_HOUR), nHour );
				strMinutes.Format( prj.GetText(TID_PK_LIMIT_MINUTE), nMin );

				strTemp2 = strHours + strMinutes;
			}else if( nMin > 0 )
			{
				strMinutes.Format( prj.GetText( TID_PK_LIMIT_MINUTE ), nMin );
				strSeconds.Format( prj.GetText( TID_PK_LIMIT_SECOND ), nSec );

				strTemp2 = strMinutes + strSeconds;
			}
			else if( nSec >= 0 )
			{
				strTemp2.Format( prj.GetText( TID_PK_LIMIT_SECOND ), nSec );
			}
			else 
			{
				strTemp2.Format( GETTEXT( TID_GAME_TOOLTIP_TIMEOUT ) ); //"시간 만료" 
			}

			strTemp = pProp->szName + CString(" (") + strTemp2 + CString(")");

			pEdit->AddString( "\n" );

			if( t > 0 )
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime );
			else 
				pEdit->AddString( strTemp, COLOR_ERROR );
	
		}
	}
}

void CWndMgr::PutPetKind( CItemElem* pItemElem, CEditString* pEdit )
{
	// PET 종류 입력 
	CString strTemp;
	
	if( pItemElem->IsEatPet() )
	{
		if( pItemElem->IsVisPet( ) )
		{
			strTemp.Format( "\n%s", GETTEXT(TID_TOOLTIP_PET_BUFF) );		//버프펫 
		//	pEdit->AddString( strTemp, GETTEXTCOLOR(TID_TOOLTIP_PET_BUFF) );
		}
		else 
		{
			strTemp.Format( "\n%s", GETTEXT(TID_TOOLTIP_PET_PICKUP) );	// 픽업펫
		//	pEdit->AddString( strTemp, GETTEXTCOLOR(TID_TOOLTIP_PET_PICKUP) );
		}
	}

	if( pItemElem->IsEgg( ) )
	{
		strTemp.Format( "\n%s", GETTEXT(TID_TOOLTIP_PET_REAR) );	//리어펫 
		//pEdit->AddString( strTemp, GETTEXTCOLOR(TID_TOOLTIP_PET_REAR) );
	}

#ifdef __AEGON_THEME_SWITCHER
	pEdit->AddString(strTemp, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else
	pEdit->AddString( strTemp );
#endif

#ifdef __PET_GLOW
	if (pItemElem->GetProp()->dwItemKind3 == IK3_PET && pItemElem->IsEatPet() && pItemElem->IsVisPet())
	{
		CString str;
		DWORD color;

		switch (pItemElem->m_nPetGlow)
		{
			case 60: str = prj.GetText(TID_PET_GLOW_TEXT_04); color = 0xFFa83a32; break;
			case 61: str = prj.GetText(TID_PET_GLOW_TEXT_05); color = 0xFF329ca8; break;
			case 62: str = prj.GetText(TID_PET_GLOW_TEXT_06); color = 0xFFa89232; break;
			case 63: str = prj.GetText(TID_PET_GLOW_TEXT_07); color = 0xFF3a9e8a; break;
			case 64: str = prj.GetText(TID_PET_GLOW_TEXT_08); color = 0xFF9e6f3a; break;
			case 65: str = prj.GetText(TID_PET_GLOW_TEXT_09); color = 0xFF6200a8; break;
			case 66: str = prj.GetText(TID_PET_GLOW_TEXT_10); color = 0xFFa8006a; break;
			case 67: str = prj.GetText(TID_PET_GLOW_TEXT_11); color = 0xFF00c6c9; break;
			case 68: str = prj.GetText(TID_PET_GLOW_TEXT_12); color = 0xFFc9a800; break;
			case 69: str = prj.GetText(TID_PET_GLOW_TEXT_13); color = 0xFFe89700; break;
			case 70: str = prj.GetText(TID_PET_GLOW_TEXT_14); color = 0xFF06b000; break;
			case 71: str = prj.GetText(TID_PET_GLOW_TEXT_15); color = 0xFFb00000; break;
		}

		if (pItemElem->m_nPetGlow >= 60 && pItemElem->m_nPetGlow <= 71)
		{
			strTemp.Format(prj.GetText(TID_PET_GLOW_TEXT_16), str);
			pEdit->AddString("\n", color);
			pEdit->AddString(strTemp, color, ESSTY_BOLD);
		}
	}
#endif // __PET_GLOW

}

BOOL CWndMgr::CheckConfirm( CItemBase* pItem )
{
	ItemProp* pItemProp = pItem->GetProp();
	if( !pItemProp )
		return FALSE;

	if( g_pPlayer->HasActivatedVisPet( ) )
	{	
		// 다음은 버프펫이 활성화 된 상태에서만 불려야 한다.
		if( pItemProp->IsVis( ) )
		{
			// gmpbigsun : 비스를 장착하기전 확인창 
			if( !g_WndMng.m_pWndBuffPetStatus )
			{
				g_WndMng.m_pWndBuffPetStatus = new CWndBuffPetStatus;
				g_WndMng.m_pWndBuffPetStatus->Initialize();	
			}
				
			g_WndMng.m_pWndBuffPetStatus->DoModal_ConfirmQuestion( pItem->m_dwObjId, g_pPlayer->GetId(), pItemProp->dwID );
			return TRUE;
		}

		if( pItemProp->IsVisKey( ) )
		{
			if( !g_WndMng.m_pWndBuffPetStatus )
			{
				g_WndMng.m_pWndBuffPetStatus = new CWndBuffPetStatus;
				g_WndMng.m_pWndBuffPetStatus->Initialize();	
			}
				
			g_WndMng.m_pWndBuffPetStatus->DoModal_ConfirmQuestion( pItem->m_dwObjId, g_pPlayer->GetId(), pItemProp->dwID, 0, CWndConfirmVis::CVS_EQUIP_VISKEY );
			return TRUE;
		}
	}

	if( pItemProp->IsPickupToBuff( ) )
	{
		// 버프펫으로 변환시키는 아이템 처리 
		// gmpbigsun : 픽업펫은 클라에서 정보를 가지고 있지 않다. 고로 사용시 확인창만 띄워주고 나머진 서버에서..
		SAFE_DELETE( g_WndMng.m_pWndConfirmVis );

		g_WndMng.m_pWndConfirmVis = new CWndConfirmVis;
		CWndConfirmVis* pWnd = g_WndMng.m_pWndConfirmVis;

		pWnd->m_dwItemId = pItem->m_dwObjId;
		pWnd->m_objid = g_pPlayer->GetId();
		pWnd->m_dwItemIndex = pItemProp->dwID;
		
		pWnd->m_eSection = CWndConfirmVis::CVS_PICKUP_TO_BUFF;		
		pWnd->Initialize();
	
		return TRUE;
	}

	//gmpbigsun : 
	//1.각성보호 확인창 으로 넘김 
	//2.확인창에서 OK시 패킷쏴줌

#ifdef __PROTECT_AWAKE
	if( II_SYS_SYS_SCR_AWAKESAFE == pItem->m_dwItemId )
	{
		//각성보호 두루마리
		g_WndMng.m_pWndConfirmVis = new CWndConfirmVis;
		CWndConfirmVis* pWnd = g_WndMng.m_pWndConfirmVis;

		pWnd->m_dwItemId = pItem->m_dwObjId;
		pWnd->m_objid = g_pPlayer->GetId();
		pWnd->m_dwItemIndex = pItemProp->dwID;
		
		pWnd->m_eSection = CWndConfirmVis::ETC_PROTECT_AWAKE;		
		pWnd->Initialize();

		return TRUE;
	}
#endif //__PROTECT_AWAKE

	return FALSE;
}
#endif //#ifdef __PETVIS

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
void CWndMgr::PutEquipItemText( CEditString* pEdit )
{
	CString strEdit = _T( "" );
	strEdit.Format( "[ %s ]\n", prj.GetText( TID_GAME_EQUIPED_ITEM ) );
#ifdef __AEGON_THEME_SWITCHER
	pEdit->AddString(strEdit, CAegonThemeSwitcherMng::GetInstance()->GetTextColor(), ESSTY_BOLD);
#else // __AEGON_THEME_SWITCHER
	pEdit->AddString( strEdit, 0xff000000, ESSTY_BOLD );
#endif // __AEGON_THEME_SWITCHER
}
#endif // __IMPROVE_SYSTEM_VER15

#ifdef __WEAPON_MERGE
void CWndMgr::MakeMergeWeaponText(CItemBase* pSource, CEditString& Edit)
{
	Edit.Empty();
	if (!pSource)
		return;
	ItemProp* pItemProp = pSource->GetProp();
	if (!pItemProp)
		return;
	CItemElem* pItemElem = (CItemElem*)pSource;

	Edit.AddString("\n", COLOR_ERROR, ESSTY_BOLD);
	Edit.AddString(prj.GetText(TID_MMI_TWOHANDEDWEAPONMERGE_RESULTTEXT01), COLOR_ERROR, ESSTY_BOLD);
	float fFactor = 0.0f;
	if (pItemElem->GetAbilityOption() == 10)
		fFactor = 0.48f;
	else if (pItemElem->GetAbilityOption() > 3)
		fFactor = 0.12f;
	DWORD dwAtkAdd = (DWORD)(pItemProp->dwAbilityMax * fFactor);
	CString strTmp;
	strTmp.Format("\nATK: #b#cffff0000%d#nc#nb", dwAtkAdd);
	Edit.AddParsingString(strTmp);
	PutPiercingOpt(pItemElem, &Edit);
	PutAwakeningBlessing(pItemElem, &Edit, false);
}
#endif //__WEAPON_MERGE

#if __VER >= 8 // __CSC_VER8_1
void CWndMgr::ClearAllWnd()
{
	CWndNeuz* pWndBase;
	DWORD dwIdApplet;
	if(!m_clearFlag)
	{
		POSITION pos = m_mapWndApplet.GetStartPosition();
		while( pos )
		{
			m_mapWndApplet.GetNextAssoc( pos, dwIdApplet, (void*&)pWndBase );
			if( pWndBase && pWndBase->GetWndId() != APP_WORLD)
			{
				m_tempWndId.push_back(pWndBase->GetWndId());
				pWndBase->SetVisible(FALSE);
			}
		}
		CWndChat* pWndChat = (CWndChat*)g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );
		if(pWndChat != NULL && pWndChat->m_bChatLog)
			m_pWndChatLog->SetVisible(FALSE);

#if __VER >= 9 // __CSC_VER9_1
		CWndPetStatus* pWndPetStatus = (CWndPetStatus*)g_WndMng.GetWndBase( APP_PET_STATUS );
		if(pWndPetStatus != NULL)
			pWndPetStatus->SetVisible(FALSE);

#if __VER >= 15 // __PETVIS
		CWndBuffPetStatus* pWndBuffPetStatus = (CWndBuffPetStatus*)g_WndMng.GetWndBase( APP_BUFFPET_STATUS );
		if(pWndBuffPetStatus != NULL)
			pWndBuffPetStatus->SetVisible(FALSE);
#endif

#if __VER >= 15 // __GUILD_HOUSE
		CWndGHMainMenu* pWndGHMain = ( CWndGHMainMenu* )g_WndMng.GetWndBase( APP_GH_MAIN );
		if( pWndGHMain )
			pWndGHMain->SetVisible( FALSE );
#endif //__GUILD_HOUSE

#endif //__CSC_VER9_1
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		CWndQuestQuickInfo* pWndQuestQuickInfo = ( CWndQuestQuickInfo* )g_WndMng.GetWndBase( APP_QUEST_QUICK_INFO );
		if( pWndQuestQuickInfo )
			pWndQuestQuickInfo->SetVisible( FALSE );
#endif // __IMPROVE_QUEST_INTERFACE

#if __VER >= 12 // __SECRET_ROOM
		CWndSecretRoomQuick* pWndSecretRoomQuick = (CWndSecretRoomQuick*)g_WndMng.GetWndBase( APP_SECRETROOM_QUICK );
		if(pWndSecretRoomQuick != NULL)
			pWndSecretRoomQuick->SetVisible(FALSE);
#endif //__SECRET_ROOM

#ifdef __SYS_PETFILTER
		CWndPetfilter* pWndPetfilter = (CWndPetfilter*)g_WndMng.GetWndBase(APP_PETFILTER);
		if (pWndPetfilter != NULL)
			pWndPetfilter->SetVisible(TRUE);
#endif //_PETFILTER

		SetFocus();
	}
	m_clearFlag = TRUE;
}

void CWndMgr::RestoreWnd()
{
	CWndNeuz* pWndBase;
	DWORD dwIdApplet;
	BOOL stopFlag = TRUE;
	int wndId;

	if(m_clearFlag)
	{
		std::list<int>::iterator it = m_tempWndId.begin();
		while(it !=  m_tempWndId.end())
		{
			POSITION pos = m_mapWndApplet.GetStartPosition();
			while( pos && stopFlag)
			{
				m_mapWndApplet.GetNextAssoc( pos, dwIdApplet, (void*&)pWndBase );
				wndId = *it;
				if(wndId == pWndBase->GetWndId())
				{
					pWndBase->SetVisible(TRUE);
					stopFlag = FALSE;
				}
			}
			it++;
			stopFlag = TRUE;
		}
		m_tempWndId.clear();
		CWndChat* pWndChat = (CWndChat*)g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );
		if(pWndChat != NULL && pWndChat->m_bChatLog)
			m_pWndChatLog->SetVisible(TRUE);
		
#if __VER >= 9 // __CSC_VER9_1
		CWndPetStatus* pWndPetStatus = (CWndPetStatus*)g_WndMng.GetWndBase( APP_PET_STATUS );
		if(pWndPetStatus != NULL)
			pWndPetStatus->SetVisible(TRUE);

#if __VER >= 15 // __PETVIS
		CWndBuffPetStatus* pWndBuffPetStatus = (CWndBuffPetStatus*)g_WndMng.GetWndBase( APP_BUFFPET_STATUS );
		if(pWndBuffPetStatus != NULL)
			pWndBuffPetStatus->SetVisible(TRUE);
#endif
#endif //__CSC_VER9_1

#if __VER >= 12 // __SECRET_ROOM
		CWndSecretRoomQuick* pWndSecretRoomQuick = (CWndSecretRoomQuick*)g_WndMng.GetWndBase( APP_SECRETROOM_QUICK );
		if(pWndSecretRoomQuick != NULL)
			pWndSecretRoomQuick->SetVisible(TRUE);
#endif //__SECRET_ROOM

#if __VER >= 15 // __GUILD_HOUSE
		CWndGHMainMenu* pWndGHMain = (CWndGHMainMenu*)g_WndMng.GetWndBase( APP_GH_MAIN );
		if(pWndGHMain != NULL)
			pWndGHMain->SetVisible(TRUE);
#endif //__GUILD_HOUSE

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		CWndQuestQuickInfo* pWndQuestQuickInfo = ( CWndQuestQuickInfo* )g_WndMng.GetWndBase( APP_QUEST_QUICK_INFO );
		if( pWndQuestQuickInfo )
			pWndQuestQuickInfo->SetVisible( TRUE );
#endif // __IMPROVE_QUEST_INTERFACE

	}
	m_clearFlag = FALSE;
}
#endif //__CSC_VER8_1

CWndBase* CWndMgr::GetWndVendorBase( void )
{
#if __VER >= 8 // __S8_VENDOR_REVISION
	return GetWndBase( APP_VENDOR_REVISION );
#else // __VER >= 8 // __S8_VENDOR_REVISION
	return GetWndBase( APP_VENDOREX );
#endif // __VER >= 8 // __S8_VENDOR_REVISION
}

#if __VER >= 12 // __CSC_VER12_4
DWORD CWndMgr::GetDSTStringId(int nDstParam)
{
	for( int i=0; ; ++i )
	{
		if( g_DstString[i].nDstParam == 0 )
			break;

		if( g_DstString[i].nDstParam == nDstParam )
			return g_DstString[i].nNameIndex;
	}

	return 0;
}

BOOL CWndMgr::IsDstRate(int nDstParam)
{
	return IsDst_Rate(nDstParam);
}
#endif //__CSC_VER12_4

void CWndMgr::FormalizeChatString( CString & str )
{	// 康: 2008-07-18
	// "을 이용하여 클라이언트 명령어 파싱을 무시하는 행위를 막기 위한 것
	str.TrimLeft();
	str.TrimRight();

	while( str.GetLength() > 0 && str.GetAt( 0 ) == '\"' )
	{
		str.Delete( 0 );

		if(str.GetLength() > 1)
		{
			if( str.GetAt( str.GetLength() - 1 ) == '\"' )
				str.Delete( str.GetLength() - 1 );
			str.TrimLeft();
			str.TrimRight();
		}
	} 
}

#ifdef __WINDOW_INTERFACE_BUG
void CWndMgr::CloseBoundWindow(void) // 아이템이 걸려 있거나, 아이템을 조작할 가능성이 있는 모든 창을 강제로 닫는다.
{
	SAFE_DELETE( m_pWndPiercing ); // 피어싱
	SAFE_DELETE( m_pWndChangeWeapon ); // 유니크 웨폰, 얼터멋 웨폰 변환 창
	SAFE_DELETE( m_pWndRemoveAttribute ); // 속성제련 제거
	SAFE_DELETE( m_pWndRemovePiercing ); // 피어싱 옵션 제거
	SAFE_DELETE( m_pWndRemoveJewel ); // 얼터멋 웨폰 보석 제거
	SAFE_DELETE( m_pWndSmeltMixJewel ); // 오리칼쿰, 문스톤 합성창
	SAFE_DELETE( m_pWndSmeltJewel ); // 보석 합성 창
	SAFE_DELETE( m_pWndExtraction ); // 보석 추출창
	SAFE_DELETE( m_pWndLvReqDown ); // 착용레벨 하락 해지창
	SAFE_DELETE( m_pWndBlessingCancel ); // 여신의 축복 취소
	SAFE_DELETE( m_pWndBank ); // 은행
#ifndef __MIKSIK_EQUALIZED_MODE
	SAFE_DELETE(m_pWndGuildBank);
#else //__MIKSIK_EQUALIZED_MODE
	if (m_pWndGuildBank)
	{
		g_DPlay.SendCloseGuildBankWnd();
		SAFE_DELETE(m_pWndGuildBank);
	}
#endif //__MIKSIK_EQUALIZED_MODE
	SAFE_DELETE( m_pWndPost ); // 우편
	SAFE_DELETE( m_pWndUpgradeBase ); // 제련
	SAFE_DELETE( m_pWndShop );
#if __VER >= 14 // __SMELT_SAFETY
	SAFE_DELETE( m_pWndSmeltSafety ); // 안전 제련
	SAFE_DELETE( m_pWndSmeltSafetyConfirm ); // 제련 확인
#endif // __SMELT_SAFETY
#if __VER >= 14 // __EQUIP_BIND
	SAFE_DELETE( m_pWndEquipBindConfirm ); // 귀속 확인
#endif // __EQUIP_BIND

#ifdef __BAGTPFIX
	CWndBagEx* pWndBagEx = (CWndBagEx*)g_WndMng.GetWndBase(APP_BAG_EX);
	if (pWndBagEx)
	{
		pWndBagEx->Destroy();
	}
#endif //__BAGTPFIX

}
#endif // __WINDOW_INTERFACE_BUG

#ifdef __BAN_CHATTING_SYSTEM
//-----------------------------------------------------------------------------
void CWndMgr::SetWarningCounter( int nWarningCounter )
{
	m_nWarningCounter = nWarningCounter;
}
//-----------------------------------------------------------------------------
void CWndMgr::SetWarning2Counter( int nWarning2Counter )
{
	m_nWarning2Counter = nWarning2Counter;
}
//-----------------------------------------------------------------------------
int CWndMgr::GetWarningCounter( void ) const
{
	return m_nWarningCounter;
}
//-----------------------------------------------------------------------------
int CWndMgr::GetWarning2Counter( void ) const
{
	return m_nWarning2Counter;
}
//-----------------------------------------------------------------------------
CTimer& CWndMgr::GetWarningTimer( void )
{
	return m_timerWarning;
}
//-----------------------------------------------------------------------------
CTimer& CWndMgr::GetWarning2Timer( void )
{
	return m_timerWarning2;
}
//-----------------------------------------------------------------------------
CTimer& CWndMgr::GetShortcutWarningTimer( void )
{
	return m_timerShortcutWarning;
}
//-----------------------------------------------------------------------------
CTimer& CWndMgr::GetBanningTimer( void )
{
	return m_timerBanning;
}
//-----------------------------------------------------------------------------
BOOL CWndMgr::IsShortcutCommand( void ) const
{
	return m_bShortcutCommand;
}
//-----------------------------------------------------------------------------
#endif // __BAN_CHATTING_SYSTEM

#ifdef __CHANGE_HOT_KEY
void CWndMgr::SaveHotkey()
{
	CFileIO file;
	if (!file.Open("hotkey", "wb"))
		return;

	DWORD dwAppletId;
	AppletFunc* pApplet;
	POSITION pos = m_mapAppletFunc.GetStartPosition();
	while (pos)
	{
		m_mapAppletFunc.GetNextAssoc(pos, dwAppletId, (void*&)pApplet);
		if (pApplet)
		{
			file.Write(&dwAppletId, sizeof(DWORD));
			file.Write(&pApplet->m_cHotkey, sizeof(char));
		}
	}
	dwAppletId = CH_KEY_DOWN;
	file.Write(&dwAppletId, sizeof(DWORD));
	file.Write(&g_Neuz.Key.chBack, sizeof(char));

	dwAppletId = CH_KEY_UP;
	file.Write(&dwAppletId, sizeof(DWORD));
	file.Write(&g_Neuz.Key.chUp, sizeof(char));

	dwAppletId = CH_KEY_LEFT;
	file.Write(&dwAppletId, sizeof(DWORD));
	file.Write(&g_Neuz.Key.chLeft, sizeof(char));

	dwAppletId = CH_KEY_RIGHT;
	file.Write(&dwAppletId, sizeof(DWORD));
	file.Write(&g_Neuz.Key.chRight, sizeof(char));

	dwAppletId = CH_KEY_WALK;
	file.Write(&dwAppletId, sizeof(DWORD));
	file.Write(&g_Neuz.Key.chWalk, sizeof(char));

	dwAppletId = CH_KEY_TRACE;
	file.Write(&dwAppletId, sizeof(DWORD));
	file.Write(&g_Neuz.Key.chTrace, sizeof(char));

#ifdef __DPS_METER
	dwAppletId = CH_KEY_DPS;
	file.Write(&dwAppletId, sizeof(DWORD));
	file.Write(&g_Neuz.Key.chDpsMeter, sizeof(char));
#endif

	file.Flush();
	file.Close();
}

void CWndMgr::LoadHotkey()
{
	CFileIO file;
	if (!file.Open("hotkey", "rb"))
		return;

	DWORD dwApplet;
	AppletFunc* pApplet;
	char cHotkey;
	while (file.Read(&dwApplet, sizeof(DWORD)))
	{
		file.Read(&cHotkey, sizeof(char));
		if (dwApplet == CH_KEY_UP)
			g_Neuz.Key.chUp = cHotkey;
		else if (dwApplet == CH_KEY_DOWN)
			g_Neuz.Key.chBack = cHotkey;
		else if (dwApplet == CH_KEY_LEFT)
			g_Neuz.Key.chLeft = cHotkey;
		else if (dwApplet == CH_KEY_RIGHT)
			g_Neuz.Key.chRight = cHotkey;
		else if (dwApplet == CH_KEY_TRACE)
			g_Neuz.Key.chTrace = cHotkey;
		else if (dwApplet == CH_KEY_WALK)
			g_Neuz.Key.chWalk = cHotkey;
#ifdef __DPS_METER
		else if (dwApplet == CH_KEY_DPS)
			g_Neuz.Key.chDpsMeter = cHotkey;
#endif
		else
		{
			if (dwApplet == APP_QUEST_EX_LIST)
				g_Neuz.Key.chQuest = cHotkey;
			if (m_mapAppletFunc.Lookup(dwApplet, (void*&)pApplet))
				pApplet->m_cHotkey = cHotkey;
		}
	}
}
#endif // __CHANGE_HOT_KEY

#ifdef __BETTER_SKILL_TOOLTIP
void CWndMgr::PutToolTip_Skill_Extended(DWORD dwSkill, DWORD dwLevel, CPoint point, CRect* pRect, BOOL bActive)
{
#ifdef __AEGON_THEME_SWITCHER	
	ItemProp* pSkillProp = prj.GetSkillProp(dwSkill);
	if (pSkillProp == NULL) {
		Error("CWndMgr::PutToolTip_Skill_Extended : Skill ID invalid: %d", dwSkill);
		return;
	}

	DWORD dwSkillLevel = dwLevel;

	AddSkillProp* pAddSkillProp = prj.GetAddSkillProp(pSkillProp->dwSubDefine, dwSkillLevel);
	if (pAddSkillProp == NULL) {
		Error("CWndMgr::PutToolTip_Skill_Extended : AddSkill ID Invalid: %d", dwSkill);
		return;
	}

	DWORD dwaf = D3DCOLOR_XRGB(199, 155, 0);
	CString str;
	CString strstr;
	CString string;
	CEditString strEdit;
	DWORD dwColorText = CAegonThemeSwitcherMng().GetInstance()->GetTextColor();

	// Skill Name
	if (bActive) {
		string.Format("%s - Lvl %d", pSkillProp->szName, dwSkillLevel);
	}
	else
		string.Format("%s", pSkillProp->szName);

	strEdit.SetParsingString(string, 0xcff2fbe6d);
	string = "";

	// Resources
	char statebuf[30] = { 0, };
	if (pAddSkillProp->nReqMp != 0xffffffff) {
		sprintf(statebuf, "\nMP: %d", g_pPlayer->GetReqMp(pAddSkillProp->nReqMp));
		string += statebuf;
	}
	if (pAddSkillProp->nReqFp != 0xffffffff) {
		sprintf(statebuf, "\nFP: %d", g_pPlayer->GetReqFp(pAddSkillProp->nReqFp));
		string += statebuf;
	}
	strEdit.AddParsingString(string, dwColorText);
	string = "";

	if (pSkillProp->dwReSkill1 != -1)
	{
		LPSKILL pskill = g_pPlayer->GetSkill(pSkillProp->dwReSkill1);

		if (pskill)
		{
			strstr.Format(prj.GetText(TID_GAME_SKILLDIS), pskill->GetProp()->szName, pSkillProp->dwReSkillLevel1);
			if (pskill->dwLevel < pSkillProp->dwReSkillLevel1) 
			{
				str.Format("\n#cffff0000%s", strstr.GetString());
				strEdit.AddParsingString(str, 0xffff0000);
			}
			else 
			{
				str.Format("\n%s#nc", strstr.GetString());
				strEdit.AddParsingString(str, dwColorText);
			}
		}
	}
	if (pSkillProp->dwReSkill2 != -1)
	{
		LPSKILL pskill = g_pPlayer->GetSkill(pSkillProp->dwReSkill2);

		if (pskill)
		{
			strstr.Format(prj.GetText(TID_GAME_SKILLDIS), pskill->GetProp()->szName, pSkillProp->dwReSkillLevel2);
			if (pskill->dwLevel < pSkillProp->dwReSkillLevel2) 
			{
				str.Format("\n%s", strstr.GetString());
				strEdit.AddParsingString(str, 0xffff0000);
			}
				
			else 
			{
				str.Format("\n%s", strstr.GetString());
				strEdit.AddParsingString(str, dwColorText);
			}
		}
	}

	// Required Level
	strstr.Format(prj.GetText(TID_GAME_BASESKILLLEVEL), pSkillProp->dwReqDisLV);

	if (g_pPlayer->GetLevel() < (int)(pSkillProp->dwReqDisLV) && !g_pPlayer->IsMaster() && !g_pPlayer->IsHero()) 
	{
		str.Format("\n%s", strstr.GetString());
		strEdit.AddParsingString(str, 0xffff0000);
	}
	else
	{
		str.Format("\n%s", strstr.GetString());
		strEdit.AddParsingString(str, dwColorText);
	}
	

	if (pSkillProp->eItemType != SAI79::NO_PROP) {
		string += "\nElemental Type: ";
		strEdit.AddParsingString(string, dwColorText, ESSTY_BOLD);
		string = "";
		switch (pSkillProp->eItemType) {
		case SAI79::FIRE:
			string += "Fire";
			break;
		case SAI79::EARTH:
			string += "Earth";
			break;
		case SAI79::WATER:
			string += "Water";
			break;
		case SAI79::WIND:
			string += "Wind";
			break;
		case SAI79::ELECTRICITY:
			string += "Lightning";
			break;
		}
	}

	strEdit.AddParsingString(string, dwColorText);
	string = "";

	// Skill Damage
	if (pAddSkillProp->dwAbilityMin != 0xffffffff && pAddSkillProp->dwAbilityMax != 0xffffffff && pAddSkillProp->dwAbilityMin != 1 && pAddSkillProp->dwAbilityMax != 1) {
		str.Format(" %d-%d", pAddSkillProp->dwAbilityMin, pAddSkillProp->dwAbilityMax);
		strEdit.AddParsingString("\n", dwColorText);
		strEdit.AddParsingString("Base Damage:", dwColorText, ESSTY_BOLD);
		strEdit.AddParsingString(str, dwColorText);
	}

	// Skill Effects
	if (pAddSkillProp->dwDestParam1 != NULL_ID && !(pAddSkillProp->dwDestParam1 == DST_CHRSTATE || pAddSkillProp->dwDestParam1 == DST_CURECHR)) {
		CString strTemp;
		char sign = '+';
		if (pAddSkillProp->nAdjParamVal1 < 0)
			sign = ' ';
		if (g_WndMng.IsDstRate(pAddSkillProp->dwDestParam1))
		{
			if (pAddSkillProp->dwDestParam1 == DST_ATTACKSPEED) 
			{
				strTemp.Format("\nEffect:");
				strEdit.AddParsingString(strTemp, dwColorText, ESSTY_BOLD);
				strTemp.Format(" %s%c%d%%", FindDstString(pAddSkillProp->dwDestParam1), sign, pAddSkillProp->nAdjParamVal1 / 2 / 10);
				strEdit.AddParsingString(strTemp, dwColorText);
			}
			else
			{
				strTemp.Format("\nEffect:");
				strEdit.AddParsingString(strTemp, dwColorText, ESSTY_BOLD);
				strTemp.Format(" %s%c%d%%", FindDstString(pAddSkillProp->dwDestParam1), sign, pAddSkillProp->nAdjParamVal1);
				strEdit.AddParsingString(strTemp, dwColorText);
			}
				
		}
		else {
			strTemp.Format("\nEffect:");
			strEdit.AddParsingString(strTemp, dwColorText, ESSTY_BOLD);
			strTemp.Format(" %s %c%d", FindDstString(pAddSkillProp->dwDestParam1), sign, pAddSkillProp->nAdjParamVal1);
			strEdit.AddParsingString(strTemp, dwColorText);
		}

	}
	if (pAddSkillProp->dwDestParam2 != NULL_ID && !(pAddSkillProp->dwDestParam2 == DST_CHRSTATE || pAddSkillProp->dwDestParam2 == DST_CURECHR)) {
		CString strTemp;
		char sign = '+';
		if (pAddSkillProp->nAdjParamVal2 < 0)
			sign = ' ';
		if (g_WndMng.IsDstRate(pAddSkillProp->dwDestParam2))
		{
			if (pAddSkillProp->dwDestParam2 == DST_ATTACKSPEED) 
			{
				strTemp.Format("\nEffect:");
				strEdit.AddParsingString(strTemp, dwColorText, ESSTY_BOLD);
				strTemp.Format(" %s%c%d%%", FindDstString(pAddSkillProp->dwDestParam2), sign, pAddSkillProp->nAdjParamVal2 / 2 / 10);
				strEdit.AddParsingString(strTemp, dwColorText);
			}
			else
			{
				strTemp.Format("\nEffect:");
				strEdit.AddParsingString(strTemp, dwColorText, ESSTY_BOLD);
				strTemp.Format(" %s%c%d%%", FindDstString(pAddSkillProp->dwDestParam2), sign, pAddSkillProp->nAdjParamVal2);
				strEdit.AddParsingString(strTemp, dwColorText);
			}
				
		}
		else {
			strTemp.Format("\nEffect:");
			strEdit.AddParsingString(strTemp, dwColorText, ESSTY_BOLD);
			strTemp.Format(" %s %c%d", FindDstString(pAddSkillProp->dwDestParam2), sign, pAddSkillProp->nAdjParamVal2);
			strEdit.AddParsingString(strTemp, dwColorText);
		}
	}

	// Skill Scaling
	if (pSkillProp->dwReferTarget1 == RT_ATTACK) {
		str.Format("\nDamage Scaling:");
		strEdit.AddParsingString(str, dwColorText, ESSTY_BOLD);
		str.Format(" %s x %d", FindDstString(pSkillProp->dwReferStat1), pSkillProp->dwReferValue1);
		strEdit.AddParsingString(str, dwColorText);
	}
	else if (pSkillProp->dwReferTarget1 == RT_HEAL) {
		str.Format("\nHeal Scaling:");
		strEdit.AddParsingString(str, dwColorText, ESSTY_BOLD);
		str.Format(" %s x %d", FindDstString(pSkillProp->dwReferStat1), pSkillProp->dwReferValue1);
		strEdit.AddParsingString(str, dwColorText);
	}

	if (pSkillProp->dwReferTarget2 == RT_ATTACK) {
		str.Format("\nDamage Scaling:");
		strEdit.AddParsingString(str, dwColorText, ESSTY_BOLD);
		str.Format(" %s x %d", FindDstString(pSkillProp->dwReferStat2), pSkillProp->dwReferValue2);
		strEdit.AddParsingString(str, dwColorText);

	}
	else if (pSkillProp->dwReferTarget2 == RT_HEAL) {
		str.Format("\nHeal Scaling:");
		strEdit.AddParsingString(str, dwColorText, ESSTY_BOLD);
		str.Format(" %s x %d", FindDstString(pSkillProp->dwReferStat2), pSkillProp->dwReferValue2);
		strEdit.AddParsingString(str, dwColorText);

	}

	// Skill Duration
	if (pAddSkillProp->dwSkillTime != NULL_ID) {
		// Convert to Seconds
		int duration = static_cast<int>(pAddSkillProp->dwSkillTime / 1000);

		CTimeSpan span(duration);

		if (span.GetHours() > 0) 
		{
			str.Format("\nDuration:");
			strEdit.AddParsingString(str, dwColorText, ESSTY_BOLD);
			str.Format(" %dh:%dm:%ds", span.GetHours(), span.GetMinutes(), span.GetSeconds());
			strEdit.AddParsingString(str, dwColorText);
		}
		else if (span.GetMinutes() > 0) 
		{
			str.Format("\nDuration:");
			strEdit.AddParsingString(str, dwColorText, ESSTY_BOLD);
			str.Format(" %dm:%ds", span.GetMinutes(), span.GetSeconds());
			strEdit.AddParsingString(str, dwColorText);
		}	
		else 
		{
			str.Format("\nDuration:");
			strEdit.AddParsingString(str, dwColorText, ESSTY_BOLD);
			str.Format(" %ds", span.GetSeconds());
			strEdit.AddParsingString(str, dwColorText);
		}
	}

	// Skill Cooldown
	if (pAddSkillProp->dwCooldown > 0 && pAddSkillProp->dwCooldown != NULL_ID) {
		int duration = static_cast<int>(pAddSkillProp->dwCooldown / 1000);

		str.Format("\nCooldown:");
		strEdit.AddParsingString(str, dwColorText, ESSTY_BOLD);
		str.Format(" %ds", duration);
		strEdit.AddParsingString(str, dwColorText);
	}

	// Skill Targeting Type
	if (pSkillProp->dwSpellRegion == SRO_AROUND)
	{
		
		str.Format("\nSpell Type:");
		strEdit.AddParsingString(str, dwColorText, ESSTY_BOLD);

		str.Format(" AoE");
		strEdit.AddParsingString(str, dwColorText);

		str.Format("\nSpell Range:");
		strEdit.AddParsingString(str, dwColorText, ESSTY_BOLD);

		str.Format(" %d", pAddSkillProp->dwSkillRange);
		strEdit.AddParsingString(str, dwColorText);

	}
	else
		if (pSkillProp->dwSpellRegion == SRO_DIRECT) {
			strEdit.AddParsingString("\nSpell Type:", dwColorText, ESSTY_BOLD);
			strEdit.AddParsingString(" Single Target", dwColorText);
		}
		else
			if (pSkillProp->dwSpellRegion == SRO_LINE) {
				strEdit.AddParsingString("\nSpell Type:", dwColorText, ESSTY_BOLD);
				strEdit.AddParsingString(" Line", dwColorText);
			}
			else
				if (pSkillProp->dwSpellRegion == SRO_REGION) {


					str.Format("\n#bSpell Type:#nb Targeted AoE\n#bSpell Range:#nb %d", pAddSkillProp->dwSkillRange);




					str.Format("\nSpell Type:");
					strEdit.AddParsingString(str, dwColorText, ESSTY_BOLD);
					str.Format(" Targeted AoE");
					strEdit.AddParsingString(str, dwColorText);
					str.Format("\nSpell Range:");
					strEdit.AddParsingString(str, dwColorText, ESSTY_BOLD);
					str.Format(" %d", pAddSkillProp->dwSkillRange);
					strEdit.AddParsingString(str, dwColorText);

				}


	if (strcmp(pSkillProp->szCommand, "=")) {
		string += "\n";
		string += pSkillProp->szCommand;
	}

	
	strEdit.AddParsingString(string, dwColorText);

	g_toolTip.PutToolTip(dwSkill, strEdit, *pRect, point, 0);
#else
	ItemProp* pSkillProp = prj.GetSkillProp(dwSkill);
	if (pSkillProp == NULL) {
		Error("CWndMgr::PutToolTip_Skill_Extended : Skill ID invalid: %d", dwSkill);
		return;
	}

	DWORD dwSkillLevel = dwLevel;

	AddSkillProp* pAddSkillProp = prj.GetAddSkillProp(pSkillProp->dwSubDefine, dwSkillLevel);
	if (pAddSkillProp == NULL) {
		Error("CWndMgr::PutToolTip_Skill_Extended : AddSkill ID Invalid: %d", dwSkill);
		return;
	}

	DWORD dwaf = D3DCOLOR_XRGB(199, 155, 0);
	CString str;
	CString strstr;
	CString string;

	// Skill Name
	if (bActive) {
		string.Format("#b#cff2fbe6d%s - Lvl %d#nb#nc", pSkillProp->szName, dwSkillLevel);
	}
	else
		string.Format("#b#cff2fbe6d%s#nb#nc", pSkillProp->szName);

	// Resources
	char statebuf[30] = { 0, };
	if (pAddSkillProp->nReqMp != 0xffffffff) {
		sprintf(statebuf, "\nMP: %d", g_pPlayer->GetReqMp(pAddSkillProp->nReqMp));
		string += statebuf;
	}
	if (pAddSkillProp->nReqFp != 0xffffffff) {
		sprintf(statebuf, "\nFP: %d", g_pPlayer->GetReqFp(pAddSkillProp->nReqFp));
		string += statebuf;
	}

	if (pSkillProp->dwReSkill1 != -1)
	{
		LPSKILL pskill = g_pPlayer->GetSkill(pSkillProp->dwReSkill1);

		if (pskill)
		{
			strstr.Format(prj.GetText(TID_GAME_SKILLDIS), pskill->GetProp()->szName, pSkillProp->dwReSkillLevel1);
			if (pskill->dwLevel < pSkillProp->dwReSkillLevel1)
				str.Format("\n#cffff0000%s#nc", strstr);
			else
				str.Format("\n%s#nc", strstr);
			string += str;
		}
	}
	if (pSkillProp->dwReSkill2 != -1)
	{
		LPSKILL pskill = g_pPlayer->GetSkill(pSkillProp->dwReSkill2);

		if (pskill)
		{
			strstr.Format(prj.GetText(TID_GAME_SKILLDIS), pskill->GetProp()->szName, pSkillProp->dwReSkillLevel2);
			if (pskill->dwLevel < pSkillProp->dwReSkillLevel2)
				str.Format("\n#cffff0000%s#nc", strstr);
			else
				str.Format("\n%s#nc", strstr);
			string += str;
		}
	}

	// Required Level
	strstr.Format(prj.GetText(TID_GAME_BASESKILLLEVEL), pSkillProp->dwReqDisLV);

	if (g_pPlayer->GetLevel() < (int)(pSkillProp->dwReqDisLV) && !g_pPlayer->IsMaster() && !g_pPlayer->IsHero())
		str.Format("\n#cffff0000%s#nc", strstr);
	else
		str.Format("\n%s", strstr);
	string += str;

	if (pSkillProp->eItemType != SAI79::NO_PROP) {
		string += "\n#bElemental Type:#nb ";
		switch (pSkillProp->eItemType) {
		case SAI79::FIRE:
			string += "Fire";
			break;
		case SAI79::EARTH:
			string += "Earth";
			break;
		case SAI79::WATER:
			string += "Water";
			break;
		case SAI79::WIND:
			string += "Wind";
			break;
		case SAI79::ELECTRICITY:
			string += "Lightning";
			break;
		}
	}

	// Skill Damage
	if (pAddSkillProp->dwAbilityMin != 0xffffffff && pAddSkillProp->dwAbilityMin != 0xffffffff && pAddSkillProp->dwAbilityMin != 1 && pAddSkillProp->dwAbilityMax != 1) {
		str.Format("\n#bBase Damage:#nb %d-%d", pAddSkillProp->dwAbilityMin, pAddSkillProp->dwAbilityMax);
		string += str;
	}

	// Skill Effects
	if (pAddSkillProp->dwDestParam1 != NULL_ID && !(pAddSkillProp->dwDestParam1 == DST_CHRSTATE || pAddSkillProp->dwDestParam1 == DST_CURECHR)) {
		CString strTemp;
		char sign = '+';
		if (pAddSkillProp->nAdjParamVal1 < 0)
			sign = ' ';
		if (g_WndMng.IsDstRate(pAddSkillProp->dwDestParam1))
		{
			if (pAddSkillProp->dwDestParam1 == DST_ATTACKSPEED)
				strTemp.Format("\n#bEffect#nb: %s%c%d%%", FindDstString(pAddSkillProp->dwDestParam1), sign, pAddSkillProp->nAdjParamVal1 / 2 / 10);
			else
				strTemp.Format("\n#bEffect#nb: %s%c%d%%", FindDstString(pAddSkillProp->dwDestParam1), sign, pAddSkillProp->nAdjParamVal1);
		}
		else {
			strTemp.Format("\n#bEffect#nb: %s %c%d", FindDstString(pAddSkillProp->dwDestParam1), sign, pAddSkillProp->nAdjParamVal1);
		}
		string += strTemp;
	}
	if (pAddSkillProp->dwDestParam2 != NULL_ID && !(pAddSkillProp->dwDestParam2 == DST_CHRSTATE || pAddSkillProp->dwDestParam2 == DST_CURECHR)) {
		CString strTemp;
		char sign = '+';
		if (pAddSkillProp->nAdjParamVal2 < 0)
			sign = ' ';
		if (g_WndMng.IsDstRate(pAddSkillProp->dwDestParam2))
		{
			if (pAddSkillProp->dwDestParam2 == DST_ATTACKSPEED)
				strTemp.Format("\n#bEffect#nb: %s%c%d%%", FindDstString(pAddSkillProp->dwDestParam2), sign, pAddSkillProp->nAdjParamVal2 / 2 / 10);
			else
				strTemp.Format("\n#bEffect#nb: %s%c%d%%", FindDstString(pAddSkillProp->dwDestParam2), sign, pAddSkillProp->nAdjParamVal2);
		}
		else {
			strTemp.Format("\n#bEffect#nb: %s %c%d", FindDstString(pAddSkillProp->dwDestParam2), sign, pAddSkillProp->nAdjParamVal2);
		}
		string += strTemp;
	}

	// Skill Scaling
	if (pSkillProp->dwReferTarget1 == RT_ATTACK) {
		str.Format("\n#bDamage Scaling:#nb %s x %d", FindDstString(pSkillProp->dwReferStat1), pSkillProp->dwReferValue1);
		string += str;
	}
	else if (pSkillProp->dwReferTarget1 == RT_HEAL) {
		str.Format("\n#bHeal Scaling:#nb %s x %d", FindDstString(pSkillProp->dwReferStat1), pSkillProp->dwReferValue1);
		string += str;
	}

	if (pSkillProp->dwReferTarget2 == RT_ATTACK) {
		str.Format("\n#bDamage Scaling:#nb %s x %d", FindDstString(pSkillProp->dwReferStat2), pSkillProp->dwReferValue2);
		string += str;
	}
	else if (pSkillProp->dwReferTarget2 == RT_HEAL) {
		str.Format("\n#bHeal Scaling:#nb %s x %d", FindDstString(pSkillProp->dwReferStat2), pSkillProp->dwReferValue2);
		string += str;
	}

	// Skill Duration
	if (pAddSkillProp->dwSkillTime != NULL_ID) {
		// Convert to Seconds
		int duration = static_cast<int>(pAddSkillProp->dwSkillTime / 1000);

		CTimeSpan span(duration);

		if (span.GetHours() > 0)
			str.Format("\n#bDuration:#nb %dh:%dm:%ds", span.GetHours(), span.GetMinutes(), span.GetSeconds());
		else if (span.GetMinutes() > 0)
			str.Format("\n#bDuration:#nb %dm:%ds", span.GetMinutes(), span.GetSeconds());
		else
			str.Format("\n#bDuration:#nb %ds", span.GetSeconds());

		string += str;
	}

	// Skill Cooldown
	if (pAddSkillProp->dwCooldown > 0 && pAddSkillProp->dwCooldown != NULL_ID) {
		int duration = static_cast<int>(pAddSkillProp->dwCooldown / 1000);

		str.Format("\n#bCooldown:#nb %ds", duration);
		string += str;
	}

	// Skill Targeting Type
	if (pSkillProp->dwSpellRegion == SRO_AROUND) {
		str.Format("\n#bSpell Type:#nb AoE\n#bSpell Range:#nb %d", pAddSkillProp->dwSkillRange);
		string += str;
	}
	else
		if (pSkillProp->dwSpellRegion == SRO_DIRECT) {
			string += "\n#bSpell Type:#nb Single Target";
		}
		else
			if (pSkillProp->dwSpellRegion == SRO_LINE) {
				string += "\n#bSpell Type:#nb Line";
			}
			else
				if (pSkillProp->dwSpellRegion == SRO_REGION) {
					str.Format("\n#bSpell Type:#nb Targeted AoE\n#bSpell Range:#nb %d", pAddSkillProp->dwSkillRange);
					string += str;
				}


	if (strcmp(pSkillProp->szCommand, "=")) {
		string += "\n";
		string += pSkillProp->szCommand;
	}

	CEditString strEdit;
	strEdit.SetParsingString(string);

	g_toolTip.PutToolTip(dwSkill, strEdit, *pRect, point, 0);
#endif
}
#endif //__BETTER_SKILL_TOOLTIP

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
void CWndMgr::OnClientResize(int clientWidth, int clientHeight, int oldClientWidth, int oldClientHeight)
{
	int sizeDiffWidth = 0;
	int sizeDiffHeight = 0;
	for (int i = 0; i < m_wndOrder.GetSize(); i++)
	{
		CWndBase* pWndBase = (CWndBase*)m_wndOrder.GetAt(i);
		if (pWndBase && pWndBase->m_nIdWnd != APP_APPLET0)
		{
			pWndBase->InvalidateDeviceObjects();

			CRect wndRect = pWndBase->GetWndRect();
			
			CRect wndRectNew;

			LONG scaledCenterPointX = wndRect.left + (wndRect.Width() / 2);
			LONG scaledCenterPointY = wndRect.top + (wndRect.Height() / 2);

			if (pWndBase->m_nIdWnd == APP_COMMUNICATION_CHAT)
			{
				int taskbarHeight = 0;
				
				if (m_pWndTaskBar)
					taskbarHeight = m_pWndTaskBar->GetWndRect().Height();

				if (taskbarHeight <= 0)
					taskbarHeight = 48;
				
				wndRectNew.top = clientHeight - taskbarHeight - wndRect.Height();
				wndRectNew.bottom = wndRectNew.top + wndRect.Height();
				wndRectNew.left = 0;
				wndRectNew.right = wndRect.Width();

			}
			else if (pWndBase->m_nIdWnd == APP_NAVIGATOR)
			{
				wndRectNew.top = 0;
				wndRectNew.bottom = wndRectNew.top + wndRect.Height();
				wndRectNew.right = clientWidth;
				wndRectNew.left = wndRectNew.right - wndRect.Width();
			}
			else if (pWndBase->m_nIdWnd == APP_STATUS1)
			{
				wndRectNew.top = 0;
				wndRectNew.bottom = wndRectNew.top + wndRect.Height();
				wndRectNew.left = 0;
				wndRectNew.right = wndRectNew.left + wndRect.Width();
			}
			//else if (pWndBase->m_nIdWnd == APP_INSTANTMSG) 
			//{
			//}
			else 
			{
				scaledCenterPointX = ((double)scaledCenterPointX / (double)oldClientWidth) * (double)clientWidth;
				scaledCenterPointY = ((double)scaledCenterPointY / (double)oldClientHeight) * (double)clientHeight;

				wndRectNew.left = scaledCenterPointX - (wndRect.Width() / 2);
				wndRectNew.top = scaledCenterPointY - (wndRect.Height() / 2);

				wndRectNew.right = wndRectNew.left + wndRect.Width();
				wndRectNew.bottom = wndRectNew.top + wndRect.Height();


				if (wndRectNew.right > clientWidth)
				{
					sizeDiffWidth = wndRectNew.right - clientWidth;
					wndRectNew.left -= sizeDiffWidth;
					wndRectNew.right -= sizeDiffWidth;

				}
				if (wndRectNew.left < 0)
				{
					wndRectNew.right += std::abs(wndRectNew.left);
					wndRectNew.left = 0;
				}
				if (wndRectNew.bottom > clientHeight)
				{
					sizeDiffHeight = wndRectNew.bottom - clientHeight;
					wndRectNew.top -= sizeDiffHeight;
					wndRectNew.bottom -= sizeDiffHeight;

				}
				if (wndRectNew.top < 0)
				{
					wndRectNew.bottom += std::abs(wndRectNew.top);
					wndRectNew.top = 0;
				}
			}
			if (pWndBase->m_nIdWnd != APP_WORLD) 
			{
				//prevent overlapping with taskbar
				int nOffset = 40;
				if (m_pWndTaskBar)
				{
					nOffset = m_pWndTaskBar->GetWndRect().Height();
				}
				CRect layoutRect = GetLayoutRect();
				layoutRect.top -= nOffset;
				layoutRect.bottom -= nOffset;
				if (wndRectNew.bottom > layoutRect.bottom)
				{
					int diff = wndRectNew.bottom - layoutRect.bottom;
					wndRectNew.top -= diff;
					wndRectNew.bottom -= diff;
				}
			}
			
			pWndBase->SetWndRect(wndRectNew);
		}

	}
}
#endif // __AEGON_FULLSCREEN_AND_RESIZE

#ifdef __AEGON_THEME_SWITCHER
void CWndMgr::OnThemeSelectionChangedTrigger(BOOL bTypeChanged)
{

	__AEGON_THEME_UNIT* pThemeUnit = CAegonThemeSwitcherMng::GetInstance()->GetThemeUnit(g_Option.m_nSelectedThemeID);
	if (pThemeUnit) 
	{
		m_resMng.Free();
		
		CString strResdata = "ResData.inc";
		BOOL bDefault = TRUE;

		if (pThemeUnit->nThemeID != 0) 
		{
			strResdata = MakePath(pThemeUnit->szThemeFolder, "ResData.inc");
			bDefault = FALSE;
		}

		BOOL bSuccess = CWndBase::m_resMng.Load(strResdata.GetString());
		if (!bDefault && !bSuccess) 
		{
			CWndBase::m_resMng.Load("ResData.inc");
		}
	}

	CWndVendor* pWndVendorBase = (CWndVendor*)GetWndVendorBase();
	BOOL bOpenVendorChat = FALSE;
	if(pWndVendorBase)
	{
		if (pWndVendorBase->m_pwndVenderMessage) 
		{
			pWndVendorBase->m_pwndVenderMessage->Destroy();
			Process();
			bOpenVendorChat = TRUE;
		}
		if (pWndVendorBase->m_pWndVendorBuy) 
		{
			pWndVendorBase->m_pWndVendorBuy->Destroy();
			Process();
			SAFE_DELETE(pWndVendorBase->m_pWndVendorBuy);
		}
		if (pWndVendorBase->m_pWndRegVend)
		{
			pWndVendorBase->m_pWndRegVend->Destroy();
			Process();
			SAFE_DELETE(pWndVendorBase->m_pWndRegVend);
		}
		
	}

	FreeTileTexture();
	for(int i = m_wndOrder.GetSize() - 1; i >= 0; i--)
	{
		if (i < 0 || i >= m_wndOrder.GetSize())
			continue;
		CWndBase* pWndBase = (CWndBase*)m_wndOrder.GetAt(i);
		if (pWndBase && pWndBase->m_nIdWnd != APP_APPLET0 && pWndBase->m_nIdWnd != APP_GUIDE)
		{
			int nWndID = pWndBase->GetWndId();
			if (/*bTypeChanged &&*/ CAegonThemeSwitcherMng::GetInstance()->WindowNeedsRestart(nWndID))
			{
				pWndBase->Destroy();
				Process();
				pWndBase = CreateApplet(nWndID);
				if (pWndBase)
				{
				}
				Process();
			}
			else if (nWndID == APP_VIEW_INVENTORY || nWndID == APP_QUERYEQUIP || nWndID == APP_SHOP_ || nWndID == APP_COMMON_BANK)
			{
				pWndBase->Destroy();
				Process();
			}
#ifdef __AEGON_PVP_MINIGAMES
			else if (nWndID == APP_PVP_MINIGAME_MAIN) 
			{
				pWndBase->Destroy();
				Process();

				__MINIGAME_COMMAND sCommand;
				sCommand.dwCommandID = AEG_MINIGAME_COMMAND_REFRESH;

				g_DPlay.SendPvPMinigameCommand(sCommand);
			}
#endif
			else 
			{
				pWndBase->OnThemeSelectionChanged();
				pWndBase->AdjustSubWindowsForTheme();			
			}
		}
	}

	g_toolTip.OnThemeChange();
	g_toolTipSub1.OnThemeChange();
	g_toolTipSub2.OnThemeChange();

	if (m_pWndMenu) 
	{
		m_pWndMenu->Destroy();
		Process();	
		SAFE_DELETE(m_pWndMenu);
	}

	if (bTypeChanged) 
	{
		g_WndMng.RestartTaskBar();
	}

	if (bOpenVendorChat) 
	{
		if (pWndVendorBase && !pWndVendorBase->m_pwndVenderMessage)
		{
			pWndVendorBase->m_pwndVenderMessage = new CWndVendorMessage;

			if (pWndVendorBase->m_pVendor->IsActiveMover())
				pWndVendorBase->m_pwndVenderMessage->m_nIsOwner = TRUE;
			pWndVendorBase->m_pwndVenderMessage->Initialize(pWndVendorBase);
		}
		
	}

	m_Theme.OnThemeChanged();
}
#endif // __AEGON_THEME_SWITCHER

#ifdef __AEGON_FONT_SWITCH
void CWndMgr::OnAegonFontSwitch()
{

	for (int i = 0; i < m_wndOrder.GetSize(); i++)
	{
		CWndBase* pWndBase = (CWndBase*)m_wndOrder.GetAt(i);
		if (pWndBase)
		{
			if (pWndBase->GetFont()) 
			{
				pWndBase->SetFont(CWndBase::m_Theme.m_pFontText);
			}
		}
	}
	
	m_pWndWorld->AegonOnSwitchFont();
	
	m_dwFontSwitched = GetTickCount();
}
#endif // __AEGON_FONT_SWITCH

#ifdef __NAVIGATOR_V19
BOOL CWndMgr::OpenMinimap()
{
	CWndBase* pWndNavigator = g_WndMng.CreateApplet(APP_NAVIGATOR);
	if(pWndNavigator)
		return TRUE;
	return FALSE;
}

void CWndMgr::RestartMinimap()
{
	CWndNavigator* pWndNav = (CWndNavigator*)g_WndMng.GetApplet(APP_NAVIGATOR);
	if (pWndNav)
	{
		pWndNav->Destroy();
		Process();
	}
	OpenMinimap();
}
#endif // __NAVIGATOR_V19

#ifdef __AEGON_INGAME_DP
CWndIngameDPPurchaseConfirm* CWndMgr::OpenDPConfirmPurchase()
{
	SAFE_DELETE(m_pWndDPPurchaseConfirm);
	m_pWndDPPurchaseConfirm = new CWndIngameDPPurchaseConfirm();
	m_pWndDPPurchaseConfirm->Initialize(this);

	return m_pWndDPPurchaseConfirm;
}
#endif

#ifdef __AEGON_INGAME_SHOP
void CWndMgr::PutToolTip_ShopPackage(DWORD dwPackID, CPoint point, CRect* pRect)
{
	__AEGIGNSHP_ITEM_UNIT* pShopUnit = CAegonIngameShopMng::GetInstance()->GetShopUnitByID(dwPackID);
	if (!pShopUnit)
		return;

	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	DWORD dwColorHighlightBlue = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE);
	DWORD dwColorHighlightGreen = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_GREEN);
	DWORD dwColorHighlightYellow = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_YELLOW);



	CEditString strEdit = _T("");
	CString tmpStr = "";

	tmpStr.Format("%s\n", pShopUnit->m_szName);
	strEdit.AddParsingString(tmpStr.GetString(), dwColorHighlightBlue, ESSTY_BOLD);

	tmpStr.Format("This package contains: \n");
	strEdit.AddParsingString(tmpStr.GetString(), dwColorText);

	for (int i = 0; i < pShopUnit->m_vCommandUnits.size(); i++) 
	{
		//tmpStr.Format("\t- %s (Quantity: %d)\n", pShopUnit->m_vCommandUnits[i].m_szItemName, pShopUnit->m_vCommandUnits[i].m_dwItemNum);
		tmpStr.Format("\t- %s ", pShopUnit->m_vCommandUnits[i].m_szItemName);
		strEdit.AddParsingString(tmpStr.GetString(), dwColorHighlightGreen);
		tmpStr.Format("(Quantity: %d)\n", pShopUnit->m_vCommandUnits[i].m_dwItemNum);
		strEdit.AddParsingString(tmpStr.GetString(), dwColorHighlightYellow);
	}

	tmpStr.Format("Price: %d DP\n", pShopUnit->m_nPrice);
	strEdit.AddParsingString(tmpStr.GetString(), dwColorText);

	g_toolTip.PutToolTip(dwPackID, strEdit, *pRect, point, 0);

}
#endif

#ifdef __AEGON_TOURNAMENT
void CWndMgr::ProcessTournament()
{
	if (IsInvalidObj(g_pPlayer) || !g_pPlayer->GetWorld())
		return;
	if (GetWndBase(APP_AEGON_TOURNAMENT_SCORE) || g_pPlayer->GetWorld()->GetID() == AT_TOURNAMENT_MAP)
	{
		if (GetAsyncKeyState(VK_TAB) & 0x8000)
		{
			if (!GetWndBase(APP_AEGON_TOURNAMENT_SCORE))
			{
				SAFE_DELETE(g_WndMng.m_pWndAegonTournamentScoreBoard);
				m_pWndAegonTournamentScoreBoard = new CWndAegonTournamentScoreBoard;
				m_pWndAegonTournamentScoreBoard->Initialize();
			}

		}
		else
		{
			if (GetWndBase(APP_AEGON_TOURNAMENT_SCORE))
			{
				GetWndBase(APP_AEGON_TOURNAMENT_SCORE)->Destroy();
			}
		}
	}
}
#endif // __AEGON_TOURNAMENT