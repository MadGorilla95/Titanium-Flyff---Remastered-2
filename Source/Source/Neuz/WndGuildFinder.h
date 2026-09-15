#ifndef __WNDGUILD_FINDER__H
#define __WNDGUILD_FINDER__H
#pragma once

#include "Guild.h"

#ifdef __GUILDFINDER
class CWndGuildFinder : public CWndNeuz
{
private:
	int		m_nSortType;
	BOOL	m_bIsGreater;
	int		m_nSelected;

public:
	vector<GUILDFINDER_LIST>	m_mapItem;

	CWndGuildFinder();
	virtual ~CWndGuildFinder();

	virtual void SerializeRegInfo(CAr& ar, DWORD& dwVersion);
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	void RefreshItemList();
	void Sort();
};


class CWndGuildFinderReq : public CWndNeuz
{
public:
	LONG					m_idPlayer;
	int						m_nClass;
	int						m_nLevel;
	BYTE					m_byCheckhero;

	CWndGuildFinderReq(LONG idPlayer, int nLevel, int nClass, BYTE cCheckhero);
	virtual ~CWndGuildFinderReq();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};
#endif //__GUILDFINDER

#endif //__WNDGUILD_FINDER__H