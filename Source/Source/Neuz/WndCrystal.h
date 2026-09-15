#ifndef __WND_CRYSTAL_H_
#define	__WND_CRYSTAL_H_

#ifdef __CRYSTAL
class CWndCrystal : public CWndNeuz
{
public:

	CWndCrystal();
	virtual ~CWndCrystal();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point = 0);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface(CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);

private:
	CTexture* pCustom;

	CItemElem* pItem1;
	CItemElem* pItem2;
	CItemElem* pItem3;

	CTexture* pItem1Tex;
	CTexture* pItem2Tex;
	CTexture* pItem3Tex;
};
#endif // __CRYSTAL
#endif // __WND_CRYSTAL_H_