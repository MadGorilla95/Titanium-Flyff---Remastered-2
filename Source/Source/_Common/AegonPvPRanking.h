#pragma once
#ifndef __AEGPVPRANK__H
#define __AEGPVPRANK__H
#ifdef __AEGON_PVP_RANKING


typedef struct __PVPRANKING_TYPE_UNIT
{
	int m_nKills;
	int m_nDeaths;
	int m_nParticipations;
	int m_nWins;
	int m_nMvp;
	void Init() 
	{
		m_nKills = 0;
		m_nDeaths = 0;
		m_nParticipations = 0;
		m_nWins = 0;
		m_nMvp = 0;
	}
	double GetKDRatio() 
	{
		return ((double)m_nKills / (double)m_nDeaths);
	}
	void WriteToString(char* szProgress) 
	{
		char szTmp[128] = {0, };
		sprintf(szTmp, "%d,%d,%d,%d,%d/", m_nKills, m_nDeaths, m_nParticipations, m_nWins, m_nMvp);
		strcat(szProgress, szTmp);
	}
	void LoadFromString(char* szProgress, int* nPos) 
	{
		m_nKills = GetIntPaFromStr(szProgress, nPos);
		m_nDeaths = GetIntPaFromStr(szProgress, nPos);
		m_nParticipations = GetIntPaFromStr(szProgress, nPos);
		m_nWins = GetIntPaFromStr(szProgress, nPos);
		m_nMvp = GetIntPaFromStr(szProgress, nPos);
	}
	int CalculatePoints() 
	{
		int out = 0;
		out += m_nKills * 20;
		out -= m_nDeaths * 5;
		out += m_nParticipations * 2;
		out += m_nWins * 50;
		out += m_nMvp * 50;

		return out < 0 ? 0 : out;
	}

}__PVPRANKING_TYPE_UNIT;

typedef struct __PVPRANKING_PROGRESS_UNIT
{
	u_long m_nPlayerID;
	char m_szName[32];
	int m_nTotalPoints;
	__PVPRANKING_TYPE_UNIT m_Guildwar;
	__PVPRANKING_TYPE_UNIT m_Tournament;
	__PVPRANKING_TYPE_UNIT m_Arena;
	__PVPRANKING_TYPE_UNIT m_Duel;

	void Init() 
	{
		m_nPlayerID = 0;
		strncpy(m_szName, "", 32);
		m_nTotalPoints = 0;
		m_Guildwar.Init();
		m_Tournament.Init();
		m_Arena.Init();
		m_Duel.Init();
	}
	void WriteValuesToString(char* szProgress) 
	{
		m_Guildwar.WriteToString(szProgress);
		m_Tournament.WriteToString(szProgress);
		m_Arena.WriteToString(szProgress);
		m_Duel.WriteToString(szProgress);

	}
	void ReadValuesFromString(char* szProgress) 
	{
		int nPos = 0;
		m_Guildwar.LoadFromString(szProgress, &nPos);
		nPos++;
		m_Tournament.LoadFromString(szProgress, &nPos);
		nPos++;
		m_Arena.LoadFromString(szProgress, &nPos);
		nPos++;
		m_Duel.LoadFromString(szProgress, &nPos);
	}
	void CalculatePoints() 
	{
		m_nTotalPoints = 0;
		m_nTotalPoints += m_Guildwar.CalculatePoints();
		m_nTotalPoints += m_Tournament.CalculatePoints();
		m_nTotalPoints += ceil((double)m_Arena.CalculatePoints() / 10.0f);
		m_nTotalPoints += ceil((double)m_Duel.CalculatePoints() / 10.0f);
	}
	void Write(CAr& ar) 
	{
		ar << m_nPlayerID;
		ar.WriteString(m_szName);
		ar << m_nTotalPoints;

		ar << m_Arena;
		ar << m_Duel;
		ar << m_Guildwar;
		ar << m_Tournament;
	}
	void Read(CAr& ar) 
	{
		ar >> m_nPlayerID;
		ar.ReadString(m_szName, 32);
		ar >> m_nTotalPoints;

		ar >> m_Arena;
		ar >> m_Duel;
		ar >> m_Guildwar;
		ar >> m_Tournament;
	}
	void Copy(__PVPRANKING_PROGRESS_UNIT* sourceUnit)
	{
		m_nPlayerID = sourceUnit->m_nPlayerID;
		strncpy(m_szName, sourceUnit->m_szName, 32);
		m_nTotalPoints = sourceUnit->m_nTotalPoints;
		m_Guildwar = sourceUnit->m_Guildwar;
		m_Tournament = sourceUnit->m_Tournament;
		m_Arena = sourceUnit->m_Arena;
		m_Duel = sourceUnit->m_Duel;
	}


}__PVPRANKING_PROGRESS_UNIT;

class CAegonPvPRankingMng
{
public:

	map<u_long, __PVPRANKING_PROGRESS_UNIT> playerRankingMap;

	CAegonPvPRankingMng();
	virtual ~CAegonPvPRankingMng();

	static CAegonPvPRankingMng* GetInstance(void);

	void WriteMap(CAr& ar);
	void ReadMap(CAr& ar);
	void CalculatePointsForAll();
	__PVPRANKING_PROGRESS_UNIT* GetStatUnit(u_long nPlayerID);
#ifdef __WORLDSERVER
	void SaveStatUnit(u_long nPlayerID);
	void PvPRankingOnJoin(CUser* pUser);
#endif // __WORLDSERVER
};

#endif // __AEGON_PVP_RANKING
#endif // __AEGPVPRANK__H