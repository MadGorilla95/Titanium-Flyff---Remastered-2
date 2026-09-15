#pragma once
#include "WndManager.h"

#ifdef __PREMIUM
class CWndPremium : public CWndNeuz
{
	vector<DWORD> m_vCustomIDs;
	vector<CString> m_vCustomImages;
public:
	CWndPremium();
	~CWndPremium();
	virtual BOOL Initialize(CWndBase* pWndParent, DWORD);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnSize(UINT nType, int cx, int cy);
};
#endif //__PREMIUM