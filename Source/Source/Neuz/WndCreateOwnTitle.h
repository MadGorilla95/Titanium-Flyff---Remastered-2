#pragma once
#ifdef __SYS_CUSTOM_TITLE
class CWndCreateOwnTitle : public CWndNeuz
{
public:
	CWndCreateOwnTitle();
	virtual ~CWndCreateOwnTitle();
	virtual	void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnSize(UINT nType, int cx, int cy);

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
#endif // __SYS_CUSTOM_TITLE