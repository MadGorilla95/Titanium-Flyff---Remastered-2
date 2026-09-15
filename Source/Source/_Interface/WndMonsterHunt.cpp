#include "stdafx.h"
#include "resData.h"
#include "WndMonsterHunt.h"
#include "DPClient.h"
#include "defineText.h"
#include "WndManager.h"
#include "MonsterHunt.h"
#include "WndManager.h"
#include "Model.h"
#include "ModelMng.h"
#include "ModelObject.h"
#include "ModelGlobal.h"
#include "math.h"


extern CDPClient g_DPlay;
extern char* FindDstString(int nDstParam);
extern BOOL IsDst_Rate(int nDstParam);

#ifdef __AEGON_MONSTER_HUNT
///////////////////////////
///////MAIN WINDOW/////////
///////////////////////////
CWndMonsterHunt::CWndMonsterHunt()
{
	m_wndQuest = NULL;
}
CWndMonsterHunt::~CWndMonsterHunt()
{

}
void CWndMonsterHunt::OnDraw(C2DRender* p2DRender)
{
}
void CWndMonsterHunt::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	//Move Window to center
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();

	CWndComboBox* regionBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	CWndComboBox* typeBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);
	CWndComboBox* stateBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX3);
	regionBox->AddWndStyle(EBS_READONLY);
	typeBox->AddWndStyle(EBS_READONLY);
	stateBox->AddWndStyle(EBS_READONLY);

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	if (pWndText)
		pWndText->m_string.SetColor(CAegonThemeSwitcherMng::GetInstance()->GetTextColor());

	m_wndQuest = new CWndMonsterHuntQuest;
	m_wndQuest->Create(WBS_VSCROLL | WBS_CHILD, GetWndCtrl(WIDC_LISTBOX1)->rect, this, APP_MONSTER_HUNT_QUEST);

	CWndEdit* pWndEditSearch = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	if (pWndEditSearch) 
	{
		pWndEditSearch->SetPlaceholderString("Search...");
	}

	g_DPlay.SendLoadMonsterHuntProgress();



}

BOOL CWndMonsterHunt::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_MONSTER_HUNT, 0, CPoint(0, 0), pWndParent);
}

void CWndMonsterHunt::OnDestroy()
{
	m_wndQuest->RemoveObjects();
	Destroy();
}

BOOL CWndMonsterHunt::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndMonsterHunt::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndMonsterHunt::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndMonsterHunt::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndMonsterHunt::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID) 
	{
	case WIDC_COMBOBOX1:
	case WIDC_COMBOBOX2:
	case WIDC_COMBOBOX3:
	case WIDC_EDIT1:
		if (m_wndQuest)
		{
			if (nID == WIDC_EDIT1 && message != EN_CHANGE)
				break;
			m_wndQuest->FilterChanged();
			m_wndQuest->SetScrollPos(0);
		}
		break;
	case WIDC_CANCEL:
	case WTBID_CLOSE:
		OnDestroy();
		break;
	}
		
	return CWndNeuz::OnChildNotify(message, nID, pLResult);

}

void CWndMonsterHunt::OnLButtonDblClk(UINT nFlags, CPoint point)
{

}

BOOL CWndMonsterHunt::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	return TRUE;
}
///////////////////////////
///////QUEST WINDOW////////
///////////////////////////
CWndMonsterHuntQuest::CWndMonsterHuntQuest()
{
	m_fVecRadius.clear();
	m_pVecModel.clear();
	m_pVecItemTexture.clear();

	
	map<DWORD, __MONSTERHUNT_QUEST>::iterator it = CMonsterHuntMng::GetInstance()->m_mapMonsterHunt.begin();
	for (; it != CMonsterHuntMng::GetInstance()->m_mapMonsterHunt.end(); it++)
	{
		m_fVecRadius.push_back(1.0f);
	}

	m_pItemLockedTexture = NULL;
	m_pRepeatableTexture = NULL;
	loadedFilters = FALSE;
}
CWndMonsterHuntQuest::~CWndMonsterHuntQuest()
{
	m_fVecRadius.clear();
	m_pVecModel.clear();
	m_pVecItemTexture.clear();

}
void CWndMonsterHuntQuest::Create(DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	CWndListBox::Create(dwListCtrlStyle, rect, pParentWnd, nID);
}
CString CWndMonsterHuntQuest::GetDstString(DWORD nDst, DWORD nDstNum)
{
	CString strOut;
	if(nDst == DST_NONE)
	{
		strOut.Format("");
		return strOut;
	}
		
	if (IsDst_Rate(nDst))
	{
		if (nDst == DST_ATTACKSPEED)
			strOut.Format("%s %c %d%c", FindDstString(nDst), (nDstNum > 0 ? '+' : '-'), (nDstNum / 2 / 10), '%');
		else
			strOut.Format("%s %c %d%c", FindDstString(nDst), (nDstNum > 0 ? '+' : '-'), nDstNum, '%');
	}
	else
	{
		strOut.Format("%s %c %d", FindDstString(nDst), (nDstNum > 0 ? '+' : '-'), nDstNum);
	}
	return strOut;
}

void CWndMonsterHuntQuest::RenderMob(C2DRender* p2DRender, DWORD mobID, DWORD questID, CRect lpFace, BOOL doRender)
{

	if (g_pPlayer == NULL)
		return;


	//if (!m_pModel[questID])
	//	return;

	if (!m_pVecModel[questID])
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

	FLOAT fov = D3DX_PI / 4.0f;//796.0f;
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


	if (!m_pVecModel[questID])
		return;

	CObject3D* pObj3D = m_pVecModel[questID]->GetObject3D();
	if (!pObj3D)
		return; 

	//CModelObject에는 이벤트 좌표가 없는데 CObject3D는 있고?
	vecPos = pObj3D->m_vEvent[0];


	// mdldyna.inc에서 스케일을 조정한경우 그에 맞게 보정을 해주는데, 어떤 원리인지 내가 했지만 이상함. 나중에 다시 정확히 잡아볼까?
	float fModelScale = m_pVecModel[questID]->m_pModelElem->m_fScale;
	if (fModelScale < 1.0f && fModelScale > 0.001f)
		vecPos *= (fModelScale - fModelScale * (0.5f + (1.0f - fModelScale) * 0.01f));	//스케일 변동치가 클수록 
	else if (fModelScale > 1.0f)
		vecPos *= (fModelScale - fModelScale * (0.9f + fModelScale * 0.01f));

	if (questID < m_fVecRadius.size()) 
	{
		m_fVecRadius[questID] = m_pVecModel[questID]->GetRadius();
		fScale = (1 / m_fVecRadius[questID]) * 1.5f; // ( 2.0f * fRadius );
	}
		

	vecPos.x += 0.5f;
	vecPos.y += 1.8f;
	vecPos.z -= (3.0f); //* fRadius );

	vecLookAt.x = -0.24f;
	vecLookAt.y = 0.28f;
	vecLookAt.z = 1.0f;


	D3DXMatrixScaling(&matScale, fScale, fScale, fScale);

	D3DXMatrixLookAtLH(&matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixMultiply(&matWorld, &matWorld, &matScale);
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

	if(doRender)
	{
		m_pVecModel[questID]->SetTextureEx(m_pVecModel[questID]->m_pModelElem->m_nTextureEx);
		m_pVecModel[questID]->Render(pd3dDevice, &matWorld);
	}


	//	SetDiffuse( 0.0f, 0.0f, 0.0f );
	//	SetAmbient( 1.0f, 1.0f, 1.0f );

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);


}
void CWndMonsterHuntQuest::OnInitialUpdate()
{
	CWndListBox::OnInitialUpdate();


	LoadItemTextures();
	LoadFilter();
	FilterChanged();
	
}
void CWndMonsterHuntQuest::OnDraw(C2DRender* p2DRender)
{
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	DWORD dwColorHighlightBlue = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE);
	DWORD dwColorHighlightGreen = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_GREEN);

	DWORD			dwColor = dwColorText;
	int m_nFontHeight = 36;
	int m_nLineSpace = 100;
	int nListFontHeight = GetFontHeight() + m_nLineSpace;
	

	D3DCOLOR progressBarFrameColor = dwColorText;
	D3DCOLOR progressBarFillColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	

	//ScrollBar
	int nPage = GetClientRect().Height() / nListFontHeight;
	{
		int nRange = m_pVecQuests.size();

		CPoint pt(3, 3);
		pt.y -= nListFontHeight * m_wndScrollBar.GetScrollPos();

		if (!nRange)
			nRange = 1;
			
		if (IsWndStyle(WBS_VSCROLL))
		{
			m_wndScrollBar.SetVisible(TRUE);
			m_wndScrollBar.SetScrollRange(0, nRange);
			m_wndScrollBar.SetScrollPage(nPage);
		}
		else
			m_wndScrollBar.SetVisible(FALSE);
	}

	CPoint pt(10, 10);

	for(int i = 0; i < nPage; i++)
	{
		int nIndex = m_wndScrollBar.GetScrollPos() + i;
		if (nIndex >= m_pVecQuests.size())
			continue;

		__MONSTERHUNT_QUEST* pQuestUnit = m_pVecQuests[nIndex];

		if (nIndex >= m_pVecModel.size())
			continue;

		if (!m_pVecModel[nIndex])
			LoadMobModel(nIndex);

		if (m_pVecModel[nIndex])
			m_pVecModel[nIndex]->FrameMove();

		__MONSTERHUNT_SINGLE* progUnit = g_pPlayer->m_MHProgress.GetProgUnit(pQuestUnit->questID);
		if (!progUnit)
			continue;


		CString strMobName;
		DWORD mobID = pQuestUnit->mobID;
		DWORD progressCount = progUnit->dwCount;
		DWORD mobNum = pQuestUnit->mobNum;
		DWORD itemID = pQuestUnit->rewardItem;
		DWORD itemNum = pQuestUnit->itemNum;
		CString strItem;

		DWORD progress = 0;
		DWORD progressWidth = 0;

		if (mobID != progUnit->dwMobID)
			progressCount = 0;
		
		MoverProp* moverProp = prj.GetMoverPropEx(mobID);
		if (moverProp)
			strMobName.Format("%s", moverProp->szName);
		else
			strMobName.Format("nameError");

		ItemProp* itemProp = prj.GetItemProp(itemID);
		if (itemProp)
			strItem.Format("%s",itemProp->szName);
		else
			strItem.Format("itemError");

		CRect rect = GetWindowRect();
		CPoint point;
		point.x = 100;
		point.y = 14 + i * nListFontHeight;

		
		//rect where mob gets rendered
		CPoint pMob1(GetClientRect().left + 4, point.y + 10);
		CPoint pMob2(GetClientRect().left + 104, point.y + 110);
		CRect mobRect(pMob1, pMob2);
		
		//quest wnd
		CPoint p5(GetClientRect().left + 14, GetClientRect().top);
		CPoint p6(GetClientRect().right - 14, GetClientRect().bottom);
		CRect questWndRect(p5, p6);

		//prevent rendering outside of wnd Y coord
		if (!(pMob2.y > GetClientRect().bottom))
			RenderMob(p2DRender, mobID, nIndex, mobRect, TRUE);
		
		RenderMob(p2DRender, mobID, nIndex, questWndRect, FALSE); // to set viewport back to the entire wnd

		
		CString strTmp;
		CEditString strEditOut;

/*			Styles
ESSTY_UNDERLINE    
ESSTY_BOLD          
ESSTY_STRIKETHROUGH 
ESSTY_SHADOW		
ESSTY_BLOCK	
*/

		strTmp.Format("%s", strMobName.GetString());

		strEditOut.SetParsingString(strTmp);
		strEditOut.SetColor(dwColorHighlightGreen);
		strEditOut.SetStyle(ESSTY_BOLD | ESSTY_UNDERLINE);	

		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD | ESSTY_UNDERLINE);
		

		strTmp.Format("Reward");
		strEditOut.SetParsingString(strTmp);
		strEditOut.SetColor(dwColorText);
		strEditOut.SetStyle(0, 6, ESSTY_BOLD | ESSTY_UNDERLINE);
		p2DRender->TextOut_EditString(GetClientRect().right - 100, point.y, strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD | ESSTY_UNDERLINE);


		//item
		strTmp.Format("%dx", itemNum);
		strEditOut.SetParsingString(strTmp);
		strEditOut.SetColor(dwColorHighlightBlue);
		strEditOut.SetStyle(ESSTY_BOLD | ESSTY_SHADOW);
		p2DRender->TextOut_EditString(GetClientRect().right - 60, point.y + 45, strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD | ESSTY_SHADOW);
		
		CPoint itemTextPoint(GetClientRect().right - 50, point.y+20);
				
		if(progUnit->dwRewardSent == 0)
		{
			p2DRender->RenderTexture2(itemTextPoint, m_pVecItemTexture[pQuestUnit->questID], 1.0f, 1.0f, D3DCOLOR_ARGB(255, 210, 90, 90));
			p2DRender->RenderTexture(itemTextPoint, m_pItemLockedTexture);
		}
		else
			p2DRender->RenderTexture(itemTextPoint, m_pVecItemTexture[pQuestUnit->questID]);
		//item

		//Item Tooltip
		CRect rectItemToolTip(itemTextPoint, CPoint(itemTextPoint.x + 28, itemTextPoint.y +28) );
		CPoint ptMouse = GetMousePoint();

		if (rectItemToolTip.PtInRect(ptMouse))
		{
			CItemElem itemElem;
			itemElem.m_dwItemId = itemID;
			itemElem.m_nItemNum = itemNum;

			if(&itemElem)
			{
				ClientToScreen(&ptMouse);
				ClientToScreen(&rectItemToolTip);
				g_WndMng.PutToolTip_Item(&itemElem, ptMouse, &rectItemToolTip);
			}
			
		}
		//Item Tooltip

		//repeatable icon
		CPoint repeatableTextPoint(GetClientRect().right - 45, point.y + 70);
		if(pQuestUnit->repeatable)
		{
			p2DRender->RenderTexture(repeatableTextPoint, m_pRepeatableTexture, 255, 0.3, 0.3);
		}
		//repeatable icon

		//progress
		CPoint p1(0, 1 + (i + 1)*nListFontHeight);
		CPoint p2((GetWndRect().right) - 40, 1 + (i + 1)*nListFontHeight);
		
		p2DRender->RenderLine(p1, p2, 0xFF776655);
		
		CPoint p7(point.x, 1 + 10 + (i + 1) * (nListFontHeight) - ceil(nListFontHeight / 3));
		CPoint p8(GetClientRect().right - 50, 30 + (i + 1) * (nListFontHeight) - ceil(nListFontHeight / 3));
		CRect progressRect(p7, p8);
		p2DRender->RenderRect(progressRect, progressBarFrameColor);

		p2DRender->SetTextColor(D3DCOLOR_ARGB(255, 255, 0, 0));

		CRect fillProgressRect = progressRect;

		progress = ceil(progressCount * 100 / pQuestUnit->mobNum);
		progressWidth = ceil(((double)(((int)progressRect.Width())-3) / 100 * progress));
		
		p7.x += 1;
		p7.y += 1;
		p8.x = p7.x + (progressWidth);
		p8.y -= 2;
		fillProgressRect.SetRect(p7, p8);
		p2DRender->RenderFillRect(fillProgressRect, progressBarFillColor);
		

		//etc_reflect.tga DIR_MODELTEX
		//if (!m_progressBarTexture)
		//	LoadTextureFromRes(m_p2DRender->m_pd3dDevice, MakePath(DIR_THEME, "mhgauge01.bmp"), &m_progressBarTexture, 1, D3DFMT_R8G8B8); //doesnt work just takes single color
		//p2DRender->RenderFillRect(fillProgressRect, progressBarFillColor, m_progressBarTexture); 

		strTmp.Format("%d", progressCount);
		strEditOut.SetParsingString(strTmp);		
		strEditOut.SetColor(dwColorText);
		strEditOut.SetStyle(ESSTY_BOLD);
		p2DRender->TextOut_EditString(progressRect.CenterPoint().x - 20 - 10 * (strEditOut.GetLength() - 1) , progressRect.TopLeft().y + 2, strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD);

		strTmp.Format("/");
		strEditOut.SetParsingString(strTmp);
		strEditOut.SetColor(dwColorText);
		strEditOut.SetStyle(ESSTY_BOLD);
		p2DRender->TextOut_EditString(progressRect.CenterPoint().x, progressRect.TopLeft().y + 2,strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD);

		strTmp.Format("%d", mobNum);
		strEditOut.SetParsingString(strTmp);
		strEditOut.SetColor(dwColorText);
		strEditOut.SetStyle(ESSTY_BOLD);
		p2DRender->TextOut_EditString(progressRect.CenterPoint().x + 20, progressRect.TopLeft().y + 2, strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD);
		//progress
	}
}
void CWndMonsterHuntQuest::LoadFilter()
{
	if (!loadedFilters)
	{
		CWndComboBox* regionBox = (CWndComboBox*)GetParentWnd()->GetDlgItem(WIDC_COMBOBOX1);
		regionBox->SetTabStop(TRUE);

		regionBox->AddString(_T("Monster Region"));
		regionBox->AddString(_T("Flaris"));
		regionBox->AddString(_T("Saint Morning"));
		regionBox->AddString(_T("Garden of Rhisis"));
		regionBox->AddString(_T("Darkon 1/2"));
		regionBox->AddString(_T("Darkon 3"));
		regionBox->AddString(_T("Shaduwar"));
		regionBox->AddString(_T("Valley of the Risen"));
		regionBox->AddString(_T("Eillun"));
		regionBox->AddString(_T("Bahara Desert"));
		regionBox->AddString(_T("Coral Island"));
		regionBox->AddString(_T("Azria"));
		regionBox->AddString(_T("Traseia"));
		regionBox->AddString(_T("Dark Traseia"));
		regionBox->AddString(_T("Forteress"));
		regionBox->AddString(_T("Volcano"));
		regionBox->AddString(_T("Tower"));
		regionBox->AddString(_T("Other"));

		regionBox->SetString(_T("Monster Region"));

		CWndComboBox* typeBox = (CWndComboBox*)GetParentWnd()->GetDlgItem(WIDC_COMBOBOX2);
		typeBox->SetTabStop(TRUE);

		typeBox->AddString(_T("Mission Type"));
		typeBox->AddString(_T("Repeatable"));
		typeBox->AddString(_T("One time"));

		typeBox->SetString(_T("Mission Type"));

		CWndComboBox* stateBox = (CWndComboBox*)GetParentWnd()->GetDlgItem(WIDC_COMBOBOX3);
		stateBox->SetTabStop(TRUE);
		stateBox->AddString(_T("Mission State"));
		stateBox->AddString(_T("Finished"));
		stateBox->AddString(_T("Not Finished"));

		stateBox->SetString(_T("Mission State"));

		loadedFilters = TRUE;
	}
}

void CWndMonsterHuntQuest::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CWndMonsterHuntQuest::OnMouseWndSurface(CPoint pt)
{
}          
void  CWndMonsterHuntQuest::FilterChanged() 
{
	FillQuestVec();
	InitModelVec();
}
BOOL CWndMonsterHuntQuest::FilterCheck(__MONSTERHUNT_QUEST* pQUestUnit)
{
	if (!pQUestUnit)
		return FALSE;

	CWndComboBox* regionBox = (CWndComboBox*)GetParentWnd()->GetDlgItem(WIDC_COMBOBOX1);
	CWndComboBox* typeBox = (CWndComboBox*)GetParentWnd()->GetDlgItem(WIDC_COMBOBOX2);
	CWndComboBox* stateBox = (CWndComboBox*)GetParentWnd()->GetDlgItem(WIDC_COMBOBOX3);

	CWndEdit* pWndEditSearch = (CWndEdit*)GetParentWnd()->GetDlgItem(WIDC_EDIT1);

	CString strRegionFilter;
	int nRegionFilter = 0;
	strRegionFilter.Format("%s", regionBox->GetString());
	if (strRegionFilter.CompareNoCase(_T("Monster Region")) == 0)
		nRegionFilter = 0;
	else if (strRegionFilter.CompareNoCase(_T("Flaris")) == 0)
		nRegionFilter = 1;
	else if (strRegionFilter.CompareNoCase(_T("Saint Morning")) == 0)
		nRegionFilter = 2;
	else if (strRegionFilter.CompareNoCase(_T("Garden of Rhisis")) == 0)
		nRegionFilter = 3;
	else if (strRegionFilter.CompareNoCase(_T("Darkon 1/2")) == 0)
		nRegionFilter = 4;
	else if (strRegionFilter.CompareNoCase(_T("Darkon 3")) == 0)
		nRegionFilter = 5;
	else if (strRegionFilter.CompareNoCase(_T("Shaduwar")) == 0)
		nRegionFilter = 6;
	else if (strRegionFilter.CompareNoCase(_T("Valley of the Risen")) == 0)
		nRegionFilter = 7;
	else if (strRegionFilter.CompareNoCase(_T("Eillun")) == 0)
		nRegionFilter = 8;
	else if (strRegionFilter.CompareNoCase(_T("Bahara Desert")) == 0)
		nRegionFilter = 9;
	else if (strRegionFilter.CompareNoCase(_T("Coral Island")) == 0)
		nRegionFilter = 10;
	else if (strRegionFilter.CompareNoCase(_T("Azria")) == 0)
		nRegionFilter = 11;
	else if (strRegionFilter.CompareNoCase(_T("Traseia")) == 0)
		nRegionFilter = 12;
	else if (strRegionFilter.CompareNoCase(_T("Dark Traseia")) == 0)
		nRegionFilter = 13;
	else if (strRegionFilter.CompareNoCase(_T("Forteress")) == 0)
		nRegionFilter = 14;
	else if (strRegionFilter.CompareNoCase(_T("Volcano")) == 0)
		nRegionFilter = 15;
	else if (strRegionFilter.CompareNoCase(_T("Tower")) == 0)
		nRegionFilter = 16;
	else if (strRegionFilter.CompareNoCase(_T("Other")) == 0)
		nRegionFilter = 17;
	else
		nRegionFilter = -1;

	CString strTypeFilter;
	int nTypeFilter = 0;
	strTypeFilter.Format("%s", typeBox->GetString());
	if (strTypeFilter.CompareNoCase(_T("Mission Type")) == 0)
		nTypeFilter = 0;
	else if (strTypeFilter.CompareNoCase(_T("Repeatable")) == 0)
		nTypeFilter = 1;
	else if (strTypeFilter.CompareNoCase(_T("One time")) == 0)
		nTypeFilter = 2;
	else
		nTypeFilter = -1;

	CString strStateFilter;
	int nStateFilter = 0;
	strStateFilter.Format("%s", stateBox->GetString());
	if (strStateFilter.CompareNoCase(_T("Mission State")) == 0)
		nStateFilter = 0;
	else if (strStateFilter.CompareNoCase(_T("Finished")) == 0)
		nStateFilter = 1;
	else if (strStateFilter.CompareNoCase(_T("Not Finished")) == 0)
		nStateFilter = 2;
	else
		nStateFilter = -1;


	if (pWndEditSearch) //search filter
	{
		CString strSearch = pWndEditSearch->GetString();
		if (strSearch.GetLength() > 0)
		{
			strSearch = strSearch.MakeLower();

			BOOL bStringFound = FALSE;
			MoverProp* pMoverProp = prj.GetMoverProp(pQUestUnit->mobID);
			if (pMoverProp) 
			{
				CString strMonsterName = pMoverProp->szName;
				strMonsterName = strMonsterName.MakeLower();
				if(strMonsterName.Find(strSearch.GetString()) != string::npos)
				{
					bStringFound = TRUE;
				}
			}
			
			ItemProp* pItemProp = prj.GetItemProp(pQUestUnit->rewardItem);
			if (pItemProp) 
			{
				CString strItemName = pItemProp->szName;
				strItemName = strItemName.MakeLower();
				if (strItemName.Find(strSearch.GetString()) != string::npos)
				{
					bStringFound = TRUE;
				}
			}

			if (!bStringFound)
				return FALSE;
		}
	}

	if (nRegionFilter != 0 || nTypeFilter != 0 || nStateFilter != 0)
	{

		__MONSTERHUNT_SINGLE* progUnit = g_pPlayer->m_MHProgress.GetProgUnit(pQUestUnit->questID);
		if (!progUnit)
			return FALSE;

		if (nRegionFilter != 0)
		{
			if (nRegionFilter != pQUestUnit->region || nRegionFilter == -1)
				return FALSE;
		}

		if (nTypeFilter != 0)
		{
			if ((pQUestUnit->repeatable && nTypeFilter == 2)
				|| (!pQUestUnit->repeatable && nTypeFilter == 1))
				return FALSE;
		}

		if (nStateFilter != 0)
		{
			if ((nStateFilter == 1 && !progUnit->dwRewardSent)
				|| (nStateFilter == 2 && progUnit->dwRewardSent))
				return FALSE;
		}

		return TRUE;
	}
	return TRUE;
}
void CWndMonsterHuntQuest::FillQuestVec() 
{
	m_pVecQuests.clear();
	map<DWORD, __MONSTERHUNT_QUEST>::iterator it = CMonsterHuntMng::GetInstance()->m_mapMonsterHunt.begin();
	for (; it != CMonsterHuntMng::GetInstance()->m_mapMonsterHunt.end(); it++)
	{
		if(FilterCheck(&it->second))
			m_pVecQuests.push_back(&it->second);
	}
}
void CWndMonsterHuntQuest::InitModelVec() 
{
	RemoveObjects();
	m_pVecModel.clear();
	for(int i = 0; i < m_pVecQuests.size(); i++)
	{
		m_pVecModel.push_back(NULL);
	}
}
void CWndMonsterHuntQuest::LoadMobModel(int nQuestID)
{
	if (nQuestID < 0 || nQuestID >= m_pVecModel.size())
		return;

	MoverProp* pMoverProp = prj.GetMoverPropEx(m_pVecQuests[nQuestID]->mobID);

	if (pMoverProp)
	{
		CModelObject* tmpObj = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, pMoverProp->dwID);
				
		prj.m_modelMng.LoadMotion(tmpObj, OT_MOVER, pMoverProp->dwID, MTI_STAND);
		tmpObj->InitDeviceObjects(g_Neuz.GetDevice());
		m_pVecModel[nQuestID] = tmpObj;
	}
}
void CWndMonsterHuntQuest::LoadItemTextures()
{
	map<DWORD, __MONSTERHUNT_QUEST>::iterator it = CMonsterHuntMng::GetInstance()->m_mapMonsterHunt.begin();
	for (; it != CMonsterHuntMng::GetInstance()->m_mapMonsterHunt.end(); it++)
	{
		if (it->first == m_pVecItemTexture.size()) 
		{
			ItemProp* pItemProp = prj.GetItemProp(it->second.rewardItem);
			CString strPath;
			if (pItemProp)
			{
				strPath.Format(DIR_ITEM  "%s", pItemProp->szIcon);
				CTexture* tmpTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, strPath, COLOR_ITEM_BG);

				m_pVecItemTexture.push_back(tmpTexture);
			}
		}

	}
	m_pItemLockedTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "lock.png"), COLOR_ITEM_BG);
	m_pRepeatableTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "repeatable_icon.png"), COLOR_ITEM_BG);
	
}
void CWndMonsterHuntQuest::RemoveObjects()
{
	for (int i = 0; i < m_pVecModel.size(); i++) 
	{
		if (m_pVecModel[i])
		{
			m_pVecModel[i]->DeleteDeviceObjects();
		}
	}
}
#endif // __AEGON_MONSTER_HUNT