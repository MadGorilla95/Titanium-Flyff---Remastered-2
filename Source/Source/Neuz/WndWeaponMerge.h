#pragma once

#ifdef __WEAPON_MERGE
class CWndWeapnMergeConfirm : public CWndNeuz
{
public:
	CWndButton m_wndButton1;
	CWndButton m_wndButton2;

protected:
	CString m_strText;
	CWndText m_wndText;
public:
	virtual void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwStyle = 0);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);

	void SetText(CString strText);
	void SetText(int nID);
};
class CWndWeaponMergeCancel : public CWndNeuz
{
private:
	LPWNDCTRL m_CtrlWeapon;
	CWndStatic* m_pStCost;
	LPWNDCTRL m_CtrlGauge;
	CWndButton* m_pStart;

	CItemElem* m_pItemElem;

	CTexture m_TexGaugeProcess;
	LPDIRECT3DVERTEXBUFFER9 m_pVBProcess;

	char m_nStatus;
	int m_nCost;

	BOOL m_bRenderGauge;

	clock_t m_clkEnd;

#ifdef __AEGON_FIXES
	CWndWeapnMergeConfirm* m_pWndConfirm;
#endif
public:
	CWndWeaponMergeCancel();
	virtual ~CWndWeaponMergeCancel();
	virtual void OnInitialUpdate(); //OK
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0); //OK
	virtual BOOL Process(); //PACKET
	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point = 0);
	virtual void OnMouseWndSurface(CPoint point);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
	virtual void OnDestroyChildWnd(CWndBase* pWndChild);
#endif

	BOOL SetItem(CItemElem* pItem);
private:
	void CalcCancelCosts();
};

class CWndWeaponMerge : public CWndNeuz
{
private:
	LPWNDCTRL m_CtrlWeaponDest;
	LPWNDCTRL m_CtrlWeaponSource;
#ifndef m_pItemProt
	LPWNDCTRL m_CtrlProtScroll;
#endif
	LPWNDCTRL m_CtrlRenderWeapon;
	LPWNDCTRL m_CtrlOutputStats;
	CItemElem* m_pItemSource;
	CItemElem* m_pItemDest;
#ifndef m_pItemProt
	CItemElem* m_pItemProt;
#endif
	CWndButton* pButtonStart;
	CModelObject* m_pDestModel;
	CEditString m_editSwordOption;
	//CWndConfirm *m_pWndConfirm;

#ifdef __AEGON_FIXES
	CWndWeapnMergeConfirm* m_pWndConfirm;
#endif

	float m_fRotate;
	char m_nStatus;
	int m_nCount;
	int m_nDelay;
	float m_nEyeYPos;
	float m_fAddRot;
public:
	CWndWeaponMerge();
	virtual ~CWndWeaponMerge();
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point = 0);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = 0, DWORD dwWndId = 0);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnMouseWndSurface(CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL Process();

	virtual void OnDestroy();

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
	virtual void OnDestroyChildWnd(CWndBase* pWndChild);
#endif

	BOOL SetItem(CItemElem* pItem, short nSlot = -1);
private:
	int RegisterWeapon(CItemElem* pItem, CItemElem* pOther);
	bool CompareItemLevel(ItemProp* pPropSource, ItemProp* pPropDest);
	void Clear(bool bProt = false);
};



#endif // __WEAPON_MERGE