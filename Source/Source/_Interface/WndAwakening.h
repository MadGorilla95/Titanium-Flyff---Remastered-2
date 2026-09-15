#ifndef __WND__H
#define __WND__H

#if __VER >= 11 // __SYS_IDENTIFY
#ifdef __AEGON_AWAKEWINDOW
class CWndAwakening : public CWndNeuz
{
public:

	CItemElem* m_pItemElem;
	ItemProp* m_pEItemProp;
	CTexture* m_pTexture;
	LPCTSTR m_pLineStr[3];
	LPCTSTR m_pLineStr1;
	LPCTSTR m_pLineStr2;
	LPCTSTR m_pLineStr3;
	DWORD dwLineColor[3];

#ifdef __AWAKE_NEEDSTONE
	//mat
	CItemElem* m_pMatElem;
	ItemProp* m_pEMatProp;
	CTexture* m_pTextureMat;
#endif // __AWAKE_NEEDSTONE

	CWndStatic* m_pLine[3];

	CWndAwakening();
	~CWndAwakening();

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
	virtual BOOL process();
	void SetAwakeLines();
	void SetEmptyLines(int num);

	void SetAwakeItem(CItemElem* pItemElem);
	void SetItem(CItemElem* pItemElem);

#ifdef __AWAKE_NEEDSTONE
	void SetMat(CItemElem* pItemElem);
#endif // __AWAKE_NEEDSTONE

};
#endif // __AEGON_AWAKEWINDOW
#endif

#ifdef __PROTECT_AWAKE
class CWndSelectAwakeCase : public CWndNeuz
{
	// 각성 보호 선택창 ( 두가지중에 하나 고름 < 각성전, 각성후 > 0
public:

	CWndSelectAwakeCase( );
	virtual ~CWndSelectAwakeCase( );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
//	virtual void OnDestroy();
//	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
//	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
//	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual BOOL process();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	void SetItemIndex( const DWORD index )			{ m_dwItemIndex = index; }
	void SetData( BYTE byObjID, DWORD dwSerialNum, __int64 n64NewOption );
	void OutputOptionString( C2DRender* p2DRender, CItemElem* pItemElem, BOOL bNew = FALSE );

protected:
	DWORD m_dwOldTime;
	DWORD m_dwDeltaTime;

	DWORD m_dwItemIndex;
	CTexture*  m_pTexture;

	BYTE m_byObjID;
	DWORD m_dwSerialNum;
	__int64 m_n64NewOption;

	CTexture* m_pTexGuage;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBufferGauge;
};

#endif //__PROTECT_AWAKE

#ifdef __AEGON_RETURN_AWAKE
class CWndReturnAwakening : public CWndNeuz
{
public:

	CItemElem* m_pItemElem;
	ItemProp* m_pEItemProp;
	CTexture* m_pItemTexture;

	CItemElem* m_pMaterialElem;
	ItemProp* m_pEMaterialProp;
	CTexture* m_pMaterialTexture;

	LPCTSTR m_pCurrentLineStr[3];
	DWORD dwCurrentLineColor[3];
	CWndStatic* m_pCurrentLine[3];

	LPCTSTR m_pOldLineStr[3];
	DWORD dwOldLineColor[3];
	CWndStatic* m_pOldLine[3];

	CWndReturnAwakening();
	~CWndReturnAwakening();

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
	virtual BOOL process();
	void CWndReturnAwakening::SetAwakeLines(int nMode);
	void CWndReturnAwakening::SetEmptyLines(int nMode, int num);
	virtual CString GetRevertPriceStr(ItemProp* itemProp);
	virtual CString GetRevertItemLevelStr(ItemProp* itemProp);

};
#endif // __AEGON_RETURN_AWAKE

#endif
