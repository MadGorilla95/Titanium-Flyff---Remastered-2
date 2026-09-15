#pragma once
#include "WndManager.h"
#include "WndField.h"
#include "WndBagEx.h"


#ifdef __AEGON_VIEW_INV
class CWndViewBag : public CWndBagEx
{
public:
	CMover* m_viewMover;

	CWndViewBag::CWndViewBag();
	CWndViewBag::~CWndViewBag();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual	void OnInitialUpdate();

	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);

	BOOL OnDropIconOnItemCtrl(LPSHORTCUT pShortcut, int nContainerField);


};

class CWndViewInventoryCtrl : public CWndInventory
{
public:
	CMover* m_viewMover;

	CWndViewInventoryCtrl::CWndViewInventoryCtrl();
	CWndViewInventoryCtrl::~CWndViewInventoryCtrl();

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	void UpdateParts();
	virtual void OnMouseWndSurface(CPoint point);
	virtual void OnDraw(C2DRender* p2DRender);
	void UpDateModel();
	virtual	void OnInitialUpdate();
	virtual BOOL Process();
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point = 0);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);

	virtual void OnDestroy(void);

};

class CWndViewInventoryMain : public CWndNeuz
{
public:
	CMover m_viewMover;

	CWndViewInventoryMain();
	~CWndViewInventoryMain();

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnDestroy(void);
	void OnInventoryView(CMover* pViewMover);
	void OnBagView(CMover* pViewMover);
	void OnBankView(CMover* pViewMover);
	void DoInventoryView(LPCTSTR viewName = NULL, int nContainer = -1);


};

#endif // __AEGON_VIEW_INV