#pragma once

#ifdef __SYS_GUILDCOLOR
class CWndGuildColor : public CWndNeuz
{
public:
	CWndGuildColor();
	virtual ~CWndGuildColor();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void ReSetBar(FLOAT a, FLOAT b, FLOAT c);
	virtual void OnMouseWndSurface(CPoint point);
private:
	CRect			m_ColorRect[3];
	FLOAT			m_fColor[3];
	CPoint			 m_ColorScrollBar[3];
	CPoint			 m_OriginalColorScrollBar[3];
	int			nChoiceBar;
	bool		m_bLBClick;
	CTexture         m_Texture;

};
#endif //__SYS_GUILDCOLOR