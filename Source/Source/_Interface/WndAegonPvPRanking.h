#pragma once
#ifndef __WNDAEGPVPRANK__H
#define __WNDAEGPVPRANK__H
#include "AegonPvPRanking.h"
#ifdef __AEGON_PVP_RANKING

#ifdef __AEGON_APNG
#include <apngHelper.h>
#endif // __AEGON_APNG

class CWndPvPRankingListBox : public CWndListBox
{
public:
	CWndPvPRankingListBox();
	~CWndPvPRankingListBox();

	int	m_nCurSelect;

	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndPvPRankingMain : public CWndNeuz
{
private:

public:
	CWndPvPRankingMain();
	~CWndPvPRankingMain();


	CWndPvPRankingListBox m_pWndRankListBox;

	vector<__PVPRANKING_PROGRESS_UNIT*> m_vecItems;

	HANDLE m_hSortThread;
	int m_nSortMode;
	BOOL m_bSortDesc;

	CWndStatic* m_pWndStaticStatField;
	CWndStatic* m_pWndStaticRank;
	CWndStatic* m_pWndStaticName;
	CWndStatic* m_pWndStaticStatsHeader;

#ifdef __AEGON_APNG
	CAnimatedTexture* m_testTexture;
#endif // __AEGON_APNG

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual void OnDestroy();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point);

	void OnPvPRankingRefresh();
	void FillListVector();
	
	void SortVectorByPoints();
	void SortVector();


};
#endif // __AEGON_PVP_RANKING
#endif // __WNDAEGPVPRANK__H