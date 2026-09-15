#pragma once

#ifdef __LEEAEG_SHOP_FINDER
#include "AegonShopFinder.h"

class CWndShopFinder : public CWndNeuz
{
private:
	CWndEdit* m_pSearch;
	CWndComboBox* m_pFilterSex;
	CWndEdit* m_pFilterLvlLow;
	CWndEdit* m_pFilterLvlHigh;
	CWndEdit* m_pFilterPriceLow;
	CWndEdit* m_pFilterPriceHigh;
	CWndComboBox* m_pFilterCurrency;
	CWndListBox* m_pListItems;

	BOOL m_bUpperCase;
	char m_cSortType;
	HANDLE m_hSortThread;
public:
	vector<__SHOP_ITEM*> m_vecItems;

public:
	CWndShopFinder();
	~CWndShopFinder();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwStyle = 0);
	virtual void OnInitialUpdate();
	//virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnThemeSelectionChanged();

	void FillItemVec();
	void RefreshItemMap();

	void SortByItemName();
	void SortBySeller();
	void SortByPrice();
	void SortByLocation();
	void Sort();
};

#endif //__LEEAEG_SHOP_FINDER