#include "StdAfx.h"
#include "defineObj.h"
#include "dbmanager.h"
#include "dploginsrvr.h"
#include "dpcoresrvr.h"
#include "dptrans.h"
#include "..\_Network\Objects\Obj.h"
#include "misc.h"
#include "mytrace.h"

#include <sstream>

#include "party.h"
#include "guild.h"
extern	CGuildMng	g_GuildMng;
#include "guildwar.h"
extern	CGuildWarMng	g_GuildWarMng;

extern const int MAX_TASKBAR; 
extern const int MAX_APPLETTASKBAR; 
extern const int MAX_ITEMTASKBAR;   
extern const char NullStr[2];

#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	#include "honor.h"
#endif	// __HONORABLE_TITLE			// 달인

#include "eveschool.h"

#if defined( __VERIFY_PLAYER ) || defined( __PROVIDE ) || defined( __S0707_ITEM_CONV ) || defined(__RECOVER0816)
#define	MAX_QUERY_SIZE	1024 * 64
#include "resource.h"
BOOL	CALLBACK	VerifyPlayerDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam );
#endif	// __VERIFY_PLAYER OR __PROVIDE

extern  CDPCoreSrvr			g_dpCoreSrvr;
extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CProject			prj;
extern	APP_INFO			g_appInfo;
extern	CMyTrace			g_MyTrace;

#define	VERIFYSTRING( lpString, lpszPlayer )	\
	if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszPlayer, lpDbOverlappedPlus ) )		return;

#define	VERIFY_GUILD_STRING( lpString, lpszGuild )	\
	if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszGuild ) )		return;

void CDbManager::SavePlayer( CQuery *qry, CQuery* pQueryLog, CMover* pMover, char* szQuery )
{
	szQuery[0] = '\0';

#ifdef __AEGON_FIXES
	if (!pMover || pMover->m_idPlayer == 0)
	{
		Error("CDbManager::SavePlayer m_idPlayer == 0");
		return;
	}
#endif

	///////// actMover
	char cActMover[50] = {0,};
	sprintf(cActMover, "%d,%d", pMover->m_pActMover->m_dwState, pMover->m_pActMover->m_dwStateFlag);

	///////// Skill
	char JobSkill[1024] = {0,};
	char LicenseSkill[1024] = {0,};			
	
	///////// JobLv
	char JobLv[500] = {0,};
	SaveJobLv( pMover, JobLv );
	
	///////// Quest
	char QuestCnt[3072]	= { 0, };
	char m_aCompleteQuest[1024] = { 0, };
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	char CheckedQuest[100] = {0, };
	SaveQuest( pMover, QuestCnt, m_aCompleteQuest, CheckedQuest );
#else // __IMPROVE_QUEST_INTERFACE
	SaveQuest( pMover, QuestCnt, m_aCompleteQuest );
#endif // __IMPROVE_QUEST_INTERFACE

	///////// Inventory
	ItemContainerStruct	icsInventory, icsBank[3];
	SaveInventory( pMover, &icsInventory );
	for( int k = 0 ; k < 3 ; ++k )
		SaveBank( pMover, &pMover->m_Bank[k], &icsBank[k] );

	///////// Equipment
	char Equipment[500] = {0,};
	SaveEquipment( pMover, Equipment );

	///////// CardCube	
	char Card[2]	= "$";
	char sCardIndex[2]	= "$";
	char sCardObjIndex[2]	= "$";
	char Cube[2]	= "$";
	char sCubeIndex[2]	= "$";
	char sCubeObjIndex[2]	= "$";
	
	///////// TaskBar
	char AppletTaskBar[MAX_APPLETTASKBAR] = { 0, }; 
	char ItemTaskBar[MAX_ITEMTASKBAR] = { 0, };
#ifdef __ACTION_SLOT_PAGES
	char SkillTaskBar[4096] = { 0, };
#else // __ACTION_SLOT_PAGES
	char SkillTaskBar[1024] = { 0, };
#endif // __ACTION_SLOT_PAGES
	SaveTaskBar( pMover, AppletTaskBar, ItemTaskBar, SkillTaskBar );

	///////// SM 상용화 아이템
	char szSMTime[3072] = { 0, };
	SaveSMCode( pMover, szSMTime );

	char szSkillInfluence[7500] = { 0, };
	SaveSkillInfluence( pMover, szSkillInfluence );

	PutExpLog( pQueryLog, pMover, ( pMover->m_dwPeriodicTick == 0 ? 'O': 'P' ) );

	//	mulcom	100218	유럽 페냐 관련 CHARACTER_STR U1 변경 ( U1 -> C1 )
	sprintf( szQuery, "{call CHARACTER_STR('U1','%07d','%02d','',"
					  "?,?,?,?,?,?,?,?,?,?,?,"		// 1
					  "?,?,?,?,?,?,?,?,?,?,?,"		// 2
					  "?,?,?,?,?,?,?,?,?,?,?,"		// 3
					  "?,?,?,?,?,?,?,?,?,?,?,"		// 4
					  "?,?,?,?,?,?,?,?,?,?,?,"		// 5
					  "?,?,?,?,?,?,?,?,?,?,?,"		// 6
					  "?,?,?,?,?,?,?,?,?,?,?,"		// 7
//					  "?,?,?,?,?,?,?,?,?,?,?,"		// 8
					  "?,?,?,?,?,?,?,?,?,%7.0f,%7.0f,"		// 8
	#ifdef __EVENT_1101
//					  "?,?,?,?,?,?"
						"%7.0f,?,?,?,?,?"
	#else	// __EVENT_1101
					  "?,?,?"
	#endif	// __EVENT_1101
#if __VER >= 8 // __S8_PK
					  ",?"
#endif // __VER >= 8 // __S8_PK
#if __VER >= 8 // __CSC_VER8_5
					  ",?,?"
#endif // __CSC_VER8_5
					  ",?,?,?"
#ifdef __EXP_ANGELEXP_LOG
					  ",?,?"
#endif // __EXP_ANGELEXP_LOG
#ifdef __EVENTLUA_COUPON
					  ",?"
#endif // __EVENTLUA_COUPON
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
					  ",?"
#endif	// __HONORABLE_TITLE			// 달인
#ifdef __LAYER_1015
					  ",?"
#endif	// __LAYER_1015
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
					  ",?"
#endif // __IMPROVE_QUEST_INTERFACE
#if __VER >= 15 // __CAMPUS
						",?,?"
#endif // __CAMPUS

#ifdef __VENDING_NPC
		",?"
#endif // __VENDING_NPC

#ifdef __HIDE_FASHION
		",?"
#endif // __HIDE_FASHION

#ifdef __SYS_CUSTOM_TITLE
		",?"
#endif // __SYS_CUSTOM_TITLE

#ifdef __AEGON_LOOT_BOX
		",?"
		",?"
#endif // __AEGON_LOOT_BOX

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
		",?"
#endif // __AEGON_ACHIEVEMENT_SYSTEM

#ifdef __SYS_NAMECOLOR
		",?,?,?"
#endif //__SYS_NAMECOLOR

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
		",?"
#endif // __AEGON_ACHIEVEMENT_SYSTEM

#ifdef __AEGON_LAST_ONLINE
		",?"
#endif // __AEGON_LAST_ONLINE

#ifdef __AEGON_INGAME_TIME
		",?"
#endif // __AEGON_INGAME_TIME

#ifdef __AEGON_CRAFTING_SYSTEM
		",?"
#endif // __AEGON_CRAFTING_SYSTEM

#ifdef __AEGON_DELETED_CHAR_CHECK
		",?"
#endif //__AEGON_DELETED_CHAR_CHECK

#ifdef __AEGON_AUTO_STATS
		",?"
#endif //__AEGON_AUTO_STATS

#ifdef __SYS_REBIRTH
		",?"
#endif //__SYS_REBIRTH

#ifdef __FL_BAG_LOOT
		",?"
#endif //__FL_BAG_LOOT

#ifdef __AEGON_DB_PETFILTER
		",?"
		",?"
#endif

		")}", pMover->m_idPlayer, g_appInfo.dwSys, pMover->m_vReturnPos.x, pMover->m_vReturnPos.y, pMover->m_vReturnPos.z );		// +3

	char szAuthority[2] = "F";
	int nSlot = 0;
	int nSex = pMover->GetSex();
	int nExpertLv = 0;
	int nRemainLP = 0;
	int nTotalPlayTime = 0;
	DWORD dwGold = pMover->GetGold();
	int i=0;
	int j=-1;
	__int64 nExp2 = 0;
	__int64 nSkillExp = 0;
	DWORD dwSkillPoint = 0;

//////////////////////////////////////////////////////////////////////////////////////
int MAX_SAVEPARAM = 88;
	MAX_SAVEPARAM += 3;
#ifdef __EVENT_1101
		MAX_SAVEPARAM += 3;
#endif // __EVENT_1101
#if __VER >= 8 // __S8_PK
	MAX_SAVEPARAM += 1;
#endif // __VER >= 8 // __S8_PK
#if __VER >= 8 // __CSC_VER8_5
	MAX_SAVEPARAM += 2;
#endif // __CSC_VER8_5

	MAX_SAVEPARAM += 3;

#ifdef __EXP_ANGELEXP_LOG
	MAX_SAVEPARAM += 2;
#endif // __EXP_ANGELEXP_LOG

#ifdef __EVENTLUA_COUPON
	MAX_SAVEPARAM += 1;
#endif // __EVENTLUA_COUPON

#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	MAX_SAVEPARAM += 1;
#endif	// __HONORABLE_TITLE			// 달인
#ifdef __LAYER_1015
	MAX_SAVEPARAM	+= 1;	// m_nLayer
#endif	// __LAYER_1015

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	MAX_SAVEPARAM	+= 1;	// m_aCheckedQuest
#endif // __IMPROVE_QUEST_INTERFACE

#if __VER >= 15 // __CAMPUS
	MAX_SAVEPARAM	+= 2;	// m_nCampusPoint, m_idCampus
#endif // __CAMPUS

#ifdef __VENDING_NPC
	MAX_SAVEPARAM += 1;
#endif // __VENDING_NPC

#ifdef __HIDE_FASHION
	MAX_SAVEPARAM += 1;
#endif // __HIDE_FASHION

#ifdef __SYS_CUSTOM_TITLE
	MAX_SAVEPARAM += 1;
#endif // __SYS_CUSTOM_TITLE

#ifdef __AEGON_LOOT_BOX
	MAX_SAVEPARAM += 2;
#endif // __AEGON_LOOT_BOX

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
	MAX_SAVEPARAM += 1;
#endif // __AEGON_ACHIEVEMENT_SYSTEM

#ifdef __SYS_NAMECOLOR
	MAX_SAVEPARAM += 3;
#endif //__SYS_NAMECOLOR

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
	MAX_SAVEPARAM += 1;
#endif // __AEGON_ACHIEVEMENT_SYSTEM

#ifdef __AEGON_LAST_ONLINE
	MAX_SAVEPARAM += 1;
#endif // __AEGON_LAST_ONLINE

#ifdef __AEGON_INGAME_TIME
	MAX_SAVEPARAM += 1;
#endif // __AEGON_INGAME_TIME

#ifdef __AEGON_CRAFTING_SYSTEM
	MAX_SAVEPARAM += 1;
#endif // __AEGON_CRAFTING_SYSTEM

#ifdef __AEGON_DELETED_CHAR_CHECK
	MAX_SAVEPARAM += 1;
#endif //__AEGON_DELETED_CHAR_CHECK

#ifdef __AEGON_AUTO_STATS
	MAX_SAVEPARAM += 1;
#endif //__AEGON_AUTO_STATS

#ifdef __SYS_REBIRTH
	MAX_SAVEPARAM += 1;
#endif //__SYS_REBIRTH

#ifdef __FL_BAG_LOOT
	MAX_SAVEPARAM += 1;
#endif //__FL_BAG_LOOT

#ifdef __AEGON_DB_PETFILTER
	MAX_SAVEPARAM += 2;
#endif

//////////////////////////////////////////////////////////////////////////////////////
	
#if __VER >= 8 // __S8_PK
	int nNumSkill = 0;
	int nSlaughter = 0;
#endif // __VER >= 8 // __S8_PK
#ifdef _WIN64
	SQLLEN cbLen = SQL_NTS;
#else
	SQLINTEGER cbLen = SQL_NTS;
#endif
	BOOL* bOK = new BOOL[MAX_SAVEPARAM];
//	BOOL bOK[MAX_SAVEPARAM];
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  32, 0, pMover->m_szName, 0, &cbLen );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &nSlot, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_dwWorldID, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_dwIndex, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL,     0, 0, &pMover->m_vPos.x, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL,     0, 0, &pMover->m_vPos.y, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL,     0, 0, &pMover->m_vPos.z, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  32, 0, pMover->m_szCharacterKey, 0, &cbLen );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_dwSkinSet, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_dwHairMesh, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_dwHairColor, 0, 0 );
	// 1
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_dwHeadMesh, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &nSex, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL,     0, 0, &pMover->m_vScale.x, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_dwMotion, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL,     0, 0, &pMover->m_fAngle, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nHitPoint, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nManaPoint, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nFatiguePoint, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_dwRideItemIdx, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &dwGold, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nJob, 0, 0 );
	// 2
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  50, 0, cActMover, 0, &cbLen );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nStr, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nSta, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nDex, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nInt, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nLevel, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, 0, 0, &pMover->m_nExp1, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, 0, 0, &nExp2, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,   500, 0, JobSkill, 0, &cbLen );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,   500, 0, LicenseSkill, 0, &cbLen );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,   500, 0, JobLv, 0, &cbLen );
	// 3
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &nExpertLv, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_idMarkingWorld, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL,     0, 0, &pMover->m_vMarkingPos.x, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL,     0, 0, &pMover->m_vMarkingPos.y, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL,     0, 0, &pMover->m_vMarkingPos.z, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nRemainGP, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &nRemainLP, 0, 0 );
#if __VER >= 12 // __MOD_TUTORIAL
	int nTutorialState	= pMover->GetTutorialState();
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &nTutorialState, 0, 0 );
#else	// __MOD_TUTORIAL
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nFlightLv, 0, 0 );
#endif	// __MOD_TUTORIAL
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nFxp, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nFxp, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  3072, 0, QuestCnt, 0, &cbLen );
	// 4
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_CHAR,     1, 0, szAuthority, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_dwMode, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_idparty, 0, 0 );
#if __VER < 8 // __S8_PK
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nNumKill, 0, 0 );
#endif // __VER < 8 // __S8_PK
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_idMurderer, 0, 0 );
#if __VER < 8 // __S8_PK
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nSlaughter, 0, 0 );
#endif // __VER < 8 // __S8_PK
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nFame, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, 0, 0, &pMover->m_nDeathExp, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nDeathLevel, 0, 0 );
#ifdef __JEFF_9_20
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_dwMute, 0, 0 );
#else	// __JEFF_9_20
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_dwFlyTime, 0, 0 );
#endif	// __JEFF_9_20
#ifdef __RT_1025
	DWORD dwState	= pMover->m_RTMessenger.GetState();
#ifdef __SECURITY_FIXES
	if (dwState < FRS_ONLINE || dwState > MAX_FRIENDSTAT)
		dwState = FRS_OFFLINE;
#endif // __SECURITY_FIXES
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &dwState, 0, 0 );
#else	// __RT_1025
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_Messenger.m_dwMyState, 0, 0 );
#endif	// __RT_1025
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &nTotalPlayTime, 0, 0 );
#if __VER >= 8 // __S8_PK
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nPKValue, 0, 0 );
	// 5
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_dwPKPropensity, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_dwPKExp, 0, 0 );
#endif // __VER >= 8 // __S8_PK
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  1980, 0, Card, 0, &cbLen );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  215,  0, sCardIndex, 0, &cbLen );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  215,  0, sCardObjIndex, 0, &cbLen );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  1980, 0, Cube, 0, &cbLen );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  215,  0, sCubeIndex, 0, &cbLen );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  215,  0, sCubeObjIndex, 0, &cbLen );
#ifdef __INVENTORY_168
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, icsInventory.szItem, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 1000, 0, icsInventory.szIndex, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 135, 0, Equipment, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 1000, 0, icsInventory.szObjIndex, 0, &cbLen);
#else //__INVENTORY_168
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  6940, 0, icsInventory.szItem, 0, &cbLen );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  345 , 0, icsInventory.szIndex, 0, &cbLen );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  135 , 0, Equipment, 0, &cbLen );
	// 6
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  345 , 0, icsInventory.szObjIndex, 0, &cbLen );
#endif //__INVENTORY_168
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  3100, 0, AppletTaskBar, 0, &cbLen );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  6885, 0, ItemTaskBar, 0, &cbLen );
#ifdef __ACTION_SLOT_PAGES
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 4096, 0, SkillTaskBar, 0, &cbLen);
#else // __ACTION_SLOT_PAGES
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  225, 0, SkillTaskBar, 0, &cbLen );
#endif // __ACTION_SLOT_PAGES
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_SHORT, SQL_SMALLINT, 0, 0, &pMover->m_UserTaskBar.m_nActionPoint, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  4290, 0, icsBank[pMover->m_nSlot].szItem, 0, &cbLen );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  215, 0, icsBank[pMover->m_nSlot].szIndex, 0, &cbLen );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  215, 0, icsBank[pMover->m_nSlot].szObjIndex, 0, &cbLen );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_dwGoldBank[pMover->m_nSlot], 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nFuel, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_tmAccFuel, 0, 0 );
	// 7
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  2560, 0, szSMTime, 0, &cbLen );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  7500, 0, szSkillInfluence, 0, &cbLen );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &dwSkillPoint, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  1024, 0, m_aCompleteQuest, 0, &cbLen );
#ifdef __INVENTORY_168
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, icsInventory.szExt, 0, &cbLen);
#else //__INVENTORY_168
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 2000, 0, icsInventory.szExt, 0, &cbLen);
#endif //__INVENTORY_168
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  2000, 0, icsBank[pMover->m_nSlot].szExt, 0, &cbLen );
#if __VER >= 15 // __PETVIS
#ifdef __INVENTORY_168
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, icsInventory.szPiercing, 0, &cbLen);
#else //__INVENTORY_168
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 7800, 0, icsInventory.szPiercing, 0, &cbLen);
#endif //__INVENTORY_168
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  7800, 0, icsBank[pMover->m_nSlot].szPiercing, 0, &cbLen );
#else // __PETVIS
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  2000, 0, icsInventory.szPiercing, 0, &cbLen );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  2000, 0, icsBank[pMover->m_nSlot].szPiercing, 0, &cbLen );
#endif // __PETVIS
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_dwReturnWorldID, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nSkillPoint, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nSkillLevel, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, 0, 0, &nSkillExp, 0, 0 );
#ifdef __EVENT_1101
	#ifdef __EVENT_1101_2
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT,   0, 0, &pMover->m_nEventFlag, 0, 0 );
	#else // __EVENT_1101_2
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_dwEventFlag, 0, 0 );
	#endif // __EVENT_1101_2
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_dwEventTime, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_dwEventElapsed, 0, 0 );
#endif	// __EVENT_1101
#if __VER >= 8 // __CSC_VER8_5
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, 0, 0, &pMover->m_nAngelExp, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_nAngelLevel, 0, 0 );
#endif // __CSC_VER8_5
#ifdef __INVENTORY_168
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, icsInventory.szPet, 0, &cbLen);
#else //__INVENTORY_168
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 2000, 0, icsInventory.szPet, 0, &cbLen);
#endif //__INVENTORY_168
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  2000, 0, icsBank[pMover->m_nSlot].szPet, 0, &cbLen );
	DWORD dwPetId	= pMover->GetPetId();
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &dwPetId, 0, 0 );
#ifdef __EXP_ANGELEXP_LOG
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nExpLog, 0, 0 );
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nAngelExpLog, 0, 0 );
#endif // __EXP_ANGELEXP_LOG

#ifdef __EVENTLUA_COUPON
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nCoupon, 0, 0 );
#endif // __EVENTLUA_COUPON
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &pMover->m_nHonor, 0, 0 );
#endif	// __HONORABLE_TITLE			// 달인
#ifdef __LAYER_1015
	int nLayer	= pMover->GetLayer();
	bOK[++j]	= qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &nLayer, 0, 0 );
#endif	// __LAYER_1015
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	bOK[++j] = qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  100, 0, CheckedQuest, 0, &cbLen );
#endif // __IMPROVE_QUEST_INTERFACE

#if __VER >= 15 // __CAMPUS
	int nCampusPoint	= pMover->GetCampusPoint();
	bOK[++j]	= qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &nCampusPoint, 0, 0 );
	u_long idCampus		= pMover->GetCampusId();
	bOK[++j]	= qry->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,   0, 0, &idCampus, 0, 0 );
#endif // __CAMPUS

#ifdef __VENDING_NPC
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_idVendorNPC, 0, 0);
#endif // __VENDING_NPC

#ifdef __HIDE_FASHION
	int nHideCoat = 0;
	for (int z = 0; z < 5; ++z)
	{
		int c = (pMover->m_abHideCoat[z] ? 1 : 0);
		nHideCoat |= (c << z);
	}
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &nHideCoat, 0, 0);
#endif // __HIDE_FASHION

#ifdef __SYS_CUSTOM_TITLE
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, MAX_CUSTOM_TITLE, 20, pMover->m_szOwnTitle, 0, &cbLen);
#endif // __SYS_CUSTOM_TITLE

#ifdef __AEGON_LOOT_BOX
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_dwLootboxItem, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_dwLootboxItemNum, 0, 0);
#endif // __AEGON_LOOT_BOX

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
	char szAchFavTmp[32];
	char szAchFav[3096];
	sprintf(szAchFavTmp, "");
	sprintf(szAchFav, "");
	for (int i = 0; i < pMover->m_AchievementFavorites.size(); i++) 
	{
		sprintf(szAchFavTmp, "%d/", pMover->m_AchievementFavorites[i]);
		strcat(szAchFav, szAchFavTmp);
	}
	strcat(szAchFav, NullStr);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 3096, 0, szAchFav, 0, &cbLen);
#endif // __AEGON_ACHIEVEMENT_SYSTEM

#ifdef __SYS_NAMECOLOR
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_ncR, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_ncG, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_ncB, 0, 0);
#endif //__SYS_NAMECOLOR

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, 0, 0, &pMover->m_nCollectingTime, 0, 0);
#endif // __AEGON_ACHIEVEMENT_SYSTEM

#ifdef __AEGON_LAST_ONLINE
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, 0, 0, &pMover->m_nLastOnline, 0, 0);
#endif // __AEGON_LAST_ONLINE

#ifdef __AEGON_INGAME_TIME
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, 0, 0, &pMover->m_nOnlineTime, 0, 0);
#endif // __AEGON_INGAME_TIME

#ifdef __AEGON_CRAFTING_SYSTEM
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_nCraftingExp, 0, 0);
#endif // __AEGON_CRAFTING_SYSTEM

#ifdef __AEGON_DELETED_CHAR_CHECK
	int nDeleted = 0;
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &nDeleted, 0, 0);
#endif //__AEGON_DELETED_CHAR_CHECK

#ifdef __AEGON_AUTO_STATS
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_nAutoStat, 0, 0);
#endif //__AEGON_AUTO_STATS

#ifdef __SYS_REBIRTH
	int nRebirthLevel = pMover->GetRebirthLevel();
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &nRebirthLevel, 0, 0);
#endif //__SYS_REBIRTH

#ifdef __FL_BAG_LOOT
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_nSelectedBag, 0, 0);
#endif //__FL_BAG_LOOT

#ifdef __AEGON_DB_PETFILTER
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, 0, 0, &pMover->m_dwPetfilter, 0, 0);


	char szPetfilterLists[4096] = {0, };
	memset(szPetfilterLists, 0, 4096);
	pMover->m_sPetfilterLists.WriteToString(szPetfilterLists);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 4096, 0, szPetfilterLists, 0, &cbLen);
#endif

	for( i=0; i<j; ++i )
	{
		if( bOK[i] == FALSE )
		{
			char szBuffer[128];
			sprintf( szBuffer, "SavePlayer(%s) - %dth bindparam error - %d", pMover->m_szName, i+1, ::GetCurrentThreadId() );
		#ifdef _DEBUG
			AfxMessageBox( szBuffer );
		#endif
			WriteLog( szBuffer );

			// chipi_바인딩 실패시 DB Reconnect
			WriteLog( "try DB Reconnect... - ThreadId : %d", ::GetCurrentThreadId() );
			qry->DisConnect();
			if( qry->Connect( 3, qry->DBName, qry->DBId, qry->DBPass ) )
				WriteLog( "DB Reconnect Success! - ThreadId : %d", ::GetCurrentThreadId() );
			else
				WriteLog( "DB Reconnect Failed! - ThreadId : %d", ::GetCurrentThreadId() );
			qry->Clear();
			// chipi_바인딩 실패시 DB Reconnect

			SAFE_DELETE_ARRAY( bOK );
			return;
		}
	}

	if( qry->Exec( szQuery ) == FALSE )
	{
		WriteLog( "SavePlayer(%s) - Exec RETURN FALSE, ThreadID : %d", pMover->m_szName, ::GetCurrentThreadId() );
		SAFE_DELETE_ARRAY( bOK );
		return;
	}

	if( qry->Fetch() )
	{
		int nError = qry->GetInt( "fError" );
		if( nError != 1 )
		{
			WriteLog( "SavePlayer(%s) - fError:%d", pMover->m_szName, nError );
			SAFE_DELETE_ARRAY( bOK );
			return;
		}
	}

#ifndef __RT_1025
	SaveMessengerFriend( qry, pMover, szQuery );
#endif	// __RT_1025

#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	SaveHonor( qry, pMover->m_idPlayer, pMover->m_aHonorTitle, szQuery );
#endif	// __HONORABLE_TITLE			// 달인

#ifndef __S_NEW_SKILL_2
#ifdef __SKILL_0205
	SaveSkill( qry, pMover->m_idPlayer, pMover->m_aJobSkill, pMover->m_abUpdateSkill, szQuery );
#else	// __SKILL_0205
	SaveSkill( qry, pMover->m_idPlayer, pMover->m_aJobSkill, szQuery );
#endif	// __SKILL_0205

#endif // __S_NEW_SKILL_2

	// 다른 캐릭터의 bank 저장
	for( i = 0 ; i < 3 ; ++i )
	{
		if( pMover->m_idPlayerBank[i] != 0 && i != pMover->m_nSlot )
		{
			ADDBANK_QUERYINFO info( "U1" );
			info.idPlayer			= pMover->m_idPlayerBank[i];
			info.dwGoldBank			= pMover->m_dwGoldBank[i];
			info.pszBank			= icsBank[i].szItem;
			info.pszBankIndex		= icsBank[i].szIndex;
			info.pszObjIndexBank	= icsBank[i].szObjIndex;
			info.pszExtBank			= icsBank[i].szExt;
			info.pszPirecingBank	= icsBank[i].szPiercing;
			info.pszBankPet		= icsBank[i].szPet;

			DBQryAddBankSave( szQuery, info );
			
			{
#ifdef _WIN64
				SQLLEN cbLen = SQL_NTS;
#else
				SQLINTEGER cbLen = SQL_NTS;
#endif
				BOOL bOK[7];
				bOK[0] = qry->BindParameter( 1, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  4290, 0, (char*)info.pszBank, 0, &cbLen );
				bOK[1] = qry->BindParameter( 2, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  215, 0, (char*)info.pszBankIndex, 0, &cbLen );
				bOK[2] = qry->BindParameter( 3, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  215, 0, (char*)info.pszObjIndexBank, 0, &cbLen );
				bOK[3] = qry->BindParameter( 4, SQL_PARAM_INPUT, SQL_C_LONG,  SQL_INTEGER,  0, 0, &info.dwGoldBank, 0, 0 );
				bOK[4] = qry->BindParameter( 5, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  2000, 0, (char*)info.pszExtBank, 0, &cbLen );
#if __VER >= 15 // __PETVIS
				bOK[5] = qry->BindParameter( 6, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  7800, 0, (char*)info.pszPirecingBank, 0, &cbLen );
#else // __PETVIS
				bOK[5] = qry->BindParameter( 6, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  2000, 0, (char*)info.pszPirecingBank, 0, &cbLen );
#endif // __PETVIS
				bOK[6] = qry->BindParameter( 7, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR,  2689, 0, (char*)info.pszBankPet, 0, &cbLen );
			}

			if( FALSE == qry->Exec( szQuery ) )
			{
				SAFE_DELETE_ARRAY( bOK );
				return;
			}
		}
	}
#if __VER >= 11 // __SYS_POCKET
	PocketStruct	aPocket[3];
	SavePocket( pMover, aPocket );
	for( i = 0; i < MAX_POCKET; i++ )
	{
		PocketParam	p;
		p.idPlayer	= pMover->m_idPlayer;
		p.pszItem	= aPocket[i].szItem;
		p.pszIndex	= aPocket[i].szIndex;
		p.pszObjIndex	= aPocket[i].szObjIndex;
		p.pszExt	= aPocket[i].szExt;
		p.pszPiercing	= aPocket[i].szPiercing;
		p.pszPet	= aPocket[i].szPet;
		p.bExpired	= aPocket[i].bExpired;
		p.tExpirationDate	= aPocket[i].tExpirationDate;

		MakeQueryPocket( szQuery, p );
#ifdef _WIN64
		SQLLEN cbLen = SQL_NTS;
#else
		SQLINTEGER	cbLen	= SQL_NTS;
#endif
		BOOL	bOk[9];

		bOk[0]	= qry->BindParameter( 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &i, 0, &cbLen );
		bOk[1]	= qry->BindParameter( 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 4290, 0, (char*)p.pszItem, 0, &cbLen );
		bOk[2]	= qry->BindParameter( 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 215, 0, (char*)p.pszIndex, 0, &cbLen );
		bOk[3]	= qry->BindParameter( 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 215, 0, (char*)p.pszObjIndex, 0, &cbLen );
		bOk[4]	= qry->BindParameter( 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 2000, 0, (char*)p.pszExt, 0, &cbLen );
#if __VER >= 15 // __PETVIS
		bOk[5]	= qry->BindParameter( 6, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 7800, 0, (char*)p.pszPiercing, 0, &cbLen );
#else // __PETVIS
		bOk[5]	= qry->BindParameter( 6, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 2000, 0, (char*)p.pszPiercing, 0, &cbLen );
#endif // __PETVIS
		bOk[6]	= qry->BindParameter( 7, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 2689, 0, (char*)p.pszPet, 0, &cbLen );
		bOk[7]	= qry->BindParameter( 8, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &p.bExpired, 0, &cbLen );
		bOk[8]	= qry->BindParameter( 9, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &p.tExpirationDate, 0, &cbLen );


		if( !qry->Exec( szQuery ) )
		{
			SAFE_DELETE_ARRAY( bOK );
			return;
		}
	}
#endif	// __SYS_POCKET

#ifdef __AEGON_FIXES
	if (!pMover || pMover->m_idPlayer == 0) 
	{
		SAFE_DELETE_ARRAY(bOK);
		return;
	}
#endif

#ifdef __AEGON_MONSTER_HUNT
	SaveMonsterHunt(pMover, qry);
#endif // __AEGON_MONSTER_HUNT

#ifdef __BADGE
	SaveBadge(pMover, qry);
#endif //__BADGE

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
	SaveAchievement(pMover, qry);
#endif // __AEGON_ACHIEVEMENT_SYSTEM

#ifdef __AEGON_SEASON_PASS
	SaveSeasonPassProgress(pMover, qry);
#endif // __AEGON_SEASON_PASS

#ifdef __SWITCH_EQUIP
	SaveEquipSwitch(pMover, qry);
#endif // __SWITCH_EQUIP

#ifdef __AEGON_PERMANENT_BUFF
	SavePermanentBuffs(pMover, qry);
#endif // __AEGON_PERMANENT_BUFF

#ifdef __AEGON_PREM_MOTIONS
	SaveUnlockedMotions(pMover, qry);
#endif // __AEGON_PREM_MOTIONS

#ifdef __AEGON_EMOTE_SYSTEM
	SaveUnlockedEmotes(pMover, qry);
#endif //__AEGON_EMOTE_SYSTEM

#ifdef __AEGON_F_TOOL
	SaveFToolUnits(pMover, qry);
#endif

	SAFE_DELETE_ARRAY( bOK );
}

#ifdef __S_NEW_SKILL_2
void CDbManager::AllSaveSkill( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	u_long			uidPlayer;
	EXPINTEGER		nSkillExp = 0;						// 스킬경험치
	int				nSkillPoint;					// SP
	int				nSkillLevel = 0;					// 지금까지 올린 스킬레벨
	SKILL			aJobSkill[ MAX_SKILL_JOB ];
	BYTE			abUpdateSkill[MAX_SKILL_JOB];
	BOOL			bSaveSkillState = FALSE;
	
	arRead >> uidPlayer;
	arRead >> nSkillLevel;
	arRead >> nSkillPoint;
	arRead.Read( (void*)aJobSkill, sizeof(SKILL) * ( MAX_SKILL_JOB ) );
	
	ACCOUNT_CACHE* pAccount		= NULL;
	g_DbManager.m_AddRemoveLock.Enter();
	pAccount = m_AccountCacheMgr.Find( lpDbOverlappedPlus->AccountInfo.szAccount );

#ifdef __GAMEGUARD
	int nLecAcc = strlen(lpDbOverlappedPlus->AccountInfo.szAccount);
	if (nLecAcc < 3)
	{
		Error("CDbManager::CreatePlayer() : nLecAcc < 3 [%s]", lpDbOverlappedPlus->AccountInfo.szAccount, nLecAcc);
		return;
	}
	if (prj.IsAllowedLetter(lpDbOverlappedPlus->AccountInfo.szAccount) == FALSE)
	{
		Error("CDbManager::CreatePlayer() : InValid Letter in szAccount [%s]", lpDbOverlappedPlus->AccountInfo.szAccount);
		FreeRequest(lpDbOverlappedPlus);
		return;
	}
#endif // __GAMEGUARD

	int nRefreshSlot = 100;
	if( pAccount )
	{
		for( int i = 0; i < 3; i++ )
		{
			if( pAccount->pMover[i] && pAccount->pMover[i]->m_idPlayer == uidPlayer )
			{
				nRefreshSlot = i;
				break;
			}
		}
	}
	if( nRefreshSlot != 100 )
	{
		if( pAccount->pMover[nRefreshSlot]->m_nSkillLevel != nSkillLevel ||
			pAccount->pMover[nRefreshSlot]->m_nSkillPoint != nSkillPoint )
		{
			bSaveSkillState = TRUE;	
			pAccount->pMover[nRefreshSlot]->m_nSkillLevel = nSkillLevel;
			pAccount->pMover[nRefreshSlot]->m_nSkillPoint = nSkillPoint;
		}

#ifdef __SKILL_0205
		for( int k = 0 ; k < MAX_SKILL_JOB; k++)
			abUpdateSkill[k]	= (BOOL)( memcmp( &pAccount->pMover[nRefreshSlot]->m_aJobSkill[k], &aJobSkill[k], sizeof(SKILL) ) != 0 );
#endif	// __SKILL_0205
		
		memcpy( pAccount->pMover[nRefreshSlot]->m_aJobSkill, &aJobSkill, sizeof(aJobSkill) );				
	}
	g_DbManager.m_AddRemoveLock.Leave();

	char szQuery[QUERY_SIZE]	= { 0,};

	if( nRefreshSlot != 100 )
	{
		if( bSaveSkillState )
		{
			sprintf( szQuery,
				"uspLearnSkillCharacter @serverindex='%02d',@pPlayerID='%07d',@im_SkillExp=%I64d,@im_SkillPoint=%d,@im_SkillLv=%d",
				g_appInfo.dwSys, uidPlayer, nSkillExp, nSkillPoint, nSkillLevel ); 
			
			if( FALSE == pQuery->Exec( szQuery ) )
			{
				WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
				FreeRequest( lpDbOverlappedPlus );
				return;
			}
		}
#ifdef __SKILL_0205
		SaveSkill( pQuery, uidPlayer, aJobSkill, abUpdateSkill, szQuery );
#else	// __SKILL_0205
		SaveSkill( pQuery, uidPlayer, aJobSkill, szQuery );
#endif // __SKILL_0205
	}
				
	FreeRequest( lpDbOverlappedPlus );
}
#endif // __S_NEW_SKIL_2

#if __VER >= 13 // __HONORABLE_TITLE			// 달인
void	CDbManager::SaveHonor( CQuery *qry, u_long uidPlayer, int * aHonor, char* szQuery )
{
	int	aHonorEtc[50] = {0,};
	int	aHonorItem[50] = {0,};
	int	aHonorMonster[50] = {0,};

    int nEtc = 0,nItem = 0,nMonster = 0;
	int nType = 0;
	int nCurrentTitleCount =  CTitleManager::Instance()->m_nCurrentTitleCount;
	for(int i=0;i<nCurrentTitleCount;i++)
	{
		nType = CTitleManager::Instance()->GetIdxType(i);
		if( nType == HI_HUNT_MONSTER)
		{
			aHonorMonster[nMonster] = aHonor[i];
			nMonster++;
		}
		else if( nType == HI_USE_ITEM)
		{
			aHonorItem[nItem] = aHonor[i];
			nItem++;
		}
		else
		{
			aHonorEtc[nEtc] = aHonor[i];
			nEtc++;
		}
	}

	sprintf( szQuery,
		"usp_Master_Update @serverindex='%02d',@m_idPlayer='%07d',@sec=%d,@c01=%d,@c02=%d,@c03=%d,@c04=%d,@c05=%d,@c06=%d,@c07=%d,@c08=%d,@c09=%d,@c10=%d,@c11=%d,@c12=%d,@c13=%d,@c14=%d,@c15=%d,@c16=%d,@c17=%d,@c18=%d,@c19=%d,@c20=%d,@c21=%d,@c22=%d,@c23=%d,@c24=%d,@c25=%d,@c26=%d,@c27=%d,@c28=%d,@c29=%d,@c30=%d,@c31=%d,@c32=%d,@c33=%d,@c34=%d,@c35=%d,@c36=%d,@c37=%d,@c38=%d,@c39=%d,@c40=%d,@c41=%d,@c42=%d,@c43=%d,@c44=%d,@c45=%d,@c46=%d,@c47=%d,@c48=%d,@c49=%d,@c50=%d",
		g_appInfo.dwSys, uidPlayer, 1,
		aHonorEtc[0],aHonorEtc[1],aHonorEtc[2],aHonorEtc[3],aHonorEtc[4],aHonorEtc[5],aHonorEtc[6],aHonorEtc[7],aHonorEtc[8],aHonorEtc[9],
		aHonorEtc[10],aHonorEtc[11],aHonorEtc[12],aHonorEtc[13],aHonorEtc[14],aHonorEtc[15],aHonorEtc[16],aHonorEtc[17],aHonorEtc[18],aHonorEtc[19],
		aHonorEtc[20],aHonorEtc[21],aHonorEtc[22],aHonorEtc[23],aHonorEtc[24],aHonorEtc[25],aHonorEtc[26],aHonorEtc[27],aHonorEtc[28],aHonorEtc[29],
		aHonorEtc[30],aHonorEtc[31],aHonorEtc[32],aHonorEtc[33],aHonorEtc[34],aHonorEtc[35],aHonorEtc[36],aHonorEtc[37],aHonorEtc[38],aHonorEtc[39],
		aHonorEtc[40],aHonorEtc[41],aHonorEtc[42],aHonorEtc[43],aHonorEtc[44],aHonorEtc[45],aHonorEtc[46],aHonorEtc[47],aHonorEtc[48],aHonorEtc[49]	); 

	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		return;
	}

	sprintf( szQuery,
		"usp_Master_Update @serverindex='%02d',@m_idPlayer='%07d',@sec=%d,@c01=%d,@c02=%d,@c03=%d,@c04=%d,@c05=%d,@c06=%d,@c07=%d,@c08=%d,@c09=%d,@c10=%d,@c11=%d,@c12=%d,@c13=%d,@c14=%d,@c15=%d,@c16=%d,@c17=%d,@c18=%d,@c19=%d,@c20=%d,@c21=%d,@c22=%d,@c23=%d,@c24=%d,@c25=%d,@c26=%d,@c27=%d,@c28=%d,@c29=%d,@c30=%d,@c31=%d,@c32=%d,@c33=%d,@c34=%d,@c35=%d,@c36=%d,@c37=%d,@c38=%d,@c39=%d,@c40=%d,@c41=%d,@c42=%d,@c43=%d,@c44=%d,@c45=%d,@c46=%d,@c47=%d,@c48=%d,@c49=%d,@c50=%d",
		g_appInfo.dwSys, uidPlayer, 2,
		aHonorItem[0],aHonorItem[1],aHonorItem[2],aHonorItem[3],aHonorItem[4],aHonorItem[5],aHonorItem[6],aHonorItem[7],aHonorItem[8],aHonorItem[9],
		aHonorItem[10],aHonorItem[11],aHonorItem[12],aHonorItem[13],aHonorItem[14],aHonorItem[15],aHonorItem[16],aHonorItem[17],aHonorItem[18],aHonorItem[19],
		aHonorItem[20],aHonorItem[21],aHonorItem[22],aHonorItem[23],aHonorItem[24],aHonorItem[25],aHonorItem[26],aHonorItem[27],aHonorItem[28],aHonorItem[29],
		aHonorItem[30],aHonorItem[31],aHonorItem[32],aHonorItem[33],aHonorItem[34],aHonorItem[35],aHonorItem[36],aHonorItem[37],aHonorItem[38],aHonorItem[39],
		aHonorItem[40],aHonorItem[41],aHonorItem[42],aHonorItem[43],aHonorItem[44],aHonorItem[45],aHonorItem[46],aHonorItem[47],aHonorItem[48],aHonorItem[49]	); 

	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		return;
	}

	sprintf( szQuery,
		"usp_Master_Update @serverindex='%02d',@m_idPlayer='%07d',@sec=%d,@c01=%d,@c02=%d,@c03=%d,@c04=%d,@c05=%d,@c06=%d,@c07=%d,@c08=%d,@c09=%d,@c10=%d,@c11=%d,@c12=%d,@c13=%d,@c14=%d,@c15=%d,@c16=%d,@c17=%d,@c18=%d,@c19=%d,@c20=%d,@c21=%d,@c22=%d,@c23=%d,@c24=%d,@c25=%d,@c26=%d,@c27=%d,@c28=%d,@c29=%d,@c30=%d,@c31=%d,@c32=%d,@c33=%d,@c34=%d,@c35=%d,@c36=%d,@c37=%d,@c38=%d,@c39=%d,@c40=%d,@c41=%d,@c42=%d,@c43=%d,@c44=%d,@c45=%d,@c46=%d,@c47=%d,@c48=%d,@c49=%d,@c50=%d",
		g_appInfo.dwSys, uidPlayer, 3,
		aHonorMonster[0],aHonorMonster[1],aHonorMonster[2],aHonorMonster[3],aHonorMonster[4],aHonorMonster[5],aHonorMonster[6],aHonorMonster[7],aHonorMonster[8],aHonorMonster[9],
		aHonorMonster[10],aHonorMonster[11],aHonorMonster[12],aHonorMonster[13],aHonorMonster[14],aHonorMonster[15],aHonorMonster[16],aHonorMonster[17],aHonorMonster[18],aHonorMonster[19],
		aHonorMonster[20],aHonorMonster[21],aHonorMonster[22],aHonorMonster[23],aHonorMonster[24],aHonorMonster[25],aHonorMonster[26],aHonorMonster[27],aHonorMonster[28],aHonorMonster[29],
		aHonorMonster[30],aHonorMonster[31],aHonorMonster[32],aHonorMonster[33],aHonorMonster[34],aHonorMonster[35],aHonorMonster[36],aHonorMonster[37],aHonorMonster[38],aHonorMonster[39],
		aHonorMonster[40],aHonorMonster[41],aHonorMonster[42],aHonorMonster[43],aHonorMonster[44],aHonorMonster[45],aHonorMonster[46],aHonorMonster[47],aHonorMonster[48],aHonorMonster[49]	); 

	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		return;
	}


}

#endif	// __HONORABLE_TITLE			// 달인

#ifdef __SKILL_0205
void CDbManager::SaveSkill( CQuery *qry, u_long uidPlayer, LPSKILL aJobSkill, LPBYTE abUpdateSkill, char* szQuery )
#else	// #define	__SKILL_0205
void CDbManager::SaveSkill( CQuery *qry, u_long uidPlayer, LPSKILL aJobSkill, char* szQuery )
#endif	// #define	__SKILL_0205
{
#ifdef __3RD_LEGEND16
	for( int i = 0 ; i < (MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL + MAX_LEGEND_HERO_SKILL ) ; i++)
#else // __3RD_LEGEND16
#if __VER >= 10 //__LEGEND	//	10차 전승시스템	Neuz, World, Trans
	for( int i = 0 ; i < (MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL ) ; i++)
#else //__LEGEND	//	10차 전승시스템	Neuz, World, Trans
	for( int i = 0 ; i < (MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL) ; i++)
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
#endif // __3RD_LEGEND16
	{
#ifdef __SKILL_0205
		if( abUpdateSkill[i] == 0 )
			continue;
#endif	// __SKILL_0205
		sprintf( szQuery,
			"uspLearnSkill @serverindex='%02d',@pPlayerID='%07d',@pSkillID=%d,@pSkillLv=%d,@pSkillPosition=%d",
			g_appInfo.dwSys, uidPlayer, aJobSkill[i].dwSkill, aJobSkill[i].dwLevel, i ); 
	
		if( FALSE == qry->Exec( szQuery ) )
		{
			WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
			return;
//			continue;
		}
	}
}

// raiders_todo 의미 없음 
void CDbManager::SaveJobLv( CMover* pMover, char* szJobLv )
{
	char OneJobLv[16] = {0,};
	for( int i = 0 ; i < MAX_JOB ; i++)
	{
		sprintf(OneJobLv, "%d/", 0);
		strncat(szJobLv, OneJobLv, sizeof(OneJobLv));
	}
	strncat(szJobLv, NullStr, sizeof(NullStr));
}

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
void CDbManager::SaveQuest( CMover* pMover, char* szQuestCnt, char* szm_aCompleteQuest, char* szCheckedQuest )
#else // __IMPROVE_QUEST_INTERFACE
void CDbManager::SaveQuest( CMover* pMover, char* szQuestCnt, char* szm_aCompleteQuest )
#endif // __IMPROVE_QUEST_INTERFACE
{
	char sPerQuest[128];
	char OneCompleteQuest[20] = {0,};
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	char OneCheckedQuest[20] = {0, };
#endif // __IMPROVE_QUEST_INTERFACE

	for( int i = 0; i < pMover->m_nQuestSize; i++ )
	{
		sprintf( sPerQuest, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d/",
			pMover->m_aQuest[i].m_wId, pMover->m_aQuest[i].m_nState, pMover->m_aQuest[i].m_wTime,
			pMover->m_aQuest[i].m_nKillNPCNum[0], pMover->m_aQuest[i].m_nKillNPCNum[1],
			pMover->m_aQuest[i].m_bPatrol, pMover->m_aQuest[i].m_bReserve2,	pMover->m_aQuest[i].m_bReserve3, pMover->m_aQuest[i].m_bReserve4,
			pMover->m_aQuest[i].m_bReserve5, pMover->m_aQuest[i].m_bReserve6, pMover->m_aQuest[i].m_bReserve7, pMover->m_aQuest[i].m_bReserve8
			);
		strncat( szQuestCnt, sPerQuest, sizeof(sPerQuest) );
	}
	
	for( int i = 0; i < pMover->m_nCompleteQuestSize; i++ )
	{
		sprintf(OneCompleteQuest, "%d/", pMover->m_aCompleteQuest[i]);
		strncat( szm_aCompleteQuest, OneCompleteQuest, sizeof(OneCompleteQuest) );
	}

	strcat( szQuestCnt, NullStr );
	strcat( szm_aCompleteQuest, NullStr );

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	for( int i = 0; i < pMover->m_nCheckedQuestSize; ++i )
	{
		sprintf( OneCheckedQuest, "%d/", pMover->m_aCheckedQuest[i]);
		strncat( szCheckedQuest, OneCheckedQuest, sizeof( OneCheckedQuest ) );
	}
	strcat( szCheckedQuest, NullStr );
#endif // __IMPROVE_QUEST_INTERFACE
}

#if __VER >= 11 // __SYS_POCKET
void	CDbManager::SavePocket( CMover* pMover, PPocketStruct pPocketStruct )
{
	for( int nPocket = 0; nPocket < MAX_POCKET; nPocket++ )
	{
		ItemStruct is;
		BOOL	bExt	= FALSE;
		BOOL	bPiercing	= FALSE;
		BOOL	bPet	= FALSE;
		char sIndex[10]		= { 0,};
		char sObjIndex[16]		= { 0,};
		CPocket* pPocket	= pMover->m_Pocket.GetPocket( nPocket );
		if( pPocket )
		{
			for( DWORD i = 0; i < pPocket->m_dwItemMax; i++ )
			{
				SaveOneItem( &pPocket->m_apItem[i], &is );
				strncat( pPocketStruct[nPocket].szItem, is.szItem, sizeof(is.szItem) );
				strncat( pPocketStruct[nPocket].szExt, is.szExt, sizeof(is.szExt) );
				strncat( pPocketStruct[nPocket].szPiercing, is.szPiercing, sizeof(is.szPiercing) );
				sprintf( sIndex, "%d/", pPocket->m_apIndex[i] );
				strncat( pPocketStruct[nPocket].szIndex, sIndex, sizeof(sIndex) );
				sprintf( sObjIndex, "%d/", pPocket->m_apItem[i].m_dwObjIndex );
				strcat( pPocketStruct[nPocket].szObjIndex, sObjIndex );
				strncat( pPocketStruct[nPocket].szPet, is.szPet, sizeof(is.szPet) );
				if( pPocket->m_apItem[i].m_bCharged || 0 < pPocket->m_apItem[i].m_dwKeepTime || 0 != pPocket->m_apItem[i].GetRandomOptItemId() 	
#ifdef __AEGON_RETURN_AWAKE
					|| 0 != pPocket->m_apItem[i].GetOldRandomOptItemId()
#endif // __AEGON_RETURN_AWAKE
					)
					bExt	= TRUE;
				if( pPocket->m_apItem[i].IsPiercedItem() )
					bPiercing	= TRUE;
				if( pPocket->m_apItem[i].m_pPet )
					bPet	= TRUE;
			}
			strncat( pPocketStruct[nPocket].szItem, NullStr, sizeof(NullStr) );
			strncat( pPocketStruct[nPocket].szIndex, NullStr, sizeof(NullStr) );
			strcat( pPocketStruct[nPocket].szObjIndex, NullStr );
			if( bExt == FALSE )
				*pPocketStruct[nPocket].szExt	= '\0';
			if( bPiercing == FALSE )
				*pPocketStruct[nPocket].szPiercing	= '\0';
			if( bPet == FALSE )
				*pPocketStruct[nPocket].szPet	= '\0';
			strcat( pPocketStruct[nPocket].szExt, NullStr );
			strcat( pPocketStruct[nPocket].szPiercing, NullStr );
			strcat( pPocketStruct[nPocket].szPet, NullStr );
			pPocketStruct[nPocket].bExpired		= pPocket->IsExpired();
			pPocketStruct[nPocket].tExpirationDate	= pPocket->GetExpirationDate();
		}
		else
		{
			lstrcpy( pPocketStruct[nPocket].szItem, NullStr ); 
			lstrcpy( pPocketStruct[nPocket].szIndex, NullStr ); 
			lstrcpy( pPocketStruct[nPocket].szObjIndex, NullStr ); 
			lstrcpy( pPocketStruct[nPocket].szExt, NullStr ); 
			lstrcpy( pPocketStruct[nPocket].szPiercing, NullStr ); 
			lstrcpy( pPocketStruct[nPocket].szPet, NullStr ); 
			pPocketStruct[nPocket].bExpired		= TRUE;
			pPocketStruct[nPocket].tExpirationDate	= 0;
		}
	}
}
#endif	// __SYS_POCKET

void CDbManager::SaveInventory( CMover* pMover, PItemContainerStruct pItemContainerStruct )
{
	ItemStruct	is;
	char Onem_apIndex[10]		= {0,};
	char sPerObjIndex[16]		= {0,};
	BOOL bExtInven = FALSE;
	BOOL bPirecingInven = FALSE;

	BOOL	bPet	= FALSE;

	for( DWORD ch = 0; ch < pMover->m_Inventory.m_dwItemMax; ch++ )	// 0-504
	{
		SaveOneItem( &pMover->m_Inventory.m_apItem[ch], &is );
		strncat( pItemContainerStruct->szItem, is.szItem, sizeof(is.szItem) );
		strncat( pItemContainerStruct->szExt, is.szExt, sizeof(is.szExt) );
		strncat( pItemContainerStruct->szPiercing, is.szPiercing, sizeof(is.szPiercing) );
		sprintf( Onem_apIndex, "%d/", pMover->m_Inventory.m_apIndex[ch] );
		strncat( pItemContainerStruct->szIndex, Onem_apIndex, sizeof(Onem_apIndex) );
		sprintf( sPerObjIndex, "%d/", pMover->m_Inventory.m_apItem[ch].m_dwObjIndex );
		strcat( pItemContainerStruct->szObjIndex, sPerObjIndex );
		strncat( pItemContainerStruct->szPet, is.szPet, sizeof(is.szPet) );

		if( pMover->m_Inventory.m_apItem[ch].m_bCharged || 0 < pMover->m_Inventory.m_apItem[ch].m_dwKeepTime || 0 != pMover->m_Inventory.m_apItem[ch].GetRandomOptItemId() 
#ifdef __AEGON_RETURN_AWAKE
			|| 0 != pMover->m_Inventory.m_apItem[ch].GetOldRandomOptItemId()
#endif // __AEGON_RETURN_AWAKE
			)
			bExtInven = TRUE;
		if( pMover->m_Inventory.m_apItem[ch].IsPiercedItem() )
			bPirecingInven = TRUE;
		if( pMover->m_Inventory.m_apItem[ch].m_pPet )
			bPet	= TRUE;
	}

	strncat( pItemContainerStruct->szItem, NullStr, sizeof(NullStr) );
	strncat( pItemContainerStruct->szIndex, NullStr, sizeof(NullStr) );
	strcat( pItemContainerStruct->szObjIndex, NullStr );
	if( bExtInven == FALSE )
		*pItemContainerStruct->szExt	= '\0';
	if( bPirecingInven == FALSE )
		*pItemContainerStruct->szPiercing	= '\0';
	if( bPet == FALSE )
		*pItemContainerStruct->szPet	= '\0';
	strcat( pItemContainerStruct->szExt, NullStr );
	strcat( pItemContainerStruct->szPiercing, NullStr );
	strcat( pItemContainerStruct->szPet, NullStr );
}

void CDbManager::SaveEquipment( CMover* pMover, char* szEquipment )
{
	char OneEquipment[16] = {0,};
	for( int i = 0 ; i < MAX_HUMAN_PARTS ; i++)
	{
		sprintf( OneEquipment, "%d/", pMover->m_aEquipInfo[i].byFlag );
		strncat(szEquipment, OneEquipment, sizeof(OneEquipment));
	}
	strncat(szEquipment, NullStr, sizeof(NullStr));
}

void CDbManager::SaveBank( CMover* pMover, CItemContainer<CItemElem  >* pPlayerBank, ItemContainerStruct * pItemContainerStruct )
{
	ItemStruct	is;
	char Onem_apIndex[10] = {0,};
	char sPerObjIndex[16]	= { 0, };
	BOOL bExtBank = FALSE;
	BOOL bPirecingBank = FALSE;

	BOOL	bPet	= FALSE;

	for( DWORD ch = 0; ch < pPlayerBank->m_dwItemMax; ch++ )	// 0-504
	{	
		SaveOneItem( &pPlayerBank->m_apItem[ch], &is );
		strncat( pItemContainerStruct->szItem, is.szItem, sizeof(is.szItem) );
		strncat( pItemContainerStruct->szExt, is.szExt, sizeof(is.szExt) );
		strncat( pItemContainerStruct->szPiercing, is.szPiercing, sizeof(is.szPiercing) );
		sprintf( Onem_apIndex, "%d/", pPlayerBank->m_apIndex[ch]);
		strncat( pItemContainerStruct->szIndex, Onem_apIndex, sizeof(Onem_apIndex) );
		sprintf( sPerObjIndex, "%d/", pPlayerBank->m_apItem[ch].m_dwObjIndex );
		strcat( pItemContainerStruct->szObjIndex, sPerObjIndex );
		strncat( pItemContainerStruct->szPet, is.szPet, sizeof(is.szPet) );
		if( pPlayerBank->m_apItem[ch].m_bCharged || 0 < pPlayerBank->m_apItem[ch].m_dwKeepTime || 0 != pPlayerBank->m_apItem[ch].GetRandomOptItemId() 
#ifdef __AEGON_RETURN_AWAKE
			|| 0 != pPlayerBank->m_apItem[ch].GetOldRandomOptItemId()
#endif // __AEGON_RETURN_AWAKE
			)
			bExtBank = TRUE;
		if( pPlayerBank->m_apItem[ch].IsPiercedItem() )
			bPirecingBank = TRUE;
		if( pPlayerBank->m_apItem[ch].m_pPet )
			bPet	= TRUE;
	}
	strncat( pItemContainerStruct->szItem, NullStr, sizeof(NullStr) );
	strncat( pItemContainerStruct->szIndex, NullStr, sizeof(NullStr) );
	strcat( pItemContainerStruct->szObjIndex, NullStr );
	if( bExtBank == FALSE )
		*pItemContainerStruct->szExt	= '\0';
	if( bPirecingBank == FALSE )
		*pItemContainerStruct->szPiercing	= '\0';
	if( bPet == FALSE )
		*pItemContainerStruct->szPet	= '\0';
	strcat( pItemContainerStruct->szExt, NullStr );
	strcat( pItemContainerStruct->szPiercing, NullStr );
	strcat( pItemContainerStruct->szPet, NullStr );
}

void CDbManager::SaveGuildBank( CItemContainer<CItemElem>*  pGuildBank, PItemContainerStruct pItemContainerStruct )
{
	ItemStruct is;
	char Onem_apIndex[10] = {0,};
	char sPerObjIndex[16]	= { 0, };
	BOOL bExtBank = FALSE;
	BOOL bPirecingBank = FALSE;
	BOOL bPet	= FALSE;

	for( DWORD ch = 0; ch < pGuildBank->m_dwItemMax; ch++ )	// 0-504
	{	
		SaveOneItem( &pGuildBank->m_apItem[ch], &is );
		strncat( pItemContainerStruct->szItem, is.szItem, sizeof(is.szItem) );
		strncat( pItemContainerStruct->szExt, is.szExt, sizeof(is.szExt) );
		strncat( pItemContainerStruct->szPiercing, is.szPiercing, sizeof(is.szPiercing) );
		sprintf( Onem_apIndex, "%d/", pGuildBank->m_apIndex[ch]);
		strncat( pItemContainerStruct->szIndex, Onem_apIndex, sizeof(Onem_apIndex) );
		sprintf( sPerObjIndex, "%d/", pGuildBank->m_apItem[ch].m_dwObjIndex );
		strcat( pItemContainerStruct->szObjIndex, sPerObjIndex );
		strncat( pItemContainerStruct->szPet, is.szPet, sizeof(is.szPet) );

		if( pGuildBank->m_apItem[ch].m_bCharged || 0 < pGuildBank->m_apItem[ch].m_dwKeepTime || 0 != pGuildBank->m_apItem[ch].GetRandomOptItemId() 
#ifdef __AEGON_RETURN_AWAKE
			|| 0 != pGuildBank->m_apItem[ch].GetOldRandomOptItemId()
#endif // __AEGON_RETURN_AWAKE
			)
			bExtBank = TRUE;
		if( pGuildBank->m_apItem[ch].IsPiercedItem() )
			bPirecingBank = TRUE;
		if( pGuildBank->m_apItem[ch].m_pPet )
			bPet	= TRUE;
	}
	strncat( pItemContainerStruct->szItem, NullStr, sizeof(NullStr) );
	strncat( pItemContainerStruct->szIndex, NullStr, sizeof(NullStr));
	strcat( pItemContainerStruct->szObjIndex, NullStr );
	if( bExtBank == FALSE )
		*pItemContainerStruct->szExt	= '\0';
	if( bPirecingBank == FALSE )
		*pItemContainerStruct->szPiercing	= '\0';
	strcat( pItemContainerStruct->szExt, NullStr );
	strcat( pItemContainerStruct->szPiercing, NullStr );
	strcat( pItemContainerStruct->szPet, NullStr );
}

void CDbManager::SaveCardCube( CMover* pMover, char* szCard, char* szsCardIndex, char* szsCardObjIndex, char* szCube, char* szsCubeIndex, char* szsCubeObjIndex )
{
}

void CDbManager::SaveTaskBar( CMover* pMover, char* szAppletTaskBar, char* szItemTaskBar, char* szSkillTaskBar )
{
	char OneAppletTaskBar[MAX_TASKBAR] = { 0, };
	char OneItemTaskBar[MAX_TASKBAR] = { 0, };
	char OneSkillTaskBar[256] = { 0, };
	for( int ch = 0 ; ch < MAX_SLOT_APPLET ; ch++ )
	{
		if( pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwShortcut != SHORTCUT_NONE )
		{
#if _WIN64
			sprintf(OneAppletTaskBar, "%d,%d,%d,%d,%d,%d,%I64d", ch,
				pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwShortcut, pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwId, pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwType,
				pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwIndex, pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwUserId, pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwData);
#else
			sprintf( OneAppletTaskBar, "%d,%d,%d,%d,%d,%d,%d", ch, 
				pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwShortcut, pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwId, pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwType,
				pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwIndex, pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwUserId, pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwData );
#endif
			
			if( pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwShortcut == SHORTCUT_CHAT)
				SetStrDBFormat( OneAppletTaskBar, pMover->m_UserTaskBar.m_aSlotApplet[ch].m_szString );
			
			strncat( szAppletTaskBar, OneAppletTaskBar, sizeof(OneAppletTaskBar));
			strcat( szAppletTaskBar, "/" );
		}
	}
	strcat( szAppletTaskBar, NullStr );
	
	for( int ch = 0; ch < MAX_SLOT_ITEM_COUNT; ch++ )
	{
		for( int j = 0; j < MAX_SLOT_ITEM; j++ )
		{
			if( pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwShortcut != SHORTCUT_NONE )
			{
#if _WIN64
				sprintf(OneItemTaskBar, "%d,%d,%d,%d,%d,%d,%d,%I64d", ch, j,
					pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwShortcut, pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwId, pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwType,
					pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwIndex, pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwUserId, pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwData);
#else
				sprintf( OneItemTaskBar, "%d,%d,%d,%d,%d,%d,%d,%d", ch, j,
					pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwShortcut, pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwId, pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwType,
					pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwIndex, pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwUserId, pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwData );
#endif
				
				if( pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwShortcut == SHORTCUT_CHAT )
					SetStrDBFormat( OneItemTaskBar, pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_szString );
				
				strncat( szItemTaskBar, OneItemTaskBar, sizeof(OneItemTaskBar) );
				strcat( szItemTaskBar, "/" );
			}
		}
	}
	strcat( szItemTaskBar, NullStr );
	
#ifdef __ACTION_SLOT_PAGES
	for (int i = 0; i < MAX_ACTION_SLOT; i++) {
		for (int ch = 0; ch < MAX_SLOT_QUEUE; ch++)
		{
			if (pMover->m_UserTaskBar.m_aSlotQueue[i][ch].m_dwShortcut != SHORTCUT_NONE)		// °ªAI μe¾iAO´AAo °E≫c
			{
#if _WIN64
				sprintf(OneSkillTaskBar, "%d,%d,%d,%d,%d,%d,%d,%I64d/", i, ch,
					pMover->m_UserTaskBar.m_aSlotQueue[i][ch].m_dwShortcut, pMover->m_UserTaskBar.m_aSlotQueue[i][ch].m_dwId, pMover->m_UserTaskBar.m_aSlotQueue[i][ch].m_dwType,
					pMover->m_UserTaskBar.m_aSlotQueue[i][ch].m_dwIndex, pMover->m_UserTaskBar.m_aSlotQueue[i][ch].m_dwUserId, pMover->m_UserTaskBar.m_aSlotQueue[i][ch].m_dwData);
				strncat(szSkillTaskBar, OneSkillTaskBar, sizeof(OneSkillTaskBar));
#else
				sprintf(OneSkillTaskBar, "%d,%d,%d,%d,%d,%d,%d,%d/", i, ch,
					pMover->m_UserTaskBar.m_aSlotQueue[i][ch].m_dwShortcut, pMover->m_UserTaskBar.m_aSlotQueue[i][ch].m_dwId, pMover->m_UserTaskBar.m_aSlotQueue[i][ch].m_dwType,
					pMover->m_UserTaskBar.m_aSlotQueue[i][ch].m_dwIndex, pMover->m_UserTaskBar.m_aSlotQueue[i][ch].m_dwUserId, pMover->m_UserTaskBar.m_aSlotQueue[i][ch].m_dwData);
				strncat(szSkillTaskBar, OneSkillTaskBar, sizeof(OneSkillTaskBar));
#endif
			}
		}
	}
#else // __ACTION_SLOT_PAGES
	for( int ch = 0; ch < MAX_SLOT_QUEUE; ch++ )
	{
		if( pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwShortcut != SHORTCUT_NONE )		// 값이 들어있는지 검사
		{
#if _WIN64
			sprintf(OneSkillTaskBar, "%d,%d,%d,%d,%d,%d,%I64d/", ch,
				pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwShortcut, pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwId, pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwType,
				pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwIndex, pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwUserId, pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwData);
#else
			sprintf( OneSkillTaskBar, "%d,%d,%d,%d,%d,%d,%d/", ch,
				pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwShortcut, pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwId, pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwType,
				pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwIndex, pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwUserId, pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwData );
#endif
			strncat( szSkillTaskBar, OneSkillTaskBar, sizeof( OneSkillTaskBar ) );
		}			
	}
#endif // __ACTION_SLOT_PAGES
	strcat( szSkillTaskBar, NullStr );
}

void CDbManager::SaveSMCode( CMover* pMover, char* szszSMTime )
{
	BOOL bSMSave = FALSE;
	for( int ch = 0 ; ch < SM_MAX ; ++ch )
	{
		if( 0 < pMover->m_dwSMTime[ch] )
		{
			bSMSave = TRUE;
		}
		char OneSMTime[256] = { 0, };
		if( ch == SM_MAX_HP50 )
		{
			sprintf( OneSMTime, "%d,%d,", pMover->m_dwSMTime[ch], pMover->m_nPlusMaxHitPoint );
		}
		else
		if( ch == SM_RESIST_ATTACK_LEFT )
		{
			sprintf( OneSMTime, "%d,%d,", pMover->m_dwSMTime[ch], pMover->m_nAttackResistLeft );
		}
		else
		if( ch == SM_RESIST_ATTACK_RIGHT )
		{
			sprintf( OneSMTime, "%d,%d,", pMover->m_dwSMTime[ch], pMover->m_nAttackResistRight );
		}
		else
		if( ch == SM_RESIST_DEFENSE )
		{
			sprintf( OneSMTime, "%d,%d,", pMover->m_dwSMTime[ch], pMover->m_nDefenseResist );
		}
		else
		{
			sprintf( OneSMTime, "%d,", pMover->m_dwSMTime[ch] );
		}
		strcat( szszSMTime, OneSMTime );
	}
	strcat( szszSMTime, NullStr );
	
	if( bSMSave == FALSE )
	{
		memset( szszSMTime, 0, sizeof( szszSMTime ) );
	}
}

void CDbManager::SaveSkillInfluence( CMover* pMover, char* szszSkillInfluence )
{
#ifdef __BUFF_1107
	pMover->m_buffs.ToString( szszSkillInfluence );
#else	// __BUFF_1107
	char OneSkillInfluence[256] = { 0, };
	for( int ch = 0 ; ch < MAX_SKILLINFLUENCE ; ++ ch )
	{
		SKILLINFLUENCE* pSkillInfluenece = &pMover->m_SkillState.m_aSkillInfluence[ch];
		if( pSkillInfluenece->wType == BUFF_EQUIP )
			continue;
#if __VER >= 13 // __HOUSING
		if( pSkillInfluenece->wType == 0 && pSkillInfluenece->wID == 0
			&& pSkillInfluenece->dwLevel == 0 && pSkillInfluenece->tmCount == 0 )
			break;
#endif // __HOUSING
		sprintf( OneSkillInfluence, "%d,%d,%d,%d/", 
			pSkillInfluenece->wType, pSkillInfluenece->wID,
			pSkillInfluenece->dwLevel, pSkillInfluenece->tmCount );
		strcat( szszSkillInfluence, OneSkillInfluence );
	}
	strcat( szszSkillInfluence, NullStr );
#endif	// __BUFF_1107
}

void CDbManager::SavePlayTime( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	DWORD dwTime;
	arRead.ReadString( lpDbOverlappedPlus->AccountInfo.szAccount, MAX_ACCOUNT );

#ifdef __GAMEGUARD
	int nLecAcc = strlen(lpDbOverlappedPlus->AccountInfo.szAccount);
	if (nLecAcc < 3)
	{
		Error("CDbManager::CreatePlayer() : nLecAcc < 3 [%s]", lpDbOverlappedPlus->AccountInfo.szAccount, nLecAcc);
		return;
	}
	if (prj.IsAllowedLetter(lpDbOverlappedPlus->AccountInfo.szAccount) == FALSE)
	{
		Error("CDbManager::CreatePlayer() : InValid Letter in szAccount [%s]", lpDbOverlappedPlus->AccountInfo.szAccount);
		FreeRequest(lpDbOverlappedPlus);
		return;
	}
#endif // __GAMEGUARD

	u_long idPlayer;
	arRead >> idPlayer;
	arRead >> dwTime; // 시각
	dwTime /= 1000;

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryCharacter( szQuery, "U3", idPlayer, g_appInfo.dwSys, '\0', "", dwTime );
	
	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s\r\n\t%s", __FILE__, __LINE__, szQuery, lpDbOverlappedPlus->AccountInfo.szPlayer );
	}
	
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::DBQryAddBankSave( char* szSql, const ADDBANK_QUERYINFO & info )
{
		sprintf( szSql, "{call ADD_BANK_STR('U1','%07d','%02d', ?, ?, ?, ?, ?, ?, ?)}", info.idPlayer, g_appInfo.dwSys ); 
}

#if __VER >= 11 // __SYS_POCKET
void CDbManager::MakeQueryPocket( char* szQuery, const PocketParam & p )
{
	sprintf( szQuery, "{call uspSavePocket( '%02d', '%07d', ?, ?, ?, ?, ?, ?, ?, ?, ?) }", g_appInfo.dwSys, p.idPlayer );
}
#endif	// __SYS_POCKET

void CDbManager::SaveOneItem( CItemElem* pItemElem, PItemStruct pItemStruct )
{
	char szPiercing[32]		= {0,};

	if( pItemElem->IsEmpty() == FALSE )
	{
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
		if( pItemElem->m_dwItemId == II_SYS_SYS_SCR_SEALCHARACTER )
			sprintf( pItemStruct->szItem, "%d,%d,%d,%d,,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d/",
					pItemElem->m_dwObjId, pItemElem->m_dwItemId,
					0, 0, 
					pItemElem->m_nItemNum, pItemElem->m_nRepairNumber,
					pItemElem->m_nHitPoint, pItemElem->m_nRepair,
					0, pItemElem->m_byFlag,
					pItemElem->GetSerialNumber(), pItemElem->GetOption(), 
  					pItemElem->m_bItemResist, pItemElem->m_nResistAbilityOption,
  					pItemElem->m_idGuild,
					pItemElem->m_nResistSMItemId
				);
		else
			sprintf( pItemStruct->szItem, "%d,%d,%d,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"
#ifdef __LOOKCHANGE
				",%d"
#endif //__LOOKCHANGE
#ifdef __SYS_LOCK_ITEM
				",%d"
#endif //__SYS_LOCK_ITEM
#ifdef __CHIP_PLAYERSHOP
				",%d"
#endif // __CHIP_PLAYERSHOP
#ifdef __WEAPON_RARITY
				",%d"
#endif // __WEAPON_RARITY
#ifdef __UPGRADE_SUCCESS
				",%d"
#endif //__UPGRADE_SUCCESS
#ifdef __WEAPON_MERGE
				",%I64d"
				",%d"
#endif //__WEAPON_MERGE
#ifdef __PET_GLOW
				",%d"
#endif // __PET_GLOW
#ifdef __CRYSTAL
				",%d"
#endif //__CRYSTAL
				"/",
					pItemElem->m_dwObjId, pItemElem->m_dwItemId,
					0, 0, pItemElem->m_szItemText,
					pItemElem->m_nItemNum, pItemElem->m_nRepairNumber,
					pItemElem->m_nHitPoint, pItemElem->m_nRepair,
					0, pItemElem->m_byFlag,
					pItemElem->GetSerialNumber(), pItemElem->GetOption(), 
  					pItemElem->m_bItemResist, pItemElem->m_nResistAbilityOption,
  					pItemElem->m_idGuild,
					pItemElem->m_nResistSMItemId,
#ifdef __LOOKCHANGE
					pItemElem->GetLook(),					// 1x %d added.
#endif //__LOOKCHANGE
#ifdef __SYS_LOCK_ITEM
					pItemElem->IsItemLocked(),				// 1x %d added.
#endif //__SYS_LOCK_ITEM
#ifdef __CHIP_PLAYERSHOP
					pItemElem->m_nBuyMode,					// 1x %d added.
#endif // __CHIP_PLAYERSHOP
#ifdef __WEAPON_RARITY
					pItemElem->GetWeaponRarity(),			// 1x %d added.
#endif // __WEAPON_RARITY
#ifdef __UPGRADE_SUCCESS
					pItemElem->m_nUpgradeTries				// 1x %d added.
#endif //__UPGRADE_SUCCESS
#ifdef __WEAPON_MERGE
					, pItemElem->GetMergedRandomOptItemId()	// 1x %I64d added.
					, pItemElem->GetMergedWeaponATK()		// 1x %d added.
#endif //__WEAPON_MERGE
#ifdef __PET_GLOW
					, pItemElem->m_nPetGlow					// 1x %d added.
#endif // __PET_GLOW
#ifdef __CRYSTAL
					, pItemElem->GetCrystal()				// 1x %d added.
#endif //__CRYSTAL
				);
#else	//  __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
		// item
		sprintf( pItemStruct->szItem, "%d,%d,%d,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d/",
				pItemElem->m_dwObjId, pItemElem->m_dwItemId,
				0, 0, pItemElem->m_szItemText,
				pItemElem->m_nItemNum, pItemElem->m_nRepairNumber,
				pItemElem->m_nHitPoint, pItemElem->m_nRepair,
				0, pItemElem->m_byFlag,
				pItemElem->GetSerialNumber(), pItemElem->GetOption(), 
  				pItemElem->m_bItemResist, pItemElem->m_nResistAbilityOption,
  				pItemElem->m_idGuild,
				pItemElem->m_nResistSMItemId
			);
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

		// ext
#if __VER >= 11 // __SYS_IDENTIFY
		sprintf( pItemStruct->szExt, "%d,%d,%I64d"
#if __VER >= 15 // __PETVIS	
			",%d"
#endif // __PETVIS
#ifdef __AEGON_RETURN_AWAKE
			",%I64d"
#endif // __AEGON_RETURN_AWAKE
			"/", pItemElem->m_bCharged, pItemElem->m_dwKeepTime, pItemElem->GetRandomOptItemId()
#if __VER >= 15 // __PETVIS
			, static_cast<int>( pItemElem->m_bTranformVisPet )
#endif // __PETVIS
#ifdef __AEGON_RETURN_AWAKE
			, pItemElem->GetOldRandomOptItemId()
#endif // __AEGON_RETURN_AWAKE
			);
#else	// __SYS_IDENTIFY
		sprintf( pItemStruct->szExt, "%d,%d,%d/", pItemElem->m_bCharged, pItemElem->m_dwKeepTime, pItemElem->GetRandomOptItemId() );
#endif	// __SYS_IDENTIFY

		// piercing
#if __VER >= 12 // __EXT_PIERCING
		ItemProp* itemProp = prj.GetItemProp( pItemElem->m_dwItemId );
		if( itemProp && itemProp->IsUltimate() )
		{
			sprintf( pItemStruct->szPiercing, "%d", pItemElem->GetUltimatePiercingSize() );
			for( int nPirecing = 0 ; nPirecing < pItemElem->GetUltimatePiercingSize(); ++nPirecing )
			{
				sprintf( szPiercing, ",%d", pItemElem->GetUltimatePiercingItem( nPirecing ) );
				strncat( pItemStruct->szPiercing, szPiercing, sizeof(szPiercing) );
			}
#ifndef __AEGON_FIXES
			if( pItemElem->GetPiercingSize() > 0 )
#endif
			{
				sprintf( szPiercing, ",%d", pItemElem->GetPiercingSize() );
				strncat( pItemStruct->szPiercing, szPiercing, sizeof(szPiercing) );
			}
		}
		else
#endif // __EXT_PIERCING
			sprintf( pItemStruct->szPiercing, "%d", pItemElem->GetPiercingSize() );

		for( int nPirecing = 0 ; nPirecing < pItemElem->GetPiercingSize(); ++nPirecing )
		{
			sprintf( szPiercing, ",%d", pItemElem->GetPiercingItem( nPirecing ) );
			strncat( pItemStruct->szPiercing, szPiercing, sizeof(szPiercing) );
		}
#if __VER >= 15 // __PETVIS
		if( pItemElem->IsVisPet() )
		{
			for( int nPirecing = 0; nPirecing < pItemElem->GetPiercingSize(); ++nPirecing )
			{
#ifdef _WIN64
				sprintf(szPiercing, ",%I64d", pItemElem->GetVisKeepTime(nPirecing));
#else
				sprintf( szPiercing, ",%d", pItemElem->GetVisKeepTime( nPirecing ) );
#endif
				strncat( pItemStruct->szPiercing, szPiercing, sizeof(szPiercing) );
			}
		}
#endif // __PETVIS

#ifdef __WEAPON_MERGE
		if (pItemElem->IsMerged())
		{
			sprintf(szPiercing, ",%d", pItemElem->GetMergedPiercingSize());
			strncat(pItemStruct->szPiercing, szPiercing, sizeof(szPiercing));
			for (int nPirecing = 0; nPirecing < pItemElem->GetMergedPiercingSize(); ++nPirecing)
			{
				sprintf(szPiercing, ",%d", pItemElem->GetMergedPiercingItem(nPirecing));
				strncat(pItemStruct->szPiercing, szPiercing, sizeof(szPiercing));
			}
		}
#endif

		strcat( pItemStruct->szPiercing, "/" );

		// pet
		if( pItemElem->m_pPet )
		{
			sprintf( pItemStruct->szPet, "1,%d,%d,%d,%d,%d", 
				pItemElem->m_pPet->GetKind(), pItemElem->m_pPet->GetLevel(),
				pItemElem->m_pPet->GetExp(), pItemElem->m_pPet->GetEnergy(),
				pItemElem->m_pPet->GetLife() );
			for( int i = PL_D; i <= pItemElem->m_pPet->GetLevel(); i++ )
			{
				char szAvailLevel[16]	= { 0,};
				sprintf( szAvailLevel, ",%d", pItemElem->m_pPet->GetAvailLevel( i ) );
				strcat( pItemStruct->szPet, szAvailLevel );
			}
#ifdef __PET_1024
			char szTemp[MAX_PET_NAME_FMT+1]		= { 0,};
			char szFmt[MAX_PET_NAME_FMT]	= { 0,};
			SetDBFormatStr( szFmt, MAX_PET_NAME_FMT, pItemElem->m_pPet->GetName() );
			sprintf( szTemp, ",%s", szFmt );
			strcat( pItemStruct->szPet, szTemp );
#endif	// __PET_1024
			strcat( pItemStruct->szPet, "/" );
		}
		else
		{
			sprintf( pItemStruct->szPet, "0/" );
		}
	}
	else
	{
		// item
		ZeroMemory( pItemStruct->szItem, sizeof(pItemStruct->szItem) );
		// ext
#ifdef __AEGON_RETURN_AWAKE
		sprintf(pItemStruct->szExt, "%d,%d,%d,%d/", 0, 0, 0, 0);
#else
		sprintf( pItemStruct->szExt, "%d,%d,%d/", 0, 0, 0 );
#endif
		// piercing
		sprintf( pItemStruct->szPiercing, "%d/", 0 );
		// pet
		sprintf( pItemStruct->szPet, "0/" );
	}
}

#ifdef __AEGON_MONSTER_HUNT
void CDbManager::SaveMonsterHunt(CMover* pMover, CQuery* qry)
{
	CString strQry;
	for (int i = 0; i < pMover->m_MHProgress.vecQuests.size(); i++) 
	{
		strQry.Format("{ CALL SAVE_MONSTERHUNT('%07d', '%02d', '%d', '%d', '%d', '%d')}", pMover->m_idPlayer, g_appInfo.dwSys, pMover->m_MHProgress.vecQuests[i].dwQuestID, pMover->m_MHProgress.vecQuests[i].dwMobID, pMover->m_MHProgress.vecQuests[i].dwCount, pMover->m_MHProgress.vecQuests[i].dwRewardSent);
		if (qry->Exec(strQry) == FALSE)
		{
			WriteLog("%s - %07d, %02d, %d, %d, %d, %d", __FUNCTION__, pMover->m_idPlayer, g_appInfo.dwSys, pMover->m_MHProgress.vecQuests[i].dwQuestID, pMover->m_MHProgress.vecQuests[i].dwMobID, pMover->m_MHProgress.vecQuests[i].dwCount, pMover->m_MHProgress.vecQuests[i].dwRewardSent);
		}
	}
}
#endif // __AEGON_MONSTER_HUNT

#ifdef __BADGE
void CDbManager::SaveBadge(CMover* pMover, CQuery* qry)
{
	CString strQry;
	char szBad[8] = { 0, };
	char szBadges[3096] = { 0, };
	const vector<int>& badges = pMover->GetOwnedBadges();
	bool has = false;

	for (auto it : badges)
	{
		has = true;
		sprintf(szBad, "%d/", it);
		strcat(szBadges, szBad);
	}

	if (has == false)
		*szBadges = '\0';

	strcat(szBadges, NullStr);

	strQry.Format("{ CALL SAVE_BADGES('%07d', '%02d', '%d', '%s')}", pMover->m_idPlayer, g_appInfo.dwSys, pMover->GetBadge(), szBadges);
	if (qry->Exec(strQry) == FALSE)
	{
		WriteLog("%s - %07d, %02d, %d, %s", __FUNCTION__, pMover->m_idPlayer, g_appInfo.dwSys, pMover->GetBadge(), szBadges);
	}
}
#endif //__BADGE

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
void CDbManager::SaveAchievement(CMover* pMover, CQuery* qry)
{
	CString strQry;	
	int achievementID = -1;
	int achievementProg = -1;
	int rewardSent = -1;

	if (!pMover || pMover->m_idPlayer == 0)
		return;

	__ACHIEVEMENT_PROGRESS* pProgress = pMover->GetAchievementProgress();
	if (!pProgress || pProgress->mapAchievementProgress.size() <= 0)
		return;

	map<int, __ACHIEVEMENT_PROGRESS_UNIT>::iterator it = pProgress->mapAchievementProgress.begin();

	for (; it != pProgress->mapAchievementProgress.end(); it++)
	{
		achievementID = it->second.nAchievementID;
		achievementProg = it->second.nProgressCount;
		rewardSent = it->second.nRewardSent;

		if (achievementID < 0 || achievementProg < 0 || rewardSent < 0)
			continue;

		strQry.Format("{ CALL SAVE_ACHIEVEMENT('%07d', '%02d', '%d', '%d', '%d')}", pMover->m_idPlayer, g_appInfo.dwSys, achievementID, achievementProg, rewardSent);
		if (qry->Exec(strQry) == FALSE)
		{
			WriteLog("%s - %07d, %02d, %d, %d, %d", __FUNCTION__, pMover->m_idPlayer, g_appInfo.dwSys, achievementID, achievementProg, rewardSent);
		}
	}
}
void CDbManager::UpdateAchievementForOfflineUser(u_long playerID, int achievementID, int additionalProg)
{
	if (playerID <= 0 || achievementID < 0 || additionalProg < 0)
		return;

	char lpOutputString[256];
	CQuery* pQuery = new CQuery;
	if (FALSE == pQuery->Connect(3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01))
	{
		sprintf(lpOutputString, "fail to connect: dsn=%s, id=%s, pwd=%s", DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01);
		OutputDebugString(lpOutputString);
		SAFE_DELETE(pQuery);
		return;
	}

	CString strQry;
	strQry.Format("SELECT m_nAchievementProg, m_nRewardSent FROM ACHIEVEMENT_TBL WHERE m_idPlayer = '%07d' AND m_nAchievementID = %d", playerID, achievementID);

	if (!pQuery->Execute(strQry.GetString()))
	{
		SAFE_DELETE(pQuery);
		return;
	}

	if (!pQuery->Fetch())
	{
		SAFE_DELETE(pQuery);
		return;
	}

	int currentAchievementProg = pQuery->GetInt("m_nAchievementProg");
	int currentRewardSent = pQuery->GetInt("m_nRewardSent");

	if (currentAchievementProg < 0 || currentRewardSent < 0)
	{
		SAFE_DELETE(pQuery);
		return;
	}

	int newProgress = currentAchievementProg + additionalProg;

	map<int, __ACHIEVEMENT_UNIT>::iterator it = CAegonAchievementMng::GetInstance()->m_mapAchievements.find(achievementID);
	if (it == CAegonAchievementMng::GetInstance()->m_mapAchievements.end()) 
	{
		SAFE_DELETE(pQuery);
		return;
	}
	if (currentAchievementProg == it->second.maxProgress) 
	{
		SAFE_DELETE(pQuery);
		return;
	}
	
	if (newProgress > it->second.maxProgress)
		newProgress = it->second.maxProgress;
	
	strQry.Format("{ CALL SAVE_ACHIEVEMENT('%07d', '%02d', '%d', '%d', '%d')}", playerID, g_appInfo.dwSys, achievementID, newProgress, currentRewardSent);
	if (pQuery->Exec(strQry) == FALSE)
	{
		WriteLog("%s - %07d, %02d, %d, %d, %d", __FUNCTION__, playerID, g_appInfo.dwSys, achievementID, newProgress, currentRewardSent);
	}
}

#endif // __AEGON_ACHIEVEMENT_SYSTEM

#ifdef __BAN_SYSTEM
void CDbManager::BanUser(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	char szAcc[MAX_ACCOUNT] = { 0, };
	char szQuery[QUERY_SIZE] = { 0, };
	int nDays;
	arRead.ReadString(szAcc, MAX_ACCOUNT);
	arRead >> nDays;
	CTime time = CTime::GetCurrentTime();
	sprintf(szQuery, "UPDATE ACCOUNT_DBF.dbo.ACCOUNT_TBL_DETAIL SET BlockTime = '%s' where account = '%s' ", time.Format("%Y%m%d").GetString(), szAcc);
	if (FALSE == qry->Exec(szQuery))
	{
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
		FreeRequest(lpDbOverlappedPlus);
		return;
	}
	time += CTimeSpan(nDays, 0, 0, 0);
	sprintf(szQuery, "UPDATE ACCOUNT_DBF.dbo.ACCOUNT_TBL_DETAIL SET EndTime = '%s' where account = '%s' ", time.Format("%Y%m%d").GetString(), szAcc);
	if (FALSE == qry->Exec(szQuery))
	{
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
		FreeRequest(lpDbOverlappedPlus);
		return;
	}
	Error("Banned Player : %s for %d day(s).", szAcc, nDays);
	FreeRequest(lpDbOverlappedPlus);
}
#endif // __BAN_SYSTEM

#ifdef __AEGON_SEASON_PASS
void CDbManager::SaveSeasonPassProgress(CMover* pMover, CQuery* qry)
{

	if (!pMover || !qry || pMover->m_idPlayer == 0)
		return;

	CString strQry;
	map<int, __SEASONPASS_PROGRESS_UNIT>::iterator it = pMover->m_SeasonpassProgress.mapProgUnits.begin();
	for (; it != pMover->m_SeasonpassProgress.mapProgUnits.end(); it++)
	{
		char singleClaimed[8] = { 0, };
		char rewardsClaimed[1024] = { 0, };
		for (int i = 0; i < it->second.rewardsClaimed.size(); i++) 
		{
			sprintf(singleClaimed, "%d/", it->second.rewardsClaimed[i]);
			strcat(rewardsClaimed, singleClaimed);
		}
		strcat(rewardsClaimed, NullStr);

		strQry.Format("{ CALL SAVE_SEASONPASS('%07d', '%02d', '%d', '%d', '%d', '%s')}", pMover->m_idPlayer, g_appInfo.dwSys, it->second.passID, it->second.progressPoints, it->second.boughtPremium, rewardsClaimed);
		if (qry->Exec(strQry) == FALSE)
		{
			WriteLog("%s - %07d, %02d, %d, %d, %d, %s", __FUNCTION__, pMover->m_idPlayer, g_appInfo.dwSys, it->second.passID, it->second.progressPoints, it->second.boughtPremium, rewardsClaimed);
		}
	}
}
#endif // __AEGON_SEASON_PASS

#ifdef __SWITCH_EQUIP
void CDbManager::SaveEquipSwitch(CMover* pMover, CQuery* pQuery)
{
	CString strQuery;
	int nIndex = 0;
	char szDBTitle[128] = { 0, };
	for (auto& pEquip : pMover->m_vEquipSwitchInfo)
	{
		CString strObjIndex;
		for (int i = 0; i < MAX_HUMAN_PARTS; i++)
		{
			if (pEquip.dwId[i] == NULL_ID)
				pEquip.dwId[i] = -1;
			strObjIndex.Format("%s%d/", strObjIndex.GetString(), pEquip.dwId[i]);
		}
		strObjIndex += "$";

		pEquip.strTitle[19] = '\0';
		FormatStrToHex(szDBTitle, pEquip.strTitle);

		strQuery.Format("{ CALL SAVE_EQUIPSWITCH('%07d', '%02d', '%d', '%s', '%d', '%s')}", pMover->m_idPlayer, g_appInfo.dwSys, nIndex, szDBTitle, pEquip.nActivate, strObjIndex.GetString());
		if (!pQuery->Exec(strQuery))
		{
			WriteLog("%s - %07d, %02d, %d, %s, %d, %s", __FUNCTION__, pMover->m_idPlayer, g_appInfo.dwSys, nIndex, szDBTitle, pEquip.nActivate, strObjIndex);
		}
		nIndex++;
	}

	strQuery.Format("{ CALL DELETE_EQUIPSWITCH('%07d', '%02d', '%d')}", pMover->m_idPlayer, g_appInfo.dwSys, nIndex);
	if (!pQuery->Exec(strQuery))
	{
		WriteLog("%s - %07d, %02d, %d", __FUNCTION__, pMover->m_idPlayer, g_appInfo.dwSys, nIndex);
	}
	
}
#endif // __SWITCH_EQUIP

#ifdef __AEGON_PERMANENT_BUFF
void CDbManager::SavePermanentBuffs(CMover* pMover, CQuery* qry)
{
	CString strQry;
	char szPermBuffs[2048] = { 0, };
	char szSingleBuff[32] = { 0, };
	for (int i = 0; i < pMover->m_vPermanenetBuffs.size(); i++) 
	{
		if (pMover->m_vPermanenetBuffs[i] > 0) 
		{
			sprintf(szSingleBuff, "%d/", pMover->m_vPermanenetBuffs[i]);
			strcat(szPermBuffs, szSingleBuff);
		}
	}

	strcat(szPermBuffs, NullStr);

	strQry.Format("{ CALL SAVE_PERM_BUFFS('%07d', '%02d', '%s')}", pMover->m_idPlayer, g_appInfo.dwSys, szPermBuffs);
	if (qry->Exec(strQry) == FALSE)
	{
		WriteLog("%s - %07d, %02d, %s", __FUNCTION__, pMover->m_idPlayer, g_appInfo.dwSys, szPermBuffs);
	}
}
#endif // __AEGON_PERMANENT_BUFF

#ifdef __AEGON_PREM_MOTIONS
void CDbManager::SaveUnlockedMotions(CMover* pMover, CQuery* qry)
{
	CString strQry;
	char szUnlockedMotions[2048] = { 0, };
	char szSingleMotion[32] = { 0, };
	for (int i = 0; i < pMover->m_vUnlockedMotions.size(); i++)
	{
		sprintf(szSingleMotion, "%d/", pMover->m_vUnlockedMotions[i]);
		strcat(szUnlockedMotions, szSingleMotion);
	}

	strcat(szUnlockedMotions, NullStr);

	strQry.Format("{ CALL SAVE_UNLOCKED_MOTIONS('%07d', '%02d', '%s')}", pMover->m_idPlayer, g_appInfo.dwSys, szUnlockedMotions);
	if (qry->Exec(strQry) == FALSE)
	{
		WriteLog("%s - %07d, %02d, %s", __FUNCTION__, pMover->m_idPlayer, g_appInfo.dwSys, szUnlockedMotions);
	}
}
#endif // __AEGON_PREM_MOTIONS

#ifdef __AEGON_EMOTE_SYSTEM
void CDbManager::SaveUnlockedEmotes(CMover* pMover, CQuery* qry)
{
	CString strQry;
	char szUnlockedEmotes[2048] = { 0, };
	char szSingleEmote[32] = { 0, };
	for (int i = 0; i < pMover->m_vUnlockedEmotes.size(); i++)
	{
		sprintf(szSingleEmote, "%d/", pMover->m_vUnlockedEmotes[i]);
		strcat(szUnlockedEmotes, szSingleEmote);
	}

	strcat(szUnlockedEmotes, NullStr);

	strQry.Format("{ CALL SAVE_UNLOCKED_EMOTES('%07d', '%02d', '%s')}", pMover->m_idPlayer, g_appInfo.dwSys, szUnlockedEmotes);
	if (qry->Exec(strQry) == FALSE)
	{
		WriteLog("%s - %07d, %02d, %s", __FUNCTION__, pMover->m_idPlayer, g_appInfo.dwSys, szUnlockedEmotes);
	}
}
#endif //__AEGON_EMOTE_SYSTEM

#ifdef __ROYAL_RUMBLE
void CDbManager::SaveWarPlayers(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	TCHAR szName[MAX_PLAYER];
	char szQuery[QUERY_SIZE] = { 0, };
	u_long idPlayer;
	u_int nCount;
	int nKills, nDeaths, nPoints, nJob;
	arRead >> nCount;
	for (u_int i = 0; i < nCount; i++)
	{
		arRead.ReadString(szName);
		arRead >> idPlayer;
		arRead >> nJob;
		arRead >> nKills;
		arRead >> nDeaths;
		arRead >> nPoints;
		if (idPlayer > 0)
		{
			sprintf(szQuery, "{ call CHARACTER_01_DBF.dbo.uspSaveSiegeUser('%02d','%s','%07d','%d','%d','%d','%d') }", g_appInfo.dwSys, szName, idPlayer, nJob, nPoints, nKills, nDeaths);
			if (FALSE == qry->Exec(szQuery))
			{
				Error("[Player Siege] Cannot save User details id = %d", idPlayer);
				WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
				FreeRequest(lpDbOverlappedPlus);
				return;
			}
		}
		else
			Error("invalid player id return - CDbManager::SaveWarPlayers");
	}
	FreeRequest(lpDbOverlappedPlus);
}
#endif // __ROYAL_RUMBLE

#ifdef __AEGON_F_TOOL
void CDbManager::SaveFToolUnits(CMover* pMover, CQuery* qry)
{
	CString strQry;
	char szFTool[4096] = { 0, };
	char szSingleUnit[32] = { 0, };
	for (int i = 0; i < pMover->m_vFToolUnits.size(); i++)
	{
		sprintf(szSingleUnit, "%d,%d,%.2f/", pMover->m_vFToolUnits[i].m_nPage, pMover->m_vFToolUnits[i].m_nFKey, pMover->m_vFToolUnits[i].m_fIntervall);
		strcat(szFTool, szSingleUnit);
	}

	strcat(szFTool, NullStr);

	strQry.Format("{ CALL SAVE_FTOOL('%07d', '%02d', '%s')}", pMover->m_idPlayer, g_appInfo.dwSys, szFTool);
	if (qry->Exec(strQry) == FALSE)
	{
		WriteLog("%s - %07d, %02d, %s", __FUNCTION__, pMover->m_idPlayer, g_appInfo.dwSys, szFTool);
	}
}
#endif