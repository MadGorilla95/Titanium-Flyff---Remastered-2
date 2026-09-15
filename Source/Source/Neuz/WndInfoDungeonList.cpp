#include "stdafx.h"
#include "resData.h"
#include "WndInfoDungeonList.h"
#include <iostream>
#include "InstanceDungeonBase.h"

#ifdef __INFO_DUNGEONS
CWndInfoDungeonList::CWndInfoDungeonList()
{
	nPlayer = 0;
}
CWndInfoDungeonList::~CWndInfoDungeonList()
{
}

BOOL CWndInfoDungeonList::Process()
{
	if (!g_pPlayer)
	{
		nPlayer = 0;
		vInfoDungeonList.clear();
	}

	if (IsValidObj(g_pPlayer))
	{
		if (g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() != worldID)
		{
			nPlayer = 0;
			vInfoDungeonList.clear();
		}
	}
	return TRUE;
}

void CWndInfoDungeonList::OnInfoDungeonList(CAr& ar)
{
	vInfoDungeonList.clear();
	int nSize;
	ar >> nSize;
	ar >> nPlayer;
	ar >> g_pPlayer->m_bShowDungeonInfo;
	ar >> nStats;
	ar >> worldID;

	if (g_pPlayer->m_bShowDungeonInfo == FALSE)
		nPlayer = 0;

	for (int i = 0; i < nSize; i++)
	{
		INFO_DUNGEONLIST InfoDungeonList;
		ar >> InfoDungeonList.monsterID;
		ar >> InfoDungeonList.bIsKill;
		ar >> InfoDungeonList.nPhase;
		vInfoDungeonList.push_back(InfoDungeonList);
	}
}
void CWndInfoDungeonList::RenderDungeonList(C2DRender* p2DRender)
{
	if (!g_pPlayer)
		return;

	if (vInfoDungeonList.size() == 0 || nPlayer == 0)
		return;

	int PosY = 120;

	DWORD dwRenderColor = D3DCOLOR_TEMP(130, 30, 30, 30);
	DWORD dwNameColor = D3DCOLOR_TEMP(255, 255, 255, 250);
	CD3DFont* pOldFont = p2DRender->GetFont();
	
	p2DRender->RenderFillRect(CRect(5, PosY, 195, PosY + 50), dwRenderColor, dwRenderColor, dwRenderColor, dwRenderColor);

	char szTitle[255];	
	sprintf(szTitle, "Player(s) in dungeon: %d", nPlayer);
	CWndBase::m_Theme.m_pFontSRGiuld->DrawText(10, (float)PosY + 6, 1.0f, 1.0f, 0xFFf5f5f5, szTitle);

	p2DRender->SetFont(pOldFont);

	char szPhase[255];
	switch (nStats)
	{
		case ID_NORMAL: sprintf(szPhase, "Phase: Monster(s)"); break;
		case ID_MIDBOSS: sprintf(szPhase, "Phase: Mini-Boss"); break;
		case ID_BOSS: sprintf(szPhase, "Phase: Boss"); break;
		default: sprintf(szPhase, "Dungeon Cleared!"); break;
	}

	CWndBase::m_Theme.m_pFontSRGiuld->DrawText(10, (float)PosY + 28, 1.0f, 1.0f, 0xFFf5f5f5, szPhase);
	p2DRender->SetFont(pOldFont);
	
	PosY += 50;

	int nSize = 0;
	for (int i = 0; i < (int)vInfoDungeonList.size(); i++)
	{
		MoverProp* pMoverProp = prj.GetMoverProp(vInfoDungeonList[i].monsterID);
		if (vInfoDungeonList[i].nPhase == nStats)
		{
			nSize++;

			if (vInfoDungeonList[i].bIsKill)
				CWndBase::m_Theme.m_pFontSRGiuld->DrawText(10, (float)PosY + 8, 1.0f, 1.0f, 0xFF84e7b6, pMoverProp->szName);
			else
				CWndBase::m_Theme.m_pFontSRGiuld->DrawText(10, (float)PosY + 8, 1.0f, 1.0f, 0xFFEEEEEE, pMoverProp->szName);

			p2DRender->SetFont(pOldFont);

			PosY += 23;
		}
	}
}

CWndInfoDungeonList g_InfoDungeonList;
#endif // __INFO_DUNGEONS