#ifndef __WND_AUTO_SHOUT_H
#define __WND_AUTO_SHOUT_H

#ifdef __SYS_AUTO_SHOUT
class CWndAutoShout : public CWndNeuz
{
public:
	CWndAutoShout();
	~CWndAutoShout();

	CWndButton* m_pWndButton[MAX_AUTO_SHOUT];

	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnMouseWndSurface(CPoint point);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point = NULL);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDestroy();

	void UpdateShout(int i, CString str);
};
#endif

#endif // !__WND_AUTO_SHOUT_H
