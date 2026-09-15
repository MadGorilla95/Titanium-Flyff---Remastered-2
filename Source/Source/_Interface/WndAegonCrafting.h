#pragma once
#ifndef __WNDAEGCRAFT__H
#define __WNDAEGCRAFT__H
#include "AegonCrafting.h"
#ifdef __AEGON_CRAFTING_SYSTEM

#define AEGCRAFT_PROGRESSELEMENTS 10 + 1

class CWndRecyclingList : public CWndBase
{
	CWndScrollBar	m_wndScrollBar;

	vector<__CRAFTING_UNIT*> m_vRecyclingUnits;

	int m_nRowHeight;
public:
	CWndRecyclingList();
	virtual ~CWndRecyclingList();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface(CPoint point);
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

#ifdef __AEGON_THEME_SWITCHER
	virtual void OnThemeSelectionChanged();
#endif

	void	SetScrollBarVisible(BOOL bVisible = FALSE) { m_wndScrollBar.SetVisible(bVisible); };

};

class CWndCraftingMain : public CWndNeuz
{
private:

	CWndRecyclingList* m_wndRecyclingList;

	CWndStatic* m_pWndRecyclingTitle;

	CWndStatic* m_pWndRecipeName;
	CWndStatic* m_pWndDescription;
	CWndStatic* m_pWndGauge;
	CWndStatic* m_pWndResultItemText;

	CWndStatic* m_pWndResultItem;
	CWndStatic* m_pWndComponentItem[AEGCRAFT_MAXCOMPONENTS];


	CWndTreeCtrl* m_pWndTreeCtrl;

	CWndButton* m_pWndPlusButton;
	CWndButton* m_pWndMinusButton;
	CWndButton* m_pWndMaxButton;
	CWndButton* m_pWndSearchButton;



	CWndEdit* m_pWndCraftNum;

	CString m_strKeyword;
	CString m_strSelectedKeyword;
	DWORD m_dwSelectedData;

	__CRAFTING_UNIT* m_selectedUnit;
	CEditString m_descriptionString;

	CTexture* m_pItemBgTexture;
	CTexture* m_pResultItemTexture;
	CTexture* m_pComponentTexture[AEGCRAFT_MAXCOMPONENTS];
	CTexture* m_pActiveProgressTexture;
	CTexture* m_pProgressTexture[AEGCRAFT_PROGRESSELEMENTS];

	CItemElem* m_pComponentItem[AEGCRAFT_MAXCOMPONENTS];

	HANDLE m_hCraftingThread;
	HANDLE m_hSearchThread;

	int m_nCraftedItems;
	BOOL m_bAbortCrafting;

	BOOL m_bRecyclingPage;

public:
	CWndButton* m_pWndCraftButton;
	CWndButton* m_pWndCancelButton;
	BOOL m_bCraftingInProgress;

	CWndComboBox* m_pWndComboSearch;

public:
	CWndCraftingMain();
	~CWndCraftingMain();




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

	void UpdateSelected();
	void SetEmptyRecipePage();
	void SetSelectedRecipePage();
	void SetRecyclingPage();
	void SetRecyclingText();
	void AddComponentItem(CItemElem* pItemElem);
	void RemoveComponentItem(int nIndex);
	void OnCraftingExpRefresh();
	void OnSuccessFullCraft();
	void CraftingProcess();
	BOOL CraftOnce();
	void OnThreadEnd();
	int GetMaxPossibleCrafts();

	void UpdateSearch();
	void OpenRecipe(CString recipeString);

	void PrepareRecipeChange();
	
};
#endif // __AEGON_CRAFTING_SYSTEM
#endif // __WNDAEGCRAFT__H