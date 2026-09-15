#pragma once

#ifdef __WIKIPEDIA_MONSTER_DROPLIST
class CWndDropInfo : public CWndNeuz
{
private:
	DWORD mMonsterID;

	CModelObject* m_modelObj;
	float m_fRadius;
	float m_fZoom;
	float m_fRot;
	float m_fRot1;
	D3DXVECTOR3 vecPosTranslate;

	CPoint m_ptOld;
	CPoint m_ptOld1;
	BOOL m_bLButtonDownRot;
	BOOL m_bRButtonDownRot;
	BOOL m_bExecMotion;

	LPWNDCTRL m_lpWndMobField;
	CWndComboBox* pWndComboBoxAnimation;

public:

	CWndDropInfo();
	~CWndDropInfo();

	void OnInitialUpdate();
	BOOL Initialize(CWndBase* pWndParent = nullptr, DWORD dwStyle = 0);

	virtual void OnDestroy();
	DWORD GetMonsterID() const;
	void SetMonsterID(DWORD dwMonsterID);
	void ResetModelView();
	void ChangeAnimation();

	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	void LoadMobModel();
	void RenderMob(C2DRender* p2DRender, CRect lpFace);
};
#endif //__WIKIPEDIA_MONSTER_DROPLIST