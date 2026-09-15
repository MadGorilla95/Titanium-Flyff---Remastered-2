#include "stdafx.h"
#include "AegonTournament.h"
#include "defineObj.h"
#include "defineItem.h"

#ifdef __WORLDSERVER
#include "User.h"
#include "defineText.h"
#include "DPCoreClient.h"
extern  CDPCoreClient g_DPCoreClient;
#include "DPDatabaseClient.h"
extern	CDPDatabaseClient g_dpDBClient;
extern	CGuildMng	g_GuildMng;
#include "worldmng.h"
extern	CWorldMng	g_WorldMng;
#include "DPSrvr.h"
extern CDPSrvr g_DPSrvr;
extern	CUserMng g_UserMng;
extern	CDPCoreClient g_DPCoreClient;
#endif // __WORLDSERVER


#ifdef __AEGON_TOURNAMENT
CAegonTournamentMng::CAegonTournamentMng()
{
#ifdef __CLIENT
	m_bScriptLoaded = LoadScript();
	if (!m_bScriptLoaded)
	{
		m_vTournamentPresets.clear();
	}
#endif
#ifdef __WORLDSERVER
	m_dwNextID = 0;
	m_dwLastStartCheck = 0;
#endif
	m_bTournamentInProgress = FALSE;
	m_pTournamentCtrlUnit = NULL;
}
CAegonTournamentMng::~CAegonTournamentMng()
{
}
CAegonTournamentMng* CAegonTournamentMng::GetInstance(void)
{
	static CAegonTournamentMng sTournamentMng;
	return &sTournamentMng;
}
#ifdef __CLIENT
BOOL CAegonTournamentMng::LoadScript()
{
	DWORD dwPresetID = 0;
	__TOURNAMENT_UNIT tmpUnit;
	int count = 0;

	CScript s;

	if (s.Load(m_scriptFile))
	{
		s.GetToken();
		while (s.tok != FINISHED)
		{
			if (s.Token.Compare("TournamentPreset") == 0)
			{
				tmpUnit.Init();
				s.GetToken(); // {

				while (s.Token.Compare("}") != 0)
				{
					s.GetToken();
					tmpUnit.m_dwTournamentID = dwPresetID;
					dwPresetID++;
					if (s.Token.CompareNoCase("teamsize") == 0)
					{
						tmpUnit.m_dwTeamSize = s.GetNumber();
					}			
					else if (s.Token.CompareNoCase("rounds") == 0)
					{
						tmpUnit.m_dwRounds = s.GetNumber();
					}
					else if (s.Token.CompareNoCase("roundtime") == 0)
					{
						tmpUnit.m_dwRoundTime = s.GetNumber();
					}
					else if (s.Token.CompareNoCase("starttime") == 0)
					{
						tmpUnit.m_sStartTime.tm_hour = s.GetNumber();
						tmpUnit.m_sStartTime.tm_min = s.GetNumber();
						tmpUnit.m_sStartTime.tm_mday = s.GetNumber();
						tmpUnit.m_sStartTime.tm_mon = s.GetNumber() - 1;
						tmpUnit.m_sStartTime.tm_year = s.GetNumber() - 1900;

						//time_t tmpTimeStruct = mktime(&tmpUnit.m_sStartTime);
						//tm* tmpTime = localtime(&tmpTimeStruct);
						//tmpUnit.m_sStartTime = *tmpTime;
					}
					else if (s.Token.CompareNoCase("friendlyfire") == 0)
					{
						tmpUnit.m_bFriendlyFire = s.GetNumber() == 0 ? FALSE : TRUE;
					}
					else if (s.Token.CompareNoCase("levelreq") == 0)
					{
						tmpUnit.m_dwLevelReqMin = s.GetNumber();
						tmpUnit.m_dwLevelReqMax = s.GetNumber();
					}
					else if (s.Token.CompareNoCase("classes") == 0)
					{
						tmpUnit.m_dwAllowedClasses = s.GetNumber();
					}
					else if (s.Token.CompareNoCase("teamsreq") == 0)
					{
						tmpUnit.m_dwTeamReqNum = s.GetNumber();
					}
					else if (s.Token.CompareNoCase("rewardfirst") == 0)
					{
						__TOURNAMENT_SINGLE_REWARD tmpRewardUnit;
						tmpRewardUnit.m_dwItemID = s.GetNumber();
						tmpRewardUnit.m_dwItemNum = s.GetNumber();

						tmpUnit.sRewardFirstPlace.m_vRewards.push_back(tmpRewardUnit);
					}
					else if (s.Token.CompareNoCase("rewardsecond") == 0)
					{
						__TOURNAMENT_SINGLE_REWARD tmpRewardUnit;
						tmpRewardUnit.m_dwItemID = s.GetNumber();
						tmpRewardUnit.m_dwItemNum = s.GetNumber();

						tmpUnit.sRewardSecondPlace.m_vRewards.push_back(tmpRewardUnit);
					}
					else if (s.Token.CompareNoCase("rewardall") == 0)
					{
						__TOURNAMENT_SINGLE_REWARD tmpRewardUnit;
						tmpRewardUnit.m_dwItemID = s.GetNumber();
						tmpRewardUnit.m_dwItemNum = s.GetNumber();

						tmpUnit.sRewardAll.m_vRewards.push_back(tmpRewardUnit);
					}
					else if (s.Token.Compare("}") == 0)
					{
						if (tmpUnit.IsValid())
						{
							m_vTournamentPresets.push_back(tmpUnit);
						}

					}
				}
			}
			s.GetToken();
		}
	}
	else
	{
		Error("failed to Load AegonTournament.inc");
		return FALSE;
	}

	return TRUE;
}
#endif
void CAegonTournamentMng::WriteTournamentMap(CAr& ar)
{
	map<DWORD, __TOURNAMENT_UNIT>::iterator it = m_mQueuedTournaments.begin();
	ar << static_cast<unsigned long>(m_mQueuedTournaments.size());
	for(; it != m_mQueuedTournaments.end(); it++)
	{
		it->second.Write(ar);
	}

}
void CAegonTournamentMng::ReadTournamentMap(CAr& ar)
{
	m_mQueuedTournaments.clear();

	__TOURNAMENT_UNIT tmpUnit;
	unsigned long nSize;
	ar >> nSize;
	for (int i = 0; i < nSize; i++) 
	{
		tmpUnit.Init();
		tmpUnit.Read(ar);
		m_mQueuedTournaments.insert(make_pair(tmpUnit.m_dwTournamentID, tmpUnit));
	}
}

__TOURNAMENT_UNIT* CAegonTournamentMng::GetTournamentByID(DWORD dwTournamentID) 
{

	map<DWORD, __TOURNAMENT_UNIT>::iterator it = m_mQueuedTournaments.find(dwTournamentID);
	if (it != m_mQueuedTournaments.end()) 
	{
		return &it->second;
	}
	return NULL;
}
__TOURNAMENT_UNIT* CAegonTournamentMng::GetTournamentByIndex(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_mQueuedTournaments.size())
		return NULL;

	map<DWORD, __TOURNAMENT_UNIT>::iterator it = CAegonTournamentMng::GetInstance()->m_mQueuedTournaments.begin();
	for (int i = 0; i < nIndex; i++)
	{
		if (it != CAegonTournamentMng::GetInstance()->m_mQueuedTournaments.end())
			it++;
		else
			break;
	}
	if (it != m_mQueuedTournaments.end())
	{
		return &it->second;
	}
	return NULL;
}
BOOL CAegonTournamentMng::HasActiveTournament()
{
	if (m_bTournamentInProgress || m_pTournamentCtrlUnit)
		return TRUE;

	return FALSE;
}
#ifdef __WORLDSERVER
void CAegonTournamentMng::Process()
{
	if (!HasActiveTournament() && GetTickCount() > m_dwLastStartCheck + 5000)
	{
		DWORD dwNextTournament = GetNextStartTournament();
		if (dwNextTournament != NULL_ID) 
			StartTournament(dwNextTournament);

		m_dwLastStartCheck = GetTickCount();
	}

	if (m_pTournamentCtrlUnit) 
	{
		BOOL bTournamentEnded = m_pTournamentCtrlUnit->Process();
		if (bTournamentEnded)
			EndTournament();
	}
}
void CAegonTournamentMng::OnTournamentCreation()
{
	g_dpDBClient.SendSaveTournaments();
	g_UserMng.AddTournamentsChanged();
}
void CAegonTournamentMng::OnTournamentLoad() 
{
	CalculateNextID();
}
void CAegonTournamentMng::CalculateNextID() 
{
	m_dwNextID = 0;
	map<DWORD, __TOURNAMENT_UNIT>::iterator it = m_mQueuedTournaments.begin();
	for (; it != m_mQueuedTournaments.end(); it++)
	{
		if (it->second.m_dwTournamentID >= m_dwNextID)
			m_dwNextID = it->second.m_dwTournamentID + 1;
	}
}
DWORD CAegonTournamentMng::GetNextID() 
{
	DWORD out = m_dwNextID;
	m_dwNextID++;
	return out;
}
void CAegonTournamentMng::DeleteTournament(DWORD dwTournamentID, CUser* pUser)
{
	map<DWORD, __TOURNAMENT_UNIT>::iterator it = m_mQueuedTournaments.find(dwTournamentID);
	if (it != m_mQueuedTournaments.end())
	{
		m_mQueuedTournaments.erase(it);
		if (IsValidObj(pUser))
		{
			CString tmpStr;
			tmpStr.Format("Tournament deleted! id %d", dwTournamentID);
			pUser->AddText(tmpStr.GetString(), COLOR_SUCCESS);
		}
	}
	else 
	{
		if (IsValidObj(pUser))
		{
			CString tmpStr;
			tmpStr.Format("Tournament not found! id: %d", dwTournamentID);
			pUser->AddText(tmpStr.GetString(), COLOR_ERROR);
		}
	}
	g_dpDBClient.SendSaveTournaments();
	g_UserMng.AddTournamentsChanged();
}
DWORD CAegonTournamentMng::GetNextStartTournament()
{
	DWORD dwOut = NULL_ID;
	time_t bestStartTime = 0;

	map<DWORD, __TOURNAMENT_UNIT>::iterator it = m_mQueuedTournaments.begin();
	for (; it != m_mQueuedTournaments.end(); it++)
	{
		if (it->second.IsReadyToStart()) 
		{
			time_t startTime = mktime(&it->second.m_sStartTime);
			if (bestStartTime == 0 || startTime < bestStartTime) 
			{
				dwOut = it->second.m_dwTournamentID;
				bestStartTime = startTime;
			}
		}
	}
	return dwOut;
}
void CAegonTournamentMng::StartTournament(DWORD dwTournamentID)
{
	m_bTournamentInProgress = TRUE;
	m_pTournamentCtrlUnit = new CAegonTournamentCtrlUnit;
	BOOL bSuccess = m_pTournamentCtrlUnit->InitializeTournament(dwTournamentID);
	if (!bSuccess)
		EndTournament();
}

void CAegonTournamentMng::EndTournament()
{
	m_pTournamentCtrlUnit->DeleteLayers();
	SAFE_DELETE(m_pTournamentCtrlUnit);
	m_bTournamentInProgress = FALSE;
}
#endif // __WORLDSERVER





CAegonTournamentCtrlUnit::CAegonTournamentCtrlUnit()
{
	m_vTournamentLayers.clear();
	m_bTournamentFinished = FALSE;
}
CAegonTournamentCtrlUnit::~CAegonTournamentCtrlUnit()
{
}
#ifdef __WORLDSERVER
BOOL CAegonTournamentCtrlUnit::InitializeTournament(DWORD dwTournamentID) 
{
	__TOURNAMENT_UNIT* pUnit = CAegonTournamentMng::GetInstance()->GetTournamentByID(dwTournamentID);
	if (!pUnit) 
		return FALSE;

	g_DPCoreClient.SendCaption("PvP Tournament starting...");

	if (pUnit->m_sTeams.m_vTeams.size() < pUnit->m_dwTeamReqNum) 
	{
		g_DPCoreClient.SendCaption("PvP Tournament cancelled. Not enough participants.");
		return FALSE;
	}

	//TODO

}
void CAegonTournamentCtrlUnit::DeleteLayers() 
{
	for(int i = 0; i < m_vTournamentLayers.size(); i++)
	{
		m_vTournamentLayers[i]->BeforeDelete();
		SAFE_DELETE(m_vTournamentLayers[i]);
	}
}

BOOL CAegonTournamentCtrlUnit::Process() 
{
	BOOL bTournamentEnded = FALSE;

	for (int i = 0; i < m_vTournamentLayers.size(); i++)
	{
		BOOL bTournamentLayerEnded = m_vTournamentLayers[i]->Process();
		if (bTournamentLayerEnded)
		{
			m_vTournamentLayers[i]->BeforeDelete();
			SAFE_DELETE(m_vTournamentLayers[i]);
			m_vTournamentLayers.erase(m_vTournamentLayers.begin() + i);
			i--;
		}
	}

	if (m_vTournamentLayers.size() == 0 && m_bTournamentFinished)
	{
		bTournamentEnded = TRUE;
	}

	return bTournamentEnded;
}
#endif





CAegonActiveTournamentLayer::CAegonActiveTournamentLayer()
{
}
CAegonActiveTournamentLayer::~CAegonActiveTournamentLayer()
{
}
void CAegonActiveTournamentLayer::BeforeDelete()
{

}
#ifdef __WORLDSERVER
BOOL CAegonActiveTournamentLayer::Process()
{
	BOOL bTournamentEnded = FALSE;

	return bTournamentEnded;
}
#endif
#endif // __AEGON_TOURNAMENT