#include "stdafx.h"
#include "WndCreateOwnTitle.h"
#include "ResData.h"
#include "defineText.h"
#include "DPClient.h"
extern CDPClient g_DPlay;

#ifdef __SYS_CUSTOM_TITLE
CWndCreateOwnTitle::CWndCreateOwnTitle()
{
}

CWndCreateOwnTitle::~CWndCreateOwnTitle()
{
}

void CWndCreateOwnTitle::OnDraw(C2DRender* p2DRender)
{
}

void CWndCreateOwnTitle::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	GetDlgItem(WIDC_STATIC1)->SetTitle("Enter your title:");
	MoveParentCenter();
}

BOOL CWndCreateOwnTitle::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_CUSTOM_TITLE, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndCreateOwnTitle::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndCreateOwnTitle::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndCreateOwnTitle::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return TRUE;
}

BOOL CWndCreateOwnTitle::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_BUTTON1)
	{
		CWndEdit* pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
		if (!pEdit)
			return NULL;

		CString str = pEdit->GetString();
		if (str.GetLength() < MIN_CUSTOM_TITLE || str.GetLength() > MAX_CUSTOM_TITLE)
		{
			g_WndMng.PutString("Your must enter a valid title between 4 and 20 characters long.", NULL, COLOR_ERROR);
			return FALSE;
		}

		g_DPlay.SendCreateOwnTitle(str);
		Destroy();
	}
	else
		if (nID == WIDC_BUTTON2)
			Destroy();

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#endif // __SYS_CUSTOM_TITLE