#include "stdafx.h"
#include "AegonCrafting.h"
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
#endif // __WORLDSERVER

#ifdef __AEGON_CRAFTING_SYSTEM

CAegonCraftingMng::CAegonCraftingMng()
{
	m_mapCraftingUnits.clear();
	
	for (int i = 0; i < MAXCRAFTINGLEVEL; i++) 
	{
		m_levels[i].Init();
	}

	m_bScriptLoaded = LoadScript();
	if (!m_bScriptLoaded)
		m_mapCraftingUnits.clear();
	else
		CalculateLevelAddiExp();
}

CAegonCraftingMng::~CAegonCraftingMng()
{
}

CAegonCraftingMng* CAegonCraftingMng::GetInstance(void)
{
	static CAegonCraftingMng sAegonSeasonPassMng;
	return &sAegonSeasonPassMng;
}

BOOL CAegonCraftingMng::LoadScript()
{
	CScript s;
	__CRAFTING_UNIT tmpUnit;

	if (s.Load(m_scriptFile))
	{
		s.GetToken();
		while (s.tok != FINISHED)
		{
			if (s.Token.Compare("Recipe") == 0)
			{
				tmpUnit.Init();
				tmpUnit.unitID = s.GetNumber();
				s.GetToken(); // {

				while (s.Token.Compare("}") != 0)
				{
					s.GetToken();
					if (s.Token.Compare("name") == 0)
					{
						s.GetToken();
						strcpy(tmpUnit.recipeName, s.Token);
					}
					if (s.Token.Compare("category") == 0)
					{
						s.GetToken();
						strcpy(tmpUnit.categoryName, s.Token);
					}
					else if (s.Token.Compare("descriptionTID") == 0)
					{
						tmpUnit.descriptionTID = s.GetNumber();
					}
					else if (s.Token.Compare("craftTime") == 0)
					{
						tmpUnit.craftTime = s.GetNumber();
					}
					else if (s.Token.Compare("resultItem") == 0)
					{
						tmpUnit.resultItemID = s.GetNumber();
						tmpUnit.resultItemNum = s.GetNumber();
					}
					else if (s.Token.Compare("componentItem") == 0)
					{
						DWORD itemID = s.GetNumber();
						int itemNum = s.GetNumber();
						if(!tmpUnit.AddComponent(itemID, itemNum))
							Error("failed to add crafting component %d %d", itemID, itemNum);

					}
					else if (s.Token.Compare("craftingExp") == 0)
					{
						tmpUnit.craftingExp = s.GetNumber();
					}
					else if (s.Token.Compare("levelRequirement") == 0)
					{
						tmpUnit.levelRequirement = s.GetNumber();
					}
					else if (s.Token.Compare("recycling") == 0)
					{
						tmpUnit.recyclingRecipe = TRUE;
						sprintf(tmpUnit.recipeName, "Recycling");
						sprintf(tmpUnit.categoryName, AEGCRAFT_RECYCLE_CATEGORY);
					}
					else if (s.Token.Compare("}") == 0)
					{
						if (m_mapCraftingUnits.find(tmpUnit.unitID) == m_mapCraftingUnits.end() && tmpUnit.IsValid())
						{
							m_mapCraftingUnits.insert(make_pair(tmpUnit.unitID, tmpUnit));
						}
					}
				}
			}
			if (s.Token.Compare("CraftingLevels") == 0)
			{
				s.GetToken(); // {

				while (s.Token.Compare("}") != 0)
				{
					s.GetToken();
					
					if (s.Token.Compare("Level") == 0)
					{
						DWORD levelIndex = s.GetNumber();
						DWORD levelExp = s.GetNumber();
						if (levelIndex >= MAXCRAFTINGLEVEL)
							continue;

						m_levels[levelIndex].m_level = levelIndex + 1;
						m_levels[levelIndex].m_levelExp = levelExp;
						
					}
				}
			}
			s.GetToken();
		}
	}
	else
	{
		Error("failed to Load AegonCrafting.inc");
		return FALSE;
	}

	return TRUE;
}

void CAegonCraftingMng::CalculateLevelAddiExp() 
{
	m_levels[MAXCRAFTINGLEVEL - 1].m_levelAdditionalExp = 0;

	for (int i = 0; i < MAXCRAFTINGLEVEL - 1; i++)
	{
		m_levels[i].m_levelAdditionalExp = m_levels[i + 1].m_levelExp - m_levels[i].m_levelExp;
	}

	maxExpValue = m_levels[MAXCRAFTINGLEVEL - 1].m_levelExp;
}

int CAegonCraftingMng::GetCraftingLevel(CMover* pMover) 
{
	int out = 1;
	if (IsValidObj(pMover)) 
	{
		for (int i = 0; i < MAXCRAFTINGLEVEL; i++) 
		{
			if (m_levels[i].m_levelExp > pMover->m_nCraftingExp)
				break;

			out = m_levels[i].m_level;
		}
	}
	return out;
}

#ifdef __WORLDSERVER
BOOL CAegonCraftingMng::AddExperience(CUser* pUser, int nAddExp)
{
	int newExpValue = pUser->m_nCraftingExp + nAddExp;
	if (newExpValue < 0)
		newExpValue = 0;
	if (newExpValue > maxExpValue)
		newExpValue = maxExpValue;

	if (pUser->m_nCraftingExp != newExpValue) 
	{
		pUser->m_nCraftingExp = newExpValue;
		return TRUE;
	}
	return FALSE;
}
#endif // __WORLDSERVER

#ifdef __CLIENT
void CAegonCraftingMng::FillSearchComboBox(CWndComboBox* m_pWndComboSearch)
{
	if (!m_pWndComboSearch)
		return;

	CString searchString = m_pWndComboSearch->GetString();
	CString recipeName;
	int nCount = 0;
	map<int, __CRAFTING_UNIT>::iterator it = m_mapCraftingUnits.begin();

	for (; it != m_mapCraftingUnits.end(); it++) 
	{
		recipeName = it->second.recipeName;
		recipeName.MakeLower();
		searchString.MakeLower();
		if(searchString.GetLength() < 1 || recipeName.Find(searchString.GetString(), 0) != string::npos)
		{
			m_pWndComboSearch->AddString(it->second.recipeName);
			nCount++;
		}
	}
	if(nCount == 0)
		m_pWndComboSearch->AddString("Nothing found");

	if (!m_pWndComboSearch->m_bOpen) 
	{
		m_pWndComboSearch->m_bOpen = TRUE;
		m_pWndComboSearch->OpenListBox();
	}
}
#endif // __CLIENT

#endif // __AEGON_CRAFTING_SYSTEM