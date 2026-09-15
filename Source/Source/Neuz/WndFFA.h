#pragma once
#ifdef __ROYAL_RUMBLE
class CWndJoinPlayerSiege : public CWndNeuz
{
public:
	CWndJoinPlayerSiege();
	virtual ~CWndJoinPlayerSiege();

	virtual	void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnSize(UINT nType, int cx, int cy);

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
#endif // __ROYAL_RUMBLE