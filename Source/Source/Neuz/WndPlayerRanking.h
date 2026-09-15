#pragma once
#ifndef __WNDPLAYERRANKING_H__
#define __WNDPLAYERRANKING_H__
#ifdef __ROYAL_RUMBLE

#include "PlayerWar.h"

class CWndPlayerRanking : public CWndNeuz
{
public:
	CWndScrollBar		m_wndScrollBar;
	int					m_nMax;
	SIEGE_RANK		m_listRank[100];

	CWndPlayerRanking();
	virtual ~CWndPlayerRanking();
	virtual void RefreshList();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual	void OnLButtonDown(UINT nFlags, CPoint point);
};

#endif //__ROYAL_RUMBLE
#endif //__WNDPLAYERRANKING_H__