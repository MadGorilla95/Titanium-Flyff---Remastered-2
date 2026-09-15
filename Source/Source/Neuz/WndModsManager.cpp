#include "stdafx.h"
#include "resData.h"
#include "WndModsManager.h"
#include "DPClient.h"
#include "defineText.h"

extern CDPClient g_DPlay;

#ifdef __MODS_MANAGER
CWndModsManager::CWndModsManager()
{
}

CWndModsManager::~CWndModsManager()
{
}

void CWndModsManager::OnDraw(C2DRender* p2DRender)
{
	if (pButton[0]) pButton[0]->SetTitle(g_pPlayer->IsMode(MODE_EXPUP_STOP) ? _T("Experience - Disabled") : _T("Experience - Enabled"));
	if (pButton[1]) pButton[1]->SetTitle(g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER) ? (g_pPlayer->IsMode(TRANSPARENT_MODE) ? _T("Invisibility - Enabled") : _T("Invisibility - Disabled")) : _T("You can't do this"));
	if (pButton[2]) pButton[2]->SetTitle(g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER3) ? (g_pPlayer->IsMode(MATCHLESS_MODE) ? _T("Undying - Enabled") : _T("Undying - Disabled")) : _T("You can't do this"));
	if (pButton[3]) pButton[3]->SetTitle(g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER3) ? (g_pPlayer->IsMode(MATCHLESS2_MODE) ? _T("Undying 2 - Enabled") : _T("Undying 2 - Disabled")) : _T("You can't do this"));
	if (pButton[4]) pButton[4]->SetTitle(g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER3) ? (g_pPlayer->IsMode(ONEKILL_MODE) ? _T("One Kill  - Enabled") : _T("One Kill  - Disabled")) : _T("You can't do this"));
	if (pButton[5]) pButton[5]->SetTitle(g_pPlayer->IsAuthHigher(AUTH_ADMINISTRATOR) ? (g_pPlayer->IsMode(FREE_MONEY_MODE) ? _T("Inf. Penyas - Enabled") : _T("Inf. Penyas - Disabled")) : _T("You can't do this"));
}

void CWndModsManager::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	pButton[0] = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	pButton[1] = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
	pButton[2] = (CWndButton*)GetDlgItem(WIDC_BUTTON3);
	pButton[3] = (CWndButton*)GetDlgItem(WIDC_BUTTON4);
	pButton[4] = (CWndButton*)GetDlgItem(WIDC_BUTTON5);
	pButton[5] = (CWndButton*)GetDlgItem(WIDC_BUTTON6);

	pButton[1]->EnableWindow(g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER));		
	pButton[2]->EnableWindow(g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER3));	
	pButton[3]->EnableWindow(g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER3));	
	pButton[4]->EnableWindow(g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER3));	
	pButton[5]->EnableWindow(g_pPlayer->IsAuthHigher(AUTH_ADMINISTRATOR));	

	MoveParentCenter();
}

BOOL CWndModsManager::Initialize(CWndBase* pWndParent, DWORD)
{
#ifdef __GAMEGUARD
	if (!g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
	{
		g_WndMng.PutString("Not allowed.", NULL, COLOR_ERROR);
		return FALSE;
	}
#endif //__GAMEGUARD
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_MODS_MANAGER, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndModsManager::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndModsManager::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndModsManager::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
		case WIDC_BUTTON1: g_DPlay.SendModsManager(0); break;
		case WIDC_BUTTON2: g_DPlay.SendModsManager(1); break;
		case WIDC_BUTTON3: g_DPlay.SendModsManager(2); break;
		case WIDC_BUTTON4: g_DPlay.SendModsManager(3); break;
		case WIDC_BUTTON5: g_DPlay.SendModsManager(4); break;
		case WIDC_BUTTON6: g_DPlay.SendModsManager(5); break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#endif // __MODS_MANAGER