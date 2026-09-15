#pragma once
#ifndef __WNDAEGSYSSH__H
#define __WNDAEGSYSSH__H

#ifdef __AEGON_ADVANCED_ANNOUNCEMENT
class CWndAegonSystemShout : public CWndNeuz
{
private:
	CWndEdit* m_pWndTextField;
	CWndComboBox* m_pWndComboBoxSelectImage;
	CWndComboBox* m_pWndComboBoxSelectFont;

	CWndStatic* m_pWndStaticComboBoxHeader;
	CWndStatic* m_pWndStaticPreview;

	CWndButton* m_pWndCheckName;
	CWndButton* m_pWndCheckAuth;

	CTexture* m_bgTexture;
	float m_texResizeFactor;

	CD3DFont* m_CaptionFont;

public:

	CWndAegonSystemShout();
	~CWndAegonSystemShout();




	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual void OnDestroy();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);

	void SetShoutString(char* szString);
	void OnImageSelChanged();
	void FitImageSize();
	void OnFontSelChanged();

};
#endif // __AEGON_ADVANCED_ANNOUNCEMENT
#endif // __WNDAEGSYSSH__H