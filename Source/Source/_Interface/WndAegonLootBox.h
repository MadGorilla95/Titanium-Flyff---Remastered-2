#pragma once
#ifndef __WNDAEGLOOBOX__H
#define __WNDAEGLOOBOX__H
#include "AegonLootBox.h"
#ifdef __AEGON_LOOT_BOX

#define LOOTBOX_ANI_ELEMENTS 100
#define LOOTBOX_ITEM_BG_TEXTURES 6

class CWndLootBoxInfo : public CWndNeuz
{
private:

public:
	CWndLootBoxInfo();
	~CWndLootBoxInfo();




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


class CWndLootBoxMain : public CWndNeuz
{
private:

	CItemElem* m_pTicketElem;
	CTexture* m_pTextureTicket;
	
	CWndStatic m_ItemFields[LOOTBOXITEMS];
	CItemElem m_BoxItems[LOOTBOXITEMS];
	CTexture* m_pTextureItems[LOOTBOXITEMS];

	CWndStatic m_TicketFields[LOOTBOXTICKETS];
	CItemElem m_TicketItems[LOOTBOXTICKETS];
	CTexture* m_pTextureTickets[LOOTBOXTICKETS];
	
	int m_nSelectedBox;

	BOOL m_bReceivedResult;
	__LOOTBOX_RESULT m_result;

	HANDLE m_hAniThread;
	BOOL m_aniInProgress;
	DWORD m_aniDelay;
	DWORD m_aniItemCount;
	DWORD m_lastAniStep;
	DWORD m_aniStep;
	BOOL m_aniNeedsInit;

	CTexture* m_pAniTextures[LOOTBOXITEMS]; 
	CTexture* m_pAniWinTexture;
	CTexture* m_pAllAniTextures[LOOTBOX_ANI_ELEMENTS];

	CTexture* m_pAllAniBGTextures[LOOTBOX_ANI_ELEMENTS];
	CTexture* m_pItemBGTextures[LOOTBOX_ITEM_BG_TEXTURES];
	CTexture* m_pAniWinBGTexture;

	CPoint m_aniPt;
	int winTextMinx;

public:
	CWndLootBoxMain();
	~CWndLootBoxMain();




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

	void OnSelBoxChanged();
	void ReceiveResult(__LOOTBOX_RESULT result);
	void RemoveTicket(BOOL force = FALSE);
	void SetTicket(CItemElem* pTicketElem);
	void InitAni();
	void StartResultAni();
	void DoAniStep();
	void RenderAni(C2DRender* p2DRender);
	void SetButtonMode(int mode);
	CTexture* GetItemBGTexture(DWORD chance);
	void RefreshPrice();
};
#endif // __AEGON_LOOT_BOX
#endif // __WNDAEGLOOBOX__H