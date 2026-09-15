#include "stdafx.h"
#include "resData.h"
#include "WndAegonTeleporter.h"
#include "AegonTeleportMng.h"
#include "DPClient.h"
#include "defineText.h"
#include "WndManager.h"
#include "WndManager.h"
#include "Model.h"
#include "ModelMng.h"
#include "ModelObject.h"
#include "ModelGlobal.h"
#include "math.h"
#include "defineObj.h"
#include "defineSound.h"

#ifdef __AEGON_TELEPORTER
extern CDPClient g_DPlay;

////////////////////////////
//////////Shortcut//////////
///////////////////////////
#ifdef __AEGON_TELEPORT_SHORTCUT
CWndTeleporterShortcut::CWndTeleporterShortcut()
{
	m_pTexShortcut = NULL;

	m_nSelected = -1;

}
CWndTeleporterShortcut::~CWndTeleporterShortcut()
{
}
void CWndTeleporterShortcut::OnDraw(C2DRender* p2DRender)
{
	CWndTeleporter* pWndTeleporter = (CWndTeleporter*)GetParentWnd();
	if (!pWndTeleporter)
		return;

	if (pWndTeleporter->m_nSelected != m_nSelected)
	{
		m_nSelected = pWndTeleporter->m_nSelected;
	
		if(m_nSelected >= 0 && m_nSelected < pWndTeleporter->m_vUnitList.size())
			m_pTexShortcut = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_ICON, pWndTeleporter->m_vUnitList[m_nSelected].m_szNameShortcutTex), COLOR_ITEM_BG);
	}

	if (m_pTexShortcut)
	{
		p2DRender->RenderTexture(CPoint(0, 0), m_pTexShortcut);
	}
}
void CWndTeleporterShortcut::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWndTeleporter* pWndTeleporter = (CWndTeleporter*)GetParentWnd();

	m_nSelected = pWndTeleporter->m_nSelected;

	if (pWndTeleporter && m_nSelected >= 0 && m_nSelected < pWndTeleporter->m_vUnitList.size())
	{
		m_GlobalShortcut.m_dwShortcut = SHORTCUT_TELEPORT;
		m_GlobalShortcut.m_pFromWnd = this;
		m_GlobalShortcut.m_pTexture = m_pTexShortcut;
		m_GlobalShortcut.m_dwData = 0;
		m_GlobalShortcut.m_dwId = pWndTeleporter->m_vUnitList[m_nSelected].m_nID;
	}
}

void CWndTeleporterShortcut::OnMouseMove(UINT nFlags, CPoint point)
{
}
void CWndTeleporterShortcut::OnInitialUpdate()
{
}
void CWndTeleporterShortcut::OnMouseWndSurface(CPoint pt)
{
	CRect rect = GetWindowRect();
	if (rect.PtInRect(pt))
	{
		CWndTeleporter* pWndTeleporter = (CWndTeleporter*)GetParentWnd();
		if (pWndTeleporter)
		{
			CWndListBox* pWndListBox = (CWndListBox*)pWndTeleporter->GetDlgItem(WIDC_LISTBOX1);
			if (pWndListBox)
			{
				CPoint pt2 = pt;
				ClientToScreen(&rect);
				ClientToScreen(&pt2);

				CString tmpStr = prj.GetTeleportShortcutString(pWndListBox->GetCurSel());

#ifdef _WIN64
				g_toolTip.PutToolTip(reinterpret_cast<ULONG_PTR>(this), tmpStr, rect, pt2);
#else
				g_toolTip.PutToolTip(reinterpret_cast<DWORD>(this), tmpStr, rect, pt2);
#endif
			}
		}
	}
}
#endif // __AEGON_TELEPORT_SHORTCUT

///////////////////////////
///////LIST WINDOW/////////
///////////////////////////
CWndTeleportList::CWndTeleportList()
{
}

CWndTeleportList::~CWndTeleportList()
{
}

void CWndTeleportList::UpdateList(int nUnitID)
{
	__AEGON_TELEPORT_UNIT* pUnit = CAegonTeleporterMng::GetInstance()->GetTeleUnitByID(nUnitID);
	if (pUnit)
		UpdateList(pUnit);
}
void CWndTeleportList::UpdateList(__AEGON_TELEPORT_UNIT* pUnit)
{
	if (!pUnit)
		return;

	m_vecItem.clear();
	ResetContent();

	m_vecItem = pUnit->m_vItemIDs;
	for (auto item : m_vecItem)
		AddString(" ");
}
void CWndTeleportList::OnDraw(C2DRender* p2DRender)
{
	DWORD	dwColor;
#ifdef __AEGON_THEME_SWITCHER
	dwColor = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
#else
	dwColor = COLOR_BLACK;
#endif

	CWndTeleporter* pWndTeleporter = (CWndTeleporter*)GetParentWnd();
	if (!pWndTeleporter)
		return;
	int				nIndex = 0;
	m_nFontHeight = 36;
	m_nLineSpace = 10;
	int nListFontHeight = GetFontHeight() + m_nLineSpace;

	CPoint point(3, 3);
	point.y -= nListFontHeight * m_wndScrollBar.GetScrollPos();

	int nPage = GetClientRect().Height() / nListFontHeight;
	int nRange = m_vecItem.size();

	if (IsWndStyle(WBS_VSCROLL))
	{
		m_wndScrollBar.SetVisible(TRUE);
		m_wndScrollBar.SetScrollRange(0, nRange);
		m_wndScrollBar.SetScrollPage(nPage);
	}
	else
	{
		m_wndScrollBar.SetVisible(FALSE);
	}

	int nDrawCount = 0;
	int nLine = 0;

	CPoint pt(10, 10);
	if (pWndTeleporter->m_nSelected < 0)
		return;

	for (auto Item = m_vecItem.begin(); Item != m_vecItem.end(); Item++)
	{
		if (nLine < GetScrollPos())
		{
			nLine++;
			continue;
		}
		else
			nLine++;
		CItemElem pItemElem;
		pItemElem.m_dwItemId = *Item;
		pItemElem.SetTexture();

		CRect rect = GetWindowRect();
		CPoint point;
		point.x = 50;
		point.y = 14 + (nIndex)*nListFontHeight;
		p2DRender->TextOut(point.x, point.y, pItemElem.GetName(), dwColor);
		p2DRender->RenderTexture(CPoint(GetClientRect().left + 10, point.y - 9), pItemElem.GetTexture());

		++nIndex;
		++nDrawCount;
		CPoint p1(0, 1 + (nIndex)*nListFontHeight);
		CPoint p2(GetWndRect().right - 20, 1 + (nIndex)*nListFontHeight);

		p2DRender->RenderLine(p1, p2, dwColor);
	}
}
void CWndTeleportList::OnMouseWndSurface(CPoint pt)
{
	CWndTeleporter* pWndTeleporter = (CWndTeleporter*)GetParentWnd();
	int nListFontHeight = GetFontHeight() + m_nLineSpace;
	int nLine = 0;

	if (pWndTeleporter->m_nSelected < 0)
		return;

	int nIndex = 0;
	for (auto Item = m_vecItem.begin(); Item != m_vecItem.end(); Item++)
	{
		if (nLine < GetScrollPos())
		{
			nLine++;
			continue;
		}
		else
			nLine++;
		CItemElem pItemElem;
		pItemElem.m_dwItemId = *Item;

		CRect rect = CRect(0, 3 + (nIndex)*nListFontHeight, GetWndRect().right - 20, 37 + (nIndex)*nListFontHeight);
		if (rect.PtInRect(pt))
		{
			CRect rect2 = rect;
			CPoint pt2 = pt;
			ClientToScreen(&rect2);
			ClientToScreen(&pt2);

			g_WndMng.PutToolTip_Item((CItemBase*)&pItemElem, pt2, &rect2, 0);
		}
		++nIndex;
	}
}
////////////////////////////
//////Teleporter MAIN///////
///////////////////////////
CWndTeleporter::CWndTeleporter(void)
{
	m_nSelected = -1;
	m_pPicTexture = NULL;
	m_pWndTeleportList = NULL;
}
CWndTeleporter::~CWndTeleporter(void)
{
#ifdef __TIMER
	vDun.clear();
#endif //__TIMER
	DeleteDeviceObjects();
}
BOOL CWndTeleporter::Initialize(CWndBase* pWndParent, DWORD dwType)
{
#ifdef __GAMEGUARD
	if (g_pPlayer->GetWorld()->GetID() == WI_WORLD_GUILDWAR || g_pPlayer->GetWorld()->GetID() == WI_WORLD_ARENA || (g_pPlayer->GetWorld()->GetID() >= WI_DUNGEON_SECRET_1 && g_pPlayer->GetWorld()->GetID() <= WI_DUNGEON_SECRET_16)
#ifdef __AEGON_TOURNAMENT
		|| g_pPlayer->GetWorld()->GetID() == AT_TOURNAMENT_MAP
#endif // __AEGON_TOURNAMENT
#ifdef __ROYAL_RUMBLE
		|| g_pPlayer->GetWorld()->GetID() == WI_WORLD_FWC
#endif //__ROYAL_RUMBLE
		)
	{
		g_WndMng.PutString("You can not use the teleporter here!", NULL, COLOR_WARNING);
		return FALSE;
	}
#endif //__GAMEGUARD
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_TELEPORTER, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndTeleporter::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndTeleporter::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndTeleporter::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndTeleporter::OnLButtonDown(UINT nFlags, CPoint point)
{
}
void CWndTeleporter::OnDraw(C2DRender* p2DRender)
{
	if (m_pPicTexture)
	{
		LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_CUSTOM);
		p2DRender->RenderTexture(lpWndCtrl->rect.TopLeft(), m_pPicTexture, g_Option.m_nWindowAlpha);
	}

}
void CWndTeleporter::OnThemeSelectionChanged()
{
	CWndNeuz::OnThemeSelectionChanged();
	m_wndTeleporterShortcut.OnThemeSelectionChanged();
	m_pWndTeleportList->OnThemeSelectionChanged();
}

HRESULT CWndTeleporter::RestoreDeviceObjects()
{
	CWndNeuz::RestoreDeviceObjects();
	return S_OK;
}
HRESULT CWndTeleporter::InvalidateDeviceObjects()
{
	CWndNeuz::InvalidateDeviceObjects();
	return S_OK;
}
HRESULT CWndTeleporter::DeleteDeviceObjects()
{
	CWndNeuz::DeleteDeviceObjects();
	InvalidateDeviceObjects();
	return S_OK;
}
void CWndTeleporter::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();

#ifdef __TIMER
	m_timer = 0;
	g_DPlay.SendHdr(PACKETTYPE_TIMER);
#endif //__TIMER

	CWndListBox* pFakeListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX2);
	if (pFakeListBox)
	{
		pFakeListBox->EnableWindow(FALSE);
		pFakeListBox->SetVisible(FALSE);
	}
	SAFE_DELETE(m_pWndTeleportList);
	m_pWndTeleportList = new CWndTeleportList;
	m_pWndTeleportList->Create(WBS_VSCROLL | GetWndCtrl(WIDC_LISTBOX2)->dwWndStyle, GetWndCtrl(WIDC_LISTBOX2)->rect, this, 1337);
	

#ifdef __AEGON_TELEPORT_SHORTCUT
	LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_CUSTOM1);
	m_wndTeleporterShortcut.Create("g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_CUSTOM1);
	m_wndTeleporterShortcut.AddWndStyle(WBS_NODRAWFRAME);
#endif // __AEGON_TELEPORT_SHORTCUT

	RestoreDeviceObjects();
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();

	FillUnitVector();
	OnSelectionChanged();

}
BOOL CWndTeleporter::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_DBLCLK)
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
		if (pWndListBox->GetCurSel() != -1 || pWndListBox->GetCurSel() > 22)
		{
			if (pWndListBox->m_nIdWnd == nID)
			{
				nID = WIDC_BUTTON1; //Teleport when double clicking
			}
		}
	}
	if (nID == WIDC_LISTBOX1)
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
		if (pWndListBox->GetCurSel() >= 0 && pWndListBox->GetCurSel() < m_vUnitList.size())
		{
			m_nSelected = pWndListBox->GetCurSel();
			if (m_pWndTeleportList)
				m_pWndTeleportList->UpdateList(m_nSelected);
#ifdef __TIMER
			m_timer = GetTimer(m_nSelected);
#endif //__TIMER

			OnSelectionChanged();
		}
	}
	else if (nID == WIDC_BUTTON1)
	{

		if (m_nSelected >= 0 && m_nSelected < m_vUnitList.size())
			g_DPlay.SendTeleportByApp(m_vUnitList[m_nSelected].m_nID);
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#ifdef __TIMER
void CWndTeleporter::OnDungeonTimer(CAr& ar)
{
	ar.ReadContainerPOD(vDun, 50);
}

const DWORD CWndTeleporter::GetTimer(int nSelected)
{

	if (nSelected < 0 || nSelected >= m_vUnitList.size())
		return 0;

	int nWorldID = m_vUnitList[nSelected].m_dwWorldDungeon;
	if (nWorldID == 0 || m_vUnitList[nSelected].m_dwUnitType != 2)
		return 0;
	auto it = std::find_if(vDun.begin(), vDun.end(), [nWorldID](const CoolDungeons& p) {return p.dwWorld == nWorldID; });

	if (it != vDun.end())
	{
		return it->dwCool;
	}

	return 0;
}
#endif //__TIMER

void CWndTeleporter::FillUnitVector()
{
	CWndListBox* pListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);

	vector<__AEGON_TELEPORT_UNIT>* pVec = &(CAegonTeleporterMng::GetInstance()->m_vUnits);
	m_vUnitList.clear();
	if (pVec)
	{
		for (int i = 0; i < pVec->size(); i++)
		{
			if ((*pVec)[i].m_dwTeleporterWnd == 0)
			{
				m_vUnitList.push_back((*pVec)[i]);
			}
		}
	}

	if (pListBox) 
	{
		pListBox->ResetContent();
		for (int i = 0; i < m_vUnitList.size(); i++) 
		{
			CString strListBox;
			strListBox.Format("%s", CAegonTeleporterMng::GetInstance()->GetTeleportString(&m_vUnitList[i]).GetString());
			pListBox->AddString(strListBox);
		}
		pListBox->SetCurSel(0);
		m_nSelected = pListBox->GetCurSel();
	}	
}
void CWndTeleporter::OnSelectionChanged()
{
	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC10);
	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	if (m_nSelected < 0 || m_nSelected >= m_vUnitList.size())
		return;

	CString strFile, strLandName, strDes, strPrice;

	if (m_vUnitList[m_nSelected].m_nCost > 0) 
	{
		strPrice.Format("%d", m_vUnitList[m_nSelected].m_nCost);
		strPrice.Format("%s\n", GetNumberFormatEx(strPrice.GetString()).GetString());
		strDes = strPrice;
		strDes += prj.GetText(m_vUnitList[m_nSelected].m_dwDescrTid);
	}
	else
		strDes = prj.GetText(m_vUnitList[m_nSelected].m_dwDescrTid);

	strFile = m_vUnitList[m_nSelected].m_szNameImage;
	strLandName = m_vUnitList[m_nSelected].m_szNameHeader;


	pStatic->SetTitle(strLandName);

#ifdef __TIMER
	if (m_vUnitList[m_nSelected].m_dwUnitType == 2)
	{
		if (m_timer > 0)
		{
			DWORD dwRemainMin = m_timer / MIN(1);
			strDes.Format(GETTEXT(TID_TELEPORT_COOLTIME), strDes, dwRemainMin);
		}
		else {
			strDes.Format("%s \n#b#cff337d33Cooldown: You can now enter!#nc#nb", strDes.GetString());
		}
	}
#endif //__TIMER

#ifdef __AEGON_THEME_SWITCHER
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	pText->SetString(strDes, dwColorText);
#else
	pText->SetString(strDes, COLOR_BLACK);
#endif

	m_pPicTexture = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_LOADMAP, strFile), COLOR_ITEM_BG, TRUE);

	if(m_pWndTeleportList)
		m_pWndTeleportList->UpdateList(m_nSelected);
}
#if __VER >= 12 // __HEAVEN_TOWER

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndHeavenTower Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndHeavenTower::CWndHeavenTower()
{
	m_pPicTexture = NULL;
	m_pWndTeleportList = NULL;
}

CWndHeavenTower::~CWndHeavenTower()
{
	DeleteDeviceObjects();
}

void CWndHeavenTower::OnDestroy()
{
}

void CWndHeavenTower::OnDraw(C2DRender* p2DRender)
{
	if (m_pPicTexture)
	{
		LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_CUSTOM);
		if (g_Option.m_nWindowAlpha > 200)
			m_pPicTexture->Render(p2DRender, lpWndCtrl->rect.TopLeft(), g_Option.m_nWindowAlpha - 55);
		else
			m_pPicTexture->Render(p2DRender, lpWndCtrl->rect.TopLeft(), g_Option.m_nWindowAlpha);
	}
}

void CWndHeavenTower::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	FillUnitVector();

	InitTeleportList();

	OnSelChanged();

	MoveParentCenter();
}
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndHeavenTower::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_HEAVEN_TOWER_NEW, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndHeavenTower::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_BUTTON1:
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
		int nCurSel = pWndListBox->GetCurSel();
		if (nCurSel >=0 && nCurSel < m_vUnitList.size())
		{
			int floor = nCurSel + 1;
			if (floor == 6)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_HEAVETOWER_FLOOR06_MSG), NULL, COLOR_ERROR);
			}
			else if (floor > 0)
			{
				g_DPlay.SendTeleportToHeavenTower(m_vUnitList[nCurSel].m_nID);
				Destroy();
			}

		}
		break;
	}
	case WIDC_LISTBOX1:
		OnSelChanged();
		break;
	}


	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndHeavenTower::OnSelChanged()
{
	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC10);
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	__AEGON_TELEPORT_UNIT* pUnit = NULL;

	if (pWndList && pText)
	{
		int nCurSel = pWndList->GetCurSel();
		if (nCurSel < 0 || nCurSel >= m_vUnitList.size())
			return;
		pUnit = &m_vUnitList[nCurSel];

		CString strFinal = prj.GetText(pUnit->m_dwDescrTid);
		CString strPrice;

		if (pUnit->m_nCost > 0)
		{
			strPrice.Format("%d", pUnit->m_nCost);
			strPrice.Format("%s", GetNumberFormatEx(strPrice.GetString()).GetString());
		}
		else
		{
			strPrice.Format("???");
		}


		strFinal.Format(prj.GetText(pUnit->m_dwDescrTid), strPrice);

		pText->SetString(strFinal);

		m_pPicTexture = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_LOADMAP, pUnit->m_szNameImage), COLOR_ITEM_BG);

		if (m_pWndTeleportList)
			m_pWndTeleportList->UpdateList(pUnit);

		pStatic->SetTitle(pUnit->m_szNameHeader);
	}

}

void CWndHeavenTower::InitTeleportList()
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);

	SAFE_DELETE(m_pWndTeleportList);
	m_pWndTeleportList = new CWndTeleportList;
	m_pWndTeleportList->Create(WBS_VSCROLL, GetWndCtrl(WIDC_LISTBOX2)->rect, this, 1337);
}

HRESULT CWndHeavenTower::DeleteDeviceObjects()
{
	CWndNeuz::DeleteDeviceObjects();
	InvalidateDeviceObjects();
	return S_OK;
}
void CWndHeavenTower::FillUnitVector()
{
	CWndListBox* pListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);

	vector<__AEGON_TELEPORT_UNIT>* pVec = &(CAegonTeleporterMng::GetInstance()->m_vUnits);
	m_vUnitList.clear();
	if (pVec)
	{
		for (int i = 0; i < pVec->size(); i++)
		{
			if ((*pVec)[i].m_dwTeleporterWnd == 1)
			{
				m_vUnitList.push_back((*pVec)[i]);
			}
		}
	}

	if (pListBox)
	{
		pListBox->ResetContent();
		for (int i = 0; i < m_vUnitList.size(); i++)
		{
			CString strListBox;
			strListBox.Format("%s", CAegonTeleporterMng::GetInstance()->GetTeleportString(&m_vUnitList[i]).GetString());
			pListBox->AddString(strListBox);
		}
		pListBox->SetCurSel(0);
	}
}
#endif //__HEAVEN_TOWER

#endif