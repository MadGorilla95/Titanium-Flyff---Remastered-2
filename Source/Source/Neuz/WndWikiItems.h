#pragma once

#ifdef __WIKIPEDIA_ITEMS
class CWndCreateItems;
class CWndWikiItems : public CWndNeuz
{
private:
	CWndCreateItems* m_pCreate;
	vector<ItemProp*> m_vecItems;
	CWndListBox* m_pListItems;
	CWndComboBox* m_pComboJob;
	CWndComboBox* m_pFilterSex;
	CWndEdit* m_pEditFilter;
	CWndEdit* m_pFilterLvlLow;
	CWndEdit* m_pFilterLvlHigh;

	CString m_strKeyword;

	HANDLE m_hSortThread;

	bool m_bUpperCase;
	char m_cSortType;

	CString Sel;
	CString Select(CString s) { return s; }

public:
	CWndWikiItems();
	~CWndWikiItems();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwStyle = 0);

	virtual void OnInitialUpdate();

	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
	virtual void OnDestroyChildWnd(CWndBase* pWndChild);
#endif

#ifdef __FL_FARM_STAT
	BOOL bWikiSelectMode;
#endif //__FL_FARM_STAT

	vector<ItemProp*>* GetItemList()
	{
		return &m_vecItems;
	}

	void UpdateList();
	void ResetFilters();
	void SortByName();
	void SortByLevel();
	void SortByJob();

	void Sort();
};

class CWndCreateItems : public CWndNeuz
{
private:
	ItemProp* m_pPropCreate;
	CWndEdit* m_pEditAmount;
	CWndEdit* m_pEditName;
	LPWNDCTRL m_pDrawCtrl;
	CWndStatic* m_pStcName;
	CWndComboBox* pComboBox[4];
	int m_Select[4];

public:
	CWndCreateItems();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwStyle = 0);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	void OnMouseWndSurface(CPoint point);

	void SetItem(ItemProp* pPropCreate);
};
#endif //__WIKIPEDIA_ITEMS