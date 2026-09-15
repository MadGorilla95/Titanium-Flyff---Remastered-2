#pragma once
#ifndef __WNDAEGINGSHP__H
#define __WNDAEGINGSHP__H
#include "AegonIngameShop.h"

#ifdef __AEGON_INGAME_SHOP

#define AEGINGSHP_SORT_NAME_A 0
#define AEGINGSHP_SORT_NAME_D 1
#define AEGINGSHP_SORT_PRICE_A 2
#define AEGINGSHP_SORT_PRICE_D 3


class CWndIngameShopList : public CWndListBox
{

	vector<__AEGIGNSHP_ITEM_UNIT> m_vItemList;

	int m_nSelectedItem;

	int m_nRowHeight;

	HANDLE m_hSortThread;
public:
	DWORD m_dwSortMethod;

	CWndIngameShopList();
	virtual ~CWndIngameShopList();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	// message
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface(CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);

	void SetSelectedItem(CPoint point);
	void UpdateItemList();
	BOOL FilterCheck(__AEGIGNSHP_ITEM_UNIT* pShopUnit);
	void SortVector();
	void SortVectorThread();
	void BuySelectedItem();
#ifdef __MODEL_VIEW
	void PreviewSelectedItem();
#endif //__MODEL_VIEW

};
class CWndIngameShopMain : public CWndNeuz
{
private:
	CWndIngameShopList* m_wndIngameShopList;	
	CWndStatic* m_pWndDpAmount;


public:
	CWndListBox* m_pWndCategoryList;
	CWndEdit* m_pWndSearchItem;
	CWndEdit* m_pWndFilterLevelLow;
	CWndEdit* m_pWndFilterLevelHigh;
	CWndEdit* m_pWndFilterPriceLow;
	CWndEdit* m_pWndFilterPriceHigh;
	CWndComboBox* m_pWndSexFilter;

	CWndIngameShopMain();
	~CWndIngameShopMain();


	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual void OnDestroy();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point);
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	void ResetFilters();


};
#endif
#endif