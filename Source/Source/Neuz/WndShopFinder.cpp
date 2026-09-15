#include "stdafx.h"

#ifdef __LEEAEG_SHOP_FINDER
#include "WndShopFinder.h"
#include "resdata.h"
#include "DPClient.h"
#include <sstream>
#include "AegonShopFinder.h"

extern CDPClient g_DPlay;

DWORD WINAPI __SortShopFinderThread(LPVOID lpParam)
{
	CWndShopFinder* pShopFinder = (CWndShopFinder*)lpParam;
	pShopFinder->Sort();
	return 0;
}

CWndShopFinder::CWndShopFinder()
{
	m_pSearch = 0;
	m_pFilterLvlLow = 0;
	m_pFilterLvlHigh = 0;
	m_pFilterPriceLow = 0;
	m_pFilterPriceHigh = 0;
	m_pFilterSex = 0;
	m_pFilterCurrency = 0;
	m_pListItems = 0;
	m_vecItems.clear();
	m_bUpperCase = FALSE;
}

CWndShopFinder::~CWndShopFinder()
{
	m_vecItems.clear();
}

BOOL CWndShopFinder::Initialize(CWndBase* pWndParent, DWORD dwStyle)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SHOP_FINDER, 0, 0, pWndParent);
}

void CWndShopFinder::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	// Search Bar
	m_pSearch = (CWndEdit*)GetDlgItem(WIDC_EDIT);

#ifdef __PLACEHOLDER_TEXT
	m_pSearch->SetPlaceholderString("Search...");
#endif //__PLACEHOLDER_TEXT

	//Level Filters
	m_pFilterLvlLow = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	m_pFilterLvlHigh = (CWndEdit*)GetDlgItem(WIDC_EDIT2);

	m_pFilterLvlLow->AddWndStyle(EBS_NUMBER);
	m_pFilterLvlLow->SetString("1");

	m_pFilterLvlHigh->AddWndStyle(EBS_NUMBER);
	m_pFilterLvlHigh->SetString("175");

	// Price Filters
	m_pFilterPriceLow = (CWndEdit*)GetDlgItem(WIDC_EDIT3);
	m_pFilterPriceHigh = (CWndEdit*)GetDlgItem(WIDC_EDIT4);

	m_pFilterPriceLow->AddWndStyle(EBS_NUMBER);
	m_pFilterPriceLow->SetString("1");

	m_pFilterPriceHigh->AddWndStyle(EBS_NUMBER);
	m_pFilterPriceHigh->SetString("2100000000"); //2.1B

	// Sex Filter
	m_pFilterSex = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	m_pFilterSex->SetString("All");
	m_pFilterSex->AddString("All");
	m_pFilterSex->AddString("Male");
	m_pFilterSex->AddString("Female");
	m_pFilterSex->AddWndStyle(EBS_READONLY);

	// Filter Currency
	m_pFilterCurrency = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX);
	m_pFilterCurrency->SetString("All");
	m_pFilterCurrency->AddString("All");
	m_pFilterCurrency->AddString("Penya");
	m_pFilterCurrency->AddString("Perin");
	m_pFilterCurrency->AddString("Red Chips");
	m_pFilterCurrency->AddString("Red Perin");
	m_pFilterCurrency->AddWndStyle(EBS_READONLY);

	m_pListItems = (CWndListBox*)GetDlgItem(WIDC_LISTBOX);

	FillItemVec();

	g_DPlay.SendShopItemRefresh();
	MoveParentCenter();
}
BOOL CWndShopFinder::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_BUTTON) //refresh list
	{
		g_DPlay.SendShopItemRefresh();
	}
	else if (nID == WIDC_BUTTON1) //search
	{
		RefreshItemMap();
	}
	else if (nID == WIDC_BUTTON2) //sort item name
	{
		TerminateThread(m_hSortThread, 0);
		m_cSortType = 0;
		m_bUpperCase = !m_bUpperCase;
		m_hSortThread = CreateThread(0, 0, __SortShopFinderThread, this, 0, 0);
	}
	else if (nID == WIDC_BUTTON3) //sort seller name
	{
		TerminateThread(m_hSortThread, 0);
		m_cSortType = 1;
		m_bUpperCase = !m_bUpperCase;
		m_hSortThread = CreateThread(0, 0, __SortShopFinderThread, this, 0, 0);
		
	}
	else if (nID == WIDC_BUTTON4) //sort price
	{
		TerminateThread(m_hSortThread, 0);
		m_cSortType = 2;
		m_bUpperCase = !m_bUpperCase;
		m_hSortThread = CreateThread(0, 0, __SortShopFinderThread, this, 0, 0);
	}
	else if (nID == WIDC_BUTTON5) //sort location
	{
		TerminateThread(m_hSortThread, 0);
		m_cSortType = 3;
		m_bUpperCase = !m_bUpperCase;
		m_hSortThread = CreateThread(0, 0, __SortShopFinderThread, this, 0, 0);
	}
	else if (nID == WIDC_BUTTON6) //teleport
	{
		if (m_pListItems->GetCurSel() != -1)
			g_DPlay.SendShopItemTele(m_vecItems[m_pListItems->GetCurSel()]->itemElem.m_shopItemIndex);
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndShopFinder::OnThemeSelectionChanged() 
{
	CWndNeuz::OnThemeSelectionChanged();
}
void CWndShopFinder::FillItemVec()
{
	m_vecItems.clear();

	map<DWORD, __SHOP_ITEM>::iterator it = CAegonShopFinderMng::GetInstance()->m_mapShopItems.begin();
	for (; it != CAegonShopFinderMng::GetInstance()->m_mapShopItems.end(); it++)
	{
		CString strSearch;
		CString itemName;

		CString strSexFilter;

		int nLevelFilterLow = 0;
		int nLevelFilterHigh = 175;

		int nPriceFilterLow = 1;
		int nPriceFilterHigh = 2100000000;

		CString strFilterCurrency;


		//name filter
		strSearch.Format("%s", m_pSearch->GetString());
		strSearch.MakeLower();

		itemName.Format("%s", it->second.itemElem.GetName().GetString());
		itemName.MakeLower();

		if (strSearch.GetLength() > 0 && itemName.Find(strSearch.GetString()) == -1)
		{
			continue;
		}

		//~name filter

		//sex filter

		strSexFilter.Format("%s", m_pFilterSex->GetString());

		if (it->second.itemElem.GetPropA()->dwItemSex == SEX_SEXLESS || strSexFilter.CompareNoCase("All") == 0)
		{

		}
		else if (it->second.itemElem.GetPropA()->dwItemSex == SEX_MALE && strSexFilter.CompareNoCase("Female") == 0)
		{
			continue;
		}
		else if (it->second.itemElem.GetPropA()->dwItemSex == SEX_FEMALE && strSexFilter.CompareNoCase("Male") == 0)
		{
			continue;
		}
		//~sex filter

		//level filter
		std::stringstream strToIntLevelLow(m_pFilterLvlLow->GetString());
		strToIntLevelLow >> nLevelFilterLow;

		if (!strToIntLevelLow)
		{
			nLevelFilterLow = 0;
		}

		std::stringstream strToIntLevelHigh(m_pFilterLvlHigh->GetString());
		strToIntLevelHigh >> nLevelFilterHigh;

		if (!strToIntLevelHigh)
		{
			nLevelFilterHigh = 175;
		}

		if (it->second.itemElem.GetLimitLevel() == 0xFFFFFFFF)
		{

		}
		else if (it->second.itemElem.GetLimitLevel() < nLevelFilterLow || it->second.itemElem.GetLimitLevel() > nLevelFilterHigh)
		{
			continue;
		}
		//~level filter

		//price filter
		std::stringstream strToIntPriceLow(m_pFilterPriceLow->GetString());
		strToIntPriceLow >> nPriceFilterLow;

		if (!strToIntPriceLow)
		{
			nPriceFilterLow = 1;
		}

		std::stringstream strToIntPriceHigh(m_pFilterPriceHigh->GetString());
		strToIntPriceHigh >> nPriceFilterHigh;

		if (!strToIntPriceHigh)
		{
			nPriceFilterHigh = 2100000000;
		}

		if (it->second.itemElem.m_nCost < nPriceFilterLow || it->second.itemElem.m_nCost > nPriceFilterHigh)
		{
			continue;
		}

		//~price filter

		//currency filter
		strFilterCurrency.Format("%s", m_pFilterCurrency->GetString());
		if (strFilterCurrency.CompareNoCase("All") == 0)
		{

		}
		else if (strFilterCurrency.CompareNoCase("Penya") == 0)
		{
			if (it->second.itemElem.m_nBuyMode != 1)
				continue;
		}
		else if (strFilterCurrency.CompareNoCase("Perin") == 0)
		{
			if (it->second.itemElem.m_nBuyMode != 2)
				continue;
		}
		else if (strFilterCurrency.CompareNoCase("Red Chips") == 0)
		{
			if (it->second.itemElem.m_nBuyMode != 3 && it->second.itemElem.m_nBuyMode != 5)
				continue;
		}
		else if (strFilterCurrency.CompareNoCase("Red Perin") == 0)
		{
			if (it->second.itemElem.m_nBuyMode != 4)
				continue;
		}
		//~currency filter

		m_vecItems.push_back(&it->second);
	}
	m_pListItems->SetScrollPos(0);
}
void CWndShopFinder::RefreshItemMap()
{
	FillItemVec();

	TerminateThread(m_hSortThread, 0);
	m_cSortType = 0;
	m_bUpperCase = TRUE;
	m_hSortThread = CreateThread(0, 0, __SortShopFinderThread, this, 0, 0);
}

void CWndShopFinder::Sort()
{
	m_pListItems->EnableWindow(FALSE);
	switch (m_cSortType)
	{
	case 0: SortByItemName();
		break;
	case 1: SortBySeller();
		break;
	case 2: SortByPrice();
		break;
	case 3: SortByLocation();
		break;
	}
	m_pListItems->EnableWindow(TRUE);
}

bool SortByItemNameUpper(const __SHOP_ITEM* pShopItemLeft, const __SHOP_ITEM* pShopItemRight)
{
	ItemProp* pPropLeft = prj.GetItemProp(pShopItemLeft->itemElem.m_dwItemId);
	ItemProp* pPropRight = prj.GetItemProp(pShopItemRight->itemElem.m_dwItemId);

	std::string strLeft = pPropLeft->szName;
	std::string strRight = pPropRight->szName;
	return (strLeft < strRight);
}
bool SortByItemNameLower(const __SHOP_ITEM* pShopItemLeft, const __SHOP_ITEM* pShopItemRight)
{
	ItemProp* pPropLeft = prj.GetItemProp(pShopItemLeft->itemElem.m_dwItemId);
	ItemProp* pPropRight = prj.GetItemProp(pShopItemRight->itemElem.m_dwItemId);
	std::string strLeft = pPropLeft->szName;
	std::string strRight = pPropRight->szName;
	return (strLeft > strRight);
}
bool SortBySellerUpper(const __SHOP_ITEM* pShopItemLeft, const __SHOP_ITEM* pShopItemRight)
{
	std::string strLeft = pShopItemLeft->nameSeller;
	std::string strRight = pShopItemRight->nameSeller;
	return (strLeft < strRight);
}

bool SortBySellerLower(const __SHOP_ITEM* pShopItemLeft, const __SHOP_ITEM* pShopItemRight)
{
	std::string strLeft = pShopItemLeft->nameSeller;
	std::string strRight = pShopItemRight->nameSeller;
	return (strLeft > strRight);
}

bool SortByPriceUpper(const __SHOP_ITEM* pShopItemLeft, const __SHOP_ITEM* pShopItemRight)
{
	int nPriceLeft = pShopItemLeft->itemElem.m_nCost;
	int nPriceRight = pShopItemRight->itemElem.m_nCost;

	return (nPriceLeft < nPriceRight);
}

bool SortByPriceLower(const __SHOP_ITEM* pShopItemLeft, const __SHOP_ITEM* pShopItemRight)
{
	int nPriceLeft = pShopItemLeft->itemElem.m_nCost;
	int nPriceRight = pShopItemRight->itemElem.m_nCost;

	return (nPriceLeft > nPriceRight);
}

bool SortByLocationUpper(const __SHOP_ITEM* pShopItemLeft, const __SHOP_ITEM* pShopItemRight)
{


	std::string strLeft = pShopItemLeft->location;
	std::string strRight = pShopItemRight->location;
	return (strLeft < strRight);
}

bool SortByLocationLower(const __SHOP_ITEM* pShopItemLeft, const __SHOP_ITEM* pShopItemRight)
{
	std::string strLeft = pShopItemLeft->location;
	std::string strRight = pShopItemRight->location;
	return (strLeft > strRight);
}
void CWndShopFinder::SortByItemName()
{
	if (m_bUpperCase)
		std::sort(m_vecItems.begin(), m_vecItems.end(), SortByItemNameUpper);
	else
		std::sort(m_vecItems.begin(), m_vecItems.end(), SortByItemNameLower);
}
void CWndShopFinder::SortBySeller()
{
	if (m_bUpperCase)
		std::sort(m_vecItems.begin(), m_vecItems.end(), SortBySellerUpper);
	else
		std::sort(m_vecItems.begin(), m_vecItems.end(), SortBySellerLower);
}

void CWndShopFinder::SortByPrice()
{
	if (m_bUpperCase)
		std::sort(m_vecItems.begin(), m_vecItems.end(), SortByPriceUpper);
	else
		std::sort(m_vecItems.begin(), m_vecItems.end(), SortByPriceLower);
}

void CWndShopFinder::SortByLocation()
{
	if (m_bUpperCase)
		std::sort(m_vecItems.begin(), m_vecItems.end(), SortByLocationUpper);
	else
		std::sort(m_vecItems.begin(), m_vecItems.end(), SortByLocationLower);
}
#endif //__LEEAEG_SHOP_FINDER