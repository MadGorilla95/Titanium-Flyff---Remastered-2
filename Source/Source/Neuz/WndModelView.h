#pragma once

#ifdef __MODEL_VIEW
class CWndModelView : public CWndNeuz
{
private:
	CMover m_Mover;
	FLOAT m_fRot;
	FLOAT m_fRot1;
	FLOAT m_fWheel;
	D3DXVECTOR3 vecPos123;
	D3DXVECTOR3 vecPos1234;
	CModelObject* m_pModel;
	CModelObject* m_pPet;
	CModelObject* m_pEgg;
	CModelObject* m_pMount;
#ifdef __ANI_WING
	CModelObject* m_pAniMask;
#endif //__ANI_WING
#ifdef __LNB_ANIMATED_HATS
	CModelObject* m_pAnimatedHat;
#endif //__LNB_ANIMATED_HATS

	CPoint m_ptOld;
	CPoint m_ptOld1;
	BOOL m_bLButtonDownRot;
	BOOL m_bRButtonDownRot;

#ifdef __AEGON_PREM_MOTIONS
	BOOL m_bMotionPreview;
	MotionProp* m_pMotionProp;
#endif // __AEGON_PREM_MOTIONS

	CWndButton* m_pRotateCheck;

public:
	CWndModelView(void);
	~CWndModelView(void);

public:
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnInitialUpdate();
	virtual BOOL Process();
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void SetWndRect(CRect rectWnd, BOOL bOnSize);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);

	void UpdMover();
	void SetSex(BOOL bSex);
	bool SetPart(CItemElem* pItem);
	void UpdItem(DWORD dwParts, DWORD dwID, int nOption);

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
};
#endif