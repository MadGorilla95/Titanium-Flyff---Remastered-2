#include "stdafx.h"
#include "resData.h"
#include "WndPetGlow.h"
#include "DPClient.h"
#include "defineText.h"

extern CDPClient g_DPlay;

#ifdef __PET_GLOW
CWndPetGlowChange::CWndPetGlowChange()
{
    m_pTexture = NULL;
    m_pItemElem = NULL;
    m_pEItemProp = NULL;
    m_nSelected = NULL;
}

CWndPetGlowChange::~CWndPetGlowChange()
{

}

void CWndPetGlowChange::OnMouseWndSurface(CPoint point)
{
}

void CWndPetGlowChange::OnSize(UINT nType, int cx, int cy)
{
    CWndNeuz::OnSize(nType, cx, cy);
}

void CWndPetGlowChange::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}

BOOL CWndPetGlowChange::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
    CItemElem* pTempElem;
    pTempElem = (CItemElem*)g_pPlayer->GetItemId(pShortcut->m_dwId);
    if (pTempElem != NULL)
    {
        m_pItemElem = pTempElem;
        m_pEItemProp = m_pItemElem->GetProp();
        if (m_pEItemProp->dwItemKind3 == IK3_PET)
        {
            m_pItemElem->SetExtra(m_pItemElem->GetExtra() + 1);
            LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_STATIC2);
            if (m_pEItemProp)
                m_pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff);
        }
        else
        {
            g_WndMng.OpenMessageBox(prj.GetText(TID_PET_GLOW_TEXT_03));
        }
    }
    return TRUE;
}

BOOL CWndPetGlowChange::Initialize(CWndBase* pWndParent, DWORD nType)
{
    return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_CHANGE_PETGLOW, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndPetGlowChange::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
    switch (nID)
    {
    case WIDC_BUTTON1:
    {
        if (m_pItemElem != NULL)
        {
            CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
            m_nSelected = pWndListBox->GetCurSel();
            if (m_nSelected > 0 || m_nSelected <= 22)
            {
                g_DPlay.OnChangePetGlow(m_pItemElem->m_dwObjId, m_nSelected);
                m_pItemElem->SetExtra(0);
                Destroy();
            }
        }
        else
            Destroy();
    }
    break;
    case WIDC_BUTTON2:
    {
        if (m_pItemElem != NULL)
        {
            m_pItemElem->SetExtra(0);
            Destroy();
        }
        else
            Destroy();
    }
    break;
    case WTBID_CLOSE:
    {
        if (m_pItemElem != NULL)
        {
            m_pItemElem->SetExtra(0);
            Destroy();
        }
        else
            Destroy();
    }
    break;
    }
    return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndPetGlowChange::OnDraw(C2DRender* p2DRender)
{
    LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_STATIC5);
    if (m_pItemElem != NULL)
    {
        wndCtrl = GetWndCtrl(WIDC_STATIC2);
        if (m_pTexture != NULL)
            m_pTexture->Render(p2DRender, CPoint(wndCtrl->rect.left, wndCtrl->rect.top));
    }
}

void CWndPetGlowChange::OnDestroy()
{
    DeleteDeviceObjects();
}

void CWndPetGlowChange::OnInitialUpdate()
{
    CWndNeuz::OnInitialUpdate();
    CString strDes;
    CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
    strDes = prj.GetText(TID_PET_GLOW_TEXT_02);

#ifdef __AEGON_THEME_SWITCHER
    pText->SetString(strDes, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else //__AEGON_THEME_SWITCHER
    pText->SetString(strDes, 0xFF000000);
#endif //__AEGON_THEME_SWITCHER

    //RestoreDeviceObjects();
    CRect rectRoot = m_pWndRoot->GetLayoutRect();
    CRect rectWindow = GetWindowRect();
    CPoint point(rectRoot.right - rectWindow.Width(), 110);
    Move(point);

    CWndListBox* pListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
    pListBox->AddString(prj.GetText(TID_PET_GLOW_TEXT_04));
    pListBox->AddString(prj.GetText(TID_PET_GLOW_TEXT_05));
    pListBox->AddString(prj.GetText(TID_PET_GLOW_TEXT_06));
    pListBox->AddString(prj.GetText(TID_PET_GLOW_TEXT_07));
    pListBox->AddString(prj.GetText(TID_PET_GLOW_TEXT_08));
    pListBox->AddString(prj.GetText(TID_PET_GLOW_TEXT_09));
    pListBox->AddString(prj.GetText(TID_PET_GLOW_TEXT_10));
    pListBox->AddString(prj.GetText(TID_PET_GLOW_TEXT_11));
    pListBox->AddString(prj.GetText(TID_PET_GLOW_TEXT_12));
    pListBox->AddString(prj.GetText(TID_PET_GLOW_TEXT_13));
    pListBox->AddString(prj.GetText(TID_PET_GLOW_TEXT_14));
    pListBox->AddString(prj.GetText(TID_PET_GLOW_TEXT_15));
    MoveParentCenter();
}
#endif // __PET_GLOW