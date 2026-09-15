#pragma once
#ifdef __SYS_PETFILTER
class CWndPetfilter : public CWndNeuz
{
public:
	CWndPetfilter();
	~CWndPetfilter();

	LONGLONG dwPetfilter;

#ifdef __AEGON_DB_PETFILTER
	BOOL m_bInitialized;
#endif

	virtual void SetButtonStatus();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);

	void ApplyFilters();

	void SetPetFilterMode(BOOL bAdd, LONGLONG dwMode);
	void AddPetFilterMode(LONGLONG dwMode);
	void RemovePetFilterMode(LONGLONG dwMode);
	BOOL DoInitCheck();
};


class CWndPetfilterItemList : public CWndListBox
{

public:
	int m_nSelectedItem;
	int m_nRowHeight;

	__AEGON_PET_FILTER_LIST_UNIT* m_pItemList;

	CWndPetfilterItemList();
	virtual ~CWndPetfilterItemList();

	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	// message
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface(CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);

	void SetSelectedItem(CPoint point);
};

class CWndPetfilterLists : public CWndNeuz
{
public:
	CWndPetfilterLists();
	~CWndPetfilterLists();

	CWndPetfilterItemList* m_pWndWhitelistItems;
	CWndPetfilterItemList* m_pWndBlacklistItems;

	BOOL m_bAddToWhitelist;

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);

	void OnAddItemToFilterList(DWORD dwItemID);
};

class CWndPetfilterMain : public CWndNeuz
{
public:

	CWndPetfilter		m_pWndFilter;
	CWndPetfilterLists	m_pWndLists;

	CWndPetfilterMain();
	~CWndPetfilterMain();

	
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);

	void OnAddItemToFilterList(DWORD dwItemID);

};
#endif //__SYS_PETFILTER