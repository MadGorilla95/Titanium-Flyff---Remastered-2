#include "stdafx.h"
#include "resData.h"
#include "WndAegonPvPRanking.h"
#include "DPClient.h"
#include "defineText.h"
#include "WndManager.h"
#include "WndManager.h"
#include "Model.h"
#include "ModelMng.h"
#include "ModelObject.h"
#include "ModelGlobal.h"
#include "math.h"

extern CDPClient g_DPlay;

#ifdef __AEGON_PVP_RANKING
DWORD WINAPI __PvPRankingSortThread(LPVOID lpParam)
{
	CWndPvPRankingMain* pWiki = (CWndPvPRankingMain*)lpParam;
	pWiki->SortVector();
	return 0;
}

///////////////////////////
///////LIST WINDOW/////////
///////////////////////////
CWndPvPRankingListBox::CWndPvPRankingListBox()
{

}
CWndPvPRankingListBox::~CWndPvPRankingListBox()
{

}
void CWndPvPRankingListBox::OnDraw(C2DRender* p2DRender)
{
	CWndPvPRankingMain* pWndPvPRankingMain = (CWndPvPRankingMain*)GetParentWnd();
	m_nFontHeight = 40;
	CPoint pt(10, 3);

	CRect rectClient = GetClientRect();

	int nRange = pWndPvPRankingMain->m_vecItems.size();
	int scrollPos = m_wndScrollBar.GetScrollPos();

	int nPage = rectClient.Height() / m_nFontHeight;
	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);

	int index = 0;
	for (unsigned i = (unsigned)m_wndScrollBar.GetScrollPos(); i < pWndPvPRankingMain->m_vecItems.size(); i++)
	{
		if (i > (unsigned)(nPage + m_wndScrollBar.GetScrollPos()))
			break;

		pt.x = 10;

		CString strTmp;

		DWORD dwColor;
		dwColor = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();

		// Render Line
		p2DRender->RenderLine(CPoint(0, pt.y + 37), CPoint(rectClient.right - 3, pt.y + 37), dwColor);

#ifdef __AEGON_THEME_SWITCHER
		DWORD RenderFillRect = CAegonThemeSwitcherMng::GetInstance()->GetRenderFillRectColor();
		DWORD RenderRect = CAegonThemeSwitcherMng::GetInstance()->GetRenderRectColor();
#endif //__AEGON_THEME_SWITCHER

		if (i == m_nCurSelect)
		{
			CRect DrawRect = CRect(0, pt.y, rectClient.right - 3, pt.y + 34);
#ifdef __AEGON_THEME_SWITCHER
			p2DRender->RenderFillRect(DrawRect, RenderFillRect);
			p2DRender->RenderRect(DrawRect, RenderRect);
#else //__AEGON_THEME_SWITCHER
			p2DRender->RenderFillRect(DrawRect, 0xFFf8e6cc);
			p2DRender->RenderRect(DrawRect, 0xFFf28818);
#endif //__AEGON_THEME_SWITCHER
			dwColor = 0xFF000000;
		}

		//draw stuff here
		pt.x = pWndPvPRankingMain->m_pWndStaticRank->GetWndRect().left;
		strTmp.Format("%d", i + 1);
		p2DRender->TextOut(pt.x, pt.y + 10, strTmp.GetString(), dwColor);


		pt.x = pWndPvPRankingMain->m_pWndStaticName->GetWndRect().left;
		p2DRender->TextOut(pt.x, pt.y + 10, pWndPvPRankingMain->m_vecItems[i]->m_szName, dwColor);
		//~draw stuff here

		pt.y += m_nFontHeight;
	}
}
void CWndPvPRankingListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	CPoint pt(3, 3);


	CWndPvPRankingMain* pWndPvPRankingMain= (CWndPvPRankingMain*)GetParentWnd();
	CRect rect;

	for (int i = 0; i < pWndPvPRankingMain->m_vecItems.size(); i++)
	{
		int nScrollBarWidth = IsWndStyle(WBS_VSCROLL) ? m_wndScrollBar.GetClientRect().Width() : 0;
		rect.SetRect(pt.x, pt.y, pt.x + m_rectWindow.Width() - nScrollBarWidth, pt.y + m_nFontHeight);
		if (rect.PtInRect(point))
		{
			m_nCurSelect = i + m_wndScrollBar.GetScrollPos();
		}
		pt.y += m_nFontHeight;
	}
		
}
///////////////////////////
///////MAIN WINDOW/////////
///////////////////////////
CWndPvPRankingMain::CWndPvPRankingMain()
{
	m_hSortThread = INVALID_HANDLE_VALUE;
	m_nSortMode = 0;
	m_bSortDesc = TRUE;
}
CWndPvPRankingMain::~CWndPvPRankingMain()
{
	m_vecItems.clear();
	TerminateThread(m_hSortThread, 0);
}
void CWndPvPRankingMain::OnDraw(C2DRender* p2DRender)
{

	CString strTmp;
	CEditString strEditOut;
	CPoint point = m_pWndStaticStatField->GetWndRect().TopLeft();
	point.x += 10;
	point.y += 8;
	strEditOut.Init(p2DRender->GetFont(), &m_pWndStaticStatField->GetWndRect());

#ifdef __AEGON_THEME_SWITCHER
	DWORD dwStaticColor = CAegonThemeSwitcherMng::GetInstance()->GetStaticColor();
#endif //__AEGON_THEME_SWITCHER

	__PVPRANKING_PROGRESS_UNIT* selectedStats = NULL;
	
	if (m_pWndRankListBox.m_nCurSelect >= 0 && m_pWndRankListBox.m_nCurSelect < m_vecItems.size())
	{
		selectedStats = m_vecItems[m_pWndRankListBox.m_nCurSelect];
	}

	if (selectedStats) 
	{
		strTmp.Format("Player: %s", selectedStats->m_szName);
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		strEditOut.SetStyle(0, strlen("Player:"),ESSTY_BOLD | ESSTY_UNDERLINE);
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD | ESSTY_UNDERLINE);
		point.x += p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cx + 20;

		strTmp.Format("Rank: %d", m_pWndRankListBox.m_nCurSelect + 1);
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		strEditOut.SetStyle(0, strlen("Rank:"), ESSTY_BOLD | ESSTY_UNDERLINE);
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD | ESSTY_UNDERLINE);
		point.x += p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cx + 20;

		strTmp.Format("PvP Score: %d", selectedStats->m_nTotalPoints);
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		strEditOut.SetStyle(0, strlen("PvP Score:"), ESSTY_BOLD | ESSTY_UNDERLINE);
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD | ESSTY_UNDERLINE);

		point.x = m_pWndStaticStatField->GetWndRect().left + 10;
		point.y += p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy + 10;
		//new line

		LONG statBeginY = point.y;

		strTmp.Format("Guildwar stats:");
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		strEditOut.SetStyle(ESSTY_BOLD | ESSTY_UNDERLINE);
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD | ESSTY_UNDERLINE);
		point.y += p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy + 5;

		strTmp.Format("Kills: %d", selectedStats->m_Guildwar.m_nKills);
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		point.y += p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy + 5;

		strTmp.Format("Deaths: %d", selectedStats->m_Guildwar.m_nDeaths);
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		point.y += p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy + 5;

		strTmp.Format("Participations: %d", selectedStats->m_Guildwar.m_nParticipations);
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		point.y += p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy + 5;

		strTmp.Format("Wins: %d", selectedStats->m_Guildwar.m_nWins);
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		point.y += p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy + 5;

		strTmp.Format("MVP: %d", selectedStats->m_Guildwar.m_nMvp);
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		point.y += p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy + 10;

		strTmp.Format("Tournament stats:");
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		strEditOut.SetStyle(ESSTY_BOLD | ESSTY_UNDERLINE);
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD | ESSTY_UNDERLINE);
		point.y += p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy + 5;

		strTmp.Format("Kills: %d", selectedStats->m_Tournament.m_nKills);
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		point.y += p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy + 5;

		strTmp.Format("Deaths: %d", selectedStats->m_Tournament.m_nDeaths);
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		point.y += p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy + 5;

		strTmp.Format("Participations: %d", selectedStats->m_Tournament.m_nParticipations);
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		point.y += p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy + 5;

		strTmp.Format("Wins: %d", selectedStats->m_Tournament.m_nWins);
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		point.y += p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy + 10;


		point.y = statBeginY;
		point.x = m_pWndStaticStatField->GetWndRect().CenterPoint().x;

		strTmp.Format("Arena stats:");
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		strEditOut.SetStyle(ESSTY_BOLD | ESSTY_UNDERLINE);
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD | ESSTY_UNDERLINE);
		point.y += p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy + 5;

		strTmp.Format("Kills: %d", selectedStats->m_Arena.m_nKills);
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		point.y += p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy + 5;

		strTmp.Format("Deaths: %d", selectedStats->m_Arena.m_nDeaths);
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		point.y += p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy + 10;

		strTmp.Format("Duel stats:");
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		strEditOut.SetStyle(ESSTY_BOLD | ESSTY_UNDERLINE);
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD | ESSTY_UNDERLINE);
		point.y += p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy + 5;

		strTmp.Format("Kills: %d", selectedStats->m_Duel.m_nKills);
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		point.y += p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy + 5;

		strTmp.Format("Deaths: %d", selectedStats->m_Duel.m_nDeaths);
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		point.y += p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy + 10;

	}
	else 
	{

	}	
	//p2DRender->RenderRect(m_pWndStaticStatField->GetWndRect(), COLOR_ERROR);
}
void CWndPvPRankingMain::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	//Move Window to center
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();

	CRect rectListBox(8, 33, 200, 360);
	m_pWndRankListBox.Create(WBS_VSCROLL, rectListBox, this, APP_PVP_RANKING_LIST);


	m_pWndStaticStatField = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	m_pWndStaticRank = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	m_pWndStaticName = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
	m_pWndStaticStatsHeader = (CWndStatic*)GetDlgItem(WIDC_STATIC4);

	g_DPlay.SendGetPvPRanking(0);

}
BOOL CWndPvPRankingMain::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PVP_RANKING, 0, CPoint(0, 0), pWndParent);
}
void CWndPvPRankingMain::OnDestroy()
{
}
BOOL CWndPvPRankingMain::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndPvPRankingMain::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndPvPRankingMain::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndPvPRankingMain::OnLButtonDown(UINT nFlags, CPoint point)
{

}
BOOL CWndPvPRankingMain::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_BUTTON1: //refresh
	{
		g_DPlay.SendGetPvPRanking(1);
		break;
	}
	case WIDC_BUTTON2: //close
	{
		Destroy();
		break;
	}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndPvPRankingMain::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}
BOOL CWndPvPRankingMain::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	return TRUE;
}

void CWndPvPRankingMain::OnPvPRankingRefresh() 
{
	FillListVector();
}

void CWndPvPRankingMain::FillListVector() 
{
	m_vecItems.clear();
	map<u_long, __PVPRANKING_PROGRESS_UNIT>::iterator it = CAegonPvPRankingMng::GetInstance()->playerRankingMap.begin();
	for (; it != CAegonPvPRankingMng::GetInstance()->playerRankingMap.end(); it++) 
	{
		m_vecItems.push_back(&it->second);
	}


	TerminateThread(m_hSortThread, 0);
	m_hSortThread = CreateThread(0, 0, __PvPRankingSortThread, this, 0, 0);
}


bool SortByTotalPoints(const __PVPRANKING_PROGRESS_UNIT* pPropLeft, const __PVPRANKING_PROGRESS_UNIT* pPropRight)
{
	return (pPropLeft->m_nTotalPoints > pPropRight->m_nTotalPoints);
}

void CWndPvPRankingMain::SortVectorByPoints()
{
	std::sort(m_vecItems.begin(), m_vecItems.end(), SortByTotalPoints);
}

void CWndPvPRankingMain::SortVector()
{
	m_pWndRankListBox.EnableWindow(FALSE);

	switch (m_nSortMode) 
	{
	case 0:
		SortVectorByPoints();
		break;
	case 1:
		break;
	default:
		SortVectorByPoints();
		break;
	}
	
	m_pWndRankListBox.EnableWindow(TRUE);
}
#endif // __AEGON_PVP_RANKING