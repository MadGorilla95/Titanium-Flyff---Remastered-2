#include "stdafx.h"
#include "AegonLootBox.h"
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


#ifdef __AEGON_LOOT_BOX

CAegonLootBoxMng::CAegonLootBoxMng()
{
	tmpLootBox.Init();
	m_bScriptLoaded = LoadScript();
	if (!m_bScriptLoaded)
		m_mapLootBoxes.clear();
}

CAegonLootBoxMng::~CAegonLootBoxMng()
{
}

CAegonLootBoxMng* CAegonLootBoxMng::GetInstance(void)
{
	static CAegonLootBoxMng sAegonLootBoxMng;
	return &sAegonLootBoxMng;
}

BOOL CAegonLootBoxMng::LoadScript()
{
	CScript s;

	if (s.Load(m_scriptFile))
	{
		s.GetToken();
		while (s.tok != FINISHED)
		{
			if (s.Token == _T("Lootbox"))
			{
				tmpLootBox.Init();
				tmpLootBox.boxID = s.GetNumber();
				s.GetToken(); // {

				while (s.Token != _T("}"))
				{
					s.GetToken();
					if (s.Token == _T("name"))
					{
						s.GetToken();
						strcpy(tmpLootBox.boxName, s.Token);
					}
					else if (s.Token == _T("item"))
					{
						DWORD itemID = s.GetNumber();
						DWORD itemNum = s.GetNumber();
						DWORD itemChance = s.GetNumber();
						tmpLootBox.AddItem(itemID, itemNum, itemChance);
					}
					else if (s.Token == _T("ticket"))
					{
						DWORD ticketID = s.GetNumber();

						tmpLootBox.AddTicket(ticketID);
					}
					else if (s.Token == _T("dpPrice"))
					{
						tmpLootBox.dpPrice = s.GetNumber();
					}
					else if (s.Token == _T("}"))
					{
						if (m_mapLootBoxes.find(tmpLootBox.boxID) == m_mapLootBoxes.end() && tmpLootBox.valid())
						{
							m_mapLootBoxes.insert(make_pair(tmpLootBox.boxID, tmpLootBox));
						}
					}
				}
			}
			s.GetToken();
		}
	}
	else
	{
		Error("LootBoxes failed to Load");
		return FALSE;
	}

	return TRUE;
}
#endif // __AEGON_LOOT_BOX