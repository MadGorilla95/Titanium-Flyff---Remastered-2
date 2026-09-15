#include "stdafx.h"
#include "AegonBossMechanics.h"
#include "defineObj.h"
#include "defineItem.h"
#include "ResData.h"
#include "d3d9types.h"

#ifdef __CLIENT
#include "DPClient.h"
#include "AppDefine.h"
#include "WndManager.h"
#endif

#ifdef __WORLDSERVER
#include "User.h"
#include "dpdatabaseclient.h"
extern	CDPDatabaseClient	g_dpDBClient;
extern	CUserMng			g_UserMng;
#endif


#ifdef __AEGON_BOSS_MECHANICS
CAegonBossMechanicsMng::CAegonBossMechanicsMng()
{
	m_bScriptLoaded = LoadScript();
	if (!m_bScriptLoaded)
	{
		//m_mapShopDesigns.clear();
		Error("Could not load File: %s", m_scriptFile);
	}
	else
	{
	}
}

CAegonBossMechanicsMng::~CAegonBossMechanicsMng()
{
}

CAegonBossMechanicsMng* CAegonBossMechanicsMng::GetInstance(void)
{
	static CAegonBossMechanicsMng sAegonBossMechanicsMng;
	return &sAegonBossMechanicsMng;
}

BOOL CAegonBossMechanicsMng::LoadScript()
{
	CScript s;

	if (s.Load(m_scriptFile))
	{
		s.GetToken();
		while (s.tok != FINISHED)
		{
			if (s.Token.CompareNoCase("RegisterMoverAttacks") == 0)
			{
				__AEGON_MOVER_ATTACKS tmpUnit;
				DWORD dwMoverId = 0;

				s.GetToken(); // {

				while (s.Token.Compare("}") != 0)
				{
					s.GetToken();
					if (s.Token.CompareNoCase("mover") == 0)
					{
						dwMoverId = s.GetNumber();
					}
					else if (s.Token.CompareNoCase("attack") == 0)
					{
						DWORD dwAttackId = s.GetNumber();
						tmpUnit.m_vdwAttacks.push_back(dwAttackId);

					}
					else if (s.Token.CompareNoCase("minWaitTime") == 0)
					{
						tmpUnit.m_dwMinWaitTime = s.GetNumber();
						if (tmpUnit.m_dwMinWaitTime <= 0)
							tmpUnit.m_dwMinWaitTime = 1;
					}
					else if (s.Token.Compare("}") == 0)
					{
						m_mapMoverAttacks.insert(make_pair(dwMoverId, tmpUnit));
					}
				}
			}
			else if (s.Token.CompareNoCase("SpecialAttack") == 0)
			{
				__AEGON_SPECIAL_ATK tmpUnit;

				s.GetToken(); // {

				while (s.Token.Compare("}") != 0)
				{
					s.GetToken();
					if (s.Token.CompareNoCase("dwAttackId") == 0)
					{
						tmpUnit.m_dwAttackAbilityID = s.GetNumber();
					}
					else if (s.Token.CompareNoCase("fChance") == 0)
					{
						tmpUnit.m_fChance = s.GetFloat();
					}
					else if (s.Token.Compare("}") == 0)
					{
						m_mapAttacks.insert(make_pair(tmpUnit.m_dwAttackAbilityID, tmpUnit));
					}
				}
			}
			s.GetToken();
		}
	}
	else
	{
		Error("failed to Load %s", m_scriptFile);
		return FALSE;
	}
	return TRUE;
}

void CAegonBossMechanicsMng::OnActEndSpecialAtk(CMover* pMover)
{
	if (IsInvalidObj(pMover))
		return;
}
__AEGON_MOVER_ATTACKS* CAegonBossMechanicsMng::GetMoverSpecialAttacks(DWORD dwMoverId)
{
	map<DWORD, __AEGON_MOVER_ATTACKS>::iterator it = m_mapMoverAttacks.find(dwMoverId);
	if (it != m_mapMoverAttacks.end())
		return &it->second;

	return NULL;
}

__AEGON_SPECIAL_ATK* CAegonBossMechanicsMng::GetSpecialAttack(DWORD dwAttackId)
{
	map<DWORD, __AEGON_SPECIAL_ATK>::iterator it = m_mapAttacks.find(dwAttackId);
	if (it != m_mapAttacks.end())
		return &it->second;

	return NULL;
}

#ifdef __WORLDSERVER
BOOL CAegonBossMechanicsMng::OnChanceSpecialAtk(CMover* pMover, CMover* pTarget)
{
	if (IsInvalidObj(pMover) || IsInvalidObj(pTarget))
		return FALSE;

	MoverProp* pMoverProp = pMover->GetProp();
	if (!pMoverProp)
		return FALSE;

	__AEGON_MOVER_ATTACKS* pAttacksUnit = GetMoverSpecialAttacks(pMoverProp->dwID);
	if (!pAttacksUnit)
		return FALSE;

	if (pAttacksUnit->m_dwMinWaitTime != 0 && pMover->m_dwLastSpecialAttack != 0)
		if(!(GetTickCount() > pMover->m_dwLastSpecialAttack + (pAttacksUnit->m_dwMinWaitTime * 1000)))
			return FALSE;
		
	if (pAttacksUnit->m_vdwAttacks.size() <= 0)
		return FALSE;

	DWORD dwAttackIndex = xRandom(pAttacksUnit->m_vdwAttacks.size());

	__AEGON_SPECIAL_ATK* pAttackUnit = GetSpecialAttack(pAttacksUnit->m_vdwAttacks[dwAttackIndex]);
	if (!pAttackUnit)
		return FALSE;

	BOOL bDoAttack = xRandom(10000) < (10000 * pAttackUnit->m_fChance) ? TRUE : FALSE;

	return bDoAttack;
}
#endif

#endif //__AEGON_BOSS_MECHANICS