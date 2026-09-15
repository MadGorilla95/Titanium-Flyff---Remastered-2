#pragma once
#include "AegonSharedTypes.h"
#ifdef __AEGON_F_TOOL
#define AT_LEADER_BUTTONS 5

class CWndFToolList : public CWndListBox
{
	int m_nSelectedItem;


public:
	int m_nRowHeight;
	CRect m_rectPage;
	CRect m_rectKey;
	CRect m_rectInterval;
	CRect m_rectStatus;
	CRect m_rectAction;

	CWndFToolList();
	virtual ~CWndFToolList();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	// message
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface(CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);


	void SetSelectedItem(CPoint point);
	void ToggleSelectedUnit();

	int GetSelectedItemIndex() { return m_nSelectedItem; };

};
class CWndAegonFTool : public CWndNeuz
{
	CWndFToolList* m_pWndFToolList;
public:

	CWndAegonFTool();
	~CWndAegonFTool();


	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point);

	void EditSelected();
	void DeleteSelected();
	void AddUnit();

};
#endif // __AEGON_F_TOOL