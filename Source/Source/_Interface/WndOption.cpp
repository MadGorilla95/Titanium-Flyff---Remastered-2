// WndArcane.cpp: implementation of the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AppDefine.h"
#include "Resdata.h"
#include "defineSound.h"
#include "definetext.h"
#include "DPClient.h"

#define		MAX_SLIDER		10

extern	CDPClient	g_DPlay;

CWndTotalOption::CWndTotalOption() 
{ 
} 
CWndTotalOption::~CWndTotalOption() 
{ 
	g_Option.Save("neuz.ini");
} 

void CWndTotalOption::UpdateDataAll()
{
	//m_OptTabVideoSnd
}

void CWndTotalOption::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndTotalOption::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;
	
#if __VER >= 12 // __UPDATE_OPT
	m_OptTabVideoSnd.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_OPTEX_AV12);
#ifdef __TITANIUM_OPTIONS
	m_OptTitanium.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_OPTEX_TITANIUM);
	m_OptTitanium2.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_OPTEX_TITANIUM2);
#endif //__TITANIUM_OPTIONS
	m_OptTabGame.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_OPTEX_GAME12);
#ifdef __CHANGE_HOT_KEY
	m_OptCHK.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_OPTEX_HOTKEY);
#endif // __CHANGE_HOT_KEY
#else
	m_OptTabVideoSnd.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_OPTEX_AV );
	m_OptTabGame.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_OPTEX_GAME );
	m_OptTabEtc.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_OPTEX_ETC );
#ifdef __SFX_OPT
	m_OptTabSound.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_OPTEX_SOUND);
#endif
#endif
	WTCITEM tabTabItem;
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;

#if __VER >= 12 // __UPDATE_OPT
	tabTabItem.pszText = prj.GetText(TID_TOOLTIP_OPT_VIDEOAUDIO);
	tabTabItem.pWndBase = &m_OptTabVideoSnd;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );

#ifdef __TITANIUM_OPTIONS
	tabTabItem.pszText = _T("Rendering");
	tabTabItem.pWndBase = &m_OptTitanium;
	pWndTabCtrl->InsertItem(1, &tabTabItem);
	
	tabTabItem.pszText = _T("Custom");
	tabTabItem.pWndBase = &m_OptTitanium2;
	pWndTabCtrl->InsertItem(2, &tabTabItem);
#endif //__TITANIUM_OPTIONS

	tabTabItem.pszText = prj.GetText(TID_TOOLTIP_OPT_GAME);
	tabTabItem.pWndBase = &m_OptTabGame;
	pWndTabCtrl->InsertItem( 3, &tabTabItem );

#ifdef __CHANGE_HOT_KEY
	tabTabItem.pszText = _T("Hotkeys");
	tabTabItem.pWndBase = &m_OptCHK;
	pWndTabCtrl->InsertItem(4, &tabTabItem);
#endif // __CHANGE_HOT_KEY

#else
	tabTabItem.pszText = prj.GetText(TID_TOOLTIP_OPT_VIDEOAUDIO);
	//tabTabItem.pszText = prj.GetText(TID_APP_OPTION_VIDEO);
	tabTabItem.pWndBase = &m_OptTabVideoSnd;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );

	tabTabItem.pszText = prj.GetText(TID_TOOLTIP_OPT_ETC);
	//tabTabItem.pszText = prj.GetText(TID_APP_OPTION_ETC);
	tabTabItem.pWndBase = &m_OptTabEtc;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );
	
	tabTabItem.pszText = prj.GetText(TID_TOOLTIP_OPT_GAME);
	//tabTabItem.pszText = prj.GetText(TID_APP_OPTION_GAME);
	tabTabItem.pWndBase = &m_OptTabGame;
	pWndTabCtrl->InsertItem( 2, &tabTabItem );
#ifdef __SFX_OPT
	tabTabItem.pszText = prj.GetText(TID_TOOLTIP_OPT_SOUND);
	tabTabItem.pWndBase = &m_OptTabSound;
	pWndTabCtrl->InsertItem( 3, &tabTabItem );
#endif
#endif

	MoveParentCenter();	
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndTotalOption::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndTotalOption::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndTotalOption::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndTotalOption::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndTotalOption::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndTotalOption::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case 10000:
			{
				CWndGuideSystem* pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
				
				if( pWndGuide )
				{
					pWndGuide->m_dwGuideLevel = *(g_Option.m_pGuide);
					
					if( pWndGuide->m_dwGuideLevel == 0 )
					{
						if( g_pPlayer )
						{
							if(pWndGuide->m_bVisible != false)
							{
								pWndGuide->m_bVisible = true;
								pWndGuide->ChangeModel( g_pPlayer->GetJob() );
								pWndGuide->SetAni( g_pPlayer->GetJob(), CWndGuideSystem::ANI_IDLE );
							}
						}
					}
					else
					{
						if( g_pPlayer )
						{
							pWndGuide->SetAni( g_pPlayer->GetJob(), CWndGuideSystem::ANI_BYTE );
						}
					}
				}
#ifdef __CHANGE_HOT_KEY
				CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
				m_OptCHK.Save();
#endif // __CHANGE_HOT_KEY
			}
			break;
	}
		
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

CWndOption::CWndOption() 
{ 
} 
CWndOption::~CWndOption() 
{ 
} 
void CWndOption::OnDraw( C2DRender* p2DRender )
{ 
} 
void CWndOption::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndButton* pWndAlpha = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
	pWndAlpha->SetCheck( g_Option.m_nWindowAlpha == 255 ? FALSE : TRUE);
	CWndButton* pWndHelp = (CWndButton*)GetDlgItem( WIDC_CHECK2 );
	pWndHelp->SetCheck( g_Option.m_nInstantHelp );
	
	CWndButton* pWndButton[ 4 ];

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nSlangWord ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_RADIO5 );
	pWndButton[ 3 ] = (CWndButton*)GetDlgItem( WIDC_RADIO6 );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nChatCommand ]->SetCheck( TRUE );

	// 인터페이스 방식(구버전이 디폴트)
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO7 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO8 );
#ifdef __Y_INTERFACE_VER3
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_RADIO12 );
#endif //__Y_INTERFACE_VER3
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nInterface ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO9 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO10 );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_RADIO11 );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ *g_Option.m_pGuide ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK4 );
	if(pWndButton[ 0 ])
	pWndButton[ 0 ]->SetCheck( g_Option.m_bCameraLock );

	// 비행시 롤링 효과.
	CWndButton* pWndRoll = (CWndButton*)GetDlgItem( WIDC_CHECK3 );		
	pWndRoll->SetCheck( g_Option.m_bRollEffect );

#ifdef __NEW_NIGHT
	CWndButton* pWndDayNight = (CWndButton*)GetDlgItem(WIDC_CHECKDAYNIGHT);
	pWndDayNight->SetCheck(g_Option.m_bNewNight);
#endif // __NEW_NIGHT

#ifdef __SYS_GROUND_ITEM_TEXT
	CWndButton* pWndGroundItem = (CWndButton*)GetDlgItem(WIDC_CHECK_GROUNDITEM);
	pWndGroundItem->SetCheck(g_Option.groundItemText);
#endif // __SYS_GROUND_ITEM_TEXT

#ifdef __ALIASING
	CWndButton* pWndAliasing = (CWndButton*)GetDlgItem(WIDC_CHECK_ALIASING);
	pWndAliasing->SetCheck(g_Option.m_nAliasing);
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK_ALIASING);
	if (pWndButton[0])
		pWndButton[0]->SetCheck(g_Option.m_nAliasing);
#endif // __ALIASING

#ifdef __ANISOTROPIC
	CWndButton* pWndAnisotropic = (CWndButton*)GetDlgItem(WIDC_CHECK_ANISOTROPIC);
	pWndAliasing->SetCheck(g_Option.m_nAnisotropic);
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK_ANISOTROPIC);
	if (pWndButton[0])
		pWndButton[0]->SetCheck(g_Option.m_nAnisotropic);
#endif // __ANISOTROPIC

#ifdef __HIDE_MONSTER_LEVEL
	CWndButton* pWndMonsterLevel = (CWndButton*)GetDlgItem(WIDC_CHECK_MOBLVL);
	pWndMonsterLevel->SetCheck(g_Option.m_nMonsterLevel);
#endif // __HIDE_MONSTER_LEVEL

#ifdef __ALPHA_CHAT
	CWndButton* pWndAlphaChat = (CWndButton*)GetDlgItem(WIDC_CHECK_ALPHACHAT);
	pWndAlphaChat->SetCheck(g_Option.alphaChat);
#endif //__ALPHA_CHAT

	#if __VER >= 11 // __ADD_ZOOMOPT
	CWndButton* pWndZoom = (CWndButton*)GetDlgItem( WIDC_CHECK5 );		
	pWndZoom->SetCheck(!g_Option.m_bZoomLimit);
	#endif
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 

BOOL CWndOption::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_OPTION_ETC, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndOption::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndOption::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndOption::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndOption::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndOption::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWndButton* pWndAlpha = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
	CWndButton* pWndHelp = (CWndButton*)GetDlgItem( WIDC_CHECK2 );
	CWndButton* pWndRoll = (CWndButton*)GetDlgItem( WIDC_CHECK3 );
	CWndButton* pWndCamearaLock = (CWndButton*)GetDlgItem( WIDC_CHECK4 );

#ifdef __NEW_NIGHT
	CWndButton* pWndDayNight = (CWndButton*)GetDlgItem(WIDC_CHECKDAYNIGHT);
#endif //__NEW_NIGHT

#ifdef __SYS_GROUND_ITEM_TEXT
	CWndButton* pWndGroundItem = (CWndButton*)GetDlgItem(WIDC_CHECK_GROUNDITEM);
#endif //__SYS_GROUND_ITEM_TEXT

#ifdef __ALIASING
	CWndButton* pWndAliasing = (CWndButton*)GetDlgItem(WIDC_CHECK_ALIASING);
#endif // __ALIASING

#ifdef __ANISOTROPIC
	CWndButton* pWndAnisotropic = (CWndButton*)GetDlgItem(WIDC_CHECK_ANISOTROPIC);
#endif // __ANISOTROPIC

#ifdef __HIDE_MONSTER_LEVEL
	CWndButton* pWndMonsterLevel = (CWndButton*)GetDlgItem(WIDC_CHECK_MOBLVL);
#endif // __HIDE_MONSTER_LEVEL

#ifdef __ALPHA_CHAT
	CWndButton* pWndAlphaChat = (CWndButton*)GetDlgItem(WIDC_CHECK_ALPHACHAT);
#endif //__ALPHA_CHAT

#if __VER >= 11 // __ADD_ZOOMOPT
	CWndButton* pWndZoomLimit   = (CWndButton*)GetDlgItem( WIDC_CHECK5 );
#endif

	switch( nID )
	{
	case WIDC_CHECK1: // 윈도 반투명 옵션 
		if( pWndAlpha->GetCheck() )
			CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 128;
		else
			CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 255;
		break;
	case WIDC_CHECK2: // 초보자 도움말 옵션 
		if( pWndHelp->GetCheck() )
			g_Option.m_nInstantHelp = TRUE;
		else
			g_Option.m_nInstantHelp = FALSE;
		break;
	case WIDC_CHECK3:	// 비행시 롤링 효과.
		if( pWndRoll->GetCheck() )
			g_Option.m_bRollEffect = TRUE;
		else
			g_Option.m_bRollEffect = FALSE;
		break;
	case WIDC_CHECK4:
		{
			if( pWndCamearaLock->GetCheck() )
				g_Option.m_bCameraLock = TRUE;
			else
				g_Option.m_bCameraLock = FALSE;			
		}
		break;

#ifdef __NEW_NIGHT
		case WIDC_CHECKDAYNIGHT:
		{
			if (pWndDayNight->GetCheck())
				g_Option.m_bNewNight = TRUE;
			else
				g_Option.m_bNewNight = FALSE;
		}
		break;
#endif // __NEW_NIGHT

#ifdef __SYS_GROUND_ITEM_TEXT
		case WIDC_CHECK_GROUNDITEM:
		{
			if (pWndGroundItem->GetCheck())
				g_Option.groundItemText = TRUE;
			else
				g_Option.groundItemText = FALSE;
		}
		break;
#endif // __SYS_GROUND_ITEM_TEXT

#ifdef __ALIASING
		case WIDC_CHECK_ALIASING:
		{
			if (pWndAliasing->GetCheck())
				g_Option.m_nAliasing = TRUE;
			else
				g_Option.m_nAliasing = FALSE;
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GAME_RESETTING), MB_OK, this);
		}
		break;
#endif // __ALIASING

#ifdef __ANISOTROPIC
		case WIDC_CHECK_ANISOTROPIC:
		{
			if (pWndAliasing->GetCheck())
				g_Option.m_nAnisotropic = TRUE;
			else
				g_Option.m_nAnisotropic = FALSE;
			//g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GAME_RESETTING), MB_OK, this );
		}
		break;
#endif // __ANISOTROPIC

#ifdef __HIDE_MONSTER_LEVEL
		case WIDC_CHECK_MOBLVL:
		{
			if (pWndMonsterLevel->GetCheck())
				g_Option.m_nMonsterLevel = TRUE;
			else
				g_Option.m_nMonsterLevel = FALSE;
		}
		break;
#endif // __HIDE_MONSTER_LEVEL

#ifdef __ALPHA_CHAT
		case WIDC_CHECK_ALPHACHAT:
		{
			if (pWndAlphaChat->GetCheck())
				g_Option.alphaChat = true;
			else
				g_Option.alphaChat = false;
		}
		break;
#endif // __ALPHA_CHAT

#if __VER >= 11 // __ADD_ZOOMOPT
	case WIDC_CHECK5:
		{
			if( pWndZoomLimit->GetCheck() )
				g_Option.m_bZoomLimit = FALSE;
			else
				g_Option.m_bZoomLimit = TRUE;			
		}	
		break;
#endif

	case WIDC_RADIO1:
		g_Option.m_nSlangWord = 0;
		break;
	case WIDC_RADIO2:
		g_Option.m_nSlangWord = 1;
		break;
	case WIDC_RADIO3:
		g_Option.m_nChatCommand = 0;
		break;
	case WIDC_RADIO4:
		g_Option.m_nChatCommand = 1;
		break;
	case WIDC_RADIO5:
		g_Option.m_nChatCommand = 2;
		break;
	case WIDC_RADIO6:
		g_Option.m_nChatCommand = 3;
		break;
	case WIDC_RADIO7:	// 휠로 카메라 / 우클릭 스킬의 고전방식
		g_Option.m_nInterface = 0;
		break;
	case WIDC_RADIO8:	// 우클릭 카메라 / 좌클릭+X 스킬의 신방식.
		g_Option.m_nInterface = 1;
		break;
#ifdef __Y_INTERFACE_VER3
	case WIDC_RADIO12:
		g_Option.m_nInterface = 2;
		break;
#endif //__Y_INTERFACE_VER3
	case WIDC_RADIO9:
		*g_Option.m_pGuide = 0;
		break;
	case WIDC_RADIO10:
		*g_Option.m_pGuide = 1;
		break;
	case WIDC_RADIO11:
		*g_Option.m_pGuide = 2;
		break;
	case 10000:
		{
			CWndGuideSystem* pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
			
			if( pWndGuide )
			{
				pWndGuide->m_dwGuideLevel = *(g_Option.m_pGuide);
				
				if( pWndGuide->m_dwGuideLevel == 0 )
				{
					if( g_pPlayer )
					{
						pWndGuide->m_bVisible = TRUE;
						pWndGuide->ChangeModel( g_pPlayer->GetJob() );
						pWndGuide->SetAni( g_pPlayer->GetJob(), CWndGuideSystem::ANI_IDLE );
					}
				}
				else
				{
					if(g_pPlayer)
					{
						pWndGuide->SetAni( g_pPlayer->GetJob(), CWndGuideSystem::ANI_BYTE );
					}
				}
			}
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

CWndOptSound::CWndOptSound() 
{ 
	m_bLButtonClick = FALSE;
	m_bLButtonClick2 = FALSE;
	
	m_nStep[0] = 0;
	m_nStep[1] = 0;
} 
CWndOptSound::~CWndOptSound() 
{ 
} 
void CWndOptSound::OnDraw( C2DRender* p2DRender ) 
{ 
	CPoint pt;

	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	CPoint pt1 = lpWndCtrl->rect.TopLeft();
	CPoint pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	p2DRender->RenderFillRect( lpWndCtrl->rect, 0);	
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[0], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 10 ).x;
#ifdef __AEGON_QOL_STUFF
	pt.y += 1;
#endif
	m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );
	
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
	pt1 = lpWndCtrl->rect.TopLeft();
	pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[1], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 10 ).x;
#ifdef __AEGON_QOL_STUFF
	pt.y += 1;
#endif
	m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );

} 
void CWndOptSound::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	CWndButton* pWndButton[ 3 ];

	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_VOICEON );
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_VOICEOFF );
	if( ::GetLanguage() == LANG_JAP )
	{
		
		pWndButton[ 1 ]->SetGroup( TRUE );
		pWndButton[ g_Option.m_bVoice ]->SetCheck( TRUE );
	}
	else
	{
		CWndStatic* pWndVoice = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
		pWndVoice->SetVisible(FALSE);
		pWndButton[ 1 ]->SetVisible(FALSE);
		pWndButton[ 0 ]->SetVisible(FALSE);
	}
#ifdef __AEGON_THEME_SWITCHER
	CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchLoadTexture(&m_Texture, g_Neuz.GetDevice(), "WndVolumeBar.tga", COLOR_ITEM_BG, TRUE);
	CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchLoadTexture(&m_TexturePt, g_Neuz.GetDevice(), "ButtSpin.tga", 0xffffffff, TRUE);
#else // __AEGON_THEME_SWITCHER
	m_Texture.LoadTexture( g_Neuz.GetDevice(), MakePath( DIR_THEME, "WndVolumeBar.tga" ), COLOR_ITEM_BG, TRUE );
	m_TexturePt.LoadTexture( g_Neuz.GetDevice(), MakePath( DIR_THEME, "ButtSpin.tga" ), 0xffffffff, TRUE );		
#endif // __AEGON_THEME_SWITCHER
#if __VER < 12 // __UPDATE_OPT
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_MSHIGH );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_MSMID );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_MSLOW );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_MouseSpeed ]->SetCheck( TRUE );
#endif
	m_nStep[0] = (int)( g_Option.m_fEffectVolume * 10 );
	m_nStep[1] = (int)( g_Option.m_fBGMVolume * 10 );

} 

BOOL CWndOptSound::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_OPTEX_SOUND, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndOptSound::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndOptSound::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndOptSound::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
#ifdef __fmod
	SoundSystemClass::playSound(SND_NONE);
#else
	PLAYSND(SND_NONE);
#endif
	ReleaseCapture();
	m_bLButtonClick = FALSE;
	m_bLButtonClick2 = FALSE;
} 
void CWndOptSound::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	SetCapture();

	int nResult = 0;
	nResult = GetSliderStep( WIDC_CUSTOM1, m_nStep[0], point );	
	if( nResult == -1 )
	{
		m_bLButtonClick = TRUE;
		return;
	}
	else
	if( nResult == 1 )
	{
		g_Option.m_fEffectVolume	= m_nStep[0]*0.1f;
#ifdef __fmod
		SoundSystemClass::changeVolume(false);
#else
		g_SoundMng.m_nSoundVolume	= (int)( -(1.0f-g_Option.m_fEffectVolume)*5000 );
#endif
	}
	/////////////////////////////////////////
	nResult = GetSliderStep( WIDC_CUSTOM2, m_nStep[1], point );	
	if( nResult == -1 )
	{
		m_bLButtonClick2 = TRUE;
		return;
	}
	else
	if( nResult == 1 )
	{
		g_Option.m_fBGMVolume = m_nStep[1]*0.1f;
#ifdef __fmod
		SoundSystemClass::changeVolume(true);
#else
		SetVolume(g_Option.m_fBGMVolume);	
#endif
	}

} 

void CWndOptSound::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bLButtonClick )
	{
		GetRangeSlider( WIDC_CUSTOM1, m_nStep[0], point );

		g_Option.m_fEffectVolume	= m_nStep[0]*0.1f;
#ifdef __fmod
		SoundSystemClass::changeVolume(false);
#else
		g_SoundMng.m_nSoundVolume	= (int)( -(1.0f-g_Option.m_fEffectVolume)*5000 );
#endif
	}

	if( m_bLButtonClick2 )
	{
		GetRangeSlider( WIDC_CUSTOM2, m_nStep[1], point );

		g_Option.m_fBGMVolume = m_nStep[1]*0.1f;
#ifdef __fmod
		SoundSystemClass::changeVolume(true);
#else
		SetVolume(g_Option.m_fBGMVolume);
#endif
	}
}
BOOL CWndOptSound::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch(nID)
	{
		case WIDC_VOICEON:
			g_Option.m_bVoice = 1;
			break;
		case WIDC_VOICEOFF:
			g_Option.m_bVoice = 0;
			break;
#if __VER < 12 // __UPDATE_OPT
		case WIDC_MSLOW:
			g_Option.m_MouseSpeed = 2;
			break;
		case WIDC_MSMID:
			g_Option.m_MouseSpeed = 1;
			break;
		case WIDC_MSHIGH:
			g_Option.m_MouseSpeed = 0;
			break;
#endif
	};
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

CPoint CWndOptSound::GetStepPos(int nStep, int nWidth, int nDivision)
{
	return nStep * (nWidth / (nDivision+1));
}

int CWndOptSound::GetSliderStep(DWORD dwWndId, int &nStep, CPoint point )
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl( dwWndId );	
	CRect crect = lpWndCtrl->rect;
	
	crect.left += GetStepPos( nStep, lpWndCtrl->rect.right - lpWndCtrl->rect.left, MAX_SLIDER ).x;
	crect.right = crect.left+16;
	
	if( PtInRect( crect, point ) )
	{
		return -1;
	}
	
	crect = lpWndCtrl->rect;
	if( PtInRect( crect, point ) )
	{
		int nWidth = lpWndCtrl->rect.right - lpWndCtrl->rect.left;
		
		if( point.x > lpWndCtrl->rect.left+GetStepPos( nStep, nWidth, MAX_SLIDER ).x )
			nStep += 1;//nWidth / 10;
		else
			nStep -= 1;//nWidth / 10;

		return 1;
	}

	return 0;
}

void CWndOptSound::GetRangeSlider(DWORD dwWndId, int &nStep, CPoint point)
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl( dwWndId );	
	int nWidth = lpWndCtrl->rect.right - lpWndCtrl->rect.left;
	
	if( point.x > lpWndCtrl->rect.left+GetStepPos( nStep, nWidth, MAX_SLIDER ).x )
		nStep += 1;//nWidth / 10;
	if( point.x < lpWndCtrl->rect.left+GetStepPos( nStep, nWidth, MAX_SLIDER ).x )
		nStep -= 1;//nWidth / 10;
	
	if( nStep < 0 )
		nStep = 0;
	
	if( nStep > MAX_SLIDER )
		nStep = MAX_SLIDER;
}

HRESULT CWndOptSound::InvalidateDeviceObjects()
{
#ifdef __YDEBUG
	m_Texture.Invalidate();
	m_TexturePt.Invalidate();
#endif //__YDEBUG
	return CWndNeuz::InvalidateDeviceObjects();
}
HRESULT CWndOptSound::RestoreDeviceObjects()
{
#ifdef __YDEBUG
	m_Texture.SetInvalidate(m_pApp->m_pd3dDevice);
	m_TexturePt.SetInvalidate(m_pApp->m_pd3dDevice);
#endif //__YDEBUG
	return CWndNeuz::RestoreDeviceObjects();
}

#ifdef __AEGON_THEME_SWITCHER
void CWndOptSound::OnThemeSelectionChanged()
{
	CWndNeuz::OnThemeSelectionChanged();

	CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchLoadTexture(&m_Texture, g_Neuz.GetDevice(), "WndVolumeBar.tga", COLOR_ITEM_BG, TRUE);
	CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchLoadTexture(&m_TexturePt, g_Neuz.GetDevice(), "ButtSpin.tga", 0xffffffff, TRUE);
}
#endif // __AEGON_THEME_SWITCHER


CWndOptVideo::CWndOptVideo() 
{ 
	m_bLButtonClick = FALSE;
	m_bLButtonClick2 = FALSE;
	
	m_nStep[0] = 0;
	m_nStep[1] = 0;

#if __VER >= 8 //__Y_GAMMA_CONTROL_8
	m_nStep[2] = 0;
	m_nStep[3] = 0;
	m_nStep[4] = 0;

	m_bLButtonClick3 = FALSE;
	m_bLButtonClick4 = FALSE;
	m_bLButtonClick5 = FALSE;

	memset( m_nBrightTable, 0, sizeof(int) * 11 );
#endif //__Y_GAMMA_CONTROL_8
} 
CWndOptVideo::~CWndOptVideo() 
{ 
	//g_Option.Save( "neuz.ini" );	
} 
HRESULT CWndOptVideo::InvalidateDeviceObjects()
{
#ifdef __YDEBUG
	m_Texture.Invalidate();
	m_TexturePt.Invalidate();
#endif //__YDEBUG
	return CWndNeuz::InvalidateDeviceObjects();
}
HRESULT CWndOptVideo::RestoreDeviceObjects()
{
#ifdef __YDEBUG
	m_Texture.SetInvalidate(m_pApp->m_pd3dDevice);
	m_TexturePt.SetInvalidate(m_pApp->m_pd3dDevice);
#endif //__YDEBUG
	return CWndNeuz::RestoreDeviceObjects();
}

CPoint CWndOptVideo::GetStepPos(int nStep, int nWidth, int nDivision)
{
	if(nDivision == 1 && nStep)
		return nStep * nWidth - 16;
	else
		return nStep * (nWidth / (nDivision+1));
}

void CWndOptVideo::OnDraw( C2DRender* p2DRender ) 
{ 
	CPoint pt;
#ifdef __SFX_OPT
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
	CPoint pt1 = lpWndCtrl->rect.TopLeft();
	CPoint pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	p2DRender->RenderFillRect( lpWndCtrl->rect, 0);	
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[0], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 5 ).x;
#ifdef __AEGON_QOL_STUFF
	pt.y += 1;
#endif
	m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );
	
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
	pt1 = lpWndCtrl->rect.TopLeft();
	pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[1], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 1 ).x;
#ifdef __AEGON_QOL_STUFF
	pt.y += 1;
#endif
	m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );
#else
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
	CPoint pt1 = lpWndCtrl->rect.TopLeft();
	CPoint pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	p2DRender->RenderFillRect( lpWndCtrl->rect, 0);	
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[0], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 10 ).x;
	m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );
	
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
	pt1 = lpWndCtrl->rect.TopLeft();
	pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[1], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 10 ).x;
	m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );
#endif
#if __VER >= 8 //__Y_GAMMA_CONTROL_8
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	pt1 = lpWndCtrl->rect.TopLeft();
	pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[2], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 10 ).x;
#ifdef __AEGON_QOL_STUFF
	pt.y += 1;
#endif
	m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );

	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
	pt1 = lpWndCtrl->rect.TopLeft();
	pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[3], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 10 ).x;
#ifdef __AEGON_QOL_STUFF
	pt.y += 1;
#endif
	m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );

	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
	pt1 = lpWndCtrl->rect.TopLeft();
	pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[4], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 10 ).x;
#ifdef __AEGON_QOL_STUFF
	pt.y += 1;
#endif
	m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );
#endif //__Y_GAMMA_CONTROL_8


} 
void CWndOptVideo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	CWndButton* pWndButton[ 3 ];

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_LOD_HIGH );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_LOD_MID );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_LOD_LOW );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nObjectDetail ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_DISTANT_HIGH );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_DISTANT_MID );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_DISTANT_LOW );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nObjectDistant ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_TERRAIN_LOD_HIGH );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_TERRAIN_LOD_MID );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_TERRAIN_LOD_LOW );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nViewArea ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_SHADOW_HIGH );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_SHADOW_MID );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_SHADOW_LOW );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nShadow ]->SetCheck( TRUE );
	if( g_Neuz.m_d3dCaps.MaxSimultaneousTextures <= 2 ) 
	{
		pWndButton[ 0 ]->EnableWindow( FALSE );
		pWndButton[ 1 ]->EnableWindow( FALSE );
		pWndButton[ 2 ]->EnableWindow( FALSE );
	}
#ifdef __SFX_OPT
	if( ::GetLanguage() == LANG_KOR || ::GetLanguage() == LANG_JAP)
	{
		pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK4 );
		if(pWndButton[ 0 ])
			pWndButton[ 0 ]->SetVisible(FALSE);
	}
#else
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_VOICEON );
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_VOICEOFF );
	pWndButton[ 1 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_bVoice ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_GLOWON );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_GLOWOFF );
	pWndButton[ 0 ]->SetGroup( TRUE );
	if( g_Neuz.m_d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1,1) )
	{
		pWndButton[ g_Option.m_nBloom^1 ]->SetCheck( TRUE );
	} else
	{
		pWndButton[0]->EnableWindow( FALSE );
		pWndButton[1]->EnableWindow( FALSE );
	}

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK5 );
	pWndButton[ 0 ]->SetCheck(!g_Option.m_bSFXRenderOff);

#endif
#ifdef __YENV
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_SEPCBUMPCHECK );
	if (pWndButton[0])
	{
		if (g_bUsableVS && g_dwUsablePS_Level == 2)
		{
			pWndButton[0]->EnableWindow(TRUE);
		}
		else
		{
			pWndButton[0]->EnableWindow(FALSE);
		}

		if (g_Option.m_bSpecBump)
		{
			pWndButton[0]->SetCheck(TRUE);
		}
		else
		{
			pWndButton[0]->SetCheck(FALSE);
		}
	}
	if( g_Neuz.m_bFakeFullMode )
	{
		pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK4 );
		if(pWndButton[ 0 ])
			pWndButton[ 0 ]->SetCheck(!g_Option.m_bStartFullScreen);
	}
#endif //__YENV	
	
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK6 );
	pWndButton[ 0 ]->SetCheck(g_Option.m_nPlayerName);
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK7 );
	pWndButton[ 0 ]->SetCheck(g_Option.m_nOtherPlayerName);
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK8 );
	pWndButton[ 0 ]->SetCheck(g_Option.m_nMonName);	
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK2 );
	pWndButton[ 0 ]->SetCheck(g_Option.m_bDamageRender);
#if __VER >= 12 // __UPDATE_OPT
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK1);
	pWndButton[ 0 ]->SetCheck(!g_Option.m_bZoomLimit);
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_VIEWMASK );
	pWndButton[ 0 ]->SetCheck(g_Option.m_bViewMask);
	if(g_pPlayer && g_pPlayer->IsOptionRenderMask() != g_Option.m_bViewMask)
		g_DPlay.SendOptionEnableRenderMask( g_Option.m_bViewMask );
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK5 );
	if(pWndButton[ 0 ])
	pWndButton[ 0 ]->SetCheck( g_Option.m_bCameraLock );

	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK4);
	if (pWndButton[0])
		pWndButton[0]->SetCheck(!g_Option.m_bStartFullScreen);


#ifdef __NEW_NIGHT
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECKDAYNIGHT);
	if (pWndButton[0])
	{
		pWndButton[0]->SetToolTip(_T("Enable or disable night mode."));
		pWndButton[0]->SetCheck(g_Option.m_bNewNight);
	}
#endif // __NEW_NIGHT


#ifdef __BACKGROUNDRENDER
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK_LOW_RENDER);
	if (pWndButton[0])
	{
		pWndButton[0]->SetToolTip(_T("Reduce rendering performance when you minimize your client."));
		pWndButton[0]->SetCheck(g_Option.inactiveRender);
	}
#endif // __BACKGROUNDRENDER

#ifdef __ALIASING
	CWndButton* pWndAliasing = (CWndButton*)GetDlgItem(WIDC_CHECK_ALIASING);
	pWndAliasing->SetCheck(g_Option.m_nAliasing);
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK_ALIASING);
	if (pWndButton[0])
	{
		pWndButton[0]->SetToolTip(_T("Reduce Aliasing."));
		pWndButton[0]->SetCheck(g_Option.m_nAliasing);
	}
#endif // __ALIASING

#ifdef __ANISOTROPIC
	CWndButton* pWndAnisotropic = (CWndButton*)GetDlgItem(WIDC_CHECK_ANISOTROPIC);
	pWndAnisotropic->SetCheck(g_Option.m_nAnisotropic);
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK_ANISOTROPIC);
	if (pWndButton[0])
	{
		pWndButton[0]->SetToolTip(_T("Toggle Anisotropic Filtering."));
		pWndButton[0]->SetCheck(g_Option.m_nAnisotropic);
	}
#endif // __ANISOTROPIC

#else
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_VISIBLEKEEPTIME );
	pWndButton[ 0 ]->SetCheck(g_Option.m_bVisibleBuffTimeRender);	
#endif
#ifdef __AEGON_THEME_SWITCHER
	CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchLoadTexture(&m_Texture, g_Neuz.GetDevice(), "WndVolumeBar.tga", COLOR_ITEM_BG, TRUE);
	CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchLoadTexture(&m_TexturePt, g_Neuz.GetDevice(), "ButtSpin.tga", 0xffffffff, TRUE);
#else // __AEGON_THEME_SWITCHER
	m_Texture.LoadTexture( g_Neuz.GetDevice(), MakePath( DIR_THEME, "WndVolumeBar.tga" ), COLOR_ITEM_BG, TRUE );
	m_TexturePt.LoadTexture( g_Neuz.GetDevice(), MakePath( DIR_THEME, "ButtSpin.tga" ), 0xffffffff, TRUE );	
#endif // __AEGON_THEME_SWITCHER
#ifdef __SFX_OPT
	m_nStep[0] = g_Option.m_nSfxLevel;
	m_nStep[1] = !g_Option.m_bSFXRenderOff;
#else
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_MOUSE_HIGH );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_MOUSE_MID );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_MOUSE_LOW );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_MouseSpeed ]->SetCheck( TRUE );

	m_nStep[0] = g_Option.m_fEffectVolume * 10;
	m_nStep[1] = g_Option.m_fBGMVolume * 10;
#endif

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK3 );
	if(pWndButton[ 0 ]) pWndButton[ 0 ]->SetCheck(g_Option.m_nWeatherEffect);

#if __VER >= 8 //__Y_GAMMA_CONTROL_8
	m_nBrightTable[0] = -40;
	m_nBrightTable[1] = -30;
	m_nBrightTable[2] = -20;
	m_nBrightTable[3] = -10;
	m_nBrightTable[4] = 0;
	m_nBrightTable[5] = 10;
	m_nBrightTable[6] = 20;
	m_nBrightTable[7] = 30;
	m_nBrightTable[8] = 40;
	m_nBrightTable[9] = 50;
	m_nBrightTable[10] = 60;

	for( int i=0; i<11; i++ )
	{
		if( m_nBrightTable[i] == g_Option.m_nOverBright )
		{
			m_nStep[3] = i;
			break;
		}
	}

	m_nStep[2] = (int)( g_Option.m_fGamma * 5 );
	m_nStep[4] = (int)( g_Option.m_fContrast * 5 );
#endif //__Y_GAMMA_CONTROL_8
} 

BOOL CWndOptVideo::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_OPTION_VIDEO, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndOptVideo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndOptVideo::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndOptVideo::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
#ifdef __fmod
	SoundSystemClass::playSound(SND_NONE);
#else
	PLAYSND(SND_NONE);
#endif
	ReleaseCapture();
	m_bLButtonClick = FALSE;
	m_bLButtonClick2 = FALSE;
#if __VER >= 8 //__Y_GAMMA_CONTROL_8
	m_bLButtonClick3 = FALSE;
	m_bLButtonClick4 = FALSE;
	m_bLButtonClick5 = FALSE;
#endif //__Y_GAMMA_CONTROL_8
} 
void CWndOptVideo::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 

	SetCapture();

	int nResult = 0;
#ifdef __SFX_OPT
	nResult = GetSliderStep( WIDC_CUSTOM2, m_nStep[0], point, 5);	
	if( nResult == -1 )
	{
		m_bLButtonClick = TRUE;
		return;
	}
	else
	if( nResult == 1 )
	{
		g_Option.m_nSfxLevel = m_nStep[0];
	}
	/////////////////////////////////////////
	nResult = GetSliderStep( WIDC_CUSTOM3, m_nStep[1], point, 1);	
	if( nResult == -1 )
	{
		m_bLButtonClick2 = TRUE;
		return;
	}
	else
	if( nResult == 1 )
	{
		g_Option.m_bSFXRenderOff = !m_nStep[1];
	}
#else
	nResult = GetSliderStep( WIDC_CUSTOM2, m_nStep[0], point );	
	if( nResult == -1 )
	{
		m_bLButtonClick = TRUE;
		return;
	}
	else
	if( nResult == 1 )
	{
		g_Option.m_fEffectVolume = m_nStep[0]*0.1f;
		g_SoundMng.m_nSoundVolume = -(1.0f-g_Option.m_fEffectVolume)*5000;		
	}
	/////////////////////////////////////////
	nResult = GetSliderStep( WIDC_CUSTOM3, m_nStep[1], point );	
	if( nResult == -1 )
	{
		m_bLButtonClick2 = TRUE;
		return;
	}
	else
	if( nResult == 1 )
	{
		g_Option.m_fBGMVolume = m_nStep[1]*0.1f;
		SetVolume(g_Option.m_fBGMVolume);		
	}
#endif
#if __VER >= 8 //__Y_GAMMA_CONTROL_8
	/////////////////////////////////////////
	nResult = GetSliderStep( WIDC_CUSTOM1, m_nStep[2], point );	
#ifdef __DISABLE_GAMMA_WND
	if( g_Neuz.m_bStartFullscreen )
	{
#endif
		if( nResult == -1 )
		{
			m_bLButtonClick3 = TRUE;
			return;
		}
		else
		if( nResult == 1 )
		{
			g_Option.m_fGamma = (FLOAT)m_nStep[2] / (FLOAT)5.0f;
			g_Neuz.Drv_SetGamma( g_Neuz.GetSafeHwnd(), g_Option.m_fGamma, g_Option.m_nOverBright, g_Option.m_fContrast );
		}
		/////////////////////////////////////////
		nResult = GetSliderStep( WIDC_CUSTOM4, m_nStep[3], point );	
		if( nResult == -1 )
		{
			m_bLButtonClick4 = TRUE;
			return;
		}
		else
		if( nResult == 1 )
		{
			g_Option.m_nOverBright = m_nBrightTable[m_nStep[3]];
			g_Neuz.Drv_SetGamma( g_Neuz.GetSafeHwnd(), g_Option.m_fGamma, g_Option.m_nOverBright, g_Option.m_fContrast );
		}
		/////////////////////////////////////////
		nResult = GetSliderStep( WIDC_CUSTOM5, m_nStep[4], point );	
		if( nResult == -1 )
		{
			m_bLButtonClick5 = TRUE;
			return;
		}
		else
		if( nResult == 1 )
		{
			g_Option.m_fContrast = (FLOAT)m_nStep[4] / (FLOAT)5.0f;
			g_Neuz.Drv_SetGamma( g_Neuz.GetSafeHwnd(), g_Option.m_fGamma, g_Option.m_nOverBright, g_Option.m_fContrast );
		}
#ifdef __DISABLE_GAMMA_WND
	}
#endif
#endif //__Y_GAMMA_CONTROL_8
} 

void CWndOptVideo::OnMouseMove(UINT nFlags, CPoint point)
{
#ifdef __SFX_OPT
	if( m_bLButtonClick )
	{
		GetRangeSlider( WIDC_CUSTOM2, m_nStep[0], point, 5);
		g_Option.m_nSfxLevel = m_nStep[0];
	}

	if( m_bLButtonClick2 )
	{
		GetRangeSlider( WIDC_CUSTOM3, m_nStep[1], point,1);
		g_Option.m_bSFXRenderOff = !m_nStep[1];
	}
#else
	if( m_bLButtonClick )
	{
		GetRangeSlider( WIDC_CUSTOM2, m_nStep[0], point );

		g_Option.m_fEffectVolume = m_nStep[0]*0.1f;
		g_SoundMng.m_nSoundVolume = -(1.0f-g_Option.m_fEffectVolume)*5000;
	}

	if( m_bLButtonClick2 )
	{
		GetRangeSlider( WIDC_CUSTOM3, m_nStep[1], point );

		g_Option.m_fBGMVolume = m_nStep[1]*0.1f;
		SetVolume(g_Option.m_fBGMVolume);
	}
#endif

#if __VER >= 8 //__Y_GAMMA_CONTROL_8
#ifdef __DISABLE_GAMMA_WND
	if(g_Neuz.m_bStartFullscreen)
	{
#endif
		if( m_bLButtonClick3 )
		{
			GetRangeSlider( WIDC_CUSTOM1, m_nStep[2], point );
			g_Option.m_fGamma = (FLOAT)m_nStep[2] / (FLOAT)5.0f;
			g_Neuz.Drv_SetGamma( g_Neuz.GetSafeHwnd(), g_Option.m_fGamma, g_Option.m_nOverBright, g_Option.m_fContrast );
		}
		if( m_bLButtonClick4 )
		{
			GetRangeSlider( WIDC_CUSTOM4, m_nStep[3], point );
			g_Option.m_nOverBright = m_nBrightTable[m_nStep[3]];
			g_Neuz.Drv_SetGamma( g_Neuz.GetSafeHwnd(), g_Option.m_fGamma, g_Option.m_nOverBright, g_Option.m_fContrast );
		}
		if( m_bLButtonClick5 )
		{
			GetRangeSlider( WIDC_CUSTOM5, m_nStep[4], point );
			g_Option.m_fContrast = (FLOAT)m_nStep[4] / (FLOAT)5.0f;
			g_Neuz.Drv_SetGamma( g_Neuz.GetSafeHwnd(), g_Option.m_fGamma, g_Option.m_nOverBright, g_Option.m_fContrast );
		}
#ifdef __DISABLE_GAMMA_WND
	}
#endif
#endif //__Y_GAMMA_CONTROL_8
}

void CWndOptVideo::OnMouseWndSurface( CPoint point )
{
}
BOOL CWndOptVideo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWndButton* pWndButton		= (CWndButton*)*pLResult;
	CWndButton* pWndZoomLimit   = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
#if __VER >= 12 // __UPDATE_OPT
	CWndButton* pWndCamearaLock = (CWndButton*)GetDlgItem( WIDC_CHECK5 );
#endif

#ifdef __NEW_NIGHT
	CWndButton* pWndDayNight = (CWndButton*)GetDlgItem(WIDC_CHECKDAYNIGHT);
#endif //__NEW_NIGHT

#ifdef __BACKGROUNDRENDER
	CWndButton* pWndLowRenderCheck = (CWndButton*)GetDlgItem(WIDC_CHECK_LOW_RENDER);
#endif // __BACKGROUNDRENDER

#ifdef __ALIASING
	CWndButton* pWndAliasing = (CWndButton*)GetDlgItem(WIDC_CHECK_ALIASING);
#endif // __ALIASING

#ifdef __ANISOTROPIC
	CWndButton* pWndAnisotropic = (CWndButton*)GetDlgItem(WIDC_CHECK_ANISOTROPIC);
#endif // __ANISOTROPIC

	switch( nID )
	{
	case WIDC_OBJECT_LOD_HIGH:
		g_Option.m_nObjectDetail = 0;
		break;
	case WIDC_OBJECT_LOD_MID:
		g_Option.m_nObjectDetail = 1;
		break;
	case WIDC_OBJECT_LOD_LOW:
		g_Option.m_nObjectDetail = 2;
		break;

	case WIDC_OBJECT_DISTANT_HIGH:
		g_Option.m_nObjectDistant = 0;
		break;
	case WIDC_OBJECT_DISTANT_MID:
		g_Option.m_nObjectDistant = 1;
		break;
	case WIDC_OBJECT_DISTANT_LOW:
		g_Option.m_nObjectDistant = 2;
		break;

	case WIDC_TERRAIN_LOD_HIGH:
		g_Option.m_nViewArea = 0;
		SetLODDetail( g_Option.m_nViewArea );
		break;
	case WIDC_TERRAIN_LOD_MID:
		g_Option.m_nViewArea = 1;
		SetLODDetail( g_Option.m_nViewArea );
		break;
	case WIDC_TERRAIN_LOD_LOW:
		g_Option.m_nViewArea = 2;
		SetLODDetail( g_Option.m_nViewArea );
		break;

	case WIDC_SHADOW_HIGH:
		g_Option.m_nShadow = 0;
		CreateShadowMap( m_pApp->m_pd3dDevice, g_Neuz.m_d3dpp.BackBufferFormat );
		break;
	case WIDC_SHADOW_MID:
		g_Option.m_nShadow = 1;
		CreateShadowMap( m_pApp->m_pd3dDevice, g_Neuz.m_d3dpp.BackBufferFormat );
		break;
	case WIDC_SHADOW_LOW:
		g_Option.m_nShadow = 2;
		break;
#ifdef __SFX_OPT
	case WIDC_CHECK4:
#ifdef __AEGON_FULLSCREEN_AND_RESIZE
		pWndButton = (CWndButton*)GetDlgItem(WIDC_CHECK4);
		if (pWndButton)
		{
			g_Option.m_bStartFullScreen = !pWndButton->GetCheck();
			g_pD3dApp->AegonToggleFullscreen();
		}
		break;
#else // __AEGON_FULLSCREEN_AND_RESIZE
			pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK4 );
			if(pWndButton)
			{
				g_Option.m_bStartFullScreen = !(pWndButton->GetCheck());
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GAME_RESETTING), MB_OK, this );
			}
		break;
#endif // __AEGON_FULLSCREEN_AND_RESIZE

#else
	case WIDC_GLOWON:
		if( g_Neuz.m_d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1,1) )
		{
			g_Option.m_nBloom = 1; 
			g_Glare.Create( D3DDEVICE, D3DFMT_R5G6B5, g_Option.m_nResWidth, g_Option.m_nResHeight - 48 );
		}
		break;
	case WIDC_GLOWOFF:
		g_Option.m_nBloom = 0;
		break;
	case WIDC_VOICEON:
		g_Option.m_bVoice = 1;
		break;
	case WIDC_VOICEOFF:
		g_Option.m_bVoice = 0;
		break;
	case WIDC_CHECK5:
		pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK5 );
		g_Option.m_bSFXRenderOff = !pWndButton->GetCheck();
		break;
	case WIDC_MOUSE_HIGH:
		g_Option.m_MouseSpeed = 0;
		break;
	case WIDC_MOUSE_MID:
		g_Option.m_MouseSpeed = 1;
		break;
	case WIDC_MOUSE_LOW:
		g_Option.m_MouseSpeed = 2;
		break;
	case WIDC_CHECK4:
		if( g_Neuz.m_bFakeFullMode )
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK4 );
			g_Option.m_bStartFullScreen = !(pWndButton->GetCheck());
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GAME_RESETTING), MB_OK, this );
		}
		break;
#endif
#ifdef __YENV
	case WIDC_SEPCBUMPCHECK:
		// 설정값을 바로 적용하는것이 아니라 종료할때 적용한다
		// 옵션중 다시 시작해야 적용되는것들을 위한 변수..( 범프맵... )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_SEPCBUMPCHECK );
		if( pWndButton->GetCheck() != g_Option.m_bSpecBump )
		{
			g_Option.m_bExitChange = TRUE;
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GAME_RESETTING), MB_OK, this );
		}
		else
			g_Option.m_bExitChange = FALSE;
		break;
#endif //__YENV
	case WIDC_CHECK2:
		pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK2 );
		g_Option.m_bDamageRender = pWndButton->GetCheck();
		break;

	case WIDC_CHECK6:
		pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK6 );
		g_Option.m_nPlayerName = pWndButton->GetCheck();
		break;
	case WIDC_CHECK7:
		pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK7 );
		g_Option.m_nOtherPlayerName = pWndButton->GetCheck();
		break;
	case WIDC_CHECK8:
		pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK8 );
		g_Option.m_nMonName = pWndButton->GetCheck();
		break;
#if __VER >= 12 // __UPDATE_OPT
	case WIDC_CHECK1:
	{
		if( pWndZoomLimit->GetCheck() )
			g_Option.m_bZoomLimit = FALSE;
		else
			g_Option.m_bZoomLimit = TRUE;			
	}	
	break;
	case WIDC_VIEWMASK:
		pWndButton = (CWndButton*)GetDlgItem( WIDC_VIEWMASK );
		g_Option.m_bViewMask = pWndButton->GetCheck();
		g_DPlay.SendOptionEnableRenderMask( pWndButton->GetCheck() );
		break;
	case WIDC_CHECK5:
		{
			if( pWndCamearaLock->GetCheck() )
				g_Option.m_bCameraLock = TRUE;
			else
				g_Option.m_bCameraLock = FALSE;			
		}
		break;
#else
	case WIDC_VISIBLEKEEPTIME:
		pWndButton = (CWndButton*)GetDlgItem( WIDC_VISIBLEKEEPTIME );
		g_Option.m_bVisibleBuffTimeRender = pWndButton->GetCheck();
		break;
#endif
	case WIDC_CHECK3:
		pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK3 );
		if(pWndButton) g_Option.m_nWeatherEffect = pWndButton->GetCheck();
		break;
	/*
	case WIDC_EFFECTONOFF:
		{
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_EFFECTONOFF);
			g_Option.m_bEffect = pButton->GetCheck();
			pButton->SetCheck( g_Option.m_bEffect );
		}
		break;
	case WIDC_BMGONOFF:
		{
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BMGONOFF);
			g_Option.m_bMusic = pButton->GetCheck();
			pButton->SetCheck( g_Option.m_bMusic );

			// 음악 끄고 켜기
			SetOn( g_Option.m_bMusic );
		}
		break;
	*/
		
/*		
	case WIDC_SOUND_ON:
		g_Option.m_nSoundEffect = g_SoundMng.m_nSoundVolume = 1;
		break;
	case WIDC_SOUND_OFF:
		g_Option.m_nSoundEffect = g_SoundMng.m_nSoundVolume = 0;
		break;

	case WIDC_MUSIC_ON:
		g_Option.m_fMusicVolume = 1.0f;
		SetVolume( 1.0f );
		break;
	case WIDC_MUSIC_OFF:
		g_Option.m_fMusicVolume = 0.0f;
		SetVolume( 0.0f );
		break;
*/
	case WIDC_OK:
		Destroy();
		break;

#ifdef __NEW_NIGHT
	case WIDC_CHECKDAYNIGHT:
	{
		if (pWndDayNight->GetCheck())
		{
			g_Option.m_bNewNight = TRUE;
		}
		else
		{
			g_Option.m_bNewNight = FALSE;
		}
		g_WorldMng.Get()->InitWorldEnvir();
	}
	break;
#endif // __NEW_NIGHT

#ifdef __BACKGROUNDRENDER
	case WIDC_CHECK_LOW_RENDER:
	{
		if (pWndLowRenderCheck->GetCheck())
		{
			g_Option.inactiveRender = 1;
		}
		else
		{
			g_Option.inactiveRender = 0;
		}
	}
	break;
#endif // __BACKGROUNDRENDER

#ifdef __ALIASING
	case WIDC_CHECK_ALIASING:
	{
		if (pWndAliasing->GetCheck())
		{
			g_Option.m_nAliasing = TRUE;
		}
		else
		{
			g_Option.m_nAliasing = FALSE;
		}
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GAME_RESETTING), MB_OK, this);
	}
	break;
#endif // __ALIASING

#ifdef __ANISOTROPIC
	case WIDC_CHECK_ANISOTROPIC:
	{
		if (pWndAnisotropic->GetCheck())
		{
			g_Option.m_nAnisotropic = TRUE;
		}
		else
		{
			g_Option.m_nAnisotropic = FALSE;
		}
	}
	break;
#endif // __ANISOTROPIC
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
#ifdef __AEGON_THEME_SWITCHER
void CWndOptVideo::OnThemeSelectionChanged()
{
	CWndNeuz::OnThemeSelectionChanged();

	CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchLoadTexture(&m_Texture, g_Neuz.GetDevice(), "WndVolumeBar.tga", COLOR_ITEM_BG, TRUE);
	CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchLoadTexture(&m_TexturePt, g_Neuz.GetDevice(), "ButtSpin.tga", 0xffffffff, TRUE);

}
#endif // __AEGON_THEME_SWITCHER


CWndOptWindow::CWndOptWindow() 
{ 
} 
CWndOptWindow::~CWndOptWindow() 
{ 
} 
void CWndOptWindow::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndOptWindow::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndButton* pWndEffect = (CWndButton*)GetDlgItem( WIDC_CHECK );
	pWndEffect->SetCheck( g_Option.m_nWindowAlpha == 255 ? FALSE : TRUE);

	CWndButton* pWndButton[ 3 ];

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nWindowEffect ]->SetCheck( TRUE );
/*
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_DISTANT_HIGH );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_DISTANT_MID );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_DISTANT_LOW );
	pWndButton[ 0 ]->SetGroup( TRUE );
*/

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndOptWindow::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_OPTION_WINDOW, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndOptWindow::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndOptWindow::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndOptWindow::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndOptWindow::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndOptWindow::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWndButton* pWndAlpha = (CWndButton*)GetDlgItem( WIDC_CHECK );
	CWndButton* pWndEffect = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	switch( nID )
	{
	case WIDC_CHECK:
		if( pWndAlpha->GetCheck() )
			CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 128;
		else
			CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 255;
	case WIDC_RADIO1:
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

CWndOptMyInfo::CWndOptMyInfo() 
{ 
} 
CWndOptMyInfo::~CWndOptMyInfo() 
{ 
} 
void CWndOptMyInfo::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndOptMyInfo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 

BOOL CWndOptMyInfo::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_OPTION_MYINFO, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndOptMyInfo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndOptMyInfo::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndOptMyInfo::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndOptMyInfo::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndOptMyInfo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

int CWndOptVideo::GetSliderStep(DWORD dwWndId, int &nStep, CPoint point )
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl( dwWndId );	
	CRect crect = lpWndCtrl->rect;
	
	crect.left += GetStepPos( nStep, lpWndCtrl->rect.right - lpWndCtrl->rect.left, MAX_SLIDER ).x;
	crect.right = crect.left+16;
	
	if( PtInRect( crect, point ) )
	{
		return -1;
	}
	
	crect = lpWndCtrl->rect;
	if( PtInRect( crect, point ) )
	{
		int nWidth = lpWndCtrl->rect.right - lpWndCtrl->rect.left;
		
		if( point.x > lpWndCtrl->rect.left+GetStepPos( nStep, nWidth, MAX_SLIDER ).x )
			nStep += 1;//nWidth / 10;
		else
			nStep -= 1;//nWidth / 10;

		return 1;
	}

	return 0;
}

int CWndOptVideo::GetSliderStep(DWORD dwWndId, int &nStep, CPoint point, int nDivision)
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl( dwWndId );	
	CRect crect = lpWndCtrl->rect;
	
	crect.left += GetStepPos( nStep, lpWndCtrl->rect.right - lpWndCtrl->rect.left, nDivision ).x;
	crect.right = crect.left+16;
	
	if( PtInRect( crect, point ) )
	{
		return -1;
	}
	
	crect = lpWndCtrl->rect;
	if( PtInRect( crect, point ) )
	{
		int nWidth = lpWndCtrl->rect.right - lpWndCtrl->rect.left;
		
		if( point.x > lpWndCtrl->rect.left+GetStepPos( nStep, nWidth, nDivision ).x )
			nStep += 1;//nWidth / 10;
		else
			nStep -= 1;//nWidth / 10;

		return 1;
	}

	return 0;
}

void CWndOptVideo::GetRangeSlider(DWORD dwWndId, int &nStep, CPoint point, int nDivision)
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl( dwWndId );	
	int nWidth = lpWndCtrl->rect.right - lpWndCtrl->rect.left;
	
	if( point.x > lpWndCtrl->rect.left+GetStepPos( nStep, nWidth, nDivision ).x )
		nStep += 1;//nWidth / 10;
	if( point.x < lpWndCtrl->rect.left+GetStepPos( nStep, nWidth, nDivision ).x )
		nStep -= 1;//nWidth / 10;
	
	if( nStep < 0 )
		nStep = 0;
	
	if( nStep > nDivision )
		nStep = nDivision;
}

void CWndOptVideo::GetRangeSlider(DWORD dwWndId, int &nStep, CPoint point)
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl( dwWndId );	
	int nWidth = lpWndCtrl->rect.right - lpWndCtrl->rect.left;
	
	if( point.x > lpWndCtrl->rect.left+GetStepPos( nStep, nWidth, MAX_SLIDER ).x )
		nStep += 1;//nWidth / 10;
	if( point.x < lpWndCtrl->rect.left+GetStepPos( nStep, nWidth, MAX_SLIDER ).x )
		nStep -= 1;//nWidth / 10;
	
	if( nStep < 0 )
		nStep = 0;
	
	if( nStep > MAX_SLIDER )
		nStep = MAX_SLIDER;
}

#ifdef __TITANIUM_OPTIONS
CWndOptTitanium::CWndOptTitanium()
{
}

CWndOptTitanium::~CWndOptTitanium()
{
	//g_Option.Save("neuz.ini");
}
HRESULT CWndOptTitanium::InvalidateDeviceObjects()
{
	return CWndNeuz::InvalidateDeviceObjects();
}
HRESULT CWndOptTitanium::RestoreDeviceObjects()
{
	return CWndNeuz::RestoreDeviceObjects();
}

void CWndOptTitanium::OnDraw(C2DRender* p2DRender)
{
	CWndButton* pWndButton[3];

#ifdef __SYS_CUSTOM_TITLE
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK_TITLE);
	if (pWndButton[0])
	{
		if (g_pPlayer && (!g_pPlayer->m_szOwnTitle > NULL || g_pPlayer->m_szOwnTitle[0] == '$'))
		{
			pWndButton[0]->SetToolTip(_T("You do not have a Custom Title."));
			pWndButton[0]->SetFontColor(COLOR_ERROR);
		}
	}
#endif // __SYS_CUSTOM_TITLE

#ifdef __PREMIUM
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK_PREMIUM);
	if (pWndButton[0])
	{
		if (g_pPlayer && !g_pPlayer->IsPremium())
		{
			pWndButton[0]->SetToolTip(_T("You do not have a Premium Scroll enabled."));
			pWndButton[0]->SetFontColor(COLOR_ERROR);
		}
	}
#endif // __PREMIUM

}

void CWndOptTitanium::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();

	CWndButton* pWndButton[3];

#ifdef __SYS_GROUND_ITEM_TEXT
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK_GROUNDITEM);
	if (pWndButton[0])
	{
		pWndButton[0]->SetToolTip(_T("Toggle item names on top of ground items."));
		pWndButton[0]->SetCheck(g_Option.groundItemText);
	}
#endif // __SYS_GROUND_ITEM_TEXT



#ifdef __AEGON_FONT_SWITCH
	CWndComboBox* pWndFont = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX);
	pWndFont->AddWndStyle(EBS_READONLY);

	for (int i = 0; i < prj.vecAvailableFonts.size(); i++) 
	{
		pWndFont->AddString(prj.vecAvailableFonts[i].GetString());
	}
	
	for (int i = 0; i < pWndFont->m_wndListBox.GetCount(); i++) 
	{
		pWndFont->SetCurSel(i);
		if (g_Option.m_strFont.Compare(pWndFont->GetString()) == 0)
			break;
	}
	CWndStatic* pWndFontHeader = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	if (pWndFontHeader) 
	{
		CString strFontHeader;
		strFontHeader.Format("Game Font (%d installed)", prj.vecAvailableFonts.size());
		pWndFontHeader->SetTitle(strFontHeader);
	}
#endif // __AEGON_FONT_SWITCH

#ifdef __SWITCH_DMG_RENDER
	CWndComboBox* pWndDamage = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	pWndDamage->AddWndStyle(EBS_READONLY);
	map<int, const char*>::iterator dit = prj.DmgData.begin();
	for (; dit != prj.DmgData.end(); dit++)
	{
		pWndDamage->AddString(dit->second);
	}
	pWndDamage->SetCurSel(g_Option.damageRender);
#endif //__SWITCH_DMG_RENDER

#ifdef __SYS_PET_NO_CLICK
	CWndComboBox* pWndPetClick = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);
	pWndPetClick->AddWndStyle(EBS_READONLY);
	map<int, const char*>::iterator pit = prj.PetData.begin();
	for (; pit != prj.PetData.end(); pit++)
	{
		pWndPetClick->AddString(pit->second);
	}
	pWndPetClick->SetCurSel(g_Option.petClickOption);
#endif //__SYS_PET_NO_CLICK

#ifdef __SYS_CUSTOM_TITLE
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK_TITLE);
	if (pWndButton[0])
	{
		pWndButton[0]->SetCheck(g_pPlayer->m_bShowTitle);
	}
#endif // __SYS_CUSTOM_TITLE

#ifdef __PREMIUM
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK_PREMIUM);
	if (pWndButton[0])
	{
		pWndButton[0]->SetCheck(g_pPlayer->m_bShowPremium);
	}
#endif // __PREMIUM

#ifdef __SYS_CUSTOM_TITLE
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK_TITLE);
	if (pWndButton[0])
	{
		pWndButton[0]->SetCheck(g_Option.m_bShowCustomTitle);
	}
#endif //__SYS_CUSTOM_TITLE

#ifdef __PREMIUM
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK_PREMIUM);
	if (pWndButton[0])
	{
		pWndButton[0]->SetCheck(g_Option.m_bShowPremiumTitle);
	}
#endif //__PREMIUM

#ifdef __NEWCUR
	CWndComboBox* pComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX3);
	if (pComboBox)
	{
		pComboBox->AddWndStyle(EBS_READONLY);

		int nIdx = pComboBox->AddString("Titanium Cursors");
		pComboBox->SetItemData(nIdx, 0);

		nIdx = pComboBox->AddString("Default Cursors");
		pComboBox->SetItemData(nIdx, 1);

		nIdx = pComboBox->AddString("Custom Cursors #1");
		pComboBox->SetItemData(nIdx, 2);

		nIdx = pComboBox->AddString("Custom Cursors #2");
		pComboBox->SetItemData(nIdx, 3);

		pComboBox->SetCurSel(g_Option.m_nCurFold);
	}
#endif // __NEWCUR

#ifdef __AEGON_THEME_SWITCHER
	CWndComboBox* pComboBoxThemes = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX4);
	if (pComboBoxThemes)
	{
		pComboBoxThemes->AddWndStyle(EBS_READONLY);

		map<int, __AEGON_THEME_UNIT>::iterator it = CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.begin();
		for (; it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end(); it++) 
		{
			int nIdx = pComboBoxThemes->AddString(it->second.szThemeName);
			pComboBoxThemes->SetItemData(nIdx, it->second.nThemeID);
		}

		if (g_Option.m_nSelectedThemeID >= CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.size())
			g_Option.m_nSelectedThemeID = 0;

		pComboBoxThemes->SetCurSel(g_Option.m_nSelectedThemeID);
	}
#endif // __AEGON_THEME_SWITCHER


#ifdef __AEGON_FULLSCREEN_AND_RESIZE
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	if (pWndButton[0])
		pWndButton[0]->SetCheck(g_Option.m_bCenterTaskbar);
#endif // __AEGON_FULLSCREEN_AND_RESIZE

#ifdef __SINGLE_TASKBAR_SWAP
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK2);
	if (pWndButton[0])
		pWndButton[0]->SetCheck(g_Option.m_bTaskbarDisplay);
#endif // __SINGLE_TASKBAR_SWAP

#ifdef __ALPHA_CHAT
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK_ALPHACHAT);
	if (pWndButton[0])
	{
		pWndButton[0]->SetToolTip(_T("Makes the chat invisible, hover over it to view the controls."));
		pWndButton[0]->SetCheck(g_Option.alphaChat);
	}
#endif //__ALPHA_CHAT

#ifdef __NEW_BUFF_FORMAT
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK3);
	if (pWndButton[0])
	{
		pWndButton[0]->SetToolTip(_T("Rendering Round Buff Icons"));
		pWndButton[0]->SetCheck(g_Option.m_Buffs);
	}
#endif // __NEW_BUFF_FORMAT

#ifdef __RENDER_SMALL_POWERUPS
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK4);
	if (pWndButton[0])
	{
		pWndButton[0]->SetToolTip(_T("Powerups are 50% smaller on top of your screen."));
		pWndButton[0]->SetCheck(g_Option.m_nSmallPowerUps);
	}
#endif //__RENDER_SMALL_POWERUPS

#ifdef __NAVIGATOR_V19
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK5);
	if (pWndButton[0])
	{
		pWndButton[0]->SetToolTip(_T("Toggle between the V15 & V19 navigator."));
		pWndButton[0]->SetCheck(g_Option.m_bV19Navigator);
	}
#endif // __NAVIGATOR_V19

#ifdef __HEAL_SHOW
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK6);
	if (pWndButton[0])
	{
		pWndButton[0]->SetToolTip(_T("Hide displaying of healing numbers."));
		pWndButton[0]->SetCheck(g_Option.m_bHideHeal);
	}
#endif //__HEAL_SHOW

#ifdef __HIDE_POWER_UPS
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK_POWERUPS);
	if (pWndButton[0])
	{
		pWndButton[0]->SetToolTip(_T("Hide certain powerups on the top of your screen."));
		pWndButton[0]->SetCheck(g_Option.m_nHidePU);
	}
#endif // __HIDE_POWER_UPS

}

BOOL CWndOptTitanium::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_OPTION_VIDEO, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndOptTitanium::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndOptTitanium::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndOptTitanium::OnLButtonUp(UINT nFlags, CPoint point)
{
#ifdef __fmod
	SoundSystemClass::playSound(SND_NONE);
#else
	PLAYSND(SND_NONE);
#endif
	ReleaseCapture();
}
void CWndOptTitanium::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
}

void CWndOptTitanium::OnMouseMove(UINT nFlags, CPoint point)
{
}

void CWndOptTitanium::OnMouseWndSurface(CPoint point)
{
}
BOOL CWndOptTitanium::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{

#ifdef __SYS_GROUND_ITEM_TEXT
	CWndButton* pWndGroundItem = (CWndButton*)GetDlgItem(WIDC_CHECK_GROUNDITEM);
#endif //__SYS_GROUND_ITEM_TEXT

#ifdef __AEGON_FONT_SWITCH
	CWndComboBox* pWndFont = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX);
#endif // __AEGON_FONT_SWITCH

#ifdef __SYS_CUSTOM_TITLE
	CWndButton* pWndShowTitle = (CWndButton*)GetDlgItem(WIDC_CHECK_TITLE);
#endif //__SYS_CUSTOM_TITLE

#ifdef __PREMIUM
	CWndButton* pWndShowPremium = (CWndButton*)GetDlgItem(WIDC_CHECK_PREMIUM);
#endif //__PREMIUM

#ifdef __SWITCH_DMG_RENDER 
	CWndComboBox* pWndDamage = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
#endif //__SWITCH_DMG_RENDER

#ifdef __SYS_PET_NO_CLICK 
	CWndComboBox* pWndPetClick = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);
#endif // __SYS_PET_NO_CLICK

#ifdef __NEWCUR
	CWndComboBox* pComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX3);
#endif // __NEWCUR

#ifdef __AEGON_THEME_SWITCHER
	CWndComboBox* pComboBoxThemes = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX4);
#endif // __AEGON_THEME_SWITCHER

#ifdef __ALPHA_CHAT
	CWndButton* pWndAlphaChat = (CWndButton*)GetDlgItem(WIDC_CHECK_ALPHACHAT);
#endif //__ALPHA_CHAT

#ifdef __NEW_BUFF_FORMAT
	CWndButton* pWndBuffs = (CWndButton*)GetDlgItem(WIDC_CHECK3);
#endif // __NEW_BUFF_FORMAT

#ifdef __RENDER_SMALL_POWERUPS
	CWndButton* pWndSmallPU = (CWndButton*)GetDlgItem(WIDC_CHECK4);
#endif //__RENDER_SMALL_POWERUPS

#ifdef __NAVIGATOR_V19
	CWndButton* pWndV19Nav = (CWndButton*)GetDlgItem(WIDC_CHECK5);
#endif // __NAVIGATOR_V19

#ifdef __HEAL_SHOW
	CWndButton* pWndHideHeal = (CWndButton*)GetDlgItem(WIDC_CHECK6);
#endif //__HEAL_SHOW

#ifdef __HIDE_POWER_UPS
	CWndButton* pWndShowPowerups = (CWndButton*)GetDlgItem(WIDC_CHECK_POWERUPS);
#endif //__HIDE_POWER_UPS

	switch (nID)
	{

#ifdef __SYS_GROUND_ITEM_TEXT
	case WIDC_CHECK_GROUNDITEM:
	{
		if (pWndGroundItem->GetCheck())
		{
			g_Option.groundItemText = TRUE;
		}
		else
		{
			g_Option.groundItemText = FALSE;
		}
	}
	break;
#endif // __SYS_GROUND_ITEM_TEXT

#ifdef __AEGON_FONT_SWITCH
	case WIDC_COMBOBOX:
	{
		g_Option.m_strFont.Format("%s", pWndFont->GetString());
		if (g_Option.m_strFont.Compare(AEG_DEFAULT_THEME) == 0)
			g_Option.m_strFont = "Verdana";
		g_Neuz.OnAegonFontSwitch();		
	}
	break;
#endif // __AEGON_FONT_SWITCH

#ifdef __SWITCH_DMG_RENDER
	case WIDC_COMBOBOX1:
	{
		CString strDmg;
		strDmg.Format("%s", pWndDamage->GetString());

		map<int, const char*>::iterator dit = prj.DmgData.begin();
		for (; dit != prj.DmgData.end(); dit++)
		{
			if (strDmg.Compare(dit->second) == 0)
			{
				g_Option.damageRender = dit->first;
			}
		}
	}
	break;
#endif //__SWITCH_DMG_RENDER

#ifdef __SYS_PET_NO_CLICK
	case WIDC_COMBOBOX2:
	{
		CString strPetNoClick;
		strPetNoClick.Format("%s", pWndPetClick->GetString());

		map<int, const char*>::iterator pit = prj.PetData.begin();
		for (; pit != prj.PetData.end(); pit++)
		{
			if (strPetNoClick.Compare(pit->second) == 0)
			{
				g_Option.petClickOption = pit->first;
			}
		}
	}
	break;
#endif //__SYS_PET_NO_CLICK

#ifdef __SYS_CUSTOM_TITLE
	case WIDC_CHECK_TITLE:
	{
		if (!g_pPlayer->m_szOwnTitle > NULL || g_pPlayer->m_szOwnTitle[0] == '$')
		{
			g_WndMng.PutString("You do not have a Custom Title.", NULL, COLOR_ERROR);
		}
		else
		{
			if (pWndShowTitle->GetCheck())
			{
				g_DPlay.SendUpdateShowTitle(TRUE);
				g_WndMng.PutString("You are now showing your Custom Title!", NULL, COLOR_SUCCESS);
				
			}
			else
			{
				g_DPlay.SendUpdateShowTitle(FALSE);
				g_WndMng.PutString("You've hidden your Custom Title!", NULL, COLOR_WARNING);
			}
		}
		if (pWndShowTitle->GetCheck()) 
			g_Option.m_bShowCustomTitle = TRUE;
		else
			g_Option.m_bShowCustomTitle = FALSE;
	}
	break;
#endif // __SYS_CUSTOM_TITLE

#ifdef __PREMIUM
	case WIDC_CHECK_PREMIUM:
	{
		if (!g_pPlayer->IsPremium())
		{
			g_WndMng.PutString("You do not have a Premium Scroll enabled.", NULL, COLOR_ERROR);
		}
		else
		{
			if (pWndShowPremium->GetCheck())
			{
				g_DPlay.SendUpdateShowPremium(TRUE);
				g_WndMng.PutString("You are now showing your Premium Title!", NULL, COLOR_SUCCESS);
			}
			else
			{
				g_DPlay.SendUpdateShowPremium(FALSE);
				g_WndMng.PutString("You've hidden your Premium Title!", NULL, COLOR_WARNING);
			}
		}
		if (pWndShowPremium->GetCheck())
			g_Option.m_bShowPremiumTitle = TRUE;
		else
			g_Option.m_bShowPremiumTitle = FALSE;
	}
	break;
#endif // __PREMIUM

#ifdef __NEWCUR
	case WIDC_COMBOBOX3:
	{
		if (message == WNM_SELCHANGE)
		{
			if (pComboBox)
			{
				g_Option.m_nCurFold = pComboBox->GetSelectedItemData();
				g_Neuz.UpdateCursor();
			}
		}
	}
	break;
#endif // __NEWCUR

#ifdef __AEGON_THEME_SWITCHER
	case WIDC_COMBOBOX4:
	{
		if (message == WNM_SELCHANGE)
		{
			if (pComboBoxThemes)
			{
				BOOL bThemeTypeSwitched = FALSE;
				if (CAegonThemeSwitcherMng::GetInstance()->ThemeTypeHasChanged(g_Option.m_nSelectedThemeID, pComboBoxThemes->GetSelectedItemData()))
				{
					bThemeTypeSwitched = TRUE;
				}

				g_Option.m_nSelectedThemeID = pComboBoxThemes->GetSelectedItemData();
				g_WndMng.OnThemeSelectionChangedTrigger(bThemeTypeSwitched);
				g_Neuz.OnAegonFontSwitch();
			}
		}
	}
	break;
#endif // __AEGON_THEME_SWITCHER

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
	case WIDC_CHECK1:
	{
		CWndButton* pWndButtonCenterTB = (CWndButton*)GetDlgItem(WIDC_CHECK1);
		if (pWndButtonCenterTB)
		{
			g_Option.m_bCenterTaskbar = pWndButtonCenterTB->GetCheck();
			if (g_WndMng.m_pWndTaskBar)
			{
				//restart taskbar
				g_WndMng.m_pWndTaskBar->Destroy();
				SAFE_DELETE(g_WndMng.m_pWndTaskBar);
				g_WndMng.OpenTaskBar();
				g_DPlay.SendReloadTaskbar();
			}
		}
	}
	break;
#endif // __AEGON_FULLSCREEN_AND_RESIZE
#ifdef __SINGLE_TASKBAR_SWAP
	case WIDC_CHECK2:
	{
		CWndButton* pWndButtonFKeys = (CWndButton*)GetDlgItem(WIDC_CHECK2);
		if (pWndButtonFKeys)
		{
			g_Option.m_bTaskbarDisplay = pWndButtonFKeys->GetCheck();
			if (g_WndMng.m_pWndTaskBar)
			{
				//restart taskbar
				g_WndMng.RestartTaskBar();
				g_DPlay.SendReloadTaskbar();
			}
		}
	}
	break;
#endif // __SINGLE_TASKBAR_SWAP

#ifdef __ALPHA_CHAT
	case WIDC_CHECK_ALPHACHAT:
	{
		if (pWndAlphaChat->GetCheck())
		{
			g_Option.alphaChat = true;
		}
		else
		{
			g_Option.alphaChat = false;
		}
	}
	break;
#endif // __ALPHA_CHAT

#ifdef __NEW_BUFF_FORMAT
	case WIDC_CHECK3:
	{
		if (pWndBuffs->GetCheck())
		{
			g_Option.m_Buffs = TRUE;
		}
		else
		{
			g_Option.m_Buffs = FALSE;
		}
	}
	break;
#endif // __NEW_BUFF_FORMAT

#ifdef __RENDER_SMALL_POWERUPS
	case WIDC_CHECK4:
	{
		if (pWndSmallPU->GetCheck())
		{
			g_Option.m_nSmallPowerUps = TRUE;
		}
		else
		{
			g_Option.m_nSmallPowerUps = FALSE;
		}
	}
	break;
#endif //__RENDER_SMALL_POWERUPS

#ifdef __NAVIGATOR_V19
	case WIDC_CHECK5:
	{
		if (pWndV19Nav->GetCheck())
		{
			g_Option.m_bV19Navigator = TRUE;
		}
		else
		{
			g_Option.m_bV19Navigator = FALSE;
		}
		g_WndMng.RestartMinimap();
	}
	break;
#endif // __NAVIGATOR_V19

#ifdef __HEAL_SHOW
	case WIDC_CHECK6:
	{
		if (pWndHideHeal->GetCheck())
		{
			g_Option.m_bHideHeal = TRUE;
		}
		else
		{
			g_Option.m_bHideHeal = FALSE;
		}
	}
	break;
#endif //__HEAL_SHOW
#ifdef __HIDE_POWER_UPS
	case WIDC_CHECK_POWERUPS:
	{
		if (pWndShowPowerups->GetCheck())
		{
			g_Option.m_nHidePU = TRUE;
		}
		else
		{
			g_Option.m_nHidePU = FALSE;
		}
	}
	break;
#endif // __HIDE_POWER_UPS

	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

CWndOptTitanium2::CWndOptTitanium2()
{
}

CWndOptTitanium2::~CWndOptTitanium2()
{
	//g_Option.Save("neuz.ini");
}
HRESULT CWndOptTitanium2::InvalidateDeviceObjects()
{
	return CWndNeuz::InvalidateDeviceObjects();
}
HRESULT CWndOptTitanium2::RestoreDeviceObjects()
{
	return CWndNeuz::RestoreDeviceObjects();
}

void CWndOptTitanium2::OnDraw(C2DRender* p2DRender)
{
	CWndButton* pWndButton[3];
}

void CWndOptTitanium2::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();

	CWndButton* pWndButton[3];

#ifdef __HIDE_NPC_BUBBLES
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK_BUBBLES);
	if (pWndButton[0])
	{
		pWndButton[0]->SetToolTip(_T("Hide all NPC Chat Bubbles to make the cities less annoying."));
		pWndButton[0]->SetCheck(g_Option.m_nNPCBubbles);
	}
#endif // __HIDE_NPC_BUBBLES

#ifdef __HIDE_MONSTER_LEVEL
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK_MOBLVL);
	if (pWndButton[0])
	{
		pWndButton[0]->SetToolTip(_T("Show monsters levels behind the monsters name."));
		pWndButton[0]->SetCheck(g_Option.m_nMonsterLevel);
	}
#endif // __HIDE_MONSTER_LEVEL

#ifdef __STOP_EXP
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK_STOPEXP);
	if (pWndButton[0])
	{
		pWndButton[0]->SetToolTip(_T("Stop the gain of experience for your character."));
		pWndButton[0]->SetCheck(g_Option.m_bExpStop);
	}
#endif //__STOP_EXP

#ifdef __AEGON_INGAME_DP
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	if (pWndButton[0])
	{
		pWndButton[0]->SetToolTip(_T("Purchase with DP directly without having to confirm."));
		pWndButton[0]->SetCheck(g_Option.m_bSkipDPConfirmLootbox ? 1 : 0);
	}
#endif // __AEGON_INGAME_DP

#ifdef __LEESE_MESSENGER_TIMER
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK2);
	if (pWndButton[0])
	{
		pWndButton[0]->SetToolTip(_T("Toggle the time indicator in Messenger & Private Shop window."));
		pWndButton[0]->SetCheck(g_Option.m_bTimeStamp);
	}
#endif // __LEESE_MESSENGER_TIMER

#ifdef __TRAILS
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK3);
	if (pWndButton[0])
	{
		pWndButton[0]->SetToolTip(_T("Hide Trails (System) on Players."));
		pWndButton[0]->SetCheck(g_Option.m_bHideTrails);
	}
#endif //__TRAILS

#ifdef __AUTO_ATTACK_NEXT_TARGET
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK4);
	if (pWndButton[0])
	{
		pWndButton[0]->SetToolTip(_T("Auto attack the next selected target."));
		pWndButton[0]->SetCheck(g_Option.m_bAutoAttackNT);
	}
#endif //__AUTO_ATTACK_NEXT_TARGET

#ifdef __AEGON_MONSTER_HUNT
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK5);
	if (pWndButton[0])
	{
		pWndButton[0]->SetCheck(g_Option.m_bPrintMHProgress);
	}
#endif //__AEGON_MONSTER_HUNT

#ifdef __WBQT_GUILD_BUFFS
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK6);
	if (pWndButton[0])
	{
		pWndButton[0]->SetCheck(g_Option.m_bHideGuildBuffs);
	}
#endif
}

BOOL CWndOptTitanium2::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_OPTION_VIDEO, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndOptTitanium2::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndOptTitanium2::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndOptTitanium2::OnLButtonUp(UINT nFlags, CPoint point)
{
#ifdef __fmod
	SoundSystemClass::playSound(SND_NONE);
#else
	PLAYSND(SND_NONE);
#endif
	ReleaseCapture();
}
void CWndOptTitanium2::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
}

BOOL CWndOptTitanium2::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
#ifdef __HIDE_NPC_BUBBLES
	CWndButton* pWndNPCBubbles = (CWndButton*)GetDlgItem(WIDC_CHECK_BUBBLES);
#endif //__HIDE_NPC_BUBBLES

#ifdef __HIDE_MONSTER_LEVEL
	CWndButton* pWndMonsterLevel = (CWndButton*)GetDlgItem(WIDC_CHECK_MOBLVL);
#endif // __HIDE_MONSTER_LEVEL

#ifdef __STOP_EXP
	CWndButton* pWndStopExp = (CWndButton*)GetDlgItem(WIDC_CHECK_STOPEXP);
#endif //__STOP_EXP

#ifdef __AEGON_INGAME_DP
	CWndButton* pWndSkipConfirmLootbox = (CWndButton*)GetDlgItem(WIDC_CHECK1);
#endif // __AEGON_INGAME_DP

#ifdef __LEESE_MESSENGER_TIMER
	CWndButton* pWndTimeStamp = (CWndButton*)GetDlgItem(WIDC_CHECK2);
#endif // __LEESE_MESSENGER_TIMER

#ifdef __TRAILS
	CWndButton* pWndTrails = (CWndButton*)GetDlgItem(WIDC_CHECK3);
#endif //__TRAILS

#ifdef __AUTO_ATTACK_NEXT_TARGET
	CWndButton* pWndAutoAttack = (CWndButton*)GetDlgItem(WIDC_CHECK4);
#endif //__AUTO_ATTACK_NEXT_TARGET

#ifdef __AEGON_MONSTER_HUNT
	CWndButton* pWndMonsterhuntProgress = (CWndButton*)GetDlgItem(WIDC_CHECK5);
#endif //__AEGON_MONSTER_HUNT


#ifdef __WBQT_GUILD_BUFFS
	CWndButton* pWndHideGuildBuffs = (CWndButton*)GetDlgItem(WIDC_CHECK6);
#endif

	switch (nID)
	{

#ifdef __HIDE_MONSTER_LEVEL
	case WIDC_CHECK_MOBLVL:
	{
		if (pWndMonsterLevel->GetCheck())
		{
			g_Option.m_nMonsterLevel = TRUE;
		}
		else
		{
			g_Option.m_nMonsterLevel = FALSE;
		}
	}
	break;
#endif // __HIDE_MONSTER_LEVEL

#ifdef __STOP_EXP
	case WIDC_CHECK_STOPEXP:
	{
		if (pWndStopExp->GetCheck()) {
			g_Option.m_bExpStop = TRUE;
			g_DPlay.SendSetExpStop(TRUE);
		}
		else {
			g_Option.m_bExpStop = FALSE;
			g_DPlay.SendSetExpStop(FALSE);
		}
	}
	break;
#endif // __STOP_EXP

#ifdef __AEGON_INGAME_DP
	case WIDC_CHECK1:
	{
		g_Option.m_bSkipDPConfirmLootbox = pWndSkipConfirmLootbox->GetCheck() == 0 ? FALSE : TRUE;
	}
	break;
#endif // __AEGON_INGAME_DP

#ifdef __HIDE_NPC_BUBBLES
	case WIDC_CHECK_BUBBLES:
	{
		if (pWndNPCBubbles->GetCheck())
		{
			g_Option.m_nNPCBubbles = TRUE;
		}
		else
		{
			g_Option.m_nNPCBubbles = FALSE;
		}
	}
	break;
#endif // __HIDE_NPC_BUBBLES

#ifdef __LEESE_MESSENGER_TIMER
	case WIDC_CHECK2:
	{
		if (pWndTimeStamp->GetCheck())
		{
			g_Option.m_bTimeStamp = TRUE;
		}
		else
		{
			g_Option.m_bTimeStamp = FALSE;
		}
	}
	break;
#endif // __LEESE_MESSENGER_TIMER

#ifdef __TRAILS
	case WIDC_CHECK3:
	{
		if (pWndTrails->GetCheck())
		{
			g_Option.m_bHideTrails = TRUE;
		}
		else
		{
			g_Option.m_bHideTrails = FALSE;
		}
	}
	break;
#endif //__TRAILS

#ifdef __AUTO_ATTACK_NEXT_TARGET
	case WIDC_CHECK4:
	{
		if (pWndAutoAttack->GetCheck())
		{
			g_Option.m_bAutoAttackNT = TRUE;
		}
		else
		{
			g_Option.m_bAutoAttackNT = FALSE;
		}
	}
	break;
#endif //__AUTO_ATTACK_NEXT_TARGET

#ifdef __AEGON_MONSTER_HUNT
	case WIDC_CHECK5:
	{
		if (pWndMonsterhuntProgress->GetCheck())
		{
			g_Option.m_bPrintMHProgress = TRUE;
		}
		else
		{
			g_Option.m_bPrintMHProgress = FALSE;
		}
	}
	break;
#endif //__AEGON_MONSTER_HUNT
#ifdef __WBQT_GUILD_BUFFS
	case WIDC_CHECK6:
	{
		if (pWndHideGuildBuffs->GetCheck())
		{
			g_Option.m_bHideGuildBuffs = TRUE;
		}
		else
		{
			g_Option.m_bHideGuildBuffs = FALSE;
		}
	}
	break;
#endif

	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

#endif //__TITANIUM_OPTIONS