#include "stdafx.h"
#include "WndFFA.h"
#include "ResData.h"
#include "DPClient.h"
extern CDPClient g_DPlay;

#ifdef __ROYAL_RUMBLE
CWndJoinPlayerSiege::CWndJoinPlayerSiege()
{
}

CWndJoinPlayerSiege::~CWndJoinPlayerSiege()
{
}

void CWndJoinPlayerSiege::OnDraw(C2DRender* p2DRender)
{
}

void CWndJoinPlayerSiege::OnInitialUpdate()
{
	g_DPlay.SendGetSiegeRanking(g_pPlayer->m_idPlayer, FALSE);
	CWndNeuz::OnInitialUpdate();
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	if (pWndText)
	{
		pWndText->SetString("Royal Rumble will start when enough players are registered. Once enough player have signed up, the server will be noticed that a Royal Rumble is about to start. After 10 minutes, you can enter the combat zone.", CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
	}

	for (DWORD i = 0; i < g_War.m_vecWarPlayers.size(); i++)
		pWndListBox->AddString(g_War.m_vecWarPlayers[i].m_szName);
		

	CSize size = g_Neuz.m_2DRender.m_pFont->GetTextExtent("123");
	if (size.cy + 2 > 16)
	{
		CRect rect;
		LPWNDCTRL lpWndCtrl;
		lpWndCtrl = GetWndCtrl(WIDC_LISTBOX1);
		rect = lpWndCtrl->rect;
		rect.bottom += ((size.cy + 2) * 10) - (rect.bottom - rect.top) + 4;
		pWndListBox->SetWndRect(rect);
	}

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndJoinPlayerSiege::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SIEGE_JOIN, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndJoinPlayerSiege::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndJoinPlayerSiege::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndJoinPlayerSiege::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return TRUE;
}

BOOL CWndJoinPlayerSiege::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_BUTTON1:
	{
		Destroy();
		break;
	}
	case WIDC_BUTTON2:
	{
		g_DPlay.SendJoinWarPlayer(TRUE, FALSE);
		Destroy();
		break;
	}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#endif // __ROYAL_RUMBLE