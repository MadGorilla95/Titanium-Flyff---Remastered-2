#include "stdafx.h"
#include "ResData.h"
#include "DPClient.h"
#include "WndCrystal.h"
#include "DefineText.h"
extern	CDPClient	g_DPlay;

#ifdef __CRYSTAL
CWndCrystal::CWndCrystal()
{
	pItem1 = NULL;
	pItem2 = NULL;
	pItem3 = NULL;

	pItem1Tex = NULL;
	pItem2Tex = NULL;
	pItem3Tex = NULL;

	pCustom = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "WndBarunaEnchant03.bmp"), COLOR_ITEM_BG);
}

CWndCrystal::~CWndCrystal()
{
	if (pItem1)
	{
		pItem1->SetExtra(0);

		if (pItem1Tex)
			pItem1Tex = NULL;

		pItem1 = NULL;
	}

	if (pItem2)
	{
		pItem2->SetExtra(0);

		if (pItem2Tex)
			pItem2Tex = NULL;

		pItem2 = NULL;
	}

	if (pItem3)
	{
		if (pItem3Tex)
			pItem3Tex = NULL;

		pItem3 = NULL;
	}
}
void CWndCrystal::OnDraw(C2DRender* p2DRender)
{
	LPWNDCTRL pCtrl = GetWndCtrl(WIDC_CUSTOM1);
	if (pCtrl)
	{
		CPoint DrawPoint = pCtrl->rect.TopLeft();
		DrawPoint.y -= 2;
		pCustom->Render(p2DRender, DrawPoint, CPoint(32, 32));
		if (pItem1Tex)
			pItem1Tex->Render(p2DRender, DrawPoint, CPoint(32, 32));
	}

	pCtrl = GetWndCtrl(WIDC_CUSTOM2);
	if (pCtrl)
	{
		CPoint DrawPoint = pCtrl->rect.TopLeft();
		DrawPoint.y -= 2;
		pCustom->Render(p2DRender, DrawPoint, CPoint(32, 32));
		if (pItem2Tex)
			pItem2Tex->Render(p2DRender, DrawPoint, CPoint(32, 32));
	}

	pCtrl = GetWndCtrl(WIDC_CUSTOM3);
	if (pCtrl)
	{
		CPoint DrawPoint = pCtrl->rect.TopLeft();
		DrawPoint.y -= 2;
		pCustom->Render(p2DRender, DrawPoint, CPoint(32, 32));
		if (pItem3Tex)
			pItem3Tex->Render(p2DRender, DrawPoint, CPoint(32, 32));
	}
}

void CWndCrystal::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();

	if (!g_WndMng.GetApplet(APP_INVENTORY))
		g_WndMng.CreateApplet(APP_INVENTORY);
}

BOOL CWndCrystal::Initialize(CWndBase* pWndParent, DWORD)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_CRYSTAL, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndCrystal::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndCrystal::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndCrystal::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndCrystal::OnLButtonDown(UINT nFlags, CPoint point)
{
}

BOOL CWndCrystal::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	LPWNDCTRL pCtrl = GetWndCtrl(WIDC_CUSTOM1);
	CItemElem* pItem = (CItemElem*)g_pPlayer->GetItemId(pShortcut->m_dwId);

	if (!pItem)
		return FALSE;

	// pCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	if (pCtrl && pCtrl->rect.PtInRect(point))
	{
		if (pItem1)
		{
			pItem1->SetExtra(0);

			if (pItem1Tex)
				pItem1Tex = NULL;

			pItem1 = NULL;
		}

		if (pItem2)
		{
			pItem2->SetExtra(0);

			if (pItem2Tex)
				pItem2Tex = NULL;

			pItem2 = NULL;
		}

		if (pItem3)
		{
			if (pItem3Tex)
				pItem3Tex = NULL;

			pItem3 = NULL;
		}

		if (IsValidCrystal(pItem))
		{
			pItem1 = pItem;
			pItem1->SetExtra(1);
			pItem1Tex = CWndBase::m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_ITEM, pItem1->GetProp()->szIcon), 0xffff00ff);
			return TRUE;
		}
		else // invalid weapon
		{
			g_WndMng.PutString("Invalid weapon...", 0, 0xFFFF0000);
			return FALSE;
		}
	}

	pCtrl = GetWndCtrl(WIDC_CUSTOM2);
	if (pCtrl && pCtrl->rect.PtInRect(point))
	{
		if (!pItem1) // no first weapon
		{
			g_WndMng.PutString("Put an ultimate weapon +10 first", 0, 0xFFFF0000);
			return FALSE;
		}
		else
		{
			if (pItem2)
			{
				pItem2->SetExtra(0);

				if (pItem2Tex)
					pItem2Tex = NULL;

				pItem2 = NULL;
			}

			if (pItem3)
			{
				if (pItem3Tex)
					pItem3Tex = NULL;

				pItem3 = NULL;
			}

			if (pItem->GetProp()->dwItemKind3 == IK3_CRYSTAL)
			{
				pItem2 = pItem;
				pItem2Tex = CWndBase::m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_ITEM, pItem2->GetProp()->szIcon), 0xffff00ff);
				pItem2->SetExtra(1);

				pItem3 = new CItemElem;
				*pItem3 = *pItem1;
				pItem3->SetCrystal(pItem2->GetProp()->dwID);
				pItem3Tex = CWndBase::m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_ITEM, pItem3->GetProp()->szIcon), 0xffff00ff);
				return TRUE;
			}
			else
			{
				g_WndMng.PutString("Sorry, that item is not a Crystal available", 0, 0xFFFF0000);
				return FALSE;
			}
		}
	}
	return TRUE;
}

void CWndCrystal::OnMouseWndSurface(CPoint point)
{
	LPWNDCTRL pCtrl = GetWndCtrl(WIDC_CUSTOM1);
	CRect Draw;
	CPoint p2;
	if (pCtrl && pCtrl->rect.PtInRect(point))
	{
		if (!pItem1)
			return;
		p2 = point;
		Draw = pCtrl->rect;
		ClientToScreen(&p2);
		ClientToScreen(&Draw);
		g_WndMng.PutToolTip_Item(pItem1, p2, &Draw);
		return;
	}

	pCtrl = GetWndCtrl(WIDC_CUSTOM2);
	if (pCtrl && pCtrl->rect.PtInRect(point))
	{
		if (!pItem2)
			return;
		p2 = point;
		Draw = pCtrl->rect;
		ClientToScreen(&p2);
		ClientToScreen(&Draw);
		g_WndMng.PutToolTip_Item(pItem2, p2, &Draw);
		return;
	}

	pCtrl = GetWndCtrl(WIDC_CUSTOM3);
	if (pCtrl && pCtrl->rect.PtInRect(point))
	{
		if (!pItem3)
			return;
		p2 = point;
		Draw = pCtrl->rect;
		ClientToScreen(&p2);
		ClientToScreen(&Draw);
		g_WndMng.PutToolTip_Item(pItem3, p2, &Draw);
		return;
	}
}

BOOL CWndCrystal::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_OK)
	{
		if (pItem1 && pItem2)
		{
			g_DPlay.SendUpdateCrystal(pItem2->m_dwObjId, pItem1->m_dwObjId);

			pItem1->SetExtra(0);
			pItem1 = NULL;

			pItem2->SetExtra(0);
			pItem2 = NULL;

			pItem3 = NULL;
			Destroy();
		}
		else
		{
			g_WndMng.PutString("You must use an ultimate weapon +10 with a crystal to continue that process...", 0, 0xFFFF0000);
			return FALSE;
		}
	}
	else if (nID == WIDC_CANCEL)
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndCrystal::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	LPWNDCTRL pCtrl = GetWndCtrl(WIDC_CUSTOM1);
	if (pCtrl && pCtrl->rect.PtInRect(point) && pItem1 && pItem1Tex)
	{
		pItem1->SetExtra(0);
		pItem1 = NULL;
		pItem1Tex = NULL;

		if (pItem2 && pItem2Tex)
		{
			pItem2->SetExtra(0);
			pItem2 = NULL;
			pItem2Tex = NULL;
			if (pItem3 && pItem3Tex)
			{
				pItem3 = NULL;
				pItem3Tex = NULL;
			}
		}
		return;
	}

	pCtrl = GetWndCtrl(WIDC_CUSTOM2);
	if (pCtrl && pCtrl->rect.PtInRect(point) && pItem2)
	{
		if (pItem2 && pItem2Tex)
		{
			pItem2->SetExtra(0);
			pItem2 = NULL;
			pItem2Tex = NULL;
			if (pItem3 && pItem3Tex)
			{
				pItem3 = NULL;
				pItem3Tex = NULL;
			}
		}
		return;
	}
}
#endif // __CRYSTAL