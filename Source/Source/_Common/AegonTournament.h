#pragma once
#ifdef __AEGON_TOURNAMENT
#define AT_MAX_TEAMS 8

#define AT_TOURNAMENT_MAP WI_WORLD_MIKSIKPVP

#include "Continent.h"
#include <ctime>;

//tournament commands
#define AT_JOIN_TEAM		0
#define AT_LEAVE_TEAM		1
#define AT_INVITE			2
#define AT_KICK				3
#define AT_TOGGLE_OPEN		4
#define AT_TRANSFER_LEAD	5
#define AT_UNIT_REFRESH		6
#define AT_RENAME_TEAM		7

#define AT_ENTRY_TIME		180000	//time between tournament opening and first round
#define AT_TELEPORT_OUT		5000	//time between round end and teleporting out
#define AT_ROUND_END_TIME	20000	//time between rounds	
#define AT_PVP_WAIT_TIME	10000
#define AT_SEND_CLOSED		10000
#define AT_INTERVAL_CHECK	2000
#define AT_NEXT_ROUND_WAIT	20000

//caption states
#define ATC_CLOSED			0
#define ATC_ENTRY_TIME		1
#define ATC_WAIT_PVP		2
#define ATC_WAIT_NEXT_ROUND	3
#define ATC_NEXT_TEAMS		4
#define ATC_WAIT_ROUND_END	5
#define ATC_ROUND_ENDED		6



typedef struct __TOURNAMENT_TEAM_UNIT
{
	DWORD m_dwTeamID;
	char m_szName[64];
	u_long m_dwTeamOwner;
	vector<u_long> m_vPlayerIDs;

	__TOURNAMENT_TEAM_UNIT()
	{
		Init();
	}
	void Init() 
	{
		m_dwTeamID = NULL_ID;
		memset(m_szName, 0, 64);
		m_dwTeamOwner = NULL_ID;
		m_vPlayerIDs.clear();
	}
	void Write(CAr& ar) 
	{
		ar << m_dwTeamID;
		ar.WriteString(m_szName);
		ar << m_dwTeamOwner;
		
		ar << static_cast<unsigned int>(m_vPlayerIDs.size());
		for (int i = 0; i < m_vPlayerIDs.size(); i++) 
		{
			ar << m_vPlayerIDs[i];
		}
	}
	void Read(CAr& ar)
	{
		ar >> m_dwTeamID;
		ar.ReadString(m_szName, 64);
		ar >> m_dwTeamOwner;

		m_vPlayerIDs.clear();
		unsigned long nSize;
		ar >> nSize;
		for (int i = 0; i < nSize; i++) 
		{
			u_long tmpID;
			ar >> tmpID;
			m_vPlayerIDs.push_back(tmpID);
		}
	}

}__TOURNAMENT_TEAM_UNIT;


typedef struct __TOURNAMENT_TEAMS
{
	vector<__TOURNAMENT_TEAM_UNIT> m_vTeams;
	__TOURNAMENT_TEAMS()
	{
		Init();
	}
	void Init()
	{
		m_vTeams.clear();
	}
	void Write(CAr& ar)
	{
		ar << static_cast<unsigned long>(m_vTeams.size());
		for (int i = 0; i < m_vTeams.size(); i++) 
		{
			m_vTeams[i].Write(ar);
		}
	}
	void Read(CAr& ar)
	{
		m_vTeams.clear();

		unsigned long nSize;
		ar >> nSize;
		for (int i = 0; i < nSize; i++) 
		{
			__TOURNAMENT_TEAM_UNIT tmpUnit;
			tmpUnit.Read(ar);
			m_vTeams.push_back(tmpUnit);
		}
	}
}__TOURNAMENT_TEAMS;

typedef struct __TOURNAMENT_SINGLE_REWARD
{
	DWORD m_dwItemID;
	DWORD m_dwItemNum;

	__TOURNAMENT_SINGLE_REWARD() 
	{
		Init();
	}
	void Init() 
	{
		m_dwItemID = 0;
		m_dwItemNum = 0;
	}
	void Write(CAr& ar) 
	{
		ar << m_dwItemID;
		ar << m_dwItemNum;
	}
	void Read(CAr& ar)
	{
		ar >> m_dwItemID;
		ar >> m_dwItemNum;
	}
	BOOL IsValid() 
	{
		if (m_dwItemID >= prj.m_aPropItem.GetSize())
			return FALSE;
		if (m_dwItemNum == 0 || m_dwItemNum > 9999)
			return FALSE;

		return TRUE;
	}


}__TOURNAMENT_SINGLE_REWARD;

typedef struct __TOURNAMENT_REWARD
{
	vector<__TOURNAMENT_SINGLE_REWARD> m_vRewards;
	__TOURNAMENT_REWARD()
	{
		Init();
	}
	void Init()
	{
		m_vRewards.clear();
	}
	void Write(CAr& ar)
	{
		ar << static_cast<unsigned long>(m_vRewards.size());
		for (int i = 0; i < m_vRewards.size(); i++) 
		{
			m_vRewards[i].Write(ar);
		}
	}
	void Read(CAr& ar)
	{

		m_vRewards.clear();

		unsigned long nSize;
		ar >> nSize;
		__TOURNAMENT_SINGLE_REWARD tmpUnit;
		for (int i = 0; i < nSize; i++) 
		{
			tmpUnit.Init();
			tmpUnit.Read(ar);
			m_vRewards.push_back(tmpUnit);
		}
	}
	void WriteToString(char* szBuff) 
	{
		for (int i = 0; i < m_vRewards.size(); i++)
		{
			char szTmp[128] = { 0, };
			sprintf(szTmp, "%d,%d", m_vRewards[i].m_dwItemID, m_vRewards[i].m_dwItemNum);
			
			if (i == m_vRewards.size() - 1)
			{
				strcat(szTmp, "/");
			}
			else
			{
				strcat(szTmp, ",");
			}
			strcat(szBuff, szTmp);
		}
	}
	void ReadFromString(int* nPos, char* szBuff)
	{
		m_vRewards.clear();

		while (szBuff[*nPos] != '/') 
		{
			__TOURNAMENT_SINGLE_REWARD tmpReward;
			tmpReward.m_dwItemID = GetIntPaFromStr(szBuff, nPos);
			tmpReward.m_dwItemNum = GetIntPaFromStr(szBuff, nPos);
			if(tmpReward.IsValid())
				m_vRewards.push_back(tmpReward);
		}
	}

}__TOURNAMENT_REWARD;

typedef struct __TOURNAMENT_UNIT
{
	DWORD m_dwTournamentID;
	DWORD m_dwTeamSize;
	DWORD m_dwRounds;
	DWORD m_dwRoundTime;
	tm m_sStartTime;
	BOOL m_bFriendlyFire;
	DWORD m_dwLevelReqMin;
	DWORD m_dwLevelReqMax;
	DWORD m_dwAllowedClasses;
	DWORD m_dwTeamReqNum;

	__TOURNAMENT_REWARD sRewardFirstPlace;
	__TOURNAMENT_REWARD sRewardSecondPlace;
	__TOURNAMENT_REWARD sRewardAll;

	__TOURNAMENT_TEAMS m_sTeams;

	__TOURNAMENT_UNIT() 
	{
		Init();
	}
	void Init() 
	{
		m_dwTournamentID = NULL_ID;
		m_dwTeamSize = 0;
		m_dwRounds = 0;
		m_dwRoundTime = 0;
		memset(&m_sStartTime, 0, sizeof(tm));
		m_bFriendlyFire = FALSE;
		m_dwLevelReqMin = 0;
		m_dwLevelReqMax = 0;
		m_dwAllowedClasses = 0;
		m_dwTeamReqNum = 0;

		sRewardFirstPlace.Init();
		sRewardSecondPlace.Init();
		sRewardAll.Init();

		m_sTeams.Init();
	}
	BOOL IsValid() 
	{
		if (m_sStartTime.tm_hour < 0 || m_sStartTime.tm_hour > 23)
			return FALSE;
		if (m_sStartTime.tm_min < 0 || m_sStartTime.tm_min > 59)
			return FALSE;
		if (m_sStartTime.tm_mday <= 0 || m_sStartTime.tm_mday > 31)
			return FALSE;
		if (m_sStartTime.tm_mon < 0 || m_sStartTime.tm_mon > 11)
			return FALSE;

		if (m_dwTournamentID == NULL_ID)
			return FALSE;
		if (m_dwTeamSize == 0)
			return FALSE;
		
		if (m_dwRounds == 0)
			return FALSE;

		if (m_dwRoundTime == 0)
			return FALSE;

		if (m_dwLevelReqMin > MAX_CHARACTER_LEVEL || m_dwLevelReqMax == 0)
			return FALSE;

		return TRUE;
	}
	void Write(CAr& ar)
	{
		ar << m_dwTournamentID;
		ar << m_dwTeamSize;
		ar << m_dwRounds;
		ar << m_dwRoundTime;
		ar << m_sStartTime;
		ar << m_bFriendlyFire;
		ar << m_dwLevelReqMin;
		ar << m_dwLevelReqMax;
		ar << m_dwAllowedClasses;
		ar << m_dwTeamReqNum;

		sRewardFirstPlace.Write(ar);
		sRewardSecondPlace.Write(ar);
		sRewardAll.Write(ar);

		m_sTeams.Write(ar);
	}
	void Read(CAr& ar)
	{
		ar >> m_dwTournamentID;
		ar >> m_dwTeamSize;
		ar >> m_dwRounds;
		ar >> m_dwRoundTime;
		ar >> m_sStartTime;
		ar >> m_bFriendlyFire;
		ar >> m_dwLevelReqMin;
		ar >> m_dwLevelReqMax;
		ar >> m_dwAllowedClasses;
		ar >> m_dwTeamReqNum;

		sRewardFirstPlace.Read(ar);
		sRewardSecondPlace.Read(ar);
		sRewardAll.Read(ar);

		m_sTeams.Read(ar);
	} 
	void WriteRewardsToString(char* szBuff) 
	{
		sRewardAll.WriteToString(szBuff);
		sRewardFirstPlace.WriteToString(szBuff);
		sRewardSecondPlace.WriteToString(szBuff);
	}
	void ReadRewardsFromString(char* szBuff, int* nPos)
	{
		sRewardAll.ReadFromString(nPos, szBuff);
		(*nPos)++;
		sRewardFirstPlace.ReadFromString(nPos, szBuff);
		(*nPos)++;
		sRewardSecondPlace.ReadFromString(nPos, szBuff);
	}

	BOOL IsReadyToStart() 
	{
		time_t startTime = mktime(&m_sStartTime);
		time_t nowTime = time_null();

		return startTime <= nowTime;
	}

}__TOURNAMENT_UNIT;
//single layer
class CAegonActiveTournamentLayer
{
public:

	CAegonActiveTournamentLayer();
	virtual ~CAegonActiveTournamentLayer();

	void BeforeDelete();
#ifdef __WORLDSERVER
	BOOL Process();
#endif

};

//contains multiple tournament layers
class CAegonTournamentCtrlUnit
{
	BOOL m_bTournamentFinished;
	DWORD m_dwTournamentID;
	vector<CAegonActiveTournamentLayer*> m_vTournamentLayers;

public:
	CAegonTournamentCtrlUnit();
	virtual ~CAegonTournamentCtrlUnit();

#ifdef __WORLDSERVER
	BOOL InitializeTournament(DWORD dwTournamentID);
	void DeleteLayers();
	BOOL Process();
#endif
};


class CAegonTournamentMng
{
private:

public:
	enum { ATMNG_CLOSE = 1, ATMNG_ENTRY, ATMNG_INIT_NEXT_ROUND, ATMNG_WAIT_NEXT_ROUND, ATMNG_START_NEXT_ROUND, ATMNG_ROUND_ENDED, ATMNG_TOURNAMENT_END };
	LPCSTR m_scriptFile = "AegonTournament.inc";

#ifdef __CLIENT
	vector<__TOURNAMENT_UNIT> m_vTournamentPresets;
#endif

	map<DWORD, __TOURNAMENT_UNIT> m_mQueuedTournaments;

	BOOL m_bScriptLoaded;
#ifdef __WORLDSERVER
	DWORD m_dwNextID;
	DWORD m_dwLastStartCheck;
#endif
	BOOL m_bTournamentInProgress;

	CAegonTournamentCtrlUnit* m_pTournamentCtrlUnit;

	CAegonTournamentMng();
	virtual ~CAegonTournamentMng();
	static CAegonTournamentMng* GetInstance(void);

#ifdef __CLIENT
	BOOL LoadScript();
#endif
	void WriteTournamentMap(CAr& ar);
	void ReadTournamentMap(CAr& ar);
	__TOURNAMENT_UNIT* GetTournamentByID(DWORD dwTournamentID);
	__TOURNAMENT_UNIT* GetTournamentByIndex(int nIndex);
	BOOL HasActiveTournament();

#ifdef __WORLDSERVER	
	void Process();
	void OnTournamentCreation();
	void OnTournamentLoad();
	void CalculateNextID();
	DWORD GetNextID();
	void DeleteTournament(DWORD dwTournamentID, CUser* pUser = NULL);
	DWORD GetNextStartTournament();
	void StartTournament(DWORD dwTournamentID);
	void EndTournament();
#endif // __WORLDSERVER
};


#endif // __AEGON_TOURNAMENT