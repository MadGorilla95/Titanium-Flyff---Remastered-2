#include "stdafx.h"
#include "MonsterHunt.h"
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


#ifdef __AEGON_MONSTER_HUNT

CMonsterHuntMng::CMonsterHuntMng()
{
	m_bScriptLoaded = LoadScript();
	if (!m_bScriptLoaded)
		m_mapMonsterHunt.clear();
}

CMonsterHuntMng::~CMonsterHuntMng()
{
	m_mobID = 0;
	m_mobNum = 0;
	m_activeHunt = 0;
	m_activeReward = 0;
	m_rewardItem = 0;
	m_itemNum = 0;
	m_repeatable = 0;
	m_region = 0;
	m_bScriptLoaded = FALSE;
}

CMonsterHuntMng* CMonsterHuntMng::GetInstance(void)
{
	static CMonsterHuntMng sMonsterHuntMng;
	return &sMonsterHuntMng;
}


BOOL CMonsterHuntMng::LoadScript()
{
	CScript s;

	if (s.Load(m_scriptFile))
	{
		s.GetToken();
		while (s.tok != FINISHED)
		{
			if (s.Token == _T("MonsterHunt"))
			{
				m_monsterHuntQuestID = s.GetNumber();
				s.GetToken(); // {

				s.GetToken();		// mobID
				m_mobID = s.GetNumber();
				s.GetToken();		// mobNum
				m_mobNum = s.GetNumber();
				s.GetToken();		// activeHunt
				m_activeHunt = s.GetNumber();
				s.GetToken();		// activeReward
				m_activeReward = s.GetNumber();
				s.GetToken();		// rewardItem
				m_rewardItem = s.GetNumber();
				m_itemNum = s.GetNumber();
				s.GetToken();		// repeatable
				m_repeatable = s.GetNumber();
				s.GetToken();		// region
				m_region = s.GetNumber();

				tmpQuest.Set(m_monsterHuntQuestID, m_mobID, m_mobNum, m_activeHunt, m_activeReward, m_rewardItem, m_itemNum, m_repeatable, m_region);

				if (m_mapMonsterHunt.find(m_monsterHuntQuestID) == m_mapMonsterHunt.end())
				{
					m_mapMonsterHunt.insert(make_pair(m_monsterHuntQuestID, tmpQuest));
					m_vecQuestMobs.push_back(m_mobID);
				}
					

			}
			s.GetToken(); // }
		}
	}
	else
	{
		Error("Monsterhunt failed to Load");
		return FALSE;
	}
		

	int nIndex = 0;

	map<DWORD, __MONSTERHUNT_QUEST>::iterator it = m_mapMonsterHunt.begin();
	for (; it != m_mapMonsterHunt.end(); it++)
	{
		if (nIndex != it->first)
		{
			Error("Monsterhunt quest index error");
			return FALSE;
		}

		nIndex++;
	}

	return TRUE;
}
BOOL CMonsterHuntMng::IsQuestMob(DWORD mobID)
{
	if (easy_find(m_vecQuestMobs, mobID))
		return TRUE;

	return FALSE;
}
#ifdef __WORLDSERVER
void CMonsterHuntMng::MonsterHuntOnJoin(CUser* pUser)
{
	if (IsValidObj(pUser))
	{
		BOOL bSave = FALSE;
		map<DWORD, __MONSTERHUNT_QUEST>::iterator it =	m_mapMonsterHunt.begin();
		for (; it != m_mapMonsterHunt.end(); it++)
		{
			if (!pUser->m_MHProgress.GetProgUnit( it->first)) 
			{
				__MONSTERHUNT_SINGLE tmpUnit;
				tmpUnit.Set(it->first, it->second.mobID, 0, 0);
				pUser->m_MHProgress.vecQuests.push_back(tmpUnit);
				bSave = TRUE;
			}
		}
	
		if (bSave && pUser->GetWorld())
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
	}
}
#endif // __WORLDSERVER
#endif // __AEGON_MONSTER_HUNT