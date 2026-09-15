#include "stdafx.h"
#include "resData.h"
#include "WndCollecting.h"
#include "Collecting.h"
#include "DPClient.h"
#include "DefineObj.h"
#include "defineText.h"

extern	CDPClient	g_DPlay;


/****************************************************
  WndId : APP_COLLECTING - Applet
  CtrlId : WIDC_BUTTON1 - Start
  CtrlId : WIDC_STATIC1 - Bettery
  CtrlId : WIDC_STATIC2 - Level
  CtrlId : WIDC_CUSTOM1 - Custom
  CtrlId : WIDC_STATIC3 - 0
  CtrlId : WIDC_STATIC4 - /  5
  CtrlId : WIDC_STATIC5 - 0
  CtrlId : WIDC_STATIC6 - / 1600
****************************************************/

CCollectingWnd::CCollectingWnd() 
{ 

	m_bIsCollecting = false;
	D3DXMatrixIdentity(&m_matModel);
	//D3DXMatrixScaling(&m_matModel, 3.0f, 3.0f, 3.0f);
	m_pSfxBase	= NULL;
	m_pSfx		= NULL;
	m_fAngle	= 0.0f;
	m_pVBGauge	= NULL;
	m_pTexGauEmptyNormal = NULL;
	m_pTexGauFillNormal = NULL;
	m_pTexBatt = m_pTexLevel = NULL;

} 


CCollectingWnd::~CCollectingWnd() 
{ 
	InvalidateDeviceObjects();
} 


void CCollectingWnd::OnDraw( C2DRender* p2DRender ) 
{ 
#if __VER >= 11 // __SYS_COLLECTING
	//if( m_pElem == NULL)
	//return;
    if(!g_pPlayer) return;
	LPWNDCTRL wndCtrlPic1 = GetWndCtrl( WIDC_PIC1 );
	LPWNDCTRL wndCtrlPic2 = GetWndCtrl( WIDC_PIC2 );

	if(m_pTexBatt)
	{
		m_pTexBatt->Render(p2DRender, wndCtrlPic2->rect.TopLeft());
	}
	if(m_pTexLevel)
	{
		m_pTexLevel->Render(p2DRender, wndCtrlPic1->rect.TopLeft());
	}

	CItemElem* pCollector = g_pPlayer->GetCollector();

	if(pCollector)
	{
		LPWNDCTRL	lpGauge   = GetWndCtrl( WIDC_GAUGE );
		CRect 		rect = GetClientRect();
		CRect 		rectTemp;
		int 		nWidthClient = lpGauge->rect.Width();
		int 		nWidth;
		int			nMax = CCollectingProperty::GetInstance()->GetMaxBattery();
		float		fRatio = (float)pCollector->m_nHitPoint / (float)nMax; 
		int			nBtryRemain = pCollector->m_nHitPoint;

		if(nBtryRemain < 0)  nBtryRemain = 0;

		nWidth = (int)( nWidthClient * fRatio );
		wsprintf(m_pbufText, "%d / %d (%d%%)", nBtryRemain, nMax, (int)(fRatio * 100));

		m_nGWidth = nWidth;
		if(m_nGWidth >= 2)
		{
			rect = lpGauge->rect;
			rectTemp = rect; 
			rectTemp.right = rectTemp.left + nWidth + 15;
			ClientToScreen( rectTemp );
			m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, D3DCOLOR_ARGB( 180, 87, 175, 207), m_pVBGauge, m_pTexGauFillNormal );
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBGauge, m_pTexGauEmptyNormal );	
		}
	}

	LPWNDCTRL lpGauge   = GetWndCtrl( WIDC_GAUGE );
	DWORD dwColor = D3DCOLOR_ARGB(255, 0, 0, 0 );
	
	CString strTmp;
	CEditString strEditOut;

#ifdef __AEGON_THEME_SWITCHER
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
#endif //__AEGON_THEME_SWITCHER

	strTmp.Format("%s", m_pbufText);
	strEditOut.SetParsingString(strTmp);
	strEditOut.SetColor(dwColorText);
	strEditOut.SetStyle(ESSTY_BOLD);
	p2DRender->TextOut_EditString(lpGauge->rect.left + 5, lpGauge->rect.top, strEditOut, 0, 0, 2);
	strEditOut.ClearStyle(ESSTY_BOLD);
	p2DRender->SetFont(CWndBase::m_Theme.m_pFontText);

#endif
}
 
void CCollectingWnd::OnDestroy()
{
	int a = 0;
}


void CCollectingWnd::AddSfx()
{
}

void CCollectingWnd::DeleteSfx()
{
}

HRESULT CCollectingWnd::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if(!m_pVBGauge)
	m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	
	return S_OK;
}

HRESULT CCollectingWnd::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}

HRESULT CCollectingWnd::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}

void CCollectingWnd::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
#if __VER >= 11 // __SYS_COLLECTING
	// 여기에 코딩하세요
	if(!g_pPlayer) return;
	CItemElem* pCollector = g_pPlayer->GetCollector();

	if(pCollector)
	{
		char		szTemp[64];
		//CWndStatic* pBettery = (CWndStatic*)GetDlgItem( WIDC_STATIC5 );
		CWndStatic* pLevel   = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
		CWndStatic* pPic1    = (CWndStatic*)GetDlgItem( WIDC_PIC1 );
		CWndStatic* pPic2    = (CWndStatic*)GetDlgItem( WIDC_PIC2 );
		LPWNDCTRL lpWndCtrl1 	= GetWndCtrl( WIDC_STATIC1 );
		LPWNDCTRL lpWndCtrl2 	= GetWndCtrl( WIDC_STATIC2 );
		LPWNDCTRL	lpGauge		= GetWndCtrl( WIDC_GAUGE );
		CRect 		rect		= GetClientRect();
		CRect 		rectTemp;
		int 		nWidthClient= lpGauge->rect.Width();
		int			nMax		= CCollectingProperty::GetInstance()->GetMaxBattery();
		int			nRatio		= pCollector->m_nHitPoint / nMax; 
		int			nWidth		= nWidthClient * nRatio;
		int			nBtryRemain = pCollector->m_nHitPoint;

		if(nBtryRemain < 0)  nBtryRemain = 0;
		m_BetteryRect	= lpWndCtrl1->rect;
		m_LevelRect		= lpWndCtrl2->rect;
		
		if(!m_pVBGauge)
			m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );

		m_pTexGauEmptyNormal = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), COLOR_ITEM_BG );
		m_pTexGauFillNormal  = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), COLOR_ITEM_BG );

		wsprintf(m_pbufText, "%d / %d (%d%%)", nBtryRemain, nMax, nRatio * 100);
		
		m_nGWidth = nWidth;
		rect = lpGauge->rect;
		rectTemp = rect; 
		rectTemp.right = rectTemp.left + nWidth;
		ClientToScreen( rectTemp );

		memset(szTemp, 0, 64);
		sprintf(szTemp, "%d  /  5", pCollector->GetAbilityOption());
		pLevel->SetTitle(szTemp);
		m_pTexBatt  = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, _T( "Icon_ColBattery.tga" )), COLOR_ITEM_BG );
		m_pTexLevel = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, _T( "Icon_ColLevel.tga" )), COLOR_ITEM_BG );
	}

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
#endif
} 

BOOL CCollectingWnd::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	#if __VER >= 11 // __SYS_COLLECTING
		// Daisy에서 설정한 리소스로 윈도를 연다.
		return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COLLECTING, 0, CPoint( 0, 0 ), pWndParent );
	#else
		return FALSE;
	#endif
} 

void  CCollectingWnd::Update()
{
#if __VER >= 11 // __SYS_COLLECTING
	if(!g_pPlayer) return;
	CItemElem* pCollector = g_pPlayer->GetCollector();

	if(pCollector)
	{
		char		szTemp[64];
		CWndStatic* pLevel   = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );

		memset(szTemp, 0, 64);
		sprintf(szTemp, "%d  /  5", pCollector->GetAbilityOption());
		pLevel->SetTitle(szTemp);
	}
#endif
}

BOOL CCollectingWnd::Process()
{
	return TRUE;
}


BOOL CCollectingWnd::SetButtonCaption(bool bIsStart)
{
#if __VER >= 11 // __SYS_COLLECTING
	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
	
	if (bIsStart)
		pWndButton->SetTitle("Stop");
	else
		pWndButton->SetTitle("Start");
#endif
	return true;

}

BOOL CCollectingWnd::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CCollectingWnd::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CCollectingWnd::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CCollectingWnd::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CCollectingWnd::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	#if __VER >= 11 // __SYS_COLLECTING
	if(!g_pPlayer) return FALSE;

#ifdef __SLEEPING_FIX
	if (!g_pPlayer->IsRegionAttr(RA_COLLECTING))
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_COLLECTING_REGION), NULL, prj.GetTextColor(TID_GAME_COLLECTING_REGION));
		return FALSE;
	}
#endif //__SLEEPING_FIX

	switch(nID)
	{
		case WIDC_BUTTON1 :
			m_bIsCollecting = !m_bIsCollecting;
			if(m_bIsCollecting)
			{
				if(!g_pPlayer->m_pActMover->IsFly()
#ifdef __MOUNT
#ifdef __AEGON_FIXES
					&& !(g_pPlayer->GetMountId() > 0 && g_pPlayer->GetMountId() != NULL_ID)
#else
					&& !(g_pPlayer->GetMountId() > 0)
#endif
#endif // __MOUNT
					)	
					g_DPlay.SendQueryStartCollecting();
				else
					g_WndMng.PutString( prj.GetText(TID_GAME_COLLECT_FLY), NULL, COLOR_ERROR );
			}
			else				
				g_DPlay.SendQueryStopCollecting();
			break;
	};
	#endif
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CCollectingWnd::OnEraseBkgnd(C2DRender* p2DRender)
{
	return CWndBase::OnEraseBkgnd( p2DRender );
}

void CCollectingWnd::OnMouseWndSurface( CPoint point )
{
#if __VER >= 11 // __SYS_COLLECTING
	CRect DrawRect 		= m_BetteryRect;


	if( DrawRect.PtInRect( point ) )
	{
		CPoint point2 = point;
		CString strText;

		ClientToScreen( &point2 );
		ClientToScreen( &DrawRect );

#if __VER >= 11
		strText = prj.GetText(TID_GAME_COLLECTOR_BETTERY_INFO);
#endif	// 
		g_toolTip.PutToolTip( 100000, strText, DrawRect, point2 );

	}

	DrawRect 		= m_LevelRect;

	if( DrawRect.PtInRect( point ) )
	{
		CPoint point2 = point;
		CString strText;

		ClientToScreen( &point2 );
		ClientToScreen( &DrawRect );
#if __VER >= 11		
		strText = prj.GetText(TID_GAME_COLLECTOR_LEVEL_INFO);
#endif	// 
		g_toolTip.PutToolTip( 100000, strText, DrawRect, point2 );

	}
#endif
}
