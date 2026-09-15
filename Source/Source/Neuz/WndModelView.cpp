#include "stdafx.h"

#ifdef __MODEL_VIEW
#include "WndModelView.h"
#include "ResData.h"
#include "defineObj.h"
#include "DPClient.h"
extern CDPClient g_DPlay;

CWndModelView::CWndModelView(void)
{
	m_pTexture = NULL;
	m_pModel = NULL;
	m_pPet = NULL;
	m_pEgg = NULL;
	m_pMount = NULL;
#ifdef __ANI_WING
	m_pAniMask = NULL;
#endif //__ANI_WING
#ifdef __LNB_ANIMATED_HATS
	m_pAnimatedHat = NULL;
#endif //__LNB_ANIMATED_HATS

	m_pRotateCheck = NULL;

	m_bLButtonDownRot = false;
	m_bRButtonDownRot = false;
	m_fRot = 0.0f;
	m_fRot1 = 0.0f;
	m_fWheel = 6.0f;
	vecPos123 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vecPos1234 = D3DXVECTOR3(0.0f, -0.6f, 0.0f);

	m_Mover.Init();

#ifdef __AEGON_PREM_MOTIONS
	m_bMotionPreview = FALSE;
	m_pMotionProp = NULL;
#endif // __AEGON_PREM_MOTIONS
}
CWndModelView::~CWndModelView(void)
{
	m_pTexture = NULL;
	SAFE_DELETE(m_pModel);
	SAFE_DELETE(m_pPet);
	SAFE_DELETE(m_pEgg);
	SAFE_DELETE(m_pMount);
#ifdef __ANI_WING
	SAFE_DELETE(m_pAniMask);
#endif //__ANI_WING
#ifdef __LNB_ANIMATED_HATS
	SAFE_DELETE(m_pAnimatedHat);
#endif //__LNB_ANIMATED_HATS

	m_fRot = 0.0f;
	m_fWheel = 6.0f;
	vecPos123 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vecPos1234 = D3DXVECTOR3(0.0f, -0.6f, 0.0f);

}
BOOL CWndModelView::Process()
{
	if (m_pModel)
		m_pModel->FrameMove();
	if (m_pPet)
		m_pPet->FrameMove();
	if (m_pEgg)
		m_pEgg->FrameMove();
	if (m_pMount)
		m_pMount->FrameMove();
#ifdef __ANI_WING
	if (m_pAniMask)
		m_pAniMask->FrameMove();
#endif //__ANI_WING
#ifdef __LNB_ANIMATED_HATS
	if (m_pAnimatedHat)
		m_pAnimatedHat->FrameMove();
#endif //__LNB_ANIMATED_HATS

	return TRUE;
}
BOOL CWndModelView::Initialize(CWndBase* pWndParent, DWORD dwType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_MODEL_VIEW, 0, CPoint(0, 0), pWndParent);
}
void CWndModelView::OnDraw(C2DRender* p2DRender)
{
	
#ifdef __AEGON_PREM_MOTIONS
	CWndButton* pButtUnlock = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
	if (pButtUnlock) 
	{
		if (m_bMotionPreview && m_pMotionProp && !easy_find(g_pPlayer->m_vUnlockedMotions, m_pMotionProp->dwUnlockItemID))
		{
			pButtUnlock->SetVisible(TRUE);
		}
		else
		{
			pButtUnlock->SetVisible(FALSE);
		}
	}
	
#endif // __AEGON_PREM_MOTIONS
	if (m_pMount)
	{
		CWndModelView* pWndView = (CWndModelView*)g_WndMng.GetWndBase(APP_MODEL_VIEW);
		CRect rect = pWndView->GetClientRect();
		LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM1);

		int nWidthClient = pWndView->GetClientRect().Width() - 110;
		CRect rectTemp;
		CPoint point;
		point = CPoint(lpFace->rect.left - 12, lpFace->rect.top - 22);

		LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0);

		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

		pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		D3DVIEWPORT9 viewport;

		lpFace->rect = rect;
		viewport.X = p2DRender->m_ptOrigin.x + lpFace->rect.left;
		viewport.Y = p2DRender->m_ptOrigin.y + lpFace->rect.top;
		viewport.Width = lpFace->rect.Width();
		viewport.Height = lpFace->rect.Height();
		viewport.MinZ = 0.0f;
		viewport.MaxZ = 1.0f;

		pd3dDevice->SetViewport(&viewport);

		D3DXMATRIX matProj;
		D3DXMatrixIdentity(&matProj);
		FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;

		FLOAT fov = D3DX_PI / m_fWheel;//796.0f;
		FLOAT h = cos(fov / 2) / sin(fov / 2);
		FLOAT w = h * fAspect;
		D3DXMatrixOrthoLH(&matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane);
		pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

		D3DXMATRIX  matView;
		D3DXMATRIXA16 matWorld;
		D3DXMATRIXA16 matScale;
		D3DXMATRIXA16 matRot1, matRot2;
		D3DXMATRIXA16 matTrans;

		D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matRot1);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);

		D3DXVECTOR3 vecPos;
		D3DXVECTOR3 vecLookAt;
		float fScale = 1.0f;

		CObject3D* pObj3D = m_pMount->GetObject3D();
		if (!pObj3D)
			return;

		vecPos = pObj3D->m_vEvent[0];

		float fModelScale = m_pMount->m_pModelElem->m_fScale;
		if (fModelScale < 1.0f && fModelScale > 0.001f)
			vecPos *= (fModelScale - fModelScale * (0.5f + (1.0f - fModelScale) * 0.01f));
		else if (fModelScale > 1.0f)
			vecPos *= (fModelScale - fModelScale * (0.9f + fModelScale * 0.01f));

		float m_fRadius = m_pMount->GetRadius();

		vecPos.x += 0.5f;
		vecPos.y += 1.8f;
		vecPos.z -= (3.0f);

		vecLookAt.x = -0.24f;
		vecLookAt.y = 0.28f;
		vecLookAt.z = 1.0f;
		fScale = (1 / m_fRadius) * 1.5f;

		D3DXMatrixRotationY(&matRot1, D3DXToRadian(m_fRot));
		D3DXMatrixRotationX(&matWorld, D3DXToRadian(m_fRot1));
		D3DXMatrixTranslation(&matTrans, vecPos123.x, vecPos123.y, vecPos123.z);

		D3DXMatrixScaling(&matScale, fScale, fScale, fScale);

		D3DXMatrixLookAtLH(&matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
		pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

		D3DXMatrixMultiply(&matWorld, &matWorld, &matScale);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matRot1);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);
		pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);


		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, CWorld::m_dwBgColor, 1.0f, 0);

		::SetTransformView(matView);
		::SetTransformProj(matProj);

		::SetLight(FALSE);
		::SetFog(FALSE);
		SetDiffuse(1.0f, 1.0f, 1.0f);
		SetAmbient(1.0f, 1.0f, 1.0f);

		m_pMount->SetTextureEx(m_pMount->m_pModelElem->m_nTextureEx);
		m_pMount->Render(pd3dDevice, &matWorld);

		pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
	else if (m_pPet)
	{
		CWndModelView* pWndView = (CWndModelView*)g_WndMng.GetWndBase(APP_MODEL_VIEW);
		CRect rect = pWndView->GetClientRect();
		LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM1);

		int nWidthClient = pWndView->GetClientRect().Width() - 110;
		CRect rectTemp;
		CPoint point;
		point = CPoint(lpFace->rect.left - 12, lpFace->rect.top - 22);

		LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0);

		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

		pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		D3DVIEWPORT9 viewport;

		lpFace->rect = rect;
		viewport.X = p2DRender->m_ptOrigin.x + lpFace->rect.left;
		viewport.Y = p2DRender->m_ptOrigin.y + lpFace->rect.top;
		viewport.Width = lpFace->rect.Width();
		viewport.Height = lpFace->rect.Height();
		viewport.MinZ = 0.0f;
		viewport.MaxZ = 1.0f;

		pd3dDevice->SetViewport(&viewport);

		// ??·??§??
		D3DXMATRIX matProj;
		D3DXMatrixIdentity(&matProj);
		FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;

		FLOAT fov = D3DX_PI / m_fWheel;//796.0f;
		FLOAT h = cos(fov / 2) / sin(fov / 2);
		FLOAT w = h * fAspect;
		D3DXMatrixOrthoLH(&matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane);
		pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

		D3DXMATRIX  matView;

		// ??µ?
		D3DXMATRIXA16 matWorld;
		D3DXMATRIXA16 matScale;
		D3DXMATRIXA16 matRot1, matRot2;
		D3DXMATRIXA16 matTrans;

		D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matRot1);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);

		D3DXVECTOR3 vecPos;
		D3DXVECTOR3 vecLookAt;
		float fScale = 1.0f;

		CObject3D* pObj3D = m_pPet->GetObject3D();
		if (!pObj3D)
			return;

		vecPos = pObj3D->m_vEvent[0];

		float fModelScale = m_pPet->m_pModelElem->m_fScale;
		if (fModelScale < 1.0f && fModelScale > 0.001f)
			vecPos *= (fModelScale - fModelScale * (0.5f + (1.0f - fModelScale) * 0.01f));
		else if (fModelScale > 1.0f)
			vecPos *= (fModelScale - fModelScale * (0.9f + fModelScale * 0.01f));

		float m_fRadius = m_pPet->GetRadius();

		vecPos.x += 0.5f;
		vecPos.y += 1.8f;
		vecPos.z -= (3.0f);

		vecLookAt.x = -0.24f;
		vecLookAt.y = 0.28f;
		vecLookAt.z = 1.0f;
		fScale = (1 / m_fRadius) * 1.5f;

		D3DXMatrixRotationY(&matRot1, D3DXToRadian(m_fRot));
		D3DXMatrixRotationX(&matWorld, D3DXToRadian(m_fRot1));
		D3DXMatrixTranslation(&matTrans, vecPos123.x, vecPos123.y, vecPos123.z);

		D3DXMatrixScaling(&matScale, fScale, fScale, fScale);

		D3DXMatrixLookAtLH(&matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
		pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

		D3DXMatrixMultiply(&matWorld, &matWorld, &matScale);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matRot1);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);
		pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);


		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, CWorld::m_dwBgColor, 1.0f, 0);

		::SetTransformView(matView);
		::SetTransformProj(matProj);

		::SetLight(FALSE);
		::SetFog(FALSE);
		SetDiffuse(1.0f, 1.0f, 1.0f);
		SetAmbient(1.0f, 1.0f, 1.0f);

		m_pPet->SetTextureEx(m_pPet->m_pModelElem->m_nTextureEx);
		m_pPet->Render(pd3dDevice, &matWorld);

		pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
	else if (m_pEgg)
	{
		CWndModelView* pWndView = (CWndModelView*)g_WndMng.GetWndBase(APP_MODEL_VIEW);
		CRect rect = pWndView->GetClientRect();
		LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM1);

		int nWidthClient = pWndView->GetClientRect().Width() - 110;
		CRect rectTemp;
		CPoint point;
		point = CPoint(lpFace->rect.left - 12, lpFace->rect.top - 22);

		LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0);

		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

		pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		D3DVIEWPORT9 viewport;

		lpFace->rect = rect;
		viewport.X = p2DRender->m_ptOrigin.x + lpFace->rect.left;
		viewport.Y = p2DRender->m_ptOrigin.y + lpFace->rect.top;
		viewport.Width = lpFace->rect.Width();
		viewport.Height = lpFace->rect.Height();
		viewport.MinZ = 0.0f;
		viewport.MaxZ = 1.0f;

		pd3dDevice->SetViewport(&viewport);

		D3DXMATRIX matProj;
		D3DXMatrixIdentity(&matProj);
		FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;

		FLOAT fov = D3DX_PI / m_fWheel;//796.0f;
		FLOAT h = cos(fov / 2) / sin(fov / 2);
		FLOAT w = h * fAspect;
		D3DXMatrixOrthoLH(&matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane);
		pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

		D3DXMATRIX  matView;

		D3DXMATRIXA16 matWorld;
		D3DXMATRIXA16 matScale;
		D3DXMATRIXA16 matRot1, matRot2;
		D3DXMATRIXA16 matTrans;

		D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matRot1);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);

		D3DXVECTOR3 vecPos;
		D3DXVECTOR3 vecLookAt;
		float fScale = 1.0f;

		CObject3D* pObj3D = m_pEgg->GetObject3D();
		if (!pObj3D)
			return;

		vecPos = pObj3D->m_vEvent[0];

		float fModelScale = m_pEgg->m_pModelElem->m_fScale;
		if (fModelScale < 1.0f && fModelScale > 0.001f)
			vecPos *= (fModelScale - fModelScale * (0.5f + (1.0f - fModelScale) * 0.01f));
		else if (fModelScale > 1.0f)
			vecPos *= (fModelScale - fModelScale * (0.9f + fModelScale * 0.01f));

		float m_fRadius = m_pEgg->GetRadius();

		vecPos.x += 0.5f;
		vecPos.y += 1.8f;
		vecPos.z -= (3.0f);

		vecLookAt.x = -0.24f;
		vecLookAt.y = 0.28f;
		vecLookAt.z = 1.0f;
		fScale = (1 / m_fRadius) * 1.5f;

		D3DXMatrixRotationY(&matRot1, D3DXToRadian(m_fRot));
		D3DXMatrixRotationX(&matWorld, D3DXToRadian(m_fRot1));
		D3DXMatrixTranslation(&matTrans, vecPos123.x, vecPos123.y, vecPos123.z);

		D3DXMatrixScaling(&matScale, fScale, fScale, fScale);

		D3DXMatrixLookAtLH(&matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
		pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

		D3DXMatrixMultiply(&matWorld, &matWorld, &matScale);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matRot1);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);
		pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);


		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, CWorld::m_dwBgColor, 1.0f, 0);

		::SetTransformView(matView);
		::SetTransformProj(matProj);

		::SetLight(FALSE);
		::SetFog(FALSE);
		SetDiffuse(1.0f, 1.0f, 1.0f);
		SetAmbient(1.0f, 1.0f, 1.0f);

		m_pEgg->SetTextureEx(m_pEgg->m_pModelElem->m_nTextureEx);
		m_pEgg->Render(pd3dDevice, &matWorld);

		pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
	else if (m_pModel)
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
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_ARGB(255, 255, 255, 255));
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		D3DVIEWPORT9 viewport;

		CWndModelView* pWndView = (CWndModelView*)g_WndMng.GetWndBase(APP_MODEL_VIEW);
		CRect rect = pWndView->GetClientRect();
		LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM1);
		lpFace->rect = rect;
		viewport.X = p2DRender->m_ptOrigin.x + 5;
		viewport.Y = p2DRender->m_ptOrigin.y + 5;
		viewport.Width = rect.Width();
		viewport.Height = rect.Height();

		viewport.MinZ = 0.0f;
		viewport.MaxZ = 1.0f;
		pd3dDevice->SetViewport(&viewport);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0);

		D3DXMATRIX matProj;
		D3DXMatrixIdentity(&matProj);
		FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;
		D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / m_fWheel, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane);
		pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

		D3DXMATRIX  matView;
		D3DXVECTOR3 vecLookAt(0.0f, 0.0f, 1.0f);
		D3DXVECTOR3 vecPos(0.0f, 0.5f, -3.5f);

		D3DXMatrixLookAtLH(&matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

		pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

		D3DXMATRIXA16 matWorld;
		D3DXMATRIXA16 matScale;
		D3DXMATRIXA16 matRot;
		D3DXMATRIXA16 matTrans;

		D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matRot);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);

		D3DXMatrixRotationY(&matRot, D3DXToRadian(m_fRot));
		D3DXMatrixRotationX(&matWorld, D3DXToRadian(m_fRot1));
		D3DXMatrixTranslation(&matTrans, vecPos1234.x, vecPos1234.y, vecPos1234.z);

		D3DXMatrixScaling(&matScale, 1.0f, 1.0f, 1.0f);

		D3DXMatrixMultiply(&matWorld, &matWorld, &matScale);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matRot);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);
		pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

		::SetLight(FALSE);
		::SetFog(FALSE);
		SetDiffuse(1.0f, 1.0f, 1.0f);
		SetAmbient(1.0f, 1.0f, 1.0f);

		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = g_pPlayer->m_fHairColorR;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = g_pPlayer->m_fHairColorG;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = g_pPlayer->m_fHairColorB;

		D3DXVECTOR4 vConst(1.0f, 1.0f, 1.0f, 1.0f);
#ifdef __YENV
		g_Neuz.m_pEffect->SetVector(g_Neuz.m_hvFog, &vConst);
#else
		pd3dDevice->SetVertexShaderConstantF(95, (float*)&vConst, 1);
#endif
		::SetTransformView(matView);
		::SetTransformProj(matProj);

		DWORD dwId = m_Mover.m_aEquipInfo[PARTS_CAP].dwId;
		ItemProp* pItemProp = NULL;

#ifdef __AEGON_FIXES
		m_pModel->SetEffectOff(PARTS_HAIR, XE_HIDE);
		m_pModel->SetEffectOff(PARTS_HEAD, XE_HIDE);
#endif

		if (dwId != NULL_ID)
		{
			O3D_ELEMENT* pElement = NULL;
			pItemProp = prj.GetItemProp(dwId);

			if (pItemProp && pItemProp->dwBasePartsIgnore != -1)
			{
				pElement = m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
			}

#ifdef __LOOKCHANGE
			if (m_Mover.m_aEquipInfo[PARTS_HAT].dwLook != NULL_ID)
				dwId = m_Mover.m_aEquipInfo[PARTS_HAT].dwLook;
			else
#endif //__LOOKCHANGE
			{
				dwId = m_Mover.m_aEquipInfo[PARTS_HAT].dwId;
			}

			if (dwId != NULL_ID
#ifdef __AEGON_FIXES
			&& !m_Mover.m_abHideCoat[0]
#endif
				)
			{
				if (!(m_Mover.m_aEquipInfo[PARTS_HAT].byFlag & CItemElem::expired))
				{
					pItemProp = prj.GetItemProp(dwId);

#ifdef __LNB_ANIMATED_HATS
					if (pItemProp && pItemProp->dwItemKind3 == IK3_ANIHAT)
					{
						if (pItemProp->dwBasePartsIgnore != -1)
						{
							if (pItemProp->dwBasePartsIgnore != PARTS_HEAD)
							{
								((CModelObject*)m_pModel)->SetEffectOff(PARTS_HAIR, XE_HIDE);
							}
							((CModelObject*)m_pModel)->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
						}
					}
					else
#endif // __LNB_ANIMATED_HATS
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
#ifdef __LOOKCHANGE
			if (m_Mover.m_aEquipInfo[PARTS_HAT].dwLook != NULL_ID)
				dwId = m_Mover.m_aEquipInfo[PARTS_HAT].dwLook;
			else
#endif //__LOOKCHANGE
			{
				dwId = m_Mover.m_aEquipInfo[PARTS_HAT].dwId;
			}

			if (dwId != NULL_ID)
			{
				if (!(m_Mover.m_aEquipInfo[PARTS_HAT].byFlag & CItemElem::expired))
				{
					pItemProp = prj.GetItemProp(dwId);

#ifdef __LNB_ANIMATED_HATS
					if (pItemProp && pItemProp->dwItemKind3 == IK3_ANIHAT)
					{
						if (pItemProp->dwBasePartsIgnore != -1)
						{
							if (pItemProp->dwBasePartsIgnore != PARTS_HEAD)
							{
								((CModelObject*)m_pModel)->SetEffectOff(PARTS_HAIR, XE_HIDE);
							}
							((CModelObject*)m_pModel)->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
						}
					}
					else
#endif // __LNB_ANIMATED_HATS
					if (pItemProp && pItemProp->dwBasePartsIgnore != -1)
					{
						if (pItemProp->dwBasePartsIgnore == PARTS_HEAD)
							((CModelObject*)m_pModel)->SetEffect(PARTS_HAIR, XE_HIDE);

						((CModelObject*)m_pModel)->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
					}
				}
			}
		}
		m_Mover.OverCoatItemRenderCheck(m_pModel);

#ifdef __YENV
		SetLightVec(D3DXVECTOR3(0.0f, 0.0f, 1.0f));
#endif //__YENV

#ifdef __ANI_WING
		D3DXMATRIX mMask = matWorld;
		if (m_pAniMask)
		{
			m_pAniMask->m_nNoEffect = m_pModel->m_nNoEffect;
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
			m_pAniMask->SetTextureEx(m_pAniMask->m_pModelElem->m_nTextureEx);
			m_pAniMask->Render(p2DRender->m_pd3dDevice, &mMask);
			m_pAniMask->m_nNoEffect = 0;
		}
#endif //__ANI_WING

#ifdef __LNB_ANIMATED_HATS
		D3DXMATRIX mAniHat = matWorld;
		if (m_pAnimatedHat)
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

				pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
				D3DXMatrixTranslation(&matEvent, 0.2f, 0.0f, 0.1f);

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

				D3DXMatrixMultiply(&mAniHat, &matEvent, &matWorld);
			}
			m_pAnimatedHat->SetTextureEx(m_pAnimatedHat->m_pModelElem->m_nTextureEx);
			m_pAnimatedHat->Render(p2DRender->m_pd3dDevice, &mAniHat);
			m_pAnimatedHat->m_nNoEffect = 0;
		}
#endif //__LNB_ANIMATED_HATS

		m_pModel->Render(p2DRender->m_pd3dDevice, &matWorld);
		p2DRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		p2DRender->m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	}
}
void CWndModelView::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();
	RestoreDeviceObjects();
	MoveParentCenter();
	SAFE_DELETE(m_pModel);
	SAFE_DELETE(m_pPet);
	SAFE_DELETE(m_pEgg);
	SAFE_DELETE(m_pMount);
#ifdef __ANI_WING
	SAFE_DELETE(m_pAniMask);
#endif //__ANI_WING
#ifdef __LNB_ANIMATED_HATS
	SAFE_DELETE(m_pAnimatedHat);
#endif //__LNB_ANIMATED_HATS

	m_pRotateCheck = (CWndButton*)GetDlgItem(WIDC_CHECK);
	if (m_pRotateCheck)
	{
		m_pRotateCheck->SetCheck(0);
	}

	m_fRot = 0.0f;
	m_fRot1 = 0.0f;
	m_fWheel = 6.0f;
	vecPos123 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vecPos1234 = D3DXVECTOR3(0.0f, -0.6f, 0.0f);
	SetSex(g_pPlayer->GetSex());
}
void CWndModelView::SetSex(BOOL bSex)
{
	if (bSex == g_pPlayer->m_bySex)
	{
		for (int i = 0; i < MAX_HUMAN_PARTS; ++i)
		{
			if (g_pPlayer->GetEquipItem(i))
			{
				m_Mover.m_aEquipInfo[i].dwId = g_pPlayer->GetEquipItem(i)->m_dwItemId;
#ifdef __LOOKCHANGE
				m_Mover.m_aEquipInfo[i].dwLook = g_pPlayer->GetEquipItem(i)->GetLook();
#endif //__LOOKCHANGE
			}
		}
#ifdef __ANI_WING
		if (g_pPlayer->m_pMask)
		{
#ifdef __LOOKCHANGE
			if (m_Mover.m_aEquipInfo[PARTS_MASK].dwLook != NULL_ID)
				m_pAniMask = (CModelObject*)prj.m_modelMng.LoadModel(D3DDEVICE, OT_ITEM, m_Mover.m_aEquipInfo[PARTS_MASK].dwLook);
			else
#endif
				m_pAniMask = (CModelObject*)prj.m_modelMng.LoadModel(D3DDEVICE, OT_ITEM, m_Mover.m_aEquipInfo[PARTS_MASK].dwId);
			m_pAniMask->LoadMotion(m_pAniMask->GetMotionFileName("stand"));
		}
#endif
	}
	else
	{
		for (int i = 0; i < MAX_HUMAN_PARTS; ++i)
		{
			if (g_pPlayer->GetEquipItem(i))
			{
				if (i == PARTS_RWEAPON || i == PARTS_LWEAPON || i == PARTS_SHIELD)
				{
					m_Mover.m_aEquipInfo[i].dwId = g_pPlayer->GetEquipItem(i)->m_dwItemId;
#ifdef __LOOKCHANGE
					m_Mover.m_aEquipInfo[i].dwLook = g_pPlayer->GetEquipItem(i)->GetLook();
#endif //__LOOKCHANGE
				}
				else
				{
					m_Mover.m_aEquipInfo[i].dwId = NULL_ID;
#ifdef __LOOKCHANGE
					m_Mover.m_aEquipInfo[i].dwLook = NULL_ID;
#endif //__LOOKCHANGE
				}
			}
		}
	}
setModel:
	int nMover = (bSex == SEX_MALE ? MI_MALE : MI_FEMALE);

	m_Mover.m_bySex = bSex;

	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE);
	prj.m_modelMng.LoadMotion(m_pModel, OT_MOVER, nMover, MTI_STAND);

	UpdMover();

	m_pModel->InitDeviceObjects(g_Neuz.GetDevice());
}
BOOL CWndModelView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_pWndRoot->GetWindowRect().PtInRect(pt))
	{
		if (zDelta < 0)
			m_fWheel -= 0.1f;
		else
			m_fWheel += 0.1f;
	}
	return true;
}
void CWndModelView::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	if (GetWndCtrl(WIDC_CUSTOM1)->rect.PtInRect(point))
	{
		m_ptOld = point;
		m_bLButtonDownRot = TRUE;
	}
}
void CWndModelView::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_bLButtonDownRot = FALSE;
}
void CWndModelView::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	if (GetWndCtrl(WIDC_CUSTOM1)->rect.PtInRect(point))
	{
		m_ptOld1 = point;
		m_bRButtonDownRot = TRUE;
	}
}
void CWndModelView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_bRButtonDownRot = FALSE;
}
void CWndModelView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bLButtonDownRot)
	{
		m_fRot += (m_ptOld.x - point.x) * 0.75f;
		if (m_pRotateCheck->GetCheck())
		{
			m_fRot1 += (m_ptOld.y - point.y) * 0.75f;
		}
		m_ptOld = point;
	}
	if (m_bRButtonDownRot)
	{
		vecPos1234.x -= (m_ptOld1.x - point.x) * 0.01f;
		vecPos1234.y += (m_ptOld1.y - point.y) * 0.01f;
		m_ptOld1 = point;
	}
}
void CWndModelView::UpdItem(DWORD dwParts, DWORD dwID, int nOption)
{
	int nArryEquip1[4] = { PARTS_CAP, PARTS_UPPER_BODY, PARTS_HAND, PARTS_FOOT };
	int nArryEquip2[4] = { PARTS_HAT, PARTS_CLOTH, PARTS_GLOVE, PARTS_BOOTS };
	for (int i = 0; i < 4; i++)
	{
		if (dwParts == nArryEquip1[i])
		{
			m_Mover.m_aEquipInfo[nArryEquip2[i]].dwId = NULL_ID;
#ifdef __LOOKCHANGE
			m_Mover.m_aEquipInfo[nArryEquip2[i]].dwLook = NULL_ID;
#endif //__LOOKCHANGE
			break;
		}
		if (dwParts == nArryEquip2[i])
		{
			m_Mover.m_aEquipInfo[nArryEquip1[i]].dwId = NULL_ID;
#ifdef __LOOKCHANGE
			m_Mover.m_aEquipInfo[nArryEquip1[i]].dwLook = NULL_ID;
#endif //__LOOKCHANGE
			break;
		}
	}
	m_Mover.m_aEquipInfo[dwParts].dwId = dwID;
#ifdef __LOOKCHANGE
	m_Mover.m_aEquipInfo[dwParts].dwLook = NULL_ID;
#endif //__LOOKCHANGE
}
void CWndModelView::UpdMover()
{
	if (m_Mover.m_bySex == g_pPlayer->m_bySex)
		CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, m_Mover.m_aEquipInfo, m_pModel, NULL);
	else
		CMover::UpdateParts(m_Mover.m_bySex, m_Mover.m_dwSkinSet, m_Mover.m_dwFace, m_Mover.m_dwHairMesh, m_Mover.m_dwHeadMesh, m_Mover.m_aEquipInfo, m_pModel, NULL);
}
bool CWndModelView::SetPart(CItemElem* pItem)
{
	ItemProp* pProp = pItem->GetProp();
	SAFE_DELETE(m_pPet);
	SAFE_DELETE(m_pEgg);
	SAFE_DELETE(m_pMount);
	if (pProp)
	{
		if (pProp->dwItemSex != NULL_ID)
		{
			if (pProp->dwItemSex != m_Mover.m_bySex)
				SetSex(pProp->dwItemSex);
		}
		else
		{
			if (pProp->dwItemKind1 == IK1_WEAPON)
			{
				if (pProp->dwHanded == HD_TWO) 
				{
					m_Mover.m_aEquipInfo[PARTS_LWEAPON].dwId = NULL_ID;
					m_Mover.m_aEquipInfo[PARTS_LWEAPON].dwLook = NULL_ID;

					m_Mover.m_aEquipInfo[PARTS_RWEAPON].dwId = NULL_ID;
					m_Mover.m_aEquipInfo[PARTS_RWEAPON].dwLook = NULL_ID;

					m_Mover.m_aEquipInfo[PARTS_SHIELD].dwId = NULL_ID;
					m_Mover.m_aEquipInfo[PARTS_SHIELD].dwLook = NULL_ID;
					UpdMover();
				}				
			}

			if (pProp->dwItemKind1 != IK1_WEAPON && pProp->dwItemSex != SEX_SEXLESS)
				SetSex(g_pPlayer->m_bySex);
		}

		if (pProp->dwParts > 1 && pProp->dwParts < MAX_HUMAN_PARTS && pProp->dwParts != PARTS_RIDE)
		{
			if (pProp->dwParts >= PARTS_NECKLACE1 && pProp->dwParts <= PARTS_BULLET)
				return false;

#ifdef __ANI_WING
			if (pProp->dwParts == PARTS_MASK)
			{
				if (pProp->dwItemKind3 == IK3_ANI_WING)
				{
					m_pAniMask = NULL;
					m_Mover.m_aEquipInfo[pProp->dwParts].dwId = NULL_ID;
					m_Mover.m_aEquipInfo[pProp->dwParts].dwLook = NULL_ID;
					UpdMover();
					m_pAniMask = (CModelObject*)prj.m_modelMng.LoadModel(D3DDEVICE, OT_ITEM, pProp->dwID);
					m_pAniMask->LoadMotion(m_pAniMask->GetMotionFileName("stand"));
					return true;
				}
				else
				{
					m_pAniMask = NULL;
					UpdMover();
					UpdItem(pProp->dwParts, pProp->dwID, pItem->GetAttrOption());
				}
			}
			else
#endif //__ANI_WING

#ifdef __LNB_ANIMATED_HATS
			if (pProp->dwParts == PARTS_HAT)
			{
				if (pProp->dwItemKind3 == IK3_ANIHAT)
				{
					m_pAnimatedHat = NULL;
					m_Mover.m_aEquipInfo[pProp->dwParts].dwId = NULL_ID;
					m_Mover.m_aEquipInfo[pProp->dwParts].dwLook = NULL_ID;
					UpdMover();
					m_pAnimatedHat = (CModelObject*)prj.m_modelMng.LoadModel(D3DDEVICE, OT_ITEM, pProp->dwID);
					m_pAnimatedHat->LoadMotion(m_pAnimatedHat->GetMotionFileName("stand"));
					return true;
				}
				else
				{
					m_pAnimatedHat = NULL;
					UpdMover();
					UpdItem(pProp->dwParts, pProp->dwID, pItem->GetAttrOption());
				}
			}
			else
#endif //__LNB_ANIMATED_HATS

				UpdItem(pProp->dwParts, pProp->dwID, pItem->GetAttrOption());
		}
		else
		{
			if (pProp->dwItemKind1 == IK1_SYSTEM && pProp->dwItemKind2 == IK2_SYSTEM && pProp->dwItemKind3 == IK3_SCROLL)
			{
				PPACKITEMELEM pPackItemElem = CPackItem::GetInstance()->Open(pProp->dwID);
				if (!pPackItemElem)
					return false;
				if (pPackItemElem->nSize < 1)
					return false;
				for (int i = 0; i < pPackItemElem->nSize; i++)
				{
					ItemProp* pItemProp = prj.GetItemProp(pPackItemElem->adwItem[i]);

					if (pItemProp->dwItemSex != g_pPlayer->GetSex() && pItemProp->dwItemSex != NULL_ID)
					{
						if (m_Mover.m_bySex != pItemProp->dwItemSex)
							SetSex(pItemProp->dwItemSex);
					}
					if (pItemProp->dwParts < 2 || pItemProp->dwParts >= MAX_HUMAN_PARTS)
						return false;
					UpdItem(pItemProp->dwParts, pItemProp->dwID, pItem->GetAttrOption());
				}
			}
			else if (pProp->dwItemKind3 == IK3_PET)
			{
				m_pPet = (CModelObject*)prj.m_modelMng.LoadModel(D3DDEVICE, OT_MOVER, pProp->dwLinkKind);
				m_pPet->LoadMotion(m_pPet->GetMotionFileName("stand"));
			}
			else if (pProp->dwItemKind3 == IK3_EGG)
			{
				int dwEggID = MI_PET_EGG;
				switch (pProp->dwID)
				{
					case II_PET_WHITETIGER01: dwEggID = MI_PET_WHITETIGER01; break;
					case II_PET_BARBARYLION01: dwEggID = MI_PET_BARBARYLION01; break;
					case II_PET_RABBIT02: dwEggID = MI_PET_RABBIT02; break;
					case II_PET_DRAGON01: dwEggID = MI_PET_DRAGON01; break;
					case II_PET_UNICORN01: dwEggID = MI_PET_UNICORN01; break;
					case II_PET_NINEFOX01: dwEggID = MI_PET_NINEFOX01; break;
					case II_PET_EAGLE01: dwEggID = MI_PET_EAGLE01; break;
				}
				m_pEgg = (CModelObject*)prj.m_modelMng.LoadModel(D3DDEVICE, OT_MOVER, dwEggID);
				m_pEgg->LoadMotion(m_pEgg->GetMotionFileName(pProp->dwID == II_PET_EGG ? "idle" : "stand"));
			}
			else if (pProp->dwItemKind3 == IK3_MOUNT)
			{
				MOUNT_ANIMATION* pMount = prj.GetMountAnimation(pProp->dwID);
				m_pMount = (CModelObject*)prj.m_modelMng.LoadModel(D3DDEVICE, OT_ITEM, pProp->dwID);
				CString strMotion = m_pMount->GetMotionFileName(pMount->m_aMonsterAnimation.find(MTI_STAND)->second);
				assert(strMotion != "");
				m_pMount->LoadMotion(strMotion);
				m_pMount->SetMotionBlending(TRUE);
				m_pMount->SetLoop(ANILOOP_LOOP);
			}
#ifdef __AEGON_PREM_MOTIONS
			else if (pProp->dwItemKind3 == IK3_UL_MOTION) 
			{
				if (m_pModel) 
				{
					MotionProp* motionProp = prj.GetMotionPropByUnlockItem(pProp->dwID);
					if (motionProp) 
					{
						BOOL bResult = prj.m_modelMng.LoadMotion(m_pModel, OT_MOVER, g_pPlayer->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE, motionProp->dwMotion);
						m_pModel->InitDeviceObjects(g_Neuz.GetDevice());
						m_bMotionPreview = TRUE;
						m_pMotionProp = motionProp;
						return true;
					}
				}
				return false;
			}
#endif // __AEGON_PREM_MOTIONS
			else
				return false;
		}
		UpdMover();
		return true;
	}
	return false;
}
void CWndModelView::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	AdjustMinRect(&rectWnd, 200, 300);
	AdjustMaxRect(&rectWnd, 495, 590);
	CWndNeuz::SetWndRect(rectWnd, bOnSize);
}
void CWndModelView::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);

	CWndButton* pWndButtonReset = (CWndButton*)GetDlgItem(WIDC_BUTTON);
	if (pWndButtonReset) 
	{
		CPoint ptMove = pWndButtonReset->GetWndRect().TopLeft();
		ptMove.y = GetWndRect().Height() - pWndButtonReset->GetWndRect().Height() - m_wndTitleBar.GetWndRect().Height();
		pWndButtonReset->Move(ptMove);
	}

	CWndButton* pWndButtonCheck = (CWndButton*)GetDlgItem(WIDC_CHECK);
	if (pWndButtonCheck) 
	{
		CPoint ptMove = pWndButtonCheck->GetWndRect().TopLeft();
		ptMove.y = GetWndRect().Height() - pWndButtonCheck->GetWndRect().Height() - m_wndTitleBar.GetWndRect().Height();
		pWndButtonCheck->Move(ptMove);
	}
	
}
BOOL CWndModelView::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
		case WIDC_BUTTON: // reset
		{
#ifdef __AEGON_PREM_MOTIONS
			m_bMotionPreview = FALSE;
			m_pMotionProp = NULL;
#endif // __AEGON_PREM_MOTIONS

			for (int i = 0; i < MAX_HUMAN_PARTS; ++i)
			{
				m_Mover.m_aEquipInfo[i].dwId = NULL_ID;
#ifdef __LOOKCHANGE
				m_Mover.m_aEquipInfo[i].dwLook = NULL_ID;
#endif //__LOOKCHANGE
			}

			SAFE_DELETE(m_pPet);
			SAFE_DELETE(m_pEgg);
			SAFE_DELETE(m_pMount);
#ifdef __ANI_WING
			m_pAniMask = NULL;
#endif //__ANI_WING
#ifdef __LNB_ANIMATED_HATS
			m_pAnimatedHat = NULL;
#endif //__LNB_ANIMATED_HATS

			m_fRot = 0.0f;
			m_fRot1 = 0.0f;
			m_fWheel = 6.0f;
			vecPos123 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			vecPos1234 = D3DXVECTOR3(0.0f, -0.6f, 0.0f);

			SetSex(g_pPlayer->GetSex());
			break;
		}
		case WIDC_BUTTON2:
		{
			if (m_pMotionProp && !easy_find(g_pPlayer->m_vUnlockedMotions, m_pMotionProp->dwUnlockItemID))
			{
				CWndIngameDPPurchaseConfirm* pWndConfirmPurchase = g_WndMng.OpenDPConfirmPurchase();
				if (pWndConfirmPurchase)
				{
					pWndConfirmPurchase->m_nCommand = INGAMEDP_BUYMOTION;
					pWndConfirmPurchase->dwID = m_pMotionProp->dwID;

					CString strName;
					strName.Format("%s", m_pMotionProp->szName);
					if (strName.GetLength() == 0) 
					{
						strName.Format("%s", m_pMotionProp->szDesc);
					}

					pWndConfirmPurchase->SetInfo((char*)strName.GetString(), m_pMotionProp->nPriceDP);
				}				
			}			
			break;
		}
		default:
			break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

HRESULT CWndModelView::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	return S_OK;
}
HRESULT CWndModelView::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	return S_OK;
}
HRESULT CWndModelView::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return S_OK;
}

#endif //__MODEL_VIEW