#include "stdafx.h"
#include "resData.h"
#include "WndAegonIngameShop.h"
#include "DPClient.h"
#include "defineText.h"
#include "WndManager.h"
#include "WndManager.h"
#include "Model.h"
#include "ModelMng.h"
#include "ModelObject.h"
#include "ModelGlobal.h"
#include "math.h"
#include <sstream>

#ifdef __MODEL_VIEW
#include "WndModelView.h"
#endif //__MODEL_VIEW

extern CDPClient g_DPlay;

#ifdef __AEGON_INGAME_SHOP


DWORD WINAPI __SortIngameShopThread(LPVOID lpParam)
{
	CWndIngameShopList* pIngameShopList = (CWndIngameShopList*)lpParam;
	pIngameShopList->SortVectorThread();
	return 0;
}

bool SortByNameUpper(__AEGIGNSHP_ITEM_UNIT szUnitLeft, __AEGIGNSHP_ITEM_UNIT szUnitRight)
{
	std::string strLeft = szUnitLeft.m_szName;
	std::string strRight = szUnitRight.m_szName;
	return (strLeft < strRight);
}
bool SortByNameLower(__AEGIGNSHP_ITEM_UNIT szUnitLeft, __AEGIGNSHP_ITEM_UNIT szUnitRight)
{
	std::string strLeft = szUnitLeft.m_szName;
	std::string strRight = szUnitRight.m_szName;
	return (strLeft > strRight);
}
bool SortByPriceUpper(__AEGIGNSHP_ITEM_UNIT szUnitLeft, __AEGIGNSHP_ITEM_UNIT szUnitRight)
{
	return (szUnitLeft.m_nPrice < szUnitRight.m_nPrice);
}
bool SortByPriceLower(__AEGIGNSHP_ITEM_UNIT szUnitLeft, __AEGIGNSHP_ITEM_UNIT szUnitRight)
{
	return (szUnitLeft.m_nPrice > szUnitRight.m_nPrice);
}
///////////////////////////
///////LIST WINDOW/////////
///////////////////////////
CWndIngameShopList::CWndIngameShopList()
{
	m_nRowHeight = 40;
	m_nSelectedItem = -1;
	m_dwSortMethod = AEGINGSHP_SORT_NAME_A;
}
CWndIngameShopList::~CWndIngameShopList()
{
}
void CWndIngameShopList::OnDraw(C2DRender* p2DRender)
{
	CRect wndRect = GetWindowRect();
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	CPoint point = CPoint(0, 0);
	CString str;

	int nRange = 0;
	int nPage = wndRect.Height() / m_nRowHeight;

	nRange = m_vItemList.size();

	if (!nRange)
		nRange = 1;

	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);
	m_wndScrollBar.EnableScrollBar(TRUE);
	m_wndScrollBar.SetScrollStep(1);

	
	int nIndex = 0;
	for (int i = m_wndScrollBar.GetScrollPos(); i < m_vItemList.size(); i++, nIndex++)
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

		ItemProp* pItemProp = prj.GetItemProp(m_vItemList[i].m_vCommandUnits[0].m_dwItemID);
		if (pItemProp)
		{
			CTexture* pItemTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), COLOR_ITEM_BG);
			if (pItemTexture)
			{
				p2DRender->RenderTexture(CPoint(point.x, point.y + 4), pItemTexture);
				point.x += pItemTexture->m_size.cx + 10;
			}

		}

		str.Format("%s", m_vItemList[i].m_szName);
		CSize strExtent = p2DRender->GetFont()->GetTextExtent(str.GetString());
		p2DRender->TextOut(point.x, point.y + (m_nRowHeight / 2) - (strExtent.cy / 2), str.GetString(), dwColorText);

		str.Format("%d DP", m_vItemList[i].m_nPrice);
		strExtent = p2DRender->GetFont()->GetTextExtent(str.GetString());
		p2DRender->TextOut(wndRect.Width() - 50 - strExtent.cx, point.y + (m_nRowHeight / 2) - (strExtent.cy / 2), str.GetString(), dwColorText);

		
	}


}
void CWndIngameShopList::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	CRect rect = GetWindowRect();
	m_wndScrollBar.AddWndStyle(WBS_DOCKING);
	m_wndScrollBar.Create(WBS_VERT, rect, this, 1000);


	UpdateItemList();
}
BOOL CWndIngameShopList::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndBase::OnChildNotify(message, nID, pLResult);
}
BOOL CWndIngameShopList::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndBase::OnCommand(nID, dwMessage, pWndBase);
}
void CWndIngameShopList::OnSize(UINT nType, int cx, int cy)
{
	CWndBase::OnSize(nType, cx, cy);
}
void CWndIngameShopList::OnLButtonUp(UINT nFlags, CPoint point)
{

}
void CWndIngameShopList::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetSelectedItem(point);
}

void CWndIngameShopList::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetSelectedItem(point);
#ifdef __MODEL_VIEW
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		PreviewSelectedItem();
	else
		BuySelectedItem();
#else //__MODEL_VIEW
	BuySelectedItem();
#endif //__MODEL_VIEW
}



void CWndIngameShopList::OnMouseWndSurface(CPoint point)
{
	int nScrollPos = m_wndScrollBar.GetScrollPos();
	CRect wndRect = GetWndRect();
	int nRowCount = ceil((float)wndRect.Height() / (float)m_nRowHeight);

	for (int i = 0; i < nRowCount; i++)
	{
		if (nScrollPos + i >= m_vItemList.size())
			break;

		CRect tmpRect;
		tmpRect.left = 0;
		tmpRect.top = 0 + i * m_nRowHeight;
		tmpRect.right = wndRect.Width() - 30;
		tmpRect.bottom = tmpRect.top + m_nRowHeight;

		if (tmpRect.PtInRect(point))
		{
			int nSelectedItem = nScrollPos + i;
			if (nSelectedItem >= 0 && nSelectedItem < m_vItemList.size()) 
			{
				CRect hitrect = tmpRect;

				CPoint point2 = point;
				ClientToScreen(&point2);
				ClientToScreen(&hitrect);

				g_WndMng.PutToolTip_ShopPackage(m_vItemList[nSelectedItem].m_dwItemUnitID, point2, &hitrect);				
			}
			return;
		}
	}
}
void CWndIngameShopList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	SetSelectedItem(point);
#ifdef __MODEL_VIEW
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		PreviewSelectedItem();
	else
		BuySelectedItem();
#else //__MODEL_VIEW
	BuySelectedItem();
#endif //__MODEL_VIEW
}
void CWndIngameShopList::SetSelectedItem(CPoint point) 
{
	int nScrollPos = m_wndScrollBar.GetScrollPos();
	CRect wndRect = GetWndRect();
	int nRowCount = ceil((float)wndRect.Height() / (float)m_nRowHeight);

	for (int i = 0; i < nRowCount; i++)
	{
		if (nScrollPos + i >= m_vItemList.size())
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

void CWndIngameShopList::UpdateItemList()
{
	CWndIngameShopMain* pParentWnd = (CWndIngameShopMain*)GetParentWnd();
	if (!pParentWnd)
		return;

	m_vItemList.clear();

	if (pParentWnd->m_pWndCategoryList->GetCurSel() < 0) 
	{
		m_nSelectedItem = -1;
		SetScrollPos(0);
		return;
	}

	DWORD nCategoryID = (DWORD)pParentWnd->m_pWndCategoryList->GetItemData(pParentWnd->m_pWndCategoryList->GetCurSel());

	map<DWORD, __AEGIGNSHP_ITEM_UNIT>* itemMap = &CAegonIngameShopMng::GetInstance()->m_mapShopItems;
	if (itemMap)
	{
		map<DWORD, __AEGIGNSHP_ITEM_UNIT>::iterator it = itemMap->begin();
		for (; it != itemMap->end(); it++)
		{
			if (it->second.m_bIsEnabled)
			{
				if (nCategoryID == it->second.m_dwCategoryID)
				{
					if(FilterCheck(&it->second))
						m_vItemList.push_back(it->second);
				}
			}
		}
	}
	SortVector();
	m_nSelectedItem = -1;
	SetScrollPos(0);
}
BOOL CWndIngameShopList::FilterCheck(__AEGIGNSHP_ITEM_UNIT* pShopUnit) 
{
	BOOL bSearchPhrase = FALSE;
	BOOL bSex = FALSE;
	BOOL bLevelRange = FALSE;
	BOOL bPriceRange = FALSE;
	CString tmpStr = "";

	int nLevelFilterLow = 0;
	int nLevelFilterHigh = MAX_CHARACTER_LEVEL;

	int nPriceFilterLow = 1;
	int nPriceFilterHigh = 2100000000;

	CWndIngameShopMain* pParentWnd = (CWndIngameShopMain*)GetParentWnd();

	if (!pParentWnd)
		return FALSE;

	if (!pShopUnit)
		return FALSE;

	//Search Filter
	if (pParentWnd->m_pWndSearchItem) 
	{
		tmpStr = pParentWnd->m_pWndSearchItem->GetString();
		tmpStr.MakeLower();
		CString searchString;
		if (tmpStr.GetLength() > 0)
		{
			searchString.Format("%s", pShopUnit->m_szName);
			searchString.MakeLower();
			if (searchString.Find(tmpStr.GetString()) != std::string::npos)
			{
				bSearchPhrase = TRUE;
			}
			else 
			{
				for (int i = 0; i < pShopUnit->m_vCommandUnits.size(); i++) 
				{
					searchString.Format("%s", pShopUnit->m_vCommandUnits[i].m_szItemName);
					searchString.MakeLower();
					if (searchString.Find(tmpStr.GetString()) != std::string::npos)
					{
						bSearchPhrase = TRUE;
						break;
					}
				}
			}
		}
		else
			bSearchPhrase = TRUE;
	}
	//~Search Filter

	//Sex Filter
	if (pParentWnd->m_pWndSexFilter)
	{
		tmpStr = pParentWnd->m_pWndSexFilter->GetString();
		if (tmpStr.CompareNoCase("All") == 0) 
		{
			bSex = TRUE;
		}
		else
		{
			int nSelectedSex = tmpStr.CompareNoCase("Male") == 0 ? SEX_MALE : SEX_FEMALE;
			
			for (int i = 0; i < pShopUnit->m_vCommandUnits.size(); i++)
			{
				ItemProp* pProp = prj.GetItemProp(pShopUnit->m_vCommandUnits[i].m_dwItemID);
				if (pProp) 
				{
					if (/*pProp->dwItemSex == NULL_ID ||*/ pProp->dwItemSex == nSelectedSex) 
					{
						bSex = TRUE;
						break;
					}
				}
			}
		}
	}
	//~Sex Filter

	//Level Filter
	if (pParentWnd->m_pWndFilterLevelLow && pParentWnd->m_pWndFilterLevelHigh)
	{
		std::stringstream strToIntLevelLow(pParentWnd->m_pWndFilterLevelLow->GetString());
		strToIntLevelLow >> nLevelFilterLow;

		if (!strToIntLevelLow)
		{
			nLevelFilterLow = 0;
		}

		std::stringstream strToIntLevelHigh(pParentWnd->m_pWndFilterLevelHigh->GetString());
		strToIntLevelHigh >> nLevelFilterHigh;

		if (!strToIntLevelHigh)
		{
			nLevelFilterHigh = MAX_CHARACTER_LEVEL;
		}

		for (int i = 0; i < pShopUnit->m_vCommandUnits.size(); i++)
		{
			ItemProp* pProp = prj.GetItemProp(pShopUnit->m_vCommandUnits[i].m_dwItemID);
			if (pProp) 
			{
				if (pProp->dwLimitLevel1 == NULL_ID || (pProp->dwLimitLevel1 >= nLevelFilterLow && pProp->dwLimitLevel1 <= nLevelFilterHigh))
				{
					bLevelRange = TRUE;
					break;
				}
			}
		}
	}
	//~Level Filter

	//Price Filter
	if (pParentWnd->m_pWndFilterPriceLow && pParentWnd->m_pWndFilterPriceHigh)
	{
		std::stringstream strToIntPriceLow(pParentWnd->m_pWndFilterPriceLow->GetString());
		strToIntPriceLow >> nPriceFilterLow;

		if (!strToIntPriceLow)
		{
			nPriceFilterLow = 1;
		}

		std::stringstream strToIntPriceHigh(pParentWnd->m_pWndFilterPriceHigh->GetString());
		strToIntPriceHigh >> nPriceFilterHigh;

		if (!strToIntPriceHigh)
		{
			nPriceFilterHigh = 2100000000;
		}

		if (pShopUnit->m_nPrice >= nPriceFilterLow && pShopUnit->m_nPrice <= nPriceFilterHigh)
			bPriceRange = TRUE;
	}
	//~Price Filter


	if(!bSearchPhrase || !bSex || !bLevelRange || !bPriceRange)
		return FALSE;

	return TRUE;
}

void CWndIngameShopList::SortVector()
{
	TerminateThread(m_hSortThread, 0);
	m_hSortThread = CreateThread(0, 0, __SortIngameShopThread, this, 0, 0);
}
void CWndIngameShopList::SortVectorThread()
{
	EnableWindow(FALSE);
	switch (m_dwSortMethod)
	{
	case AEGINGSHP_SORT_NAME_A: 
		std::sort(m_vItemList.begin(), m_vItemList.end(), SortByNameUpper);
		break;
	case AEGINGSHP_SORT_NAME_D: 
		std::sort(m_vItemList.begin(), m_vItemList.end(), SortByNameLower);
		break;
	case AEGINGSHP_SORT_PRICE_A: 
		std::sort(m_vItemList.begin(), m_vItemList.end(), SortByPriceUpper);
		break;
	case AEGINGSHP_SORT_PRICE_D:
		std::sort(m_vItemList.begin(), m_vItemList.end(), SortByPriceLower);
		break;
	}
	EnableWindow(TRUE);
}

#ifdef __MODEL_VIEW
void CWndIngameShopList::PreviewSelectedItem()
{
	if (m_nSelectedItem >= 0 && m_nSelectedItem < m_vItemList.size())
	{
		for (int i = 0; i < m_vItemList[m_nSelectedItem].m_vCommandUnits.size(); i++)
		{
			CItemElem itemElem;
			itemElem.m_dwItemId = m_vItemList[m_nSelectedItem].m_vCommandUnits[i].m_dwItemID;
			PPACKITEMELEM pPackItemElem = CPackItem::GetInstance()->Open(itemElem.m_dwItemId);

			if (pPackItemElem)
			{
				for (int i = 0; i < pPackItemElem->nSize; i++)
				{
					CItemElem* itemElem2 = new CItemElem;
					itemElem2->m_dwItemId = pPackItemElem->adwItem[i];
					CWndModelView* pWndViewer = (CWndModelView*)g_WndMng.GetApplet(APP_MODEL_VIEW);
					pWndViewer = (CWndModelView*)g_WndMng.CreateApplet(APP_MODEL_VIEW);
					if (pWndViewer)
					{
						pWndViewer->SetPart(&itemElem);
					}
				}
			}
			else
			{
				CWndModelView* pWndViewer = (CWndModelView*)g_WndMng.GetApplet(APP_MODEL_VIEW);
				if (!pWndViewer)
				{
					pWndViewer = (CWndModelView*)g_WndMng.CreateApplet(APP_MODEL_VIEW);
					if (pWndViewer)
					{
						pWndViewer->AddWndStyle(WBS_TOPMOST);
						if (!pWndViewer->SetPart(&itemElem))
							pWndViewer->Destroy();
					}
				}
				else
					pWndViewer->SetPart(&itemElem);
			}
		}
	}
}
#endif //__MODEL_VIEW

void CWndIngameShopList::BuySelectedItem() 
{
	if (m_nSelectedItem >= 0 && m_nSelectedItem < m_vItemList.size())
	{
		CWndIngameDPPurchaseConfirm* pWndConfirmPurchase = g_WndMng.OpenDPConfirmPurchase();
		if (pWndConfirmPurchase)
		{
			pWndConfirmPurchase->m_nCommand = INGAMEDP_WEBSHOP;
			pWndConfirmPurchase->dwID = m_vItemList[m_nSelectedItem].m_dwItemUnitID;

			for(int i = 0; i < m_vItemList[m_nSelectedItem].m_vCommandUnits.size(); i++)
				pWndConfirmPurchase->m_vItemQuantityTuple.push_back(make_tuple(m_vItemList[m_nSelectedItem].m_vCommandUnits[i].m_dwItemID, m_vItemList[m_nSelectedItem].m_vCommandUnits[i].m_dwItemNum));

			pWndConfirmPurchase->SetInfo(m_vItemList[m_nSelectedItem].m_szName, m_vItemList[m_nSelectedItem].m_nPrice, TRUE);

			pWndConfirmPurchase->CreateItemPreview();
		}
	}
}
///////////////////////////
///////MAIN WINDOW/////////
///////////////////////////
CWndIngameShopMain::CWndIngameShopMain()
{
	m_wndIngameShopList = NULL;
	m_pWndCategoryList = NULL;
	m_pWndDpAmount = NULL;
	m_pWndSearchItem = NULL;
	m_pWndSexFilter = NULL;
	m_pWndFilterLevelLow = NULL;
	m_pWndFilterLevelHigh = NULL;
	m_pWndFilterPriceLow = NULL;
	m_pWndFilterPriceHigh = NULL;
}
CWndIngameShopMain::~CWndIngameShopMain()
{
}
void CWndIngameShopMain::OnDraw(C2DRender* p2DRender)
{
	CString cash;
	CString total;
	cash.Format("%d", g_pPlayer->GetIngameDP());
	total.Format("%s DP", GetNumberFormatEx(cash.GetString()).GetString());
	m_pWndDpAmount->SetTitle(total);
}
void CWndIngameShopMain::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	//Move Window to center
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();

	m_pWndCategoryList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	m_pWndDpAmount = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	m_pWndSearchItem = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	m_pWndFilterLevelLow = (CWndEdit*)GetDlgItem(WIDC_EDIT2);
	m_pWndFilterLevelHigh = (CWndEdit*)GetDlgItem(WIDC_EDIT3);
	m_pWndFilterPriceLow = (CWndEdit*)GetDlgItem(WIDC_EDIT4);
	m_pWndFilterPriceHigh = (CWndEdit*)GetDlgItem(WIDC_EDIT5);
	m_pWndSexFilter = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);

	m_pWndFilterLevelLow->AddWndStyle(EBS_NUMBER);
	m_pWndFilterLevelLow->SetMaxStringNumber(3);

	m_pWndFilterLevelHigh->AddWndStyle(EBS_NUMBER);
	m_pWndFilterLevelHigh->SetMaxStringNumber(3);

	m_pWndFilterPriceLow->AddWndStyle(EBS_NUMBER);
	m_pWndFilterPriceLow->SetMaxStringNumber(10);

	m_pWndFilterPriceHigh->AddWndStyle(EBS_NUMBER);
	m_pWndFilterPriceHigh->SetMaxStringNumber(10);

	m_pWndSexFilter->SetString("All");
	m_pWndSexFilter->AddString("All");
	m_pWndSexFilter->AddString("Male");
	m_pWndSexFilter->AddString("Female");
	m_pWndSexFilter->AddWndStyle(EBS_READONLY);

#ifdef __PLACEHOLDER_TEXT
	m_pWndSearchItem->SetPlaceholderString("Search...");
#endif //__PLACEHOLDER_TEXT

	ResetFilters();

	if (m_pWndDpAmount) 
	{
		CString total;
		CString cash;
		cash.Format("%d", g_pPlayer->GetIngameDP());
		total.Format("%s DP", GetNumberFormatEx(cash.GetString()).GetString());
		m_pWndDpAmount->SetTitle(total);
		m_pWndDpAmount->m_dwColor = dwColorText;
		m_pWndDpAmount->AddWndStyle(WSS_ALIGNHRIGHT);

	}

	map<DWORD, __AEGIGNSHP_CATEGORY>* categoriesMap = &CAegonIngameShopMng::GetInstance()->m_mapCategories;
	if (categoriesMap) 
	{
		map<DWORD, __AEGIGNSHP_CATEGORY>::iterator it = categoriesMap->begin();
		for (; it != categoriesMap->end(); it++) 
		{
			if (it->second.m_bIsEnabled) 
			{
				m_pWndCategoryList->AddString(it->second.m_szName);
				m_pWndCategoryList->SetItemData(m_pWndCategoryList->GetCount() - 1, it->second.m_dwCategoryID);
			}
		}
	}
	if (m_pWndCategoryList->GetCount() > 0)
		m_pWndCategoryList->SetCurSel(0);

	LPWNDCTRL listCtrl = GetWndCtrl(WIDC_CUSTOM1);

	if (listCtrl)
	{
		SAFE_DELETE(m_wndIngameShopList);
		m_wndIngameShopList = new CWndIngameShopList;
		m_wndIngameShopList->Create(WBS_CHILD | WBS_VSCROLL, listCtrl->rect, this, 100000);
		m_wndIngameShopList->m_byWndType = WTYPE_LISTBOX;
		m_wndIngameShopList->m_bTile = true;
		m_wndIngameShopList->m_bVisible = true;
		m_wndIngameShopList->m_strTexture = "WndEditTile00.tga";

		AdjustWndBase();
	}

	m_wndIngameShopList->UpdateItemList();
}
BOOL CWndIngameShopMain::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_AEGON_INGAME_SHOP, 0, CPoint(0, 0), pWndParent);
}
void CWndIngameShopMain::OnDestroy()
{
}
BOOL CWndIngameShopMain::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndIngameShopMain::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndIngameShopMain::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndIngameShopMain::OnLButtonDown(UINT nFlags, CPoint point)
{

}
BOOL CWndIngameShopMain::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_LISTBOX1:
	{
		if (message == WNM_DBLCLK)
			return FALSE;
	
		m_wndIngameShopList->UpdateItemList();
		
		break;
	}
	case WIDC_BUTTON1: //Purchase
	{
		if (m_wndIngameShopList) 
		{
			m_wndIngameShopList->BuySelectedItem();
		}
		break;
	}
	case WIDC_BUTTON2: //Reset filter
	{
		ResetFilters();
		break;
	}
	case WIDC_BUTTON3: //Sort Name
	{
		if (m_wndIngameShopList)
		{
			if (m_wndIngameShopList->m_dwSortMethod == AEGINGSHP_SORT_NAME_A)
			{
				m_wndIngameShopList->m_dwSortMethod = AEGINGSHP_SORT_NAME_D;
			}
			else
				m_wndIngameShopList->m_dwSortMethod = AEGINGSHP_SORT_NAME_A;

			m_wndIngameShopList->SortVector();
		}
		break;
	}
	case WIDC_BUTTON4: //Sort Price
	{
		if (m_wndIngameShopList)
		{
			if (m_wndIngameShopList->m_dwSortMethod == AEGINGSHP_SORT_PRICE_A)
			{
				m_wndIngameShopList->m_dwSortMethod = AEGINGSHP_SORT_PRICE_D;
			}
			else
				m_wndIngameShopList->m_dwSortMethod = AEGINGSHP_SORT_PRICE_A;

			m_wndIngameShopList->SortVector();
		}
		break;
	}
	case WIDC_COMBOBOX1:
	{
		if (m_wndIngameShopList)
		{
			m_wndIngameShopList->UpdateItemList();
		}
		break;
	}
	case WIDC_EDIT1:
	case WIDC_EDIT2:
	case WIDC_EDIT3:
	case WIDC_EDIT4:
	case WIDC_EDIT5:
	{
		if (m_wndIngameShopList)
		{
			if (message == EN_CHANGE)
			{
				m_wndIngameShopList->UpdateItemList();
			}
		}
		break;
	}
	case WIDC_BUTTON5:
	{
		ShellExecute(0, TEXT("open"), TEXT("https://titanium-mmo.com/shop/offers"), 0, 0, 0);
		break;
	}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndIngameShopMain::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}
BOOL CWndIngameShopMain::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	return TRUE;
}
BOOL CWndIngameShopMain::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return FALSE;
}
void CWndIngameShopMain::ResetFilters()
{
	CString tmpStr;
	tmpStr.Format("%d", MAX_CHARACTER_LEVEL);
	m_pWndSearchItem->SetString("");
	m_pWndFilterLevelLow->SetString("1");
	m_pWndFilterLevelHigh->SetString(tmpStr.GetString());
	m_pWndFilterPriceLow->SetString("1");
	m_pWndFilterPriceHigh->SetString("2100000000");
	m_pWndSexFilter->SetCurSel(0);

	if (!m_wndIngameShopList)
		return;
	m_wndIngameShopList->UpdateItemList();
}

#endif