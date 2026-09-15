#include "stdafx.h"
#include "resData.h"
#include "WndAegonFTool.h"
#include "DPClient.h"
#include "defineText.h"
#include "WndManager.h"
#include "WndManager.h"
#include "Model.h"
#include "ModelMng.h"
#include "ModelObject.h"
#include "ModelGlobal.h"
#include "math.h"
#include <ctime>
#include "defineObj.h"

#ifdef __AEGON_F_TOOL
extern CDPClient g_DPlay;

///////////////////////////
///////LIST WINDOW/////////
///////////////////////////
CWndFToolList::CWndFToolList()
{
	m_nRowHeight = 40;
	m_nSelectedItem = -1;
}
CWndFToolList::~CWndFToolList()
{
}
void CWndFToolList::OnDraw(C2DRender* p2DRender)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	CRect wndRect = GetWindowRect();
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	CPoint point = CPoint(0, 0);
	CString str;

	int nRange = 0;
	int nPage = wndRect.Height() / m_nRowHeight;

	nRange = g_pPlayer->m_vFToolUnits.size();

	if (!nRange)
		nRange = 1;

	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);
	m_wndScrollBar.EnableScrollBar(TRUE);
	m_wndScrollBar.SetScrollStep(1);	


	int nIndex = 0;
	for (int i = m_wndScrollBar.GetScrollPos(); i < g_pPlayer->m_vFToolUnits.size(); i++, nIndex++)
	{
		point.x = 10;
		point.y = nIndex * m_nRowHeight;

		if (i > m_wndScrollBar.GetScrollPos() + nPage)
			break;


		if (point.y + m_nRowHeight >= m_nRowHeight)
			p2DRender->RenderLine(CPoint(0, point.y + m_nRowHeight), CPoint(wndRect.Width() - 30, point.y + m_nRowHeight), dwColorText);

#ifdef __AEGON_THEME_SWITCHER
		DWORD RenderFillRect = CAegonThemeSwitcherMng::GetInstance()->GetRenderFillRectColor();
		DWORD RenderRect = CAegonThemeSwitcherMng::GetInstance()->GetRenderRectColor();
#endif //__AEGON_THEME_SWITCHER

		if (m_nSelectedItem == i)
		{
			CRect rectSelected = CRect(0, point.y, wndRect.right - 30, point.y + m_nRowHeight);
#ifdef __AEGON_THEME_SWITCHER
			p2DRender->RenderFillRect(rectSelected, RenderFillRect);
			p2DRender->RenderRect(rectSelected, RenderRect);
#else //__AEGON_THEME_SWITCHER
			p2DRender->RenderFillRect(rectSelected, 0xFFf8e6cc);
			p2DRender->RenderRect(rectSelected, 0xFFf28818);
#endif //__AEGON_THEME_SWITCHER
		}

		str.Format("%d", g_pPlayer->m_vFToolUnits[i].m_nPage + 1);
		CSize strExtent = p2DRender->GetFont()->GetTextExtent(str.GetString());
		p2DRender->TextOut(point.x, point.y + (m_nRowHeight / 2) - (strExtent.cy / 2), str.GetString(), dwColorText);

		str.Format("%d", g_pPlayer->m_vFToolUnits[i].m_nFKey + 1);
		strExtent = p2DRender->GetFont()->GetTextExtent(str.GetString());
		p2DRender->TextOut(m_rectKey.CenterPoint().x - strExtent.cx / 2, point.y + (m_nRowHeight / 2) - (strExtent.cy / 2), str.GetString(), dwColorText);

		str.Format("%.2f", g_pPlayer->m_vFToolUnits[i].m_fIntervall);
		strExtent = p2DRender->GetFont()->GetTextExtent(str.GetString());
		p2DRender->TextOut(m_rectInterval.CenterPoint().x - strExtent.cx / 2, point.y + (m_nRowHeight / 2) - (strExtent.cy / 2), str.GetString(), dwColorText);


		CTexture* pActionTexture = NULL;
		if (g_WndMng.m_pWndTaskBar) 
		{
			SHORTCUT* pShortcut = &g_WndMng.m_pWndTaskBar->m_aSlotItem[g_pPlayer->m_vFToolUnits[i].m_nPage][g_pPlayer->m_vFToolUnits[i].m_nFKey];
			if (pShortcut && pShortcut->m_dwShortcut > SHORTCUT_NONE)
			{
				pActionTexture = pShortcut->m_pTexture;
			}
		}
		if (pActionTexture) 
		{
			p2DRender->RenderTexture(CPoint(m_rectAction.CenterPoint().x - pActionTexture->m_size.cx / 2, point.y + (m_nRowHeight / 2) - (pActionTexture->m_size.cy / 2)), pActionTexture);
		}


		double fTimeLeft = g_pPlayer->m_vFToolUnits[i].m_fIntervall;
		if (g_pPlayer->m_vFToolUnits[i].m_bActive && g_pPlayer->m_vFToolUnits[i].m_dwNextUse > GetTickCount()) 
		{
			DWORD dwTimeLeftMilSec = g_pPlayer->m_vFToolUnits[i].m_dwNextUse - GetTickCount();
			fTimeLeft = ((double)dwTimeLeftMilSec) / ((double)1000.0);
		}
		str.Format("%.2f", fTimeLeft);
		strExtent = p2DRender->GetFont()->GetTextExtent(str.GetString());
		p2DRender->TextOut(m_rectStatus.right - strExtent.cx, point.y + (m_nRowHeight / 2) - (strExtent.cy / 2), str.GetString(), dwColorText);

	}


}
void CWndFToolList::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	CRect rect = GetWindowRect();

	m_wndScrollBar.Create(WBS_DOCKING | WBS_VERT, rect, this, 1000);
	m_wndScrollBar.SetVisible(IsWndStyle(WBS_VSCROLL));

}
BOOL CWndFToolList::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndBase::OnChildNotify(message, nID, pLResult);
}
BOOL CWndFToolList::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndBase::OnCommand(nID, dwMessage, pWndBase);
}
void CWndFToolList::OnLButtonUp(UINT nFlags, CPoint point)
{

}
void CWndFToolList::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	SetSelectedItem(point);

	if (m_nSelectedItem < 0 || m_nSelectedItem >= g_pPlayer->m_vFToolUnits.size())
		return;

	CWndAegonFTool* pParentWnd = (CWndAegonFTool*)GetParentWnd();
	if (pParentWnd) 
	{
		CWndComboBox* pWndPage = (CWndComboBox*)pParentWnd->GetDlgItem(WIDC_COMBOBOX1);
		CWndComboBox* pWndKey = (CWndComboBox*)pParentWnd->GetDlgItem(WIDC_COMBOBOX2);
		CWndEdit* pWndInterval = (CWndEdit*)pParentWnd->GetDlgItem(WIDC_EDIT1);

		if (pWndPage && pWndKey && pWndInterval) 
		{
			pWndPage->SetCurSel(g_pPlayer->m_vFToolUnits[m_nSelectedItem].m_nPage);
			pWndKey->SetCurSel(g_pPlayer->m_vFToolUnits[m_nSelectedItem].m_nFKey);
			string strInterval = to_string(g_pPlayer->m_vFToolUnits[m_nSelectedItem].m_fIntervall);
			size_t nDotIndex = strInterval.find('.');
			if (nDotIndex != std::string::npos)
				strInterval = strInterval.substr(0, nDotIndex + 3);
			pWndInterval->SetString(strInterval.c_str());
		}

	}
}

void CWndFToolList::OnMouseWndSurface(CPoint point)
{
	if (IsInvalidObj(g_pPlayer))
		return; 

	int nScrollPos = m_wndScrollBar.GetScrollPos();
	CRect wndRect = GetWndRect();
	int nRowCount = ceil((float)wndRect.Height() / (float)m_nRowHeight);

	for (int i = 0; i < nRowCount; i++)
	{
		if (nScrollPos + i >= g_pPlayer->m_vFToolUnits.size())
			continue;

		int nSelectedItem = nScrollPos + i;
		if (nSelectedItem < 0 || nSelectedItem >= g_pPlayer->m_vFToolUnits.size())
			continue;

		LPSHORTCUT pShortcut = &g_WndMng.m_pWndTaskBar->m_aSlotItem[g_pPlayer->m_vFToolUnits[nSelectedItem].m_nPage][g_pPlayer->m_vFToolUnits[nSelectedItem].m_nFKey];

		if (!pShortcut || pShortcut->m_dwShortcut <= SHORTCUT_NONE)
			continue;

		SIZE texSize;
		if (pShortcut->m_pTexture)
		{
			texSize = pShortcut->m_pTexture->m_size;
		}
		else 
		{
			texSize.cx = 0;
			texSize.cy = 0;
		}

		CRect tmpRect;
		tmpRect.left = 0;
		tmpRect.top = 0 + i * m_nRowHeight;
		tmpRect.right = wndRect.Width() - 30;
		tmpRect.bottom = tmpRect.top + m_nRowHeight;

		CRect rectIcon = CRect(m_rectAction.CenterPoint().x - texSize.cx / 2, tmpRect.CenterPoint().y - texSize.cy / 2, m_rectAction.CenterPoint().x + texSize.cx / 2, tmpRect.CenterPoint().y + texSize.cy / 2);


		if (!rectIcon.PtInRect(point))
			continue;

		
		

		CRect hitrect = rectIcon;

		CPoint point2 = point;
		ClientToScreen(&point2);
		ClientToScreen(&hitrect);


		if(g_WndMng.m_pWndTaskBar)
		{
			g_WndMng.m_pWndTaskBar->PutTooTip(pShortcut, point2, &hitrect, TRUE);
		}
		break;
		
	}
}
void CWndFToolList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	ToggleSelectedUnit();
}
void CWndFToolList::OnRButtonUp(UINT nFlags, CPoint point)
{
	SetSelectedItem(point);
	ToggleSelectedUnit();
}
void CWndFToolList::SetSelectedItem(CPoint point) 
{
	if (IsInvalidObj(g_pPlayer))
		return;

	int nScrollPos = m_wndScrollBar.GetScrollPos();
	CRect wndRect = GetWndRect();
	int nRowCount = ceil((float)wndRect.Height() / (float)m_nRowHeight);

	for (int i = 0; i < nRowCount; i++)
	{
		if (nScrollPos + i >= g_pPlayer->m_vFToolUnits.size())
			break;

		CRect tmpRect;
		tmpRect.left = 0;
		tmpRect.top = 0 + i * m_nRowHeight;
		tmpRect.right = wndRect.Width() - 30;
		tmpRect.bottom = tmpRect.top + m_nRowHeight;

		if (tmpRect.PtInRect(point))
		{
			m_nSelectedItem = nScrollPos + i;
			return;
		}
	}
	m_nSelectedItem = -1;
}
void CWndFToolList::ToggleSelectedUnit() 
{
	if (IsInvalidObj(g_pPlayer))
		return;

	if (m_nSelectedItem < 0 || m_nSelectedItem >= g_pPlayer->m_vFToolUnits.size())
		return;

	LPSHORTCUT pShortcut = &g_WndMng.m_pWndTaskBar->m_aSlotItem[g_pPlayer->m_vFToolUnits[m_nSelectedItem].m_nPage][g_pPlayer->m_vFToolUnits[m_nSelectedItem].m_nFKey];
	if (!pShortcut || pShortcut->m_dwShortcut == SHORTCUT_NONE)
	{
		g_pPlayer->m_vFToolUnits[m_nSelectedItem].m_dwNextUse = 0;
		g_pPlayer->m_vFToolUnits[m_nSelectedItem].m_bActive = FALSE;
		return;
	}

	BOOL bCurrentState = g_pPlayer->m_vFToolUnits[m_nSelectedItem].m_bActive;
	g_pPlayer->m_vFToolUnits[m_nSelectedItem].m_bActive = !bCurrentState;
	if (bCurrentState)
		g_pPlayer->m_vFToolUnits[m_nSelectedItem].m_dwNextUse = 0;
	else
		g_pPlayer->m_vFToolUnits[m_nSelectedItem].m_dwNextUse = GetTickCount() + ceil(g_pPlayer->m_vFToolUnits[m_nSelectedItem].m_fIntervall * 1000.0f);

}
////////////////////////////
////////FTOOL MAIN/////////
///////////////////////////
CWndAegonFTool::CWndAegonFTool()
{
	m_pWndFToolList = NULL;
}
CWndAegonFTool::~CWndAegonFTool()
{
}
void CWndAegonFTool::OnDraw(C2DRender* p2DRender)
{
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	CWndStatic* pWndCreateHeader = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
	CWndButton* pWndDelete = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
	if (pWndCreateHeader && pWndDelete)
	{
		CRect rectCreateEdit = pWndCreateHeader->GetWndRect();
		rectCreateEdit.top = rectCreateEdit.bottom;
		rectCreateEdit.bottom = pWndDelete->GetWndRect().bottom + 5;

		p2DRender->RenderRect(rectCreateEdit, dwColorText);
	}

}
void CWndAegonFTool::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	//Move Window to center
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();

	LPWNDCTRL listCtrl = GetWndCtrl(WIDC_CUSTOM1);

	if (listCtrl)
	{
		SAFE_DELETE(m_pWndFToolList);
		m_pWndFToolList = new CWndFToolList;
		m_pWndFToolList->Create(WBS_CHILD | WBS_VSCROLL, listCtrl->rect, this, 100000);
		m_pWndFToolList->m_byWndType = WTYPE_LISTBOX;
		m_pWndFToolList->m_bTile = true;
		m_pWndFToolList->m_bVisible = true;
		m_pWndFToolList->m_strTexture = "WndEditTile00.tga";

		int nLeftOffset = 0;
		CWndStatic* pWndStaticPage = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
		if (pWndStaticPage) 
		{
			CRect rect = pWndStaticPage->GetWndRect();
			rect.top = 0;
			rect.bottom = m_pWndFToolList->m_nRowHeight;
			nLeftOffset = rect.left;
			rect.left -= nLeftOffset;
			rect.right -= nLeftOffset;
			m_pWndFToolList->m_rectPage = rect;

		}
		CWndStatic* pWndStaticKey = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
		if (pWndStaticKey)
		{
			CRect rect = pWndStaticKey->GetWndRect();
			rect.top = 0;
			rect.bottom = m_pWndFToolList->m_nRowHeight;
			rect.left -= nLeftOffset;
			rect.right -= nLeftOffset;
			m_pWndFToolList->m_rectKey = rect;

		}
		CWndStatic* pWndStaticInterval = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
		if (pWndStaticInterval)
		{
			CRect rect = pWndStaticInterval->GetWndRect();
			rect.top = 0;
			rect.bottom = m_pWndFToolList->m_nRowHeight;
			rect.left -= nLeftOffset;
			rect.right -= nLeftOffset;
			m_pWndFToolList->m_rectInterval = rect;

		}
		CWndStatic* pWndStaticStatus = (CWndStatic*)GetDlgItem(WIDC_STATIC4);
		if (pWndStaticStatus)
		{
			CRect rect = pWndStaticStatus->GetWndRect();
			rect.top = 0;
			rect.bottom = m_pWndFToolList->m_nRowHeight;
			rect.left -= nLeftOffset;
			rect.right -= nLeftOffset + 10;
			m_pWndFToolList->m_rectStatus = rect;

		}

		CWndStatic* pWndStaticAction = (CWndStatic*)GetDlgItem(WIDC_STATIC5);
		if (pWndStaticAction)
		{
			CRect rect = pWndStaticAction->GetWndRect();
			rect.top = 0;
			rect.bottom = m_pWndFToolList->m_nRowHeight;
			rect.left -= nLeftOffset;
			rect.right -= nLeftOffset;
			m_pWndFToolList->m_rectAction = rect;

		}

		AdjustWndBase();
	}
	CWndComboBox* pWndPage = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	CWndComboBox* pWndKey = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);
	CWndEdit* pWndInterval = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	CString tmpStr;

	if (pWndPage) 
	{
		pWndPage->AddWndStyle(EBS_READONLY);
		for (int i = 0; i < MAX_SLOT_ITEM_COUNT; i++)
		{
			tmpStr.Format("%d", i + 1);
			pWndPage->AddString(tmpStr.GetString());
			if (pWndPage->m_wndListBox.GetCount() > 0)
				pWndPage->SetCurSel(0);
		}
	}

	if (pWndKey)
	{
		pWndKey->AddWndStyle(EBS_READONLY);
		for (int i = 0; i < MAX_SLOT_ITEM; i++)
		{
			tmpStr.Format("%d", i + 1);
			pWndKey->AddString(tmpStr.GetString());

		}
		if (pWndKey->m_wndListBox.GetCount() > 0)
			pWndKey->SetCurSel(0);
	}

	if (pWndInterval) 
	{
		pWndInterval->SetMaxStringNumber(8);
		pWndInterval->SetString("1.00");
	}
	

}
BOOL CWndAegonFTool::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_AEGON_FTOOL, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndAegonFTool::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndAegonFTool::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndAegonFTool::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndAegonFTool::OnLButtonDown(UINT nFlags, CPoint point)
{

}
BOOL CWndAegonFTool::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_BUTTON1:
	{
		AddUnit();
		break;
	}
	case WIDC_BUTTON2:
	{
		DeleteSelected();
		break;
	}
	case WIDC_BUTTON3:
	{
		EditSelected();
		break;
	}

	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndAegonFTool::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}
BOOL CWndAegonFTool::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	return TRUE;
}
void CWndAegonFTool::AddUnit() 
{
	if (!m_pWndFToolList || IsInvalidObj(g_pPlayer))
		return;

	CWndComboBox* pWndPage = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	CWndComboBox* pWndKey = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);
	CWndEdit* pWndInterval = (CWndEdit*)GetDlgItem(WIDC_EDIT1);

	if (!pWndPage || !pWndKey || !pWndInterval)
		return;

	__AEGON_F_TOOL_UNIT tmpUnit;

	tmpUnit.m_nPage = pWndPage->GetCurSel();
	tmpUnit.m_nFKey = pWndKey->GetCurSel();
	string strInterval = pWndInterval->GetString();
	size_t nDotIndex = strInterval.find('.');
	if (nDotIndex != std::string::npos)
		strInterval = strInterval.substr(0, nDotIndex + 3);
	try 
	{
		tmpUnit.m_fIntervall = stof(strInterval);

	}
	catch (...) 
	{
		g_WndMng.PutString("Invalid interval value", 0, COLOR_ERROR);
		return;
	}

	tmpUnit.TimeCheck();
	if (tmpUnit.IsValid()) 
	{
		g_pPlayer->m_vFToolUnits.push_back(tmpUnit);
		g_DPlay.SendUpdateFTools();
	}
	else 
	{
		g_WndMng.PutString("Error invalid Data!", 0, COLOR_ERROR);
	}
}
void CWndAegonFTool::DeleteSelected()
{
	if (!m_pWndFToolList || IsInvalidObj(g_pPlayer))
		return;

	int nSelectedItem = m_pWndFToolList->GetSelectedItemIndex();
	if (nSelectedItem < 0 || nSelectedItem >= g_pPlayer->m_vFToolUnits.size())
		return;

	g_pPlayer->m_vFToolUnits[nSelectedItem].m_bActive = FALSE;

	g_pPlayer->m_vFToolUnits.erase(g_pPlayer->m_vFToolUnits.begin() + nSelectedItem);
	g_DPlay.SendUpdateFTools();
}
void CWndAegonFTool::EditSelected()
{
	if (!m_pWndFToolList || IsInvalidObj(g_pPlayer))
		return;

	int nSelectedItem = m_pWndFToolList->GetSelectedItemIndex();
	if (nSelectedItem < 0 || nSelectedItem >= g_pPlayer->m_vFToolUnits.size())
		return;

	CWndComboBox* pWndPage = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	CWndComboBox* pWndKey = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);
	CWndEdit* pWndInterval = (CWndEdit*)GetDlgItem(WIDC_EDIT1);

	if (!pWndPage || !pWndKey || !pWndInterval)
		return;

	g_pPlayer->m_vFToolUnits[nSelectedItem].m_bActive = FALSE;

	g_pPlayer->m_vFToolUnits[nSelectedItem].m_nPage = pWndPage->GetCurSel();
	g_pPlayer->m_vFToolUnits[nSelectedItem].m_nFKey = pWndKey->GetCurSel();
	string strInterval = pWndInterval->GetString();
	size_t nDotIndex = strInterval.find('.');
	if (nDotIndex != std::string::npos)
		strInterval = strInterval.substr(0, nDotIndex + 3);
	try
	{
		g_pPlayer->m_vFToolUnits[nSelectedItem].m_fIntervall = stof(strInterval);
	}
	catch (...)
	{
		g_WndMng.PutString("Invalid interval value", 0, COLOR_ERROR);
		return;
	}
	

	g_pPlayer->m_vFToolUnits[nSelectedItem].TimeCheck();

	if (!g_pPlayer->m_vFToolUnits[nSelectedItem].IsValid()) 
	{
		g_WndMng.PutString("Error invalid Edit!", 0, COLOR_ERROR);
		DeleteSelected();
	}
	g_DPlay.SendUpdateFTools();
	
}
#endif