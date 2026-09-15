#pragma once

#ifdef __BAN_SYSTEM
class CWndPlayerPurge : public CWndNeuz
{
public:
	CWndPlayerPurge();
	~CWndPlayerPurge();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);

};
#endif //__BAN_SYSTEM
