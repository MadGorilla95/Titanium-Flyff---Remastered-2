#include "stdafx.h"
#include "ResData.h"
#include "DPClient.h"
#include "defineText.h"
extern CDPClient g_DPlay;

#ifdef __WEAPON_MERGE
#include "randomoption.h"
#include "WndWeaponMerge.h"

/*	Weapon Merge Main Window
 *	Window Id:	APP_WEAPONMERGE
 *	Controls:	WIDC_STATIC4	=	Weapon Output Render Target
 *				WIDC_STATIC5	=	Weapon Output Stats Target
 *				WIDC_PICTURE1	=	Weapon Input Picture Destination
 *				WIDC_PICTURE2	=	Weapon Input Picture Source
 *				WIDC_PICTURE3	=	Protection Scroll Input Picture OPTIONAL
 *				//Buttons Start
 *				WIDC_BUTTON1	=	Start Merge Process
 *				WIDC_BUTTON2	=	Destroy
 *	Class:		CWndWeaponMerge
 */

CWndWeaponMerge::CWndWeaponMerge()
{
	m_pItemDest = NULL;
	m_pItemSource = NULL;
#ifndef __AEGON_WEAPON_MERGE_100
	m_pItemProt = NULL;
#endif
	m_pDestModel = NULL;
	pButtonStart = NULL;
	m_nStatus = 0;
	m_nDelay = 25;
	m_fRotate = 0.0f;
	m_fAddRot = 2.0f;
	m_nEyeYPos = 0.0f;
	m_nCount = 0;

#ifdef __AEGON_FIXES
	m_pWndConfirm = NULL;

	m_CtrlWeaponDest = NULL;
	m_CtrlWeaponSource = NULL;
#ifndef m_pItemProt
	m_CtrlProtScroll = NULL;
#endif
	m_CtrlRenderWeapon = NULL;
	m_CtrlOutputStats = NULL;
#endif


}
CWndWeaponMerge::~CWndWeaponMerge()
{
	SAFE_DELETE(m_pWndConfirm);
}
void CWndWeaponMerge::OnDestroy()
{
	Clear(true);
}

BOOL CWndWeaponMerge::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_WEAPONMERGE, 0, CPoint(0, 0), pWndParent);
}

void CWndWeaponMerge::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_CtrlWeaponDest = GetWndCtrl(WIDC_PICTURE1);
	m_CtrlWeaponSource = GetWndCtrl(WIDC_PICTURE2);
#ifndef __AEGON_WEAPON_MERGE_100
	m_CtrlProtScroll = GetWndCtrl(WIDC_PICTURE3);
#endif
	m_CtrlRenderWeapon = GetWndCtrl(WIDC_STATIC4);
	m_CtrlOutputStats = GetWndCtrl(WIDC_STATIC5);

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);
	CString strTitle;
	strTitle.Format("As a way to balance out 1-handed weapon users and 2-handed weapon users, we introduce Weapon Merge. Weapon Merge combines a degraded amount of ATK, Awakes & Pieced cards placed in the weapons.");
	pWndText->SetString(strTitle);

	pButtonStart = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	pButtonStart->EnableWindow(FALSE);

	m_editSwordOption.Init(m_pFont, &m_CtrlOutputStats->rect);

	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.CreateApplet(APP_INVENTORY);
	assert(pWndInventory != NULL);

#ifdef __AEGON_WEAPON_MERGE_100
	CWndStatic* pWndStaticProtectScroll = (CWndStatic*)GetDlgItem(WIDC_PICTURE3);
	CWndStatic* pWndStaticWeap2 = (CWndStatic*)GetDlgItem(WIDC_PICTURE2);

	if (pWndStaticProtectScroll) 
	{
		pWndStaticProtectScroll->SetVisible(FALSE);
		pWndStaticProtectScroll->EnableWindow(FALSE);

		if (m_CtrlWeaponSource && pWndStaticWeap2) 
		{
			m_CtrlWeaponSource->rect = pWndStaticProtectScroll->GetWndRect();
			pWndStaticWeap2->Move(m_CtrlWeaponSource->rect.TopLeft());
		}
	}

	
	

#endif

	pWndInventory->m_wndItemCtrl.SetDieFlag(TRUE);
	MoveParentCenter();
}

int CWndWeaponMerge::RegisterWeapon(CItemElem* pItem, CItemElem* pOther)
{
	ItemProp* pItemProp = pItem->GetProp();

	if (g_pPlayer->m_vtInfo.IsTrading(pItem))
		return -2;

	if (pItem->IsMerged())
		return TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT01;

	if (pItem->IsExpiring())
		return TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT02;

	if (pItem->IsItemLocked())
		return TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT14;

	if (!pItemProp || pItemProp->dwHanded != HD_TWO)
		return TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT03;

#ifdef __NEW_ITEM_VARUNA
	if (pItemProp->IsBaruna())
		return TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT04;
#endif //__NEW_ITEM_VARUNA

	if (pOther)
	{
		ItemProp* pPropOther = pOther->GetProp();
		if (pPropOther)
			if (pPropOther->dwItemKind3 != pItemProp->dwItemKind3)
				return TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT05;
	}
	//if (pItem->IsBinds())
		//return TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT06;

	if (g_xRandomOptionProperty->IsCheckedSafeFlag(pItem->GetRandomOptItemId()))
		return TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT13;

	return -1;
}

void CWndWeaponMerge::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (m_CtrlWeaponDest->rect.PtInRect(point) && m_pItemDest)
	{
		m_pItemDest->SetExtra(0);
		m_pItemDest = NULL;
		pButtonStart->EnableWindow(FALSE);
	}
	else if (m_CtrlWeaponSource->rect.PtInRect(point))
	{
		if (m_pItemSource)
		{
			m_pItemSource->SetExtra(0);
			m_pItemSource = NULL;
			pButtonStart->EnableWindow(FALSE);
		}
	}
#ifndef __AEGON_WEAPON_MERGE_100
	else if (m_CtrlProtScroll->rect.PtInRect(point))
	{
		if (m_pItemProt)
		{
			m_pItemProt->SetExtra(0);
			m_pItemProt = NULL;
		}
	}
#endif
	CWndNeuz::OnRButtonUp(nFlags, point);
}

bool CWndWeaponMerge::CompareItemLevel(ItemProp* pPropSource, ItemProp* pPropDest)
{
	if (pPropSource->dwLimitLevel1 < pPropDest->dwLimitLevel1)
		return false;
	if (pPropSource->dwItemJob < pPropDest->dwItemJob)
		return false;
	return true;
}

BOOL CWndWeaponMerge::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	short nSlot = -1;
	CItemElem* pItem = NULL;
	if (m_CtrlWeaponDest->rect.PtInRect(point))
	{
		pItem = (CItemElem*)g_pPlayer->GetItemId(pShortcut->m_dwId);
		nSlot = 0;
	}
	else if (m_CtrlWeaponSource->rect.PtInRect(point))
	{
		pItem = (CItemElem*)g_pPlayer->GetItemId(pShortcut->m_dwId);
		nSlot = 1;
	}
#ifndef __AEGON_WEAPON_MERGE_100
	else if (m_CtrlProtScroll->rect.PtInRect(point))
	{
		pItem = (CItemElem*)g_pPlayer->GetItemId(pShortcut->m_dwId);
		nSlot = 2;
	}
#endif
	if (nSlot != -1)
		if (SetItem(pItem, nSlot))
			return TRUE;
	return CWndNeuz::OnDropIcon(pShortcut, point);
}

void CWndWeaponMerge::OnMouseWndSurface(CPoint point)
{
	if (m_CtrlWeaponDest->rect.PtInRect(point) && m_pItemDest)
	{
		PUT_TOOL_TIP(m_CtrlWeaponDest->rect, point, m_pItemDest);
	}
	else if (m_CtrlWeaponSource->rect.PtInRect(point) && m_pItemSource)
	{
		PUT_TOOL_TIP(m_CtrlWeaponSource->rect, point, m_pItemSource);
	}
#ifndef __AEGON_WEAPON_MERGE_100
	else if (m_CtrlProtScroll->rect.PtInRect(point) && m_pItemProt)
	{
		PUT_TOOL_TIP(m_CtrlProtScroll->rect, point, m_pItemProt);
	}
#endif
	CWndNeuz::OnMouseWndSurface(point);
}

BOOL CWndWeaponMerge::Process()
{
	if (m_nStatus == 1) //Start
	{
		if (m_nCount > m_nDelay)
		{
			m_fAddRot += 4.0f;

			if (m_nDelay < 10)
			{
				m_nEyeYPos -= 0.05f;
			}

			m_nDelay -= 1;
			if (m_nDelay < 0)
			{
				m_nStatus = 0;
				m_nDelay = 25;
				m_fRotate = 0.0f;
				m_fAddRot = 2.0f;
				m_nEyeYPos = 0.0f;

				if (m_pItemSource && m_pItemDest
#ifndef __AEGON_WEAPON_MERGE_100
					&& m_pItemProt
#endif
					)
				{
					g_DPlay.SendWeaponMerge(m_pItemSource->m_dwObjId, m_pItemDest->m_dwObjId
#ifndef __AEGON_WEAPON_MERGE_100
						, m_pItemProt->m_dwObjId
#else
						, NULL_ID
#endif
					);
				}
				else if (m_pItemSource && m_pItemDest)
				{
					g_DPlay.SendWeaponMerge(m_pItemSource->m_dwObjId, m_pItemDest->m_dwObjId, NULL_ID);
				}

				if(m_pItemSource)
					m_pItemSource->SetExtra(0);

				if(m_pItemDest)
					m_pItemDest->SetExtra(0);

#ifndef __AEGON_WEAPON_MERGE_100
				if(m_pItemProt)
					m_pItemProt->SetExtra(0);
#endif

				pButtonStart->EnableWindow(FALSE);
				m_pItemSource = NULL;
				m_pItemDest = NULL;
#ifndef __AEGON_WEAPON_MERGE_100
				m_pItemProt = NULL;
#endif
			}

			m_nCount = 0;
		}
		m_nCount++;
	}
	return TRUE;
}

void CWndWeaponMerge::OnDraw(C2DRender* p2DRender)
{
	if (m_pItemDest && m_pItemSource && m_pDestModel)
	{
		LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

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

		D3DVIEWPORT9 viewport;

		D3DXMATRIXA16 matWorld;
		D3DXMATRIXA16 matScale;
		D3DXMATRIXA16 matRot;
		D3DXMATRIXA16 matTrans;

		D3DXCOLOR color;

		D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);

		D3DXVECTOR3 vEyePt(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);

		if (m_pItemDest->GetProp()->dwItemKind3 == IK3_AXE ||
			m_pItemDest->GetProp()->dwItemKind3 == IK3_SWD)
		{
			if (m_pItemDest->GetProp()->dwHanded == HD_ONE)
			{
				vEyePt.x = 0.0f;
				vEyePt.y = 3.0f;
				vEyePt.z = 0.0f;

				vLookatPt.x = 0.6f;
				vLookatPt.y = -0.2f;
				vLookatPt.z = 0.0f;
			}
			else if (m_pItemDest->GetProp()->dwHanded == HD_TWO)
			{
				vEyePt.x = 1.0f;
				vEyePt.y = 5.0f;
				vEyePt.z = 0.0f;

				vLookatPt.x = 1.2f;
				vLookatPt.y = -0.2f;
				vLookatPt.z = 0.0f;
			}
		}
		else if (m_pItemDest->GetProp()->dwItemKind3 == IK3_YOYO ||
			m_pItemDest->GetProp()->dwItemKind3 == IK3_KNUCKLEHAMMER ||
#ifdef __CROSSBOW
			m_pItemDest->GetProp()->dwItemKind3 == IK3_CROSSBOW ||
#endif // __CROSSBOW
			m_pItemDest->GetProp()->dwItemKind3 == IK3_BOW)
		{
			vEyePt.x = 0.0f;
			vEyePt.y = 3.0f;
			vEyePt.z = 0.0f;

			vLookatPt.x = 0.01f;
			vLookatPt.y = -0.2f;
			vLookatPt.z = 0.0f;
		}
		else if (m_pItemDest->GetProp()->dwItemKind3 == IK3_WAND)
		{
			vEyePt.x = 0.0f;
			vEyePt.y = 3.0f;
			vEyePt.z = 0.0f;

			vLookatPt.x = 0.4f;
			vLookatPt.y = -0.2f;
			vLookatPt.z = 0.0f;
		}
		else if (m_pItemDest->GetProp()->dwItemKind3 == IK3_CHEERSTICK ||
			m_pItemDest->GetProp()->dwItemKind3 == IK3_STAFF)
		{
			vEyePt.x = 0.0f;
			vEyePt.y = 4.0f;
			vEyePt.z = 0.0f;

			vLookatPt.x = 0.01f;
			vLookatPt.y = -0.2f;
			vLookatPt.z = 0.0f;
		}

		if (m_nStatus == 1)
		{
			vEyePt.y += m_nEyeYPos;
		}

		D3DXMATRIXA16 matView;
		D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
		pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

		D3DXMATRIXA16 matProj;
		D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
		pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

		m_fRotate += m_fAddRot;
		D3DXMatrixRotationX(&matRot, D3DXToRadian(m_fRotate));

		D3DXMatrixScaling(&matScale, 1.1f, 1.1f, 1.1f);

		viewport.X = p2DRender->m_ptOrigin.x + m_CtrlRenderWeapon->rect.left;
		viewport.Y = p2DRender->m_ptOrigin.y + m_CtrlRenderWeapon->rect.top;
		viewport.Width = m_CtrlRenderWeapon->rect.Width();
		viewport.Height = m_CtrlRenderWeapon->rect.Height();

		viewport.MinZ = 0.0f;
		viewport.MaxZ = 1.0f;
		pd3dDevice->SetViewport(&viewport);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0);

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

		D3DXVECTOR4 vConst(1.0f, 1.0f, 1.0f, 1.0f);
		pd3dDevice->SetVertexShaderConstantF(95, (float*)&vConst, 1);
		::SetTransformView(matView);
		::SetTransformProj(matProj);

		m_pDestModel->Render(p2DRender->m_pd3dDevice, &matWorld);

		viewport.X = p2DRender->m_ptOrigin.x;
		viewport.Y = p2DRender->m_ptOrigin.y;
		viewport.Width = p2DRender->m_clipRect.Width();
		viewport.Height = p2DRender->m_clipRect.Height();
		viewport.MinZ = 0.0f;
		viewport.MaxZ = 1.0f;
		pd3dDevice->SetViewport(&viewport);

		pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	}
	if (m_pItemDest)
	{
		CTexture* pTex = m_pItemDest->GetTexture();
		if (pTex)
			pTex->Render(p2DRender, m_CtrlWeaponDest->rect.TopLeft());
	}
	if (m_pItemSource)
	{
		CTexture* pTex = m_pItemSource->GetTexture();
		if (pTex)
			pTex->Render(p2DRender, m_CtrlWeaponSource->rect.TopLeft());
	}
#ifndef __AEGON_WEAPON_MERGE_100
	if (m_pItemProt)
	{
		CTexture* pTex = m_pItemProt->GetTexture();
		if (pTex)
			pTex->Render(p2DRender, m_CtrlProtScroll->rect.TopLeft());
	}
#endif
	if (m_pItemSource && m_pItemDest)
		p2DRender->TextOut_EditString(m_CtrlOutputStats->rect.left + 25, m_CtrlOutputStats->rect.top + 5, m_editSwordOption, 0, 0, 2);

	CWndNeuz::OnDraw(p2DRender);
}

BOOL CWndWeaponMerge::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	if (pWndBase && pWndBase->GetWndId() == APP_MESSAGEBOX)
	{
		if (nID == IDYES)
		{
			pButtonStart->EnableWindow(FALSE);
			m_nStatus = 1;
		}
	}

	return TRUE;
}

BOOL CWndWeaponMerge::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_BUTTON1:
	{
		if (
#ifndef __AEGON_WEAPON_MERGE_100
			m_pItemProt &&
#endif
			m_pItemDest && m_pItemSource)
		{
			m_nStatus = 1;
			pButtonStart->EnableWindow(FALSE);
		}
		else if (m_pItemDest && m_pItemSource)
		{
#ifdef __AEGON_FIXES
			SAFE_DELETE(m_pWndConfirm);
			m_pWndConfirm = new CWndWeapnMergeConfirm();
			m_pWndConfirm->SetText(TID_MMI_TWOHANDEDWEAPONMERGE_CONFIRM02);
			m_pWndConfirm->Initialize(this);
#else
			CWndConfirm* pConfirm = (CWndConfirm*)g_WndMng.GetWndBase(APP_MESSAGEBOX);
			if (pConfirm)
				SAFE_DELETE(pConfirm);
			pConfirm = new CWndConfirm();
			pConfirm->SetText(TID_MMI_TWOHANDEDWEAPONMERGE_CONFIRM02);
			pConfirm->Initialize(this);
#endif
		}
		break;
	}
	case WIDC_BUTTON2:
	{
		Destroy();
		break;
	}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
void CWndWeaponMerge::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (m_pWndConfirm == pWndChild)
	{
		SAFE_DELETE(m_pWndConfirm);
		pWndChild = NULL;
	}
}
#endif

BOOL CWndWeaponMerge::SetItem(CItemElem* pItem, short nSlot /* = -1 */)
{
	if (!pItem)
		return FALSE;
	if (nSlot == -1)
	{
		if (!m_pItemDest)
			nSlot = 0;
		else if (!m_pItemSource)
			nSlot = 1;
#ifndef __AEGON_WEAPON_MERGE_100
		else if (!m_pItemProt && pItem->m_dwItemId == II_SYS_SYS_BX_MAGICGRINDSTONE)
			nSlot = 2;
#endif
		else
		{
			Clear();
			nSlot = 0;
		}
	}
	if (nSlot >= 0 && nSlot <= 1)
	{
		CItemElem* pSource = NULL, * pDest = NULL, ** pTargetItem = NULL;
		pSource = ((nSlot == 0) ? m_pItemSource : pItem);
		pDest = ((nSlot == 0) ? pItem : m_pItemDest);
		pTargetItem = ((nSlot == 0) ? &m_pItemDest : &m_pItemSource);
		int nMsg = RegisterWeapon(pItem, (nSlot == 0 ? pSource : pDest));
		if (nMsg == -1 && pSource && pDest)
		{
			if (!CompareItemLevel(pSource->GetProp(), pDest->GetProp()))
			{
				nMsg = TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT08;
			}
		}
		if (nMsg == -1)
		{
			if ((*pTargetItem) != NULL)
				(*pTargetItem)->SetExtra(0);
			*pTargetItem = pItem;
			if (*pTargetItem == m_pItemDest)
			{
				m_pDestModel = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_ITEM, m_pItemDest->m_dwItemId);
				m_pDestModel->InitDeviceObjects(g_Neuz.GetDevice());
			}
			else
			{
				g_WndMng.MakeMergeWeaponText(m_pItemSource, m_editSwordOption);
			}
			pItem->SetExtra(1);
			if (m_pItemSource && m_pItemDest)
				pButtonStart->EnableWindow(TRUE);
			return TRUE;
		}
		else if (nMsg != -2)
			g_WndMng.PutDefinedString(nMsg);
		return FALSE;
	}
#ifndef __AEGON_WEAPON_MERGE_100
	else if (nSlot == 2)
	{
		ItemProp* pItemProp = pItem->GetProp();
		if (pItemProp && pItemProp->dwID == II_SYS_SYS_BX_MAGICGRINDSTONE && !g_pPlayer->m_vtInfo.IsTrading(pItem))
		{
			if (m_pItemProt)
				m_pItemProt->SetExtra(0);
			pItem->SetExtra(1);
			m_pItemProt = pItem;
			return TRUE;
		}
	}
#endif
	return FALSE;
}

void CWndWeaponMerge::Clear(bool bProt /* = false */)
{
	if (m_pItemDest)
		m_pItemDest->SetExtra(0);
	if (m_pItemSource)
		m_pItemSource->SetExtra(0);
	m_pItemDest = m_pItemSource = NULL;
#ifndef __AEGON_WEAPON_MERGE_100
	if (bProt)
	{
		if (m_pItemProt)
			m_pItemProt->SetExtra(0);
		m_pItemProt = NULL;
	}
#endif
}

/*	Weapon Merge Cancel Window
 *	Window Id:	APP_WEAPONMERGE_CANCEL
 *	Controls:	WIDC_PICTURE1	=	Weapon Output Picture
 *				WIDC_ST_PENYA02	=	Cost Static Control
 *				WIDC_PC_DELAY	=	Gauge Time Left
 *				//Buttons Start
 *				WIDC_START		=	Finally Start the Process
 *				WIDC_CANCEL		=	Destroy
 *	Class:		CWndWeaponMergeCancel
 */

#define MS_DELAY_MERGE 3000

CWndWeaponMergeCancel::CWndWeaponMergeCancel()
{
	m_pItemElem = NULL;
	m_pStCost = NULL;
	m_pVBProcess = NULL;
	m_bRenderGauge = FALSE;
	m_clkEnd = 0;
	m_nStatus = 0;
	m_nCost = 0;

#ifdef __AEGON_FIXES
	m_pWndConfirm = NULL;
#endif
}

CWndWeaponMergeCancel::~CWndWeaponMergeCancel()
{
	if (m_pItemElem)
		m_pItemElem->SetExtra(0);
	DeleteDeviceObjects();

#ifdef __AEGON_FIXES
	SAFE_DELETE(m_pWndConfirm);
#endif
}

HRESULT CWndWeaponMergeCancel::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if (!m_pVBProcess)
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBProcess, NULL);

	m_TexGaugeProcess.SetInvalidate(m_pApp->m_pd3dDevice);
	return S_OK;
}
HRESULT CWndWeaponMergeCancel::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE(m_pVBProcess);
	m_TexGaugeProcess.Invalidate();
	return S_OK;
}
HRESULT CWndWeaponMergeCancel::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}
BOOL CWndWeaponMergeCancel::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_WEAPONMERGE_CANCEL, 0, CPoint(0, 0), pWndParent);
}

void CWndWeaponMergeCancel::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	RestoreDeviceObjects();

	m_pStCost = (CWndStatic*)GetDlgItem(WIDC_ST_PENYA02);
	m_pStart = (CWndButton*)GetDlgItem(WIDC_START);
	m_CtrlWeapon = GetWndCtrl(WIDC_PICTURE1);
	m_CtrlGauge = GetWndCtrl(WIDC_PC_DELAY);
#ifdef __AEGON_THEME_SWITCHER
	CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchLoadTexture(&m_TexGaugeProcess, g_Neuz.GetDevice(), "BarSky.tga", COLOR_ITEM_BG, TRUE);
#else
	m_TexGaugeProcess.LoadTexture(g_Neuz.GetDevice(), MakePath(DIR_THEME, "BarSky.tga"), COLOR_ITEM_BG, TRUE);
#endif
	m_pStCost->SetTitle("0");
	m_pStart->EnableWindow(FALSE);

	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.CreateApplet(APP_INVENTORY);
	assert(pWndInventory != NULL);

	pWndInventory->m_wndItemCtrl.SetDieFlag(TRUE);
	MoveParentCenter();
}

BOOL CWndWeaponMergeCancel::Process()
{
	if (m_nStatus == 1)
	{
		if (clock() >= m_clkEnd)
		{
			m_nStatus = 0;
			if (m_pItemElem)
			{
				g_DPlay.SendWeaponMergeCancel(m_pItemElem->m_dwObjId);
				m_pItemElem->SetExtra(0);
				m_pItemElem = NULL;
			}
		}
	}
	return TRUE;
}

void CWndWeaponMergeCancel::OnDraw(C2DRender* p2DRender)
{
	if (m_pItemElem)
	{
		CTexture* pTex = m_pItemElem->GetTexture();
		if (pTex)
			pTex->Render(p2DRender, m_CtrlWeapon->rect.TopLeft());
	}
	if (m_nStatus != 0)
	{
		float fPercent = 1.0f;
		if (m_nStatus == 1)
		{
			float fTimeDiff = (float)(m_clkEnd - clock());
			fPercent = 1.0f - fTimeDiff / MS_DELAY_MERGE;
		}

		CRect rect, rectTemp;
		int nWidth = (int)(m_CtrlGauge->rect.Width() * fPercent);
		rect = m_CtrlGauge->rect;
		rectTemp = rect;
		rectTemp.right = rectTemp.left + nWidth;
		ClientToScreen(rectTemp);
		m_bRenderGauge = m_pTheme->MakeGaugeVertex(m_pApp->m_pd3dDevice, &rectTemp, 0xffffffff, m_pVBProcess, &m_TexGaugeProcess);

		if (m_bRenderGauge)
			m_pTheme->RenderGauge(p2DRender->m_pd3dDevice, m_pVBProcess, &m_TexGaugeProcess);
	}
}

void CWndWeaponMergeCancel::OnMouseWndSurface(CPoint point)
{
	if (m_pItemElem && m_CtrlWeapon->rect.PtInRect(point))
	{
		PUT_TOOL_TIP(m_CtrlWeapon->rect, point, m_pItemElem);
	}
	CWndNeuz::OnMouseWndSurface(point);
}

void CWndWeaponMergeCancel::CalcCancelCosts()
{
	m_nCost = 0;
	if (m_pItemElem->GetMergedPiercingSize() > 0)
		m_nCost += 100000 + 100000 * m_pItemElem->GetMergedPiercingSize();
	if (m_pItemElem->GetMergedRandomOptItemId() > 0)
		m_nCost += 100000;
	if (m_pItemElem->GetMergedWeaponATK() > 0)
		m_nCost += 100000;
	CString str;
	str.Format("%d", m_nCost);
	m_pStCost->SetTitle(str);
}

BOOL CWndWeaponMergeCancel::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	if (m_CtrlWeapon->rect.PtInRect(point))
	{
		CItemElem* pItem = (CItemElem*)g_pPlayer->GetItemId(pShortcut->m_dwId);
		if (SetItem(pItem))
			return TRUE;
	}
	return CWndNeuz::OnDropIcon(pShortcut, point);
}

BOOL CWndWeaponMergeCancel::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_START:
	{
		if (m_pItemElem)
		{
			if (m_nStatus == 0)
			{
				if (m_nCost > g_pPlayer->GetGold())
				{
					CString strAdd;
					strAdd.Format(prj.GetText(TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT12), m_nCost);
					return FALSE;
				}
#ifdef __AEGON_FIXES
				SAFE_DELETE(m_pWndConfirm);
				m_pWndConfirm = new CWndWeapnMergeConfirm();
				m_pWndConfirm->SetText(TID_MMI_TWOHANDEDWEAPONMERGE_CANCEL_CONFIRM);
				m_pWndConfirm->Initialize(this);
#else
				CWndConfirm* pConfirm = (CWndConfirm*)g_WndMng.GetWndBase(APP_MESSAGEBOX);

				SAFE_DELETE(pConfirm);
				pConfirm = new CWndConfirm();
				pConfirm->SetText(TID_MMI_TWOHANDEDWEAPONMERGE_CANCEL_CONFIRM);
				pConfirm->Initialize(this);
#endif
			}
		}
		break;
	}
	case WIDC_CANCEL:
	{
		Destroy();
		break;
	}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

BOOL CWndWeaponMergeCancel::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	if (pWndBase && pWndBase->GetWndId() == APP_MESSAGEBOX)
	{
		if (nID == IDYES)
		{
			m_pStart->EnableWindow(FALSE);
			m_clkEnd = clock() + MS_DELAY_MERGE;
			m_nStatus = 1;
		}
	}
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
void CWndWeaponMergeCancel::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (m_pWndConfirm == pWndChild)
	{
		SAFE_DELETE(m_pWndConfirm);
		pWndChild = NULL;
	}
}
#endif

BOOL CWndWeaponMergeCancel::SetItem(CItemElem* pItem)
{
	if (pItem && pItem->IsMerged() && !g_pPlayer->m_vtInfo.IsTrading(pItem))
	{
		if (m_pItemElem)
			m_pItemElem->SetExtra(0);
		m_pItemElem = pItem;
		m_pItemElem->SetExtra(1);
		m_pStart->EnableWindow();
		CalcCancelCosts();
		return TRUE;
	}
	return FALSE;
}

void CWndWeapnMergeConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	CRect rect = GetClientRect();
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(72, 20);

	CRect rect2_1(x - size.cx - 10, y, (x - size.cx - 10) + size.cx, y + size.cy);
	CRect rect2_2(x + 5, y, (x + 5) + size.cx, y + size.cy);

	rect.DeflateRect(10, 10, 10, 35);
	m_wndText.AddWndStyle(WBS_VSCROLL);
	m_wndText.Create(WBS_NODRAWFRAME, rect, this, 0);

#ifdef __AEGON_THEME_SWITCHER
	m_wndText.SetString(m_strText, CAegonThemeSwitcherMng::GetInstance()->GetTextColor()); //0xffffffff
#else //__AEGON_THEME_SWITCHER
	m_wndText.SetString(m_strText, 0xff000000); //0xffffffff
#endif //__AEGON_THEME_SWITCHER
	m_wndText.ResetString();

	m_wndButton1.Create("", 0, rect2_1, this, IDYES);
	m_wndButton2.Create("", 0, rect2_2, this, IDNO);
#if defined(__BUTTONS_V19) && defined(__AEGON_THEME_SWITCHER)
	CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchSetTexture(&m_wndButton1, m_pApp->m_pd3dDevice, "Button72x20.tga");
	m_wndButton1.SetTitle("Yes");
	CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchSetTexture(&m_wndButton2, m_pApp->m_pd3dDevice, "Button72x20.tga");
	m_wndButton2.SetTitle("No");
#else
	m_wndButton1.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtYes.tga"));
	m_wndButton2.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtNo.tga"));
#endif
	m_wndButton1.FitTextureSize();
	m_wndButton2.FitTextureSize();

	MoveParentCenter();
}
void CWndWeapnMergeConfirm::SetText(CString strText)
{
	m_strText = strText;
}
void CWndWeapnMergeConfirm::SetText(int nID)
{
	m_strText = prj.GetText(nID);
}
BOOL CWndWeapnMergeConfirm::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	if (g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->GetMouseMode() == 1)
	{
		g_WndMng.m_pWndWorld->SetMouseMode(0);
	}
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_MESSAGEBOX, WBS_MODAL, CPoint(0, 0), pWndParent);
}
BOOL CWndWeapnMergeConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == IDYES || nID == IDNO)
	{
		GetParentWnd()->OnCommand(nID, message, this);
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

#endif //__WEAPON_MERGE