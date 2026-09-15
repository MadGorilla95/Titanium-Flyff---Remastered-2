#include "stdafx.h"
#include "AegonLastOnlineMng.h"
#include "defineObj.h"
#include "defineItem.h"
#include "defineText.h"


#ifdef __WORLDSERVER
#include "User.h"
#include "DPCoreClient.h"
extern  CDPCoreClient g_DPCoreClient;
#include "DPDatabaseClient.h"
extern	CDPDatabaseClient g_dpDBClient;
extern	CGuildMng	g_GuildMng;
#include "worldmng.h"
extern	CWorldMng	g_WorldMng;
#include "DPSrvr.h"
extern CDPSrvr g_DPSrvr;
extern CUserMng g_UserMng;
#endif // __WORLDSERVER

#ifdef __CLIENT
#include "ResData.h"
#include "WndManager.h"

#include "DPClient.h"
extern CDPClient g_DPlay;
#endif // __CLIENT

#ifdef __AEGON_LAST_ONLINE

CAegonLastOnlineMng::CAegonLastOnlineMng()
{
	m_mapLastOnline.clear();

#ifdef __WORLDSERVER
	g_dpDBClient.SendInitLoadLastLogin();
#endif // __WORLDSERVER
}

CAegonLastOnlineMng::~CAegonLastOnlineMng()
{
}

CAegonLastOnlineMng* CAegonLastOnlineMng::GetInstance(void)
{
	static CAegonLastOnlineMng sAegonLastOnlineMngMng;
	return &sAegonLastOnlineMngMng;
}

__LAST_ONLINE_UNIT* CAegonLastOnlineMng::GetLastOnlineByName(CString playerName)
{
	map<u_long, __LAST_ONLINE_UNIT>::iterator it = m_mapLastOnline.begin();
	for (; it != m_mapLastOnline.end(); it++)
	{
		if (playerName.Compare(it->second.m_szName) == 0)
		{
			return &it->second;
		}
	}
	return NULL;
}


__LAST_ONLINE_UNIT* CAegonLastOnlineMng::GetLastOnlineByPlayerID(u_long playerID) 
{
	map<u_long, __LAST_ONLINE_UNIT>::iterator it = m_mapLastOnline.find(playerID);
	if (it != m_mapLastOnline.end())
	{
		return &it->second;
	}
	return NULL;
}

#ifdef __DBSERVER
void CAegonLastOnlineMng::WriteMap(CAr& ar)
{
#if _WIN64
	ar << static_cast<unsigned long>(m_mapLastOnline.size());
#else
	ar << m_mapLastOnline.size();
#endif
	map<u_long, __LAST_ONLINE_UNIT>::iterator it = m_mapLastOnline.begin();
	for (; it != m_mapLastOnline.end(); it++) 
	{
		ar.WriteString(it->second.m_szName);
		ar << it->second.playerID;
		ar << it->second.m_nLastOnline;
		ar << it->second.m_bOnline;
	}
}
#endif // __DBSERVER

#ifdef __WORLDSERVER
void CAegonLastOnlineMng::WriteForVec(CAr& ar, vector<u_long> playerIDs)
{
#if _WIN64
	ar << static_cast<unsigned long>(playerIDs.size());
#else
	ar << playerIDs.size();
#endif
	for (int i = 0; i < playerIDs.size(); i++) 
	{
		__LAST_ONLINE_UNIT* tmpUnit = GetLastOnlineByPlayerID(playerIDs[i]);
		if (tmpUnit) 
		{
			ar.WriteString(tmpUnit->m_szName);
			ar << tmpUnit->playerID;
			ar << tmpUnit->m_nLastOnline;
			ar << tmpUnit->m_bOnline;
		}
		else 
		{
			ar.WriteString("a");
			ar << 0;
			ar << 0;
			ar << FALSE;
		}
	}
}

void CAegonLastOnlineMng::OnUserLogin(CUser* pUser) 
{
	if (!pUser)
		return;

	map<u_long, __LAST_ONLINE_UNIT>::iterator it = m_mapLastOnline.find(pUser->m_idPlayer);
	if (it != m_mapLastOnline.end()) 
	{
		if (strcmp(pUser->GetName(), it->second.m_szName) != 0)
			sprintf(it->second.m_szName, pUser->GetName());
		_time64(&it->second.m_nLastOnline);
		it->second.m_bOnline = TRUE;
	}
	else 
	{
		__LAST_ONLINE_UNIT tmpUnit;
		tmpUnit.Init();
		sprintf(tmpUnit.m_szName, "%s", pUser->GetName());
		tmpUnit.playerID = pUser->m_idPlayer;
		_time64(&tmpUnit.m_nLastOnline);
		tmpUnit.m_bOnline = TRUE;
		
		if (m_mapLastOnline.find(pUser->m_idPlayer) == m_mapLastOnline.end()) 
		{
			m_mapLastOnline.insert(make_pair(pUser->m_idPlayer, tmpUnit));
		}
	}

	DistributeRefreshs(pUser);
}
void CAegonLastOnlineMng::OnUserLogout(CUser* pUser)
{
	if (!pUser)
		return;

	map<u_long, __LAST_ONLINE_UNIT>::iterator it = m_mapLastOnline.find(pUser->m_idPlayer);
	if (it != m_mapLastOnline.end())
	{
		if (strcmp(pUser->GetName(), it->second.m_szName) != 0)
			sprintf(it->second.m_szName, pUser->GetName());
		_time64(&it->second.m_nLastOnline);
		it->second.m_bOnline = FALSE;
	}
	else
	{
		__LAST_ONLINE_UNIT tmpUnit;
		tmpUnit.Init();
		sprintf(tmpUnit.m_szName, "%s", pUser->GetName());
		tmpUnit.playerID = pUser->m_idPlayer;
		_time64(&tmpUnit.m_nLastOnline);
		tmpUnit.m_bOnline = FALSE;

		if (m_mapLastOnline.find(pUser->m_idPlayer) == m_mapLastOnline.end())
		{
			m_mapLastOnline.insert(make_pair(pUser->m_idPlayer, tmpUnit));
		}
	}

	DistributeRefreshs(pUser);
}

void CAegonLastOnlineMng::DistributeRefreshs(CUser* pUser) 
{
	//send refresh to all players in friendlist and guild

	if (IsInvalidObj(pUser))
		return;

	map<u_long, Friend>::iterator itFriend = pUser->m_RTMessenger.begin();
	for (; itFriend != pUser->m_RTMessenger.end(); itFriend++)
	{
		CUser* friendUser = g_UserMng.GetUserByPlayerID(itFriend->first);
		if (friendUser)
		{
			friendUser->AddSingleLastOnlineRefresh(pUser->m_idPlayer);
		}
	}

	CGuild* userGuild = pUser->GetGuild();
	if (userGuild)
	{
		map<u_long, CGuildMember*>::iterator itGuild = userGuild->m_mapPMember.begin();
		for (; itGuild != userGuild->m_mapPMember.end(); itGuild++)
		{
			CUser* guilddUser = g_UserMng.GetUserByPlayerID(itGuild->first);
			if (guilddUser)
			{
				guilddUser->AddSingleLastOnlineRefresh(pUser->m_idPlayer);
			}
		}
	}
}
#endif // __WORLDSERVER

#if defined(__CLIENT) || defined(__WORLDSERVER)
void CAegonLastOnlineMng::ReadMap(CAr& ar)
{
	int nSize;
	m_mapLastOnline.clear();
	ar >> nSize;
	__LAST_ONLINE_UNIT tmpUnit;

	for (int i = 0; i < nSize; i++) 
	{
		tmpUnit.Init();
		ar.ReadString(tmpUnit.m_szName);
		ar >> tmpUnit.playerID;
		ar >> tmpUnit.m_nLastOnline;
		ar >> tmpUnit.m_bOnline;

		if(m_mapLastOnline.find(tmpUnit.playerID) == m_mapLastOnline.end())
			m_mapLastOnline.insert(make_pair(tmpUnit.playerID, tmpUnit));
	}
}
#endif // __CLIENT || __WORLDSERVER

#ifdef __CLIENT
CString CAegonLastOnlineMng::GetLastOnlineString(u_long playerID)
{
	CString out = "Not logged in yet.";
	CString lo = "Last online: ";
	__time64_t lastOnline;
	int month = 0;
	int days = 0;
	int hours = 0;
	int minutes = 0;
	int seconds = 0;
	
	__LAST_ONLINE_UNIT* tmpUnit = GetLastOnlineByPlayerID(playerID);
	if (tmpUnit) 
	{
		_time64(&lastOnline);
		lastOnline -= tmpUnit->m_nLastOnline;

		if (tmpUnit->m_bOnline) 
		{
			out.Format("Online");
		}
		else 
		{
			while (lastOnline > 0)
			{
				if (lastOnline >= (60 * 60 * 24 * 30))
				{
					month++;
					lastOnline -= (60 * 60 * 24 * 30);
				}
				else if (lastOnline >= (60 * 60 * 24))
				{
					days++;
					lastOnline -= (60 * 60 * 24);
				}
				else if (lastOnline >= (60 * 60))
				{
					hours++;
					lastOnline -= (60 * 60);
				}
				else if (lastOnline >= 60)
				{
					minutes++;
					lastOnline -= 60;
				}
				else
				{
					seconds = lastOnline;
					lastOnline = 0;
				}

			}

			if (month > 0)
			{
				out.Format(lo + "%d month(s) ago.", month);
			}
			else if (days > 0)
			{
				out.Format(lo + "%d day(s) ago.", days);
			}
			else if (hours > 0)
			{
				out.Format(lo + "%d hour(s) ago.", hours);
			}
			else if(minutes > 0)
			{
				out.Format(lo + "%d minute(s) ago.", minutes);
			}
			else
			{
				out.Format(lo + "%d second(s) ago.", seconds);
			}
		}
	}

	return out;
}
#endif // __CLIENT
#endif // __AEGON_LAST_ONLINE