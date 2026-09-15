#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndManager.h"
#include "defineObj.h"
#include "DefineItem.h"
#include <afxdisp.h>
#include "defineNeuz.h"
#include "DialogMsg.h"
#include "DPClient.h"
#include "MsgHdr.h"
extern	CDPClient	g_DPlay;

#ifdef __PREMIUM
CWndPremium::CWndPremium()
{
}
CWndPremium::~CWndPremium()
{
}
void CWndPremium::OnDraw(C2DRender* p2DRender)
{
	//Premium Status
	CWndStatic* pStatus = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
	CString str;

#ifdef __AEGON_THEME_SWITCHER
	DWORD dwColorHighlightGreen = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_GREEN);
	DWORD dwColorHighlightRed = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_RED);
#endif //__AEGON_THEME_SWITCHER

	if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER3))
	{
		str.Format("Premium Status • Staff");
		pStatus->m_dwColor = dwColorHighlightGreen;
	}
	else if (g_pPlayer->IsPremium())
	{
		str.Format("Premium Status • Active!");
		pStatus->m_dwColor = dwColorHighlightGreen;
	}
	else
	{
		str.Format("Premium Status • Inactive...");
		pStatus->m_dwColor = dwColorHighlightRed;
	}
	pStatus->SetTitle(str);


	for (int i = 0; i < m_vCustomIDs.size(); i++)
	{
		LPWNDCTRL pWndCtrl = GetWndCtrl(m_vCustomIDs[i]);
		if (pWndCtrl)
		{
			if (i >= 0 && i < m_vCustomImages.size()) 
			{
				CTexture* pTexture = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(g_Neuz.m_pd3dDevice, m_vCustomImages[i].GetString(), COLOR_ITEM_BG);
				if(pTexture)
					pTexture->Render(p2DRender, pWndCtrl->rect.TopLeft());
			}
		}
	}
}

BOOL CWndPremium::Initialize(CWndBase* pWndParent, DWORD)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PREMIUM_MENU, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndPremium::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndPremium::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();

	MoveParentCenter();

	m_vCustomIDs.push_back(WIDC_CUSTOM1);
	m_vCustomImages.push_back("map064.tga");

	m_vCustomIDs.push_back(WIDC_CUSTOM2);
	m_vCustomImages.push_back("map021.tga");

	m_vCustomIDs.push_back(WIDC_CUSTOM3);
	m_vCustomImages.push_back("map034.tga");

	m_vCustomIDs.push_back(WIDC_CUSTOM4);
	m_vCustomImages.push_back("map029.tga");

	m_vCustomIDs.push_back(WIDC_CUSTOM5);
	m_vCustomImages.push_back("map078.tga");

	m_vCustomIDs.push_back(WIDC_CUSTOM6);
	m_vCustomImages.push_back("map044.tga");

	m_vCustomIDs.push_back(WIDC_CUSTOM7);
	m_vCustomImages.push_back("map020.tga");
}

void CWndPremium::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndPremium::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CString errPrem = "You need Premium to use this feature.";
	
	switch (nID)
	{
		case WIDC_BUTTON: // Shop Finder
		{
			if (g_pPlayer->IsPremium() || g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER3)) {
				if (!g_WndMng.GetApplet(APP_SHOP_FINDER))
					g_WndMng.CreateApplet(APP_SHOP_FINDER);
			}
			else {
				g_WndMng.PutString(errPrem, NULL, COLOR_ERROR);
			}
			break;
		}
		case WIDC_BUTTON1: // Character Bank
		{
			if (g_pPlayer->IsPremium() || g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER3)) {
				g_DPlay.SendOpenBankWnd(NULL_ID, 0);
			}
			else {
				g_WndMng.PutString(errPrem, NULL, COLOR_ERROR);
			}
			break;
		}
		case WIDC_BUTTON2: // Guild Storage
		{
			if (g_pPlayer->IsPremium() || g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER3)) {
				if (g_pPlayer->GetGuild() != NULL) 
				{
					g_DPlay.SendOpenGuildBankWnd();
				}
				else
				{
					g_WndMng.PutString("You are not in a Guild!", NULL, COLOR_ERROR);
				}
				
			}
			else {
				g_WndMng.PutString(errPrem, NULL, COLOR_ERROR);
			}
			break;
		}
		case WIDC_BUTTON3: // Mailbox
		{
			if (g_pPlayer->IsPremium() || g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER3)) {
				g_WndMng.CreateApplet(APP_INVENTORY);

				SAFE_DELETE(g_WndMng.m_pWndPost);
				g_WndMng.m_pWndPost = new CWndPost;
				g_WndMng.m_pWndPost->Initialize(NULL, APP_POST);
			}
			else {
				g_WndMng.PutString(errPrem, NULL, COLOR_ERROR);
			}
			break;
		}
#ifdef __SYS_AUTO_SHOUT
		case WIDC_BUTTON4: // Auto Shout
		{
			if (g_pPlayer->IsPremium() || g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER3)) {
				if (!g_WndMng.GetApplet(APP_AUTO_SHOUT))
					g_WndMng.CreateApplet(APP_AUTO_SHOUT);
			}
			else {
				g_WndMng.PutString(errPrem, NULL, COLOR_ERROR);
			}
			break;
		}
#endif //__SYS_AUTO_SHOUT
#ifdef __AEGON_CRAFTING_SYSTEM
		case WIDC_BUTTON5: // Crafting System
		{
			if (g_pPlayer->IsPremium() || g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER3)) {
				if (!g_WndMng.m_pWndCraftingMain)
				{
					SAFE_DELETE(g_WndMng.m_pWndCraftingMain);
					g_WndMng.m_pWndCraftingMain = new CWndCraftingMain;
					g_WndMng.m_pWndCraftingMain->Initialize();
				}
			}
			else {
				g_WndMng.PutString(errPrem, NULL, COLOR_ERROR);
			}
			break;
		}
#endif //__AEGON_CRAFTING_SYSTEM

#ifdef __ANARCHY_SYSTEM
		case WIDC_BUTTON6: // Anarchy Buffs
		{
			if (g_pPlayer->IsPremium() || g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER3)) {
				if (!g_WndMng.m_pWndAnarchy)
				{
					SAFE_DELETE(g_WndMng.m_pWndAnarchy);
					g_WndMng.m_pWndAnarchy = new CWndAnarchySkill;
					g_WndMng.m_pWndAnarchy->Initialize();
				}
			}
			else {
				g_WndMng.PutString(errPrem, NULL, COLOR_ERROR);
			}
			break;
		}
#endif
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#endif //__PREMIUM