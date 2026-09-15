#include "stdafx.h"
#include "ResData.h"
#include "WndGuildTabWar.h"
#include "WndGuildWarDecl.h"
#include "WndGuildWarGiveUp.h"
#include "WndGuildWarPeace.h"

#ifdef __ROYAL_RUMBLE
#include "PlayerWar.h"
#include "defineText.h"
#include "DPClient.h"
#include "playerdata.h"
#include "WndPlayerRanking.h"

extern	CPlayerWar	g_War;
extern	CDPClient	g_DPlay;

#define MAX_RANKING 20

bool CompType_Points(SIEGE_RANK& first, SIEGE_RANK& second)
{
	if (first.nPoints > second.nPoints)
		return true;
	else
		return false;
}

CString GetJobIconPath(int nJob)
{
	CString strPath;
	switch (nJob)
	{
	case JOB_VAGRANT:
		strPath = MakePath("Icon\\", "vagi_rank.png"); break;
	case JOB_MERCENARY:
		strPath = MakePath("Icon\\", "merc_rank.png"); break;
	case JOB_ACROBAT:
		strPath = MakePath("Icon\\", "acro_rank.png"); break;
	case JOB_ASSIST:
		strPath = MakePath("Icon\\", "assist_rank.png"); break;
	case JOB_MAGICIAN:
		strPath = MakePath("Icon\\", "mage_rank.png"); break;
	case JOB_BLADE:
	case JOB_BLADE_MASTER:
	case JOB_BLADE_HERO:
	case JOB_STORMBLADE_HERO:
		strPath = MakePath("Icon\\", "blade_rank.png"); break;
	case JOB_KNIGHT:
	case JOB_KNIGHT_MASTER:
	case JOB_KNIGHT_HERO:
	case JOB_LORDTEMPLER_HERO:
		strPath = MakePath("Icon\\", "knight_rank.png"); break;
	case JOB_RINGMASTER:
	case JOB_RINGMASTER_MASTER:
	case JOB_RINGMASTER_HERO:
	case JOB_FLORIST_HERO:
		strPath = MakePath("Icon\\", "rm_rank.png"); break;
	case JOB_BILLPOSTER:
	case JOB_BILLPOSTER_MASTER:
	case JOB_BILLPOSTER_HERO:
	case JOB_FORCEMASTER_HERO:
		strPath = MakePath("Icon\\", "bp_rank.png"); break;
	case JOB_ELEMENTOR:
	case JOB_ELEMENTOR_MASTER:
	case JOB_ELEMENTOR_HERO:
	case JOB_ELEMENTORLORD_HERO:
		strPath = MakePath("Icon\\", "mage_rank.png"); break;
	case JOB_PSYCHIKEEPER:
	case JOB_PSYCHIKEEPER_MASTER:
	case JOB_PSYCHIKEEPER_HERO:
	case JOB_MENTALIST_HERO:
		strPath = MakePath("Icon\\", "psy_rank.png"); break;
	case JOB_RANGER:
	case JOB_RANGER_MASTER:
	case JOB_RANGER_HERO:
	case JOB_CRACKSHOOTER_HERO:
		strPath = MakePath("Icon\\", "ranger_rank.png"); break;
	case JOB_JESTER:
	case JOB_JESTER_MASTER:
	case JOB_JESTER_HERO:
	case JOB_WINDLURKER_HERO:
		strPath = MakePath("Icon\\", "jester_rank.png"); break;
	default: return strPath = MakePath("Icon\\", "vagi_rank.png");
	}
	return strPath;
}

CWndPlayerRanking::CWndPlayerRanking()
{
}

CWndPlayerRanking::~CWndPlayerRanking()
{
}

void CWndPlayerRanking::OnDraw(C2DRender* p2DRender)
{
	CTexture* pTexture = NULL;
	DWORD dwColor = D3DCOLOR_ARGB(255, 46, 112, 169);
	DWORD dwColor2 = D3DCOLOR_ARGB(255, 0, 0, 0);
	int	sx, sy;
	sx = 8;
	sy = 20;
	p2DRender->TextOut(sx + 10, 17 - 3, prj.GetText(TID_PLAYER_WAR_06), dwColor);
	p2DRender->TextOut(sx + 75, 17 - 3, prj.GetText(TID_PLAYER_WAR_08), dwColor);
	p2DRender->TextOut(sx + 170, 17 - 3, prj.GetText(TID_PLAYER_WAR_07), dwColor);
	p2DRender->TextOut(sx + 260, 17 - 3, prj.GetText(TID_PLAYER_WAR_09), dwColor);
	p2DRender->TextOut(sx + 340, 17 - 3, prj.GetText(TID_PLAYER_WAR_10), dwColor);

	// line's
	p2DRender->RenderLine(CPoint(sx, 32), CPoint(sx + 400, 32), dwColor2);
	p2DRender->RenderLine(CPoint(sx + 40, 32), CPoint(sx + 40, 500), dwColor2);
	p2DRender->RenderLine(CPoint(sx + 145, 32), CPoint(sx + 145, 500), dwColor2);
	p2DRender->RenderLine(CPoint(sx + 230, 32), CPoint(sx + 230, 500), dwColor2);
	p2DRender->RenderLine(CPoint(sx + 320, 32), CPoint(sx + 320, 500), dwColor2);
	sy = 20;
	std::sort(g_War.m_vecRankSiege.begin(), g_War.m_vecRankSiege.end(), CompType_Points);
	for (u_int i = 0; i < g_War.m_vecRankSiege.size(); i++)
	{
		int nNameLen = strlen(g_War.m_vecRankSiege[i].szName);
		if (nNameLen < 4)
			return;

		if (i > MAX_RANKING)
			break;

		sy += 6;
		pTexture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, GetJobIconPath(g_War.m_vecRankSiege[i].nJob), 0xffff00ff);
		p2DRender->TextOut(sx + 10, sy + 8, i + 1, D3DCOLOR_ARGB(255, 100, 100, 255)); // render '#'
		p2DRender->TextOut(sx + 70, sy + 8, g_War.m_vecRankSiege[i].szName, D3DCOLOR_ARGB(255, 100, 100, 255));
		p2DRender->TextOut(sx + 180, sy + 8, g_War.m_vecRankSiege[i].nPoints, D3DCOLOR_ARGB(255, 100, 100, 255));
		p2DRender->TextOut(sx + 265, sy + 8, g_War.m_vecRankSiege[i].nKills, D3DCOLOR_ARGB(255, 100, 100, 255));
		p2DRender->TextOut(sx + 355, sy + 8, g_War.m_vecRankSiege[i].nDeaths, D3DCOLOR_ARGB(255, 100, 100, 255));
		if (pTexture != NULL)
			pTexture->Render(p2DRender, CPoint(sx + 52, sy + 8), 255);

		sy = sy + 12;
	}
}

void CWndPlayerRanking::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CWndPlayerRanking::OnInitialUpdate()
{
	g_DPlay.SendGetSiegeRanking(g_pPlayer->m_idPlayer, TRUE);
	CWndNeuz::OnInitialUpdate();

	g_War.m_vecRankSiege.clear();

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	if (pButton)
		pButton->SetVisible(FALSE);

	MoveParentCenter();
}
void CWndPlayerRanking::RefreshList()
{
}

BOOL CWndPlayerRanking::Initialize(CWndBase* pWndParent, DWORD)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SIEGE_RANKING, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndPlayerRanking::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

BOOL CWndPlayerRanking::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#endif // __ROYAL_RUMBLE
