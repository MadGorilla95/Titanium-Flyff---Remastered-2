#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndManager.h"
#include "WndAutoShout.h"

#ifdef __CLIENT
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif // CLIENT

#ifdef __SYS_AUTO_SHOUT
CWndAutoShout::CWndAutoShout()
{
}
CWndAutoShout::~CWndAutoShout()
{
}

void CWndAutoShout::OnMouseWndSurface(CPoint point)
{
	for (int i = NULL; i < MAX_AUTO_SHOUT; i++)
	{
		if (!g_Option.m_Shouts[i].IsEmpty())
		{
			CRect hitrect = GetWndCtrl(WIDC_STATICP01 + i)->rect;
			CPoint point = GetMousePoint();
			if (hitrect.PtInRect(point))
			{
				CPoint point2 = point;
				ClientToScreen(&point2);
				ClientToScreen(&hitrect);
#ifdef _WIN64
				g_toolTip.PutToolTip((ULONG_PTR)this, g_Option.m_Shouts[i], hitrect, point2, 0);
#else
				g_toolTip.PutToolTip((DWORD)this, g_Option.m_Shouts[i], hitrect, point2, 0);
#endif
			}
		}
	}
}

void CWndAutoShout::OnDraw(C2DRender* p2DRender)
{
	for (int i = NULL; i < MAX_AUTO_SHOUT; i++)
	{
		CRect rct = GetWndCtrl(WIDC_STATICP01 + i)->rect;
		if (!g_Option.m_Shouts[i].IsEmpty())
		{
			CTexture* pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "icon_MacroChat.dds"), COLOR_ITEM_BG);
			if (pTexture)
				pTexture->Render(p2DRender, rct.TopLeft());
		}
	}
}

void CWndAutoShout::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

#ifdef __AEGON_THEME_SWITCHER
	DWORD dwColor = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
#endif //__AEGON_THEME_SWITCHER

	for (int i = 0; i < MAX_AUTO_SHOUT; i++)
	{
		m_pWndButton[i] = (CWndButton*)GetDlgItem(WIDC_CHECKP01 + i);
	}

	// clear values
	for (int i = NULL; i < MAX_AUTO_SHOUT; i++)
	{
		if (g_Option.m_Shouts[i].IsEmpty())
		{
			g_Option.m_CheckShouts[i] = false;
			g_Option.m_Shouts[i] = "";
			m_pWndButton[i]->EnableWindow(false);
		}
		else
		{
			m_pWndButton[i]->EnableWindow(true);
			m_pWndButton[i]->SetCheck(g_Option.m_CheckShouts[i]);
		}
	}

	// resdata
	for (size_t i = 0; i < MAX_AUTO_SHOUT; i++)
	{
		CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_SHOUT_STATIC9 + i);
		CString strFormat;
		strFormat.Format("%d Min", i + 3);
		pWndStatic->SetTitle(strFormat);
#ifdef __AEGON_THEME_SWITCHER
		pWndStatic->m_dwColor = dwColor;
#else //__AEGON_THEME_SWITCHER
		pWndStatic->m_dwColor = 0xff000000;
#endif //__AEGON_THEME_SWITCHER
	}

	MoveParentCenter();
}

BOOL CWndAutoShout::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_AUTO_SHOUT, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndAutoShout::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
	if (pShortcut->m_dwShortcut != SHORTCUT_CHAT)
		return FALSE;

	for (int i = NULL; i < MAX_AUTO_SHOUT; i++)
	{
		if (GetWndCtrl(WIDC_STATICP01 + i)->rect.PtInRect(point))
		{
			UpdateShout(i, pShortcut->m_szString);
			return TRUE;
		}
	}
	return CWndNeuz::OnDropIcon(pShortcut, point);
}
void CWndAutoShout::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	for (int i = NULL; i < MAX_AUTO_SHOUT; i++)
	{
		if (GetWndCtrl(WIDC_STATICP01 + i)->rect.PtInRect(point))
		{
			UpdateShout(i, "");
			return;
		}
	}
}

void CWndAutoShout::OnRButtonDown(UINT nFlags, CPoint point)
{
	for (int i = NULL; i < MAX_AUTO_SHOUT; i++)
	{
		if (GetWndCtrl(WIDC_STATICP01 + i)->rect.PtInRect(point))
		{
			UpdateShout(i, "");
			return;
		}
	}
}

BOOL CWndAutoShout::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	for (int i = NULL; i < MAX_AUTO_SHOUT; i++)
	{
		if (nID == WIDC_CHECKP01 + i)
		{
			g_Option.m_CheckShouts[i] = !g_Option.m_CheckShouts[i];
			break;
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndAutoShout::UpdateShout(int i, CString str)
{
	g_Option.m_Shouts[i] = str;

	bool isNull = str.IsEmpty();
	m_pWndButton[i]->SetCheck(false);
	m_pWndButton[i]->EnableWindow(!isNull);

	g_Option.m_CheckShouts[i] = false;
}

void CWndAutoShout::OnDestroy()
{
	g_Option.SaveShout();
}
#endif // __SYS_AUTO_SHOUT