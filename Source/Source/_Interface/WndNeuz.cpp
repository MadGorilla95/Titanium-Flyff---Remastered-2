// WndArcane.cpp: implementation of the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resdata.h"
#include "WndStyle.h"
#include "WndHelp.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 타이틀 바 (축소, 최대, 종료 버튼)
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
#define WTBID_CLOSE  100
#define WTBID_MAX    101
#define WTBID_MIN    102
#define WTBID_HELP   103
#define WTBID_REPORT 104
#define WTBID_PIN    105
*/

CWndTitleBar::CWndTitleBar()
{
	m_nButtonSizeX = 0;
	m_nGapSize = 0;
}

CWndTitleBar::~CWndTitleBar()
{

}
void CWndTitleBar::OnDraw(C2DRender* p2DRender)
{
}
void CWndTitleBar::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	m_nButtonMax = 0;

#ifdef __AEGON_THEME_SWITCHER
	CTexture* pTextureClose = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "ButtWndExit.tga", COLOR_ITEM_BG, TRUE);
	m_nGapSize = 2;
	m_nButtonSizeX = pTextureClose->m_size.cx / 4 + m_nGapSize;
#endif

	if (m_pParentWnd->IsWndStyle(WBS_PIN))
	{
#ifdef __AEGON_THEME_SWITCHER
		m_awndButton[WTBID_PIN - 10000].Create(_T(""), 0, CRect(m_nButtonMax * m_nButtonSizeX, 1, m_nButtonMax * m_nButtonSizeX + m_nButtonSizeX, 20), this, WTBID_PIN), m_nButtonMax++;
#else
		m_awndButton[WTBID_PIN - 10000].Create(_T(""), 0, CRect(m_nButtonMax * 16, 1, m_nButtonMax * 16 + 16, 20), this, WTBID_PIN), m_nButtonMax++;
#endif
	}

	if (m_pParentWnd->IsWndStyle(WBS_VIEW))
	{
#ifdef __AEGON_THEME_SWITCHER
		m_awndButton[WTBID_REPORT - 10000].Create(_T(""), 0, CRect(m_nButtonMax * m_nButtonSizeX, 1, m_nButtonMax * m_nButtonSizeX + m_nButtonSizeX, 20), this, WTBID_REPORT), m_nButtonMax++;
#else
		m_awndButton[WTBID_REPORT - 10000].Create(_T(""), 0, CRect(m_nButtonMax * 16, 1, m_nButtonMax * 16 + 16, 20), this, WTBID_REPORT), m_nButtonMax++;
#endif
	}

	if (m_pParentWnd->IsWndStyle(WBS_EXTENSION))
	{
#ifdef __AEGON_THEME_SWITCHER
		m_awndButton[WTBID_EXTENSION - 10000].Create(_T(""), WBS_CHECK | WBS_PUSHLIKE, CRect(m_nButtonMax * (m_nButtonSizeX - 1), 0, m_nButtonMax * m_nButtonSizeX + m_nButtonSizeX, 20), this, WTBID_EXTENSION), m_nButtonMax++;
#else
		m_awndButton[WTBID_EXTENSION - 10000].Create(_T(""), WBS_CHECK | WBS_PUSHLIKE, CRect(m_nButtonMax * 15, 0, m_nButtonMax * 16 + 16, 20), this, WTBID_EXTENSION), m_nButtonMax++;
#endif
	}

	if(m_pParentWnd->IsWndStyle( WBS_HELP ) )
	{
#ifdef __AEGON_THEME_SWITCHER
		CTexture* pTextureHelp = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "ButtWndHelp.tga", COLOR_ITEM_BG, TRUE);

		m_awndButton[WTBID_HELP - 10000].Create(_T(""), 0, CRect(m_nButtonMax * m_nButtonSizeX, 1, m_nButtonMax * m_nButtonSizeX + m_nButtonSizeX, 20), this, WTBID_HELP), m_nButtonMax++;
		m_awndButton[WTBID_HELP - 10000].SetTexture(m_pApp->m_pd3dDevice, NULL, pTextureHelp);
#else
		m_awndButton[ WTBID_HELP - 10000 ].Create( _T( "" ), 0, CRect( m_nButtonMax * 16, 1 , m_nButtonMax * 16 + 16, 20), this, WTBID_HELP ), m_nButtonMax++;
		m_awndButton[ WTBID_HELP - 10000 ].SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtWndHelp.tga" ), TRUE );
#endif
		m_awndButton[WTBID_HELP - 10000].FitTextureSize();
	}

	if (m_pParentWnd->IsWndStyle(WBS_MINIMIZEBOX)) 
	{
#ifdef __AEGON_THEME_SWITCHER
		CTexture* pTextureMin = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "ButtWndMin.tga", COLOR_ITEM_BG, TRUE);
		m_awndButton[WTBID_MIN - 10000].Create(_T(""), 0, CRect(m_nButtonMax * m_nButtonSizeX, 1, m_nButtonMax * m_nButtonSizeX + m_nButtonSizeX, 20), this, WTBID_MIN), m_nButtonMax++;
		m_awndButton[WTBID_MIN - 10000].SetTexture(m_pApp->m_pd3dDevice, NULL, pTextureMin);
#else
		m_awndButton[WTBID_MIN - 10000].Create(_T(""), 0, CRect(m_nButtonMax * 16, 1, m_nButtonMax * 16 + 16, 20), this, WTBID_MIN), m_nButtonMax++;
		m_awndButton[WTBID_MIN - 10000].SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtWndMin.tga"), TRUE);
#endif
		m_awndButton[WTBID_MIN - 10000].FitTextureSize();
	}

	if(m_pParentWnd->IsWndStyle( WBS_MAXIMIZEBOX ) )
	{
#ifdef __AEGON_THEME_SWITCHER
		CTexture* pTextureMax = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "ButtWndMax.tga", COLOR_ITEM_BG, TRUE);
		m_awndButton[WTBID_MAX - 10000].Create(_T(""), 0, CRect(m_nButtonMax * m_nButtonSizeX, 1, m_nButtonMax * m_nButtonSizeX + m_nButtonSizeX, 20), this, WTBID_MAX), m_nButtonMax++;
		m_awndButton[WTBID_MAX - 10000].SetTexture(m_pApp->m_pd3dDevice, NULL, pTextureMax);
#else
		m_awndButton[WTBID_MAX - 10000].Create(_T(""), 0, CRect(m_nButtonMax * 16, 1, m_nButtonMax * 16 + 16, 20), this, WTBID_MAX), m_nButtonMax++;
		m_awndButton[WTBID_MAX - 10000].SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtWndMax.tga"), TRUE);
#endif
		m_awndButton[WTBID_MAX - 10000].FitTextureSize();
	}
	if(m_pParentWnd->IsWndStyle( WBS_CAPTION ) )
	{
		if( m_pParentWnd->m_bNoCloseButton == FALSE )
		{
#ifdef __AEGON_THEME_SWITCHER
			m_awndButton[WTBID_CLOSE - 10000].Create(_T(""), 0, CRect(m_nButtonMax * m_nButtonSizeX, 1, m_nButtonMax * m_nButtonSizeX + m_nButtonSizeX, 20), this, WTBID_CLOSE), m_nButtonMax++;
			CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchSetTexture((CWndBase*)&m_awndButton[WTBID_CLOSE - 10000], m_pApp->m_pd3dDevice, "ButtWndExit.tga", TRUE);
#else
			m_awndButton[WTBID_CLOSE - 10000].Create(_T(""), 0, CRect(m_nButtonMax * 16, 1, m_nButtonMax * 16 + 16, 20), this, WTBID_CLOSE), m_nButtonMax++;
			m_awndButton[WTBID_CLOSE - 10000].SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtWndExit.tga"), TRUE);
#endif
			m_awndButton[WTBID_CLOSE - 10000].FitTextureSize();
		}
	}
	
#ifdef __AEGON_THEME_SWITCHER
	CRect rect(0, 4, m_nButtonMax * m_nButtonSizeX, 20 + CAegonThemeSwitcherMng::GetInstance()->GetTitleBarYOffset());
#else
	CRect rect( 0, 4, m_nButtonMax * 16, 20 );
#endif

	SetWndRect( rect );

		///wndTitleBar.Create( WBS_CHILD | WBS_DOCKING | WBS_NOFRAME, CRect(0,0,48,20), this, 100000);// ,m_pSprPack,9);
		//m_wndTitleBar.SetTitle("fuck");
		//m_wndTitleBar.Move(CPoint(GetWindowRect().Width()-52,3)); //SetWndRect(rect);

}
void CWndTitleBar::Replace()
{
#ifdef __AEGON_THEME_SWITCHER
	if (m_pParentWnd)
		Move(CPoint((m_pParentWnd->GetWindowRect().Width() - m_nButtonMax * 16 - 7) + CAegonThemeSwitcherMng::GetInstance()->GetTitleBarPosOffset(m_pParentWnd->GetWndId()).cx, 4 + CAegonThemeSwitcherMng::GetInstance()->GetTitleBarPosOffset(m_pParentWnd->GetWndId()).cy));
#else
	if( m_pParentWnd )
		Move( CPoint( m_pParentWnd->GetWindowRect().Width() - m_nButtonMax * 16 - 7, 4 ) ); 
#endif
}
BOOL CWndTitleBar::Initialize(CWndBase* pWndParent,DWORD dwStyle)
{
	return TRUE;
}
BOOL CWndTitleBar::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	//if(m_pParentWnd) m_pParentWnd->OnCommand(nID,dwMessage);
	return TRUE;
}
BOOL CWndTitleBar::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if(m_pParentWnd) m_pParentWnd->OnChildNotify( message, nID, pLResult );
	return TRUE;
}
void CWndTitleBar::OnSize(UINT nType, int cx, int cy)
{
}
BOOL CWndTitleBar::OnEraseBkgnd(C2DRender* p2DRender)
{
	return 1;
}

#ifdef __AEGON_MINIMIZE_WINDOWS
void CWndTitleBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWndBase* pWndParent = GetParentWnd();
	if (pWndParent && pWndParent->m_nMinimizedMode == __AEG_MINIMIZE_MODE_TEX)
	{
		pWndParent->OnMinimizeToggle();
	}
}
void CWndTitleBar::SetButtonVisible(BOOL bVisible) 
{
	if (m_pParentWnd->IsWndStyle(WBS_PIN)) 
	{
		m_awndButton[WTBID_PIN - 10000].SetVisible(bVisible);
		m_awndButton[WTBID_PIN - 10000].EnableWindow(bVisible);
	}

	if (m_pParentWnd->IsWndStyle(WBS_VIEW)) 
	{
		m_awndButton[WTBID_REPORT - 10000].SetVisible(bVisible);
		m_awndButton[WTBID_REPORT - 10000].EnableWindow(bVisible);
	}

	if (m_pParentWnd->IsWndStyle(WBS_EXTENSION)) 
	{
		m_awndButton[WTBID_EXTENSION - 10000].SetVisible(bVisible);
		m_awndButton[WTBID_EXTENSION - 10000].EnableWindow(bVisible);
	}

	if (m_pParentWnd->IsWndStyle(WBS_HELP))
	{
		m_awndButton[WTBID_HELP - 10000].SetVisible(bVisible);
		m_awndButton[WTBID_HELP - 10000].EnableWindow(bVisible);
	}
	if (m_pParentWnd->IsWndStyle(WBS_MINIMIZEBOX))
	{
		m_awndButton[WTBID_MIN - 10000].SetVisible(bVisible);
		m_awndButton[WTBID_MIN - 10000].EnableWindow(bVisible);
	}
	if (m_pParentWnd->IsWndStyle(WBS_MAXIMIZEBOX))
	{
		m_awndButton[WTBID_MAX - 10000].SetVisible(bVisible);
		m_awndButton[WTBID_MAX - 10000].EnableWindow(bVisible);
	}
	if (m_pParentWnd->IsWndStyle(WBS_CAPTION))
	{
		m_awndButton[WTBID_CLOSE - 10000].SetVisible(bVisible);
		m_awndButton[WTBID_CLOSE - 10000].EnableWindow(bVisible);
	}
}
#endif
#ifdef __AEGON_THEME_SWITCHER
void CWndTitleBar::OnThemeSwitch() 
{
	CTexture* pTextureClose = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "ButtWndExit.tga", COLOR_ITEM_BG, TRUE);
	m_nButtonSizeX = pTextureClose->m_size.cx / 4 + m_nGapSize;

	int nButtonNum = 0;

	if (m_pParentWnd->IsWndStyle(WBS_PIN))
		nButtonNum++;

	if (m_pParentWnd->IsWndStyle(WBS_VIEW))
		nButtonNum++;

	if (m_pParentWnd->IsWndStyle(WBS_EXTENSION))
		nButtonNum++;

	if (m_pParentWnd->IsWndStyle(WBS_HELP))
	{
		CTexture* pTextureHelp = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "ButtWndHelp.tga", COLOR_ITEM_BG, TRUE);
		m_awndButton[WTBID_HELP - 10000].SetTexture(m_pApp->m_pd3dDevice, NULL, pTextureHelp);
		m_awndButton[WTBID_HELP - 10000].SetWndRect(CRect(nButtonNum * m_nButtonSizeX, 1, nButtonNum * m_nButtonSizeX + m_nButtonSizeX, 20));
		nButtonNum++;
		m_awndButton[WTBID_HELP - 10000].FitTextureSize();
	}
	if (m_pParentWnd->IsWndStyle(WBS_MINIMIZEBOX))
	{
		CTexture* pTextureMin = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "ButtWndMin.tga", COLOR_ITEM_BG, TRUE);
		m_awndButton[WTBID_MIN - 10000].SetTexture(m_pApp->m_pd3dDevice, NULL, pTextureMin);
		m_awndButton[WTBID_MIN - 10000].SetWndRect(CRect(nButtonNum * m_nButtonSizeX, 1, nButtonNum * m_nButtonSizeX + m_nButtonSizeX, 20));
		nButtonNum++;
		m_awndButton[WTBID_MIN - 10000].FitTextureSize();
	}
	if (m_pParentWnd->IsWndStyle(WBS_MAXIMIZEBOX))
	{
		CTexture* pTextureMax = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "ButtWndMax.tga", COLOR_ITEM_BG, TRUE);
		m_awndButton[WTBID_MAX - 10000].SetTexture(m_pApp->m_pd3dDevice, NULL, pTextureMax);
		m_awndButton[WTBID_MAX - 10000].SetWndRect(CRect(nButtonNum * m_nButtonSizeX, 1, nButtonNum * m_nButtonSizeX + m_nButtonSizeX, 20));
		nButtonNum++;
		m_awndButton[WTBID_MAX - 10000].FitTextureSize();
	}
	if (m_pParentWnd->IsWndStyle(WBS_CAPTION))
	{
		m_awndButton[WTBID_CLOSE - 10000].SetTexture(m_pApp->m_pd3dDevice, NULL, pTextureClose);
		m_awndButton[WTBID_CLOSE - 10000].SetWndRect(CRect(nButtonNum * m_nButtonSizeX, 1, nButtonNum * m_nButtonSizeX + m_nButtonSizeX, 20));
		nButtonNum++;
		m_awndButton[WTBID_CLOSE - 10000].FitTextureSize();
	}

	CRect rectTitleBar(0, 4, m_nButtonMax * m_nButtonSizeX, 20 + CAegonThemeSwitcherMng::GetInstance()->GetTitleBarYOffset());
	SetWndRect(rectTitleBar);

	Replace();
}
#endif // __AEGON_THEME_SWITCHER
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Neuz 윈도 기본 베이스 클레스 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndNeuz::CWndNeuz()
{
	m_bFullMax = FALSE;	
	m_dwWID = 0;
	m_ptMouseCenter = CPoint( -1, -1 );
//	m_bWndTile = FALSE;//TRUE;
	m_dwColor = 0xffffffff;
	m_bPutRegInfo = TRUE;
	m_d3dFormat = D3DFMT_A4R4G4B4 ;

}

CWndNeuz::~CWndNeuz()
{
	// PLAYSND( SND_INF_CLOSE );
	for( int i = 0; i < m_wndArrayTemp.GetSize(); i++ )
		safe_delete( (CWndBase*)m_wndArrayTemp.GetAt( i ) );
#ifdef __CLIENT
	// 애플렛으로 등록된 윈도만 파괴할 때 윈도의 정보를 저장한다. 
	//if( g_WndMng.GetAppletFunc( GetWndId() ) && m_bPutRegInfo )
	//	g_WndMng.PutRegInfo( this, 1, FALSE );
	//	g_WndMng.PutRegInfo( GetWndId(), GetWindowRect( TRUE ), FALSE );
	// 타일 형태의 윈도는 윈도가 종료할 때 텍스춰를 파괴해야한다.
	if( m_strTexture.IsEmpty() == FALSE )
	{
		// 타일 형태는 파일을 읽는 것이 아니므로 자기 포인터가 텍스춰 ID로 사용되었다.
		CString strTextureId;
		strTextureId.Format( "%p", this );
		m_textureMng.RemoveTexture( strTextureId );

#ifdef __AEGON_APNG
		m_animatedTextureMng.RemoveAnimatedTexture(strTextureId);
#endif // __AEGON_APNG
	}
#endif
}
#ifdef __CLIENT
void CWndNeuz::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	if( ar.IsLoading() )
	{

	}
	else
	{
		dwVersion = 0;
	}
}
#endif
void CWndNeuz::OnDraw(C2DRender* p2DRender)
{
}
void CWndNeuz::PaintFrame( C2DRender* p2DRender )
{
	if( IsFullMax() && m_nWinSize == WSIZE_MAX )
		return;
	CWndBase::PaintFrame( p2DRender );
}

void CWndNeuz::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();
	
	m_rectBackup = m_rectWindow;

	int x = m_rectWindow.Width() / 2;
	int y = m_rectWindow.Height() - 45;
	CSize size = CSize( 60, 25 );//m_pSprPack->GetAt(9)->GetSize();
//	//DWORD dwStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID);
	if( IsWndStyle( WBS_CAPTION ) )
	{ 
		m_wndTitleBar.Create( WBS_CHILD | WBS_DOCKING | WBS_NODRAWFRAME | WBS_NOFRAME, CRect(0,0,0,0), this, 100000 );
		m_wndTitleBar.SetTitle( "fuck" );
		m_wndTitleBar.Replace(); 
	}
//	m_wndButton.Create("OK",0,CRect(0,0,0+size.cx,0+size.cy),this,IDOK);//,m_pSprPack,9);
//	m_wndClose.SetDefault(TRUE);

	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( GetWndId() );
	if( lpWndApplet )
	{
//#ifndef __NEWINTERFACE
//		lpWndApplet->bTile = FALSE;
//#endif
		SetTitle( lpWndApplet->strTitle );
		m_bTile = ( lpWndApplet->bTile != FALSE );
		m_strTexture = lpWndApplet->strTexture;
		m_d3dFormat = lpWndApplet->d3dFormat;

#ifdef __AEGON_THEME_SWITCHER
		map<int, __AEGON_THEME_UNIT>::iterator it = CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.find(g_Option.m_nSelectedThemeID);
		if (it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end())
		{
			if (it->second.nThemeType == 1 && m_bTile) 
			{
				m_d3dFormat = it->second.d3dFormat;
			}
		}
#endif // __AEGON_THEME_SWITCHER

		for( int i = 0; i < lpWndApplet->ptrCtrlArray.GetSize(); i++ )
		{
			m_wndArrayTemp.Add(
				CreateControl( m_pApp->GetSafeHwnd(), (LPWNDCTRL)lpWndApplet->ptrCtrlArray.GetAt( i ) )
			);
		}
	}


//	SetFocus();
	//m_bWndTile = TRUE;
//	AdjustWndBase();
#if __VER >= 8	
#ifdef __DISABLE_GAMMA_WND
	if(g_Neuz.m_bStartFullscreen)
#endif
	g_Neuz.Drv_SetGamma( g_Neuz.GetSafeHwnd(), g_Option.m_fGamma, g_Option.m_nOverBright, g_Option.m_fContrast );
#endif //__Y_GAMMA_CONTROL_8
}

void CWndNeuz::AdjustWndBase( D3DFORMAT d3dFormat)
{
	if( m_strTexture.IsEmpty() || IsVisible() == FALSE || IsWndStyle( WBS_NODRAWFRAME ) )
		return;

	if( m_bTile == FALSE )
	{
#ifdef __AEGON_THEME_SWITCHER
		CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchSetTexture((CWndBase*)this, m_pApp->m_pd3dDevice, m_strTexture, TRUE);
#else // __AEGON_THEME_SWITCHER
		SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, m_strTexture), TRUE);
#endif // __AEGON_THEME_SWITCHER
		return;
	}
	CWndBase::AdjustWndBase( m_d3dFormat );
}


void CWndNeuz::AdditionalSkinTexture( LPWORD pDest, CSize size1, D3DFORMAT d3dFormat )
{
	if( m_strTexture.IsEmpty() )
		return;

	CRect rect = GetWindowRect( TRUE );
	CSize size2 = CSize( rect.Width(), rect.Height() );

	LPIMAGE lpImage[12];
	CSize sizeBmp[1]; 
	CString strFileName;
	/*
	0 1 2
	3 4 5
	6 7 8
	9 0 1
	*/
//	m_strTile = "WndTile00.bmp";
	int nImgBit = IMGBIT_32;
	int i = 0;
	for( int i = 0; i < 12; i++ )
	{
		CString strTemp1 = m_strTexture.Left( m_strTexture.GetLength() - 6 );
		CString strTemp2 = m_strTexture.Right( 4 );
		strFileName.Format( "%s%02d%s", strTemp1.GetString(), i, strTemp2.GetString() );

		if( m_strWndTileMap.Lookup( strFileName, (void*&)lpImage[i] ) == FALSE)
		{
			lpImage[i] = new IMAGE;
			lpImage[i]->size.cx = 99;
#ifdef __AEGON_THEME_SWITCHER
			map<int, __AEGON_THEME_UNIT>::iterator it = CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.find(g_Option.m_nSelectedThemeID);
			BOOL bSuccess = FALSE;
			if (it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end())
			{
				if (LoadImage(MakePath(_T(it->second.szThemeFolder), strFileName), lpImage[i]))
				{
					bSuccess = TRUE;
				}
			}
			if(!bSuccess)
			{
				if (LoadImage(MakePath(DIR_THEME, strFileName), lpImage[i]) == FALSE)
					Error("CWndNeuz::AdditionalSkinTexture에서 %s Open1 실패", strFileName);
			}
#else // __AEGON_THEME_SWITCHER
			if( LoadImage( MakePath( DIR_THEME, strFileName ), lpImage[i] ) == FALSE )
				Error( "CWndNeuz::AdditionalSkinTexture에서 %s Open1 실패", strFileName );
#endif // __AEGON_THEME_SWITCHER

			m_strWndTileMap.SetAt( strFileName, lpImage[i] );
		}
		//else
		//{
		//	sizeBmp[i] = CSize(16,16);
		//}
	}
	sizeBmp[0] = CSize(16,16);
	CSize sizeDiv = size2;
	sizeDiv.cx %= sizeBmp[ 0 ].cx;
	sizeDiv.cy %= sizeBmp[ 0 ].cy;
	size2.cx /= sizeBmp[ 0 ].cx; size2.cx *= sizeBmp[ 0 ].cx;
	size2.cy /= sizeBmp[ 0 ].cy; size2.cy *= sizeBmp[ 0 ].cy;
	if( sizeDiv.cx ) size2.cx += sizeBmp[ 0 ].cx;
	if( sizeDiv.cy ) size2.cy += sizeBmp[ 0 ].cy;

	i = 0;
	CSize sizeTile = size2;
	sizeTile.cx /= 16;
	sizeTile.cy /= 16;
	CPoint point( 0, 0);
	
	for( int y = 0; y < sizeTile.cy; y++ )
	{
		point.x = 0;
		for( int x = 0; x < sizeTile.cx; x++)
		{
			///////////////////////////////
			if( x == 0 && y == 0 ) i = 0; else
			if( x < sizeTile.cx - 1 && y == 0 ) i = 1; else
			if( x == sizeTile.cx - 1 && y == 0 ) i = 2; else
			///////////////////////////////
			if( x == 0 && y == 1 ) i = 3; else
			if( x < sizeTile.cx - 1 && y == 1 ) i = 4; else
			if( x == sizeTile.cx - 1 && y == 1 ) i = 5;	else
			///////////////////////////////
			if( x == 0 && y < sizeTile.cy - 1 )	i = 6; else
			if( x < sizeTile.cx - 1 && y < sizeTile.cy - 1 ) i = 7; else
			if( x == sizeTile.cx - 1 && y < sizeTile.cy - 1 ) i = 8; else
			///////////////////////////////
			if( x == 0 && y == sizeTile.cy - 1 ) i = 9; else
			if( x < sizeTile.cx - 1 && y == sizeTile.cy - 1 ) i = 10; else
			if( x == sizeTile.cx - 1 && y == sizeTile.cy - 1 ) i = 11;
			PaintTexture( pDest, lpImage[i], point, size1 );
			point.x += 16;
		}
		point.y += 16;
	}

}
BOOL CWndNeuz::Initialize(CWndBase* pWndParent,DWORD dwStyle)
{
	CRect rect(0,0,300,300);
	//m_strMessage = lpszMessage;
	//m_nType = nType;
	//SetTitle("메지시 윈도");
	return CWndBase::Create(dwStyle | WBS_MOVE|/*WBS_MODAL|*/WBS_SOUND|WBS_CAPTION,rect,m_pWndRoot,10);
}
BOOL CWndNeuz::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return TRUE;
}
BOOL CWndNeuz::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == WNM_CLICKED )
	{
		switch(nID)
		{
			case WTBID_HELP: // 핼프 버튼
				{
#ifdef __CLIENT					
					LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( GetWndId() );
#ifdef __HELP_BUG_FIX
					if(g_WndMng.m_pWndHelpInstant)
						SAFE_DELETE(g_WndMng.m_pWndHelpInstant);

					g_WndMng.m_pWndHelpInstant = new CWndHelpInstant;
					g_WndMng.m_pWndHelpInstant->m_strHelpKey = lpWndApplet->strToolTip;
					g_WndMng.m_pWndHelpInstant->Initialize();
#else //__HELP_BUG_FIX
					// 헬프 이중생성 방지
					for( int i=0; i<g_vecHelpInsKey.size(); i++ )
					{
						if( g_vecHelpInsKey[i] == lpWndApplet->strToolTip )
							return FALSE;
					}

					CWndHelpInstant* pHelpInstant = new CWndHelpInstant;
					pHelpInstant->m_strHelpKey = lpWndApplet->strToolTip;

					// 헬프 이중생성 방지
					g_vecHelpInsKey.push_back( pHelpInstant->m_strHelpKey );

					pHelpInstant->Initialize();
#endif //__HELP_BUG_FIX
#endif
				}
				break;
			case WTBID_MIN: // 최소화 
#ifdef __AEGON_MINIMIZE_WINDOWS
				OnMinimizeToggle();
#endif
				break;
			case WTBID_MAX: // 최대화 
				if( m_nWinSize == WSIZE_WINDOW )
					SetSizeMax();
				else
					SetSizeWnd();
				break;
			case WTBID_CLOSE: // 종료
				{
#ifdef __CLIENT		
#ifndef __HELP_BUG_FIX
					// 헬프 이중생성 방지
					if( GetWndId() == APP_HELP_INSTANT )
					{
						CString strKey = ((CWndHelpInstant*)this)->m_strHelpKey;
						vector<CString>::iterator where = find(g_vecHelpInsKey.begin(), g_vecHelpInsKey.end(), strKey );

						if(where != g_vecHelpInsKey.end())
							g_vecHelpInsKey.erase(where);
					}
#endif //__HELP_BUG_FIX
#endif
					
					Destroy();
				}

				break;
		}
	}


	return TRUE;
}
void CWndNeuz::SetSizeMax()
{
	if( m_bFullMax )
	{
		if( m_nWinSize == WSIZE_WINDOW )
		{
			m_rectBackup = GetWindowRect(TRUE);
			CRect rect;
			CWndBase* pWndFull 
				= m_pWndRoot->FindFullWnd();
			if(pWndFull)
				rect = pWndFull->GetWindowRect(TRUE);
			//rect = pWndFull->GetLayoutRect( TRUE );
			else
				if(IsWndStyle(WBS_CHILD))
					rect = m_pParentWnd->GetClientRect(TRUE);
				else
				{
					//rect = m_pWndRoot->GetWindowRect();
					rect = m_pWndRoot->GetLayoutRect();
					//rect.left = rect.Width() - 200;
				}
				m_wndTitleBar.GetDlgItem( WTBID_MAX )->SetTitle( _T( "#" ) );
				m_bFullWnd = TRUE;
				m_nWinSize = WSIZE_MAX;
				SetWndRect(rect);
		}
	}
	else
	if( m_nWinSize == WSIZE_WINDOW )
	{
		m_rectBackup = GetWindowRect(TRUE);
		CRect rect;
		CWndBase* pWndFull 
			= m_pWndRoot->FindFullWnd();
		if(pWndFull)
			rect = pWndFull->GetWindowRect(TRUE);
		else
			if(IsWndStyle(WBS_CHILD))
				rect = m_pParentWnd->GetClientRect(TRUE);
			else
			{
				//rect = m_pWndRoot->GetWindowRect();
				rect = m_pWndRoot->GetLayoutRect();
				rect.left = rect.Width() - 200;
			}
			SetWndRect(rect);
			m_wndTitleBar.GetDlgItem( WTBID_MAX )->SetTitle( _T( "#" ) );
			AddWndStyle(WBS_MANAGER);
			m_bFullWnd = TRUE;
			m_nWinSize = 2;
	}
}
void CWndNeuz::SetSizeWnd()
{
	if( m_bFullMax )
	{
		if( m_nWinSize != WSIZE_WINDOW )
		{
			m_nWinSize = WSIZE_WINDOW;
			SetWndRect(m_rectBackup);
			m_wndTitleBar.GetDlgItem( WTBID_MAX )->SetTitle( _T( "#" ) );
		}
	}
	else
	{
		if( m_nWinSize != WSIZE_WINDOW )
		{
			m_nWinSize = WSIZE_WINDOW;
			SetWndRect(m_rectBackup);
			m_wndTitleBar.GetDlgItem( WTBID_MAX )->SetTitle( _T( "#" ) );
			DelWndStyle(WBS_MANAGER);
		}
	}
}
void CWndNeuz::OnSize(UINT nType, int cx, int cy)
{
	if(IsWndRoot())
		return;

	if( m_bTile ) //m_strTexture.IsEmpty() == FALSE )
	{
		CRect rectWnd = GetWndRect();
		CSize size2( rectWnd.Width(), rectWnd.Height() );
		CSize sizeDiv = size2;
		sizeDiv.cx %= 16;
		sizeDiv.cy %= 16;
		size2.cx /= 16; size2.cx *= 16;
		size2.cy /= 16; size2.cy *= 16;
		if( sizeDiv.cx ) size2.cx += 16;
		if( sizeDiv.cy ) size2.cy += 16;
		rectWnd.bottom = rectWnd.top + size2.cy;
		rectWnd.right = rectWnd.left + size2.cx;
		SetWndRect( rectWnd, FALSE );
	}
	AdjustWndBase();
	m_wndTitleBar.Replace(); 

	CWndBase::OnSize( nType, cx, cy );

//	if( rectOld.Width() != m_rectClient.Width() || rectOld.Height() != m_rectClient.Height() )


	// 차일드 윈도우들의 사이즈를 조절 
	/*
	for(int i = 0; i < m_wndArray.GetSize(); i++)
	{
		CWndBase* pWnd = (CWndBase*)m_wndArray.GetAt(i);
		if(pWnd->IsWndStyle(WBS_DOCKED))// && pWnd->IsWndStyle(WBS_CHILD))
		{
			CRect rect = pWnd->GetWindowRect(TRUE);
			rect.SetRect(0,0,cx,cy);
			pWnd->SetWndRect(rect);
		}
	}
	*/
	/*
	if(IsWndStyle(WBS_DOCKED))// && IsWndStyle(WBS_CHILD))
	{
		m_wndTitleBar.m_wndMinimize.SetVisible(FALSE);
		m_wndTitleBar.m_wndMaximize.SetVisible(FALSE);
	}
	*/
}
void CWndNeuz::OnNonClientLButtonDblClk( UINT nFlags, CPoint point )
{

	if (IsWndStyle(WBS_MAXIMIZEBOX))
		OnChildNotify(WNM_CLICKED, WTBID_MAX, NULL);
#ifdef __AEGON_MINIMIZE_WINDOWS
	else if(IsWndStyle(WBS_MINIMIZEBOX))
		OnMinimizeToggle();
#endif
}
// 윈도 사이즈를 세팅한다.
// 클라이언트 사이즈도 자동으로 조정된다.
void CWndNeuz::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	CRect rectOld = m_rectClient;
	if( m_bFullMax  && m_nWinSize == 2 )
	{
		m_rectWindow = rectWnd;
		m_rectClient = m_rectWindow;
		if( bOnSize && ( rectOld.Width() != m_rectClient.Width() || rectOld.Height() != m_rectClient.Height() ) )
			OnSize(0,m_rectClient.Width(),m_rectClient.Height());
	}
	else
		CWndBase::SetWndRect( rectWnd, bOnSize );
}
CWndBase* CWndNeuz::CreateControl( HWND hWnd, LPWNDCTRL lpWndCtrl ) 
{
	DWORD dwWndStyle = lpWndCtrl->dwWndStyle;
	CWndBase* pWndBase = NULL;
	switch( lpWndCtrl->dwWndType )
	{
	case WTYPE_BUTTON:
		pWndBase = new CWndButton;
		((CWndButton*)pWndBase)->Create( lpWndCtrl->strTitle, dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );

#ifdef __AEGON_THEME_SWITCHER
		if (lpWndCtrl->strTexture.IsEmpty() == FALSE) 
		{
			CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchSetTexture((CWndBase*)pWndBase, m_pApp->m_pd3dDevice, lpWndCtrl->strTexture, TRUE);
		}
		if (lpWndCtrl->strTexture.IsEmpty() == FALSE)
			((CWndButton*)pWndBase)->m_strTextureName = lpWndCtrl->strTexture;
#else // __AEGON_THEME_SWITCHER
		if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
			((CWndButton*)pWndBase)->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, lpWndCtrl->strTexture ), TRUE );
#endif // __AEGON_THEME_SWITCHER

		pWndBase->m_bTile = (lpWndCtrl->bTile != FALSE);

		((CWndButton*)pWndBase)->SetGroup( lpWndCtrl->m_bGroup );
		break;
	case WTYPE_TEXT:
		pWndBase = new CWndText;
		((CWndText*)pWndBase)->Create( dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
		((CWndText*)pWndBase)->m_string = lpWndCtrl->strTitle;

		if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
			((CWndStatic*)pWndBase)->m_strTexture = lpWndCtrl->strTexture;
		pWndBase->m_bTile = (lpWndCtrl->bTile != FALSE);

		break;
	case WTYPE_STATIC:
		pWndBase = new CWndStatic;
		((CWndStatic*)pWndBase)->Create( lpWndCtrl->strTitle, dwWndStyle | WBS_CAPTION, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
		if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
			((CWndStatic*)pWndBase)->m_strTexture = lpWndCtrl->strTexture;

		pWndBase->m_bTile = (lpWndCtrl->bTile != FALSE);

		break;
	case WTYPE_EDITCTRL:
		pWndBase = new CWndEdit;
		((CWndEdit*)pWndBase)->Create( hWnd, dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
		//((CWndButton*)pWndBase)->Create( _T( "Button" ), 0, CRect( 0, 0, 100, 20), &m_dialogWnd, 10 );

		if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
			((CWndStatic*)pWndBase)->m_strTexture = lpWndCtrl->strTexture;
		pWndBase->m_bTile = (lpWndCtrl->bTile != FALSE);

		break;
	case WTYPE_LISTBOX:
		pWndBase = new CWndListBox;
		((CWndListBox*)pWndBase)->Create( dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
		//((CWndButton*)pWndBase)->Create( _T( "Button" ), 0, CRect( 0, 0, 100, 20), &m_dialogWnd, 10 );
		if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
			((CWndStatic*)pWndBase)->m_strTexture = lpWndCtrl->strTexture;
		pWndBase->m_bTile = (lpWndCtrl->bTile != FALSE);
		break;
	case WTYPE_LISTCTRL:
		pWndBase = new CWndListCtrl;
		((CWndListCtrl*)pWndBase)->Create( dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
		//((CWndButton*)pWndBase)->Create( _T( "Button" ), 0, CRect( 0, 0, 100, 20), &m_dialogWnd, 10 );
		if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
			((CWndStatic*)pWndBase)->m_strTexture = lpWndCtrl->strTexture;
		pWndBase->m_bTile = (lpWndCtrl->bTile != FALSE);
		break;
	case WTYPE_COMBOBOX:
		pWndBase = new CWndComboBox;
		((CWndComboBox*)pWndBase)->Create( dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
		if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
			((CWndStatic*)pWndBase)->m_strTexture = lpWndCtrl->strTexture;
		pWndBase->m_bTile = (lpWndCtrl->bTile != FALSE);
		break;
	case WTYPE_TABCTRL:
		pWndBase = new CWndTabCtrl;
		((CWndTabCtrl*)pWndBase)->Create( dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
		//((CWndButton*)pWndBase)->Create( _T( "Button" ), 0, CRect( 0, 0, 100, 20), &m_dialogWnd, 10 );
#ifdef __AEGON_THEME_SWITCHER
		if (lpWndCtrl->strTexture.IsEmpty() == FALSE)
			((CWndTabCtrl*)pWndBase)->m_strTexture = lpWndCtrl->strTexture;
#endif // __AEGON_THEME_SWITCHER
		break;
	case WTYPE_TREECTRL:
		pWndBase = new CWndTreeCtrl;
		((CWndTreeCtrl*)pWndBase)->Create( dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
		//((CWndButton*)pWndBase)->Create( _T( "Button" ), 0, CRect( 0, 0, 100, 20), &m_dialogWnd, 10 );
		if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
			((CWndStatic*)pWndBase)->m_strTexture = lpWndCtrl->strTexture;
		pWndBase->m_bTile = (lpWndCtrl->bTile != FALSE);
		break;
	} 
	if( pWndBase )
		pWndBase->m_strToolTip = lpWndCtrl->strToolTip;
	return pWndBase;
}

BOOL CWndNeuz::InitDialog( HWND hWnd, DWORD dwWID, DWORD dwStyle, CPoint ptLeftTop, CWndBase* pWndParent )
{
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt(dwWID);
	CRect rect;

	if (dwWID == APP_LOGIN)
		rect = CRect(ptLeftTop.x, ptLeftTop.y, ptLeftTop.x + lpWndApplet->size.cx, ptLeftTop.y + 200);
	else
		rect = CRect( ptLeftTop.x, ptLeftTop.y, ptLeftTop.x + lpWndApplet->size.cx, ptLeftTop.y + lpWndApplet->size.cy );

	if( m_ptMouseCenter.x != -1 )
	{
		GET_CLIENT_POINT( hWnd, point );
		rect.OffsetRect( -point );
	}
	//m_strToolTip = lpWndApplet->strToolTip;
	if( lpWndApplet->strToolTip.IsEmpty() )
		return CWndNeuz::Create( lpWndApplet->dwWndStyle | dwStyle, rect, pWndParent, lpWndApplet->dwWndId );
	return CWndNeuz::Create( lpWndApplet->dwWndStyle | dwStyle | WBS_HELP, rect, pWndParent, lpWndApplet->dwWndId );
}  

BOOL CWndNeuz::OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	if( IsWndStyle( WBS_THICKFRAME ) && ( IsFullMax() == FALSE || m_nWinSize != WSIZE_MAX ) )
	{
		if( m_bLButtonDown == FALSE )
		{
			CPoint point = GetMousePoint();
			ClientToScreen( &point );
			CRect rectWindow = GetWindowRect( TRUE );
			point -= rectWindow.TopLeft();
			int nResizeDir = GetResizeDir( point );
			if( nResizeDir )
			{
				// 1 = top
				// 2 = bottom
				// 3 = left
				// 4 = right
				// 5 = topLeft
				// 6 = topRight
				// 7 = bottomLeft
				// 8 = bottomRigh;
				switch( nResizeDir )
				{
				case 1:
				case 2:
#ifdef __NEWCUR
					g_Neuz.SetMouseCursor(CUR_RESIZE_VERT);
#else //__NEWCUR
					SetMouseCursor(CUR_RESIZE_VERT);
#endif //__NEWCUR
					break;
				case 3:
				case 4:
#ifdef __NEWCUR
					g_Neuz.SetMouseCursor(CUR_RESIZE_HORI);
#else //__NEWCUR
					SetMouseCursor(CUR_RESIZE_HORI);
#endif //__NEWCUR
					break;
				case 5:
				case 8:
#ifdef __NEWCUR
					g_Neuz.SetMouseCursor(CUR_RESIZE_HV1);
#else //__NEWCUR
					SetMouseCursor(CUR_RESIZE_HV1);
#endif //__NEWCUR
					break;
				case 6:
				case 7:
#ifdef __NEWCUR
					g_Neuz.SetMouseCursor(CUR_RESIZE_HV2);
#else //__NEWCUR
					SetMouseCursor(CUR_RESIZE_HV2);
#endif //__NEWCUR
					break;
						
				}
			}
			else
				CWndBase::OnSetCursor( pWndBase, nHitTest, message );
		}
	}
	else
		CWndBase::OnSetCursor( pWndBase, nHitTest, message );

	return TRUE;
}
//enum { IDD = IDD_PROP_APPLET_GENERAL };

#ifdef __AEGON_MINIMIZE_WINDOWS
void CWndNeuz::MinimizeToTexture(CString strTextureName)
{
	if (m_nMinimizedMode == __AEG_MINIMIZE_MODE_NONE)
	{
		CRect rectMin = GetWndRect();
		m_rectBeforeMinimized = rectMin;
		CPoint ptMove = CPoint(rectMin.right, rectMin.top);

		m_nMinimizedMode = __AEG_MINIMIZE_MODE_TEX;

		m_strTextureBeforeMinimized = m_strTexture;
		m_strTexture = strTextureName;

		m_bTileBeforeMinimized = m_bTile;
		m_bTile = false;

		AdjustWndBase();

		FitTextureSize();

		m_wndTitleBar.Replace();
		m_wndTitleBar.SetButtonVisible(FALSE);
		MakeVertexBuffer();

		if (m_ptLastMinimized != CPoint(0, 0))
		{
			Move(m_ptLastMinimized);
		}
		else 
		{
			ptMove.x -= GetWndRect().Width();
			Move(ptMove);
		}
	}
}
void CWndNeuz::OnMinimizeToggle()
{
	if (m_nMinimizedMode != __AEG_MINIMIZE_MODE_NONE)
	{
		CRect rect = GetWndRect();
		m_ptLastMinimized = rect.TopLeft();

		CPoint ptMove = m_rectBeforeMinimized.TopLeft();
		SetWndRect(m_rectBeforeMinimized);

		m_strTexture = m_strTextureBeforeMinimized;

		if (m_bTileBeforeMinimized && !m_bTile)
			m_bTile = true;

		m_nMinimizedMode = __AEG_MINIMIZE_MODE_NONE;
		AdjustWndBase();
		m_wndTitleBar.Replace();
		m_wndTitleBar.SetButtonVisible(TRUE);
		MakeVertexBuffer();

		rect = GetWndRect();
		
		ptMove = rect.TopLeft();

		int nTaskBarHeight = 0;
		if (g_WndMng.m_pWndTaskBar)
			nTaskBarHeight = g_WndMng.m_pWndTaskBar->GetWndRect().Height();
		if (rect.left < 0)
		{
			ptMove.x += abs(rect.left);
		}
		else if (rect.right > g_WndMng.m_rectClient.right) 
		{
			ptMove.x -= abs(rect.right - g_WndMng.m_rectClient.right);
		}

		if (rect.bottom > g_WndMng.m_rectClient.bottom - nTaskBarHeight)
		{
			int nDiff = rect.bottom - g_WndMng.m_rectClient.bottom;
			nDiff += nTaskBarHeight;
			ptMove.y -= nDiff;
		}
		else if (rect.top < g_WndMng.m_rectClient.top) 
		{
			ptMove.y += abs(rect.top - g_WndMng.m_rectClient.top);
		}
		Move(ptMove);
	}
	else
	{
		CRect rectMin = GetWndRect();
		m_rectBeforeMinimized = rectMin;
		CPoint ptMove = CPoint(rectMin.right, rectMin.top);

		CSize sizeTitleStr = CWndBase::m_Theme.m_pFontWndTitle->GetTextExtent(m_strTitle);
		int nWidth = sizeTitleStr.cx + (m_wndTitleBar.m_nButtonMax * m_wndTitleBar.m_nButtonSizeX) * 2 + 20;
		if (nWidth < 100)
			nWidth = 100;
		rectMin.right = rectMin.left + nWidth;
		rectMin.bottom = rectMin.top + 16*3;
		SetWndRect(rectMin);

		m_nMinimizedMode = __AEG_MINIMIZE_MODE_REG;

		AdjustWndBase();
		m_wndTitleBar.Replace();
		MakeVertexBuffer();

		ptMove.x -= GetWndRect().Width();
		Move(ptMove);		
	}
}
#endif

#ifdef __AEGON_THEME_SWITCHER
void CWndNeuz::OnThemeSelectionChanged()
{

	LPWNDAPPLET lpWndApplet = m_resMng.GetAt(GetWndId());
	if (lpWndApplet)
	{
		SetTitle(lpWndApplet->strTitle);
		m_bTile = (lpWndApplet->bTile != FALSE);
		m_strTexture = lpWndApplet->strTexture;
		m_d3dFormat = lpWndApplet->d3dFormat;

#ifdef __AEGON_THEME_SWITCHER
		map<int, __AEGON_THEME_UNIT>::iterator it = CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.find(g_Option.m_nSelectedThemeID);
		if (it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end())
		{
			if (it->second.nThemeType == 1 && m_bTile)
			{
				m_d3dFormat = it->second.d3dFormat;
			}
		}
#endif // __AEGON_THEME_SWITCHER

		for (int i = 0; i < m_wndArray.GetSize(); i++)
		{
			CWndBase* tmpBase = (CWndBase*)m_wndArray.GetAt(i);
			if (!tmpBase)
				continue;
			for (int z = 0; z < lpWndApplet->ptrCtrlArray.GetSize(); z++)
			{
				LPWNDCTRL tmpApplet = (LPWNDCTRL)lpWndApplet->ptrCtrlArray.GetAt(z);
				if (!tmpApplet)
					continue;
				if (tmpApplet->dwWndId == tmpBase->m_nIdWnd)
				{
					tmpBase->SetToolTip(tmpApplet->strToolTip);
					tmpBase->SetTitle(tmpApplet->strTitle);
					tmpBase->SetWndRect(tmpApplet->rect);
				}
			}
		}
	}

	if (IsWndStyle(WBS_CAPTION))
	{
		m_wndTitleBar.OnThemeSwitch();
	}
	map<int, __AEGON_THEME_UNIT>::iterator it = CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.find(g_Option.m_nSelectedThemeID);
	if (it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end())
	{
		m_d3dFormat = it->second.d3dFormat;
	}

	CWndBase::OnThemeSelectionChanged();

}
#endif // __AEGON_THEME_SWITCHER