#pragma once
#ifndef __WNDAEGACHSYS__H
#define __WNDAEGACHSYS__H
#include "AegonAchievementSystem.h"
#ifdef __AEGON_ACHIEVEMENT_SYSTEM

#define MAXCATEGORIES MAXSUBCATEGORIES + 1
#define AEGACH_MAIN_GAUGE_HEIGHT 20
#define AEGACH_SIDE_GAUGE_HEIGHT 20

class CWndAchievementSystemMain : public CWndNeuz
{
private:
	CWndListBox* m_pListItems;
	CWndStatic* m_pImageWnd;
	CWndStatic* m_pProgressWnd;
	CWndText* m_pTextWnd;
	CWndStatic* m_pProgressBarWnd;
	CWndTreeCtrl* m_pWndTreeCtrl;

	CWndButton* m_CollectButton;
	CWndButton* m_FavoriteCheck;

	CRect m_pMainProgressBarGaugeRect;
	CRect m_pSideProgressBarGaugeRect;


	CString m_strKeyword;
	CString m_strSelectedKeyword;

	DWORD m_dwSelectedData;

	int nCurListSelection;

	CTexture* m_achievementImage;

	CTexture* m_mainProgressGaugeBack;
	CTexture* m_mainProgressGaugeFront;
	LPDIRECT3DVERTEXBUFFER9 m_pMainGaugeBack;
	LPDIRECT3DVERTEXBUFFER9 m_pMainGaugeFront;

	CTexture* m_sideProgressGaugeBack;
	CTexture* m_sideProgressGaugeFront;
	CTexture* m_sideProgressGaugeFront2;

	LPDIRECT3DVERTEXBUFFER9 m_pSideGaugeBack;
	LPDIRECT3DVERTEXBUFFER9 m_pSideGaugeFront;
	

public:
	vector<__ACHIEVEMENT_UNIT*> m_vecItems;

public:
	CWndAchievementSystemMain();
	~CWndAchievementSystemMain();




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

	void UpdateList();
	void ListBoxSelChanged();
	void OpenAchievement(int achievementID);
	void RefreshButtonState();
	void OnFavoritesRefresh();
	void OnProgressRefresh();

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

};
#endif // __AEGON_ACHIEVEMENT_SYSTEM
#endif // __WNDAEGACHSYS__H