#pragma once
#ifndef __AEGSEAPASS__H
#define __AEGSEAPASS__H
#ifdef __AEGON_SEASON_PASS

#define MAXPASSNAME 32
#define MAXPASSITEMS 30

typedef struct __SEASONPASS_PROGRESS_UNIT
{
	int passID;
	int progressPoints;
	int boughtPremium;
	vector<int> rewardsClaimed;

	void Init() 
	{
		passID = -1;
		progressPoints = 0;
		boughtPremium = 0;
		rewardsClaimed.clear();
	}
	BOOL IsValid() 
	{
		if (passID < 0)
			return FALSE;
		if (progressPoints < 0)
			return FALSE;

		return TRUE;
	}
	BOOL IsClaimedReward(int rewardID) 
	{
	
		for (int i = 0; i < rewardsClaimed.size(); i++) 
		{
			if (rewardsClaimed[i] == rewardID)
				return TRUE;
		}

		return FALSE;
	}

}__SEASONPASS_PROGRESS_UNIT;

typedef struct __SEASONPASS_PROGRESS
{
	map<int,  __SEASONPASS_PROGRESS_UNIT> mapProgUnits;

	void Init() 
	{
		mapProgUnits.clear();
	}
	BOOL AddUnit(__SEASONPASS_PROGRESS_UNIT unit)
	{
		map< int, __SEASONPASS_PROGRESS_UNIT>::iterator it = mapProgUnits.find(unit.passID);
		if (it == mapProgUnits.end())
		{
			mapProgUnits.insert(make_pair(unit.passID, unit));
			return TRUE;
		}
		return FALSE;
	}
	__SEASONPASS_PROGRESS_UNIT* GetProgressUnit(int passID) 
	{
		map< int, __SEASONPASS_PROGRESS_UNIT>::iterator it = mapProgUnits.find(passID);
		if (it != mapProgUnits.end())
		{
			return &it->second;
		}
		return NULL;
	}
	void WriteSingle(CAr& ar, __SEASONPASS_PROGRESS_UNIT* spUnit)
	{
		if(spUnit)
		{
			ar << spUnit->passID;
			ar << spUnit->progressPoints;
			ar << spUnit->boughtPremium;
#if _WIN64
			ar << static_cast<unsigned long>(spUnit->rewardsClaimed.size());
#else
			ar << spUnit->rewardsClaimed.size();
#endif

			for (int i = 0; i < spUnit->rewardsClaimed.size(); i++)
			{
				ar << spUnit->rewardsClaimed[i];
			}
		}
	}
	void ReadSingle(CAr& ar)
	{
		int passID, claimSize;;
		ar >> passID;

		map< int, __SEASONPASS_PROGRESS_UNIT>::iterator it = mapProgUnits.find(passID);
		if (it != mapProgUnits.end()) 
		{
			mapProgUnits.erase(it);
		}

		__SEASONPASS_PROGRESS_UNIT tmpUnit;
		tmpUnit.passID = passID;
		ar >> tmpUnit.progressPoints;
		ar >> tmpUnit.boughtPremium;
			
			
		ar >> claimSize;
		tmpUnit.rewardsClaimed.reserve(claimSize);
		for (int i = 0; i < claimSize; i++) 
		{
			int claim = -1;
			ar >> claim;
			tmpUnit.rewardsClaimed.push_back(claim);
		}
		AddUnit(tmpUnit);
		
	}
	void WriteMap(CAr& ar)
	{

		DWORD mapSize = mapProgUnits.size();
		ar << mapSize;

		map< int, __SEASONPASS_PROGRESS_UNIT>::iterator it = mapProgUnits.begin();
		for (; it != mapProgUnits.end(); it++) 
		{
			ar << it->second.passID;
			ar << it->second.progressPoints;
			ar << it->second.boughtPremium;
#if _WIN64
			ar << static_cast<unsigned long>(it->second.rewardsClaimed.size());
#else
			ar << it->second.rewardsClaimed.size();
#endif

			for (int i = 0; i < it->second.rewardsClaimed.size(); i++) 
			{
				ar << it->second.rewardsClaimed[i];
			}
		}

	}
	void ReadMap(CAr& ar)
	{
		DWORD mapSize;
		ar >> mapSize;

		mapProgUnits.clear();
		for (int i = 0; i < mapSize; i++)
		{
			__SEASONPASS_PROGRESS_UNIT tmpUnit;
			tmpUnit.Init();
			ar >> tmpUnit.passID;
			ar >> tmpUnit.progressPoints;
			ar >> tmpUnit.boughtPremium;
#if _WIN64
			unsigned long vecSize;
#else
			size_t vecSize;
#endif
			ar >> vecSize;
			tmpUnit.rewardsClaimed.reserve(vecSize);
			for (int i = 0; i < vecSize; i++) 
			{
				int vecVal = -1;
				ar >> vecVal;
				tmpUnit.rewardsClaimed.push_back(vecVal);
			
			}
			AddUnit(tmpUnit);
		}
	}
	BOOL UnlockSeasonPassPremium(int nPassID) 
	{
		__SEASONPASS_PROGRESS_UNIT* unlockUnit = GetProgressUnit(nPassID);
		if (unlockUnit) 
		{
			if (unlockUnit->boughtPremium == 0) 
			{
				unlockUnit->boughtPremium = 1;
				return TRUE;
			}
		}
		return FALSE;
	}
}__SEASONPASS_PROGRESS;

typedef struct __SEASONPASS_REWARD
{
	int rewardID;
	DWORD itemID;
	DWORD itemNum;
	int passPointsNeeded;
	int rewardRarity;
	int pointsToReach;

	void Init() 
	{
		rewardID = -1;
		itemID = 0;
		itemNum = 0;
		passPointsNeeded = 0;
		rewardRarity = -1;
		pointsToReach = -1;
	}
	BOOL IsValid() 
	{
		if (rewardID < 0)
			return FALSE;
		if (itemID <= 0)
			return FALSE;
		if (itemNum <= 0)
			return FALSE;
		if (passPointsNeeded <= 0)
			return FALSE;
		if (rewardRarity < 0)
			return FALSE;
		if (pointsToReach < 0)
			return FALSE;

		return TRUE;
	}
	BOOL IsFreeReward() 
	{
		return rewardRarity == 0 ? TRUE : FALSE;
	}
	int GetTotalPointsToFinish() 
	{
		return pointsToReach + passPointsNeeded;
	}


}__SEASONPASS_REWARD;

typedef struct __SEASONPASS_UNIT
{
	int passID;
	int nPvEPass;
	char passName[MAXPASSNAME];
	tm endTimeStruct;
	time_t timeEnd;
	vector<__SEASONPASS_REWARD> rewards;
	BOOL bVisible;
	int nPriceDP;

	void Init() 
	{
		passID = -1;
		nPvEPass = -1;
		rewards.clear();
		strcpy(passName, "invalid season pass");
		bVisible = FALSE;
		timeEnd = 0;
		nPriceDP = 0;
	}

	BOOL IsValid() 
	{
		if (passID < 0)
			return FALSE;
		if (rewards.size() <= 0)
			return FALSE;
		if (((string)passName).compare("invalid season pass") == 0)
			return FALSE;
		if (nPvEPass == -1)
			return FALSE;
		if (!CheckValidRewards())
			return FALSE;
		if (nPriceDP <= 0)
			return FALSE;

		return TRUE;
	}
	int GetCurrentLevel(int passPoints) 
	{
		for (int i = 0; i < rewards.size(); i++) 
		{
			if (passPoints < rewards[i].GetTotalPointsToFinish())
				return i;
		}
		return rewards.size() - 1;
	}
	BOOL AddRewardUnit(__SEASONPASS_REWARD rewardUnit)
	{
		rewardUnit.rewardID = rewards.size();
		int nPointsToReach = 0;
		for (int i = 0; i < rewards.size(); i++) 
		{
			nPointsToReach += rewards[i].passPointsNeeded;
		}
		rewardUnit.pointsToReach = nPointsToReach;
		if (rewardUnit.IsValid())
		{
			rewards.push_back(rewardUnit);
			return TRUE;
		}
		return FALSE;
	}

	BOOL IsPvEPass() 
	{
		return nPvEPass == 0 ? FALSE : TRUE;
	}

	BOOL CheckValidRewards() 
	{
		if (rewards.size() <= 0)
			return FALSE;
		for (int i = 1; i < rewards.size(); i++) 
		{
			if (rewards[i - 1].GetTotalPointsToFinish() > rewards[i].GetTotalPointsToFinish())
				return FALSE;
		}

		return TRUE;
	}
	time_t GetTimeRemaining() 
	{
		time_t out = 0;
		time_t currentTime = time(NULL);
		
		time(&currentTime);
		tm* localTimeStruct = localtime(&currentTime);
		currentTime = mktime(localTimeStruct);
		
		out = timeEnd - currentTime;

		if (out < 0)
			out = 0;

		return out;
	}
#ifdef __WORLDSERVER
	void SetEndTime() 
	{
		time_t endTime = time(NULL);
		endTime = mktime(&endTimeStruct);
		tm* endTimeStructLocal = localtime(&endTime);
		timeEnd = mktime(endTimeStructLocal);
	}
#endif // __WORLDSERVER

}__SEASONPASS_UNIT;



class CAegonSeasonPassMng
{
public:

	LPCSTR m_scriptFile = "AegonSeasonPass.inc";
	map<DWORD, __SEASONPASS_UNIT> m_mapSeasonPassPvE;
	map<DWORD, __SEASONPASS_UNIT> m_mapSeasonPassPvP;
	BOOL m_bScriptLoaded;

	int activePvEPass;
	int activePvPPass;

	CAegonSeasonPassMng();
	virtual ~CAegonSeasonPassMng();

	static CAegonSeasonPassMng* GetInstance(void);

	BOOL LoadScript();

	__SEASONPASS_UNIT* GetCurrentUnit(BOOL pve);
	__SEASONPASS_UNIT* GetUnit(int passID);

#ifdef __WORLDSERVER
	BOOL SafeAddProgUnit(CUser* pUser, int passID);
	void SeasonPassOnJoin(CUser* pUser);
	BOOL SafeAddPoints(BOOL pve, CUser* pUser, int nPoints);
	
	void OnPvEKill(CMover* pMoverDied, CMover* pAttacker);
	void OnPvPKill(CMover* pMoverDied, CMover* pAttacker);
#endif // __WORLDSERVER

};
#endif // __AEGON_SEASON_PASS

#endif //__AEGSEAPASS__H