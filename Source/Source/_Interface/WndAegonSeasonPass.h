#pragma once
#ifndef __WNDAEGSEAPASS__H
#define __WNDAEGSEAPASS__H
#include "AegonSeasonPass.h"
#ifdef __AEGON_SEASON_PASS

class CWndSeasonPassRewards : public CWndBase
{
public:

	CWndScrollBar m_wndScrollBar;

	CTexture* m_rewardBox[3][3];
	CTexture* m_numberField[3];

	CTexture* m_glowTexture;

	CRect m_rewardBoxRect[6];
	CRect m_rewardNumRect[6];
	CRect m_rewardFields[6];

	int m_selectedReward;
	int m_reachedLevel;

	CTexture* m_itemTextures[6];

	CTexture* m_pTextureLevelConnection[2];

	int m_nRewardBoxOffset;
	int m_nScrollFactor;
	int m_nScrollStep;


	CWndSeasonPassRewards();
	~CWndSeasonPassRewards();

	void Create(DWORD m_dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID);

	virtual	void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface(CPoint pt);
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	void OnSelectedPassChanged();
	void SetSelectedRewardCurrentLevel();
	void SetScrollPosReward(int nReward);
	void OnProgressRefresh();
	
};

class CWndSeasonPassMain : public CWndNeuz
{
private:

	CWndStatic* m_pWndStaticPassBox;


	CWndButton* m_pWndButtonClaim;
	CWndButton* m_pWndButtonUpgrade;

	CRect m_rectPvESelect;
	CRect m_rectPvPSelect;

	CRect m_rectCountdown;
	CRect m_rectLevelProg;
	CRect m_rectProgBar;
	CRect m_rectItemPreview;
	CRect m_rectItemDescr;

	CTexture* m_pTexturePvESelect;
	CTexture* m_pTexturePvPSelect;

	CTexture* m_pTextureItemPreviewBox;
	CTexture* m_pTextureItemPreviewField[2];

	CTexture* m_pTextureProgBar[2];

	CTexture* m_pTextureSelReward;

	CWndSeasonPassRewards* m_pWndRewards;

	BOOL m_pveSelected;

public:
	__SEASONPASS_UNIT* m_selectedUnit;

	CWndSeasonPassMain();
	~CWndSeasonPassMain();




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
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual void OnMouseWndSurface(CPoint point);

	void OnSetPassType(BOOL bPvE);
	void OnProgressRefresh();
	

};
#endif // __AEGON_SEASON_PASS
#endif // __WNDAEGSEAPASS__H