#include "stdafx.h"
#include "defineText.h"
#include "defineItem.h"
#ifdef __V19_FORMATS
#include "defineItemGrade.h"
#include "defineItemType.h"
#endif //__V19_FORMATS
#include "defineSkill.h"
#include "dpcoreclient.h"
#include "dpdatabaseclient.h"
#include "dpsrvr.h"
#include "definesound.h"
#include "defineObj.h"
#include "user.h"
#include "worldmng.h"
#include "misc.h"
#include "lang.h"
#include "npchecker.h"
#include "guild.h"
#include "..\_Common\Ship.h"
#include "Party.h"
#include "Chatting.h"
#include "post.h"
#include "..\_Network\ErrorCode.h"
#include "WantedListSnapshot.h"
#include "definequest.h"

#if __VER >= 12 // __PET_0519
#include "pet.h"
#endif	// __PET_0519

#if __VER >= 12 // __LORD
#include "slord.h"
#include "lordskillexecutable.h"
#endif	// __LORD

#if __VER >= 11 // __SYS_IDENTIFY
#include "randomoption.h"
#endif	// __SYS_IDENTIFY

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#if __VER >= 12 // __ITEMCREATEMON_S0602
	#include "CreateMonster.h"
#endif // __ITEMCREATEMON_S0602

#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	#include "honor.h"
#endif	// __HONORABLE_TITLE			// 달인

#if __VER >= 12 // __SECRET_ROOM
#include "SecretRoom.h"
#endif // __SECRET_ROOM

#if __VER >= 12 // __TAX
#include "Tax.h"
#endif // __TAX

#if __VER >= 12 // __EXT_PIERCING
#include "ItemUpgrade.h"
#endif // __EXT_PIERCING

#include "tools.h"

#if __VER >= 13 // __COUPLE_1117
#include "couplehelper.h"
#endif	// __COUPLE_1117

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#endif // __GUILD_HOUSE

#if __VER >= 15 // __CAMPUS
#include "CampusHelper.h"
#endif // __CAMPUS

#ifdef	__COLOSSEUM
#include "Colosseum.h"
#endif // __COLOSSEUM

#ifdef __AEGON_MONSTER_HUNT
#include "MonsterHunt.h"
#endif // __AEGON_MONSTER_HUNT

#ifdef __LEEAEG_SHOP_FINDER
#include "AegonShopFinder.h"
#endif // __LEEAEG_SHOP_FINDER

#ifdef __AEGON_LOOT_BOX
#include "AegonLootBox.h"
#endif // __AEGON_LOOT_BOX

#ifdef __AEGON_TOURNAMENT
#include "AegonTournament.h"
#endif // __AEGON_TOURNAMENT

#ifdef __AEGON_LAST_ONLINE
#include "AegonLastOnlineMng.h"
#endif // __AEGON_LAST_ONLINE

#ifdef __AEGON_CRAFTING_SYSTEM
#include "AegonCrafting.h"
#endif // __AEGON_CRAFTING_SYSTEM

#ifdef __SWITCH_EQUIP
#include "..\_aiinterface\AIPet.h"
#endif // __SWITCH_EQUIP

#ifdef __AEGON_EMOTE_SYSTEM
#include "AegonEmoticonMng.h"
#endif //__AEGON_EMOTE_SYSTEM

#ifdef __AEGON_PVP_MINIGAMES
#include "AegonPvPMinigames.h"
#endif

#ifdef __AEGON_IMPROVED_SHOPDESIGN
#include "AegonShopDesign.h"
#endif

#ifdef __AEGON_TELEPORTER
#include "AegonTeleportMng.h"
#endif

#if __VER >= 8 //__CSC_VER8_5
struct ItemCountSet{
	OBJID itemid;
	int extracount;
};
#endif //__CSC_VER8_5

#define	MAX_RANGE_ASYNC			1024
#define	MAX_RANGE_NPC_MENU		1024

extern	CGuildMng			g_GuildMng;
extern	CPartyMng			g_PartyMng;
extern	CUserMng			g_UserMng;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient		g_DPCoreClient;
extern	CWorldMng			g_WorldMng;
extern	CChattingMng		g_ChattingMng;
extern	CGuildCombat		g_GuildCombatMng;

#if __VER >= 12 // __ITEMCREATEMON_S0602
#if __VER < 12 // __NEW_ITEMCREATEMON_SERVER
extern	CCreateMonster		g_CreateMonster;
#endif // __NEW_ITEMCREATEMON_SERVER
#endif // __ITEMCREATEMON_S0602

CCommonCtrl* CreateExpBox( CUser* pUser );
CDPSrvr		g_DPSrvr;

// dwGold과 nPlus을 더 할 수 있는가?
BOOL CanAdd( DWORD dwGold, int nPlus )
{
	if( nPlus <= 0 )		// 더하려는 값이 0이하이면 넌센스 
		return FALSE;

	int nGold = dwGold;
	ASSERT( nGold >= 0 );
	return ( (nGold + nPlus) > nGold );		// 더한 값이 overflow이면 막아야한다.
}

#ifdef __BUYBACK
void Reorganize(CItemContainer<CItemElem>* pSrc)
{
	CItemContainer<CItemElem> tmp;
	tmp.SetItemContainer(ITYPE_ITEM, pSrc->GetMax());
	CItemElem* pItemElem;
	for (int i = 0; i < pSrc->GetMax(); i++)
	{
		pItemElem = pSrc->GetAt(i);
		if (pItemElem)
		{
			tmp.Add(pItemElem);
		}
	}

	pSrc->Clear();
	for (int i = 0; i < tmp.GetMax(); i++)
	{
		pItemElem = tmp.GetAt(i);
		if (pItemElem)
		{
			pSrc->Add(pItemElem);
		}
	}
}
#endif // __BUYBACK

CDPSrvr::CDPSrvr()
{
#ifdef __WORLSERVER_MENU
	m_bExchange = TRUE;
	m_bChat = TRUE;
	m_bPk = TRUE;
#endif //__WORLSERVER_MENU

	BEGIN_MSG;

	ON_MSG( PACKETTYPE_JOIN, &CDPSrvr::OnAddUser );
	ON_MSG( PACKETTYPE_LEAVE, &CDPSrvr::OnRemoveUser );
	ON_MSG( PACKETTYPE_REPLACE, &CDPSrvr::OnReplace );
	ON_MSG( PACKETTYPE_SUMMONPLAYER, &CDPSrvr::OnSummonPlayer );
	ON_MSG( PACKETTYPE_TELEPORTPLAYER, &CDPSrvr::OnTeleportPlayer );
	ON_MSG( PACKETTYPE_CORR_REQ, &CDPSrvr::OnCorrReq );
	ON_MSG( PACKETTYPE_SCRIPTDLG, &CDPSrvr::OnScriptDialogReq );
	ON_MSG( PACKETTYPE_TRADEPUT, &CDPSrvr::OnTradePut );
	ON_MSG( PACKETTYPE_TRADEPULL, &CDPSrvr::OnTradePull );
	ON_MSG( PACKETTYPE_TRADEPUTGOLD, &CDPSrvr::OnTradePutGold );
//raiders.2006.11.28
//	ON_MSG( PACKETTYPE_TRADECLEARGOLD, &CDPSrvr::OnTradeClearGold );
	ON_MSG( PACKETTYPE_SFX_ID, &CDPSrvr::OnSfxID );
	ON_MSG( PACKETTYPE_SFX_CLEAR, &CDPSrvr::OnSfxClear );
	ON_MSG( PACKETTYPE_CREATE_GUILDCLOAK, &CDPSrvr::OnCreateGuildCloak );
	ON_MSG( PACKETTYPE_CHAT, &CDPSrvr::OnChat );
	ON_MSG( PACKETTYPE_CTRL_COOLTIME_CANCEL, &CDPSrvr::OnCtrlCoolTimeCancel );
	ON_MSG( PACKETTYPE_DOEQUIP, &CDPSrvr::OnDoEquip );
	ON_MSG( PACKETTYPE_MOVEITEM, &CDPSrvr::OnMoveItem );
	ON_MSG( PACKETTYPE_SNAPSHOT, &CDPSrvr::OnSnapshot );
	ON_MSG( PACKETTYPE_INC_STAT_LEVEL, &CDPSrvr::OnIncStatLevel );
	ON_MSG( PACKETTYPE_SEND_TO_SERVER_CHANGEJOB, &CDPSrvr::OnChangeJob );
	ON_MSG( PACKETTYPE_SETLODELIGHT, &CDPSrvr::OnSetLodelight );
	ON_MSG( PACKETTYPE_MODIFYMODE, &CDPSrvr::OnModifyMode );
	ON_MSG( PACKETTYPE_REVIVAL, &CDPSrvr::OnRevival );
	ON_MSG( PACKETTYPE_REVIVAL_TO_LODESTAR, &CDPSrvr::OnRevivalLodestar );
	ON_MSG( PACKETTYPE_REVIVAL_TO_LODELIGHT, &CDPSrvr::OnRevivalLodelight );
	ON_MSG( PACKETTYPE_OPENSHOPWND, &CDPSrvr::OnOpenShopWnd );
	ON_MSG( PACKETTYPE_CLOSESHOPWND, &CDPSrvr::OnCloseShopWnd );
	ON_MSG( PACKETTYPE_SEND_TO_SERVER_EXP, &CDPSrvr::OnExpUp );
	ON_MSG( PACKETTYPE_OPENBANKWND, &CDPSrvr::OnOpenBankWnd );
	ON_MSG( PACKETTYPE_GUILD_BANK_WND, &CDPSrvr::OnOpenGuildBankWnd );
	ON_MSG( PACKETTYPE_PUTITEMGUILDBANK, &CDPSrvr::OnPutItemGuildBank );
	ON_MSG( PACKETTYPE_GETITEMGUILDBANK, &CDPSrvr::OnGetItemGuildBank );
	ON_MSG( PACKETTYPE_GUILD_BANK_WND_CLOSE, &CDPSrvr::OnCloseGuildBankWnd );
	ON_MSG( PACKETTYPE_GUILD_BANK_MOVEITEM, &CDPSrvr::OnGuildBankMoveItem );
	ON_MSG( PACKETTYPE_CLOSEBANKWND, &CDPSrvr::OnCloseBankWnd );
	ON_MSG( PACKETTYPE_PUTITEMBACK, &CDPSrvr::OnPutItemBank );
	ON_MSG( PACKETTYPE_DOUSESKILLPOINT, &CDPSrvr::OnDoUseSkillPoint );
	ON_MSG( PACKETTYPE_PUTBACKTOBANK, &CDPSrvr::OnBankToBank );
	ON_MSG( PACKETTYPE_GETITEMBACK, &CDPSrvr::OnGetItemBank );
	ON_MSG( PACKETTYPE_PUTGOLDBACK, &CDPSrvr::OnPutGoldBank );
	ON_MSG( PACKETTYPE_GETGOLDBACK, &CDPSrvr::OnGetGoldBank );
	ON_MSG( PACKETTYPE_MOVEBANKITEM, &CDPSrvr::OnMoveBankItem );
	ON_MSG( PACKETTYPE_CHANGEBANKPASS, &CDPSrvr::OnChangeBankPass );
	ON_MSG( PACKETTYPE_CONFIRMBANK, &CDPSrvr::OnConfirmBank );	
	ON_MSG( PACKETTYPE_PLAYERMOVED, &CDPSrvr::OnPlayerMoved );
	ON_MSG( PACKETTYPE_PLAYERBEHAVIOR, &CDPSrvr::OnPlayerBehavior );
	ON_MSG( PACKETTYPE_PLAYERMOVED2, &CDPSrvr::OnPlayerMoved2 );
	ON_MSG( PACKETTYPE_PLAYERBEHAVIOR2, &CDPSrvr::OnPlayerBehavior2 );
	ON_MSG( PACKETTYPE_PLAYERANGLE, &CDPSrvr::OnPlayerAngle );
	ON_MSG( PACKETTYPE_PLAYERCORR, &CDPSrvr::OnPlayerCorr );
	ON_MSG( PACKETTYPE_PLAYERCORR2, &CDPSrvr::OnPlayerCorr2 );
	ON_MSG( PACKETTYPE_PLAYERSETDESTOBJ, &CDPSrvr::OnPlayerSetDestObj );
	ON_MSG( PACKETTYPE_TRADE, &CDPSrvr::OnTrade );
	ON_MSG( PACKETTYPE_CONFIRMTRADE, &CDPSrvr::OnConfirmTrade );
	ON_MSG( PACKETTYPE_CONFIRMTRADECANCEL, &CDPSrvr::OnConfirmTradeCancel )
	ON_MSG( PACKETTYPE_TRADECANCEL, &CDPSrvr::OnTradeCancel );
	ON_MSG( PACKETTYPE_DOUSEITEM, &CDPSrvr::OnDoUseItem );
#ifdef __FL_BAG_UPDATE
	ON_MSG(PACKETTYPE_DOUSEITEM_BAG, &CDPSrvr::OnDoUseItemBag);
#endif //__FL_BAG_UPDATE
#if __VER >= 11 // __SYS_IDENTIFY
	ON_MSG( PACKETTYPE_DO_USE_ITEM_TARGET, &CDPSrvr::OnDoUseItemTarget );
	ON_MSG( PACKETTYPE_REMOVE_ITEM_LEVEL_DOWN, &CDPSrvr::OnRemoveItemLevelDown );
	ON_MSG( PACKETTYPE_AWAKENING, &CDPSrvr::OnAwakening );
	ON_MSG( PACKETTYPE_BLESSEDNESS_CANCEL, &CDPSrvr::OnBlessednessCancel );
#endif	// __SYS_IDENTIFY
#ifdef __AZRIA_1023
	ON_MSG( PACKETTYPE_DO_USE_ITEM_INPUT, &CDPSrvr::OnDoUseItemInput );
#endif	// __AZRIA_1023
#ifdef __PET_1024
	ON_MSG( PACKETTYPE_CLEAR_PET_NAME, &CDPSrvr::OnClearPetName );
#endif	// __PET_1024
#if __VER >= 11 // __SYS_POCKET
	ON_MSG( PACKETTYPE_AVAIL_POCKET, &CDPSrvr::OnAvailPocket );
	ON_MSG( PACKETTYPE_MOVE_ITEM_POCKET, &CDPSrvr::OnMoveItemOnPocket );
#endif	// __SYS_POCKET

#ifdef __JEFF_11
	ON_MSG( PACKETTYPE_QUE_PETRESURRECTION, &CDPSrvr::OnQuePetResurrection );
#endif	// __JEFF_11

#ifdef __JEFF_11_4
	ON_MSG( PACKETTYPE_ARENA_ENTER, &CDPSrvr::OnArenaEnter );
	ON_MSG( PACKETTYPE_ARENA_EXIT, &CDPSrvr::OnArenaExit );
#endif	// __JEFF_11_4

	ON_MSG( PACKETTYPE_TRADECONFIRM, &CDPSrvr::OnTradelastConfirm );
	ON_MSG( PACKETTYPE_MOVERFOCOUS, &CDPSrvr::OnMoverFocus );
	ON_MSG( PACKETTYPE_DROPITEM, &CDPSrvr::OnDropItem );
	ON_MSG( PACKETTYPE_DROPGOLD, &CDPSrvr::OnDropGold );
	ON_MSG( PACKETTYPE_BUYITEM, &CDPSrvr::OnBuyItem );
#if __VER >= 11 // __GUILDCOMBATCHIP
	ON_MSG( PACKETTYPE_BUYCHIPITEM, &CDPSrvr::OnBuyChipItem );
#endif // __GUILDCOMBATCHIP
	ON_MSG( PACKETTYPE_SELLITEM, &CDPSrvr::OnSellItem );
	ON_MSG( PACKETTYPE_TRADEOK, &CDPSrvr::OnTradeOk );
	ON_MSG( PACKETTYPE_MELEE_ATTACK, &CDPSrvr::OnMeleeAttack );
	ON_MSG( PACKETTYPE_MELEE_ATTACK2, &CDPSrvr::OnMeleeAttack2 );
	ON_MSG( PACKETTYPE_MAGIC_ATTACK, &CDPSrvr::OnMagicAttack );
	ON_MSG( PACKETTYPE_RANGE_ATTACK, &CDPSrvr::OnRangeAttack );
	ON_MSG( PACKETTYPE_SFX_HIT, &CDPSrvr::OnSfxHit );
	ON_MSG( PACKETTYPE_USESKILL, &CDPSrvr::OnUseSkill );
	ON_MSG( PACKETTYPE_SETTARGET, &CDPSrvr::OnSetTarget );	// Core에 보내야 하는지 확인해 줄것.

#ifdef __STOP_EXP
	ON_MSG(PACKETTYPE_SETEXPSTOP, &CDPSrvr::OnSetExpStop);
#endif //__STOP_EXP

	ON_MSG( PACKETTYPE_TELESKILL, &CDPSrvr::OnTeleSkill );	
	ON_MSG( PACKETTYPE_SKILLTASKBAR, &CDPSrvr::OnSkillTaskBar );
	ON_MSG( PACKETTYPE_ADDAPPLETTASKBAR, &CDPSrvr::OnAddAppletTaskBar );
	ON_MSG( PACKETTYPE_REMOVEAPPLETTASKBAR, &CDPSrvr::OnRemoveAppletTaskBar );
	ON_MSG( PACKETTYPE_ADDITEMTASKBAR, &CDPSrvr::OnAddItemTaskBar );
	ON_MSG( PACKETTYPE_REMOVEITEMTASKBAR, &CDPSrvr::OnRemoveItemTaskBar );
	ON_MSG( PACKETTYPE_QUERYGETPOS, &CDPSrvr::OnQueryGetPos );
	ON_MSG( PACKETTYPE_GETPOS, &CDPSrvr::OnGetPos );
	ON_MSG( PACKETTYPE_QUERYGETDESTOBJ, &CDPSrvr::OnQueryGetDestObj );
	ON_MSG( PACKETTYPE_GETDESTOBJ, &CDPSrvr::OnGetDestObj );
	ON_MSG( PACKETTYPE_MEMBERREQUEST, &CDPSrvr::OnPartyRequest );
	ON_MSG( PACKETTYPE_MEMBERREQUESTCANCLE, &CDPSrvr::OnPartyRequestCancle );
	ON_MSG( PACKETTYPE_PARTYSKILLUSE, &CDPSrvr::OnPartySkillUse );
	ON_MSG( PACKETTYPE_ADDFRIENDREQEST, &CDPSrvr::OnAddFriendReqest );
	ON_MSG( PACKETTYPE_ADDFRIENDNAMEREQEST, &CDPSrvr::OnAddFriendNameReqest );
	ON_MSG( PACKETTYPE_ADDFRIENDCANCEL, &CDPSrvr::OnAddFriendCancel );
	ON_MSG( PACKETTYPE_DUELREQUEST, &CDPSrvr::OnDuelRequest );
	ON_MSG( PACKETTYPE_DUELYES, &CDPSrvr::OnDuelYes );
	ON_MSG( PACKETTYPE_DUELNO, &CDPSrvr::OnDuelNo );
	ON_MSG( PACKETTYPE_DUELPARTYREQUEST, &CDPSrvr::OnDuelPartyRequest );
	ON_MSG( PACKETTYPE_DUELPARTYYES, &CDPSrvr::OnDuelPartyYes );
	ON_MSG( PACKETTYPE_DUELPARTYNO, &CDPSrvr::OnDuelPartyNo );
	ON_MSG( PACKETTYPE_SEND_TO_SERVER_AP, &CDPSrvr::OnActionPoint );
#if __VER >= 11 // __SYS_PLAYER_DATA
	ON_MSG( PACKETTYPE_QUERY_PLAYER_DATA, &CDPSrvr::OnQueryPlayerData );
	ON_MSG( PACKETTYPE_QUERY_PLAYER_DATA2, &CDPSrvr::OnQueryPlayerData2 );
#else	// __SYS_PLAYER_DATA
	ON_MSG( PACKETTYPE_QUERYPLAYERSTRING, &CDPSrvr::OnQueryPlayerString );
	ON_MSG( PACKETTYPE_QUERYPLAYERLISTSTRING, &CDPSrvr::OnQueryPlayerListString );
	ON_MSG( PACKETTYPE_GETFRIENDNAME, &CDPSrvr::OnGetFriendName );
#endif	// __SYS_PLAYER_DATA
	ON_MSG( PACKETTYPE_GUILD_INVITE, &CDPSrvr::OnGuildInvite );
	ON_MSG( PACKETTYPE_IGNORE_GUILD_INVITE, &CDPSrvr::OnIgnoreGuildInvite );
	ON_MSG( PACKETTYPE_NW_GUILDLOGO, &CDPSrvr::OnGuildLogo );			// 로고 변경 
	ON_MSG( PACKETTYPE_NW_GUILDCONTRIBUTION, &CDPSrvr::OnGuildContribution );		// 공헌도 
	ON_MSG( PACKETTYPE_NW_GUILDNOTICE, &CDPSrvr::OnGuildNotice );		// 공지사항  
	ON_MSG( PACKETTYPE_REQUEST_GUILD_RANKING, &CDPSrvr::OnRequestGuildRank );
	ON_MSG( PACKETTYPE_PVENDOR_OPEN, &CDPSrvr::OnPVendorOpen );
	ON_MSG( PACKETTYPE_PVENDOR_CLOSE, &CDPSrvr::OnPVendorClose );
	ON_MSG( PACKETTYPE_REGISTER_PVENDOR_ITEM, &CDPSrvr::OnRegisterPVendorItem );
	ON_MSG( PACKETTYPE_UNREGISTER_PVENDOR_ITEM, &CDPSrvr::OnUnregisterPVendorItem );
	ON_MSG( PACKETTYPE_QUERY_PVENDOR_ITEM, &CDPSrvr::OnQueryPVendorItem );
	ON_MSG( PACKETTYPE_BUY_PVENDOR_ITEM, &CDPSrvr::OnBuyPVendorItem );
	ON_MSG( PACKETTYPE_REPAIRITEM, &CDPSrvr::OnRepairItem );
	ON_MSG( PACKETTYPE_SET_HAIR, &CDPSrvr::OnSetHair );
	ON_MSG( PACKETTYPE_BLOCK, &CDPSrvr::OnBlock );
	ON_MSG( PACKETTYPE_MOTION, &CDPSrvr::OnMotion );
	ON_MSG( PACKETTYPE_SHIP_ACTMSG, &CDPSrvr::OnShipActMsg );
	ON_MSG( PACKETTYPE_LOCALPOSFROMIA, &CDPSrvr::OnLocalPosFromIA );
	ON_MSG( PACKETTYPE_UPGRADEBASE, &CDPSrvr::OnUpgradeBase );
	ON_MSG( PACKETTYPE_ENCHANT, &CDPSrvr::OnEnchant );
#if __VER >= 14 // __SMELT_SAFETY
	ON_MSG( PACKETTYPE_SMELT_SAFETY, &CDPSrvr::OnSmeltSafety );
#endif // __SMELT_SAFETY
#if __VER >= 10 // __REMOVE_ATTRIBUTE
	ON_MSG( PACKETTYPE_REMVOE_ATTRIBUTE, &CDPSrvr::OnRemoveAttribute );
#endif // __REMOVE_ATTRIBUTE
#if __VER >= 13 // __EXT_ENCHANT
	ON_MSG( PACKETTYPE_CHANGE_ATTRIBUTE, &CDPSrvr::OnChangeAttribute );
#endif // __EXT_ENCHANT
	ON_MSG( PACKETTYPE_PIERCING_SIZE, &CDPSrvr::OnPiercingSize );
	ON_MSG( PACKETTYPE_EXPBOXINFO, &CDPSrvr::OnExpBoxInfo );
	ON_MSG( PACKETTYPE_RANDOMSCROLL, &CDPSrvr::OnRandomScroll );
	ON_MSG( PACHETTYPE_ITEMTRANSY, &CDPSrvr::OnItemTransy );
	ON_MSG( PACKETTYPE_PIERCING, &CDPSrvr::OnPiercing );
#if __VER >= 11 // __PIERCING_REMOVE
	ON_MSG( PACKETTYPE_PIERCINGREMOVE, &CDPSrvr::OnPiercingRemove );
#endif // __PIERCING_REMOVE
	ON_MSG( PACKETTYPE_BUYING_INFO, &CDPSrvr::OnBuyingInfo );
	ON_MSG( PACKETTYPE_ENTERCHTTING, &CDPSrvr::OnEnterChattingRoom );
	ON_MSG( PACKETTYPE_CHATTING, &CDPSrvr::OnChatting );
	ON_MSG( PACKETTYPE_OPENCHATTINGROOM, &CDPSrvr::OnOpenChattingRoom );
	ON_MSG( PACKETTYPE_CLOSECHATTINGROOM, &CDPSrvr::OnCloseChattingRoom );
	ON_MSG( PACKETTYPE_COMMONPLACE, &CDPSrvr::OnCommonPlace );
	ON_MSG( PACKETTYPE_SETNAVIPOINT, &CDPSrvr::OnSetNaviPoint );
	ON_MSG( PACKETTYPE_DO_ESCAPE, &CDPSrvr::OnDoEscape );
	ON_MSG( PACKETTYPE_LOG_GAMEMASTER_CHAT, &CDPSrvr::OnGameMasterWhisper );
	ON_MSG( PACKETTYPE_RETURNSCROLL, &CDPSrvr::OnReturnScroll );
	ON_MSG( PACKETTYPE_ENDSKILLQUEUE, &CDPSrvr::OnEndSkillQueue );
	ON_MSG( PACKETTYPE_FOCUSOBJ, &CDPSrvr::OnFoucusObj );

	ON_MSG( PACKETTYPE_REMOVEQUEST, &CDPSrvr::OnRemoveQuest );
	ON_MSG( PACKETTYPE_COMMERCIALELEM, &CDPSrvr::OnCommercialElem );
//	ON_MSG( PACKETTYPE_DO_COLLECT, &CDPSrvr::OnDoCollect );

	ON_MSG( PACKETTYPE_NW_WANTED_GOLD, &CDPSrvr::OnNWWantedGold );
	ON_MSG( PACKETTYPE_NW_WANTED_LIST, &CDPSrvr::OnNWWantedList );
	ON_MSG( PACKETTYPE_NW_WANTED_INFO, &CDPSrvr::OnNWWantedInfo );
	ON_MSG( PACKETTYPE_NW_WANTED_NAME, &CDPSrvr::OnNWWantedName );
	ON_MSG( PACKETTYPE_REQ_LEAVE, &CDPSrvr::OnReqLeave );
	ON_MSG( PACKETTYPE_RESURRECTION_OK, &CDPSrvr::OnResurrectionOK );
	ON_MSG( PACKETTYPE_RESURRECTION_CANCEL, &CDPSrvr::OnResurrectionCancel );
	ON_MSG( PACKETTYPE_MODE, &CDPSrvr::OnChangeMode );

	ON_MSG( PACKETTYPE_STATEMODE, &CDPSrvr::OnStateMode );
	ON_MSG( PACKETTYPE_QUERYSETPLAYERNAME, &CDPSrvr::OnQuerySetPlayerName );
	ON_MSG( PACKETTYPE_QUERYSETGUILDNAME, &CDPSrvr::OnQuerySetGuildName );
	ON_MSG( PACKETTYPE_CHEERING, &CDPSrvr::OnCheering );
	ON_MSG( PACKETTYPE_QUERYEQUIP, &CDPSrvr::OnQueryEquip );
	ON_MSG( PACKETTYPE_QUERYEQUIPSETTING, &CDPSrvr::OnQueryEquipSetting );
	ON_MSG( PACKETTYPE_QUERYPOSTMAIL, &CDPSrvr::OnQueryPostMail );
	ON_MSG( PACKETTYPE_QUERYREMOVEMAIL, &CDPSrvr::OnQueryRemoveMail );
	ON_MSG( PACKETTYPE_QUERYGETMAILITEM, &CDPSrvr::OnQueryGetMailItem );
	ON_MSG( PACKETTYPE_QUERYGETMAILGOLD, &CDPSrvr::OnQueryGetMailGold );
	ON_MSG( PACKETTYPE_READMAIL, &CDPSrvr::OnQueryReadMail );
	ON_MSG( PACKETTYPE_QUERYMAILBOX, &CDPSrvr::OnQueryMailBox );
	ON_MSG( PACKETTYPE_CHANGEFACE, &CDPSrvr::OnChangeFace );

#if __VER >= 12 // __ITEMCREATEMON_S0602
	ON_MSG( PACKETTYPE_CREATEMONSTER, &CDPSrvr::OnCreateMonster );
#endif // __ITEMCREATEMON_S0602

	ON_MSG( PACKETTYPE_IN_GUILDCOMBAT, &CDPSrvr::OnGCApp );
	ON_MSG( PACKETTYPE_OUT_GUILDCOMBAT, &CDPSrvr::OnGCCancel );	
	ON_MSG( PACKETTYPE_REQUEST_STATUS, &CDPSrvr::OnGCRequestStatus );
	ON_MSG( PACKETTYPE_SELECTPLAYER_GUILDCOMBAT, &CDPSrvr::OnGCSelectPlayer );
	ON_MSG( PACKETTYPE_SELECTMAP_GUILDCOMBAT, &CDPSrvr::OnGCSelectMap );
	ON_MSG( PACKETTYPE_JOIN_GUILDCOMBAT, &CDPSrvr::OnGCJoin );	
	ON_MSG( PACKETTYPE_GETPENYAGUILD_GUILDCOMBAT, &CDPSrvr::OnGCGetPenyaGuild );
	ON_MSG( PACKETTYPE_GETPENYAPLAYER_GUILDCOMBAT, &CDPSrvr::OnGCGetPenyaPlayer );
#if __VER < 8 // #ifndef __GUILDCOMBAT_85
	ON_MSG( PACKETTYPE_GETITEM_GUILDCOMBAT, &CDPSrvr::OnGCGetItem );
#endif // __VER < 8
	ON_MSG( PACKETTYPE_TELE_GUILDCOMBAT, &CDPSrvr::OnGCTele );
	ON_MSG( PACKETTYPE_PLAYERPOINT_GUILDCOMBAT, &CDPSrvr::OnGCPlayerPoint );
	ON_MSG( PACKETTYPE_SUMMON_FRIEND, &CDPSrvr::OnSummonFriend );
	ON_MSG( PACKETTYPE_SUMMON_FRIEND_CONFIRM, &CDPSrvr::OnSummonFriendConfirm );
	ON_MSG( PACKETTYPE_SUMMON_FRIEND_CANCEL, &CDPSrvr::OnSummonFriendCancel );
	ON_MSG( PACKETTYPE_SUMMON_PARTY, &CDPSrvr::OnSummonParty );
	ON_MSG( PACKETTYPE_SUMMON_PARTY_CONFIRM, &CDPSrvr::OnSummonPartyConfirm );

	ON_MSG( PACKETTYPE_REMOVEINVENITEM, &CDPSrvr::OnRemoveInvenItem );
	////////////////////////////////////////////////////////////////////////
#if __VER >= 8 //__CSC_VER8_5
	ON_MSG( PACKETTYPE_CREATEANGEL, &CDPSrvr::OnCreateAngel );
	ON_MSG( PACKETTYPE_ANGELBUFF, &CDPSrvr::OnAngleBuff );
#endif //__CSC_VER8_5
	
#ifdef __EVE_MINIGAME
	ON_MSG( PACKETTYPE_KAWIBAWIBO_START, &CDPSrvr::OnKawibawiboStart );
	ON_MSG( PACKETTYPE_KAWIBAWIBO_GETITEM, &CDPSrvr::OnKawibawiboGetItem );
	ON_MSG( PACKETTYPE_REASSEMBLE_START, &CDPSrvr::OnReassembleStart );
	ON_MSG( PACKETTYPE_REASSEMBLE_OPENWND, &CDPSrvr::OnReassembleOpenWnd );
	ON_MSG( PACKETTYPE_ALPHABET_OPENWND, &CDPSrvr::OnAlphabetOpenWnd );
	ON_MSG( PACKETTYPE_ALPHABET_START, &CDPSrvr::OnAlphabetStart );
	ON_MSG( PACKETTYPE_FIVESYSTEM_OPENWND, &CDPSrvr::OnFiveSystemOpenWnd );
	ON_MSG( PACKETTYPE_FIVESYSTEM_BET, &CDPSrvr::OnFiveSystemBet );
	ON_MSG( PACKETTYPE_FIVESYSTEM_START, &CDPSrvr::OnFiveSystemStart );
	ON_MSG( PACKETTYPE_FIVESYSTEM_DESTROYWND, &CDPSrvr::OnFiveSystemDestroyWnd );
#endif // __EVE_MINIGAME
	
#if __VER >= 9 // __ULTIMATE
	ON_MSG( PACKETTYPE_ULTIMATE_MAKEITEM, &CDPSrvr::OnUltimateMakeItem );
	ON_MSG( PACKETTYPE_ULTIMATE_MAKEGEM, &CDPSrvr::OnUltimateMakeGem );
	ON_MSG( PACKETTYPE_ULTIMATE_TRANSWEAPON, &CDPSrvr::OnUltimateTransWeapon );
	ON_MSG( PACKETTYPE_ULTIMATE_SETGEM, &CDPSrvr::OnUltimateSetGem );
	ON_MSG( PACKETTYPE_ULTIMATE_REMOVEGEM, &CDPSrvr::OnUltimateRemoveGem );
	ON_MSG( PACKETTYPE_ULTIMATE_ENCHANTWEAPON, &CDPSrvr::OnUltimateEnchantWeapon );
#endif // __ULTIMATE

#ifdef __TRADESYS
	ON_MSG( PACKETTYPE_EXCHANGE, &CDPSrvr::OnExchange );
#endif // __TRADESYS

#if __VER >= 9	// __PET_0410
	ON_MSG( PACKETTYPE_PET_RELEASE, &CDPSrvr::OnPetRelease );
	ON_MSG( PACKETTYPE_USE_PET_FEED, &CDPSrvr::OnUsePetFeed );
	ON_MSG( PACKETTYPE_MAKE_PET_FEED, &CDPSrvr::OnMakePetFeed );
	ON_MSG( PACKETTYPE_PET_TAMER_MISTAKE, &CDPSrvr::OnPetTamerMistake );
	ON_MSG( PACKETTYPE_PET_TAMER_MIRACLE, &CDPSrvr::OnPetTamerMiracle );
	ON_MSG( PACKETTYPE_FEED_POCKET_INACTIVE, &CDPSrvr::OnFeedPocketInactive );
#endif	// __PET_0410

#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
	ON_MSG( PACKETTYPE_LEGENDSKILLUP_START, &CDPSrvr::OnLegendSkillStart );
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans

#if __VER >= 9 // __CSC_VER9_2
	ON_MSG( PACKETTYPE_MODIFY_STATUS, &CDPSrvr::OnModifyStatus );
#endif //__CSC_VER9_2

#if __VER >= 11 // __GUILD_COMBAT_1TO1
	ON_MSG( PACKETTYPE_GC1TO1_TENDEROPENWND, &CDPSrvr::OnGC1to1TenderOpenWnd );
	ON_MSG( PACKETTYPE_GC1TO1_TENDERVIEW, &CDPSrvr::OnGC1to1TenderView );
	ON_MSG( PACKETTYPE_GC1TO1_TENDER, &CDPSrvr::OnGC1to1Tender );
	ON_MSG( PACKETTYPE_GC1TO1_TENDERCANCEL, &CDPSrvr::OnGC1to1CancelTender );
	ON_MSG( PACKETTYPE_GC1TO1_TENDERFAILED, &CDPSrvr::OnGC1to1FailedTender );
	ON_MSG( PACKETTYPE_GC1TO1_MEMBERLINEUPOPENWND, &CDPSrvr::OnGC1to1MemberLineUpOpenWnd );
	ON_MSG( PACKETTYPE_GC1TO1_MEMBERLINEUP, &CDPSrvr::OnGC1to1MemberLineUp );
	ON_MSG( PACKETTYPE_GC1TO1_TELEPORTTONPC, &CDPSrvr::OnGC1to1TeleportToNPC );
	ON_MSG( PACKETTYPE_GC1TO1_TELEPORTTOSTAGE, &CDPSrvr::OnGC1to1TeleportToStage );	
#endif // __GUILD_COMBAT_1TO1

#if __VER >= 11 // __MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
	ON_MSG( PACKETTYPE_GUILDLOG_VIEW, &CDPSrvr::OnQueryGuildBankLogList );
#endif //__MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	ON_MSG( PACKETTYPE_SEALCHAR_REQ, &CDPSrvr::OnSealCharReq );
	ON_MSG( PACKETTYPE_SEALCHARCONM_REQ, &CDPSrvr::OnSealCharConmReq );
	ON_MSG( PACKETTYPE_SEALCHARGET_REQ, &CDPSrvr::OnSealCharGetReq );
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	ON_MSG( PACKETTYPE_HONOR_LIST_REQ, &CDPSrvr::OnHonorListReq );
	ON_MSG( PACKETTYPE_HONOR_CHANGE_REQ, &CDPSrvr::OnHonorChangeReq );
#endif	// __HONORABLE_TITLE			// 달인

#if __VER >= 11 // __SYS_COLLECTING
	ON_MSG( PACKETTYPE_QUERY_START_COLLECTING, &CDPSrvr::OnQueryStartCollecting );
	ON_MSG( PACKETTYPE_QUERY_STOP_COLLECTING, &CDPSrvr::OnQueryStopCollecting );
#endif	// __SYS_COLLECTING

#ifdef __NPC_BUFF
	ON_MSG( PACKETTYPE_NPC_BUFF, &CDPSrvr::OnNPCBuff );
#endif // __NPC_BUFF

#if __VER >= 12 // __SECRET_ROOM
	ON_MSG( PACKETTYPE_SECRETROOM_TENDEROPENWND, &CDPSrvr::OnSecretRoomTenderOpenWnd );
	ON_MSG( PACKETTYPE_SECRETROOM_TENDER, &CDPSrvr::OnSecretRoomTender );
	ON_MSG( PACKETTYPE_SECRETROOM_TENDERCANCELRETURN, &CDPSrvr::OnSecretRoomTenderCancelReturn );
	ON_MSG( PACKETTYPE_SECRETROOM_LINEUPOPENWND, &CDPSrvr::OnSecretRoomLineUpOpenWnd );
	ON_MSG( PACKETTYPE_SECRETROOM_LINEUPMEMBER, &CDPSrvr::OnSecretRoomLineUpMember );
	ON_MSG( PACKETTYPE_SECRETROOM_ENTRANCE, &CDPSrvr::OnSecretRoomEntrance );
	ON_MSG( PACKETTYPE_SECRETROOM_TELEPORTTONPC, &CDPSrvr::OnSecretRoomTeleportToNPC );
	ON_MSG( PACKETTYPE_SECRETROOM_TENDERVIEW, &CDPSrvr::OnSecretRoomTenderView );
	ON_MSG( PACKETTYPE_SECRETROOM_TELEPORTTODUNGEON, &CDPSrvr::OnTeleportSecretRoomDungeon );
#endif // __SECRET_ROOM

#if __VER >= 12 // __LORD
	ON_MSG( PACKETTYPE_ELECTION_ADD_DEPOSIT, &CDPSrvr::OnElectionAddDeposit );
	ON_MSG( PACKETTYPE_ELECTION_SET_PLEDGE, &CDPSrvr::OnElectionSetPledge );
	ON_MSG( PACKETTYPE_ELECTION_INC_VOTE, &CDPSrvr::OnElectionIncVote );
	ON_MSG( PACKETTYPE_L_EVENT_CREATE, &CDPSrvr::OnLEventCreate );
	ON_MSG( PACKETTYPE_LORD_SKILL_USE, &CDPSrvr::OnLordSkillUse );
#endif	// __LORD
#if __VER >= 12 // __PET_0519
	// 알변환 핸들러
	ON_MSG( PACKETTYPE_TRANSFORM_ITEM, &CDPSrvr::OnTransformItem );
#endif	// __PET_0519

#if __VER >= 12 // __TAX
	ON_MSG( PACKETTYPE_TAX_SET_TAXRATE, &CDPSrvr::OnSetTaxRate );
#endif // __TAX

#if __VER >= 12 // __HEAVEN_TOWER
	ON_MSG( PACKETTYPE_HEAVENTOWER_TELEPORT, &CDPSrvr::OnTeleportToHeavenTower );
#endif // __HEAVEN_TOWER

#if __VER >= 12 // __MOD_TUTORIAL
	ON_MSG( PACKETTYPE_TUTORIAL_STATE, &CDPSrvr::OnTutorialState );
#endif	// __MOD_TUTORIAL

#if __VER >= 12 // __PET_0519
	ON_MSG( PACKETTYPE_PICKUP_PET_AWAKENING_CANCEL, &CDPSrvr::OnPickupPetAwakeningCancel );
#endif	// __PET_0519

#if __VER >= 12 // __UPDATE_OPT
	ON_MSG( PACKETTYPE_OPTION_ENABLE_RENDER_MASK, &CDPSrvr::OnOptionEnableRenderMask );
#endif	// __UPDATE_OPT

#if __VER >= 13 // __RAINBOW_RACE
	ON_MSG( PACKETTYPE_RAINBOWRACE_PREVRANKING_OPENWND, &CDPSrvr::OnRainbowRacePrevRankingOpenWnd );
	ON_MSG( PACKETTYPE_RAINBOWRACE_APPLICATION_OPENWND, &CDPSrvr::OnRainbowRaceApplicationOpenWnd );
	ON_MSG( PACKETTYPE_RAINBOWRACE_APPLICATION, &CDPSrvr::OnRainbowRaceApplication );
	ON_MSG( PACKETTYPE_RAINBOWRACE_MINIGAME_PACKET, &CDPSrvr::OnRainbowRaceMiniGamePacket );
	ON_MSG( PACKETTYPE_RAINBOWRACE_REQ_FINISH, &CDPSrvr::OnRainbowRaceReqFinish );
#endif // __RAINBOW_RACE

#if __VER >= 13 // __HOUSING
	ON_MSG( PACKETTYPE_HOUSING_SETUPFURNITURE, &CDPSrvr::OnHousingSetupFurniture );
	ON_MSG( PACKETTYPE_HOUSING_SETVISITALLOW, &CDPSrvr::OnHousingSetVisitAllow );
	ON_MSG( PACKETTYPE_HOUSING_VISITROOM, &CDPSrvr::OnHousingVisitRoom );
	ON_MSG( PACKETTYPE_HOUSING_REQVISITABLELIST, &CDPSrvr::OnHousingVisitableList );
	ON_MSG( PACKETTYPE_HOUSING_GOOUT, &CDPSrvr::OnHousingGoOut );
#endif // __HOUSING

#if __VER >= 13 // __QUEST_HELPER
	ON_MSG( PACKETTYPE_QUESTHELPER_REQNPCPOS, &CDPSrvr::OnReqQuestNPCPos );
#endif // __QUEST_HELPER

#if __VER >= 13 // __COUPLE_1117
	ON_MSG( PACKETTYPE_PROPOSE, &CDPSrvr::OnPropose );
	ON_MSG( PACKETTYPE_REFUSE, &CDPSrvr::OnRefuse );
	ON_MSG( PACKETTYPE_COUPLE, &CDPSrvr::OnCouple );
	ON_MSG( PACKETTYPE_DECOUPLE, &CDPSrvr::OnDecouple );
#endif	// __COUPLE_1117
#ifdef __MAP_SECURITY
	ON_MSG( PACKETTYPE_MAP_KEY, &CDPSrvr::OnMapKey );
#endif // __MAP_SECURITY

#if __VER >= 15 // __PETVIS
	ON_MSG( PACKETTYPE_VISPET_REMOVEVIS, &CDPSrvr::OnRemoveVis );
	ON_MSG( PACKETTYPE_VISPET_SWAPVIS, &CDPSrvr::OnSwapVis );
#endif // __PETVIS
#if __VER >= 15 // __GUILD_HOUSE
	ON_MSG( PACKETTYPE_GUILDHOUSE_BUY, &CDPSrvr::OnBuyGuildHouse );
	ON_MSG( PACKETTYPE_GUILDHOUSE_PACKET, &CDPSrvr::OnGuildHousePacket );
	ON_MSG( PACKETTYPE_GUILDHOUSE_ENTER, &CDPSrvr::OnGuildHouseEnter );
	ON_MSG( PACKETTYPE_GUILDHOUSE_GOOUT, &CDPSrvr::OnGuildHouseGoOut );
#endif // __GUILD_HOUSE
#if __VER >= 15 // __TELEPORTER
	ON_MSG( PACKETTYPE_TELEPORTER, &CDPSrvr::OnTeleporterReq );
#endif // __TELEPORTER
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	ON_MSG( PACKETTYPE_QUEST_CHECK, &CDPSrvr::OnCheckedQuest );
#endif // __IMPROVE_QUEST_INTERFACE
#if __VER >= 15 // __CAMPUS
	ON_MSG( PACKETTYPE_CAMPUS_INVITE, &CDPSrvr::OnInviteCampusMember );
	ON_MSG( PACKETTYPE_CAMPUS_ACCEPT, &CDPSrvr::OnAcceptCampusMember );
	ON_MSG( PACKETTYPE_CAMPUS_REFUSE, &CDPSrvr::OnRefuseCampusMember );
	ON_MSG( PACKETTYPE_CAMPUS_REMOVE_MEMBER, &CDPSrvr::OnRemoveCampusMember );
#endif // __CAMPUS

	ON_MSG( PACKETTYPE_ITEM_SELECT_AWAKENING_VALUE, &CDPSrvr::OnItemSelectAwakeningValue );

#ifdef __GUILD_HOUSE_MIDDLE
	ON_MSG( PACKETTYPE_GUILDHOUSE_TENDER_MAINWND, &CDPSrvr::OnGuildHouseTenderMainWnd );
	ON_MSG( PACKETTYPE_GUILDHOUSE_TENDER_INFOWND, &CDPSrvr::OnGuildHouseTenderInfoWnd );
	ON_MSG( PACKETTYPE_GUILDHOUSE_TENDER_JOIN, &CDPSrvr::OnGuildHouseTenderJoin );
#endif // __GUILD_HOUSE_MIDDLE

#ifdef __LOOKCHANGE
	ON_MSG(PACKETTYPE_LOOKCHANGE, &CDPSrvr::OnLookChange);
#endif // __LOOKCHANGE

#ifdef	__COLOSSEUM
	ON_MSG( PACKETTYPE_COLOSSEUM, &CDPSrvr::OnColosseum );
#endif //__COLOSSEUM

#ifdef __NEW_ITEM_VARUNA
	ON_MSG( PACKETTYPE_BARUNA, &CDPSrvr::OnBaruna );
#endif //__NEW_ITEM_VARUNA

#ifdef __FASHION_COMBINE
	ON_MSG( PACKETTYPE_COMBINE_FASHION, &CDPSrvr::OnFashionCombine );
#endif //__FASHION_COMBINE

#ifdef __ITEMLINK
	ON_MSG( PACKETTYPE_ITEMLINK, &CDPSrvr::OnItemLink );
#endif //__ITEMLINK

#ifdef __TREASUREBOX
	ON_MSG( PACKETTYPE_OPENBOX, &CDPSrvr::OnTreasureBoxOpen );
#endif//__TREASUREBOX

#ifdef __SYS_LOCK_ITEM
	ON_MSG(PACKETTYPE_LOCK_ITEM, &CDPSrvr::OnItemLock);
#endif // __SYS_LOCK_ITEM

#ifdef __WIKIPEDIA_ITEMS
	ON_MSG(PACKETTYPE_WIKICI, &CDPSrvr::OnWikiCreateItem);
#endif //__WIKIPEDIA_ITEMS

#ifdef __AEGON_PETMISTAKE
	ON_MSG(PACKETTYPE_PET_TAMER_MISTAKE_CUSTOM, &CDPSrvr::OnPetTamerMistakeCustom);
#endif // __AEGON_PETMISTAKE

#ifdef	__AEGON_MONSTER_HUNT
	ON_MSG(PACKETTYPE_SENDLOADMHPROGRESS, &CDPSrvr::OnLoadMonsterHuntProgress);
#endif // __AEGON_MONSTER_HUNT

#ifdef __LEEAEG_SHOP_FINDER
	ON_MSG(PACKETTYPE_SEND_SHOP_ITEM_TELE, &CDPSrvr::OnShopItemTele);
	ON_MSG(PACKETTYPE_SEND_SHOP_ITEM_REFRESH, &CDPSrvr::OnShopItemRefresh);
#endif // __LEEAEG_SHOP_FINDER

#ifdef __GIFTBOX_LIST_ITEMS
	ON_MSG(PACKETTYPE_GIFTBOX_PREVIEW, &CDPSrvr::OnGiftboxQuery);
#endif // __GIFTBOX_LIST_ITEMS

#ifdef __AEGON_RETURN_AWAKE
	ON_MSG(PACKETTYPE_RETURN_AWAKENING, &CDPSrvr::OnReturnAwakening);
#endif // __AEGON_RETURN_AWAKE

#ifdef __VENDING_NPC
	ON_MSG(PACKETTYPE_CREATEVENDOR, &CDPSrvr::OnCreateVendor);
	ON_MSG(PACKETTYPE_UNLINKNPCVENDOR, &CDPSrvr::OnUnlinkNpcVendor);
#endif // __VENDING_NPC

#ifdef __AEGON_CONVERT_PERIN
	ON_MSG(PACKETTYPE_AEGON_CONVERT_PERIN, &CDPSrvr::OnAegonConvertPerin);
#endif //__AEGON_CONVERT_PERIN

#ifdef __NEW_CHANGE_JOB
	ON_MSG(PACKETTYPE_SEND_JOB_CHANGE, &CDPSrvr::OnSendJobChange);
#endif // __NEW_CHANGE_JOB

#ifdef __WEAPON_RARITY
	ON_MSG(PACKETTYPE_WEAPON_RARITY, &CDPSrvr::OnUpdateWeaponRarity);
#endif // __WEAPON_RARITY

#ifdef __HIDE_FASHION
	ON_MSG(PACKETTYPE_HIDE_COAT, &CDPSrvr::OnHideCoat);
#endif // __HIDE_FASHION

#ifdef __SYS_CUSTOM_TITLE
	ON_MSG(PACKETTYPE_CREATE_OWN_TITLE, &CDPSrvr::OnCreateOwnTitle);
	ON_MSG(PACKETTYPE_UPDATE_SHOW_TITLE, &CDPSrvr::OnUpdateShowTitle);
#endif // __SYS_CUSTOM_TITLE

#ifdef __PREMIUM
	ON_MSG(PACKETTYPE_UPDATE_SHOW_PREMIUM, &CDPSrvr::OnUpdateShowPremium);
#endif //__PREMIUM

#ifdef __MOUNT
	ON_MSG(PACKETTYPE_UPDATE_MOUNT, &CDPSrvr::OnMountChange);
#endif // __MOUNT

#ifdef __SYS_PARTYFINDER
	ON_MSG(PACKETTYPE_JOIN_PARTY_REQ, &CDPSrvr::OnPartyList);
	ON_MSG(PACKETTYPE_PARTYALLOWJOIN, &CDPSrvr::OnPartyAllowJoin);
#endif // __SYS_PARTYFINDER

#ifdef __ANARCHY_SYSTEM
	ON_MSG(PACKETTYPE_ANARCHY_BUFFS, &CDPSrvr::OnAnarchyBuffs);
#endif // __ANARCHY_SYSTEM

#ifdef __AEGON_TELEPORTER
	ON_MSG(PACKETTYPE_APP_TELEPORT, &CDPSrvr::OnTeleportByAPP);
#endif

#ifdef __AEGON_LOOT_BOX
	ON_MSG(PACKETTYPE_LOOTBOX_START, &CDPSrvr::OnLootboxStart);
	ON_MSG(PACKETTYPE_LOOTBOX_COLLECT, &CDPSrvr::OnLootboxCollect);
#endif // __AEGON_LOOT_BOX
	
#ifdef __AEGON_VIEW_INV
	ON_MSG(PACKETTYPE_VIEW_INVENTORY, &CDPSrvr::OnInvView);
	ON_MSG(PACKETTYPE_VIEW_INVENTORY_COMMAND, &CDPSrvr::OnInvViewCommand);
#endif // __AEGON_VIEW_INV

#ifdef __SYS_PETFILTER
	ON_MSG(PACKETTYPE_PETFILTER, &CDPSrvr::OnSetPetfilter);
#endif //__SYS_PETFILTER

#ifdef __BADGE
	ON_MSG(PACKETTYPE_BADGE_CHANGE, &CDPSrvr::OnChangeBadgeIcon);
#endif //__BADGE

#ifdef __SYS_GUILDCOLOR
	ON_MSG(PACKETTYPE_SETGUILDCOLOR_CTS, &CDPSrvr::OnSetGuildColor);
#endif //__SYS_GUILDCOLOR

#ifdef __SYS_NAMECOLOR
	ON_MSG(PACKETTYPE_SETNAMECOLOR, &CDPSrvr::OnSetNameColor);
#endif //__SYS_NAMECOLOR

#ifdef __GUILDFINDER
	ON_MSG(PACKETTYPE_GUILD_LIST, &CDPSrvr::OnGuildList);
	ON_MSG(PACKETTYPE_GUILD_REQUEST, &CDPSrvr::OnGuildFinderRequest);
	ON_MSG(PACKETTYPE_NW_GUILDFINDER, &CDPSrvr::OnGuildFinderOpt);
#endif //__GUILDFINDER

#ifdef	__REMOVE_DURATION_ITEMS
	ON_MSG(PACKETTYPE_REMOVE_CMM, &CDPSrvr::OnRemoveCmm);
#endif	// __REMOVE_DURATION_ITEMS

#ifdef __SYS_MONSTER_CLASH
	ON_MSG(PACKETTYPE_MONSTER_CLASH_REGISTER, &CDPSrvr::OnMonsterClashRegister);
	ON_MSG(PACKETTYPE_MONSTER_CLASH_REGISTER_MEMBER, &CDPSrvr::OnMonsterClashRegisterMember);
	ON_MSG(PACKETTYPE_MONSTER_CLASH_REGISTER_MEMBER_WINDOW, &CDPSrvr::OnMonsterClashRegisterMemberWindow);
	ON_MSG(PACKETTYPE_MONSTER_CLASH_REGISTER_WINDOW, &CDPSrvr::OnMonsterClashRegisterWindow);
	ON_MSG(PACKETTYPE_MONSTER_CLASH_UNREGISTER_MEMBER, &CDPSrvr::OnMonsterClashUnRegisterMember);
#endif //__SYS_MONSTER_CLASH
	 
#ifdef	__REPORT_SYSTEM
	ON_MSG(PACKETTYPE_SENDREPORT, &CDPSrvr::OnSendReport);
#endif // __REPORT_SYSTEM

#ifdef __SYS_SANDBOX
	ON_MSG(PACKETTYPE_SANDBOX, &CDPSrvr::OnSandbox);
	ON_MSG(PACKETTYPE_SANDBOX_TELEPORT, &CDPSrvr::OnSandboxTeleport);
#endif //__SYS_SANDBOX

#ifdef __CHECKLOAD
	ON_MSG(PACKETTYPE_REQUEST_CHECKLOAD, &CDPSrvr::OnCheckLoad);
#endif //__CHECKLOAD

#ifdef __TIMER
	ON_MSG(PACKETTYPE_TIMER, &CDPSrvr::OnDungeonTimer);
#endif //__TIMER

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
	ON_MSG(PACKETTYPE_SENDLOADACHIEVEMENTPROG, &CDPSrvr::OnLoadAchievementProgress);
	ON_MSG(PACKETTYPE_ONCHECKACHIEVEMENTPROG, &CDPSrvr::OnCheckAchievementProgress);
	ON_MSG(PACKETTYPE_SENDCOLLECTACHIEVEMENT, &CDPSrvr::OnCollectAchievement);	
	ON_MSG(PACKETTYPE_SENDACHIEVEMENTFAV, &CDPSrvr::OnAchievementFavorite);
#endif // __AEGON_ACHIEVEMENT_SYSTEM

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
	ON_MSG(PACKETTYPE_SENDRELOADTASKBAR, &CDPSrvr::OnReloadTaskbar);
#endif // __AEGON_FULLSCREEN_AND_RESIZE

#ifdef __PLAYER_LIST
	ON_MSG(PACKETTYPE_PLAYER_LIST_SEND_REQUEST, &CDPSrvr::OnPlayerListRequest);
#endif // __PLAYER_LIST

#ifdef __AEGON_CRAFTING_SYSTEM
	ON_MSG(PACKETTYPE_SENDCRAFTRECIPE, &CDPSrvr::OnCraftRecipe);
#endif // __AEGON_CRAFTING_SYSTEM

#ifdef __WEAPON_MERGE
	ON_MSG(PACKETTYPE_WEAPONMERGE, &CDPSrvr::OnWeaponMerge);
	ON_MSG(PACKETTYPE_WEAPONMERGE_CANCEL, &CDPSrvr::OnWeaponMergeCancel);
#endif //__WEAPON_MERGE

#ifdef __NPC_BUFF_CLASS
	ON_MSG(PACKETTYPE_NPC_BUFF_CLASS, &CDPSrvr::OnNPCBuffClass);
#endif // __NPC_BUFF_CLASS

#ifdef __BUYBACK
	ON_MSG(PACKETTYPE_SEND_BUYBACK, &CDPSrvr::OnBuyBack);
#endif // __BUYBACK

#ifdef __AEGON_SEASON_PASS
	ON_MSG(PACKETTYPE_SENDLOADSEASONPASSPROG, &CDPSrvr::OnLoadSeasonPassProgress);
	ON_MSG(PACKETTYPE_SENDCLAIMSEASONPASSREWARD, &CDPSrvr::OnClaimSeasonPassReward);
	ON_MSG(PACKETTYPE_SENDGETSEASONPASSCOUNTDOWN, &CDPSrvr::OnGetSeasonPassCountdown);
	ON_MSG(PACKETTYPE_SENDPURCHASESEASONPASS, &CDPSrvr::OnPurchaseSeasonPass);
#endif // __AEGON_SEASON_PASS

#ifdef __AEGON_ADVANCED_ANNOUNCEMENT
	ON_MSG(PACKETTYPE_SENDAEGONSYSTEMSHOUT, &CDPSrvr::OnAegonSystemShout);
#endif // __AEGON_ADVANCED_ANNOUNCEMENT

#ifdef __SWITCH_EQUIP
	ON_MSG(PACKETTYPE_EQUIPSWITCH_UPDATE, &CDPSrvr::OnEquipSwitch);
	ON_MSG(PACKETTYPE_EQUIPSWITCH_USE, &CDPSrvr::OnEquipSwitchUse);
#endif // __SWITCH_EQUIP

#ifdef __AEGON_PVP_RANKING
	ON_MSG(PACKETTYPE_GET_PVP_RANKING, &CDPSrvr::OnGetPvPRanking);
#endif // __AEGON_PVP_RANKING

#ifdef __AEGON_PREM_MOTIONS
	ON_MSG(PACKETTYPE_SENDPURCHASEMOTION, &CDPSrvr::OnPurchaseMotion);
#endif // __AEGON_PREM_MOTIONS

#ifdef __PET_GLOW
	ON_MSG(PACKETTYPE_CHANGEPETGLOW, &CDPSrvr::GoToChangePetGlow);
#endif // __PET_GLOW

#ifdef __MODS_MANAGER
	ON_MSG(PACKETTYPE_SEND_MODS_MANAGER, &CDPSrvr::OnSendModsManager);
#endif // __MODS_MANAGER

#ifdef __SYS_WORLDBOSS
	ON_MSG(PACKETTYPE_REGISTER_HWID, &CDPSrvr::OnRegisterHWID);
#endif //__SYS_WORLDBOSS

#ifdef __NPC_QUICK_SELL
	ON_MSG(PACKETTYPE_QUICKSELL, &CDPSrvr::OnQuickSell);
#endif // __NPC_QUICK_SELL

#ifdef __AEGON_EMOTE_SYSTEM
	ON_MSG(PACKETTYPE_SENDPURCHASEEMOTE, &CDPSrvr::OnPurchaseEmote);
#endif //__AEGON_EMOTE_SYSTEM

#ifdef __CRYSTAL
	ON_MSG(PACKETTYPE_CRYSTAL, &CDPSrvr::OnCrystal);
#endif // __CRYSTAL

#ifdef __AEGON_INGAME_SHOP
	ON_MSG(PACKETTYPE_SENDWEBSHOPPURCHASE, &CDPSrvr::OnWebshopPurchase);
#endif

#ifdef __AEGON_TOURNAMENT
	ON_MSG(PACKETTYPE_SENDCREATETOURNAMENT, &CDPSrvr::OnCreateTournament);	
	ON_MSG(PACKETTYPE_TOURNAMENT_GET_MAP, &CDPSrvr::OnGetTournamentMap);	
	ON_MSG(PACKETTYPE_SENDDELETETOURNAMENT, &CDPSrvr::OnDeleteTournament);
#endif

#ifdef __ROYAL_RUMBLE
	ON_MSG(PACKETTYPE_PLAYERWAR_JOIN, &CDPSrvr::OnPlayerSiegeJoin);
	ON_MSG(PACKETTYPE_PLAYERWAR_RANKING, &CDPSrvr::OnGetSiegeRanking);
#endif // __ROYAL_RUMBLE

#ifdef __AEGON_AUTO_STATS
	ON_MSG(PACKETTYPE_UPDATE_AUTO_STATS, &CDPSrvr::OnUpdateAutoStats);
#endif

#ifdef __AEGON_F_TOOL
	ON_MSG(PACKETTYPE_UPDATE_FTOOLS, &CDPSrvr::OnUpdateFTools);
#endif

#ifdef __REMOVE_SKILL_BUFF
	ON_MSG(PACKETTYPE_REMOVE_SKILL_BUFF, &CDPSrvr::OnSendRemoveSkillBuff);
#endif // __REMOVE_SKILL_BUFF

#ifdef __AEGON_TIME_DIFF
	ON_MSG(PACKETTYPE_UPDATE_TIME_DIFF, &CDPSrvr::OnUpdateTimeDiff);
#endif //__AEGON_TIME_DIFF

#ifdef __FL_BAG_LOOT
	ON_MSG(PACKETTYPE_SELECTED_BAG, &CDPSrvr::OnSelectedBag);
#endif //__FL_BAG_LOOT

#ifdef __AEGON_PVP_MINIGAMES
	ON_MSG(PACKETTYPE_PVPMINIGAME_CREATE, &CDPSrvr::OnPvPMinigameCreate);
	ON_MSG(PACKETTYPE_PVPMINIGAME_COMMAND, &CDPSrvr::OnPvPMinigameCommand);
	ON_MSG(PACKETTYPE_PVPMINIGAME_CHAT, &CDPSrvr::OnPvPMinigameChat);
	ON_MSG(PACKETTYPE_PVPMINIGAME_GET_LIST, &CDPSrvr::OnPvPMinigameGetList);
#endif

#ifdef __MIKSIK_EQUALIZED_MODE
	ON_MSG(PACKETTYPE_SELECT_TMP_CLASS, &CDPSrvr::OnSelectTmpClass);
#endif //__MIKSIK_EQUALIZED_MODE

#ifdef __AEGON_MESSENGER
	ON_MSG(PACKETTYPE_MESSENGER_STATUS_UPDATE, &CDPSrvr::OnMessengerUpdateRequest);
	ON_MSG(PACKETTYPE_MESSENGER_ADD_CHAT_UNIT, &CDPSrvr::OnMessengerAddChatUnit);
#endif

#ifdef __AEGON_BAG_FREE_MOVE
	ON_MSG(PACKETTYPE_MOVE_POCKET_ITEM, &CDPSrvr::OnMovePocketItem);
#endif
}

CDPSrvr::~CDPSrvr()
{
	m_dpidCache = DPID_UNKNOWN;			// 캐쉬서버 DPID
}

void CDPSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
				OnAddConnection( lpCreatePlayer->dpId );
				break;
			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				OnRemoveConnection( lpDestroyPlayer->dpId );
				break;
			}
	}
}

void CDPSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	LPBYTE lpBuffer		= (LPBYTE)lpMsg + sizeof(DPID);
	u_long uBufSize		= dwMsgSize - sizeof(DPID);

	CAr ar( lpBuffer, uBufSize );
	GETTYPE( ar )

	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );

	if( pfn ) 
	{
		( this->*( pfn ) )( ar, idFrom, *(UNALIGNED LPDPID)lpMsg, lpBuffer, uBufSize );
	}
}

void CDPSrvr::OnAddConnection( DPID dpid )
{
}

void CDPSrvr::OnRemoveConnection( DPID dpid )
{
	if( dpid == m_dpidCache )
	{
		g_UserMng.RemoveAllUsers();		
		m_dpidCache = DPID_UNKNOWN;	
	}
}

void CDPSrvr::OnAddUser( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	m_dpidCache = dpidCache;			// 캐쉬서버의 DPID를 보관한다.

	TCHAR	lpszAccount[MAX_ACCOUNT], lpszpw[MAX_PASSWORD], lpAddr[16];
	DWORD	dwAuthKey;
	u_long	idPlayer;
	BYTE	nSlot;
	DPID	dpidSocket;

	ar >> dwAuthKey >> idPlayer >> nSlot;
	ar >> dpidSocket;
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	ar.ReadString( lpszpw, MAX_PASSWORD );
	ar.ReadString( lpAddr, 16 );

#ifdef __GAMEGUARD
	if (nSlot > 2 || nSlot < 0)
#else // __GAMEGUARD
	if (nSlot >= 3)
#endif // __GAMEGUARD
	{
		return;
	}

#ifdef __FIX_CHARACTER_DISCONNECT
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
#else //__FIX_CHARACTER_DISCONNECT
	CUser* pUser = (CUser*)prj.GetUserByID(idPlayer);
#endif //__FIX_CHARACTER_DISCONNECT
	if( pUser )
	{
		//WriteLog( "CDPSrvr::OnAddUser idPlayer:%d account:%s DPID:%d", idPlayer, lpszAccount, dpidUser );
		// 캐쉬서버에는 socket번호를 보내야 한다. ( pUser->m_Snapshot.dpidUser는 소켓번호 )
		QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer ); // pUser->m_Snapshot.dpidUser에는 소켓번호가 들어가 있다.
		QueryDestroyPlayer( dpidCache, dpidSocket, dpidUser, idPlayer );	
		return;
	}

	pUser = g_UserMng.AddUser( dpidCache, dpidUser, dpidSocket );
	if( pUser == NULL )
		return;

	pUser->m_dwAuthKey = dwAuthKey;
	memcpy( pUser->m_playAccount.lpAddr, lpAddr, 16 );

	//	TRANS
	BEFORESENDDUAL( arJoin, PACKETTYPE_JOIN, dpidCache, dpidUser );
	arJoin << dwAuthKey;
	arJoin.WriteString( lpszAccount );
	arJoin.WriteString( lpszpw );
	arJoin << nSlot << idPlayer;
	SEND( arJoin, &g_dpDBClient, DPID_SERVERPLAYER );
}


void CDPSrvr::OnRemoveUser( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	g_UserMng.RemoveUser( (DWORD)dpidUser ); // dpidUser는 CACHE에서 사용되는 serial한 값 
}

void CDPSrvr::OnChat( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	static	TCHAR	sChat[1024];
	if( uBufSize > 1031 )	// 4 + 4 + 1024 - 1		= 1031
		return;

	ar.ReadString( sChat, 1024 );
	CString strChat	= sChat;
	strChat.Replace( "\\n", " " );
	
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		if( pUser->m_dwAuthorization >= AUTH_LOGCHATTING )		// 일반유저가 아니면 로그남김 모든 로그남김
		{
			g_dpDBClient.SendLogGamemaChat( pUser, strChat );
		}

		if( sChat[0] == '/'  && ParsingCommand( strChat, (CMover*)pUser ) )
			return;
#ifdef __AEGON_EMOTE_SYSTEM
		if (sChat[0] == '/') 
		{
			__AEGON_EMOTE_UNIT* pEmoteUnit = CAegonEmoticonMng::GetInstance()->GetUnitByCommand(sChat, 1);
			if (!pEmoteUnit && pUser->IsAuthHigher(AUTH_GAMEMASTER)) 
				pEmoteUnit = CAegonEmoticonMng::GetInstance()->GetUnitByCommand(sChat, 2);
			if (pEmoteUnit)
			{
				BOOL bEmoteOwned = CAegonEmoticonMng::GetInstance()->PlayerOwnsEmote((CMover*)pUser, pEmoteUnit->nEmoteID, pEmoteUnit->GetUnitType());
				if (!bEmoteOwned) 
					return;
			}
		}
#endif //__AEGON_EMOTE_SYSTEM

		if( pUser->IsMode( TALK_MODE ) )
			return;

#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bChat == FALSE) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_01), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU

#ifdef __JEFF_9_20
#if __VER >= 12 // __LORD
		int nText	= pUser->GetMuteText();
		if(  nText )
		{
			pUser->AddDefinedText( nText );
			return;
		}
#else	// __LORD
		if( pUser->IsMute() )
			return;
#endif	// __LORD
#endif	// __JEFF_9_20

		if( !( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT ) ) )
		{
//			strChat		= sChat;
//			strChat.Replace( "\\n", " " );
			ParsingEffect( sChat, strlen(sChat) );
		}
		strChat	= sChat;
		strChat.Replace( "\\n", " " );
//		RemoveCRLF( sChat );

	
		g_UserMng.AddChat( pUser, strChat );

#ifdef __AFK_MONITORING
		if (pUser->m_dwAFKState == 3)
		{
			pUser->m_dwAFKState = NULL;
			g_UserMng.AddMessiState(pUser);
		}
		pUser->m_dwLastWalkTick[0] = GetTickCount() + MIN(MIN_AUTO_AFK);
#endif //__AFK_MONITORING
	}
}

void CDPSrvr::OnCtrlCoolTimeCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser;
	CCommonCtrl* pCtrl;
	pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;
	
	((CMover*)pUser)->m_dwCtrlReadyTime = 0xffffffff;

	pCtrl = (CCommonCtrl*)prj.GetCtrl( ((CMover*)pUser)->m_dwCtrlReadyId );
	if( IsValidObj( pCtrl ) == FALSE )
		return;

	((CMover*)pUser)->m_dwCtrlReadyId = NULL_ID;

	pCtrl->m_dwCtrlReadyTime = 0xffffffff;
	pCtrl->m_bAction         = FALSE;
}

void CDPSrvr::OnDoEquip( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD nId;
	int nPart;

	ar >> nId;
	ar >> nPart;		
	
	if( nPart >= MAX_HUMAN_PARTS )	
		return;

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

#ifdef __AEGON_FIXES
	//Fix neuz overload by spamming eq
	if (GetTickCount() > pUser->m_dwEqUseLastReset + 10000)
	{
		pUser->m_dwEqUseLastReset = GetTickCount();
		pUser->m_dwEqUseCount = 0;
	}

	if (++pUser->m_dwEqUseCount > 30)
	{
		return;
	}
#endif

#ifdef __GAMEGUARD
	if (pUser->GetHitPoint() > pUser->GetMaxHitPoint())
		pUser->SetHitPoint(pUser->GetMaxHitPoint());

	if (pUser->GetManaPoint() > pUser->GetMaxManaPoint())
		pUser->SetManaPoint(pUser->GetMaxManaPoint());

	if (pUser->GetFatiguePoint() > pUser->GetMaxFatiguePoint())
		pUser->SetFatiguePoint(pUser->GetMaxFatiguePoint());
#endif //__GAMEGUARD

	CItemElem* pItemElem = pUser->m_Inventory.GetAtId( nId );
	if( IsUsableItem( pItemElem ) == FALSE )
			return;
	if( nPart > 0 )
	{
		if( pUser->m_Inventory.IsEquip( nId ) )
		{
			if( pItemElem != pUser->m_Inventory.GetEquip( nPart ) )
				return;
		}
	}
	else
	{
		if( pUser->m_Inventory.IsEquip( nId ) )
			return;
	}
#ifdef __HACK_1023
	ItemProp* pItemProp		= pItemElem->GetProp();
	if( pItemProp && pItemProp->dwParts == PARTS_RIDE )
	{
		if( !pUser->m_Inventory.IsEquip( nId ) )
		{
			FLOAT fVal;
			ar >> fVal;
			if( fVal != pItemProp->fFlightSpeed )
			{
				pUser->AddDefinedText( TID_GAME_MODIFY_FLIGHT_SPEED );
				return;
			}
		}
	}
#endif	// __HACK_1023

#ifdef __WEAPON_RARITY
	if (!prj.CheckWeaponRarity(pItemElem->GetWeaponRarity()))
	{
		Error("%s : pItemElem->GetWeaponRarity() returns %d!", pUser->GetName(), pItemElem->GetWeaponRarity());

		int nRarity = pItemElem->GetWeaponRarity() > prj.m_nHighWeaponRarityLevel ? prj.m_nHighWeaponRarityLevel : 0;
		pItemElem->SetWeaponRarity(nRarity);
		pUser->AddWeaponRarity(pItemElem->m_dwObjId, nRarity);
		return;
	}
#endif // __WEAPON_RARITY

	if( pUser->IsDie() == FALSE )
		pUser->DoUseEquipmentItem( pItemElem, nId, nPart );
}

void CDPSrvr::OnMoveItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BYTE nItemType, nSrcIndex, nDstIndex;
	ar >> nItemType >> nSrcIndex >> nDstIndex;

	if( nSrcIndex == nDstIndex )
		return;
	if( nDstIndex >= MAX_INVENTORY || nSrcIndex >= MAX_INVENTORY )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	CItemElem* pItemSrc = pUser->m_Inventory.GetAt( nSrcIndex );
	CItemElem* pItemDst = pUser->m_Inventory.GetAt( nDstIndex );
	if( pItemDst == NULL || IsUsableItem( pItemDst ) ) // 빈 공간 or 거래중이지 않는 아이템 ?			
	{	
		if( IsUsableItem( pItemSrc ) )					// 거래중이지 않는 아이템 ?
		{
			pUser->m_Inventory.Swap( nSrcIndex, nDstIndex );
			pUser->AddMoveItem( 0, nSrcIndex, nDstIndex );
		}
	}
}

void CDPSrvr::OnDropItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwItemType;
	DWORD dwItemId;
	short nDropNum;
	D3DXVECTOR3 vPos;

	ar >> dwItemType >> dwItemId >> nDropNum >> vPos;

#ifdef __GAMEGUARD
	if (nDropNum <= 0)
		return;
#endif // __GAMEGUARD

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj(pUser) )
	{
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE
#ifdef __SECURITY_FIXES
		CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwItemId);
		if (!IsUsableItem(pItemElem))
			return;
		if (pItemElem->m_nItemNum < nDropNum)
			return;
#endif // __SECURITY_FIXES
		if( g_eLocal.GetState( EVE_DROPITEMREMOVE ) )
		{
			CItemElem* pItemElem	= (CItemElem*)pUser->GetItemId( dwItemId );
			if (IsUsableItem(pItemElem) && pUser->IsDropable(pItemElem, FALSE))
			{
#ifdef __SYS_LOCK_ITEM
				if (pItemElem->IsItemLocked())
				{
					CString str;
					str.Format("Please unlock %s before dropping.", pItemElem->GetProp()->szName);
					pUser->AddText(str, COLOR_WARNING);
					return;
				}
#endif // __SYS_LOCK_ITEM
				pUser->RemoveItem((BYTE)dwItemId, nDropNum);
			}
		}
		else
		{
			pUser->DropItem( (DWORD)dwItemId, nDropNum, vPos );
		}
	}
}

void CDPSrvr::OnDropGold( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{

#ifdef __MIKSIK_EQUALIZED_MODE
	return;
#endif //__MIKSIK_EQUALIZED_MODE
	
#if __VER >= 8  	//8차게임내돈드롭금지

	return;

#else	//  __VER >= 8  

	DWORD dwGold;
	D3DXVECTOR3 vPos;
	ar >> dwGold >> vPos;

	BOOL bLimit = FALSE;
	if( dwGold > 30000 )	// 3만 페냐 이상은 땅에 떨어뜨릴 수 없습니다. 
	{
		dwGold = 30000;
		bLimit = TRUE;
	}

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( g_eLocal.GetState( EVE_DROPITEMREMOVE ) )
		{
			if( pUser->GetGold() >= (int)dwGold )	// raiders.2006.11.28
				pUser->AddGold( -dwGold );
		}
		else
		{
			if( pUser->DropGold( dwGold, vPos ) != NULL && bLimit )
				pUser->AddDefinedText( TID_GAME_LIMITMONEY, "" );
		}
	}

#endif	//	  __VER >= 8  
}


void CDPSrvr::OnReplace( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	ASSERT( 0 );
	Error("CDPSrvr::OnReplace called\n");
}

void CDPSrvr::OnScriptDialogReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	static TCHAR lpKey[256];
	OBJID objid;
	int nGlobal1, nGlobal2, nGlobal3, nGlobal4;

	ar >> objid;
 	ar.ReadString( lpKey, 256 );

	ar >> nGlobal1 >> nGlobal2 >> nGlobal3 >> nGlobal4;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __QUEST_1208
		DWORD dwTickCount	= GetTickCount();
		if( dwTickCount < pUser->m_tickScript + 400 )
			return;
		pUser->m_tickScript	= dwTickCount;
#endif	// __QUEST_1208
		CMover* pMover	= prj.GetMover( objid );
		if( IsValidObj( pMover ) )
		{
#ifdef __HACK_1130
			char lpOutputString[512]	= { 0,};
			sprintf( lpOutputString, "npc = %s, key = %s, n1 = %d, n2 = %d, n3 = %d, n4 = %d",
				pMover->GetName(), lpKey, nGlobal1, nGlobal2, nGlobal3, nGlobal4 );
			OutputDebugString( lpOutputString );
#endif	// __HACK_1130

			D3DXVECTOR3 vOut	= pUser->GetPos() - pMover->GetPos();
			if( fabs( (double)D3DXVec3LengthSq( &vOut ) ) > MAX_LEN_MOVER_MENU )
			{
				return;
			}
		#if !defined(__REMOVE_SCIRPT_060712)
			CScriptDialog::SetLatestDialog( pMover->GetName(), lpKey );
		#endif
			if( pMover->m_pNpcProperty &&
				pMover->m_pNpcProperty->IsDialogLoaded() )
			{
				if( lstrlen( lpKey ) == 0 )
					lstrcpy( lpKey, _T( "#init" ) );
				if( nGlobal3 == 0 )	
					nGlobal3 = (int)pMover->GetId();
				if( nGlobal4 == 0 )	
					nGlobal4 = (int)pUser->GetId();

				pMover->m_pNpcProperty->RunDialog( lpKey, NULL, nGlobal1, (int)pMover->GetId(), (int)pUser->GetId(), nGlobal2 );

				// 퀘스트 조건 대화에 맞는 키일 경우는 대화 성공 플렉을 세팅하고 퀘스트 정보를 클라이언트에 보내준다.
				for( int i = 0; i < pUser->m_nQuestSize; i++ )
				{
					LPQUEST lpQuest = &pUser->m_aQuest[ i ];
					QuestProp* pQuestProp = pUser->m_aQuest[ i ].GetProp();
					if( pQuestProp )
					{
						if( strcmp( pQuestProp->m_szEndCondDlgCharKey, pMover->m_szCharacterKey ) == 0 )
						{
							if( strcmp( pQuestProp->m_szEndCondDlgAddKey, lpKey ) == 0 )
							{
								lpQuest->m_bDialog = TRUE;
								pUser->AddSetQuest( lpQuest );
								break;
							}
						}
					}
				}
			}
		}
	}
}

void CDPSrvr::OnRevival( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CWorld* pWorld;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) )
	{
		pUser->m_Resurrection_Data.bUseing = FALSE;

		if( pUser->IsDie() == FALSE )
		{
			Error( "OnRevival\t// %s is not dead", pUser->GetName() );
			return;
		}
		
		BOOL bGuildCombat = FALSE;

		CItemElem* pItemElem = pUser->m_Inventory.GetAtItemId( II_SYS_SYS_SCR_RESURRECTION );
		if( IsUsableItem( pItemElem ) )
		{
			pUser->m_nDead = PROCESS_COUNT * 5;		// 죽은 후 5초간은 무적

#if __VER >= 8 // __S8_PK
			if( pUser->IsChaotic() )
			{
				pUser->SubDieDecExp( TRUE, 0, TRUE );		// 죽어서 부활하면 겸치 깎임,.
			}
			else
			{
#if __VER < 12 // __ANGEL_NODIE
				if( !pUser->m_bLastPK && !pUser->m_bGuildCombat )
					pUser->RemoveAngel();
#endif // __ANGEL_NODIE
			}
#endif // __VER >= 8 // __S8_PK
			float fRate = 0.1f;

#if __VER >= 9 // __S_9_ADD
			fRate = 0.2f;
#else // __S_9_ADD
			int nLevel	= pUser->GetLevel();
			if( nLevel == 1 )		{ fRate	= 0.8f; }
			else if( nLevel == 2 )	{ fRate	= 0.6f; }
			else if( nLevel <= 5 )	{ fRate	= 0.5f; }
			else if( nLevel <= 10 )	{ fRate	= 0.4f; }
			else if( nLevel <= 15 )	{ fRate	= 0.3f; }
#endif // __S_9_ADD

			pUser->m_pActMover->ClearState();

			if( pWorld->GetID() == WI_WORLD_GUILDWAR )
				fRate = 1.0f;

#if __VER < 9 // __S_9_ADD	// 아래에서 채워주고 패킷까지 보내버리는 것으로 수정
			pUser->SetHitPoint( (int)(pUser->GetMaxHitPoint() * fRate) );	// hp 회복

			int nVal	= (int)(pUser->GetMaxManaPoint() * fRate);			// mp 회복
			if( pUser->GetManaPoint() < nVal )
				pUser->SetManaPoint( nVal );
			
			nVal	= (int)(pUser->GetMaxFatiguePoint() * fRate);			// fp 회복
			if( pUser->GetFatiguePoint() < nVal )
				pUser->SetFatiguePoint( nVal );
#endif // __S_9_ADD

#ifdef __ROYAL_RUMBLE
			if (pWorld->GetID() == WI_WORLD_FWC)
			{
				g_UserMng.AddHdr(pUser, SNAPSHOTTYPE_REVIVAL);
			}
			else
#endif // __ROYAL_RUMBLE
			if( pWorld->GetID() == WI_WORLD_GUILDWAR )
			{
				g_UserMng.AddHdr( pUser, SNAPSHOTTYPE_REVIVAL );
				g_GuildCombatMng.JoinObserver( pUser );
			}
			else
			{
				g_UserMng.AddHdr( pUser, SNAPSHOTTYPE_REVIVAL );

				ItemProp* pItemProp = pItemElem->GetProp();
				if( pItemProp && pItemProp->dwSfxObj3 != -1 )
					g_UserMng.AddCreateSfxObj((CMover *)pUser, pItemElem->GetProp()->dwSfxObj3 );

				// 상용화 아이템 사용 로그 삽입
				g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemElem, pItemProp );
				pUser->RemoveItem( (BYTE)pItemElem->m_dwObjId, 1 );
			}

#if __VER >= 9 // __S_9_ADD
			pUser->SetPointParam( DST_HP, (int)( pUser->GetMaxHitPoint() * fRate ) );
					
			int nVal	= (int)(pUser->GetMaxManaPoint() * fRate);			// mp 회복
			if( pUser->GetManaPoint() < nVal )
				pUser->SetPointParam( DST_MP, nVal );
			
			nVal	= (int)(pUser->GetMaxFatiguePoint() * fRate);			// fp 회복
			if( pUser->GetFatiguePoint() < nVal )
				pUser->SetPointParam( DST_FP, nVal );			
#endif // __S_9_ADD
		}
		else
		{
			Error( "Error CMover::OnRevival Not Inventory RevivalItem" );
		}
	}
}

void CDPSrvr::OnRevivalLodestar( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CWorld* pWorld;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) )
	{
		pUser->m_Resurrection_Data.bUseing = FALSE;

		if( pUser->IsDie() == FALSE )
		{
			// 이리로 들어오는 경우가 생긴다. 생각엔 이미 한번 여길 들어왔다가 렉 걸린동안 
			// 로드스타를 또 누르면 들어오는게 아닌가 싶다.
			return;
		}

		CCommonCtrl* pCtrl	= CreateExpBox( pUser );
		if( pCtrl )
		{
			pCtrl->AddItToGlobalId();
			pWorld->ADDOBJ( pCtrl, FALSE, pUser->GetLayer() );
			g_dpDBClient.SendLogExpBox( pUser->m_idPlayer, pCtrl->GetId(), pCtrl->m_nExpBox );
		}
		
		BOOL bGuildCombat = FALSE;
		
		g_dpDBClient.SendLogLevelUp( (CMover*)pUser, 9 );	// 로드스타로 부활 로그

		pUser->m_nDead = PROCESS_COUNT * 5;		// 죽은 후 5초간은 무적
		float fRate		= pUser->SubDieDecExp();		// 죽어서 부활하면 겸치 깎임,.
		pUser->m_pActMover->ClearState();

#ifdef __ROYAL_RUMBLE
		if (pWorld->GetID() == WI_WORLD_FWC)
			fRate = 1.0f;
		else
#endif // __ROYAL_RUMBLE
		if( pWorld->GetID() == WI_WORLD_GUILDWAR )
			fRate = 1.0f;
#if __VER >= 11 // __GUILD_COMBAT_1TO1
		else if( g_GuildCombat1to1Mng.IsPossibleUser( pUser ) )
			fRate = 1.0f;
#endif // __GUILD_COMBAT_1TO1
#if __VER >= 9 // __S_9_ADD
		else
			fRate = 0.2f;
#endif // __S_9_ADD
		
#if __VER < 9 // __S_9_ADD	// 아래에서 채워주고 패킷까지 보내버리는 것으로 수정
		pUser->SetHitPoint( (int)(pUser->GetMaxHitPoint() * fRate) );	// hp 회복
		
		int nVal	= (int)(pUser->GetMaxManaPoint() * fRate);			// mp 회복
		if( pUser->GetManaPoint() < nVal )
			pUser->SetManaPoint( nVal );
		
		nVal	= (int)(pUser->GetMaxFatiguePoint() * fRate);			// fp 회복
		if( pUser->GetFatiguePoint() < nVal )
			pUser->SetFatiguePoint( nVal );
#endif // __S_9_ADD

#ifdef __ROYAL_RUMBLE
		if (pWorld->GetID() == WI_WORLD_FWC)
		{
			pUser->Revive();
			g_UserMng.AddHdr(pUser, SNAPSHOTTYPE_REVIVAL_TO_LODESTAR);
		}
		else
#endif // __ROYAL_RUMBLE
		if( pWorld->GetID() == WI_WORLD_GUILDWAR )
		{
			g_UserMng.AddHdr( pUser, SNAPSHOTTYPE_REVIVAL );
			g_GuildCombatMng.JoinObserver( pUser );
		}
		else
		{
			g_UserMng.AddHdr( pUser, SNAPSHOTTYPE_REVIVAL_TO_LODESTAR );
		}		

#if __VER >= 9 // __S_9_ADD
		pUser->SetPointParam( DST_HP, (int)(pUser->GetMaxHitPoint() * fRate) );	// hp 회복
		
		int nVal	= (int)(pUser->GetMaxManaPoint() * fRate);			// mp 회복
		//if( pUser->GetManaPoint() < nVal )
			pUser->SetPointParam( DST_MP, nVal );
		
		nVal	= (int)(pUser->GetMaxFatiguePoint() * fRate);			// fp 회복
		//if( pUser->GetFatiguePoint() < nVal )
			pUser->SetPointParam( DST_FP, nVal );
#endif // __S_9_ADD

		if( pWorld->GetID() == WI_WORLD_GUILDWAR )
			return;

#ifdef __ROYAL_RUMBLE
		if (pWorld->GetID() == WI_WORLD_FWC)
			return;
#endif // __ROYAL_RUMBLE

#if __VER >= 12 // __SECRET_ROOM
		if( CSecretRoomMng::GetInstance()->IsInTheSecretRoom( pUser ) )
		{
			pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, CSecretRoomMng::GetInstance()->GetRevivalPos( pUser ), REPLACE_NORMAL, nDefaultLayer );
			return;
		}
#endif // __SECRET_ROOM
		
		// 보스몹 맵에서 죽었다...
		// 그러면 부활은 마을에서...
		if( pWorld->GetID() == WI_DUNGEON_MUSCLE || pWorld->GetID() == WI_DUNGEON_KRRR || pWorld->GetID() == WI_DUNGEON_BEAR )
		{			
			pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3(6975.65f, 0.0f, 3331.5f), REPLACE_NORMAL, nDefaultLayer );
			return;
		}

		PRegionElem	pRgnElem	= NULL;
		if( pUser->IsChaotic()
#ifdef __JEFF_11_4
			&& !pWorld->IsArena()
#endif	// __JEFF_11_4
			)
		{
#if __VER >= 8 // __S8_PK
			if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
				pRgnElem	= g_WorldMng.GetRevivalPosChao( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
			if( NULL == pRgnElem )	// Find near revival pos
				pRgnElem	= g_WorldMng.GetNearRevivalPosChao( pWorld->GetID(), pUser->GetPos() );	
#else // __VER >= 8 // __S8_PK
			pRgnElem	= g_WorldMng.GetNearRevivalPosChao( pWorld->GetID(), pUser->GetPos() );
#endif // __VER >= 8 // __S8_PK
		}
		else
		{
			if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
				pRgnElem	= g_WorldMng.GetRevivalPos( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
			
			if( NULL == pRgnElem )	// Find near revival pos
				pRgnElem	= g_WorldMng.GetNearRevivalPos( pWorld->GetID(), pUser->GetPos() );	
		}

		if( pRgnElem ) 

#ifdef __SYS_MONSTER_CLASH
			if (CMonsterClashMng::GetInstance()->IsClash(pUser))
				pUser->REPLACE(g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_FORCE, pUser->GetLayer());
			else
#endif //__SYS_MONSTER_CLASH
#ifdef __SYS_SANDBOX
			if (pUser->IsWorldSandBox())
				pUser->REPLACE(g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_FORCE, pUser->GetLayer());
			else
#endif //__SYS_SANDBOX
			pUser->REPLACE( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_FORCE, nRevivalLayer );
		else 
			pUser->REPLACE( g_uIdofMulti, pWorld->GetID(), pUser->GetPos(), REPLACE_FORCE, pUser->GetLayer() );
	}
}

void CDPSrvr::OnRevivalLodelight( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
}

void CDPSrvr::OnSetLodelight( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_MARKING, pUser->GetWorld(), pUser->GetPos() ) )
			return;
		pUser->SetMarkingPos();
		pUser->AddDefinedText( TID_GAME_LODELIGHT, "" );
#ifdef __GAMEGUARD	
		if (pUser->GetWorld())
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
#endif // __GAMEGUARD
	}
}

void CDPSrvr::OnIncStatLevel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CHAR chID;
	ar >> chID;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		long nValue;
		
		switch( chID )
		{
			case 100:	
				pUser->IncStrLevel();
				nValue	= pUser->m_nStr;
				break;
			case 101:
				pUser->IncStaLevel();
				nValue	= pUser->m_nSta;
				break;
			case 102:
				pUser->IncDexLevel();
				nValue	= pUser->m_nDex;
				break;
			case 103:
				pUser->IncIntLevel();
				nValue	= pUser->m_nInt;
				break;
			default:
				return;			
		}
//		pUser->AddSetStatLevel( chID, nValue, pUser->m_nRemainGP );
		g_UserMng.AddSetStateLevel( pUser, chID, nValue );
		g_dpDBClient.SendLogLevelUp( pUser, 2 );
	}
}

void CDPSrvr::OnCorrReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID idObj;
	ar >> idObj;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );		// 어느유저로부터 날아온거냐.
	if( IsValidObj( pUser ) )
	{
		CMover *pMover = prj.GetMover( idObj );	// 선택된 오브젝트의 포인터
		if( IsValidObj( pMover ) )
		{
			pUser->AddCorrReq( pMover );	// 요청한 클라에게 선택된 오브젝트의 정보를 보냄.
		}
	}
}

void CDPSrvr::OnCreateGuildCloak( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )		
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );		// 어느유저로부터 날아온거냐.
	if( IsValidObj( pUser ) )
	{
		CGuild *pGuild = pUser->GetGuild();
		if( pGuild == NULL )
			return;

#ifdef __FIX_GUILD_OVERFLOW
		if (pGuild->m_dwLogo == 0 || pGuild->m_dwLogo > 20 || pGuild->IsMaster(pUser->m_idPlayer) == FALSE)
#else //__FIX_GUILD_OVERFLOW
		if (pGuild->m_dwLogo == 0 || pGuild->IsMaster(pUser->m_idPlayer) == FALSE)
#endif //__FIX_GUILD_OVERFLOW
		{
			if( pGuild->m_dwLogo == 0 )
			{
				// 로그가 지정되지 않아서 못만듬
				pUser->AddDefinedText( TID_GAME_GUILDSETTINGLOGO, "" );
			}
			else
			{
				// 마스터가 아니므로 만들수가 없음.
				pUser->AddDefinedText( TID_GAME_GUILDONLYMASTERLOGO, "" );
			}
			return;
		}

		if ( MAX_BANK <= pGuild->m_GuildBank.GetCount() )
		{
			pUser->AddDefinedText( TID_GAME_GUILDBANKFULL, "" );		// 길드창고가 꽉찼시유~
			return;
		}

		if ( pGuild->m_nGoldGuild >= 10000 )	 // 길드창고에 돈이 충분하냐?
		{
			pGuild->m_nGoldGuild -= 10000;

			CItemElem itemElem;
			if( pGuild->m_dwLogo == 999 )	// 커스텀 로고로 설정되어 있을때.
				itemElem.m_dwItemId	= II_ARM_S_CLO_CLO_BLANK;		// 커스텀용 민짜 망토생성.
			else
			{
				if(pUser->IsAuthHigher(AUTH_GAMEMASTER) && pGuild->m_dwLogo > CUSTOM_LOGO_MAX - 7) // GM Guild Logo사용 시 임의로 망토 생성
					itemElem.m_dwItemId	= II_ARM_S_CLO_CLO_SYSCLOAK01;
				else
					itemElem.m_dwItemId	= II_ARM_S_CLO_CLO_SYSCLOAK01 + (pGuild->m_dwLogo - 1);
			}
			itemElem.m_nItemNum		= 1;
			itemElem.m_idGuild	= pGuild->m_idGuild;			

			itemElem.SetSerialNumber();
			ItemProp* pItemProp		= itemElem.GetProp();
			if( pItemProp )
				itemElem.m_nHitPoint	= pItemProp->dwEndurance;
			else
				itemElem.m_nHitPoint	= 0;

			pUser->AddPutItemGuildBank( &itemElem );
			pGuild->m_GuildBank.Add( &itemElem );
			g_UserMng.AddPutItemElem( pUser, &itemElem );
						
			CGuildMember*	pMember;
			CUser*			pUsertmp;
			CGuild*			pGuild = pUser->GetGuild();
			for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
			i != pGuild->m_mapPMember.end(); ++i )
			{
				pMember		= i->second;
				pUsertmp	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
				if( IsValidObj( pUsertmp ) )
				{
					pUsertmp->AddGetGoldGuildBank( 10000, 2, pMember->m_idPlayer, 1 );	// 2는 업데이트 해야할 클라이게
				}
			}
			g_DPCoreClient.SendGuildMsgControl_Bank_Penya( pUser, 10000, 2, 1 ); 	// 2는 업데이트 해야할 다른 월드서버의 클라이언트
			UpdateGuildBank(pGuild, GUILD_CLOAK, 0, pUser->m_idPlayer, &itemElem, 10000, 1 ); // 0은 길드 페냐를 업데이트 한다는 것이다.(실은 모든것을 업데이트하지만 -_-)
			pUser->AddDefinedText( TID_GAME_GUILDCREATECLOAK, "" );
		} 
		else
		{
			pUser->AddDefinedText( TID_GAME_GUILDNEEDGOLD, "" );		// 길드창고에 돈이 엄떵!
		}
	}
}

void CDPSrvr::OnQueryGetDestObj( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objid;
	ar >> objid;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CMover* pMover	= prj.GetMover( objid );
		if( IsValidObj( pMover ) && !pMover->IsEmptyDestObj() )
			pUser->AddGetDestObj( objid, pMover->GetDestId(), pMover->m_fArrivalRange );
	}
}

void CDPSrvr::OnGetDestObj( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objid, objidDest;
	ar >> objid >> objidDest;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
		pUser->SetDestObj( objidDest );
}

void CDPSrvr::OnQueryGetPos( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objid;
	ar >> objid;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CMover* pMover	= prj.GetMover( objid );
		if( IsValidObj( pMover ) )
		{
			if( FALSE == pMover->IsPlayer() )
			{
				pUser->AddGetPos( objid, pMover->GetPos(), pMover->GetAngle() );
			}
			else
			{
				( (CUser*)pMover )->AddQueryGetPos( pUser->GetId() );
			}
		}
	}
}

void CDPSrvr::OnGetPos( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	D3DXVECTOR3 vPos;
	float fAngle;
	OBJID objid;
	ar >> vPos >> fAngle >> objid;
	
	if( _isnan((double)fAngle) )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		D3DXVECTOR3 vDistance	= pUser->GetPos() - vPos;
		if( D3DXVec3LengthSq( &vDistance ) > 1000000.0F )
		{
			return;
		}

		if( NULL_ID == objid )
		{
			pUser->SetPos( vPos );
			pUser->SetAngle( fAngle );
			
			pUser->m_fWaitQueryGetPos	= FALSE;
			
			if( FALSE == pUser->IsEmptyDestPos() )
				pUser->SetDestPos( pUser->GetDestPos(), pUser->m_bForward, FALSE );
		}
		else
		{
			CUser* ptr	= prj.GetUser( objid );
			if( IsValidObj( ptr ) )
				ptr->AddGetPos( pUser->GetId(), vPos, fAngle );
		}
	}
}

void CDPSrvr::OnPartyRequest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uLeaderid, uMemberid;
	BOOL bTroup;
	ar >> uLeaderid >> uMemberid;
	ar >> bTroup;

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) && pUser->m_idPlayer == uLeaderid )
		InviteParty( uLeaderid, uMemberid, bTroup );
}

void CDPSrvr::OnPartyRequestCancle( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uLeaderid, uMemberid;
	int nMode;
	ar >> uLeaderid >> uMemberid >> nMode;

	CUser* pUser = g_UserMng.GetUserByPlayerID( uLeaderid );
	if( IsValidObj( pUser ) )
		pUser->AddPartyRequestCancel( uLeaderid, uMemberid, nMode );
}

void CDPSrvr::OnPartySkillUse( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uLeaderid;
	int nSkill;
	ar >> uLeaderid >> nSkill;	

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsInvalidObj( pUser ) )		
		return;
	
	CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
	if( pParty )
	{
#ifdef __PARTY_ASSISTANT
		if (pParty->IsLeader(pUser->m_idPlayer) || (pParty->IsMember(pUser->m_idPlayer) && pUser->GetAssist() == TRUE))
		{
			pParty->DoUsePartySkill(pUser->m_idparty, pUser->m_idPlayer, nSkill);
		}
#else //__PARTY_ASSISTANT
		if (pParty->IsLeader(pUser->m_idPlayer))
			pParty->DoUsePartySkill(pUser->m_idparty, pUser->m_idPlayer, nSkill);
#endif //__PARTY_ASSISTANT
	}
}

void CDPSrvr::OnAddFriendReqest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uLeaderid, uMemberid;
	ar >> uLeaderid >> uMemberid;
	
	CUser* pLeader	= g_UserMng.GetUserByPlayerID( uLeaderid );
	CUser* pMember	= g_UserMng.GetUserByPlayerID( uMemberid );
	if( IsValidObj( pMember ) && IsValidObj( pLeader ) )
	{
#ifdef __SECURITY_FIXES
		// Check if user try to add himself
		if (pLeader->m_idPlayer == pMember->m_idPlayer)
			return;
#endif // __SECURITY_FIXES

#if __VER >= 8 // 8차 듀얼 061226 ma
		if( 0 < pLeader->m_nDuel ||  0 < pMember->m_nDuel )
		{
			return;
		}
#endif // __VER >= 8 // 8차 듀얼 061226 ma

		// 길드대전장에는 친구추가를 할수 없습니다
		CWorld* pWorldLeader = pLeader->GetWorld();
		CWorld* pWorldMember = pMember->GetWorld();
		if( ( pWorldLeader && pWorldLeader->GetID() == WI_WORLD_GUILDWAR ) ||
			( pWorldMember && pWorldMember->GetID() == WI_WORLD_GUILDWAR ) )
		{			
			pLeader->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_FRIENDADD) );
			return;
		}
#if __VER >= 11 // __GUILD_COMBAT_1TO1
		if( g_GuildCombat1to1Mng.IsPossibleUser( pLeader ) || g_GuildCombat1to1Mng.IsPossibleUser( pMember ) )
		{
			pLeader->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_FRIENDADD) );
			return;
		}
#endif // __GUILD_COMBAT_1TO1

#ifdef __RT_1025
		if( !pLeader->m_RTMessenger.GetFriend( uMemberid ) )
#else	// __RT_1025
		if( FALSE == pLeader->m_Messenger.IsFriend( uMemberid ) )
#endif	// __RT_1025
		{
			if( pMember->IsAttackMode() )
				pLeader->AddDefinedText( TID_GAME_BATTLE_NOTFRIEND, "" );
			else
				pMember->AddFriendReqest( uLeaderid, pLeader->m_nJob, (BYTE)pLeader->GetSex(), pLeader->GetName() );	// 친구 등록 여부 질의
		}
	}
}

// 다른 멀티서버에 있는 캐릭을 추가 시키려면 코어로 보내야 한다.
// 이름으로 오므로 월드에서 idPlayer로 바꿔서 보냄
void CDPSrvr::OnAddFriendNameReqest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __GAMEGUARD
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
#endif // __GAMEGUARD
	{
		u_long uLeaderid, uMember;
		char szMemberName[64] = { 0, };

		ar >> uLeaderid;
		ar.ReadString(szMemberName, 64);

#ifdef __GAMEGUARD
		if (pUser->m_idPlayer != uLeaderid)
		{
			Error("CDPSrvr::OnAddFriendNameReqest : pUser->m_idPlayer == %d - uLeaderid == %d", pUser->GetId(), uLeaderid);
			return;
		}
		if (prj.IsInvalidName(szMemberName) || !prj.IsAllowedLetter(szMemberName) || strlen(szMemberName) > MAX_NAME || strcmp(szMemberName, pUser->GetName()) == 0)
		{
			Error("CDPSrvr::OnAddFriendNameReqest : pUser->m_idPlayer == %d - uLeaderid == %d - szMemberName == %s", pUser->GetId(), uLeaderid, szMemberName);
			return;
		}
#endif // __GAMEGUARD

#if __VER >= 11 // __SYS_PLAYER_DATA
		uMember = CPlayerDataCenter::GetInstance()->GetPlayerId(szMemberName);
#else	// __SYS_PLAYER_DATA
		uMember = prj.GetPlayerID(szMemberName);
#endif	// __SYS_PLAYER_DATA
		CUser* pLeader = g_UserMng.GetUserByPlayerID(uLeaderid);

		if (IsValidObj(pLeader))
		{
			if (uMember > 0)
			{
#ifdef __RT_1025
				if (!pLeader->m_RTMessenger.GetFriend(uMember))
#else	// __RT_1025
				if (pLeader->m_Messenger.IsFriend(uMember) == FALSE)
#endif	// __RT_1025
					g_DPCoreClient.SendAddFriendNameReqest(uLeaderid, pLeader->m_nJob, (BYTE)pLeader->GetSex(), uMember, pLeader->GetName(), szMemberName);
				else
					pLeader->AddFriendError(1, szMemberName);
			}
			else
			{
				// 이 이름을 가지고 잇는 캐릭은 없음.
				pLeader->AddFriendError(2, szMemberName);
			}
		}
	}
}


void CDPSrvr::OnAddFriendCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uLeaderid, uMemberid;
	ar >> uLeaderid >> uMemberid;

	CUser* pLeader = g_UserMng.GetUserByPlayerID( uLeaderid );
	if( IsValidObj( pLeader ) )
		pLeader->AddFriendCancel();	// uMemberid
}

void CDPSrvr::OnActionPoint( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int nAP;
	ar >> nAP;

	if( nAP < 0 )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		pUser->m_playTaskBar.m_nActionPoint = nAP;
	}
}


void CDPSrvr::OnRemoveQuest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwQuestCancelID;
	ar >> dwQuestCancelID;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __QUEST_1208
		DWORD dwTickCount	= GetTickCount();
		if( dwTickCount < pUser->m_tickScript + 400 )
			return;
		pUser->m_tickScript	= dwTickCount;
#endif	// __QUEST_1208
		LPQUEST lpQuest = pUser->GetQuest( dwQuestCancelID );
		if( lpQuest )
		{
			if( lpQuest->m_nState != QS_END )
			{
				QuestProp* pQuestProp	= prj.m_aPropQuest.GetAt( lpQuest->m_wId );
				if( pQuestProp && pQuestProp->m_bNoRemove == FALSE )
				{
					pUser->RemoveQuest( dwQuestCancelID );
					pUser->AddCancelQuest( dwQuestCancelID );
					g_dpDBClient.CalluspLoggingQuest( pUser->m_idPlayer, dwQuestCancelID, 30 );
					// 시작시 변신을 했으면 퀘스트 삭제시 변신 해제시킨다.
					if( pQuestProp->m_nBeginSetDisguiseMoverIndex )
					{
						pUser->NoDisguise();
						g_UserMng.AddNoDisguise( pUser );
					}
				}
			}
		}
	}
}

#if __VER >= 11 // __SYS_PLAYER_DATA
void CDPSrvr::OnQueryPlayerData( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long idPlayer;
	ar >> idPlayer;
	int nVer;
	ar >> nVer;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( idPlayer );
		if( pPlayerData && pPlayerData->data.nVer != nVer )
			pUser->AddQueryPlayerData( idPlayer, pPlayerData );
	}
}

void CDPSrvr::OnQueryPlayerData2( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int nSize;
//	u_long idPlayer;
	ar >> nSize;

	if( nSize > 1024 )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		for( int i = 0; i < nSize; i++ )
		{
			PDVer	pdv;
			ar.Read( &pdv, sizeof(PDVer) );
			PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( pdv.idPlayer );
			if( pPlayerData && pPlayerData->data.nVer != pdv.nVer )
				pUser->AddQueryPlayerData( pdv.idPlayer, pPlayerData );
		}
	}
}
#else	// __SYS_PLAYER_DATA
void CDPSrvr::OnQueryPlayerString( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long idPlayer;
	BYTE nQuery;
	ar >> idPlayer >> nQuery;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		LPCSTR lpszPlayer	= prj.GetPlayerString( idPlayer );
		if( lpszPlayer )
			pUser->AddQueryPlayerString( idPlayer, lpszPlayer, nQuery );
	}
}

void CDPSrvr::OnQueryPlayerListString( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int nSize;
	u_long idPlayer;
	BYTE nQuery;
	ar >> nSize >> nQuery;

	if( nSize > 1024 )
		return;
		
	CUser* pUser;

	pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) )
	{
		for( int i=0; i<nSize; i++ )
		{
			ar >> idPlayer;
			
			LPCSTR lpszPlayer	= prj.GetPlayerString( idPlayer );
			if( lpszPlayer )
			{
				pUser->AddQueryPlayerString( idPlayer, lpszPlayer, nQuery );
			}
			else
			{
				pUser->AddQueryPlayerString( idPlayer, "Unknown", nQuery );
			}
		}

		if( nSize > 0 )
			pUser->AddQueryPlayerListString( nQuery );
	}
}

void CDPSrvr::OnGetFriendName( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int nCount;
	ar >> nCount;		
	if( nCount <= 0 || nCount > MAX_FRIEND )
		return;

	FRIEND aFriend[MAX_FRIEND];
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		for( int i = 0 ; i < nCount ; i++ )
		{
			ar >> aFriend[i].dwUserId;
#if __VER >= 11 // __SYS_PLAYER_DATA
//			LPCSTR lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( aFriend[i].dwUserId );
#else	// __SYS_PLAYER_DATA
			LPCSTR lpszPlayer	= prj.GetPlayerString( aFriend[i].dwUserId );
			strcpy( aFriend[i].szName, lpszPlayer? lpszPlayer: "" );
#endif	// __SYS_PLAYER_DATA
			
		}
		pUser->AddGetFriendName( nCount, aFriend );
	}
}
#endif	// __SYS_PLAYER_DATA

void CDPSrvr::OnGuildInvite( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objid;
	ar >> objid;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

#ifdef __BLOCK_STAFF_ACTIONS
	if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
	{
		pUser->AddText("[Titanium] Unauthorized Action. (Guild Invite)", COLOR_ERROR);
		return;
	}
#endif //__BLOCK_STAFF_ACTIONS

	InviteCompany( pUser, objid );
}

void CDPSrvr::OnIgnoreGuildInvite( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long idPlayer;
	ar >> idPlayer;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CUser* pPlayer	= g_UserMng.GetUserByPlayerID( idPlayer );		// kingpin
		if( IsValidObj( pPlayer ) )
		{
			pPlayer->AddDefinedText( TID_GAME_COMACCEPTDENY, "%s", pUser->GetName( TRUE ) );
		}
	}
}

// 로고 변경 
void CDPSrvr::OnGuildLogo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwLogo;
	ar >> dwLogo;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( dwLogo > CUSTOM_LOGO_MAX )
		return;

#ifdef __SECURITY_FIXES
	if (dwLogo < 0)
		return;

	if (pUser->GetGuild() == NULL)
		return;

	if (pUser->GetGuild()->m_nLevel < 4)
		return;
#endif // __SECURITY_FIXES

	if( dwLogo > 20 && !pUser->IsAuthHigher( AUTH_GAMEMASTER ) )
		return;

	g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_LOGO, dwLogo );
}

// 공헌도 
void CDPSrvr::OnGuildContribution( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE cbPxpCount, cbItemFlag;
	int nGold;

	cbItemFlag = 0;
	ar >> cbPxpCount >> nGold;
	ar >> cbItemFlag;

	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE ) 
		return;

#ifdef __BLOCK_STAFF_ACTIONS
	if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
	{
		pUser->AddText("[Titanium] Unauthorized Action. (Guild Contributing)", COLOR_ERROR);
		return;
	}
#endif //__BLOCK_STAFF_ACTIONS

	if( nGold > 0 )
	{
		if( pUser->GetGold() >= nGold ) 
		{
			if( g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_PENYA, nGold ) )
			{
				pUser->AddGold( -nGold );

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
				CAegonAchievementMng::GetInstance()->AddAchievementProgressForType(pUser, AEGACH_GUILDPENYA, nGold);
#endif // __AEGON_ACHIEVEMENT_SYSTEM

				LogItemInfo aLogItem;
				aLogItem.Action = "W";
				aLogItem.SendName = pUser->GetName();
				aLogItem.RecvName = "GUILDBANK";
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = pUser->GetGold() + nGold;
				aLogItem.Gold2 = pUser->GetGold();
				//aLogItem.ItemName = "SEED";
				_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
				aLogItem.itemNumber = nGold;
				OnLogItem( aLogItem );
			}
		} 
		else
		{
			pUser->AddDefinedText( TID_GAME_GUILDNOTENGGOLD, "" );	// 인벤에 돈이부족
		}
	}
	else if( cbItemFlag )
	{
		for( int i=0; i< pUser->m_Inventory.GetMax(); ++i )
		{
			CItemElem* pItemElem = pUser->m_Inventory.GetAtId( i );
			if( IsUsableItem( pItemElem ) == FALSE )
				continue;

			if( pItemElem->GetProp()->dwItemKind3 != IK3_GEM )
				continue;

			int nValue = 0;

			if( pItemElem->m_nItemNum > 0 )
			{
				// 아이템 레벨에 따라서 공헌도를 다르게 한다.
				nValue = (pItemElem->GetProp()->dwItemLV + 1) / 2;	
				nValue *= pItemElem->m_nItemNum;					
			}

			if( nValue > 0 )
			{
				if( g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_PXPCOUNT, nValue ) )
				{

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
					CAegonAchievementMng::GetInstance()->AddAchievementProgressForType(pUser, AEGACH_GUILDEXP, nValue);
#endif // __AEGON_ACHIEVEMENT_SYSTEM

					LogItemInfo aLogItem;
					aLogItem.Action = "V";
					aLogItem.SendName = pUser->GetName();
					aLogItem.RecvName = "GUILDBANK";
					aLogItem.WorldId = pUser->GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
					OnLogItem( aLogItem, pItemElem, pItemElem->m_nItemNum );
					pUser->RemoveItem( (BYTE)i, pItemElem->m_nItemNum );
				}
			}
		}
	}
}

// 공지사항
void CDPSrvr::OnGuildNotice( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	char szNotice[MAX_BYTE_NOTICE];
	ar.ReadString( szNotice, MAX_BYTE_NOTICE );
	szNotice[MAX_BYTE_NOTICE-1] = '\0';

	if( strlen( szNotice ) == 0 )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

#ifdef __SECURITY_FIXES
	if (pUser->GetGuild() == NULL)
		return;
#endif // __SECURITY_FIXES

#ifdef _WIN64
	g_DPCoreClient.SendGuildStat(pUser, GUILD_STAT_NOTICE, (ULONG_PTR)szNotice);
#else
	g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_NOTICE, (DWORD)szNotice );
#endif
}

void CDPSrvr::OnDuelRequest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uidSrc, uidDst;
	ar >> uidSrc >> uidDst;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	CUser* pDstUser = g_UserMng.GetUserByPlayerID( uidDst );
	if( IsValidObj( pUser ) && IsValidObj( pDstUser ) )
	{
		if( 0 < pUser->m_idparty && pUser->m_idparty == pDstUser->m_idparty )
		{
			pUser->AddDefinedText( TID_PK_PARTY_LIMIT, "" );	// 같은파티원
			return;
		}

#ifdef __GAMEGUARD
		if (pUser->m_idPlayer == pDstUser->m_idPlayer)
			return;
#endif //__GAMEGUARD

#if __VER >= 8 // 8차 듀얼 061226 ma
		if( 0 < pUser->m_idparty ||  0 < pDstUser->m_idparty )
			return;
		if( pUser->m_vtInfo.GetOther() )	// 거래중 이면 듀얼 불가 
			return;
		if( pDstUser->m_vtInfo.GetOther() )	// 거래중 이면 듀얼 불가 
			return;
	
		int	nState = pUser->GetSummonState();
		if( nState != 0 )
			return;
		nState = pUser->GetSummonState();
		if( nState != 0 )
			return;

#endif // __VER >= 8 // 8차 듀얼 061226 ma

		if( pUser->IsPVPInspection( pDstUser, 1 ) )
		{
			if( pDstUser->IsMode( PVPCONFIRM_MODE ) )
			{
				pUser->AddDefinedText( TID_PK_MODE_REJECT, "" );	// PVP거절 모드입니다
			}
			else
			{
#ifdef __HACK_1130
				pUser->m_tmDuelRequest	= GetTickCount();
#endif	// __HACK_1130
				pDstUser->AddDuelRequest( uidSrc, uidDst );
			}
		}
	}
}

// 듀얼승락을 받음.  두캐릭터에게 시작하라고 보내줘야 함.
void CDPSrvr::OnDuelYes( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uidSrc, uidDst;
	ar >> uidSrc >> uidDst;

	CUser* pSrc = g_UserMng.GetUserByPlayerID( uidSrc );
#ifdef __HACK_1130
	CUser* pDst	=	g_UserMng.GetUser( dpidCache, dpidUser );
#else	// __HACK_1130
	CUser* pDst = g_UserMng.GetUserByPlayerID( uidDst );
#endif	// __HACK_1130

	if( IsValidObj(pSrc) && IsValidObj(pDst) )
	{
		if( 0 < pSrc->m_idparty && pSrc->m_idparty == pDst->m_idparty )
		{
			pSrc->AddDefinedText( TID_PK_PARTY_LIMIT, "" );	// 같은파티원
			pDst->AddDefinedText( TID_PK_PARTY_LIMIT, "" );	// 같은파티원
			return;
		}

#if __VER >= 8 // 8차 듀얼 061226 ma
		if( 0 < pSrc->m_idparty ||  0 < pDst->m_idparty )
		{
			return;
		}
		//개인상점 중에는 듀얼 불가 
		if( pSrc->m_vtInfo.VendorIsVendor() || pSrc->m_vtInfo.IsVendorOpen() ||
            pDst->m_vtInfo.VendorIsVendor() || pDst->m_vtInfo.IsVendorOpen() )
		{
			return;	//
		}
#endif // __VER >= 8 // 8차 듀얼 061226 ma


		if( pSrc->IsPVPInspection( pDst, 1 ) )
		{
#ifdef __HACK_1130
			if( pSrc->m_tmDuelRequest + SEC( 10 ) < GetTickCount() )	// 듀얼 신청 시간을 10초 초과하면
			{
				pSrc->m_tmDuelRequest	= 0;
				return;
			}
#endif	// __HACK_1130
			pSrc->m_nDuel = 1;
			pSrc->m_nDuelState = 104;
			pSrc->m_idDuelOther = pDst->GetId();
			pDst->m_nDuel = 1;
			pDst->m_nDuelState = 104;
			pDst->m_idDuelOther = pSrc->GetId();
			pSrc->AddDuelStart( uidDst );	// 서로 상대방에 대한 아이디만 보내주면 된다.
			pDst->AddDuelStart( uidSrc );
			pSrc->m_dwTickEndDuel = ::timeGetTime() + NEXT_TICK_ENDDUEL;
			pDst->m_dwTickEndDuel = ::timeGetTime() + NEXT_TICK_ENDDUEL;
			pSrc->SetPosChanged( TRUE );
			pDst->SetPosChanged( TRUE );
		}
	}
}

// pUser가 듀얼 신청을 거부했다. pSrc에게 알려야 한다.
void CDPSrvr::OnDuelNo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uidSrc; //, uidDst;
	ar >> uidSrc;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj(pUser) )
	{
		CUser* pSrc = g_UserMng.GetUserByPlayerID( uidSrc );
		if( IsValidObj(pSrc) )
			pSrc->AddDuelNo( pUser->GetId() );	// pSrc에게 pUser가 거부했다는걸 알림.
	}
}

// 파티듀얼 ----------------------------------------------------------------
// Src가 Dst에게 한판 붙자고 신청해왔다.
void CDPSrvr::OnDuelPartyRequest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uidSrc, uidDst;
	ar >> uidSrc >> uidDst;
	
	CUser* pSrcUser	= g_UserMng.GetUser( dpidCache, dpidUser );	// 신청자 유저.
	CUser* pDstUser	= g_UserMng.GetUserByPlayerID( uidDst );	// 상대 유저
	if( IsValidObj( pDstUser ) && IsValidObj( pSrcUser ) )
	{
		if( pDstUser->IsMode( PVPCONFIRM_MODE ) )
		{
			pSrcUser->AddDefinedText( TID_PK_MODE_REJECT, "" );	// PVP거절 모드입니다
		}
		else
		{

			if( pSrcUser->IsPVPInspection( pDstUser, 2 ) )
			{
				CParty* pSrcParty = g_PartyMng.GetParty( pSrcUser->m_idparty );		// 신청자의 파티꺼냄
				if( pSrcParty == NULL || pSrcParty->IsLeader( pSrcUser->m_idPlayer ) == FALSE )
				{
					pSrcUser->AddDefinedText( TID_PK_NO_IPARTYLEADER, "" );	// 파티장이 아닙니다
					return;
				}
				CParty *pDstParty = g_PartyMng.GetParty( pDstUser->m_idparty );		// 도전받는자의 파티꺼냄.
				if( pDstParty == NULL || pDstParty->IsLeader( pDstUser->m_idPlayer ) == FALSE )
				{
					pSrcUser->AddDefinedText( TID_PK_NO_UPARTYLEADER, "" );		// 상대방이 파티장이 아닙니다
					return;
				}
#ifdef __SECURITY_FIXES
				if (pSrcParty->m_uPartyId == pDstParty->m_uPartyId)
					return;

				if (pSrcUser->m_idPlayer == pDstUser->m_idPlayer)
					return;
#endif // __SECURITY_FIXES
				pDstUser->AddDuelPartyRequest( uidSrc, uidDst );
			}
		}

	}
}

// 파티듀얼승락을 받음.  모든 양측 파티원들에게 듀얼이 시작됨을 알림.
void CDPSrvr::OnDuelPartyYes( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uidSrc, uidDst;
	ar >> uidSrc >> uidDst;
	
	CUser* pSrc = g_UserMng.GetUserByPlayerID( uidSrc );	
	CUser* pDst = g_UserMng.GetUserByPlayerID( uidDst );
	if( IsValidObj(pSrc) && IsValidObj(pDst) )
	{

		if( pSrc->IsPVPInspection( pDst, 2 ) )
		{
			CParty* pSrcParty = g_PartyMng.GetParty( pSrc->m_idparty );		// 신청자의 파티꺼냄
			if( pSrcParty == NULL || pSrcParty->IsLeader( pSrc->m_idPlayer ) == FALSE )
			{
				Error( "CDPSrvr::OnDuelPartyYes : PLAYER NO LEADER SOURCE  %d %s", pSrc->m_idparty, pSrc->GetName() );
				return;
			}
			CParty *pDstParty = g_PartyMng.GetParty( pDst->m_idparty );		// 도전받는자의 파티꺼냄.
			//			if( pDstParty == NULL || pDstParty->IsMember( pDst->m_idPlayer ) == FALSE )
			if( pDstParty == NULL || pDstParty->IsLeader( pDst->m_idPlayer ) == FALSE )
			{
				Error( "CDPSrvr::OnDuelPartyYes : PLAYER NO LEADER DESTINATION %d %s", pDst->m_idparty, pDst->GetName() );
				return;
			}

#ifdef __SECURITY_FIXES
			if (pSrcParty->m_uPartyId == pDstParty->m_uPartyId)
				return;
#endif // __SECURITY_FIXES

			pSrc->m_dwTickEndDuel = ::timeGetTime() + NEXT_TICK_ENDDUEL;
			pDst->m_dwTickEndDuel = ::timeGetTime() + NEXT_TICK_ENDDUEL;
			pSrc->SetPosChanged( TRUE );	// UpdateRegionAttr
			pDst->SetPosChanged( TRUE );

			g_DPCoreClient.SendSetPartyDuel( pSrcParty->m_uPartyId, pDstParty->m_uPartyId, TRUE );

			pSrcParty->DoDuelPartyStart( pDstParty );		// 상대파티와 결투가 시작됐다는걸 세팅.
			pDstParty->DoDuelPartyStart( pSrcParty );		// 상대파티와 결투가 시작됐다는걸 세팅.
		}
	}
}

// pUser가 듀얼 신청을 거부했다. pSrc에게 알려야 한다.
void CDPSrvr::OnDuelPartyNo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uidSrc; //, uidDst;
	ar >> uidSrc;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj(pUser) )
	{
		CUser* pSrc = g_UserMng.GetUserByPlayerID( uidSrc );
		if( IsValidObj(pSrc) )
			pSrc->AddDuelPartyNo( pUser->GetId() );		// pSrc에게 pUser가 거부했다는걸 알림.
	}
}

void CDPSrvr::OnMoverFocus( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uidPlayer;
	ar >> uidPlayer;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj(pUser) )
	{
		CUser* pFocus = g_UserMng.GetUserByPlayerID( uidPlayer );
		if( IsValidObj(pFocus) )
			pUser->AddMoverFocus( pFocus );
	}
}

void CDPSrvr::OnSkillTaskBar( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int nCount;
	ar >> nCount;

#ifdef __ACTION_SLOT_PAGES
	if (nCount > MAX_SLOT_QUEUE * MAX_ACTION_SLOT)
		return;
#else // __ACTION_SLOT_PAGES
	if( nCount > MAX_SLOT_QUEUE )
		return;
#endif // __ACTION_SLOT_PAGES

#ifdef __SECURITY_FIXES
	if (nCount < 0)
		return;
#endif // __SECURITY_FIXES

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __ACTION_SLOT_PAGES
		for (int i = 0; i < nCount; i++)
		{
			BYTE nIndex;
			ar >> nIndex;

			BYTE nIndex2;
			ar >> nIndex2;

			if (nIndex >= MAX_ACTION_SLOT)
				return;

			if (nIndex2 >= MAX_SLOT_QUEUE)
				return;

			ar >> pUser->m_playTaskBar.m_aSlotQueue[nIndex][nIndex2].m_dwShortcut >> pUser->m_playTaskBar.m_aSlotQueue[nIndex][nIndex2].m_dwId >> pUser->m_playTaskBar.m_aSlotQueue[nIndex][nIndex2].m_dwType;
			ar >> pUser->m_playTaskBar.m_aSlotQueue[nIndex][nIndex2].m_dwIndex >> pUser->m_playTaskBar.m_aSlotQueue[nIndex][nIndex2].m_dwUserId >> pUser->m_playTaskBar.m_aSlotQueue[nIndex][nIndex2].m_dwData;
		}
#else //__ACTION_SLOT_PAGES
		for( int i = 0 ; i < nCount ; i++)
		{
			BYTE nIndex;
			ar >> nIndex;

			if( nIndex >= MAX_SLOT_QUEUE )
				return;

			ar >> pUser->m_playTaskBar.m_aSlotQueue[nIndex].m_dwShortcut >> pUser->m_playTaskBar.m_aSlotQueue[nIndex].m_dwId >> pUser->m_playTaskBar.m_aSlotQueue[nIndex].m_dwType;
			ar >> pUser->m_playTaskBar.m_aSlotQueue[nIndex].m_dwIndex >> pUser->m_playTaskBar.m_aSlotQueue[nIndex].m_dwUserId >> pUser->m_playTaskBar.m_aSlotQueue[nIndex].m_dwData;
		}
#endif //__ACTION_SLOT_PAGES
	}
}
void CDPSrvr::OnAddAppletTaskBar( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE nIndex;
	ar >> nIndex;

	if( nIndex >= MAX_SLOT_APPLET )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		ar >> pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwShortcut >> pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwId >> pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwType;
		ar >> pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwIndex >> pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwUserId >> pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwData;
		if( pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwShortcut == SHORTCUT_CHAT)
		{
			ar.ReadString( pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_szString, MAX_SHORTCUT_STRING );
			pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_szString[MAX_SHORTCUT_STRING-1] = '\0';
		}
	}
}
void CDPSrvr::OnRemoveAppletTaskBar( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE nIndex;
	ar >> nIndex;

	if( nIndex >= MAX_SLOT_APPLET )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwShortcut == SHORTCUT_CHAT)
		{
			memset( pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_szString, 0, sizeof( pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_szString ) );
		}
		pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwShortcut = SHORTCUT_NONE;
	}
}
void CDPSrvr::OnAddItemTaskBar( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE nSlotIndex, nIndex;
	ar >> nSlotIndex >> nIndex;

	if( nSlotIndex >= MAX_SLOT_ITEM_COUNT || nIndex >= MAX_SLOT_ITEM )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#if __VER >= 11 // __CSC_VER11_5
		DWORD dwShortCut;
		ar >> dwShortCut;

		// Chat Shortcut 10개로 제한
		if(dwShortCut == SHORTCUT_CHAT)
		{
			int nchatshortcut = 0;
			for( int i=0; i<MAX_SLOT_ITEM_COUNT; i++ )
			{
				for( int j=0; j<MAX_SLOT_ITEM; j++ )
				{
					if( pUser->m_playTaskBar.m_aSlotItem[i][j].m_dwShortcut == SHORTCUT_CHAT )
						nchatshortcut++;
				}
			}

			if(nchatshortcut > 9)
			{
				pUser->AddDefinedText( TID_GAME_MAX_SHORTCUT_CHAT );
				return;
			}
		}

		pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwShortcut = dwShortCut;
	 	ar >> pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwId >> pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwType;
#else //__CSC_VER11_5
		ar >> pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwShortcut >> pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwId >> pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwType;
#endif //__CSC_VER11_5
		ar >> pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwIndex >> pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwUserId >> pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwData;
		if( pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwShortcut == SHORTCUT_CHAT)
		{
			ar.ReadString( pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_szString, MAX_SHORTCUT_STRING );
			pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_szString[MAX_SHORTCUT_STRING-1] = '\0';
		}
	}
}
void CDPSrvr::OnRemoveItemTaskBar( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE nSlotIndex, nIndex;
	ar >> nSlotIndex >> nIndex;

	if( nSlotIndex >= MAX_SLOT_ITEM_COUNT || nIndex >= MAX_SLOT_ITEM )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwShortcut == SHORTCUT_CHAT)
		{
			memset( pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_szString, 0, sizeof( pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_szString ) );
		}
		pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwShortcut = SHORTCUT_NONE;
	}
}

void CDPSrvr::OnPlayerMoved( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsInvalidObj( pUser ) )
		return;

	if( pUser->GetIndex() == 0 )
	{
		WriteError( "PACKETTYPE_PLAYERMOVED" );
		return;
	}

	if( pUser->m_pActMover->IsDie() )
		return;

	D3DXVECTOR3 v, vd;
	float f;
	DWORD dwState, dwStateFlag, dwMotion	= 0;
	int nMotionEx	= 0, nLoop;
	DWORD dwMotionOption;
	__int64 nTickCount;

	ar >> v >> vd >> f;
	ar >> dwState >> dwStateFlag;
	ar >> dwMotion >> nMotionEx;
	ar >> nLoop >> dwMotionOption;
	ar >> nTickCount;

	if( pUser->m_pActMover->IsFly() )	return;		// 비행상태인데 일로 들어왔다면 취소시키자.

	D3DXVECTOR3 vDistance	= pUser->GetPos() - v;
	if( D3DXVec3LengthSq( &vDistance ) > 1000000.0F )
	{
//		Error( "PACKETTYPE_PLAYERMOVED" );
		return;
	}

	int delay	= (int)( (float)g_TickCount.GetOffset( nTickCount ) / 66.6667f );

	if( delay >= MAX_CORR_SIZE_45 ) {
		pUser->ActionForceSet( v, vd, f, dwState, dwStateFlag, dwMotion, nMotionEx, nLoop, dwMotionOption );
	}
	else 	
	{
		if( pUser->m_pActMover->IsStateFlag( OBJSTAF_FLY ) || dwStateFlag & OBJSTAF_FLY ) 
		{
			pUser->ActionForceSet( v, vd, f, dwState, dwStateFlag, dwMotion, nMotionEx, nLoop, dwMotionOption );
		}
		else 
		{
			pUser->m_pActMover->DefaultSet();

			int nRemnant	= (int)( MAX_CORR_SIZE_45 - delay );
			D3DXVECTOR3 d	= v - pUser->GetPos();
			float fLength	= D3DXVec3Length( &d );
			float fSpeed	= ( fLength / (float)nRemnant ) / 4 + 0.020f;
			
			pUser->m_fCrrSpd	= pUser->m_pActMover->IsStateFlag( OBJSTAF_WALK )? fSpeed*4.0f: fSpeed;

			pUser->m_CorrAction.v	= v;
			pUser->m_CorrAction.vd		= vd;
			pUser->m_CorrAction.f	= f;
			pUser->m_CorrAction.dwState	= dwState;
			pUser->m_CorrAction.dwStateFlag	= dwStateFlag;
			pUser->m_CorrAction.dwMotion	= dwMotion;
			pUser->m_CorrAction.nMotionEx	= nMotionEx;
			pUser->m_CorrAction.nLoop	= nLoop;
			pUser->m_CorrAction.dwMotionOption	= dwMotionOption;
			pUser->m_CorrAction.fValid	= TRUE;

			pUser->SetDestPos( v, ( dwState & OBJSTA_MOVE_ALL ) != OBJSTA_BMOVE );
		}
	}

#ifdef __AFK_MONITORING
	if (pUser->m_dwAFKState == 3)
	{
		pUser->m_dwAFKState = NULL;
		g_UserMng.AddMessiState(pUser);
	}
	pUser->m_dwLastWalkTick[0] = GetTickCount() + MIN(MIN_AUTO_AFK);
#endif //__AFK_MONITORING

	g_UserMng.AddMoverMoved
		( pUser, v, vd, f, dwState, dwStateFlag, dwMotion,  nMotionEx, nLoop, dwMotionOption, nTickCount );
}

void CDPSrvr::OnPlayerBehavior( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsInvalidObj( pUser ) )
		return;

	if( pUser->GetIndex() == 0 )
	{
		WriteError( "PACKETTYPE_PLAYERBEHAVIOR" );
		return;
	}

	if( pUser->m_pActMover->IsDie() )
			return;
	
	if( pUser->m_pActMover->IsState( OBJSTA_ATK_CASTING1 | OBJSTA_ATK_CASTING2 | OBJSTA_ATK_MAGIC1 | OBJSTA_ATK_RANGE1 | OBJSTA_ATK_MELEESKILL | OBJSTA_ATK_RANGESKILL ) )
		return;

	D3DXVECTOR3 v, vd;
	float f;
	DWORD dwState, dwStateFlag, dwMotion	= 0;
	int nMotionEx	= 0, nLoop;
	DWORD dwMotionOption;
	__int64 nTickCount;

	ar >> v >> vd >> f;
	ar >> dwState >> dwStateFlag;
	ar >> dwMotion >> nMotionEx;

	ar >> nLoop >> dwMotionOption;
	ar >> nTickCount;

	if( pUser->m_pActMover->IsFly() )
		return;

#ifdef __AEGON_FIXES
	//playerbehaviour teleport hack
	if (!pUser->IsAuthHigher(AUTH_GAMEMASTER)) 
	{
		D3DXVECTOR3 vPlayerPosDiff = pUser->GetPos() - v;
		if (abs(vPlayerPosDiff.x) > 10.0 || abs(vPlayerPosDiff.y) > 10.0 || abs(vPlayerPosDiff.z) > 10.0)
		{
			//v = pUser->GetPos();
			//pUser->AddGetPos(pUser->GetId(), v, f);
			//log to find possible false positives
			//Error("PlayerBehaviour possible teleport attempt player: %s - id: %lu - diff: %f - %f - %f", pUser->GetName(), pUser->m_idPlayer, vPlayerPosDiff.x, vPlayerPosDiff.y, vPlayerPosDiff.z);
		}
	}
#endif
	
#ifndef __GAMEGUARD
	int delay = (int)((float)g_TickCount.GetOffset(nTickCount) / 66.6667f);
#endif // __GAMEGUARD

#ifdef __AFK_MONITORING
	if (pUser->m_dwAFKState == 3)
	{
		pUser->m_dwAFKState = 0;
		g_UserMng.AddMessiState(pUser);
	}
	pUser->m_dwLastWalkTick[0] = GetTickCount() + MIN(MIN_AUTO_AFK);
#endif //__AFK_MONITORING

	if( !pUser->IsEmptyDest() && pUser->m_CorrAction.fValid == FALSE ) 
		pUser->ActionForceSet( v, vd, f, dwState, dwStateFlag, dwMotion, nMotionEx, nLoop, dwMotionOption );
	else 
		pUser->ActionForceSet( v, vd, f, dwState, dwStateFlag, dwMotion, nMotionEx, nLoop, dwMotionOption );

#ifdef __AEGON_PREM_MOTIONS
	if (dwMotion >= MTI_DANCE01) 
	{
		MotionProp* propMotion = prj.GetMotionPropByAniID(dwMotion);
		if (propMotion)
		{
			BOOL bPremiumUser = pUser->IsPremium();

			if (propMotion->dwUnlockItemID > 0 && !bPremiumUser && !easy_find(pUser->m_vUnlockedMotions, propMotion->dwUnlockItemID))
			{
				dwMotion = MTI_STAND;
				nMotionEx = MTI_STAND;
			}
		}
	}
#endif // __AEGON_PREM_MOTIONS

	g_UserMng.AddMoverBehavior( pUser, v, vd, f, dwState, dwStateFlag, dwMotion,  nMotionEx, nLoop, dwMotionOption, nTickCount );
}

void CDPSrvr::OnPlayerMoved2( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		if( pUser->GetIndex() == 0 )
		{
			WriteError( "PACKETTYPE_PLAYERMOVED2" );
			return;
		}

		if( pUser->m_pActMover->IsDie() )
			return;

		D3DXVECTOR3 v, vd;
		float f, fAngleX, fAccPower, fTurnAngle;
		DWORD dwState, dwStateFlag, dwMotion	= 0;
		int nMotionEx	= 0, nLoop;
		DWORD dwMotionOption;
		__int64 nTickCount;
		BYTE nFrame;

		ar >> v >> vd >> f >> fAngleX >> fAccPower >> fTurnAngle;
		ar >> dwState >> dwStateFlag;
		ar >> dwMotion >> nMotionEx;

		ar >> nLoop >> dwMotionOption;
		ar >> nTickCount;
		ar >> nFrame;
		int delay	= (int)( (float)g_TickCount.GetOffset( nTickCount ) / 66.6667f );

#ifdef __AFK_MONITORING
		if (pUser->m_dwAFKState == 3)
		{
			pUser->m_dwAFKState = NULL;
			g_UserMng.AddMessiState(pUser);
		}
		pUser->m_dwLastWalkTick[0] = GetTickCount() + MIN(MIN_AUTO_AFK);
#endif //__AFK_MONITORING

		if( pUser->m_pActMover->IsFly() )
		{
			if( delay >= nFrame ) {
				pUser->ActionForceSet2( v, vd, f, fAngleX, fAccPower, fTurnAngle, dwState, dwStateFlag, dwMotion, nMotionEx, nLoop, dwMotionOption );
			}
			else {
				pUser->m_pActMover->DefaultSet();

				pUser->m_CorrAction.v	= v;
				pUser->m_CorrAction.vd		= vd;
				pUser->m_CorrAction.f	= f;
				pUser->m_CorrAction.dwState	= dwState;
				pUser->m_CorrAction.dwStateFlag	= dwStateFlag;
				pUser->m_CorrAction.dwMotion	= dwMotion;
				pUser->m_CorrAction.nMotionEx	= nMotionEx;
				pUser->m_CorrAction.nLoop	= nLoop;
				pUser->m_CorrAction.dwMotionOption	= dwMotionOption;
				pUser->m_CorrAction.fAngleX	= fAngleX;
				pUser->m_CorrAction.fAccPower	= fAccPower;
				pUser->m_CorrAction.fTurnAngle	= fTurnAngle;
				pUser->m_CorrAction.fValid	= TRUE;

				int nRemnant	= (int)nFrame - delay;
				pUser->m_uRemnantCorrFrm	= nRemnant;
				pUser->SetDestPos( v );
			}

			g_UserMng.AddMoverMoved2
				( pUser, v, vd, f, fAngleX, fAccPower, fTurnAngle, dwState, dwStateFlag, dwMotion,  nMotionEx, nLoop, dwMotionOption, nTickCount, nFrame );
		} 
	}
}

void CDPSrvr::OnPlayerBehavior2( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->GetIndex() == 0 )
		{
			WriteError( "PACKETTYPE_PLAYERBEHAVIOR2" );
			return;
		}

		if( pUser->m_pActMover->IsDie() )
			return;

		D3DXVECTOR3 v, vd;
		float f, fAngleX, fAccPower, fTurnAngle;
		DWORD dwState, dwStateFlag, dwMotion;
		int nMotionEx, nLoop;
		DWORD dwMotionOption;
		__int64 nTickCount;

		ar >> v >> vd >> f >> fAngleX >> fAccPower >> fTurnAngle;
		ar >> dwState >> dwStateFlag;
		ar >> dwMotion >> nMotionEx;
		ar >> nLoop >> dwMotionOption;
		ar >> nTickCount;



#ifdef __AEGON_FIXES
		//playerbehaviour2 teleport hack
		if (!pUser->IsAuthHigher(AUTH_GAMEMASTER))
		{
			D3DXVECTOR3 vPlayerPosDiff = pUser->GetPos() - v;

			if (abs(vPlayerPosDiff.x) > 10.0 || abs(vPlayerPosDiff.y) > 10.0 || abs(vPlayerPosDiff.z) > 10.0)
			{
				//v = pUser->GetPos();
				//pUser->AddGetPos(pUser->GetId(), v, f);
				//log to find possible false positives
				//Error("PlayerBehaviour2 possible teleport attempt player: %s - id: %lu - diff: %f - %f - %f", pUser->GetName(), pUser->m_idPlayer, vPlayerPosDiff.x, vPlayerPosDiff.y, vPlayerPosDiff.z);
			}
	}
#endif

#ifdef _DEBUG
		if( dwStateFlag & OBJSTAF_ACC )	
		{
			int a = 0;
		}
#endif

		if( pUser->m_pActMover->IsFly() == FALSE )	return;	// 비행상태가 아닌데 이리로 들어왔다면 취소.

#ifdef __AFK_MONITORING
		if (pUser->m_dwAFKState == 3)
		{
			pUser->m_dwAFKState = NULL;
			g_UserMng.AddMessiState(pUser);
		}
		pUser->m_dwLastWalkTick[0] = GetTickCount() + MIN(MIN_AUTO_AFK);
#endif //__AFK_MONITORING

		// 1
		// 2
		// 3
		pUser->ActionForceSet2( v, vd, f, fAngleX, fAccPower, fTurnAngle, dwState, dwStateFlag, dwMotion, nMotionEx, nLoop, dwMotionOption );
		// 4
		g_UserMng.AddMoverBehavior2
			( pUser, v, vd, f, fAngleX, fAccPower, fTurnAngle, dwState, dwStateFlag, dwMotion,  nMotionEx, nLoop, dwMotionOption, nTickCount );
	}
}

void CDPSrvr::OnPlayerAngle( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) )
	{
		if( pUser->GetIndex() == 0 )
		{
			WriteError( "PACKETTYPE_PLAYERANGLE" );
			return;
		}

		D3DXVECTOR3 v, vd;
		float f, fAngleX, fAccPower, fTurnAngle;
		__int64 nTickCount;

		ar >> v >> vd >> f >> fAngleX >> fAccPower >> fTurnAngle;
		ar >> nTickCount;

		int delay	= (int)( (float)g_TickCount.GetOffset( nTickCount ) / 66.6667f );

		if( pUser->m_pActMover->IsFly() )
		{
			if( delay >= MAX_CORR_SIZE_150 )
			{
				if( pUser->m_CorrAction.fValid &&  !pUser->m_CorrAction.fHalf )
				{
						pUser->ActionForceSet2( v, vd, f, fAngleX, fAccPower, fTurnAngle,
						pUser->m_CorrAction.dwState, pUser->m_CorrAction.dwStateFlag, pUser->m_CorrAction.dwMotion, pUser->m_CorrAction.nMotionEx,
						pUser->m_CorrAction.nLoop, pUser->m_CorrAction.dwMotionOption );
				}
				else
				{
					pUser->HalfForceSet( v, vd, f, fAngleX, fAccPower, fTurnAngle );
				}
			}
			else
			{
				pUser->m_pActMover->DefaultSet();

				pUser->m_CorrAction.v	= v;
				pUser->m_CorrAction.vd		= vd;
				pUser->m_CorrAction.f	= f;
				pUser->m_CorrAction.fAngleX	= fAngleX;
				pUser->m_CorrAction.fAccPower	= fAccPower;
				pUser->m_CorrAction.fTurnAngle	= fTurnAngle;
				pUser->m_CorrAction.fHalf	= !pUser->m_CorrAction.fValid;
				pUser->m_CorrAction.fValid	= TRUE;

				int nRemnant	= MAX_CORR_SIZE_150 - delay;
				pUser->m_uRemnantCorrFrm	= nRemnant;
				pUser->SetDestPos( v );
			}
			g_UserMng.AddMoverAngle( pUser, v, vd, f, fAngleX, fAccPower, fTurnAngle, nTickCount );
		}
	}
}

void CDPSrvr::OnPlayerSetDestObj( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		OBJID objid;
		float fRange;
		ar >> objid >> fRange;

		CCtrl* pCtrl;

		pCtrl	= prj.GetCtrl( objid );
		if( IsValidObj( pCtrl ) )
		{
#ifdef __TRAFIC_1222
			if( pUser->GetDestId() == objid )
				return;
#endif	// __TRAFIC_1222
			pUser->m_pActMover->DefaultSet();
			pUser->BehaviorActionForceSet();
			pUser->SetDestObj( objid, fRange );

			g_UserMng.AddMoverSetDestObj( (CMover*)pUser, objid, fRange );
		}

#ifdef __AFK_MONITORING
		if (pUser->m_dwAFKState == 3)
		{
			pUser->m_dwAFKState = NULL;
			g_UserMng.AddMessiState(pUser);
		}
		pUser->m_dwLastWalkTick[0] = GetTickCount() + MIN(MIN_AUTO_AFK);
#endif //__AFK_MONITORING
	}
}

void CDPSrvr::OnDoUseItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwData;
	OBJID objid;
	int	  nPart;

	ar >> dwData >> objid >> nPart;

#ifdef __GAMEGUARD
	if (nPart >= MAX_HUMAN_PARTS || nPart < -1 || dwData < 0)
#else // __GAMEGUARD
	if (nPart >= MAX_HUMAN_PARTS)
#endif // __GAMEGUARD	
	{
		return;
	}

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	WORD nId = HIWORD( dwData );
	if( nPart > 0 )
	{
		CItemElem* pItemElem	= pUser->m_Inventory.GetAtId( nId );
		if( IsUsableItem( pItemElem ) == FALSE )
			return;
		if( pUser->m_Inventory.IsEquip( nId ) )
		{
			if( pItemElem != pUser->m_Inventory.GetEquip( nPart ) )
				return;
		}
	}
#ifdef __HACK_1023
	CItemElem* pItemElem	= pUser->m_Inventory.GetAtId( nId );
	if( IsUsableItem( pItemElem ) )
	{
		ItemProp* pItemProp	= pItemElem->GetProp();
		if( pItemProp && pItemProp->dwParts == PARTS_RIDE )
		{
			if( !pUser->m_Inventory.IsEquip( nId ) )
			{
				FLOAT fVal;
				ar >> fVal;
				if( fVal != pItemProp->fFlightSpeed )
				{
					pUser->AddDefinedText( TID_GAME_MODIFY_FLIGHT_SPEED );
					return;
				}
			}
		}
	}
#endif	// __HACK_1023

#ifdef __WEAPON_RARITY
	if (pItemElem && !prj.CheckWeaponRarity(pItemElem->GetWeaponRarity()))
	{
		Error("%s : pItemElem->GetWeaponRarity() returns %d!", pUser->GetName(), pItemElem->GetWeaponRarity());

		int nRarity = pItemElem->GetWeaponRarity() > prj.m_nHighWeaponRarityLevel ? prj.m_nHighWeaponRarityLevel : 0;
		pItemElem->SetWeaponRarity(nRarity);
		pUser->AddWeaponRarity(pItemElem->m_dwObjId, nRarity);
		return;
	}
#endif // __WEAPON_RARITY

	pUser->OnDoUseItem( dwData, objid, nPart );

#ifdef __GAMEGUARD
	if (pUser->GetWorld())
		g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
#endif // __GAMEGUARD
}

void CDPSrvr::OnPlayerCorr( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->GetIndex() == 0 )
		{
			WriteError( "PACKETTYPE_PLAYERCORR" );
			return;
		}

		D3DXVECTOR3 v, vd;
		float f;
		DWORD dwState, dwStateFlag, dwMotion;
		int nMotionEx, nLoop;
		DWORD dwMotionOption;
		__int64 nTickCount;

		ar >> v >> vd >> f;
		ar >> dwState >> dwStateFlag;
		ar >> dwMotion >> nMotionEx;
		ar >> nLoop >> dwMotionOption;
		ar >> nTickCount;

		if( pUser->m_pActMover->IsFly() == FALSE )
		{
			D3DXVECTOR3 vDistance	= pUser->GetPos() - v;
			if( D3DXVec3LengthSq( &vDistance ) > 1000000.0F )
			{
//				Error( "PACKETTYPE_PLAYERCORR" );
				return;
			}
			// 1
			// 2
			// 3
			pUser->ClearDest();
			pUser->ClearDestAngle();
			memset( &pUser->m_CorrAction, 0, sizeof(CORR_ACTION) );

			pUser->ActionForceSet( v, vd, f, dwState, dwStateFlag, dwMotion, nMotionEx, nLoop, dwMotionOption );

			// 4
			g_UserMng.AddMoverCorr
				( pUser, v, vd, f, dwState, dwStateFlag, dwMotion,  nMotionEx, nLoop, dwMotionOption, nTickCount );
		}
	}
}

void CDPSrvr::OnPlayerCorr2( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->GetIndex() == 0 )
		{
			WriteError( "PACKETTYPE_PLAYERCORR2" );
			return;
		}

		D3DXVECTOR3 v, vd;
		float f, fAngleX, fAccPower, fTurnAngle;
		DWORD dwState, dwStateFlag, dwMotion;
		int nMotionEx, nLoop;
		DWORD dwMotionOption;
		__int64 nTickCount;

		ar >> v >> vd >> f >> fAngleX >> fAccPower >> fTurnAngle;
		ar >> dwState >> dwStateFlag;
		ar >> dwMotion >> nMotionEx;

		ar >> nLoop >> dwMotionOption;
		ar >> nTickCount;

		if( pUser->m_pActMover->IsFly() )
		{
			// 1
			// 2
			// 3
			pUser->ClearDest();
			pUser->ClearDestAngle();
			memset( &pUser->m_CorrAction, 0, sizeof(CORR_ACTION) );

			pUser->ActionForceSet2( v, vd, f, fAngleX, fAccPower, fTurnAngle, dwState, dwStateFlag, dwMotion, nMotionEx, nLoop, dwMotionOption );

			// 4
			g_UserMng.AddMoverCorr2
				( pUser, v, vd, f, fAngleX, fAccPower, fTurnAngle, dwState, dwStateFlag, dwMotion,  nMotionEx, nLoop, dwMotionOption, nTickCount );
		}
	}
}

void CDPSrvr::OnOpenShopWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		OBJID objid;
		ar >> objid;
		
		CMover* pVendor	= prj.GetMover( objid );
		if( IsValidObj( pVendor ) && pUser->m_vtInfo.GetOther() == NULL )
		{
			if( pVendor->IsNPC() == FALSE )		// 대상이 NPC가 아니면?
				return;

#if __VER >= 8 // __S8_PK
			if( pUser->IsChaotic() )
			{
				CHAO_PROPENSITY Propensity = prj.GetPropensityPenalty( pUser->GetPKPropensity() );
				if( !Propensity.nShop )
					return;
			}
#endif // __VER >= 8 // __S8_PK

			if( pVendor->IsVendorNPC() == FALSE )
			{
				WriteError( "VENDOR//%s", pVendor->GetName() );
				return;
			}

			if( pUser->m_bBank )
			{
				pUser->AddDefinedText( TID_GAME_TRADELIMITNPC, "" );
				return;
			}

			if( pUser->m_vtInfo.VendorIsVendor() )
				return;

#ifdef __S_SERVER_UNIFY
			if( pUser->m_bAllAction == FALSE )
				return;
#endif // __S_SERVER_UNIFY

			pUser->m_vtInfo.SetOther( pVendor );
			pUser->AddOpenShopWnd( pVendor );
		}
	}
}

void CDPSrvr::OnCloseShopWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		CMover* pMover = pUser->m_vtInfo.GetOther();
		if( IsValidObj( pMover ) && pMover->IsNPC() )
			pUser->m_vtInfo.SetOther( NULL );
	}
}

void CDPSrvr::OnBuyItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __GAMEGUARD
	BYTE cTab;
#else // __GAMEGUARD
	CHAR cTab;
#endif // __GAMEGUARD
	BYTE nId;
	short nNum;
	DWORD dwItemId;

	ar >> cTab >> nId >> nNum >> dwItemId;

#ifdef __GAMEGUARD
	if (cTab > 3 || cTab < 0 || nId < 0 || nId > MAX_VENDOR_INVENTORY + 1 || nNum < 1 || nNum > 9999 || dwItemId <= 0)
#else // __GAMEGUARD
	if (cTab >= MAX_VENDOR_INVENTORY_TAB || nNum < 1)
#endif // __GAMEGUARD
	{
		return;
	}

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) && pUser->m_vtInfo.GetOther() )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU
		CMover* pVendor = pUser->m_vtInfo.GetOther();
#ifdef __SECURITY_FIXES
		if (!pVendor)
			return;
#endif // __SECURITY_FIXES

#if __VER >= 11 // __GUILDCOMBATCHIP
		LPCHARACTER lpChar = prj.GetCharacter( pVendor->m_szCharacterKey );
		if( lpChar && lpChar->m_nVenderType != 0 )	// 0 - 페냐 상인
			return;
#endif // __GUILDCOMBATCHIP

		if( pVendor->IsNPC() == FALSE )		// 판매할 대상이 NPC가 아니면?
			return;

#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE

		if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_TRADE, pUser->GetWorld(), pUser->GetPos() ) )
			return;

		CItemElem* pItemElem = pVendor->m_ShopInventory[cTab]->GetAtId( nId );
		if( NULL == pItemElem )
			return;

		if( dwItemId != pItemElem->m_dwItemId )
			return;

#ifndef __NPCVEND_STACKBUY
		if (nNum > pItemElem->m_nItemNum)
			nNum = pItemElem->m_nItemNum;
#endif //__NPCVEND_STACKBUY

#ifdef __OCCUPATION_SHOPITEM
		if( CTax::GetInstance()->IsOccupationShopItem( dwItemId ) && !CTax::GetInstance()->IsOccupationGuildMember( pUser ) )
		{
			pUser->AddDefinedText( TID_GAME_SECRETROOM_STORE_BUY );
			return;
		}
#endif // __OCCUPATION_SHOPITEM
		
		int nCost = (int)pItemElem->GetCost();
		nCost = (int)( prj.m_fShopCost * nCost );
#ifdef __SHOP_COST_RATE
		nCost = (int)( prj.m_EventLua.GetShopBuyFactor() * nCost );
#endif // __SHOP_COST_RATE
		
#if __VER < 8 // __S8_PK
		// 직위에 따른 상점가격 변동
		KarmaProp* pProp = prj.GetKarmaProp( pUser->m_nSlaughter );
		if( pProp )
		{
			if( pProp->nGrade == -6 )
				return;

			if( pProp->fDiscountRate != 0.0f )
			{
				nCost = (int)( nCost * pProp->fDiscountRate );
			}
		}
#endif // __VER < 8 // __S8_PK

#if __VER >= 11 // __MA_VER11_02
		if( pItemElem->m_dwItemId == II_SYS_SYS_SCR_PERIN )
			nCost = PERIN_VALUE;
#endif //__MA_VER11_02
		if( nCost < 1 )
			nCost = 1;
		
#ifdef __GAMEGUARD
		long long lEntireCost = nNum * nCost;
		if (lEntireCost > INT_MAX)
			return;
#endif // __GAMEGUARD

		int nPracticable = pUser->GetGold() / nCost;
		if( nNum > nPracticable )
			nNum = (short)nPracticable;

		if( nNum <= 0 )
		{
			pUser->AddDefinedText( TID_GAME_LACKMONEY, "" );
			return;
		}

#if defined(__NPCVEND_STACKBUY) && defined(__PERIN_BUY_BUG)
		if (pUser->m_dwLastBuyItemTick + 500 > GetTickCount()) // 아이템 구입시도 후 0.5초이내에 다시 구입시도한 경우
		{
			Error("CDPSrvr::OnBuyItem : __PERIN_BUY_BUG -> [PlayerId:%07d(%s)], [LastTick:%d], [CurTick:%d], [LastTryItem:%d], [Packet:%d,%d,%d,%d]",
				pUser->m_idPlayer, pUser->GetName(), pUser->m_dwLastBuyItemTick, GetTickCount(), pUser->m_dwLastTryBuyItem, cTab, nId, nNum, dwItemId);
			return;
		}
#endif //__NPCVEND_STACKBUY

#if __VER >= 12 // __TAX
		int nTax = 0;
		if( CTax::GetInstance()->IsApplyTaxRate( pUser, pItemElem ) )
			nTax = (int)( nCost * CTax::GetInstance()->GetPurchaseTaxRate( pUser ) );
		nCost += nTax;

#ifdef __NPCVEND_STACKBUY
		ItemProp* pProp = pItemElem->GetProp();
		int nRemain = 0;

	tp_redo:
		if (nNum > pProp->dwPackMax || nRemain > pProp->dwPackMax)
		{
			if (nRemain > 0)
			{
				nRemain -= pProp->dwPackMax;
			}
			else
			{
				nRemain = nNum - pProp->dwPackMax;
			}
			nNum = pProp->dwPackMax;
		}
		else {
			nRemain = 0;
		}
#endif //__NPCVEND_STACKBUY

		nTax *= nNum;
#endif // __TAX
		int nGold = nCost * nNum;
		if( nGold <= 0 )
			return;

		if( pUser->GetGold() >= nGold )
		{
#ifdef __PERIN_BUY_BUG
#ifndef __NPCVEND_STACKBUY
			if (pUser->m_dwLastBuyItemTick + 500 > GetTickCount()) // 아이템 구입시도 후 0.5초이내에 다시 구입시도한 경우
			{
				Error("CDPSrvr::OnBuyItem : __PERIN_BUY_BUG -> [PlayerId:%07d(%s)], [LastTick:%d], [CurTick:%d], [LastTryItem:%d], [Packet:%d,%d,%d,%d]",
					pUser->m_idPlayer, pUser->GetName(), pUser->m_dwLastBuyItemTick, GetTickCount(), pUser->m_dwLastTryBuyItem, cTab, nId, nNum, dwItemId);
				//g_DPSrvr.QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer );
				return;
			}
#endif //__NPCVEND_STACKBUY
			pUser->m_dwLastTryBuyItem = pItemElem->m_dwItemId;
			pUser->m_dwLastBuyItemTick = GetTickCount();
#endif // __PERIN_BUY_BUG
	
			CItemElem itemElem;
			itemElem	= *pItemElem;
			itemElem.m_nItemNum	      = nNum;
			itemElem.SetSerialNumber();

			if( pUser->CreateItem( &itemElem ) )
			{
				LogItemInfo aLogItem;
				aLogItem.Action = "B";
				aLogItem.SendName = pUser->GetName();
				aLogItem.RecvName = pVendor->GetName();
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = pUser->GetGold();
				aLogItem.Gold2 = pUser->GetGold() - nGold;
				aLogItem.Gold_1 = pVendor->GetGold();

				pItemElem->SetSerialNumber( itemElem.GetSerialNumber() );
				OnLogItem( aLogItem, pItemElem, nNum );		// why do not pass &itemElem as argument?
				pItemElem->SetSerialNumber( 0 );
				pUser->AddGold( -nGold );	
#if __VER >= 12 // __TAX
				if( nTax )
					CTax::GetInstance()->AddTax( CTax::GetInstance()->GetContinent( pUser ), nTax, TAX_PURCHASE );
#endif // __TAX
			}
			else
				pUser->AddDefinedText( TID_GAME_LACKSPACE, "" );

#ifdef __NPCVEND_STACKBUY
			if (nRemain > 0)
				goto tp_redo;
#endif //__NPCVEND_STACKBUY

		}
#ifdef __GAMEGUARD				
		if (pUser->GetWorld())
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
#endif // __GAMEGUARD
	}
}

#if __VER >= 11 // __GUILDCOMBATCHIP
// 칩으로 아이템 구매
void CDPSrvr::OnBuyChipItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __GAMEGUARD
	BYTE cTab;
#else // __GAMEGUARD
	CHAR cTab;
#endif // __GAMEGUARD
	BYTE nId;
	short nNum;
	DWORD dwItemId;

	ar >> cTab >> nId >> nNum >> dwItemId;

#ifdef __GAMEGUARD
	if (cTab > 3 || cTab < 0 || nId < 0 || nNum < 1 || nNum > 9999 || dwItemId < 0)
#else // __GAMEGUARD
	if (cTab >= MAX_VENDOR_INVENTORY_TAB || nNum < 1)
#endif // __GAMEGUARD
	{
		return;
	}

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) && pUser->m_vtInfo.GetOther() )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU
		CMover* pVendor = pUser->m_vtInfo.GetOther();
		LPCHARACTER lpChar = prj.GetCharacter( pVendor->m_szCharacterKey );

#ifdef __VENDOR_UPDATE
		if (lpChar && lpChar->m_nVenderType == 0) //Is gold
#else // __VENDOR_UPDATE
		if (lpChar && lpChar->m_nVenderType != 1)	// 1 - 칩 상인
#endif // __VENDOR_UPDATE
		{
			return;
		}

#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE

		if( pVendor->IsNPC() == FALSE )		// 판매할 대상이 NPC가 아니면?
			return;

		if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_TRADE, pUser->GetWorld(), pUser->GetPos() ) )
			return;

		CItemElem* pItemElem = pVendor->m_ShopInventory[cTab]->GetAtId( nId );
		if( NULL == pItemElem )
			return;

		if( dwItemId != pItemElem->m_dwItemId )
			return;

#ifdef __OCCUPATION_SHOPITEM
		if( CTax::GetInstance()->IsOccupationShopItem( dwItemId ) && !CTax::GetInstance()->IsOccupationGuildMember( pUser ) )
		{
			pUser->AddDefinedText( TID_GAME_SECRETROOM_STORE_BUY );
			return;
		}
#endif // __OCCUPATION_SHOPITEM

#ifndef __NPCVEND_STACKBUY
		if (nNum > pItemElem->m_nItemNum)
			nNum = pItemElem->m_nItemNum;
#else //__NPCVEND_STACKBUY
		ItemProp* pProp = pItemElem->GetProp();
		int nRemain = 0;

	chip_redo:
		if (nNum > pProp->dwPackMax || nRemain > pProp->dwPackMax)
		{
			if (nRemain > 0)
			{
				nRemain -= pProp->dwPackMax;
			}
			else
			{
				nRemain = nNum - pProp->dwPackMax;
			}
			nNum = pProp->dwPackMax;
		}
		else {
			nRemain = 0;
		}
#endif //__NPCVEND_STACKBUY
		
		// 소지한 칩의 개수가 부족할 때
#ifdef __VENDOR_UPDATE
#ifdef __SQK_MULTIALTERNATEPRICE
		int cost = pItemElem->GetCurrencyCost(lpChar->m_nVenderType);
		if (pUser->m_Inventory.GetAtItemNum(lpChar->m_nVenderType) < (int)(DWORD(cost) * nNum) || cost == -1)
#else //__SQK_MULTIALTERNATEPRICE
		if (pUser->m_Inventory.GetAtItemNum(lpChar->m_nVenderType) < (int)(pItemElem->GetChipCost() * nNum))
#endif //__SQK_MULTIALTERNATEPRICE
#else // __VENDOR_UPDATE
		if (pUser->m_Inventory.GetAtItemNum(II_CHP_RED) < (int)(pItemElem->GetChipCost() * nNum))
#endif // __VENDOR_UPDATE
		{
			// 칩 개수 부족 텍스트 출력
			pUser->AddDefinedText( TID_GAME_LACKCHIP );
			return;
		}

		// 인벤토리가 꽉 찼을 때 
		if( pUser->m_Inventory.IsFull( pItemElem, pItemElem->GetProp(), nNum ) )
		{
#ifdef __SQK_MULTIALTERNATEPRICE
			int cost = pItemElem->GetCurrencyCost(lpChar->m_nVenderType);
			int nChipNum = cost * nNum;
#else //__SQK_MULTIALTERNATEPRICE
			int nChipNum = pItemElem->GetChipCost() * nNum;
#endif //__SQK_MULTIALTERNATEPRICE

#ifdef __VENDOR_UPDATE
			ItemProp* pChipItemProp = prj.GetItemProp(lpChar->m_nVenderType);
#else // __VENDOR_UPDATE
			ItemProp* pChipItemProp = prj.GetItemProp(II_CHP_RED);
#endif // __VENDOR_UPDATE
			if( nChipNum < (int)( pChipItemProp->dwPackMax ) )
			{
				CItemElem* pTempElem;
				int bEmpty = FALSE;
				for( int i = 0; i < pUser->m_Inventory.GetSize(); i++ )
				{
					pTempElem = pUser->m_Inventory.GetAtId( i );
					if( IsUsableItem(pTempElem) && pChipItemProp->dwID == pTempElem->m_dwItemId )
					{
						if( pTempElem->m_nItemNum <= nChipNum )
							bEmpty = TRUE;
						break;
					}
				}
				if( !bEmpty )
				{
					// 인벤이 꽉찼다는 텍스트 출력
					pUser->AddDefinedText( TID_GAME_LACKSPACE, "" );
					return;
				}
			}
		}
		
#ifdef __SQK_MULTIALTERNATEPRICE
		DWORD dwChipCost = cost * nNum;
#else //__SQK_MULTIALTERNATEPRICE
		DWORD dwChipCost = pItemElem->GetChipCost() * nNum;
#endif //__SQK_MULTIALTERNATEPRICE

		if( dwChipCost > 0x7fff )
		{
			for( ; dwChipCost > 0x7fff; )
			{
#ifdef __VENDOR_UPDATE
				pUser->RemoveItemA(lpChar->m_nVenderType, 0x7fff);
#else // __VENDOR_UPDATE
				pUser->RemoveItemA(II_CHP_RED, 0x7fff);
#endif // __VENDOR_UPDATE
				dwChipCost -= 0x7fff;
			}
		}
#ifdef __VENDOR_UPDATE
		pUser->RemoveItemA(lpChar->m_nVenderType, (short)dwChipCost);
#else // __VENDOR_UPDATE
		pUser->RemoveItemA(II_CHP_RED, (short)dwChipCost);
#endif // __VENDOR_UPDATE
				
		// 구매 아이템 생성
		CItemElem itemElem;
		itemElem	= *pItemElem;
		itemElem.m_nItemNum = nNum;
		itemElem.SetSerialNumber();
		if( pUser->CreateItem( &itemElem ) )
		{
			// 로그 남김
			LogItemInfo aLogItem;
			aLogItem.Action = "B";
			aLogItem.SendName = pUser->GetName();
			CString strTemp;
			strTemp.Format( "%s_C", pVendor->GetName() );
			aLogItem.RecvName = (LPCTSTR)strTemp;
			aLogItem.WorldId = pUser->GetWorld()->GetID();
#ifdef __VENDOR_UPDATE
#ifdef __SQK_MULTIALTERNATEPRICE
			int cost = pItemElem->GetCurrencyCost(lpChar->m_nVenderType);
			aLogItem.Gold = pUser->GetItemNum(lpChar->m_nVenderType) + cost * nNum;
#else //__SQK_MULTIALTERNATEPRICE
			aLogItem.Gold = pUser->GetItemNum(lpChar->m_nVenderType) + itemElem.GetChipCost() * nNum;
#endif //__SQK_MULTIALTERNATEPRICE
			aLogItem.Gold2 = pUser->GetItemNum(lpChar->m_nVenderType);
#else // __VENDOR_UPDATE
			aLogItem.Gold = pUser->GetItemNum(II_CHP_RED) + itemElem.GetChipCost() * nNum;
			aLogItem.Gold2 = pUser->GetItemNum(II_CHP_RED);
#endif // __VENDOR_UPDATE

#ifdef __SQK_MULTIALTERNATEPRICE
			aLogItem.Gold_1 = (DWORD)((-1) * (int)((cost * nNum)));
#else //__SQK_MULTIALTERNATEPRICE
			aLogItem.Gold_1 = (DWORD)( (-1) * (int)( (itemElem.GetChipCost() * nNum) ) );
#endif //__SQK_MULTIALTERNATEPRICE
			OnLogItem( aLogItem, &itemElem, nNum );
		}
		else
		{
			LogItemInfo aLogItem;
			aLogItem.Action = "B";
			aLogItem.SendName = pUser->GetName();
			CString strTemp;
			strTemp.Format( "%s_CF", pVendor->GetName() );
			aLogItem.RecvName = (LPCTSTR)strTemp;
			aLogItem.WorldId = pUser->GetWorld()->GetID();
#ifdef __VENDOR_UPDATE
#ifdef __SQK_MULTIALTERNATEPRICE
			int cost = pItemElem->GetCurrencyCost(lpChar->m_nVenderType);
			aLogItem.Gold = pUser->GetItemNum(lpChar->m_nVenderType) + cost * nNum;
#else //__SQK_MULTIALTERNATEPRICE
			aLogItem.Gold = pUser->GetItemNum(lpChar->m_nVenderType) + itemElem.GetChipCost() * nNum;
#endif //__SQK_MULTIALTERNATEPRICE
			aLogItem.Gold2 = pUser->GetItemNum(lpChar->m_nVenderType);
#else // __VENDOR_UPDATE
			aLogItem.Gold = pUser->GetItemNum(II_CHP_RED) + itemElem.GetChipCost() * nNum;
			aLogItem.Gold2 = pUser->GetItemNum(II_CHP_RED);
#endif // __VENDOR_UPDATE

#ifdef __SQK_MULTIALTERNATEPRICE
			aLogItem.Gold_1 = (DWORD)((-1) * (int)((cost * nNum)));
#else //__SQK_MULTIALTERNATEPRICE
			aLogItem.Gold_1 = (DWORD)((-1) * (int)((itemElem.GetChipCost() * nNum)));
#endif //__SQK_MULTIALTERNATEPRICE
			OnLogItem( aLogItem, &itemElem, nNum );
		}
#ifdef __NPCVEND_STACKBUY
		if (nRemain > 0)
			goto chip_redo;
#endif //__NPCVEND_STACKBUY

#ifdef __GAMEGUARD				
		if (pUser->GetWorld())
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
#endif // __GAMEGUARD

	}
}
#endif // __GUILDCOMBATCHIP

//NPC에게 파는 경우
void CDPSrvr::OnSellItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE nId;
	short nNum;

	ar >> nId >> nNum;
	if( nNum < 1 )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) && IsValidObj( pUser->m_vtInfo.GetOther() ) )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU

		if( pUser->m_vtInfo.GetOther()->IsNPC() == FALSE )		// 판매할 대상이 NPC가 아니면?
			return;
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE
		if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_TRADE, pUser->GetWorld(), pUser->GetPos() ) )
			return;

		CItemElem* pItemElem = pUser->m_Inventory.GetAtId( nId );		
		if( IsUsableItem( pItemElem ) == FALSE )
			return;

#ifdef __SYS_LOCK_ITEM
		if (pItemElem->IsItemLocked())
		{
			CString str;
			str.Format("Please unlock %s before selling.", pItemElem->GetProp()->szName);
			pUser->AddText(str, COLOR_WARNING);
			return;
		}
#endif // __SYS_LOCK_ITEM

		ItemProp* pProp	= pItemElem->GetProp();
		if( pProp->dwItemKind3 == IK3_EVENTMAIN )
			return;
		
#ifdef __GAMEGUARD
		if (pProp->dwItemKind3 == IK3_EGG)
		{
			CString str;
			str.Format("Sorry, you cannot sell pets to an NPC.");
			pUser->AddText(str, COLOR_ERROR);
			return;
		}
#endif // __GAMEGUARD

		if( pItemElem->IsQuest() )
			return;

		if( pUser->m_Inventory.IsEquip( nId ) )
		{
			pUser->AddDefinedText( TID_GAME_EQUIPTRADE, "" );
			return;
		}
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
		if( pItemElem->m_dwItemId == II_SYS_SYS_SCR_SEALCHARACTER )
			return;
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
#if __VER >= 11 // __MA_VER11_02				// 수표 개념 화페 '페린' 추가
		if( pItemElem->m_dwItemId == II_SYS_SYS_SCR_PERIN )
			return;
#endif // __MA_VER11_02				// 수표 개념 화페 '페린' 추가

		if( nNum > pItemElem->m_nItemNum )
			nNum = pItemElem->m_nItemNum;
		if( nNum < 1 )
			nNum = 1;

#if __VER >= 12 // __TAX
		int nGold = ( pItemElem->GetCost() / 4 );

#ifdef __NO_TICKET_RESELL
		if (pItemElem->GetProp()->dwItemKind3 == IK3_TICKET && (pItemElem->m_dwKeepTime > 0 || pItemElem->IsFlag(CItemElem::expired)))
		{
			nGold = 1;
		}
#endif // __NO_TICKET_RESELL

#ifdef __LEESE_NO_EVENTITEM_RESELL
		if (pItemElem->IsFlag(CItemElem::binds) && (pItemElem->m_dwItemId != II_GEN_MAT_ORICHALCUM01_1 && pItemElem->m_dwItemId != II_GEN_MAT_MOONSTONE_1) && pItemElem->GetProp()->dwFlag != IP_FLAG_EQUIP_BIND)
		{
			nGold = 1;
		}
#endif //__LEESE_NO_EVENTITEM_RESELL

#ifdef __SHOP_COST_RATE
		nGold = (int)( prj.m_EventLua.GetShopSellFactor() * nGold );
#endif // __SHOP_COST_RATE
		if( nGold == 0 )
			nGold = 1;

#ifdef __SECURITY_FIXES
		if (pItemElem->IsBinds())
			nGold = 1;
#endif // __SECURITY_FIXES

		int nTax = 0;
		if (CTax::GetInstance()->IsApplyTaxRate(pUser, pItemElem))
		{
			nTax = (int)(nGold * CTax::GetInstance()->GetSalesTaxRate(pUser));
#ifdef __BUYBACK
			pItemElem->setBuyBackCont(CTax::GetInstance()->GetContinent(pUser));
			pItemElem->setBuyBackTaxes(nTax);
#endif // __BUYBACK
		}
		nGold -= nTax;
#ifdef __BUYBACK
		pItemElem->setBuyBackPrice(nGold + ceil(nGold * (double)((double)__BUYBACK_PERCENT_COST / 100.0f)));
#endif // __BUYBACK
		nGold *= nNum;
		nTax  *= nNum;
		float fTmpGold = (float)( pUser->GetGold() + nGold );
#else // __TAX
		float fTmpGold = pUser->GetGold() + ( pItemElem->GetCost() / 4 ) * nNum;
#endif // __TAX

		if( fTmpGold >= 2100000000 )
		{
			return;
		}

		if( pProp->dwParts == PARTS_RIDE && pProp->dwItemJob == JOB_VAGRANT )
			return;

		if( pUser->IsUsing( pItemElem ) )
		{
			pUser->AddDefinedText( TID_GAME_CANNOT_DO_USINGITEM );
			return;
		}

		LogItemInfo aLogItem;
		aLogItem.Action = "S";
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = pUser->m_vtInfo.GetOther()->GetName();
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = pUser->GetGold();
#if __VER >= 12 // __TAX
		aLogItem.Gold2 = pUser->GetGold() + nGold;
#else // __TAX
		aLogItem.Gold2 = pUser->GetGold() + ( pItemElem->GetCost() / 4 ) * nNum;
#endif // __TAX
		OnLogItem( aLogItem, pItemElem, nNum );
		int nCost	= (int)pItemElem->GetCost() / 4;

#if __VER < 8 // __S8_PK
		KarmaProp* pProp = prj.GetKarmaProp( pUser->m_nSlaughter );
		if( pProp )
		{
			if( pProp->nGrade == -6 )
				return;

			if( pProp->fSellPenaltyRate != 0.0f )
			{
				nCost = (int)( nCost * pProp->fSellPenaltyRate );
			}
		}
#endif // __VER < 8 // __S8_PK

		if( nCost < 1 )	
			nCost = 1;

#ifdef __GAMEGUARD
		long long lEntireCost = nNum * nCost;
		if (lEntireCost > INT_MAX)
			return;
#endif // __GAMEGUARD

#if __VER >= 12 // __TAX
		if( nGold < 1 )
			nGold = 1;
		
		pUser->AddGold( nGold );
		if( nTax )
			CTax::GetInstance()->AddTax( CTax::GetInstance()->GetContinent( pUser ), nTax, TAX_SALES );
#else // __TAX
		pUser->AddGold( nCost * nNum );
#endif // __TAX

#ifdef __BUYBACK
		if (pUser->m_BuyBack.GetCount() >= pUser->m_BuyBack.GetMax())
		{
			pUser->m_BuyBack.RemoveAtId(0);
			Reorganize(&pUser->m_BuyBack);
			pUser->BuyBackRemoveItem(0);
		}
		CItemElem itemElem = *pItemElem;
		itemElem.m_nItemNum = nNum;

		pUser->m_BuyBack.Add(&itemElem);
		pUser->SendBuyBack(&itemElem);
#endif // __BUYBACK

		pUser->RemoveItem( nId, nNum );

#ifdef __GAMEGUARD
		if (pUser->GetWorld())
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
#endif // __GAMEGUARD
	}
}

// 패스워드 변경창을 띄울것인지 패스워드 확인창을 띄을것인지를 알려준다
void CDPSrvr::OnOpenBankWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD	dwId, dwItemId;
	ar >> dwId >> dwItemId;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( dwId == NULL_ID && !CNpcChecker::GetInstance()->IsCloseNpc( MMI_BANKING, pUser->GetWorld(), pUser->GetPos() ) 
#ifdef __PREMIUM
			&& !pUser->IsPremium() && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)
#endif // __PREMIUM
			)
			return;

#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE

#if __VER >= 8 // __S8_PK
		if( pUser->IsChaotic() )
		{
			CHAO_PROPENSITY Propensity = prj.GetPropensityPenalty( pUser->GetPKPropensity() );
			if( !Propensity.nBank )
				return;
		}
#endif // __VER >= 8 // __S8_PK
		if( 0 == strcmp( pUser->m_szBankPass, "0000") )
		{
			// 변경창을 띄우라고 함
			pUser->AddBankWindow( 0, dwId, dwItemId );
		}
		else
		{
			// 확인창을 띄우라고 함
			pUser->AddBankWindow( 1, dwId, dwItemId );
		}
	}
}

void CDPSrvr::OnOpenGuildBankWnd(CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )		
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE
		if( pUser->m_vtInfo.GetOther() )	// 거래중인 대상이 있으면?
			return;
		if( pUser->m_vtInfo.VendorIsVendor() )		// 내가 팔고 있으면?
			return;
		if( pUser->m_bBank )				// 창고를 열고 있으면?
			return;
#ifdef __S_SERVER_UNIFY
		if( pUser->m_bAllAction == FALSE )
			return;
#endif // __S_SERVER_UNIFY

		pUser->AddGuildBankWindow( 0 );
		pUser->m_bGuildBank = TRUE;
	}
}

void CDPSrvr::OnCloseBankWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );	
	if( IsValidObj( pUser ) )
	{
		pUser->m_bBank = FALSE;
		pUser->m_bInstantBank	= FALSE;
	}
}

void CDPSrvr::OnDoUseSkillPoint( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	SKILL aJobSkill[ MAX_SKILL_JOB ] = {0, };
	
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
		ar >> aJobSkill[i].dwSkill >> aJobSkill[i].dwLevel;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );	
	if( IsValidObj( pUser ) )
	{	// 바꿀수 있을지?  확인
		int nChangePoint = 0;
		for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
		{		
			if( aJobSkill[i].dwSkill == NULL_ID )
				continue;
			ItemProp* pSkillProp    = prj.GetSkillProp( aJobSkill[i].dwSkill );			
			if( pSkillProp == NULL )
				continue;

			if( aJobSkill[i].dwLevel < pUser->m_aJobSkill[i].dwLevel || aJobSkill[i].dwLevel > pSkillProp->dwExpertMax )
			{
				pUser->AddDefinedText(TID_RESKILLPOINT_ERROR);
				return;						
			}
			int nPoint = (aJobSkill[i].dwLevel - pUser->m_aJobSkill[i].dwLevel) * prj.GetSkillPoint( pSkillProp );
			nChangePoint += nPoint;
		}

		if( nChangePoint == 0 ) 
			return;
		if( pUser->m_nSkillPoint < nChangePoint )
		{
			pUser->AddDefinedText(TID_RESKILLPOINT_ERROR);
			return;						
		}
		
		// 스킬별 재분배 하기 // 스킬 레벨 셋팅
		pUser->m_nSkillPoint -= nChangePoint;

		for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
		{				
			LPSKILL lpSkill = &(pUser->m_aJobSkill[i]);
			if( lpSkill )
			{
				if( aJobSkill[i].dwSkill == NULL_ID )
					continue;
				ItemProp* pSkillProp    = prj.GetSkillProp( lpSkill->dwSkill );			
				if( pSkillProp == NULL )
					continue;

				int nPoint = (aJobSkill[i].dwLevel - lpSkill->dwLevel) * prj.GetSkillPoint( pSkillProp );
				if( 0 < nPoint ) // 스킬 부여한것만 로그에 남기자
					g_dpDBClient.SendLogSkillPoint( LOG_SKILLPOINT_USE, nPoint, (CMover*)pUser, &aJobSkill[i] );

				lpSkill->dwLevel = aJobSkill[i].dwLevel;
			}
		}	
		g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_SYS_EXCHAN01, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
		pUser->AddDoUseSkillPoint( &aJobSkill[0], pUser->m_nSkillPoint );
#ifdef __S_NEW_SKILL_2
		g_dpDBClient.SaveSkill( pUser );
#endif // __S_NEW_SKILL_2
	}
}

void CDPSrvr::OnCloseGuildBankWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )		
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) )
	{
		pUser->m_bGuildBank = FALSE;
	}
}

void CDPSrvr::OnBankToBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE nFlag, nPutSlot, nSlot, nId;
	short nItemNum;
	DWORD dwGold;
	ar >> nFlag >> nPutSlot >> nSlot;

	if( nPutSlot >= 3 || nSlot >= 3 )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU
#ifdef __BLOCK_STAFF_ACTIONS
		if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
		{
			pUser->AddText("[Titanium] Unauthorized Action. (Banking)", COLOR_ERROR);
			return;
		}
#endif //__BLOCK_STAFF_ACTIONS
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE
		if( pUser->m_idPlayerBank[nPutSlot] != 0 && pUser->m_idPlayerBank[nSlot] != 0 && pUser->IsCommBank() )
		{
			if( nFlag == 1 )	// Item
			{
				ar >> nId >> nItemNum;
				CItemElem* pItemElem	= pUser->m_Bank[nPutSlot].GetAtId( nId );
				if( pItemElem == NULL )
					return;

				if( nItemNum > pItemElem->m_nItemNum )
					nItemNum	= pItemElem->m_nItemNum;
				if( nItemNum < 1 )
					nItemNum	= 1;

				if( MAX_BANK > pUser->m_Bank[nSlot].GetCount() )
				{
					CItemElem itemElem;
					itemElem	= *pItemElem;
					itemElem.m_nItemNum		= nItemNum;
					pUser->AddPutItemBank( nSlot, &itemElem );
					pUser->m_Bank[nSlot].Add( &itemElem );
					pUser->UpdateItemBank( nPutSlot, nId, UI_NUM, pItemElem->m_nItemNum - nItemNum );		// 은행에 빼기및 전송

					LogItemInfo aLogItem;
					aLogItem.Action = "A";
					aLogItem.SendName = pUser->GetName();
					aLogItem.RecvName = "BANK";
					aLogItem.WorldId = pUser->GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
					aLogItem.Gold_1 = pUser->m_dwGoldBank[pUser->m_nSlot];
					aLogItem.nSlot = nSlot;
					aLogItem.nSlot1 = nPutSlot;
					OnLogItem( aLogItem, &itemElem, itemElem.m_nItemNum );
				}
				else
				{
					// 꽉차서 넣을수 가 없음. 메세지 처리
					pUser->AddBankIsFull();
				}
			}
			else		// Gold
			{
				 ar >> dwGold;
				
				 if( dwGold > pUser->m_dwGoldBank[nPutSlot] )
					 dwGold = pUser->m_dwGoldBank[nPutSlot];
				 
				 int nGold = dwGold;
				 if( CanAdd( pUser->m_dwGoldBank[nSlot], nGold ) )
				 {
					 pUser->m_dwGoldBank[nSlot]	+= nGold;
					 pUser->m_dwGoldBank[nPutSlot] -= nGold;
					 
					 pUser->AddPutGoldBank( nSlot, pUser->GetGold(), pUser->m_dwGoldBank[nSlot] );
					 pUser->AddPutGoldBank( nPutSlot, pUser->GetGold(), pUser->m_dwGoldBank[nPutSlot] );

					 LogItemInfo aLogItem;
					 aLogItem.Action = "A";
					 aLogItem.SendName = pUser->GetName();
					 aLogItem.RecvName = "BANK";
					 aLogItem.WorldId = pUser->GetWorld()->GetID();
					 aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
					 //aLogItem.ItemName = "SEED";
					 _stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
					 aLogItem.itemNumber = nGold;
					 aLogItem.Gold_1 = pUser->m_dwGoldBank[pUser->m_nSlot];
					 aLogItem.nSlot = nSlot;
					 aLogItem.nSlot1 = nPutSlot;
					 OnLogItem( aLogItem );
				 }	
			}
#ifdef __GAMEGUARD
			if (pUser->GetWorld())
				g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
#endif // __GAMEGUARD
		}
	}
}
void CDPSrvr::OnPutItemBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE nSlot, nId;
	short nItemNum;

	ar >> nSlot >> nId >> nItemNum;
	if( nSlot >= 3 )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU

#ifdef __BLOCK_STAFF_ACTIONS
		if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
		{
			pUser->AddText("[Titanium] Unauthorized Action. (Banking)", COLOR_ERROR);
			return;
		}
#endif //__BLOCK_STAFF_ACTIONS

		if( !pUser->m_bInstantBank )
		{
			if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_BANKING, pUser->GetWorld(), pUser->GetPos() ) 
#ifdef __PREMIUM
				&& !pUser->IsPremium() && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)
#endif // __PREMIUM
				)
				return;
		}
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE
		if( pUser->m_nSlot == nSlot || ( pUser->m_idPlayerBank[nSlot] != 0 && pUser->IsCommBank() ) )
		{
			CItemElem* pItemElem = pUser->m_Inventory.GetAtId( nId );
			if( IsUsableItem( pItemElem ) == FALSE )
				return;

			if( nSlot != pUser->m_nSlot )
			{
				if( pItemElem->IsQuest() )
					return;
			}
				
#if __VER >= 11 // __SYS_IDENTIFY
			if( pItemElem->IsBinds() )
				return;
#else	// __SYS_IDENTIFY
			if( pItemElem->IsFlag( CItemElem::binds ) )
				return;
#endif	// __SYS_IDENTIFY

			ItemProp* pProp	= pItemElem->GetProp();
			if( !pProp )
				return;

			if( pUser->IsUsing( pItemElem ) )
			{
				pUser->AddDefinedText( TID_GAME_CANNOT_DO_USINGITEM );
				return;
			}
			
			if( pProp->dwItemKind3 == IK3_EVENTMAIN ||
				pProp->dwItemKind3 == IK3_LINK ||
				( pProp->dwItemKind3 == IK3_CLOAK && pItemElem->m_idGuild != 0 ) )
				return;

			if( pProp->dwParts == PARTS_RIDE && pProp->dwItemJob == JOB_VAGRANT )
				return;


			if( pUser->m_Inventory.IsEquip( nId ) )
			{
				pUser->AddDefinedText( TID_GAME_EQUIPTRADE, "" );
				return;
			}
			
#ifdef __SYS_LOCK_ITEM
			if (pItemElem->IsItemLocked())
			{
				CString str;
				str.Format("Please unlock %s before placing it on your bank.", pItemElem->GetProp()->szName);
				pUser->AddText(str, COLOR_WARNING);
				return;
			}
#endif // __SYS_LOCK_ITEM

			if( nItemNum > pItemElem->m_nItemNum )
				nItemNum	= pItemElem->m_nItemNum;
			if( nItemNum < 1 )
				nItemNum	= 1;

			if( pUser->m_Bank[nSlot].IsFull( pItemElem, pProp, nItemNum ) == FALSE )
			{
				CItemElem itemElem;
				itemElem	= *pItemElem;
				itemElem.m_nItemNum		= nItemNum;
				pUser->AddPutItemBank( nSlot, &itemElem );
				pUser->m_Bank[nSlot].Add( &itemElem );

				LogItemInfo aLogItem;
				aLogItem.Action = "P";
				aLogItem.SendName = pUser->GetName();
				aLogItem.RecvName = "BANK";
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
				aLogItem.Gold_1 = pUser->m_dwGoldBank[pUser->m_nSlot];
				aLogItem.nSlot1 = nSlot;
				OnLogItem( aLogItem, pItemElem, nItemNum );

				pUser->RemoveItem( nId, nItemNum );
#ifdef __SECURITY_FIXES
				CWorld* pWorld = pUser->GetWorld();				
				g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos(), pUser->GetLayer() );
#endif //__SECURITY_FIXES			
			}
			else
			{
				// 꽉차서 넣을수 가 없음. 메세지 처리
				pUser->AddBankIsFull();
			}
		}
#ifdef __GAMEGUARD
		if (pUser->GetWorld())
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
#endif // __GAMEGUARD
	}
}

void CDPSrvr::OnPutItemGuildBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )		
		return;

	BYTE nId, mode;
#ifdef __GAMEGUARD
	UINT nItemNum;
#else // __GAMEGUARD
	DWORD nItemNum;
#endif // __GAMEGUARD

	ar >> nId >> nItemNum >> mode;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );	
	if( IsValidObj( pUser ) )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU

#ifdef __BLOCK_STAFF_ACTIONS
		if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
		{
			pUser->AddText("[Titanium] Unauthorized Action. (Guild Banking)", COLOR_ERROR);
			return;
		}
#endif //__BLOCK_STAFF_ACTIONS

#if __VER >= 15 // __GUILD_HOUSE
		if( !pUser->GetWorld() || !GuildHouseMng->IsGuildHouse( pUser->GetWorld()->GetID() ) )
#endif // __GUILD_HOUSE
			if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_GUILDBANKING, pUser->GetWorld(), pUser->GetPos() ) 
#ifdef __PREMIUM
				&& !pUser->IsPremium() && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)
#endif // __PREMIUM
				)
				return;

#ifdef __GAMEGUARD
		if (nId < 1 || nId > pUser->m_Inventory.GetMax())
			return;
#endif // __GAMEGUARD

		if( mode == 0 ) // 길드창고에는 Gold를 넣을수 없습니다.
			return;

		if( nId < 0 || nId > pUser->m_Inventory.GetMax() )
			return;
		
		CItemElem* pItemElem = pUser->m_Inventory.GetAtId( nId );		
		if( IsUsableItem( pItemElem ) == FALSE )
			return;
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE
		if( pItemElem->IsQuest() )
			return;
		
		if( pItemElem->IsBinds() )
			return;

		if( pUser->IsUsing( pItemElem ) )
		{
			pUser->AddDefinedText( TID_GAME_CANNOT_DO_USINGITEM );
			return;
		}

		ItemProp* pProp	= pItemElem->GetProp();

#ifdef __GAMEGUARD
		if (!pProp)
			return;
#endif // __GAMEGUARD

		if( pProp->dwParts == PARTS_RIDE && pProp->dwItemJob == JOB_VAGRANT )
			return;

		if( pUser->m_Inventory.IsEquip( nId ) )
		{
			pUser->AddDefinedText( TID_GAME_EQUIPTRADE, "" );
			return;
		}

		if( pItemElem->IsCharged() )
			return;
	
#ifdef __SYS_LOCK_ITEM
		if (pItemElem->IsItemLocked())
		{
			CString str;
			str.Format("Please unlock %s before placing it on your guild bank.", pItemElem->GetProp()->szName);
			pUser->AddText(str, COLOR_WARNING);
			return;
		}
#endif // __SYS_LOCK_ITEM

#ifdef __GAMEGUARD
		if ((short)(nItemNum) > pItemElem->m_nItemNum)
#else // __GAMEGUARD
		if ((int)(nItemNum) > pItemElem->m_nItemNum)
#endif // __GAMEGUARD
		{
			nItemNum = pItemElem->m_nItemNum;
		}

#ifdef __GAMEGUARD
		if ((short)(nItemNum) < 1)
#else // __GAMEGUARD
		if (nItemNum < 1)
#endif // __GAMEGUARD
		{
			nItemNum = 1;
		}
		
		CGuild*	pGuild = pUser->GetGuild();
		if( pGuild )
		{
			CItemElem itemElem;
			itemElem	= *pItemElem;
#ifdef __GAMEGUARD
			itemElem.m_nItemNum = nItemNum;
#else // __GAMEGUARD
			itemElem.m_nItemNum = (short)nItemNum;
#endif // __GAMEGUARD
			if ( pGuild->m_GuildBank.Add( &itemElem ) )
			{
				LogItemInfo aLogItem;
				aLogItem.Action = "W";
				aLogItem.SendName = pUser->GetName();
				aLogItem.RecvName = "GUILDBANK";
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
				OnLogItem( aLogItem, &itemElem, nItemNum );
#ifdef __GAMEGUARD
				pUser->RemoveItem((BYTE)(nId), nItemNum);
				UpdateGuildBank(pGuild, GUILD_PUT_ITEM, 0, pUser->m_idPlayer, &itemElem, 0, (short)(nItemNum));
#else // __GAMEGUARD
				pUser->RemoveItem((BYTE)(nId), (short)(nItemNum));
				UpdateGuildBank(pGuild, GUILD_PUT_ITEM, 0, pUser->m_idPlayer, &itemElem, 0, nItemNum);
#endif // __GAMEGUARD
				pUser->AddPutItemGuildBank( &itemElem );
				g_UserMng.AddPutItemElem( pUser, &itemElem );
			}
			else
			{
				pUser->AddDefinedText( TID_GAME_GUILDBANKFULL, "" );		// 길드창고가 꽉찼시유~
			}
		}
#ifdef __GAMEGUARD
		if (pUser->GetWorld())
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
#endif // __GAMEGUARD
	}
}

void CDPSrvr::OnGetItemGuildBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )		
		return;

	BYTE nId, mode;
#ifdef __GAMEGUARD
	DWORD dwItemNum;
#else // __GAMEGUARD
	int dwItemNum;
#endif // __GAMEGUARD

	ar >> nId >> dwItemNum >> mode;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );	
	if( IsValidObj( pUser ) )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU

#ifdef __BLOCK_STAFF_ACTIONS
		if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
		{
			pUser->AddText("[Titanium] Unauthorized Action. (Guild Banking)", COLOR_ERROR);
			return;
		}
#endif //__BLOCK_STAFF_ACTIONS
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE
#if __VER >= 15 // __GUILD_HOUSE
		if( !pUser->GetWorld() || !GuildHouseMng->IsGuildHouse( pUser->GetWorld()->GetID() ) )
#endif // __GUILD_HOUSE
			if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_GUILDBANKING, pUser->GetWorld(), pUser->GetPos() ) 
#ifdef __PREMIUM
				&& !pUser->IsPremium() && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)
#endif // __PREMIUM
				)
				return;
		
#ifdef __SECURITY_FIXES
		if (!pUser->GetGuild())
			return;
#endif // __SECURITY_FIXES

#ifdef __GAMEGUARD
		if (dwItemNum < 1 || dwItemNum > 2100000000)
			return;
#endif // __GAMEGUARD

		if (mode == 0) // Gold를 길드창고에서 빼낼때
		{
#ifdef __GAMEGUARD
			int nGold = (UINT)dwItemNum;
#else // __GAMEGUARD
			int nGold = (int)dwItemNum;
#endif // __GAMEGUARD
			if( nGold <= 0 || CanAdd( pUser->GetGold(), nGold ) == FALSE )
				return;

			CGuild*	pGuild = pUser->GetGuild();
			if (pGuild && pGuild->IsGetPenya(pUser->m_idPlayer))
			{
				if( (DWORD)nGold > pGuild->m_nGoldGuild )
					return;

				pUser->AddGold( nGold, FALSE );
				pGuild->m_nGoldGuild -= nGold;
				pUser->AddGetGoldGuildBank( nGold, 0, pUser->m_idPlayer, 0 );	// 0은 업데이트 시킨 클라에게 
				pGuild->DecrementMemberContribution( pUser->m_idPlayer, nGold, 0 );
				UpdateGuildBank(pGuild, GUILD_GET_PENYA, 1, pUser->m_idPlayer, NULL, nGold );

				LogItemInfo aLogItem;
				aLogItem.Action = "Y";
				aLogItem.SendName = "GUILDBANK";
				aLogItem.RecvName = pUser->GetName();
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = pUser->GetGold() - nGold;
				aLogItem.Gold2 = pUser->GetGold();
				//aLogItem.ItemName = "SEED";
				_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
				aLogItem.itemNumber = nGold;
				OnLogItem( aLogItem );

				CGuildMember*	pMember;
				CUser*			pUsertmp;
				map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
				for( ; i != pGuild->m_mapPMember.end(); ++i )
				{
					pMember		= i->second;
					pUsertmp	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
					if( IsValidObj( pUsertmp ) && pUsertmp != pUser )
					{
						pUsertmp->AddGetGoldGuildBank( nGold, 2, pUser->m_idPlayer, 0 );	// 2는 업데이트 해야할 클라이게
					}
				}

				g_DPCoreClient.SendGuildMsgControl_Bank_Penya( pUser, nGold, 2, 0 ); 	// 2는 업데이트 해야할 다른 월드서버의 클라이언트
			}
			//	Core 서버에 전 서버에 업데이트 되야함을 알린다.
		}
		else if (mode == 1) // 아이템을 길드창고에서 빼낼때
		{
			CGuild*			pGuild = pUser->GetGuild();
			if (pGuild && pGuild->IsGetItem(pUser->m_idPlayer))
			{
#ifdef __GAMEGUARD
				if (nId < 0 || nId > pGuild->m_GuildBank.GetMax())
					return;
#endif // __GAMEGUARD
				
				CItemElem* pItemElem	= pGuild->m_GuildBank.GetAtId( nId );
				
				if( NULL == pItemElem )
					return;
				
#ifdef __GAMEGUARD
				if ((short)(dwItemNum) > pItemElem->m_nItemNum)
#else // __GAMEGUARD
				if ((int)(dwItemNum) > pItemElem->m_nItemNum)
#endif // __GAMEGUARD
				{
					dwItemNum = pItemElem->m_nItemNum;
				}

#ifdef __GAMEGUARD
				if ((short)(dwItemNum) < 1)
#else // __GAMEGUARD
				if (dwItemNum < 1)
#endif // __GAMEGUARD
				{
					dwItemNum = 1;
				}
				
				CItemElem itemElem;
				itemElem	= *pItemElem;
				itemElem.m_nItemNum		= (short)( dwItemNum );
				itemElem.m_dwObjId		= pItemElem->m_dwObjId;

				if( pUser->m_Inventory.Add( &itemElem ) )
				{
					if (pItemElem->m_nItemNum > (int)( dwItemNum ) )
						pItemElem->m_nItemNum	= (short)( pItemElem->m_nItemNum - dwItemNum );
					else 
						pGuild->m_GuildBank.RemoveAtId( nId );
					UpdateGuildBank(pGuild, GUILD_GET_ITEM, 0, pUser->m_idPlayer, &itemElem, 0, (short)( dwItemNum ) );
					LogItemInfo aLogItem;
					aLogItem.Action = "Y";
					aLogItem.SendName = "GUILDBANK";
					aLogItem.RecvName = pUser->GetName();
					aLogItem.WorldId = pUser->GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
					OnLogItem( aLogItem, &itemElem, dwItemNum );
				
					// 클라이언트에게 아이템이 인벤토리에 추가됨을 알린다.
					pUser->AddGetItemGuildBank( &itemElem );
					// 자신을 제외한 모든 클라이언트에게 알려준다.
					g_UserMng.AddGetItemElem( pUser, &itemElem );
				}
				else
				{
					// 꽉차서 넣을수 가 없음. 메세지 처리
					pUser->AddBankIsFull();
				}
			}
		}
#ifdef __GAMEGUARD
		if (pUser->GetWorld())
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
#endif // __GAMEGUARD
	}
}


void CDPSrvr::OnGuildBankMoveItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )		
		return;

	BYTE nSrcIndex, nDestIndex;
	
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		ar >> nSrcIndex >> nDestIndex;
		
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE

		// 길드 창고의 아이템을 스왑한다.
		CGuild* pGuild = pUser->GetGuild();
		if ( pGuild )
		{
			pGuild->m_GuildBank.Swap( nSrcIndex, nDestIndex );
			// 모든 클라이언트에게 길드창고에서 아이템이 이동했음을 알려준다.
			// 길드창고를 업데이트한다.
			UpdateGuildBank(pGuild, 4); // 4번은 아이템이 스왑된것임
		}
#ifdef __GAMEGUARD
		if (pUser->GetWorld())
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
#endif // __GAMEGUARD
	}
}

void CDPSrvr::UpdateGuildBank(CGuild* p_GuildBank, int p_Mode, BYTE cbUpdate, u_long idPlayer, CItemElem* pItemElem, DWORD dwPenya, short nItemCount )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_BANK_UPDATE, DPID_UNKNOWN, DPID_UNKNOWN );

	if (p_GuildBank)
	{
		ar << p_GuildBank->m_idGuild;
		ar << p_GuildBank->m_nGoldGuild;
		p_GuildBank->m_GuildBank.Serialize(ar);
		ar << cbUpdate;	// 멤버의 공헌페냐를 업뎃해야하는가? 
		ar << idPlayer;
		ar << p_Mode;
		if( pItemElem == NULL )
		{
			ar << (DWORD)0;
			ar << (int)0;
			ar << (SERIALNUMBER)0;
		}
		else
		{
			ar << pItemElem->m_dwItemId;
			ar << pItemElem->GetAbilityOption();
			ar << pItemElem->GetSerialNumber();
		}		
		ar << nItemCount;
		ar << dwPenya;
		SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
	}
}


void CDPSrvr::OnGetItemBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE nSlot, nId;
	short nItemNum;

	ar >> nSlot >> nId >> nItemNum;
	if( nSlot >= 3 )
		return;
	
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) )
	{
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif	//__MIKSIK_EQUALIZED_MODE	
		
		if( !pUser->m_bInstantBank )
		{
			if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_BANKING, pUser->GetWorld(), pUser->GetPos() ) 
#ifdef __PREMIUM
				&& !pUser->IsPremium() && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)
#endif // __PREMIUM
				)
				return;
		}
		if( pUser->m_nSlot == nSlot || ( pUser->m_idPlayerBank[nSlot] != 0 && pUser->IsCommBank() ) )
		{
			CItemElem* pItemElem	= pUser->m_Bank[nSlot].GetAtId( nId );
			if( NULL == pItemElem )
				return;
	
			if( nItemNum > pItemElem->m_nItemNum )
				nItemNum = pItemElem->m_nItemNum;
			if( nItemNum < 1 )
				nItemNum	= 1;
			
			if( MAX_INVENTORY > pUser->m_Inventory.GetCount() )
			{
				CItemElem itemElem;
				itemElem	= *pItemElem;
				itemElem.m_nItemNum		= nItemNum;
				pUser->AddGetItemBank( &itemElem );			// 유저에게 전송
				pUser->m_Inventory.Add( &itemElem );		// 인벤에 넣기

				LogItemInfo aLogItem;
				aLogItem.Action = "G";
				aLogItem.SendName = "BANK";
				aLogItem.RecvName = pUser->GetName();
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
				aLogItem.Gold_1 = pUser->m_dwGoldBank[pUser->m_nSlot];
				aLogItem.nSlot = nSlot;
				OnLogItem( aLogItem, pItemElem, nItemNum );
				pUser->UpdateItemBank( nSlot, nId, UI_NUM, pItemElem->m_nItemNum - nItemNum );		// 은행에 빼기및 전송	
			}
			else
			{
				// 꽉차서 넣을수 가 없음. 메세지 처리
				pUser->AddBankIsFull();
			}
		}
#ifdef __GAMEGUARD
		if (pUser->GetWorld())
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
#endif // __GAMEGUARD
	}
}

void CDPSrvr::OnPutGoldBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpbuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	DWORD dwGold;
	BYTE nSlot;
	ar >> nSlot >> dwGold;
	
	int nGold = dwGold;
	if( nGold <= 0 )
		return;

	if( nSlot >= 3 )
		return;

	if( IsValidObj( pUser ) )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU

#ifdef __BLOCK_STAFF_ACTIONS
		if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
		{
			pUser->AddText("[Titanium] Unauthorized Action. (Banking)", COLOR_ERROR);
			return;
		}
#endif //__BLOCK_STAFF_ACTIONS

		if( !pUser->m_bInstantBank )
		{
			if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_BANKING, pUser->GetWorld(), pUser->GetPos() ) 
#ifdef __PREMIUM
				&& !pUser->IsPremium() && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)
#endif // __PREMIUM
				)
				return;
		}
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE
		if( pUser->m_nSlot == nSlot || ( pUser->m_idPlayerBank[nSlot] != 0 && pUser->IsCommBank() ) )
		{
			if( nGold > pUser->GetGold() )
				nGold = pUser->GetGold();
			
			// 은행돈이 overflow되지 않게 한다.
			if( CanAdd(pUser->m_dwGoldBank[nSlot], nGold) )
			{
				LogItemInfo aLogItem;
				aLogItem.Action = "P";
				aLogItem.SendName = pUser->GetName();
				aLogItem.RecvName = "BANK";
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = pUser->GetGold();
				aLogItem.Gold2 = pUser->GetGold() - nGold;
				//aLogItem.ItemName = "SEED";
				_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
				aLogItem.itemNumber = nGold;
				aLogItem.Gold_1 = pUser->m_dwGoldBank[pUser->m_nSlot];
				aLogItem.nSlot1 = nSlot;
				OnLogItem( aLogItem );
				
				pUser->m_dwGoldBank[nSlot] += nGold;
				pUser->AddGold( -nGold, FALSE );
				
				pUser->AddPutGoldBank( nSlot, pUser->GetGold(), pUser->m_dwGoldBank[nSlot] );
			}
		}
#ifdef __GAMEGUARD
		if (pUser->GetWorld())
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
#endif // __GAMEGUARD
	}
}

void CDPSrvr::OnGetGoldBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpbuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );

	DWORD dwGold;
	BYTE nSlot;
	ar >> nSlot >> dwGold;

	int nGold = dwGold;		
	if( nGold <= 0 )
		return;

	if( nSlot >= 3 )
		return;

	if( IsValidObj( pUser ) )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU

#ifdef __BLOCK_STAFF_ACTIONS
		if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
		{
			pUser->AddText("[Titanium] Unauthorized Action. (Banking)", COLOR_ERROR);
			return;
		}
#endif //__BLOCK_STAFF_ACTIONS

		if( !pUser->m_bInstantBank )
		{
			if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_BANKING, pUser->GetWorld(), pUser->GetPos() ) 
#ifdef __PREMIUM
				&& !pUser->IsPremium() && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)
#endif // __PREMIUM
				)
				return;
		}
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE
		if( pUser->m_nSlot == nSlot || ( pUser->m_idPlayerBank[nSlot] != 0 && pUser->IsCommBank() ) )
		{
			if( nGold > (int)( pUser->m_dwGoldBank[nSlot] ) )
				nGold = pUser->m_dwGoldBank[nSlot];

			if( CanAdd(pUser->GetGold(), nGold) )
			{
				LogItemInfo aLogItem;
				aLogItem.Action = "G";
				aLogItem.SendName = "BANK";
				aLogItem.RecvName = pUser->GetName();
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = pUser->GetGold();
				aLogItem.Gold2 = pUser->GetGold() + nGold;
				//aLogItem.ItemName = "SEED";
				_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
				aLogItem.itemNumber = nGold;
				aLogItem.Gold_1 = pUser->m_dwGoldBank[pUser->m_nSlot];
				aLogItem.nSlot = nSlot;
				OnLogItem( aLogItem );

				pUser->AddGold( nGold, FALSE );
				pUser->m_dwGoldBank[nSlot] -= nGold;
				pUser->AddPutGoldBank( nSlot, pUser->GetGold(), pUser->m_dwGoldBank[nSlot] );		
			}
		}
#ifdef __GAMEGUARD
		if (pUser->GetWorld())
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
#endif // __GAMEGUARD
	}
}

void CDPSrvr::OnMoveBankItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
}

void CDPSrvr::OnChangeBankPass( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	char szLastPass[10] ={0,};
	char szNewPass[10] ={0,};
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		ar.ReadString( szLastPass, 10 );
		ar.ReadString( szNewPass, 10 );

		if( strlen( szLastPass ) > 4 || strlen( szNewPass ) > 4 )
		{
			WriteError( "%s %d, %s, %s", __FILE__, __LINE__, szLastPass, szNewPass );
			return;
		}

		DWORD dwId, dwItemId;
		ar >> dwId >> dwItemId;

		// 여기서 비밀번호 확인작업
		if( 0 == strcmp( szLastPass, pUser->m_szBankPass ) )
		{
			// 패스워드가 바꿨으므로 DB와 클라이언트에 게 바뀠다고 보내줌
			strcpy( pUser->m_szBankPass, szNewPass );
			g_dpDBClient.SendChangeBankPass( pUser->GetName(), szNewPass, pUser->m_idPlayer );
			pUser->AddChangeBankPass( 1, dwId, dwItemId );
		}
		else
		{
			// 다시 입력하라고 알려줌
			// 패스워드가 틀렸음
			pUser->AddChangeBankPass( 0, dwId, dwItemId );
		}
	}
}

void CDPSrvr::OnConfirmBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	char szPass[10] ={0,};
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->m_vtInfo.GetOther() )
			return;
		if( pUser->m_vtInfo.VendorIsVendor() )
			return;
		if( pUser->m_bGuildBank )
			return;
#ifdef __S_SERVER_UNIFY
		if( pUser->m_bAllAction == FALSE )
			return;
#endif // __S_SERVER_UNIFY
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE

		// 여기서 비밀번호 확인작업
		ar.ReadString( szPass, 10 );

		DWORD dwId, dwItemId;
		ar >> dwId >> dwItemId;

		if( dwId == NULL_ID && !CNpcChecker::GetInstance()->IsCloseNpc( MMI_BANKING, pUser->GetWorld(), pUser->GetPos() ) 
#ifdef __PREMIUM
			&& !pUser->IsPremium() && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)
#endif // __PREMIUM
			)
				return;

		if( 0 == strcmp( szPass, pUser->m_szBankPass ) )
		{
			// 비밀번호를 확인 하였으므로 은행을 열수 잇게 해줌
			if( dwId != NULL_ID )
			{
				CItemElem* pItemElem = pUser->m_Inventory.GetAtId( dwId );
				if( IsUsableItem( pItemElem ) == FALSE || pItemElem->m_dwItemId != dwItemId )
				{
					return;
				}
				else
				{
					pUser->m_bInstantBank	= TRUE;
					pUser->RemoveItem( (BYTE)( pItemElem->m_dwObjId ), 1 );
				}
			}
			pUser->m_bBank = TRUE;
			pUser->AddconfirmBankPass( 1, dwId, dwItemId );
		}
		else
		{
			// 다시 입력하라고 알려줌
			// 패스워드가 틀렸음
			pUser->AddconfirmBankPass( 0, dwId, dwItemId );
		}
	}
}

void CDPSrvr::OnSfxHit( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int idSfxHit;
	int nMagicPower;
	DWORD dwSkill;
	OBJID idAttacker;
	int	nDmgCnt;	// 일반적으론 0, 지속데미지의 경우 첫빵이후는 1이상이 넘어온다. 이경우는 데미지의 10%만 준다.
	float fDmgAngle, fDmgPower;
	PSfxHit pSfxHit		= NULL;
	CMover* pAttacker	= NULL;

	ar >> idSfxHit >> nMagicPower >> dwSkill >> idAttacker >> nDmgCnt >> fDmgAngle >> fDmgPower;		

	// idAttacker가 NULL_ID면 어태커를 dpidUser로 한다.
	if( idAttacker == NULL_ID )
		pAttacker = g_UserMng.GetUser( dpidCache, dpidUser );	
	else
		pAttacker = prj.GetMover( idAttacker );

	if( IsValidObj( pAttacker ) == FALSE ) 
		return;

	pSfxHit	= pAttacker->m_sfxHitArray.GetSfxHit( idSfxHit );
	if( pSfxHit == NULL ) 
		return;

	CMover* pTarget	= prj.GetMover( pSfxHit->objid );

	pAttacker->RemoveSFX( pSfxHit->objid, idSfxHit, ( IsInvalidObj( pTarget ) || pTarget->IsDie() ), dwSkill );
	pAttacker->m_sfxHitArray.RemoveSfxHit( idSfxHit, TRUE );	// 무조건 제거
}

// 클라로부터 받은 idSfx를 어레이에 추가시켜둠
void CDPSrvr::OnSfxID( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID	idTarget;
	int		idSfxHit;
	DWORD	dwType,	dwSkill;
	int		nMaxDmgCnt;

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		ar >> idTarget >> idSfxHit >> dwType >> dwSkill >> nMaxDmgCnt;
		pUser->m_sfxHitArray.Add( idSfxHit, idTarget, dwType, dwSkill, nMaxDmgCnt );		
	}
}

// 공격이 빗나가서 저절로 없어졌을때 삭제 명령.
void CDPSrvr::OnSfxClear( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID	idMover = NULL_ID;
	int	  idSfxHit;

	ar >> idSfxHit;
	ar >> idMover;

	CMover *pMover;

	if( idMover == NULL_ID )
		pMover = g_UserMng.GetUser( dpidCache, dpidUser );
	else
		pMover = prj.GetMover( idMover );

	if( IsValidObj( pMover ) )
	{
		PSfxHit pSfxHit	= pMover->m_sfxHitArray.GetSfxHit( idSfxHit );
		pMover->m_sfxHitArray.RemoveSfxHit( idSfxHit, TRUE );
	}
}

void CDPSrvr::OnMeleeAttack( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwAtkMsg;
	OBJID objid;
	int nParam2, nParam3;
#ifdef __HACK_1023
	FLOAT fVal;
	ar >> dwAtkMsg >> objid >> nParam2 >> nParam3 >> fVal;
#else	// __HACK_1023
	ar >> dwAtkMsg >> objid >> nParam2 >> nParam3;
#endif	// __HACK_1023

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->GetIndex() == 0 )
		{
			WriteError( "PACKETTYPE_MELEE_ATTACK" );
			return;
		}
#ifdef __HACK_1023
		ItemProp* pHandItemProp	= pUser->GetActiveHandItemProp();
		FLOAT f	= pHandItemProp? pHandItemProp->fAttackSpeed: 0.0F;
		if( f != fVal )
		{
			pUser->AddDefinedText( TID_GAME_MODIFY_ATTACK_SPEED );
			return;
		}
#endif	// __HACK_1023

		CMover* pTargetObj	= prj.GetMover( objid );
		if( IsValidObj( pTargetObj ) )
		{
#ifdef __GAMEGUARD
			if (!pTargetObj->IsNPC() && !pTargetObj->IsPlayer())
				return;
#endif // __GAMEGUARD

			int nRet = pUser->SendActMsg( (OBJMSG)dwAtkMsg, objid, nParam2, nParam3 );
			
			if( nRet == 0 && pUser->IsFly() == FALSE )
#ifdef __OPT_MEM_0811
				pUser->m_pActMover->m_qMeleeAtkMsg.AddTail( new ACTMSG( dwAtkMsg, objid, nParam2, nParam3 ) );
#else	// __OPT_MEM_0811
				pUser->m_pActMover->m_qMeleeAtkMsg.AddTail( ACTMSG( dwAtkMsg, objid, nParam2, nParam3 ) );
#endif	// __OPT_MEM_0811
			if( nRet != -2 )	// -2는 명령 완전 무시.
			{
				g_UserMng.AddMeleeAttack( pUser, dwAtkMsg, objid, nParam2, nParam3 );
			}
		}
	}
}

void CDPSrvr::OnMeleeAttack2( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwAtkMsg;
	OBJID objid;
	int nParam2, nParam3;
	ar >> dwAtkMsg >> objid >> nParam2 >> nParam3;
	
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->GetIndex() == 0 )
		{
			WriteError( "PACKETTYPE_MELEE_ATTACK2" );
			return;
		}

		CMover* pTargetObj	= prj.GetMover( objid );
		if( IsValidObj( pTargetObj ) )
		{		
#ifdef __GAMEGUARD
			if (!pTargetObj->IsNPC() && !pTargetObj->IsPlayer())
				return;
#endif // __GAMEGUARD

			int nRet = pUser->SendActMsg( (OBJMSG)dwAtkMsg, objid, nParam2, nParam3 );
			if( nRet == 0 )
#ifdef __OPT_MEM_0811
				pUser->m_pActMover->m_qMeleeAtkMsg.AddTail( new ACTMSG( dwAtkMsg, objid, nParam2, nParam3 ) );
#else	// __OPT_MEM_0811
				pUser->m_pActMover->m_qMeleeAtkMsg.AddTail( ACTMSG( dwAtkMsg, objid, nParam2, nParam3 ) );
#endif	// __OPT_MEM_0811
			
			if( nRet != -2 )	// -2는 명령 완전 무시.
				g_UserMng.AddMeleeAttack2( pUser, dwAtkMsg, objid, nParam2, nParam3 );
		}
	}
}


void CDPSrvr::OnMagicAttack( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwAtkMsg;
	OBJID objid;
	int nParam2, nParam3, nMagicPower, idSfxHit;
	ar >> dwAtkMsg >> objid >> nParam2 >> nParam3 >> nMagicPower >> idSfxHit;

	nParam2 = 0;		//  m_qMagicAtkMsg에서 nParam2가 0이면 range attack으로 간주된다.

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CMover* pTargetObj	= prj.GetMover( objid );

		if( IsValidObj( pTargetObj ) ) 
		{
			int nRet = pUser->DoAttackMagic( pTargetObj, nMagicPower, idSfxHit );
			
			if( nRet == 0 && pUser->IsFly() == FALSE )
#ifdef __OPT_MEM_0811
				pUser->m_pActMover->m_qMagicAtkMsg.AddTail( new MAGICATKMSG( dwAtkMsg, objid, nParam2, nParam3, nMagicPower, idSfxHit ) );
#else	// __OPT_MEM_0811
				pUser->m_pActMover->m_qMagicAtkMsg.AddTail( (OBJMSG)dwAtkMsg, objid, nParam2, nParam3, nMagicPower, idSfxHit );
#endif	// __OPT_MEM_0811
		}
	}
}

void CDPSrvr::OnRangeAttack( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{	
	DWORD dwAtkMsg;
	OBJID objid;
	DWORD dwItemID;
	int idSfxHit;

	ar >> dwAtkMsg >> objid >> dwItemID >> idSfxHit;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CMover* pTargetObj	= prj.GetMover( objid );

		if( IsValidObj( pTargetObj ) ) 
		{
#ifdef __GAMEGUARD
			if (!pTargetObj->IsNPC() && !pTargetObj->IsPlayer())
				return;
#endif // __GAMEGUARD

			if( pUser->DoAttackRange( pTargetObj, dwItemID, idSfxHit ) == 0 )
#ifdef __OPT_MEM_0811
				pUser->m_pActMover->m_qMagicAtkMsg.AddTail( new MAGICATKMSG( dwAtkMsg, objid, 1, dwItemID, 0, idSfxHit ) );
#else	// __OPT_MEM_0811
				pUser->m_pActMover->m_qMagicAtkMsg.AddTail( (OBJMSG)dwAtkMsg, objid, 1, dwItemID, 0, idSfxHit );
#endif	// __OPT_MEM_0811
		}
	}
}

void CDPSrvr::OnTeleSkill( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
//	OBJID objid;
	D3DXVECTOR3 vPos;
	ar >> vPos;
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		LPSKILL pSkill	= pUser->GetSkill( SI_MAG_MAG_BLINKPOOL );
		if( pSkill == NULL || pSkill->dwLevel == 0 )
			return;
		// 현재 위치 - 클락워크 지역 x, 대상 위치 - 클락워크 지역 o = 불가
//		if( prj.IsGuildQuestRegion( pUser->GetPos() ) == FALSE && prj.IsGuildQuestRegion( vPos ) == TRUE )
		D3DXVECTOR3 v	= pUser->GetPos();
		if( prj.IsGuildQuestRegion( v ) == TRUE || prj.IsGuildQuestRegion( vPos ) )		// 현 좌표, 혹은 대상 좌표가 클락워크 지역이다.
		{
			CWorld* pWorld	= pUser->GetWorld();
			if( !pWorld )
				return;
			if( fabs( pWorld->GetLandHeight( v ) - v.y ) > 1.0F || fabs( pWorld->GetLandHeight( vPos ) - vPos.y ) > 1.0F )
				return;
		}
		pUser->REPLACE( g_uIdofMulti, pUser->GetWorld()->GetID(), vPos, REPLACE_NORMAL, pUser->GetLayer() );
	}
}

#ifdef __STOP_EXP
void CDPSrvr::OnSetExpStop(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BOOL bState;
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		ar >> bState;
		pUser->SetExpStop(bState);
		pUser->AddExpStopState(bState);
	}
}
#endif //__STOP_EXP

void CDPSrvr::OnSetTarget( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID idTarget;
	BYTE bClear;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		ar >> idTarget >> bClear;	// idTarget은 MOVER라고 가정하자.

		if( bClear == 2 )		// 2 : 타겟잡은놈을 기억.
			pUser->m_idSetTarget = idTarget;
		if( bClear < 2 )		// 0 / 1 : 타겟잡은놈에게 나를 기록 / 타겟잡은놈에게서 나를 지움.
		{
			CMover *pTarget = prj.GetMover( idTarget );		// 타겟의 포인터
			if( IsValidObj( pTarget ) )
			{
#ifdef __GAMEGUARD
				if (!pTarget->IsNPC() && !pTarget->IsPlayer())
				{
					pUser->m_idSetTarget = NULL_ID;
					return;
				}
#endif // __GAMEGUARD
				if( bClear )	// 타겟이 해제됬�?
				{
					if( pTarget->m_idTargeter == pUser->GetId() )	// 자기가 잡았던 타겟만 자기가 풀수있다.
					{
						pTarget->m_idTargeter = NULL_ID;
#if __VER >= 10 // __LEGEND
						DWORD	dwTmpTick = GetTickCount();
						int	nTmpSkillID = pUser->m_pActMover->GetCastingSKillID();
						if( pUser->m_pActMover->GetCastingEndTick() > dwTmpTick && ( nTmpSkillID == SI_KNT_HERO_DRAWING 
#ifndef __AEGON_RM_HERO_SKILL
							|| nTmpSkillID == SI_RIG_HERO_RETURN 
#endif
							) )
						{
							pUser->m_pActMover->SetCastingEndTick(0);
							pUser->m_pActMover->ClearState();				// 상태 클리어하고 다시 맞춤.
						}
#endif  //#if __VER >= 10 // __LEGEND
					}
				}
				else
				{
					if( pTarget->m_idTargeter == NULL_ID )			// 타겟잡은 사람이 없을때만 타게터를 박을 수 있다. 0819
						pTarget->m_idTargeter = pUser->GetId();		// pUser가 타겟을 잡았다.
				}
			}
		}
	}
}


void CDPSrvr::OnSnapshot( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE c;
	WORD	wHdr;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		ar >> c;
		while( c-- )
		{
			ar >> wHdr;
#ifdef __SYS_PLAYER_SYNC
			OnPlayerDestPos(ar, pUser);
#else // __SYS_PLAYER_SYNC
			switch( wHdr )
			{
				case SNAPSHOTTYPE_DESTPOS:	OnPlayerDestPos( ar, pUser );	break;
				default:
					{
						// handler not found
						ASSERT( 0 );
						break;
					}
			}
#endif // __SYS_PLAYER_SYNC
		}
	}
}

void CDPSrvr::OnPlayerDestPos( CAr & ar, CUser* pUser )
{
	D3DXVECTOR3 vPos;
	BYTE fForward;
	ar >> vPos >> fForward;

	D3DXVECTOR3 vDistance	= pUser->GetPos() - vPos;
	if( D3DXVec3LengthSq( &vDistance ) > 1000000.0F )
	{
//		Error( "SNAPSHOTTYPE_DESTPOS" );
		return;
	}

#ifdef __IAOBJ0622
	CShip* pIAObj	= NULL;
	OBJID objidIAObj;
	ar >> objidIAObj;

	if( objidIAObj != NULL_ID )
	{
		pIAObj	= (CShip*)prj.GetCtrl( objidIAObj );
		if( IsValidObj( pIAObj ) == FALSE )
		{
			Error( "CShip object not found" );
			return;
		}
	}
#endif	// __IAOBJ0622

	bool bForward = (fForward != 0);

	pUser->m_pActMover->DefaultSet();
	pUser->BehaviorActionForceSet();	// 명령이 행동 명령이면 강제 동기하고, 무빙 명령이면 무시한다.

#ifdef __IAOBJ0622
	if( pIAObj )
		pUser->SetDestPos( pIAObj, vPos );
	else
		pUser->SetDestPos( vPos, bForward );
#else	// __IAOBJ0622
	pUser->SetDestPos( vPos, bForward );
#endif	// __IAOBJ0622

#ifdef __IAOBJ0622
	g_UserMng.AddSetDestPos( pUser, vPos, bForward, objidIAObj );
#else	// __IAOBJ0622
	g_UserMng.AddSetDestPos( pUser, vPos, bForward );
#endif	// __IAOBJ0622

#ifdef __AFK_MONITORING
	if (pUser->m_dwAFKState == 3)
	{
		pUser->m_dwAFKState = NULL;
		g_UserMng.AddMessiState(pUser);
	}
	pUser->m_dwLastWalkTick[0] = GetTickCount() + MIN(MIN_AUTO_AFK);
#endif //__AFK_MONITORING
}

void CDPSrvr::OnModifyMode( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUserTo	= g_UserMng.GetUser( dpidCache, dpidUser );

	CUser* pUserFrom = NULL;

	if( IsValidObj(pUserTo) )
	{
		DWORD dwMode;
		BYTE f;
		u_long idFrom;

		ar >> dwMode >> f >> idFrom;

		pUserFrom = g_UserMng.GetUserByPlayerID(idFrom);
		if (IsInvalidObj(pUserFrom))
			return;

#ifdef __HACK_0516
		DPID dpid;
		ar >> dpid;
		if(pUserTo->m_Snapshot.dpidUser != dpid )
		{
			Error("Invalid dpid of pUserTo Name: %s", pUserTo->GetName());
			return;
		}
		
		if( !pUserFrom->IsAuthHigher(AUTH_GAMEMASTER) )
        {
			Error("OnModifyMode auth too low. Name: %s idFrom: %lu", pUserFrom->GetName(), pUserFrom->m_idPlayer);
            return;
        }
#endif	// __HACK_0516

#ifdef __GAMEGUARD
		if (!pUserTo->IsAuthHigher(AUTH_GAMEMASTER))
		{
#ifdef __AEGON_NINJA_MODE
			if (pUserTo->m_dwMode & (AEGON_NINJA_MODE))
			{
				pUserTo->SetNotMode(AEGON_NINJA_MODE);
				g_UserMng.AddModifyMode(pUserTo);
				return;
			}
#endif

#ifdef __AEGON_STAFF_MOB_IGNORE_MODE
			if (pUserTo->m_dwMode & (AEGON_MOB_IGNORE_MODE))
			{
				pUserTo->SetNotMode(AEGON_MOB_IGNORE_MODE);
				g_UserMng.AddModifyMode(pUserTo);
				return;
			}
#endif

			if (pUserTo->m_dwMode & (TRANSPARENT_MODE))
			{
				pUserTo->SetNotMode(TRANSPARENT_MODE);
				g_UserMng.AddModifyMode(pUserTo);
				return;
			}
			if (pUserTo->m_dwMode & (EVENT_OLDBOY_MODE | QUERYSETPLAYERNAME_MODE))
			{
				pUserTo->SetNotMode(EVENT_OLDBOY_MODE | QUERYSETPLAYERNAME_MODE);
				g_UserMng.AddModifyMode(pUserTo);
				return;
			}
		}

		if (!pUserTo->IsAuthHigher(AUTH_GAMEMASTER2))
		{
			if (pUserTo->m_dwMode & (MATCHLESS_MODE | ONEKILL_MODE | MATCHLESS2_MODE))
			{
				pUserTo->SetNotMode(MATCHLESS_MODE | ONEKILL_MODE | MATCHLESS2_MODE);
				g_UserMng.AddModifyMode(pUserTo);
				return;
			}
		}
#endif // __GAMEGUARD

		if( f )
			pUserTo->m_dwMode		|= dwMode;
		else
			pUserTo->m_dwMode		&= ~dwMode;

		g_UserMng.AddModifyMode(pUserTo);
	}
}

// 운영자의 소환 명령어 
void CDPSrvr::OnSummonPlayer( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
#ifdef __SECURITY_FIXES
	return;
#else // __SECURITY_FIXES
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		u_long idOperator;
		DWORD dwWorldID;
		D3DXVECTOR3 vPos;
		u_long uIdofMulti;

		ar >> idOperator;
		ar >> dwWorldID;
		ar >> vPos;
		ar >> uIdofMulti;

#ifdef __HACK_0516
		DPID dpid;
		ar >> dpid;
		if( pUser->m_Snapshot.dpidUser != dpid )
		{
			Error( "[%s] try to hack : PACKETTYPE_SUMMONPLAYER", pUser->GetName() );
			return;
		}
#endif	// __HACK_0516
		if( !pUser->IsAuthHigher(AUTH_GAMEMASTER) )
		{
            Error( "[%s] try to hack with dpid : PACKETTYPE_SUMMONPLAYER", pUser->GetName() );
            return;
		}
#ifdef __LAYER_1015
		int nLayer;
		ar >> nLayer;
#endif	// __LAYER_1015

		if( !pUser->GetWorld() )
		{
			WriteError( "PACKETTYPE_SUMMONPLAYER//1" );
			return;
		}

		pUser->REPLACE( uIdofMulti, dwWorldID, vPos, REPLACE_FORCE, nLayer );
	}
#endif // __SECURITY_FIXES
}

void CDPSrvr::OnTeleportPlayer( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
#ifdef __SECURITY_FIXES
	return;
#else // __SECURITY_FIXES
	u_long idOperator;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	
	CWorld* pWorld;
	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) )
	{
		ar >> idOperator;
#ifdef __HACK_0516
		DPID dpid;
		ar >> dpid;
		if( pUser->m_Snapshot.dpidUser != dpid )
		{
			Error( "[%s] try to hack : PACKETTYPE_TELEPORTPLAYER", pUser->GetName() );
			return;
		}
		if( !pUser->IsAuthHigher(AUTH_GAMEMASTER) )
        {
            Error( "[%s] try to hack with dpid : PACKETTYPE_TELEPORTPLAYER", pUser->GetName() );
            return;
        }
#endif	// __HACK_0516
#ifdef __SECURITY_FIXES
		if( !pUser->IsAuthHigher(AUTH_GAMEMASTER) )
		{
            Error( "[%s] try to hack with dpid : PACKETTYPE_TELEPORTPLAYER", pUser->GetName() );
            return;
		}
#endif // __SECURITY_FIXES
#ifdef __LAYER_1015
		g_DPCoreClient.SendSummonPlayer( pUser->m_idPlayer, pWorld->GetID(), pUser->GetPos(), idOperator, pUser->GetLayer() );
#else	// __LAYER_1015
		g_DPCoreClient.SendSummonPlayer( pUser->m_idPlayer, pWorld->GetID(), pUser->GetPos(), idOperator );
#endif	// __LAYER_1015
	}
#endif // __SECURITY_FIXES
}

void CDPSrvr::OnChangeFace( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long objId;
	DWORD dwFaceNum;
	int cost;

#ifdef __NEWYEARDAY_EVENT_COUPON
	BOOL bUseCoupon;
#ifdef __GAMEGUARD
	ar >> objId >> dwFaceNum >> bUseCoupon;
#else // __GAMEGUARD
	ar >> objId >> dwFaceNum >> cost >> bUseCoupon;
#endif // __GAMEGUARD
#else // __NEWYEARDAY_EVENT_COUPON
#ifdef __GAMEGUARD
	ar >> objId >> dwFaceNum;
#else // __GAMEGUARD
	ar >> objId >> dwFaceNum >> cost;
#endif // __GAMEGUARD
#endif // __NEWYEARDAY_EVENT_COUPON
	
#ifdef __GAMEGUARD
	if (dwFaceNum >= MAX_HEAD || dwFaceNum < 0)
		return;
#endif // __GAMEGUARD

	cost = CHANGE_FACE_COST;
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{	
#ifdef __NEWYEARDAY_EVENT_COUPON
		if(!bUseCoupon)
		{
#ifdef __GAMEGUARD
			if (pUser->GetGold() < CHANGE_FACE_COST)
#else // __GAMEGUARD
			if (pUser->GetGold() < cost)
#endif // __GAMEGUARD
			{
				pUser->AddDefinedText(TID_GAME_LACKMONEY, "");
				return;
			}
#ifdef __GAMEGUARD
			if (CHANGE_FACE_COST < 0)
				return;

			pUser->AddGold(-(CHANGE_FACE_COST));
#else // __GAMEGUARD
			pUser->AddGold(-(cost));
#endif // __GAMEGUARD
		}
		else
		{
			CItemElem* pItemElem = NULL;
			pItemElem = pUser->m_Inventory.GetAtItemId( II_SYS_SYS_SCR_FACEOFFFREE );
			if( IsUsableItem( pItemElem ) == FALSE )
			{
				pUser->AddDefinedText( TID_GAME_WARNNING_COUPON, "" );
				return;
			}
			pUser->UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_NUM, pItemElem->m_nItemNum - 1 );
		}
		pUser->SetHead(dwFaceNum);
#else //__NEWYEARDAY_EVENT_COUPON
#ifdef __GAMEGUARD
		if (pUser->GetGold() < CHANGE_FACE_COST)
#else // __GAMEGUARD
		if (pUser->GetGold() < cost)
#endif // __GAMEGUARD
		{
			pUser->AddDefinedText(TID_GAME_LACKMONEY, "");
			return;
	}
		pUser->SetHead(dwFaceNum);
#ifdef __GAMEGUARD
		pUser->AddGold(-(CHANGE_FACE_COST));
#else // __GAMEGUARD
		pUser->AddGold(-(cost));
#endif // __GAMEGUARD
#endif //__NEWYEARDAY_EVENT_COUPON
		g_UserMng.AddChangeFace( objId, dwFaceNum );

#ifdef __GAMEGUARD
		if (pUser->GetWorld())
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
#endif // __GAMEGUARD
	}
}

void CDPSrvr::OnExpUp( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{	
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		//if( (DWORD)AUTH_GAMEMASTER <= pUser->m_dwAuthorization )
		if(pUser->IsAuthHigher(AUTH_GAMEMASTER2))
		{
			EXPINTEGER nExp;
			ar >> nExp;

			if (pUser->AddExperience(nExp, TRUE, TRUE, TRUE)) 
			{
				pUser->LevelUpSetting();
			}
			else 
			{
				pUser->ExpUpSetting();
			}
				
			pUser->AddSetExperience( pUser->GetExp1(), (WORD)pUser->m_nLevel, pUser->m_nSkillPoint, pUser->m_nSkillLevel );
		}
	}
}

void	CDPSrvr::OnChangeJob( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
}

void	CDPSrvr::OnLogItem( LogItemInfo & info, CItemElem* pItemElem, int nItemCount )
{
	if( pItemElem != NULL )
	{
		info.ItemNo = pItemElem->GetSerialNumber();
		//info.ItemName = pItemElem->GetProp()->szName;
		_stprintf( info.szItemName, "%d", pItemElem->GetProp()->dwID );
		info.itemNumber = nItemCount;
		info.nItemResist = pItemElem->m_bItemResist;
		info.nResistAbilityOption = pItemElem->m_nResistAbilityOption;
		info.nAbilityOption = pItemElem->GetAbilityOption();
		info.Negudo = pItemElem->m_nHitPoint;
		info.MaxNegudo	= pItemElem->m_nRepair;
		info.m_bCharged = pItemElem->m_bCharged;
		info.m_dwKeepTime = pItemElem->m_dwKeepTime;
#if __VER >= 12 // __EXT_PIERCING
		info.nPiercedSize = pItemElem->GetPiercingSize();
		for( int i=0; i<pItemElem->GetPiercingSize(); i++ )
			info.adwItemId[i] = pItemElem->GetPiercingItem( i );
		info.nUMPiercedSize = pItemElem->GetUltimatePiercingSize();
		for( int i=0; i<pItemElem->GetUltimatePiercingSize(); i++ )
			info.adwUMItemId[i] = pItemElem->GetUltimatePiercingItem( i );
#else // __EXT_PIERCING
		info.nPiercedSize = pItemElem->GetPiercingSize();
		info.adwItemId0 = pItemElem->GetPiercingItem( 0 );
		info.adwItemId1 = pItemElem->GetPiercingItem( 1 );
		info.adwItemId2 = pItemElem->GetPiercingItem( 2 );
		info.adwItemId3 = pItemElem->GetPiercingItem( 3 );
#if __VER >= 9 // __ULTIMATE
		info.adwItemId4 = pItemElem->GetPiercingItem( 4 );
#endif // __ULTIMATE
#endif // __EXT_PIERCING
#if __VER >= 11 // __SYS_IDENTIFY
		info.m_iRandomOptItemId = pItemElem->GetRandomOptItemId();
#else	// __SYS_IDENTIFY
		info.m_nRandomOptItemId = pItemElem->GetRandomOptItemId();
#endif	// __SYS_IDENTIFY
#ifdef __AEGON_RETURN_AWAKE
		info.m_iOldRandomOptItemId = pItemElem->GetOldRandomOptItemId();
#endif // _AEGON_RETURN_AWAKE
#if __VER >= 9 // __PET_0410
		if( pItemElem->m_pPet )
		{
			CPet* pPet = pItemElem->m_pPet;

			info.nPetKind = pPet->GetKind();
			info.nPetLevel = pPet->GetLevel();
			info.dwPetExp = pPet->GetExp();
			info.wPetEnergy = pPet->GetEnergy();
			info.wPetLife = pPet->GetLife();
			info.nPetAL_D = pPet->GetAvailLevel( PL_D );
			info.nPetAL_C = pPet->GetAvailLevel( PL_C );
			info.nPetAL_B = pPet->GetAvailLevel( PL_B );
			info.nPetAL_A = pPet->GetAvailLevel( PL_A );
			info.nPetAL_S = pPet->GetAvailLevel( PL_S );
		}
		// mirchang_100514 TransformVisPet_Log
#if __VER >= 15 // __PETVIS
		else if( pItemElem->IsTransformVisPet() == TRUE )
		{
			info.nPetKind = (BYTE)100;
		}
#endif // __PETVIS
		// mirchang_100514 TransformVisPet_Log
#endif // __PET_0410

#ifdef __UPGRADE_SUCCESS
		info.nUpgradeTries = pItemElem->m_nUpgradeTries;
#endif // __UPGRADE_SUCCESS

	}

	BEFORESENDDUAL( ar, PACKETTYPE_LOG_ALLITEM, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.WriteString( info.Action );
	ar.WriteString( info.SendName );
	ar.WriteString( info.RecvName );
	ar << info.WorldId;
	ar << info.Gold;
	ar << info.Gold2;
	ar << info.ItemNo; // 아이템 고유번호
	ar << info.Negudo; // 내구도 
	ar << info.MaxNegudo; // 내구도 
	if( _tcslen( info.szItemName ) == 0 )
	{
		_stprintf( info.szItemName, "%d", -1 );
	}
	ar.WriteString( info.szItemName );
	ar << info.itemNumber;
	ar << info.nAbilityOption;
	ar << info.Gold_1;
	ar << info.nSlot;
	ar << info.nSlot1;
	ar << info.nItemResist;
	ar << info.nResistAbilityOption;
	ar << info.m_bCharged;
	ar << info.m_dwKeepTime;
#if __VER >= 12 // __EXT_PIERCING
	ar << info.nPiercedSize;
	for( int i=0; i<info.nPiercedSize; i++ )
		ar << info.adwItemId[i];
	ar << info.nUMPiercedSize;
	for( int i=0; i<info.nUMPiercedSize; i++ )
		ar << info.adwUMItemId[i];
#else // __EXT_PIERCING
	ar << info.nPiercedSize;
	ar << info.adwItemId0;
	ar << info.adwItemId1;
	ar << info.adwItemId2;
	ar << info.adwItemId3;
#if __VER >= 9 // __ULTIMATE
	ar << info.adwItemId4;
#endif // __ULTIMATE
#endif // __EXT_PIERCING
#if __VER >= 11 // __SYS_IDENTIFY
	ar << info.m_iRandomOptItemId;
#else	// __SYS_IDENTIFY
	ar << info.m_nRandomOptItemId;
#endif	// __SYS_IDENTIFY
#if __VER >= 9 // __PET_0410
	ar << info.nPetKind;
	ar << info.nPetLevel;
	ar << info.dwPetExp;
	ar << info.wPetEnergy;
	ar << info.wPetLife;
	ar << info.nPetAL_D;
	ar << info.nPetAL_C;
	ar << info.nPetAL_B;
	ar << info.nPetAL_A;
	ar << info.nPetAL_S;
#endif // __PET_0410
	
#ifdef __UPGRADE_SUCCESS
	ar << info.nUpgradeTries;
#endif // __UPGRADE_SUCCESS

	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
}


void CDPSrvr::OnMotion( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->GetIndex() == 0 )
		{
			WriteError( "PACKETTYPE_MOTION" );
			return;
		}

		DWORD dwMsg;
		ar >> dwMsg;

		if( pUser->SendActMsg( (OBJMSG)dwMsg ) == 1 )
		{
//#ifdef __JEFF_VER_8
//			if( dwMsg != OBJMSG_STANDUP )
//#endif	// __JEFF_VER_8
			{
				pUser->ClearDest();
				pUser->ClearDestAngle();
				memset( &pUser->m_CorrAction, 0, sizeof(CORR_ACTION) );
			}
			g_UserMng.AddMotion( pUser, dwMsg );
		}
		else
			pUser->AddMotionError();
	}
}

void CDPSrvr::OnRepairItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		BYTE c, nId;
		ar >> c;

		if( c == 0 || c > MAX_REPAIRINGITEM )
			return;

		CItemElem* apItemRepair[MAX_REPAIRINGITEM];
		int		anRepair[MAX_REPAIRINGITEM];
		memset( anRepair, 0, sizeof(int) * MAX_REPAIRINGITEM );

		CItemElem* pItemElem;
		int nCost	= 0;
		for( int i = 0; i < c; i++ )
		{
			apItemRepair[i]		= NULL;
			ar >> nId;
			pItemElem	= pUser->m_Inventory.GetAtId( nId );
			if( pItemElem )
			{
				ItemProp* pItemProp	= pItemElem->GetProp();
				if( pItemProp && pItemProp->dwItemKind2 >= IK2_WEAPON_HAND && pItemProp->dwItemKind2 <= IK2_ARMORETC && 
					pItemElem->m_nHitPoint < (int)( pItemProp->dwEndurance ) && pItemElem->m_nRepair < pItemProp->nMaxRepair )
				{
					int nRepair	= 100 - ( ( pItemElem->m_nHitPoint * 100 ) / pItemProp->dwEndurance );
					if( nRepair == 0 )
						continue;
					nCost	+= nRepair * ( pItemProp->dwCost / 1000 + 1 );
					apItemRepair[i]		= pItemElem;
					anRepair[i]		= nRepair;
				}
			}
		}

		if( pUser->GetGold() < nCost )
			return;

		for( int i = 0; i < c; i++ )
		{
			if( apItemRepair[i] )
			{
				pUser->UpdateItem( (BYTE)apItemRepair[i]->m_dwObjId, UI_HP, apItemRepair[i]->GetProp()->dwEndurance );
				pUser->UpdateItem( (BYTE)apItemRepair[i]->m_dwObjId, UI_RN, apItemRepair[i]->m_nRepairNumber + anRepair[i] );
			}
		}
#if __VER < 11 // __REMOVE_ENDURANCE
		pUser->AddGold( -nCost );
#endif // __REMOVE_ENDURANCE
		pUser->AddDefinedText( TID_GAME_REPAIRITEM );
	}
}

void CDPSrvr::OnSetHair( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE nHair, nR, nG, nB;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __NEWYEARDAY_EVENT_COUPON
		BOOL bUseCoupon;
		ar >> nHair >> nR >> nG >> nB >> bUseCoupon;
#else //__NEWYEARDAY_EVENT_COUPON
		ar >> nHair >> nR >> nG >> nB;// >> nCost;
#endif //__NEWYEARDAY_EVENT_COUPON

		int nCost;
	
		nCost	= CMover::GetHairCost( (CMover*)pUser, nR, nG, nB, nHair );

#ifdef __NEWYEARDAY_EVENT_COUPON
		if( pUser->GetGold() < nCost  && !bUseCoupon)
#else //__NEWYEARDAY_EVENT_COUPON
		if( pUser->GetGold() < nCost )
#endif //__NEWYEARDAY_EVENT_COUPON
		{
			pUser->AddDefinedText( TID_GAME_LACKMONEY, "" );
			return;
		}

		pUser->SetHair( nHair );
		float r, g, b;
		r	= (float)nR / 255.0f;
		g	= (float)nG / 255.0f;
		b	= (float)nB / 255.0f;

#ifdef __NEWYEARDAY_EVENT_COUPON
		if(!bUseCoupon)
			pUser->AddGold( -( nCost ) );
		else
		{
			CItemElem* pItemElem = NULL;
			pItemElem = pUser->m_Inventory.GetAtItemId( II_SYS_SYS_SCR_HAIRCHANGE );
			if( IsUsableItem( pItemElem ) == FALSE )
			{
				pUser->AddDefinedText( TID_GAME_WARNNING_COUPON, "" );
				return;
			}
			pUser->UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_NUM, pItemElem->m_nItemNum - 1 );
		}
#else //__NEWYEARDAY_EVENT_COUPON
		pUser->AddGold( -( nCost ) );
#endif //__NEWYEARDAY_EVENT_COUPON

		pUser->SetHairColor( r, g, b );
		g_UserMng.AddSetHair( pUser, nHair, nR, nG, nB );
	}
}

void CDPSrvr::OnBlock( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __GAMEGUARD
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;
#endif // __GAMEGUARD
	BYTE nGu;
	u_long uidPlayerTo, uidPlayerFrom;
	char szNameTo[MAX_NAME] = {0,};
	char szNameFrom[MAX_NAME] = {0,};
	ar >> nGu;
	ar.ReadString( szNameTo, MAX_NAME );
	ar.ReadString( szNameFrom, MAX_NAME );

#if __VER >= 11 // __SYS_PLAYER_DATA
	uidPlayerTo	 = CPlayerDataCenter::GetInstance()->GetPlayerId( szNameTo );
	uidPlayerFrom	= CPlayerDataCenter::GetInstance()->GetPlayerId( szNameFrom );
#else	// __SYS_PLAYER_DATA
	uidPlayerTo = prj.GetPlayerID( szNameTo );
	uidPlayerFrom = prj.GetPlayerID( szNameFrom );
#endif	// __SYS_PLAYER_DATA

#ifdef __SECURITY_FIXES
	CUser* pUserTo = (CUser*)prj.GetUserByID(uidPlayerTo);
	CUser* pUserFrom = (CUser*)prj.GetUserByID(uidPlayerFrom);

	if (!pUserTo || !pUserFrom)
		return;

	if (pUserTo->m_idPlayer == pUserFrom->m_idPlayer)
		return;
#endif // __SECURITY_FIXES

	if( uidPlayerTo > 0 && uidPlayerFrom > 0 )
		g_DPCoreClient.SendBlock( nGu, uidPlayerTo, szNameTo, uidPlayerFrom );
}

#if __VER >= 12 // __J12_0
DWORD WhatEleCard( DWORD dwItemType )
{	
	switch( dwItemType )
	{
		case SAI79::FIRE:
			return II_GEN_MAT_ELE_FLAME;
		case SAI79::WATER:
			return II_GEN_MAT_ELE_RIVER;
		case SAI79::ELECTRICITY:
			return II_GEN_MAT_ELE_GENERATOR;
		case SAI79::EARTH:
			return II_GEN_MAT_ELE_DESERT;
		case SAI79::WIND:
			return II_GEN_MAT_ELE_CYCLON;
		default:
			return 0;
	}
}
#else	// __J12_0
DWORD WhatEleCard( DWORD dwLV, DWORD dwItemType )
{
	switch( dwItemType )
	{
		case SAI79::FIRE:
			return ( dwLV <= 4 ) ? II_GEN_MAT_ELE_TOUCH: II_GEN_MAT_ELE_FLAME;
		case SAI79::WATER:
			return ( dwLV <= 4 ) ? II_GEN_MAT_ELE_LAKE: II_GEN_MAT_ELE_RIVER;
		case SAI79::ELECTRICITY:
			return ( dwLV <= 4 ) ? II_GEN_MAT_ELE_VOLTAGE: II_GEN_MAT_ELE_GENERATOR;
		case SAI79::EARTH:
			return ( dwLV <= 4 ) ? II_GEN_MAT_ELE_STONE: II_GEN_MAT_ELE_DESERT;
		case SAI79::WIND:
			return ( dwLV <= 4 ) ? II_GEN_MAT_ELE_GALE: II_GEN_MAT_ELE_CYCLON;
		default:
			return 0;
	}
}
#endif	// __J12_0

void CDPSrvr::OnPiercingSize( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwId1, dwId2, dwId3;
	
	ar >> dwId1;
	ar >> dwId2;
	ar >> dwId3;
	
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
#if __VER >= 12 // __EXT_PIERCING
	if( IsValidObj( pUser ) )
		CItemUpgrade::GetInstance()->OnPiercingSize( pUser, dwId1, dwId2, dwId3 );
#else // __EXT_PIERCING
	if( IsValidObj( pUser ) )
	{
		CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwId1 );
		CItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwId2 );
		CItemElem* pItemElem2	= pUser->m_Inventory.GetAtId( dwId3 );

		if( IsUsableItem( pItemElem0 ) == FALSE || IsUsableItem( pItemElem1 ) == FALSE )
			return;

		// 康
		if( pUser->m_vtInfo.GetOther() )	// 거래중인 대상이 있으면?
			return;
		if( pUser->m_vtInfo.VendorIsVendor() )		// 내가 팔고 있으면?
			return;

		int nError = 1;
		if( IsPiercingSize( pUser, pItemElem0, pItemElem1, pItemElem2, nError ) == FALSE )
		{
			if( nError != 1 )
				pUser->AddDefinedText( nError );
			return;
		}

		LogItemInfo aLogItem;
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = "PIERCING";
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		
		int nCost = pItemElem0->GetProp()->dwItemRare * ( 200  + ( pItemElem0->GetPiercingSize()+1) * 200 );

		if( 0 < nCost )
		{
			if( pUser->GetGold() < nCost )
			{
				pUser->AddDefinedText( TID_GAME_LACKMONEY , "" );
				return;
			}
				
			pUser->AddGold( -( nCost ) );

			aLogItem.Gold = pUser->GetGold() + nCost;
			aLogItem.Gold2 = pUser->GetGold();
			aLogItem.Action = "!";
			//aLogItem.ItemName = "SEED";
			_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
			aLogItem.itemNumber = nCost;
			OnLogItem( aLogItem );
		}
		else
		{
			return;
		}
		aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();


		int nPersent = 0;
		int n8DicePersent[4] = { 80, 50, 20, 5 };
#if __VER >= 8 //__Y_NEW_ENCHANT
		if( pItemElem1->GetProp()->dwID == II_GEN_MAT_MOONSTONE
			|| pItemElem1->GetProp()->dwID == II_GEN_MAT_MOONSTONE_1 )
			nPersent = n8DicePersent[pItemElem0->GetPiercingSize()];
#else //__Y_NEW_ENCHANT
		int n10DicePersent[4] = { 85, 55, 25, 10 };
		if( pItemElem1->GetProp()->dwID == II_GEN_MAT_DIE_EIGHT )
			nPersent = n8DicePersent[pItemElem0->GetPiercingSize()];
		else if( pItemElem1->GetProp()->dwID == II_GEN_MAT_DIE_TEN )
			nPersent = n10DicePersent[pItemElem0->GetPiercingSize()];
#endif //__Y_NEW_ENCHANT

		if( xRandom( 100 ) > nPersent )
		{	// 실패
			aLogItem.Action = "!";
			OnLogItem( aLogItem, pItemElem0, pItemElem0->m_nItemNum );
			pUser->AddPlaySound( SND_INF_UPGRADEFAIL );
			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
			pUser->AddDefinedText( TID_MMI_PIERCINGFAIL , "" );

			if( pItemElem2 == NULL )								// 상용화 아이템을 사용하지 않았다면 
				pUser->RemoveItem( dwId1, (short)1 );	// 피어싱 대상 아이템은 삭제된다.
		}
		else
		{	// 성공			
			pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );			
			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);			
			pUser->UpdateItem( (BYTE)pItemElem0->m_dwObjId, UI_PIERCING_SIZE, pItemElem0->GetPiercingSize() + 1 );
			pUser->AddDefinedText( TID_MMI_PIERCINGSUCCESS , "" );

			aLogItem.Action = "#";
			OnLogItem( aLogItem, pItemElem0, pItemElem0->m_nItemNum );
		}
		aLogItem.Action = "!";
		OnLogItem( aLogItem, pItemElem1, pItemElem1->m_nItemNum );
		
		// 다이스와 유료아이템 삭제한다.
		pUser->RemoveItem( dwId2, (short)1 );
		
		if( dwId3 != NULL_ID )
		{
			aLogItem.Action = "!";
			if( IsUsableItem( pItemElem2 ) )
			{
				OnLogItem( aLogItem, pItemElem2, pItemElem2->m_nItemNum );
				g_dpDBClient.SendLogSMItemUse( "2", pUser, pItemElem2, pItemElem2->GetProp() );
				pUser->RemoveItem( dwId3, (short)1 );
			}
			else
			{
				OnLogItem( aLogItem, NULL, 0 );
			}
		}
	}
#endif // __EXT_PIERCING
}

void CDPSrvr::OnItemTransy( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objidTarget, objidTransy;
	ar >> objidTarget;
	ar >> objidTransy;
#ifdef __SYS_ITEMTRANSY
	DWORD dwChangeId;
	BOOL bCash;
	ar >> dwChangeId;
	ar >> bCash;
#endif // __SYS_ITEMTRANSY
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __SYS_ITEMTRANSY
		CItemUpgrade::GetInstance()->OnItemTransy( pUser, objidTarget, objidTransy, dwChangeId, bCash );
#else // __SYS_ITEMTRANSY
		CItemElem* pItemElemTarget = pUser->m_Inventory.GetAtId( objidTarget );
		CItemElem* pItemElemTransy = pUser->m_Inventory.GetAtId( objidTransy );

		// 사용중인 아이템인지 검사
		if( IsUsableItem( pItemElemTarget ) == FALSE || IsUsableItem( pItemElemTransy ) == FALSE )
			return;

		// 장착되어 있는 아이템이면 리턴( 오라이~~~ ㅋㅋ )
		if( pUser->m_Inventory.IsEquip( objidTarget ) || pUser->m_Inventory.IsEquip( objidTransy ) )
		{
			pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
			return;
		}
		
		// 재료가 트랜지(ITM)인지 검사
		if( !(pItemElemTransy->GetProp()->dwID == II_CHR_SYS_SCR_ITEMTRANSY_A || pItemElemTransy->GetProp()->dwID == II_CHR_SYS_SCR_ITEMTRANSY_B) )
			return;
		
		// 레벨 검사
		if( pItemElemTransy->GetProp()->dwID == II_CHR_SYS_SCR_ITEMTRANSY_A )
		{
			if( 61 <= pItemElemTarget->GetProp()->dwLimitLevel1 )
				return;
		}
		else
		{
			if( pItemElemTarget->GetProp()->dwLimitLevel1 < 61 )
				return;
		}

		// 성별이 있는 아이템 인지 검사
		ItemProp* pItemPropChange = NULL;
		ItemProp* pItemProp = pItemElemTarget->GetProp();
		
		pItemPropChange = pUser->GetTransyItem( pItemProp );

		if( pItemPropChange == NULL )
			return;

		// 아이템 트랜지 성공
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );			
		g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);			

		// 기존 아이템의 Elem 정보를 저장 하고 잇음...
		CItemElem ItemElemSend;
		ItemElemSend = *pItemElemTarget;
		ItemElemSend.m_dwItemId = pItemPropChange->dwID;
		ItemElemSend.m_nHitPoint	= pItemPropChange->dwEndurance;		// 내구력 100%

		g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemElemTransy, pItemElemTransy->GetProp() );
		g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemElemTarget, pItemElemTarget->GetProp(), "RemoveItem" );	
		g_dpDBClient.SendLogSMItemUse( "1", pUser, &ItemElemSend, ItemElemSend.GetProp(), "CreateItem" );	
		pUser->AddDefinedText( TID_GAME_ITEM_TRANSY_SUCCESS, "\"%s\" \"%s\"", pItemElemTarget->GetProp()->szName, ItemElemSend.GetProp()->szName );

		// 기존 아이템 재료 삭제
		pUser->RemoveItem( objidTarget, (short)1 );
		pUser->RemoveItem( objidTransy, (short)1 );		

		// 새로운 아이템 지급
		pUser->CreateItem( &ItemElemSend );
#endif // __SYS_ITEMTRANSY
	}
}

void CDPSrvr::OnExpBoxInfo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	u_long uIdPlayer;
	OBJID objid;
	
	ar >> uIdPlayer >> objid;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CCtrl* pCtrl;

		pCtrl	= prj.GetCtrl( objid );
		if( IsValidObj( pCtrl ) )
			pUser->AddExpBoxInfo( objid,  ((CCommonCtrl*)pCtrl)->m_CtrlElem.m_dwSet, ((CCommonCtrl*)pCtrl)->m_dwDelete - timeGetTime(), ((CCommonCtrl*)pCtrl)->m_idExpPlayer );
	}
}

void CDPSrvr::OnPiercing( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwId1, dwId2;
	
	ar >> dwId1;
	ar >> dwId2;
	
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
#if __VER >= 12 // __EXT_PIERCING
	if( IsValidObj( pUser ) )
		CItemUpgrade::GetInstance()->OnPiercing( pUser, dwId1, dwId2 );
#else // __EXT_PIERCING
	if( IsValidObj( pUser ) )
	{
		// 인벤토리에 있는지 장착되어 있는지 확인을 해야 함
		CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwId1 );
		CItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwId2 );
		if( IsUsableItem( pItemElem0 ) == FALSE || IsUsableItem( pItemElem1 ) == FALSE )
			return;

		// 장착되어 있는 아이템은 피어싱 못함
		if( pUser->m_Inventory.IsEquip( dwId1 ) )
		{
			pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
			return;
		}			

		// 카드가 IK3_SOCKETCARD가 아니면 피어싱 못함
		if( pItemElem1->GetProp()->dwItemKind3 != IK3_SOCKETCARD )
		{
			pUser->AddDefinedText( TID_UPGRADE_ERROR_WRONGUPLEVEL , "" );			
			return;					
		}

		// 카드가 들어갈 아이템이 슈트( IK3_SUIT ) 인지 검사
		if( pItemElem0->GetProp()->dwItemKind3 != IK3_SUIT )
		{
			pUser->AddDefinedText(  TID_PIERCING_POSSIBLE_ITEM, "" );
			return;
		}
		
		// 총 피어싱된수와 전체 수를 비교한다.
		int nSize = pItemElem0->GetPiercingSize();
		
		int nCount = 0;
		for( int j = 0; j < nSize; j++ )
		{
			if( pItemElem0->GetPiercingItem( j ) != 0 )
				nCount++;
		}
		
		// 빈곳이 없으면 중단
		if( nCount == nSize )
		{
			pUser->AddDefinedText( TID_PIERCING_ERROR_NOPIERCING, "" );
			return;
		}

		// 康
		if( pUser->m_vtInfo.GetOther() )	// 거래중인 대상이 있으면?
			return;
		if( pUser->m_vtInfo.VendorIsVendor() )		// 내가 팔고 있으면?
			return;
		
		LogItemInfo aLogItem;
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = "PIERCING";
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
		
		if( pUser->Pierce( pItemElem0, pItemElem1->m_dwItemId ) )
		{
			aLogItem.Action = "$";
			OnLogItem( aLogItem, pItemElem0, pItemElem0->m_nItemNum );
			aLogItem.Action = "!";
			OnLogItem( aLogItem, pItemElem1, pItemElem1->m_nItemNum );

			// 아이템 박기 성공~
			pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );			
			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);			
			// 재료 아이템 삭제
			pUser->RemoveItem( dwId2, (short)1 );		
		}
	}
#endif // __EXT_PIERCING
}

#if __VER >= 11 // __PIERCING_REMOVE
// 피어싱 옵션 제거(카드 제거)
void CDPSrvr::OnPiercingRemove( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID objId;
	ar >> objId;
	
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;
	
#if __VER >= 12 // __EXT_PIERCING
	CItemUpgrade::GetInstance()->OnPiercingRemove( pUser, objId );
#else // __EXT_PIERCING
	CItemElem* pItemSuit = pUser->m_Inventory.GetAtId( objId );
	if( !IsUsableItem( pItemSuit ) || pItemSuit->GetProp()->dwItemKind3 != IK3_SUIT )
		return;

	if( pUser->m_Inventory.IsEquip( objId ) )
		return;

	// 피어싱 옵션이 없는 경우
	if( pItemSuit->GetPiercingSize() == 0 || pItemSuit->GetPiercingItem( 0 ) == 0 )
	{
		pUser->AddDefinedText( TID_GAME_REMOVE_PIERCING_ERROR );
		return;
	}

	int nPayPenya = 1000000; // 지불할 페냐
	if( pUser->GetGold() < nPayPenya )	// 페냐가 부족하다.
	{
		pUser->AddDefinedText( TID_GAME_LACKMONEY );
		return;
	}

	for( int i=pItemSuit->GetPiercingSize()-1; i>=0; i-- )
	{
		if( pItemSuit->GetPiercingItem( i ) != 0 )
		{
			pUser->AddGold( -nPayPenya );	// 페냐 지불
			pUser->AddDefinedText( TID_GAME_REMOVE_PIERCING_SUCCESS );
			pUser->UpdateItem( pItemSuit->m_dwObjId, UI_PIERCING, MAKELONG( i, 0 ) );
			
			LogItemInfo aLogItem;
			aLogItem.Action = "$";
			aLogItem.SendName = pUser->GetName();
			aLogItem.RecvName = "PIERCING_REMOVE";
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = pUser->GetGold() + nPayPenya;
			aLogItem.Gold2 = pUser->GetGold();
			aLogItem.Gold_1 = -nPayPenya;
			OnLogItem( aLogItem, pItemSuit, 1 );
			break;
		}
	}
#endif // __EXT_PIERCING
}
#endif // __PIERCING_REMOVE

void CDPSrvr::OnCreateSfxObj( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwSfxId;
	u_long uIdPlayer;
	BOOL bFlag;
	ar >> dwSfxId >> uIdPlayer >> bFlag;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( uIdPlayer == NULL_ID )
		{
			g_UserMng.AddCreateSfxObj((CMover *)pUser, dwSfxId, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z, bFlag);
		}
		else
		{
			CUser* pUsertmp	= (CUser*)prj.GetUserByID( uIdPlayer );
			if( IsValidObj( pUsertmp ) )
				g_UserMng.AddCreateSfxObj((CMover *)pUsertmp, dwSfxId, pUsertmp->GetPos().x, pUsertmp->GetPos().y, pUsertmp->GetPos().z, bFlag);
		}		
	}
}

// 1000 단위의 퍼센트를 넘긴다.
// dwID - 제련 아이템 아이디, n - 제련 단계 
int GetEnchantPercent( DWORD dwID, int n )
{
#if __VER >= 8 //__Y_NEW_ENCHANT	
	static int nPersent1[10] = { 1000, 1000, 700, 500, 400, 300, 200, 100, 50, 20 };
#else //__Y_NEW_ENCHANT
	static int nPersent1[10] = {  900,  850, 800, 600, 450, 300, 200, 100, 50, 20 };
#endif //__Y_NEW_ENCHANT
	static int nPersent2[10] = { 1000, 1000, 900, 750, 550, 400, 250, 150, 80, 40 };

	float fFactor = 1.0f;
	if( ::GetLanguage() != LANG_KOR && n >= 3 )	// 제련 4부터 10% 확률 감소 
		fFactor = 0.9f;

	if( dwID == II_GEN_MAT_DIE_TWELVE )
		return	( (int)( nPersent2[n] * fFactor ) );
	else
		return	( (int)( nPersent1[n] * fFactor ) );
}

#if __VER >= 11 // __SYS_IDENTIFY
void CDPSrvr::OnRemoveItemLevelDown( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwId;
	ar >> dwId;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CItemElem* pItemElem	= (CItemElem*)pUser->GetItemId( dwId );
		if( !IsUsableItem( pItemElem ) )
			return;

#ifdef __GAMEGUARD
		if (pUser->m_Inventory.IsEquip(pItemElem->m_dwObjId))
		{
			pUser->AddDiagText(prj.GetText(TID_GAME_EQUIPPUT));
			return;
		}
#endif //__GAMEGUARD

		if( pItemElem->GetLevelDown() == 0 )
		{
			pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
			return;
		}
		PutItemLog( pUser, "v", "OnRemoveItemLevelDown", pItemElem );
		pItemElem->SetLevelDown();
		pUser->UpdateItemEx( (BYTE)( pItemElem->m_dwObjId ), UI_RANDOMOPTITEMID, pItemElem->GetRandomOptItemId() );
	}
}

void CDPSrvr::OnDoUseItemTarget( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwMaterial, dwTarget;
	ar >> dwMaterial >> dwTarget;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CItemElem* pMaterial	= (CItemElem*)pUser->GetItemId( dwMaterial );
		CItemElem* pTarget	= (CItemElem*)pUser->GetItemId( dwTarget );
		if( !IsUsableItem( pMaterial ) || !IsUsableItem( pTarget ) )
			return;
		if( pUser->m_Inventory.IsEquip( dwTarget ) )
		{
			pUser->AddDefinedText( TID_GAME_EQUIPPUT );
			return;
		}
		BOOL	b	= FALSE;
		switch( pMaterial->m_dwItemId ) 
		{
#ifdef __LOOKCHANGE
#ifdef __SYS_SYS_ITEMMODELRETURN
				// Transmute Model Remove
				case II_SYS_SYS_ITEMMODELRETURN:
				{
					if (b = (pTarget->GetLook() != NULL_ID))
					{
						if (pUser->IsUsingEatPet(pTarget))
						{
							b = FALSE;
							pUser->AddText("You must unequip your pet first!", COLOR_ERROR);
							return;
						}

						pTarget->SetLook(NULL_ID);
						pUser->AddLookChange(pTarget->m_dwObjId, NULL_ID);
						pUser->AddText("Model removed successfully!", COLOR_SUCCESS);
					}
					else
					{
						pUser->AddDefinedText(TID_MMI_LOOKCHANGE02);
					}
					break;
				}
				// Transmute Model Remove & Recovery
				case II_SYS_SYS_ITEMMODELRETURN_PRO:
				{
					if (b = (pTarget->GetLook() != NULL_ID))
					{
						if (pUser->IsUsingEatPet(pTarget))
						{
							b = FALSE;
							pUser->AddText("You must unequip your pet first!", COLOR_ERROR);
							return;
						}

						ItemProp* pItemProp = prj.GetItemProp(pTarget->GetLook());
						if (pItemProp)
						{
							CItemElem itemModel;
							itemModel.m_dwItemId = pTarget->GetLook();
							itemModel.m_nItemNum = 1;
							itemModel.SetSerialNumber();

							if (!pUser->CreateItem(&itemModel))
							{
								g_dpDBClient.SendQueryPostMail(pUser->m_idPlayer, 0, itemModel, 0, pItemProp->szName, "Your recovered model!");
							}

							pTarget->SetLook(NULL_ID);
							pUser->AddLookChange(pTarget->m_dwObjId, NULL_ID);
							pUser->AddText("Model removed and recovered successfully!", COLOR_SUCCESS);
						}
					}
					else
					{
						pUser->AddDefinedText(TID_MMI_LOOKCHANGE02);
					}
					break;
				}
#endif // __SYS_SYS_ITEMMODELRETURN
#endif //__LOOKCHANGE
			case II_SYS_SYS_QUE_PETRESURRECTION02_S:
			case II_SYS_SYS_QUE_PETRESURRECTION02_A:
			case II_SYS_SYS_QUE_PETRESURRECTION02_B:
				{
					CPet* pPet	= pTarget->m_pPet;
					if( !pPet || !pTarget->IsFlag( CItemElem::expired ) )
					{
						pUser->AddDefinedText( TID_GAME_PETRESURRECTION_WRONG_TARGET_01 );
					}
					else
					{
						BYTE nLevel		= pPet->GetLevel();
						if( ( nLevel == PL_B && pMaterial->m_dwItemId != II_SYS_SYS_QUE_PETRESURRECTION02_B )
							|| ( nLevel == PL_A && pMaterial->m_dwItemId != II_SYS_SYS_QUE_PETRESURRECTION02_A )
							|| ( nLevel == PL_S && pMaterial->m_dwItemId != II_SYS_SYS_QUE_PETRESURRECTION02_S )
							|| ( nLevel < PL_B )
							)
						{
							pUser->AddDefinedText( TID_GAME_PETRESURRECTION_WRONG_TARGET_02 );
						}
						else
						{
							PutItemLog( pUser, "r", "::PetResurrection", pTarget );

							pTarget->ResetFlag( CItemElem::expired );
							pUser->UpdateItem( (BYTE)( pTarget->m_dwObjId ), UI_FLAG, MAKELONG( pTarget->m_dwObjIndex, pTarget->m_byFlag ) );
							pPet->SetLife( 0 );
							pPet->SetEnergy( pPet->GetMaxEnergy() / 2 );
							pUser->AddPetState( pTarget->m_dwObjId, pPet->GetLife(), pPet->GetEnergy(), pPet->GetExp() );
							pUser->AddDefinedText( TID_GAME_PETRESURRECTION_SUCCESS );
							b	= TRUE;
						}
					}
					break;
				}
			case II_SYS_SYS_SCR_AWAKECANCEL:
			case II_SYS_SYS_SCR_AWAKECANCEL02:
				b	= DoUseItemTarget_InitializeRandomOption( pUser, pTarget, CRandomOptionProperty::eAwakening,
					TID_GAME_AWAKECANCEL_INFO, TID_GAME_AWAKECANCEL,
					"r", "::AwakeCancel" );
				break;
			case II_SYS_SYS_SCR_AWAKE:
#ifdef __NEW_ITEM_VARUNA
				if( pTarget->GetProp()->IsBaruna() )
				{
					b	= DoUseItemTarget_GenRandomOption( pUser, pTarget, CRandomOptionProperty::eAwakening, 
						0, TID_MMI_ERRORSENCHANT_BARUNA, TID_GAME_AWAKE_OR_BLESSEDNESS01,
						"r", "::Awake" );
				}
				else
#endif // __NEW_ITEM_VARUNA
				{
					b	= DoUseItemTarget_GenRandomOption( pUser, pTarget, CRandomOptionProperty::eAwakening, 
						0, TID_GAME_INVALID_TARGET_ITEM, TID_GAME_AWAKE_OR_BLESSEDNESS01,
						"r", "::Awake" );
				}
				break;
			case II_SYS_SYS_SCR_BLESSEDNESS:
			case II_SYS_SYS_SCR_BLESSEDNESS02:
				b	= DoUseItemTarget_GenRandomOption( pUser, pTarget, CRandomOptionProperty::eBlessing, 
					0, TID_GAME_USE_BLESSEDNESS_INFO, TID_GAME_BLESSEDNESS_INVALID_ITEM,
					"r", "::Blessedness" );
				break;
#if __VER >= 12 // __PET_0519
			case II_SYS_SYS_SCR_EATPETAWAKE:	// 먹펫 각성
				b	= DoUseItemTarget_GenRandomOption( pUser, pTarget, CRandomOptionProperty::eEatPet,
					TID_GAME_PETAWAKE_S00, TID_GAME_PETAWAKE_E00, TID_GAME_PETAWAKE_E00,
					"r", "EATPETAWAKE" );
				break;
			case II_SYS_SYS_SCR_PETAWAKE:	// 시스템 펫 각성
				b	= DoUseItemTarget_GenRandomOption( pUser, pTarget, CRandomOptionProperty::eSystemPet, 
					TID_GAME_PETAWAKE_S00, TID_GAME_PETAWAKE_E00, TID_GAME_PETAWAKE_E00,
					"r", "PETAWAKE" );
				break;
			case II_SYS_SYS_SCR_PETAWAKECANCEL:		// 시스템 펫 각성 취소
				b	= DoUseItemTarget_InitializeRandomOption( pUser, pTarget, CRandomOptionProperty::eSystemPet,
					TID_GAME_PETAWAKECANCEL_S00, TID_GAME_PETAWAKECANCEL_E00,
					"r", "PETAWAKECANCEL" );
				break;
#endif	// __PET_0519
#ifdef __SYS_SCR_CANCELLATION
			case II_SYS_SYS_SCR_CANCELLATION:
				b = UnbindScroll(pUser, pMaterial, pTarget);
				break;
#endif //__SYS_SCR_CANCELLATION

#ifdef __BLING_BLING
			case II_SYS_SYS_BLING_BLING:
			{
				ItemProp* pPropTarget = pTarget->GetProp();
				if (pPropTarget->dwParts == PARTS_LWEAPON || pPropTarget->dwParts == PARTS_RWEAPON || pPropTarget->dwParts == PARTS_SHIELD)
				{
					if (!pTarget->IsFlag(CItemElem::reflect)
#ifndef __LOOKCHANGE
						&& pPropTarget->nReflect < 1
#endif // __LOOKCHANGE
						)
					{
						pTarget->SetFlag(CItemElem::reflect);
						pUser->UpdateItem((BYTE)pTarget->m_dwObjId, UI_FLAG, MAKELONG(pTarget->m_dwObjIndex, pTarget->m_byFlag));
						b = TRUE;
					}
					else
						pUser->AddText(prj.GetText(TID_BLING_BLING_ERROR), COLOR_ERROR);
				}
				else
					pUser->AddText(prj.GetText(TID_BLING_BLING_ERROR), COLOR_ERROR);
			}
			break;
			case II_SYS_SYS_REMOVE_BLING_BLING:
			{
				ItemProp* pPropTarget = pTarget->GetProp();
				if (pPropTarget->dwParts == PARTS_LWEAPON || pPropTarget->dwParts == PARTS_RWEAPON || pPropTarget->dwParts == PARTS_SHIELD)
				{
					if (pTarget->IsFlag(CItemElem::reflect))
					{
						pTarget->ResetFlag(CItemElem::reflect);
						pUser->UpdateItem((BYTE)(pTarget->m_dwObjId), UI_FLAG, MAKELONG(pTarget->m_dwObjIndex, pTarget->m_byFlag));
						b = TRUE;
					}
				}
			}
			break;
#endif // __BLING_BLING

			case II_SYS_SYS_SCR_LEVELDOWN01:
			case II_SYS_SYS_SCR_LEVELDOWN02:
				b	= DoUseItemTarget_ItemLevelDown( pUser, pMaterial, pTarget );
				break;
			default:
				break;
		}
		if( b )
		{
			pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );
			if( pUser->IsMode( TRANSPARENT_MODE ) == 0 )
				g_UserMng.AddCreateSfxObj( pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );

			PutItemLog( pUser, "u", "OnDoUseItemTarget", pMaterial );

			pUser->UpdateItem( (BYTE)( pMaterial->m_dwObjId ), UI_NUM, pMaterial->m_nItemNum - 1 );	
		}
	}
}
#endif	// __SYS_IDENTIFY

#if __VER >= 14 // __SMELT_SAFETY
void CDPSrvr::OnSmeltSafety( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID dwItemId, dwItemMaterialId, dwItemProtScrId, dwItemSmeltScrId;

	//	pItemSmeltScrId - 일반제련시의 제련두루마리(사용안할시엔 Client에서 NULL_ID를 입력)
	ar >> dwItemId >> dwItemMaterialId >> dwItemProtScrId >> dwItemSmeltScrId;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) )
	{
		if( pUser->m_vtInfo.GetOther() || pUser->m_vtInfo.VendorIsVendor() )	// 거래중인 대상이 있으면?
		{
			pUser->AddSmeltSafety( 0 );
			return;
		}

		// 인벤토리에 있는지 장착되어 있는지 확인을 해야 함
		CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwItemId );
		CItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwItemMaterialId );
		CItemElem* pItemElem2	= pUser->m_Inventory.GetAtId( dwItemProtScrId );
		CItemElem* pItemElem3	= NULL;
		if( dwItemSmeltScrId != NULL_ID )
		{
			pItemElem3	= pUser->m_Inventory.GetAtId( dwItemSmeltScrId );
			if( !IsUsableItem( pItemElem3 ) )
				return;
		}

		if( IsUsableItem( pItemElem0 ) == FALSE || IsUsableItem( pItemElem1 ) == FALSE || IsUsableItem( pItemElem2 ) == FALSE )
		{
			pUser->AddSmeltSafety( 0 );
			return;
		}

		// 장착되어 있는 아이템은 제련 못함
		if( pUser->m_Inventory.IsEquip( dwItemId ) )
		{
			pUser->AddSmeltSafety( 0 );
			return;
		}

		if( pItemElem0->m_nResistSMItemId != 0 ) // 상용화 아이템 적용중이면 불가능
		{
			pUser->AddSmeltSafety( 0 );
			return;
		}
		
		BYTE nResult = CItemUpgrade::GetInstance()->OnSmeltSafety( pUser, pItemElem0, pItemElem1, pItemElem2, pItemElem3 );

		pUser->AddSmeltSafety( nResult );
	}
}
#endif // __SMELT_SAFETY

void CDPSrvr::OnEnchant( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwItemId, dwItemMaterialId;
	
	ar >> dwItemId;
	ar >> dwItemMaterialId;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->m_vtInfo.GetOther() )	// 거래중인 대상이 있으면?
			return;
		if( pUser->m_vtInfo.VendorIsVendor() )		// 내가 팔고 있으면?
			return;
#if __VER >= 11 // __SYS_COLLECTING
#if __VER < 14 // __SMELT_SAFETY
		if( pUser->PreRefine( dwItemId, dwItemMaterialId ) )
			return;
#endif // __SMELT_SAFETY
#endif	// __SYS_COLLECTING


		// 인벤토리에 있는지 장착되어 있는지 확인을 해야 함
		CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwItemId );
		CItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwItemMaterialId );		

		if( IsUsableItem( pItemElem0 ) == FALSE || IsUsableItem( pItemElem1 ) == FALSE )
			return;

		if( pItemElem0->m_nResistSMItemId != 0 ) // 상용화 아이템 적용중이면 불가능
		{
			pUser->AddDefinedText( TID_GAME_NOTUPGRADE , "" );
			return;
		}

#if __VER < 11 // __SYS_COLLECTING
		if( pUser->m_Inventory.IsEquip( dwItemId ) )
		{
			pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
			return;
		}
#endif	// __SYS_COLLECTING

#if __VER < 14 // __SMELT_SAFETY
	#if __VER >= 8 //__Y_NEW_ENCHANT
		if( pItemElem1->GetProp()->dwItemKind3 != IK3_ELECARD && pItemElem1->GetProp()->dwItemKind3 != IK3_ENCHANT
#ifdef __V19_FORMATS
			|| pItemElem1->GetProp()->dwItemKind3 != IK3_SMELT_ULTIMATE_MATERIAL
			|| pItemElem1->GetProp()->dwItemKind3 != IK3_SMELT_GENERAL_MATERIAL
#endif //__V19_FORMATS
 )
	#else //__Y_NEW_ENCHANT
		if( pItemElem1->GetProp()->dwItemKind3 != IK3_ELECARD && pItemElem1->GetProp()->dwItemKind3 != IK3_DICE )
	#endif //__Y_NEW_ENCHANT
		{
			pUser->AddDefinedText( TID_UPGRADE_ERROR_WRONGUPLEVEL , "" );			
			return;					
		}
#endif // __SMELT_SAFETY
#if __VER >= 13 // __EXT_ENCHANT
		CItemUpgrade::GetInstance()->OnEnchant( pUser, pItemElem0, pItemElem1 );
#else // __EXT_ENCHANT
		int nLevDown = 3;		
		int*  pAbilityOption = NULL;
			
		// 속성제련은 슈트와, 무기에만 적용
		if( pItemElem1->GetProp()->dwItemKind3 == IK3_ELECARD )
		{
			pAbilityOption = &(pItemElem0->m_nResistAbilityOption);

			if( pAbilityOption == NULL )
				return;

			// 2가지 속성은 제련할수 없음
			if( pItemElem0->m_bItemResist != SAI79::NO_PROP )
			{
				if( pItemElem0->m_bItemResist != pItemElem1->GetProp()->eItemType )
				{
					pUser->AddDefinedText( TID_UPGRADE_ERROR_TWOELEMENT , "" );								
					return;
				}
			}

			if( !CItemElem::IsEleRefineryAble(pItemElem0->GetProp()) )
			{
				pUser->AddDefinedText( TID_GAME_NOTEQUALITEM , "" );			
				return;								
			}

#if __VER >= 12 // __J12_0
			// 속성 당 하나의 속성 제련 카드를 사용하도록 수정
			DWORD dwReqCard	= WhatEleCard( pItemElem1->GetProp()->eItemType );
#else	// __J12_0
			DWORD dwReqCard = WhatEleCard( *pAbilityOption, pItemElem1->GetProp()->eItemType );
#endif	// __J12_0

			if( pItemElem1->GetProp()->dwID != dwReqCard )
			{
				pUser->AddDefinedText( TID_UPGRADE_ERROR_WRONGUPLEVEL , "" );			
				return;					
			}
		}
		else
		// 일반제련은 방어구, 무기
	#if __VER >= 8 //__Y_NEW_ENCHANT
		if( pItemElem1->GetProp()->dwItemKind3 == IK3_ENCHANT
#ifdef __V19_FORMATS
			|| pItemElem1->GetProp()->dwItemKind3 == IK3_SMELT_ULTIMATE_MATERIAL
			|| pItemElem1->GetProp()->dwItemKind3 == IK3_SMELT_GENERAL_MATERIAL
#endif //__V19_FORMATS
			)
	#else //__Y_NEW_ENCHANT
		if( pItemElem1->GetProp()->dwItemKind3 == IK3_DICE )
	#endif //__Y_NEW_ENCHANT
		{
#if __VER >= 9 // __ULTIMATE
			if( pItemElem0->GetProp()->dwReferStat1 == WEAPON_ULTIMATE )
			{
				pUser->AddDefinedText( TID_GAME_NOTEQUALITEM , "" );
				return;
			}
			
			if( pItemElem1->GetProp()->dwID != II_GEN_MAT_ORICHALCUM01
				&& pItemElem1->GetProp()->dwID != II_GEN_MAT_ORICHALCUM01_1 )
			{
				pUser->AddDefinedText( TID_GAME_NOTEQUALITEM , "" );
				return;
			}
#endif // __ULTIMATE
			pAbilityOption = pItemElem0->GetAbilityOptionPtr();

			if( pAbilityOption == NULL )
				return;

			if( !CItemElem::IsDiceRefineryAble(pItemElem0->GetProp()) )
			{
				pUser->AddDefinedText( TID_GAME_NOTEQUALITEM , "" );			
				return;								
			}

		#if __VER < 8 //__Y_NEW_ENCHANT
			if( pItemElem1->GetProp()->dwID != II_GEN_MAT_DIE_TWELVE )
			{
				if( *pAbilityOption <= 4 )
				{
					if( pItemElem1->GetProp()->dwID != II_GEN_MAT_DIE_FOUR )
					{
						pUser->AddDefinedText( TID_UPGRADE_ERROR_WRONGUPLEVEL , "" );			
						return;					
					}
				}
				else
				{
					if( pItemElem1->GetProp()->dwID != II_GEN_MAT_DIE_SIX )
					{
						pUser->AddDefinedText( TID_UPGRADE_ERROR_WRONGUPLEVEL , "" );			
						return;					
					}
				}
			}
		#endif //__Y_NEW_ENCHANT
		}	
		
		if( *pAbilityOption >= 10 )
		{
			pUser->AddDefinedText( TID_UPGRADE_MAXOVER , "" );			
			return;
		}
		// 1000단위의 성공 퍼센트 
		int nPercent = ::GetEnchantPercent( pItemElem1->GetProp()->dwID, *pAbilityOption );

		LogItemInfo aLogItem;
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = "UPGRADEITEM";
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = pUser->GetGold();
		aLogItem.Gold2 = pUser->GetGold();


		BOOL bSmelprot	= FALSE;
		if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT ) )
		{
			bSmelprot	= TRUE;
			pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT );

			ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SMELPROT );
			if( pItemProp )
				g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
		}
		if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELTING ) )
		{
			if( *pAbilityOption < 7 )
			{
				nPercent	+= 100;
				pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELTING );

				ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SMELTING );
				if( pItemProp )
					g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
			}
		}
#ifdef __SM_ITEM_2ND_EX
		BOOL bSmelprot2	= FALSE;
		if( !bSmelprot && pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT2 ) )
		{
			bSmelprot2	= TRUE;
			pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT2 );
		}
#endif	// __SM_ITEM_2ND_EX

		if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SUPERSMELTING ) )
		{
			if( *pAbilityOption < 7 )
			{
				nPercent	+= 1000;
				pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_SUPERSMELTING );
				
				ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SUPERSMELTING );
				if( pItemProp )
					g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
			}				
		}

		// 해당 아이템의 속성, 일반 레벨을 얻어 확율을 꺼낸다.
		if( xRandom( 1000 ) > nPercent )
		{
			// 실패 메세지 출력
			pUser->AddDefinedText( TID_UPGRADE_FAIL, "" );
			pUser->AddPlaySound( SND_INF_UPGRADEFAIL );

			if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
				g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

			// 실패하면 nLevDown이상이면 아이템 삭제
			if( *pAbilityOption >= nLevDown )
			{
				if( !bSmelprot )
				{	// 사용안하면 들어옴.. 대상 아이템 삭제 실패 로그
				#ifdef __SM_ITEM_2ND_EX
					if( bSmelprot2  )
					{
						if( pItemElem1->GetProp()->dwItemKind3 == IK3_ELECARD )
						{
							pUser->UpdateItem( (BYTE)pItemElem0->m_dwObjId, UI_RAO, pItemElem0->m_nResistAbilityOption - 1 );
							aLogItem.Action = "8";
						}
						else 
					#if __VER >= 8 //__Y_NEW_ENCHANT
						if( pItemElem1->GetProp()->dwItemKind3 == IK3_ENCHANT
#ifdef __V19_FORMATS
							|| pItemElem1->GetProp()->dwItemKind3 == IK3_SMELT_ULTIMATE_MATERIAL
							|| pItemElem1->GetProp()->dwItemKind3 == IK3_SMELT_GENERAL_MATERIAL
#endif //__V19_FORMATS
							)
					#else //__Y_NEW_ENCHANT
						if( pItemElem1->GetProp()->dwItemKind3 == IK3_DICE )
					#endif //__Y_NEW_ENCHANT
						{
							pUser->UpdateItem( (BYTE)pItemElem0->m_dwObjId, UI_AO,  pItemElem0->GetAbilityOption() - 1 );
							aLogItem.Action = "9";
						}
						OnLogItem( aLogItem, pItemElem0, pItemElem0->m_nItemNum );
					}
					else
				#endif	// __SM_ITEM_2ND_EX
					{
						aLogItem.Action = "L";
						OnLogItem( aLogItem, pItemElem0, pItemElem0->m_nItemNum );
						pUser->RemoveItem( dwItemId, (short)1 );
					}
				}
			}
			else
			{	// 사용을 하면 실패 로그
				if( pItemElem1->GetProp()->dwItemKind3 == IK3_ELECARD )
				{
					aLogItem.Action = "J";
					OnLogItem( aLogItem, pItemElem0, pItemElem0->m_nItemNum );
				}
				else 
			#if __VER >= 8 //__Y_NEW_ENCHANT
				if( pItemElem1->GetProp()->dwItemKind3 == IK3_ENCHANT
#ifdef __V19_FORMATS
					|| pItemElem1->GetProp()->dwItemKind3 == IK3_SMELT_ULTIMATE_MATERIAL
					|| pItemElem1->GetProp()->dwItemKind3 == IK3_SMELT_GENERAL_MATERIAL
#endif //__V19_FORMATS
					)
			#else //__Y_NEW_ENCHANT
				if( pItemElem1->GetProp()->dwItemKind3 == IK3_DICE )
			#endif //__Y_NEW_ENCHANT
				{
					aLogItem.Action = "F";
					OnLogItem( aLogItem, pItemElem0, pItemElem0->m_nItemNum );
				}		
			}
		}
		else
		{
			// 성공
			pUser->AddDefinedText( TID_UPGRADE_SUCCEEFUL, "" );
			pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );
			
			if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
				g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
			
			if( pItemElem1->GetProp()->dwItemKind3 == IK3_ELECARD )
			{
				pUser->UpdateItem( (BYTE)pItemElem0->m_dwObjId, UI_IR,  pItemElem1->GetProp()->eItemType );
				pUser->UpdateItem( (BYTE)pItemElem0->m_dwObjId, UI_RAO,  pItemElem0->m_nResistAbilityOption+1 );
				aLogItem.Action = "O";
				OnLogItem( aLogItem, pItemElem0, pItemElem0->m_nItemNum );
			}
			else 
		#if __VER >= 8 //__Y_NEW_ENCHANT
			if( pItemElem1->GetProp()->dwItemKind3 == IK3_ENCHANT
#ifdef __V19_FORMATS
				|| pItemElem1->GetProp()->dwItemKind3 == IK3_SMELT_ULTIMATE_MATERIAL
				|| pItemElem1->GetProp()->dwItemKind3 == IK3_SMELT_GENERAL_MATERIAL
#endif //__V19_FORMATS
				) // // 주사위
		#else //__Y_NEW_ENCHANT
			if( pItemElem1->GetProp()->dwItemKind3 == IK3_DICE ) // // 주사위
		#endif //__Y_NEW_ENCHANT
			{
				pUser->UpdateItem( (BYTE)pItemElem0->m_dwObjId, UI_AO,  pItemElem0->GetAbilityOption()+1 );
				aLogItem.Action = "H";
				OnLogItem( aLogItem, pItemElem0, pItemElem0->m_nItemNum );
			}
		}

		aLogItem.Action = "N";
		OnLogItem( aLogItem, pItemElem1, pItemElem1->m_nItemNum );
		// 제련아템 삭제 - 성공이던, 실패던...
		pUser->RemoveItem( dwItemMaterialId, (short)1 );
#endif // __EXT_ENCHANT
	}	
}

#if __VER >= 10 // __REMOVE_ATTRIBUTE
void CDPSrvr::OnRemoveAttribute( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	int nPayPenya = 100000; //속성제련 제거시 필요한 페냐

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	OBJID objItemId;
	ar >> objItemId;

	CItemElem* pItemElem = pUser->m_Inventory.GetAtId( objItemId );

	if( IsUsableItem( pItemElem ) == FALSE )
	{
		pUser->AddRemoveAttribute( FALSE );
		return;
	}
	// 무기나 방어구만 가능
	if( !CItemElem::IsEleRefineryAble(pItemElem->GetProp()) )
	{
		pUser->AddRemoveAttribute( FALSE );
		pUser->AddDefinedText( TID_GAME_NOTEQUALITEM , "" );
		return;
	}
	if( pItemElem->m_nResistSMItemId != 0 ) // 상용화 아이템 적용중이면 불가능
	{
		pUser->AddRemoveAttribute( FALSE );
		pUser->AddDefinedText( TID_GAME_NOTUPGRADE , "" );
		return;
	}
	// 장착되어 있는지 확인을 해야 함.
	if( pUser->m_Inventory.IsEquip( objItemId ) )
	{
		pUser->AddRemoveAttribute( FALSE );
		pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
		return;
	}
	// 10만 페냐 이상을 소지해야만 속성 제거 가능.
	if( pUser->GetGold() < nPayPenya )
	{
		pUser->AddRemoveAttribute( FALSE );
		pUser->AddDefinedText( TID_GAME_LACKMONEY , "" );
		return;
	}
	
	// 현재 무기에 속성이 적용되어 있어야 가능.
	if( (pItemElem->m_bItemResist != SAI79::NO_PROP) && (pItemElem->m_nResistAbilityOption > 0) )
	{
		pUser->AddGold( -nPayPenya );
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
	
		pUser->UpdateItem( (BYTE)pItemElem->m_dwObjId, UI_IR,  SAI79::NO_PROP );
		pUser->UpdateItem( (BYTE)pItemElem->m_dwObjId, UI_RAO,  0 );
		pUser->AddRemoveAttribute( TRUE );
		
		// 속성제련 제거 성공 로그
		LogItemInfo aLogItem;
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = "REMOVE_ATTRIBUTE";
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = pUser->GetGold() + nPayPenya;
		aLogItem.Gold2 = pUser->GetGold();
		aLogItem.Gold_1 = -nPayPenya;
		aLogItem.Action = "O";
		OnLogItem( aLogItem, pItemElem, 1 );
	}
	else
		pUser->AddRemoveAttribute( FALSE );
}
#endif // __REMOVE_ATTRIBUTE

#if __VER >= 13 // __EXT_ENCHANT
void CDPSrvr::OnChangeAttribute( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpbuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	OBJID objTargetItem, objMaterialItem;
	int nAttribute;

	ar >> objTargetItem >> objMaterialItem;
	ar >> nAttribute;

	CItemUpgrade::GetInstance()->ChangeAttribute( pUser, objTargetItem, objMaterialItem, static_cast<SAI79::ePropType>(nAttribute) );
}
#endif // __EXT_ENCHANT

void CDPSrvr::OnRandomScroll( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpbuf, u_long uBufSize )
{
	DWORD dwId1, dwId2;
	
	ar >> dwId1;
	ar >> dwId2;
	
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		// 인벤토리에 있는지 장착되어 있는지 확인을 해야 함
		CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwId1 );
		CItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwId2 );
		if( IsUsableItem( pItemElem0 ) == FALSE || IsUsableItem( pItemElem1 ) == FALSE )
		{
			return;
		}

		if( pUser->m_Inventory.IsEquip( dwId1 ) )
		{
			pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
			return;
		}			

		if( pItemElem1->GetProp()->dwItemKind3 != IK3_RANDOM_SCROLL )
		{
			return;					
		}

		if( !(pItemElem0->GetProp()->dwItemKind1 == IK1_WEAPON || pItemElem0->GetProp()->dwItemKind2 == IK2_ARMOR || pItemElem0->GetProp()->dwItemKind2 == IK2_ARMORETC ) )
		{
			pUser->AddDefinedText(  TID_GAME_RANDOMSCROLL_ERROR, "" );
			return;
		}

		int nSTR[4] = { 1,  9, 21, 37 };
		int nDEX[4] = { 2, 10, 22, 38 };
		int nINT[4] = { 3, 11, 23, 39 };
		int nSTA[4] = { 4, 12, 24, 40 };

		int nValue = 0;
		int nRandom = xRandom( 100 );
		if( nRandom < 64 )
			nValue = 1;
		else if( nRandom < 94 )
			nValue = 2;
		else if( nRandom < 99 )
			nValue = 3;
		else if( nRandom < 100 )
			nValue = 4;

		if( 0 < nValue )
		{
			int nKind = 0;
			int nToolKind = 0;
			if( pItemElem1->GetProp()->dwID == II_SYS_SYS_SCR_RANDOMSTR )
			{
				nKind = nSTR[nValue-1];
				nToolKind = DST_STR;
			}
			else if( pItemElem1->GetProp()->dwID == II_SYS_SYS_SCR_RANDOMDEX )
			{
				nKind = nDEX[nValue-1];
				nToolKind = DST_DEX;
			}
			else if( pItemElem1->GetProp()->dwID == II_SYS_SYS_SCR_RANDOMINT )
			{
				nKind = nINT[nValue-1];
				nToolKind = DST_INT;
			}
			else if( pItemElem1->GetProp()->dwID == II_SYS_SYS_SCR_RANDOMSTA )
			{
				nKind = nSTA[nValue-1];
				nToolKind = DST_STA;
			}

			pUser->UpdateItem( (BYTE)pItemElem0->m_dwObjId, UI_RANDOMOPTITEMID, nKind );

			LogItemInfo aLogItem;
			aLogItem.SendName = pUser->GetName();
			aLogItem.RecvName = "RANDOMSCROLL";
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			aLogItem.Action = "(";
			OnLogItem( aLogItem, pItemElem0, pItemElem0->m_nItemNum );
			aLogItem.Action = ")";
			OnLogItem( aLogItem, pItemElem1, pItemElem1->m_nItemNum );

			pUser->RemoveItem( (BYTE)( dwId2 ), (short)1 );		

			// 아이템 박기 성공~
			pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );			
			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);			

			DWORD dwStringNum = 0;
			switch( nToolKind )
			{
			case DST_STR:
				dwStringNum = TID_TOOLTIP_STR;
				break;
			case DST_DEX:
				dwStringNum = TID_TOOLTIP_DEX;
				break;
			case DST_STA:
				dwStringNum = TID_TOOLTIP_STA;
				break;
			default: //case DST_INT:
				dwStringNum = TID_TOOLTIP_INT;
				break;
			}
			CString strMessage;
			strMessage.Format( prj.GetText( TID_GAME_RANDOMSCROLL_SUCCESS ), pItemElem0->GetProp()->szName, prj.GetText( dwStringNum ), nValue );
			pUser->AddText( strMessage );
		}
	}
}
	
void CDPSrvr::OnUpgradeBase( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
}

void CDPSrvr::OnCommercialElem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwItemId0, dwItemId1;
	ar >> dwItemId0 >> dwItemId1;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		// 인벤토리에 있는지 장착되어 있는지 확인을 해야 함
		// 인벤토리에 있는지 검사
		CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwItemId0 );
		CItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwItemId1 );

		if( IsUsableItem( pItemElem0 ) == FALSE || 
			IsUsableItem( pItemElem1 ) == FALSE || 
			pItemElem0->GetProp()->nLog >=2 || 
			pItemElem1->GetProp()->nLog >=2 )
		{
			return;
		}

		if( pUser->m_Inventory.IsEquip( dwItemId0 ) || pUser->m_Inventory.IsEquip( dwItemId1 ) )
		{
			pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
			return;
		}

		// 방어구(슈트만), 무기류가 아니면 제련불가능
		if( !( ( pItemElem0->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC ||
			pItemElem0->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT ) ||
			( ( pItemElem0->GetProp()->dwItemKind2 == IK2_ARMOR || pItemElem0->GetProp()->dwItemKind2 == IK2_ARMORETC ) 
			&& pItemElem0->GetProp()->dwItemKind3 == IK3_SUIT )
			) )		
		{
			return;			
		}

		if( pItemElem0->m_nResistSMItemId != 0 ) // 이미적용한 아이템이면 불가능
		{
			return;	
		}

		BOOL bResistDelete = FALSE;

		if( pItemElem1->GetProp()->dwItemKind2 == IK2_SYSTEM )
		{
			// 속성 공격력 추가
			if( pItemElem1->m_dwItemId == II_CHR_SYS_SCR_FIREASTONE ||
				pItemElem1->m_dwItemId == II_CHR_SYS_SCR_WATEILSTONE ||
				pItemElem1->m_dwItemId == II_CHR_SYS_SCR_WINDYOSTONE ||
				pItemElem1->m_dwItemId == II_CHR_SYS_SCR_LIGHTINESTONE ||
				pItemElem1->m_dwItemId == II_CHR_SYS_SCR_EARTHYSTONE ) 
				
			{
				if( pItemElem0->GetProp()->dwItemKind2 == IK2_ARMOR ||
					pItemElem0->GetProp()->dwItemKind2 == IK2_ARMORETC )
				{
					return;	
				}
			}
			else // 속성 방어력 추가
			if(	pItemElem1->m_dwItemId == II_CHR_SYS_SCR_DEFIREASTONE ||
				pItemElem1->m_dwItemId == II_CHR_SYS_SCR_DEWATEILSTONE ||
				pItemElem1->m_dwItemId == II_CHR_SYS_SCR_DEWINDYOSTONE ||
				pItemElem1->m_dwItemId == II_CHR_SYS_SCR_DELIGHTINESTONE ||
				pItemElem1->m_dwItemId == II_CHR_SYS_SCR_DEEARTHYSTONE )
			{
				if( pItemElem0->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC ||
					pItemElem0->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT )
				{
					return;	
				}
			}
			else // 속성 제거
			if( pItemElem1->m_dwItemId == II_CHR_SYS_SCR_TINEINEDSTONE )
			{
				if( pItemElem0->m_bItemResist == SAI79::NO_PROP )
				{
					return;	
				}
				pItemElem0->m_bItemResist = SAI79::NO_PROP;
				pItemElem0->m_nResistAbilityOption = 0;
				bResistDelete = TRUE;
			}
			else
			{
				return;	
			}
		}
		else 
		{
			return;	
		}

		// 성공
		
		if( bResistDelete )
		{
			pUser->AddCommercialElem( pItemElem0->m_dwObjId, 9999 );
		}
		else
		{
			pItemElem0->m_nResistSMItemId = pItemElem1->m_dwItemId;
		}
		
		// 로그
		g_dpDBClient.SendLogSMItemUse( "5", pUser, pItemElem1, pItemElem1->GetProp() );
		LogItemInfo aLogItem;
		aLogItem.Action = "5";
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = pUser->GetName();
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = pUser->GetGold();
		aLogItem.Gold2 = pUser->GetGold();
		aLogItem.ItemNo = pItemElem0->GetSerialNumber();
		//aLogItem.ItemName = pItemElem0->GetProp()->szName;
		_stprintf( aLogItem.szItemName, "%d", pItemElem0->GetProp()->dwID );
		aLogItem.itemNumber = pItemElem0->m_nItemNum;
		aLogItem.nAbilityOption = pItemElem0->GetAbilityOption();
		aLogItem.nItemResist = pItemElem0->m_bItemResist;
		aLogItem.nResistAbilityOption = pItemElem0->m_nResistAbilityOption;
		aLogItem.Negudo = int((float)pItemElem0->m_nHitPoint * 100 / (float)pItemElem0->GetProp()->dwEndurance );
		OnLogItem( aLogItem );

		pUser->RemoveItem( (BYTE)dwItemId1, (short)1 );
		
		g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_SYS_EXPAN01, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
		pUser->AddCommercialElem( pItemElem0->m_dwObjId, pItemElem0->m_nResistSMItemId );
	}
}

void CDPSrvr::OnRequestGuildRank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CTime		tm			= CTime::GetCurrentTime();
	CTimeSpan	tm_update	= tm - CGuildRank::Instance()->m_UpdateTime;

	DWORD	ver;
	
	ar >> ver;

	if ( tm_update.GetHours() >= 24 )
	{
		// TRANS 서버에게 다시 랭크 정보를 갱신할 것을 요청한다.
		g_dpDBClient.UpdateGuildRanking();
	}
	else
	{
		// 랭크 정보 버젼이 다를 경우엔 랭크 정보를 보내게 된다.
		if ( CGuildRank::Instance()->m_Version != ver )
		{
			// 랭킹 정보를 보낸다.
			CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
			if( IsValidObj( pUser ) )
			{
				pUser->SendGuildRank();
			}
		}
	}
}

void CDPSrvr::OnBuyingInfo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __AZURIOM_INGAME_SHOP
	BUYING_INFO2 bi2;
	ar.Read((void*)&bi2, sizeof(BUYING_INFO2));

	CWorld* pWorld;
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	SERIALNUMBER iSerialNumber = 0;

	bi2.dwRetVal = 0;
	CItemElem itemElem;
	itemElem.m_dwItemId = bi2.dwItemId;
	itemElem.m_nItemNum = (short)bi2.dwItemNum;
	itemElem.m_bCharged = TRUE;
	ItemProp* tmpProp = itemElem.GetProp();
	if(tmpProp)
		itemElem.m_bCharged = tmpProp->bCharged;

#ifdef __AEGON_WEBSHOP_FLAG
	itemElem.SetFlag(CItemElem::webshop);
#endif
	BYTE nId;

	CString CheckPw;
	CheckPw = "jcvo9n0sx4179mbuoa2ylztz";

	if (IsValidObj(pUser) && (pWorld = pUser->GetWorld()))
	{
		if(CheckPw.Compare(bi2.szBxaid) == 0)
		{
			bi2.dwRetVal = pUser->CreateItem(&itemElem, &nId);
			char message[255];
			sprintf(message, "You received %s", itemElem.GetName().GetString());
			pUser->AddText(message);
		

			if (bi2.dwRetVal)
			{
				CItemElem* pItemElem = pUser->m_Inventory.GetAtId(nId);
				if (pItemElem)
				{
					iSerialNumber = pItemElem->GetSerialNumber();					
				}
				else 
				{
					Error("OnBuyingInfo item not found in inventory - dwPlayerId: %lu - dwItemId: %lu - dwItemNum: %lu", bi2.dwPlayerId, bi2.dwItemId, bi2.dwItemNum);
				}

				LogItemInfo aLogItem;
				aLogItem.Action = "S";
				aLogItem.SendName = "HOMEPAGE_SHOP";
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();

				aLogItem.RecvName = pUser->GetName();

				if (itemElem.m_dwItemId == II_SYS_VOTECOIN)
				{
					aLogItem.SendName = "HOMEPAGE_VOTE";
				}

				g_DPSrvr.OnLogItem(aLogItem, &itemElem, itemElem.m_nItemNum);
			}
			else
			{
				char* szTitle = "Webshop Purchase";

				//item creation failed
				LogItemInfo aLogItem;
				aLogItem.Action = "S";
				aLogItem.SendName = "HOMEPAGE_SHOP";

				if (itemElem.m_dwItemId == II_SYS_VOTECOIN) 
				{
					aLogItem.SendName = "HOMEPAGE_VOTE";
					szTitle = "Website Vote";
				}

				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();

				g_dpDBClient.SendQueryPostMail(pUser->m_idPlayer, 0, itemElem, 0, szTitle, "");
				aLogItem.RecvName = pUser->GetName();
				g_DPSrvr.OnLogItem(aLogItem, &itemElem, itemElem.m_nItemNum);
			}
	
			g_dpDBClient.SavePlayer(pUser, pWorld->GetID(), pUser->GetPos(), pUser->GetLayer());
		}
		else 
		{
			Error("OnBuyingInfo invalid webshop key - dwPlayerId: %lu - dwItemId: %lu - dwItemNum: %lu - bi2.szBxaid: %s", bi2.dwPlayerId, bi2.dwItemId, bi2.dwItemNum, bi2.szBxaid);
		}
	}
	else 
	{
		Error("OnBuyingInfo pUser invalid - dwPlayerId: %lu - dwItemId: %lu - dwItemNum: %lu", bi2.dwPlayerId, bi2.dwItemId, bi2.dwItemNum);
	}
	g_dpDBClient.SendBuyingInfo(&bi2, iSerialNumber);
#else // __AZURIOM_INGAME_SHOP
	return;
#endif // __AZURIOM_INGAME_SHOP
}

void CDPSrvr::OnEnterChattingRoom( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uidChattingRoom;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		BOOL bChatting = FALSE;
		if( pUser->m_idChatting == 0 )
		{
			ar >> uidChattingRoom;

			CChatting* pChatting = g_ChattingMng.GetChttingRoom( uidChattingRoom );
			if( pChatting )
			{
				if( pChatting->AddChattingMember( pUser->m_idPlayer ) )
				{
					CUser*			pUsertmp;
					pUser->m_idChatting = uidChattingRoom;

					for( int i = 0 ; i < pChatting->GetChattingMember() - 1 ; ++i )
					{
						pUsertmp	= (CUser*)prj.GetUserByID( pChatting->m_idMember[i] );
						if( IsValidObj( pUsertmp ) )
							pUsertmp->AddEnterChatting( pUser );
					}

					pUser->AddNewChatting( pChatting );
				}
				else
				{
					bChatting	= TRUE;
				}
			}

		}
		else
		{
			bChatting	= TRUE;
		}
	}
}

void CDPSrvr::OnChatting( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	static	TCHAR	sChat[1024];
	ar.ReadString( sChat, 1024 );

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __JEFF_9_20
#if __VER >= 12 // __LORD
		int nText	= pUser->GetMuteText();
		if(  nText )
		{
			pUser->AddDefinedText( nText );
			return;
		}
#else	// __LORD
		if( pUser->IsMute() )
			return;
#endif	// __LORD
#endif	// __JEFF_9_20

		if( !( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT)))
			ParsingEffect( sChat, strlen(sChat) );

		RemoveCRLF( sChat );

		CChatting* pChatting = g_ChattingMng.GetChttingRoom( pUser->m_idChatting );
		if( pChatting )
		{
			int nFind = pChatting->FindChattingMember( pUser->m_idPlayer );
			if( nFind != -1 )
			{
				CUser* pUsertmp;
				for( int i = 0 ; i < pChatting->GetChattingMember() ; ++i )
				{
					pUsertmp	= (CUser*)prj.GetUserByID( pChatting->m_idMember[i] );
					if( IsValidObj( pUsertmp ) )
						pUsertmp->AddChatting( pUser->m_idPlayer, sChat );
				}
			}
			else
			{
				// 채팅멤버가 아님
				pUser->m_idChatting = 0;
				pUser->AddDeleteChatting();
			}
		}
		else
		{
			// 채팅방이 없음.
			pUser->m_idChatting = 0;
			pUser->AddDeleteChatting();
		}
	}
}

void CDPSrvr::OnOpenChattingRoom( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{

		CChatting* pChatting	= g_ChattingMng.GetChttingRoom( pUser->m_idChatting );
		if( pChatting )
		{
			pChatting->m_bState		= TRUE;
			CUser* pUsertmp;
			for( int i = 0 ; i < pChatting->GetChattingMember() ; ++i )
			{
				pUsertmp	= (CUser*)prj.GetUserByID( pChatting->m_idMember[i] );
				if( IsValidObj( pUsertmp ) && pUser->m_idPlayer != pUsertmp->m_idPlayer )
					pUsertmp->AddChttingRoomState( pChatting->m_bState );
			}
		}
	}
}

void CDPSrvr::OnCloseChattingRoom( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{

		CChatting* pChatting = g_ChattingMng.GetChttingRoom( pUser->m_idChatting );
		if( pChatting )
		{
			pChatting->m_bState = FALSE;
			CUser*			pUsertmp;
			
			for( int i = 0 ; i < pChatting->GetChattingMember() ; ++i )
			{
				pUsertmp	= (CUser*)prj.GetUserByID( pChatting->m_idMember[i] );
				if( IsValidObj( pUsertmp ) && pUser->m_idPlayer != pUsertmp->m_idPlayer )
				{
					pUsertmp->AddChttingRoomState( pChatting->m_bState );
				}						
			}
		}

	}
}

void CDPSrvr::OnCommonPlace( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
//		BYTE nType;
//		ar >> nType;
}

void CDPSrvr::QueryDestroyPlayer( DPID dpidCache, DPID dpidSocket, DWORD dwSerial, u_long idPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_QUERY_DESTROY_PLAYER, dpidSocket );
	ar << dwSerial;
	ar << idPlayer;
	SEND( ar, this, dpidCache );
}

void CDPSrvr::OnSetNaviPoint( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	NaviPoint nv;
	OBJID objidTarget;
	ar >> nv.Pos >> objidTarget;

	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( objidTarget == NULL_ID )
		{
			CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
			if( pParty )
			{
				for( int i = 0 ; i < pParty->GetSizeofMember() ; ++i )
				{
					CUser* pUsertmp = (CUser *)prj.GetUserByID( pParty->GetPlayerId( i ) );
					if( IsValidObj( pUsertmp ) )
					{
						pUsertmp->AddSetNaviPoint( nv, pUser->GetId(), pUser->GetName( TRUE ) );
					}
				}
			}
		}
		else
		{
			CUser* pUsertmp	= prj.GetUser( objidTarget );
			if( IsValidObj( pUsertmp ) )
			{
				pUser->AddSetNaviPoint( nv, pUser->GetId(), pUser->GetName( TRUE ) );
				pUsertmp->AddSetNaviPoint( nv, pUser->GetId(), pUser->GetName( TRUE ) );
			}
		}
	}
}

void CDPSrvr::OnGameMasterWhisper( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CHAR	sPlayerFrom[MAX_PLAYER], lpString[260], szChat[1024];
	ar.ReadString( sPlayerFrom, MAX_PLAYER );
	ar.ReadString( lpString, 260 );

	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{	
		sprintf( szChat, "%s -> %s", sPlayerFrom, lpString );
		g_dpDBClient.SendLogGamemaChat( pUser, szChat );
	}
}

void CDPSrvr::OnNWWantedGold( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int		nGold;
	char	szMsg[WANTED_MSG_MAX + 1];

	ar >> nGold;
	ar.ReadString( szMsg, WANTED_MSG_MAX + 1 );

	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	if( pUser->m_idMurderer == 0 )		// 나를 죽인자가 없었으면 현상금을 걸 수 없다.
		return;

	if( szMsg[0] == '\0' )
		return;
	
	if( strlen(szMsg) > WANTED_MSG_MAX )
		return;

	if( nGold < MIN_INPUT_REWARD || nGold > MAX_INPUT_REWARD )			// 현상금은 최소 1000패냐에서 최대 2억 패냐까지 걸 수 있다. 
		return;

	int nTax = MulDiv( nGold, 10, 100 );					// 건 현상금의 10%는 수수료로 지급된다. 

	if( pUser->GetGold() >= (nGold + nTax) ) 
		pUser->AddGold( -(nGold + nTax) );
	else
		return;

#if __VER >= 11 // __SYS_PLAYER_DATA
	const char* lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( pUser->m_idMurderer );
#else	// __SYS_PLAYER_DATA
	LPCSTR lpszPlayer = prj.GetPlayerString( pUser->m_idMurderer );
#endif	// __SYS_PLAYER_DATA
	if( lpszPlayer == NULL )
		lpszPlayer = "";
	g_DPCoreClient.SendWCWantedGold( lpszPlayer, pUser->m_idMurderer, nGold, szMsg );
}

void CDPSrvr::OnNWWantedList(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	{
		BEFORESENDSOLE(out, PACKETTYPE_WN_WANTED_LIST, pUser->m_Snapshot.dpidUser);

		CWantedListSnapshot& wantedListSnapshot = CWantedListSnapshot::GetInstance();
		wantedListSnapshot.Write(out);

		SEND(out, this, dpidCache);
	}
}

void CDPSrvr::OnNWWantedName( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	LPCSTR lpszPlayer = "";
	if( pUser->m_idMurderer )		
	{
#if __VER >= 11 // __SYS_PLAYER_DATA
		lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( pUser->m_idMurderer );
#else	// __SYS_PLAYER_DATA
		lpszPlayer = prj.GetPlayerString( pUser->m_idMurderer );
#endif	// __SYS_PLAYER_DATA
		if( lpszPlayer == NULL )
			lpszPlayer	= "";
	}
	
	{
		BEFORESENDSOLE( ar, PACKETTYPE_WN_WANTED_NAME, pUser->m_Snapshot.dpidUser );
		ar.WriteString( lpszPlayer );
		SEND( ar, this, dpidCache );
	}
}

void CDPSrvr::OnNWWantedInfo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	char szPlayer[64];
	ar.ReadString( szPlayer, 64 );

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	CWantedListSnapshot& wantedListSnapshot = CWantedListSnapshot::GetInstance();
	int nIndex = wantedListSnapshot.GetPlayerIndex( szPlayer );
	if( nIndex < 0 )
		return;

	{
		int nGold = REQ_WANTED_GOLD;
		if( pUser->GetGold() >= nGold ) 
		{
			D3DXVECTOR3 vPos( 0.0f, 0.0f, 0.0f );		// 현상범의 위치 
			BYTE		byOnline = 0;					// 1 이면 online
			DWORD		dwWorldID = 0;
			LPCTSTR		lpszWorld = "";

#if __VER >= 11 // __SYS_PLAYER_DATA
			u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( szPlayer );
#else	// __SYS_PLAYER_DATA
			u_long idPlayer = prj.GetPlayerID( szPlayer );
#endif	// __SYS_PLAYER_DATA
			CUser* pTarget	= g_UserMng.GetUserByPlayerID( idPlayer );	
			if( IsValidObj(pTarget) && pTarget->GetWorld() )
			{
				vPos      = pTarget->GetPos();
				byOnline  = 1;
				dwWorldID = pTarget->GetWorld()->GetID();
				lpszWorld = pTarget->GetWorld()->m_szWorldName;

				pUser->AddGold( -nGold );
			}

			pUser->AddWantedInfo( vPos, byOnline, dwWorldID, lpszWorld );
		} 
		else
		{
			pUser->AddDefinedText( TID_GAME_LACKMONEY, "" );	// 인벤에 돈이부족
		}
	}
}

void CDPSrvr::OnReqLeave( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	if( pUser->m_dwLeavePenatyTime == 0 )	// 페널티 타임을 세팅한 적이 없는가?
		pUser->m_dwLeavePenatyTime = ::timeGetTime() + TIMEWAIT_CLOSE * 1000;	//  세팅 
}

void CDPSrvr::OnStateMode( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwStateMode;
	BYTE nFlag;
	ar >> dwStateMode;
	ar >> nFlag;

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj(pUser) )
	{	
		if( pUser->IsStateMode( dwStateMode ) )
		{
			if( nFlag == STATEMODE_BASEMOTION_CANCEL )
			{
				pUser->SetStateNotMode( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL );
				pUser->m_nReadyTime = 0;
				pUser->m_dwUseItemId = 0;
			}
		}
	}
}

// 부활취소
void CDPSrvr::OnResurrectionCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj(pUser) && pUser->m_Resurrection_Data.bUseing )
	{	
		pUser->m_Resurrection_Data.bUseing = FALSE;
	}
}

// 사용자가 OK하면 부활을 쓰게 한다
void CDPSrvr::OnResurrectionOK( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );

	// 서버에서 부활모드인가??
	if( IsValidObj(pUser) && pUser->m_Resurrection_Data.bUseing )
	{	
		if( pUser->IsDie() == FALSE )
		{
			pUser->m_Resurrection_Data.bUseing = FALSE;
			return;
		}
		// 약간의 에너지를 채워주자
		if( pUser->GetType() == OT_MOVER )	// 타겟이 무버일때만.
		{		
			RESURRECTION_DATA* pData = &(((CMover *)pUser)->m_Resurrection_Data);
			CUser* pSrc	= (CUser*)prj.GetUserByID( pData->dwPlayerID );

			if( IsValidObj(pSrc) )
			{
				// 부활 SFX효과
				g_UserMng.AddCreateSfxObj( pUser, XI_SKILL_ASS_HEAL_RESURRECTION01 );
				
				// 부활하기
				g_UserMng.AddHdr( pUser, SNAPSHOTTYPE_RESURRECTION );
				pUser->m_pActMover->SendActMsg( OBJMSG_RESURRECTION );
				pUser->m_Resurrection_Data.bUseing = FALSE;
				
				pUser->ApplyParam( (CMover *)pSrc, pData->pSkillProp, pData->pAddSkillProp, TRUE , 0 );
				
				if( pData->pAddSkillProp->dwDestParam2 == DST_RECOVERY_EXP )
					pUser->SubDieDecExp(TRUE, pData->pAddSkillProp->nAdjParamVal2 );	// 부활이 되면서 겸치가 조금 깎임.			
			}	
			else
				pUser->m_Resurrection_Data.bUseing = FALSE;

		}
	}
}

void CDPSrvr::OnChangeMode( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int nFlag;
	DWORD dwMode;
	ar >> dwMode;
	ar >> nFlag;
	
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj(pUser) )
	{	
		switch( nFlag )
		{
		case 0:	// 자동 PK ON
			pUser->SetMode( FREEPK_MODE );
			break;
		case 1:	// 자동 PK OFF
			pUser->SetNotMode( FREEPK_MODE );
			break;
		case 2:	// 자동 PVP ON
			pUser->SetMode( PVPCONFIRM_MODE );
			break;
		case 3:	// 자동 PVP OFF
			pUser->SetNotMode( PVPCONFIRM_MODE );
			break;
		}
		g_UserMng.AddModifyMode( pUser );
	}
}

void CDPSrvr::OnQuerySetPlayerName( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{

	DWORD dwData;
	char lpszPlayer[MAX_PLAYER]	= { 0, };

	ar >> dwData;
	ar.ReadString( lpszPlayer, MAX_PLAYER );

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) )
	{
#ifdef __AEGON_FIXES
		if (!pUser->IsAuthHigher(AUTH_ADMINISTRATOR))
		{
			return;
		}
#endif



#ifdef __RULE_0615
		if (prj.IsInvalidName(lpszPlayer) || prj.IsAllowedLetter(lpszPlayer) == FALSE)
		{
			pUser->AddDiagText(prj.GetText(TID_DIAG_0020));
#ifdef __GAMEGUARD
			return;
#endif // __GAMEGUARD
		}
		prj.Formalize(lpszPlayer);
#endif	// __RULE_0615

#ifdef __GAMEGUARD
		int nLen = strlen(lpszPlayer);
		char szH[2];
		char szA[10];
		strcpy(szH, "'");
		strcpy(szA, "\\");

		for (int i = 0; i < nLen; i++)
		{
			if (lpszPlayer[i] != 'a' && lpszPlayer[i] != 'b' && lpszPlayer[i] != 'c' && lpszPlayer[i] != 'd' && lpszPlayer[i] != 'e' && lpszPlayer[i] != 'f'
				&& lpszPlayer[i] != 'g' && lpszPlayer[i] != 'h' && lpszPlayer[i] != 'i' && lpszPlayer[i] != 'j' && lpszPlayer[i] != 'k' && lpszPlayer[i] != 'l'
				&& lpszPlayer[i] != 'm' && lpszPlayer[i] != 'n' && lpszPlayer[i] != 'o' && lpszPlayer[i] != 'p' && lpszPlayer[i] != 'q' && lpszPlayer[i] != 'r'
				&& lpszPlayer[i] != 's' && lpszPlayer[i] != 't' && lpszPlayer[i] != 'u' && lpszPlayer[i] != 'v' && lpszPlayer[i] != 'w' && lpszPlayer[i] != 'x'
				&& lpszPlayer[i] != 'y' && lpszPlayer[i] != 'z' && lpszPlayer[i] != 'A' && lpszPlayer[i] != 'B' && lpszPlayer[i] != 'C' && lpszPlayer[i] != 'D'
				&& lpszPlayer[i] != 'E' && lpszPlayer[i] != 'F' && lpszPlayer[i] != 'G' && lpszPlayer[i] != 'H' && lpszPlayer[i] != 'I' && lpszPlayer[i] != 'J'
				&& lpszPlayer[i] != 'K' && lpszPlayer[i] != 'L' && lpszPlayer[i] != 'M' && lpszPlayer[i] != 'N' && lpszPlayer[i] != 'O' && lpszPlayer[i] != 'P'
				&& lpszPlayer[i] != 'Q' && lpszPlayer[i] != 'R' && lpszPlayer[i] != 'S' && lpszPlayer[i] != 'T' && lpszPlayer[i] != 'U' && lpszPlayer[i] != 'V'
				&& lpszPlayer[i] != 'W' && lpszPlayer[i] != 'X' && lpszPlayer[i] != 'Y' && lpszPlayer[i] != 'Z')
			{
				pUser->AddDiagText(prj.GetText(TID_DIAG_0020));
				return;
			}
		}
#endif // __GAMEGUARD

#ifdef __S_SERVER_UNIFY
		if( pUser->m_bAllAction )
#endif // __S_SERVER_UNIFY
		{
			WORD wId	= LOWORD( dwData );
			WORD wMode	= HIWORD( dwData );
			if( (short)wId >= 0 )
			{
				CItemElem* pItemElem = (CItemElem*)pUser->GetItemId( wId );
				if( IsUsableItem( pItemElem ) && pItemElem->m_dwItemId == II_SYS_SYS_SCR_CHANAM && pItemElem->m_bQuery == FALSE )
				{
					pItemElem->m_bQuery	= TRUE;
					g_dpDBClient.SendQuerySetPlayerName( pUser->m_idPlayer, lpszPlayer, dwData );
				}
			}
		}
#ifdef __S_SERVER_UNIFY
		else
		{
			g_dpDBClient.SendQuerySetPlayerName( pUser->m_idPlayer, lpszPlayer, dwData );
		}
#endif // __S_SERVER_UNIFY
	}
}

void CDPSrvr::OnQuerySetGuildName( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE nId;
	char lpszGuild[MAX_G_NAME]	= { 0, };

	ar >> nId;
	ar.ReadString( lpszGuild, MAX_G_NAME );

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __RULE_0615
		if (prj.IsInvalidName(lpszGuild) || prj.IsAllowedLetter(lpszGuild) == FALSE)
		{
			pUser->AddDiagText(prj.GetText(TID_DIAG_0020));
#ifdef __GAMEGUARD
			return;
#endif // __GAMEGUARD
		}
#endif	// __RULE_0615

#ifdef __GAMEGUARD
		int nLen = strlen(lpszGuild);
		char szH[2];
		char szA[10];
		strcpy(szH, "'");
		strcpy(szA, "\\");

		for (int i = 0; i < nLen; i++)
		{
			if (lpszGuild[i] != 'a' && lpszGuild[i] != 'b' && lpszGuild[i] != 'c' && lpszGuild[i] != 'd' && lpszGuild[i] != 'e' && lpszGuild[i] != 'f'
				&& lpszGuild[i] != 'g' && lpszGuild[i] != 'h' && lpszGuild[i] != 'i' && lpszGuild[i] != 'j' && lpszGuild[i] != 'k' && lpszGuild[i] != 'l'
				&& lpszGuild[i] != 'm' && lpszGuild[i] != 'n' && lpszGuild[i] != 'o' && lpszGuild[i] != 'p' && lpszGuild[i] != 'q' && lpszGuild[i] != 'r'
				&& lpszGuild[i] != 's' && lpszGuild[i] != 't' && lpszGuild[i] != 'u' && lpszGuild[i] != 'v' && lpszGuild[i] != 'w' && lpszGuild[i] != 'x'
				&& lpszGuild[i] != 'y' && lpszGuild[i] != 'z' && lpszGuild[i] != 'A' && lpszGuild[i] != 'B' && lpszGuild[i] != 'C' && lpszGuild[i] != 'D'
				&& lpszGuild[i] != 'E' && lpszGuild[i] != 'F' && lpszGuild[i] != 'G' && lpszGuild[i] != 'H' && lpszGuild[i] != 'I' && lpszGuild[i] != 'J'
				&& lpszGuild[i] != 'K' && lpszGuild[i] != 'L' && lpszGuild[i] != 'M' && lpszGuild[i] != 'N' && lpszGuild[i] != 'O' && lpszGuild[i] != 'P'
				&& lpszGuild[i] != 'Q' && lpszGuild[i] != 'R' && lpszGuild[i] != 'S' && lpszGuild[i] != 'T' && lpszGuild[i] != 'U' && lpszGuild[i] != 'V'
				&& lpszGuild[i] != 'W' && lpszGuild[i] != 'X' && lpszGuild[i] != 'Y' && lpszGuild[i] != 'Z')
			{
				pUser->AddDiagText(prj.GetText(TID_DIAG_0020));
				return;
			}
		}
#endif // __GAMEGUARD

#ifdef __S_SERVER_UNIFY
		if( pUser->m_bAllAction )
#endif // __S_SERVER_UNIFY
		{
			CItemElem* pItemElem = (CItemElem*)pUser->GetItemId(nId);
#ifdef __GAMEGUARD
			if (IsUsableItem(pItemElem) && pItemElem->m_bQuery == FALSE && pItemElem->m_dwItemId == II_SYS_SYS_SCR_GCHANAM)
#else // __GAMEGUARD
			if (IsUsableItem(pItemElem) && pItemElem->m_bQuery == FALSE)
#endif // __GAMEGUARD
			{
				pItemElem->m_bQuery = TRUE;
				g_DPCoreClient.SendQuerySetGuildName(pUser->m_idPlayer, pUser->m_idGuild, lpszGuild, nId);
			}
		}
#ifdef __S_SERVER_UNIFY
		else
		{
			g_DPCoreClient.SendQuerySetGuildName( pUser->m_idPlayer, pUser->m_idGuild, lpszGuild, nId );
		}
#endif // __S_SERVER_UNIFY
	}
}

void CDPSrvr::PutCreateItemLog( CUser* pUser, CItemElem* pItemElem, const char* szAction, const char* recv )
{
	LogItemInfo logitem;
	logitem.Action		= szAction;
	logitem.SendName = pUser->GetName();
	logitem.RecvName = recv;
	logitem.WorldId = pUser->GetWorld()->GetID();
	logitem.Gold = pUser->GetGold();
	logitem.Gold2 = 0;
	logitem.ItemNo = pItemElem->GetSerialNumber();
	//logitem.ItemName = pItemElem->GetProp()->szName;
	_stprintf( logitem.szItemName, "%d", pItemElem->GetProp()->dwID );
	logitem.itemNumber = pItemElem->m_nItemNum;
	logitem.nAbilityOption = 0;
	logitem.Gold_1 = 0;
	logitem.nItemResist = pItemElem->m_bItemResist;
	logitem.nResistAbilityOption = pItemElem->m_nResistAbilityOption;
	logitem.Negudo		= 100;
	OnLogItem( logitem );
}

// 클라로부터 탈출요청이 들어옴
void CDPSrvr::OnDoEscape( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		ItemProp* pItemProp = prj.GetItemProp( II_CHR_SYS_SCR_ESCAPEBLINKWING );
		if( pItemProp )
		{
			if( !pUser->IsSMMode( SM_ESCAPE ) )
				pUser->DoUseItemVirtual( II_CHR_SYS_SCR_ESCAPEBLINKWING, FALSE );
			else
				pUser->AddDefinedText( TID_GAME_STILLNOTUSE );		
		}
	}
}

void CDPSrvr::OnCheering( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objid;
	ar >> objid;

	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->GetId() == objid )
			return;

		CMover* pTarget		= prj.GetMover( objid );
		if( IsValidObj( pTarget ) && pTarget->GetType() == OT_MOVER && pTarget->IsPlayer() )
		{
			DWORD dwTickCount	= GetTickCount();
			if( pUser->m_nCheerPoint <= 0 )
			{
				pUser->AddDefinedText( TID_CHEER_NO1, "%d", (pUser->m_dwTickCheer - dwTickCount) / 60000 );
				return;
			}

			if( pUser->m_nCheerPoint == MAX_CHEERPOINT )
				pUser->SetCheerParam( pUser->m_nCheerPoint - 1, dwTickCount, TICK_CHEERPOINT );
			else
				pUser->SetCheerParam( pUser->m_nCheerPoint - 1, dwTickCount, pUser->m_dwTickCheer - dwTickCount );

			FLOAT fAngle = GetDegree(pTarget->GetPos(), pUser->GetPos());
			pUser->SetAngle(fAngle);

			if( pTarget->GetSex() == pUser->GetSex() )
			{
				((CUser*)pTarget)->AddDefinedText( TID_CHEER_MESSAGE3, "%s", pUser->GetName() );
				pUser->SendActMsg( OBJMSG_MOTION, MTI_CHEERSAME, ANILOOP_1PLAY );
			}
			else
			{
				((CUser*)pTarget)->AddDefinedText( TID_CHEER_MESSAGE4, "%s", pUser->GetName() );
				pUser->SendActMsg( OBJMSG_MOTION, MTI_CHEEROTHER, ANILOOP_1PLAY );
			}

			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_CHEERSENDEFFECT );
			g_UserMng.AddCreateSfxObj(pTarget, XI_CHEERRECEIVEEFFECT );


			CMover* pSrc = (CMover*)pUser ;
			g_UserMng.AddMoverBehavior( pSrc, pSrc->GetPos(), pSrc->m_pActMover->m_vDelta,
				pSrc->GetAngle(), pSrc->m_pActMover->GetState(), pSrc->m_pActMover->GetStateFlag(), 
				pSrc->m_dwMotion, pSrc->m_pActMover->m_nMotionEx, pSrc->m_pModel->m_nLoop, pSrc->m_dwMotionOption, g_TickCount.GetTickCount(), TRUE );

			ItemProp *pItemProp = prj.GetItemProp( II_CHEERUP ); // 응원 아이템
			if( pItemProp )
				pTarget->DoApplySkill( pTarget, pItemProp, NULL );
		}
		else
		{
			pUser->AddDefinedText( TID_CHEER_NO2, "" );	
		}
	}
}

void CDPSrvr::OnQueryEquip( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objid;
	ar >> objid;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CUser* pUsertmp		= prj.GetUser( objid );
		if( IsValidObj( pUsertmp ) )
		{
			
			if( pUsertmp->IsMode( EQUIP_DENIAL_MODE ) && pUser->IsAuthHigher( AUTH_GAMEMASTER ) == FALSE )
			{
				pUser->AddDefinedText( TID_DIAG_0088 );
				return;
			}
			pUser->AddQueryEquip( pUsertmp );
		}
	}
}

void CDPSrvr::OnQueryEquipSetting( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BOOL bAllow;
	ar >> bAllow;

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( bAllow )
			pUser->SetNotMode( EQUIP_DENIAL_MODE );
		else
			pUser->SetMode( EQUIP_DENIAL_MODE );
		g_UserMng.AddModifyMode( pUser );
	}
}

#if __VER >= 12 // __UPDATE_OPT
void CDPSrvr::OnOptionEnableRenderMask( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		BOOL bEnable;
		ar >> bEnable;
		if( bEnable )
			pUser->SetNotMode( MODE_OPTION_DONT_RENDER_MASK );
		else
			pUser->SetMode( MODE_OPTION_DONT_RENDER_MASK );
#ifdef __AEGON_FIXES
		g_UserMng.AddModifyMode(pUser, FALSE);
#else
		g_UserMng.AddModifyMode( pUser );
#endif
	}
}
#endif	// __UPDATE_OPT

void CDPSrvr::OnReturnScroll( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int nSelect;
	ar >> nSelect;
	if( nSelect < -1 || nSelect > 2 )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( nSelect >= 0 )
	{
		static char* szPos[] = { "flaris",	"saintmorning",	"darkon" };
		pUser->m_lpszVillage = szPos[nSelect];
		pUser->AddReturnScroll();	//응답을 보내면 '귀환의 두루마리'아이템을 사용한다.	
	}
	else
	{
		// 저장된 위치로 돌아가기 
		if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_RETURN ) )
			pUser->DoUseItemVirtual( II_SYS_SYS_SCR_RETURN, TRUE );
	}
}

void CDPSrvr::OnEndSkillQueue( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
		pUser->m_playTaskBar.OnEndSkillQueue( pUser );
}

void CDPSrvr::OnQueryPostMail( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	char lpszReceiver[MAX_PLAYER]	= { 0, };
	char lpszTitle[MAX_MAILTITLE]	= { 0, };
	char lpszText[MAX_MAILTEXT]		= { 0, };
	BYTE nItem;
	short nItemNum;
	int nGold;

	ar >> nItem >> nItemNum;
	ar.ReadString( lpszReceiver, MAX_PLAYER );
	
	ar >> nGold;
	ar.ReadString( lpszTitle, MAX_MAILTITLE );
	ar.ReadString( lpszText, MAX_MAILTEXT );

	DWORD nPostGold = 100;

	if( ::GetLanguage() != LANG_KOR )
		nPostGold = 500;
					
	if( nItem < 0 || nGold < 0 || ( nGold + nPostGold ) <= 0 )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bChat == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_01), COLOR_ERROR);
			return;
		}
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU
		
#ifdef __BLOCK_STAFF_ACTIONS
		if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
		{
			pUser->AddText("[Titanium] Unauthorized Action. (Postbox)", COLOR_ERROR);
			return;
		}
#endif //__BLOCK_STAFF_ACTIONS

		if( pUser->m_vtInfo.GetOther() )
			return;
		
		if( pUser->m_vtInfo.VendorIsVendor() )
			return;
		
		if( pUser->m_bBank )
			return;
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE
#ifdef __S_SERVER_UNIFY
		if( pUser->m_bAllAction == FALSE )
			return;
#endif // __S_SERVER_UNIFY

#if __VER >= 11 // __SYS_PLAYER_DATA
		u_long idReceiver	= CPlayerDataCenter::GetInstance()->GetPlayerId( lpszReceiver );
#else	// __SYS_PLAYER_DATA
		u_long idReceiver	= prj.GetPlayerID( lpszReceiver );
#endif	// __SYS_PLAYER_DATA
		if( idReceiver > 0 )
		{
#if __VER >= 11 // __SYS_PLAYER_DATA
			if( CPlayerDataCenter::GetInstance()->GetPlayerId( (char*)pUser->GetName() ) == idReceiver )
#else	// __SYS_PLAYER_DATA
			if( prj.GetPlayerID( pUser->GetName() ) == idReceiver )
#endif	// __SYS_PLAYER_DATA
			{
				pUser->AddDiagText(prj.GetText(TID_GAME_MSGSELFSENDERROR));
				return;
			}

			if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_POST, pUser->GetWorld(), pUser->GetPos() ) 
#ifdef __PREMIUM
				&& !pUser->IsPremium() && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)
#endif // __PREMIUM
				)
			{
				return;
			}
			CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );
			if( pMailBox && pMailBox->size() >= MAX_MAIL )
			{
				pUser->AddDefinedText( TID_GAME_MAILBOX_FULL, "%s", lpszReceiver );
				return;
			}

			CItemElem* pItemElem	= pUser->m_Inventory.GetAtId( nItem );
			if( pItemElem )
			{
				if( IsUsableItem( pItemElem ) == FALSE )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
					return;
				}
				if( pUser->m_Inventory.IsEquip( pItemElem->m_dwObjId ) )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
					return;
				}
				if( pItemElem->IsQuest() )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
					return;
				}
#ifdef __SYS_LOCK_ITEM
				if (pItemElem->IsItemLocked())
				{
					CString str;
					str.Format("Please unlock %s before mailing it.", pItemElem->GetProp()->szName);
					pUser->AddText(str, COLOR_WARNING);
					return;
				}
#endif // __SYS_LOCK_ITEM
				if( pItemElem->IsBinds() )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
					return;
				}
				if( pUser->IsUsing( pItemElem ) )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_DO_USINGITEM ) );
					return;
				}
				ItemProp* pProp	= pItemElem->GetProp();
				if( pProp->dwItemKind3 == IK3_CLOAK && pItemElem->m_idGuild != 0 )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
					return;
				}

				if( pProp->dwParts == PARTS_RIDE && pProp->dwItemJob == JOB_VAGRANT )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
					return;
				}

				if( nItemNum <= 0 )			// hacking
					return;
				if( pItemElem->m_nItemNum < nItemNum )
					nItemNum	= pItemElem->m_nItemNum;
				if( pItemElem->IsCharged() )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
					return;
				}
			}

#ifdef __GAMEGUARD
			if (pUser->GetGold() < (int)((nPostGold + nGold)) || nGold > 2000000000)
#else // __GAMEGUARD
			if (pUser->GetGold() < (int)((nPostGold + nGold)))
#endif // __GAMEGUARD
			{
				pUser->AddDiagText(prj.GetText(TID_GAME_LACKMONEY));
				return;
			}

			pUser->AddGold((int)((int)(nPostGold + nGold) * (-1)), TRUE);	// 사용료 지급

			CItemElem	itemElem;
			if( pItemElem )
			{
				itemElem	= *pItemElem;
				itemElem.m_nItemNum		= nItemNum;
				pUser->RemoveItem( (BYTE)pItemElem->m_dwObjId, nItemNum );
				CWorld* pWorld	= pUser->GetWorld();
				if( pWorld )
#ifdef __LAYER_1015
					g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos(), pUser->GetLayer() );
#else	// __LAYER_1015
					g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos() );
#endif	// __LAYER_1015
			}

			g_dpDBClient.SendQueryPostMail( idReceiver, pUser->m_idPlayer, itemElem, nGold, lpszTitle, lpszText );
		}
		else
		{
			// input name is wrong
			pUser->AddDiagText(prj.GetText(TID_MAIL_UNKNOW));
		}
	}
}

void CDPSrvr::OnQueryRemoveMail( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long nMail;
	ar >> nMail;

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE
		CMailBox* pMailBox = CPost::GetInstance()->GetMailBox( pUser->m_idPlayer );
		if( pMailBox != NULL )
		{
			CMail* pMail = pMailBox->GetMail( nMail );
			if( pMail != NULL )
			{
				g_dpDBClient.SendQueryRemoveMail( pUser->m_idPlayer, nMail );
			}
			else
			{
				Error( "CDPSrvr::OnQueryRemoveMail - Invalid nMail. idReceiver : %07d, nMail :[%d]", pUser->m_idPlayer, nMail );
			}
		}
		else
		{
			Error( "CDPSrvr::OnQueryRemoveMail - Invalid pMailBox. idReceiver : %07d, nMail :[%d]", pUser->m_idPlayer, nMail );
		}
	}
}

void CDPSrvr::OnQueryGetMailItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long nMail;
	ar >> nMail;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU
		if( pUser->m_Inventory.GetEmptyCount() < 1 )
		{
			pUser->AddDiagText(  prj.GetText( TID_GAME_LACKSPACE ) );
			return;
		}
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE
		CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( pUser->m_idPlayer );
		if( pMailBox )
		{
			CMail* pMail = pMailBox->GetMail(nMail);
			if( pMail && pMail->m_pItemElem )
			{
				if( pUser->m_Inventory.GetEmptyCount() < 1 )
				{
					pUser->AddDiagText(  prj.GetText( TID_GAME_LACKSPACE ) );
					return;
				}
				// 기본 보관일수 지났는지를 검사하여 보관료 부과한다.
				int nDay = 0;
				DWORD dwTime = 0;
				pMail->GetMailInfo( &nDay, &dwTime );

				// 기본 보관일수가 지났다!!!
				if( (MAX_KEEP_MAX_DAY*24) - dwTime > (MAX_KEEP_BASIC_DAY*24) )
				{
					FLOAT fCustody = 0.0f;
					FLOAT fPay = 0.0f;
					fCustody = (FLOAT)( (FLOAT)( MAX_KEEP_MAX_DAY - MAX_KEEP_BASIC_DAY - nDay ) / (FLOAT)( MAX_KEEP_MAX_DAY - MAX_KEEP_BASIC_DAY ) );
					fPay = pMail->m_pItemElem->GetCost() * fCustody;
					if( fPay < 0.0f )
						fPay = 0.0f;

					if( (int)fPay > pUser->GetGold() )
					{
						pUser->AddDiagText( prj.GetText( TID_GAME_LACKMONEY ) );
						return;
					}
					pUser->AddGold( -((int)fPay) );
				}
			}
		}
		g_dpDBClient.SendQueryGetMailItem( pUser->m_idPlayer, nMail );
	}
}

void CDPSrvr::OnQueryGetMailGold( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long nMail;
	ar >> nMail;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE
		CMailBox* pMailBox = CPost::GetInstance()->GetMailBox( pUser->m_idPlayer );
		if( pMailBox != NULL )
		{
			CMail* pMail = pMailBox->GetMail( nMail );
			if( pMail != NULL )
			{
				g_dpDBClient.SendQueryGetMailGold( pUser->m_idPlayer, nMail );
			}
			else
			{
				Error( "CDPSrvr::OnQueryGetMailGold - Invalid nMail. idReceiver : %07d, nMail :[%d]", pUser->m_idPlayer, nMail );
			}
		}
		else
		{
			Error( "CDPSrvr::OnQueryGetMailGold - Invalid pMailBox. idReceiver : %07d, nMail :[%d]", pUser->m_idPlayer, nMail );
		}
	}
}

void CDPSrvr::OnQueryReadMail( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long nMail;
	ar >> nMail;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bChat == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_01), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU
		CMailBox* pMailBox = CPost::GetInstance()->GetMailBox( pUser->m_idPlayer );
		if( pMailBox != NULL )
		{
			CMail* pMail = pMailBox->GetMail( nMail );
			if( pMail != NULL )
			{
				g_dpDBClient.SendQueryReadMail( pUser->m_idPlayer, nMail );
			}
			else
			{
				Error( "CDPSrvr::OnQueryReadMail - Invalid nMail. idReceiver : %07d, nMail :[%d]", pUser->m_idPlayer, nMail );
			}
		}
		else
		{
			Error( "CDPSrvr::OnQueryReadMail - Invalid pMailBox. idReceiver : %07d, nMail :[%d]", pUser->m_idPlayer, nMail );
		}
	}
}

void CDPSrvr::OnQueryMailBox( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );

	int	nClientReqCount	= 1;

	if( IsValidObj( pUser ) )
	{
		if( pUser->CheckClientReq()== false )
		{
			nClientReqCount	= 1;
		}
		else
		{
			nClientReqCount	= pUser->GetCountClientReq();
		}

		CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( pUser->m_idPlayer );
		if( pMailBox )
		{
			switch( pMailBox->m_nStatus )
			{
				case CMailBox::data:	// 데이터가 들어있는 메일 박스면 바로 사용자 전송
					{
						if( nClientReqCount <= 1 )
						{
							pUser->AddMailBox( pMailBox );
						}
						else
						{
							g_dpDBClient.SendQueryMailBoxCount( pUser->m_idPlayer, pUser->GetCountClientReq() );
						}

					}
					break;
				case CMailBox::nodata:	// 데이터가 없는 메일 박스면 트랜스 서버에 정보 요청, 상태는 읽는 중
					{
						if( nClientReqCount >= 2 )
						{
							g_dpDBClient.SendQueryMailBoxCount( pUser->m_idPlayer, pUser->GetCountClientReq() );
						}
						else
						{
							g_dpDBClient.SendQueryMailBox( pUser->m_idPlayer );
						}

						pMailBox->m_nStatus		= CMailBox::read;
					}
					break;
				case CMailBox::read:	// 데이터를 요청하고 대기하는 상태면 무시
					{
						if( nClientReqCount >= 2 )
						{
							g_dpDBClient.SendQueryMailBoxCount( pUser->m_idPlayer, pUser->GetCountClientReq() );
						}
					}
					break;
				default: 
				{
				} 
				break;
			}
		}
		else // 월드서버에 유저의 메일박스가 없다. 트랜스에 요청
		{
			if( pUser->GetCheckTransMailBox() == FALSE )
			{
				g_dpDBClient.SendQueryMailBoxReq( pUser->m_idPlayer );
			}
			else
			{
				pUser->SendCheckMailBoxReq( FALSE );
			}
		}
	}
	else
	{
		Error( "CDPSrvr::OnQueryMailBox - Invalid User : %d", dpidUser );
	}
}

void CDPSrvr::OnGCApp( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE nState;
	ar >> nState;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __GAMEGUARD
		if (g_eLocal.GetState(EVE_GUILDWAR) == FALSE)
		{
			pUser->AddText("Guild Siege is not active", COLOR_ERROR);
			return;
		}
#endif // __GAMEGUARD

		if (pUser->IsDie() == TRUE)
		{
			return;
		}

#ifdef __V21_SIEGE_BUFFS
		if (pUser->m_idGuild == g_GuildCombatMng.m_uWinGuildId && g_GuildCombatMng.m_nWinGuildCount >= 2)
		{
			pUser->AddDiagText(prj.GetText(TID_GAME_GUILDCOMBAT_WIN_EXCESS));
			return;
		}
#endif //__V21_SIEGE_BUFFS

		CGuild *pGuild = pUser->GetGuild();

		// 길드가 없거나 길드장이 아니면 신청 불가
#ifdef __NEW_GUILD_PERM
		if (!pGuild)
		{
			pUser->AddText("You do not have a guild.", COLOR_ERROR);
			return;
		}
		CGuildMember* pMember = pGuild->GetMember(pUser->m_idPlayer);
		if (!pMember)
		{
			return;
		}
		if (pGuild == NULL || (pGuild->IsMaster(pUser->m_idPlayer) == FALSE && pGuild->IsCmdCap(pMember->m_nMemberLv, PF_GUILDCOMBAT) == FALSE))
#else //__NEW_GUILD_PERM
		if (pGuild == NULL || pGuild->IsMaster(pUser->m_idPlayer) == FALSE)
#endif //__NEW_GUILD_PERM
		{
			pUser->AddDiagText( prj.GetText( TID_GAME_GUILDCOMBAT_NOT_GUILD_LEADER ) );
			return;
		}

		if( nState == GC_IN_WINDOW )
		{
			CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
#ifdef __NEW_GUILD_PERM
			if (pGuild && (pGuild->IsMaster(pUser->m_idPlayer) || pGuild->IsCmdCap(pMember->m_nMemberLv, PF_GUILDCOMBAT)))
#else //__NEW_GUILD_PERM
			if (pGuild && pGuild->IsMaster(pUser->m_idPlayer))
#endif //__NEW_GUILD_PERM
			{
#ifdef __GAMEGUARD
				if (strlen(pGuild->m_szGuild) < 4)
				{
					pUser->AddDiagText(prj.GetText(TID_DIAG_0020));
					return;
				}
#endif // __GAMEGUARD
#if __VER >= 11 // __GUILD_COMBAT_1TO1
				// 1:1길드대전에 입찰한 길드는 입찰 불가능하다.
				int nIndex = g_GuildCombat1to1Mng.GetTenderGuildIndexByUser( pUser );
				if( nIndex != NULL_ID )
				{
					pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_ISGC1TO1TENDER );
					return;
				}
#endif // __GUILD_COMBAT_1TO1		
				pUser->AddGCWindow( g_GuildCombatMng.GetPrizePenya( 2 ), g_GuildCombatMng.GetRequstPenya( pUser->m_idGuild ), g_GuildCombatMng.m_nJoinPanya );
			}
		}
		else if( nState == GC_IN_APP )
		{
			DWORD dwPenya;
			ar >> dwPenya;
			g_GuildCombatMng.GuildCombatRequest( pUser, dwPenya );	// 
		}
	}
}
void CDPSrvr::OnGCCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
#ifdef __GAMEGUARD
		if (g_eLocal.GetState(EVE_GUILDWAR) == FALSE)
		{
			pUser->AddText("Guild Siege is not active", COLOR_ERROR);
			return;
		}
#endif // __GAMEGUARD
	g_GuildCombatMng.GuildCombatCancel( pUser );
}
void CDPSrvr::OnGCRequestStatus( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
#ifdef __GAMEGUARD
	if (g_eLocal.GetState(EVE_GUILDWAR) == FALSE)
	{
		pUser->AddText("Guild Siege is not active", COLOR_ERROR);
		return;
	}
#endif // __GAMEGUARD
	if( IsValidObj( pUser ) )
		pUser->AddGCRequestStatus( g_GuildCombatMng.GetPrizePenya( 2 ), g_GuildCombatMng.vecRequestRanking );
}
void CDPSrvr::OnGCSelectPlayer( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BOOL bWindow;
	ar >> bWindow;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __GAMEGUARD
		if (g_eLocal.GetState(EVE_GUILDWAR) == FALSE)
		{
			pUser->AddText("Guild Siege is not active", COLOR_ERROR);
			return;
		}
#endif // __GAMEGUARD

		// 캐릭터를 선택할수 있는 시간인지 검사
		if( g_GuildCombatMng.m_nGCState != CGuildCombat::NOTENTER_COUNT_STATE ) 
		{
			pUser->AddText(prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_MAKEUP), COLOR_ERROR);
			return;
		}

		// 캐릭터를 선택할수 있는 길드 인지 검사
		if( g_GuildCombatMng.IsRequestWarGuild( pUser->m_idGuild, FALSE ) == FALSE )
		{
			pUser->AddText(prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_MAKEUP_FAIL), COLOR_ERROR);	//길드대전 입찰을 하지 않았거나 최종 선발 길드순위에 들지 못했습니다.		
			return;
		}

		CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
		// 캐릭터를 선택할수 잇는것은 마스터와 킹핀급이다.
		BOOL bMK = FALSE;
		if( pGuild )
		{
			CGuildMember* pGuildMember = pGuild->GetMember( pUser->m_idPlayer );
			if( pGuildMember )
			{
#ifdef __NEW_GUILD_PERM
				if (pGuildMember->m_nMemberLv == GUD_KINGPIN || pGuildMember->m_nMemberLv == GUD_MASTER || pGuild->IsCmdCap(pGuildMember->m_nMemberLv, PF_GUILDCOMBAT)) // Select players for combat
					bMK = TRUE;
#else //__NEW_GUILD_PERM
				if (pGuildMember->m_nMemberLv == GUD_KINGPIN || pGuildMember->m_nMemberLv == GUD_MASTER)
					bMK = TRUE;
#endif //__NEW_GUILD_PERM
			}
		}
		
		
		if( pGuild && bMK )
		{
			if( bWindow == FALSE )
			{
				// 윈도우 메세지가 아니므로 Settting
				int nSize;
				u_long uidPlayer, uidDefender;
				vector< u_long > vecSelectPlayer;
				BOOL bMasterOrKinpin = FALSE;
				BOOL bDefender = FALSE;
				BOOL bLevel = FALSE;
				BOOL bLogOut = FALSE;
				BOOL bGuildMember = FALSE;
				BOOL bMastertoDefender = FALSE;
				vecSelectPlayer.clear();
				ar >> uidDefender;
				ar >> nSize;
				if( nSize > g_GuildCombatMng.m_nMaxJoinMember )
					return;

				for( int i = 0 ; i < nSize ; ++i )
				{
					ar >> uidPlayer;
					vecSelectPlayer.push_back( uidPlayer );
					CUser* pUsertmp = g_UserMng.GetUserByPlayerID( uidPlayer );
					if( IsValidObj( pUsertmp ) )
					{
						CGuildMember* pGuildMember = pGuild->GetMember( uidPlayer );

						if( pGuildMember )
						{
							if (pGuild->IsMaster(pUsertmp->m_idPlayer))
							{
								bMasterOrKinpin = TRUE;
							}
							if( pGuildMember->m_nMemberLv == GUD_KINGPIN )
							{
								bMasterOrKinpin = TRUE;
							}
							if (pUsertmp->m_idPlayer == uidDefender)
							{
								bDefender = TRUE;
							}
#ifdef __NEW_GUILD_PERM
							if (pGuild->IsCmdCap(pGuildMember->m_nMemberLv, PF_GUILDCOMBAT))
							{
								bMasterOrKinpin = TRUE;
							}
#endif //__NEW_GUILD_PERM
						}
					}
				}
				// 마스터는 디펜더가 될수 없음.
				if( 1 < nSize && pGuild->IsMaster( uidDefender ) )
					bMastertoDefender = TRUE;

				if( bMasterOrKinpin && bMastertoDefender == FALSE 
#ifndef _DEBUG
					&& bDefender
#endif // _DEBUG
					)
				{
					g_GuildCombatMng.SelectPlayerClear( pUser->m_idGuild );
					for( int veci = 0 ; veci < (int)( vecSelectPlayer.size() ) ; ++veci )
					{
						// 최대 인원수 이상은 안들어가짐.
						if( veci >= g_GuildCombatMng.m_nMaxJoinMember )
							break;

						u_long uidSelectPlayer = vecSelectPlayer[veci];
						CUser* pUsertmp = g_UserMng.GetUserByPlayerID( uidSelectPlayer );
						{
							if( IsValidObj( pUsertmp ) )
							{
								CGuildMember* pGuildMember = pGuild->GetMember( uidPlayer );
								// 길드의 맴버
								if( pGuildMember )
								{
									// 레벨이 30이상만 참여가능
									if( 30 <= pUsertmp->GetLevel() )
									{
										g_GuildCombatMng.AddSelectPlayer( pUser->m_idGuild, uidSelectPlayer );
									}
								}
							}
						}						
					}
					g_GuildCombatMng.SetDefender( pUser->m_idGuild, uidDefender );			
#ifndef __FIX_TIMER_GW
					WriteLog( "GuildCombat SelectPlayer GuildID=%d", pUser->m_idGuild );
#endif //__FIX_TIMER_GW
				}
			}

			vector<CGuildCombat::__JOINPLAYER> vecSelectList;
			g_GuildCombatMng.GetSelectPlayer( pUser->m_idGuild, vecSelectList );
			pUser->AddGCSelectPlayerWindow( vecSelectList, g_GuildCombatMng.GetDefender(pUser->m_idGuild), bWindow, g_GuildCombatMng.IsRequestWarGuild(pUser->m_idGuild, FALSE) );
			if( bWindow == FALSE )
			{
				g_UserMng.AddGCGuildStatus( pUser->m_idGuild );
				g_UserMng.AddGCWarPlayerlist( pUser->m_idGuild );
			}
		}
	}
}
void CDPSrvr::OnGCSelectMap( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int nMap;
	ar >> nMap;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
#ifdef __GAMEGUARD
	if (g_eLocal.GetState(EVE_GUILDWAR) == FALSE)
	{
		pUser->AddText("Guild Siege is not active", COLOR_ERROR);
		return;
	}
#endif // __GAMEGUARD
	g_GuildCombatMng.SetSelectMap( pUser, nMap );
}
void CDPSrvr::OnGCJoin( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __GAMEGUARD
		if (g_eLocal.GetState(EVE_GUILDWAR) == FALSE)
		{
			pUser->AddText("Guild Siege is not active", COLOR_ERROR);
			return;
		}
#endif // __GAMEGUARD
		if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_GUILDWAR_JOIN, pUser->GetWorld(), pUser->GetPos() ) )
			return;
		g_GuildCombatMng.GuildCombatEnter( pUser );
#ifdef __GAMEGUARD	
		g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
#endif // __GAMEGUARD
	}
}
void CDPSrvr::OnGCGetPenyaGuild( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __GAMEGUARD
		if (g_eLocal.GetState(EVE_GUILDWAR) == FALSE)
		{
			pUser->AddText("Guild Siege is not active", COLOR_ERROR);
			return;
		}
#endif // __GAMEGUARD
		CGuild *pGuild = pUser->GetGuild();		
		if( pGuild && pGuild->IsMaster( pUser->m_idPlayer ) )
		{
			// 신청금액 및 보상이 있는가?
			__int64 nGetPenya = 0;
			BOOL bFind = FALSE;
			CGuildCombat::__GCRESULTVALUEGUILD ResultValueGuild;
			for( int veci = 0 ; veci < (int)( g_GuildCombatMng.m_GCResultValueGuild.size() ) ; ++veci )
			{
				ResultValueGuild = g_GuildCombatMng.m_GCResultValueGuild[veci];
				if( pGuild->m_idGuild == ResultValueGuild.uidGuild )
				{
					bFind = TRUE;
					nGetPenya = ResultValueGuild.nReturnCombatFee + ResultValueGuild.nReward;
					break;
				}
			}

			if( bFind )
			{
				__int64 nTotal = (__int64)pUser->GetGold() + nGetPenya;
				if( nGetPenya > INT_MAX || nTotal > INT_MAX )
					pUser->AddGCGetPenyaGuild( 3, nGetPenya );
				else
					g_dpDBClient.SendGCGetPenyaGuild( pUser->m_idPlayer, ResultValueGuild.nCombatID, ResultValueGuild.uidGuild );
			}
			else
			{
				pUser->AddGCGetPenyaGuild( 2, nGetPenya );
			}
		}
	}
}

void CDPSrvr::OnGCGetPenyaPlayer( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __GAMEGUARD
		if (g_eLocal.GetState(EVE_GUILDWAR) == FALSE)
		{
			pUser->AddText("Guild Siege is not active", COLOR_ERROR);
			return;
		}
#endif // __GAMEGUARD
		// 보상이 있는가?
		__int64 nGetPenya = 0;
		BOOL bFind = FALSE;
		CGuildCombat::__GCRESULTVALUEPLAYER ResultValuePlayer;
		for( int veci = 0 ; veci < (int)( g_GuildCombatMng.m_GCResultValuePlayer.size() ) ; ++veci )
		{
			ResultValuePlayer = g_GuildCombatMng.m_GCResultValuePlayer[veci];
			if( pUser->m_idPlayer == ResultValuePlayer.uidPlayer )
			{
				bFind = TRUE;
				nGetPenya = ResultValuePlayer.nReward;
				break;
			}
		}

		if( bFind )
		{
			__int64 nTotal = (__int64)pUser->GetGold() + nGetPenya;
			if( nGetPenya > INT_MAX || nTotal > INT_MAX )
				pUser->AddGCGetPenyaPlayer( 2, nGetPenya );
			else
				g_dpDBClient.SendGCGetPenyaPlayer( pUser->m_idPlayer, ResultValuePlayer.nCombatID, ResultValuePlayer.uidGuild );
		}
		else
		{
			pUser->AddGCGetPenyaPlayer( 1, nGetPenya );
		}
	}
}

#if __VER < 8 // #ifndef __GUILDCOMBAT_85
void CDPSrvr::OnGCGetItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CGuild *pGuild = pUser->GetGuild();		
		if( pGuild && pGuild->IsMember( pUser->m_idPlayer ) )
		{
			if( pGuild->GetGuildId() != g_GuildCombatMng.m_uWinGuildId )
				return;

			if( g_GuildCombatMng.m_nItemPenya > pUser->GetGold() )
			{
				pUser->AddText( prj.GetText(TID_GAME_LACKMONEY) ); //페냐(돈)가 부족합니다.
				return;
			}

			if( g_GuildCombatMng.m_nGCState != CGuildCombat::WAR_CLOSE_STATE && g_GuildCombatMng.m_nGCState != CGuildCombat::WAR_TELEPORT_STATE )
			{
				pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_GETITEM) ); // 길드대전 중에는 사용할수 없습니다.
				return;
			}

			// 대전 상품 지급
			CItemElem itemElem;
			
			if( g_GuildCombatMng.m_nWinGuildCount == 1 )
				itemElem.m_dwItemId	= II_ARM_S_CLO_CLO_DRAGON1;		// 용망토 생성
			else if( g_GuildCombatMng.m_nWinGuildCount == 2 )
				itemElem.m_dwItemId	= II_ARM_S_CLO_CLO_DRAGON2;		// 용망토 생성
			else if( g_GuildCombatMng.m_nWinGuildCount == 3 )
				itemElem.m_dwItemId	= II_ARM_S_CLO_CLO_DRAGON3;		// 용망토 생성
			else if( g_GuildCombatMng.m_nWinGuildCount >= 4 )
				itemElem.m_dwItemId	= II_ARM_S_CLO_CLO_DRAGON4;		// 용망토 생성
			itemElem.m_nItemNum		= 1;
			itemElem.m_idGuild	= pGuild->m_idGuild;
			itemElem.SetSerialNumber();
			ItemProp* pItemProp		= itemElem.GetProp();
			if( pItemProp )
				itemElem.m_nHitPoint	= pItemProp->dwEndurance;
			else
				itemElem.m_nHitPoint	= 0;

			CTime tNextCombat = g_GuildCombatMng.GetNextGuildCobmatTime();
			itemElem.m_dwKeepTime = tNextCombat.GetTime();
			
			if( pUser->CreateItem( &itemElem ) )
			{
				LogItemInfo aLogItem;
				aLogItem.Action = "9";
				aLogItem.SendName = pUser->GetName();
				aLogItem.RecvName = "GUILDCOMBAT";
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = pUser->GetGold();
				aLogItem.Gold2 = pUser->GetGold() - g_GuildCombatMng.m_nItemPenya;
				OnLogItem( aLogItem, &itemElem, 1 );		// why do not pass &itemElem as argument?
				pUser->AddGold( -g_GuildCombatMng.m_nItemPenya );	
			}
			else
			{
				pUser->AddDefinedText( TID_GAME_LACKSPACE, "" );
			}
		}
	}
}
#endif // __VER < 8
void CDPSrvr::OnGCTele( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __GAMEGUARD
		if (g_eLocal.GetState(EVE_GUILDWAR) == FALSE)
		{
			pUser->AddText("Guild Siege is not active", COLOR_ERROR);
			return;
		}

		if (pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR)
#endif // __GAMEGUARD
		{
			PRegionElem pRgnElem = g_WorldMng.GetRevivalPos(WI_WORLD_MADRIGAL, "flaris");
			if (pRgnElem)
				((CMover*)pUser)->REPLACE(g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3(6983.0f, 0.0f, 3330.0f), REPLACE_NORMAL, nDefaultLayer);
		}
	}
}
void CDPSrvr::OnGCPlayerPoint( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
#ifdef __GAMEGUARD
	if (g_eLocal.GetState(EVE_GUILDWAR) == FALSE)
	{
		pUser->AddText("Guild Siege is not active", COLOR_ERROR);
		return;
	}
#endif // __GAMEGUARD
	pUser->AddGCPlayerPoint();
}

void CDPSrvr::OnSummonFriend( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwData;
	char lpszPlayer[MAX_PLAYER]	= { 0, };
	
	ar >> dwData;
	ar.ReadString( lpszPlayer, MAX_PLAYER );

	if( strlen(lpszPlayer) >= MAX_NAME )
		return;
	
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int nState = pUser->GetSummonState();
		if( nState != 0 )
		{
			DWORD dwMsgId = 0;
			if( nState == 1 )	// 거래중
				dwMsgId = TID_GAME_TRADE_NOTUSE;
			else if( nState == 2 ) // 죽음
				dwMsgId = TID_GAME_DIE_NOTUSE;
			else if( nState == 3 ) // 개인상점 중
				dwMsgId = TID_GAME_VENDOR_NOTUSE;
			else if( nState == 4 ) // 전투중
				dwMsgId = TID_GAME_ATTACK_NOTUSE;
			else if( nState == 5 ) // 비행중
				dwMsgId = TID_GAME_FLY_NOTUSE;
#if __VER >= 8 // 8차 듀얼 061226 ma
			else if( nState == 6 ) // 듀얼중
				dwMsgId = TID_GAME_ATTACK_NOTUSE;
#endif // __VER >= 8 // 8차 듀얼 061226 ma

			pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText( dwMsgId ) );
			return;
		}

		WORD wId	= LOWORD( dwData );
		WORD wMode	= HIWORD( dwData );
		CItemElem* pItemElem = (CItemElem*)pUser->GetItemId( wId );
		if( IsUsableItem( pItemElem ) )
		{
			if( pItemElem->m_dwItemId != II_SYS_SYS_SCR_FRIENDSUMMON_A && pItemElem->m_dwItemId != II_SYS_SYS_SCR_FRIENDSUMMON_B )
				return;
			if( pItemElem->m_bQuery )
				return;

#if __VER >= 11 // __SYS_PLAYER_DATA
			CUser* pUsertmp = g_UserMng.GetUserByPlayerID( CPlayerDataCenter::GetInstance()->GetPlayerId( lpszPlayer ) );
#else	// __SYS_PLAYER_DATA
			CUser* pUsertmp = g_UserMng.GetUserByPlayerID( prj.GetPlayerID( lpszPlayer ) );
#endif	// __SYS_PLAYER_DATA
			if( IsValidObj( (CObj*)pUsertmp ) )
			{
				nState = pUsertmp->GetSummonState();
				if( nState != 0 )
				{
					DWORD dwMsgId = 0;
					if( nState == 1 )	// 거래중
						dwMsgId = TID_GAME_TRADE_NOTUSE1;
					else if( nState == 2 ) // 죽음
						dwMsgId = TID_GAME_DIE_NOTUSE1;
					else if( nState == 3 ) // 개인상점 중
						dwMsgId = TID_GAME_VENDOR_NOTUSE1;
					else if( nState == 4 ) // 전투중
						dwMsgId = TID_GAME_ATTACK_NOTUSE1;
					else if( nState == 5 ) // 비행중
						dwMsgId = TID_GAME_FLY_NOTUSE1;
#if __VER >= 8 // 8차 듀얼 061226 ma
					else if( nState == 6 ) // 듀얼중
						dwMsgId = TID_GAME_ATTACK_NOTUSE1;
#endif // __VER >= 8 // 8차 듀얼 061226 ma
					
					pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText( dwMsgId ) );
				}
				else if( pUser->m_idPlayer == pUsertmp->m_idPlayer )
				{
					pUser->AddDefinedText( TID_GAME_SUMMON_FRIEND_MY_NOUSE );
				}
				else if( prj.IsGuildQuestRegion( pUser->GetPos() ) )
				{
					pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE ) );
				}
				else if( prj.IsGuildQuestRegion( pUsertmp->GetPos() ) )
				{
					pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE1 ) );
				}
				else if( pUser->GetWorld()->GetID() != pUsertmp->GetWorld()->GetID()
#if __VER >= 14 // __INSTANCE_DUNGEON
						|| pUser->GetLayer() != pUsertmp->GetLayer()
#endif // __INSTANCE_DUNGEON
					)
				{
					CString strtmp;
					strtmp.Format( prj.GetText( TID_GAME_WORLD_NOTUSE ), pUser->GetWorld()->m_szWorldName, pUsertmp->GetWorld()->m_szWorldName );
					pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", strtmp );
				}
#if __VER >= 13 // __RAINBOW_RACE
				else if( CRainbowRaceMng::GetInstance()->IsEntry( pUser->m_idPlayer )
						|| CRainbowRaceMng::GetInstance()->IsEntry( pUsertmp->m_idPlayer ) )
				{
					pUser->AddDefinedText( TID_GAME_RAINBOWRACE_NOTELEPORT );
					return;
				}
#endif // __RAINBOW_RACE
				else
				{
#ifdef __RT_1025
					if( pUsertmp->m_RTMessenger.IsBlock( pUser->m_idPlayer ) )
#else	//__RT_1025
					LPFRIEND pFriend	= pUsertmp->m_Messenger.GetFriend( pUser->m_idPlayer );
					if( pFriend && pFriend->dwState == FRS_BLOCK )
#endif	// __RT_1025
					{
						pUser->AddDefinedText( TID_ERROR_SUMMONFRIEND_NOUSER, "\"%s\"", lpszPlayer );
						return;
					}
					pItemElem->m_bQuery		= TRUE;
					pUsertmp->AddSummonFriendConfirm( pUser->GetId(), dwData, pUser->GetName(), pUser->GetWorld()->m_szWorldName );
					pUser->AddDefinedText( TID_GAME_SUMMONFRIEND_CONFIRM, "\"%s\"", lpszPlayer );
				}
			}
			else
			{
				pUser->AddDefinedText( TID_ERROR_SUMMONFRIEND_NOUSER, "\"%s\"", lpszPlayer );
			}
		}	
		else
		{
			ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_FRIENDSUMMON_A );
			if( pItemProp )
				pUser->AddDefinedText( TID_ERROR_SUMMONFRIEND_NOITEM, "\"%s\" \"%s\"", pItemProp->szName, lpszPlayer );
		}
	}
}
void CDPSrvr::OnSummonFriendConfirm( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objid;
	DWORD dwData;

	ar >> objid >> dwData;
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CUser* pUsertmp = prj.GetUser( objid );
		if( IsValidObj( (CObj*)pUsertmp ) )
		{
#ifdef __MOUNT
			if (pUser->m_pActMover->IsUseMount())
			{
				pUser->AddText("You can't teleport while using a mount.", COLOR_ERROR);
				return;
			}
#endif // __MOUNT

			int nState = pUser->GetSummonState();
			if( nState != 0 )
			{
				DWORD dwMsgId = 0;
				if( nState == 1 )	// 거래중
					dwMsgId = TID_GAME_TRADE_NOTUSE;
				else if( nState == 2 ) // 죽음
					dwMsgId = TID_GAME_DIE_NOTUSE;
				else if( nState == 3 ) // 개인상점 중
					dwMsgId = TID_GAME_VENDOR_NOTUSE;
				else if( nState == 4 ) // 전투중
					dwMsgId = TID_GAME_ATTACK_NOTUSE;
				else if( nState == 5 ) // 비행중
					dwMsgId = TID_GAME_FLY_NOTUSE;
#if __VER >= 8 // 8차 듀얼 061226 ma
				else if( nState == 6 ) // 듀얼중
					dwMsgId = TID_GAME_ATTACK_NOTUSE;
#endif // __VER >= 8 // 8차 듀얼 061226 ma
				
				pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", prj.GetText( dwMsgId ) );
				pUsertmp->AddDefinedText( TID_GAME_STATE_NOTSUMMON, "\"%s\"", prj.GetText( dwMsgId + 1 ) );
				return;
			}

			nState = pUsertmp->GetSummonState();
			if( nState != 0 )
			{
				DWORD dwMsgId = 0;
				if( nState == 1 )	// 거래중
					dwMsgId = TID_GAME_TRADE_NOTUSE1;
				else if( nState == 2 ) // 죽음
					dwMsgId = TID_GAME_DIE_NOTUSE1;
				else if( nState == 3 ) // 개인상점 중
					dwMsgId = TID_GAME_VENDOR_NOTUSE1;
				else if( nState == 4 ) // 전투중
					dwMsgId = TID_GAME_ATTACK_NOTUSE1;
				else if( nState == 5 ) // 비행중
					dwMsgId = TID_GAME_FLY_NOTUSE1;
#if __VER >= 8 // 8차 듀얼 061226 ma
				else if( nState == 6 ) // 듀얼중
					dwMsgId = TID_GAME_ATTACK_NOTUSE1;
#endif // __VER >= 8 // 8차 듀얼 061226 ma
				
				pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK , "\"%s\"", prj.GetText( dwMsgId ) );
				pUsertmp->AddDefinedText( TID_GAME_STATE_NOTSUMMON , "\"%s\"", prj.GetText( dwMsgId - 1 ) );
				return;
			}

			WORD wId	= LOWORD( dwData );
			WORD wMode	= HIWORD( dwData );
			CItemElem* pItemElem = (CItemElem*)pUsertmp->GetItemId( wId );
			if( IsUsableItem( pItemElem ) )
			{
				if( ( pItemElem->m_dwItemId != II_SYS_SYS_SCR_FRIENDSUMMON_A && pItemElem->m_dwItemId != II_SYS_SYS_SCR_FRIENDSUMMON_B ) || pItemElem->m_bQuery == FALSE )
					return;

				if( prj.IsGuildQuestRegion( pUser->GetPos() ) )
				{
					pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK , "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE ) );
					pUsertmp->AddDefinedText( TID_GAME_STATE_NOTSUMMON , "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE1 ) );
				}
				else if( prj.IsGuildQuestRegion( pUsertmp->GetPos() ) )
				{
					pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK , "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE1 ) );
					pUsertmp->AddDefinedText( TID_GAME_STATE_NOTSUMMON , "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE ) );
				}
				else if( pUser->GetWorld()->GetID() != pUsertmp->GetWorld()->GetID()
#if __VER >= 14 // __INSTANCE_DUNGEON
						|| pUser->GetLayer() != pUsertmp->GetLayer()
#endif // __INSTANCE_DUNGEON
					)
				{
					CString strtmp;
					strtmp.Format( prj.GetText( TID_GAME_WORLD_NOTUSE ), pUser->GetWorld()->m_szWorldName, pUsertmp->GetWorld()->m_szWorldName );
					pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", strtmp );
					strtmp.Format( prj.GetText( TID_GAME_WORLD_NOTUSE ), pUsertmp->GetWorld()->m_szWorldName, pUser->GetWorld()->m_szWorldName );
					pUsertmp->AddDefinedText( TID_GAME_STATE_NOTSUMMON, "\"%s\"", strtmp );
				}
				else 
				{
					g_dpDBClient.SendLogSMItemUse( "1", pUsertmp, pItemElem, pItemElem->GetProp(), pUser->GetName() );

					pUser->REPLACE( g_uIdofMulti, pUsertmp->GetWorld()->GetID(), pUsertmp->GetPos(), REPLACE_NORMAL, pUsertmp->GetLayer() );
					pItemElem->m_bQuery		= FALSE;
					pUsertmp->RemoveItem( (BYTE)( wId ), (short)1 );
					pUser->AddDefinedText( TID_GAME_SUMMON_SUCCESS1, "\"%s\"", pUsertmp->GetName() );
					pUsertmp->AddDefinedText( TID_GAME_SUMMON_SUCCESS, "\"%s\"", pUser->GetName() );
				}
			}
			else
			{
				ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_FRIENDSUMMON_A );
				if( pItemProp )
					pUsertmp->AddDefinedText( TID_ERROR_SUMMONFRIEND_NOITEM, "\"%s\" \"%s\"", pItemProp->szName, pUser->GetName() );
			}
		}
	}
}

void CDPSrvr::OnSummonFriendCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objid;
	DWORD dwData;
	ar >> objid >> dwData;
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CUser* pUsertmp = prj.GetUser( objid );
		if( IsValidObj( (CObj*)pUsertmp ) )
		{
			WORD wId	= LOWORD( dwData );
			CItemElem* pItemElem = (CItemElem*)pUsertmp->GetItemId( wId );
			if( IsUsableItem( pItemElem ) && ( pItemElem->m_dwItemId == II_SYS_SYS_SCR_FRIENDSUMMON_A || pItemElem->m_dwItemId == II_SYS_SYS_SCR_FRIENDSUMMON_B ) )
				pItemElem->m_bQuery		= FALSE;
			pUsertmp->AddDefinedText( TID_GAME_SUMMON_FRIEND_CANCEL, "\"%s\"", pUser->GetName() );
		}
	}
}

void CDPSrvr::OnSummonParty( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwData;
	
	ar >> dwData;

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int nState = pUser->GetSummonState();
		if( nState != 0 )
		{
			DWORD dwMsgId = 0;
			if( nState == 1 )	// 거래중
				dwMsgId = TID_GAME_TRADE_NOTUSE;
			else if( nState == 2 ) // 죽음
				dwMsgId = TID_GAME_DIE_NOTUSE;
			else if( nState == 3 ) // 개인상점 중
				dwMsgId = TID_GAME_VENDOR_NOTUSE;
			else if( nState == 4 ) // 전투중
				dwMsgId = TID_GAME_ATTACK_NOTUSE;
			else if( nState == 5 ) // 비행중
				dwMsgId = TID_GAME_FLY_NOTUSE;
#if __VER >= 8 // 8차 듀얼 061226 ma
			else if( nState == 6 ) // 듀얼중
				dwMsgId = TID_GAME_ATTACK_NOTUSE;
#endif // __VER >= 8 // 8차 듀얼 061226 ma
			
			pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText( dwMsgId ) );
			return;
		}

		if( prj.IsGuildQuestRegion( pUser->GetPos() ) )
		{
			pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE ) );
			return;
		}
#if __VER >= 13 // __RAINBOW_RACE
		if( CRainbowRaceMng::GetInstance()->IsEntry( pUser->m_idPlayer ) )
		{
			pUser->AddDefinedText( TID_GAME_RAINBOWRACE_NOTELEPORT );
			return;
		}
#endif // __RAINBOW_RACE

		
		WORD wId	= LOWORD( dwData );
		WORD wMode	= HIWORD( dwData );
		CItemElem* pItemElem = (CItemElem*)pUser->GetItemId( wId );

#ifdef __GAMEGUARD
		if (pItemElem && IsUsableItem(pItemElem) && pItemElem->GetProp()->dwID == II_SYS_SYS_SCR_PARTYSUMMON)
#else // __GAMEGUARD
		if (IsUsableItem(pItemElem))
#endif // __GAMEGUARD
		{
			CParty* pParty;
			pParty = g_PartyMng.GetParty( pUser->GetPartyId() );
			if( pParty && pParty->IsLeader( pUser->m_idPlayer ) )
			{
#ifdef __INCREASE_SKILL_ID_VALUE
				if (!pUser->HasBuff(BUFF_ITEM, pItemElem->GetProp()->dwID))
#else
				if (!pUser->HasBuff(BUFF_ITEM, (WORD)(pItemElem->GetProp()->dwID)))
#endif
				{
					pParty->m_dwWorldId = pUser->GetWorld()->GetID();

					g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemElem, pItemElem->GetProp(), pParty->m_sParty );
					
					ItemProp* pItemProptmp = prj.GetItemProp( II_SYS_SYS_SCR_PARTYSUMMON );
					for( int i = 1 ; i < pParty->m_nSizeofMember ; i++ )
					{
						CUser* pUsertmp		= g_UserMng.GetUserByPlayerID( pParty->GetPlayerId( i ) );
						if( IsValidObj( (CObj*)pUsertmp ) )
						{
//								if( pUser->GetWorld()->GetID() == pUsertmp->GetWorld()->GetID() && !prj.IsGuildQuestRegion( pUsertmp->GetPos() ) )
							{
								pUser->DoApplySkill( (CCtrl*)pUsertmp, pItemElem->GetProp(), NULL );
								pUsertmp->AddSummonPartyConfirm( pUser->GetId(), dwData, pUser->GetWorld()->m_szWorldName );
								if( pItemProptmp )
									g_UserMng.AddCreateSfxObj((CMover *)pUsertmp, pItemProptmp->dwSfxObj3 );
								pUser->AddDefinedText( TID_GAME_SUMMONFRIEND_CONFIRM, "\"%s\"", pUsertmp->GetName() );
							}
//								else
//								{
//									pUser->AddDefinedText( TID_ERROR_SUMMONFRIEND_NOTWORLD );
//								}
						}
						else
						{
#if __VER >= 11 // __SYS_PLAYER_DATA
							pUser->AddDefinedText( TID_ERROR_SUMMONFRIEND_NOUSER, "\"%s\"", CPlayerDataCenter::GetInstance()->GetPlayerString( pParty->GetPlayerId( i ) ) );
#else	// __SYS_PLAYER_DATA
							pUser->AddDefinedText( TID_ERROR_SUMMONFRIEND_NOUSER, "\"%s\"", prj.GetPlayerString( pParty->GetPlayerId( i ) ) );
#endif	// __SYS_PLAYER_DATA
						}
					}
					pUser->DoApplySkill( (CCtrl*)pUser, pItemElem->GetProp(), NULL );
					pUser->RemoveItem( (BYTE)( wId ), (short)1 );

					if( pItemProptmp )
						g_UserMng.AddCreateSfxObj((CMover *)pUser, pItemProptmp->dwSfxObj3 );
				}
				else
				{
					pUser->AddDefinedText( TID_GAME_LIMITED_USE );
				}
			}
		}	
		else
		{
			ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_PARTYSUMMON );
			if( pItemProp )
				pUser->AddDefinedText( TID_ERROR_SUMMONPARTY_NOITEM, "\"%s\"", pItemProp->szName );
		}
	}	
}

void CDPSrvr::OnSummonPartyConfirm( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objid;
	DWORD dwData;
	ar >> objid;
	ar >> dwData;
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __MOUNT
		if (pUser->m_pActMover->IsUseMount())
		{
			pUser->AddText("You can't teleport while using a mount.", COLOR_ERROR);
			return;
		}
#endif // __MOUNT

		CParty* pParty;
		pParty = g_PartyMng.GetParty( pUser->GetPartyId() );
		if( pParty )
		{
			CMover* pLeader = prj.GetUser( objid );
			if( IsValidObj( pLeader ) && pParty->IsLeader( pLeader->m_idPlayer ) && pLeader->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_PARTYSUMMON ) )
			{
				if( pUser == pLeader )
					return;
				int nState = pUser->GetSummonState();
				if( nState != 0 && nState != 5 )
				{
					DWORD dwMsgId = 0;
					if( nState == 1 )	// 거래중
						dwMsgId = TID_GAME_TRADE_NOTUSE;
					else if( nState == 2 ) // 죽음
						dwMsgId = TID_GAME_DIE_NOTUSE;
					else if( nState == 3 ) // 개인상점 중
						dwMsgId = TID_GAME_VENDOR_NOTUSE;
					else if( nState == 4 ) // 전투중
						dwMsgId = TID_GAME_ATTACK_NOTUSE;
#if __VER >= 8 // 8차 듀얼 061226 ma
					else if( nState == 6 ) // 듀얼중
						dwMsgId = TID_GAME_ATTACK_NOTUSE;
#endif // __VER >= 8 // 8차 듀얼 061226 ma
					
					pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK , "\"%s\"", prj.GetText( dwMsgId ) );
					return;
				}
				
				nState = pLeader->GetSummonState();
				if( nState != 0 )
				{
					DWORD dwMsgId = 0;
					if( nState == 1 )	// 거래중
						dwMsgId = TID_GAME_TRADE_NOTUSE1;
					else if( nState == 2 ) // 죽음
						dwMsgId = TID_GAME_DIE_NOTUSE1;
					else if( nState == 3 ) // 개인상점 중
						dwMsgId = TID_GAME_VENDOR_NOTUSE1;
					else if( nState == 4 ) // 전투중
						dwMsgId = TID_GAME_ATTACK_NOTUSE1;
					else if( nState == 5 ) // 비행중
						dwMsgId = TID_GAME_FLY_NOTUSE1;
#if __VER >= 8 // 8차 듀얼 061226 ma
					else if( nState == 6 ) // 듀얼중
						dwMsgId = TID_GAME_ATTACK_NOTUSE1;
#endif // __VER >= 8 // 8차 듀얼 061226 ma
					
					pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK , "\"%s\"", prj.GetText( dwMsgId ) );
					return;
				}
				
				if( prj.IsGuildQuestRegion( pLeader->GetPos() ) )
				{
					pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE1) );
				}
				else if( prj.IsGuildQuestRegion( pUser->GetPos() ) )
				{
					pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE ) );
				}
				else if( pUser->GetWorld()->GetID() != pParty->m_dwWorldId )
				{
					CWorld* pWorld = g_WorldMng.GetWorld( pParty->m_dwWorldId );
					if( pWorld )
					{
						CString strtmp;
						strtmp.Format( prj.GetText( TID_GAME_WORLD_NOTUSE ), pUser->GetWorld()->m_szWorldName, pWorld->m_szWorldName );
						pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", strtmp );
					}
				}
				else if( pParty->m_dwWorldId != pLeader->GetWorld()->GetID() )
				{
					CWorld* pWorld = g_WorldMng.GetWorld( pParty->m_dwWorldId );
					if( pWorld )
					{
						CString strtmp;
						strtmp.Format( prj.GetText( TID_GAME_WORLDLEADER_NOTUSE ) );
						pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", strtmp );
					}
				}
#if __VER >= 14 // __INSTANCE_DUNGEON
				else if( pLeader->GetLayer() != pUser->GetLayer() )
				{
					CString strtmp;
					strtmp.Format( prj.GetText( TID_GAME_WORLD_NOTUSE ) );
					pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", strtmp );
				}
#endif // __INSTANCE_DUNGEON
#if __VER >= 13 // __RAINBOW_RACE
				else if( CRainbowRaceMng::GetInstance()->IsEntry( pUser->m_idPlayer ) )
				{
					pUser->AddDefinedText( TID_GAME_RAINBOWRACE_NOTELEPORT );
				}
#endif // __RAINBOW_RACE
				else
				{
					pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_PARTYSUMMON );
					pUser->REPLACE( g_uIdofMulti, pLeader->GetWorld()->GetID(), pLeader->GetPos(), REPLACE_FORCE, pLeader->GetLayer() );
					pUser->AddDefinedText( TID_GAME_SUMMON_SUCCESS1, "\"%s\"", pLeader->GetName() );
					((CUser*)pLeader)->AddDefinedText( TID_GAME_SUMMON_SUCCESS, "\"%s\"", pUser->GetName() );					
					ItemProp* pItemProptmp = prj.GetItemProp( II_SYS_SYS_SCR_PARTYSUMMON );
					if( pItemProptmp )
						g_UserMng.AddCreateSfxObj((CMover *)pUser, pItemProptmp->dwSfxObj3 );
				}
			}
			else
			{
				pUser->AddDefinedText( TID_ERROR_SUMMONPARTY_NOTTIME );
			}
		}
		else
		{
			pUser->AddDefinedText( TID_GAME_NOPARTY );
		}
	}
}

void CDPSrvr::OnRemoveInvenItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD	dwId;
	int		nNum;
	ar >> dwId;
	ar >> nNum;

	if( nNum <= 0 )
		return;

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;
	
	CItemElem* pItemElem = pUser->m_Inventory.GetAtId( dwId );
	if( IsUsableItem( pItemElem ) == FALSE )
		return;

	if( pUser->m_Inventory.IsEquip( dwId ) )
		return;

#ifdef __MIKSIK_EQUALIZED_MODE
	if (pUser->bTmpMode && pUser->IsAuthorization(AUTH_GENERAL))
	{
		pUser->AddText("You cant use that in temporal mode!");
		return;
	}
#endif //__MIKSIK_EQUALIZED_MODE
	
	if( pUser->IsUsing( pItemElem ) )
	{
		pUser->AddDefinedText( TID_GAME_CANNOT_DO_USINGITEM );
		return;
	}
	
	if( pItemElem->m_nItemNum < nNum )
		return ;

	if (pItemElem->IsUndestructable() && !pUser->IsAuthHigher(AUTH_ADMINISTRATOR))
		return;

#ifdef __SYS_LOCK_ITEM
	if (pItemElem->IsItemLocked())
	{
		CString str;
		str.Format("Please unlock %s before deleting it.", pItemElem->GetProp()->szName);
		pUser->AddText(str, COLOR_WARNING);
		return;
	}
#endif // __SYS_LOCK_ITEM

	CString strNum;
	strNum.Format("%d", nNum );
	pUser->AddDefinedText( TID_GAME_SUCCESS_REMOVE_ITEM, "\"%s\" \"%s\"", pItemElem->GetProp()->szName, strNum );

	LogItemInfo aLogItem;
	aLogItem.Action = "*";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "GARBAGE";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	OnLogItem( aLogItem, pItemElem, nNum );

	pUser->RemoveItem( (BYTE)( dwId ), (short)nNum );
}

#if __VER >= 12 // __ITEMCREATEMON_S0602
void CDPSrvr::OnCreateMonster( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwItemIdRec;
	D3DXVECTOR3 vPos;
	ar >> dwItemIdRec;
	ar >> vPos;

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;
	
	DWORD dwId = HIWORD( dwItemIdRec );
	
	if( pUser->IsUsableState( dwId ) == FALSE )
		return;
	
#if __VER >= 12 // __NEW_ITEMCREATEMON_SERVER
	CCreateMonster::GetInstance()->CreateMonster( pUser, dwId, vPos );

#else // __NEW_ITEMCREATEMON_SERVER
	CItemElem* pItemElem = pUser->m_Inventory.GetAtId( dwId );
	if( IsUsableItem( pItemElem ) )
	{
		CWorld* pWorld	= pUser->GetWorld();
		MoverProp* pMoverProp	= NULL;

		DWORD dwMonsterId = g_CreateMonster.GetCreateMonster( pItemElem->GetProp()->dwID );
		if( dwMonsterId )
			pMoverProp	= prj.GetMoverProp( dwMonsterId );
				
		if( pWorld && pMoverProp && pMoverProp->dwID != 0 )
		{
			D3DXVECTOR3 vDist2 = pUser->GetPos() - vPos;
			float fDist = D3DXVec3Length( &vDist2 );			// 두좌표간의 거리
			if( 15.f < fDist )
				return;

			int nAttr = pWorld->GetHeightAttribute( vPos.x, vPos.z );		// 이동할 위치의 속성 읽음.
			if( nAttr == HATTR_NOWALK || nAttr == HATTR_NOMOVE )		// 못 움직이는 곳이면 Pass
				return;
			if( pUser->IsRegionAttr( RA_SAFETY ) )		// 안전지역이면 Pass
				return;
			if( pWorld->GetID() == WI_WORLD_GUILDWAR )
				return;

//			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_CHR_CURSOR2, vPos.x, vPos.y, vPos.z);
			for( DWORD dw = 0; dw < 1; dw++ )
			{
				CObj* pObj	= CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
				if( NULL == pObj )	return;	// ASSERT( pObj );
				pObj->SetPos( vPos );
				pObj->InitMotion( MTI_STAND );
				pObj->UpdateLocalMatrix();
			
				((CMover*)pObj)->m_bActiveAttack = 0;
				pWorld->AddObj( pObj, TRUE );
			}
			pUser->AddDefinedText( TID_GAME_CREATEMON_S, "\"%s\"", pMoverProp->szName );
			pUser->RemoveItem( dwId, (short)1 );
		}
	}
	else
	{
		// 아이템이 없어서 사용할수 없음.
	}
	#endif // __NEW_ITEMCREATEMON_SERVER
}
#endif // __ITEMCREATEMON_S0602

void CDPSrvr::OnFoucusObj( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objid;
	ar >> objid;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
		pUser->AddFocusObj(objid);
}

void CDPSrvr::OnTrade( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objidTrader;
	ar >> objidTrader;

	CWorld* pWorld;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) && pUser->m_vtInfo.GetOther() == NULL )
	{
		CMover* pTrader		= prj.GetMover( objidTrader );
		if( IsValidObj( pTrader ) && pTrader->GetWorld() && pTrader->m_vtInfo.GetOther() == NULL )
		{
#ifdef __WORLSERVER_MENU
			if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
				pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
				return;
			}
#endif // __WORLSERVER_MENU
			if( pTrader->IsPlayer() )	// pc
			{
#ifdef __SECURITY_FIXES
				// Check if player want to trade with himself
				if (pUser->m_idPlayer == pTrader->m_idPlayer)
					return;

				// Check if player is ded.
				if (pUser->IsDie() || pTrader->IsDie())
					return;
#endif // __SECURITY_FIXES

#if __VER >= 8 // 8차 듀얼 061226 ma
				if( 0 < pUser->m_nDuel ||  0 < pTrader->m_nDuel )
				{
					return;
				}
#endif // __VER >= 8 // 8차 듀얼 061226 ma


				//개인상점 중에는 거래 불가 
				if( pUser->m_vtInfo.VendorIsVendor() || pUser->m_vtInfo.IsVendorOpen() ||
                    pTrader->m_vtInfo.VendorIsVendor() || pTrader->m_vtInfo.IsVendorOpen() )
				{
					return;	//
				}
#ifdef __S_SERVER_UNIFY
				if( pUser->m_bAllAction == FALSE || ((CUser*)pTrader)->m_bAllAction == FALSE )
					return;
#endif // __S_SERVER_UNIFY

				if( pUser->m_bBank || pTrader->m_bBank )
				{
					pUser->AddDefinedText( TID_GAME_TRADELIMITPC, "" );
					return;	//
				}

				if( pUser->m_bGuildBank || pTrader->m_bGuildBank )
				{
					pUser->AddDefinedText( TID_GAME_TRADELIMITPC, "" );
					return;	//
				}

#ifdef __BLOCK_STAFF_ACTIONS
				if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
				{
					pUser->AddText("[Titanium] Unauthorized Action. (Trade)", COLOR_ERROR);
					return;
				}
				if (!(CUser*)pTrader->IsAuthorization(AUTH_GENERAL) && !(CUser*)pTrader->IsAuthorization(AUTH_ADMINISTRATOR))
				{
					((CUser*)pTrader)->AddText("Staff members are not allowed to Trade.", COLOR_WARNING);
					return;
				}
#endif //__BLOCK_STAFF_ACTIONS

#ifdef __MIKSIK_EQUALIZED_MODE
				if (pUser->bTmpMode || ((CUser*)pTrader)->bTmpMode)
				{
					pUser->AddText("You cant use that in temporal mode!");
					return;
				}
#endif //__MIKSIK_EQUALIZED_MODE

				pUser->m_vtInfo.SetOther( pTrader );
				pTrader->m_vtInfo.SetOther( pUser );

				pUser->AddTrade( (CUser*)pTrader, pUser->m_idPlayer );
				( (CUser*)pTrader )->AddTrade( pUser, pUser->m_idPlayer );
			}
		}
	}
}

void CDPSrvr::OnConfirmTrade( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objidTrader;
	ar >> objidTrader;

	CWorld* pWorld;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) && pUser->m_vtInfo.GetOther() == NULL )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU
		// 대전장에서는 거래를 할수 없습니다.
		if( pWorld && pWorld->GetID() == WI_WORLD_GUILDWAR )
		{			
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_TRADE) ); //길드대전장 에서는 거래에 관한 모든것들을 이용 할 수 없습니다.
			return;
		}
#if __VER >= 11 // __GUILD_COMBAT_1TO1
		if( g_GuildCombat1to1Mng.IsPossibleUser( pUser ) )
		{
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_TRADE) ); //길드대전장 에서는 거래에 관한 모든것들을 이용 할 수 없습니다.
			return;
		}
#endif // __GUILD_COMBAT_1TO1

		CMover* pTrader		= prj.GetMover( objidTrader );
		if( IsValidObj( pTrader ) && pTrader->GetWorld() && pTrader->m_vtInfo.GetOther() == NULL )
		{
			if( pTrader->IsPlayer() == FALSE )
				return;

#ifdef __SECURITY_FIXES
			// Check if player want to trade with himself
			if (pUser->m_idPlayer == pTrader->m_idPlayer)
				return;
			
			// Check if player is ded.
			if (pUser->IsDie() || pTrader->IsDie())
				return;
#endif // __SECURITY_FIXES

#if __VER >= 8 // 8차 듀얼 061226 ma
			if( 0 < pUser->m_nDuel ||  0 < pTrader->m_nDuel )
			{
				return;
			}
#endif // __VER >= 8 // 8차 듀얼 061226 ma

#ifdef __BLOCK_STAFF_ACTIONS
			if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
			{
				pUser->AddText("[Titanium] Unauthorized Action. (Trade)", COLOR_ERROR);
				return;
			}
			if (!(CUser*)pTrader->IsAuthorization(AUTH_GENERAL) && !(CUser*)pTrader->IsAuthorization(AUTH_ADMINISTRATOR))
			{
				((CUser*)pTrader)->AddText("Staff members are not allowed to Trade.", COLOR_WARNING);
				return;
			}
#endif //__BLOCK_STAFF_ACTIONS

#ifdef __MIKSIK_EQUALIZED_MODE
			if (pUser->bTmpMode || ((CUser*)pTrader)->bTmpMode)
			{
				pUser->AddText("You cant use that in temporal mode!");
				return;
			}
#endif //__MIKSIK_EQUALIZED_MODE

			if( pTrader->IsAttackMode() )
				pUser->AddDefinedText( TID_GAME_BATTLE_NOTTRADE, "" );
			else
				((CUser*)pTrader)->AddComfirmTrade( pUser->GetId() );
		}

		pUser->RemoveInvisible();		// 거래를 하면 투명은 풀린다.
	}
}

void CDPSrvr::OnConfirmTradeCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objidTrader;
	ar >> objidTrader;

	CMover* pTrader		= prj.GetMover( objidTrader );
	if (IsValidObj(pTrader) && pTrader->GetWorld() && pTrader->m_vtInfo.GetOther() == NULL)
	{
#ifdef __WORLSERVER_MENU
		CUser* pUser = ((CUser*)pTrader);
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			if (IsValidObj(pUser)) {
				pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			}
			return;
		}
#endif // __WORLSERVER_MENU
		((CUser*)pTrader)->AddComfirmTradeCancel(objidTrader);
	}
}

void CDPSrvr::OnTradePut( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE i, nItemType, nId;
	short nItemNum;

	ar >> i >> nItemType >> nId >> nItemNum;
	if( i >= MAX_TRADE || nItemNum < 1 )
		return;

#ifdef __AEGON_FIXES
	if (i < 0)
		return;
#endif

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	CMover* pTrader	= ( pUser ? pUser->m_vtInfo.GetOther() : NULL );
	CWorld* pWorld;

	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) && 
		IsValidObj( pTrader ) && pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU

#ifdef __BLOCK_STAFF_ACTIONS
		if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
		{
			pUser->AddText("[Titanium] Unauthorized Action. (Trade)", COLOR_ERROR);
			return;
		}
		if (!(CUser*)pTrader->IsAuthorization(AUTH_GENERAL) && !(CUser*)pTrader->IsAuthorization(AUTH_ADMINISTRATOR))
		{
			((CUser*)pTrader)->AddText("Staff members are not allowed to Trade.", COLOR_WARNING);
			return;
		}
#endif //__BLOCK_STAFF_ACTIONS

#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode || ((CUser*)pTrader)->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE

#if __VER >= 9 // JYJ
		CItemElem* pItem	= (CItemElem*)pUser->GetItemId( nId );
		if( pItem && pItem->IsFlag( CItemElem::expired ) )
			return;
#endif // __VER >= 9 // JYJ

#ifdef __SYS_LOCK_ITEM
		if (pItem && pItem->IsItemLocked())
		{
			CString str;
			str.Format("Please unlock %s before trading it.", pItem->GetProp()->szName);
			pUser->AddText(str, COLOR_WARNING);
			return;
		}
#endif // __SYS_LOCK_ITEM

		if( pUser->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM && pTrader->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM )
		{
			short nItemNumResult	= nItemNum;
			DWORD dwText	= pUser->m_vtInfo.TradeSetItem2( nId, i, nItemNumResult );
			if( dwText == 0 )
			{
				pUser->AddTradePut( pUser->GetId(), i, nItemType, nId, nItemNumResult );
				( (CUser*)pTrader )->AddTradePut( pUser->GetId(), i, nItemType, nId, nItemNumResult );
			}
			else
			{
				pUser->AddDefinedText( dwText );
			}
		}
		else
		{
			pUser->AddTradePutError();
		}
	}
}

void CDPSrvr::OnTradePull( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE i;
	ar >> i;

	if( i >= MAX_TRADE )
		return;

#ifdef __AEGON_FIXES
	if (i < 0)
		return;
#endif

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	CMover* pTrader	= ( pUser ? pUser->m_vtInfo.GetOther() : NULL );
	CWorld* pWorld;

	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) && 
		IsValidObj( pTrader ) && pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU

#ifdef __BLOCK_STAFF_ACTIONS
		if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
		{
			pUser->AddText("[Titanium] Unauthorized Action. (Trade)", COLOR_ERROR);
			return;
		}
		if (!(CUser*)pTrader->IsAuthorization(AUTH_GENERAL) && !(CUser*)pTrader->IsAuthorization(AUTH_ADMINISTRATOR))
		{
			((CUser*)pTrader)->AddText("Staff members are not allowed to Trade.", COLOR_WARNING);
			return;
		}
#endif //__BLOCK_STAFF_ACTIONS

#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode || ((CUser*)pTrader)->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE

		if( pUser->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM && pTrader->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM )
		{
			if( pUser->m_vtInfo.TradeClearItem( i ) )
			{
				pUser->AddTradePull( pUser->GetId(), i );
				( (CUser*)pUser->m_vtInfo.GetOther() )->AddTradePull( pUser->GetId(), i );
			}
		}
	}
}

void CDPSrvr::OnTradePutGold( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwGold;
	ar >> dwGold;
	
	int nGold = dwGold;
	if( nGold <= 0 )
		return;

#ifdef __SECURITY_FIXES
	if (dwGold >= INT_MAX)
		return;
#endif // __SECURITY_FIXES

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	CMover* pTrader	= ( pUser ? pUser->m_vtInfo.GetOther() : NULL );
	CWorld* pWorld;

	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) && 
		IsValidObj( pTrader ) && pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU

#ifdef __BLOCK_STAFF_ACTIONS
		if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
		{
			pUser->AddText("[Titanium] Unauthorized Action. (Trade)", COLOR_ERROR);
			return;
		}
		if (!(CUser*)pTrader->IsAuthorization(AUTH_GENERAL) && !(CUser*)pTrader->IsAuthorization(AUTH_ADMINISTRATOR))
		{
			((CUser*)pTrader)->AddText("Staff members are not allowed to Trade.", COLOR_WARNING);
			return;
		}
#endif //__BLOCK_STAFF_ACTIONS

#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode || ((CUser*)pTrader)->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE

		if( pUser->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM && pTrader->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM )
		{
			if( nGold > pUser->GetGold() )
				nGold = pUser->GetGold();
			
			pUser->m_vtInfo.TradeSetGold( nGold );
			pUser->AddGold( -nGold, FALSE );	// raiders.2006.11.28  인벤돈 = 인벤돈 - 거래창 돈 

			pUser->AddTradePutGold( pUser->GetId(), nGold );
			( (CUser*)pTrader )->AddTradePutGold( pUser->GetId(), nGold );
		}
	}
}

void CDPSrvr::OnTradeCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int nMode;
	ar >> nMode;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	CMover* pTrader	= ( pUser ? pUser->m_vtInfo.GetOther() : NULL );
	CWorld* pWorld;

	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) &&
		IsValidObj( pTrader ) && pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU
		pUser->m_vtInfo.TradeClear();
		pTrader->m_vtInfo.TradeClear();

		pUser->AddTradeCancel( pUser->GetId(), pUser->m_idPlayer, nMode );
		( (CUser*)pTrader )->AddTradeCancel( pUser->GetId(), pUser->m_idPlayer, nMode );
	}
}

void CDPSrvr::OnTradelastConfirm( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	CMover* pTrader	= ( pUser ? pUser->m_vtInfo.GetOther() : NULL );	
	CWorld* pWorld;

	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) && 
		IsValidObj( pTrader ) && pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3))	{
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU

#ifdef __BLOCK_STAFF_ACTIONS
		if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
		{
			pUser->AddText("[Titanium] Unauthorized Action. (Trade)", COLOR_ERROR);
			return;
		}
		if (!(CUser*)pTrader->IsAuthorization(AUTH_GENERAL) && !(CUser*)pTrader->IsAuthorization(AUTH_ADMINISTRATOR))
		{
			((CUser*)pTrader)->AddText("Staff members are not allowed to Trade.", COLOR_WARNING);
			return;
		}
#endif //__BLOCK_STAFF_ACTIONS

#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode || ((CUser*)pTrader)->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE

		if( pUser->m_vtInfo.TradeGetState() != TRADE_STEP_OK )
			return;

#ifdef __SECURITY_FIXES
		// Check if player want to trade with himself
		if (pUser->m_idPlayer == pTrader->m_idPlayer)
			return;
#endif // __SECURITY_FIXES

		switch( pTrader->m_vtInfo.TradeGetState() )
		{
		case TRADE_STEP_OK:
			pUser->m_vtInfo.TradeSetState( TRADE_STEP_CONFIRM );	

			pUser->AddTradelastConfirmOk( pUser->GetId() );					// ok를 눌렀음을 표시하게 한다.
			( (CUser*)pTrader )->AddTradelastConfirmOk( pUser->GetId() );	// ok를 눌렀음을 표시하게 한다.
			break;

		case TRADE_STEP_CONFIRM:
			{
				BEFORESENDDUAL( out, PACKETTYPE_LOG_ALLITEM, DPID_UNKNOWN, DPID_UNKNOWN );		// log용 ar
				TRADE_CONFIRM_TYPE type = pUser->m_vtInfo.TradeLastConfirm( out );
				switch( type )
				{
				case TRADE_CONFIRM_ERROR:	
					pUser->AddTradeCancel( NULL_ID, pUser->m_idPlayer );	
					( (CUser*)pTrader )->AddTradeCancel( NULL_ID, pUser->m_idPlayer );
					break;
				case TRADE_CONFIRM_OK:
					SEND( out, &g_dpDBClient, DPID_SERVERPLAYER );

					pUser->AddTradeConsent();	
					( (CUser*)pTrader )->AddTradeConsent();
					break;
				}					
			}
			break;	
		}
#ifdef __GAMEGUARD
		if (pUser->GetWorld() && (CUser*)pTrader->GetWorld())
		{
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
			g_dpDBClient.SavePlayer((CUser*)pTrader, ((CUser*)pTrader)->GetWorld()->GetID(), ((CUser*)pTrader)->GetPos(), ((CUser*)pTrader)->GetLayer());
		}
#endif // __GAMEGUARD
	}
}

// 사용자가 ok 버튼을 눌렀을 때
void CDPSrvr::OnTradeOk( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	CMover* pTrader	= ( pUser ? pUser->m_vtInfo.GetOther() : NULL );
	CWorld* pWorld;

	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) && 
		IsValidObj( pTrader ) && pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU

#ifdef __BLOCK_STAFF_ACTIONS
		if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
		{
			pUser->AddText("[Titanium] Unauthorized Action. (Trade)", COLOR_ERROR);
			return;
		}
		if (!(CUser*)pTrader->IsAuthorization(AUTH_GENERAL) && !(CUser*)pTrader->IsAuthorization(AUTH_ADMINISTRATOR))
		{
			((CUser*)pTrader)->AddText("Staff members are not allowed to Trade.", COLOR_WARNING);
			return;
		}
#endif //__BLOCK_STAFF_ACTIONS

#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode || ((CUser*)pTrader)->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE

		if( pUser->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM )
		{
			pUser->m_vtInfo.TradeSetState( TRADE_STEP_OK ); 

			if( pTrader->m_vtInfo.TradeGetState() == TRADE_STEP_OK )	// 상대가 먼저 ok를 눌러서 교환이 성립되는 경우
			{
				( (CUser*)pUser )->AddTradelastConfirm();
				( (CUser*)pTrader )->AddTradelastConfirm();
			}
			else	// 내가 먼저 ok 버튼을 누른 경우
			{
				pUser->AddTradeOk( pUser->GetId() );				// 클라에 전송해 ok를 눌렀음을 표시하게 한다.
				( (CUser*)pTrader )->AddTradeOk( pUser->GetId() );	// 클라에 전송해 ok를 눌렀음을 표시하게 한다.
			}
		}
	}
}

void CDPSrvr::OnPVendorOpen( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	if( uBufSize > 55 )	// 4 + 4 + 48 - 1		= 55
		return;

	char szPVendor[MAX_VENDORNAME];	// 개인 상점 이름( 48 )
	ar.ReadString( szPVendor, MAX_VENDORNAME );	

#ifdef __SECURITY_FIXES
	// Check if string length is valid
	if (strlen(szPVendor) < 6 || strlen(szPVendor) > 32)
		return;
#endif // __SECURITY_FIXES

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU

#ifdef __BLOCK_STAFF_ACTIONS
		if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
		{
			pUser->AddText("[Titanium] Unauthorized Action. (Vending)", COLOR_ERROR);
			return;
		}
#endif //__BLOCK_STAFF_ACTIONS

#ifdef __RULE_0615
		if (prj.IsInvalidName(szPVendor) || prj.IsAllowedLetter(szPVendor, TRUE) == FALSE)
		{
			pUser->AddDiagText(prj.GetText(TID_DIAG_0020));
#ifdef __GAMEGUARD
			return;
#endif // __GAMEGUARD
		}
#endif	// __RULE_0615

		if( !( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT ) ) )
			ParsingEffect(szPVendor, strlen(szPVendor) );
		
#ifdef __SHOPS_ONLY_CONT
		BYTE UserContinent = CContinent::GetInstance()->GetTown(pUser->GetPos());
		if (UserContinent) // Town Check
		{
			if (UserContinent != TOWN_DARKEN)
			{
				pUser->AddText("You're not allowed to create a private shop here.", COLOR_WARNING);
				pUser->AddText("Please press (V) and move to Darken City.", COLOR_WARNING);
				return;
			}
		}
		else // No Town Found
		{
			pUser->AddText("You're not allowed to create a private shop here.", COLOR_WARNING);
			pUser->AddText("Please press (V) and move to Darken City.", COLOR_WARNING);
			return;
		}
#endif //__SHOPS_ONLY_CONT

		CWorld* pWorld = pUser->GetWorld();
		if( pWorld && pWorld->GetID() == WI_WORLD_GUILDWAR )
		{			
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_TRADE) ); //길드대전장 에서는 거래에 관한 모든것들을 이용 할 수 없습니다.
			return;
		}
#if __VER >= 11 // __GUILD_COMBAT_1TO1
		if( g_GuildCombat1to1Mng.IsPossibleUser( pUser ) )
		{
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_TRADE) ); //길드대전장 에서는 거래에 관한 모든것들을 이용 할 수 없습니다.
			return;
		}
#endif // __GUILD_COMBAT_1TO1
#if __VER >= 13 // __HOUSING
		if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_MINIROOM )
			return;
#endif // __HOUSING

#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE

		if( pUser->m_vtInfo.GetOther() )	// 거래중 이면 개인상점 불가 
			return;
#if __VER >= 8 // 8차 듀얼 061226 ma
		if( 0 < pUser->m_nDuel )
		{
			return;
		}
#endif // __VER >= 8 // 8차 듀얼 061226 ma

		if( pUser->IsAttackMode() )
			return;

		pUser->m_vtInfo.SetTitle( szPVendor );

		if( pUser->m_pActMover->IsFly() )
			return;

#if __VER >= 8 // __S8_PK
		if( pUser->IsChaotic() )
		{
			CHAO_PROPENSITY Propensity = prj.GetPropensityPenalty( pUser->GetPKPropensity() );
			if( !Propensity.nVendor )
			{
				pUser->AddDefinedText( TID_GAME_CHAOTIC_NOT_VENDOR );
				return;
			}
		}
#endif // __VER >= 8 // __S8_PK

#ifdef __S_SERVER_UNIFY
		if( pUser->m_bAllAction == FALSE )
			return;
#endif // __S_SERVER_UNIFY

		if( pUser->m_vtInfo.IsVendorOpen() )
		{
			if( pUser->m_vtInfo.VendorIsVendor() )	
			{
				g_ChattingMng.NewChattingRoom( pUser->m_idPlayer );
				CChatting * pChatting	= g_ChattingMng.GetChttingRoom( pUser->m_idPlayer );
#ifdef __VENDING_NPC
				pUser->m_vtInfo.m_tmOpened = GetTickCount();
#endif // __VENDING_NPC
				g_UserMng.AddPVendorOpen( pUser );
				if( pChatting )
				{
					pChatting->m_bState		= TRUE;
					if( pChatting->AddChattingMember( pUser->m_idPlayer ) )
						pUser->m_idChatting		= pUser->m_idPlayer;
					pUser->AddNewChatting( pChatting );
				}
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
				pUser->m_dwHonorCheckTime = GetTickCount();
#endif	// __HONORABLE_TITLE			// 달인
			}
		}
#ifdef __AEGON_ACHIEVEMENT_SYSTEM
		CAegonAchievementMng::GetInstance()->AddAchievementProgressForType(pUser, AEGACH_VENDOPEN, 1);
#endif // __AEGON_ACHIEVEMENT_SYSTEM
#ifdef __LEEAEG_SHOP_FINDER
		//add items to map
		if (pUser && pUser->GetWorld())
		{
			for (int i = 0; i < MAX_VENDITEM; i++)
			{
				CItemElem* tmpItem = (CItemElem*)pUser->m_vtInfo.GetItem(i);
				if (tmpItem)
					CAegonShopFinderMng::GetInstance()->AddItem(tmpItem, pUser->GetName(), pUser->GetPos(), pUser->GetWorld()->GetID(), CContinent::GetInstance()->GetContinentName(CContinent::GetInstance()->GetContinent(pUser)));
			}
		}
#endif // __LEEAEG_SHOP_FINDER

#ifdef __GAMEGUARD		
		if (pUser->GetWorld())
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
#endif // __GAMEGUARD	
	}
}

void CDPSrvr::OnPVendorClose( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objidVendor;
	ar >> objidVendor;
#ifdef __VENDING_NPC
	BOOL bClearShop;
	ar >> bClearShop;
#endif // __VENDING_NPC

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __VENDING_NPC
		ClosePVendor(pUser, objidVendor, bClearShop);
#else // __VENDING_NPC
		ClosePVendor(pUser, objidVendor);
#endif // __VENDING_NPC
#ifdef __SHOPS_VISITED
		g_UserMng.AddRemoveShop(objidVendor);
#endif //__SHOPS_VISITED
	}	
}

#ifdef __VENDING_NPC
BOOL CDPSrvr::ClosePVendor(CUser* pUser, OBJID objidVendor, BOOL bClearShop)
#else // __VENDING_NPC
BOOL CDPSrvr::ClosePVendor(CUser* pUser, OBJID objidVendor)
#endif // __VENDING_NPC
{
	if( pUser->GetId() == objidVendor )
	{
		if( pUser->m_vtInfo.IsVendorOpen() == FALSE && 
			pUser->m_vtInfo.VendorIsVendor() == FALSE )
		{
			return FALSE;
		}
		
		CMover* pTrader	= pUser->m_vtInfo.GetOther();   //raiders.2006.11.27

#ifdef __VENDING_NPC
		g_dpDBClient.SendShopCloseLog(pUser->m_idPlayer, pUser->m_vtInfo.GetAllItems());
		pUser->m_vtInfo.VendorClose(TRUE, bClearShop);

		if (bClearShop)
		{
			pUser->m_vtInfo.TradeClear();
			if (pTrader)
				pTrader->m_vtInfo.TradeClear();
		}

		g_UserMng.AddPVendorClose(pUser, bClearShop);
#else // __VENDING_NPC
		pUser->m_vtInfo.VendorClose();
		
		//raiders.2006.11.27
		pUser->m_vtInfo.TradeClear();
		if( pTrader )
			pTrader->m_vtInfo.TradeClear();
		//--

		g_UserMng.AddPVendorClose( pUser );
#endif // __VENDING_NPC

		CChatting* pChatting = g_ChattingMng.GetChttingRoom( pUser->m_idChatting );
		if( pChatting )
		{
			for( int i = 0 ; i < pChatting->GetChattingMember() ; ++i )
			{
				CUser* pUserBuf = (CUser*)prj.GetUserByID( pChatting->m_idMember[i] );
				if( IsValidObj( pUserBuf ) )
				{
					// 채팅방이 없어짐
					pUserBuf->AddDeleteChatting();
					pUserBuf->m_idChatting	= 0;
				}
			}
		}
		g_ChattingMng.DeleteChattingRoom( pUser->m_idPlayer );
		pUser->m_idChatting		= 0;
	}
	else
	{
		if( IsValidObj( pUser->m_vtInfo.GetOther() ) )
		{
			pUser->m_vtInfo.SetOther( NULL );
			pUser->AddPVendorClose( objidVendor );

			CChatting* pChatting	= g_ChattingMng.GetChttingRoom( pUser->m_idChatting );
			if( pChatting )
			{
				for( int i = 0 ; i < pChatting->GetChattingMember() ; ++i )
				{
					CUser* pUserBuf = (CUser*)prj.GetUserByID( pChatting->m_idMember[i] );
					if( IsValidObj( pUserBuf ) )		// 채팅에서 나감
					{							
						pUserBuf->AddRemoveChatting( pUser->m_idPlayer );
					}
				}
				pChatting->RemoveChattingMember( pUser->m_idPlayer );
				pUser->m_idChatting		= 0;
			}
		}
	}

	return TRUE;
}

void CDPSrvr::OnBuyPVendorItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objidVendor;
	BYTE nItem;
	DWORD dwItemId;
	short nNum;

	ar >> objidVendor >> nItem >> dwItemId >> nNum;
	if( nItem >= MAX_VENDITEM || nNum <= 0 )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

#ifdef __WORLSERVER_MENU
	if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
		pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
		return;
	}
#endif // __WORLSERVER_MENU

#ifdef __BLOCK_STAFF_ACTIONS
	if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
	{
		pUser->AddText("[Titanium] Unauthorized Action. (Vending)", COLOR_ERROR);
		return;
	}
#endif //__BLOCK_STAFF_ACTIONS

#ifdef __MIKSIK_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddText("You cant use that in temporal mode!");
		return;
	}
#endif //__MIKSIK_EQUALIZED_MODE

	CUser* pPVendor	= prj.GetUser( objidVendor );

#ifdef __VENDING_NPC
	if (IsValidObj(pPVendor) == FALSE)
	{
		pPVendor = (CUser*)prj.GetMover(objidVendor);
	}
#endif // __VENDING_NPC

	if( IsValidObj( pPVendor ) )
	{
		VENDOR_SELL_RESULT result;
		BOOL bOK = pPVendor->m_vtInfo.VendorSellItem( pUser, nItem, dwItemId, nNum, result );
		if( bOK )
		{
#if __VER < 11 // __MOD_VENDOR
			g_UserMng.AddPVendorItemNum( pPVendor, nItem, result.nRemain );
#endif	// __MOD_VENDOR
#ifdef __AEGON_ACHIEVEMENT_SYSTEM
			CAegonAchievementMng::GetInstance()->AddAchievementProgressForType(pUser, AEGACH_VENDBUYS, 1);
#endif // __AEGON_ACHIEVEMENT_SYSTEM

			LogItemInfo info;

			info.Action = "Z";
			info.SendName = pUser->GetName();
			info.RecvName = pPVendor->GetName();
			info.WorldId = pUser->GetWorld()->GetID();
			info.Gold = pUser->GetGold() + ( result.item.m_nCost * nNum );
			info.Gold2 = pUser->GetGold();
			info.Gold_1 = pPVendor->GetGold();
			OnLogItem( info, &result.item, nNum );

			info.Action = "X";
			info.SendName = pPVendor->GetName();
			info.RecvName = pUser->GetName();
			info.WorldId = pPVendor->GetWorld()->GetID();
			info.Gold = pPVendor->GetGold() - ( result.item.m_nCost * nNum );
			info.Gold2 = pPVendor->GetGold();
			info.Gold_1 = pUser->GetGold();
			OnLogItem( info, &result.item, nNum );
		}
		else
		{
			if( result.nErrorCode )
				pUser->AddDefinedText( result.nErrorCode, "" );
		}
#ifdef __GAMEGUARD
		if (pUser->GetWorld())
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
#ifdef __VENDING_NPC
		CUser* pVendorOwner = ((CUser*)prj.GetUserByID(pPVendor->m_vtInfo.GetOwner()));
		if (pVendorOwner)
			if (pVendorOwner->GetWorld())
				g_dpDBClient.SavePlayer(pVendorOwner, pVendorOwner->GetWorld()->GetID(), pVendorOwner->GetPos(), pVendorOwner->GetLayer());
#else // __VENDING_NPC
		if (pPVendor->GetWorld())
			g_dpDBClient.SavePlayer(pPVendor, pPVendor->GetWorld()->GetID(), pPVendor->GetPos(), pPVendor->GetLayer());
#endif // __VENDING_NPC
#endif // __GAMEGUARD
	}
}

void CDPSrvr::OnQueryPVendorItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objidVendor;
	ar >> objidVendor;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU

		if( pUser->m_vtInfo.GetOther() )
			return;

		if( pUser->m_pActMover->IsFly() )
			return;

#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE

		CUser* pPVendor	= prj.GetUser( objidVendor );

#ifdef __VENDING_NPC
		CMover* pPVendorNPC = prj.GetMover(objidVendor);
#endif //__VENDING_NPC

		if( IsValidObj( pPVendor ) )
		{
			BOOL bChatting = TRUE;
			CChatting* pChatting = g_ChattingMng.GetChttingRoom( pPVendor->m_idChatting );
			if( pChatting )
				bChatting = pChatting->m_bState;

			if( pPVendor->m_vtInfo.IsVendorOpen() )
			{
				pUser->m_vtInfo.SetOther( pPVendor );
				pUser->AddPVendorItem( pPVendor, bChatting );
			}
		}
#ifdef __VENDING_NPC
		else if (IsValidObj(pPVendorNPC))
		{
			if (pPVendorNPC->m_vtInfo.IsVendorOpen())
			{
				pUser->m_vtInfo.SetOther(pPVendorNPC);
				pUser->AddPVendorItem((CUser*)pPVendorNPC, FALSE);
			}
		}
#endif // __VENDING_NPC
	}
}

void CDPSrvr::OnUnregisterPVendorItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE i;
	ar >> i;
	if( i >= MAX_VENDITEM )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __BLOCK_STAFF_ACTIONS
		if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
		{
			pUser->AddText("[Titanium] Unauthorized Action. (Vending)", COLOR_ERROR);
			return;
		}
#endif //__BLOCK_STAFF_ACTIONS

		if( pUser->m_vtInfo.GetOther() )
			return;
		if( pUser->m_vtInfo.IsVendorOpen() )
			return;
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE
		if( pUser->m_vtInfo.VendorClearItem( i ) )
			pUser->AddUnregisterPVendorItem( i );
	}
}

void CDPSrvr::OnRegisterPVendorItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE iIndex, nType, nId;
	short nNum;
	int nCost;

#ifdef __CHIP_PLAYERSHOP
	int nBuyMode;
#endif // __CHIP_PLAYERSHOP

	ar >> iIndex >> nType >> nId >> nNum >> nCost;

#ifdef __CHIP_PLAYERSHOP
	ar >> nBuyMode;
#endif // __CHIP_PLAYERSHOP

#if __VER >= 8 //__S8_VENDOR_REVISION
	if( iIndex >= MAX_VENDOR_REVISION )
		return;
#else //__S8_VENDOR_REVISION
	if( iIndex >= MAX_VENDITEM )
		return;
#endif //__S8_VENDOR_REVISION
	if( nCost < 1 )	
		nCost = 1;

#ifdef __GAMEGUARD
	long long lEntireCost = nNum * nCost;
	if (lEntireCost > INT_MAX)
		return;
#endif // __GAMEGUARD

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU

#ifdef __BLOCK_STAFF_ACTIONS
		if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
		{
			pUser->AddText("[Titanium] Unauthorized Action. (Vending)", COLOR_ERROR);
			return;
		}
#endif //__BLOCK_STAFF_ACTIONS
		
		if( pUser->m_vtInfo.GetOther() )
			return;

		if( pUser->m_vtInfo.IsVendorOpen() )
			return;
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE
		if( pUser->IsRegionAttr( RA_FIGHT ) )
		{
			pUser->AddDefinedText( TID_GAME_FAIL_TO_OPEN_SHOP );
			return;
		}

#if __VER >= 13 // __HOUSING
		if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_MINIROOM )
			return;
#endif // __HOUSING

#if __VER >= 9	// __JEFF_9
		if( CNpcChecker::GetInstance()->IsCloseNpc( pUser->GetWorld(), pUser->GetPos() ) )
		{
			// NPC근처 개인 상점 불가(3m) - 시도 시 확인창이 생성 되도록 처리
			pUser->AddDiagText( prj.GetText( TID_GAME_NPC_RADIUS ) );
//			pUser->AddDefinedText( TID_GAME_NPC_RADIUS );
			return;
		}
#endif	// __JEFF_9

		CItemElem* pItemElem = (CItemElem *)pUser->GetItemId( nId );
		if( IsUsableItem( pItemElem ) )		
		{
			if( pItemElem->IsQuest() )
				return;

			if( pItemElem->IsBinds() )
				return;
			if( pUser->IsUsing( pItemElem ) )
			{
				pUser->AddDefinedText( TID_GAME_CANNOT_DO_USINGITEM );
				return;
			}
			
#ifdef __SYS_LOCK_ITEM
			if (pItemElem->IsItemLocked())
			{
				CString str;
				str.Format("Please unlock %s before vending it.", pItemElem->GetProp()->szName);
				pUser->AddText(str, COLOR_WARNING);
				return;
			}
#endif // __SYS_LOCK_ITEM

#ifdef __CHIP_PLAYERSHOP
			if (nBuyMode != 1)
			{
				if (pUser->m_Inventory.GetEmptyCount() < 3)
				{
					pUser->AddDefinedText(TID_GAME_LACKSPACE);
					return;
				}
				int nPrice = nCost * nNum;
				CItemElem* pSellItem;
				for (int i = 0; i < MAX_VENDITEM; i++)
				{
					pSellItem = (CItemElem*)pUser->m_vtInfo.GetItem(i);
					if (pSellItem)
						if (pSellItem->GetBuyMode() == nBuyMode)
							nPrice += (pSellItem->m_nCost * pSellItem->GetExtra());
				}
				if (pUser->m_Inventory.GetEmptyCount() < (nPrice / 9999))
				{
					pUser->AddDefinedText(TID_GAME_LACKSPACE);
					return;
				}
			}
#endif // __CHIP_PLAYERSHOP

			ItemProp* pProp	= pItemElem->GetProp();
			if( pProp->dwItemKind3 == IK3_CLOAK  && pItemElem->m_idGuild != 0 )
				return;

			if( pProp->dwParts == PARTS_RIDE && pProp->dwItemJob == JOB_VAGRANT )
				return;

			if( pUser->m_Inventory.IsEquip( pItemElem->m_dwObjId ) )
				return;

#if __VER >= 9 // JYJ
			if( pItemElem->IsFlag( CItemElem::expired ) )
				return;
#endif // __VER >= 9 // JYJ
			
			if( nNum > pItemElem->m_nItemNum )
				nNum = pItemElem->m_nItemNum;
			if( nNum < 1 )	
				nNum = 1;

			ItemProp* pItemProp	= pItemElem->GetProp();

#if __VER < 8     //8차게임내아이템판매가격제한풀기
			if( (DWORD)nCost > (pItemProp->dwCost * 1000) )
				return;
#endif	//  __VER < 8  

#ifdef __CHIP_PLAYERSHOP
			if (nBuyMode > 4 || nBuyMode < 1)
				return;

			if (nBuyMode == 2 && pItemProp->dwID == II_SYS_SYS_SCR_PERIN)
			{
				pUser->AddDefinedText(TID_GAME_PLAYERSHOP_ERRPERIN);
				return;
			}
			else if (nBuyMode == 3 && pItemProp->dwID == II_CHP_RED)
			{
				pUser->AddDefinedText(TID_GAME_PLAYERSHOP_ERRCHIP);
				return;
			}
			else if (nBuyMode == 4 && pItemProp->dwID == II_SYS_REDPERIN)
			{
				pUser->AddDefinedText(TID_GAME_PLAYERSHOP_ERRRPERIN);
				return;
			}
			pUser->m_vtInfo.VendorSetItem(nId, iIndex, nNum, nCost, nBuyMode);
			pUser->AddRegisterPVendorItem(iIndex, 0, nId, nNum, nCost, nBuyMode);
#else // __CHIP_PLAYERSHOP
			pUser->m_vtInfo.VendorSetItem(nId, iIndex, nNum, nCost);
			pUser->AddRegisterPVendorItem(iIndex, 0, nId, nNum, nCost);
#endif // __CHIP_PLAYERSHOP
		}
	}
}

#if __VER >= 8 //__CSC_VER8_5
#ifdef __ANGEL_CREATION_FIX
void CDPSrvr::OnCreateAngel(CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
#ifdef __AEGON_DISABLE_ANGEL_CREATION
	return;
#endif

	float greenAngelRate, whiteAngelRate, blueAngelRate, redAngelRate;
	FLOAT fRate[4];
	static DWORD adwItemId[4] = { II_SYS_SYS_QUE_ANGEL_RED, II_SYS_SYS_QUE_ANGEL_BLUE, II_SYS_SYS_QUE_ANGEL_GREEN,  II_SYS_SYS_QUE_ANGEL_WHITE };

	BYTE nSuns, nMoons;

	ar >> nSuns >> nMoons;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (nSuns > 10 || nMoons > 10)
	{
		Error("[Security] OnCreateAngel() Player [%08d] Sunstone[%03d] or Moonstone[%03d] count too high.");
		return;
	}

	int nSunCount = pUser->GetItemNum(II_GEN_MAT_ORICHALCUM01);
	nSunCount += pUser->GetItemNum(II_GEN_MAT_ORICHALCUM01_1);
	if (nSuns > nSunCount)
	{
		pUser->AddText("You don't have enough sunstones in your inventory.", COLOR_ERROR);
		return;
	}

	int nMoonCount = pUser->GetItemNum(II_GEN_MAT_MOONSTONE);
	nMoonCount += pUser->GetItemNum(II_GEN_MAT_MOONSTONE_1);
	if (nMoons > nMoonCount)
	{
		pUser->AddText("You don't have enough moonstones in your inventory.", COLOR_ERROR);
		return;
	}

	// R/B/G/W
	greenAngelRate = (nSuns * 1.0f) + (nMoons * 1.0f);
	whiteAngelRate = greenAngelRate / 10.0f;
	blueAngelRate = greenAngelRate * 2.0f;
	redAngelRate = 100.0f - (whiteAngelRate + greenAngelRate + blueAngelRate);

	fRate[0] = redAngelRate;
	fRate[1] = fRate[0] + blueAngelRate;
	fRate[2] = fRate[1] + greenAngelRate;
	fRate[3] = fRate[2] + whiteAngelRate;

	float rand = xRandom(1000) / 10.0f;
	DWORD dwItemId = 0;
	for (int i = 0; i < 4; i++)
	{
		if (rand <= fRate[i])
		{
			dwItemId = adwItemId[i];
			break;
		}
	}

	if (dwItemId == 0)
		return;

	if( pUser->m_Inventory.GetEmptyCount() < 1 )
	{
		pUser->AddDiagText(prj.GetText(TID_GAME_LACKSPACE));
		return;
	}

	LogItemInfo aLogItem;
	aLogItem.Action = "&";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "ANGEL_MATERIAL";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	// Suns removal
	_stprintf(aLogItem.szItemName, "%d", II_GEN_MAT_ORICHALCUM01_1);
	int nSunsDeleted = pUser->RemoveItemA(II_GEN_MAT_ORICHALCUM01_1, nSuns);
	aLogItem.itemNumber = nSunsDeleted;
	if (aLogItem.itemNumber > 0)
		OnLogItem(aLogItem);

	_stprintf(aLogItem.szItemName, "%d", II_GEN_MAT_ORICHALCUM01);
	aLogItem.itemNumber = nSuns - nSunsDeleted;
	pUser->RemoveItemA(II_GEN_MAT_ORICHALCUM01, aLogItem.itemNumber);
	if (aLogItem.itemNumber > 0)
		OnLogItem(aLogItem);

	// Moons removal
	_stprintf(aLogItem.szItemName, "%d", II_GEN_MAT_MOONSTONE_1);
	int nMoonsDeleted = pUser->RemoveItemA(II_GEN_MAT_MOONSTONE_1, nMoons);
	aLogItem.itemNumber = nMoonsDeleted;
	if (aLogItem.itemNumber > 0)
		OnLogItem(aLogItem);

	_stprintf(aLogItem.szItemName, "%d", II_GEN_MAT_MOONSTONE);
	aLogItem.itemNumber = nMoons - nMoonsDeleted;
	pUser->RemoveItemA(II_GEN_MAT_MOONSTONE, aLogItem.itemNumber);
	if (aLogItem.itemNumber > 0)
		OnLogItem(aLogItem);

	CItemElem itemElem;
	itemElem.m_dwItemId = dwItemId;
	itemElem.m_nItemNum = 1;
	if (pUser->CreateItem(&itemElem))
	{
		//#ifdef __ANGEL_LOG
		LogItemInfo aLogItem2;
		aLogItem2.Action = "&";
		aLogItem2.SendName = pUser->GetName();
		aLogItem2.RecvName = "ANGEL_CREATE";
		aLogItem2.WorldId = pUser->GetWorld()->GetID();
		aLogItem2.Gold = aLogItem.Gold2 = pUser->GetGold();
		OnLogItem(aLogItem2, &itemElem, 1);
		//#endif // __ANGEL_LOG
	}
}
#else // __ANGEL_CREATION_FIX
void CDPSrvr::OnCreateAngel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int nOrichalcum, nMoonstone;
	ItemCountSet itemset[20];
	float greenAngelRate, whiteAngelRate, blueAngelRate, redAngelRate;
	FLOAT fRate[4];
	static DWORD adwItemId[4]	= { II_SYS_SYS_QUE_ANGEL_RED, II_SYS_SYS_QUE_ANGEL_BLUE, II_SYS_SYS_QUE_ANGEL_GREEN,  II_SYS_SYS_QUE_ANGEL_WHITE };

	BOOL isSummonWhite = FALSE;
	BOOL isSummonSuccess = FALSE;
	TCHAR sendstr[1024];
	TCHAR tempchr[10];
	CHAR temp[2];
//	TCHAR createAngel[12];
	int j, k, itemcount;
	CItemElem* pItemElem;

	ar.ReadString( sendstr, 1024 );
	CString infostr = sendstr;
	
	int strlen = infostr.GetLength();
	j = 0;
	k = 0;
	itemcount = 0;
	memset(tempchr, 0, sizeof(TCHAR)*10);
	
	for(int i=0; i<strlen; i++)
	{
		temp[0] = infostr.GetAt(i);
		temp[1] = NULL;
		if(strcmp(temp, "D") == 0)
		{
			CString tempstr;
			tempstr.Format("%s", tempchr);
			
			temp[0] = infostr.GetAt(i+1);
			temp[1] = NULL;
			if(strcmp(temp, "D") == 0)
			{
				if(k <= 20)
				{
					itemset[k].extracount = atoi(tempstr);
					memset(tempchr, 0, sizeof(TCHAR)*10);
					k++;
					i++;
					itemcount++;
					j = 0;
				}
				else
					return; //k값은 MAX인 20을 넘을 수 없다.
			}
			else
			{
				itemset[k].itemid = atoi(tempstr);
				memset(tempchr, 0, sizeof(TCHAR)*10);
				j = 0;
			}
		}
		else
		{
			tempchr[j] = temp[0];
			j++;
		}
	}
	
	nOrichalcum = 0;
	nMoonstone = 0;

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;
	
	for( int i=0; i<itemcount; i++)
	{
		pItemElem = (CItemElem*)pUser->GetItemId( itemset[i].itemid );
		if(pItemElem != NULL)
		{
			switch(pItemElem->GetProp()->dwID) 
			{
				case II_GEN_MAT_ORICHALCUM01:
				case II_GEN_MAT_ORICHALCUM01_1:
					nOrichalcum += itemset[i].extracount;
					break;
				case II_GEN_MAT_MOONSTONE:
				case II_GEN_MAT_MOONSTONE_1:
					nMoonstone += itemset[i].extracount;
					break;
			};
		}
		else
			return;
	}

	// R/B/G/W
	greenAngelRate = (nOrichalcum * 1.0f) + (nMoonstone * 1.0f);
	whiteAngelRate = greenAngelRate / 10.0f;
	blueAngelRate = greenAngelRate * 2.0f;
	redAngelRate = 100.0f - ( whiteAngelRate + greenAngelRate + blueAngelRate );
	
	fRate[0]	= redAngelRate;
	fRate[1]	= fRate[0] + blueAngelRate;
	fRate[2]	= fRate[1] + greenAngelRate;
	fRate[3]	= fRate[2] + whiteAngelRate;

	float rand = xRandom(1000) / 10.0f;
	DWORD dwItemId	= 0;
	for( int i = 0; i < 4; i++ )
	{
		if( rand <= fRate[i] )
		{
			dwItemId = adwItemId[i];
			break;
		}
	}	

	if( dwItemId > 0 )
	{
		if( IsValidObj(pUser) )
		{
			for( int i=0; i<itemcount; i++)
			{
				pItemElem = (CItemElem*)pUser->GetItemId( itemset[i].itemid );
				if( !IsUsableItem( pItemElem ) ) //소비될 아이템의 유효성 검사. 실패 시 RETURN
					return;
			}

			if( pUser->m_Inventory.GetEmptyCount() < 1 )
			{
				pUser->AddDiagText(  prj.GetText( TID_GAME_LACKSPACE ) );
				return;
			}

			LogItemInfo aLogItem;
			aLogItem.Action = "&";
			aLogItem.SendName = pUser->GetName();
			aLogItem.RecvName = "ANGEL_MATERIAL";
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			for( int i=0; i<itemcount; i++)
			{
				pItemElem = (CItemElem*)pUser->GetItemId( itemset[i].itemid );
				if( pItemElem->GetExtra() > 0 )
					pItemElem->SetExtra(0);
//#ifdef __ANGEL_LOG				
				OnLogItem( aLogItem, pItemElem, itemset[i].extracount );	
//#endif // __ANGEL_LOG
				pUser->RemoveItem( (BYTE)( pItemElem->m_dwObjId ), itemset[i].extracount );	
			}

		}
		CItemElem itemElem;
		itemElem.m_dwItemId	= dwItemId;
		itemElem.m_nItemNum	= 1;
		if( pUser->CreateItem( &itemElem ) )
		{
//#ifdef __ANGEL_LOG
			LogItemInfo aLogItem;
			aLogItem.Action = "&";
			aLogItem.SendName = pUser->GetName();
			aLogItem.RecvName = "ANGEL_CREATE";
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			OnLogItem( aLogItem, &itemElem, 1 );
//#endif // __ANGEL_LOG
		}
	}
}
#endif // __ANGEL_CREATION_FIX

void CDPSrvr::OnAngleBuff( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) )
	{
		if( pUser->HasBuffByIk3( IK3_ANGEL_BUFF ) == FALSE )
			return;

#ifdef __BUFF_1107
		IBuff* pBuff	= pUser->m_buffs.GetBuffByIk3( IK3_ANGEL_BUFF );
		if( !pBuff )
			return;
		ItemProp* pItemProp	= pBuff->GetProp();
#else	// __BUFF_1107
		LPSKILLINFLUENCE lpSkillIn = pUser->m_SkillState.GetItemBuf( IK3_ANGEL_BUFF );
		if( lpSkillIn == NULL )
			return;
		ItemProp* pItemProp = prj.GetItemProp( lpSkillIn->wID );
#endif	// __BUFF_1107
		if( pItemProp == NULL )
			return;

		int nAngel = (int)( (float)pItemProp->nAdjParamVal1 );
		if( nAngel <= 0 || 100 < nAngel  )
			nAngel = 100;

		EXPINTEGER nMaxAngelExp = prj.m_aExpCharacter[pUser->m_nAngelLevel].nExp1 / 100 * nAngel;
		if( pUser->m_nAngelExp < nMaxAngelExp )
			return;

		CItemElem itemElem;
		switch( pItemProp->dwID )
		{
		case II_SYS_SYS_QUE_ANGEL_RED:
			itemElem.m_dwItemId	= II_SYS_SYS_QUE_ANGEL_RED100;						
			break;
		case II_SYS_SYS_QUE_ANGEL_BLUE:
			itemElem.m_dwItemId	= II_SYS_SYS_QUE_ANGEL_BLUE100;
			break;
		case II_SYS_SYS_QUE_ANGEL_GREEN:
			itemElem.m_dwItemId	= II_SYS_SYS_QUE_ANGEL_GREEN100;
			break;
		default:
			itemElem.m_dwItemId	= II_SYS_SYS_QUE_ANGEL_WHITE100;
			break;
		}

		itemElem.m_nItemNum		= 1;
		if( pUser->CreateItem( &itemElem ) )
		{
			pUser->RemoveIk3Buffs( IK3_ANGEL_BUFF );

			LogItemInfo aLogItem;
			aLogItem.Action = "&";
			aLogItem.SendName = pUser->GetName();
			aLogItem.RecvName = "ANGEL_CREATE_COMPLETED";
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			OnLogItem( aLogItem, &itemElem, 1 );

#ifndef __DROP_LOG
			char szMessage[512] = { 0, };
			sprintf(szMessage, prj.GetText(TID_EVE_REAPITEM), itemElem.GetProp()->szName);
			pUser->AddText(szMessage);
#endif // __DROP_LOG

		}
		else
			pUser->AddDefinedText( TID_GAME_NOT_INVEN_ANGEL, "" );
	}
}
#endif //__CSC_VER8_5

#ifdef __EVE_MINIGAME
void CDPSrvr::OnKawibawiboStart( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	pUser->m_nKawibawiboState = prj.m_MiniGame.Result_Kawibawibo( pUser );
	if( pUser->m_nKawibawiboState == CMiniGame::KAWIBAWIBO_WIN )
	{
		CMiniGame::__KAWIBAWIBO Kawibawibo = prj.m_MiniGame.FindKawibawibo( pUser->m_nKawibawiboWin );
		CMiniGame::__KAWIBAWIBO KawibawiboNext = prj.m_MiniGame.FindKawibawibo( pUser->m_nKawibawiboWin + 1 );
		pUser->AddKawibawiboResult( pUser->m_nKawibawiboState, pUser->m_nKawibawiboWin, Kawibawibo.dwItemId, Kawibawibo.nItemCount, KawibawiboNext.dwItemId, KawibawiboNext.nItemCount );
	}
	else
		pUser->AddKawibawiboResult( pUser->m_nKawibawiboState, pUser->m_nKawibawiboWin );
}
void CDPSrvr::OnKawibawiboGetItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;
	
	prj.m_MiniGame.ResultItem_Kawibawibo( pUser );
}

void CDPSrvr::OnReassembleStart( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;
	
	OBJID objItemId[9];
	
	for( int i=0; i<9; ++i )
		ar >> objItemId[i];
		
	BOOL nResult = prj.m_MiniGame.Result_Reassemble( pUser, objItemId, 9 );
	if( nResult == TRUE )
		prj.m_MiniGame.ResultItem_Reassemble( pUser );
}

void CDPSrvr::OnReassembleOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	prj.m_MiniGame.OpenWnd_Reassemble( pUser );
}

void CDPSrvr::OnAlphabetOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	prj.m_MiniGame.OpenWnd_Alphabet( pUser );
}

void CDPSrvr::OnAlphabetStart( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;
	
	int nQuestionID = 0;
	OBJID objItemId[5];

	ar >> nQuestionID;
	for( int i=0; i<5; ++i )
		ar >> objItemId[i];

	int nResult = prj.m_MiniGame.Result_Alphabet( pUser, objItemId, 5, nQuestionID );
	if( nResult != CMiniGame::ALPHABET_FAILED && nResult != CMiniGame::ALPHABET_NOTENOUGH_MONEY )
		prj.m_MiniGame.ResultItem_Alphabet( pUser, nResult );
	else
		pUser->AddAlphabetResult( nResult );
}

void CDPSrvr::OnFiveSystemOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;
	
	prj.m_MiniGame.OpenWnd_FiveSystem( pUser );
}

void CDPSrvr::OnFiveSystemBet( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	int nBetNum;
	int nBetPenya;
	
	ar >> nBetNum;
	ar >> nBetPenya;

	int nResult = prj.m_MiniGame.Bet_FiveSystem( pUser, nBetNum, nBetPenya );
	if( nResult == CMiniGame::FIVESYSTEM_NOTENOUGH )
	{
		pUser->AddFiveSystemResult( nResult );
	}
	else if( nResult == CMiniGame::FIVESYSTEM_OVERMAX )
	{
		pUser->AddFiveSystemResult( nResult );
	}
	else if( nResult == CMiniGame::FIVESYSTEM_FAILED )
	{
		pUser->AddFiveSystemResult( nResult );
	}
}

void CDPSrvr::OnFiveSystemStart( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	int nResult = prj.m_MiniGame.Result_FiveSystem( pUser );

	if( !( prj.m_MiniGame.ResultPenya_FiveSystem( pUser, nResult ) ) )
		pUser->AddFiveSystemResult( CMiniGame::FIVESYSTEM_FAILED );
}	

void CDPSrvr::OnFiveSystemDestroyWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;
	
	prj.m_MiniGame.DestroyWnd_FiveSystem( pUser );
}
#endif // __EVE_MINIGAME

#if __VER >= 9 // __ULTIMATE
void CDPSrvr::OnUltimateMakeItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	OBJID objItemId[MAX_JEWEL];

	ar.Read( objItemId, sizeof(OBJID) * MAX_JEWEL );

	int nResult = prj.m_UltimateWeapon.MakeItem( pUser, objItemId );
	pUser->AddUltimateMakeItem( nResult );
}

void CDPSrvr::OnUltimateMakeGem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;
	
	OBJID objItemId;
	ar >> objItemId;

	int nNum;
	int nResult = prj.m_UltimateWeapon.MakeGem( pUser, objItemId, nNum );
	pUser->AddUltimateMakeGem( nResult, nNum );
}

void CDPSrvr::OnUltimateTransWeapon( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	OBJID objItemWeapon;
	OBJID objItemGem1;
	OBJID objItemGem2;

	ar >> objItemWeapon;
	ar >> objItemGem1;
	ar >> objItemGem2;

	int nResult = prj.m_UltimateWeapon.TransWeapon( pUser, objItemWeapon, objItemGem1, objItemGem2 );
	pUser->AddUltimateWeapon( ULTIMATE_TRANSWEAPON, nResult );
}

void CDPSrvr::OnUltimateSetGem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	OBJID objItemWeapon;
	OBJID objItemGem;

	ar >> objItemWeapon;
	ar >> objItemGem;

	int nResult = prj.m_UltimateWeapon.SetGem( pUser, objItemWeapon, objItemGem );
	pUser->AddUltimateWeapon( ULTIMATE_SETGEM, nResult );
}

void CDPSrvr::OnUltimateRemoveGem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;
	
	OBJID objItemWeapon;
	OBJID objItemGem;
	
	ar >> objItemWeapon;
	ar >> objItemGem;
	
#ifdef __SYS_DIA_REMOVE_CHOICE
	int nSlot;
	ar >> nSlot;
	int nResult = prj.m_UltimateWeapon.RemoveGem(pUser, objItemWeapon, objItemGem, nSlot);
#else //__SYS_DIA_REMOVE_CHOICE
	int nResult = prj.m_UltimateWeapon.RemoveGem(pUser, objItemWeapon, objItemGem);
#endif //__SYS_DIA_REMOVE_CHOICE

	if(nResult == CUltimateWeapon::ULTIMATE_SUCCESS)
	{
		//성공 메세지 출력
		pUser->AddDefinedText( TID_GAME_REMOVEGEM_SUCCESS, "" );
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );		
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);		
	}
	else if(nResult == CUltimateWeapon::ULTIMATE_FAILED)
	{
		// 실패 메세지 출력
		pUser->AddDefinedText( TID_GAME_REMOVEGEM_FAILED, "" );
		pUser->AddPlaySound( SND_INF_UPGRADEFAIL );
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);				
	}
	pUser->AddUltimateWeapon( ULTIMATE_REMOVEGEM, nResult );
}

void CDPSrvr::OnUltimateEnchantWeapon( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	OBJID objItemWeapon;
	OBJID objItemGem;
	
	ar >> objItemWeapon;
	ar >> objItemGem;

	int nResult = prj.m_UltimateWeapon.EnchantWeapon( pUser, objItemWeapon, objItemGem );

	if(nResult == CUltimateWeapon::ULTIMATE_SUCCESS)
	{
		//성공 메세지 출력
		pUser->AddDefinedText( TID_UPGRADE_SUCCEEFUL, "" );
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );		
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);		
	}
	else if(nResult == CUltimateWeapon::ULTIMATE_FAILED)
	{
		// 실패 메세지 출력
		pUser->AddDefinedText( TID_UPGRADE_FAIL, "" );
		pUser->AddPlaySound( SND_INF_UPGRADEFAIL );
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);				
	}
	pUser->AddUltimateWeapon( ULTIMATE_ENCHANTWEAPON, nResult );
}
#endif // __ULTIMATE

#ifdef __MIKSIK_EQUALIZED_MODE
void CDPSrvr::OnSelectTmpClass(CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser) == FALSE)
		return;

	if (!pUser->GetWorld())
		return;

	int nTmpClass;
	ar >> nTmpClass;

	BOOL bOk = FALSE;
	DWORD dwWorldID = pUser->GetWorld()->GetID();
	int nTypeOfClass = -1;

	for (unsigned i = 0; i < prj.m_vecTmpWorlds.size(); i++)
	{
		if (prj.m_vecTmpWorlds[i].dwWorldID == dwWorldID)
		{
			nTypeOfClass = prj.m_vecTmpWorlds[i].nMode;
			bOk = TRUE;
			break;
		}
	}

	if (!bOk)
		return;

	if (pUser->bTmpMode)
		return;

	if (pUser->m_vtInfo.GetOther() || pUser->m_vtInfo.VendorIsVendor() || pUser->m_vtInfo.IsVendorOpen() || pUser->m_bBank || pUser->m_bGuildBank)
	{
		pUser->AddText("Please, close bank, guildbank, shop and cancel trade. After that try again..");
		pUser->AddOpenTempWindow(nTypeOfClass);
		return;
	}

	if (nTmpClass >= 0 && nTmpClass <= 7)
	{
		int nJob = 0;
		switch (nTypeOfClass)
		{
		case 0:
		{ 
			switch (nTmpClass)
			{
			case 0:	nJob = JOB_BLADE;	break;
			case 1:	nJob = JOB_KNIGHT;	break;
			case 2:	nJob = JOB_JESTER;	break;
			case 3:	nJob = JOB_RANGER;	break;
			case 4:	nJob = JOB_PSYCHIKEEPER;	break;
			case 5: nJob = JOB_ELEMENTOR;	break;
			case 6:	nJob = JOB_BILLPOSTER;	break;
			case 7:	nJob = JOB_RINGMASTER;	break;
			}
			break;
		}
#ifdef __3RD_LEGEND16 
		case 1:
		{
			switch (nTmpClass)
			{
			case 0:	nJob = JOB_STORMBLADE_HERO;	break;
			case 1:	nJob = JOB_LORDTEMPLER_HERO;	break;
			case 2:	nJob = JOB_WINDLURKER_HERO;	break;
			case 3:	nJob = JOB_CRACKSHOOTER_HERO;	break;
			case 4:	nJob = JOB_MENTALIST_HERO;	break;
			case 5: nJob = JOB_ELEMENTORLORD_HERO;	break;
			case 6:	nJob = JOB_FORCEMASTER_HERO;	break;
			case 7:	nJob = JOB_FLORIST_HERO;	break;
			}
			break;
		}
#endif //__3RD_LEGEND16
		default: Error("Something wrong in CDPSrvr::OnSelectTmpClass: %s", pUser->GetName()); pUser->REPLACE(g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3(6968.0f, 0.0f, 3328.8f), REPLACE_NORMAL, nDefaultLayer); return;
		}

		((CMover*)pUser)->EnterTmpPve(nTypeOfClass, nJob);
	}
}
#endif //__MIKSIK_EQUALIZED_MODE

#ifdef __TRADESYS
void CDPSrvr::OnExchange( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;
#ifdef __MIKSIK_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddText("You cant use that in temporal mode!");
		return;
	}
#endif //__MIKSIK_EQUALIZED_MODE

#ifdef __WORLSERVER_MENU
	if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
		pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
		return;
	}
#endif // __WORLSERVER_MENU

	int nMMIid;
	int nListNum;

	ar >> nMMIid;
	ar >> nListNum;

#ifdef __GAMEGUARD
	if (nListNum < 0 || !CNpcChecker::GetInstance()->IsCloseNpc(nMMIid, pUser->GetWorld(), pUser->GetPos()))
		return;
#endif // __GAMEGUARD

	int	nResult = prj.m_Exchange.ResultExchange( pUser, nMMIid, nListNum );

	pUser->AddExchangeResult( EXCHANGE_RESULT, nResult );
}
#endif // __TRADESYS

void CDPSrvr::InviteParty( u_long uLeaderid, u_long uMemberid, BOOL bTroup )
{
	CUser* pUser = g_UserMng.GetUserByPlayerID( uMemberid );
	CUser* pLeaderUser = g_UserMng.GetUserByPlayerID( uLeaderid );
	
	if( IsValidObj( pLeaderUser ) && IsValidObj( pUser ) )
	{
#ifdef __SECURITY_FIXES
		if (pUser->m_idPlayer == pLeaderUser->m_idPlayer)
			return;
#endif // __SECURITY_FIXES
		// 대전장에서는 파티를 할수 없습니다.
		CWorld* pWorld = pUser->GetWorld();
		if( ( pWorld && pWorld->GetID() == WI_WORLD_GUILDWAR ) || pLeaderUser->GetWorld() && pLeaderUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
		{			
			pLeaderUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_PARTY) );// "수정해야함 : 길드대전장에는 파티를 할수 없습니다" );
			return;
		}
#if __VER >= 11 // __GUILD_COMBAT_1TO1
		if( g_GuildCombat1to1Mng.IsPossibleUser( pUser ) )
		{
			pLeaderUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_PARTY) );// "수정해야함 : 길드대전장에는 파티를 할수 없습니다" );
			return;
		}
#endif // __GUILD_COMBAT_1TO1

#if __VER >= 8 // 8차 듀얼 061226 ma
		if( 0 < pUser->m_nDuel ||  0 < pLeaderUser->m_nDuel )
		{
			return;
		}
#endif // __VER >= 8 // 8차 듀얼 061226 ma
		
		if( pLeaderUser->m_nDuel == 2 )
		{			
			pLeaderUser->AddDefinedText( TID_GAME_PPVP_ADDPARTY, "" );		// 극단 듀얼중엔 초청 못함다.
		} 
		else
		{
			if( 0 < (CMover*)pUser->GetPartyId() )	// 이미 파티가 있을때
			{
				pLeaderUser->AddPartyRequestCancel( uLeaderid, uMemberid, 1 );
			}
			else
			{
				if( pUser->IsAttackMode() )
					pLeaderUser->AddDefinedText( TID_GAME_BATTLE_NOTPARTY, "" );
				else
					pUser->AddPartyRequest( pLeaderUser, pUser, bTroup );
			}
		}
	}
	else
	{
		if( IsValidObj( pLeaderUser ) )
			pLeaderUser->AddPartyRequestCancel( uLeaderid, uMemberid, 4 );
	}
}

void CDPSrvr::InviteCompany( CUser* pUser, OBJID objid )
{
#if __VER >= 12 // __SECRET_ROOM
	if( !IsInviteAbleGuild( pUser ) )
	{
		return;
	}
#endif // __SECRET_ROOM
	
	if( IsValidObj( pUser ) )
	{
		CUser* pUsertmp	= prj.GetUser( objid );
		if( IsValidObj( pUsertmp ) )
		{
#ifdef __BLOCK_STAFF_ACTIONS
			if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
			{
				pUser->AddText("[Titanium] Unauthorized Action. (Guild Invite)", COLOR_ERROR);
				return;
			}
#endif //__BLOCK_STAFF_ACTIONS

			CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
			if( pGuild )
			{
				CGuildMember* pMember	= pGuild->GetMember( pUser->m_idPlayer );
				if( !pMember )
				{
					// is not member
					return;
				}
				if( !pGuild->IsCmdCap( pMember->m_nMemberLv, PF_INVITATION ) )
				{
					// have no power
					pUser->AddDefinedText( TID_GAME_GUILDINVAITNOTWARR );
					return;
				}
				CGuild* pGuildtmp	= g_GuildMng.GetGuild( pUsertmp->m_idGuild );
				if( pGuildtmp && pGuildtmp->IsMember( pUsertmp->m_idPlayer ) )
				{
					// is already guild member
					pUser->AddDefinedText( TID_GAME_COMACCEPTHAVECOM, "%s", pUsertmp->GetName( TRUE ) );
				}
				else
				{
#if __VER >= 8 // 8차 듀얼 061226 ma
					if( 0 < pUsertmp->m_nDuel )
					{
						return;
					}
#endif // __VER >= 8 // 8차 듀얼 061226 ma

					if( !pGuild->GetWar() )
					{
						if( pUsertmp->IsAttackMode() )
						{
							pUser->AddDefinedText( TID_GAME_BATTLE_NOTGUILD, "" );
						} 
						else
						{
							
							pUsertmp->m_idGuild	= 0;
							pUsertmp->AddGuildInvite( pGuild->m_idGuild, pUser->m_idPlayer );
							pUser->AddDefinedText( TID_GAME_COMACCEPTKINGPIN, "%s", pUsertmp->GetName( TRUE ) );
						}
					}
					else
					{
						pUser->AddDefinedText( TID_GAME_GUILDWARNOMEMBER );
					}
				}
			}
		}
	}	
}

#if __VER >= 12 // __SECRET_ROOM
BOOL CDPSrvr::IsInviteAbleGuild( CUser* pUser )
{
	CGuild* pGuild = pUser->GetGuild();
	if( !pGuild )
		return FALSE;

	// 길드대전
	if( g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE )
	{
		if( g_GuildCombatMng.FindGuildCombatMember( pUser->m_idGuild ) &&
			g_GuildCombatMng.FindGuildCombatMember( pUser->m_idGuild )->bRequest )
		{
			pUser->AddDefinedText( TID_GAME_GUILDCOMBAT_NOT_INVITATION_GUILD );
			return FALSE;
		}
	}

	// 1:1 길드대전
	if( g_GuildCombat1to1Mng.m_nState != CGuildCombat1to1Mng::GC1TO1_CLOSE )
	{
		if( g_GuildCombat1to1Mng.GetTenderGuildIndexByUser( pUser ) != NULL_ID )
		{
			pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_NOT_INVITATION_GUILD );
			return FALSE;
		}
	}

	// 비밀의 방
	if( CSecretRoomMng::GetInstance()->m_nState != SRMNG_CLOSE )
	{
		map<BYTE, CSecretRoomContinent*>::iterator it = CSecretRoomMng::GetInstance()->m_mapSecretRoomContinent.begin();
		for( ; it!=CSecretRoomMng::GetInstance()->m_mapSecretRoomContinent.end(); it++ )
		{
			CSecretRoomContinent* pSRCont = it->second;
			if( pSRCont && ( pSRCont->GetTenderGuild( pGuild->GetGuildId() ) != NULL_ID ) )
			{
				pUser->AddDefinedText( TID_GAME_SECRETROOM_NOT_INVITATION_GUILD );
				return FALSE;
			}
		}
	}

	return TRUE;
}
#endif // __SECRET_ROOM

#if __VER < 12 // __EXT_PIERCING
BOOL CDPSrvr::IsPiercingSize( CUser* pUser, CItemElem* pItemElem0, CItemElem* pItemElem1, CItemElem* pItemElem2, int& nError )
{
	if( IsUsableItem( pItemElem0 ) == FALSE || IsUsableItem( pItemElem1 ) == FALSE )
		return FALSE;
	
	if( pUser->m_Inventory.IsEquip( pItemElem0->m_dwObjId ) )
	{
		nError = TID_GAME_EQUIPPUT;
		return FALSE;
	}	
	
	//////////////// 첫번째 아이템 //////////////// 
	if( pItemElem0->GetProp()->dwItemKind3 != IK3_SUIT )
	{
		nError = TID_PIERCING_POSSIBLE_ITEM;
		return FALSE;
	}
		
	// 피어싱 사이즈 검사 지금은 4개가 최고	
	if( pItemElem0->GetPiercingSize() >= MAX_PIERCING_SUIT )
	{
		nError = TID_PIERCING_POSSIBLE;
		return FALSE;
	}
	//////////////// 두번째 아이템 //////////////// 
	// 다이스인지 검사
	if( pItemElem1->GetProp()->dwItemKind3 != IK3_PIERDICE
#ifdef __V19_FORMATS
		|| pItemElem1->GetProp()->dwItemKind3 != IK3_SMELT_ACCESSORY_MATERIAL
#endif //__V19_FORMATS
		)
	{
		nError = TID_UPGRADE_ERROR_WRONGUPLEVEL;			
		return FALSE;					
	}
	
#if __VER >= 8 //__Y_NEW_ENCHANT
	if( pItemElem1->GetProp()->dwID != II_GEN_MAT_MOONSTONE 
		&& pItemElem1->GetProp()->dwID != II_GEN_MAT_MOONSTONE_1 )
#else //__Y_NEW_ENCHANT
	if( pItemElem1->GetProp()->dwID != II_GEN_MAT_DIE_EIGHT && pItemElem1->GetProp()->dwID != II_GEN_MAT_DIE_TEN )
#endif //__Y_NEW_ENCHANT
	{
		nError = TID_SBEVE_NOTUSEITEM;			// 피어싱에 필요한 주사위가 아니면 불가능
		return FALSE;
	}
	//////////////// 세번째 아이템 //////////////// 
	if( IsUsableItem( pItemElem2 ) && pItemElem2->m_dwItemId != II_SYS_SYS_SCR_PIEPROT )
	{
		nError = TID_SBEVE_NOTUSEITEM;			// 상용아이템이 아니면 불가능
		return FALSE;
	}
	
	return TRUE;
}
#endif // __EXT_PIERCING

CCommonCtrl* CreateExpBox( CUser* pUser )
{
#if __VER >= 9 // __S_9_ADD
	return NULL;
#endif // __S_9_ADD
	CWorld* pWorld = pUser->GetWorld();

	if( pWorld == NULL  )
		return NULL;

	///////////////////////////////////////////////////////////////////////
	// 경험치가 깎이지 않으면 드랍을 안함!!!
	CMover* pMover = (CMover*)pUser;
	
	float fRate = 0.1f, fDecExp = 0.0f;
	BOOL  bPxpClear = FALSE, bLvDown = FALSE;
	int   nLevel	= pMover->GetLevel();	
	pMover->GetDieDecExp( nLevel, fRate, fDecExp, bPxpClear, bLvDown );
	if( fDecExp )
	{
#if __VER >= 8 // __S8_PK
		pMover->GetDieDecExpRate( fDecExp, 0, FALSE );
#else // __VER >= 8 // __S8_PK
		pMover->GetDieDecExpRate( fDecExp, 0, pMover->m_nSlaughter );
#endif // __VER >= 8 // __S8_PK
	}

	// 축복의 두루마리 사용시에는 경험치 상자 만들지 않는다...

	if( pMover->IsSMMode( SM_REVIVAL ) )
		fDecExp = 0.0f;

#ifdef __SYS_SANDBOX
	if (pMover->IsWorldSandBox())
		fDecExp = 0.0f;
#endif //__SYS_SANDBOX

	if( pMover->GetExp1() == 0 )
		fDecExp = 0.0f;	

	if( pMover->m_bLastPK || pMover->m_bGuildCombat || pMover->m_bLastDuelParty )		// 무조건 경험치 안깍는다...
		fDecExp = 0.0f;			
		
	if( fDecExp == 0.0f )
		return NULL;
	
	CCommonCtrl* pCtrl	= (CCommonCtrl*)CreateObj( D3DDEVICE, OT_CTRL, 46 );
	
	if( !pCtrl )
		return NULL;
	///////////////////////////////////////////////////////////////////////

	pCtrl->m_CtrlElem.m_dwSet    = UA_PLAYER_ID;
	pCtrl->m_idExpPlayer = pUser->m_idPlayer;
	
	EXPINTEGER	nDecExp = (EXPINTEGER)(prj.m_aExpCharacter[pUser->m_nLevel+1].nExp1 * fDecExp );	// 현재레벨의 최대경험치 * 퍼센트

	if( nDecExp > pMover->GetExp1() )
		nDecExp = pMover->GetExp1();
	
	pCtrl->m_nExpBox     = (EXPINTEGER)(nDecExp * 0.3f);
	pCtrl->m_dwDelete    = timeGetTime() + MIN( 30 );
	
	pCtrl->SetPos( pUser->GetPos() );
	
	pUser->AddDefinedText(TID_GAME_EXPBOX_INFO_MSG, "" );
	pUser->AddChatText(   TID_GAME_EXPBOX_INFO_MSG, "");
	return pCtrl;
}

#if __VER >= 9	// __PET_0410
void CDPSrvr::OnPetRelease( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( !pUser->HasActivatedSystemPet() )
		{
			pUser->AddDefinedText( TID_GAME_PET_NOT_FOUND );
		}
		else
		{
			CItemElem* pItemElem	= pUser->GetPetItem();
			if( pItemElem && pItemElem->m_pPet )
				pUser->PetRelease();
		}
	}
}

void CDPSrvr::OnUsePetFeed( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CPet* pPet	= pUser->GetPet();
		if( pPet == NULL )
		{
			pUser->AddDefinedText( TID_GAME_PET_NOT_FOUND );
			return;
		}
		DWORD dwFeedId;		// 먹이 식별자
		short nNum;
		ar >> dwFeedId;
#if __VER < 12 // __PET_0519
		ar >> nNum;		// 먹이 개수를 서버에서 구하도록 수정
#endif	// __PET_0519
		CItemElem* pFeed	= static_cast<CItemElem*>( pUser->GetItemId( dwFeedId ) );
		if( IsUsableItem( pFeed ) == FALSE )
			return;
		if( !pFeed->IsFeed() )
			return;

#if __VER >= 12 // __PET_0519
		nNum	= pFeed->m_nItemNum;
#else	// __PET_0519
		if( nNum <= 0 )
			nNum	= 1;
		if( nNum > pFeed->m_nItemNum )
			nNum	= pFeed->m_nItemNum;
		if( nNum > 1000 )	// 0723
			nNum	= 1000;
#endif	// __PET_0519

		int nMaxNum	= 0;
		if( pPet->GetLevel() == PL_EGG )
		{
			nMaxNum	= MAX_PET_EGG_EXP - pPet->GetExp();
		}
		else
		{
			nMaxNum	= pPet->GetMaxEnergy() - pPet->GetEnergy();
			nMaxNum	/= 2;	// 먹이 1당 기력 2회복	// 0723
		}

		if( nNum > nMaxNum )
			nNum	= nMaxNum;
		if( nNum == 0 )
			return;

		if( pPet->GetLevel() == PL_EGG )
		{
			pPet->SetExp( pPet->GetExp() + nNum );
			pUser->AddPetSetExp( pPet->GetExp() );
		}
		else
		{
			pPet->SetEnergy( pPet->GetEnergy() + nNum * 2 );	// 먹이 1당 기력 2회복	// 0723
			g_UserMng.AddPetFeed( pUser, pPet->GetEnergy() );
		}
		pUser->UpdateItem( (BYTE)( pFeed->m_dwObjId ), UI_NUM, pFeed->m_nItemNum - nNum );
#if __VER >= 12 // __PET_0519
		pUser->AddDefinedText( TID_GAME_PETFEED_S01, "%d", nNum );
#endif	// __PET_0519

		// log
		CItemElem* pPetItem	= pUser->GetPetItem();
		g_dpDBClient.CalluspPetLog( pUser->m_idPlayer, pPetItem->GetSerialNumber(), nNum, PETLOGTYPE_FEED, pPet );
	}
}

void CDPSrvr::OnMakePetFeed( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		DWORD dwMaterialId, dwToolId;
		short nNum;

		ar >> dwMaterialId >> dwToolId >> nNum;

		BOOL bTool	= FALSE;
		CItemElem* pTool	= NULL;
		if( dwToolId != NULL_ID )
		{
			pTool	= (CItemElem*)pUser->GetItemId( dwToolId );
			if( IsUsableItem( pTool ) )
			{
				if( pTool->m_dwItemId == II_SYS_SYS_FEED_MAKER )
					bTool	= TRUE;
			}
			if( !bTool )	// error
				return;
		}

		// 펫 테이머에 의한 먹이 제조 시, 펫 테이머와 인접해 있지 않으면 무시
		if( bTool == FALSE && CNpcChecker::GetInstance()->IsCloseNpc( MMI_PET_FOODMILL, pUser->GetWorld(), pUser->GetPos() ) == FALSE )
			return;

		CItemElem* pMaterial	= (CItemElem*)pUser->GetItemId( dwMaterialId );
		if( IsUsableItem( pMaterial ) == FALSE )
		{
			// error
			return;
		}
		ItemProp* pProp	= pMaterial->GetProp();
		if( pProp == NULL )
		{
			// error
			return;
		}

		if( pMaterial->IsCharged() )
		{
			pUser->AddDefinedText( TID_GAME_PET_FEED_CHARGED );
			return;
		}

		if( pProp->dwItemKind3 != IK3_GEM )
		{
			pUser->AddDefinedText( TID_GAME_PET_IS_NOT_FEED );
			return;
		}

		if( nNum <= 0 )
			nNum	= 1;
		if( nNum > pMaterial->m_nItemNum )
			nNum	= pMaterial->m_nItemNum;

		if( pUser->m_Inventory.IsEquip( dwMaterialId ) )
		{
			pUser->AddDefinedText( TID_GAME_PET_FEED_EQUIPED );
			return;
		}

		CItemElem itemElem;
		itemElem.m_dwItemId		= II_SYS_SYS_FEED_01;
		int nTotalFeed	= 0;
		int nPackMax	= itemElem.GetProp()->dwPackMax;
		for( int i = 0; i < nNum; i++ )
		{
			int nFeed	= CPetProperty::GetInstance()->GetFeedEnergy( pProp->dwCost, (int)bTool );
			if( nTotalFeed + nFeed > nPackMax )
			{
				nNum	= i;
				break;
			}
			nTotalFeed	+= nFeed;
		}
		itemElem.m_nItemNum		= nTotalFeed;
		itemElem.m_nHitPoint	= -1;

		int nResult = pUser->CreateItem( &itemElem );
		if( nResult )
		{
			//먹이 만들기 로그 : pMaterial이 없어질 먹이 재료, itemElem.m_nItemNum이 새로 생성된 먹이 량
			LogItemInfo aLogItem;
			aLogItem.Action = "~";
			aLogItem.SendName = pUser->GetName();
			aLogItem.RecvName = "PET_FOOD_MATERIAL_REMOVE";
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			OnLogItem( aLogItem, pMaterial, nNum );

			aLogItem.RecvName = "PET_FOOD_CREATE";
			OnLogItem( aLogItem, &itemElem, itemElem.m_nItemNum );

			pUser->UpdateItem( (BYTE)( pMaterial->m_dwObjId ), UI_NUM, pMaterial->m_nItemNum - nNum );
			if( bTool )
				pUser->UpdateItem( (BYTE)( pTool->m_dwObjId ), UI_NUM, pTool->m_nItemNum - 1 );
		}
		pUser->AddPetFoodMill(nResult, itemElem.m_nItemNum);
	}
}

void CDPSrvr::OnPetTamerMistake( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwId;
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		ar >> dwId;
		CItemElem* pItemElem	= (CItemElem*)pUser->GetItemId( dwId );
		if( pItemElem && pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_TAMER_MISTAKE ) // m_dwItemId
		{
			CPet* pPet	= pUser->GetPet();
			if( pPet )
			{
				if( pPet->GetLevel() >= PL_C && pPet->GetLevel() <= PL_S )
				{
					// 현재 레벨에서 얻은 능력치가 취소되며,
					// 전 단계 레벨, 경험치 100%로 돌아간다.
					pPet->SetAvailLevel( pPet->GetLevel(), 0 );		// 능력치 취소
					pPet->SetLevel( pPet->GetLevel() - 1 );
					pPet->SetExp( MAX_PET_EXP );
					// 기존 버프 제거
					if( pUser->HasPet() )
						pUser->RemovePet();
					pUser->AddPet( pPet, PF_PET_LEVEL_DOWN );	// 自
					g_UserMng.AddPetLevelup( pUser, MAKELONG( (WORD)pPet->GetIndex(), (WORD)pPet->GetLevel() ) );	// 他
					pUser->UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_NUM, pItemElem->m_nItemNum - 1 );

					// log
					CItemElem* pPetItem		= pUser->GetPetItem();
					g_dpDBClient.CalluspPetLog( pUser->m_idPlayer, pPetItem->GetSerialNumber(), 0, PETLOGTYPE_MISTAKE, pPet );
				}
				else
				{
					pUser->AddDefinedText( TID_GAME_PET_BETWEEN_C_TO_S );
				}
			}
			else
			{
				pUser->AddDefinedText( TID_GAME_PET_NOT_FOUND );
			}
		}
	}
}

void CDPSrvr::OnPetTamerMiracle( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwId;
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		ar >> dwId;
		CItemElem* pItemElem	= (CItemElem*)pUser->GetItemId( dwId );
		if( pItemElem && pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_TAMER_MIRACLE )
		{
			CPet* pPet	= pUser->GetPet();
			if( pPet )
			{
				if( pPet->GetLevel() >= PL_B && pPet->GetLevel() <= PL_S )
				{
					pPet->SetAvailLevel( pPet->GetLevel() - 1, 0 );		// 능력치 취소
					pPet->SetAvailLevel( pPet->GetLevel(), 0 );		// 능력치 취소

					BYTE nAvailLevel	= CPetProperty::GetInstance()->GetLevelupAvailLevel( pPet->GetLevel() - 1 );
					pPet->SetAvailLevel( pPet->GetLevel() - 1, nAvailLevel );
					nAvailLevel	= CPetProperty::GetInstance()->GetLevelupAvailLevel( pPet->GetLevel() );
					pPet->SetAvailLevel( pPet->GetLevel(), nAvailLevel );

					if( pUser->HasPet() )
						pUser->RemovePet();
					pUser->AddPet( pPet, PF_PET_GET_AVAIL );	// 自	// PF_PET_GET_AVAIL 
					g_UserMng.AddPetLevelup( pUser, MAKELONG( (WORD)pPet->GetIndex(), (WORD)pPet->GetLevel() ) );	// 他
					pUser->UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_NUM, pItemElem->m_nItemNum - 1 );

					// log
					CItemElem* pPetItem		= pUser->GetPetItem();
					g_dpDBClient.CalluspPetLog( pUser->m_idPlayer, pPetItem->GetSerialNumber(), 0, PETLOGTYPE_MIRACLE, pPet );
				}
				else
				{
					pUser->AddDefinedText( TID_GAME_PET_BETWEEN_B_TO_S );
				}
			}
			else
			{
				pUser->AddDefinedText( TID_GAME_PET_NOT_FOUND );
			}
		}
		else
		{
			// error
		}
	}
}

void CDPSrvr::OnFeedPocketInactive( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_PET_FEED_POCKET ) )
			pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_PET_FEED_POCKET );
	}
}
#endif	// __PET_0410

#if __VER >= 9 // __CSC_VER9_2
void CDPSrvr::OnModifyStatus( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int nStrCount, nStaCount, nDexCount, nIntCount;
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	ar >> nStrCount >> nStaCount >> nDexCount >> nIntCount;
	
	if((nStrCount < 0 || nStaCount < 0 || nDexCount < 0 || nIntCount < 0) ||
		(nStrCount + nStaCount + nDexCount + nIntCount <= 0)) //양수 검사 및 합이 0이하일 경우 중단.
		return;

#ifdef __GAMEGUARD
	if (pUser->m_nRemainGP < 0 || nStrCount > pUser->m_nRemainGP || nStaCount > pUser->m_nRemainGP || nDexCount > pUser->m_nRemainGP || nIntCount > pUser->m_nRemainGP)
		return;
#endif // __GAMEGUARD

	if( pUser->m_nRemainGP >= (nStrCount + nStaCount + nDexCount + nIntCount) )
	{
		pUser->m_nStr += nStrCount;
		pUser->m_nSta += nStaCount;
		pUser->m_nDex += nDexCount;
		pUser->m_nInt += nIntCount;
		pUser->m_nRemainGP = pUser->m_nRemainGP - (nStrCount + nStaCount + nDexCount + nIntCount);
#ifdef __GAMEGUARD
		pUser->AddSetState(pUser->m_nStr, pUser->m_nSta, pUser->m_nDex, pUser->m_nInt, pUser->m_nRemainGP, FALSE);
#else // __GAMEGUARD
		pUser->AddSetState(pUser->m_nStr, pUser->m_nSta, pUser->m_nDex, pUser->m_nInt, pUser->m_nRemainGP);
#endif // __GAMEGUARD
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
		pUser->CheckHonorStat();
		pUser->AddHonorListAck();//09.02.12
		g_UserMng.AddHonorTitleChange( pUser, pUser->m_nHonor);
#endif	// __HONORABLE_TITLE			// 달인
	}
}	
#endif //__CSC_VER9_2

#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
void CDPSrvr::OnLegendSkillStart( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;
	
	OBJID objItemId[5];
	
	for( int i=0; i<5; ++i )
		ar >> objItemId[i];

	if (!pUser->IsHero() && !pUser->IsLegendHero())
		return;

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{				
		LPSKILL lpSkill = &(pUser->m_aJobSkill[i]);
		if( lpSkill && lpSkill->dwSkill != NULL_ID )
		{
			ItemProp* pSkillProp    = prj.GetSkillProp( lpSkill->dwSkill );			
			if( pSkillProp == NULL )
				continue;
			if( pSkillProp->dwItemKind1 != JTYPE_HERO)
				continue;
			if( lpSkill->dwLevel > 4 )
			{
				pUser->AddLegendSkillResult(-1);
				return;
			}
		}
	}	

	CItemElem* pItemElem[5];
	// 일치하는지 검사 (인벤토리에서 검사)
	for( int i=0; i<5; i++ )
	{
		pItemElem[i]	= (CItemElem*)pUser->m_Inventory.GetAtId( objItemId[i] );
		if( IsUsableItem( pItemElem[i] ) == FALSE )
			return ;
	}

	if( pItemElem[0]->m_dwItemId != II_GEN_MAT_DIAMOND ||
		pItemElem[1]->m_dwItemId != II_GEN_MAT_EMERALD ||
		pItemElem[2]->m_dwItemId != II_GEN_MAT_SAPPHIRE ||
		pItemElem[3]->m_dwItemId != II_GEN_MAT_RUBY ||
		pItemElem[4]->m_dwItemId != II_GEN_MAT_TOPAZ )
		return;

	// 모두 일치하면 보석 아이템 삭제
	for( int i=0; i<5 ; i++ )
	{
		LogItemInfo aLogItem;
		aLogItem.Action = "+";
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = "LEGENDSKILL_USE";
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		OnLogItem( aLogItem, pItemElem[i], 1 );
		pUser->RemoveItem( (BYTE)( objItemId[i] ), (short)1 );
	}

	
	if( xRandom(1000) > 766 )
	{
		for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
		{				
			LPSKILL lpSkill = &(pUser->m_aJobSkill[i]);
			if( lpSkill && lpSkill->dwSkill != NULL_ID )
			{
				ItemProp* pSkillProp    = prj.GetSkillProp( lpSkill->dwSkill );			
				if( pSkillProp == NULL )
					continue;
				if( pSkillProp->dwItemKind1 != JTYPE_HERO)
					continue;
				lpSkill->dwLevel++;

				g_dpDBClient.SendLogSkillPoint( LOG_SKILLPOINT_USE, 1, (CMover*)pUser, &(pUser->m_aJobSkill[i]) );
			}
		}	
		g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_SYS_EXCHAN01, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
		pUser->AddDoUseSkillPoint( &(pUser->m_aJobSkill[0]), pUser->m_nSkillPoint );
#ifdef __S_NEW_SKILL_2
		g_dpDBClient.SaveSkill( pUser );
#endif // __S_NEW_SKILL_2
		pUser->AddLegendSkillResult(TRUE);	
	}
	else
	{
		pUser->AddLegendSkillResult(FALSE);
	}
}
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans

#if __VER >= 11 // __GUILD_COMBAT_1TO1
void CDPSrvr::OnGC1to1TenderOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

#ifdef __GAMEGUARD
	if (g_eLocal.GetState(EVE_GUILDWAR) == FALSE)
	{
		pUser->AddText("Guild Siege is not active", COLOR_ERROR);
		return;
	}
#endif // __GAMEGUARD

	g_GuildCombat1to1Mng.SendTenderGuildOpenWnd( pUser );
}

void CDPSrvr::OnGC1to1TenderView( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

#ifdef __GAMEGUARD
	if (g_eLocal.GetState(EVE_GUILDWAR) == FALSE)
	{
		pUser->AddText("Guild Siege is not active", COLOR_ERROR);
		return;
	}
#endif // __GAMEGUARD

	g_GuildCombat1to1Mng.SendTenderGuildView( pUser );
}

void CDPSrvr::OnGC1to1Tender( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

#ifdef __GAMEGUARD
	if (g_eLocal.GetState(EVE_GUILDWAR) == FALSE)
	{
		pUser->AddText("Guild Siege is not active", COLOR_ERROR);
		return;
	}
#endif // __GAMEGUARD

	int nPenya;
	ar >> nPenya;

	g_GuildCombat1to1Mng.SetTenderGuild( pUser, nPenya );
}

void CDPSrvr::OnGC1to1CancelTender( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

#ifdef __GAMEGUARD
	if (g_eLocal.GetState(EVE_GUILDWAR) == FALSE)
	{
		pUser->AddText("Guild Siege is not active", COLOR_ERROR);
		return;
	}
#endif // __GAMEGUARD

	g_GuildCombat1to1Mng.SetCancelTenderGuild( pUser );
}

void CDPSrvr::OnGC1to1FailedTender( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

#ifdef __GAMEGUARD
	if (g_eLocal.GetState(EVE_GUILDWAR) == FALSE)
	{
		pUser->AddText("Guild Siege is not active", COLOR_ERROR);
		return;
	}
#endif // __GAMEGUARD

	g_GuildCombat1to1Mng.SetFailedTenderGuild( pUser );
}


void CDPSrvr::OnGC1to1MemberLineUpOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

#ifdef __GAMEGUARD
	if (g_eLocal.GetState(EVE_GUILDWAR) == FALSE)
	{
		pUser->AddText("Guild Siege is not active", COLOR_ERROR);
		return;
	}
#endif // __GAMEGUARD

	g_GuildCombat1to1Mng.SendMemberLineUpOpenWnd( pUser );
}

void CDPSrvr::OnGC1to1MemberLineUp( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

#ifdef __GAMEGUARD
	if (g_eLocal.GetState(EVE_GUILDWAR) == FALSE)
	{
		pUser->AddText("Guild Siege is not active", COLOR_ERROR);
		return;
	}
#endif // __GAMEGUARD

	vector<u_long> vecMemberId;
	u_long nTemp;
	int nSize;

	ar >> nSize;

	if( nSize > g_GuildCombat1to1Mng.m_nMaxJoinPlayer || nSize < 0 )
		return;

	for( int i=0; i<nSize; i++ )
	{
		ar >> nTemp;
		vecMemberId.push_back( nTemp );
	}
	
	g_GuildCombat1to1Mng.SetMemberLineUp( pUser, vecMemberId );
}

void CDPSrvr::OnGC1to1TeleportToNPC( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

#ifdef __GAMEGUARD
	if (g_eLocal.GetState(EVE_GUILDWAR) == FALSE)
	{
		pUser->AddText("Guild Siege is not active", COLOR_ERROR);
		return;
	}
#endif // __GAMEGUARD

	g_GuildCombat1to1Mng.SetTeleportToNPC( pUser );
}

void CDPSrvr::OnGC1to1TeleportToStage( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

#ifdef __GAMEGUARD
	if (g_eLocal.GetState(EVE_GUILDWAR) == FALSE)
	{
		pUser->AddText("Guild Siege is not active", COLOR_ERROR);
		return;
	}
#endif // __GAMEGUARD

	g_GuildCombat1to1Mng.SetTeleportToStage( pUser );
}
#endif // __GUILD_COMBAT_1TO1

#if __VER >= 11 // __SYS_COLLECTING
void CDPSrvr::OnQueryStartCollecting( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		if( pUser->IsDisguise() )
			return;
#ifdef __GAMEGUARD
		if (pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_MADRIGAL)
#endif // __GAMEGUARD
		{

#ifdef __MAX_COLLECT_BY_IP
			if (g_UserMng.CanCollect(pUser->m_playAccount.lpAddr)) 
			{
				pUser->StartCollecting();
			}
			else
			{
				CString str;
				str.Format("You are already collecting with %d collectors.", __MAX_COLLECT_BY_IP);
				pUser->AddText(str);
				pUser->AddCollectingStatus(0);
			}
#else 
			pUser->StartCollecting();
#endif
		}
	}
}

void CDPSrvr::OnQueryStopCollecting( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) 
#ifdef __AEGON_FIXES
		&& pUser->IsCollecting()
#endif
		) 
		pUser->StopCollecting();
}
#endif	// __SYS_COLLECTING

	
#if __VER >= 11 // __MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
void CDPSrvr::OnQueryGuildBankLogList( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	bool bMK = FALSE;
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		BYTE byListType;
		ar >> byListType;
		CGuild *pGuild = pUser->GetGuild();

		if( pGuild )
		{
			CGuildMember* pGuildMember = pGuild->GetMember( pUser->m_idPlayer );
			if( pGuildMember )
			{
				if( pGuildMember->m_nMemberLv == GUD_KINGPIN || pGuildMember->m_nMemberLv == GUD_MASTER )
					g_dpDBClient.SendQueryGetGuildBankLogList( pUser->m_idPlayer, pGuild->m_idGuild,byListType );
			}
		}
		// 길드가 없거나 길드장이 아니면 신청 불가
	}
}
#endif //__MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
void CDPSrvr::OnHonorListReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		pUser->AddHonorListAck();
	}
}
void CDPSrvr::OnHonorChangeReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		int nChange;
		ar >> nChange;
		if( nChange > -1 && nChange < MAX_HONOR_TITLE )
		{
			int nNeed =  CTitleManager::Instance()->GetNeedCount( nChange,-1);
			if( nNeed < 0)
				return;
			if( pUser->GetHonorTitle(nChange) >= nNeed )
			{
				pUser->m_nHonor = nChange;
				g_UserMng.AddHonorTitleChange( pUser, nChange );
			}
		}
		else if(nChange == -1)
		{
			pUser->m_nHonor = nChange;
			g_UserMng.AddHonorTitleChange( pUser, nChange );
		}
	}
}
#endif	// __HONORABLE_TITLE			// 달인

#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
void CDPSrvr::OnSealCharReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		CGuild *pGuild = pUser->GetGuild();
		if( pGuild != NULL )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANGUILD );
			return;
		}
		if( pUser->IsChaotic() )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANEQUIP );
			return;
		}
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE
		int nSize	= 0;
		for( int i=0; i< pUser->m_Inventory.GetMax(); ++i )
		{
			CItemElem* pItemElem = pUser->m_Inventory.GetAtId( i );
			if( IsUsableItem( pItemElem ) == FALSE )
				continue;
			if( pItemElem->m_nItemNum > 0 )
				nSize++;
		}
		
		if( nSize > 1 )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANINVEN );
			return;
		}
		else
		{
			CItemElem* pItemElem = NULL;
			pItemElem = pUser->m_Inventory.GetAtItemId( II_SYS_SYS_SCR_SEAL );
			if( IsUsableItem( pItemElem ) == FALSE )
			{
				pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANINVEN );
				return;
			}
			else if(pItemElem->m_nItemNum > 1)
			{
				pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANINVEN );
				return;
			}
		}

		if( pUser->GetGold() > 0 )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANEQUIP );
			return;
		}
		int nBankSize	= 0;
		for( int i=0; i< pUser->m_Bank[pUser->m_nSlot].GetMax(); ++i )
		{
			CItemElem* pItemElem = pUser->m_Bank[pUser->m_nSlot].GetAtId( i );
			if( IsUsableItem( pItemElem ) == FALSE )
				continue;
			if( pItemElem->m_nItemNum > 0 )
				nBankSize++;
		}
#if __VER >= 11 // __SYS_POCKET
		if( !pUser->m_Pocket.IsAllClean() )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANBANK );
			return;
		}
#endif		
		if( nBankSize > 0 || pUser->m_dwGoldBank[pUser->m_nSlot] > 0)
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANBANK );
			return;
		}
		g_dpDBClient.SendQueryGetSealChar( pUser->m_idPlayer,pUser->m_playAccount.lpszAccount);
	}
}
void CDPSrvr::OnSealCharConmReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objidSend;
	ar >> objidSend;

	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		CGuild *pGuild = pUser->GetGuild();
		if( pGuild != NULL )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANGUILD );
			return;
		}
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE
		int nSize	= 0;
		for( int i=0; i< pUser->m_Inventory.GetMax(); ++i )
		{
			CItemElem* pItemElem = pUser->m_Inventory.GetAtId( i );
			if( IsUsableItem( pItemElem ) == FALSE )
				continue;
			if( pItemElem->m_nItemNum > 0 )
				nSize++;
		}

		CItemElem* pItemElemTrue = NULL;
		CItemElem* pItemElemtmp = NULL;
		
		if( nSize > 1 )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANINVEN );
			return;
		}
		else
		{
			pItemElemTrue = pUser->m_Inventory.GetAtItemId( II_SYS_SYS_SCR_SEAL );
			if( IsUsableItem( pItemElemTrue ) == FALSE )
			{
				pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANINVEN );
				return;
			}
			else if(pItemElemTrue->m_nItemNum > 1)
			{
				pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANINVEN );
				return;
			}
			pItemElemtmp = pItemElemTrue;
		}

		if( pUser->GetGold() > 0 )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANEQUIP );
			return;
		}
		int nBankSize	= 0;
		for( int i=0; i< pUser->m_Bank[pUser->m_nSlot].GetMax(); ++i )
		{
			CItemElem* pItemElem = pUser->m_Bank[pUser->m_nSlot].GetAtId( i );
			if( IsUsableItem( pItemElem ) == FALSE )
				continue;
			if( pItemElem->m_nItemNum > 0 )
				nBankSize++;
		}
#if __VER >= 11 // __SYS_POCKET
		if( !pUser->m_Pocket.IsAllClean() )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANBANK );
			return;
		}
#endif
		if( nBankSize > 0 || pUser->m_dwGoldBank[pUser->m_nSlot] > 0)
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANBANK );
			return;
		}
	
#if __VER >= 11 // __SYS_PLAYER_DATA
		const char* lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( objidSend );
#else	// __SYS_PLAYER_DATA
		LPCSTR lpszPlayer = prj.GetPlayerString( objidSend );
#endif	// __SYS_PLAYER_DATA

		if( lpszPlayer == NULL )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CHARSEND );
			return;
		}


		CItemElem itemElemSend;
		itemElemSend.m_dwItemId = II_SYS_SYS_SCR_SEALCHARACTER;
		itemElemSend.m_nItemNum	= 1;
		itemElemSend.SetSerialNumber();

//		lstrcpy( itemElemSend.m_szItemText, pUser->GetName() );
		memcpy( itemElemSend.m_szItemText, pUser->GetName(),sizeof(itemElemSend.m_szItemText) );

		itemElemSend.m_nRepairNumber	= pUser->GetLevel();//nlevel
		itemElemSend.m_nHitPoint	= pUser->m_idPlayer;//m_idPlayer
		itemElemSend.m_nRepair	= pUser->GetJob();//njob

		itemElemSend.m_nResistAbilityOption	= pUser->GetRemainGP();//nPOINT

		itemElemSend.SetPiercingSize( 4 );
		itemElemSend.SetPiercingItem( 0, pUser->GetSta() );		//nSTA
		itemElemSend.SetPiercingItem( 1, pUser->GetStr() );		//nSTR
		itemElemSend.SetPiercingItem( 2, pUser->GetDex() );		//nDEX
		itemElemSend.SetPiercingItem( 3, pUser->GetInt() );		//nINT
		
		LogItemInfo aLogItem;
		aLogItem.RecvName = "SEALCHAR";
		g_dpDBClient.SendQueryPostMail( objidSend, 0, itemElemSend, 0, itemElemSend.GetProp()->szName, 	(char*)GETTEXT( TID_MMI_SEALCHARITEM ) );

		aLogItem.Action = "+";
		aLogItem.SendName = pUser->GetName();
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		OnLogItem( aLogItem, &itemElemSend, 1 );


		ItemProp* pItemProp  = pItemElemtmp->GetProp();
		g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemElemtmp, pItemProp );		
//		pItemElemtmp->UseItem();
		OBJID       dwTmpObjId = pItemElemtmp->m_dwObjId;
		pUser->RemoveItem( (BYTE)( dwTmpObjId ), (short)1 );

		pUser->ClearAllSMMode();
		pUser->RemoveAllBuff();

		g_dpDBClient.SendQueryGetSealCharConm( pUser->m_idPlayer);

		QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer ); // pUser->m_Snapshot.dpidUser에는 소켓번호가 들어가 있다.
	}
}
void CDPSrvr::OnSealCharGetReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwData;
	DWORD dwId;

	ar >> dwData ;

	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		dwId = HIWORD( dwData );

		if( pUser->IsUsableState( dwId ) == FALSE )
			return;

#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE

		CItemElem* pItemElem = pUser->m_Inventory.GetAtId( dwId );
		if( IsUsableItem( pItemElem ) )
		{
			if(pItemElem->m_dwItemId != II_SYS_SYS_SCR_SEALCHARACTER )
				return;
			g_dpDBClient.SendQueryGetSealCharGet( pUser->m_idPlayer,pUser->m_playAccount.lpszAccount,dwId);
		}
	}
}

#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

#if __VER >= 11 // __SYS_POCKET
void	CDPSrvr::OnMoveItemOnPocket( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int nPocket1, nItem, nPocket2;
		short nNum;
		ar >> nPocket1 >> nItem >> nNum >> nPocket2;

		if( nPocket1 == nPocket2 )
			return;

#ifdef __GAMEGUARD
		if (nNum <= 0 || nItem < 0 || nPocket2 > MAX_POCKET || nPocket2 < -1 || nPocket1 < -1 || nPocket1 > MAX_POCKET)
			return;
#endif // __GAMEGUARD

#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE

		// mirchang 091214 - 착용중인 아이템인지 체크
		if( nPocket1 == -1 )	// 원본이 인벤토리인지 검사!
		{
			if( pUser->m_Inventory.IsEquip( nItem ) )
				return;
		}

		CItemElem* pItem	= pUser->GetItemId2( nPocket1, nItem );	// 여기서 휴대가방 만료검사 같이함.
		if( IsUsableItem( pItem ) )
		{
			if( nPocket1 < 0 && pUser->IsUsing( pItem ) )
				return;

			if( nNum > pItem->m_nItemNum )
				return;

#ifdef __SECURITY_FIXES
			if (nNum < 1)
				return;
#endif // __SECURITY_FIXES

			CItemElem item;
			item	= *pItem;
			item.m_nItemNum		= nNum;
#if (defined (__FL_BAG_LOOT) && defined (__DROP_LOG)) || (defined (__FL_BAG_LOOT) && defined (__FL_FARM_STAT))
			if (pUser->CreateItem2(&item, nPocket2, FALSE))
#else
			if (pUser->CreateItem2(&item, nPocket2))
#endif
			{
				pUser->RemoveItem2( nItem, nNum, nPocket1 );
				// log
				LogItemInfo	log;
				log.Action	= "m";
				log.SendName	= pUser->GetName();
				log.RecvName	= "OnMoveItemOnPocket";
				log.WorldId		= pUser->GetWorld()->GetID();
				log.Gold	= nPocket1;
				log.Gold2	= nPocket2;
				OnLogItem( log, &item, nNum );
			}
		}
	}
}

void	CDPSrvr::OnAvailPocket( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		int nPocket, nItem;
		ar >> nPocket >> nItem;
		if( nPocket <= 0 && nPocket >= MAX_POCKET )
			return;
#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE
		CItemElem* pItemElem	= pUser->m_Inventory.GetAtId( nItem );
		if( IsUsableItem( pItemElem ) )
		{
			ItemProp* pItemProp		= pItemElem->GetProp();
			if( pItemProp->dwItemKind3 == IK3_POCKET )
			{
				if( !pUser->m_Pocket.IsAvailable( nPocket ) )
				{
					pUser->m_Pocket.SetAttribute( CPocketController::avail, nPocket, pItemProp->dwSkillTime );
					// log
					LogItemInfo	log;
					log.Action	= "u";
					log.SendName	= pUser->GetName();
					log.RecvName	= "OnAvailPocket";
					log.WorldId		= pUser->GetWorld()->GetID();
					log.Gold	= pUser->GetGold();
					log.Gold2	= pUser->GetGold();
					log.Gold_1	= nPocket;
					OnLogItem( log, pItemElem, 1 );
					pUser->UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_NUM, pItemElem->m_nItemNum - 1 );
#ifdef __INTERNALSERVER
					pUser->AddPocketView();
#endif	// __INTERNALSERVER
				}
			}
		}
	}
}
#endif	// __SYS_POCKET

#if __VER >= 11 // __SYS_IDENTIFY
void	CDPSrvr::OnBlessednessCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_BLESSING_CANCEL, pUser->GetWorld(), pUser->GetPos() ) )
			return;

		int nItem;
		ar >> nItem;
		CItemElem* pItem	= (CItemElem*)pUser->GetItemId( nItem );
		if( IsUsableItem( pItem ) )
		{
#ifdef __GAMEGUARD
			if (pUser->m_Inventory.IsEquip(pItem->m_dwObjId))
				return;
#endif // __GAMEGUARD
			if( g_xRandomOptionProperty->GetRandomOptionKind( pItem ) == CRandomOptionProperty::eBlessing
				&& g_xRandomOptionProperty->GetRandomOptionSize( pItem->GetRandomOptItemId() ) > 0 )
			{
#ifdef __AEGON_RETURN_AWAKE
				if (pItem->GetRandomOptItemId())
				{
					g_xRandomOptionProperty->InitializeRandomOption(pItem->GetOldRandomOptItemIdPtr());
					pItem->SetOldRandomOptItemId(pItem->GetRandomOptItemId());
					pUser->UpdateItemEx((BYTE)(pItem->m_dwObjId), UI_OLDRANDOMOPTITEMID, pItem->GetOldRandomOptItemId());
				}
#endif // __AEGONE_RETURN_AWAKE
				g_xRandomOptionProperty->InitializeRandomOption( pItem->GetRandomOptItemIdPtr() );
				pUser->UpdateItemEx( (BYTE)( pItem->m_dwObjId ), UI_RANDOMOPTITEMID, pItem->GetRandomOptItemId() );
				pUser->AddDiagText( prj.GetText( TID_GAME_BLESSEDNESS_CANCEL_INFO ) );
				// log
				LogItemInfo	log;
				log.Action	= "r";
				log.SendName	= pUser->GetName();
				log.RecvName	= "OnBlessednessCancel";
				log.WorldId		= pUser->GetWorld()->GetID();
				log.Gold	= pUser->GetGold();
				log.Gold2	= pUser->GetGold();
				OnLogItem( log, pItem, 1 );
			}
			else
			{
				pUser->AddDefinedText( TID_GAME_BLESSEDNESS_CANCEL );	
			}
		}		
	}
}

void	CDPSrvr::OnAwakening( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{

#ifdef __AEGON_AWAKEWINDOW
		const int	nCost = 200000; // 200K
#else //__AEGON_AWAKEWINDOW
		if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_ITEM_AWAKENING, pUser->GetWorld(), pUser->GetPos()))
			return;
		const int	nCost = 100000;
#endif //__AEGON_AWAKEWINDOW

		if( pUser->GetGold() < nCost )
		{
			pUser->AddDefinedText( TID_GAME_LACKMONEY, "" );
			return;
		}
		int	nItem;
		ar >> nItem;
		CItemElem* pItem	= pUser->m_Inventory.GetAtId( nItem );
		if( IsUsableItem( pItem ) )
		{

#ifdef __GAMEGUARD
			if (pItem->m_dwItemId == II_GEN_TOO_COL_NOMAL || pItem->m_dwItemId == II_GEN_TOO_COL_VTHREE || pItem->m_dwItemId == II_GEN_TOO_COL_POWER || pItem->m_dwItemId == II_GEN_TOO_COL_ULTRA || pItem->m_dwItemId == II_GEN_TOO_COL_NORMAL || pItem->m_dwItemId == II_GEN_TOO_COL_EVENT)
			{
				pUser->AddText(("You can't awake this item."), COLOR_ERROR);
				return;
			}
#endif // __GAMEGUARD

			int nRandomOptionKind	= g_xRandomOptionProperty->GetRandomOptionKind( pItem );

#ifdef __AEGON_AWAKEWINDOW
			if (nRandomOptionKind != CRandomOptionProperty::eAwakening					//eAwakening -> Sets / Weapons
#ifdef __AWAKENING_EXTENDED
				&& nRandomOptionKind != CRandomOptionProperty::eWeapon
#endif //__AWAKENING_EXTENDED
				//&& nRandomOptionKind != CRandomOptionProperty::eAwakeningExtension	//?
				//&& nRandomOptionKind != CRandomOptionProperty::eMaxRandomOptionKind	//?
				&& nRandomOptionKind != CRandomOptionProperty::eBlessing				//eBlessing -> Fashion
				&& nRandomOptionKind != CRandomOptionProperty::eEatPet					//eEatPet -> buffpet
				&& nRandomOptionKind != CRandomOptionProperty::eSystemPet)			    //eSystemPet -> normal pet 			
			{
				LPCTSTR wrongItem = _T("Invalid Item!");
				pUser->AddText(wrongItem, COLOR_ERROR);
				return;
			}
#else // __AEGON_AWAKEWINDOW
			if (nRandomOptionKind != CRandomOptionProperty::eAwakening
#ifdef __AWAKENING_EXTENDED
				&& nRandomOptionKind != CRandomOptionProperty::eWeapon
#endif //__AWAKENING_EXTENDED						
				)
			{
				pUser->AddDefinedText(TID_GAME_INVALID_TARGET_ITEM);
				return;
			}
			if (g_xRandomOptionProperty->GetRandomOptionSize(pItem->GetRandomOptItemId()) > 0)
			{
				pUser->AddDefinedText(TID_GAME_AWAKE_OR_BLESSEDNESS01);
				return;
			}
#endif //__AEGON_AWAKEWINDOW

#ifdef __AEGON_RETURN_AWAKE
			if (pItem->GetRandomOptItemId())
			{
				g_xRandomOptionProperty->InitializeRandomOption(pItem->GetOldRandomOptItemIdPtr());
				pItem->SetOldRandomOptItemId(pItem->GetRandomOptItemId());
				pUser->UpdateItemEx((BYTE)(pItem->m_dwObjId), UI_OLDRANDOMOPTITEMID, pItem->GetOldRandomOptItemId());
			}

#endif // __AEGON_RETURN_AWAKE
			g_xRandomOptionProperty->InitializeRandomOption( pItem->GetRandomOptItemIdPtr() );
			g_xRandomOptionProperty->GenRandomOption( pItem->GetRandomOptItemIdPtr(), nRandomOptionKind, pItem->GetProp()->dwParts );
			pUser->UpdateItemEx( (BYTE)( pItem->m_dwObjId ), UI_RANDOMOPTITEMID, pItem->GetRandomOptItemId() );
			pUser->AddGold( -nCost );
			pUser->AddDefinedText( TID_GAME_AWAKENING_SUCCESS );
			// log
			LogItemInfo	log;
			log.Action	= "r";
			log.SendName	= pUser->GetName();
			log.RecvName	= "OnAwakening";
			log.WorldId		= pUser->GetWorld()->GetID();
			log.Gold	= pUser->GetGold() + nCost;
			log.Gold2	= pUser->GetGold();
			log.Gold_1	= -nCost;
			OnLogItem( log, pItem, 1 );
		}
	}
}
#endif	// __SYS_IDENTIFY

#ifdef __NPC_BUFF
void CDPSrvr::OnNPCBuff( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
#ifdef __FIX_BUFFPANG_SPAM
		if (pUser->m_dwLastBuff + SEC(10) > GetTickCount() && !pUser->IsAuthHigher(AUTH_ADMINISTRATOR))
		{
			pUser->AddText("Please wait ...", COLOR_WARNING);
			return;
		}
#endif // __FIX_BUFFPANG_SPAM
		CHAR	m_szKey[64];
		ar.ReadString(m_szKey, 64);
		LPCHARACTER lpChar = prj.GetCharacter( m_szKey );
		
		if( lpChar )
		{
			if( !CNpcChecker::GetInstance()->IsCloseNpc(MMI_NPC_BUFF, pUser->GetWorld(), pUser->GetPos() ) )
				return;

			vector<NPC_BUFF_SKILL> vecNPCBuff = lpChar->m_vecNPCBuffSkill;
			for( int i=0; i<(int)( vecNPCBuff.size() ); i++ )
			{
				if( pUser->GetLevel() >= vecNPCBuff[i].nMinPlayerLV && pUser->GetLevel() <= vecNPCBuff[i].nMaxPlayerLV )
				{
					ItemProp* pSkillProp = prj.GetSkillProp( vecNPCBuff[i].dwSkillID );
					if( pSkillProp )
					{
						if( vecNPCBuff[i].dwSkillLV < pSkillProp->dwExpertLV || vecNPCBuff[i].dwSkillLV > pSkillProp->dwExpertMax )
							continue;
						
						if(	( pSkillProp->dwID == SI_GEN_EVE_QUICKSTEP && pUser->HasBuff(BUFF_SKILL, SI_ASS_CHEER_QUICKSTEP) )
							|| ( pSkillProp->dwID == SI_GEN_EVE_HASTE && pUser->HasBuff(BUFF_SKILL, SI_ASS_CHEER_HASTE) )
							|| ( pSkillProp->dwID == SI_GEN_EVE_HEAPUP && pUser->HasBuff(BUFF_SKILL, SI_ASS_CHEER_HEAPUP) )
	  						|| ( pSkillProp->dwID == SI_GEN_EVE_ACCURACY && pUser->HasBuff(BUFF_SKILL, SI_ASS_CHEER_ACCURACY) ) )
						{
							pUser->AddDefinedText( TID_GAME_NPCBUFF_FAILED, "\"%s\"", pSkillProp->szName );
							continue;
						}
						
						AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine + vecNPCBuff[i].dwSkillLV - 1 );
						if( pAddSkillProp )
						{
							// skill property를 수정하기 때문에 백업해두었다가
							// 스킬 시전후 restore시킨다.
							DWORD dwReferTarget1Backup = pSkillProp->dwReferTarget1;
							pSkillProp->dwReferTarget1 = NULL_ID;
							DWORD dwReferTarget2Backup = pSkillProp->dwReferTarget2;
							pSkillProp->dwReferTarget2 = NULL_ID;
							DWORD dwSkillTimeBackup = pAddSkillProp->dwSkillTime;
							pAddSkillProp->dwSkillTime = vecNPCBuff[i].dwSkillTime;

							pUser->DoApplySkill( pUser, pSkillProp, pAddSkillProp );
							g_UserMng.AddDoApplySkill( pUser, pUser->GetId(), vecNPCBuff[i].dwSkillID, vecNPCBuff[i].dwSkillLV );

							pSkillProp->dwReferTarget1 = dwReferTarget1Backup;
							pSkillProp->dwReferTarget2 = dwReferTarget2Backup;
							pAddSkillProp->dwSkillTime = dwSkillTimeBackup;
						}
					}
				}
				else
				{
					ItemProp* pSkillProp = prj.GetSkillProp( vecNPCBuff[i].dwSkillID );
					if( pSkillProp )
						pUser->AddDefinedText( TID_GAME_NPCBUFF_LEVELLIMIT, "%d %d \"%s\"", vecNPCBuff[i].nMinPlayerLV, vecNPCBuff[i].nMaxPlayerLV, pSkillProp->szName );
				}
			}
		}
#ifdef __FIX_BUFFPANG_SPAM
		pUser->m_dwLastBuff = GetTickCount();
#endif // __FIX_BUFFPANG_SPAM
	}
}
#endif // __NPC_BUFF

#ifdef __NPC_BUFF_CLASS
void CDPSrvr::OnNPCBuffClass(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
#ifdef __FIX_BUFFPANG_SPAM
		if (pUser->m_dwLastBuffClass + SEC(10) > GetTickCount() && !pUser->IsAuthHigher(AUTH_ADMINISTRATOR))
		{
			pUser->AddText("Please wait ...", COLOR_WARNING);
			return;
		}
#endif // __FIX_BUFFPANG_SPAM
		CHAR	m_szKey[64];
		ar.ReadString(m_szKey, 64);
		LPCHARACTER lpChar = prj.GetCharacter(m_szKey);

		if (lpChar)
		{
			if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_NPC_BUFF_CLASS, pUser->GetWorld(), pUser->GetPos()))
				return;

			int nJob = pUser->GetJob();
			int nLevel = pUser->GetLevel();
			vector<DWORD> vBuffs;

			if (nJob == JOB_MERCENARY || nJob == JOB_KNIGHT || nJob == JOB_KNIGHT_MASTER || nJob == JOB_KNIGHT_HERO || nJob == JOB_LORDTEMPLER_HERO || nJob == JOB_BLADE || nJob == JOB_BLADE_MASTER || nJob == JOB_BLADE_HERO || nJob == JOB_STORMBLADE_HERO)
			{
				vBuffs.push_back(SI_MER_SHIELD_PROTECTION);
				vBuffs.push_back(SI_MER_SHIELD_PANBARRIER);
				vBuffs.push_back(SI_MER_SUP_IMPOWERWEAPON);
				vBuffs.push_back(SI_MER_SUP_SMITEAXE);
				vBuffs.push_back(SI_MER_SUP_AXEMASTER);
				vBuffs.push_back(SI_MER_SUP_BLAZINGSWORD);
				vBuffs.push_back(SI_MER_SUP_SWORDMASTER);

				if (nJob == JOB_KNIGHT || nJob == JOB_KNIGHT_MASTER || nJob == JOB_KNIGHT_HERO || nJob == JOB_LORDTEMPLER_HERO)
				{
					if (nJob >= JOB_KNIGHT)
						vBuffs.push_back(SI_KNT_SUP_PAINREFLECTION);
					vBuffs.push_back(SI_KNT_SUP_GUARD);
					vBuffs.push_back(SI_KNT_SUP_RAGE);
					if (nJob >= JOB_KNIGHT_MASTER)
						vBuffs.push_back(SI_KNT_MASTER_TWOHANDMASTER);
					if (nJob >= JOB_KNIGHT_HERO)
					{
					}
					if (nJob >= JOB_LORDTEMPLER_HERO)
						vBuffs.push_back(SI_LOD_SUP_HOLYARMOR);
				}
				if (nJob == JOB_BLADE || nJob == JOB_BLADE_MASTER || nJob == JOB_BLADE_HERO || nJob == JOB_STORMBLADE_HERO)
				{
					if (nJob >= JOB_BLADE)
					{
						vBuffs.push_back(SI_BLD_SUP_BERSERK);
					}
					if (nJob >= JOB_BLADE_MASTER)
						vBuffs.push_back(SI_BLD_MASTER_ONEHANDMASTER);
					if (nJob >= JOB_BLADE_HERO)
					{
					}
					if (nJob >= JOB_STORMBLADE_HERO)
						vBuffs.push_back(SI_STORM_SUP_POWERINCREASE);
				}
			}

			if (nJob == JOB_ACROBAT || nJob == JOB_JESTER || nJob == JOB_JESTER_MASTER || nJob == JOB_JESTER_HERO || nJob == JOB_WINDLURKER_HERO || nJob == JOB_RANGER || nJob == JOB_RANGER_MASTER || nJob == JOB_RANGER_HERO || nJob == JOB_CRACKSHOOTER_HERO)
			{
				vBuffs.push_back(SI_ACR_YOYO_ABSOLUTEBLOCK);
				vBuffs.push_back(SI_ACR_SUP_YOYOMASTER);
				vBuffs.push_back(SI_ACR_SUP_BOWMASTER);

				if (nJob == JOB_JESTER || nJob == JOB_JESTER_MASTER || nJob == JOB_JESTER_HERO || nJob == JOB_WINDLURKER_HERO)
				{
					if (nJob >= JOB_JESTER)
					{
						vBuffs.push_back(SI_JST_SUP_POISON);
						vBuffs.push_back(SI_JST_SUP_BLEEDING);
						vBuffs.push_back(SI_JST_YOYO_CRITICALSWING);
						vBuffs.push_back(SI_JST_SUP_ABSORB);
					}
					if (nJob >= JOB_JESTER_MASTER)
						vBuffs.push_back(SI_JST_MASTER_YOYOMASTER);
					if (nJob >= JOB_JESTER_HERO)
					{
					}
					if (nJob >= JOB_WINDLURKER_HERO)
					{
						vBuffs.push_back(SI_WIN_SUP_EVASIONINCREASE);
						vBuffs.push_back(SI_WIN_SUP_CONTROLINCREASE);
					}
				}
				if (nJob == JOB_RANGER || nJob == JOB_RANGER_MASTER || nJob == JOB_RANGER_HERO || nJob == JOB_CRACKSHOOTER_HERO)
				{
					if (nJob >= JOB_RANGER)
					{
						vBuffs.push_back(SI_RAG_SUP_FASTATTACK);
						vBuffs.push_back(SI_RAG_SUP_NATURE);
					}
					if (nJob >= JOB_RANGER_MASTER)
						vBuffs.push_back(SI_RAG_MASTER_BOWMASTER);
					if( nJob >= JOB_RANGER_HERO )		
						vBuffs.push_back( SI_RAG_HERO_HAWKEYE );
					if (nJob >= JOB_CRACKSHOOTER_HERO)
					{
						vBuffs.push_back(SI_CRA_SUP_CONTROLINCREASE);
						vBuffs.push_back(SI_CRA_SUP_POWERINCREASE);
						vBuffs.push_back(SI_CRA_SUP_HAWKEYE);
					}
				}
			}

			if (nJob == JOB_ASSIST || nJob == JOB_RINGMASTER || nJob == JOB_RINGMASTER_MASTER || nJob == JOB_RINGMASTER_HERO || nJob == JOB_FLORIST_HERO || nJob == JOB_BILLPOSTER || nJob == JOB_BILLPOSTER_MASTER || nJob == JOB_BILLPOSTER_HERO || nJob == JOB_FORCEMASTER_HERO)
			{
				vBuffs.push_back(SI_ASS_CHEER_STONEHAND);

				vBuffs.push_back(SI_ASS_HEAL_PREVENTION);

				vBuffs.push_back(SI_ASS_HEAL_PATIENCE);
				vBuffs.push_back(SI_ASS_CHEER_HEAPUP);
				vBuffs.push_back(SI_ASS_CHEER_CANNONBALL);
				vBuffs.push_back(SI_ASS_CHEER_MENTALSIGN);
				vBuffs.push_back(SI_ASS_CHEER_BEEFUP);
				vBuffs.push_back(SI_ASS_CHEER_QUICKSTEP);
				vBuffs.push_back(SI_ASS_CHEER_CATSREFLEX);
				vBuffs.push_back(SI_ASS_CHEER_ACCURACY);
				vBuffs.push_back(SI_ASS_CHEER_HASTE);


				if (nJob == JOB_RINGMASTER || nJob == JOB_RINGMASTER_MASTER || nJob == JOB_RINGMASTER_HERO || nJob == JOB_FLORIST_HERO)
				{
					if (nJob >= JOB_RINGMASTER)
					{
						vBuffs.push_back(SI_RIN_SUP_PROTECT);
						vBuffs.push_back(SI_RIN_SUP_HOLYGUARD);
						vBuffs.push_back(SI_RIN_SUP_SPIRITUREFORTUNE);
						vBuffs.push_back(SI_RIN_SQU_GEBURAHTIPHRETH);

					}
					if (nJob >= JOB_RINGMASTER_MASTER)
					{
					}
					if (nJob >= JOB_RINGMASTER_HERO)
					{
					}
					if (nJob >= JOB_FLORIST_HERO)
					{
						vBuffs.push_back(SI_FLO_SQU_BLESSBODY);
						vBuffs.push_back(SI_FLO_SQU_BLESSARMOR);
						vBuffs.push_back(SI_FLO_SQU_BLESSSTEP);
						vBuffs.push_back(SI_FLO_SUP_ABSOLUTE);
						//vBuffs.push_back( SI_FLO_HEAL_DOT );
					}
				}
				if (nJob == JOB_BILLPOSTER || nJob == JOB_BILLPOSTER_MASTER || nJob == JOB_BILLPOSTER_HERO || nJob == JOB_FORCEMASTER_HERO)
				{
					if (nJob >= JOB_BILLPOSTER)
						vBuffs.push_back(SI_BIL_PST_ASMODEUS);
					if (nJob >= JOB_BILLPOSTER_MASTER)
						vBuffs.push_back(SI_BIL_MASTER_KNUCKLEMASTER);
					if (nJob >= JOB_BILLPOSTER_HERO)
					{
					}
					if (nJob >= JOB_FORCEMASTER_HERO)
					{
						vBuffs.push_back(SI_FOR_SQU_FORCETENACITY);
						vBuffs.push_back(SI_FOR_SQU_FORCEANGER);
						vBuffs.push_back(SI_FOR_SQU_FORCESPEED);
						vBuffs.push_back(SI_FOR_SQU_FORCEMAD);
					}
				}
			}

			if (nJob == JOB_MAGICIAN || nJob == JOB_PSYCHIKEEPER || nJob == JOB_PSYCHIKEEPER_MASTER || nJob == JOB_PSYCHIKEEPER_HERO || nJob == JOB_MENTALIST_HERO || nJob == JOB_ELEMENTOR || nJob == JOB_ELEMENTOR_MASTER || nJob == JOB_ELEMENTOR_HERO || nJob == JOB_ELEMENTORLORD_HERO)
			{
				if (nJob == JOB_PSYCHIKEEPER || nJob == JOB_PSYCHIKEEPER_MASTER || nJob == JOB_PSYCHIKEEPER_HERO || nJob == JOB_MENTALIST_HERO)
				{
					if (nJob >= JOB_PSYCHIKEEPER)
						vBuffs.push_back(SI_PSY_NLG_CRUCIOSPELL);
					if (nJob >= JOB_PSYCHIKEEPER_MASTER)
						vBuffs.push_back(SI_PSY_MASTER_INTMASTER);
					if (nJob >= JOB_PSYCHIKEEPER_HERO)
					{
					}
					if (nJob >= JOB_MENTALIST_HERO)
					{
					}
				}
				if (nJob == JOB_ELEMENTOR || nJob == JOB_ELEMENTOR_MASTER || nJob == JOB_ELEMENTOR_HERO || nJob == JOB_ELEMENTORLORD_HERO)
				{
					if (nJob >= JOB_ELEMENTOR)
					{
						vBuffs.push_back(SI_ELE_FIRE_FIREMASTER);
						vBuffs.push_back(SI_ELE_ELECTRICITY_LIGHTINGMASTER);
						vBuffs.push_back(SI_ELE_EARTH_EARTHMASTER);
						vBuffs.push_back(SI_ELE_WATER_WATERMASTER);
						vBuffs.push_back(SI_ELE_WIND_WINDMASTER);
					}
					if (nJob >= JOB_ELEMENTOR_MASTER)
						vBuffs.push_back(SI_ELE_MASTER_INTMASTER);
					if (nJob >= JOB_ELEMENTOR_HERO)
					{
					}
					if (nJob >= JOB_ELEMENTORLORD_HERO)
					{
					}
				}
			}

			if (vBuffs.empty())
				return;

			BOOL bMaster = pUser->IsMaster();
			BOOL bHero = pUser->IsHero();
			for (unsigned int i = 0; i < vBuffs.size(); i++)
			{

				if (vBuffs[i] == 0)
					return;

				ItemProp* pSkillProp = prj.GetSkillProp(vBuffs[i]);
				if (pSkillProp)
				{
					if (pSkillProp->dwReqDisLV <= (DWORD)nLevel)
					{
						AddSkillProp* pAddSkillProp = prj.GetAddSkillProp(pSkillProp->dwSubDefine + pSkillProp->dwExpertMax - 1);
						if (pAddSkillProp)
						{
							DWORD dwReferTarget1Backup = pSkillProp->dwReferTarget1;
							pSkillProp->dwReferTarget1 = NULL_ID;
							DWORD dwReferTarget2Backup = pSkillProp->dwReferTarget2;
							pSkillProp->dwReferTarget2 = NULL_ID;
							DWORD dwSkillTimeBackup = 3600000; // 1 Hour.
							pAddSkillProp->dwSkillTime = dwSkillTimeBackup;

							pUser->DoApplySkill(pUser, pSkillProp, pAddSkillProp);
							g_UserMng.AddDoApplySkill(pUser, pUser->GetId(), vBuffs[i], pSkillProp->dwExpertMax);
							pSkillProp->dwReferTarget1 = dwReferTarget1Backup;
							pSkillProp->dwReferTarget2 = dwReferTarget2Backup;
							pAddSkillProp->dwSkillTime = dwSkillTimeBackup;
						}
					}
				}
			}
		}
#ifdef __FIX_BUFFPANG_SPAM
		pUser->m_dwLastBuffClass = GetTickCount();
#endif // __FIX_BUFFPANG_SPAM
	}
}
#endif // __NPC_BUFF_CLASS

#ifdef __JEFF_11_4
void	CDPSrvr::OnArenaEnter( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __GAMEGUARD
		if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_ARENA_ENTER, pUser->GetWorld(), pUser->GetPos()))
		{
			pUser->AddText("Please use the Arena NPC to enter the Arena.", COLOR_ERROR);
			return;
		}
#endif //__GAMEGUARD

#ifdef __MOUNT
		if (pUser->m_pActMover->IsUseMount())
		{
			pUser->AddText("You can't enter the arena while using a mount.", COLOR_ERROR);
			return;
		}
#endif // __MOUNT

		if( pUser->IsBaseJob() )	// 1차 전직을 완료한 유저만 가능
			return;
		pUser->SetMarkingPos();
		pUser->REPLACE( g_uIdofMulti, WI_WORLD_ARENA, D3DXVECTOR3( 540.0F, 140.0F, 485.0F ), REPLACE_NORMAL, nDefaultLayer );
	}
}

void	CDPSrvr::OnArenaExit( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if (IsValidObj(pUser))
	{
#ifdef __GAMEGUARD
		if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_ARENA_EXIT, pUser->GetWorld(), pUser->GetPos()))
		{
			pUser->AddText("Please use the Arena NPC to leave the Arena.", COLOR_ERROR);
			return;
		}

		if (pUser->GetWorld()->IsArena())
#endif // __GAMEGUARD
		{
			pUser->REPLACE(g_uIdofMulti, pUser->m_idMarkingWorld, pUser->m_vMarkingPos, REPLACE_NORMAL, nTempLayer);
		}
	}
}
#endif	// __JEFF_11_4

#ifdef __JEFF_11
// 펫을 조각으로 교환
void	CDPSrvr::OnQuePetResurrection( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int nItem;
		ar >> nItem;
		CItemElem*	pItem	= (CItemElem*)pUser->GetItemId( nItem );
		if( !IsUsableItem( pItem ) )
			return;

		CPet* pPet	= pItem->m_pPet;
		if( !pPet || pUser->GetPetId() == pItem->m_dwObjId || pItem->IsFlag( CItemElem::expired ) )
		{
			pUser->AddQuePetResurrectionResult( FALSE );
			return;
		}

		BYTE nLevel		= pPet->GetLevel();
		if( nLevel < PL_B || nLevel > PL_S )
		{
			pUser->AddQuePetResurrectionResult( FALSE );
			return;
		}

		CItemElem itemElem;
		itemElem.m_nItemNum	= 1;
		switch( nLevel )
		{
			case PL_B:	itemElem.m_dwItemId		= II_SYS_SYS_QUE_PETRESURRECTION01_B;	break;
			case PL_A:	itemElem.m_dwItemId		= II_SYS_SYS_QUE_PETRESURRECTION01_A;	break;
			case PL_S:	itemElem.m_dwItemId		= II_SYS_SYS_QUE_PETRESURRECTION01_S;	break;
		}
		// log
		LogItemInfo	log;
		log.Action	= "x";
		log.SendName	= pUser->GetName();
		log.RecvName	= "OnQuePetResurrection";
		OnLogItem( log, pItem, 1 );

		pUser->RemoveItem( nItem, 1 );
		pUser->CreateItem( &itemElem );
		pUser->AddQuePetResurrectionResult( TRUE );
	}
}
#endif	// __JEFF_11

#if __VER >= 12 // __SECRET_ROOM
void	CDPSrvr::OnSecretRoomTenderOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	CSecretRoomMng::GetInstance()->SetTenderOpenWnd( pUser );
}

void	CDPSrvr::OnSecretRoomTender( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	int nPenya;
	ar >> nPenya;

	if( nPenya <= 0 )
		return;

	CSecretRoomMng::GetInstance()->SetTender( pUser, nPenya );
}

void	CDPSrvr::OnSecretRoomTenderCancelReturn( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	CSecretRoomMng::GetInstance()->SetTenderCancelReturn( pUser );
}

void	CDPSrvr::OnSecretRoomLineUpOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	CSecretRoomMng::GetInstance()->SetLineUpOpenWnd( pUser );
}

void	CDPSrvr::OnSecretRoomLineUpMember( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	set<DWORD> checker;
	vector<DWORD> vecLineUpMember;
	int nSize;
	ar >> nSize;

	if( nSize > CSecretRoomMng::GetInstance()->m_nMaxGuildMemberNum || nSize <= 0 )
		return;

	for( int i=0; i<nSize; i++ )
	{
		DWORD dwIdPlayer;
		ar >> dwIdPlayer;

		// 유효한 Player ID 인가?
		PlayerData* pData	= CPlayerDataCenter::GetInstance()->GetPlayerData( dwIdPlayer );
		if( !pData )
			return;

		// ID 중복체크		
		if( !checker.insert( dwIdPlayer ).second )
			return;

		vecLineUpMember.push_back( dwIdPlayer );
	}

	CSecretRoomMng::GetInstance()->SetLineUp( pUser, vecLineUpMember );
}

void	CDPSrvr::OnSecretRoomEntrance( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	CSecretRoomMng::GetInstance()->SetTeleportSecretRoom( pUser );
}

void	CDPSrvr::OnSecretRoomTeleportToNPC( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;
	pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, CSecretRoomMng::GetInstance()->GetRevivalPos( pUser ), REPLACE_NORMAL, nDefaultLayer );
}

void CDPSrvr::OnSecretRoomTenderView( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	CSecretRoomMng::GetInstance()->GetTenderView( pUser );
}

void CDPSrvr::OnTeleportSecretRoomDungeon( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;
#if __VER >= 12 // __TAX
	if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_SECRET_ENTRANCE_1, pUser->GetWorld(), pUser->GetPos() ) )
		return;

#ifdef __MOUNT
	if (pUser->m_pActMover->IsUseMount())
	{
		pUser->AddText("You can't teleport while using a mount.", COLOR_ERROR);
		return;
	}
#endif // __MOUNT

	__TAXINFO* pTaxInfo = CTax::GetInstance()->GetTaxInfo( CTax::GetInstance()->GetContinent( pUser ) );
	if( pTaxInfo && pUser->GetGuild() && (pTaxInfo->dwId == pUser->GetGuild()->GetGuildId()) )
	{
		pUser->SetMarkingPos();
		pUser->SetAngle( 180.0f );
		int nRandx = xRandom(4) - 2;
		int nRandz = xRandom(4) - 2;
		pUser->REPLACE( g_uIdofMulti, WI_DUNGEON_SECRET_0, D3DXVECTOR3( (float)( 295 + nRandx ), 102.0f, (float)( 530 + nRandz ) ), REPLACE_NORMAL, nDefaultLayer );
	}
	else
		pUser->AddDefinedText( TID_GAME_SECRETROOM_NOENTRANCE_1 );
#endif // __TAX
}
#endif // __SECRET_ROOM

#if __VER >= 12 // __LORD
void CDPSrvr::OnElectionAddDeposit( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		__int64 iTotal;
		ar >> iTotal;

		__int64 iDeposit;
		int nRet	= election::AddDepositRequirements( pUser, iTotal, iDeposit );
		if( nRet )
		{
			pUser->AddDefinedText( nRet );
			return;
		}
		pUser->AddGold( -static_cast<int>( iDeposit ) );
		g_dpDBClient.SendElectionAddDeposit( pUser->m_idPlayer, iDeposit );

		PutPenyaLog( pUser, "d", "DEPOSIT", static_cast<int>( iDeposit ) );
	}
}

void CDPSrvr::PutItemLog( CUser* pUser, const char* szAction, const char* szContext, CItemElem* pItem, int nNum )
{
	LogItemInfo	log;
	log.Action	=  szAction;
	log.SendName	= pUser->GetName();
	log.RecvName	= szContext;
	log.WorldId		= pUser->GetWorld() ? pUser->GetWorld()->GetID() : WI_WORLD_NONE;	// chipi_090623 수정 - 첫 접속시 만료된 버프인 경우 월드가 없는 상태로 들어온다. 
	log.Gold	= pUser->GetGold();
	log.Gold2	= pUser->GetGold();
	if( nNum == 0 )
		nNum	= pItem->m_nItemNum;
	OnLogItem( log, pItem, nNum );
}

void CDPSrvr::PutPenyaLog( CUser* pUser, const char* szAction, const char* szContext,  int nCost )
{
	LogItemInfo	log;
	log.Action	= szAction;
	log.SendName	= pUser->GetName();
	log.RecvName	= szContext;
	log.WorldId		= pUser->GetWorld() ? pUser->GetWorld()->GetID() : WI_WORLD_NONE;	// chipi_090623 수정 - 첫 접속시 만료된 버프인 경우 월드가 없는 상태로 들어온다. 
	//log.ItemName	= "SEED";
	_stprintf( log.szItemName, "%d", II_GOLD_SEED1 );
	log.Gold	= pUser->GetGold() + nCost;
	log.Gold2	= pUser->GetGold();
	log.Gold_1	= -nCost;
	OnLogItem( log );
}

void CDPSrvr::OnElectionSetPledge( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	char szPledge[CCandidate::nMaxPledgeLen]	= { 0,};
	ar.ReadString( szPledge, CCandidate::nMaxPledgeLen );
	if( strlen( szPledge ) == 0 )
		return;

	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int nCost;
		int nRet	= election::SetPledgeRequirements( pUser, nCost );
		if( nRet )
		{
			pUser->AddDefinedText( nRet );
			return;
		}
		pUser->AddGold( -nCost );
		g_dpDBClient.SendElectionSetPledge( pUser->m_idPlayer, szPledge );

		PutPenyaLog( pUser, "p", "PLEDGE", nCost );
	}
}

void CDPSrvr::OnElectionIncVote( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		u_long idPlayer;
		ar >> idPlayer;

		int nRet	= election::IncVoteRequirements( pUser, idPlayer );
		if( nRet )
		{
			pUser->AddDefinedText( nRet );
			return;
		}
		g_dpDBClient.SendElectionIncVote( idPlayer, pUser->m_idPlayer );
	}
}

void CDPSrvr::OnLEventCreate( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int iEEvent, iIEvent;
		ar >> iEEvent >> iIEvent;
		int nRet	= lordevent::CreateRequirements( pUser, iEEvent, iIEvent );
		if( nRet )
		{
			pUser->AddDefinedText( nRet );
			return;
		}
		ILordEvent* pEvent		= CSLord::Instance()->GetEvent();

		int nPerin	= pUser->RemoveTotalGold( pEvent->GetCost( iEEvent, iIEvent ) );
		char szContext[100]		= { 0,};
		sprintf( szContext, "OnLEventCreate: cost: %d(perin), %I64d(penya)", nPerin, pEvent->GetCost( iEEvent, iIEvent ) - ( nPerin * PERIN_VALUE ) );
		PutPenyaLog( pUser, "e", szContext, 0 );

		g_dpDBClient.SendLEventCreate( pUser->m_idPlayer, iEEvent, iIEvent );
	}
}

void CDPSrvr::OnLordSkillUse( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int nSkill;
		char szTarget[MAX_PLAYER]	= { 0,};
		ar >> nSkill;
		ar.ReadString( szTarget, MAX_PLAYER );
		u_long idTarget;
		int nRet	= lordskill::UseRequirements( pUser, szTarget, nSkill, idTarget );
		if( nRet )
		{
			pUser->AddDefinedText( nRet );
			return;
		}
		g_dpDBClient.SendLordSkillUse( pUser->m_idPlayer, idTarget, nSkill );
	}
}
#endif	// __LORD

#if __VER >= 12 // __TAX
void CDPSrvr::OnSetTaxRate( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	BYTE nCont;
	int nSalesTaxRate, nPurchaseTaxRate;
	ar >> nCont;
	ar >> nSalesTaxRate >> nPurchaseTaxRate;

	__TAXINFO* pTaxInfo = CTax::GetInstance()->GetTaxInfo( nCont );
	if( pTaxInfo && pUser->m_idGuild == pTaxInfo->dwNextId && pUser->IsGuildMaster() )
		CTax::GetInstance()->SetNextTaxRate( nCont, nSalesTaxRate, nPurchaseTaxRate );
	else
		Error( "\"%\"s User Is Not Next Win Guild Master!!!", pUser->GetName() );
}
#endif // __TAX

#if __VER >= 12 // __HEAVEN_TOWER
void CDPSrvr::OnTeleportToHeavenTower( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_HEAVEN_TOWER, pUser->GetWorld(), pUser->GetPos() ) )
		return;
    	
#ifdef __MOUNT
	if (pUser->m_pActMover->IsUseMount())
	{
		pUser->AddText("You can't teleport while using a mount.", COLOR_ERROR);
		return;
	}
#endif // __MOUNT
    	
	int nUnitID;
	ar >> nUnitID;

	int nCost = 0;
	DWORD dwWorldId = NULL_ID;
	float fAngle = 0.0f;
	D3DXVECTOR3 vPos;

	__AEGON_TELEPORT_UNIT* pUnit = CAegonTeleporterMng::GetInstance()->GetTeleUnitByID(nUnitID);
	if (!pUnit || pUnit->m_dwTeleporterWnd != 1)
		return;

	vPos = pUnit->m_telePos;
	dwWorldId = pUnit->m_dwWorldID;
	nCost = pUnit->m_nCost;
	fAngle = pUnit->m_fAngle;

	if( pUser->GetGold() < nCost )
	{
		pUser->AddDefinedText( TID_GAME_LACKMONEY );
		return;
	}

#if __VER >= 12 // __TAX
	BYTE nCont = CTax::GetInstance()->GetContinent( pUser );
#endif // __TAX	
	// 해당 층으로 텔레포트 -> 실패시 그냥 리턴...
	if( pUser->REPLACE( g_uIdofMulti, dwWorldId, vPos, REPLACE_NORMAL, nDefaultLayer ) )
	{
		pUser->AddGold( -nCost );
		pUser->SetAngle( fAngle );
#if __VER >= 12 // __TAX
		__TAXINFO* pTaxInfo = CTax::GetInstance()->GetTaxInfo( nCont );
		if( pTaxInfo && pTaxInfo->dwId != NULL_ID )
			CTax::GetInstance()->AddTax( nCont, nCost, TAX_ADMISSION );
#endif // __TAX		
		CString strFloor;
		strFloor.Format( "HEAVEN_TOWER_%2d", nUnitID);
		PutPenyaLog( pUser, "h", strFloor, nCost );
	}
	else
		return;
}
#endif // __HEAVEN_TOWER

BOOL CDPSrvr::DoUseItemTarget_GenRandomOption(
											  CUser* pUser, CItemElem* pTarget, int nKind,
											  int nOk, int nMismatchKind, int nHasOption,
											  const char* szOperation, const char* szReceive )
{
	int nRandomOptionKind	= g_xRandomOptionProperty->GetRandomOptionKind( pTarget );
	if( nRandomOptionKind != nKind || pTarget->IsFlag( CItemElem::expired ) )
	{
		pUser->AddDefinedText( nMismatchKind );
		return FALSE;
	}

	if( 
#if __VER >= 12 // __J12_0
		// 여신의 축복과 먹펫 각성은 각성 취소 없이 덮어 쓸 수 있게 한다
		nKind != CRandomOptionProperty::eBlessing && nKind != CRandomOptionProperty::eEatPet &&
#endif	// __J12_0
		g_xRandomOptionProperty->GetRandomOptionSize( pTarget->GetRandomOptItemId() ) > 0
	)
	{
		pUser->AddDefinedText( nHasOption );
		return FALSE;
	}

	if( nOk > 0 )
		pUser->AddDefinedText( nOk );

	// 활성화 된 픽업펫이면  기존 효과를 제거	// 康: 2008-09-29 추가
	if( pUser->IsUsing( pTarget ) && nKind == CRandomOptionProperty::eEatPet )
		pUser->ResetDestParamRandomOptExtension( pTarget );


	//	mulcom	BEGIN100405	각성 보호의 두루마리
	//g_xRandomOptionProperty->InitializeRandomOption( pTarget->GetRandomOptItemIdPtr() );
	//g_xRandomOptionProperty->GenRandomOption( pTarget->GetRandomOptItemIdPtr(), nRandomOptionKind, pTarget->GetProp()->dwParts );
	bool	bCheckedSafeFlag	= false;
	bCheckedSafeFlag	= g_xRandomOptionProperty->IsCheckedSafeFlag( pTarget->GetRandomOptItemId() );

#ifdef __AEGON_RETURN_AWAKE
	if (pTarget->GetRandomOptItemId())
	{
		g_xRandomOptionProperty->InitializeRandomOption(pTarget->GetOldRandomOptItemIdPtr());
		pTarget->SetOldRandomOptItemId(pTarget->GetRandomOptItemId());
		pUser->UpdateItemEx((BYTE)(pTarget->m_dwObjId), UI_OLDRANDOMOPTITEMID, pTarget->GetOldRandomOptItemId());
	}

#endif // __AEGON_RETURN_AWAKE

	if( bCheckedSafeFlag == true )
	{
		g_xRandomOptionProperty->ResetSafeFlag( pTarget->GetRandomOptItemIdPtr() );

		g_xRandomOptionProperty->InitializeRandomOption( pTarget->GetNewRandomOptionPtr() );
		g_xRandomOptionProperty->GenRandomOption( pTarget->GetNewRandomOptionPtr(), nRandomOptionKind, pTarget->GetProp()->dwParts, true );

		//	mulcom	BEGIN100426	각성 보호의 두루마리 로그.
// 		WriteLog( "UserID [%d] : ItemSerialNumber[%d] Protect GenNewRandomOption [%I64d]", (int)( pUser->GetId() ), (int)( pTarget->GetSerialNumber() ), pTarget->GetNewRandomOption() );
 
		TCHAR	szNewOption[128];
		::memset( szNewOption, 0, sizeof(szNewOption) );
		::_stprintf( szNewOption, "%I64d", pTarget->GetNewRandomOption() );
		g_DPSrvr.PutItemLog( pUser, "z", szNewOption, pTarget, 1 );
		//	mulcom	END100426	각성 보호의 두루마리 로그.

		pUser->SendNewRandomOption( (BYTE)( pTarget->m_dwObjId ), (DWORD)( pTarget->GetSerialNumber() ), pTarget->GetNewRandomOption() );

	}
	else
	{
		g_xRandomOptionProperty->InitializeRandomOption( pTarget->GetRandomOptItemIdPtr() );
		g_xRandomOptionProperty->GenRandomOption( pTarget->GetRandomOptItemIdPtr(), nRandomOptionKind, pTarget->GetProp()->dwParts );
	}
	//	mulcom	END100405	각성 보호의 두루마리

	pUser->UpdateItemEx( (BYTE)( pTarget->m_dwObjId ), UI_RANDOMOPTITEMID, pTarget->GetRandomOptItemId() );

#if __VER >= 12 // __PET_0519
	// 활성화 된 픽업펫 또는 리어펫 각성 직 후 효과 적용
	if( pUser->IsUsing( pTarget ) 
		&& ( nKind == CRandomOptionProperty::eSystemPet || nKind == CRandomOptionProperty::eEatPet ) )
		pUser->SetDestParamRandomOptExtension( pTarget );
#endif	// __PET_0519

	// log
	LogItemInfo	log;
	log.Action	= szOperation;
	log.SendName	= pUser->GetName();
	log.RecvName	= szReceive;
	log.WorldId		= pUser->GetWorld()->GetID();
	log.Gold	= pUser->GetGold();
	log.Gold2	= pUser->GetGold();
	OnLogItem( log, pTarget, 1 );
	return TRUE;
}

BOOL CDPSrvr::DoUseItemTarget_InitializeRandomOption(
											  CUser* pUser, CItemElem* pTarget, int nKind,
											  int nOk, int nError,
											  const char* szOperation, const char* szRecv )
{
	int nRandomOptionKind	= g_xRandomOptionProperty->GetRandomOptionKind( pTarget );
	if( nRandomOptionKind == nKind && g_xRandomOptionProperty->GetRandomOptionSize( pTarget->GetRandomOptItemId() ) > 0 )
	{
#if __VER >= 12 // __PET_0519
		// 먹펫 또는 시스템 펫 각성 취소 후 효과 제거
		if( pUser->IsUsing( pTarget ) 
			&& ( nKind == CRandomOptionProperty::eSystemPet || nKind == CRandomOptionProperty::eEatPet ) )
			pUser->ResetDestParamRandomOptExtension( pTarget );
#endif	// __PET_0519

		//	mulcom	BEGIN100405	각성 보호의 두루마리
		//g_xRandomOptionProperty->InitializeRandomOption( pTarget->GetRandomOptItemIdPtr() );
#ifdef __PROTECT_AWAKE
		if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AWAKESAFE ) == TRUE && nKind == CRandomOptionProperty::eAwakening )
		{
			nOk = TID_GAME_REGARDLESS_USE04;
			g_xRandomOptionProperty->SetSafeFlag( pTarget->GetRandomOptItemIdPtr() );

			pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_AWAKESAFE );

			//	mulcom	BEGIN100426	각성 보호의 두루마리 로그.
			g_DPSrvr.PutItemLog( pUser, "z", "USE_AWAKESAVE_ITEM", pTarget, 1 );
			//	mulcom	END100426	각성 보호의 두루마리 로그.

		}
		else
		{
#ifdef __AEGON_RETURN_AWAKE
			if (pTarget->GetRandomOptItemId())
			{
				g_xRandomOptionProperty->InitializeRandomOption(pTarget->GetOldRandomOptItemIdPtr());
				pTarget->SetOldRandomOptItemId(pTarget->GetRandomOptItemId());
				pUser->UpdateItemEx((BYTE)(pTarget->m_dwObjId), UI_OLDRANDOMOPTITEMID, pTarget->GetOldRandomOptItemId());
		}

#endif // __AEGON_RETURN_AWAKE
			g_xRandomOptionProperty->InitializeRandomOption( pTarget->GetRandomOptItemIdPtr() );
		}
#else	//__PROTECT_AWAKE
		g_xRandomOptionProperty->InitializeRandomOption( pTarget->GetRandomOptItemIdPtr() );
#endif	//__PROTECT_AWAKE
		//	mulcom	END100405	각성 보호의 두루마리

		pUser->UpdateItemEx((BYTE)(pTarget->m_dwObjId), UI_RANDOMOPTITEMID, pTarget->GetRandomOptItemId());
		pUser->AddDefinedText(TID_GAME_AWAKECANCEL_INFO);

		// log
		LogItemInfo	log;
		log.Action	= szOperation;
		log.SendName	= pUser->GetName();
		log.RecvName	= szRecv;
		log.WorldId		= pUser->GetWorld()->GetID();
		log.Gold	= pUser->GetGold();
		log.Gold2	= pUser->GetGold();
		OnLogItem( log, pTarget, 1 );
		return TRUE;
	}
	pUser->AddDefinedText( nError );	
	return FALSE;
}

BOOL CDPSrvr::DoUseItemTarget_ItemLevelDown( CUser* pUser, CItemElem* pMaterial, CItemElem* pTarget )
{
	ItemProp* pProp		= pTarget->GetProp();
	if( pProp->dwParts == NULL_ID || pProp->dwLimitLevel1 == 0xFFFFFFFF )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return FALSE;
	}

	int nLevelDown	= pTarget->GetLevelDown();
	if( nLevelDown == 0 || ( nLevelDown == -5 && pMaterial->m_dwItemId == II_SYS_SYS_SCR_LEVELDOWN02 ) )
	{
		pTarget->SetLevelDown( pMaterial->m_dwItemId == II_SYS_SYS_SCR_LEVELDOWN01? CItemElem::e5LevelDown: CItemElem::e10LevelDown );
		pUser->UpdateItemEx( (BYTE)( pTarget->m_dwObjId ), UI_RANDOMOPTITEMID, pTarget->GetRandomOptItemId() );
		// log
		LogItemInfo	log;
		log.Action	= "r";
		log.SendName	= pUser->GetName();
		log.RecvName	= "::ItemLevelDown";
		log.WorldId		= pUser->GetWorld()->GetID();
		log.Gold	= pUser->GetGold();
		log.Gold2	= pUser->GetGold();
		OnLogItem( log, pTarget, 1 );
		return TRUE;
	}
	else
		pUser->AddDefinedText( TID_GAME_ITEM_LEVELDOWN01 );

	return FALSE;
}

#if __VER >= 12 // __PET_0519
void CDPSrvr::OnTransformItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	CTransformStuff stuff;
	stuff.Serialize( ar );	// 재료를 수신

	ITransformer* pTransformer	= ITransformer::Transformer( stuff.GetTransform() );
#ifdef __SECURITY_FIXES
	if (pTransformer)
#endif //__SECURITY_FIXES
	pTransformer->Transform( pUser, stuff );	// 변환
}
#endif	// __PET_0519

#if __VER >= 12 // __MOD_TUTORIAL
void CDPSrvr::OnTutorialState( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	int nState;
	char szState[64]	= { 0,};
	char szOut[64]	= { 0,};
	ar >> nState;
	ar.ReadString( szState, 64 );
	MakeTutorialStateString( szOut, nState, pUser->GetName() );
	if( lstrcmp( szOut, szState ) == 0 )
	{
		pUser->SetTutorialState( nState );
		pUser->AddSetTutorialState();
	}
}
#endif	// __MOD_TUTORIAL

#if __VER >= 12 // __PET_0519
// 픽업펫 각성 취소 메뉴 선택 핸들러
void CDPSrvr::OnPickupPetAwakeningCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;
	if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_PET_AWAK_CANCEL, pUser->GetWorld(), pUser->GetPos() ) )
		return;

	DWORD dwItem;
	ar >> dwItem;
	CItemElem* pItem	= (CItemElem*)pUser->GetItemId( dwItem );
	if( !IsUsableItem( pItem ) )
		return;

	if( DoUseItemTarget_InitializeRandomOption( pUser, pItem, CRandomOptionProperty::eEatPet,
					TID_GAME_PICKUP_PET_AWAKENING_CANCEL_S001, TID_GAME_PICKUP_PET_AWAKENING_CANCEL_E001,
					"k", "PPAC" ) )
	{
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );
		if( pUser->IsMode( TRANSPARENT_MODE ) == 0 )
			g_UserMng.AddCreateSfxObj( pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
	}
}
#endif	// __PET_0519

#ifdef __AZRIA_1023
void CDPSrvr::OnDoUseItemInput( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	DWORD dwData;
	ar >> dwData;
	char szInput[MAX_INPUT_LEN]		= { 0,};
	ar.ReadString( szInput, MAX_INPUT_LEN );
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;
	pUser->SetInput( szInput );
	pUser->OnDoUseItem( dwData, 0, -1 );
	pUser->ResetInput();
}
#endif	// __AZRIA_1023

#ifdef __PET_1024
void CDPSrvr::OnClearPetName( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CPet* pPet	= pUser->GetPet();
		if( !pPet )
		{
			pUser->AddDefinedText( TID_GAME_NAME_PET_E00 );
			return;
		}
		pPet->SetName( "" );
		g_UserMng.AddSetPetName( pUser, pPet->GetName() );
	}
}
#endif	// __PET_1024

#if __VER >= 13 // __RAINBOW_RACE
void CDPSrvr::OnRainbowRacePrevRankingOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	pUser->AddRainbowRacePrevRankingOpenWnd();
}

void CDPSrvr::OnRainbowRaceApplicationOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	pUser->AddRainbowRaceApplicationOpenWnd();
}
void CDPSrvr::OnRainbowRaceApplication( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;
	
	CRainbowRaceMng::GetInstance()->SetApplicationUser( pUser );
}
void CDPSrvr::OnRainbowRaceMiniGamePacket( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;
	
	BOOL bExt;
	ar >> bExt;

	__MINIGAME_PACKET* pMiniGamePacket;
	if( bExt )	pMiniGamePacket = new __MINIGAME_EXT_PACKET;
	else		pMiniGamePacket = new __MINIGAME_PACKET;

	pMiniGamePacket->Serialize( ar );
	CRainbowRaceMng::GetInstance()->OnMiniGamePacket( pUser, pMiniGamePacket );
	SAFE_DELETE( pMiniGamePacket );
}

void CDPSrvr::OnRainbowRaceReqFinish( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	CRainbowRaceMng::GetInstance()->SetRanking( pUser );
}
#endif // __RAINBOW_RACE

#if __VER >= 13 // __HOUSING
void CDPSrvr::OnHousingSetupFurniture( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	HOUSINGINFO housingInfo;
	housingInfo.Serialize( ar );

	CHousingMng::GetInstance()->ReqSetupFurniture( pUser, housingInfo );
}

void CDPSrvr::OnHousingSetVisitAllow( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	DWORD dwPlayerId;
	BOOL  bAllow;

	ar >> dwPlayerId >> bAllow;

	CHousingMng::GetInstance()->ReqSetAllowVisit( pUser, dwPlayerId, bAllow );
}

void CDPSrvr::OnHousingVisitRoom( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	DWORD dwPlayerId;
	ar >> dwPlayerId;
#ifdef __SECURITY_FIXES
	if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_VISIT_MYROOM, pUser->GetWorld(), pUser->GetPos()))
		return;
#endif // __SECURITY_FIXES

	CHousingMng::GetInstance()->SetVisitRoom( pUser, dwPlayerId );
}

void CDPSrvr::OnHousingVisitableList( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	CHousingMng::GetInstance()->OnReqVisitableList( pUser );
}

void CDPSrvr::OnHousingGoOut( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	CHousingMng::GetInstance()->GoOut( pUser );
}
#endif // __HOUSING

#if __VER >= 13 // __QUEST_HELPER
void CDPSrvr::OnReqQuestNPCPos( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	char szCharKey[64] = {0,};
	ar.ReadString( szCharKey, 64 );

	LPCHARACTER lpChar = prj.GetCharacter( szCharKey );
	if( lpChar )
	{
		if( pUser->GetWorld() && pUser->GetWorld()->GetID() == lpChar->m_dwWorldId )
			pUser->AddNPCPos( lpChar->m_vPos );
		else
			pUser->AddDefinedText( TID_GAME_QUESTINFO_FAIL );
	}
}
#endif // __QUEST_HELPER

#if __VER >= 13 // __COUPLE_1117
void CDPSrvr::OnPropose( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		char szPlayer[MAX_PLAYER]	= { 0,};
		ar.ReadString( szPlayer, MAX_PLAYER );
		CCoupleHelper::Instance()->OnPropose( pUser, szPlayer );
	}
}

void CDPSrvr::OnRefuse( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
		CCoupleHelper::Instance()->OnRefuse( pUser );
}

void CDPSrvr::OnCouple( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
		CCoupleHelper::Instance()->OnCouple( pUser );
}

void CDPSrvr::OnDecouple( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
		CCoupleHelper::Instance()->OnDecouple( pUser );
}
#endif	// __COUPLE_1117

#ifdef __MAP_SECURITY
void CDPSrvr::OnMapKey( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		char szFileName[33] = {0,}, szMapKey[33] = {0,};
		ar.ReadString( szFileName, 33 );
		ar.ReadString( szMapKey, 33 );
		g_WorldMng.CheckMapKey( pUser, szFileName, szMapKey );
	}
}
#endif // __MAP_SECURITY

#if __VER >= 15 // __PETVIS
void CDPSrvr::OnRemoveVis( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	int nPos;
	ar >> nPos;

	CItemUpgrade::GetInstance()->RemovePetVisItem( pUser, nPos );
}

void CDPSrvr::OnSwapVis( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	int nPos1, nPos2;
	ar >> nPos1 >> nPos2;

	CItemUpgrade::GetInstance()->SwapVis( pUser, nPos1, nPos2 );
}
#endif // __PETVIS

#if __VER >= 15 // __GUILD_HOUSE
void CDPSrvr::OnBuyGuildHouse( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	GuildHouseMng->ReqBuyGuildHouse( pUser );
}

void CDPSrvr::OnGuildHousePacket( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	int nPacketType, nIndex;
	GH_Fntr_Info gfi;

	ar >> nPacketType >> nIndex;
	gfi.Serialize( ar );

	if( nPacketType == GUILDHOUSE_PCKTTYPE_LISTUP )
		return;

	GuildHouseMng->SendWorldToDatabase( pUser, nPacketType, gfi, nIndex );
}

void CDPSrvr::OnGuildHouseEnter( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	GuildHouseMng->EnteranceGuildHouse( pUser );
}

void CDPSrvr::OnGuildHouseGoOut( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	GuildHouseMng->GoOutGuildHouse( pUser );
}
#endif // __GUILD_HOUSE

#if __VER >= 15 // __TELEPORTER
void CDPSrvr::OnTeleporterReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		CHAR	m_szKey[64];
		int		nIndex;
		ar.ReadString(m_szKey, 64);
		ar >> nIndex;
		
		LPCHARACTER lpChar = prj.GetCharacter( m_szKey );
		if( lpChar )
		{
			if( !CNpcChecker::GetInstance()->IsCloseNpc( MMI_TELEPORTER, pUser->GetWorld(), pUser->GetPos() ) )
				return;

			if( (int)( lpChar->m_vecTeleportPos.size() ) <= nIndex )
				return;

#ifdef __MOUNT
			if (pUser->m_pActMover->IsUseMount())
			{
				pUser->AddText("You can't teleport while using a mount.", COLOR_ERROR);
				return;
			}
#endif // __MOUNT

			pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, lpChar->m_vecTeleportPos[nIndex], REPLACE_NORMAL, nDefaultLayer );
		}
	}
}
#endif // __TELEPORTER

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
void CDPSrvr::OnCheckedQuest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int nQuestid;
		BOOL bCheck;
		ar >> nQuestid >> bCheck;
		if( bCheck )
		{
			if( pUser->m_nCheckedQuestSize >= MAX_CHECKED_QUEST )
				return;

			for( int i = 0; i < pUser->m_nCheckedQuestSize; ++i )
			{
				if( pUser->m_aCheckedQuest[ i ] == nQuestid )
				{
					for( int j = i; j < pUser->m_nCheckedQuestSize -1; ++j )
						pUser->m_aCheckedQuest[ j ] = pUser->m_aCheckedQuest[ j+1 ];
					pUser->m_aCheckedQuest[ --pUser->m_nCheckedQuestSize ] = 0;
					break;
				}
			}
			pUser->m_aCheckedQuest[ pUser->m_nCheckedQuestSize++ ] = nQuestid;
		}
		else
		{
			if( pUser->m_nCheckedQuestSize <= 0 )
				return;

			for( int i = 0; i < pUser->m_nCheckedQuestSize; ++i )
			{
				if( pUser->m_aCheckedQuest[ i ] == nQuestid )
				{
					for( int j = i; j < pUser->m_nCheckedQuestSize -1; ++j )
						pUser->m_aCheckedQuest[ j ] = pUser->m_aCheckedQuest[ j+1 ];
					pUser->m_aCheckedQuest[ --pUser->m_nCheckedQuestSize ] = 0;
					break;
				}
			}
		}
		pUser->AddCheckedQuest();
	}
}
#endif // __IMPROVE_QUEST_INTERFACE

#if __VER >= 15 // __CAMPUS
void CDPSrvr::OnInviteCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pRequest = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pRequest ) )
	{
		u_long idTarget;
		ar >> idTarget;

		CUser* pTarget = g_UserMng.GetUserByPlayerID( idTarget );
		if( IsValidObj( pTarget ) )
			CCampusHelper::GetInstance()->OnInviteCampusMember( pRequest, pTarget );
	}
}

void CDPSrvr::OnAcceptCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pTarget = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pTarget ) )
	{
		u_long idRequest;
		ar >> idRequest;

		CUser* pRequest = g_UserMng.GetUserByPlayerID( idRequest );
		if( IsValidObj( pRequest ) )
			CCampusHelper::GetInstance()->OnAcceptCampusMember( pRequest, pTarget );
	}
}

void CDPSrvr::OnRefuseCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pTarget = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pTarget ) )
	{
		u_long idRequest;
		ar >> idRequest;

		CUser* pRequest = g_UserMng.GetUserByPlayerID( idRequest );
		if( IsValidObj( pRequest ) )
			pRequest->AddDefinedText( TID_GAME_TS_REFUSAL, "\"%s\"", pTarget->GetName() );
	}
}

void CDPSrvr::OnRemoveCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pRequest = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pRequest ) )
	{
		u_long idTarget;
		ar >> idTarget;

		if( idTarget > 0 )
			CCampusHelper::GetInstance()->OnRemoveCampusMember( pRequest, idTarget );
	}
}
#endif // __CAMPUS

//	mulcom	BEGIN100405	각성 보호의 두루마리
void	CDPSrvr::OnItemSelectAwakeningValue( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pRequest = g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pRequest ) == TRUE )
	{
		DWORD	dwItemObjID		= 0;
		DWORD	dwSerialNumber	= 0;
		BYTE	bySelectFlag	= 0;

		ar >> dwItemObjID;
		ar >> dwSerialNumber;
		ar >> bySelectFlag;

		pRequest->SelectAwakeningValue( dwItemObjID, dwSerialNumber, bySelectFlag );
	}
	else
	{
		WriteLog( "pUser is invalid in OnItemSelectAwakeningValue function." );
	}
}
//	mulcom	END100405	각성 보호의 두루마리

#ifdef __GUILD_HOUSE_MIDDLE
void CDPSrvr::OnGuildHouseTenderMainWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		DWORD dwGHType;
		OBJID objNpcId;
		ar >> dwGHType >> objNpcId;

		GuildHouseMng->ReqTenderGuildHouseList( pUser, dwGHType, objNpcId );
	}
}

void CDPSrvr::OnGuildHouseTenderInfoWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		OBJID objGHId;
		ar >> objGHId;

		GuildHouseMng->ReqTenderGuildHouseInfo( pUser, objGHId );
	}
}

void CDPSrvr::OnGuildHouseTenderJoin( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		OBJID objGHId;
		int nTenderPerin, nTenderPenya;
		ar >> objGHId >> nTenderPerin >> nTenderPenya;
		
		GuildHouseMng->OnGuildHouseTenderJoin( pUser, objGHId, nTenderPerin, nTenderPenya );
	}
}
#endif // __GUILD_HOUSE_MIDDLE

#ifdef __LOOKCHANGE
void CDPSrvr::OnLookChange(CAr &ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser *pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser))
		return;

#ifdef __WORLSERVER_MENU
	if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
		pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
		return;
	}
#endif // __WORLSERVER_MENU

	CItemElem *pDest, *pSrc;
	DWORD dwIdSrc, dwIdDest;
	ar >> dwIdSrc >> dwIdDest;

	pSrc = pUser->m_Inventory.GetAtId(dwIdSrc);
	pDest = pUser->m_Inventory.GetAtId(dwIdDest);

	if (!pSrc || !pDest)
		return;

	if (!IsUsableItem(pSrc) || !IsUsableItem(pDest))
	{
		pUser->AddDefinedText(TID_MMI_LOOKCHANGE06);
		return;
	}
	if (pSrc->IsChangedLook() || pDest->IsChangedLook())
	{
		pUser->AddDefinedText(TID_MMI_LOOKCHANGE01);
		return;
	}
	if (pUser->m_Inventory.IsEquip(pSrc->m_dwObjId) || pUser->m_Inventory.IsEquip(pDest->m_dwObjId))
	{
		pUser->AddDefinedText(TID_GAME_EQUIPPUT);
		return;
	}
	if (pSrc->m_dwItemId == pDest->m_dwItemId)
	{
		pUser->AddDefinedText(TID_MMI_LOOKCHANGE08);
		return;
	}

#ifdef __MIKSIK_EQUALIZED_MODE 
	if (pUser->bTmpMode)
	{
		pUser->AddText("You cant use that in temporal mode!");
		return;
	}
#endif //__MIKSIK_EQUALIZED_MODE

	ItemProp* iPropSrc = pSrc->GetProp();
	ItemProp* iPropDest = pDest->GetProp();
	if (!iPropSrc || !iPropDest)
	{
		pUser->AddDefinedText(TID_MMI_LOOKCHANGE07);
		return;
	}
	switch (pSrc->m_dwItemId)
	{
		case II_ARM_M_CHR_MAJESTY01HAT:
		case II_ARM_M_CHR_MAJESTY01SUIT:
		case II_ARM_M_CHR_MAJESTY01GLOVES:
		case II_ARM_M_CHR_MAJESTY01SHOES:
		case II_ARM_S_CLO_CLO_MAJESTY:
		case II_ARM_F_CHR_MAJESTY01HAT:
		case II_ARM_F_CHR_MAJESTY01SUIT:
		case II_ARM_F_CHR_MAJESTY01GLOVES:
		case II_ARM_F_CHR_MAJESTY01SHOES:
		{
			pUser->AddDefinedText(TID_MMI_LOOKCHANGE08);
			return;
		}
	}
	switch (pDest->m_dwItemId)
	{
		case II_ARM_M_CHR_MAJESTY01HAT:
		case II_ARM_M_CHR_MAJESTY01SUIT:
		case II_ARM_M_CHR_MAJESTY01GLOVES:
		case II_ARM_M_CHR_MAJESTY01SHOES:
		case II_ARM_S_CLO_CLO_MAJESTY:
		case II_ARM_F_CHR_MAJESTY01HAT:
		case II_ARM_F_CHR_MAJESTY01SUIT:
		case II_ARM_F_CHR_MAJESTY01GLOVES:
		case II_ARM_F_CHR_MAJESTY01SHOES:
		{
			pUser->AddDefinedText(TID_MMI_LOOKCHANGE08);
			return;
		}
	}
	if (((iPropSrc->dwItemKind3 == IK3_PET || iPropDest->dwItemKind3 == IK3_PET) && pUser->HasActivatedEatPet()) || (pUser->HasActivatedSystemPet() || (iPropSrc->dwItemKind3 == IK3_EGG || iPropDest->dwItemKind3 == IK3_EGG)))
	{
		pUser->AddText("Please deactivate your Egg / Pet / CS Pet before using the model changer!", COLOR_ERROR);
		return;
	}

	if ((iPropDest->dwItemSex == SEX_MALE || iPropDest->dwItemSex == SEX_FEMALE) && (iPropSrc->dwItemSex == SEX_MALE || iPropSrc->dwItemSex == SEX_FEMALE))
	{
		if (iPropDest->dwItemSex != iPropSrc->dwItemSex)
		{
			pUser->AddDefinedText(TID_MMI_LOOKCHANGE07);
			return;
		}
	}

	if (iPropDest->dwItemKind3 != IK3_SHIELD && iPropDest->dwItemKind3 != IK3_MAGICBARUNA && iPropDest->dwItemKind3 != IK3_ZEMBARUNA && 
		iPropSrc->dwItemKind3 != IK3_SHIELD && iPropSrc->dwItemKind3 != IK3_MAGICBARUNA && iPropSrc->dwItemKind3 != IK3_ZEMBARUNA)
	{
		if (iPropDest->dwItemKind3 != iPropSrc->dwItemKind3)
		{
			pUser->AddDefinedText(TID_MMI_LOOKCHANGE07);
			return;
		}
	}
	if ((iPropSrc->dwItemKind3 == IK3_SHIELD || iPropSrc->dwItemKind3 != IK3_MAGICBARUNA || iPropSrc->dwItemKind3 != IK3_ZEMBARUNA) && (iPropDest->dwItemKind3 != IK3_SHIELD || iPropDest->dwItemKind3 != IK3_MAGICBARUNA || iPropDest->dwItemKind3 != IK3_ZEMBARUNA))
	{
	}
	else
	{
		if ((iPropSrc->dwItemKind3 != IK3_SWD || iPropDest->dwItemKind3 != IK3_SWD)
			&& (iPropSrc->dwItemKind3 != IK3_AXE || iPropDest->dwItemKind3 != IK3_AXE)
			&& (iPropSrc->dwItemKind3 != IK3_KNUCKLEHAMMER || iPropDest->dwItemKind3 != IK3_KNUCKLEHAMMER)
			&& (iPropSrc->dwItemKind3 != IK3_CHEERSTICK || iPropDest->dwItemKind3 != IK3_CHEERSTICK)
			&& (iPropSrc->dwItemKind3 != IK3_WAND || iPropDest->dwItemKind3 != IK3_WAND)
			&& (iPropSrc->dwItemKind3 != IK3_STAFF || iPropDest->dwItemKind3 != IK3_STAFF)
			&& (iPropSrc->dwItemKind3 != IK3_BOW || iPropDest->dwItemKind3 != IK3_BOW)
#ifdef __CROSSBOW
			&& (iPropSrc->dwItemKind3 != IK3_CROSSBOW || iPropDest->dwItemKind3 != IK3_CROSSBOW)
#endif // __CROSSBOW
			&& (iPropSrc->dwItemKind3 != IK3_YOYO || iPropDest->dwItemKind3 != IK3_YOYO)
			&& (iPropSrc->dwItemKind3 != IK3_HELMET || iPropDest->dwItemKind3 != IK3_HELMET)
			&& (iPropSrc->dwItemKind3 != IK3_SUIT || iPropDest->dwItemKind3 != IK3_SUIT)
			&& (iPropSrc->dwItemKind3 != IK3_GAUNTLET || iPropDest->dwItemKind3 != IK3_GAUNTLET)
			&& (iPropSrc->dwItemKind3 != IK3_BOOTS || iPropDest->dwItemKind3 != IK3_BOOTS)
			&& (iPropSrc->dwItemKind3 != IK3_HAT || iPropDest->dwItemKind3 != IK3_HAT)
			&& (iPropSrc->dwItemKind3 != IK3_CLOTH || iPropDest->dwItemKind3 != IK3_CLOTH)
			&& (iPropSrc->dwItemKind3 != IK3_GLOVE || iPropDest->dwItemKind3 != IK3_GLOVE)
			&& (iPropSrc->dwItemKind3 != IK3_SHOES || iPropDest->dwItemKind3 != IK3_SHOES)
			&& (iPropSrc->dwItemKind3 != IK3_CLOAK || iPropDest->dwItemKind3 != IK3_CLOAK)
			&& (iPropSrc->dwItemKind3 != IK3_MASK || iPropDest->dwItemKind3 != IK3_MASK)
			&& (iPropSrc->dwItemKind3 != IK3_PET || iPropDest->dwItemKind3 != IK3_PET)
#ifdef __ANI_WING
			&& (iPropSrc->dwItemKind3 != IK3_ANI_WING || iPropDest->dwItemKind3 != IK3_ANI_WING)
#endif // __ANI_WING
			)
		{
			pUser->AddDefinedText(TID_MMI_ELLDINPOTION_TEXT08);
			return;
		}
	}

	pDest->SetLook(pSrc->m_dwItemId);
	pUser->RemoveItem((BYTE)pSrc->m_dwObjId, 1);
	pUser->AddLookChange(pDest->m_dwObjId, pDest->GetLook());
	pUser->AddText("Item transmutation has been successfully completed.", COLOR_SUCCESS);
}
#endif // __LOOKCHANGE

#ifdef __NEW_ITEM_VARUNA
void CDPSrvr::OnBaruna( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->m_vtInfo.GetOther() )
			return;
		if( pUser->m_vtInfo.VendorIsVendor() )
			return;

		CMover*	pBarunaPet = prj.GetMover( pUser->GetEatPetId( ) );
 		if( pBarunaPet && pBarunaPet->GetProp()->dwAI == AII_NONE )
		{
		}
		else
		{
			pUser->AddDefinedText( TID_MMI_NEWSMELT_OPER01 );
			return;
		}

		BYTE nState;
		ar >> nState;
		switch( nState )
		{
			case BARUNA_ENCHANT: 
			{
				DWORD dwItem01, dwItem02, dwItem03, dwItem04;
				ar >> dwItem01 >> dwItem02 >> dwItem03 >> dwItem04;
				CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwItem01 );
				CItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwItem02 );
				CItemElem* pItemElem2	= pUser->m_Inventory.GetAtId( dwItem03 );
				CItemElem* pItemElem3	= pUser->m_Inventory.GetAtId( dwItem04 );

				if( IsUsableItem( pItemElem0 ) == FALSE || IsUsableItem( pItemElem1 ) == FALSE )
					return;
				if( IsUsableItem( pItemElem3 ) == FALSE )
					pItemElem3 = NULL;

				CItemUpgrade::GetInstance()->EnchantBaruna( pUser, pItemElem0, pItemElem1, pItemElem2, pItemElem3 );
			}
			break;
			case BARUNA_CREATE_CRYSTAL:
			{
				DWORD dwItem01, dwItem02;
				ar >> dwItem01 >> dwItem02;
				CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwItem01 );
				CItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwItem02 );

				if( IsUsableItem( pItemElem0 ) == FALSE || IsUsableItem( pItemElem1 ) == FALSE )
					return;

				CItemUpgrade::GetInstance()->CreateBarunaCrystal( pUser, pItemElem0, pItemElem1 );
			}
			break;
			case BARUNA_EXTRACT_CHAOS_GEM:
			{
				DWORD dwItem01;
				ar >> dwItem01;
				CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwItem01 );

				if( IsUsableItem( pItemElem0 ) == FALSE )
					return;

				CItemUpgrade::GetInstance()->BarunaChaosGemExtract( pUser, pItemElem0 );
			}
			break;
			case BARUNA_CREATE_STONE:
			{
				DWORD dwItem01, dwItem02, dwItem03;
				ar >> dwItem01 >> dwItem02 >> dwItem03;
				CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwItem01 );
				CItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwItem02 );
				CItemElem* pItemElem2	= pUser->m_Inventory.GetAtId( dwItem03 );

				if( IsUsableItem( pItemElem0 ) == FALSE || IsUsableItem( pItemElem1 ) == FALSE || IsUsableItem( pItemElem2 ) == FALSE )
					return;

				CItemUpgrade::GetInstance()->CreateStone( pUser, pItemElem0, pItemElem1, pItemElem2 );
			}
			break;
			case BARUNA_CREATE_CHAOS_GEM:
			{
				DWORD dwItem01, dwItem02;
				ar >> dwItem01 >> dwItem02;
				CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwItem01 );
				CItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwItem02 );

				if( IsUsableItem( pItemElem0 ) == FALSE || IsUsableItem( pItemElem1 ) == FALSE )
					return;

				CItemUpgrade::GetInstance()->CreateBarunaChaosGem( pUser, pItemElem0, pItemElem1 );
			}
			break;
#ifdef __BARUNA_PIERCING
			case BARUNA_PIERCE:
			{
				DWORD dwItem01, dwItem02, dwItem03, dwItem04;
				ar >> dwItem01 >> dwItem02 >> dwItem03 >> dwItem04;
				CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwItem01 );
				CItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwItem02 );
				CItemElem* pItemElem2	= pUser->m_Inventory.GetAtId( dwItem03 );
				CItemElem* pItemElem3	= pUser->m_Inventory.GetAtId( dwItem04 );

				if( IsUsableItem( pItemElem0 ) == FALSE || IsUsableItem( pItemElem1 ) == FALSE )
					return;
				if( IsUsableItem( pItemElem2 ) == FALSE )
					pItemElem2 = NULL;
				if( IsUsableItem( pItemElem3 ) == FALSE )
					pItemElem3 = NULL;

				CItemUpgrade::GetInstance()->CreateBarunaPiercing( pUser, pItemElem0, pItemElem1, pItemElem2, pItemElem3 );

			}break;	
#endif // __BARUNA_PIERCING
		}
	}
}
#endif // __NEW_ITEM_VARUNA

#ifdef __FASHION_COMBINE
void CDPSrvr::OnFashionCombine( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		DWORD dwItem = NULL_ID;
		CItemElem* pItemElem[8];

		for( int i = 0; i < 8; i++ )
		{
			ar >> dwItem;
			pItemElem[i] = pUser->m_Inventory.GetAtId( dwItem );
			if( !IsUsableItem( pItemElem[i]))
				return;

#ifdef __GAMEGUARD
			//Check for valid ItemProp.
			ItemProp* pItemProp = pItemElem[i]->GetProp();
			if (!pItemProp)
				return;

			// Check if the object is equipped.
			if (pUser->m_Inventory.IsEquip(pItemElem[i]->m_dwObjId))
				return;

			// Check if it's a fash.
			if (pItemProp->dwItemKind2 != IK2_CLOTHETC && pItemProp->dwItemKind2 != IK2_CLOTH && pItemProp->dwItemKind2 != IK2_CLOTHWIG)
				return;

			// Check if it's a fash awakening.
			if (g_xRandomOptionProperty->GetRandomOptionKind(pItemElem[i]) != CRandomOptionProperty::eBlessing)
				return;

			//Check if inventory is full.
			if (pUser->m_Inventory.IsFull(pItemElem[i], pItemElem[i]->GetProp(), 1))
			{
				pUser->AddText(GETTEXT(TID_GAME_LACKSPACE));
				return;
			}
#endif //__GAMEGUARD
		}
		if( xRandom( 10000 ) < 10000 )
		{		
			int nResult = 0;
			nResult = g_xRandomOptionProperty->MergeRandomOption( pItemElem[0], pItemElem[1] );
			nResult += g_xRandomOptionProperty->MergeRandomOption( pItemElem[2], pItemElem[3] );
			nResult += g_xRandomOptionProperty->MergeRandomOption( pItemElem[4], pItemElem[5] );
			nResult += g_xRandomOptionProperty->MergeRandomOption( pItemElem[6], pItemElem[7] );
			if( nResult > 0 )
			{
				Error( "Packet editor detected. tried to combine more then 3 awakening on one item. User: %s", pUser->GetName() );
				return;
			}
			for( int i = 1; i < 8; i += 2 )
			{
				pUser->UpdateItem( (BYTE)pItemElem[i]->m_dwObjId, UI_NUM, 0 );
			}
			for( int i = 0; i < 8; i += 2 )
			{
				pUser->UpdateItemEx( (BYTE)( pItemElem[i]->m_dwObjId ), UI_RANDOMOPTITEMID, pItemElem[i]->GetRandomOptItemId() );
			}

			pUser->AddDefinedText( TID_COSTUME_COSTUMEMIXCOMPLETE );
		}
		else
		{
			pUser->AddPlaySound( SND_INF_UPGRADEFAIL );
			if( pUser->IsMode( TRANSPARENT_MODE ) == 0 )
				g_UserMng.AddCreateSfxObj( (CMover *)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );

			if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_COSTUMEDEFENDER ) )
			{
				pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_COSTUMEDEFENDER );
				pUser->AddDefinedText( TID_MMI_COSTUME_SAFEMIXITEM03 );

			}else
			{
				pUser->AddDefinedText( TID_MMI_COSTUME_COSTUMEMIXFAILED );
				for( int i = 0; i < 8; i++ )
				{
					pUser->UpdateItem( (BYTE)pItemElem[i]->m_dwObjId, UI_NUM, 0 );
				}
			}
		}
	}
}
#endif // __FASHION_COMBINE

#ifdef	__COLOSSEUM
void CDPSrvr::OnColosseum( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		BYTE nState;
		ar >> nState;
		CParty* pParty = g_PartyMng.GetParty( pUser->GetPartyId() );
		switch( nState )
		{
		case 1:
			BYTE byDifficult, byMenu;
			ar >> byDifficult >> byMenu;
			if( pParty && pParty->IsLeader( pUser->m_idPlayer ) )
			{
				if( CColosseumMng::GetInstance()->CreateNewMatch( pUser, byDifficult, byMenu ) )
				{
					CColosseumMatch* pColosseumMatch = CColosseumMng::GetInstance()->GetMatchByParty( pParty );
					if( pColosseumMatch )
						pColosseumMatch->EnterRoom( pUser );
					pUser->AddDefinedText(TID_COLOSSEUM_NOTENTERTIME01);
				}
				else
					pUser->AddDefinedText(TID_COLOSSEUM_NOTENTERTIME01);
			}
			else
				pUser->AddDefinedText(TID_COLOSSEUM_COLONOWPLAYING01);
		break;
		case 2:
			if( pParty )
			{
				CColosseumMatch* pColosseumMatch = CColosseumMng::GetInstance()->GetMatchByParty( pParty );
				if( pColosseumMatch )
					if( pColosseumMatch->m_byMenu == 2 )
					{
						if( pColosseumMatch->m_uGuildID == pUser->m_idGuild )
							pColosseumMatch->EnterRoom( pUser );
					}
					else
						pColosseumMatch->EnterRoom( pUser );
			}
		break;
		case 3:
			if( pParty && pParty->IsLeader( pUser->m_idPlayer ) )
			{
				CColosseumMatch* pColosseumMatch = CColosseumMng::GetInstance()->GetMatchByParty( pParty );
				if( pColosseumMatch )
					pColosseumMatch->EndWaitTime();
			}
		break;
		}
	}
}
#endif // __COLOSSEUM

#ifdef __ITEMLINK
void CDPSrvr::OnItemLink( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		BYTE nId;
		DWORD dwPlayerId,dwObjId;
		ar >> nId >> dwPlayerId >> dwObjId;

		if( nId != BYTE_SEND )
			return;

#ifdef __GAMEGUARD
		if (dwPlayerId == NULL || dwPlayerId == NULL_ID)
			return;
#endif //__GAMEGUARD

		CUser* pOther = g_UserMng.GetUserByPlayerID( dwPlayerId );
		if( !IsValidObj(pOther) )
		{
			pUser->AddDefinedText(TID_GAME_NOTLOGIN);
			return;
		}

#ifdef __GAMEGUARD
		if (dwObjId >= pOther->m_Inventory.m_dwItemMax)
			return;
#endif //__GAMEGUARD

		CItemElem* pItemElem = pOther->m_Inventory.GetAtId( dwObjId );
		if( pItemElem == NULL )
		{
			pUser->AddDefinedText(TID_GAME_ITEM_EXPIRED);
			return;
		}
		pUser->SendItemLink( pItemElem );
	}
}
#endif //__ITEMLINK

#if defined(__TREASUREBOX)
void CDPSrvr::OnTreasureBoxOpen( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if (IsValidObj(pUser))
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU

		DWORD dwBoxID = NULL, dwKeyID = NULL;
		ar >> dwBoxID >> dwKeyID;
		CItemElem* pBox = pUser->m_Inventory.GetAtId(dwBoxID);
		CItemElem* pKey = pUser->m_Inventory.GetAtId(dwKeyID);
		if (pBox && pKey)
		{
			int nFound = 0;

			if ((pBox->GetProp()->dwID == II_SYS_SYS_SCR_TREASUREBOXLV15) && (pKey->GetProp()->dwID == II_SYS_SYS_SCR_TREASUREKEYLOW15 || pKey->GetProp()->dwID == II_SYS_SYS_SCR_TREASUREKEYMIDDLE15 || pKey->GetProp()->dwID == II_SYS_SYS_SCR_TREASUREKEYHIGH15))
				nFound = 1;
			if ((pBox->GetProp()->dwID == II_SYS_SYS_SCR_TREASUREBOXLV60) && (pKey->GetProp()->dwID == II_SYS_SYS_SCR_TREASUREKEYLOW60 || pKey->GetProp()->dwID == II_SYS_SYS_SCR_TREASUREKEYMIDDLE60 || pKey->GetProp()->dwID == II_SYS_SYS_SCR_TREASUREKEYHIGH60))
				nFound = 1;
			if ((pBox->GetProp()->dwID == II_SYS_SYS_SCR_TREASUREBOXLV90) && (pKey->GetProp()->dwID == II_SYS_SYS_SCR_TREASUREKEYLOW90 || pKey->GetProp()->dwID == II_SYS_SYS_SCR_TREASUREKEYMIDDLE90 || pKey->GetProp()->dwID == II_SYS_SYS_SCR_TREASUREKEYHIGH90))
				nFound = 1;
			if ((pBox->GetProp()->dwID == II_SYS_SYS_SCR_TREASUREBOXLV120) && (pKey->GetProp()->dwID == II_SYS_SYS_SCR_TREASUREKEYLOW120 || pKey->GetProp()->dwID == II_SYS_SYS_SCR_TREASUREKEYMIDDLE120 || pKey->GetProp()->dwID == II_SYS_SYS_SCR_TREASUREKEYHIGH120))
				nFound = 1;
			if ((pBox->GetProp()->dwID == II_SYS_SYS_SCR_TREASUREBOXLV150) && (pKey->GetProp()->dwID == II_SYS_SYS_SCR_TREASUREKEYLOW150 || pKey->GetProp()->dwID == II_SYS_SYS_SCR_TREASUREKEYMIDDLE150 || pKey->GetProp()->dwID == II_SYS_SYS_SCR_TREASUREKEYHIGH150))
				nFound = 1;

			if (nFound == 0)
				return;

			if (pUser->m_Inventory.GetEmptyCount() < 3)
			{
				pUser->AddDefinedText(TID_GAME_LACKSPACE);
				return;
			}

			TreasureItem dwIDs[3];
			ZeroMemory(&dwIDs, sizeof(dwIDs));
			for (int i = 0;i < 3;i++)
			{
				int nChance = xRandom(0, 5000);
				int nOffset = xRandom(0, (int)prj.m_vTreasureItem.size());
				BOOL bFound = FALSE;
				for (int j = nOffset;j < (int)prj.m_vTreasureItem.size();j++)
				{
					if (pKey->GetProp()->dwID == prj.m_vTreasureItem.at(j).dwKeyID && nChance < prj.m_vTreasureItem.at(j).nChance)
					{
						int nContinue = 0;
						for (int k = 0;k < 3;k++)
						{
							if (dwIDs[k].dwItemID == prj.m_vTreasureItem.at(j).dwItemID)
								nContinue++;
						}
						if (nContinue == 0 || (nContinue > 0 && prj.m_vTreasureItem.at(j).nRepeat > nContinue))
						{
							dwIDs[i] = prj.m_vTreasureItem.at(j);
							bFound = TRUE;
							break;
						}
					}
				}
				if (bFound)
					continue;
				for (int j = 0;j < nOffset;j++)
				{
					if (pKey->GetProp()->dwID == prj.m_vTreasureItem.at(j).dwKeyID && nChance < prj.m_vTreasureItem.at(j).nChance)
					{
						int nContinue = 0;
						for (int k = 0;k < 3;k++)
						{
							if (dwIDs[k].dwItemID == prj.m_vTreasureItem.at(j).dwItemID)
								nContinue++;
						}
						if (nContinue == 0 || (nContinue > 0 && prj.m_vTreasureItem.at(j).nRepeat > nContinue))
						{
							dwIDs[i] = prj.m_vTreasureItem.at(j);
							break;
						}
					}
				}
			}

			int nCounter[3];
			ZeroMemory(&nCounter, sizeof(nCounter));
			for (int i = 0;i < 3;i++)
			{
				if (dwIDs[i].dwItemID == NULL)
					continue;
				int nCount = xRandom(dwIDs[i].nLimitamount, dwIDs[i].nMaxamount);
				nCounter[i] = nCount;

				CItemElem itemElem;
				itemElem.m_dwItemId = dwIDs[i].dwItemID;
				itemElem.m_nItemNum = nCount;
				itemElem.SetSerialNumber();
				itemElem.m_nHitPoint = 0;

				if (dwIDs[i].bNotice)
				{
					CString str;
					str.Format(prj.GetText(TID_MMI_TREASURE_NOTICETREASUREITEM), pUser->GetName(), itemElem.GetProp()->szName);
#ifdef __AEGON_ADVANCED_ANNOUNCEMENT
					g_DPCoreClient.SendSystem(str, 5, 0);	// Discovery - Default Shout Font
#else //__AEGON_ADVANCED_ANNOUNCEMENT
					g_DPCoreClient.SendSystem(str);
#endif //__AEGON_ADVANCED_ANNOUNCEMENT
				}

				pUser->CreateItem(&itemElem);
			}
			pUser->RemoveItem((BYTE)dwBoxID, 1);
			pUser->RemoveItem((BYTE)dwKeyID, 1);
			pUser->SendTreasure(dwIDs, nCounter);
		}
	}
}
#endif//__TREASUREBOX

#ifdef __SYS_SCR_CANCELLATION
BOOL CDPSrvr::UnbindScroll(CUser* pUser, CItemElem* pMaterial, CItemElem* pTarget)
{
	if (pTarget->GetProp() == NULL || !pTarget || !pMaterial || !IsValidObj(pUser))
		return FALSE;

#ifdef __BLOCK_STAFF_ACTIONS
	if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
	{
		pUser->AddText("[Titanium] Unauthorized Action. (Unbind)", COLOR_ERROR);
		return FALSE;
	}
#endif //__BLOCK_STAFF_ACTIONS

	if (pTarget->IsFlag(CItemElem::binds) && pMaterial->m_dwItemId == II_SYS_SYS_SCR_CANCELLATION && (pTarget->GetProp()->dwFlag & IP_FLAG_EQUIP_BIND))
	{
		pTarget->ResetFlag(CItemElem::binds);
		pUser->UpdateItem((BYTE)(pTarget->m_dwObjId), UI_FLAG, MAKELONG(pTarget->m_dwObjIndex, pTarget->m_byFlag));
		LogItemInfo    log;
		log.Action = "r";
		log.SendName = pUser->GetName();
		log.RecvName = "::binds";
		log.WorldId = pUser->GetWorld()->GetID();
		log.Gold = pUser->GetGold();
		log.Gold2 = pUser->GetGold();
		OnLogItem(log, pTarget, 1);
		return TRUE;
	}
	else
		pUser->AddDefinedText(TID_ITEM_CANCEL_ERROR1);
	return FALSE;
}
#endif //__SYS_SCR_CANCELLATION

#ifdef __SYS_LOCK_ITEM
void CDPSrvr::OnItemLock(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD objId;
	bool bIsLocked;
	ar >> objId;
	ar >> bIsLocked;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		
		CItemElem* pItemElem = pUser->m_Inventory.GetAtId(objId);
		if (pItemElem)
		{
			if (pUser->m_vtInfo.GetOther() != NULL) 
			{
				pUser->AddText("Cannot lock/unlock items while trading.");
				return;
			}
			pItemElem->SetItemLock(bIsLocked);
			pUser->AddItemLock(objId, bIsLocked);
		}
	}
}
#endif // __SYS_LOCK_ITEM

#ifdef __WIKIPEDIA_ITEMS
void CDPSrvr::OnWikiCreateItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsInvalidObj(pUser))
		return;

#ifdef __BLOCK_STAFF_ACTIONS
	if (pUser->m_dwAuthorization < AUTH_BALANCE) // Balancer and up only.
		return;
#else //__BLOCK_STAFF_ACTIONS
	if (pUser->m_dwAuthorization < AUTH_ADMINISTRATOR) // Admin Only.
		return;
#endif //__BLOCK_STAFF_ACTIONS

	CItemElem* pItemElem = new CItemElem;
	char szName[MAX_PLAYER];

	pItemElem->Serialize(ar);
	ar.ReadString(szName, MAX_PLAYER);

	if (!pItemElem)
		return;

	if (pItemElem->m_nItemNum <= 0 || pItemElem->m_nItemNum > 10000)
		return;

	if (pItemElem->GetProp()->dwPackMax < pItemElem->m_nItemNum)
		pItemElem->m_nItemNum = pItemElem->GetProp()->dwPackMax;

	u_long idPlayer = CPlayerDataCenter::GetInstance()->GetPlayerId(szName);

	if (pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
	{
		pItemElem->SetFlag(CItemElem::binds); // Character Bound, all auth's lower then admin.

		// Mail Item if inventory is full.
		if (!pUser->CreateItem(pItemElem))
			g_dpDBClient.SendQueryPostMail(pUser->m_idPlayer, 0, *pItemElem, 0, pItemElem->GetProp()->szName, "Staff Created Item");

		// Message
		CString strItem;
		strItem.Format("%d x %s created.", pItemElem->m_nItemNum, pItemElem->GetProp()->szName);
		pUser->AddText(strItem, COLOR_SUCCESS);

#ifndef __LOCALSERVER
		// Logging
		FILE* pFile = fopen(MakeFileNameDate("..\\Created_Items.txt"), "a");
		if (pFile == NULL)
		{
			Error("The item could not be created because the server could not create the file!");
			return;
		}

		CTime time = CTime::GetCurrentTime();
		char szString[1024];
		sprintf(szString, "[%02d/%02d/%d %02d:%02d:%02d] [%s] created %dx %s (Binds) - OnWikiCreateItem.\n", time.GetDay(), time.GetMonth(), time.GetYear(), time.GetHour(), time.GetMinute(), time.GetSecond(), pUser->GetName(), pItemElem->m_nItemNum, pItemElem->GetProp()->szName);
		fprintf(pFile, szString);
		fclose(pFile);
#endif //__LOCALSERVER
	}
	else // Player = Admin
	{
		if (idPlayer)
		{
			CUser* pReceiver = g_UserMng.GetUserByPlayerID(idPlayer);
			if (IsValidObj(pReceiver))
			{
				CString strSender, strReceiver;

				// Mail Item if inventory is full.
				if (!pReceiver->CreateItem(pItemElem))
					g_dpDBClient.SendQueryPostMail(pReceiver->m_idPlayer, 0, *pItemElem, 0, pItemElem->GetProp()->szName, "Staff Created Item");

				// Message
				strSender.Format("%d x %s created for player %s.", pItemElem->m_nItemNum, pItemElem->GetProp()->szName, pReceiver->GetName());
				strReceiver.Format("%d x %s received from [%s] %s", pItemElem->m_nItemNum, pItemElem->GetProp()->szName, pUser->GetAuthString().GetString(), pUser->GetName());

				pUser->AddText(strSender, COLOR_SUCCESS);
				pReceiver->AddText(strReceiver, COLOR_SUCCESS);

#ifndef __LOCALSERVER
				// Logging
				FILE* pFile = fopen(MakeFileNameDate("..\\Created_Items_Admins.txt"), "a");
				if (pFile == NULL)
				{
					Error("The item could not be created because the server could not create the file!");
					return;
				}

				CTime time = CTime::GetCurrentTime();
				char szString[1024];
				sprintf(szString, "[%02d/%02d/%d %02d:%02d:%02d] [%s] sent %dx %s to [%s] - OnWikiCreateItem.\n", time.GetDay(), time.GetMonth(), time.GetYear(), time.GetHour(), time.GetMinute(), time.GetSecond(), pUser->GetName(), pItemElem->m_nItemNum, pItemElem->GetProp()->szName, pReceiver->GetName());
				fprintf(pFile, szString);
				fclose(pFile);
#endif //__LOCALSERVER
			}
			else
			{
				pUser->AddText("Player is not logged in.", COLOR_ERROR);
			}
		}
		else
		{
			// Mail Item if inventory is full.
			if (!pUser->CreateItem(pItemElem))
				g_dpDBClient.SendQueryPostMail(pUser->m_idPlayer, 0, *pItemElem, 0, pItemElem->GetProp()->szName, "Staff Created Item");

			// Message
			CString strItem;
			strItem.Format("%d x %s created.", pItemElem->m_nItemNum, pItemElem->GetProp()->szName);
			pUser->AddText(strItem, COLOR_SUCCESS);
		}
	}

	SAFE_DELETE(pItemElem);
}
#endif //__WIKIPEDIA_ITEMS

#ifdef __AEGON_PETMISTAKE
void CDPSrvr::OnPetTamerMistakeCustom(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwId;
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		ar >> dwId;
		CItemElem* pItemElem = (CItemElem*)pUser->GetItemId(dwId);
		if (pItemElem && pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_TAMER_MISTAKE)
		{
			CPet* pPet = pUser->GetPet();
			if (pPet)
			{
				if (pPet->GetLevel() >= PL_C && pPet->GetLevel() <= PL_S)
				{
					pPet->SetAvailLevel(pPet->GetLevel(), 0);
					BYTE nAvailLevel = CPetProperty::GetInstance()->GetLevelupAvailLevel(pPet->GetLevel());
					pPet->SetAvailLevel(pPet->GetLevel(), nAvailLevel);

					if (pUser->HasPet())
						pUser->RemovePet();

					pUser->AddPet(pPet, PF_PET_GET_AVAIL);
					g_UserMng.AddPetLevelup(pUser, MAKELONG((WORD)pPet->GetIndex(), (WORD)pPet->GetLevel()));
					pUser->UpdateItem((BYTE)(pItemElem->m_dwObjId), UI_NUM, pItemElem->m_nItemNum - 1);

					CItemElem* pPetItem = pUser->GetPetItem();
					g_dpDBClient.CalluspPetLog(pUser->m_idPlayer, pPetItem->GetSerialNumber(), 0, PETLOGTYPE_MISTAKE, pPet);
				}
				else
				{
					pUser->AddDefinedText(TID_GAME_PET_BETWEEN_C_TO_S);
				}
			}
			else
			{
				pUser->AddDefinedText(TID_GAME_PET_NOT_FOUND);
			}
		}
		else
		{
			// error
		}
	}
}
#endif //__AEGON_PETMISTAKE

#ifdef __AEGON_MONSTER_HUNT
void CDPSrvr::OnLoadMonsterHuntProgress(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
		pUser->AddMHLoadProgress();
}
#endif // __AEGON_MONSTER_HUNT

#ifdef __LEEAEG_SHOP_FINDER
void CDPSrvr::OnShopItemTele(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD itemIndex;
	ar >> itemIndex;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	CWorld* pWorld;
	if (IsValidObj(pUser) && (pWorld = pUser->GetWorld()))
	{
#ifdef __TELEPORT_DELAY
		DWORD dwTickCount = GetTickCount();
		if (dwTickCount < pUser->m_dwTickLastTeleport + SEC(__TELEPORT_DELAY) && !pUser->IsAuthHigher(AUTH_GAMEMASTER))
		{
			pUser->AddText("You must wait at least 5 seconds between two teleports!", COLOR_ERROR);
			return;
		}
#endif //__TELEPORT_DELAY

#ifdef __TELEPORTAPP_EXCEPTIONS
		if (pUser->IsCollecting()) { pUser->AddText("You can't teleport while collecting.", COLOR_ERROR); return; }

		if (pUser->IsDie()) { pUser->AddText("You can't teleport when you died.", COLOR_ERROR); return; }

		if (pUser->IsFly()) { pUser->AddText("You can't teleport while flying.", COLOR_ERROR); return; }

		if (pUser->IsAttackMode()) { pUser->AddText("You cannot teleport if you are in combat.", COLOR_ERROR); return; }

#ifdef __MOUNT
		if (pUser->m_pActMover->IsUseMount()) { pUser->AddText("You can't teleport while using a mount.", COLOR_ERROR); return; }
#endif // __MOUNT

		if (pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR || pUser->GetWorld()->GetID() == WI_WORLD_ARENA)
		{
			pUser->AddText("You can't teleport at this location.", COLOR_ERROR);
			return;
		}

		if (pUser->m_vtInfo.VendorIsVendor() || pUser->m_vtInfo.IsVendorOpen())
		{
			pUser->AddText("You can't teleport while having a shop open or looking at someone elses shop.", COLOR_ERROR);
			return;
		}

#ifndef __LOOTPET_TELEPORT
		if (pUser->HasActivatedEatPet() || pUser->HasActivatedSystemPet() || pUser->HasPet())
		{
			if (pUser->HasActivatedSystemPet())
				pUser->PetRelease();
			if (pUser->HasActivatedEatPet())
				pUser->InactivateEatPet();
			if (pUser->HasPet())
				pUser->RemovePet();
		}
#endif //__LOOTPET_TELEPORT
#endif //__TELEPORTAPP_EXCEPTIONS

		CAegonShopFinderMng::GetInstance()->TeleportToItem(pUser, itemIndex);

#ifdef __TELEPORT_DELAY
		pUser->m_dwTickLastTeleport = GetTickCount();
#endif //__TELEPORT_DELAY

	}
}
void CDPSrvr::OnShopItemRefresh(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (pUser && IsValidObj(pUser))
	{
		pUser->AddShopFinderRefresh();
	}
}
#endif // __LEEAEG_SHOP_FINDER

#ifdef __GIFTBOX_LIST_ITEMS
void CDPSrvr::OnGiftboxQuery(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	DWORD dwID;
	ar >> dwID;

	auto map = CGiftboxMan::GetInstance()->GetBoxContent(dwID);
	if (map.size() != 0)
	{
		pUser->AddGiftboxPreview(map);
	}
}
#endif // __GIFTBOX_LIST_ITEMS

#ifdef __AEGON_RETURN_AWAKE
void CDPSrvr::OnReturnAwakening(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwItem, dwMaterial;
	ar >> dwItem >> dwMaterial;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		int	nCost = 0;
		DWORD nItemLevel = 0;

		CItemElem* pItem = pUser->m_Inventory.GetAtId(dwItem);
		CItemElem* pMaterial = pUser->m_Inventory.GetAtId(dwMaterial);

		nItemLevel = pItem->GetPropA()->dwItemLV;

		if (nItemLevel > 1 && nItemLevel <= 60)
		{
			nCost = 25000000; //25M
		}
		else if (nItemLevel > 60 && nItemLevel <= 90)
		{
			nCost = 50000000; //50M
		}
		else if (nItemLevel > 90 && nItemLevel <= 125)
		{
			nCost = 100000000; //1P
		}
		else if (nItemLevel > 125 && nItemLevel <= 155)
		{
			nCost = 500000000; //5P
		}
		else if (nItemLevel > 155 && nItemLevel <= 175)
		{
			nCost = 1000000000; //10P
		}
		else
		{
			//level 0-1
			if (pItem->GetPropA()->dwItemKind3 == IK3_PET)
			{
				//pick up pet
				nCost = 100000000; //1P
			}
			else
			{
				//fashion
				nCost = 100000000; //1P
			}

			//real level 1 weapons / parts
			if (pItem->GetPropA()->dwID == II_WEA_SWO_WOODEN || pItem->GetPropA()->dwID == II_WEA_SWO_LONG
				|| pItem->GetPropA()->dwID == II_ARM_M_VAG_SUIT01 || pItem->GetPropA()->dwID == II_ARM_F_VAG_SUIT01
				|| pItem->GetPropA()->dwID == II_ARM_M_VAG_GAUNTLET01 || pItem->GetPropA()->dwID == II_ARM_F_VAG_GAUNTLET01
				|| pItem->GetPropA()->dwID == II_ARM_M_VAG_BOOTS01 || pItem->GetPropA()->dwID == II_ARM_F_VAG_BOOTS01
				)
			{
				nCost = 25000000; //25M
			}

			if (nItemLevel == -1)
			{
				//raised pet
				nCost = 500000000; //5P
			}

		}

		if (pUser->GetGold() < nCost)
		{
			pUser->AddDefinedText(TID_GAME_LACKMONEY, "");
			return;
		}

		if (IsUsableItem(pItem))
		{
			if (IsUsableItem(pMaterial))
			{
				if (pMaterial->GetPropA()->dwID != II_SYS_SYS_SCR_AEGONREVERT)
				{
					LPCTSTR wrongMat = _T("Invalid Material!");
					pUser->AddText(wrongMat, COLOR_ERROR);
					return;
				}
			}
			else
			{
				LPCTSTR wrongMat = _T("Invalid Material!");
				pUser->AddText(wrongMat, COLOR_ERROR);
				return;
			}

			int nRandomOptionKind = g_xRandomOptionProperty->GetRandomOptionKind(pItem);


			if (nRandomOptionKind != CRandomOptionProperty::eAwakening					//eAwakening -> Sets / Weapons
#ifdef __AWAKENING_EXTENDED
				&& nRandomOptionKind != CRandomOptionProperty::eWeapon
#endif //__AWAKENING_EXTENDED
				//&& nRandomOptionKind != CRandomOptionProperty::eAwakeningExtension	//?
				//&& nRandomOptionKind != CRandomOptionProperty::eMaxRandomOptionKind	//?
				&& nRandomOptionKind != CRandomOptionProperty::eBlessing				//eBlessing -> Fashion
				&& nRandomOptionKind != CRandomOptionProperty::eEatPet					//eEatPet -> buffpet
				&& nRandomOptionKind != CRandomOptionProperty::eSystemPet)			    //eSystemPet -> normal pet 			
			{
				LPCTSTR wrongItem = _T("Invalid Item!");
				pUser->AddText(wrongItem, COLOR_ERROR);
				return;
			}

			//save old awake
			__int64 n64TmpOptions = pItem->GetOldRandomOptItemId();

			if (!n64TmpOptions)
			{
				LPCTSTR strNoAwake = _T("There is no previous awakening on this item!");
				pUser->AddText(strNoAwake, COLOR_ERROR);
				return;
			}

			//set old awake
			if (pItem->GetRandomOptItemId())
			{
				g_xRandomOptionProperty->InitializeRandomOption(pItem->GetOldRandomOptItemIdPtr());
				pItem->SetOldRandomOptItemId(pItem->GetRandomOptItemId());
				pUser->UpdateItemEx((BYTE)(pItem->m_dwObjId), UI_OLDRANDOMOPTITEMID, pItem->GetOldRandomOptItemId());
			}


			//set new awake = old tmp(old awake)
			g_xRandomOptionProperty->InitializeRandomOption(pItem->GetRandomOptItemIdPtr());
			pItem->SetRandomOptItemId(n64TmpOptions);
			pUser->UpdateItemEx((BYTE)(pItem->m_dwObjId), UI_RANDOMOPTITEMID, pItem->GetRandomOptItemId());

			//remove material 1x
			pUser->UpdateItem((BYTE)(pMaterial->m_dwObjId), UI_NUM, pMaterial->m_nItemNum - 1);
			pUser->AddGold(-nCost);
			pUser->AddText(_T("Awakening successfully reverted!"), COLOR_SUCCESS);
		}
	}
}
#endif // __AEGON_RETURN_AWAKE

#ifdef __VENDING_NPC
void CDPSrvr::OnCreateVendor(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwItemIdRec;
	D3DXVECTOR3 vPos;
	FLOAT fAngle;
	char szTitle[MAX_VENDORNAME];
	ar >> dwItemIdRec;
	ar >> vPos;
	ar >> fAngle;
	ar.ReadString(szTitle, MAX_VENDORNAME);

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE || pUser->GetWorld() == NULL)
		return;

	CWorld* pWorld = pUser->GetWorld();
	if (pWorld->GetID() != WI_WORLD_MADRIGAL)
	{
		pUser->m_vtInfo.VendorClose();
		return;
	}

#ifdef __AEGON_FIXES
	if (strlen(szTitle) <= 5 || strlen(szTitle) > 18)
	{
		pUser->AddText("Invalid Title length.", COLOR_WARNING);
		return;
	} 
#endif

#ifdef __BLOCK_STAFF_ACTIONS
	if (pUser->m_dwAuthorization > AUTH_GENERAL && pUser->m_dwAuthorization < AUTH_ADMINISTRATOR)
	{
		pUser->AddText("[Titanium] Unauthorized Action. (Vending)", COLOR_ERROR);
		return;
	}
#endif //__BLOCK_STAFF_ACTIONS

#ifdef __SHOPS_ONLY_CONT
	BYTE UserContinent = CContinent::GetInstance()->GetTown(pUser->GetPos());
	if (UserContinent) // Town Check
	{
		if (UserContinent != TOWN_DARKEN)
		{
			pUser->AddText("You're not allowed to create a private shop here.", COLOR_WARNING);
			pUser->AddText("Please press (V) and move to Darken City.", COLOR_WARNING);
			return;
		}
	}
	else // No Town Found
	{
		pUser->AddText("You're not allowed to create a private shop here.", COLOR_WARNING);
		pUser->AddText("Please press (V) and move to Darken City.", COLOR_WARNING);
		return;
	}
#endif //__SHOPS_ONLY_CONT

	if (CNpcChecker::GetInstance()->IsCloseNpc(pUser->GetWorld(), vPos))
	{
		pUser->m_vtInfo.VendorClose();
		pUser->AddDiagText(prj.GetText(TID_GAME_NPC_RADIUS));
		return;
	}

	if (g_GuildCombat1to1Mng.IsPossibleUser(pUser))
	{
		pUser->AddText(prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_TRADE));
		pUser->m_vtInfo.VendorClose();
		return;
	}

	if (pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_MINIROOM)
	{
		pUser->m_vtInfo.VendorClose();
		return;
	}

	if (pUser->m_vtInfo.GetOther())
	{
		pUser->m_vtInfo.VendorClose();
		return;
	}

	if (pUser->m_nDuel > 0)
	{
		pUser->m_vtInfo.VendorClose();
		return;
	}

	if (pUser->m_pActMover->IsFly())
	{
		pUser->m_vtInfo.VendorClose();
		return;
	}
	if (pUser->m_idVendorNPC != 0 /*&& !pUser->IsAuthHigher(AUTH_ADMINISTRATOR)*/)
	{
		CMover* pVendorNpc = prj.GetVendorNpcByID(pUser->m_idVendorNPC);
		if (IsValidObj(pVendorNpc))
		{
			pUser->AddText("Wait for your vendor to expire before opening another one.");
			pUser->m_vtInfo.VendorClose();
			return;
		}
	}
	DWORD dwId = HIWORD(dwItemIdRec);

	CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwId);
	if (IsUsableItem(pItemElem) && pItemElem->GetProp() && pItemElem->GetProp()->dwItemKind3 == IK3_VENDING)
	{
		CWorld* pWorld = pUser->GetWorld();
		MoverProp* pMoverProp = NULL;

		DWORD dwMonsterId = pItemElem->GetProp()->dwAbilityMin;
		FLOAT fMonsterSize = (FLOAT)pItemElem->GetProp()->fFlightSpeed;
		DWORD dwSkillTime = pItemElem->m_dwKeepTime ? pItemElem->m_dwKeepTime : pItemElem->GetProp()->dwSkillTime;

		//dwSkillTime = pItemElem->GetProp()->dwSkillTime;
		if (dwMonsterId == 0 || dwMonsterId == NULL_ID || dwSkillTime == 0 || dwSkillTime == NULL_ID)
		{
			Error("CDPSrvr::OnCreateVendor dwMonsterId=%d,fMonsterSize=%1.2f,dwSkillTime=%d", dwMonsterId, fMonsterSize, dwSkillTime);
			pUser->m_vtInfo.VendorClose();
			return;
		}

		if (fMonsterSize == 0 || fMonsterSize == NULL_ID)
			fMonsterSize = 1.0f;
		if (fMonsterSize < 0)
			fMonsterSize = -fMonsterSize;

		pMoverProp = prj.GetMoverProp(dwMonsterId);

		if (pWorld && pMoverProp && pMoverProp->dwID != 0)
		{
			char szName[MAX_NAME];
			//sprintf(szName, "[%s's Vendor] %s", pUser->GetName(), pMoverProp->szName);
			sprintf(szName, "[%s's Vendor]", pUser->GetName());

			CMover* pMover = (CMover*)CreateObj(D3DDEVICE, OT_MOVER, pMoverProp->dwID, FALSE);
			if (pMover != NULL)
			{
				pMover->AddItToGlobalId();
				pMover->m_dwVendorItemID = pItemElem->GetProp()->dwID;
				pMover->m_bNPCVendor = TRUE;
				pMover->InitProp(FALSE);

#ifdef __AEGON_IMPROVED_SHOPDESIGN
				map<int, __AEGON_SHOP_DESIGN>::iterator it = CAegonShopDesignMng::GetInstance()->m_mapShopDesigns.begin();
				for (; it != CAegonShopDesignMng::GetInstance()->m_mapShopDesigns.end(); it++)
				{
					if (pUser->HasBuff(BUFF_ITEM2, it->first)) {
						pMover->m_dwShopDesign = it->first;
						break;
					}
				}
#endif

				pMover->SetPos(vPos);
				pMover->InitMotion(MTI_STAND);
				pMover->UpdateLocalMatrix();
				pMover->SetAIInterface(AII_HUMAN);
				pMover->SetName(szName);
				pMover->SetAngle(fAngle);
				pMover->SetScale(D3DXVECTOR3(fMonsterSize, fMonsterSize, fMonsterSize));
				pMover->m_dwBelligerence = BELLI_PEACEFUL;
				pMover->m_bActiveAttack = 0;

				if (pWorld->AddObj(pMover, FALSE, pUser->GetLayer()))
				{
					pMover->m_dwVendorTime = pItemElem->m_dwKeepTime ? pItemElem->m_dwKeepTime + time(0) : time(0) + pItemElem->GetProp()->dwSkillTime * 60;
					pMover->m_vtInfo.SetTitle(szTitle);
					pMover->m_vtInfo.SetOwner(pUser);
					//pMover->m_vtInfo.SetOwnerId(pUser->GetId());
					pMover->m_vtInfo.VendorExchange();

					//pUser->m_idVendorNPC = pMover->GetId();
					g_dpDBClient.SendAddNPCVendor(pUser, pMover, vPos, fAngle, pMover->m_dwVendorTime, fMonsterSize, pItemElem->GetProp()->dwID, pMover->m_dwShopDesign);

					g_UserMng.AddPVendorOpen(pUser, pMover);
					pUser->AddNpcVendorLink(pMover->m_dwVendorTime, pMover->m_dwVendorItemID);

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
					CAegonAchievementMng::GetInstance()->AddAchievementProgressForType(pUser, AEGACH_NPCVENDOPEN, 1);
#endif // __AEGON_ACHIEVEMENT_SYSTEM
				}
			}

			pUser->AddDefinedText(TID_GAME_CREATEMON_S, "\"%s\"", szName);
			pUser->RemoveItem((BYTE)dwId, 1);

			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());

#ifdef __LEEAEG_SHOP_FINDER
			//add items to map
			if (pUser && pMover && pMover->GetWorld())
			{
				for (int i = 0; i < MAX_VENDITEM; i++)
				{
					CItemElem* tmpItem = (CItemElem*)pMover->m_vtInfo.GetItem(i);
					if (tmpItem)
						CAegonShopFinderMng::GetInstance()->AddItem(tmpItem, pUser->GetName(), pMover->GetPos(), pMover->GetWorld()->GetID(), CContinent::GetInstance()->GetContinentName(CContinent::GetInstance()->GetContinent(pMover)));
				}

			}
#endif // __LEEAEG_SHOP_FINDER

		}
	}
	pUser->m_vtInfo.VendorClose();
}

void CDPSrvr::OnUnlinkNpcVendor(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		CMover* pVendor = prj.GetVendorNpcByID(pUser->m_idVendorNPC);
		if (pVendor == NULL)
			return;

		ItemProp* pItemProp = prj.GetItemProp(pVendor->m_dwVendorItemID);
		if (pItemProp)
		{
			CItemElem itemVendor;
			itemVendor.m_dwItemId = pVendor->m_dwVendorItemID;
			itemVendor.m_nItemNum = 1;
			itemVendor.SetSerialNumber();
			if (time(0) > pVendor->m_dwVendorTime)
				Error("Vendor KeepTime %d %d", pVendor->m_dwVendorTime, time(0));
			itemVendor.m_dwKeepTime = pVendor->m_dwVendorTime - time(0);
			pUser->AddText("Your vendor is back!");

			if (pUser->CreateItem(&itemVendor) == FALSE)
				g_dpDBClient.SendQueryPostMail(pUser->m_idPlayer, 0, itemVendor, 0, pItemProp->szName, "Your vendor is back!");
		}

		prj.RemoveVendorNpcFromGlobal(pVendor->m_idVendorNPC);
		g_dpDBClient.SendRemoveNPCVendor(pVendor);
		pUser->m_idVendorNPC = 0;
		for (int i = 0; i < MAX_VENDITEM; i++)
		{
			CItemElem* pItemElem = (CItemElem*)pVendor->m_vtInfo.GetItem(i);

			if (pItemElem == NULL || !pItemElem->GetProp())
				continue;

			CItemElem itemElem;
			itemElem.m_dwItemId = pItemElem->m_dwItemId;
			itemElem.m_nItemNum = pItemElem->m_nItemNum;
			itemElem.SetAbilityOption(pItemElem->GetAbilityOption());
			itemElem.m_bCharged = pItemElem->m_bCharged;

			itemElem.m_dwKeepTime = pItemElem->m_dwKeepTime;

			itemElem.SetPiercingSize(pItemElem->GetPiercingSize());
			for (int i = 0; i < pItemElem->GetPiercingSize(); i++)
				itemElem.SetPiercingItem(i, pItemElem->GetPiercingItem(i));


			itemElem.SetPiercingSize(pItemElem->GetUltimatePiercingSize());
			for (int i = 0; i < pItemElem->GetUltimatePiercingSize(); i++)
				itemElem.SetUltimatePiercingItem(i, pItemElem->GetUltimatePiercingItem(i));

			itemElem.SetRandomOptItemId(pItemElem->GetRandomOptItemId());

			if (pItemElem->m_pPet)
			{
				itemElem.m_pPet = new CPet;
				itemElem.m_pPet->SetKind(pItemElem->m_pPet->GetKind());
				itemElem.m_pPet->SetLevel(pItemElem->m_pPet->GetLevel());
				itemElem.m_pPet->SetExp(pItemElem->m_pPet->GetExp());
				itemElem.m_pPet->SetEnergy(pItemElem->m_pPet->GetEnergy());
				itemElem.m_pPet->SetLife(pItemElem->m_pPet->GetLife());
				itemElem.m_pPet->SetAvailLevel(PL_D, pItemElem->m_pPet->GetAvailLevel(PL_D));
				itemElem.m_pPet->SetAvailLevel(PL_C, pItemElem->m_pPet->GetAvailLevel(PL_C));
				itemElem.m_pPet->SetAvailLevel(PL_B, pItemElem->m_pPet->GetAvailLevel(PL_B));
				itemElem.m_pPet->SetAvailLevel(PL_A, pItemElem->m_pPet->GetAvailLevel(PL_A));
				itemElem.m_pPet->SetAvailLevel(PL_S, pItemElem->m_pPet->GetAvailLevel(PL_S));
			}

#ifdef __LEEAEG_SHOP_FINDER
			//remove item from map
			CAegonShopFinderMng::GetInstance()->RemoveItem(pItemElem);
#endif // __LEEAEG_SHOP_FINDER

			if (pUser->CreateItem(&itemElem) == FALSE)
				g_dpDBClient.SendQueryPostMail(pUser->m_idPlayer, 0, *pItemElem, 0, pItemElem->GetProp()->szName, "");
		}
		pUser->AddText("Items that are not sold will be returned to your inventory / mailbox.");

		g_UserMng.AddCreateSfxObj(pVendor, XI_GEN_MONSTER_SPAWN01);
		pVendor->Delete();
	}
}
#endif // __VENDING_NPC

#ifdef __AEGON_CONVERT_PERIN
void CDPSrvr::OnAegonConvertPerin(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser)) {
		pUser->AutoConvertPerin();
	}
}
#endif // __AEGON_CONVERT_PERIN

#ifdef __NEW_CHANGE_JOB
void CDPSrvr::OnSendJobChange(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	int nJob;
	ar >> nJob;

	if (IsValidObj(pUser))
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU
		if (pUser->GetLevel() == MAX_JOB_LEVEL && pUser->IsBaseJob())
		{
			switch (nJob)
			{
			case JOB_MERCENARY: pUser->ChangeJob(JOB_MERCENARY); break;
			case JOB_ASSIST: pUser->ChangeJob(JOB_ASSIST); break;
			case JOB_ACROBAT: pUser->ChangeJob(JOB_ACROBAT); break;
			case JOB_MAGICIAN: pUser->ChangeJob(JOB_MAGICIAN); break;
			default: Error("[Warning] CDPSrvr::OnSendJobChange (1) - %s", pUser->GetName()); break;
			}
		}
		else if (pUser->GetLevel() == (MAX_JOB_LEVEL + MAX_EXP_LEVEL) && pUser->IsExpert())
		{
			switch (nJob)
			{
			case JOB_KNIGHT: if (pUser->GetJob() == JOB_MERCENARY) pUser->ChangeJob(JOB_KNIGHT); break;
			case JOB_BLADE: if (pUser->GetJob() == JOB_MERCENARY) pUser->ChangeJob(JOB_BLADE); break;
			case JOB_JESTER: if (pUser->GetJob() == JOB_ACROBAT) pUser->ChangeJob(JOB_JESTER); break;
			case JOB_RANGER: if (pUser->GetJob() == JOB_ACROBAT) pUser->ChangeJob(JOB_RANGER); break;
			case JOB_RINGMASTER: if (pUser->GetJob() == JOB_ASSIST) pUser->ChangeJob(JOB_RINGMASTER); break;
			case JOB_BILLPOSTER: if (pUser->GetJob() == JOB_ASSIST) pUser->ChangeJob(JOB_BILLPOSTER); break;
			case JOB_PSYCHIKEEPER: if (pUser->GetJob() == JOB_MAGICIAN) pUser->ChangeJob(JOB_PSYCHIKEEPER); break;
			case JOB_ELEMENTOR: if (pUser->GetJob() == JOB_MAGICIAN) pUser->ChangeJob(JOB_ELEMENTOR); break;
			default: Error("[Warning] CDPSrvr::OnSendJobChange (2) - %s", pUser->GetName()); break;
			}
		}
		else if (pUser->GetLevel() == MAX_GENERAL_LEVEL && pUser->IsPro() && pUser->GetExpPercent() >= 9999)
		{
			switch (nJob)
			{
			case JOB_KNIGHT_MASTER: if (pUser->GetJob() == JOB_KNIGHT) pUser->ChangeJob(JOB_KNIGHT_MASTER); break;
			case JOB_BLADE_MASTER: if (pUser->GetJob() == JOB_BLADE) pUser->ChangeJob(JOB_BLADE_MASTER); break;
			case JOB_JESTER_MASTER: if (pUser->GetJob() == JOB_JESTER) pUser->ChangeJob(JOB_JESTER_MASTER); break;
			case JOB_RANGER_MASTER: if (pUser->GetJob() == JOB_RANGER) pUser->ChangeJob(JOB_RANGER_MASTER); break;
			case JOB_RINGMASTER_MASTER: if (pUser->GetJob() == JOB_RINGMASTER) pUser->ChangeJob(JOB_RINGMASTER_MASTER); break;
			case JOB_BILLPOSTER_MASTER: if (pUser->GetJob() == JOB_BILLPOSTER) pUser->ChangeJob(JOB_BILLPOSTER_MASTER); break;
			case JOB_PSYCHIKEEPER_MASTER: if (pUser->GetJob() == JOB_PSYCHIKEEPER) pUser->ChangeJob(JOB_PSYCHIKEEPER_MASTER); break;
			case JOB_ELEMENTOR_MASTER: if (pUser->GetJob() == JOB_ELEMENTOR) pUser->ChangeJob(JOB_ELEMENTOR_MASTER); break;
			default: Error("[Warning] CDPSrvr::OnSendJobChange (3) - %s", pUser->GetName()); break;
			}
		}
		else if (pUser->GetLevel() == MAX_GENERAL_LEVEL && pUser->IsMaster() && pUser->GetExpPercent() >= 9999)
		{
			switch (nJob)
			{
			case JOB_KNIGHT_HERO: if (pUser->GetJob() == JOB_KNIGHT_MASTER) pUser->ChangeJob(JOB_KNIGHT_HERO); break;
			case JOB_BLADE_HERO: if (pUser->GetJob() == JOB_BLADE_MASTER) pUser->ChangeJob(JOB_BLADE_HERO); break;
			case JOB_JESTER_HERO: if (pUser->GetJob() == JOB_JESTER_MASTER) pUser->ChangeJob(JOB_JESTER_HERO); break;
			case JOB_RANGER_HERO: if (pUser->GetJob() == JOB_RANGER_MASTER) pUser->ChangeJob(JOB_RANGER_HERO); break;
			case JOB_RINGMASTER_HERO: if (pUser->GetJob() == JOB_RINGMASTER_MASTER) pUser->ChangeJob(JOB_RINGMASTER_HERO); break;
			case JOB_BILLPOSTER_HERO: if (pUser->GetJob() == JOB_BILLPOSTER_MASTER) pUser->ChangeJob(JOB_BILLPOSTER_HERO); break;
			case JOB_PSYCHIKEEPER_HERO: if (pUser->GetJob() == JOB_PSYCHIKEEPER_MASTER) pUser->ChangeJob(JOB_PSYCHIKEEPER_HERO); break;
			case JOB_ELEMENTOR_HERO: if (pUser->GetJob() == JOB_ELEMENTOR_MASTER) pUser->ChangeJob(JOB_ELEMENTOR_HERO); break;
			default: Error("[Warning] CDPSrvr::OnSendJobChange (4) - %s", pUser->GetName()); break;
			}
		}
#ifdef __3RD_LEGEND16
		else if (pUser->GetLevel() == MAX_LEGEND_LEVEL && pUser->IsHero())
		{
			switch (nJob)
			{
			case JOB_LORDTEMPLER_HERO: if (pUser->GetJob() == JOB_KNIGHT_HERO) pUser->ChangeJob(JOB_LORDTEMPLER_HERO); break;
			case JOB_STORMBLADE_HERO: if (pUser->GetJob() == JOB_BLADE_HERO) pUser->ChangeJob(JOB_STORMBLADE_HERO); break;
			case JOB_WINDLURKER_HERO: if (pUser->GetJob() == JOB_JESTER_HERO) pUser->ChangeJob(JOB_WINDLURKER_HERO); break;
			case JOB_CRACKSHOOTER_HERO: if (pUser->GetJob() == JOB_RANGER_HERO) pUser->ChangeJob(JOB_CRACKSHOOTER_HERO); break;
			case JOB_FLORIST_HERO: if (pUser->GetJob() == JOB_RINGMASTER_HERO) pUser->ChangeJob(JOB_FLORIST_HERO); break;
			case JOB_FORCEMASTER_HERO: if (pUser->GetJob() == JOB_BILLPOSTER_HERO) pUser->ChangeJob(JOB_FORCEMASTER_HERO); break;
			case JOB_MENTALIST_HERO: if (pUser->GetJob() == JOB_PSYCHIKEEPER_HERO) pUser->ChangeJob(JOB_MENTALIST_HERO); break;
			case JOB_ELEMENTORLORD_HERO: if (pUser->GetJob() == JOB_ELEMENTOR_HERO) pUser->ChangeJob(JOB_ELEMENTORLORD_HERO); break;
			default: Error("[Warning] CDPSrvr::OnSendJobChange (5) - %s", pUser->GetName()); break;
			}
		}
#endif // __3RD_LEGEND16
	}
}
#endif // __NEW_CHANGE_JOB

#ifdef __WEAPON_RARITY
void CDPSrvr::OnUpdateWeaponRarity(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		DWORD dwId1, dwId2; ar >> dwId1 >> dwId2;
		CItemElem* pItem1 = NULL, * pItem2 = NULL; pItem1 = pUser->m_Inventory.GetAtId(dwId1); pItem2 = pUser->m_Inventory.GetAtId(dwId2);

		if (!pItem1 || !pItem2 || pUser->IsUsing(pItem1) || pUser->IsUsing(pItem2)
			|| !IsValidItem(pItem1->GetProp()->dwItemKind3) || !IsValidItem(pItem2->GetProp()->dwItemKind3))
		{
			pUser->AddDefinedText(TID_WR_TEXT_004);
			return;
		}
		if (pItem1->GetProp()->dwID != pItem2->GetProp()->dwID)
		{
			pUser->AddDefinedText(TID_WR_TEXT_006);
			return;
		}
		if (pItem1->GetWeaponRarity() >= prj.m_nHighWeaponRarityLevel || pItem2->GetWeaponRarity() >= prj.m_nHighWeaponRarityLevel
			|| (pItem1->GetWeaponRarity() != pItem2->GetWeaponRarity()))
		{
			pUser->AddDefinedText(TID_WR_TEXT_007);
			return;
		}
		if (pUser->GetGold() < (int)prj.m_dwUpgradePrice)
		{
			pUser->AddDefinedText(TID_GAME_LACKMONEY);
			return;
		}
		pUser->AddGold(-(int)(prj.m_dwUpgradePrice));

		int nRandom = xRandom(100);
		WeaponRarity* nWeaponRarity = prj.m_nWeaponRarity.GetAt(pItem1->GetWeaponRarity());
		if (nWeaponRarity && nRandom >= 0 && nRandom < nWeaponRarity->m_fUpgradeRate)
		{
			pUser->AddDefinedText(TID_WR_TEXT_010);
			pItem1->SetWeaponRarity(pItem1->GetWeaponRarity() + 1);
			pUser->UpdateItem((BYTE)pItem2->m_dwObjId, UI_NUM, pItem2->m_nItemNum - 1);
			pUser->AddWeaponRarity(pItem1->m_dwObjId, pItem1->GetWeaponRarity() + 1);

			if (pUser->HasBuff(BUFF_ITEM, II_SYS_SCR_RPROTECT))
				pUser->RemoveBuff(BUFF_ITEM, II_SYS_SCR_RPROTECT);

			nWeaponRarity = prj.m_nWeaponRarity.GetAt(pItem1->GetWeaponRarity());
			if (nWeaponRarity->m_bReflect)
			{
				pItem1->SetFlag(CItemElem::reflect);
				pUser->UpdateItem((BYTE)pItem1->m_dwObjId, UI_FLAG, MAKELONG(pItem1->m_dwObjIndex, pItem1->m_byFlag));
			}

			pUser->AddSfxSnd(XI_INT_SUCCESS, SND_INF_UPGRADESUCCESS);
		}
		else
		{
			pUser->AddDefinedText(TID_WR_TEXT_009);
			if (!pUser->HasBuff(BUFF_ITEM, II_SYS_SCR_RPROTECT))
			{
				pUser->UpdateItem((BYTE)pItem1->m_dwObjId, UI_NUM, pItem1->m_nItemNum - 1);
				pUser->UpdateItem((BYTE)pItem2->m_dwObjId, UI_NUM, pItem2->m_nItemNum - 1);
			}
			else
				pUser->RemoveBuff(BUFF_ITEM, II_SYS_SCR_RPROTECT);

			pUser->AddSfxSnd(XI_INT_FAIL, SND_INF_UPGRADEFAIL);
		}
	}
}
#endif // __WEAPON_RARITY

#ifdef __HIDE_FASHION
void CDPSrvr::OnHideCoat(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		int nPosition;
		BOOL bHide;
		ar >> nPosition >> bHide;
		if (nPosition >= 5 || nPosition < 0)
			return;

		if ((bHide == 1) == pUser->m_abHideCoat[nPosition])
			return;

		pUser->m_abHideCoat[nPosition] = (bHide == 1);
		g_UserMng.AddHideCoat(pUser, nPosition, bHide);
	}
}
#endif // __HIDE_FASHION

#ifdef __SYS_CUSTOM_TITLE
void CDPSrvr::OnUpdateShowTitle(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		BOOL State;
		ar >> State;
		pUser->m_bShowTitle = State;
		g_UserMng.UpdateTitle(pUser);
	}
}
void CDPSrvr::OnCreateOwnTitle(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU

		char str[MAX_CUSTOM_TITLE];
		ar.ReadString(str, MAX_CUSTOM_TITLE);
		BOOL b = FALSE;
		if (str[0] == '$')
			ar >> b;

		CItemElem* pItemElem = NULL;
		if (!b)
		{
			int nLen = strlen(str);
			if (nLen < MIN_CUSTOM_TITLE
				|| nLen > MAX_CUSTOM_TITLE
				|| prj.IsInvalidName(str)
#ifdef __RULE_0615
				|| !prj.IsAllowedLetter(str)
#endif
				)
			{
				pUser->AddText(("Your must enter a valid title between 4 and 20 characters long."), COLOR_ERROR);
				return;
			}
			char szH[2], szA[10];
			strcpy(szH, "'");
			strcpy(szA, "\\");
			for (int i = 0; i < nLen; i++)
			{
				if (str[i] == szH[0] || str[i] == szA[0] || str[i] == '_' || str[i] == '-' || str[i] == ',' || str[i] == '.'
					|| str[i] == '+' || str[i] == '*' || str[i] == '!' || str[i] == '"' || str[i] == '?' || str[i] == '%'
					|| str[i] == '&' || str[i] == '/' || str[i] == '(' || str[i] == ')' || str[i] == '=' || str[i] == '?'
					|| str[i] == '`' || str[i] == '?' || str[i] == '<' || str[i] == '>' || str[i] == '|' || str[i] == '}'
					|| str[i] == ']' || str[i] == '[' || str[i] == '{' || str[i] == '~' || str[i] == '^' || str[i] == '$'
					|| str[i] == '?' || str[i] == ';' || str[i] == ':' || str[i] == '#' || str[i] == ' ')
				{
					pUser->AddText(("Your must enter a valid title between 4 and 20 characters long."), COLOR_ERROR);
					return;
				}
			}
			pItemElem = pUser->m_Inventory.GetAtItemId(II_SYS_SCR_CUSTOMTITLE);
		}
		else
		{
			if (lstrlen(pUser->m_szOwnTitle) > NULL && pUser->m_szOwnTitle[0] != '$')
				pItemElem = pUser->m_Inventory.GetAtItemId(II_SYS_SCR_REMOVE_CUSTOM);
			else
			{
				pUser->AddText(("You do not have a Custom Title."), COLOR_ERROR);
				return;
			}
		}

		if (pItemElem && IsUsableItem(pItemElem))
		{
			pUser->UpdateItem((BYTE)(pItemElem->m_dwObjId), UI_NUM, pItemElem->m_nItemNum - 1);
			lstrcpy(pUser->m_szOwnTitle, str);
			g_UserMng.UpdateTitle(pUser);
			pUser->AddText(("Your Custom Title has been updated!"), COLOR_SUCCESS);
		}
		else
			pUser->AddText(("Error with the Custom Title Scroll."), COLOR_ERROR);
	}
}
#endif // __SYS_CUSTOM_TITLE

#ifdef __PREMIUM
void CDPSrvr::OnUpdateShowPremium(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		BOOL State;
		ar >> State;
		pUser->m_bShowPremium = State;
		g_UserMng.UpdatePremium(pUser);
	}
}
#endif //__PREMIUM

#ifdef __MOUNT
void CDPSrvr::OnMountChange(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		DWORD dwID, dwMountId = 0;
		ar >> dwID;
		ar >> dwMountId;

		if (dwID == 0)
			pUser->SendActMsg(OBJMSG_MODE_FLY, dwMountId, 0, 1);
		else if (dwID == 1)
			pUser->m_pActMover->m_bLand = TRUE;
		else if (dwID == 2)
		{
			pUser->SendActMsg(OBJMSG_MODE_GROUND, 0, 0, 1);
			pUser->SendActMsg(OBJMSG_MOUNT_GROUND_STAND, dwMountId, 0, 1);
		}

		g_UserMng.UpdateMountState(pUser, dwID);
	}
}
#endif // __MOUNT

#ifdef __SYS_PARTYFINDER
void CDPSrvr::OnPartyList(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		vector<PARTYFINDER_LIST> vectmp;
		int nCount = 0;

		for (C2PartyPtr::iterator i = g_PartyMng.m_2PartyPtr.begin(); i != g_PartyMng.m_2PartyPtr.end(); i++)
		{
			CParty* pParty = (CParty*)i->second;

			if (!pParty)
				continue;

			if (pParty->GetLeader() != NULL)
				if (!pParty->GetLeader()->m_bPartyJoin)
					continue;


			if (pParty->GetSizeofMember() < MAX_PTMEMBER_SIZE_SPECIAL)
			{
				PARTYFINDER_LIST partytmp;

				partytmp.m_uPartyId = pParty->m_uPartyId;
				strcpy(partytmp.m_sParty, strlen(pParty->m_sParty) > 3 ? pParty->m_sParty : prj.GetText(TID_APP_PARTY));
				partytmp.m_nSizeofMember = pParty->m_nSizeofMember;
				partytmp.m_nLevel = pParty->m_nLevel;
				partytmp.m_nPoint = pParty->m_nPoint;

				PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pParty->m_aMember[0].m_uPlayerId);
				if (pPlayerData)
				{
					partytmp.m_nLeaderId = pParty->m_aMember[0].m_uPlayerId;
					sprintf(partytmp.m_sPartyList, "%s, ", pPlayerData->szPlayer);
					strcpy(partytmp.m_sLeader, pPlayerData->szPlayer);
				}
				else
				{
					sprintf(partytmp.m_sPartyList, "N/A, ");
					strcpy(partytmp.m_sLeader, "N/A");
				}

				for (int j = 1; j < pParty->GetSizeofMember(); j++)
				{
					PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pParty->m_aMember[j].m_uPlayerId);

					if (!pPlayerData)
						continue;

					strcat(partytmp.m_sPartyList, pPlayerData->szPlayer);
					strcat(partytmp.m_sPartyList, ", ");
				}
				partytmp.m_bLinkAttack = pParty->m_nModeTime[PARTY_LINKATTACK_MODE] ? true : false;
				partytmp.m_bFortuneCircle = pParty->m_nModeTime[PARTY_FORTUNECIRCLE_MODE] ? true : false;
				partytmp.m_bGiftBox = pParty->m_nModeTime[PARTY_GIFTBOX_MODE] ? true : false;
				CUser* pLeader = g_UserMng.GetUserByPlayerID(pParty->m_aMember[0].m_uPlayerId);
				if (IsValidObj(pLeader))
				{
					partytmp.m_bSkillFull = (pLeader->IsSMMode(SM_PARTYSKILL1) || pLeader->IsSMMode(SM_PARTYSKILL15) || pLeader->IsSMMode(SM_PARTYSKILL30)) ? true : false;
					partytmp.m_bExpUp = (pLeader->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP01) || pLeader->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP01) || pLeader->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP02)) ? true : false;
					partytmp.m_bSkillUp = (pLeader->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01) || pLeader->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP02) || pLeader->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01)) ? true : false;
				}
#ifdef __AEGON_FIXES
				if (!partytmp.m_bSkillFull) 
				{
					//Check other players for red scroll
					for (int j = 1; j < pParty->GetSizeofMember(); j++)
					{
						CUser* pMember = g_UserMng.GetUserByPlayerID(pParty->m_aMember[j].m_uPlayerId);
						if (pMember)
						{
							partytmp.m_bSkillFull = (pMember->IsSMMode(SM_PARTYSKILL1) || pMember->IsSMMode(SM_PARTYSKILL15) || pMember->IsSMMode(SM_PARTYSKILL30)) ? true : false;
							if (partytmp.m_bSkillFull)
								break;
						}
					}
				}
#endif
				vectmp.push_back(partytmp);
				nCount++;
			}
		}
		pUser->AddPartyList(vectmp, nCount);
		vectmp.clear();
	}
}

void CDPSrvr::OnPartyAllowJoin(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		int nMode;
		ar >> nMode;
		CParty* pParty = g_PartyMng.GetParty(pUser->GetPartyId());
		if (pParty)
		{
			if (!pParty->IsLeader(pUser->m_idPlayer))
				return;

			pUser->m_bPartyJoin = nMode;
			pUser->AddPartyChangeJoinMode(pUser->m_bPartyJoin);

			for (int i = 1; i < pParty->GetSizeofMember(); i++)
			{
				CUser* pMember = (CUser*)prj.GetUserByID(pParty->m_aMember[i].m_uPlayerId);
				if (IsValidObj(pMember))
					pMember->AddPartyChangeJoinMode(pUser->m_bPartyJoin);
			}
		}
	}
}
#endif // __SYS_PARTYFINDER

#ifdef __ANARCHY_SYSTEM
void CDPSrvr::OnAnarchyBuffs(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsInvalidObj(pUser))
		return;

#ifdef __WORLSERVER_MENU
	if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
		pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
		return;
	}
#endif // __WORLSERVER_MENU

	if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_ANARCHY, pUser->GetWorld(), pUser->GetPos())
#ifdef __PREMIUM
		&& !pUser->IsPremium()
		&& !pUser->IsAuthHigher(AUTH_GAMEMASTER3)
#endif // __PREMIUM
		) 
	{
		pUser->AddText("Premium required to activate Anarchy Buffs anywhere.", COLOR_ERROR);
		return;
	}

	int buff, nMode;
	ar >> buff >> nMode;

	if (buff < 0 || nMode == 0 && buff >(int)prj.m_vAnarchyBuffProp.size())
		return;

	// Global anarchy
	if (nMode == 0)
	{
		auto pBuff = &prj.m_vAnarchyBuffProp.at(buff);

		ItemProp* pBuffProp = prj.GetItemProp(pBuff->nId);

		if (!pBuffProp)
			return;

		// Check if player has shop
		if (pUser->m_vtInfo.GetOther() || pUser->m_vtInfo.VendorIsVendor())
		{
			pUser->AddText("You can't do that while vending!", COLOR_ERROR);
			return;
		}

		int nAnarchyCoin = pUser->GetItemNum(pBuff->currency);
		int nCost = pBuff->nPerin;

		if (nCost > nAnarchyCoin && !pUser->IsAuthorization(AUTH_ADMINISTRATOR))
		{
			pUser->AddText("You can't afford this anarchy buff!", COLOR_ERROR);
			return;
		}

		if (!pUser->IsAuthHigher(AUTH_ADMINISTRATOR) && pUser->HasBuff(BUFF_ITEM, pBuffProp->dwID))
		{
			return;
		}


		if (nAnarchyCoin && nCost && nAnarchyCoin >= nCost && !pUser->IsAuthorization(AUTH_ADMINISTRATOR))
		{
			CItemElem* pItemElem = pUser->m_Inventory.GetAtItemId(pBuff->currency);
			if (pItemElem)
				pUser->RemoveItem(pItemElem->m_dwObjId, nCost);
		}

		CString str;
		str.Format("%s activated %s", pUser->GetName(), pBuffProp->szName);
		g_DPCoreClient.SendSystem(str, 3);
		g_DPCoreClient.SendAnarchyBuff(pBuff->nId);

		// Create Anarchy Coupon
		CItemElem itemElem;
		itemElem.m_dwItemId = II_ANARCHY_COUPON;
		itemElem.m_nItemNum = 1;
		pUser->CreateItem(&itemElem);
		pUser->AddText("Thanks for activating an Anarchy buff. Received 1x Anarchy Coupon.", COLOR_SUCCESS);

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
		CAegonAchievementMng::GetInstance()->AddAchievementProgressForType(pUser, AEGACH_USEANARCHY, 1);
#endif // __AEGON_ACHIEVEMENT_SYSTEM
	}
}
#endif // __ANARCHY_SYSTEM

#ifdef __AEGON_TELEPORTER
void CDPSrvr::OnTeleportByAPP(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	int nUnitID;
	CWorld* pWorld;
	D3DXVECTOR3 vPos;
	if (IsValidObj(pUser) && (pWorld = pUser->GetWorld()))
	{

#ifdef __AEGON_FIXES
		if (pUser->IsMode(DONMOVE_MODE)) 
			return;
#endif

#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU

#ifdef __TELEPORT_DELAY
		DWORD dwTickCount = GetTickCount();
		if (dwTickCount < pUser->m_dwTickLastTeleport + SEC(__TELEPORT_DELAY) && !pUser->IsAuthHigher(AUTH_GAMEMASTER))
		{
			pUser->AddText("You must wait at least 5 seconds between two teleports!", COLOR_ERROR);
			return;
		}
#endif //__TELEPORT_DELAY

#ifdef __TELEPORTAPP_EXCEPTIONS
		if (pUser->IsCollecting()) { pUser->AddText("You can't teleport while collecting.", COLOR_ERROR); return; }

		if (pUser->IsDie()) { pUser->AddText("You can't teleport while being dead.", COLOR_ERROR); return; }

		if (pUser->IsFly()) { pUser->AddText("You can't teleport while flying.", COLOR_ERROR); return; }

		if (pUser->IsAttackMode()) { pUser->AddText("You cannot teleport if you are in combat.", COLOR_ERROR); return; }

#ifdef __MOUNT
		if (pUser->m_pActMover->IsUseMount()) { pUser->AddText("You can't teleport while using a mount.", COLOR_ERROR); return; }
#endif // __MOUNT

		if (pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR || pUser->GetWorld()->GetID() == WI_WORLD_ARENA)
		{
			pUser->AddText("You can't teleport at this location.", COLOR_ERROR);
			return;
		}

		if (pUser->m_vtInfo.VendorIsVendor() || pUser->m_vtInfo.IsVendorOpen())
		{
			pUser->AddText("You can't teleport while having a shop open or looking at someone elses shop.", COLOR_ERROR);
			return;
		}

#ifndef __LOOTPET_TELEPORT
		if (pUser->HasActivatedEatPet() || pUser->HasActivatedSystemPet() || pUser->HasPet())
		{
			if (pUser->HasActivatedSystemPet())
				pUser->PetRelease();
			if (pUser->HasActivatedEatPet())
				pUser->InactivateEatPet();
			if (pUser->HasPet())
				pUser->RemovePet();
		}
#endif //__LOOTPET_TELEPORT
#endif //__TELEPORTAPP_EXCEPTIONS

		ar >> nUnitID;

		__AEGON_TELEPORT_UNIT* pUnit = CAegonTeleporterMng::GetInstance()->GetTeleUnitByID(nUnitID);
		if (!pUnit)
			return;

		if (pUnit->m_nCost > 0 && pUser->GetGold() < pUnit->m_nCost)
		{
			CString strError;
			strError.Format("Not enough penya. Penya needed for teleport: %d", pUnit->m_nCost);
			pUser->AddText(strError.GetString(), COLOR_ERROR);
			return;
		}
		
		if (pUser->REPLACE_IMAGE_ARG(g_uIdofMulti, pUnit->m_dwWorldID, pUnit->m_telePos, REPLACE_NORMAL, nDefaultLayer))
		{
			if(pUnit->m_fAngle != 0.0f)
				pUser->SetAngle(pUnit->m_fAngle);

			if (pUnit->m_nCost > 0) 
			{
				pUser->AddGold(-1 * pUnit->m_nCost);

#if __VER >= 12 // __TAX
				BYTE nCont = CTax::GetInstance()->GetContinent(pUser);
				__TAXINFO* pTaxInfo = CTax::GetInstance()->GetTaxInfo(nCont);
				if (pTaxInfo && pTaxInfo->dwId != NULL_ID)
					CTax::GetInstance()->AddTax(nCont, pUnit->m_nCost, TAX_ADMISSION);
#endif // __TAX		
				CString strLog;
				strLog.Format("TELEPORT_APP_UNIT_%d", nUnitID);
				PutPenyaLog(pUser, "h", strLog, pUnit->m_nCost);
			}
		}

#ifdef __TELEPORT_DELAY
		pUser->m_dwTickLastTeleport = GetTickCount();
#endif //__TELEPORT_DELAY
	}
}
#endif

#ifdef __AEGON_LOOT_BOX
void CDPSrvr::OnLootboxStart(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	int nSelectedBox, nBuyMode;
	DWORD ticketObjID;
	ar >> nSelectedBox >> ticketObjID >> nBuyMode;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) && pUser->GetWorld() && nSelectedBox >= 0)
	{
		if(pUser->m_bLootBoxInProgress || pUser->m_bDPCommandInProgress)
		{
			return;
		}

		map<DWORD, __LOOTBOX_UNIT>::iterator it = CAegonLootBoxMng::GetInstance()->m_mapLootBoxes.find(nSelectedBox);

		if (it != CAegonLootBoxMng::GetInstance()->m_mapLootBoxes.end())
		{
			CItemElem* ticketItemElem = pUser->m_Inventory.GetAtId(ticketObjID);
			if (nBuyMode == LOOTBOX_BUY_TICKET) {

				if (ticketItemElem && ticketItemElem->m_nItemNum > 0 && it->second.IsValidTicket(ticketItemElem)) {
					__LOOTBOX_RESULT result;
					it->second.GetRandomResult(&result);
					if (result.validResult)
					{
						pUser->m_bLootBoxInProgress = TRUE;
						pUser->m_dwLootboxItem = result.receiveItem;
						pUser->m_dwLootboxItemNum = result.receiveNum;

						pUser->AddLootboxResult(result);
						pUser->UpdateItem((BYTE)(ticketItemElem->m_dwObjId), UI_NUM, ticketItemElem->m_nItemNum - 1);
					}
					
				}
			}
			else if (nBuyMode == LOOTBOX_BUY_DP) {
				if (it->second.dpPrice > 0) {
					__LOOTBOX_RESULT result;
					it->second.GetRandomResult(&result);
					if (result.validResult)
					{
						__DP_COMMAND_INFO info;
						info.Init();

						info.command = INGAMEDP_BUYLOOTBOX;
						info.dpCost = it->second.dpPrice;
						info.lootboxResult = result;

						g_dpDBClient.SendDPCommand(pUser, info);
					}
				}
				
			}
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
		}
	}
}
void CDPSrvr::OnLootboxCollect(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) && pUser->GetWorld() && pUser->m_bLootBoxInProgress && pUser->m_dwLootboxItem > 0 && pUser->m_dwLootboxItemNum > 0)
	{
		CItemElem collectItem;
		collectItem.m_dwItemId = pUser->m_dwLootboxItem;
		collectItem.m_nItemNum = pUser->m_dwLootboxItemNum;

		if (pUser->CreateItem(&collectItem))
		{
			pUser->m_bLootBoxInProgress = FALSE;
			pUser->m_dwLootboxItem = 0;
			pUser->m_dwLootboxItemNum = 0;
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());

			CString tmpOut;
			tmpOut.Format("You won %dx %s.", collectItem.m_nItemNum, collectItem.GetName().GetString());
			pUser->AddText(tmpOut.GetString(), COLOR_SUCCESS);
		}
		else
		{
			if(!pUser->m_Inventory.GetEmptyCount())
			{
				CString tmpOut;
				tmpOut.Format("Error creating lootbox reward. Inventory is full.");
				pUser->AddText(tmpOut.GetString(), COLOR_ERROR);
				return;
			}
			else
			{
				CString tmpOut;
				tmpOut.Format("Error creating lootbox reward.");
				pUser->AddText(tmpOut.GetString(), COLOR_ERROR);
				return;
			}
			
		}
	}
}
#endif // __AEGON_LOOT_BOX

#ifdef __AEGON_VIEW_INV
void CDPSrvr::OnInvView(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	u_long  uUserSenderId, uUserViewId;
	int nContainer = 0;
	ar >> uUserSenderId >> uUserViewId;
	ar >> nContainer;

	if (nContainer < 0 || nContainer > 2)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (pUser && IsValidObj(pUser) && pUser->m_idPlayer == uUserSenderId && pUser->IsAuthHigher(AUTH_GAMEMASTER2))
	{
		if (uUserSenderId == uUserViewId)
		{
			pUser->AddText("Press I to view your own Inventory, idiot.", COLOR_ERROR);
			return;
		}

		//CMover* viewMover = prj.GetUserByID(uUserViewId);
		CUser* viewUser = g_UserMng.GetUserByPlayerID(uUserViewId);
		if (viewUser && IsValidObj(viewUser))
		{
			switch (nContainer) 
			{
			case 0: //inventory
				pUser->AddViewInv(viewUser);
				break;
			case 1: //bag
				pUser->AddViewBag(viewUser);
				break;
			case 2: //bank
				pUser->AddViewBank(viewUser);
				break;
			}
		}
		else
		{
			pUser->AddText("Player not found", COLOR_ERROR);
		}
	}
	else
	{
		pUser->AddText("Error", COLOR_ERROR);
	}
}

void CDPSrvr::OnInvViewCommand(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BOOL success = FALSE;
	CString strOut;
	u_long uUserSenderId, uUserViewId;
	int nCommand, nContainer, nContainerField; //nContainer -> inv = 0, bag = 1 bank = 2 // nContainerField bag and bank 0/1/2
	CItemElem commandItem;
	ar >> uUserSenderId >> uUserViewId >> nCommand >> nContainer >> nContainerField;
	commandItem.Serialize(ar);

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	CUser* viewUser = g_UserMng.GetUserByPlayerID(uUserViewId);

	if (nContainer != 0 && nContainer != 1)
		return;

	if (commandItem.m_nItemNum > 0 && pUser && IsValidObj(pUser) && viewUser && IsValidObj(viewUser) && viewUser->m_idPlayer == uUserViewId && pUser->m_idPlayer == uUserSenderId && pUser->IsAuthHigher(AUTH_GAMEMASTER2))
	{
		switch (nCommand)
		{
		//Copy
		case 0:
		{
			CItemElem itemElem;
			itemElem = commandItem;
			itemElem.SetSerialNumber();

			if (&itemElem && pUser->CreateItem(&itemElem))
			{
				strOut.Format("item copied to inventory");
				success = TRUE;

				LogItemInfo aLogItem;
				aLogItem.Action = "A";
				aLogItem.SendName = "admin_copy";
				aLogItem.RecvName = pUser->GetName();
				aLogItem.WorldId = pUser->GetWorld() != NULL ? pUser->GetWorld()->GetID() : 0;
				aLogItem.Gold = pUser->GetGold();
				aLogItem.Gold2 = pUser->GetGold();
				OnLogItem(aLogItem, &itemElem, itemElem.m_nItemNum);
			}
			else
			{
				strOut.Format("copy failed");
				success = FALSE;
			}
		}
		break;
		//Delete
		case 1:
		{
			switch (nContainer)
			{
			case 0: //inventory
			case 1: //bag
			{
				CItemElem* pItem = viewUser->GetItemId2(nContainerField, commandItem.m_dwObjId, FALSE);

				if (pItem)
				{
					LogItemInfo aLogItem;
					aLogItem.Action = "A";
					aLogItem.SendName = viewUser->GetName();
					CString strRecv;
					strRecv.Format("admin_delete_%s", pUser->GetName());
					aLogItem.RecvName = strRecv.GetString();
					aLogItem.WorldId = pUser->GetWorld() != NULL ? pUser->GetWorld()->GetID() : 0;
					aLogItem.Gold = pUser->GetGold();
					aLogItem.Gold2 = pUser->GetGold();
					OnLogItem(aLogItem, pItem, pItem->m_nItemNum);


					if (pItem->IsPet() && viewUser->GetPetItem() == pItem)
					{
						viewUser->InactivateSystemPet(pItem);
					}
					else if (pItem->IsEatPet() && viewUser->HasActivatedEatPet() && viewUser->IsUsingEatPet(pItem))
					{
						viewUser->InactivateEatPet();
					}
					viewUser->RemoveItem2(pItem->m_dwObjId, pItem->m_nItemNum, nContainerField);

					pItem = viewUser->GetItemId2(nContainerField, commandItem.m_dwObjId, FALSE);

					if (pItem)
					{
						strOut.Format("delete failed");
						success = FALSE;

						LogItemInfo aLogItem;
						aLogItem.Action = "A";
						aLogItem.SendName = viewUser->GetName();
						CString strRecv;
						strRecv.Format("admin_delete_failed_%s", pUser->GetName());
						aLogItem.RecvName = strRecv.GetString();
						aLogItem.WorldId = pUser->GetWorld() != NULL ? pUser->GetWorld()->GetID() : 0;
						aLogItem.Gold = pUser->GetGold();
						aLogItem.Gold2 = pUser->GetGold();
						OnLogItem(aLogItem, pItem, pItem->m_nItemNum);
					}
					else
					{
						strOut.Format("item deleted");
						success = TRUE;

						if (viewUser)
						{
							if(nContainer == 0)
								pUser->AddViewInv(viewUser);
							else if(nContainer == 1)
								pUser->AddViewBag(viewUser);

						}
					}
				}
				else
				{
					strOut = "item not found refresh inventory/bag view";
					success = FALSE;
				}
			}
				break;
			case 2: //Bank
			{
				CItemElem* pItem = (CItemElem*)viewUser->GetItemBankId(nContainerField, commandItem.m_dwObjId);

				if (pItem)
				{
					viewUser->RemoveItemBankId(nContainerField, commandItem.m_dwObjId);

					pItem = (CItemElem*)viewUser->GetItemBankId(nContainerField, commandItem.m_dwObjId);

					if (pItem)
					{
						strOut.Format("delete failed");
						success = FALSE;
					}
					else
					{
						strOut.Format("item deleted");
						success = TRUE;

						if (viewUser)
							pUser->AddViewBank(viewUser);

					}
				}
				else
				{
					strOut = "item not found refresh bank view";
					success = FALSE;
				}
			}
				break;
			}
			
		}
		break;
		//Move
		case 2:
		{
			CItemElem itemElem;
			itemElem = commandItem;
			itemElem.SetSerialNumber();
			BOOL bCopySuccess = FALSE;

			if (&itemElem && pUser->CreateItem(&itemElem))
			{
				LogItemInfo aLogItem;
				aLogItem.Action = "A";
				aLogItem.SendName = "admin_move_copy";
				aLogItem.RecvName = pUser->GetName();
				aLogItem.WorldId = pUser->GetWorld() != NULL ? pUser->GetWorld()->GetID() : 0;
				aLogItem.Gold = pUser->GetGold();
				aLogItem.Gold2 = pUser->GetGold();
				OnLogItem(aLogItem, &itemElem, itemElem.m_nItemNum);

				strOut.Format("item copied to inventory");
				bCopySuccess = TRUE;
			}
			else
			{
				strOut.Format("copy failed");
				bCopySuccess = FALSE;
			}


			if (bCopySuccess) //copy success -> delete item
			{
				switch (nContainer) 
				{
				case 0: //inventory
				case 1: //bag
				{
					CItemElem* pItem = viewUser->GetItemId2(nContainerField, commandItem.m_dwObjId, FALSE);

					if (pItem)
					{
						if (pItem->IsPet() && viewUser->GetPetItem() == pItem)
						{
							viewUser->InactivateSystemPet(pItem);
						}
						else if (pItem->IsEatPet() && viewUser->HasActivatedEatPet() && viewUser->IsUsingEatPet(pItem))
						{
							viewUser->InactivateEatPet();
						}

						LogItemInfo aLogItem;
						aLogItem.Action = "A";
						CString strRecv;
						strRecv.Format("admin_move_delete_%s", pUser->GetName());
						aLogItem.SendName = viewUser->GetName();
						aLogItem.RecvName = strRecv.GetString();
						aLogItem.WorldId = pUser->GetWorld() != NULL ? pUser->GetWorld()->GetID() : 0;
						aLogItem.Gold = pUser->GetGold();
						aLogItem.Gold2 = pUser->GetGold();
						OnLogItem(aLogItem, pItem, pItem->m_nItemNum);

						viewUser->RemoveItem2(pItem->m_dwObjId, pItem->m_nItemNum, nContainerField);

						pItem = viewUser->GetItemId2(nContainerField, commandItem.m_dwObjId, FALSE);

						if (pItem)
						{
							strOut += " - delete failed";
							success = FALSE;

							LogItemInfo aLogItem;
							aLogItem.Action = "A";
							CString strRecv;
							aLogItem.SendName = viewUser->GetName();
							aLogItem.RecvName = "admin_move_delete_failed";
							aLogItem.WorldId = pUser->GetWorld() != NULL ? pUser->GetWorld()->GetID() : 0;
							aLogItem.Gold = pUser->GetGold();
							aLogItem.Gold2 = pUser->GetGold();
							OnLogItem(aLogItem, pItem, pItem->m_nItemNum);
						}
						else
						{
							strOut += " - item deleted";
							success = TRUE;

							if (viewUser)
							{
								if (nContainer == 0)
									pUser->AddViewInv(viewUser);
								else if (nContainer == 1)
									pUser->AddViewBag(viewUser);

							}
						}
					}
					else
					{
						strOut = "item not found refresh inventory/bag view";
						success = FALSE;
					}
				}
					break;
				case 2: //bank
				{
					CItemElem* pItem = (CItemElem*)viewUser->GetItemBankId(nContainerField, commandItem.m_dwObjId);

					if (pItem)
					{
						viewUser->RemoveItemBankId(nContainerField, commandItem.m_dwObjId);

						pItem = (CItemElem*)viewUser->GetItemBankId(nContainerField, commandItem.m_dwObjId);

						if (pItem)
						{
							strOut += " - delete failed";
							success = FALSE;
						}
						else
						{
							strOut += " - item deleted";
							success = TRUE;

							if (viewUser)
								pUser->AddViewBank(viewUser);
						}
					}
					else
					{
						strOut = "item not found refresh bank view";
						success = FALSE;
					}
				}
					break;
				}
			}



		}
		break;
		//give item
		case 3:
		{

			switch (nContainer)
			{
			case 0: //inventory
			case 1: //bag
			{
				CItemElem itemElem;
				itemElem = commandItem;
				itemElem.SetSerialNumber();

				if (&itemElem && viewUser->CreateItem2(&itemElem, nContainerField))
				{
					strOut.Format("item given");
					success = TRUE;

					LogItemInfo aLogItem;
					aLogItem.Action = "A";
					CString strSend;
					strSend.Format("admin_give_%s", pUser->GetName());
					aLogItem.SendName = strSend.GetString();
					aLogItem.RecvName = viewUser->GetName();
					aLogItem.WorldId = pUser->GetWorld() != NULL ? pUser->GetWorld()->GetID() : 0;
					aLogItem.Gold = pUser->GetGold();
					aLogItem.Gold2 = pUser->GetGold();
					OnLogItem(aLogItem, &itemElem, itemElem.m_nItemNum);

					if (viewUser) 
					{
						if (nContainer == 0) 
							pUser->AddViewInv(viewUser);
						else if(nContainer == 1)
							pUser->AddViewBag(viewUser);
					}
				}
				else
				{
					strOut.Format("giving item failed");
					success = FALSE;
				}
			}
			break;
			case 2: //bank
			{
				strOut.Format("cant put items in player banks");
				success = FALSE;
			}
			break;
			}
			//delete item if giving was successfull
			if (success)
			{
				BOOL bBank = FALSE;
				CItemElem* pGiveItem = NULL;
				int nIndex = -1;
				for (int i = -1; i < 3; i++)
				{
					pGiveItem = pUser->GetItemId2(i, commandItem.m_dwObjId, FALSE);
					if (pGiveItem)
					{
						nIndex = i;
						break;
					}
				}

				if (!pGiveItem)
				{
					for (int i = 0; i < 3; i++)
					{
						pGiveItem = (CItemElem*)pUser->GetItemBankId(i, commandItem.m_dwObjId);
						if (pGiveItem)
						{
							nIndex = i;
							bBank = TRUE;
							break;
						}
					}
				}

				if (pGiveItem)
				{

					LogItemInfo aLogItem;
					aLogItem.Action = "A";
					aLogItem.SendName = pUser->GetName();
					aLogItem.RecvName = "delete_admin_give_item";
					aLogItem.WorldId = pUser->GetWorld() != NULL ? pUser->GetWorld()->GetID() : 0;
					aLogItem.Gold = pUser->GetGold();
					aLogItem.Gold2 = pUser->GetGold();
					OnLogItem(aLogItem, pGiveItem, pGiveItem->m_nItemNum);

					if (!bBank)
						pUser->RemoveItem2(commandItem.m_dwObjId, commandItem.m_nItemNum, nIndex);
					else
						pUser->RemoveItemBankId(nIndex, commandItem.m_dwObjId);

					if (!bBank)
						pGiveItem = pUser->GetItemId2(nIndex, commandItem.m_dwObjId, FALSE);
					else
						pGiveItem = (CItemElem*)pUser->GetItemBankId(nIndex, commandItem.m_dwObjId);

					if (pGiveItem)
					{
						strOut += " - delete failed";
						success = FALSE;
					}
					else
					{
						strOut += " - item deleted";
						success = TRUE;
					}

				}
				else
				{
					strOut = "item not found refresh inventory/bag/bank";
					success = FALSE;
				}
			}
		}
		break;
		}

		if (pUser && pUser->GetWorld())
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());

		if (viewUser && viewUser->GetWorld())
			g_dpDBClient.SavePlayer(viewUser, viewUser->GetWorld()->GetID(), viewUser->GetPos(), viewUser->GetLayer());

		if (success)
			pUser->AddText(strOut, COLOR_SUCCESS);
		else
			pUser->AddText(strOut, COLOR_ERROR);
	}
}
#endif // __AEGON_VIEW_INV

#ifdef __SYS_PETFILTER
void CDPSrvr::OnSetPetfilter(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) && GetTickCount() > pUser->m_dwPetfilterLastSave + 1000)
	{
		LONGLONG dwPetFilter;
		ar >> dwPetFilter;
		pUser->m_dwPetfilter = dwPetFilter;
		pUser->m_sPetfilterLists.Read(ar);
		pUser->AddText("Petfilter saved successfully.", COLOR_SUCCESS);
		pUser->m_dwPetfilterLastSave = GetTickCount();
	}
}
#endif //__SYS_PETFILTER

#ifdef __BADGE
#include "Badge.h"
void CDPSrvr::OnChangeBadgeIcon(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	int nChange;
	ar >> nChange;
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (nChange > static_cast<int>(CBadge::GetInstance()->GetBadgesMap().size()) || nChange < -1)
			return;

		if (CBadge::GetInstance()->IsOwned(static_cast<CMover*>(pUser), nChange))
		{
			pUser->SetBadge(nChange);
			g_UserMng.AddBadgeChange(pUser, nChange);
		}
	}
}
#endif //__BADGE

#ifdef __SYS_GUILDCOLOR
void CDPSrvr::OnSetGuildColor(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	int r, g, b;
	ar >> r >> g >> b;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!pUser) return;

	CItemElem* pitem = pUser->m_Inventory.GetAtItemId(II_SYS_SYS_SCR_GUILDCOLOR);
	if (!pitem || pitem->m_nItemNum < 1)
	{
		pUser->AddText("You can't do this without a Scroll of Guild Name Color.", COLOR_ERROR);
		Error("CDPSrvr::OnSetGuildColor - User %s (ID: %d) sent guild color change without scroll - Hacking attempt", pUser->GetName(), pUser->m_idPlayer);
		return;
	}

	CGuild* Guild = pUser->GetGuild();
	if (!Guild) return;

	DWORD guildid = Guild->GetGuildId();

	if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255)
		return;

	g_GuildMng.SetColor(Guild, r, g, b);
	g_DPCoreClient.SendSetGuildColor(guildid, r, g, b);
	pUser->UpdateItem((BYTE)pitem->m_dwObjId, UI_NUM, (pitem->m_nItemNum - 1));
	pUser->AddText("The guild name color was changed successfully!", COLOR_SUCCESS);
}
#endif //__SYS_GUILDCOLOR

#ifdef __SYS_NAMECOLOR
void CDPSrvr::OnSetNameColor(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE plBuf, u_long uBufSize)
{
	int r, g, b;
	ar >> r >> g >> b;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!pUser) return;

	CItemElem* pItem = pUser->m_Inventory.GetAtItemId(II_SYS_SYS_SCR_NAMECOLOR);
	if (!pItem || pItem->m_nItemNum < 1)
	{
		pUser->AddText("You can't do this without a Scroll of Name Color Change.", COLOR_ERROR);
		Error("CDPSrvr::OnSetNameColor - User %s (ID: %d) sent name color change without scroll - Hacking attempt", pUser->GetName(), pUser->m_idPlayer);
		return;
	}

	((CMover*)pUser)->m_ncR = r;
	((CMover*)pUser)->m_ncG = g;
	((CMover*)pUser)->m_ncB = b;
	g_UserMng.AddSetNameColor(pUser, r, g, b);
	g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());

	pUser->UpdateItem((BYTE)pItem->m_dwObjId, UI_NUM, (pItem->m_nItemNum - 1));
	pUser->AddText("Your name color was changed successfully!", COLOR_SUCCESS);
}
#endif //__SYS_NAMECOLOR

#ifdef __GUILDFINDER
void CDPSrvr::OnGuildFinderRequest(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (pUser)
	{
		if (pUser->GetGuild() == NULL)
		{
			u_long uGuildID, dwMasterID;

			ar >> uGuildID >> dwMasterID;
			CGuild* pGuild = g_GuildMng.GetGuild(uGuildID);
			if (pGuild)
			{
				if (pGuild->m_idMaster == dwMasterID)
				{
					CUser* pMaster = g_UserMng.GetUserByPlayerID(pGuild->m_idMaster);
					if (pMaster)
					{
						pMaster->SendGuildApplication(pUser);
					}
					else
					{
						CItemElem itemelem;
						char szSub[MAX_MAILTEXT] = { 0, };
						_stprintf(szSub, "Player %s wants to join your guild\nName: %s\nJob: %s\nLevel: %d", pUser->GetName(), pUser->GetName(), prj.m_aJob[pUser->GetJob()].szName, pUser->GetLevel());
						g_dpDBClient.SendQueryPostMail(pGuild->m_idMaster, pUser->m_idPlayer, itemelem, 0, "Guild Finder", szSub);
					}
				}
			}
		}
	}
}

void CDPSrvr::OnGuildList(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (pUser)
	{
		vector<GUILDFINDER_LIST> vectmp;
		int nCount = 0;

		for (map<u_long, CGuild*>::iterator i = g_GuildMng.m_mapPGuild.begin(); i != g_GuildMng.m_mapPGuild.end(); ++i)
		{
			if (vectmp.size() == 100)
				break;

			CGuild* pGuild = (CGuild*)i->second;

			if (!pGuild)
				continue;

			if (pGuild->m_bGuildFinder == FALSE)
				continue;

			if (pGuild->m_mapPMember.size() < CGuild::sm_anMaxMemberSize[pGuild->m_nLevel - 1])
			{
				GUILDFINDER_LIST guildtmp;

				guildtmp.m_uGuildId = pGuild->m_idGuild;						// 극단 ID
				strcpy(guildtmp.m_sGuild, pGuild->m_szGuild);							// 극단 명칭( 단막극단 : NO, 순회극단 : YES )
				guildtmp.m_nSizeofMember = pGuild->m_mapPMember.size();								// 극단원 숫자	( 2 ~ 8 )
				guildtmp.m_nLevel = pGuild->m_nLevel;
				guildtmp.m_nLeaderId = pGuild->m_idMaster;
				guildtmp.m_nMaxSize = CGuild::sm_anMaxMemberSize[pGuild->m_nLevel - 1];
				vectmp.push_back(guildtmp);
				nCount++;
			}
		}
		pUser->AddGuildList(vectmp, nCount);
	}
}

void CDPSrvr::OnGuildFinderOpt(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BOOL bGuildFinder;
	ar >> bGuildFinder;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;
	if (pUser->GetGuild() && pUser->IsGuildMaster())
		g_DPCoreClient.SendGuildFinder(pUser, bGuildFinder);
	else
		Error("(Error)Player Changed Clientside Guildadmin");
}
#endif //__GUILDFINDER

#ifdef	__REMOVE_DURATION_ITEMS
void CDPSrvr::OnRemoveCmm(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		DWORD nItem;
		ar >> nItem;
		ItemProp* pItemProp = prj.GetItemProp(nItem);
		if (pItemProp)
		{
			bool bRemoved = false;

#ifdef __AEGON_PERMANENT_BUFF
			if (pItemProp->dwItemKind3 == IK3_PERM_BUFF) 
			{
				return;
			}
#endif // __AEGON_PERMANENT_BUFF

#ifdef __SYS_MAP_BUFFS
			if (pItemProp->dwItemKind3 == IK3_MAPBUFF)
			{
				return;
			}
#endif //__SYS_MAP_BUFFS

#ifdef __VENDING_NPC
			if (pItemProp->dwItemKind3 == IK3_VENDING)
			{
				return;
			}
#endif //__VENDING_NPC

#ifdef __ANARCHY_SYSTEM
			if (pItemProp->dwItemKind3 == IK3_ANARCHY)
			{
				return;
			}
#endif //__ANARCHY_SYSTEM

#ifdef __WBQT_GUILD_BUFFS
			if (pItemProp->dwItemKind3 == IK3_GUILD_BUFF)
			{
				return;
			}
#endif //__WBQT_GUILD_BUFFS

			for (int i = 0; i <= BUFF_EQUIP; i++)
			{
#ifdef __INCREASE_SKILL_ID_VALUE
				if (pUser->HasBuff(i, pItemProp->dwID))
				{
					pUser->RemoveBuff(i, pItemProp->dwID);
					bRemoved = true;
				}
#else
				if (pUser->HasBuff(i, (WORD)(pItemProp->dwID)))
				{
					pUser->RemoveBuff(i, (WORD)(pItemProp->dwID));
					bRemoved = true;
				}
#endif
				
			}

			if (!bRemoved)
			{
				int nType = -1;
				switch (pItemProp->dwID)
				{
				case II_CHR_SYS_SCR_SHOUTFULL15: 	nType = SM_SHOUT15; 	 break;
				case II_CHR_SYS_SCR_SHOUTFULL30: 	nType = SM_SHOUT30; 	 break;
				case II_CHR_SYS_SCR_SHOUTFULL001: 	nType = SM_SHOUT001; 	 break;
				case II_CHR_SYS_SCR_PSKILLFULL1: 	nType = SM_PARTYSKILL1;  break;
				case II_CHR_SYS_SCR_PSKILLFULL15: 	nType = SM_PARTYSKILL15; break;
				case II_CHR_SYS_SCR_PSKILLFULL30: 	nType = SM_PARTYSKILL30; break;
				case II_CHR_SYS_SCR_COMMBANK15: 	nType = SM_BANK15; 		 break;
				case II_CHR_SYS_SCR_COMMBANK30: 	nType = SM_BANK30; 		 break;
				case II_CHR_SYS_SCR_COMMBANK001: 	nType = SM_BANK001; 	 break;
				case II_CHR_SYS_SCR_ACTIVITION: 	nType = SM_ACTPOINT; 	 break;
				case II_CHR_SYS_SCR_UPCUTSTONE01: 	nType = SM_ATTACK_UP1; 	 break;
				case II_CHR_SYS_SCR_UPCUTSTONE: 	nType = SM_ATTACK_UP; 	 break;
				case II_SYS_SYS_SCR_BLESSING: 		nType = SM_REVIVAL; 	 break;
				case II_SYS_SYS_SCR_VELOCIJUMP: 	nType = SM_VELOCIJUMP; 	 break;
				case II_CHR_SYS_SCR_ESCAPEBLINKWING:nType = SM_ESCAPE; 		 break;
				case II_CHR_REF_REF_HOLD: 			nType = SM_MAINTAIN_MP;  break;
				case II_CHR_POT_DRI_VITALX: 		nType = SM_MAINTAIN_FP;  break;
				case II_CHR_FOO_COO_BULLHAMS: 		nType = SM_MAX_HP; 		 break;
				case II_CHR_FOO_COO_GRILLEDEEL: 	nType = SM_MAX_HP50; 	 break;
				case II_SYS_SYS_VIR_PENALTYSTR: 	nType = SM_STR_DOWN; 	 break;
				case II_SYS_SYS_VIR_PENALTYSTA: 	nType = SM_STA_DOWN; 	 break;
				case II_SYS_SYS_VIR_PENALTYDEX: 	nType = SM_DEX_DOWN; 	 break;
				case II_SYS_SYS_VIR_PENALTYINT: 	nType = SM_INT_DOWN; 	 break;
#ifdef __PREMIUM
				case II_SCR_PREM_NOTUSE:			nType = SM_PREMIUM;		 break;
#endif //__PREMIUM
				}
				if (nType != -1)
				{
					pUser->RemoveSMMode(nType);
					bRemoved = true;
				}
			}

			if (!bRemoved)
			{
				CString strCR;
				strCR.Format("You can't remove %s!", pItemProp->szName);
				pUser->AddText(strCR, COLOR_ERROR);
			}
			else
			{
#ifdef __AEGON_IMPROVED_SHOPDESIGN
				if (pItemProp->dwItemKind3 ==  IK3_SHOPDESIGN)
				{
					CAegonShopDesignMng::GetInstance()->OnPlayerShopDesignBuffChange(pUser, TRUE);
				}
#endif

				CString strRM;
				strRM.Format("You succesfully removed %s!", pItemProp->szName);
				pUser->AddText(strRM, COLOR_SUCCESS);
			}
		}
	}
}
#endif	// __REMOVE_DURATION_ITEMS

#ifdef __SYS_MONSTER_CLASH
void	CDPSrvr::OnMonsterClashRegisterWindow(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	pUser->AddMonsterClashRegisterWindow();
}

void	CDPSrvr::OnMonsterClashRegister(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (CMonsterClashMng::GetInstance()->RegisterGuild(pUser))
		pUser->AddMonsterClashRegister(pUser->m_idGuild, pUser->GetGuild() == nullptr ? "Unkown" : pUser->GetGuild()->m_szGuild);
}

void	CDPSrvr::OnMonsterClashRegisterMemberWindow(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	pUser->AddMOnsterClashRegisterMemberWindow();
}

void	CDPSrvr::OnMonsterClashRegisterMember(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	u_long idPlayer;
	ar >> idPlayer;

	CUser* pMember = g_UserMng.GetUserByPlayerID(idPlayer);
	if (!IsValidObj(pMember))
	{
		pUser->AddDefinedText(TID_GAME_MONSTERCLASH_MEMBER_OFFLINE);
		return;
	}

	if (CMonsterClashMng::GetInstance()->RegisterGuildMember(pUser, pMember))
		pUser->AddMonsterClashRegisterMember(pMember->m_idPlayer, pMember->GetName());
}

void	CDPSrvr::OnMonsterClashUnRegisterMember(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	u_long idPlayer;
	ar >> idPlayer;

	CUser* pMember = g_UserMng.GetUserByPlayerID(idPlayer);
	if (!IsValidObj(pMember))
	{
		pUser->AddDefinedText(TID_GAME_MONSTERCLASH_MEMBER_OFFLINE);
		return;
	}
	if (CMonsterClashMng::GetInstance()->UnregisterGuildMember(pUser, pMember))
		pUser->AddMonsterClashUnregisterMember(pMember->m_idPlayer, pMember->GetName());
}
#endif //__SYS_MONSTER_CLASH

#ifdef __REPORT_SYSTEM
void CDPSrvr::OnSendReport(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		BYTE nReportType;
		char lpszText[512] = { 0, };

		ar >> nReportType;
		ar.ReadString(lpszText, 512);

		if (strlen(lpszText) >= 1 && strlen(lpszText) <= 512)
		{
			CString szText, strFile;
			switch (nReportType)
			{
			case 0: 
				szText.Format(prj.GetText(TID_REPORT_TEXT_01), pUser->GetName(), lpszText);
				strFile.Format("..\\Program\\Reports\\%s_BUG_REPORT.txt", pUser->GetName());
				break;
			case 1: 
				szText.Format(prj.GetText(TID_REPORT_TEXT_02), pUser->GetName(), lpszText);
				strFile.Format("..\\Program\\Reports\\%s_REPORT_PLAYER.txt", pUser->GetName());
				break;
			case 2: 
				szText.Format(prj.GetText(TID_REPORT_TEXT_03), pUser->GetName(), lpszText);
				strFile.Format("..\\Program\\Reports\\%s_SUGGESTION.txt", pUser->GetName());
				break;
			case 3: 
				szText.Format(prj.GetText(TID_REPORT_TEXT_04), pUser->GetName(), lpszText);
				strFile.Format("..\\Program\\Reports\\%s_OTHER.txt", pUser->GetName());
				break;
			default: 
				pUser->AddText(prj.GetText(TID_REPORT_TEXT_10)); 
				return; 
				break;
			}
			if (GetTickCount() < pUser->m_dwTickLastReport + SEC(60))
			{
				pUser->AddText(prj.GetText(TID_REPORT_TEXT_13), COLOR_ERROR);
				return;
			}
			pUser->m_dwTickLastReport = GetTickCount();

#ifdef __GAMEGUARD
			GameGuardLog(szText, strFile, 0);
#else // __GAMEGUARD
			Error(szText);
#endif // __GAMEGUARD
			pUser->AddText(prj.GetText(TID_REPORT_TEXT_05), COLOR_SUCCESS);
		}
		else
			pUser->AddText(prj.GetText(TID_REPORT_TEXT_11), COLOR_ERROR);
	}
}
#endif // __REPORT_SYSTEM

#ifdef __SYS_SANDBOX
void CDPSrvr::OnSandbox(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BOOL bClear = FALSE;
	DWORD dwIndex = NULL_ID;
	BOOL bDamage = FALSE;
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (!IsValidObj(pUser))
		return;

	if (!pUser->IsWorldSandBox())
		return;

	ar >> bClear;
	if (bClear)
	{
		CSandbox::GetInstance()->ClearMonster(pUser->GetLayer());
	}
	else
	{
		ar >> dwIndex;
		ar >> bDamage;
		int nJump = 0;
		MoverProp* pProp = nullptr;
		const static vector<DWORD> blacklist = {
		MI_BRIGADIER, MI_LIEUTENANT, MI_SPIKETAIL, MI_GLYPHAXZ, MI_EVENT01, MI_EVENT02, MI_EVENT03, MI_EVENT04, MI_IMP01, MI_IMP02, MI_IMP03,
		MI_CAITSITH04_1, MI_HARPY04_1, MI_POLEVIK04_1, MI_ABRAXAS04_1, MI_HAG04_1, MI_THOTH04_1, MI_KHNEMU04_1, MI_DANTALIAN04_1, MI_GANESA04_1,
		MI_ASURA04_1, MI_DREADSTONE01, MI_DREADSTONE02, MI_DREADSTONE03, MI_DREADSTONE04, MI_DREADSTONE05, MI_DREADSTONE06, MI_RUSTIACRASHGATE01,
		MI_RUSTIACRASHGATE02, MI_RUSTIACRASHGATE03, MI_RUSTIACRASHGATE04, MI_BEHESTATUE01, MI_KALGASEGG01, MI_KALGASSTELE01, MI_KALGASSTELE02,
		MI_CLOCKWORK1, MI_LOADCLOCKWORK, MI_RANGDA01, MI_RANGDA02, MI_RANGDA03, MI_RANGDA04};

		for (int i = 0; i < prj.m_nMoverPropSize; i++)
		{
			MoverProp* pMoverProp = prj.m_pPropMover + i;
			if (pMoverProp->dwID == 0)
				continue;
			if (pMoverProp->dwClass != RANK_SUPER)
				continue;
			if (std::find(blacklist.begin(), blacklist.end(), pMoverProp->dwID) != blacklist.end())
			{
				continue;
			}
			if (dwIndex == nJump)
				pProp = pMoverProp;

			nJump++;
		}
		if (pProp)
			CSandbox::GetInstance()->SpawnMonster(pUser->GetLayer(), pProp->dwID, bDamage);
	}
}

void CDPSrvr::OnSandboxTeleport(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (!IsValidObj(pUser))
		return;

	if (pUser->IsWorldSandBox())
		pUser->REPLACE(g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3(6975.65f, 0.0f, 3331.5f), REPLACE_FORCE, nDefaultLayer);
	else
		pUser->REPLACE(g_uIdofMulti, WORLD_SANDBOX, D3DXVECTOR3(558.763F, 100.481F, 570.177F), REPLACE_FORCE, nDefaultLayer);
}
#endif //__SYS_SANDBOX

#ifdef __CHECKLOAD 
void CDPSrvr::OnCheckLoad(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	unsigned long worldId;
	int layer;
	ar >> worldId >> layer;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
		pUser->DoCheckLoad(worldId, layer);
}
#endif //__CHECKLOAD

#ifdef __TIMER
void CDPSrvr::OnDungeonTimer(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
		pUser->AddTimer();
}
#endif //__TIMER

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
void CDPSrvr::OnLoadAchievementProgress(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (pUser && IsValidObj(pUser)) 
	{
		pUser->AddAchievementLoadProgress();
	}	
}
void CDPSrvr::OnCheckAchievementProgress(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	int mode, msg;
	ar >> mode >> msg;
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (pUser && IsValidObj(pUser))
	{
		switch (mode) 
		{
		case 0: // single achievement
			CAegonAchievementMng::GetInstance()->CheckAchievementProgress(pUser, msg);
			break;
		case 1: // type
			CAegonAchievementMng::GetInstance()->CheckAchievementProgressForType(pUser, msg);
			break;
		default:
			break;
		}
			
	}
}
void CDPSrvr::OnCollectAchievement(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	int achievementID;
	ar >> achievementID;
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (pUser && IsValidObj(pUser))
	{
		CAegonAchievementMng::GetInstance()->CollectAchievementReward(pUser, achievementID);
	}
}

void CDPSrvr::OnAchievementFavorite(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	int achievementID;
	ar >> achievementID;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (pUser && IsValidObj(pUser))
	{
		vector<int>::iterator it = pUser->m_AchievementFavorites.begin();
		for (; it != pUser->m_AchievementFavorites.end(); it++)
		{
			if (*it == achievementID)
			{
				pUser->m_AchievementFavorites.erase(it);
				pUser->AddFavoriteAchievementRefresh();
				return;
			}
		}

		pUser->m_AchievementFavorites.push_back(achievementID);
		pUser->AddFavoriteAchievementRefresh();
		return;			
	}
}
#endif // __AEGON_ACHIEVEMENT_SYSTEM

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
void CDPSrvr::OnReloadTaskbar(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (pUser && IsValidObj(pUser))
	{
		pUser->AddTaskBar();
	}
}
#endif // __AEGON_FULLSCREEN_AND_RESIZE

#ifdef __PLAYER_LIST
void CDPSrvr::OnPlayerListRequest(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (pUser->m_dwPlayerList + 500 > GetTickCount())
		{
			pUser->AddText(prj.GetText(TID_PLAYER_LIST_ERR_04), COLOR_ERROR);
			return;
		}
		pUser->m_dwPlayerList = GetTickCount();

		BOOL bCheck[17];
		for (int i = 0; i < 17; i++)
			ar >> bCheck[i];

		int nMin, nMax;
		ar >> nMin >> nMax;

		TCHAR search[32];
		ar.ReadString(search, 32);

		int nTotal = 0;
		for (auto const& user : g_UserMng.m_users)
		{
			if (IsValidObj(user.second))
			{
				if (bCheck[0] == FALSE && user.second->m_dwAuthorization == AUTH_GENERAL)
					continue;
				if (bCheck[1] == FALSE && user.second->m_dwAuthorization > AUTH_GENERAL)
					continue;
				if (bCheck[2] == FALSE && user.second->GetJob() == JOB_MERCENARY)
					continue;
				if (bCheck[3] == FALSE && (user.second->GetJob() == JOB_KNIGHT || user.second->GetJob() == JOB_KNIGHT_MASTER || user.second->GetJob() == JOB_KNIGHT_HERO
#ifdef __3RD_LEGEND16
					|| user.second->GetJob() == JOB_LORDTEMPLER_HERO
#endif // __3RD_LEGEND16
					))
				{
					continue;
				}
				if (bCheck[4] == FALSE && (user.second->GetJob() == JOB_BLADE || user.second->GetJob() == JOB_BLADE_MASTER || user.second->GetJob() == JOB_BLADE_HERO
#ifdef __3RD_LEGEND16
					|| user.second->GetJob() == JOB_STORMBLADE_HERO
#endif // __3RD_LEGEND16
					))
				{
					continue;
				}
				if (bCheck[5] == FALSE && user.second->GetJob() == JOB_ACROBAT)
					continue;
				if (bCheck[6] == FALSE && (user.second->GetJob() == JOB_JESTER || user.second->GetJob() == JOB_JESTER_MASTER || user.second->GetJob() == JOB_JESTER_HERO
#ifdef __3RD_LEGEND16
					|| user.second->GetJob() == JOB_WINDLURKER_HERO
#endif // __3RD_LEGEND16
					))
				{
					continue;
				}
				if (bCheck[7] == FALSE && (user.second->GetJob() == JOB_RANGER || user.second->GetJob() == JOB_RANGER_MASTER || user.second->GetJob() == JOB_RANGER_HERO
#ifdef __3RD_LEGEND16
					|| user.second->GetJob() == JOB_CRACKSHOOTER_HERO
#endif // __3RD_LEGEND16
					))
				{
					continue;
				}
				if (bCheck[8] == FALSE && user.second->GetJob() == JOB_ASSIST)
					continue;
				if (bCheck[9] == FALSE && (user.second->GetJob() == JOB_RINGMASTER || user.second->GetJob() == JOB_RINGMASTER_MASTER || user.second->GetJob() == JOB_RINGMASTER_HERO
#ifdef __3RD_LEGEND16
					|| user.second->GetJob() == JOB_FLORIST_HERO
#endif // __3RD_LEGEND16
					))
				{
					continue;
				}
				if (bCheck[10] == FALSE && (user.second->GetJob() == JOB_BILLPOSTER || user.second->GetJob() == JOB_BILLPOSTER_MASTER || user.second->GetJob() == JOB_BILLPOSTER_HERO
#ifdef __3RD_LEGEND16
					|| user.second->GetJob() == JOB_FORCEMASTER_HERO
#endif // __3RD_LEGEND16
					))
				{
					continue;
				}
				if (bCheck[11] == FALSE && user.second->GetJob() == JOB_MAGICIAN)
					continue;
				if (bCheck[12] == FALSE && (user.second->GetJob() == JOB_PSYCHIKEEPER || user.second->GetJob() == JOB_PSYCHIKEEPER_MASTER || user.second->GetJob() == JOB_PSYCHIKEEPER_HERO
#ifdef __3RD_LEGEND16
					|| user.second->GetJob() == JOB_MENTALIST_HERO
#endif // __3RD_LEGEND16
					))
				{
					continue;
				}
				if (bCheck[13] == FALSE && (user.second->GetJob() == JOB_ELEMENTOR || user.second->GetJob() == JOB_ELEMENTOR_MASTER || user.second->GetJob() == JOB_ELEMENTOR_HERO
#ifdef __3RD_LEGEND16
					|| user.second->GetJob() == JOB_ELEMENTORLORD_HERO
#endif // __3RD_LEGEND16
					))
				{
					continue;
				}

				if (bCheck[14] == FALSE && user.second->GetSex() == SEX_MALE)
					continue;

				if (bCheck[15] == FALSE && user.second->GetSex() == SEX_FEMALE)
					continue;

				if (bCheck[16] == FALSE && user.second->GetJob() == JOB_VAGRANT)
					continue;

				if (user.second->GetLevel() < nMin || user.second->GetLevel() > nMax)
					continue;

				if (CString(search).GetLength())
				{
					if (!strstr(CString(user.second->GetName()).MakeLower(), CString(search).MakeLower()))
						continue;
				}

				if (!strlen(user.second->GetName()))
					continue;

				PlayerList player;
				strcpy(player.szName, user.second->GetName());
				//strcpy(player.szRank, user.second->GetAuthStr(TRUE));//
				player.dwJob = user.second->GetJob();
				player.dwLevel = user.second->GetLevel();
				player.dwSex = user.second->GetSex();
				player.nID = nTotal;

				pUser->SendPlayerList(FALSE, player, TRUE, (nTotal == 0));
				nTotal++;
			}
		}

		if (nTotal < 1)
		{
			pUser->SendPlayerList(TRUE);
		}
	}
}
#endif // __PLAYER_LIST

#ifdef __AEGON_CRAFTING_SYSTEM
void CDPSrvr::OnCraftRecipe(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	int unitID, nComponents;
	DWORD componentObjID[AEGCRAFT_MAXCOMPONENTS];
	ar >> unitID;
	ar >> nComponents;

	for (int i = 0; i < nComponents; i++)
	{
		ar >> componentObjID[i];
	}


	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (pUser && IsValidObj(pUser))
	{
		CItemElem* componentItems[AEGCRAFT_MAXCOMPONENTS];
		__CRAFTING_UNIT* selectedUnit;
		
		for (int i = 0; i < nComponents; i++) 
		{
			componentItems[i] = pUser->m_Inventory.GetAtId(componentObjID[i]);
			if (!componentItems[i]) 
			{
				pUser->AddText("Error component missing.", COLOR_ERROR);
				return;
			}	
		}
		map <int, __CRAFTING_UNIT>::iterator it = CAegonCraftingMng::GetInstance()->m_mapCraftingUnits.find(unitID);
		if (it == CAegonCraftingMng::GetInstance()->m_mapCraftingUnits.end()) 
		{
			pUser->AddText("Error invalid recipe.", COLOR_ERROR);
			return;
		}
		selectedUnit = &it->second;

		if (selectedUnit->levelRequirement > CAegonCraftingMng::GetInstance()->GetCraftingLevel(pUser))
		{
			pUser->AddText("Error crafting level too low.", COLOR_ERROR);
			return;
		}

		if(selectedUnit->components.size() != nComponents)
		{
			pUser->AddText("Error component count mismatch.", COLOR_ERROR);
			return;
		}

		for (int i = 0; i < nComponents; i++)
		{
			if (componentItems[i]->GetPropA()->dwID != selectedUnit->components[i].m_itemID)
			{
				pUser->AddText("Error wrong component.", COLOR_ERROR);
				return;
			}
			if (componentItems[i]->m_nItemNum < selectedUnit->components[i].m_itemNum )
			{
				pUser->AddText("Error not enough components.", COLOR_ERROR);
				return;
			}
		}
		DWORD dwTickCount = GetTickCount();
		if ((pUser->m_dwLastCrafting + selectedUnit->craftTime * 1000) - 100 >= dwTickCount)
		{
			//pUser->AddText("time error", COLOR_ERROR);
			return;
		}

		CItemElem resultItem;
		resultItem.m_dwItemId = selectedUnit->resultItemID;
		resultItem.m_nItemNum = selectedUnit->resultItemNum;
		resultItem.SetSerialNumber();

		if (pUser->CreateItem(&resultItem)) 
		{
			for (int i = 0; i < nComponents; i++) 
			{
				pUser->RemoveItem(componentItems[i]->m_dwObjId, selectedUnit->components[i].m_itemNum);
			}

			pUser->m_dwLastCrafting = GetTickCount();

			CString strTmp;
			strTmp.Format("Successfully crafted %dx %s", resultItem.m_nItemNum, resultItem.GetName().GetString());
			pUser->AddText(strTmp, COLOR_SUCCESS);

			if(CAegonCraftingMng::GetInstance()->AddExperience(pUser, selectedUnit->craftingExp))
				pUser->AddCraftingExpRefresh();

			pUser->AddCraftingSuccess();

			if(pUser->GetWorld())
				g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
		}
		else 
		{
			pUser->AddText("Error check inventory space.", COLOR_ERROR);
			return;
		}	
	}
}
#endif // __AEGON_CRAFTING_SYSTEM

#ifdef __WEAPON_MERGE
void CDPSrvr::OnWeaponMerge(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsInvalidObj(pUser))
		return;

	DWORD dwIdSource, dwIdDest, dwIdProt;
	CItemElem* pItemSource = NULL, * pItemDest = NULL, * pItemProt = NULL;
	ItemProp* pPropSource, * pPropDest;
	BOOL bProt = FALSE;

	ar >> dwIdSource >> dwIdDest >> dwIdProt;

	if (dwIdSource == dwIdDest)
		return;

	pItemSource = pUser->m_Inventory.GetAtId(dwIdSource);
	pItemDest = pUser->m_Inventory.GetAtId(dwIdDest);
	if (dwIdProt != NULL_ID)
		pItemProt = pUser->m_Inventory.GetAtId(dwIdProt);
	else
		pItemProt = NULL;

	if (!IsUsableItem(pItemSource) || !IsUsableItem(pItemDest))
		return;

	if (pUser->m_vtInfo.IsTrading(pItemSource) || pUser->m_vtInfo.IsTrading(pItemDest))
		return;

	pPropSource = pItemSource->GetProp();
	pPropDest = pItemDest->GetProp();

	if (!pPropSource || !pPropDest)
		return;

	if (pItemSource->IsMerged() || pItemDest->IsMerged())					//already merged?
	{
		pUser->AddText(prj.GetText(TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT01), COLOR_ERROR);
		return;
	}

	if (pItemSource->IsExpiring() || pItemDest->IsExpiring())				//time-limited?
	{
		pUser->AddText(prj.GetText(TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT02), COLOR_ERROR);
		return;
	}

	if (pPropSource->dwHanded != HD_TWO || pPropDest->dwHanded != HD_TWO)	//two handed?
	{
		pUser->AddText(prj.GetText(TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT03), COLOR_ERROR);
		return;
	}

#ifdef __NEW_ITEM_VARUNA
	if (pPropSource->IsBaruna() || pPropDest->IsBaruna())					//baruna?
	{
		pUser->AddText(prj.GetText(TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT04), COLOR_ERROR);
		return;
	}
#endif //__NEW_ITEM_VARUNA

	if (pPropSource->dwItemKind3 != pPropDest->dwItemKind3)				//same item kind?
	{
		pUser->AddText(prj.GetText(TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT05), COLOR_ERROR);
		return;
	}
	
	BOOL bSoulbound = FALSE;
	if (pItemSource->IsBinds() || pItemDest->IsBinds())					//soulbound?
	{
		//pUser->AddText(prj.GetText(TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT06), COLOR_ERROR);
		//return;
		bSoulbound = TRUE;
	}
	if (pPropSource->dwLimitLevel1 < pPropDest->dwLimitLevel1)				//source higher level than destination item?
	{
		pUser->AddText(prj.GetText(TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT08), COLOR_ERROR);
		return;
	}

	if (pPropSource->dwItemJob < pPropDest->dwItemJob)						//source higher level than destination item?
	{
		pUser->AddText(prj.GetText(TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT08), COLOR_ERROR);
		return;
	}

	if (g_xRandomOptionProperty->IsCheckedSafeFlag(pItemSource->GetRandomOptItemId()) || g_xRandomOptionProperty->IsCheckedSafeFlag(pItemDest->GetRandomOptItemId()))
	{	//safe random option?
		pUser->AddText(prj.GetText(TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT13), COLOR_ERROR);
		return;
	}

	if ((bProt = IsUsableItem(pItemProt)))												// check if a protection scoll has been placed
	{
		if (pItemProt->GetProp()->dwID != II_SYS_SYS_BX_MAGICGRINDSTONE)
			bProt = FALSE;
		if (pUser->m_vtInfo.IsTrading(pItemProt))
			bProt = FALSE;
	}
#ifdef __AEGON_WEAPON_MERGE_100
	if(TRUE)
#else
	if (xRandom(100000) < 90000) //success
#endif
	{
		if (g_xRandomOptionProperty->MergeWeaponRandomOption(pItemSource, pItemDest) != 0)
		{
			Error("Bad behaviour on Line %d File %s", __LINE__, __FILE__);
			return;
		}
		if (bProt)
			pUser->RemoveItem((BYTE)pItemProt->m_dwObjId, 1);
		int nSize = pItemSource->GetPiercingSize();											// transfer the piercing data
		pItemDest->SetMergedPiercingSize(nSize);
		for (int i = 0; i < nSize; ++i)
		{
			DWORD dwItem = pItemSource->GetPiercingItem(i);
			pItemDest->SetMergedPiercingItem(i, dwItem);
		}

		float fFactor = 0.0f;																// calculate attack factor

		if (pItemSource->GetAbilityOption() >= 10)
			fFactor = 0.48f;
		else if (pItemSource->GetAbilityOption() > 3)
			fFactor = 0.12f;

		int nATK = (int)((pItemSource->GetProp()->dwAbilityMax * fFactor));					// afterwards calculate the attack to be added
		pItemDest->SetMergedWeaponATK(nATK);												// and assign it to the destination weapon

		pUser->RemoveItem((BYTE)pItemSource->m_dwObjId, 1);									// destroy the source item

		pItemDest->SetFlag(CItemElem::merged);												// and safe the flag

		if (bSoulbound && !pItemDest->IsFlag(CItemElem::binds))
			pItemDest->SetFlag(CItemElem::binds);

		pUser->AddMergedWeapon(pItemDest);													// send new information to the client
		
		pUser->UpdateItem((BYTE)(pItemDest->m_dwObjId), UI_FLAG, MAKELONG(pItemDest->m_dwObjIndex, pItemDest->m_byFlag));

		pUser->AddText(prj.GetText(TID_MMI_TWOHANDEDWEAPONMERGE_SUCCESSTEXT01), COLOR_SUCCESS);
	}
#ifdef __AEGON_WEAPON_MERGE_100
	else 
	{
		Error("CDPSrvr::OnWeaponMerge user: %s", pUser->GetName());
	}
#else
	else {						  //miss
		if (bProt) {																				/* if a protection scroll has been placed, just destroy the scroll */
			pUser->RemoveItem((BYTE)pItemProt->m_dwObjId, 1);
			pUser->AddText(prj.GetText(TID_MMI_TWOHANDEDWEAPONMERGE_FAILTEXT02), COLOR_ERROR);
		}
		else {																						/* otherwise the source item will get destroyed */
			pUser->AddDefinedText(TID_MMI_TWOHANDEDWEAPONMERGE_FAILTEXT01, pItemSource->GetName());
			pUser->RemoveItem((BYTE)pItemSource->m_dwObjId, 1);
			//if( ::GetLang() != LANG_GER )															/* only in german flyff the destination weapon wont*/
			//	pUser->RemoveItem((BYTE)pItemDest->m_dwObjId, 1);										/* get destroyed */
		}
	}
#endif
}
void CDPSrvr::OnWeaponMergeCancel(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsInvalidObj(pUser))
		return;

	DWORD dwIdItem;
	ar >> dwIdItem;

	CItemElem* pItem = pUser->m_Inventory.GetAtId(dwIdItem);

	if (!IsUsableItem(pItem))
		return;

	if (pUser->m_vtInfo.IsTrading(pItem))
		return;

	if (!pItem->IsMerged())
		return;

	int nCost = 0;													// Calculate removing costs

	if (pItem->GetMergedPiercingSize() > 0)
		nCost += 100000 + 100000 * pItem->GetMergedPiercingSize();

	if (pItem->GetMergedRandomOptItemId() > 0)
		nCost += 100000;

	if (pItem->GetMergedWeaponATK() > 0)
		nCost += 100000;

	if (pUser->GetGold() < nCost)
	{
		pUser->AddDefinedText(TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT12, "%d", nCost);
		return;
	}

	pUser->AddGold(-nCost);
	pItem->SetMergedRandomOptItemId(0);								// and remove the merged data
	pItem->SetMergedPiercingSize(0);
	pItem->SetMergedWeaponATK(0);
	pItem->ResetFlag(CItemElem::merged);
	pUser->AddMergedWeaponCancel(pItem->m_dwObjId);
	pUser->AddText(prj.GetText(TID_MMI_TWOHANDEDWEAPONMERGE_SUCCESSTEXT02), COLOR_SUCCESS);
}
#endif //__WEAPON_MERGE

#ifdef __BUYBACK
void CDPSrvr::OnBuyBack(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		int nItem;
		ar >> nItem;

#ifdef __MIKSIK_EQUALIZED_MODE
		if (pUser->bTmpMode) 
		{
			pUser->AddText("You cant use that in temporal mode!");
			return;
		}
#endif //__MIKSIK_EQUALIZED_MODE

		CItemElem* pItemElem = pUser->m_BuyBack.GetAt(nItem);
		if (pItemElem)
		{
			int nNum = pItemElem->m_nItemNum;
#ifdef __GAMEGUARD
			__int64 nTotal = (__int64)pItemElem->getBuyBackPrice() * nNum;
			if (nTotal < 0 || nTotal > INT_MAX)
			{
				Error("CDPSrvr::OnBuyBack overflow on %s : nTotal(%I64d)", pUser->GetName(), nTotal);
				pUser->BuyBackRemoveItem(-1);
				return;
			}
#endif // __GAMEGUARD
			if (pUser->GetGold() < nTotal)
			{
				pUser->AddDefinedText(TID_GAME_LACKMONEY);
				pUser->BuyBackRemoveItem(-1);
				return;
			}

			if (pUser->CreateItem(pItemElem))
			{
				pUser->AddGold((-1) * nTotal);
				if (pItemElem->getBuyBackTaxes() > 0)
				{
					__TAXINFO* pTaxInfo = CTax::GetInstance()->GetTaxInfo(pItemElem->getBuyBackCont());
					if (pTaxInfo && pTaxInfo->dwId != NULL_ID)
					{
						CTax::GetInstance()->AddTax(pItemElem->getBuyBackCont(), -pItemElem->getBuyBackTaxes() * nNum, TAX_PURCHASE);
					}
				}
				pUser->m_BuyBack.RemoveAtId(nItem);
				Reorganize(&pUser->m_BuyBack);
				pUser->BuyBackRemoveItem(nItem);
				pUser->BuyBackRemoveItem(-1);
#ifdef __GAMEGUARD
				if (pUser && pUser->GetWorld())
					g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
#endif // __GAMEGUARD
			}
			else
				pUser->AddDefinedText(TID_GAME_LACKSPACE);
		}
		else
			Error("CDPSrvr::OnBuyBack id %d doesn't exist in %s CItemElem container!", nItem, pUser->GetName());
	}
}
#endif // __BUYBACK

#ifdef __AEGON_SEASON_PASS
void CDPSrvr::OnLoadSeasonPassProgress(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser) || !pUser->m_bUpdateSeasonPass)
		return;

	pUser->AddSeasonPassLoadProgress();
	pUser->m_bUpdateSeasonPass = FALSE;
}
void CDPSrvr::OnClaimSeasonPassReward(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	int passID, rewardID;
	ar >> passID;
	ar >> rewardID;
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser) || passID < 0 || rewardID < 0)
		return;

	__SEASONPASS_UNIT* selectedUnit = CAegonSeasonPassMng::GetInstance()->GetUnit(passID);
	if (!selectedUnit)
		return;

	__SEASONPASS_PROGRESS_UNIT* progUnit = pUser->m_SeasonpassProgress.GetProgressUnit(passID);
	if (!progUnit)
		return;
	if (easy_find(progUnit->rewardsClaimed, rewardID))
		return;

	__SEASONPASS_REWARD* rewardUnit = NULL;
	for (int i = 0; i < selectedUnit->rewards.size(); i++) 
	{
		if (selectedUnit->rewards[i].rewardID == rewardID) 
		{
			rewardUnit = &selectedUnit->rewards[i];
			break;
		}
	}
	if (!rewardUnit)
		return;

	if (progUnit->progressPoints < rewardUnit->GetTotalPointsToFinish())
		return;

	if (!rewardUnit->IsFreeReward() && !progUnit->boughtPremium)
		return;

	CItemElem rewardItem;
	rewardItem.m_dwItemId = rewardUnit->itemID;
	rewardItem.m_nItemNum = rewardUnit->itemNum;
	rewardItem.SetSerialNumber();

	if (pUser->CreateItem(&rewardItem))
	{
		CString strTmp;
		strTmp.Format("You received %dx %s", rewardItem.m_nItemNum, rewardItem.GetName().GetString());
		pUser->AddText(strTmp, COLOR_SUCCESS);

		progUnit->rewardsClaimed.push_back(rewardUnit->rewardID);
		pUser->AddSeasonPassUpdateProgress(progUnit);

		if (pUser->GetWorld())
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
	}
	else
	{
		pUser->AddText("Error check inventory space.", COLOR_ERROR);
		return;
	}
}

void CDPSrvr::OnGetSeasonPassCountdown(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	__SEASONPASS_UNIT* pveUnit = CAegonSeasonPassMng::GetInstance()->GetCurrentUnit(TRUE);
	__SEASONPASS_UNIT* pvpUnit = CAegonSeasonPassMng::GetInstance()->GetCurrentUnit(FALSE);

	if(pveUnit && pvpUnit)
		pUser->AddSeasonPassCountdown(pveUnit->timeEnd, pvpUnit->timeEnd);
}
void CDPSrvr::OnPurchaseSeasonPass(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BOOL bPvE;
	int nPassID;
	ar >> bPvE;
	ar >> nPassID;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	__SEASONPASS_UNIT* currentUnit = CAegonSeasonPassMng::GetInstance()->GetCurrentUnit(bPvE);
	if (!currentUnit || currentUnit->passID != nPassID || pUser->m_bDPCommandInProgress)
	{
		return;
	}


	__SEASONPASS_PROGRESS_UNIT* progUnit = pUser->m_SeasonpassProgress.GetProgressUnit(currentUnit->passID);
	if (progUnit && progUnit->boughtPremium == 0)
	{
		__DP_COMMAND_INFO buyInfo;
		buyInfo.Init();

		buyInfo.command = INGAMEDP_BUYSEASONPASS;
		buyInfo.dpCost = currentUnit->nPriceDP;
		buyInfo.nDPCommandVar1 = currentUnit->passID;

		g_dpDBClient.SendDPCommand(pUser, buyInfo);
	}
}
#endif // __AEGON_SEASON_PASS

#ifdef __AEGON_ADVANCED_ANNOUNCEMENT
void CDPSrvr::OnAegonSystemShout(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CHAR szString[512];
	BOOL bShowName, bShowAuth;
	int nImage, nFont;
	ar.ReadString(szString, 512);
	ar >> bShowName;
	ar >> bShowAuth;
	ar >> nImage;
	ar >> nFont;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser) || strlen(szString) <= 0 )
		return;

	if (!pUser->IsAuthHigher(AUTH_GAMEMASTER)) 
	{
		Error("unauthorized sys shout attempt - playerID: %d, playerName: %s", pUser->m_idPlayer, pUser->GetName());
		return;
	}

	if (!pUser->IsAuthHigher(AUTH_OPERATOR))
		bShowName = TRUE;

	CString completeString;

	if(bShowName && bShowAuth)
		completeString.Format("[%s] [%s] %s", pUser->GetAuthString().GetString(), pUser->GetName(), szString);
	else if(bShowName)
		completeString.Format("[%s] %s", pUser->GetName(), szString);
	else if(bShowAuth)
		completeString.Format("[%s] %s", pUser->GetAuthString().GetString(), szString);
	else
		completeString.Format("%s", szString);

	g_DPCoreClient.SendSystem(completeString.GetString(), nImage, nFont);
}
#endif // __AEGON_ADVANCED_ANNOUNCEMENT

#ifdef __SWITCH_EQUIP
void CDPSrvr::OnEquipSwitch(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	vector<__EQUIP_SWITCH_INFO> vecTemp;
	
	//ar >> vecTemp;
	pUser->ReadEQSwitch(ar, &vecTemp);

	if (GetTickCount() - pUser->m_dwEquipSwitchTime < 500)
	{
		pUser->AddDefinedText(TID_SWITCH_COOLDOWN);
		return;
	}

	pUser->m_dwEquipSwitchTime = GetTickCount();

	bool bAble = true;

	CString strErr = "";

	if (vecTemp.size() > 5)
	{
		bAble = false;
		strErr += "size limit exceeded.";
	}
	else
	{
		for (int j = 0; j < vecTemp.size(); j++)
		{
			__EQUIP_SWITCH_INFO* pEquip = &vecTemp[j];
			for (int i = 0; i < MAX_HUMAN_PARTS; i++)
			{
				CItemElem* pItemElem = pUser->m_Inventory.GetAtId(pEquip->dwId[i]);
				if (IsUsableItem(pItemElem))
				{
					if (i == 0 && pItemElem->GetProp()->dwItemKind3 == IK3_PET || i == 1 && pItemElem->GetProp()->dwItemKind3 == IK3_EGG)
						continue;

					ItemProp* pProp = pItemElem->GetProp();

					if (!pUser->IsInteriorityJob(pProp->dwItemJob) && pProp->dwItemJob != NULL_ID)
					{
						bAble = false;
						CString strTmp;
						strTmp.Format("%s/%d/1.", pEquip[j].strTitle, i);
						strErr += strTmp;
						break;
					}
					else if (pProp->dwParts < 2 || pProp->dwParts >= MAX_HUMAN_PARTS || (pProp->dwItemSex != NULL_ID && pProp->dwItemSex != SEX_SEXLESS && pUser->GetSex() != pProp->dwItemSex))
					{
						bAble = false;
						CString strTmp;
						strTmp.Format("%s/%d/2.", pEquip[j].strTitle, i);
						strErr += strTmp;
						break;
					}
					else if (!(pProp->dwParts >= PARTS_HAT && pProp->dwParts <= PARTS_BOOTS) && pProp->dwLimitLevel1 != NULL_ID && pUser->GetLevel() < (int)pProp->dwLimitLevel1)
					{
						bAble = false;
						CString strTmp;
						strTmp.Format("%s/%d/3.", pEquip[j].strTitle, i);
						strErr += strTmp;
						break;
					}
				}
			}
			/*
			if (!bAble) 
			{
				vecTemp.erase(vecTemp.begin() + j);
				j--;
				bAble = true;
			}
			*/
		}
	}

	if (bAble)
	{
		pUser->m_vEquipSwitchInfo = vecTemp;
		pUser->AddText("Equipment switch saved!", COLOR_SUCCESS);
	}
	else
	{
		CString strTotal;
		strTotal.Format("Equipment switch failed... Error: %s", strErr.GetString());
		pUser->AddText(strTotal.GetString(), COLOR_ERROR);
	}

	pUser->SendEquipSwitch();
}
void CDPSrvr::OnEquipSwitchUse(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	int nIndex;
	ar >> nIndex;

	if (GetTickCount() - pUser->m_dwEquipSwitchTime < 1000)
	{
		pUser->AddDefinedText(TID_SWITCH_COOLDOWN);
		return;
	}

	pUser->m_dwEquipSwitchTime = GetTickCount();

	if (pUser->IsDie())
		return;

	if (pUser->IsFly())
		return;

	if (nIndex < 0 || nIndex >= (int)pUser->m_vEquipSwitchInfo.size())
		return;

	if (pUser->m_Inventory.GetEmptyCount() < 3)
		return;

	__EQUIP_SWITCH_INFO* pEquip = &pUser->m_vEquipSwitchInfo[nIndex];

	CItemElem* pElemR = pUser->GetEquipItem(PARTS_RWEAPON);
	CItemElem* pElemL = pUser->GetEquipItem(PARTS_LWEAPON);

	if (pEquip->dwId[PARTS_LWEAPON] != NULL_ID && pEquip->dwId[PARTS_RWEAPON] != NULL_ID &&
		pElemL && pElemR && pElemR->GetProp()->dwHanded == HD_ONE &&
		(pEquip->dwId[PARTS_LWEAPON] != pElemL->m_dwObjId || pEquip->dwId[PARTS_RWEAPON] != pElemR->m_dwObjId))
	{
		pUser->EquipItem(pElemL, FALSE, PARTS_LWEAPON);
		pUser->EquipItem(pElemR, FALSE, PARTS_RWEAPON);
	}
	else if ((pEquip->dwId[PARTS_LWEAPON] != NULL_ID || pEquip->dwId[PARTS_RWEAPON] != NULL_ID) && pElemL && !pElemR &&
		pEquip->dwId[PARTS_LWEAPON] != pElemL->m_dwObjId)
		pUser->EquipItem(pElemL, FALSE, PARTS_RWEAPON);
	else if ((pEquip->dwId[PARTS_LWEAPON] != NULL_ID || pEquip->dwId[PARTS_RWEAPON] != NULL_ID) && !pElemL && pElemR &&
		pEquip->dwId[PARTS_RWEAPON] != pElemR->m_dwObjId)
		pUser->EquipItem(pElemR, FALSE, PARTS_RWEAPON);

	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		if (pEquip->dwId[i] == NULL_ID)
			continue;

		pUser->m_tLastOpenPet = 0;
		int nPart = i;
		if (pUser->IsInteriorityJob(JOB_BLADE))
		{
			if (i == PARTS_LWEAPON)
				nPart = PARTS_RWEAPON;
			else if (i == PARTS_RWEAPON)
				nPart = PARTS_LWEAPON;
		}

		CItemElem* pItemElem = pUser->m_Inventory.GetAtId(pEquip->dwId[nPart]);
		if (IsUsableItem(pItemElem))
		{
			if (nPart == 0)
			{
				CMover* pEatPet = prj.GetMover(pUser->GetEatPetId());
				if (IsValidObj(pEatPet))
				{
					CAIPet* pAI = static_cast<CAIPet*>(pEatPet->m_pAIInterface);
					if (pAI)
					{
						CItemElem* pItemEatPet = pUser->m_Inventory.GetAtId(pAI->GetPetItemId());
						if (IsUsableItem(pItemEatPet))
						{
							if (pItemEatPet->m_dwObjId != pItemElem->m_dwObjId)
							{
								if (pUser->HasActivatedEatPet())
									pUser->InactivateEatPet();
								pUser->OnDoUseItem(MAKELONG(ITYPE_ITEM, pItemElem->m_dwObjId), pItemElem->m_dwObjId, pItemElem->GetProp()->dwParts);
							}
						}
					}
				}
				else
				{
					pUser->OnDoUseItem(MAKELONG(ITYPE_ITEM, pItemElem->m_dwObjId), pItemElem->m_dwObjId, pItemElem->GetProp()->dwParts);
				}
			}
			else if (nPart == 1)
			{
				CItemElem* pPet = pUser->GetPetItem();
				if (IsUsableItem(pPet))
				{
					if (pPet->m_dwObjId != pItemElem->m_dwObjId)
					{
						if (pUser->HasActivatedSystemPet())
							pUser->PetRelease();
						pUser->OnDoUseItem(MAKELONG(ITYPE_ITEM, pItemElem->m_dwObjId), pItemElem->m_dwObjId, pItemElem->GetProp()->dwParts);
					}
				}
				else
				{
					pUser->OnDoUseItem(MAKELONG(ITYPE_ITEM, pItemElem->m_dwObjId), pItemElem->m_dwObjId, pItemElem->GetProp()->dwParts);
				}
			}
			else
			{
				if (!pUser->m_Inventory.IsEquip(pEquip->dwId[nPart]))
					pUser->DoUseEquipmentItem(pItemElem, pItemElem->m_dwObjId, pItemElem->GetProp()->dwParts);
			}
		}
	}
}
#endif // __SWITCH_EQUIP

#ifdef __AEGON_PVP_RANKING
void CDPSrvr::OnGetPvPRanking(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long) 
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	int nMode;
	ar >> nMode;

	if (GetTickCount() < pUser->m_dwLastPvPRankRefresh + 5000) 
	{
		if (nMode == 1) 
		{
			pUser->AddText("Please wait 5 seconds before refreshing.", COLOR_ERROR);
		}
		return;
	}

	pUser->m_dwLastPvPRankRefresh = GetTickCount();
	pUser->AddPvPRanking();
	
}
#endif // __AEGON_PVP_RANKING

#ifdef __AEGON_PREM_MOTIONS
void CDPSrvr::OnPurchaseMotion(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	
	if (!IsValidObj(pUser))
		return;

	DWORD dwMotionProp;
	ar >> dwMotionProp;
	MotionProp* pMotionProp = prj.GetMotionProp(dwMotionProp);
	if (pMotionProp && pMotionProp->nPriceDP > 0 && pMotionProp->dwUnlockItemID > 0)
	{
		if (!easy_find(pUser->m_vUnlockedMotions, pMotionProp->dwUnlockItemID))
		{
			__DP_COMMAND_INFO buyInfo;
			buyInfo.Init();

			buyInfo.command = INGAMEDP_BUYMOTION;
			buyInfo.dpCost = pMotionProp->nPriceDP;
			buyInfo.nDPCommandVar1 = pMotionProp->dwID;

			g_dpDBClient.SendDPCommand(pUser, buyInfo);
		}
	}
}
#endif // __AEGON_PREM_MOTIONS

#ifdef __AEGON_EMOTE_SYSTEM
void CDPSrvr::OnPurchaseEmote(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (!IsValidObj(pUser))
		return;

	DWORD dwEmoteID;
	ar >> dwEmoteID;
	__AEGON_EMOTE_UNIT* pEmoteUnit = CAegonEmoticonMng::GetInstance()->GetUnitByID(dwEmoteID, 1);
	if (pEmoteUnit && pEmoteUnit->m_dwPriceDP > 0 && pEmoteUnit->m_dwSingleUnlockItem > 0)
	{
		BOOL bOwnedEmote = CAegonEmoticonMng::GetInstance()->PlayerOwnsEmote((CMover*)pUser, pEmoteUnit->nEmoteID, 1);
		if (!bOwnedEmote) 
		{
			__DP_COMMAND_INFO buyInfo;
			buyInfo.Init();

			buyInfo.command = INGAMEDP_BUYEMOTE;
			buyInfo.dpCost = pEmoteUnit->m_dwPriceDP;
			buyInfo.nDPCommandVar1 = pEmoteUnit->nEmoteID;

			g_dpDBClient.SendDPCommand(pUser, buyInfo);
		}
	}
}
#endif //__AEGON_EMOTE_SYSTEM

#ifdef __PET_GLOW
void CDPSrvr::GoToChangePetGlow(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		DWORD m_dwObjId;
		int m_nSelected;

		ar >> m_dwObjId;
		ar >> m_nSelected;

		m_nSelected += 60;
		CMover* pMover = pUser;
		CCtrl* pCtrl = (CCtrl*)pMover;

		CItemElem* pPet = pUser->m_Inventory.GetAtId(m_dwObjId);
		if (pPet)
		{
			int nGold = 10000000;
			if (pUser->GetGold() >= nGold)
			{
				if (pPet->IsVisPet())
				{
					pUser->AddGold(-nGold);
					pUser->UpdateItem((BYTE)pPet->m_dwObjId, UI_CHANGE_PET_GLOW, m_nSelected);
					pUser->AddText("Glow successfully changed!", COLOR_SUCCESS);
					if (pUser->HasActivatedEatPet() && pPet->IsVisPet())
					{
						pUser->AddGold(-nGold);
						CMover* pVisPet = prj.GetMover(pUser->GetEatPetId());
						pPet->m_nPetGlow = m_nSelected;
						DWORD dwPetGlow = pPet->GetPetGlowChange();
						if (pVisPet->m_dwPetGlow != dwPetGlow)
						{
							pVisPet->m_dwPetGlow = dwPetGlow;
							g_UserMng.AddChangePetGlowSfxId(pVisPet);
						}
					}
					else if (pUser->HasActivatedEatPet() && !pPet->IsVisPet())
					{
						pUser->AddText(prj.GetText(TID_PET_GLOW_TEXT_01), COLOR_ERROR);
					}
				}
				else
				{
					pUser->AddText(prj.GetText(TID_PET_GLOW_TEXT_01), COLOR_ERROR);
				}
			}
			else
			{
				pUser->AddDefinedText(TID_GAME_GUILDHOUSE_BUY_EXPENSE);
			}
		}
	}
}
#endif // __PET_GLOW

#ifdef __MODS_MANAGER
void CDPSrvr::OnSendModsManager(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (!pUser->IsAuthHigher(AUTH_GAMEMASTER))
		return;

		BYTE nMode;
		ar >> nMode;
		switch (nMode)
		{
		case 0: pUser->IsMode(MODE_EXPUP_STOP) ? pUser->SetNotMode(MODE_EXPUP_STOP) : pUser->SetMode(MODE_EXPUP_STOP); break;
		case 1: pUser->IsAuthHigher(AUTH_GAMEMASTER) ? (pUser->IsMode(TRANSPARENT_MODE) ? pUser->SetNotMode(TRANSPARENT_MODE) : pUser->SetMode(TRANSPARENT_MODE)) : pUser->SetNotMode(TRANSPARENT_MODE); break;
		case 2:
			if (pUser->IsAuthHigher(AUTH_GAMEMASTER3))
			{
				if (pUser->IsMode(MATCHLESS_MODE))
					pUser->SetNotMode(MATCHLESS_MODE | MATCHLESS2_MODE);
				else
				{
					pUser->SetMode(MATCHLESS_MODE);
					pUser->SetNotMode(MATCHLESS2_MODE);
				}
			}
			else
				pUser->SetNotMode(MATCHLESS_MODE | MATCHLESS2_MODE);
			break;
		case 3:
			if (pUser->IsAuthHigher(AUTH_GAMEMASTER3))
			{
				if (pUser->IsMode(MATCHLESS2_MODE))
					pUser->SetNotMode(MATCHLESS_MODE | MATCHLESS2_MODE);
				else
				{
					pUser->SetMode(MATCHLESS2_MODE);
					pUser->SetNotMode(MATCHLESS_MODE);
				}
			}
			else
				pUser->SetNotMode(MATCHLESS_MODE | MATCHLESS2_MODE);
			break;
		case 4: pUser->IsAuthHigher(AUTH_GAMEMASTER3) ? (pUser->IsMode(ONEKILL_MODE) ? pUser->SetNotMode(ONEKILL_MODE) : pUser->SetMode(ONEKILL_MODE)) : pUser->SetNotMode(ONEKILL_MODE); break;
		case 5: pUser->IsAuthHigher(AUTH_ADMINISTRATOR) ? (pUser->IsMode(FREE_MONEY_MODE) ? pUser->SetNotMode(FREE_MONEY_MODE) : pUser->SetMode(FREE_MONEY_MODE)) : pUser->SetNotMode(FREE_MONEY_MODE);
			pUser->AddGold(-pUser->GetGold());
			break;
		}
		g_UserMng.AddModifyMode(pUser);
	}
}
#endif // __MODS_MANAGER

#ifdef __SYS_WORLDBOSS
void CDPSrvr::OnRegisterHWID(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long) 
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	char HWID[38];

	ar.ReadString(HWID);

	lstrcpy(pUser->HWID, HWID);
}
#endif //__SYS_WORLDBOSS

#ifdef __NPC_QUICK_SELL
void CDPSrvr::OnQuickSell(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
#ifdef __WORLSERVER_MENU
		if (CDPSrvr::GetInstance()->m_bExchange == FALSE && !pUser->IsAuthHigher(AUTH_GAMEMASTER3)) {
			pUser->AddText(prj.GetText(TID_WORLDSRVR_GAME_02), COLOR_ERROR);
			return;
		}
#endif // __WORLSERVER_MENU

		if (pUser->m_vtInfo.GetOther()->IsNPC() == FALSE)
			return;

		if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_TRADE, pUser->GetWorld(), pUser->GetPos()))
			return;

#ifdef __AEGON_IMPROVED_QUICKSELL
		BOOL bUppedItems, bStattedItems, bAwakedItems, bRaretyItems, bElementItems;
		ar >> bUppedItems;
		ar >> bStattedItems;
		ar >> bAwakedItems;
		ar >> bRaretyItems;
		ar >> bElementItems;
#endif

		int vecsize;
		ar >> vecsize;

		for (int i = 0; i < vecsize; i++)
		{
			DWORD objid;
			ar >> objid;
			CItemElem* pItemElem = static_cast<CItemElem*>(pUser->m_Inventory.GetAtId(objid));
			if (IsUsableItem(pItemElem))
			{

				if (pUser->m_Inventory.IsEquip(objid))
				{
					continue;
				}

				if (pUser->IsUsing(pItemElem))
				{
					continue;
				}

#ifdef __AEGON_IMPROVED_QUICKSELL

#ifdef __SYS_LOCK_ITEM
				if (pItemElem->IsItemLocked())
					continue;
#endif //__SYS_LOCK_ITEM

				if (pItemElem->GetAbilityOption() > 0 && !bUppedItems)
					continue;

				if (pItemElem->GetWeaponRarity() > 0 && !bRaretyItems)
					continue;

				__int64 n64RandomOptItemId = pItemElem->GetRandomOptItemId();
				if ((n64RandomOptItemId & 0xFFFFFFFFFFFFFF00) > 0 && !bAwakedItems)
					continue;

				if (pItemElem->GetRandomOpt() > 0 && !bStattedItems)
					continue;

				if (pItemElem->GetItemResist() > SAI79::NO_PROP && pItemElem->GetItemResist() < SAI79::END_PROP && !bElementItems)
					continue;
#else
				if (pItemElem->GetRandomOptItemId() > 0)
					continue;

				if (pItemElem->GetPiercingSize() > 0)
					continue;

				if (pItemElem->GetAttrOption() > 0)
					continue;

#ifdef __SYS_LOCK_ITEM
				if (pItemElem->IsItemLocked())
					continue;
#endif //__SYS_LOCK_ITEM

#endif
				int nGold = (pItemElem->GetCost() / 4);
#ifdef __SHOP_COST_RATE
				nGold = (int)(prj.m_EventLua.GetShopSellFactor() * nGold);
#endif // __SHOP_COST_RATE

				int nNum = pItemElem->m_nItemNum;//1;

				if (nGold == 0)
					nGold = 1;

				int nTax = 0;

				if (CTax::GetInstance()->IsApplyTaxRate(pUser, pItemElem))
				{
					nTax = (int)(nGold * CTax::GetInstance()->GetSalesTaxRate(pUser));
#ifdef __BUYBACK
					pItemElem->setBuyBackCont(CTax::GetInstance()->GetContinent(pUser));
					pItemElem->setBuyBackTaxes(nTax);
#endif // __BUYBACK
				}
				nGold -= nTax;
#ifdef __BUYBACK
				pItemElem->setBuyBackPrice(nGold + ceil(nGold * (double)((double)__BUYBACK_PERCENT_COST / 100.0f)));
#endif // __BUYBACK

				if (pItemElem->IsBinds())
					nGold = 1;

				float fTmpGold = (float)(pUser->GetGold() + nGold);

				if (fTmpGold >= INT_MAX)
				{
#ifdef __AEGON_IMPROVED_QUICKSELL
					if (!pUser->ConvertSinglePerin()) 
					{
						pUser->AddText("You have too much penya in your inventory.", COLOR_WARNING);
						return;
					}
#else
					int nCount = pUser->m_Inventory.GetAtItemNum(II_SYS_SYS_SCR_PERIN);
					if (pUser->m_Inventory.GetEmptyCount() > 0 || nCount % 9999 != 0)
					{
						{
							CItemElem item;
							item.m_nItemNum = 1;
							item.m_dwItemId = II_SYS_SYS_SCR_PERIN;
							item.SetSerialNumber();

							if (pUser->CreateItem(&item))
							{
								pUser->AddGold(-PERIN_VALUE);
							}
							else
								return;
						}
					}
					else
					{
						pUser->AddText("You have too much penya in your inventory.", COLOR_WARNING);
						return;
					}
#endif
				}

				LogItemInfo aLogItem;
				aLogItem.Action = "Q";
				aLogItem.SendName = pUser->GetName();
				aLogItem.RecvName = pUser->m_vtInfo.GetOther()->GetName();
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = pUser->GetGold();
#if __VER >= 12 // __TAX
				aLogItem.Gold2 = pUser->GetGold() + nGold;
#else // __TAX
				aLogItem.Gold2 = pUser->GetGold() + (pItemElem->GetCost() / 4) * nNum;
#endif // __TAX
				OnLogItem(aLogItem, pItemElem, nNum);
				int nCost = (int)pItemElem->GetCost() / 4;

				if (nCost < 1)
					nCost = 1;

#ifdef __GAMEGUARD
				__int64 lEntireCost = nNum * (__int64)nCost;
				if (lEntireCost > INT_MAX)
					continue;
#endif // __GAMEGUARD

#if __VER >= 12 // __TAX
				if (nGold < 1)
					nGold = 1;

				pUser->AddGold((int)lEntireCost);
				if (nTax)
					CTax::GetInstance()->AddTax(CTax::GetInstance()->GetContinent(pUser), nTax, TAX_SALES);
#else // __TAX
				pUser->AddGold(nCost * nNum);
#endif // __TAX

#ifdef __BUYBACK
				if (pUser->m_BuyBack.GetCount() >= pUser->m_BuyBack.GetMax())
				{
					pUser->m_BuyBack.RemoveAtId(0);
					Reorganize(&pUser->m_BuyBack);
					pUser->BuyBackRemoveItem(0);
				}
				CItemElem itemElem = *pItemElem;
				itemElem.m_nItemNum = nNum;

				pUser->m_BuyBack.Add(&itemElem);
				pUser->SendBuyBack(&itemElem);
#endif // __BUYBACK

				pUser->RemoveItem(objid, nNum);
			}
		}

#ifdef __GAMEGUARD
		if (pUser->GetWorld())
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
#endif // __GAMEGUARD
	}
}
#endif // __NPC_QUICK_SELL

#ifdef __WORLSERVER_MENU
CDPSrvr* CDPSrvr::GetInstance(void)
{
	static CDPSrvr sCDPSrvr;
	return &sCDPSrvr;
}

void CDPSrvr::SendExchangeSetApply(DPID dpId)
{
	BEFORESENDDUAL(ar, PACKETTYPE_EXCHANGE_SETAPPLY, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << m_bExchange;
	SEND(ar, this, dpId);
}

void CDPSrvr::SendChatSetApply(DPID dpId)
{
	BEFORESENDDUAL(ar, PACKETTYPE_CHAT_SETAPPLY, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << m_bChat;
	SEND(ar, this, dpId);
}

void CDPSrvr::SendPkSetApply(DPID dpId)
{
	BEFORESENDDUAL(ar, PACKETTYPE_PK_SETAPPLY, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << m_bPk;
	SEND(ar, this, dpId);
}
#endif // __WORLSERVER_MENU

#ifdef __CRYSTAL
void CDPSrvr::OnCrystal(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsInvalidObj(pUser))
	{
		CItemElem* pDest, * pSrc;
		DWORD dwIdSrc, dwIdDest;
		ar >> dwIdSrc >> dwIdDest;

		pSrc = pUser->m_Inventory.GetAtId(dwIdSrc);
		pDest = pUser->m_Inventory.GetAtId(dwIdDest);

		if (pSrc == NULL || pDest == NULL || !pSrc || !pDest)
			return;

		if (!IsUsableItem(pSrc) || !IsUsableItem(pDest))
		{
			pUser->AddText("Item are in use");
			return;
		}

		if (pUser->m_Inventory.IsEquip(pSrc->m_dwObjId) || pUser->m_Inventory.IsEquip(pDest->m_dwObjId))
		{
			pUser->AddDefinedText(TID_GAME_EQUIPPUT);
			return;
		}

		pDest->SetCrystal(pSrc->m_dwItemId);
		pUser->RemoveItem((BYTE)pSrc->m_dwObjId, 1);
		pUser->AddCrystal(pDest->m_dwObjId, pDest->GetCrystal());

		pUser->AddText("Crystal added on your weapon!");
	}
}
#endif // __CRYSTAL

#ifdef __AEGON_INGAME_SHOP
void CDPSrvr::OnWebshopPurchase(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (!IsValidObj(pUser))
		return;

	int nAmount;
	DWORD dwUnitID;
	ar >> dwUnitID;
	ar >> nAmount;

	if (nAmount <= 0)
		return;

	__AEGIGNSHP_ITEM_UNIT* pShopUnit = CAegonIngameShopMng::GetInstance()->GetShopUnitByID(dwUnitID);
	if (pShopUnit && pShopUnit->m_nPrice > 0 && pShopUnit->m_vCommandUnits.size() > 0 && pShopUnit->m_bIsEnabled)
	{
		__DP_COMMAND_INFO buyInfo;
		buyInfo.Init();

		buyInfo.command = INGAMEDP_WEBSHOP;
		buyInfo.dpCost = pShopUnit->m_nPrice;
		buyInfo.nDPCommandVar1 = pShopUnit->m_dwItemUnitID;
		buyInfo.dwAmount = (DWORD)nAmount;

		g_dpDBClient.SendDPCommand(pUser, buyInfo);
	}
}
#endif

#ifdef __AEGON_TOURNAMENT
void CDPSrvr::OnCreateTournament(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser) || !pUser->IsAuthHigher(AUTH_ADMINISTRATOR))
		return;

	__TOURNAMENT_UNIT tmpUnit;
	tmpUnit.Read(ar);
	tmpUnit.m_dwTournamentID = CAegonTournamentMng::GetInstance()->GetNextID();
	tm tmp_tm;
	memcpy(&tmp_tm, &tmpUnit.m_sStartTime, sizeof(tm));
	time_t start_time = mktime(&tmp_tm);
	if (tmp_tm.tm_isdst) 
	{
		tmpUnit.m_sStartTime.tm_isdst = 1;
		start_time = mktime(&tmpUnit.m_sStartTime);
	}
	if (start_time < time_null()) 
	{
		pUser->AddText("failed to create tournament. past time", COLOR_ERROR);
		return;
	}

	if (!tmpUnit.IsValid()) 
	{
		pUser->AddText("failed to create tournament.", COLOR_ERROR);
		return;
	}
	CAegonTournamentMng::GetInstance()->m_mQueuedTournaments.insert(make_pair(tmpUnit.m_dwTournamentID, tmpUnit));
	CAegonTournamentMng::GetInstance()->OnTournamentCreation();
	pUser->AddText("tournament created successfully.", COLOR_SUCCESS);
}
void CDPSrvr::OnGetTournamentMap(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser) || !pUser->IsAuthHigher(AUTH_ADMINISTRATOR))
		return;

	pUser->AddTournamentMap();
}
void CDPSrvr::OnDeleteTournament(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser) || !pUser->IsAuthHigher(AUTH_ADMINISTRATOR))
		return;

	DWORD dwTournamentID;
	ar >> dwTournamentID;

	CAegonTournamentMng::GetInstance()->DeleteTournament(dwTournamentID, pUser);
	
}
#endif

#ifdef __ROYAL_RUMBLE
void CDPSrvr::OnPlayerSiegeJoin(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		BOOL bNew, bWarJoin;
		ar >> bNew;
		ar >> bWarJoin;
		if (bWarJoin)
		{
			if (g_War.GetState() != -1)
			{
				if (g_War.IsSiegeUser(pUser->m_idPlayer))
				{
					WAR_PLAYER sMember = g_War.GetUser(pUser->m_idPlayer);
					if (sMember.m_idPlayer > 0)
					{
						g_War.CheckUsers(pUser->m_idPlayer, TRUE);
#ifdef __RUMBLE_SPAWNFIX
						pUser->Replace(g_uIdofMulti, WI_WORLD_FWC, D3DXVECTOR3(1204.320f, 105.000f, 1370.583f), REPLACE_NORMAL, nDefaultLayer);
#else //__RUMBLE_SPAWNFIX
						pUser->Replace(g_uIdofMulti, WI_WORLD_FWC, D3DXVECTOR3(1761.0f, 100.0f, 1837.0f), REPLACE_NORMAL, nDefaultLayer);
#endif //__RUMBLE_SPAWNFIX
						sMember.m_dwRespawnTime = ::GetCurrentTime() + SEC(g_War.WARINFO.m_nRespawnTime);
						g_War.UpdateUser(pUser->m_idPlayer, sMember);
						g_UserMng.AddClientUpdate(pUser->m_idPlayer);
						return;
					}
				}
				else
				{
					pUser->AddText(prj.GetText(TID_PLAYER_WAR_02), 0xFFFF0000);
					return;
				}
			}
			else
			{
				pUser->AddText(prj.GetText(TID_PLAYER_WAR_03), 0xFFFF0000);
				return;
			}
		}

		if (bNew)
		{
			if (g_War.GetState() == -1)
			{
#ifndef __RUMBLE_FREE_SIGNUP
				if (pUser->GetGold() >= __ROYAL_RUMBLE)
				{
					pUser->AddGold(-(__ROYAL_RUMBLE));
#endif //__RUMBLE_FREE_SIGNUP
					g_War.CheckUsers(pUser->m_idPlayer, TRUE);
					int signup = g_War.NewUser(pUser->m_idPlayer);
					switch (signup)
					{
					case 0: // Signup successful
					{
						pUser->AddWarTime(g_War.m_dwWaitTime - GetTickCount(), g_War.GetState());
						g_UserMng.AddClientUpdate(pUser->m_idPlayer);

						pUser->AddText(prj.GetText(TID_PLAYER_WAR_04), COLOR_SUCCESS);
					}
					break;
					case -1: // User already signed up
					{
						pUser->AddText(prj.GetText(TID_PLAYER_WAR_13), COLOR_ERROR);
					}
					break;
					case -2: // User is not correct level
					{
						pUser->AddDefinedText(TID_PLAYER_WAR_38, "%d", pUser->GetLevel());
						pUser->AddDefinedText(TID_PLAYER_WAR_39, "%d~%d", g_War.WARINFO.m_nMinPlayerLevel, g_War.WARINFO.m_nMaxPlayerLevel);
					}
					break;
					case -3: // Rumble is full
					{
						pUser->AddText(prj.GetText(TID_PLAYER_WAR_40), COLOR_ERROR);
					}
					break;
					default:
					{
						pUser->AddText(prj.GetText(TID_PLAYER_WAR_41), COLOR_ERROR);
						Error("CDPSrvr::OnPlayerSiegenJoin - Wrong signup result (%d)", signup);
					}
					break;
					}
#ifndef __RUMBLE_FREE_SIGNUP
				}
				else
					pUser->AddText(prj.GetText(TID_PLAYER_WAR_01));
#endif //__RUMBLE_FREE_SIGNUP
			}
			else
				pUser->AddText(prj.GetText(TID_PLAYER_WAR_05), 0xFFFF0000);
		}
		else
		{
			if (!bNew && pUser->IsSiegeUser())
				g_War.RejoinSiegeFight(pUser);
		}
	}
}

void CDPSrvr::OnGetSiegeRanking(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	BOOL bFightRank;
	if (IsValidObj(pUser))
	{
		ar >> bFightRank;
		if (bFightRank != FALSE)
			pUser->AddRankingList();
		else
			g_UserMng.AddClientUpdate(pUser->m_idPlayer);
	}
}
#endif // __ROYAL_RUMBLE


#ifdef __AEGON_AUTO_STATS
void CDPSrvr::OnUpdateAutoStats(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	int nSelectedAutoStat;
	ar >> nSelectedAutoStat;

	if (nSelectedAutoStat < 0 || nSelectedAutoStat > 4)
		return;

	pUser->m_nAutoStat = nSelectedAutoStat;
	pUser->AddAutoStatRefresh();
}
#endif

#ifdef __AEGON_F_TOOL
void CDPSrvr::OnUpdateFTools(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	pUser->m_vFToolUnits.clear();
	unsigned long nSize;
	ar >> nSize;
	if (nSize > MAX_F_TOOL_UNITS) 
	{
		pUser->AddText("Maximum F-Tool Units exceeded. List cleared.");
		pUser->m_vFToolUnits.clear();
		
		return;
	}
	for (int i = 0; i < nSize; i++) 
	{
		__AEGON_F_TOOL_UNIT tmpUnit;
		tmpUnit.Read(ar);
		tmpUnit.TimeCheck();
		if (tmpUnit.IsValid() && pUser->m_vFToolUnits.size() <= MAX_F_TOOL_UNITS) 
		{
			pUser->m_vFToolUnits.push_back(tmpUnit);
		}

	}
	pUser->AddFToolRefresh();
}
#endif

#ifdef __REMOVE_SKILL_BUFF
void CDPSrvr::OnSendRemoveSkillBuff(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		DWORD dwSkillBuff;
		ar >> dwSkillBuff;
		ItemProp* pSkillProp = prj.GetSkillProp(dwSkillBuff);
		if (pSkillProp && (pSkillProp->nEvildoing >= 1 || pUser->IsAuthHigher(AUTH_ADMINISTRATOR)) && pUser->HasBuff(BUFF_SKILL, (WORD)(pSkillProp->dwID)))
		{
			pUser->RemoveBuff(BUFF_SKILL, (WORD)(pSkillProp->dwID));
		}
	}
}
#endif // __REMOVE_SKILL_BUFF

#ifdef __AEGON_TIME_DIFF
void CDPSrvr::OnUpdateTimeDiff(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		pUser->AddUpdateTimeDiff();
	}
}
#endif //__AEGON_TIME_DIFF

#ifdef __FL_BAG_LOOT
void CDPSrvr::OnSelectedBag(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	int nSelect;
	ar >> nSelect;

	if (nSelect >= 0 && nSelect <= 3)
		((CMover*)pUser)->m_nSelectedBag = nSelect;
}
#endif //__FL_BAG_LOOT

#ifdef __FL_BAG_UPDATE
void CDPSrvr::OnDoUseItemBag(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwData;
	OBJID objid;
	int nBag;

	ar >> dwData >> objid >> nBag;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

	if (nBag != 0 && nBag != 1 && nBag != 2)
		return;

	if (dwData < 0)
		return;

	pUser->OnDoUseItemBag(dwData, objid, nBag);
}
#endif //__FL_BAG_UPDATE

#ifdef __AEGON_PVP_MINIGAMES
void CDPSrvr::OnPvPMinigameCreate(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	int nGameID;
	ar >> nGameID;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	LPCTSTR  szUserName = pUser->GetName();

	if (IsInvalidObj(pUser))
		return;

	CWorld* pWorld = pUser->GetWorld();

	if (!pWorld)
		return;

	if (CAegonPvPMinigamesMng::GetInstance()->IsPlaying(pUser->m_idPlayer)) 
	{
		pUser->AddPvPMinigameRefresh();
		return;	
	}

	if (g_GuildCombat1to1Mng.IsPossibleUser(pUser) || pWorld->GetID() == WI_WORLD_GUILDWAR || pUser->m_nDuel > 0 || pUser->IsAttackMode())
	{
		pUser->AddText(_T("You cannot do this now."), COLOR_ERROR);
		return;
	}

	CAegonPvPMinigamesMng::GetInstance()->StartMinigame(pUser, nGameID);
}

void CDPSrvr::OnPvPMinigameCommand(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser))
		return;

	char szName[MAX_NAME] = {0, };
	__MINIGAME_COMMAND sCommand;
	ar >> sCommand;

	if (sCommand.dwCommandID == AEG_MINIGAME_COMMAND_INVITE)
	{
		ar.ReadString(szName, MAX_NAME);
	}

	if (!sCommand.IsValid())
		return;

	CAegonPvPMinigamesMng::GetInstance()->OnPlayerCommand(pUser, sCommand, szName);
}
void CDPSrvr::OnPvPMinigameChat(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser))
		return;

	char szMessage[1024];
	ar.ReadString(szMessage, 1024);

	if (strlen(szMessage) <= 0)
		return;

	CAegonPvPMinigamesMng::GetInstance()->OnPlayerChat(pUser, szMessage);
}
void CDPSrvr::OnPvPMinigameGetList(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser))
		return;

	pUser->AddPvPMinigameList();
}
#endif

#ifdef __AEGON_MESSENGER
void CDPSrvr::OnMessengerUpdateRequest(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser))
		return;

	unsigned long nSize;
	ar >> nSize;

	vector<int> vStatus;
	int nStatus = 1;

	char szName[MAX_PLAYER];
	for (int i = 0; i < nSize; i++) 
	{
		ar.ReadString(szName, MAX_PLAYER);

		CUser* pUserChat = g_UserMng.GetUserByName(szName);
		if (pUserChat) 
		{
			if (pUserChat->m_dwAFKState == 3) 
			{
				nStatus = 2; //afk
			}
			else
				nStatus = 0; //online
		}
		else 
		{
			nStatus = 1; //offline
		}
		vStatus.push_back(nStatus);
	}
	pUser->AddMessengerUpdate(&vStatus);
}
void CDPSrvr::OnMessengerAddChatUnit(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser))
		return;

	char szName[MAX_PLAYER];
	ar.ReadString(szName, MAX_PLAYER);

	pUser->AddMessengerChatUnit(CPlayerDataCenter::GetInstance()->GetPlayerId(szName) > 0 ? TRUE : FALSE, szName);
}
#endif

#ifdef __AEGON_BAG_FREE_MOVE
void CDPSrvr::OnMovePocketItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

	int nPocket, nSrcIndex, nDstIndex;
	ar >> nPocket >> nSrcIndex >> nDstIndex;

	if (nSrcIndex == nDstIndex)
		return;

	if (nPocket < 0 || nPocket >= MAX_POCKET)
		return;

	if (nPocket == 0 && (nDstIndex >= POCKET_SLOT_SIZE_0 || nSrcIndex >= POCKET_SLOT_SIZE_0))
		return;
	else if (nPocket > 0 && (nDstIndex >= POCKET_SLOT_SIZE_1_2 || nSrcIndex >= POCKET_SLOT_SIZE_1_2))
		return;

	if (nDstIndex < 0 || nSrcIndex < 0)
		return;

	if (!pUser->m_Pocket.IsAvailable(nPocket, TRUE))
		return;

	CPocket* pPocket = pUser->m_Pocket.GetPocket(nPocket);
	if (!pPocket || pPocket->IsExpired())
		return;
	
	CItemElem* pItemSrc = pPocket->GetAt(nSrcIndex);
	CItemElem* pItemDst = pPocket->GetAt(nDstIndex);

	if (pItemDst == NULL || IsUsableItem(pItemDst)) // 빈 공간 or 거래중이지 않는 아이템 ?			
	{
		if (IsUsableItem(pItemSrc))					// 거래중이지 않는 아이템 ?
		{
			pPocket->Swap(nSrcIndex, nDstIndex);
			pUser->AddMovePocketItem(nPocket, nSrcIndex, nDstIndex);
		}
	}
}
#endif