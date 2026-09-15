#include "stdafx.h"
#include "WndDropInfo.h"
#include "ResData.h"
#include "DPClient.h"
#include "WndMonsterWiki.h"

#ifdef __WIKIPEDIA_MONSTER_DROPLIST
CWndDropInfo::CWndDropInfo()
{
	mMonsterID = 0;
	m_modelObj = NULL;

	ResetModelView();
}
CWndDropInfo::~CWndDropInfo()
{
	InvalidateDeviceObjects();
}
void CWndDropInfo::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	RestoreDeviceObjects();
	MoveParentCenter();

	m_lpWndMobField = GetWndCtrl(WIDC_CUSTOM1);
	pWndComboBoxAnimation = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	pWndComboBoxAnimation->AddWndStyle(EBS_READONLY);
	pWndComboBoxAnimation->AddString("Stand");
	pWndComboBoxAnimation->AddString("Idle 1");
	pWndComboBoxAnimation->AddString("Idle 2");
	pWndComboBoxAnimation->AddString("Walk");
	pWndComboBoxAnimation->AddString("Walk Back");
	pWndComboBoxAnimation->AddString("Run");
	pWndComboBoxAnimation->AddString("Dmg 1");
	pWndComboBoxAnimation->AddString("Dmg 2");
	pWndComboBoxAnimation->AddString("Dmg Survive");
	pWndComboBoxAnimation->AddString("Dmg Die");
	pWndComboBoxAnimation->AddString("Dmg Flying");
	pWndComboBoxAnimation->AddString("Die");
	pWndComboBoxAnimation->AddString("Groggy");

	pWndComboBoxAnimation->AddString("Atk 1");
	pWndComboBoxAnimation->AddString("Atk 2");
	pWndComboBoxAnimation->AddString("Atk 3");
	pWndComboBoxAnimation->AddString("Atk 4");
	pWndComboBoxAnimation->AddString("Atk 5");
	pWndComboBoxAnimation->AddString("Atk 6");
	pWndComboBoxAnimation->AddString("Atk 7");
	pWndComboBoxAnimation->AddString("Atk 8");

	pWndComboBoxAnimation->SetCurSel(0);
}

BOOL CWndDropInfo::Initialize(CWndBase* pWndParent, DWORD dwStyle)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_DROP_INFORMATION, 0, CPoint(0, 0), pWndParent);
}
void CWndDropInfo::OnDestroy()
{
	DeleteDeviceObjects();
	Destroy();
}
DWORD CWndDropInfo::GetMonsterID() const
{
	return mMonsterID;
}

void CWndDropInfo::SetMonsterID(DWORD dwMonsterID)
{
	mMonsterID = dwMonsterID;

	LoadMobModel();
}
void CWndDropInfo::ResetModelView() 
{
	m_fZoom = 4.0f;
	m_fRot = 0.0f;
	m_fRot1 = 0.0f;
	vecPosTranslate = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_bLButtonDownRot = FALSE;
	m_bRButtonDownRot = FALSE;
	m_bExecMotion = TRUE;
}
void CWndDropInfo::ChangeAnimation() 
{
	if (pWndComboBoxAnimation && m_modelObj)
	{
		CString tmpStr = pWndComboBoxAnimation->GetString();
		BOOL bResult = FALSE;
		
		if (tmpStr.Compare("Stand") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_STAND);
		}
		else if (tmpStr.Compare("Idle 1") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_IDLE1);
		}
		else if (tmpStr.Compare("Idle 2") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_IDLE2);
		}
		else if (tmpStr.Compare("Walk") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_WALK);
		}
		else if (tmpStr.Compare("Walk Back") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_BACK);
		}
		else if (tmpStr.Compare("Run") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_RUN);
		}
		else if (tmpStr.Compare("Dmg 1") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_DMG1);
		}
		else if (tmpStr.Compare("Dmg 2") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_DMG2);
		}
		else if (tmpStr.Compare("Die") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_DIE1);
		}
		else if (tmpStr.Compare("Dmg Survive") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_DMGLIVE);			
		}
		else if (tmpStr.Compare("Dmg Die") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_DMGDIE);
		}
		else if (tmpStr.Compare("Dmg Flying") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_DMGFLY);
		}
		else if (tmpStr.Compare("Groggy") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_GROGGY);
		}
		else if (tmpStr.Compare("Atk 1") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_ATK1);
		}
		else if (tmpStr.Compare("Atk 2") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_ATK2);
		}
		else if (tmpStr.Compare("Atk 3") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_ATK3);
		}
		else if (tmpStr.Compare("Atk 4") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_ATK4);
		}
		else if (tmpStr.Compare("Atk 5") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_ATK5);
		}
		else if (tmpStr.Compare("Atk 6") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_ATK6);
		}
		else if (tmpStr.Compare("Atk 7") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_ATK7);
		}
		else if (tmpStr.Compare("Atk 8") == 0)
		{
			bResult = prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_ATK8);
		}
		else 
		{
			pWndComboBoxAnimation->SetCurSel(0);
			prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_STAND);
		}


		if (!bResult) 
		{
			pWndComboBoxAnimation->SetCurSel(0);
			prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, mMonsterID, MTI_STAND);
			CString errorString;
			errorString.Format("%s Motion not found.", tmpStr.GetString());
			g_WndMng.PutString(errorString.GetString(), 0, COLOR_ERROR);
		}
		else	
			m_modelObj->InitDeviceObjects(g_Neuz.GetDevice());

	}
}
BOOL CWndDropInfo::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID) 
	{
		case WIDC_COMBOBOX1: // change motion
		{
			ChangeAnimation();
		}
		break;
		case WIDC_BUTTON1: // freeze
		{
			m_bExecMotion = m_bExecMotion ? FALSE : TRUE;
		}
		break;
		case WIDC_BUTTON2: // reset
		{
			ResetModelView();
		}
		break;
	default:
		break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndDropInfo::OnDraw(C2DRender* p2DRender)
{
	DWORD dwMonsterID = GetMonsterID();
	MoverProp* pMoverProp = prj.GetMoverProp(dwMonsterID);
	CString Drops = " Drops";
	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	pStatic->SetTitle(pMoverProp->szName + Drops);


	

	//rect where mob gets rendered
	CRect mobRect = m_lpWndMobField->rect;

	//quest wnd
	CPoint p5(GetClientRect().left + 14, GetClientRect().top);
	CPoint p6(GetClientRect().right - 14, GetClientRect().bottom);
	CRect questWndRect(p5, p6);


	RenderMob(p2DRender, mobRect); // render mob

	p2DRender->RenderRect(mobRect, COLOR_BLACK);

	if (m_modelObj && m_bExecMotion)
		m_modelObj->FrameMove();
}
void CWndDropInfo::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndDropInfo::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	//rect where mob gets rendered
	CRect mobRect = m_lpWndMobField->rect;
	CPoint point = GetMousePoint();
	if (mobRect.PtInRect(point))
	{
		if (zDelta < 0)
			m_fZoom += 0.15f;
		else
			m_fZoom -= 0.15f;
	}
	

	return TRUE;
}
void CWndDropInfo::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bLButtonDownRot)
	{
		m_fRot += (m_ptOld.x - point.x) * 0.75f;
		m_fRot1 += (m_ptOld.y - point.y) * 0.75f;
		m_ptOld = point;
	}
	if (m_bRButtonDownRot)
	{
		vecPosTranslate.x -= (m_ptOld1.x - point.x) * 0.01f;
		vecPosTranslate.y += (m_ptOld1.y - point.y) * 0.01f;
		m_ptOld1 = point;
	}
}
void CWndDropInfo::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	if (GetWndCtrl(WIDC_CUSTOM1)->rect.PtInRect(point))
	{
		m_ptOld = point;
		m_bLButtonDownRot = TRUE;
	}
}
void CWndDropInfo::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_bLButtonDownRot = FALSE;
}
void CWndDropInfo::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	if (GetWndCtrl(WIDC_CUSTOM1)->rect.PtInRect(point))
	{
		m_ptOld1 = point;
		m_bRButtonDownRot = TRUE;
	}
}
void CWndDropInfo::OnRButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_bRButtonDownRot = FALSE;
}
HRESULT CWndDropInfo::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if(m_modelObj)
		m_modelObj->RestoreDeviceObjects();
	return S_OK;
}
HRESULT CWndDropInfo::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	if (m_modelObj)
		m_modelObj->InvalidateDeviceObjects();
	return S_OK;
}
HRESULT CWndDropInfo::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	if (m_modelObj)
		m_modelObj->DeleteDeviceObjects();
	return S_OK;
}


void CWndDropInfo::LoadMobModel() 
{
	if (mMonsterID > 0) 
	{
		MoverProp* pMoverProp = prj.GetMoverPropEx(mMonsterID);

		if (pMoverProp)
		{
			if (m_modelObj) 
			{
				m_modelObj->DeleteDeviceObjects();
			}
			m_modelObj = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, pMoverProp->dwID);
			prj.m_modelMng.LoadMotion(m_modelObj, OT_MOVER, pMoverProp->dwID, MTI_STAND);
			m_modelObj->InitDeviceObjects(g_Neuz.GetDevice());
		}
	}
}
void CWndDropInfo::RenderMob(C2DRender* p2DRender, CRect lpFace)
{

	if (g_pPlayer == NULL)
		return;

	if (!m_modelObj)
		return;

	CRect rect = GetClientRect();
	int nWidthClient = GetClientRect().Width() - 110;

	CRect rectTemp;

	CPoint point;
	point = CPoint(lpFace.left - 12, lpFace.top - 22);

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

	viewport.X = p2DRender->m_ptOrigin.x + lpFace.left;
	viewport.Y = p2DRender->m_ptOrigin.y + lpFace.top;
	viewport.Width = lpFace.Width();
	viewport.Height = lpFace.Height();
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;

	pd3dDevice->SetViewport(&viewport);


	// 프로젝션 
	D3DXMATRIX matProj;
	D3DXMatrixIdentity(&matProj);
	FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;

	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / m_fZoom, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane);

	FLOAT fov = D3DX_PI / m_fZoom;//796.0f;
	FLOAT h = cos(fov / 2) / sin(fov / 2);
	FLOAT w = h * fAspect;
	D3DXMatrixOrthoLH(&matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane);
	pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	D3DXMATRIX  matView;

	// 월드 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot1, matRot2;
	D3DXMATRIXA16 matTrans;

	// 초기화 
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matRot1);
	D3DXMatrixIdentity(&matRot2);
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matWorld);

	//펫 종류에 따라 설정.
	D3DXVECTOR3 vecPos;
	D3DXVECTOR3 vecLookAt;
	float fScale = 1.0f;


	if (!m_modelObj)
		return;

	CObject3D* pObj3D = m_modelObj->GetObject3D();
	if (!pObj3D)
		return;

	//CModelObject에는 이벤트 좌표가 없는데 CObject3D는 있고?
	vecPos = pObj3D->m_vEvent[0];


	// mdldyna.inc에서 스케일을 조정한경우 그에 맞게 보정을 해주는데, 어떤 원리인지 내가 했지만 이상함. 나중에 다시 정확히 잡아볼까?
	float fModelScale = m_modelObj->m_pModelElem->m_fScale;
	if (fModelScale < 1.0f && fModelScale > 0.001f)
		vecPos *= (fModelScale - fModelScale * (0.5f + (1.0f - fModelScale) * 0.01f));	//스케일 변동치가 클수록 
	else if (fModelScale > 1.0f)
		vecPos *= (fModelScale - fModelScale * (0.9f + fModelScale * 0.01f));


	m_fRadius = m_modelObj->GetRadius();
	fScale = (1.0f / m_fRadius);
	

	FLOAT maxModelHeight = m_modelObj->GetMaxHeight();
	FLOAT maxModelWidth = m_modelObj->GetMaxWidth();
	maxModelHeight = (float)maxModelHeight * fScale;
	maxModelWidth = (float)maxModelWidth * fScale;



	vecPos.x += 0.5f;
	vecPos.y += 1.8f;
	vecPos.z -= (3.0f); //* fRadius );

	vecLookAt.x = -0.24f;
	vecLookAt.y = 0.6f;
	vecLookAt.z = 1.0f;

	D3DXMATRIXA16 matRot;
	D3DXMatrixIdentity(&matRot);

	D3DXMatrixRotationY(&matRot, D3DXToRadian(m_fRot));
	D3DXMatrixRotationX(&matWorld, D3DXToRadian(m_fRot1));
	D3DXMatrixTranslation(&matTrans, vecPosTranslate.x, vecPosTranslate.y, vecPosTranslate.z);

	D3DXMatrixScaling(&matScale, fScale, fScale, fScale);

	D3DXMatrixLookAtLH(&matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixMultiply(&matWorld, &matWorld, &matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matRot);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matRot1);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	// 랜더링 
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, CWorld::m_dwBgColor, 1.0f, 0);

	::SetTransformView(matView);
	::SetTransformProj(matProj);

	//gmpbigsun : 윈도 페이스 고정라이트
	::SetLight(FALSE);
	::SetFog(FALSE);
	SetDiffuse(1.0f, 1.0f, 1.0f);
	SetAmbient(1.0f, 1.0f, 1.0f);

	m_modelObj->SetTextureEx(m_modelObj->m_pModelElem->m_nTextureEx);
	m_modelObj->Render(pd3dDevice, &matWorld);


	//	SetDiffuse( 0.0f, 0.0f, 0.0f );
	//	SetAmbient( 1.0f, 1.0f, 1.0f );

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);


}

#endif //__WIKIPEDIA_MONSTER_DROPLIST