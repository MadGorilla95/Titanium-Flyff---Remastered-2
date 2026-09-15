#pragma once

#ifndef __WNDAEGTELWND__H
#define __WNDAEGTELWND__H

#ifdef __AEGON_TELEPORTER
#include "AegonTeleportMng.h"

#ifdef __AEGON_TELEPORT_SHORTCUT
class CWndTeleporterShortcut : public CWndCustom
{
public:
	CTexture* m_pTexShortcut;
	int m_nSelected;
	CString m_strSelected;
	CWndTeleporterShortcut();
	~CWndTeleporterShortcut();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual	void OnInitialUpdate();
	virtual void OnMouseWndSurface(CPoint pt);
};
#endif // __AEGON_TELEPORT_SHORTCUT
class CWndTeleportList : public CWndListBox
{
private:
	vector<DWORD> m_vecItem;
public:
	CWndTeleportList();
	~CWndTeleportList();

	void UpdateList(int nUnitID);
	void UpdateList(__AEGON_TELEPORT_UNIT* pUnit);

	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnMouseWndSurface(CPoint pt);
};
class CWndTeleporter : public CWndNeuz
{
#ifdef __AEGON_TELEPORT_SHORTCUT
	CWndTeleporterShortcut m_wndTeleporterShortcut;
#endif

public:
	vector<__AEGON_TELEPORT_UNIT> m_vUnitList;
	int m_nSelected;
	CTexture* m_pPicTexture;
	CWndTeleportList* m_pWndTeleportList;
	CWndTeleporter(void);
	~CWndTeleporter(void);

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

public:
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	//virtual BOOL Process();

#ifdef __AEGON_THEME_SWITCHER
	virtual void OnThemeSelectionChanged();
#endif

#ifdef __TIMER
public:
	void	OnDungeonTimer(CAr& ar);
private:
	DWORD m_timer;
	vector<CoolDungeons> vDun;
	const void ConvertDungeons(vector<CoolDungeons>& v);
	const DWORD GetTimer(int nselected);
#endif //__TIMER

	void FillUnitVector();
	void OnSelectionChanged();

};
#endif



#if __VER >= 12 // __HEAVEN_TOWER
#define MAX_FLOOR_COUNT 15

class CWndHeavenTower : public CWndNeuz
{
	vector<__AEGON_TELEPORT_UNIT> m_vUnitList;

public:

	CTexture* m_pPicTexture;
	CWndTeleportList* m_pWndTeleportList;

	virtual HRESULT DeleteDeviceObjects();

public:
	CWndHeavenTower();
	virtual ~CWndHeavenTower();

	virtual void OnDestroy();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();

	void OnSelChanged();
	void InitTeleportList();

	void FillUnitVector();
};

#endif //__HEAVEN_TOWER

#endif