#include "stdafx.h"
#include "AppDefine.h"
#include "WndPlayerPurge.h"

#ifdef __BAN_SYSTEM
CWndPlayerPurge::CWndPlayerPurge()
{
}

CWndPlayerPurge::~CWndPlayerPurge()
{
}

void CWndPlayerPurge::OnDraw(C2DRender* p2DRender)
{
}

void CWndPlayerPurge::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndEdit* pWndPlayerName = (CWndEdit*)GetDlgItem(WIDC_EDIT);
	CWndComboBox* pWndMethod = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX);
	CWndEdit* pWndDuration = (CWndEdit*)GetDlgItem(WIDC_EDIT1);

	pWndMethod->SetString("Mute");
	pWndMethod->AddString("Mute");
	pWndMethod->AddString("Kick");
	pWndMethod->AddString("BAN");

	pWndDuration->AddWndStyle(EBS_NUMBER);
	pWndMethod->AddWndStyle(EBS_READONLY);
}

BOOL CWndPlayerPurge::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
#ifdef __GAMEGUARD
	if (!g_pPlayer->IsAuthHigher(AUTH_ADMINISTRATOR))
	{
		g_WndMng.PutString("Not allowed.", NULL, COLOR_ERROR);
		return FALSE;
	}
#endif //__GAMEGUARD
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PLAYER_PURGE, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndPlayerPurge::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndPlayerPurge::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndPlayerPurge::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CWndEdit* pWndPlayerName = (CWndEdit*)GetDlgItem(WIDC_EDIT);
	CWndEdit* pWndDuration = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	CString string;

	DWORD dwDuration;
	if (pWndDuration->m_string.GetLength())
	{
		dwDuration = _ttoi(pWndDuration->m_string);
	}
	else
	{
		dwDuration = 1;
	}

	CString strPlayerName;
	strPlayerName = pWndPlayerName->m_string;

	switch (nID)
	{
		case WIDC_BUTTON:
		{
			string.Format("/ban %s %d", strPlayerName, dwDuration);
			ParsingCommand(string.LockBuffer(), g_pPlayer);
			string.UnlockBuffer();
			Destroy(TRUE);
			return TRUE;
		}
		break;
		case WIDC_BUTTON1:
		{
			Destroy(TRUE);
			return TRUE;
		}
		break;
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#endif //__BAN_SYSTEM