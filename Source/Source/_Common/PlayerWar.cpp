#include "stdafx.h"
#include "defineText.h"
#include "defineobj.h"
#include "PlayerWar.h"
#include "playerdata.h"
#include "defineSound.h"
#include <windows.h> 
#include <stdio.h>
#include "resdata.h"

#ifdef __ROYAL_RUMBLE
#ifdef __WORLDSERVER
#ifdef __RUMBLE_LOGGING
#include "guild.h"
extern CGuildMng g_GuildMng;
#endif //__RUMBLE_LOGGING

#include "stdio.h"
#include "debug.h"
#include "DPSrvr.h"
#include "..\_aiinterface\AIPet.h"
#include "DPDatabaseClient.h"
#include "dpcoreclient.h"
#include "WorldMng.h"
#include "User.h"
#include "..\_Common\Ship.h"
#ifdef __SYS_TICKET
#include "ticket.h"
#endif // __SYS_TICKET
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient		g_DPCoreClient;
extern	CDPSrvr		g_DPSrvr;
extern	CWorldMng	g_WorldMng;
extern	CUserMng	g_UserMng;
#endif //__WORLDSERVER

#include "../XML/XMLWrapper.h"

#define FOR_MAXWARPLAYER for( u_int i=0; i<m_vecWarPlayers.size(); i++) {
#define END_MAXWARPLAYER }

#ifndef __DBSERVER
bool CompType_Points(WAR_PLAYER& first, WAR_PLAYER& second)
{
	if (first.m_nPoints > second.m_nPoints)
		return true;
	else
		return false;
}
#endif // __DBSERVER
CPlayerWar::CPlayerWar()
{
#ifdef __WORLDSERVER
	m_dwWaitTime = 0;
	idLeader = 0;
	m_nState = -1;
	m_dwUserWaitTime = 0;
	_tLastRanking = time(0);

	// Check these locations
#ifdef __RUMBLE_SPAWNFIX
	vPos[0] = D3DXVECTOR3(1248.616f, 101.0f, 1234.101f);
	vPos[1] = D3DXVECTOR3(1264.429f, 101.0f, 1232.440f);
	vPos[2] = D3DXVECTOR3(1268.144f, 101.0f, 1289.473f);
	vPos[3] = D3DXVECTOR3(1249.650f, 101.0f, 1308.957f);
	vPos[4] = D3DXVECTOR3(1215.047f, 101.0f, 1299.843f);
	vPos[5] = D3DXVECTOR3(1207.258f, 101.0f, 1272.108f);
	vPos[6] = D3DXVECTOR3(1213.129f, 101.0f, 1249.948f);
	vPos[7] = D3DXVECTOR3(1231.156f, 101.0f, 1261.541f);
#else //__RUMBLE_SPAWNFIX
	vPos[0] = D3DXVECTOR3(720.0f, 101.0f, 726.0f);
	vPos[1] = D3DXVECTOR3(722.0f, 101.0f, 817.0f);
	vPos[2] = D3DXVECTOR3(774.0f, 101.0f, 882.0f);
	vPos[3] = D3DXVECTOR3(868.0f, 101.0f, 839.0f);
	vPos[4] = D3DXVECTOR3(922.0f, 101.0f, 779.0f);
	vPos[5] = D3DXVECTOR3(876.0f, 101.0f, 715.0f);
	vPos[6] = D3DXVECTOR3(755.0f, 101.0f, 686.0f);
	vPos[7] = D3DXVECTOR3(753.0f, 101.0f, 763.0f);
	vPos[8] = D3DXVECTOR3(829.0f, 101.0f, 776.0f);
#endif //__RUMBLE_SPAWNFIX

	m_bMsg = false;
	m_bWar = false;
	StartTime = NULL;
	m_bStart = false;
#endif // __WORLDSERVER
}

CPlayerWar::~CPlayerWar()
{
}

#ifndef __DBSERVER
int CPlayerWar::GetState()
{
	return m_nState;
}

void CPlayerWar::SetState(int nState)
{
	m_nState = nState;
}

BOOL CPlayerWar::RemoveSiegeUser(u_long idPlayer)
{
	FOR_MAXWARPLAYER
		if (idPlayer == m_vecWarPlayers.at(i).m_idPlayer)
		{
			m_vecWarPlayers.erase(m_vecWarPlayers.begin() + i);
			return TRUE;
		}
	END_MAXWARPLAYER
		return FALSE;
}

BOOL CPlayerWar::CheckUsers(u_long idPlayer, BOOL bRemove)
{
	if (!bRemove)
	{
		FOR_MAXWARPLAYER
			if (m_vecWarPlayers[i].m_idPlayer == idPlayer)
				return FALSE;
		END_MAXWARPLAYER
	}
	int nCount = 0;
	FOR_MAXWARPLAYER
		if (m_vecWarPlayers[i].m_idPlayer == idPlayer)
		{
			nCount++;
			if (nCount > 1)
			{
				if (bRemove)
				{
					if (RemoveSiegeUser(idPlayer) != TRUE)
					{
						Error("CPlayerWar::CheckUsers cant remove player [%s]", m_vecWarPlayers[i].m_szName);
						return FALSE;
					}
				}
				else
				{
					Error("CPlayerWar::CheckUsers user %s, id %d", m_vecWarPlayers[i].m_szName, m_vecWarPlayers[i].m_idPlayer);
					return FALSE;
				}
			}
		}
	END_MAXWARPLAYER
		return TRUE;
}

#ifdef __CLIENT
void CPlayerWar::Paint(C2DRender* p2DRender)
{
	if (g_pPlayer && g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_FWC && GetState() != -1)
	{
		SortUsers();
		int x = g_Option.m_nResWidth - 282;
		int y = 215;
		p2DRender->RenderFillRect(CRect(g_Option.m_nResWidth - 282, 176, g_Option.m_nResWidth - 8, g_Option.m_nResHeight - 410), D3DCOLOR_ARGB(80, 0, 0, 0), D3DCOLOR_ARGB(80, 0, 0, 0), D3DCOLOR_ARGB(10, 0, 0, 0), D3DCOLOR_ARGB(10, 0, 0, 0));
		DWORD dwNameColor = D3DCOLOR_TEMP(210, 255, 255, 250);
		DWORD dwKillColor = D3DCOLOR_TEMP(255, 0, 0, 170);
		p2DRender->TextOut(x + 15, y - 13, prj.GetText(TID_PLAYER_WAR_06), 0xFF32cd32);
		p2DRender->TextOut(x + 48, y - 13, prj.GetText(TID_PLAYER_WAR_07), 0xFF32cd32);
		p2DRender->TextOut(x + 125, y - 13, prj.GetText(TID_PLAYER_WAR_08), 0xFF32cd32);
		p2DRender->TextOut(x + 195, y - 13, prj.GetText(TID_PLAYER_WAR_09), 0xFF32cd32);
		p2DRender->TextOut(x + 225, y - 13, prj.GetText(TID_PLAYER_WAR_10), 0xFF32cd32);
		p2DRender->TextOut(x + 10, y - 10, "____________________________________________", 0xFF32cd32);

		for (u_int i = 0; i < m_vecWarPlayers.size(); i++)
		{
			if (i > 15)
				break;

			int nNameLen = strlen(m_vecWarPlayers[i].m_szName);
			if (nNameLen < 3)
				return;

			if (nNameLen > 11)
			{
				m_vecWarPlayers[i].m_szName[11] = '.';
				m_vecWarPlayers[i].m_szName[12] = '.';
				m_vecWarPlayers[i].m_szName[13] = '\0';
				m_vecWarPlayers[i].m_szName[14] = '\0';
				m_vecWarPlayers[i].m_szName[15] = '\0';
				m_vecWarPlayers[i].m_szName[16] = '\0';
			}
			p2DRender->TextOut(x + 205, y + 5, m_vecWarPlayers[i].m_nKills, dwKillColor);
			p2DRender->TextOut(x + 235, y + 5, m_vecWarPlayers[i].m_nDeaths, 0xFF00bfff);
			p2DRender->TextOut(x + 12, y + 5, i + 1, 0xFF32cd32);
			p2DRender->TextOut(x + 105, y + 5, m_vecWarPlayers[i].m_szName, dwNameColor);
			p2DRender->TextOut(x + 60, y + 5, m_vecWarPlayers[i].m_nPoints, 0xFF32cd32);
			y = y + 13;
		}
	}
}

void CPlayerWar::PaintTime(C2DRender* p2DRender)
{
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
	if (pWndWorld)
	{
		if (pWndWorld->m_dwPlayerSiegeTime > 0 && GetState() != -1)
		{
			CD3DFont* pOldFont = g_Neuz.m_2DRender.GetFont();
			g_Neuz.m_2DRender.SetFont(CWndBase::m_Theme.m_pFontGuildCombatText);

			CTimeSpan ct(((pWndWorld->m_dwPlayerSiegeTime - GetTickCount()) / 1000));
			char sMsg[256] = { 0 };
			if (GetState() == OPEN_STATE)
				sprintf(sMsg, prj.GetText(TID_PLAYER_WAR_11), ct.GetMinutes(), ct.GetSeconds());
			else
				if (GetState() == WAR_STATE)
					sprintf(sMsg, prj.GetText(TID_PLAYER_WAR_12), ct.GetMinutes(), ct.GetSeconds());

			if (ct.GetHours() <= 0 && ct.GetMinutes() <= 0 && ct.GetSeconds() <= 0)
				pWndWorld->m_dwPlayerSiegeTime = 0;

			int nX = pWndWorld->m_rectWindow.Width() / 2;
			int nY = 110;
			CSize cSize = p2DRender->m_pFont->GetTextExtent(sMsg);
			nX -= (cSize.cx / 2);
			p2DRender->TextOut(nX, nY, sMsg, 0xffffffff, 0xff000000);
			g_Neuz.m_2DRender.SetFont(pOldFont);
		}
	}
}
#endif // __CLIENT

void CPlayerWar::SortUsers()
{
	std::sort(m_vecWarPlayers.begin(), m_vecWarPlayers.end(), CompType_Points);
}

#ifdef __WORLDSERVER
WAR_PLAYER CPlayerWar::GetUser(u_long idPlayer)
{
	WAR_PLAYER pPlayer;
	if (m_vecWarPlayers.size() > NULL)
	{
		FOR_MAXWARPLAYER
			if (m_vecWarPlayers[i].m_idPlayer == idPlayer)
			{
				pPlayer = m_vecWarPlayers[i];
				break;
			}
		END_MAXWARPLAYER
	}
	return pPlayer;
}

SIEGE_RANK CPlayerWar::GetRankingUser(u_long idPlayer)
{
	SIEGE_RANK player;
	for (u_int i = 0; i < m_vecRankSiege.size(); i++)
	{
		if (idPlayer == m_vecRankSiege[i].uidPlayer)
		{
			player = m_vecRankSiege[i];
			break;
		}
	}
	return player;
}

void CPlayerWar::UpdateUser(u_long idPlayer, WAR_PLAYER pPlayer)
{
	FOR_MAXWARPLAYER
		if (m_vecWarPlayers[i].m_idPlayer == idPlayer)
			m_vecWarPlayers[i] = pPlayer;
	END_MAXWARPLAYER
}
int CPlayerWar::NewUser(u_long idPlayer)
{
	/*
		Return results:
		-1: User already signed up
		-2: User not correct level
		-3: Rumble is full
		0: Signup successful
	*/
	CUser* pUser = g_UserMng.GetUserByPlayerID(idPlayer);
	if (IsValidObj(pUser))
	{
		if (CheckUsers(pUser->m_idPlayer, FALSE) != TRUE)
		{
			return -1;
		}
		else
			if ((u_int)WARINFO.m_nMaxWarPlayers >= m_vecWarPlayers.size() && !pUser->IsSiegeUser())
			{
				if (pUser->GetLevel() < WARINFO.m_nMinPlayerLevel || pUser->GetLevel() > WARINFO.m_nMaxPlayerLevel)
					return -2;

				WAR_PLAYER pPlayer;
				pPlayer.m_idPlayer = idPlayer;
				pPlayer.m_nDeaths = 0;
				pPlayer.m_nKills = 0;
				pPlayer.m_nPoints = 0;
				pPlayer.m_nJob = pUser->GetJob();
				pPlayer.m_dwRespawnTime = 10;
				lstrcpy(pPlayer.m_szName, pUser->GetName());
				m_vecWarPlayers.push_back(pPlayer);
				pUser->SetRegister(TRUE);
				g_UserMng.AddClientUpdate(0);
#ifdef __RUMBLE_LOGGING
				CString logStr;
				logStr.Format("User %s (%07d) signed up", pUser->GetName(), idPlayer);
				LogRumble(logStr);
#endif //__RUMBLE_LOGGING
				return 0;
			}
			else
				return -3;
	}
	return -4;
}

BOOL CPlayerWar::CheckJob(CUser* pUser)
{
	return TRUE;
}

BOOL CPlayerWar::CheckIP(CUser* pUser)
{
	return TRUE;
}

void CPlayerWar::ProcessJoinWar()
{
	CString str;
	DWORD dwWaitTime = 0;
	FOR_MAXWARPLAYER
		CUser* pUser = g_UserMng.GetUserByPlayerID(m_vecWarPlayers[i].m_idPlayer);
	if (IsValidObj(pUser))
	{
		WAR_PLAYER pDef = GetUser(m_vecWarPlayers[i].m_idPlayer);
		if (pDef.m_idPlayer <= NULL)
			return;

		if (pDef.m_dwRespawnTime == 10)
			pDef.m_dwRespawnTime = ::GetCurrentTime() + SEC(WARINFO.m_nRespawnTime);

		if (pUser->GetWorld()->m_dwWorldID == WI_WORLD_FWC && pUser->GetSiegeFightState() != TRUE)
		{
			dwWaitTime = pDef.m_dwRespawnTime - GetTickCount();
			dwWaitTime /= 1000;
			str.Format(prj.GetText(TID_PLAYER_WAR_14), dwWaitTime);
			if (dwWaitTime < 100 && dwWaitTime > 0)
				pUser->AddText(str, 0xFFFFFFFF);

			if (pDef.m_dwRespawnTime < ::GetCurrentTime() && pDef.m_dwRespawnTime != NULL)
			{
				pDef.m_dwRespawnTime = 0;
				pUser->Replace(g_uIdofMulti, WI_WORLD_FWC, vPos[xRandom(0, 8)], REPLACE_FORCE, nDefaultLayer);
				pUser->SetFightState(TRUE);
				UpdateUser(pDef.m_idPlayer, pDef);
			}
		}
	}
	END_MAXWARPLAYER
}

void CPlayerWar::Process()
{
	if (g_uKey == 101)
	{
		if (GetNextWar() != FALSE)
			OpenWar();

		switch (m_nState)
		{
		case OPEN_STATE:
		{
			if (m_dwWaitTime < ::GetCurrentTime())
			{
				m_dwWaitTime = ::GetCurrentTime() + MIN(WARINFO.m_nWaitTime);
				g_DPCoreClient.SendSystem(prj.GetText(TID_PLAYER_WAR_15));
				sprintf(szMsg, prj.GetText(TID_PLAYER_WAR_16), WARINFO.m_nWaitTime);
				g_DPCoreClient.SendSystem(szMsg);
				g_UserMng.AddClientUpdate(0);
				FOR_MAXWARPLAYER
					CUser* pUser = g_UserMng.GetUserByPlayerID(m_vecWarPlayers[i].m_idPlayer);
				if (IsValidObj(pUser))
				{
					CheckUsers(pUser->m_idPlayer, TRUE);
					pUser->SetFightState(FALSE);
					pUser->AddWarTime(m_dwWaitTime - GetTickCount(), OPEN_STATE);
				}
				END_MAXWARPLAYER
					SetState(WAR_STATE);
			}
		}
		break;
		case WAR_STATE:
		{
			if (m_dwWaitTime < ::GetCurrentTime())
			{
				m_dwWaitTime = ::GetCurrentTime() + MIN(WARINFO.m_nFightTime);
				g_UserMng.AddClientUpdate(0);
				FOR_MAXWARPLAYER
					CUser* pUser = g_UserMng.GetUserByPlayerID(m_vecWarPlayers[i].m_idPlayer);
				if (IsValidObj(pUser))
				{
					if (pUser->GetWorld()->GetID() == WI_WORLD_FWC)
					{
						pUser->Replace(g_uIdofMulti, WI_WORLD_FWC, vPos[xRandom(0, 8)], REPLACE_FORCE, nDefaultLayer);
						pUser->SetFightState(TRUE);
						pUser->AddWarTime(m_dwWaitTime - GetTickCount(), GetState());
						pUser->AddText(prj.GetText(TID_PLAYER_WAR_17));
					}
				}
				END_MAXWARPLAYER
					SetState(CLOSE_STATE);
			}
		}
		break;
		case CLOSE_STATE:
		{
			if (m_vecWarPlayers.size() < (u_int)WARINFO.m_nMinWarPlayers)
#ifdef __RUMBLE_GIVEUP
				if (m_vecWarPlayers.size() < (u_int)WARINFO.m_nMinWarPlayersGiveUp)
#endif //__RUMBLE_GIVEUP
				EndWar();
			else
				if (m_dwWaitTime < ::GetCurrentTime())
					EndWar();

			ProcessJoinWar();
		}
		break;
		case -1:
		{
			const time_t t = time(0);
			if (t > (_tLastRanking + 120))
			{
				g_dpDBClient.LoadSiegeRanking();
				_tLastRanking = t;
			}
		}
			return;
		}
	}
}
void CPlayerWar::EndWar()
{
	SetState(-1);
	if (IsValidObj(g_UserMng.GetUserByPlayerID(idLeader)))
	{
		if (idLeader > 0)
		{
			WAR_PLAYER pLeader = GetUser(idLeader);
			if (pLeader.m_idPlayer)
				sprintf(szMsg, prj.GetText(TID_PLAYER_WAR_18), pLeader.m_szName);
		}
		else
			sprintf(szMsg, prj.GetText(TID_PLAYER_WAR_19));
	}
	// New Ranking Update until DB updates
	auto rank = m_vecRankSiege;
	for (auto wp : m_vecWarPlayers)
	{
		bool exists = false;
		for (int i = 0; i < rank.size(); i++)
		{
			if (wp.m_idPlayer == rank[i].uidPlayer)
			{
				exists = true;
				rank[i].nKills += wp.m_nKills;
				rank[i].nDeaths += wp.m_nDeaths;
				rank[i].nPoints += wp.m_nPoints;
			}
		}

			if (exists) continue;

			SIEGE_RANK sr;
			sr.uidPlayer = wp.m_idPlayer;
			strcpy_s(sr.szName, wp.m_szName);
			sr.nJob = wp.m_nJob;
			sr.nKills = wp.m_nKills;
			sr.nDeaths = wp.m_nDeaths;
			sr.nPoints = wp.m_nPoints;
			m_vecRankSiege.push_back(sr);
	}

	TeleportAllUser(CLOSE_STATE);
	g_dpDBClient.SaveWarPlayers();
	g_DPCoreClient.SendSystem(szMsg);
	m_vecWarPlayers.clear();
	g_UserMng.AddClientUpdate(0);
	idLeader = 0;
	m_bStart = false;
	StartTime = NULL;
#ifdef __RUMBLE_LOGGING
	LogRumble("Rumble ended");
#endif //__RUMBLE_LOGGING
}

void CPlayerWar::OpenWar()
{
	if (g_uKey == 101)
	{
		if (m_vecWarPlayers.size() >= (u_int)WARINFO.m_nMinWarPlayers)
		{
			m_dwWaitTime = ::GetCurrentTime();
			SetState(OPEN_STATE);
		}
		else
		{
			g_DPCoreClient.SendSystem(prj.GetText(TID_PLAYER_WAR_20));
		}
	}
}

BOOL CPlayerWar::LoadScript(LPCSTR lpszFileName)
{
	auto doc = new XMLWrapper(lpszFileName,"FFASettings");
	bool bFound = false;

	auto timeConfig = doc->GetOne("TimeSetting");
	WARINFO.m_nFightTime = timeConfig.GetInt("CombatDuration");
	WARINFO.m_nRespawnTime = timeConfig.GetInt("RespawnTimer");
	WARINFO.m_nPreparationTime = timeConfig.GetInt("PreparationTime");
	WARINFO.m_nWaitTime = timeConfig.GetInt("SignupDelay");

	auto signupConfig = doc->GetOne("SignupSetting");
	WARINFO.m_nMinWarPlayers = signupConfig.GetInt("MinPlayers");
	WARINFO.m_nMaxWarPlayers = signupConfig.GetInt("MaxPlayers");
	
	WARINFO.m_nMinPlayerLevel = signupConfig.GetInt("MinLevel");
	WARINFO.m_nMaxPlayerLevel = signupConfig.GetInt("MaxLevel");
#ifdef __RUMBLE_GIVEUP
	WARINFO.m_nMinWarPlayersGiveUp = signupConfig.GetInt("MinPlayersGiveUp");
#endif //__RUMBLE_GIVEUP

	auto rewardConfig = doc->GetOne("RewardSetting");
	WARINFO.m_dwItemID = rewardConfig.GetID("Item", bFound);
	WARINFO.m_nNum = rewardConfig.GetInt("Amount");

	delete doc;
	/*
	CScript s;
	if (!s.Load(lpszFileName))
		return FALSE;

	while (s.tok != FINISHED)
	{
		if (s.Token == "Time")
		{
			WAR_TIME time;
			time.nHour = (short)s.GetNumber();
			time.nMin = (short)s.GetNumber();
			WARINFO.vecTime.push_back(time);
		}
		else if (s.Token == _T("WaitTime"))
		{
			WARINFO.m_nWaitTime = s.GetNumber();
			if (WARINFO.m_nWaitTime < 0 || WARINFO.m_nWaitTime  > 59)
			{
				Error("CPlayerWar::LoadScript() nWaitTime = %d", WARINFO.m_nWaitTime);
				return FALSE;
			}
		}
		else if (s.Token == _T("FightTime"))
		{
			WARINFO.m_nFightTime = s.GetNumber();
			if (WARINFO.m_nFightTime < 0 || WARINFO.m_nFightTime > 59)
			{
				Error("CPlayerWar::LoadScript() FightTime = %d", WARINFO.m_nFightTime);
				return FALSE;
			}
		}
		else if (s.Token == _T("MinPlayerLevel"))
			WARINFO.m_nMinPlayerLevel = s.GetNumber();
		else if (s.Token == _T("MaxPlayerLevel"))
			WARINFO.m_nMaxPlayerLevel = s.GetNumber();
		else if (s.Token == _T("ItemID"))
		{
			WARINFO.m_dwItemID = s.GetNumber();
			if (WARINFO.m_dwItemID <= 0)
			{
				Error(" [ Player Siege ]: Reward Item (ItemID) is not valid! ");
				return FALSE;
			}
		}
		else if (s.Token == _T("ItemNum"))
		{
			WARINFO.m_nNum = s.GetNumber();
			if (WARINFO.m_nNum <= 0)
			{
				Error(" [ Player Siege ]: ItemNum is not valid! ");
				return FALSE;
			}
		}
		else if (s.Token == _T("MIN_PLAYERS"))
		{
			WARINFO.m_nMinWarPlayers = s.GetNumber();
			if (WARINFO.m_nMinWarPlayers <= 0)
			{
				Error(" [ Player Siege ]: MIN_PLAYERS %d!", WARINFO.m_nMinWarPlayers);
				return FALSE;
			}
		}
		else if (s.Token == _T("MAX_PLAYERS"))
		{
			WARINFO.m_nMaxWarPlayers = s.GetNumber();
			if (WARINFO.m_nMaxWarPlayers <= 0)
			{
				Error(" [ Player Siege ]: MAX_PLAYERS %d!", WARINFO.m_nMaxWarPlayers);
				return FALSE;
			}
		}
		else if (s.Token == _T("RespawnTime"))
		{
			WARINFO.m_nRespawnTime = s.GetNumber();
			if (WARINFO.m_nRespawnTime <= 0)
			{
				Error(" [ Player Siege ]: m_nRespawnTime %d!", WARINFO.m_nRespawnTime);
				return FALSE;
			}
		}
#ifdef __RUMBLE_GIVEUP
		else if (s.Token == _T("MIN_PLAYERS_SURRENDER"))
		{
			WARINFO.m_nMinWarPlayersGiveUp = s.GetNumber();
			if (WARINFO.m_nMinWarPlayersGiveUp <= 0)
			{
				Error(" [Player Siege ]: m_nWinWarPlayersSurrender %d!", WARINFO.m_nMinWarPlayersGiveUp);
				return FALSE;
			}
		}
#endif //__RUMBLE_GIVEUP
		s.GetToken();
	}
	*/
	return TRUE;
}

void CPlayerWar::TeleportAllUser(int nState)
{
	FOR_MAXWARPLAYER
		CUser* pUser = g_UserMng.GetUserByPlayerID(m_vecWarPlayers[i].m_idPlayer);
	if (IsValidObj(pUser))
	{
		if (pUser->GetWorld()->GetID() == WI_WORLD_FWC)
			pUser->Replace(g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3(6968.0f, 0, 3328.8f), REPLACE_NORMAL, nDefaultLayer);

		if (nState == CLOSE_STATE)
		{
			if (pUser->GetWorld()->GetID() == WI_WORLD_FWC)
			{
				pUser->SetFightState(FALSE);
				pUser->Replace(g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3(6968.0f, 0, 3328.8f), REPLACE_NORMAL, nDefaultLayer);
				pUser->SetRegister(FALSE);
				pUser->AddWarTime(0, GetState());
			}
		}
	}
	END_MAXWARPLAYER
}

BOOL CPlayerWar::GetNextWar()
{
	if (StartTime != NULL && GetTickCount() > StartTime) m_bStart = true;
	if ((int)(m_vecWarPlayers.size()) >= WARINFO.m_nMinWarPlayers)
	{
		if (!m_bMsg && !m_bStart)
		{
			g_DPCoreClient.SendSystem(prj.GetText(TID_PLAYER_WAR_21));
			g_DPCoreClient.SendSystem(prj.GetText(TID_PLAYER_WAR_22));

			m_bMsg = true;
			m_bWar = false;
			StartTime = GetTickCount() + MIN(WARINFO.m_nPreparationTime);
			return FALSE;
		}
		if (!m_bWar && m_bStart)
		{
			m_bMsg = false;
			m_bWar = true;
			m_bStart = false;
#ifdef __RUMBLE_LOGGING
			LogRumble("Rumble started");
#endif //__RUMBLE_LOGGING
			return TRUE;
		}
	}
	return FALSE;

}
BOOL CPlayerWar::IsLeader(u_long idPlayer)
{
	if (idPlayer == idLeader)
		return TRUE;
	else
		return FALSE;

	return FALSE;
}

void CPlayerWar::RejoinSiegeFight(CUser* pUser)
{
	if (pUser->GetWorld()->GetID() == WI_WORLD_FWC)
	{
		pUser->SetFightState(TRUE);
		pUser->Replace(g_uIdofMulti, WI_WORLD_FWC, vPos[xRandom(0, 8)], REPLACE_NORMAL, nDefaultLayer);
	}
}

void CPlayerWar::Kill(CUser* pAttacker, CUser* pDefender)
{
	if (IsValidObj(pAttacker) && IsValidObj(pDefender))
	{
		WAR_PLAYER pAtk = GetUser(pAttacker->m_idPlayer);
		WAR_PLAYER pDef = GetUser(pDefender->m_idPlayer);
		if (pAtk.m_idPlayer <= NULL || pDef.m_idPlayer <= NULL)
			return;

		if (pAtk.m_idPlayer == pDef.m_idPlayer)
			return;

		pDef.m_dwRespawnTime = ::GetCurrentTime() + SEC(WARINFO.m_nRespawnTime);
		pAtk.m_nKills++;
		pDef.m_nDeaths++;
		pAtk.m_nPoints++;

		sprintf(szMsg, prj.GetText(TID_PLAYER_WAR_23), pAtk.m_szName, pDef.m_szName);
		int nNum = WARINFO.m_nNum;
		WAR_PLAYER pLeader = GetUser(idLeader);
		if (idLeader <= 0)
		{
			sprintf(szMsg, prj.GetText(TID_PLAYER_WAR_24), pAtk.m_szName, pDef.m_szName);
			pAtk.m_nPoints += 3;
			idLeader = pAtk.m_idPlayer;
		}
		else
		{
			if (pLeader.m_idPlayer <= NULL)
				return;

			if (pAtk.m_nPoints > pLeader.m_nPoints && !IsLeader(pAtk.m_idPlayer))
			{
				if (idLeader <= 0)
					sprintf(szMsg, prj.GetText(TID_PLAYER_WAR_24), pAtk.m_szName, pDef.m_szName);
				else
					sprintf(szMsg, prj.GetText(TID_PLAYER_WAR_25), pAtk.m_szName);

				pAtk.m_nPoints += 3;
				idLeader = pAtk.m_idPlayer;
			}
			else
			{
				if (IsLeader(pDef.m_idPlayer))
				{
					if (pAtk.m_nPoints + 2 > pLeader.m_nPoints && !IsLeader(pAtk.m_idPlayer))
					{
						idLeader = pAtk.m_idPlayer;
						sprintf(szMsg, prj.GetText(TID_PLAYER_WAR_25), pAtk.m_szName);
						pAtk.m_nPoints += 3;
					}
					else
					{
						sprintf(szMsg, prj.GetText(TID_PLAYER_WAR_24), pAtk.m_szName, pDef.m_szName);
						pAtk.m_nPoints += 2;
					}
				}
			}
		}

		g_UserMng.AddWorldMsg(WI_WORLD_FWC, szMsg);
		CreateItem(pAttacker, nNum);
		SortUsers();
		UpdateUser(pAtk.m_idPlayer, pAtk);
		UpdateUser(pDef.m_idPlayer, pDef);
		TeleportToWaitArea(pDefender);
		g_UserMng.AddClientUpdate(0);
#ifdef __RUMBLE_LOGGING
		CString logStr;
		// Get Users
		CUser* pAttacker = (CUser*)prj.GetUserByID(pAtk.m_idPlayer);
		CUser* pDefender = (CUser*)prj.GetUserByID(pDef.m_idPlayer);
		if (pAttacker && pDefender)
		{
			// Get Guilds 
			CGuild* pDefenderGuild = g_GuildMng.GetGuild(pDefender->m_idGuild);
			CGuild* pAttackerGuild = g_GuildMng.GetGuild(pAttacker->m_idGuild);

			if (pAttackerGuild)
				logStr.AppendFormat("[%s]", pAttackerGuild->m_szGuild);

			logStr.AppendFormat("[%s] has killed ", pAttacker->GetName());

			if (pDefenderGuild)
				logStr.AppendFormat("[%s]", pDefenderGuild->m_szGuild);

			logStr.AppendFormat("[%s]", pDefender->GetName());

			LogRumble(logStr);
		}
#endif //__RUMBLE_LOGGING
	}
}

BOOL CPlayerWar::CreateItem(CUser* pAttacker, int nNum)
{
	CItemElem item;
	item.m_dwItemId = WARINFO.m_dwItemID;
	item.m_nItemNum = nNum;
	if (pAttacker->m_Inventory.GetEmptyCount() > 0)
	{
		if (pAttacker->CreateItem(&item))
		{
			sprintf(szMsg, prj.GetText(TID_PLAYER_WAR_26), item.m_nItemNum, item.GetProp()->szName);
			pAttacker->AddText(szMsg, 0xFFFFFFFF);
			return TRUE;
		}
		else
			return FALSE;
	}
	else
	{
		g_dpDBClient.SendQueryPostMail(pAttacker->m_idPlayer, 0, item, 0, "Death Match", "");
		return TRUE;
	}
}

void CPlayerWar::TeleportToWaitArea(CUser* pDefender)
{
	if (IsValidObj(pDefender))
	{
		if (pDefender->GetWorld()->GetID() == WI_WORLD_FWC)
		{
			WAR_PLAYER pPlayer = GetUser(pDefender->m_idPlayer);
			if (pPlayer.m_idPlayer <= NULL)
				return;
			if (pPlayer.m_nDeaths <= 0)
			{
				pDefender->SetFightState(FALSE);
#ifdef __RUMBLE_SPAWNFIX
				pDefender->Replace(g_uIdofMulti, WI_WORLD_FWC, D3DXVECTOR3(1204.320f,105.000f,1370.583f), REPLACE_NORMAL, nDefaultLayer);
#else //__RUMBLE_SPAWNFIX
				pDefender->Replace(g_uIdofMulti, WI_WORLD_FWC, D3DXVECTOR3(1761.0f, 100.0f, 1837.0f), REPLACE_NORMAL, nDefaultLayer);
#endif //__RUMBLE_SPAWNFIX
			}
			else
			{
				pDefender->SetFightState(FALSE);
#ifdef __RUMBLE_SPAWNFIX
				pDefender->Replace(g_uIdofMulti, WI_WORLD_FWC, D3DXVECTOR3(1204.320f, 105.000f, 1370.583f), REPLACE_NORMAL, nDefaultLayer);
#else //__RUMBLE_SPAWNFIX
				pDefender->Replace(g_uIdofMulti, WI_WORLD_FWC, D3DXVECTOR3(1761.0f, 100.0f, 1837.0f), REPLACE_NORMAL, nDefaultLayer);
#endif //__RUMBLE_SPAWNFIX
			}
		}
	}
}

BOOL CPlayerWar::IsSiegeUser(u_long idPlayer)
{
	FOR_MAXWARPLAYER
		if (m_vecWarPlayers[i].m_idPlayer == idPlayer)
			return TRUE;
	END_MAXWARPLAYER

		return FALSE;
}
void CPlayerWar::UserGiveUp(u_long idPlayer)
{
	bool bFind = false;
	WAR_PLAYER wpPlayer;
	// Check if the user is a player 
	FOR_MAXWARPLAYER
		if (idPlayer == m_vecWarPlayers[i].m_idPlayer)
		{
			bFind = true;
			wpPlayer = m_vecWarPlayers[i];
			break;
		}
	END_MAXWARPLAYER

		if (!bFind)
			return;

	// Check if user is still siege on the map
	CUser* pUser = (CUser*)prj.GetUserByID(idPlayer);

	if (!IsValidObj(pUser)) return;

	if (!pUser->GetWorld()) return;

	if (pUser->GetWorld()->GetID() != WI_WORLD_FWC) return;

	// Check if the user still has rewards
	bool bCanSurrender = false;
	CItemElem* pItemElem;

	if (wpPlayer.m_nKills > 0)
	{
		pItemElem = pUser->m_Inventory.GetAtItemId(WARINFO.m_dwItemID);
		if (pItemElem)
			if (pItemElem->m_nItemNum == (int)(wpPlayer.m_nKills * WARINFO.m_nNum))
				bCanSurrender = true;
	}
	else
		bCanSurrender = true;

	if (!bCanSurrender)
	{
		pUser->AddText(prj.GetText(TID_PLAYER_WAR_36), COLOR_ERROR);
		return;
	}

	// Remove rewards, then remove from siege
	if (wpPlayer.m_nKills > 0)
		pUser->RemoveItem((BYTE)(pItemElem->m_dwObjId), (short)(wpPlayer.m_nKills * WARINFO.m_nNum));

	// Remove Player from war
	RemoveSiegeUser(idPlayer);
	pUser->SetRegister(FALSE);

	// Remove Player from map
	pUser->Replace(g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3(6968.0f, 0, 3328.8f), REPLACE_NORMAL, nDefaultLayer);
	pUser->AddText(prj.GetText(TID_PLAYER_WAR_37),COLOR_SUCCESS);
}
#endif // __DBSERVER
bool CPlayerWar::NextState() {
	switch (m_nState)
	{
	case -1:
		if (m_vecWarPlayers.size() > 0) {
			m_nState = OPEN_STATE;
			return true;
		}
		else
			return false;
	case OPEN_STATE:
		return false;
	case WAR_STATE:
		m_nState = CLOSE_STATE;
		return true;
	case WAIT_STATE:
		m_nState = WAR_STATE;
		return true;
	case FIGHT_STATE:
	case CLOSE_STATE:
	default:
		return false;
	}
}
#endif // __WORLDSERVER
CPlayerWar g_War;
#endif // __ROYAL_RUMBLE
