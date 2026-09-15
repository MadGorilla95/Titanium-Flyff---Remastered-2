#include "StdAfx.h"
#ifdef __SWITCH_EQUIP
#include "WndSwitchEquip.h"
#include "ResData.h"
#include "defineObj.h"
#include "DPClient.h"
#include "defineText.h"
extern CDPClient g_DPlay;

CWndSwitchEquip::CWndSwitchEquip()
{
	memset(m_InvenRect, 0, sizeof(CRect) * MAX_HUMAN_PARTS);
	memset(m_dwId, NULL_ID, sizeof(DWORD) * MAX_HUMAN_PARTS);
	m_nIndex = 0;
	m_pModel = NULL;
	m_bSelect = false;
	m_OldPos = CPoint(0, 0);
	m_pTextureDrag = NULL;
}
CWndSwitchEquip::~CWndSwitchEquip()
{
	SAFE_DELETE(m_pModel);
	//SAFE_DELETE(m_pTextureDrag);
}

BOOL CWndSwitchEquip::Process()
{
	if (g_pPlayer == NULL)
		return TRUE;

	if (m_pModel)
		m_pModel->FrameMove();

	UpdateItem();

	return TRUE;
}
void CWndSwitchEquip::OnMouseWndSurface(CPoint point)
{
	if (g_pPlayer == NULL)
		return;

	int nTemp = 0;
	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		//if (m_dwId[i] == NULL_ID)
		//	continue;


		CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId(m_dwId[i]);
		CRect DrawRect = m_InvenRect[i];
		CPoint point = GetMousePoint();
		if (DrawRect.PtInRect(point))
		{
			nTemp = i;
			if (i == PARTS_LWEAPON)
			{
				pItemElem = g_pPlayer->m_Inventory.GetAtId(m_dwId[PARTS_SHIELD]);

				if (IsUsableItem(pItemElem))
				{
					nTemp = PARTS_SHIELD;
				}
				else
				{
					pItemElem = g_pPlayer->m_Inventory.GetAtId(m_dwId[PARTS_LWEAPON]);
					if (pItemElem == NULL)
						pItemElem = g_pPlayer->m_Inventory.GetAtId(m_dwId[PARTS_RWEAPON]);
				}
			}

			if (IsUsableItem(pItemElem))
			{
				CPoint point2 = point;
				ClientToScreen(&point2);
				ClientToScreen(&DrawRect);

				CItemElem itemElem;
				itemElem.m_dwItemId = pItemElem->m_dwItemId;
				itemElem.m_byFlag = pItemElem->m_byFlag;
				itemElem.SetAbilityOption(pItemElem->GetAbilityOption());
				itemElem.m_nResistAbilityOption = pItemElem->m_nResistAbilityOption;
				itemElem.m_bItemResist = pItemElem->m_bItemResist;
				itemElem.SetRandomOptItemId(pItemElem->GetRandomOptItemId());
				itemElem.CopyPiercing(pItemElem->GetPiercing());
#ifdef __LOOKCHANGE
				itemElem.SetLook(pItemElem->GetLook());
#endif // __LOOKCHANGE
				g_WndMng.PutToolTip_Item((CItemBase*)&itemElem, point2, &DrawRect, APP_QUERYEQUIP);
				break;
			}
			else
			{
				CPoint point2 = point;
				ClientToScreen(&point2);
				ClientToScreen(&DrawRect);

				CString strText;

				switch (nTemp)
				{
				case 0:
					strText = "Loot Pet";
					break;
				case 1:
					strText = "Raising Pet";
					break;
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
		}
	}
}
void CWndSwitchEquip::OnDraw(C2DRender* p2DRender)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;
	D3DVIEWPORT9 viewport;

	viewport.X = p2DRender->m_ptOrigin.x + 5;
	viewport.Y = p2DRender->m_ptOrigin.y + 5;
	viewport.Width = p2DRender->m_clipRect.Width() - 100;
	viewport.Height = 182;
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;

	DWORD dwColor1 = D3DCOLOR_ARGB(100, 0, 0, 0);
	DWORD dwColor2 = D3DCOLOR_ARGB(255, 240, 240, 240);
	DWORD dwColor3 = D3DCOLOR_ARGB(100, 200, 200, 200);

	if (!m_pTextureDrag)
	{
#ifdef __AEGON_THEME_SWITCHER
		m_pTextureDrag = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(g_Neuz.m_pd3dDevice, "icon_ability.dds", 0xffff00ff);
#else
		m_pTextureDrag = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "icon_ability.dds"), 0xffff00ff);
#endif
	}

#ifdef __AEGON_THEME_SWITCHER
	p2DRender->RenderTexture(m_hotkeyRect.TopLeft(), m_pTextureDrag, 255, 1.0f, 1.0f);
#else // __AEGON_THEME_SWITCHER
	LPWNDCTRL lpWndDrag = GetWndCtrl(WIDC_CUSTOM23);
	p2DRender->RenderTexture(lpWndDrag->rect.TopLeft(), m_pTextureDrag, 255, 1.0f, 1.0f);
#endif // __AEGON_THEME_SWITCHER

	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		CRect DrawRect = m_InvenRect[i];
		CPoint cpAdd = CPoint(6, 6);
		FLOAT sx = 1.0f, sy = 1.0f;
		DWORD dwAlpha = 255;

		if (m_dwId[i] == NULL_ID)
			continue;

		CItemBase* pItemBase = g_pPlayer->m_Inventory.GetAtId(m_dwId[i]);
		if (!IsUsableItem(pItemBase))
			continue;

		if (i == PARTS_LWEAPON)
		{
			if (m_dwId[PARTS_RWEAPON] != NULL_ID)
			{
				CItemBase* pRWeapon = g_pPlayer->m_Inventory.GetAtId(m_dwId[PARTS_RWEAPON]);
				if (IsUsableItem(pRWeapon) && pRWeapon->GetProp()->dwHanded == HD_TWO)
				{
					pItemBase = pRWeapon;
					dwAlpha = 100;
				}
			}
		}

		if (i >= PARTS_NECKLACE1 && i <= PARTS_EARRING2)
		{
			cpAdd = CPoint(0, 0);
			sx = 0.8f;
			sy = 0.8f;
		}
		else if (i >= PARTS_HAT && i <= PARTS_BOOTS)
		{
			cpAdd = CPoint(0, 0);
			sx = 0.9f;
			sy = 0.9f;
		}

		if (pItemBase && pItemBase->GetTexture())
		{
			if (((CItemElem*)pItemBase)->IsFlag(CItemElem::expired))
				pItemBase->GetTexture()->Render2(p2DRender, DrawRect.TopLeft() + cpAdd, D3DCOLOR_XRGB(255, 100, 100), sx, sy);
			else
				pItemBase->GetTexture()->Render2(p2DRender, DrawRect.TopLeft() + cpAdd, D3DCOLOR_ARGB(dwAlpha, 255, 255, 255), sx, sy);
		}
	}

	if (m_pModel == NULL)
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

	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot;
	D3DXMATRIXA16 matTrans;
	D3DXMATRIX  matView;
	D3DXVECTOR3 vecLookAt(0.0f, 0.0f, 3.0f);
	D3DXVECTOR3 vecPos(0.0f, 0.7f, -3.5f);

	D3DXMatrixLookAtLH(&matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
	{
#ifdef __AEGON_THEME_SWITCHER
		viewport.X = p2DRender->m_ptOrigin.x + m_charRenderRect.left;
		viewport.Y = p2DRender->m_ptOrigin.y + m_charRenderRect.top;
		viewport.Width = m_charRenderRect.Width();
		viewport.Height = m_charRenderRect.Height();
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
		FLOAT fov = D3DX_PI / 4.0f;
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
		pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

		::SetLight(FALSE);
		::SetFog(FALSE);
		SetDiffuse(1.0f, 1.0f, 1.0f);
		SetAmbient(1.0f, 1.0f, 1.0f);

		if (m_pModel->GetObject3D(PARTS_HAIR))
		{
			m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = g_pPlayer->m_fHairColorR;
			m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = g_pPlayer->m_fHairColorG;
			m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = g_pPlayer->m_fHairColorB;
		}
		D3DXVECTOR4 vConst(1.0f, 1.0f, 1.0f, 1.0f);
		pd3dDevice->SetVertexShaderConstantF(95, (float*)&vConst, 1);
		::SetTransformView(matView);
		::SetTransformProj(matProj);

		m_Mover.OverCoatItemRenderCheck(m_pModel);

		DWORD dwId = m_Mover.m_aEquipInfo[PARTS_CAP].dwId;;

#ifdef __LOOKCHANGE
		if (m_Mover.m_aEquipInfo[PARTS_CAP].dwLook != NULL_ID)
			dwId = m_Mover.m_aEquipInfo[PARTS_CAP].dwLook;
#endif
		O3D_ELEMENT* pElement = NULL;
		ItemProp* pItemProp = NULL;

		if (dwId != NULL_ID)
		{
			pItemProp = prj.GetItemProp(dwId);

			if (pItemProp && pItemProp->dwBasePartsIgnore != -1)
			{
				pElement = m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
			}

			// ¢¯UAoAC¡íoA¡í AO¨úuA¡í¡Æ©¡¢¯i ¢¬O¢¬¢ç©ø?¢¬¡¾¡ÆIAI¡Æ¢®AC ¡¾aA¨ªA¡í ¢¯UAo ¢¬©£AU¢¬| ¡¾aA¨ªA¢¬¡¤I ©öU©÷U¢¥U
			dwId = m_Mover.m_aEquipInfo[PARTS_HAT].dwId;
#ifdef __LOOKCHANGE
			if (m_Mover.m_aEquipInfo[PARTS_HAT].dwLook != NULL_ID)
				dwId = m_Mover.m_aEquipInfo[PARTS_HAT].dwLook;
#endif
			if (dwId != NULL_ID)
			{
				if (!(m_Mover.m_aEquipInfo[PARTS_HAT].byFlag & CItemElem::expired))
				{
					pItemProp = prj.GetItemProp(dwId);
					if (pItemProp && pItemProp->dwBasePartsIgnore != -1)
					{
						if (pItemProp->dwBasePartsIgnore == PARTS_HEAD)
							((CModelObject*)m_pModel)->SetEffect(PARTS_HAIR, XE_HIDE);

						((CModelObject*)m_pModel)->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
					}
					else
					{
						if (pElement)
							pElement->m_nEffect &= ~XE_HIDE;
					}
				}
			}

		}
		else
		{
			// ¢¯UAoAC¡íoA¡í AO¨úuA¡í¡Æ©¡¢¯i ¢¬O¢¬¢ç©ø?¢¬¡¾¡ÆIAI¡Æ¢®AC ¡¾aA¨ªA¡í ¢¯UAo ¢¬©£AU¢¬| ¡¾aA¨ªA¢¬¡¤I ©öU©÷U¢¥U

			dwId = m_Mover.m_aEquipInfo[PARTS_HAT].dwId;
#ifdef __LOOKCHANGE
			if (m_Mover.m_aEquipInfo[PARTS_HAT].dwLook != NULL_ID)
				dwId = m_Mover.m_aEquipInfo[PARTS_HAT].dwLook;
#endif
			if (dwId != NULL_ID)
			{
				if (!(m_Mover.m_aEquipInfo[PARTS_HAT].byFlag & CItemElem::expired))
				{
					pItemProp = prj.GetItemProp(dwId);
					if (pItemProp && pItemProp->dwBasePartsIgnore != -1)
					{
						if (pItemProp->dwBasePartsIgnore == PARTS_HEAD)
							((CModelObject*)m_pModel)->SetEffect(PARTS_HAIR, XE_HIDE);

						((CModelObject*)m_pModel)->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
					}
				}
			}
		}

		if (m_pModel->GetObject3D(PARTS_HAIR))
		{
			m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = g_pPlayer->m_fHairColorR;
			m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = g_pPlayer->m_fHairColorG;
			m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = g_pPlayer->m_fHairColorB;
		}
		m_pModel->SetGroup(0);
		m_pModel->Render(p2DRender->m_pd3dDevice, &matWorld);
	}
}
void CWndSwitchEquip::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bLButtonDownRot)
		m_fRot += (m_OldPos.x - point.x) * 0.5f;

	m_OldPos = point;

	if (m_bSelect == false)
		return;

	if (m_nIndex > 0 && m_nIndex <= (int)g_pPlayer->m_vEquipSwitchInfo.size())
	{
		m_bSelect = false;
		m_GlobalShortcut.m_pFromWnd = this;
		m_GlobalShortcut.m_dwShortcut = SHORTCUT_SWITCH_EQUIP;
		m_GlobalShortcut.m_dwType = 0;
		m_GlobalShortcut.m_dwIndex = 0;
		m_GlobalShortcut.m_dwData = 0;
		m_GlobalShortcut.m_dwId = m_nIndex - 1;
		m_GlobalShortcut.m_pTexture = m_pTextureDrag;
		_tcscpy(m_GlobalShortcut.m_szString, g_pPlayer->m_vEquipSwitchInfo[m_nIndex - 1].strTitle);
	}
}
void CWndSwitchEquip::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bSelect = false;
	m_bLButtonDownRot = FALSE;

#ifdef __AEGON_THEME_SWITCHER
	if (m_hotkeyRect.PtInRect(point))
		g_WndMng.ObjectExecutor(SHORTCUT_SWITCH_EQUIP, m_nIndex - 1);
#else // __AEGON_THEME_SWITCHER
	if (GetWndCtrl(WIDC_CUSTOM23)->rect.PtInRect(point))
		g_WndMng.ObjectExecutor(SHORTCUT_SWITCH_EQUIP, m_nIndex - 1);
#endif // __AEGON_THEME_SWITCHER

	//ReleaseCapture();
}
void CWndSwitchEquip::OnLButtonDown(UINT nFlags, CPoint point)
{
	//SetCapture();
#ifdef __AEGON_THEME_SWITCHER
	if (m_charRenderRect.PtInRect(point))
#else __AEGON_THEME_SWITCHER
	LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_CUSTOM20);
	if (lpWndCtrl->rect.PtInRect(point))
#endif // __AEGON_THEME_SWITCHER
	{
		m_OldPos = point;
		m_bLButtonDownRot = TRUE;
	}
#ifdef __AEGON_THEME_SWITCHER
	if (m_hotkeyRect.PtInRect(point))
#else // __AEGON_THEME_SWITCHER
	lpWndCtrl = GetWndCtrl(WIDC_CUSTOM23);
	if (lpWndCtrl->rect.PtInRect(point)) 
#endif // __AEGON_THEME_SWITCHER
	{
		m_bSelect = true;
	}
		
}

void CWndSwitchEquip::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		if (m_InvenRect[i].PtInRect(point))
		{
			if (!m_aSlotItem[i].IsEmpty())
			{
				m_aSlotItem[i].Empty();
				m_Mover.m_aEquipInfo[i].dwId = NULL_ID;
#ifdef __LOOKCHANGE
				m_Mover.m_aEquipInfo[i].dwLook = NULL_ID;
#endif
				m_dwId[i] = NULL_ID;
				CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, m_Mover.m_aEquipInfo, m_pModel, NULL);
			}
		}
	}
}

BOOL CWndSwitchEquip::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	if (pShortcut->m_dwShortcut == SHORTCUT_ITEM && pShortcut->m_pFromWnd != this && pShortcut->m_pFromWnd->GetFrameWnd()->GetWndId() == APP_INVENTORY)
	{
		if (pShortcut->m_dwType == ITYPE_ITEM && pShortcut->m_dwData)
		{
			CItemElem* pItemElem = (CItemElem*)g_pPlayer->GetItemId(pShortcut->m_dwId);
			if (IsUsableItem(pItemElem))
			{
				if (pItemElem->GetProp()->dwParts != NULL_ID || pItemElem->GetProp()->dwItemKind3 == IK3_EGG || pItemElem->GetProp()->dwItemKind3 == IK3_PET)
				{
					CRect rect;
#ifdef __AEGON_THEME_SWITCHER
					rect = m_InvenRect[10];
					rect.bottom = m_InvenRect[13].bottom;
					rect.right = m_InvenRect[13].right;
#else // __AEGON_THEME_SWITCHER
					LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_CUSTOM5);
					rect = lpWndCtrl->rect;
					lpWndCtrl = GetWndCtrl(WIDC_CUSTOM10);
					rect.bottom = lpWndCtrl->rect.bottom;
					rect.right = lpWndCtrl->rect.right;
#endif // __AEGON_THEME_SWITCHER

					if (rect.PtInRect(point))
						AddItem(pItemElem, pShortcut);

#ifdef __AEGON_THEME_SWITCHER
					rect = m_InvenRect[0];
					rect.bottom = m_InvenRect[1].bottom;
					rect.right = m_InvenRect[1].right;
#else // __AEGON_THEME_SWITCHER
					lpWndCtrl = GetWndCtrl(WIDC_CUSTOM21);
					rect = lpWndCtrl->rect;
					lpWndCtrl = GetWndCtrl(WIDC_CUSTOM22);
					rect.bottom = lpWndCtrl->rect.bottom;
					rect.right = lpWndCtrl->rect.right;
#endif // __AEGON_THEME_SWITCHER

					if (rect.PtInRect(point))
						AddItem(pItemElem, pShortcut);
				}
			}
		}
	}
	return TRUE;
}

BOOL CWndSwitchEquip::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_COMBOBOX:
	{
		if (message == WNM_SELCHANGE || message == EN_CHANGE)
		{
			CWndComboBox* pComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX);
			if (pComboBox->GetCurSel() == 0)
			{
				CWndEdit* pEditTitle = (CWndEdit*)GetDlgItem(WIDC_EDIT);
				pEditTitle->SetString("");

				m_nIndex = 0;

				for (int i = 0; i < MAX_HUMAN_PARTS; i++)
				{
					m_aSlotItem[i].Empty();
					m_Mover.m_aEquipInfo[i].dwId = NULL_ID;
#ifdef __LOOKCHANGE
					m_Mover.m_aEquipInfo[i].dwLook = NULL_ID;
#endif
					m_dwId[i] = NULL_ID;
				}
				CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, m_Mover.m_aEquipInfo, m_pModel, NULL);
			}
			else if (pComboBox->GetCurSel() <= (int)g_pPlayer->m_vEquipSwitchInfo.size())
			{
				m_nIndex = pComboBox->GetCurSel();
				LoadSwitch();
			}
		}
	}
	break;
	case WIDC_BUTTON:
		{
			for (int i = 0; i < MAX_HUMAN_PARTS; i++)
			{
				m_aSlotItem[i].Empty();
				m_Mover.m_aEquipInfo[i].dwId = NULL_ID;
#ifdef __LOOKCHANGE
				m_Mover.m_aEquipInfo[i].dwLook = NULL_ID;
#endif
				m_dwId[i] = NULL_ID;
			}
			CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, m_Mover.m_aEquipInfo, m_pModel, NULL);
			if (m_nIndex != 0 && (m_nIndex - 1) >= 0 && (m_nIndex - 1) < g_pPlayer->m_vEquipSwitchInfo.size())
			{
				g_pPlayer->m_vEquipSwitchInfo.erase(g_pPlayer->m_vEquipSwitchInfo.begin() + m_nIndex - 1);
				m_nIndex = 0;
				g_DPlay.SendEquipSwitch();
				CWndComboBox* pComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX);
				pComboBox->ResetContent();
				pComboBox->AddString("Create Set");
				for (auto& pEquip : g_pPlayer->m_vEquipSwitchInfo)
					pComboBox->AddString(pEquip.strTitle);
				pComboBox->SetCurSel(m_nIndex);
			}
			//Destroy();
			break;
		}
	case WIDC_BUTTON1:
		{
			CWndEdit* pEditTitle = (CWndEdit*)GetDlgItem(WIDC_EDIT);
			CWndComboBox* pComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX);

			__EQUIP_SWITCH_INFO esi;
			CString strTemp = pEditTitle->GetString();
			
			if (strTemp.GetLength() < 4 || strTemp.GetLength() > 16)
			{
				g_WndMng.PutString("Title has to be between 4 and 16 characters long!", NULL, 0xffffff00);
				return CWndNeuz::OnChildNotify(message, nID, pLResult);
			}

			strcpy_s(esi.strTitle, sizeof(char)*20,  pEditTitle->GetString());
			esi.nActivate = 0;
			for (auto& pEquip : g_pPlayer->m_vEquipSwitchInfo)
			{
				if (pEquip.nActivate == esi.nActivate)
					pEquip.nActivate = 0;
			}

			for (int i = 0; i < MAX_HUMAN_PARTS; i++)
			{
				int nIndex = i;
				esi.dwId[i] = m_aSlotItem[i].IsEmpty() ? NULL_ID : m_aSlotItem[nIndex].m_dwId;
			}

			if (m_nIndex == 0 && g_pPlayer->m_vEquipSwitchInfo.size() < 5)
			{
				g_pPlayer->m_vEquipSwitchInfo.push_back(esi);
				m_nIndex = g_pPlayer->m_vEquipSwitchInfo.size();
				pComboBox->AddString(esi.strTitle);
				pComboBox->SetCurSel(m_nIndex);
			}
			else
			{
				g_pPlayer->m_vEquipSwitchInfo[m_nIndex - 1] = esi;
				pComboBox->ResetContent();
				pComboBox->AddString("Create Set");
				for (auto& pEquip : g_pPlayer->m_vEquipSwitchInfo)
					pComboBox->AddString(pEquip.strTitle);
				pComboBox->SetCurSel(m_nIndex);
			}

			g_DPlay.SendEquipSwitch();

			//Destroy();
			break;
		}
	case WIDC_BUTTON2:
		{
			for (int i = 0; i < MAX_HUMAN_PARTS; i++)
			{
				m_aSlotItem[i].Empty();
				m_Mover.m_aEquipInfo[i].dwId = NULL_ID;
#ifdef __LOOKCHANGE
				m_Mover.m_aEquipInfo[i].dwLook = NULL_ID;
#endif
				m_dwId[i] = NULL_ID;
			}
			CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, m_Mover.m_aEquipInfo, m_pModel, NULL);
			for (int i = 2; i < MAX_HUMAN_PARTS; i++)
			{
				CItemElem* pItemElem = g_pPlayer->GetEquipItem(i);
				if (IsUsableItem(pItemElem))
				{
					AddItem(pItemElem, NULL);
				}
			}
			if (g_pPlayer->HasActivatedSystemPet()) 
			{
				CItemElem* pPetItem = g_pPlayer->GetPetItem();
				if (IsUsableItem(pPetItem))
				{
					AddItem(pPetItem, NULL);
				}
			}
			if (g_pPlayer->HasActivatedVisPet())
			{
				CItemElem* pPetItem = g_pPlayer->GetVisPetItem();
				if (IsUsableItem(pPetItem))
				{
					AddItem(pPetItem, NULL);
				}
			}
			else if (g_pPlayer->HasActivatedEatPet()) 
			{
				CItemElem* pPetItem = g_pPlayer->GetEatPetItem();
				if (IsUsableItem(pPetItem))
				{
					AddItem(pPetItem, NULL);
				}
			}
			break;
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndSwitchEquip::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_bSelect = false;
	m_bLButtonDownRot = FALSE;
	m_fRot = 0.0f;

	SAFE_DELETE(m_pTextureDrag);

#ifdef __AEGON_THEME_SWITCHER
	m_pTextureDrag = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(g_Neuz.m_pd3dDevice, "icon_ability.dds", 0xffff00ff);
#else
	m_pTextureDrag = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "icon_ability.dds"), 0xffff00ff);
#endif

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

	lpWndCtrl1 = GetWndCtrl(WIDC_CUSTOM21);
	lpWndCtrl2 = GetWndCtrl(WIDC_CUSTOM22);

	m_InvenRect[0] = lpWndCtrl1->rect;
	m_InvenRect[1] = lpWndCtrl2->rect;

	SAFE_DELETE(m_pModel);

	for (int i = 0; i < MAX_HUMAN_PARTS; ++i)
	{
		m_Mover.m_aEquipInfo[i].dwId = NULL_ID;
#ifdef __LOOKCHANGE
		m_Mover.m_aEquipInfo[i].dwLook = NULL_ID;
#endif
		m_aSlotItem[i].Empty();
		m_dwId[i] = NULL_ID;
	}

	int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);

	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE);
	prj.m_modelMng.LoadMotion(m_pModel, OT_MOVER, nMover, MTI_STAND);

	CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, m_Mover.m_aEquipInfo, m_pModel, NULL);
	m_pModel->InitDeviceObjects(g_Neuz.GetDevice());

	CWndEdit* pEditTitle = (CWndEdit*)GetDlgItem(WIDC_EDIT);
	pEditTitle->m_dwMaxStringNumber = 16;
	pEditTitle->EnableModeChange(FALSE);
	pEditTitle->SetTabStop(TRUE);

	CWndComboBox* pComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX);
	pComboBox->AddWndStyle(EBS_READONLY);
	pComboBox->AddString("Create Set");
	for (auto& pEquip : g_pPlayer->m_vEquipSwitchInfo)
		pComboBox->AddString(pEquip.strTitle);
	pComboBox->SetCurSel(0);

	LoadSwitch();

	MoveParentCenter();

#ifdef __AEGON_THEME_SWITCHER
	m_charRenderRect = GetWndCtrl(WIDC_CUSTOM20)->rect;
	m_hotkeyRect = GetWndCtrl(WIDC_CUSTOM23)->rect;
#endif // __AEGON_THEME_SWITCHER
}

void CWndSwitchEquip::OnDestroy(void)
{
	SAFE_DELETE(m_pModel);
}

BOOL CWndSwitchEquip::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	memset(m_InvenRect, 0, sizeof(CRect) * MAX_HUMAN_PARTS);
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint(792, 130), pWndParent);
}

void CWndSwitchEquip::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	CWndNeuz::SetWndRect(rectWnd, bOnSize);

	CWndQuestDetail* pWndQuestDetail = g_WndMng.m_pWndQuestDetail;
	if (pWndQuestDetail)
	{
		CRect rtCurrentWndQuestDetail = pWndQuestDetail->GetWndRect();
		CRect rtChangedWndQuestDetail;
		int nRevisedLeft = rectWnd.right - 2;
		pWndQuestDetail->Move(nRevisedLeft, rectWnd.top);
	}
}

void CWndSwitchEquip::LoadSwitch()
{
	if (g_pPlayer == NULL)
		return;

	if (m_nIndex > 0 && m_nIndex <= (int)g_pPlayer->m_vEquipSwitchInfo.size())
	{
		__EQUIP_SWITCH_INFO esi = g_pPlayer->m_vEquipSwitchInfo[m_nIndex - 1];

		CWndEdit* pEditTitle = (CWndEdit*)GetDlgItem(WIDC_EDIT);
		CWndComboBox* pComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX);

		pEditTitle->SetString(esi.strTitle);
		pComboBox->SetCurSel(m_nIndex);

		for (int i = 0; i < MAX_HUMAN_PARTS; i++)
		{
			m_Mover.m_aEquipInfo[i].dwId = NULL_ID;
#ifdef __LOOKCHANGE
			m_Mover.m_aEquipInfo[i].dwLook = NULL_ID;
#endif
			m_aSlotItem[i].Empty();
			m_aSlotItem[i].m_dwId = NULL_ID;
			m_dwId[i] = NULL_ID;
		}

		for (int i = 0; i < MAX_HUMAN_PARTS; i++)
		{
			int nPart = i;
			CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId(esi.dwId[nPart]);
			if (pItemElem)
			{
				BOOL bEquiped = g_pPlayer->m_Inventory.IsEquip(pItemElem->m_dwObjId);
				if (bEquiped)
					nPart = pItemElem->m_dwObjIndex - g_pPlayer->m_Inventory.m_dwIndexNum;

				SHORTCUT shortcut;
				shortcut.m_pFromWnd = this;
#if _WIN64
				shortcut.m_dwData = reinterpret_cast<INT_PTR>(pItemElem);
#else
				shortcut.m_dwData = (DWORD)pItemElem;
#endif
				shortcut.m_dwType = ITYPE_ITEM;
				shortcut.m_dwId = pItemElem->m_dwObjId;
				shortcut.m_dwShortcut = SHORTCUT_ITEM;
				m_aSlotItem[nPart] = shortcut;
				m_dwId[nPart] = pItemElem->m_dwObjId;

				if (i > 1)
				{
					m_aEquipInfoAdd[nPart].iRandomOptItemId = pItemElem->GetRandomOptItemId();
					m_aEquipInfoAdd[nPart].piercing = pItemElem->GetPiercing();
					m_aEquipInfoAdd[nPart].bItemResist = pItemElem->m_bItemResist;
					m_aEquipInfoAdd[nPart].nResistAbilityOption = pItemElem->m_nResistAbilityOption;
					m_aEquipInfoAdd[nPart].pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemElem->GetProp()->szIcon), 0xffff00ff);

					m_Mover.m_aEquipInfo[nPart].dwId = pItemElem->m_dwItemId;
#ifdef __LOOKCHANGE
					m_Mover.m_aEquipInfo[nPart].dwLook = NULL_ID;
#endif
				}
			}
		}

		CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, m_Mover.m_aEquipInfo, m_pModel, NULL);
	}
}

void CWndSwitchEquip::AddItem(CItemElem* pItemElem, LPSHORTCUT pShortcut)
{
	if (IsUsableItem(pItemElem))
	{
		int nPart = GetPartId(pItemElem, pShortcut);
		if (nPart == NULL_ID)
		{
			SetForbid(TRUE);
			return;
		}

		ItemProp* pProp = pItemElem->GetProp();

		if (pProp->dwItemKind3 != IK3_EGG && pProp->dwItemKind3 != IK3_PET)
		{
			if (!g_pPlayer->IsInteriorityJob(pProp->dwItemJob) && pProp->dwItemJob != NULL_ID)
			{
				SetForbid(TRUE);
				return;
			}
			if (pProp->dwParts < 2 || pProp->dwParts >= MAX_HUMAN_PARTS || (pProp->dwItemSex != NULL_ID && g_pPlayer->GetSex() != pProp->dwItemSex))
			{
				SetForbid(TRUE);
				return;
			}
			if (!(pProp->dwParts >= PARTS_HAT && pProp->dwParts <= PARTS_BOOTS) && pProp->dwLimitLevel1 != NULL_ID && g_pPlayer->GetLevel() < (int)pProp->dwLimitLevel1)
			{
				SetForbid(TRUE);
				return;
			}

			if (pProp->dwParts == PARTS_RWEAPON && nPart == PARTS_LWEAPON)
			{
				if (!m_aSlotItem[PARTS_RWEAPON].IsEmpty())
				{
					if (pShortcut != NULL && m_aSlotItem[PARTS_RWEAPON].m_dwId != NULL_ID && m_aSlotItem[PARTS_RWEAPON].m_dwId == pShortcut->m_dwId)
					{
						m_aSlotItem[PARTS_RWEAPON].m_dwId = NULL_ID;
						nPart = PARTS_RWEAPON;
					}
				}
			}

			if (pProp->dwParts == PARTS_SHIELD)
			{
				CItemElem* pItemElemTemp = g_pPlayer->m_Inventory.GetAtId(m_dwId[PARTS_RWEAPON]);
				if (IsUsableItem(pItemElemTemp))
				{
					if (pItemElemTemp->GetProp()->dwHanded != HD_ONE)
					{
						m_Mover.m_aEquipInfo[PARTS_RWEAPON].dwId = NULL_ID;
#ifdef __LOOKCHANGE
						m_Mover.m_aEquipInfo[PARTS_RWEAPON].dwLook = NULL_ID;
#endif
						m_aSlotItem[PARTS_RWEAPON].m_dwId = NULL_ID;
						m_dwId[PARTS_RWEAPON] = NULL_ID;
					}
				}
			}

			int nArryEquip1[5] = { PARTS_CAP, PARTS_UPPER_BODY, PARTS_HAND, PARTS_FOOT, PARTS_CLOAK };
			int nArryEquip2[5] = { PARTS_HAT, PARTS_CLOTH, PARTS_GLOVE, PARTS_BOOTS, PARTS_CLOAK2 };
			for (int i = 0; i < 5; ++i)
			{
				if (pProp->dwParts == nArryEquip1[i])
				{
					m_Mover.m_aEquipInfo[nArryEquip2[i]].dwId = NULL_ID;
#ifdef __LOOKCHANGE
					m_Mover.m_aEquipInfo[nArryEquip2[i]].dwLook = NULL_ID;
#endif
					break;
				}
				if (pProp->dwParts == nArryEquip2[i])
				{
					m_Mover.m_aEquipInfo[nArryEquip1[i]].dwId = NULL_ID;
#ifdef __LOOKCHANGE
					m_Mover.m_aEquipInfo[nArryEquip1[i]].dwLook = NULL_ID;
#endif
					break;
				}
			}

			m_aEquipInfoAdd[nPart].iRandomOptItemId = pItemElem->GetRandomOptItemId();
			m_aEquipInfoAdd[nPart].piercing = pItemElem->GetPiercing();
			m_aEquipInfoAdd[nPart].bItemResist = pItemElem->m_bItemResist;
			m_aEquipInfoAdd[nPart].nResistAbilityOption = pItemElem->m_nResistAbilityOption;
			m_aEquipInfoAdd[nPart].pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemElem->GetProp()->szIcon), 0xffff00ff);

			m_Mover.m_aEquipInfo[nPart].dwId = pItemElem->m_dwItemId;
#ifdef __LOOKCHANGE
			m_Mover.m_aEquipInfo[nPart].dwLook = NULL_ID;
#endif
		}

		m_aSlotItem[nPart].m_pFromWnd = this;
#if _WIN64
		m_aSlotItem[nPart].m_dwData = reinterpret_cast<UINT_PTR>(pItemElem);
#else
		m_aSlotItem[nPart].m_dwData = (DWORD)pItemElem;
#endif
		m_aSlotItem[nPart].m_dwType = ITYPE_ITEM;
		m_aSlotItem[nPart].m_dwId = pItemElem->m_dwObjId;
		m_aSlotItem[nPart].m_dwShortcut = SHORTCUT_ITEM;
		m_dwId[nPart] = pItemElem->m_dwObjId;

		CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, m_Mover.m_aEquipInfo, m_pModel, NULL);
	}
}

void CWndSwitchEquip::UpdateItem()
{
	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		if (!m_aSlotItem[i].IsEmpty())
		{
			if (m_aSlotItem[i].m_dwShortcut == SHORTCUT_ITEM)
			{
				CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId(m_aSlotItem[i].m_dwId);
				if (pItemElem)
				{
					if (m_aSlotItem[i].m_pTexture != pItemElem->GetTexture())
					{
						pItemElem->SetTexture();
						m_aSlotItem[i].m_pTexture = pItemElem->GetTexture();
					}
				}
				else
				{
					for (auto& pEquip : g_pPlayer->m_vEquipSwitchInfo)
					{
						for (int j = 0; j < MAX_HUMAN_PARTS; j++)
						{
							if (pEquip.dwId[j] == m_dwId[i])
								pEquip.dwId[j] = NULL_ID;
						}
					}
					m_aSlotItem[i].Empty();
					m_Mover.m_aEquipInfo[i].dwId = NULL_ID;
#ifdef __LOOKCHANGE
					m_Mover.m_aEquipInfo[i].dwLook = NULL_ID;
#endif // __LOOKCHANGE
					m_dwId[i] = NULL_ID;
					CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, m_Mover.m_aEquipInfo, m_pModel, NULL);
					g_DPlay.SendEquipSwitch();
				}
			}
		}
	}
}

int CWndSwitchEquip::GetPartId(CItemElem* pItemElem, LPSHORTCUT pShortcut)
{
	if (!IsUsableItem(pItemElem))
		return NULL_ID;
	ItemProp* pItemProp = pItemElem->GetProp();
	DWORD dwParts = pItemProp->dwParts;

	if (pItemProp->dwItemKind3 == IK3_PET)
		return 0;
	if (pItemProp->dwItemKind3 == IK3_EGG)
		return 1;

	if (dwParts == NULL_ID || dwParts == PARTS_RIDE)
		return NULL_ID;

	if (pItemProp->dwHanded == HD_ONE && pItemProp->dwParts == PARTS_RWEAPON)
	{
		ItemProp* pProp = m_Mover.GetEquipItemProp(NULL, m_Mover.m_aEquipInfo, PARTS_RWEAPON);
		if (pProp && g_pPlayer->IsInteriorityJob(pProp->dwItemJob))
		{
			if (pItemProp->dwID != II_WEA_KNU_ISHOFIST)
			{
				if (pProp && pProp->dwHanded == HD_ONE)
				{
					if (m_Mover.GetEquipItemProp(NULL, m_Mover.m_aEquipInfo, PARTS_SHIELD) == NULL)
						dwParts = PARTS_LWEAPON;
				}
			}
		}
	}
	if (pItemProp->dwHanded == HD_TWO && pItemProp->dwParts == PARTS_RWEAPON)
	{
		ItemProp* pProp = m_Mover.GetEquipItemProp(NULL, m_Mover.m_aEquipInfo, PARTS_RWEAPON);
		if (pProp && g_pPlayer->IsInteriorityJob(pProp->dwItemJob))
		{
			dwParts = PARTS_RWEAPON;
			if (!m_aSlotItem[PARTS_LWEAPON].IsEmpty())
			{
				m_aSlotItem[PARTS_LWEAPON].Empty();
				m_Mover.m_aEquipInfo[PARTS_LWEAPON].dwId = NULL_ID;
#ifdef __LOOKCHANGE
				m_Mover.m_aEquipInfo[PARTS_LWEAPON].dwLook = NULL_ID;
#endif
			}
			if (!m_aSlotItem[PARTS_SHIELD].IsEmpty())
			{
				m_aSlotItem[PARTS_SHIELD].Empty();
				m_Mover.m_aEquipInfo[PARTS_SHIELD].dwId = NULL_ID;
#ifdef __LOOKCHANGE
				m_Mover.m_aEquipInfo[PARTS_SHIELD].dwLook = NULL_ID;
#endif
			}
		}
	}

	if (dwParts == PARTS_RING1 || dwParts == PARTS_EARRING1)
	{
		DWORD dwElem1 = NULL_ID, dwElem2 = NULL_ID, dwPartsTemp = dwParts;

		if (!m_aSlotItem[dwParts].IsEmpty())
			dwElem1 = m_aSlotItem[dwParts].m_dwItemId;
		if (!m_aSlotItem[dwParts + 1].IsEmpty())
			dwElem2 = m_aSlotItem[dwParts + 1].m_dwItemId;

		if (dwElem1 != NULL_ID && dwElem2 == NULL_ID)
			dwParts = pItemProp->dwPartsub;

		if (dwElem1 != NULL_ID && dwElem2 != NULL_ID && dwElem1 == pItemProp->dwID)
			dwParts = pItemProp->dwPartsub;

		if (dwParts != dwPartsTemp && dwElem1 != NULL_ID && dwElem2 == NULL_ID)
		{
			if (m_dwId[dwPartsTemp] == pItemElem->m_dwObjId)
				dwParts = dwPartsTemp;
		}
	}

	return dwParts;
}
#ifdef __MIKSIK_EQUALIZED_MODE 
void CWndSwitchEquip::UpdateParts()
{
	CWndEdit* pEditTitle = (CWndEdit*)GetDlgItem(WIDC_EDIT);
	pEditTitle->SetString("");

	m_nIndex = 0;
	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		m_aSlotItem[i].Empty();
		m_Mover.m_aEquipInfo[i].dwId = NULL_ID;
#ifdef __LOOKCHANGE 
		m_Mover.m_aEquipInfo[i].dwLook = NULL_ID;
#endif //__LOOKCHANGE
		m_dwId[i] = NULL_ID;
	}
	CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, m_Mover.m_aEquipInfo, m_pModel, 0);
}
#endif //__MIKSIK_EQUALIZED_MODE

#ifdef __AEGON_THEME_SWITCHER
void CWndSwitchEquip::OnThemeSelectionChanged() 
{
	CWndNeuz::OnThemeSelectionChanged();

	m_pTextureDrag = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(g_Neuz.m_pd3dDevice, "icon_ability.dds", 0xffff00ff);
}
#endif


#endif // __SWITCH_EQUIP