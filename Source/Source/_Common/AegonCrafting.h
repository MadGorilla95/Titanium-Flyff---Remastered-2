#pragma once
#ifndef __AEGCRAFT__H
#define __AEGCRAFT__H
#ifdef __AEGON_CRAFTING_SYSTEM

#define MAXRECIPENAME 32
#define MAXCRAFTINGLEVEL	25
#define AEGCRAFT_MAXCOMPONENTS	6

#define AEGCRAFT_RECYCLE_CATEGORY "-------Recycling-------"

typedef struct __CRAFTING_LEVEL
{
	int m_level;
	int m_levelExp;
	int m_levelAdditionalExp;

	void Init() 
	{
		m_level = -1;
		m_levelExp = -1;
		m_levelAdditionalExp = -1;
	}

}__CRAFTING_LEVEL;

typedef struct __CRAFTING_COMPONENT
{
	DWORD m_itemID;
	int m_itemNum;
	void Init() 
	{
		m_itemID = 0;
		m_itemNum = 0;
	}

	BOOL IsValid() 
	{
		if (m_itemID <= 0 || m_itemNum <= 0)
			return FALSE;

		return TRUE;
	}

	void Set(DWORD itemID, int itemNum) 
	{
		m_itemID = itemID;
		m_itemNum = itemNum;
	}
}__CRAFTING_COMPONENT;

typedef struct __CRAFTING_UNIT
{
	int unitID;
	char recipeName[MAXRECIPENAME];
	char categoryName[MAXRECIPENAME];
	DWORD resultItemID;
	DWORD resultItemNum;
	vector<__CRAFTING_COMPONENT> components;
	DWORD craftingExp;
	DWORD levelRequirement;
	DWORD descriptionTID;
	DWORD craftTime;
	BOOL recyclingRecipe;
	

	void Init() 
	{
		unitID = -1;
		sprintf(recipeName, "");
		resultItemID = 0;
		resultItemNum = 0;
		components.clear();
		craftingExp = 0;
		levelRequirement = 0;
		recyclingRecipe = FALSE;
	}
	BOOL IsValid() 
	{
		if (unitID < 0)
			return FALSE;
		if (resultItemID <= 0)
			return FALSE;
		if (resultItemNum <= 0)
			return FALSE;
		if (components.size() <= 0)
			return FALSE;
		if (descriptionTID <= 0 && !recyclingRecipe)
			return FALSE;

		if (recyclingRecipe && components.size() > 1)
			return FALSE;

		return TRUE;
	}

	BOOL AddComponent(DWORD itemID, int itemNum) 
	{
		__CRAFTING_COMPONENT tmpComponent;
		tmpComponent.Init();
		tmpComponent.Set(itemID, itemNum);
		if (tmpComponent.IsValid()) 
		{
			components.push_back(tmpComponent);
			return TRUE;
		}
		return FALSE;
	}

}__CRAFTING_UNIT;



class CAegonCraftingMng
{
public:

	LPCSTR m_scriptFile = "AegonCrafting.inc";
	map<int, __CRAFTING_UNIT> m_mapCraftingUnits;
	__CRAFTING_LEVEL m_levels[MAXCRAFTINGLEVEL];
	BOOL m_bScriptLoaded;
	int maxExpValue;

	CAegonCraftingMng();
	virtual ~CAegonCraftingMng();

	static CAegonCraftingMng* GetInstance(void);

	BOOL LoadScript();
	void CalculateLevelAddiExp();
	int GetCraftingLevel(CMover* pMover);

#ifdef __WORLDSERVER
	BOOL AddExperience(CUser* pUser, int nAddExp);
#endif // __WORLDSERVER

#ifdef __CLIENT
	void FillSearchComboBox(CWndComboBox* m_pWndComboSearch);
#endif // __CLIENT

};
#endif // __AEGON_CRAFTING_SYSTEM

#endif //__AEGCRAFT__H