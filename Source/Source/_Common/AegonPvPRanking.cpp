#include "stdafx.h"
#include "AegonPvPRanking.h"
#include "defineObj.h"
#include "defineItem.h"
#include "defineText.h"

#if __VER >= 14 // __NEW_CONTINENT
#include "Continent.h"
#endif // __NEW_CONTINENT

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
#include "Party.h"
extern	CPartyMng			g_PartyMng;
#if __VER >= 14 // __INSTANCE_DUNGEON
#include "InstanceDungeonParty.h"
#endif //  __INSTANCE_DUNGEON
#endif // __WORLDSERVER

#ifdef __CLIENT
#include "ResData.h"
#include "WndManager.h"

#include "DPClient.h"
extern CDPClient g_DPlay;
#endif // __CLIENT

#ifdef __AEGON_PVP_RANKING

CAegonPvPRankingMng::CAegonPvPRankingMng()
{

	playerRankingMap.clear();

#ifdef __WORLDSERVER
	g_dpDBClient.SendInitLoadPvPRanking();
#endif // __WORLDSERVER
}

CAegonPvPRankingMng::~CAegonPvPRankingMng()
{
}

CAegonPvPRankingMng* CAegonPvPRankingMng::GetInstance(void)
{
	static CAegonPvPRankingMng sAegonPvPRankingMng;
	return &sAegonPvPRankingMng;
}
void CAegonPvPRankingMng::WriteMap(CAr& ar)
{
#if _WIN64
	ar << static_cast<unsigned long>(playerRankingMap.size());
#else
	ar << playerRankingMap.size();
#endif

	map<u_long, __PVPRANKING_PROGRESS_UNIT>::iterator it = playerRankingMap.begin();
	for (; it != playerRankingMap.end(); it++) 
	{
		it->second.Write(ar);
	}
}
void CAegonPvPRankingMng::ReadMap(CAr& ar)
{
	playerRankingMap.clear();

	unsigned long nSize;
	ar >> nSize;
	for (int i = 0; i < nSize; i++) 
	{
		__PVPRANKING_PROGRESS_UNIT tmpUnit;
		tmpUnit.Init();

		tmpUnit.Read(ar);

		playerRankingMap.insert(make_pair(tmpUnit.m_nPlayerID, tmpUnit));
	}
}
__PVPRANKING_PROGRESS_UNIT* CAegonPvPRankingMng::GetStatUnit(u_long nPlayerID)
{
	map< u_long, __PVPRANKING_PROGRESS_UNIT>::iterator it = playerRankingMap.find(nPlayerID);
	if (it != playerRankingMap.end()) 
	{
		return &it->second;
	}
	return NULL;
}
void CAegonPvPRankingMng::CalculatePointsForAll()
{
	map<u_long, __PVPRANKING_PROGRESS_UNIT>::iterator it = playerRankingMap.begin();
	for (; it != playerRankingMap.end(); it++) 
	{
		it->second.CalculatePoints();
	}
}
#ifdef __WORLDSERVER
void CAegonPvPRankingMng::SaveStatUnit(u_long nPlayerID)
{
	__PVPRANKING_PROGRESS_UNIT* saveUnit = GetStatUnit(nPlayerID);
	if (saveUnit) 
	{
		g_dpDBClient.SendSavePvPRanking(saveUnit);
	}

}
void CAegonPvPRankingMng::PvPRankingOnJoin(CUser* pUser)
{
	if (IsValidObj(pUser))
	{

		map<u_long, __PVPRANKING_PROGRESS_UNIT>::iterator it = playerRankingMap.find(pUser->m_idPlayer);
		if (it == playerRankingMap.end())
		{
			__PVPRANKING_PROGRESS_UNIT newUnit;
			newUnit.Init();
			newUnit.m_nPlayerID = pUser->m_idPlayer;
			strncpy(newUnit.m_szName, pUser->GetName(), 32);
			newUnit.CalculatePoints();

			playerRankingMap.insert(make_pair(pUser->m_idPlayer, newUnit));

			SaveStatUnit(pUser->m_idPlayer);
		}

	}
}
#endif // __WORLDSERVER
#endif // __AEGON_PVP_RANKING