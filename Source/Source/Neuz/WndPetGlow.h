#pragma once

#ifndef __WNDPETGLOW__H
#define __WNDPETGLOW__H
#ifdef __PET_GLOW
class CWndPetGlowChange : public CWndNeuz
{
private:
    int m_nSelected;
    CItemElem* m_pItemElem;
    ItemProp* m_pEItemProp;
    CTexture* m_pTexture;

public:
    CWndPetGlowChange();
    virtual ~CWndPetGlowChange();
    virtual BOOL    OnDropIcon(LPSHORTCUT pShortcut, CPoint point);
    virtual BOOL    Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
    virtual BOOL    OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
    virtual void    OnLButtonDblClk(UINT nFlags, CPoint point);
    virtual void    OnDraw(C2DRender* p2DRender);
    virtual void    OnDestroy();
    virtual void    OnInitialUpdate();
    virtual void    OnMouseWndSurface(CPoint point);
    virtual void    OnSize(UINT nType, int cx, int cy);
};
#endif // __PET_GLOW
#endif // __WNDPETGLOW__H