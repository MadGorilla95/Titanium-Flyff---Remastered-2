#pragma once

#ifdef __TASKBAR_OPEN_SITE
class CWndHomepage : public CWndNeuz
{
public:
	CWndHomepage();
	~CWndHomepage();
	virtual BOOL Initialize(CWndBase* pWndParent, DWORD);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
};

class CWndDiscord : public CWndNeuz
{
public:
	CWndDiscord();
	~CWndDiscord();
	virtual BOOL Initialize(CWndBase* pWndParent, DWORD);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
};
#endif //__TASKBAR_OPEN_SITE