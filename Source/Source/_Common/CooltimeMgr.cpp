#include "stdafx.h"
#include "misc.h"
#include "CooltimeMgr.h"
#include "ProjectCmn.h"

CCooltimeMgr::CCooltimeMgr()
{
	ZeroMemory( m_times, sizeof(m_times) );
	ZeroMemory( m_bases, sizeof(m_bases) );
}
	
CCooltimeMgr::~CCooltimeMgr()
{
}

DWORD CCooltimeMgr::GetGroup(ItemProp* pItemProp)
{
	DWORD dwCooltimeItemGroup = 0;

	if (pItemProp->dwSkillReady > 0)
	{
#ifdef __ITEM_COOLDOWN
		if (!pItemProp)
			dwCooltimeItemGroup = 0;

		switch (pItemProp->dwItemKind2)
		{
		case IK2_FOOD:
			dwCooltimeItemGroup = 1;
			if (pItemProp->dwItemKind3 == IK3_PILL)
			{
				dwCooltimeItemGroup = 2;
			}
			if (pItemProp->dwID == II_CHR_FOO_COO_REMANTIS)
			{
				dwCooltimeItemGroup = 6;
			}
			break;
		case IK2_SKILL:
			dwCooltimeItemGroup = 3;
			break;
		case IK2_SYSTEM:
			if (pItemProp->dwID == II_SYS_SYS_SCR_HOLY)
				dwCooltimeItemGroup = 5;
			break;
		}
#else //__ITEM_COOLDOWN
		switch (pItemProp->dwItemKind2)
		{
		case IK2_FOOD:
			dwCooltimeItemGroup = 1;
			if (pItemProp->dwItemKind3 == IK3_PILL)
				dwCooltimeItemGroup = 2;
			break;
		case IK2_SKILL:
			dwCooltimeItemGroup = 3;
			break;
		}
#endif //__ITEM_COOLDOWN
	}
	return dwCooltimeItemGroup;
}

// dwGroup을 사용할 수 있는가?
BOOL CCooltimeMgr::CanUse( DWORD dwGroup )
{
	ASSERT( dwGroup > 0 );
	return g_tmCurrent > GetTime( dwGroup );		// 재사용 시각을 넘으면 사용가능 
}

// 사용한 시각을 기록해서, CanUse에서 판단 근거로 사용한다.
void CCooltimeMgr::SetTime( DWORD dwGroup, DWORD dwCoolTime )
{
	ASSERT( dwGroup > 0 && dwCoolTime > 0 );
	DWORD dwTick = g_tmCurrent;

	m_times[dwGroup - 1] = dwTick + dwCoolTime;
	m_bases[dwGroup - 1] = dwTick;
}	


