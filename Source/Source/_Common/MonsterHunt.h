#pragma once
#ifndef __MNSTHNT__H
#define __MNSTHNT__H
#ifdef __AEGON_MONSTER_HUNT

#define MH_MOBID 0
#define MH_MOBCOUNT 1
#define MH_REWARDED 2

typedef struct __MONSTERHUNT_QUEST
{
	DWORD	questID;
	DWORD	mobID;
	DWORD	mobNum;
	DWORD	activeHunt;
	DWORD	activeReward;
	DWORD	rewardItem;
	DWORD	itemNum;
	DWORD	repeatable;
	DWORD	region;
	void Set(DWORD dw_questID, DWORD dw_mobID, DWORD dw_mobNum, DWORD dw_activeHunt, DWORD dw_activeReward, DWORD dw_rewardItem, DWORD dw_itemNum, DWORD dw_repeatable, DWORD dw_region)
	{
		questID = dw_questID;
		mobID = dw_mobID;
		mobNum = dw_mobNum;
		activeHunt = dw_activeHunt;
		activeReward = dw_activeReward;
		rewardItem = dw_rewardItem;
		itemNum = dw_itemNum;
		repeatable = dw_repeatable;
		region = dw_region;
		
	}
}__MONSTERHUNT_QUEST;




class CMonsterHuntMng
{
public:

	LPCSTR m_scriptFile = "MonsterHunt.inc";

	CMonsterHuntMng();
	virtual ~CMonsterHuntMng();

	static CMonsterHuntMng* GetInstance(void);
	BOOL IsQuestMob(DWORD mobID);

	__MONSTERHUNT_QUEST tmpQuest;
	

	map<DWORD, __MONSTERHUNT_QUEST>	m_mapMonsterHunt;
	BOOL m_bScriptLoaded;

	DWORD	m_monsterHuntQuestID;

	vector<DWORD> m_vecQuestMobs;

	DWORD	m_mobID;
	DWORD	m_mobNum;
	DWORD	m_activeHunt;
	DWORD	m_activeReward;
	DWORD	m_rewardItem;
	DWORD	m_itemNum;
	DWORD	m_repeatable;
	DWORD	m_region;
	
	BOOL LoadScript();

#ifdef __WORLDSERVER
	void MonsterHuntOnJoin(CUser* pUser);
#endif // __WORLDSERVER

	
};

typedef struct __MONSTERHUNT_SINGLE
{
	DWORD dwQuestID;
	DWORD dwMobID;
	DWORD dwCount;
	DWORD dwRewardSent;
	void Set(DWORD idwQuestID, DWORD idwMobID, DWORD idwCount, DWORD idwRewardSent)
	{
		dwQuestID = idwQuestID;
		dwMobID = idwMobID;
		dwCount = idwCount;
		dwRewardSent = idwRewardSent;
	}

}__MONSTERHUNT_SINGLE;

typedef struct __MONSTERHUNT_PROGRESS
{
	vector<__MONSTERHUNT_SINGLE> vecQuests;
	void Init()
	{
		vecQuests.clear();
	}
	void AddQuest(__MONSTERHUNT_SINGLE questProg)
	{
		if (questProg.dwQuestID >= vecQuests.size())
		{
			vecQuests.push_back(questProg);
		}
	}
	__MONSTERHUNT_SINGLE* GetProgUnit(DWORD questID)
	{
		if (questID > 0 && questID < vecQuests.size())
		{
			if (vecQuests[questID].dwQuestID == questID)
				return &vecQuests[questID];
		}

		for (int i = 0; i < vecQuests.size(); i++)
		{
			if (vecQuests[i].dwQuestID == questID)
				return &vecQuests[i];
		}

		return NULL;
	}
	void WriteMap(CAr& ar) 
	{
#if _WIN64
		ar << static_cast<unsigned long>(vecQuests.size());
#else
		ar << vecQuests.size();
#endif

		for (int i = 0; i < vecQuests.size(); i++) 
		{
			ar << vecQuests[i].dwQuestID;
			ar << vecQuests[i].dwMobID;
			ar << vecQuests[i].dwCount;
			ar << vecQuests[i].dwRewardSent;
		}
	}
	void ReadMap(CAr& ar) 
	{
		vecQuests.clear();
#if _WIN64
		unsigned long vecSize;
#else
		size_t vecSize;
#endif
		ar >> vecSize;
		vecQuests.reserve(vecSize);
		for (int i = 0; i < vecSize; i++) 
		{
			__MONSTERHUNT_SINGLE tmpUnit;
			DWORD questID, mobID, count, rewardSent;
			ar >> questID;
			ar >> mobID;
			ar >> count;
			ar >> rewardSent;
			tmpUnit.Set(questID, mobID, count, rewardSent);
			vecQuests.push_back(tmpUnit);
		}

	}

}__MONSTERHUNT_PROGRESS;


#endif // __AEGON_MONSTER_HUNT

#endif //__MNSTHNT__H