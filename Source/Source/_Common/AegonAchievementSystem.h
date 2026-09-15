#pragma once
#ifndef __AEGACHSYS__H
#define __AEGACHSYS__H
#ifdef __AEGON_ACHIEVEMENT_SYSTEM

#include "Region.h"

#define AEGACH_MAXACHIEVEMENTNAME 64
#define AEGACH_NOTICETIME	5000

#define AEGACH_STATUS_COMPLETED 0
#define AEGACH_STATUS_NOTSTARTED 1
#define AEGACH_STATUS_REQUIREMENTS 2
#define AEGACH_STATUS_STARTED 3

#define AEGACH_STATUS_MAX 4

#define AEGACH_CLIENT_PROCESS_TIME_SMALL 5000
#define AEGACH_CLIENT_PROCESS_TIME_MED 60000

#define AEGACH_TVAR_CMP_EQUAL	0
#define AEGACH_TVAR_CMP_SMALLER	1
#define AEGACH_TVAR_CMP_BIGGER	2
#define AEGACH_PROG_CMP			3

typedef struct __ACHIEVEMENT_UNIT
{
	int achievementID;
	char achievementName[AEGACH_MAXACHIEVEMENTNAME];
	char categoryName[AEGACH_MAXACHIEVEMENTNAME];
	vector<CString> subcategories;
	DWORD descriptionTID;
	DWORD shortDescriptionTID;
	char imageFile[128];
	int subcategoriesNum;
	DWORD reward;
	int rewardNum;
	int maxProgress;
	int achievementType;
	vector<int> requirements;
	int nTypeVar;
	BOOL bRewardTradeable;
	char szRegionTitle[MAX_REGIONTITLE];
	int nTypeExtension;

	void Init() 
	{
		achievementID = -1;
		strcpy(achievementName, "");
		strcpy(categoryName, "");
		subcategories.clear();
		descriptionTID = 0;
		strcpy(imageFile, "");
		reward = 0;
		rewardNum = 0;
		maxProgress = -1;
		achievementType = -1;
		requirements.clear();
		nTypeVar = -1;
		bRewardTradeable = FALSE;
		memset(szRegionTitle, 0, MAX_REGIONTITLE);
		nTypeExtension = -1;
	}

	BOOL IsValid() 
	{
		if (achievementID < 0)
			return FALSE;
		if (strcmp(achievementName, "") == 0)
			return FALSE;
		if (strcmp(categoryName, "") == 0)
			return FALSE;
		if (descriptionTID == 0)
			return FALSE;
		if (maxProgress < 0)
			return FALSE;
		if (achievementType < 0)
			return FALSE;
		if (achievementType == AEGACH_VISITREGION && strlen(szRegionTitle) <= 0)
			return FALSE;

		return TRUE;
	}

	void AddSubcategory(CString strSubcategory) 
	{
		subcategories.push_back(strSubcategory);
	}

	BOOL IsInCategory(CString strCategory) 
	{
		if (strCategory.Compare(categoryName) == 0)
			return TRUE;

		for (int i = 0; i < subcategories.size(); i++)
		{
			if (strCategory.Compare(subcategories[i].GetString()) == 0)
				return TRUE;
		}

		return FALSE;
	}
	void AddRequirement(int achievementID)
	{
		requirements.push_back(achievementID);
	}
}__ACHIEVEMENT_UNIT;







class CAegonAchievementMng
{
public:

	LPCSTR m_scriptFile = "AegonAchievementSystem.inc";
	map<int, __ACHIEVEMENT_UNIT> m_mapAchievements;
	BOOL m_bScriptLoaded;

#ifdef __CLIENT
	CTexture* m_AchievementNoticeTexture;
	DWORD lastProcessSmall;
	DWORD lastProcessMed;
	map<CString, DWORD> m_mapRegionLastCheck;
#endif // __CLIENT

	CAegonAchievementMng();
	virtual ~CAegonAchievementMng();

	static CAegonAchievementMng* GetInstance(void);

	BOOL LoadScript();

	DWORD GetCategoryDescription(CString strCategory, DWORD data);
	CString GetCategoryImage(CString strCategory, DWORD data);
	int GetFinishedCount(CString strCategory = "");
	


#if defined(__WORLDSERVER) || defined(__CLIENT)
	int GetAchievementStatus(int achievementID, CMover* pMover);
	BOOL HasRequirements(int achievementID, CUser* pUser = NULL);
#endif // world || client

#ifdef __WORLDSERVER
	void CheckAchievementProgress(CUser* pUser, int achievementID);
	void CheckAchievementProgressForType(CUser* pUser, int type);
	
	BOOL AddAchievementProgress(CUser* pUser, int achievementID, int value, int nTypeExtension = -1);
	void AddAchievementProgressForType(CUser* pUser, int type, int value, int nTypeExtension = -1);
	BOOL HasFinishedAchievement(CUser* pUser, int achievementID);
	void RewardFinished(CUser* pUser, int achievementID);
	void CollectAchievementReward(CUser* pUser, int achievementID);
	void SafeAddProgUnit(CUser* pUser, int achievementID);
	void AddAllProgUnits(CUser* pUser);
	void CollectOfflineVendorSellCount(CMover* pVendor);
#endif // __WORLDSERVER
#ifdef __CLIENT
	void ProcessAchievementNotice(C2DRender* p2DRender);
	void OpenAchievementWnd(int achievementID);
	void ClientProcess();
	void ClientAchievementCheck(int type, int typeVar, DWORD compareType);
	int GetCategoryAchievementCount(CString strCategory);
	void GetAchievementDescriptionEditString(CEditString* editString, int achievementID);
	void CheckPlayerRegionAchievement(LPREGIONELEM lpRegionElem);
#endif // __CLIENT
};

typedef struct __ACHIEVEMENT_PROGRESS_UNIT
{
	int nAchievementID;
	int nProgressCount;
	int nRewardSent;
	void Init()
	{
		nAchievementID = -1;
		nProgressCount = -1;
		nRewardSent = -1;
	}
	void SetUnitAchievement(int achievementID)
	{
		nAchievementID = achievementID;
	}
	void SetUnitProgress(int progressCount, int rewardSent)
	{
		nProgressCount = progressCount;
		nRewardSent = rewardSent;
	}
	BOOL IsValid()
	{
		if (nAchievementID < 0 || nProgressCount < 0 || nRewardSent < 0)
			return FALSE;

		return TRUE;
	}
	BOOL IsFinished()
	{
		map<int, __ACHIEVEMENT_UNIT>::iterator it = CAegonAchievementMng::GetInstance()->m_mapAchievements.find(nAchievementID);
		if (it != CAegonAchievementMng::GetInstance()->m_mapAchievements.end())
		{
			if (nProgressCount >= it->second.maxProgress)
				return TRUE;
		}
		return FALSE;
	}
	double GetProgressPercent()
	{
		map<int, __ACHIEVEMENT_UNIT>::iterator it = CAegonAchievementMng::GetInstance()->m_mapAchievements.find(nAchievementID);
		if (it != CAegonAchievementMng::GetInstance()->m_mapAchievements.end())
		{
			if (it->second.maxProgress != 0)
				return ((double)nProgressCount / (double)it->second.maxProgress);
			else
				return 0;
		}
		return -1.0;

	}
	void IncreaseProgress(int value)
	{
		nProgressCount += value;
	}


}__ACHIEVEMENT_PROGRESS_UNIT;

typedef struct __ACHIEVEMENT_PROGRESS
{
	map<int, __ACHIEVEMENT_PROGRESS_UNIT> mapAchievementProgress;
	vector<int> unfinishedUnit;
	vector<int> unfinishedType;

	__ACHIEVEMENT_PROGRESS() 
	{
		Init();
	}
	void Init()
	{
		mapAchievementProgress.clear();
		unfinishedUnit.clear();
		unfinishedType.clear();
	}

	void fillUnfinishedUnit() 
	{
		unfinishedUnit.clear();
		map<int, __ACHIEVEMENT_PROGRESS_UNIT>::iterator it = mapAchievementProgress.begin();
		for (; it != mapAchievementProgress.end(); it++) 
		{
			if (!it->second.IsFinished()) 
			{
				if (!easy_find(unfinishedUnit, it->second.nAchievementID)) 
				{
					unfinishedUnit.push_back(it->second.nAchievementID);
				}
			}
		}
	}

	void fillUnfinishedType()
	{
		unfinishedType.clear();
		map<int, __ACHIEVEMENT_UNIT>::iterator it = CAegonAchievementMng::GetInstance()->m_mapAchievements.begin();
		for (; it != CAegonAchievementMng::GetInstance()->m_mapAchievements.end(); it++)
		{
			map<int, __ACHIEVEMENT_PROGRESS_UNIT>::iterator itProg = mapAchievementProgress.find(it->second.achievementID);

			if (itProg != mapAchievementProgress.end())
			{
				if (!itProg->second.IsFinished())
				{
					if (!easy_find(unfinishedType, it->second.achievementType)) 
					{
						unfinishedType.push_back(it->second.achievementType);
					}
				}
			}
		}
	}

	BOOL isUnfinishedUnit(int achievementID)
	{
		if (easy_find(unfinishedUnit, achievementID))
		{
			return TRUE;
		}
		else
			return FALSE;
	}

	BOOL isUnfinishedType(int achievementType)
	{
		if (easy_find(unfinishedType, achievementType))
		{
			return TRUE;
		}
		else
			return FALSE;
	}

	BOOL AddUnit(__ACHIEVEMENT_PROGRESS_UNIT unit)
	{
		map< int, __ACHIEVEMENT_PROGRESS_UNIT>::iterator it = mapAchievementProgress.find(unit.nAchievementID);
		if (it == mapAchievementProgress.end())
		{
			mapAchievementProgress.insert(make_pair(unit.nAchievementID, unit));
			return TRUE;
		}
		return FALSE;
	}
	int GetFinishedCount()
	{
		int out = 0;
		map< int, __ACHIEVEMENT_PROGRESS_UNIT>::iterator it = mapAchievementProgress.begin();
		for (; it != mapAchievementProgress.end(); it++)
		{
			if (it->second.IsFinished())
				out++;
		}
		return out;
	}
	__ACHIEVEMENT_PROGRESS_UNIT* GetUnit(int achievementID)
	{
		map< int, __ACHIEVEMENT_PROGRESS_UNIT>::iterator it = mapAchievementProgress.find(achievementID);
		if (it != mapAchievementProgress.end())
		{
			return &it->second;
		}
		return NULL;
	}
	void WriteMap(CAr& ar)
	{

		DWORD mapSize = mapAchievementProgress.size();
		ar << mapSize;

		map<int, __ACHIEVEMENT_PROGRESS_UNIT>::iterator it = mapAchievementProgress.begin();
		for (; it != mapAchievementProgress.end(); it++)
		{
			ar << it->second;
		}

#if _WIN64
		ar << static_cast<unsigned long>(unfinishedUnit.size());
#else
		ar << unfinishedUnit.size();
#endif
		for (int i = 0; i < unfinishedUnit.size(); i++) 
		{
			ar << unfinishedUnit[i];
		}

#if _WIN64
		ar << static_cast<unsigned long>(unfinishedType.size());
#else
		ar << unfinishedType.size();
#endif
		for (int j = 0; j < unfinishedType.size(); j++)
		{
			ar << unfinishedType[j];
		}

	}
	void ReadMap(CAr& ar)
	{
		DWORD mapSize, unfinishedUnitSize, unfinishedTypeSize;
		ar >> mapSize;

		mapAchievementProgress.clear();
		for (int i = 0; i < mapSize; i++)
		{
			__ACHIEVEMENT_PROGRESS_UNIT tmpUnit;
			tmpUnit.Init();
			ar >> tmpUnit;

			if (tmpUnit.IsValid() && mapAchievementProgress.find(tmpUnit.nAchievementID) == mapAchievementProgress.end())
			{
				mapAchievementProgress.insert(make_pair(tmpUnit.nAchievementID, tmpUnit));
			}
		}
		
		ar >> unfinishedUnitSize;
		unfinishedUnit.clear();
		for (int j = 0; j < unfinishedUnitSize; j++) 
		{
			int tmpUnfUnit;
			ar >> tmpUnfUnit;
			unfinishedUnit.push_back(tmpUnfUnit);
		}

		ar >> unfinishedTypeSize;
		unfinishedType.clear();
		for (int k = 0; k < unfinishedTypeSize; k++)
		{
			int tmpUnfType;
			ar >> tmpUnfType;
			unfinishedType.push_back(tmpUnfType);
		}


	}
	BOOL SafeAddProgress(int achievementID, int value, int maxValue) 
	{
		map<int, __ACHIEVEMENT_PROGRESS_UNIT>::iterator it = mapAchievementProgress.find(achievementID);
		if (it != mapAchievementProgress.end()) 
		{
			int maxAddValue = maxValue - it->second.nProgressCount;
			if (value > maxAddValue)
				value = maxAddValue;

			if (value > 0) 
			{
				it->second.IncreaseProgress(value);
				return TRUE;
			}
				
		}
		return FALSE;
	}
	BOOL MeetsRequirements(int achievementID) 
	{
		map<int, __ACHIEVEMENT_UNIT>::iterator it = CAegonAchievementMng::GetInstance()->m_mapAchievements.find(achievementID);
		if (it != CAegonAchievementMng::GetInstance()->m_mapAchievements.end())
		{
			vector<int>::iterator vecIterator = it->second.requirements.begin();
			for (; vecIterator != it->second.requirements.end(); vecIterator++)
			{
				map<int, __ACHIEVEMENT_PROGRESS_UNIT>::iterator it =  mapAchievementProgress.find(*vecIterator);

				if (it != mapAchievementProgress.end()) 
				{
					if (it->second.IsFinished()) 
					{
						continue;
					}
				}
				return FALSE;
			}
			return TRUE;
		}
		return FALSE;
	}
	

}__ACHIEVEMENT_PROGRESS;
#endif // __AEGON_ACHIEVEMENT_SYSTEM

#endif //__AEGACHSYS__H