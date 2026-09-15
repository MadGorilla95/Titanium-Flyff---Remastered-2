#pragma once
#ifdef __SWITCH_EQUIP
class CWndSwitchEquip : public CWndNeuz
{
private:
	CPoint				m_OldPos;
	FLOAT				m_fRot;
	CModelObject* m_pModel;
	CMover				m_Mover;
	BOOL				m_bLButtonDownRot;
	CTexture* m_pTextureDrag;

	EQUIP_INFO_ADD		m_aEquipInfoAdd[MAX_HUMAN_PARTS];
	SHORTCUT			m_aSlotItem[MAX_HUMAN_PARTS];
	DWORD				m_dwId[MAX_HUMAN_PARTS];
	bool				m_bSelect;

	void UpdateItem();

	int GetPartId(CItemElem* pItemElem, LPSHORTCUT pShortcut);

public:
	CRect				m_InvenRect[MAX_HUMAN_PARTS];
#ifdef __AEGON_THEME_SWITCHER
	CRect		m_charRenderRect;
	CRect		m_hotkeyRect;
#endif // __AEGON_THEME_SWITCHER

	CWndSwitchEquip();
	virtual ~CWndSwitchEquip();
	virtual BOOL Process();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnMouseWndSurface(CPoint point);
	virtual	void OnInitialUpdate();
	virtual void OnDestroy(void);
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point);
	virtual void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);

	int m_nIndex;
	void LoadSwitch();
	void AddItem(CItemElem* pItemElem, LPSHORTCUT pShortcut);

#ifdef __MIKSIK_EQUALIZED_MODE 
	void UpdateParts();
#endif //__MIKSIK_EQUALIZED_MODE

#ifdef __AEGON_THEME_SWITCHER
	virtual void OnThemeSelectionChanged();
#endif

};
#endif // __SWITCH_EQUIP