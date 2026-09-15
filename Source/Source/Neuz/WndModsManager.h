#pragma once

#ifndef __WNDMODSMANAGER__H
#define __WNDMODSMANAGER__H

#ifdef __MODS_MANAGER
class CWndModsManager : public CWndNeuz
{
private:
	CWndButton* pButton[7];

public:
	CWndModsManager();
	virtual ~CWndModsManager();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
};
#endif // __MODS_MANAGER

#endif //__WNDMODSMANAGER__H