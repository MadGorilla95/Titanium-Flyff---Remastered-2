#include "stdafx.h"
#include "ThreadMng.h"
#include "WorldMng.h"
#include "DPSrvr.h"
#include "User.h"
#include "WorldServer.h"
#include "GlobalTime.h"
#include "party.h"
#include "dpdatabaseclient.h"
#include "guildquest.h"

#ifdef __VENDING_NPC
bool g_bVendLoaded = false;
#endif // __VENDING_NPC

#ifdef	__COLOSSEUM
#include "Colosseum.h"
#endif // __COLOSSEUM

#include "eveschool.h"
extern	CGuildCombat	g_GuildCombatMng;

#ifdef __MIKSIK_EQUALIZED_MODE
BOOL bPresetsLoaded = FALSE;
#endif //__MIKSIK_EQUALIZED_MODE

#if __VER >= 12 // __ITEMCREATEMON_S0602
#if __VER < 12 // __NEW_ITEMCREATEMON_SERVER
#include "CreateMonster.h"
extern	CCreateMonster	g_CreateMonster;
#endif // __NEW_ITEMCREATEMON_SERVER
#endif // __ITEMCREATEMON_S0602

#if __VER >= 12 // __SECRET_ROOM
#include "SecretRoom.h"
#endif // __SECRET_ROOM

#if __VER >= 12 // __NEW_ITEMCREATEMON_SERVER
#include "CreateMonster.h"
#endif // __NEW_ITEMCREATEMON_SERVER

#if __VER >= 12 // __RANGDA_0521
#include "rangda.h"
#endif	// __RANGDA_0521

#if __VER >= 13 // __RAINBOW_RACE
#include "RainbowRace.h"
#endif // __RAINBOW_RACE

#if __VER >= 14 // __PCBANG
#include "PCBang.h"
#endif // __PCBANG

#if __VER >= 14 // __INSTANCE_DUNGEON
#include "InstanceDungeonParty.h"
#endif // __INSTANCE_DUNGEON

#ifdef __NEW_PROFILE
#include "ProfileInfo.h"
#endif // __NEW_PROFILE

#ifdef __AEGON_TOURNAMENT
#include "AegonTournament.h"
#endif // __AEGON_TOURNAMENT

#ifdef __AEGON_LAST_ONLINE
#include "AegonLastOnlineMng.h"
#endif // __AEGON_LAST_ONLINE

#ifdef __SYS_MAP_BUFFS
#include "MapBuffManager.h"
#endif //__SYS_MAP_BUFFS

#ifdef __AEGON_PVP_RANKING
#include "AegonPvPRanking.h"
#endif // __AEGON_PVP_RANKING

#ifdef __SYS_WORLDBOSS
#include "worldboss.h"
#endif // __SYS_WORLDBOSS

#ifdef __AEGON_INGAME_SHOP
#include "AegonIngameShop.h"
#endif

#ifdef __AEGON_PVP_MINIGAMES
#include "AegonPvPMinigames.h"
#endif

#include "dpcoreclient.h"
#include "dpdatabaseclient.h"
#include "spevent.h"
#include "guild.h"
#include "guildwar.h"
extern	CDPCoreClient		g_DPCoreClient;
extern	CDPDatabaseClient	g_dpDBClient;
extern	char				g_szDBAddr[16];
extern	char				g_szCoreAddr[16];
extern	CGuildMng			g_GuildMng;
extern	CGuildWarMng		g_GuildWarMng;

extern	CWorldMng			g_WorldMng;
extern	CDPSrvr				g_DPSrvr;
extern	CUserMng			g_UserMng;
extern  DWORD				g_tmCurrent;
//extern	time_t	g_tCurrent;
extern	char				g_sHeartbeat[32];
extern	CPartyMng			g_PartyMng;

extern  BOOL LoadAIScript();		// aimonster2.cpp

float	r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12;

#if defined(_DEBUG) || defined(__TESTSERVER)
	BOOL g_bProfiling = TRUE;
#else
	BOOL g_bProfiling = FALSE;
#endif

void LogPerformance( DWORD dwCurTick );

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CTimeout
////////////////////////////////////////////////////////////////////////////////////////////////////////

CTimeout::CTimeout( DWORD dwInterval, DWORD dwFirstTimeout )
{
	m_dwInterval		= dwInterval;
	m_endTime	= timeGetTime() + dwFirstTimeout;
}

DWORD CTimeout::Over( DWORD dwCurr )
{
	if( dwCurr > m_endTime )
		return ( dwCurr - m_endTime );
	return 0;
}
void CTimeout::Reset( DWORD dwCurr, DWORD dwTimeout )
{
	if( dwTimeout > m_dwInterval )
		m_endTime	= dwCurr;
	else
		m_endTime	= dwCurr + ( m_dwInterval - dwTimeout );
}
BOOL CTimeout::TimeoutReset( DWORD dwCurr )
{
#ifdef __GAMEGUARD
	if (dwCurr < m_endTime && dwCurr + m_dwInterval < m_endTime)
	{
		m_endTime = dwCurr + m_dwInterval;
		return FALSE;
	}
#endif // __GAMEGUARD
	DWORD dwTimeout	= Over( dwCurr );
	if( dwTimeout > 0 )
	{
		Reset( dwCurr, dwTimeout );
		return TRUE; 
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogPerformance( DWORD dwCurTick )
{
	static int nFrame = 0;			
	static DWORD dwPrev = 0;
	static DWORD dwElapsed = 0;

	++nFrame;
	DWORD dwTick = dwCurTick - dwPrev; 
	dwPrev = dwCurTick;
	dwElapsed += dwTick;

	if( dwTick > 100 )
	{
		SetLogInfo( LOGTYPE_WARN1, "warning - tick:%d", dwTick );

		if( g_bProfiling )
		{
			float sum = r1 + r2 + r3 + r4 + r5 + r6 + r7 + r8 + r9 + r10 + r11 + r12 + 1.0f;

			char szBuffer[256];
			sprintf( szBuffer, 
					"%d, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f\n",
					dwTick,
					( r1 * 100.0f / sum ),
					( r2 * 100.0f / sum ),
					( r3 * 100.0f / sum ),
					( r4 * 100.0f / sum ),
					( r5 * 100.0f / sum ),
					( r6 * 100.0f / sum ),
					( r7 * 100.0f / sum ),
					( r8 * 100.0f / sum ),
					( r9 * 100.0f / sum ),
					( r10 * 100.0f / sum ),
					( r11 * 100.0f / sum ),
					( r12 * 100.0f / sum ) );

			SetLogInfo( LOGTYPE_WARN2, szBuffer );
			OutputDebugString( szBuffer );
		}
	}

#ifdef __INFINITE_0227
	/*if( dwTick > 1000 )
	{
		char szBuffer[256];
			sprintf( szBuffer, 
					"OP: %d, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f\n",
					dwTick, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12 );
			Error( szBuffer );
	}*/
#endif	// __INFINITE_0227

	if (dwElapsed > 1000)
	{
		SetLogInfo(LOGTYPE_BEFORE, "------------------------------");
#ifdef __WS_STAFF_COUNT
		SetLogInfo(LOGTYPE_CCU, "Online Players: %d (Staff: %d)", g_UserMng.GetCount(), g_UserMng.GetStaffCount());
#else //__WS_STAFF_COUNT
		SetLogInfo(LOGTYPE_CCU, "Online Players: %d", g_UserMng.GetCount());
#endif //__WS_STAFF_COUNT
#ifdef __VENDING_NPC
		SetLogInfo(LOGTYPE_VENDOR, "Offline Vendors: %d", prj.m_mapVendorNPC.size());
#endif //__VENDING_NPC
		SetLogInfo(LOGTYPE_DUNGEON, "Active Dungeon Layers: %d", CInstanceDungeonHelper::GetInstance()->m_nIDLayerNum);
		SetLogInfo(LOGTYPE_AFTER, "------------------------------");

		if (g_bProfiling)
			SetLogInfo(LOGTYPE_PERFOMANCE, "Frame: %d | Tick: %d", nFrame, dwTick);

		dwElapsed = 0; // -= 1000
		nFrame = 0;

		r1 = r2 = r3 = r4 = r5 = r6 = r7 = r8 = r9 = r10 = r11 = r12 = 0;
		SetLogInfo(LOGTYPE_REDRAW, "");
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CRunObject
////////////////////////////////////////////////////////////////////////////////////////////////////////
CRunObject::CRunObject()
{
	m_hRunObject	= (HANDLE)0;
	m_hClose	= (HANDLE)0;
}

CRunObject::~CRunObject()
{
	Close();
}

BOOL CRunObject::Init( void )
{
	if( LoadAIScript() == FALSE )
		return FALSE;

// 
	m_hClose	= CreateEvent( NULL, FALSE, FALSE, NULL );
	DWORD dwThreadId;
	m_hRunObject	= chBEGINTHREADEX( NULL, 0, _Run, (LPVOID)this, 0, &dwThreadId ); 
//
	/*
#ifdef __GIFTBOX0213
	if( !CDPAccountClient::GetInstance()->ConnectToServer( s_szAccountAddr, PN_ACCOUNTSRVR_2, TRUE ) )
	{
		OutputDebugString( "Can't connect to account server." );
		return FALSE;
	}
	CGiftboxMan::GetInstance()->Upload( CDPAccountClient::GetInstance() );
#endif	// __GIFTBOX0213
	*/
	if( !g_DPSrvr.StartServer( (u_short)( g_uKey + PN_WORLDSRVR ), TRUE ) )
	{
		OutputDebugString( "Can't start server." );
		return FALSE;
	}
	if( !g_DPCoreClient.Run( g_szCoreAddr, PN_CORESRVR + 0, g_uKey ) )
	{
		OutputDebugString( "Can't connect to core server." );
		return FALSE;
	}
	if( !g_dpDBClient.ConnectToServer( g_szDBAddr, PN_DBSRVR_1, TRUE ) )
	{
		OutputDebugString( "Can't connect to database server." );
		return FALSE;
	}
	if ( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) )
		g_dpDBClient.SendQueryGuildBank();
	if( g_eLocal.GetState( EVE_WORMON ) )
		g_dpDBClient.SendQueryGuildQuest();

#if __VER >= 13 // __RAINBOW_RACE
	if( g_eLocal.GetState( EVE_RAINBOWRACE ) )
		g_dpDBClient.SendRainbowRaceReqLoad();
#endif // __RAINBOW_RACE

#ifdef __INVALID_LOGIN_0320
	g_dpDBClient.CalluspXXXMultiServer( g_uIdofMulti, NULL );
#else	// __INVALID_LOGIN_0320
	g_dpDBClient.CalluspXXXMultiServer( g_uIdofMulti, 0 );
#endif	// __INVALID_LOGIN_0320
	if( !CXMasEvent::GetInstance()->LoadScript( "spevent.txt" ) )
	{
		OutputDebugString( "can't read spevent.txt" );
		return FALSE;
	}
	if( g_eLocal.GetState( EVE_SCHOOL ) )
	{
#ifdef __IDC
		if( !CEveSchool::GetInstance()->LoadPos( "..\\script\\school.txt" ) )	//
#else	// __IDC
		if( !CEveSchool::GetInstance()->LoadPos( "school.txt" ) )
#endif	// __IDC
		{
			OutputDebugString( "[CRITICAL] Missing File -> school.txt" );
			return FALSE;
		}
	}
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) && !g_GuildCombatMng.LoadScript( "GuildCombat.txt" ) )
	{
		OutputDebugString( "[CRITICAL] Missing File ->GuildCombat.txt" );
		return FALSE;
	}
#if __VER >= 12 // __ITEMCREATEMON_S0602
#if __VER < 12 // __NEW_ITEMCREATEMON_SERVER
	if( !g_CreateMonster.LoadScript( "CreateMonster.txt" ) )
	{
		OutputDebugString( "[CRITICAL] Missing File -> CreateMonster.txt" );
		return FALSE;
	}
#endif // __NEW_ITEMCREATEMON_SERVER
#endif // __ITEMCREATEMON_S0602

#ifdef __SYS_MONSTER_CLASH
	CMonsterClashMng::GetInstance()->ReadScript("Monsterclash.inc");
#endif //__SYS_MONSTER_CLASH

#ifdef __AEGON_LAST_ONLINE
	CAegonLastOnlineMng::GetInstance(); // trigger database load
#endif // __AEGON_LAST_ONLINE

#ifdef __AEGON_PVP_RANKING
	CAegonPvPRankingMng::GetInstance(); // trigger database load
#endif // __AEGON_PVP_RANKING

#ifdef __AEGON_INGAME_SHOP
	CAegonIngameShopMng::GetInstance(); // trigger database load
#endif //__AEGON_INGAME_SHOP

#ifdef __AEGON_TOURNAMENT
	g_dpDBClient.SendLoadTournaments();
#endif //__AEGON_TOURNAMENT

#ifdef __ROYAL_RUMBLE
	g_dpDBClient.LoadSiegeRanking();
	g_War.LoadScript("FFASettings.xml");
#endif // __ROYAL_RUMBLE

#ifdef __MIKSIK_EQUALIZED_MODE
	if (!bPresetsLoaded)
	{
		g_dpDBClient.SendGetTmpPresets();
		bPresetsLoaded = TRUE;
	}
#endif //__MIKSIK_EQUALIZED_MODE

	return TRUE;
}

void CRunObject::Close( void )
{
	CLOSE_THREAD( m_hRunObject, m_hClose );
}

u_int CRunObject::_Run( LPVOID pParam )
{
	CRunObject* pRunObject	= (CRunObject*)pParam;
	pRunObject->Run();
	return 0;
}

#ifdef __PROFILE_RUN
	#define PROFILE_RUN( a ) \
			 dwCur = timeGetTime(); \
			 a = (float)( dwCur - dwOld ); \
			 dwOld = dwCur;
#else
	#define PROFILE_RUN( a ) ((void)0);
#endif

void CRunObject::Run( void )
{
	HANDLE hHeartbeat;
	if( !( hHeartbeat = CreateEvent( NULL, FALSE, FALSE, g_sHeartbeat ) ) )
		hHeartbeat	= OpenEvent( EVENT_MODIFY_STATE, FALSE, g_sHeartbeat );
	if( NULL == hHeartbeat )
		Error( "MAPI_E_NOT_ENOUGH_RESOURCES" );
	DWORD dwTickHearbeat = timeGetTime() + SEC( 2 );

	CTimeout	timeoutObject( 67, 0 );
	CTimeout	timeoutReadAgent( 67, 20 );
	CTimeout	timeoutReadCore( 67, 40 );
	CTimeout	timeoutReadTrans( 67, 50 );
	CTimeout	timeoutReadAccount( 67, 60 );
	CTimeout	timeoutRespawn( 1000, 200 );
	CTimeout	timeout( 1000, 600 );
#ifdef __EVENT_1101
	CTimeout	timeoutCallTheRoll( MIN( 1 ), 70 );
#endif	// __EVENT_1101

#ifdef __SYS_MONSTER_CLASH
	CTimeout	timeoutMonsterClash(500, 0);
#endif //__SYS_MONSTER_CLASH

	DWORD dwCur, dwOld;

	while( WaitForSingleObject( m_hClose, 1 ) != WAIT_OBJECT_0 )
	{
		{
			_PROFILE( "CRunObject::Run()" );
			g_tmCurrent		= timeGetTime();
			g_tCurrent	= time( NULL );
			
			dwCur = g_tmCurrent;
			dwOld = dwCur;

			if( timeoutObject.TimeoutReset( g_tmCurrent ) )
			{
				_PROFILE( "CWorldMng::Process()" );
				g_WorldMng.Process();
				if( g_tmCurrent > dwTickHearbeat )
				{
					SetEvent( hHeartbeat );
					dwTickHearbeat = g_tmCurrent + SEC( 2 );
				}
			}
			
			PROFILE_RUN( r1 );

			if( timeoutReadAgent.TimeoutReset( g_tmCurrent ) )
			{
				_PROFILE( "g_DPSrvr.ReceiveMessage()" );
				g_DPSrvr.ReceiveMessage();
			}

			PROFILE_RUN( r2 );

			if( timeoutReadCore.TimeoutReset( g_tmCurrent ) )
			{
				_PROFILE( "g_DPCoreClient.ReceiveMessage()" );
				g_DPCoreClient.ReceiveMessage();
			}

			PROFILE_RUN( r3 );

			if( timeoutReadTrans.TimeoutReset( g_tmCurrent ) )
			{
				_PROFILE( "g_dpDBClient.ReceiveMessage()" );
				g_dpDBClient.ReceiveMessage();
			}

			PROFILE_RUN( r4 );
		
/*
		#ifdef __GIFTBOX0213
			if( timeoutReadAccount.TimeoutReset( g_tmCurrent ) )
				CDPAccountClient::GetInstance()->ReceiveMessage();
		#endif	// __GIFTBOX0213
*/
		#ifdef __EVENTLUA_KEEPCONNECT
			if( timeoutReadAccount.TimeoutReset( g_tmCurrent ) )
				prj.m_EventLua.KeepConnectEventProcess();
		#endif // __EVENTLUA_KEEPCONNECT
			PROFILE_RUN( r5 );

			if( timeoutRespawn.TimeoutReset( g_tmCurrent ) )
			{
				_PROFILE( "g_WorldMng.Respawn()" );
				u_long uRespawned = g_WorldMng.Respawn();
//				if( uRespawned > 0 )
					SetLogInfo( LOGTYPE_RESPAWN, "Respawn:%d Object:%d", uRespawned, g_WorldMng.GetObjCount() );
			}

			PROFILE_RUN( r6 );
			

			if( timeout.TimeoutReset( g_tmCurrent ) )
			{
				_PROFILE( "1SEC_Process" );
				g_GameTimer.Compute();
				CGuildQuestProcessor::GetInstance()->Process();
				CPartyQuestProcessor::GetInstance()->Process();

				PROFILE_RUN( r7 );

				if( g_eLocal.GetState( EVE_GUILDCOMBAT ) )
					g_GuildCombatMng.Process();
	#if __VER >= 11 // __GUILD_COMBAT_1TO1
				if( g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
					g_GuildCombat1to1Mng.Process();
	#endif // __GUILD_COMBAT_1TO1

	#if __VER >= 12 // __SECRET_ROOM
				if( g_eLocal.GetState( EVE_SECRETROOM ) )
				{
					_PROFILE( "CSecretRoomMng::Process()" );
					CSecretRoomMng::GetInstance()->Process();
				}
	#endif // __SECRET_ROOM
	#if __VER >= 13 // __RAINBOW_RACE
				if( g_eLocal.GetState( EVE_RAINBOWRACE ) )
				{
					_PROFILE( "CRainbowRaceMng::Process()" );
					CRainbowRaceMng::GetInstance()->Process();
				}
	#endif // __RAINBOW_RACE
	#if __VER >= 14 // __PCBANG
				{
					_PROFILE( "CPCBang::ProcessPCBang()" );
					CPCBang::GetInstance()->ProcessPCBang();
				}
	#endif // __PCBANG
	#if __VER >= 14 // __INSTANCE_DUNGEON
				{
					CInstanceDungeonParty::GetInstance()->Process();
				}
	#endif // __INSTANCE_DUNGEON

#ifdef __ROYAL_RUMBLE
				g_War.Process();
#endif // __ROYAL_RUMBLE

				PROFILE_RUN( r8 );
						
				if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) )
					g_GuildMng.Process();

				PROFILE_RUN( r9 );

				if( g_eLocal.GetState( EVE_GUILDWAR ) )
					g_GuildWarMng.Process();

				g_PartyMng.PartyMapInfo();

#ifdef __VENDING_NPC
				if (g_bVendLoaded == false)
				{
					g_dpDBClient.SendGetNPCVendor();
					g_bVendLoaded = true;
				}
#endif // __VENDING_NPC

				PROFILE_RUN( r10 );

	#if __VER >= 12 // __RANGDA_0521
				// 랜덤 이벤트 몬스터 초당 1회 틱
				CRangdaController::Instance()->OnTimer();
	#endif	// __RANGDA_0521
	#ifdef __EVENTLUA_SPAWN
				prj.m_EventLua.EventSpawnProcess();
	#endif // __EVENTLUA_SPAWN
	
	#ifdef	__COLOSSEUM
				CColosseumMng::GetInstance()->Process();
	#endif // __COLOSSEUM	

#ifdef __SYS_SANDBOX
				CSandbox::GetInstance()->Process();
#endif //__SYS_SANDBOX

#ifdef __SYS_WORLDBOSS
				CWorldBossController::Instance()->Process();
#endif // __SYS_WORLDBOSS

			}

#ifdef __EVENT_1101
			if( timeoutCallTheRoll.TimeoutReset( g_tmCurrent ) )	// 1분당 
			{
				CEventGeneric::GetInstance()->CallTheRoll();
	#if __VER >= 12 // __NEW_ITEMCREATEMON_SERVER
				CCreateMonster::GetInstance()->ProcessRemoveMonster();
	#endif // __NEW_ITEMCREATEMON_SERVER
			}
	#endif	// __EVENT_1101

				PROFILE_RUN( r11 );

	#ifdef __EVENT_0117
			CEventGeneric::GetInstance()->Spawn();
	#endif	// __EVENT_0117

			PROFILE_RUN( r12 );

#ifdef __SYS_MONSTER_CLASH
			if (timeoutMonsterClash.TimeoutReset(g_tmCurrent))
			{
				CMonsterClashMng::GetInstance()->Process();
			}
#endif //__SYS_MONSTER_CLASH

#ifdef __AEGON_TOURNAMENT
			{
				CAegonTournamentMng::GetInstance()->Process();
			}
#endif // __AEGON_TOURNAMENT

#ifdef __AEGON_PVP_MINIGAMES
			{
				CAegonPvPMinigamesMng::GetInstance()->Process();
			}
#endif

		}
		
		LogPerformance( timeGetTime() );
	
	}
	CLOSE_HANDLE( m_hClose );
	CLOSE_HANDLE( hHeartbeat );
}

CRunObject*	CRunObject::GetInstance( void )
{
	static CRunObject	sRunObject;
	return &sRunObject;
}

void CRunObject::ToggleProfiling()
{
	g_bProfiling = !g_bProfiling; 
}