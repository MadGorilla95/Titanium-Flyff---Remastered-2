#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndManager.h"
#include "WndViewInv.h"
#include "defineObj.h"
#include "DefineItem.h"
#include <afxdisp.h>
#include "defineNeuz.h"
#include "DialogMsg.h"

#include "DPClient.h"
#include "MsgHdr.h"
extern	CDPClient	g_DPlay;

#if __VER >= 15 // __PETVIS
#include "definesound.h"
#endif


#ifdef __AEGON_VIEW_INV
// view bag window
CWndViewBag::CWndViewBag()
{
	m_viewMover = NULL;
}
CWndViewBag::~CWndViewBag()
{
}
BOOL CWndViewBag::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_VIEW_BAG, 0, CPoint(0, 0), pWndParent);
}
void CWndViewBag::OnInitialUpdate()
{
	m_ppMover = &m_viewMover;
	CWndBagEx::OnInitialUpdate();	
}
void CWndViewBag::OnLButtonUp(UINT nFlags, CPoint point) 
{
}
void CWndViewBag::OnLButtonDown(UINT nFlags, CPoint point) 
{
}
BOOL CWndViewBag::OnDropIcon(LPSHORTCUT pShortcut, CPoint point) 
{
	return CWndBase::OnDropIcon(pShortcut, point);
}
BOOL CWndViewBag::OnDropIconOnItemCtrl(LPSHORTCUT pShortcut, int nContainerField)
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();

	if (pWndFrame == NULL)
		return FALSE;

	if (pWndFrame->GetWndId() == APP_INVENTORY)
	{
		CItemElem* pItemElem = (CItemElem*)g_pPlayer->GetItemId(pShortcut->m_dwId);
		if (pItemElem)
		{
			int nContainer = 1;

			if (nContainerField >= 0 && nContainerField < 3)
				g_DPlay.SendViewInventoryCommand(pItemElem, 3, nContainer, nContainerField);
		}
	}

}
BOOL CWndViewBag::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
// main window
CWndViewInventoryMain::CWndViewInventoryMain()
{
}
CWndViewInventoryMain::~CWndViewInventoryMain()
{
	SAFE_DELETE(g_WndMng.m_pWndViewInventoryCtrl);
	SAFE_DELETE(g_WndMng.m_pWndViewBag);
}
void CWndViewInventoryMain::DoInventoryView(LPCTSTR viewName, int nContainer)
{
	CWndComboBox* pWndComboPlayer = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	CWndStatic* pWndInfo = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	if (viewName && strlen(viewName) > 0)
	{
		pWndComboPlayer->SetString(viewName);
	}

	CWndComboBox* pWndComboContainer = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);

	LPCTSTR strPlayer = pWndComboPlayer->GetString();
	u_long uidPlayer;

	uidPlayer = CPlayerDataCenter::GetInstance()->GetPlayerId((char*)strPlayer);
	CMover* pMover = prj.GetUserByID(uidPlayer);

	if (nContainer >= 0 && nContainer < pWndComboContainer->m_wndListBox.GetCount())
	{
		pWndComboContainer->SetCurSel(nContainer);
	}
	else
		nContainer = pWndComboContainer->GetCurSel();

	if (nContainer == 2)
	{
		g_WndMng.PutString("bank view coming soon", 0, COLOR_ERROR);
		return;
	}

	g_DPlay.SendViewInventory(uidPlayer, nContainer);
	pWndInfo->SetTitle("Command sent");
}
void CWndViewInventoryMain::OnInventoryView(CMover* pViewMover)
{
	if (!g_WndMng.m_pWndViewInventoryCtrl)
	{
		g_WndMng.m_pWndViewInventoryCtrl = new CWndViewInventoryCtrl;
		g_WndMng.m_pWndViewInventoryCtrl->m_viewMover = pViewMover;
		g_WndMng.m_pWndViewInventoryCtrl->Initialize(NULL, APP_VIEW_INVENTORY_CTRL);
		g_WndMng.m_pWndViewInventoryCtrl->MoveParentCenter();

	}
	else
	{
		if (!g_WndMng.m_pWndViewInventoryCtrl->IsDestroy())
		{
			g_WndMng.m_pWndViewInventoryCtrl->m_viewMover = pViewMover;
			g_WndMng.m_pWndViewInventoryCtrl->UpDateModel();
		}
	}
}
void CWndViewInventoryMain::OnBagView(CMover* pViewMover)
{
	if (!g_WndMng.m_pWndViewBag)
	{
		g_WndMng.m_pWndViewBag = new CWndViewBag;
		g_WndMng.m_pWndViewBag->m_viewMover = pViewMover;
		g_WndMng.m_pWndViewBag->Initialize(NULL, APP_VIEW_BAG);
		g_WndMng.m_pWndViewBag->MoveParentCenter();

	}
	else
	{
		if (!g_WndMng.m_pWndViewBag->IsDestroy())
		{
			g_WndMng.m_pWndViewBag->m_viewMover = pViewMover;
		}
	}
}
void CWndViewInventoryMain::OnBankView(CMover* pViewMover)
{
}
void CWndViewInventoryMain::OnDraw(C2DRender* p2DRender)
{


}

void CWndViewInventoryMain::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	SAFE_DELETE(g_WndMng.m_pWndViewInventoryCtrl);
	SAFE_DELETE(g_WndMng.m_pWndViewBag);

	CWndComboBox* pWndComboPlayer = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	pWndComboPlayer->SetTabStop(TRUE);

	CWndComboBox* pWndComboContainer = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);
	if (pWndComboContainer) 
	{
		pWndComboContainer->AddWndStyle(EBS_READONLY);
		pWndComboContainer->AddString("Inventory");
		pWndComboContainer->AddString("Bag");
		pWndComboContainer->AddString("Bank");
		pWndComboContainer->SetCurSel(0);

	}
	

	Move(0, (g_WndMng.GetClientRect().Height() / 2) - GetClientRect().Height());
}

BOOL CWndViewInventoryMain::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID) 
	{
	case WIDC_BUTTON1:
		DoInventoryView();
		break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

BOOL CWndViewInventoryMain::Initialize(CWndBase* pWndParent, DWORD)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_VIEW_INVENTORY, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndViewInventoryMain::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndViewInventoryMain::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndViewInventoryMain::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndViewInventoryMain::OnLButtonDown(UINT nFlags, CPoint point)
{
}
HRESULT CWndViewInventoryMain::RestoreDeviceObjects()
{
	CWndNeuz::RestoreDeviceObjects();
	return S_OK;
}
HRESULT CWndViewInventoryMain::InvalidateDeviceObjects()
{
	CWndNeuz::InvalidateDeviceObjects();
	return S_OK;
}
HRESULT CWndViewInventoryMain::DeleteDeviceObjects()
{
	CWndNeuz::DeleteDeviceObjects();
	InvalidateDeviceObjects();
	return S_OK;
}
void CWndViewInventoryMain::OnDestroy(void)
{
	if (g_WndMng.m_pWndViewInventoryCtrl)
		g_WndMng.m_pWndViewInventoryCtrl->Destroy();

	if (g_WndMng.m_pWndViewBag)
		g_WndMng.m_pWndViewBag->Destroy();

	DeleteDeviceObjects();
	Destroy();
}


/////////////CTRL//////////////////
CWndViewInventoryCtrl::CWndViewInventoryCtrl()
{
	m_viewMover = NULL;

#ifdef __INVENTORY_SEARCH
	m_pWndSearch = nullptr;
#endif // __INVENTORY_SEARCH
	m_bReport = FALSE;
	m_pSelectItem = NULL;
	m_pWndConfirmBuy = NULL;

	m_bIsUpgradeMode = FALSE;
	m_pUpgradeItem = NULL;
	m_pUpgradeMaterialItem = NULL;
	m_pSfxUpgrade = NULL;
	m_dwEnchantWaitTime = 0xffffffff;
	m_TexRemoveItem = NULL;

	m_pModel = NULL;
	m_OldPos = CPoint(0, 0);

	memset(m_InvenRect, 0, sizeof(CRect) * MAX_HUMAN_PARTS);
#if __VER >= 9 // __CSC_VER9_1
	m_pWndRemoveJewelConfirm = NULL;
	m_bRemoveJewel = FALSE;
#endif //__CSC_VER9_1

#ifdef __AEGON_THEME_SWITCHER
	m_rectRenderChar = CRect(0, 0, 0, 0);
	m_rectDeleteButt = CRect(0, 0, 0, 0);
#endif // __AEGON_THEME_SWITCHER
}
CWndViewInventoryCtrl::~CWndViewInventoryCtrl()
{
	SAFE_DELETE(m_pModel);
	SAFE_DELETE(m_pWndConfirmBuy);

	if (m_pSfxUpgrade)
	{
		m_pSfxUpgrade->Delete();
		m_pSfxUpgrade = NULL;
	}
#if __VER >= 9 // __CSC_VER9_1
	SAFE_DELETE(m_pWndRemoveJewelConfirm);
#endif //__CSC_VER9_1
}

void CWndViewInventoryCtrl::UpdateParts()
{
	if (IsInvalidObj(m_viewMover))
		return;
	CMover::UpdateParts(m_viewMover->GetSex(), m_viewMover->m_dwSkinSet, m_viewMover->m_dwFace, m_viewMover->m_dwHairMesh, m_viewMover->m_dwHeadMesh, m_viewMover->m_aEquipInfo, m_pModel, &m_viewMover->m_Inventory);
}

void CWndViewInventoryCtrl::OnMouseWndSurface(CPoint point)
{
	if (IsInvalidObj(m_viewMover))
		return;
	int nTemp = 0;
	for (int i = 2; i < MAX_HUMAN_PARTS; i++)
	{
		CItemBase* pItemBase = NULL;

		pItemBase = m_viewMover->GetEquipItem(i);

		CRect DrawRect = m_InvenRect[i];

		CPoint point = GetMousePoint();
		// 툴팁
		if (DrawRect.PtInRect(point))
		{
			nTemp = i;

			if (i == PARTS_LWEAPON)
			{
				pItemBase = m_viewMover->GetEquipItem(PARTS_SHIELD);

				if (pItemBase)
				{
					nTemp = PARTS_SHIELD;
				}
				else
				{
					pItemBase = m_viewMover->GetEquipItem(PARTS_LWEAPON);

					if (pItemBase == NULL)
						pItemBase = m_viewMover->GetEquipItem(PARTS_RWEAPON);
				}
			}

			if (pItemBase)
			{
				CPoint point2 = point;
				ClientToScreen(&point2);
				ClientToScreen(&DrawRect);

				// 장비창에 있는것 툴팁
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
				g_toolTip.SetSubToolTipNumber(0);
#endif // __IMPROVE_SYSTEM_VER15
				g_WndMng.PutToolTip_Item(pItemBase, point2, &DrawRect, APP_VIEW_INVENTORY_CTRL);
				break;
			}
			else
			{
				// 장비창에 없는것 툴팁

				CPoint point2 = point;
				ClientToScreen(&point2);
				ClientToScreen(&DrawRect);

				CString strText;

				switch (nTemp)
				{
				case PARTS_UPPER_BODY:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_SUIT);
					break;
				case PARTS_HAND:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_GAUNTLET);
					break;
				case PARTS_FOOT:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_BOOTS);
					break;
				case PARTS_CAP:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_HELMET);
					break;
				case PARTS_CLOAK:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLO);
					break;
				case PARTS_LWEAPON:
				case PARTS_RWEAPON:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_WEAPON);
					break;
				case PARTS_SHIELD:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_GUARD);
					break;
				case PARTS_MASK:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_MASK);
					break;
				case PARTS_RIDE:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_RID);
					break;
				case PARTS_NECKLACE1:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_NECKLACE);
					break;
				case PARTS_RING1:
				case PARTS_RING2:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_RING);
					break;
				case PARTS_EARRING1:
				case PARTS_EARRING2:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_EARRING);
					break;
				case PARTS_BULLET:
#ifdef __SYS_GLYPHS
					strText = "Glyph";
#else //__SYS_GLYPHS
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_BULLET);
#endif //__SYS_GLYPHS
					break;
				case PARTS_HAT:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLOTHES_01);
					break;
				case PARTS_CLOTH:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLOTHES_02);
					break;
				case PARTS_GLOVE:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLOTHES_03);
					break;
				case PARTS_BOOTS:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLOTHES_04);
					break;
				}

				g_toolTip.PutToolTip(100000, strText, DrawRect, point2);
				break;
			}
			//			break;
		}
	}
}


void CWndViewInventoryCtrl::OnDraw(C2DRender* p2DRender)
{
	if (IsInvalidObj(m_viewMover))
		return;
	CMover* pMover = m_viewMover;
	if (!pMover)
		return;

#ifdef __AEGON_INGAME_DP
	CWndStatic* pWndInvCash = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	if (pWndInvCash)
	{
		CString total;
		CString cash;
		cash.Format("%d", m_viewMover->GetIngameDP());
		total.Format("%s DP", GetNumberFormatEx(cash.GetString()).GetString());
		pWndInvCash->SetTitle(total);
		pWndInvCash->m_dwColor = 0xFFCC8b00;
	}
#endif //__AEGON_INGAME_DP

	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;

	viewport.X = p2DRender->m_ptOrigin.x + 5;
	viewport.Y = p2DRender->m_ptOrigin.y + 5;
	viewport.Width = p2DRender->m_clipRect.Width() - 100;
	viewport.Height = 182;
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;

	CString strGoldNum;
	CWndStatic* pGoldNum = (CWndStatic*)GetDlgItem(WIDC_GOLD_NUM);
	CWndTrade* pWndTrade = (CWndTrade*)g_WndMng.GetWndBase(APP_TRADE);
	strGoldNum.Format(_T("%d"), m_viewMover->GetGold());


#ifdef __AEGON_THEME_SWITCHER

	if (m_TexRemoveItem)
	{
		CPoint point = GetMousePoint();
		float fScal = 1.0f;
		if (m_rectDeleteButt.PtInRect(point))
		{
			//fScal = 1.2f;
			CPoint point2 = point;
			CRect DrawRect = m_rectDeleteButt;
			ClientToScreen(&point2);
			ClientToScreen(&DrawRect);
			g_toolTip.PutToolTip(100, prj.GetText(TID_GAME_INVEONTORY_REMOVE_TOOL), DrawRect, point2, 0);
		}
		m_TexRemoveItem->Render(p2DRender, m_rectDeleteButt.TopLeft(), CPoint(m_rectDeleteButt.Size().cx, m_rectDeleteButt.Size().cy), 255, fScal, fScal);
	}
#else // __AEGON_THEME_SWITCHER
	LPWNDCTRL lpWndCtrl1 = GetWndCtrl(WIDC_CUSTOM21);
	if (lpWndCtrl1 && m_TexRemoveItem)
	{
		CPoint point = GetMousePoint();
		float fScal = 1.0f;
		if (lpWndCtrl1->rect.PtInRect(point))
		{
			fScal = 1.2f;
			CPoint point2 = point;
			CRect DrawRect = lpWndCtrl1->rect;
			ClientToScreen(&point2);
			ClientToScreen(&DrawRect);
			g_toolTip.PutToolTip(100, prj.GetText(TID_GAME_INVEONTORY_REMOVE_TOOL), DrawRect, point2, 0);
		}
		if (m_TexRemoveItem)
			m_TexRemoveItem->Render(p2DRender, lpWndCtrl1->rect.TopLeft(), CPoint(27, 27), 255, fScal, fScal);
	}
#endif // __AEGON_THEME_SWITCHER

	pGoldNum->SetTitle(strGoldNum);

	DWORD dwColor1 = D3DCOLOR_ARGB(100, 0, 0, 0);//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB(255, 240, 240, 240);//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB(100, 200, 200, 200);//D3DCOLOR_TEMP( 255,  80,  80, 120 );//

	for (int i = 2; i < MAX_HUMAN_PARTS; i++)
	{
		DWORD dwAlpha = 255;
		CItemBase* pItemBase = m_viewMover->GetEquipItem(i);
		if (i == PARTS_LWEAPON)		// 왼손무기 그릴타이밍일때
		{
			CItemBase* pRWeapon = m_viewMover->GetEquipItem(PARTS_RWEAPON);		// 오른손 무기를 꺼내보고
			if (pRWeapon && pRWeapon->GetProp()->dwHanded == HD_TWO)	// 투핸드 무기면
			{
				pItemBase = pRWeapon;	// 오른손무기랑 같은걸 그리자.
				dwAlpha = 100;
			}
		}

		FLOAT sx = 1.0f;
		FLOAT sy = 1.0f;

		CPoint cpAdd = CPoint(6, 6);
		CRect DrawRect = m_InvenRect[i];

		if (i >= PARTS_NECKLACE1 && i <= PARTS_EARRING2)
		{
			cpAdd = CPoint(0, 0);

			sx = 0.8f;
			sy = 0.8f;
		}
		else
			if (i >= PARTS_HAT && i <= PARTS_BOOTS)
			{
				cpAdd = CPoint(0, 0);

				sx = 0.9f;
				sy = 0.9f;
			}


		if (pItemBase && pItemBase->GetTexture())
		{
#ifdef __HIDE_FASHION
			if (i == PARTS_CLOAK)
			{
				if (!m_awndCheckHide[4].IsVisible())
					m_awndCheckHide[4].SetVisible(TRUE);
			}
			else if (i >= PARTS_HAT && i <= PARTS_BOOTS)
			{
				if (!m_awndCheckHide[i - PARTS_HAT].IsVisible())
					m_awndCheckHide[i - PARTS_HAT].SetVisible(TRUE);

				if (m_viewMover->m_abHideCoat[i - PARTS_HAT])
					dwAlpha = 100;
			}
			else if (i == PARTS_MASK)
			{
				CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK2);
				if (!pWndCheck->IsVisible())
					pWndCheck->SetVisible(TRUE);
			}
#endif // __HIDE_FASHION

			if (((CItemElem*)pItemBase)->IsFlag(CItemElem::expired))
			{
				pItemBase->GetTexture()->Render2(p2DRender, DrawRect.TopLeft() + cpAdd, D3DCOLOR_XRGB(255, 100, 100), sx, sy);
			}
			else
			{
				pItemBase->GetTexture()->Render2(p2DRender, DrawRect.TopLeft() + cpAdd, D3DCOLOR_ARGB(dwAlpha, 255, 255, 255), sx, sy);
			}

			CItemElem* pItemElem = (CItemElem*)pItemBase;

#ifdef __SYS_LOCK_ITEM
			if (pItemElem->IsItemLocked())
			{
				CTexture* LockTexture;
				CPoint cpLock = CPoint(26, 6);
				LockTexture = m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_THEME, "Lock.png"), COLOR_ITEM_BG);
				LockTexture->Render2(p2DRender, DrawRect.TopLeft() + cpLock, D3DCOLOR_XRGB(255, 255, 255), sx, sy);
			}
#endif // __SYS_LOCK_ITEM

#ifdef __WEAPON_RARITY
			WeaponRarity* nWeaponRarity = prj.m_nWeaponRarity.GetAt(pItemElem->GetWeaponRarity());
			CTexture* RarityTexture = NULL;
			if (nWeaponRarity && pItemElem->GetWeaponRarity() > 0)
			{
				if (pItemElem->GetWeaponRarity() == 0) // Common 
				{
					RarityTexture = m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_ITEM, "Rare-Item-Icons_0000s_0005_1.png"), COLOR_ITEM_BG);
				}
				else if (pItemElem->GetWeaponRarity() == 1) // Precious
				{
					RarityTexture = m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_ITEM, "Rare-Item-Icons_0000s_0004_2.png"), COLOR_ITEM_BG);
				}
				else if (pItemElem->GetWeaponRarity() == 2) // Rare
				{
					RarityTexture = m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_ITEM, "Rare-Item-Icons_0000s_0002_4.png"), COLOR_ITEM_BG);
				}
				else if (pItemElem->GetWeaponRarity() == 3) // Epic
				{
					RarityTexture = m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_ITEM, "Rare-Item-Icons_0000s_0000_6.png"), COLOR_ITEM_BG);
				}
				else if (pItemElem->GetWeaponRarity() == 4) // Legendary
				{
					RarityTexture = m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_ITEM, "Rare-Item-Icons_0000s_0003_3.png"), COLOR_ITEM_BG);
				}
				else if (pItemElem->GetWeaponRarity() == 5) // Mystique
				{
					RarityTexture = m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_ITEM, "Rare-Item-Icons_0000s_0001_5.png"), COLOR_ITEM_BG);
				}
				if (RarityTexture)
					RarityTexture->Render2(p2DRender, DrawRect.TopLeft() + cpAdd, D3DCOLOR_XRGB(255, 255, 255), sx, sy);
			}
#endif // __WEAPON_RARITY

			if (pItemElem->GetProp()->dwPackMax > 1)		// 묶음 아이템이냐?
			{
				short nItemNum = pItemElem->m_nItemNum;

				TCHAR szTemp[32];
				_stprintf(szTemp, "%d", nItemNum);
				CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
				int x = DrawRect.left;
				int y = DrawRect.top;

#ifdef __LEESE_RENDER_ITEMCOUNT
				CD3DFont* pOldFont = p2DRender->GetFont();
				p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemCount);

				m_p2DRender->TextOut(x + 42 - size.cx, y + 42 - size.cy, szTemp, 0xFF000000);
				m_p2DRender->TextOut(x + 41 - size.cx, y + 41 - size.cy, szTemp, 0xFFffb912);

				p2DRender->SetFont(pOldFont);
#else //__LEESE_RENDER_ITEMCOUNT
				m_p2DRender->TextOut(x + 42 - size.cx, y + 42 - size.cy, szTemp, 0xff0000ff);
				m_p2DRender->TextOut(x + 41 - size.cx, y + 41 - size.cy, szTemp, 0xffb0b0f0);
#endif //__LEESE_RENDER_ITEMCOUNT
			}
		}
#ifdef __HIDE_FASHION
		else
		{
			if (i == PARTS_CLOAK)
			{
				if (m_awndCheckHide[4].IsVisible())
					m_awndCheckHide[4].SetVisible(FALSE);
			}
			else if (i >= PARTS_HAT && i <= PARTS_BOOTS)
			{
				if (m_awndCheckHide[i - PARTS_HAT].IsVisible())
					m_awndCheckHide[i - PARTS_HAT].SetVisible(FALSE);
			}
			if (i == PARTS_MASK)
			{
				CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK2);
				if (pWndCheck->IsVisible())
					pWndCheck->SetVisible(FALSE);
			}
			}
#endif // __HIDE_FASHION
		}

	if (IsInvalidObj(m_viewMover) || m_pModel == NULL)
		return;

	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

	pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_ARGB(255, 255, 255, 255));

	CRect rect = GetClientRect();

	// 뷰포트 세팅 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot;
	D3DXMATRIXA16 matTrans;

	// 카메라 
	D3DXMATRIX  matView;
	D3DXVECTOR3 vecLookAt(0.0f, 0.0f, 3.0f);
	D3DXVECTOR3 vecPos(0.0f, 0.7f, -3.5f);

	D3DXMatrixLookAtLH(&matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

#ifdef __YENV
	D3DXVECTOR3 vDir(0.0f, 0.0f, 1.0f);
	SetLightVec(vDir);
#endif //__YENV

	{

#ifdef __AEGON_THEME_SWITCHER
		viewport.X = p2DRender->m_ptOrigin.x + m_rectRenderChar.left;
		viewport.Y = p2DRender->m_ptOrigin.y + m_rectRenderChar.top;
		viewport.Width = m_rectRenderChar.Width();
		viewport.Height = m_rectRenderChar.Height();
#else // __AEGON_THEME_SWITCHER
		LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM20);

		viewport.X = p2DRender->m_ptOrigin.x + lpFace->rect.left;
		viewport.Y = p2DRender->m_ptOrigin.y + lpFace->rect.top;
		viewport.Width = lpFace->rect.Width();
		viewport.Height = lpFace->rect.Height();
#endif // __AEGON_THEME_SWITCHER

		viewport.MinZ = 0.0f;
		viewport.MaxZ = 1.0f;
		pd3dDevice->SetViewport(&viewport);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0);

		D3DXMATRIX matProj;
		D3DXMatrixIdentity(&matProj);
		FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;
		/*
				D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
				pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
		*/
		FLOAT fov = D3DX_PI / 4.0f;//796.0f;
		FLOAT h = cos(fov / 2) / sin(fov / 2);
		FLOAT w = h * fAspect;
		D3DXMatrixOrthoLH(&matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane);
		pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

		D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matRot);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);

		D3DXMatrixRotationY(&matRot, D3DXToRadian(m_fRot));

		D3DXMatrixScaling(&matScale, 1.2f, 1.2f, 1.2f);
		D3DXMatrixTranslation(&matTrans, 0.0f, -0.6f, 0.0f);

		D3DXMatrixMultiply(&matWorld, &matWorld, &matScale);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matRot);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);
		pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

		// 랜더링 
		pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);//m_bViewLight );

		::SetLight(FALSE);
		::SetFog(FALSE);
		SetDiffuse(1.0f, 1.0f, 1.0f);
		SetAmbient(1.0f, 1.0f, 1.0f);

		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = m_viewMover->m_fHairColorR;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = m_viewMover->m_fHairColorG;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = m_viewMover->m_fHairColorB;

		D3DXVECTOR4 vConst(1.0f, 1.0f, 1.0f, 1.0f);
#ifdef __YENV
		g_Neuz.m_pEffect->SetVector(g_Neuz.m_hvFog, &vConst);
#else //__YENV						
		pd3dDevice->SetVertexShaderConstantF(95, (float*)&vConst, 1);
#endif //__YENV
		::SetTransformView(matView);
		::SetTransformProj(matProj);

		if (IsValidObj(m_viewMover))
			m_viewMover->OverCoatItemRenderCheck(m_pModel);


#ifdef __AEGON_FIXES
		m_pModel->SetEffectOff(PARTS_HAIR, XE_HIDE);
		m_pModel->SetEffectOff(PARTS_HEAD, XE_HIDE);
#endif

		CItemElem* pItemElem = m_viewMover->GetEquipItem(PARTS_CAP);
		if (pItemElem)
		{
			O3D_ELEMENT* pElement = NULL;
#ifdef __LOOKCHANGE
			ItemProp* pItemProp = pItemElem->IsChangedLook() ? prj.GetItemProp(pItemElem->GetLook()) : pItemElem->GetProp();
#else 
			ItemProp* pItemProp = pItemElem->GetProp();
#endif 
			if (pItemProp && pItemProp->dwBasePartsIgnore != -1)
			{
				pElement = m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
			}

			CItemElem* pItemElemOvercoat = m_viewMover->GetEquipItem(PARTS_HAT);

			if (pItemElemOvercoat
#ifdef __AEGON_FIXES
				&& !m_viewMover->m_abHideCoat[0]
#endif
				)
			{
				if (!(pItemElemOvercoat->IsFlag(CItemElem::expired)))
				{
#ifdef __LOOKCHANGE
					ItemProp* pItemPropOC = pItemElemOvercoat->IsChangedLook() ? prj.GetItemProp(pItemElemOvercoat->GetLook()) : pItemElemOvercoat->GetProp();
#else //__LOOKCHANGE
					ItemProp* pItemPropOC = pItemElemOvercoat->GetProp();
#endif //__LOOKCHANGE

#ifdef __LNB_ANIMATED_HATS
					if (m_viewMover->GainAniHatInfo() || pItemPropOC->dwItemKind3 == IK3_ANIHAT)
					{
						ItemProp* aniHatProp = pItemElemOvercoat->GetPropA();
						if (aniHatProp && aniHatProp->dwBasePartsIgnore != -1)
						{
							if (aniHatProp->dwBasePartsIgnore != PARTS_HEAD)
							{
								m_pModel->SetEffectOff(PARTS_HAIR, XE_HIDE);
							}
							m_pModel->SetEffect(aniHatProp->dwBasePartsIgnore, XE_HIDE);
						}
					}
					else
#endif // __LNB_ANIMATED_HATS
					if (pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1)
					{
						if (pItemPropOC->dwBasePartsIgnore == PARTS_HEAD)
							m_pModel->SetEffect(PARTS_HAIR, XE_HIDE);

						m_pModel->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE);
					}
					else
					{
						if (pElement)
							pElement->m_nEffect &= ~XE_HIDE;
					}
				}
				else
				{
					if (pItemProp && pItemProp->dwBasePartsIgnore != -1)
					{
						m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
					}

					m_pModel->SetEffectOff(PARTS_HEAD, XE_HIDE);
				}
			}
			}
		else
		{
			CItemElem* pItemElemOvercoat = m_viewMover->GetEquipItem(PARTS_HAT);

			if (pItemElemOvercoat)
			{
				if (!(pItemElemOvercoat->IsFlag(CItemElem::expired)))
				{
#ifdef __LOOKCHANGE
					ItemProp* pItemPropOC = pItemElemOvercoat->IsChangedLook() ? prj.GetItemProp(pItemElemOvercoat->GetLook()) : pItemElemOvercoat->GetProp();
#else //__LOOKCHANGE
					ItemProp* pItemPropOC = pItemElemOvercoat->GetProp();
#endif //__LOOKCHANGE

#ifdef __LNB_ANIMATED_HATS
					if (m_viewMover->GainAniHatInfo() || pItemPropOC->dwItemKind3 == IK3_ANIHAT)
					{
						ItemProp* aniHatProp = pItemElemOvercoat->GetPropA();
						if (aniHatProp && aniHatProp->dwBasePartsIgnore != -1)
						{
							if (aniHatProp->dwBasePartsIgnore != PARTS_HEAD)
							{
								m_pModel->SetEffectOff(PARTS_HAIR, XE_HIDE);
							}
							m_pModel->SetEffect(aniHatProp->dwBasePartsIgnore, XE_HIDE);
						}
					}
					else
#endif // __LNB_ANIMATED_HATS
					if (pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1)
					{
						if (pItemPropOC->dwBasePartsIgnore == PARTS_HEAD)
							m_pModel->SetEffect(PARTS_HAIR, XE_HIDE);

						m_pModel->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE);
					}

				}
			}
		}

		m_pModel->Render(p2DRender->m_pd3dDevice, &matWorld);

#ifdef __ANI_WING
		if (m_viewMover->m_pMask && m_viewMover->IsOptionRenderMask())
		{
			D3DXMATRIX mMask = matWorld;

			if (m_viewMover->m_pMask)
			{
				m_viewMover->m_pMask->m_nNoEffect = m_pModel->m_nNoEffect;
				static const int SPINE1_BONE = 4;
				static const int SPINE2_BONE = 5;
				D3DXMATRIX* pmatSpine1Bone = m_pModel->GetMatrixBone(SPINE1_BONE);
				D3DXMATRIX* pmatSpine2Bone = m_pModel->GetMatrixBone(SPINE2_BONE);
				if (pmatSpine1Bone && pmatSpine2Bone)
				{
					D3DXMATRIX matEvent;
					D3DXMatrixIdentity(&matEvent);

					pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
					D3DXMatrixTranslation(&matEvent, 0.2f, 0.0f, 0.1f);

					D3DXMATRIX matRotation;
					D3DXMatrixIdentity(&matRotation);
					D3DXMatrixRotationX(&matRotation, D3DXToRadian(180));
					D3DXMatrixMultiply(&matEvent, &matRotation, &matEvent);
					D3DXMatrixRotationZ(&matRotation, D3DXToRadian(-90));
					D3DXMatrixMultiply(&matEvent, &matRotation, &matEvent);
					D3DXMatrixMultiply(&matEvent, &matEvent, pmatSpine1Bone);

					D3DXMatrixMultiply(&mMask, &matEvent, &matWorld);
				}
				m_viewMover->m_pMask->SetTextureEx(m_viewMover->m_pMask->m_pModelElem->m_nTextureEx);

				m_viewMover->m_pMask->Render(pd3dDevice, &mMask);
				m_viewMover->m_pMask->m_nNoEffect = 0;
			}
		}
#endif

#ifdef __LNB_ANIMATED_HATS
#ifdef __HIDE_FASHION
		if (pMover->m_abHideCoat[0] == 0)
		{
#endif //__HIDE_FASHION
			D3DXMATRIX mRide = matWorld;
			if (m_viewMover->GainAniHatInfo())
			{
				m_pModel->SetEffect(PARTS_CAP, XE_HIDE);
				static const int NECK_BONE = 6;
				static const int HEAD_BONE = 6;
				D3DXMATRIX* pmatNeckBone = m_pModel->GetMatrixBone(NECK_BONE);
				D3DXMATRIX* pmatHeadBone = m_pModel->GetMatrixBone(HEAD_BONE);
				if (pmatNeckBone && pmatHeadBone)
				{
					D3DXMATRIX matEvent;
					D3DXMatrixIdentity(&matEvent);

					D3DXMATRIX matRotation;
					D3DXMatrixIdentity(&matRotation);
					D3DXMatrixRotationX(&matRotation, D3DXToRadian(180));
					D3DXMatrixMultiply(&matEvent, &matRotation, &matEvent);
					D3DXMatrixRotationZ(&matRotation, D3DXToRadian(-90));
					D3DXMatrixMultiply(&matEvent, &matRotation, &matEvent);
					D3DXMatrixMultiply(&matEvent, &matEvent, pmatNeckBone);


					matEvent._41 = pmatHeadBone->_41;
					matEvent._42 = pmatHeadBone->_42;
					matEvent._43 = pmatHeadBone->_43;


					D3DXMatrixMultiply(&mRide, &matEvent, &matWorld);
				}


#ifdef __CSC_EXTEXTURE
				m_pAnimatedHat->SetTextureEx(m_pRide->m_pModelElem->m_nTextureEx);
#endif //__CSC_EXTEXTURE
				m_viewMover->GainAniHatInfo()->Render(pd3dDevice, &mRide);

			}
#ifdef __HIDE_FASHION
		}
#endif //__HIDE_FASHION
#endif //__LNB_ANIMATED_HATS

	}

	return;
}

void CWndViewInventoryCtrl::UpDateModel()
{
	SAFE_DELETE(m_pModel);
	m_viewMover = &g_WndMng.m_pWndViewInventoryMain->m_viewMover;
	if (IsInvalidObj(m_viewMover))
		return;

	int nMover = (m_viewMover->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE);
	prj.m_modelMng.LoadMotion(m_pModel, OT_MOVER, nMover, MTI_STAND);
	UpdateParts();
	m_pModel->InitDeviceObjects(g_Neuz.GetDevice());
}

void CWndViewInventoryCtrl::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_viewMover = &g_WndMng.m_pWndViewInventoryMain->m_viewMover;

	if (IsInvalidObj(m_viewMover))
		return;

	m_bLButtonDownRot = FALSE;
	m_fRot = 0.0f;

#ifdef __AUTO_PERIN_CONVERTER
	CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	pWndCheck->SetCheck(m_viewMover && m_viewMover->IsMode(AUTO_CONVERT_MODE));
#endif // __AUTO_PERIN_CONVERTER

	LPWNDCTRL lpWndCtrl1 = GetWndCtrl(WIDC_CUSTOM1);
	LPWNDCTRL lpWndCtrl2 = GetWndCtrl(WIDC_CUSTOM2);
	LPWNDCTRL lpWndCtrl3 = GetWndCtrl(WIDC_CUSTOM3);
	LPWNDCTRL lpWndCtrl4 = GetWndCtrl(WIDC_CUSTOM4);
	LPWNDCTRL lpWndCtrl5 = GetWndCtrl(WIDC_CUSTOM5);
	LPWNDCTRL lpWndCtrl6 = GetWndCtrl(WIDC_CUSTOM6);
	LPWNDCTRL lpWndCtrl7 = GetWndCtrl(WIDC_CUSTOM7);
	LPWNDCTRL lpWndCtrl8 = GetWndCtrl(WIDC_CUSTOM8);
	LPWNDCTRL lpWndCtrl9 = GetWndCtrl(WIDC_CUSTOM9);
	LPWNDCTRL lpWndCtrl10 = GetWndCtrl(WIDC_CUSTOM10);

	m_InvenRect[6] = m_InvenRect[14] = lpWndCtrl1->rect;
	m_InvenRect[2] = m_InvenRect[15] = lpWndCtrl2->rect;
	m_InvenRect[4] = m_InvenRect[17] = lpWndCtrl3->rect;
	m_InvenRect[5] = m_InvenRect[18] = lpWndCtrl4->rect;

	m_InvenRect[10] = lpWndCtrl5->rect;
	m_InvenRect[9] = m_InvenRect[11] = lpWndCtrl6->rect;
	m_InvenRect[25] = lpWndCtrl7->rect;
	m_InvenRect[8] = lpWndCtrl8->rect;
	m_InvenRect[12] = lpWndCtrl9->rect;
	m_InvenRect[13] = lpWndCtrl10->rect;

	lpWndCtrl1 = GetWndCtrl(WIDC_CUSTOM11);
	lpWndCtrl2 = GetWndCtrl(WIDC_CUSTOM12);
	lpWndCtrl3 = GetWndCtrl(WIDC_CUSTOM13);
	lpWndCtrl4 = GetWndCtrl(WIDC_CUSTOM14);
	lpWndCtrl5 = GetWndCtrl(WIDC_CUSTOM15);
	lpWndCtrl6 = GetWndCtrl(WIDC_CUSTOM16);
	lpWndCtrl7 = GetWndCtrl(WIDC_CUSTOM17);
	lpWndCtrl8 = GetWndCtrl(WIDC_CUSTOM18);
	lpWndCtrl9 = GetWndCtrl(WIDC_CUSTOM19);

	m_InvenRect[20] = lpWndCtrl1->rect;
	m_InvenRect[22] = lpWndCtrl2->rect;
	m_InvenRect[19] = lpWndCtrl3->rect;
	m_InvenRect[23] = lpWndCtrl4->rect;
	m_InvenRect[21] = lpWndCtrl5->rect;

	m_InvenRect[26] = lpWndCtrl6->rect;
	m_InvenRect[27] = lpWndCtrl7->rect;
	m_InvenRect[28] = lpWndCtrl8->rect;
	m_InvenRect[29] = lpWndCtrl9->rect;

	SAFE_DELETE(m_pModel);

#ifdef __SYS_280514
	int nArryEquip[5] = { PARTS_HAT, PARTS_CLOTH, PARTS_GLOVE, PARTS_BOOTS, PARTS_CLOAK };
	for (int i = 0; i < 5; ++i)
	{
		CRect rect = m_InvenRect[nArryEquip[i]];
		rect.right = rect.left + 16;
		rect.bottom = rect.top + 16;
	}
#endif // __SYS_280514

	int nMover = (m_viewMover->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE);
	prj.m_modelMng.LoadMotion(m_pModel, OT_MOVER, nMover, MTI_STAND);
	UpdateParts();
	m_pModel->InitDeviceObjects(g_Neuz.GetDevice());

	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_INVENTORY);
	m_wndItemCtrl.Create(WLVS_ICON, CRect(0, 0, 250, 250), pTabCtrl, 11);
	m_wndItemCtrl.InitItem(&m_viewMover->m_Inventory, APP_VIEW_INVENTORY_CTRL);

	WTCITEM tabTabItem;
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = GETTEXT(TID_GAME_ITEM);//"아이템";
	tabTabItem.pWndBase = &m_wndItemCtrl;
	pTabCtrl->InsertItem(0, &tabTabItem);

	LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_GOLD);
	m_wndGold.Create("g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_GOLD);
	m_wndGold.AddWndStyle(WBS_NODRAWFRAME);

	CWndStatic* pGoldNum = (CWndStatic*)GetDlgItem(WIDC_GOLD_NUM);
	pGoldNum->AddWndStyle(WSS_MONEY);

	m_pUpgradeItem = NULL;
	m_pUpgradeMaterialItem = NULL;
	m_bIsUpgradeMode = FALSE;
	m_dwEnchantWaitTime = 0xffffffff;

#ifdef __AEGON_THEME_SWITCHER
	m_TexRemoveItem = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(g_Neuz.m_pd3dDevice, "WndInventoryGarbage.dds", COLOR_ITEM_BG);
#else
	m_TexRemoveItem = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "WndInventoryGarbage.dds"), COLOR_ITEM_BG);
#endif

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 112 + 48);
	Move(point);

#ifdef __AEGON_THEME_SWITCHER
	LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM20);
	if (lpFace)
	{
		m_rectRenderChar = lpFace->rect;
	}
	LPWNDCTRL lpWndCtrl21 = GetWndCtrl(WIDC_CUSTOM21);
	if (lpWndCtrl21)
	{
		m_rectDeleteButt = lpWndCtrl21->rect;
	}
#endif // __AEGON_THEME_SWITCHER
}

BOOL CWndViewInventoryCtrl::Process()
{
#ifdef __AUTO_PERIN_CONVERTER
	CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	pWndCheck->SetCheck(m_viewMover && m_viewMover->IsMode(AUTO_CONVERT_MODE));
#endif // __AUTO_PERIN_CONVERTER
#ifdef __ANI_WING
	if (m_viewMover->m_pMask)
		m_viewMover->m_pMask->FrameMove();
#endif
	if (m_pModel)
		m_pModel->FrameMove();

	if (m_dwEnchantWaitTime < g_tmCurrent)
	{
		m_dwEnchantWaitTime = 0xffffffff;

		if (m_pSfxUpgrade)
		{
			m_pSfxUpgrade->Delete();
			m_pSfxUpgrade = NULL;
		}

		CItemElem* pItemElem = (CItemElem*)m_pUpgradeItem;
		if (pItemElem && m_pUpgradeMaterialItem)
		{
			CItemElem* pItemMaterialElem = (CItemElem*)m_pUpgradeMaterialItem;
			// 패킷 전송

			if (pItemMaterialElem->GetProp())
			{
				// 인첸트에 관한 아이템이냐?
#if __VER >= 8 //__Y_NEW_ENCHANT
				if (pItemMaterialElem->GetProp()->dwItemKind3 == IK3_ELECARD
					|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_ENCHANT
#if __VER >= 9 // __ULTIMATE
					|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_PIERDICE
#endif // __ULTIMATE			
					)
#else //__Y_NEW_ENCHANT
				if (pItemMaterialElem->GetProp()->dwItemKind3 == IK3_ELECARD || pItemMaterialElem->GetProp()->dwItemKind3 == IK3_DICE)
#endif //__Y_NEW_ENCHANT
				{
				}
				else
					// 피어싱에 관한 아이템이냐?
					if (pItemMaterialElem->GetProp()->dwItemKind3 == IK3_SOCKETCARD
#if __VER >= 12 // __EXT_PIERCING
						|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_SOCKETCARD2
#endif // __EXT_PIERCING
						)
					{
						//g_DPlay.SendPiercing(pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId);
					}
#if __VER >= 11
					else if (IsNeedTarget(pItemMaterialElem->GetProp()))
					{
						//g_DPlay.SendDoUseItemTarget(m_pUpgradeMaterialItem->m_dwObjId, pItemElem->m_dwObjId);
					}
#endif	// __VER
					else
						if (pItemMaterialElem->GetProp()->dwItemKind3 == IK3_RANDOM_SCROLL)
						{
						}
			}
		}
	}
	return TRUE;
}

BOOL CWndViewInventoryCtrl::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
BOOL CWndViewInventoryCtrl::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndViewInventoryCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
}
void CWndViewInventoryCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}
BOOL CWndViewInventoryCtrl::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();

	if (pWndFrame == NULL)
		return FALSE;

	if (pWndFrame->GetWndId() == APP_INVENTORY)
	{
		CItemElem* pItemElem = (CItemElem*)g_pPlayer->GetItemId(pShortcut->m_dwId);
		if (pItemElem)
		{
			int nContainer = 0;
			int nContainerField = -1;
			g_DPlay.SendViewInventoryCommand(pItemElem, 3, nContainer, nContainerField);
		}
	}

	return CWndBase::OnDropIcon(pShortcut, point);
}
void CWndViewInventoryCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{

}

void CWndViewInventoryCtrl::OnDestroy(void)
{
	if (m_pSfxUpgrade)
	{
		m_pSfxUpgrade->Delete();
		m_pSfxUpgrade = NULL;
	}
	DeleteDeviceObjects();
	Destroy();
}

HRESULT CWndViewInventoryCtrl::RestoreDeviceObjects()
{
	CWndNeuz::RestoreDeviceObjects();
	return S_OK;
}
HRESULT CWndViewInventoryCtrl::InvalidateDeviceObjects()
{
	CWndNeuz::InvalidateDeviceObjects();
	return S_OK;
}
HRESULT CWndViewInventoryCtrl::DeleteDeviceObjects()
{
	CWndNeuz::DeleteDeviceObjects();

	//SAFE_DELETE(m_viewMover);
	SAFE_DELETE(m_pModel);
	SAFE_DELETE(m_pWndConfirmBuy);

#if __VER >= 9 // __CSC_VER9_1
	SAFE_DELETE(m_pWndRemoveJewelConfirm);
#endif //__CSC_VER9_1

	InvalidateDeviceObjects();
	return S_OK;
}
#endif // __AEGON_VIEW_INV