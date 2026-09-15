#pragma once
#ifndef __WNDMNSTHNT__H
#define __WNDMNSTHNT__H
#include "MonsterHunt.h"
#ifdef __AEGON_MONSTER_HUNT


class CWndMonsterHuntQuest : public CWndListBox
{
public:

	vector<float> m_fVecRadius;

	vector< __MONSTERHUNT_QUEST*> m_pVecQuests;

	vector<CModelObject*> m_pVecModel;
	vector<CTexture*> m_pVecItemTexture;

	CTexture* m_pItemLockedTexture;
	CTexture* m_pRepeatableTexture;
	LPDIRECT3DTEXTURE9 m_progressBarTexture;

	BOOL loadedFilters;

	CWndMonsterHuntQuest();
	~CWndMonsterHuntQuest();

	void Create(DWORD m_dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID);
	
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface(CPoint pt);
	virtual void LoadFilter();
	virtual CString GetDstString(DWORD nDst, DWORD nDstNum);
	virtual void RenderMob(C2DRender* p2DRender, DWORD mobID, DWORD questID, CRect lpFace, BOOL doRender);

	void FilterChanged();
	BOOL FilterCheck(__MONSTERHUNT_QUEST* pQUestUnit);

	void FillQuestVec();
	void InitModelVec();
	void LoadMobModel(int nQuestID);
	virtual void LoadItemTextures();
	void RemoveObjects();
};

class CWndMonsterHunt : public CWndNeuz
{
public:
	CWndMonsterHunt();
	~CWndMonsterHunt();

	CWndMonsterHuntQuest* m_wndQuest;

	

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
	
};
#endif // __AEGON_MONSTER_HUNT
#endif // __WNDMNSTHNT__H