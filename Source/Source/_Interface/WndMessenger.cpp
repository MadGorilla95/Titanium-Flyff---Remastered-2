#include "stdafx.h"
#include "defineSound.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndMessenger.h"
#include "WndFriendCtrl.h"
#include "messenger.h"
#include "WndFriendConFirm.h"
#include "dpcertified.h"
extern	CDPCertified	g_dpCertified;

#include "WndManager.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;	 

#ifdef __PARTYLIST_MESSENGER
#include "party.h"
extern CParty		g_Party;
#endif // __PARTYLIST_MESSENGER

/****************************************************
  WndId : APP_MESSENGER - Messenger
  CtrlId : WIDC_TABCTRL1 - TabCtrl
  CtrlId : WIDC_BUTTON1 - Button
****************************************************/


CWndMessenger::CWndMessenger() 
{ 
} 
CWndMessenger::~CWndMessenger() 
{ 
	m_TexMail.DeleteDeviceObjects();
} 	
void CWndMessenger::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	CWndTabCtrl* lpTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	if( ar.IsLoading() )
	{
		int nCurSel;
		ar >> nCurSel;
		lpTabCtrl->SetCurSel( nCurSel );
	}
	else
	{
		ar << lpTabCtrl->GetCurSel();
	}
}

void CWndMessenger::OnMouseWndSurface( CPoint point )
{
	// 직업 아이콘 위치에 커서를 가져가면 접속한 서버의 정보(서버이름과 멀티서버 번호)를 보여준다.
	CRect rectHittest( 2, 2, 35, 27 );	
	if( rectHittest.PtInRect( point ) )
	{
		CPoint point2 = point;
		ClientToScreen( &point2 );
		ClientToScreen( &rectHittest );
#ifdef _WIN64
		g_toolTip.PutToolTip((ULONG_PTR)this, m_strTooltip, rectHittest, point2, 0);
#else
		g_toolTip.PutToolTip( (DWORD)this, m_strTooltip, rectHittest, point2, 0 );
#endif
	}
}

void CWndMessenger::OnDraw( C2DRender* p2DRender ) 
{ 
#ifdef __AEGON_FIXES
	if (IsInvalidObj(g_pPlayer))
		return;
#endif

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );

	DWORD dwMyState;
#ifdef __RT_1025
	if( g_WndMng.m_RTMessenger.GetState() == FRS_AUTOABSENT )
		dwMyState	= FRS_ABSENT;
	else
		dwMyState	=  g_WndMng.m_RTMessenger.GetState();
#else	// __RT_1025
	if( g_WndMng.m_Messenger.m_dwMyState == FRS_AUTOABSENT )
		dwMyState	= FRS_ABSENT;
	else
		dwMyState	=  g_WndMng.m_Messenger.m_dwMyState;
#endif	// __RT_1025
	
	TEXTUREVERTEX2 vertex[ 6 * 2 ];
	TEXTUREVERTEX2* pVertices = vertex;
	
	if( dwMyState == FRS_OFFLINE )
	{
		if( g_pPlayer->IsPro() )
		{
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 5, 0 ),  ( 54 + g_pPlayer->GetJob() - 6 ) + ( 8 * g_pPlayer->GetSex() ), &pVertices, 0xffff6464 );
		}
#if __VER >= 10 // __LEGEND
		else if( g_pPlayer->IsMaster() )
		{
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 5, 0 ),  ( 54 + g_pPlayer->GetJob() - 16 ) + ( 8 * g_pPlayer->GetSex() ), &pVertices, 0xffff6464 );
		}
		else if( g_pPlayer->IsHero() )
		{
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 5, 0 ),  ( 54 + g_pPlayer->GetJob() - 24 ) + ( 8 * g_pPlayer->GetSex() ), &pVertices, 0xffff6464 );
		}
#ifdef __3RD_LEGEND16
		else if( g_pPlayer->IsLegendHero() )
		{
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 5, 0 ),  ( 54 + g_pPlayer->GetJob() - 32 ) + ( 8 * g_pPlayer->GetSex() ), &pVertices, 0xffff6464 );
		}
#endif // __3RD_LEGEND16
#endif //__LEGEND
		else
		{
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 5, 0 ),  g_pPlayer->GetJob() + ( 6 * g_pPlayer->GetSex() ), &pVertices, 0xffff6464 );
		}
	}
	else
	{
		if( g_pPlayer->IsPro() )
		{
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 5, 0 ),  ( 54 + g_pPlayer->GetJob() - 6 ) + ( 8 * g_pPlayer->GetSex() ), &pVertices, 0xffffffff );
		}
#if __VER >= 10 // __LEGEND
		else if( g_pPlayer->IsMaster() )
		{
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 5, 0 ),  ( 54 + g_pPlayer->GetJob() - 16 ) + ( 8 * g_pPlayer->GetSex() ), &pVertices, 0xffffffff );
		}
		else if( g_pPlayer->IsHero() )
		{
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 5, 0 ),  ( 54 + g_pPlayer->GetJob() - 24 ) + ( 8 * g_pPlayer->GetSex() ), &pVertices, 0xffffffff );
		}
#ifdef __3RD_LEGEND16
		else if( g_pPlayer->IsLegendHero() )
		{
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 5, 0 ),  ( 54 + g_pPlayer->GetJob() - 32 ) + ( 8 * g_pPlayer->GetSex() ), &pVertices, 0xffffffff );
		}
#endif // __3RD_LEGEND16
#endif //__LEGEND
		else
		{
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 5, 0 ),  g_pPlayer->GetJob() + ( 6 * g_pPlayer->GetSex() ), &pVertices, 0xffffffff );
		}

		if( dwMyState != FRS_ONLINE )
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 5 + 18, 0 + 16 ), 27 + ( dwMyState - 2 ), &pVertices, 0xffffffff );
	}	
	
#ifdef _WIN64
	pWndWorld->m_texMsgIcon.Render(m_pApp->m_pd3dDevice, vertex, (reinterpret_cast<INT_PTR>(pVertices) - reinterpret_cast<INT_PTR>(vertex)) / sizeof(TEXTUREVERTEX2));
#else
	pWndWorld->m_texMsgIcon.Render( m_pApp->m_pd3dDevice, vertex, ( (int) pVertices - (int) vertex ) / sizeof( TEXTUREVERTEX2 ) );
#endif
	
	CString strState;
	strState.Format( "(%s)", prj.GetText( dwMyState + TID_FRS_ONLINE ) );
	
	LPCTSTR pszName = g_pPlayer->GetName( TRUE );
	p2DRender->TextOut( 70,  2, 1, 1, pszName, 0xff606060 );
	p2DRender->TextOut( 71,  2, 1, 1, pszName, 0xff606060 );
	p2DRender->TextOut( 70, 18, 1, 1, strState, 0xff606060 );

	if( g_pPlayer && g_pPlayer->IsMode(MODE_MAILBOX) )
	{
		LPWNDCTRL lpMail = GetWndCtrl( WIDC_CUSTOM1 );
		p2DRender->RenderTexture( lpMail->rect.TopLeft(), &m_TexMail, m_nFlashCounter );
	}
} 

BOOL CWndMessenger::Process()
{
	LPWNDCTRL lpMail = GetWndCtrl( WIDC_CUSTOM1 );

	if( IsValidObj(g_pPlayer) && g_pPlayer->IsMode(MODE_MAILBOX) )
	{
		lpMail->m_bDisabled = FALSE;

		m_nFlashCounter += (m_nSwitch*8);
		
		if( m_nFlashCounter >= 255 )
		{
			m_nFlashCounter = 255;
			m_nSwitch = -1;
		}
		
		if( m_nFlashCounter <= 50 )
		{
			m_nFlashCounter = 50;
			m_nSwitch = 1;
		}

		CRect  rect    = lpMail->rect;
		CPoint point	= GetMousePoint();
		if( PtInRect( lpMail->rect, point ) )
		{
			ClientToScreen( &point );
			ClientToScreen( &rect );
#ifdef _WIN64
			g_toolTip.PutToolTip((ULONG_PTR)this, prj.GetText(TID_MAIL_RECEIVE), rect, point);
#else
			g_toolTip.PutToolTip( (DWORD)this, prj.GetText(TID_MAIL_RECEIVE), rect, point );
#endif
		}		
	}
	return TRUE;
}

void CWndMessenger::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	LPCTSTR lpszFormat = prj.GetText(TID_GAME_TOOLTIP_MESS); //접속 서버 -  %s / 멀티 %d 서버 -
	m_strTooltip.Format( lpszFormat, g_dpCertified.GetServerName(g_Option.m_nSer), g_Option.m_nMSer+1 );

	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	
	//m_wndAdminBalanceExp.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_ADMIN_BALANCE_EXP );
	//m_wndPartySkill.Create( WBS_CHILD | WBS_NOFRAME | WBS_NODRAWFRAME , rect, pWndTabCtrl, APP_PARTY_SKILL );
	WTCITEM tabTabItem;

	m_wndFriend.Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pWndTabCtrl, 11 );
	m_wndFriend.AddWndStyle( WBS_NODRAWFRAME );
//	m_wndFriend.InitItem( g_pPlayer->m_nJob, g_pPlayer->m_nJob, g_pPlayer->m_aJobSkill );
#ifdef __RT_1025
	if( g_WndMng.m_RTMessenger.size() )
#else	// __RT_1025
	if( g_WndMng.m_Messenger.m_aFriend.size() )
#endif	// __RT_1025
	{
#if __VER < 11 // __SYS_PLAYER_DATA
		g_DPlay.SendGetFriendName();
#endif	// __SYS_PLAYER_DATA
		g_DPlay.SendGetFriendState();
	}
	m_wndParty.Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pWndTabCtrl, 12 );
	m_wndParty.AddWndStyle( WBS_NODRAWFRAME );
//	m_wndParty.InitItem( g_pPlayer->m_nJob, g_pPlayer->m_nJob, g_pPlayer->m_aJobSkill );

	m_wndGuild.Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pWndTabCtrl, 13 );
	m_wndGuild.AddWndStyle( WBS_NODRAWFRAME );
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_APP_COMMUNITY_FRIEND);//"친구";
	tabTabItem.pWndBase = &m_wndFriend;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_APP_PARTY);//"극단";
	tabTabItem.pWndBase = &m_wndParty;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_APP_COMPANY);//"극단";;	// GetText 추가해야함
	tabTabItem.pWndBase = &m_wndGuild;
	pWndTabCtrl->InsertItem( 2, &tabTabItem );

	m_menuState.CreateMenu( this );	
	m_menuState.AppendMenu( 0, FRS_ONLINE   , prj.GetText( TID_FRS_ONLINE   ) );
	m_menuState.AppendMenu( 0, FRS_OFFLINE  , prj.GetText( TID_FRS_OFFLINE  ) );
	m_menuState.AppendMenu( 0, FRS_ABSENT   , prj.GetText( TID_FRS_ABSENT   ) );
	m_menuState.AppendMenu( 0, FRS_HARDPLAY , prj.GetText( TID_FRS_HARDPLAY ) );
	m_menuState.AppendMenu( 0, FRS_EAT      , prj.GetText( TID_FRS_EAT      ) );
	m_menuState.AppendMenu( 0, FRS_REST     , prj.GetText( TID_FRS_REST     ) );
	m_menuState.AppendMenu( 0, FRS_MOVE     , prj.GetText( TID_FRS_MOVE     ) );
	
#ifdef __AEGON_THEME_SWITCHER
	CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchLoadTexture(&m_TexMail, m_pApp->m_pd3dDevice, "WndMail.dds", COLOR_ITEM_BG, FALSE);
#else // __AEGON_THEME_SWITCHER
	m_TexMail.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndMail.dds" ), COLOR_ITEM_BG );
#endif // __AEGON_THEME_SWITCHER
	m_nFlashCounter = 0;
	m_nSwitch       = 1;

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), rectRoot.bottom - rectWindow.Height() );
	Move( point );
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndMessenger::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	if( g_eLocal.GetState( EVE_SCHOOL ) )
		return FALSE;
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MESSENGER_, WBS_THICKFRAME, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndMessenger::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndMessenger::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndMessenger::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndMessenger::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_ADD )
	{
		if( !g_WndMng.m_pWndAddFriend )
		{
			g_WndMng.m_pWndAddFriend = new CWndAddFriend;
			g_WndMng.m_pWndAddFriend->Initialize();
		}
	}
	else
	if( nID == WIDC_FIND )
	{
		g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0027) ) );
	}
	else
	if( nID == WIDC_BUTTON1 )
	{
		CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
		CRect rect = pWndButton->GetWindowRect( TRUE );
		ClientToScreen( &rect );
		rect.OffsetRect( CPoint( 0, rect.Height() ) );
		m_menuState.Move( rect.TopLeft() );
		m_menuState.SetVisible( TRUE );
		m_menuState.SetFocus();
	}
	else
	if( nID == WIDC_BUTTON2 )
	{
		if( !g_WndMng.m_pWndMessengerNote )
		{
			g_WndMng.m_pWndMessengerNote = new CWndMessengerNote;
			g_WndMng.m_pWndMessengerNote->Initialize();  
		}
	}
	else
	{
		if( FRS_ONLINE <= nID && nID < MAX_FRIENDSTAT )
		{
			// 내 상태가 바뀌었따~ 코어로 보내어 모두 알려주자~
			g_DPlay.SendSetState( nID );
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndMessenger::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	AdjustMinRect( &rectWnd, 16 * 11, 16 * 10 );
//	AdjustMaxRect( &rectWnd, 16 * 12, 16 * 12 );
	CWndNeuz::SetWndRect( rectWnd, bOnSize );
}

void CWndMessenger::OnSize(UINT nType, int cx, int cy)
{
	CRect rect = GetClientRect();//GetWndRect();
	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CWndButton* pAdd = (CWndButton*)GetDlgItem( WIDC_ADD );
	CWndButton* pFind = (CWndButton*)GetDlgItem( WIDC_FIND );
	CWndButton* pTag = (CWndButton*)GetDlgItem( WIDC_BUTTON2 );

	rect.top += 32;
	rect.left += 4;
	rect.bottom -= 20;
	pTabCtrl->SetWndRect( rect );
	pAdd->Move( rect.left + 5, rect.bottom + 2 );
	pFind->Move( rect.left + 25, rect.bottom + 2 );
	pTag->Move( rect.left + 45, rect.bottom + 2 );

	m_wndFriend.ScrollBarPos( 0 );
	m_wndParty.ScrollBarPos( 0 );
	m_wndGuild.ScrollBarPos( 0 );

	CWndNeuz::OnSize( nType, cx, cy );
}

#if __VER >= 11 // __CSC_VER11_4
//////////////////////////////////////////////////////////////////////////
// New Messenger Window
//////////////////////////////////////////////////////////////////////////

CWndMessengerEx::CWndMessengerEx() 
{ 
}

CWndMessengerEx::~CWndMessengerEx() 
{ 
	m_TexMail.DeleteDeviceObjects();
}

void CWndMessengerEx::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	CWndTabCtrl* lpTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	if( ar.IsLoading() )
	{
		int nCurSel;
		ar >> nCurSel;
		lpTabCtrl->SetCurSel( nCurSel );
	}
	else
	{
		ar << lpTabCtrl->GetCurSel();
	}
}

void CWndMessengerEx::OnDraw( C2DRender* p2DRender ) 
{ 

#ifdef __AEGON_FIXES
	if (IsInvalidObj(g_pPlayer))
		return;
#endif

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );

	// Draw Status Icon
#ifdef __RT_1025
	DWORD dwMyState;
	if( g_WndMng.m_RTMessenger.GetState() == FRS_AUTOABSENT )
		dwMyState	= FRS_ABSENT;
	else if( g_WndMng.m_RTMessenger.GetState() == FRS_ONLINE )
		dwMyState	= 2;
	else if( g_WndMng.m_RTMessenger.GetState() == FRS_OFFLINE )
		dwMyState	= 8;
	else
		dwMyState	= g_WndMng.m_RTMessenger.GetState();
#else	// __RT_1025
	DWORD dwMyState;
	if( g_WndMng.m_Messenger.m_dwMyState == FRS_AUTOABSENT )
		dwMyState = FRS_ABSENT;
	else if( g_WndMng.m_Messenger.m_dwMyState == FRS_ONLINE )
		dwMyState = 2;
	else if( g_WndMng.m_Messenger.m_dwMyState == FRS_OFFLINE )
		dwMyState = 8;
	else
		dwMyState = g_WndMng.m_Messenger.m_dwMyState;
#endif	// __RT_1025
	
	TEXTUREVERTEX2 vertex[ 6 ];
	TEXTUREVERTEX2* pVertices = vertex;
	
	
	
#ifdef __AEGON_THEME_SWITCHER
	int nThemeType = CAegonThemeSwitcherMng::GetInstance()->GetThemeType(g_Option.m_nSelectedThemeID);
	switch (nThemeType) 
	{
	case 1:
		pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(9, 14), dwMyState - 2, &pVertices, 0xffffffff);
		break;
	default:
		pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(8, 13), dwMyState - 2, &pVertices, 0xffffffff);
		break;
	}
	
#else // __AEGON_THEME_SWITCHER
	pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(8, 8), dwMyState - 2, &pVertices, 0xffffffff);
#endif // __AEGON_THEME_SWITCHER
#ifdef _WIN64
	pWndWorld->m_texPlayerDataIcon.Render(m_pApp->m_pd3dDevice, vertex, (int)(((__int64)pVertices - (__int64)vertex) / sizeof(TEXTUREVERTEX2)));
#else
	pWndWorld->m_texPlayerDataIcon.Render(m_pApp->m_pd3dDevice, vertex, ((int)pVertices - (int)vertex) / sizeof(TEXTUREVERTEX2));
#endif
	
	// Draw Name & Status
	CString strState;
#ifdef __RT_1025
	switch( g_WndMng.m_RTMessenger.GetState() )
#else	// __RT_1025
	switch(g_WndMng.m_Messenger.m_dwMyState)
#endif	// __RT_1025
	{
		case FRS_ONLINE:
			strState.Format( "(%s)", prj.GetText( TID_FRS_ONLINE_STATUS ) );
			break;
		case FRS_OFFLINE:
			strState.Format( "(%s)", prj.GetText( TID_FRS_OFFLINE_STATUS ) );
			break;
		case FRS_ABSENT:
			strState.Format( "(%s)", prj.GetText( TID_FRS_ABSENT ) );
			break;
		case FRS_HARDPLAY:
			strState.Format( "(%s)", prj.GetText( TID_FRS_HARDPLAY ) );
			break;
		case FRS_EAT:
			strState.Format( "(%s)", prj.GetText( TID_FRS_EAT ) );
			break;
		case FRS_REST:
			strState.Format( "(%s)", prj.GetText( TID_FRS_REST ) );
			break;
		case FRS_MOVE:
			strState.Format( "(%s)", prj.GetText( TID_FRS_MOVE ) );
			break;
	}
#ifdef __AEGON_FIXES
	LPCTSTR pszName = "name";
	if(g_pPlayer)
		pszName = g_pPlayer->GetName(TRUE);
#else
	LPCTSTR pszName = g_pPlayer->GetName( TRUE );
#endif
	p2DRender->TextOut( 47, 10, 1, 1, pszName, 0xff606060 );
	p2DRender->TextOut( 48, 10, 1, 1, pszName, 0xff606060 );
	p2DRender->TextOut( 46, 27, 1, 1, strState, 0xff606060 );

	if( g_pPlayer && g_pPlayer->IsMode(MODE_MAILBOX) )
	{
		LPWNDCTRL lpMail = GetWndCtrl( WIDC_CUSTOM1 );
		p2DRender->RenderTexture( CPoint(lpMail->rect.left, lpMail->rect.top), &m_TexMail, m_nFlashCounter );
	}

	if(g_Neuz.m_nTagCount > 0 && g_Neuz.m_bCheckTag == FALSE)
	{
		LPWNDCTRL lpTagButton = GetWndCtrl( WIDC_BUTTON2 );
		p2DRender->RenderTexture( CPoint(lpTagButton->rect.left + 2, lpTagButton->rect.top + 2), &m_TexMail, m_nFlashCounter );
	}

#if __VER >= 15 // __CAMPUS
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	if( pWndTabCtrl == NULL )
		return;

#ifdef __PARTYLIST_MESSENGER
	if (pWndTabCtrl->GetCurSel() == 1)
	{
		CString strPartyName;
		if (g_Party.m_nSizeofMember >= 2)
		{
			if (g_Party.m_nKindTroup == 0)
			{
				strPartyName.Format("%s", prj.GetText(TID_GAME_PARTY1));
			}
			else
			{
				if (0 == strlen(g_Party.m_sParty))
					strPartyName.Format("%s", prj.GetText(TID_GAME_PARTY2));
				else
					strPartyName.Format("%s", g_Party.m_sParty);
			}
			p2DRender->TextOut(180, 12, 1, 1, strPartyName, 0xff606060);

			CString strPartyShareExp;
			if (g_Party.m_nTroupsShareExp == 0)
				strPartyShareExp.Format(prj.GetText(TID_MESSENGER_EXP_T1));
			else
				strPartyShareExp.Format(prj.GetText(TID_MESSENGER_EXP_T2));
			p2DRender->TextOut(180, 24, 1, 1, strPartyShareExp, 0xff606060);
		}
		else
		{
			strPartyName.Format("You're not in a party.");
			p2DRender->TextOut(178, 18, 1, 1, strPartyName, 0xff606060);
		}
	}
#ifdef __MESSENGER_CTRL_EXTENTED
	else if (pWndTabCtrl->GetCurSel() == 2)
	{
		CGuild* pGuild = g_pPlayer->GetGuild();
		CString strGuildName;
		if (pGuild)
		{
			strGuildName.Format("%s", pGuild->m_szGuild);
			if (strGuildName.GetLength() > 16)
			{
				int	nReduceCount = 0;

				for (nReduceCount = 0; nReduceCount < 16; )
				{
					if (IsDBCSLeadByte(strGuildName[nReduceCount]))
						nReduceCount += 2;
					else
						++nReduceCount;
				}
				strGuildName = strGuildName.Left(nReduceCount);
				strGuildName += "...";
			}
			else if (strGuildName.GetLength() == 0)
				strGuildName.Format(prj.GetText(TID_APP_COMPANY));
			p2DRender->TextOut(180, 12, 1, 1, strGuildName, 0xff606060);

			CString strGuildMember = _T("");
			strGuildMember.Format(prj.GetText(TID_MESSENGER_MEMBERS), pGuild->GetSize(), CGuildTable::GetInstance().GetMaxMemeber(pGuild->m_nLevel), pGuild->GetSize() == 1 ? "" : "s");
			p2DRender->TextOut(180, 24, 1, 1, strGuildMember, 0xff606060);
		}
		else
		{
			strGuildName.Format("You're not in a guild.");
			p2DRender->TextOut(178, 18, 1, 1, strGuildName, 0xff606060);
		}
	}

#ifdef __IGNORE_LIST_OPT
	else if (pWndTabCtrl->GetCurSel() == 3)
	{
		CString strCampusPoint;
		strCampusPoint.Format("/ignore & /unignore");
		p2DRender->TextOut(180, 18, 1, 1, strCampusPoint, 0xff606060);
	}
#else //__IGNORE_LIST_OPT
	else if (pWndTabCtrl->GetCurSel() == 3)
	{
		CString strCampusPoint;
		strCampusPoint.Format(prj.GetText(TID_GAME_CAMPUS_POINTS), g_pPlayer->GetCampusPoint());
		p2DRender->TextOut(180, 18, 1, 1, strCampusPoint, 0xff606060);
	}
#endif //__IGNORE_LIST_OPT
#else // __MESSENGER_CTRL_EXTENTED
	if (pWndTabCtrl->GetCurSel() == 2)
	{
		CString strCampusPoint = _T("");
		strCampusPoint.Format(prj.GetText(TID_GAME_CAMPUS_POINTS), g_pPlayer->GetCampusPoint());
		p2DRender->TextOut(190, 20, 1, 1, strCampusPoint, 0xff606060);
	}
#endif // __MESSENGER_CTRL_EXTENTED
#endif // __PARTYLIST_MESSENGER
	else
	{
		CString strServerName = _T( "" );
		strServerName.Format( "%s", g_dpCertified.GetServerName( g_Option.m_nSer ) );
		if( strServerName.GetLength() > 18 ) 
		{
			int	nReduceCount = 0;

			for( nReduceCount = 0; nReduceCount < 18; )
			{
				if( IsDBCSLeadByte( strServerName[ nReduceCount ] ) )
					nReduceCount += 2;
				else
					++nReduceCount;
			}
			strServerName = strServerName.Left( nReduceCount );
			strServerName += "...";
		}
#ifdef __AEGON_QOL_STUFF
		CWndStatic* pWndTextField = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
		if (pWndTextField)
		{
			CSize textExtent = p2DRender->m_pFont->GetTextExtent(strServerName);
			p2DRender->TextOut(pWndTextField->GetWndRect().CenterPoint().x - textExtent.cx / 2, pWndTextField->GetWndRect().CenterPoint().y - textExtent.cy + 2, strServerName.GetString(), 0xff606060);
		}
		else
			p2DRender->TextOut(180, 12, strServerName.GetString(), 0xff606060);
#else // __AEGON_QOL_STUFF
		p2DRender->TextOut(180, 12, 1, 1, strServerName, 0xff606060);
#endif // __AEGON_QOL_STUFF

		//채널 정보
		LPSERVER_DESC pServerDesc = NULL;
		int nCount = 0;
		for( int j = 0; j < (int)( g_dpCertified.m_dwSizeofServerset ); ++j )
		{
			if( g_dpCertified.m_aServerset[ j ].dwParent == NULL_ID )
			{
				if(nCount++ == g_Option.m_nSer)
					pServerDesc = g_dpCertified.m_aServerset + j;
			}
			if( g_dpCertified.m_aServerset[ j ].dwParent != NULL_ID && g_dpCertified.m_aServerset[ j ].lEnable != 0L )
			{
				if( pServerDesc != NULL && g_dpCertified.m_aServerset[ j ].dwParent == pServerDesc->dwID )
				{
					strServerName.Format( "%s", g_dpCertified.m_aServerset[ j + g_Option.m_nMSer ].lpName );
					if( strServerName.GetLength() > 18 ) 
					{
						int	nReduceCount = 0;

						for( nReduceCount = 0; nReduceCount < 18; )
						{
							if( IsDBCSLeadByte( strServerName[ nReduceCount ] ) )
								nReduceCount += 2;
							else
								++nReduceCount;
						}
						strServerName = strServerName.Left( nReduceCount );
						strServerName += "...";
					}
#ifdef __AEGON_QOL_STUFF
					if (pWndTextField)
					{
						CSize textExtent = p2DRender->m_pFont->GetTextExtent(strServerName);
						p2DRender->TextOut(pWndTextField->GetWndRect().CenterPoint().x - textExtent.cx / 2, pWndTextField->GetWndRect().CenterPoint().y + 2, strServerName, 0xff606060);
					}
					else
						p2DRender->TextOut(180, 24, strServerName, 0xff606060);
#else // __AEGON_QOL_STUFF
					p2DRender->TextOut(180, 24, 1, 1, strServerName, 0xff606060);
#endif // __AEGON_QOL_STUFF
					j = g_dpCertified.m_dwSizeofServerset;
				}
			}
		}
	}
#else // __CAMPUS
	//서버 정보
	CString strServerName;
	strServerName.Format( "%s", g_dpCertified.GetServerName(g_Option.m_nSer) );
	if( strServerName.GetLength() > 18 ) 
	{
		int	nReduceCount = 0;

		for( nReduceCount=0; nReduceCount<18; )
		{
			if( IsDBCSLeadByte( strServerName[ nReduceCount ] ) )
				nReduceCount+=2;
			else
				nReduceCount++;
		}
		strServerName = strServerName.Left( nReduceCount );
		strServerName += "...";
	}
	p2DRender->TextOut( 170, 13, 1, 1, strServerName, 0xff606060 );

	//채널 정보
	LPSERVER_DESC pServerDesc = NULL;
	int nCount = 0;
	for( int j = 0; j < g_dpCertified.m_dwSizeofServerset; j++ )
	{
		if(g_dpCertified.m_aServerset[j].dwParent == NULL_ID)
		{
			if(nCount++ == g_Option.m_nSer)
				pServerDesc = g_dpCertified.m_aServerset + j;
		}
		if(g_dpCertified.m_aServerset[j].dwParent != NULL_ID && g_dpCertified.m_aServerset[j].lEnable != 0L)
		{
			if(pServerDesc != NULL && g_dpCertified.m_aServerset[j].dwParent == pServerDesc->dwID)
			{
				strServerName.Format( "%s", g_dpCertified.m_aServerset[j+g_Option.m_nMSer].lpName );
				if( strServerName.GetLength() > 18 ) 
				{
					int	nReduceCount = 0;

					for( nRevivalLayer=0; nRevivalLayer<18; )
					{
						if( IsDBCSLeadByte( strServerName[ nRevivalLayer ] ) )
							nRevivalLayer+=2;
						else
							nRevivalLayer++;
					}
					strServerName = strServerName.Left( nRevivalLayer );
					strServerName += "...";
				}
				p2DRender->TextOut( 170, 27, 1, 1, strServerName, 0xff606060 );

				j = g_dpCertified.m_dwSizeofServerset;
			}
		}
	}
#endif // __CAMPUS
} 

BOOL CWndMessengerEx::Process()
{
	if( (IsValidObj(g_pPlayer) && g_pPlayer->IsMode(MODE_MAILBOX)) || (g_Neuz.m_nTagCount > 0 && g_Neuz.m_bCheckTag == FALSE) )
	{
		m_nFlashCounter += (m_nSwitch*8);
		
		if( m_nFlashCounter >= 255 )
		{
			m_nFlashCounter = 255;
			m_nSwitch = -1;
		}
		
		if( m_nFlashCounter <= 50 )
		{
			m_nFlashCounter = 50;
			m_nSwitch = 1;
		}
	}

	if(IsValidObj(g_pPlayer) && g_pPlayer->IsMode(MODE_MAILBOX) )
	{
		LPWNDCTRL lpMail = GetWndCtrl( WIDC_CUSTOM1 );
		lpMail->m_bDisabled = FALSE;

		CRect  rect    = lpMail->rect;
		CPoint point	= GetMousePoint();
		if( PtInRect( lpMail->rect, point ) )
		{
			ClientToScreen( &point );
			ClientToScreen( &rect );
#ifdef _WIN64
			g_toolTip.PutToolTip((ULONG_PTR)this, prj.GetText(TID_MAIL_RECEIVE), rect, point);
#else
			g_toolTip.PutToolTip( (DWORD)this, prj.GetText(TID_MAIL_RECEIVE), rect, point );
#endif
		}		
	}
	return TRUE;
}

void CWndMessengerEx::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );	
	WTCITEM tabTabItem;

	m_wndFriend.Create( CRect( 0, 0, 250, 250 ), pWndTabCtrl, 11 );
	m_wndFriend.AddWndStyle( WBS_NODRAWFRAME );
#ifdef __RT_1025
	if( g_WndMng.m_RTMessenger.size() )
#else	// __RT_1025
	if( g_WndMng.m_Messenger.m_aFriend.size() )
#endif	// __RT_1025
	{
#if __VER < 11 // __SYS_PLAYER_DATA
		g_DPlay.SendGetFriendName();
#endif	// __SYS_PLAYER_DATA
		g_DPlay.SendGetFriendState();
	}

#ifdef __PARTYLIST_MESSENGER
	m_wndParty.Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pWndTabCtrl, 12 );
	m_wndParty.AddWndStyle( WBS_NODRAWFRAME );
#endif // __PARTYLIST_MESSENGER

	m_wndGuild.Create( CRect( 0, 0, 250, 250 ), pWndTabCtrl, 13 );
	m_wndGuild.AddWndStyle( WBS_NODRAWFRAME );

#ifdef __IGNORE_LIST_OPT
	m_pWndChattingBlockingList.Create(WBS_CHILD | WBS_NODRAWFRAME, GetClientRect(), pWndTabCtrl, APP_CHATTING_BLOCKING_LIST);
#else //__IGNORE_LIST_OPT
#if __VER >= 15 // __CAMPUS
	m_WndCampus.Create( WBS_CHILD | WBS_NODRAWFRAME, GetClientRect(), pWndTabCtrl, APP_MESSENGER_TAB_CAMPUS );
#endif // __CAMPUS
#endif //__IGNORE_LIST_OPT

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_APP_COMMUNITY_FRIEND); //"친구"
	tabTabItem.pWndBase = &m_wndFriend;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );
	
#ifdef __PARTYLIST_MESSENGER
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_APP_PARTY);//"극단";
	tabTabItem.pWndBase = &m_wndParty;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_APP_COMPANY); //"극단"
	tabTabItem.pWndBase = &m_wndGuild;
	pWndTabCtrl->InsertItem( 2, &tabTabItem );

#ifdef __IGNORE_LIST_OPT
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = _T("Ignore");
	tabTabItem.pWndBase = &m_pWndChattingBlockingList;
	pWndTabCtrl->InsertItem(3, &tabTabItem);
#else //__IGNORE_LIST_OPT
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = _T("Mentor");
	tabTabItem.pWndBase = &m_WndCampus;
	pWndTabCtrl->InsertItem( 3, &tabTabItem );
#endif //__IGNORE_LIST_OPT
#else // __PARTYLIST_MESSENGER
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_APP_COMPANY); //"극단"
	tabTabItem.pWndBase = &m_wndGuild;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );

#if __VER >= 15 // __CAMPUS
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_APP_COMMUNITY_CAMPUS); //"사제"
	tabTabItem.pWndBase = &m_WndCampus;
	pWndTabCtrl->InsertItem( 2, &tabTabItem );
#endif // __CAMPUS
#endif // __PARTYLIST_MESSENGER
	m_wndFriend.ScrollBarPos( 0 );
	m_wndGuild.ScrollBarPos( 0 );

	m_menuState.CreateMenu( this );	
	m_menuState.AppendMenu( 0, FRS_ONLINE   , prj.GetText( TID_FRS_ONLINE_STATUS   ) );
	m_menuState.AppendMenu( 0, FRS_ABSENT   , prj.GetText( TID_FRS_ABSENT   ) );
	m_menuState.AppendMenu( 0, FRS_HARDPLAY , prj.GetText( TID_FRS_HARDPLAY ) );
	m_menuState.AppendMenu( 0, FRS_EAT      , prj.GetText( TID_FRS_EAT      ) );
	m_menuState.AppendMenu( 0, FRS_REST     , prj.GetText( TID_FRS_REST     ) );
	m_menuState.AppendMenu( 0, FRS_MOVE     , prj.GetText( TID_FRS_MOVE     ) );
	m_menuState.AppendMenu( 0, FRS_OFFLINE  , prj.GetText( TID_FRS_OFFLINE_STATUS  ) );
	
#ifdef __AEGON_THEME_SWITCHER
	CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchLoadTexture(&m_TexMail, m_pApp->m_pd3dDevice, "WndMail.dds", COLOR_ITEM_BG, FALSE);
#else // __AEGON_THEME_SWITCHER
	m_TexMail.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndMail.dds" ), COLOR_ITEM_BG );
#endif // __AEGON_THEME_SWITCHER
	m_nFlashCounter = 0;
	m_nSwitch       = 1;
	
	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_CHANNEL);
	if(pWndButton)
	{
#ifdef __AEGON_THEME_SWITCHER
		CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchSetTexture((CWndBase*)this, m_pApp->m_pd3dDevice, "ButtMessengerChannel.bmp", TRUE );
#else // __AEGON_THEME_SWITCHER
		pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtMessengerChannel.bmp" ), COLOR_ITEM_BG );
#endif // __AEGON_THEME_SWITCHER
	}

	pWndButton = (CWndButton*)GetDlgItem(WIDC_STATE);
	if(pWndButton)
	{
#ifdef __AEGON_THEME_SWITCHER
		CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchSetTexture((CWndBase*)this, m_pApp->m_pd3dDevice, "ButtMessengerState.bmp", TRUE);
#else // __AEGON_THEME_SWITCHER
		pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtMessengerState.bmp" ), COLOR_ITEM_BG );
#endif // __AEGON_THEME_SWITCHER
	}

	pWndButton = (CWndButton*)GetDlgItem(WIDC_LEVEL);
	if(pWndButton)
	{
#ifdef __AEGON_THEME_SWITCHER
		CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchSetTexture((CWndBase*)this, m_pApp->m_pd3dDevice, "ButtMessengerLevel.bmp", TRUE);
#else // __AEGON_THEME_SWITCHER
		pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtMessengerLevel.bmp" ), COLOR_ITEM_BG );
#endif // __AEGON_THEME_SWITCHER
	}

	pWndButton = (CWndButton*)GetDlgItem(WIDC_JOB);
	if(pWndButton)
	{
#ifdef __AEGON_THEME_SWITCHER
		CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchSetTexture((CWndBase*)this, m_pApp->m_pd3dDevice, "ButtMessengerJob.bmp", TRUE);
#else // __AEGON_THEME_SWITCHER
		pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtMessengerJob.bmp" ), COLOR_ITEM_BG );
#endif // __AEGON_THEME_SWITCHER
	}

	pWndButton = (CWndButton*)GetDlgItem(WIDC_NAME);
	if(pWndButton)
	{
#ifdef __AEGON_THEME_SWITCHER
		CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchSetTexture((CWndBase*)this, m_pApp->m_pd3dDevice, "ButtMessengerChar.bmp", TRUE);
#else // __AEGON_THEME_SWITCHER
		pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtMessengerChar.bmp" ), COLOR_ITEM_BG );
#endif // __AEGON_THEME_SWITCHER
	}

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), rectRoot.bottom - rectWindow.Height() );
	Move( point );
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndMessengerEx::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	if( g_eLocal.GetState( EVE_SCHOOL ) )
		return FALSE;
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MESSENGER_EX, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndMessengerEx::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

void CWndMessengerEx::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
}

void CWndMessengerEx::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
}

BOOL CWndMessengerEx::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_ADD )
	{
		if( !g_WndMng.m_pWndAddFriend )
		{
			g_WndMng.m_pWndAddFriend = new CWndAddFriend;
			g_WndMng.m_pWndAddFriend->Initialize();
		}
	}
	else if( nID == WIDC_BUTTON1 )
	{
		CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
		CRect rect = pWndButton->GetWindowRect( TRUE );
		ClientToScreen( &rect );
		rect.OffsetRect( CPoint( 0, rect.Height() ) );
		m_menuState.Move( rect.TopLeft() );
		m_menuState.SetVisible( TRUE );
		m_menuState.SetFocus();
	}
	else if( nID == WIDC_BUTTON2 )
	{
#ifdef __AEGON_MESSENGER
		g_WndMng.OpenMessenger(FALSE);
#else
		if( !g_WndMng.m_pWndMessengerNote )
		{
			g_WndMng.m_pWndMessengerNote = new CWndMessengerNote;
			g_WndMng.m_pWndMessengerNote->Initialize();
			g_Neuz.m_bCheckTag = TRUE;
		}
#endif
	}
	else if( nID == WIDC_TABCTRL1 )
	{
		CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
		CWndBase* pChild = pTabCtrl->GetFocusChild();
		if(pChild == &m_wndFriend)
			m_wndFriend.UpdatePlayerList();
		else if(pChild == &m_wndGuild)
			m_wndGuild.UpdatePlayerList();
#ifdef __IGNORE_LIST_OPT
		else if (pChild == &m_pWndChattingBlockingList)
			m_pWndChattingBlockingList.UpdateInformation();
#else //__IGNORE_LIST_OPT
#if __VER >= 15 // __CAMPUS
		else if( pChild == &m_WndCampus )
			m_WndCampus.UpdatePlayerList();
#endif // __CAMPUS
#endif //__IGNORE_LIST_OPT
	}
	else
	{
		if( FRS_ONLINE <= nID && nID < MAX_FRIENDSTAT )
		{
			// 내 상태가 바뀌었따~ 코어로 보내어 모두 알려주자~
			g_DPlay.SendSetState( nID );
		}
	}

	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CWndBase* pChild = pTabCtrl->GetFocusChild();
	int nFocusChild = 0;
	if(pChild == &m_wndFriend)
		nFocusChild = 1;
	
#ifdef __PARTYLIST_MESSENGER
	else if(pChild == &m_wndParty)
		nFocusChild = 2;
		
	else if(pChild == &m_wndGuild)
		nFocusChild = 3;
		
#ifdef __IGNORE_LIST_OPT
	else if (pChild == &m_pWndChattingBlockingList)
		nFocusChild = 4;
#else //__IGNORE_LIST_OPT
	else if( pChild == &m_WndCampus )
		nFocusChild = 4;
#endif //__IGNORE_LIST_OPT
#else // __PARTYLIST_MESSENGER
	else if(pChild == &m_wndGuild)
		nFocusChild = 2;
#if __VER >= 15 // __CAMPUS
	else if( pChild == &m_WndCampus )
		nFocusChild = 3;
#endif // __CAMPUS
#endif // __PARTYLIST_MESSENGER

	switch(nID)
	{
		case WIDC_CHANNEL:
			{
				if(nFocusChild == 1)
					m_wndFriend.SortbyChannel();
				else if(nFocusChild == 2)
					m_wndGuild.SortbyChannel();
#ifndef __IGNORE_LIST_OPT
#if __VER >= 15 // __CAMPUS
				else if( nFocusChild == 3 )
					m_WndCampus.SortbyChannel();
#endif // __CAMPUS
#endif //__IGNORE_LIST_OPT
			}
			break;
		case WIDC_STATE:
			{
				if(nFocusChild == 1)
					m_wndFriend.SortbyStatus();
				else if(nFocusChild == 2)
					m_wndGuild.SortbyStatus();
#ifndef __IGNORE_LIST_OPT
#if __VER >= 15 // __CAMPUS
				else if( nFocusChild == 3 )
					m_WndCampus.SortbyStatus();
#endif // __CAMPUS
#endif //__IGNORE_LIST_OPT
			}
			break;
		case WIDC_LEVEL:
			{
				if(nFocusChild == 1)
					m_wndFriend.SortbyLevel();
				else if(nFocusChild == 2)
					m_wndGuild.SortbyLevel();
#ifndef __IGNORE_LIST_OPT
#if __VER >= 15 // __CAMPUS
				else if( nFocusChild == 3 )
					m_WndCampus.SortbyLevel();
#endif // __CAMPUS
#endif //__IGNORE_LIST_OPT
			}
			break;
		case WIDC_JOB:
			{
				if(nFocusChild == 1)
					m_wndFriend.SortbyJob();
				else if(nFocusChild == 2)
					m_wndGuild.SortbyJob();
#ifndef __IGNORE_LIST_OPT
#if __VER >= 15 // __CAMPUS
				else if( nFocusChild == 3 )
					m_WndCampus.SortbyJob();
#endif // __CAMPUS
#endif //__IGNORE_LIST_OPT
			}
			break;
		case WIDC_NAME:
			{
				if(nFocusChild == 1)
					m_wndFriend.SortbyName();
				else if(nFocusChild == 2)
					m_wndGuild.SortbyName();
#ifndef __IGNORE_LIST_OPT
#if __VER >= 15 // __CAMPUS
				else if( nFocusChild == 3 )
					m_WndCampus.SortbyName();
#endif // __CAMPUS
#endif //__IGNORE_LIST_OPT
			}
			break;
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
/*
void CWndMessengerEx::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	AdjustMinRect( &rectWnd, 352, 368 );
	CWndNeuz::SetWndRect( rectWnd, bOnSize );
}

void CWndMessengerEx::OnSize(UINT nType, int cx, int cy)
{
	CRect rect = GetClientRect();
	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CWndButton* pAdd = (CWndButton*)GetDlgItem( WIDC_ADD );
	CWndButton* pTag = (CWndButton*)GetDlgItem( WIDC_BUTTON2 );
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_CUSTOM1 );

	rect.top += 78;
	rect.left += 8;
	rect.bottom -= 22;
	rect.right -= 8;
	
	pTabCtrl->SetWndRect( rect );
	pAdd->Move( rect.left, rect.bottom + 3 );
	pTag->Move( rect.left + 22, rect.bottom + 3 );
	wndCtrl->rect.left = rect.left + 49;
	wndCtrl->rect.right = wndCtrl->rect.left + 12;
	wndCtrl->rect.top = rect.bottom + 3;
	wndCtrl->rect.bottom = wndCtrl->rect.top + 12;

	m_wndFriend.ScrollBarPos( 0 );
	m_wndGuild.ScrollBarPos( 0 );

	CWndNeuz::OnSize( nType, cx, cy );
}
*/
void CWndMessengerEx::UpdateFriendList()
{
	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CWndBase* pChild = pTabCtrl->GetFocusChild();
	if(pChild == &m_wndFriend)
		m_wndFriend.UpdatePlayerList();
}

void CWndMessengerEx::UpdateGuildMemberList()
{
	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CWndBase* pChild = pTabCtrl->GetFocusChild();
	if(pChild == &m_wndGuild)
		m_wndGuild.UpdatePlayerList();
}

#if __VER >= 15 // __CAMPUS
void CWndMessengerEx::UpdateCampusMemberList()
{
	CWndTabCtrl* pTabCtrl = ( CWndTabCtrl* )GetDlgItem( WIDC_TABCTRL1 );
	CWndBase* pChild = pTabCtrl->GetFocusChild();
	if( pChild == &m_WndCampus )
		m_WndCampus.UpdatePlayerList();
}
#endif // __CAMPUS
#ifdef __IGNORE_LIST_OPT
void CWndMessengerEx::UpdateIgnoreList()
{
	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	CWndBase* pChild = pTabCtrl->GetFocusChild();
	if (pChild == &m_pWndChattingBlockingList)
		m_pWndChattingBlockingList.UpdateInformation();
}
#endif //__IGNORE_LIST_OPT
#endif //__CSC_VER11_4

CWndInstantMsg::CWndInstantMsg() 
{ 
} 
CWndInstantMsg::~CWndInstantMsg() 
{ 
} 
void CWndInstantMsg::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_timer.IsTimeOut() )
	{
		Destroy();
	}
} 
void CWndInstantMsg::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	m_timer.Set( SEC( 10 ) ); // 열리거나, 매시지를 받은 이후 10초 후에 사라진다.

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), rectRoot.bottom  - rectWindow.Height() );
	Move( point );
	//MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndInstantMsg::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_INSTANTMSG, WBS_NOFOCUS, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndInstantMsg::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndInstantMsg::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndInstantMsg::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndInstantMsg::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	if( m_strPlayer == "Mail" || m_strPlayer == "Event" )
		return;

#ifdef __AEGON_MESSENGER
	CWndAegonMessenger* pWndMessenger = (CWndAegonMessenger*)g_WndMng.GetApplet(APP_AEGON_MESSENGER);
	if (!pWndMessenger)
	{
		pWndMessenger = (CWndAegonMessenger*)g_WndMng.CreateApplet(APP_AEGON_MESSENGER);
	}
	if (pWndMessenger && IsValidObj(g_pPlayer))
		pWndMessenger->OnChatMessage(m_strPlayer, g_pPlayer->GetName(), m_strMessage.GetString());
#else
	CWndMessage* pWndMessage = g_WndMng.OpenMessage( m_strPlayer );
	pWndMessage->AddMessage( m_strPlayer, m_strMessage );
#endif
	Destroy();
} 
void CWndInstantMsg::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndInstantMsg::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID != WTBID_CLOSE )
	{
		if( m_strPlayer == "Event" )
		{
			//g_WndMng.ObjectExecutor( SHORTCUT_APPLET, APP_WEBBOX2 );
			Destroy();
		}
		else
		if( m_strPlayer != "Mail" )
		{
#ifdef __AEGON_MESSENGER
			g_WndMng.OpenMessage(m_strPlayer);
#else
			CWndMessage* pWndMessage = g_WndMng.OpenMessage( m_strPlayer );
			pWndMessage->AddMessage( m_strPlayer, m_strMessage );
#endif
			Destroy();
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndInstantMsg::AddMessage( LPCTSTR lpszFrom, LPCTSTR lpszMessage )
{
#ifdef __fmod
	SoundSystemClass::playSound(SND_INF_MESSENGERRING);
#else
	PLAYSND( SND_INF_MESSENGERRING );
#endif
	
#ifdef __AEGON_THEME_SWITCHER
	DWORD dwColorHighlightBlue = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE);
#endif //__AEGON_THEME_SWITCHER

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	CString strMessage;
	m_strMessage = lpszMessage;
#ifdef __AEGON_THEME_SWITCHER
	strMessage.Format( "%s%s :\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage );
#else //__AEGON_THEME_SWITCHER
	strMessage.Format("#cff0000ff%s%s :#nc\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage);
#endif //__AEGON_THEME_SWITCHER

#ifdef __AEGON_THEME_SWITCHER
	pWndText->AddString(strMessage, dwColorHighlightBlue);
#else //__AEGON_THEME_SWITCHER
	pWndText->AddString(strMessage);
#endif //__AEGON_THEME_SWITCHER
	m_timer.Reset();
}

void CWndInstantMsg::AddMessageJoin( LPCTSTR lpszJoinName )
{
#ifdef __fmod
	SoundSystemClass::playSound(SND_INF_MESSENGERRING);
#else
	PLAYSND( SND_INF_MESSENGERRING );
#endif
	
#ifdef __AEGON_THEME_SWITCHER
	DWORD dwColorHighlightBlue = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE);
#endif //__AEGON_THEME_SWITCHER

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	CString strMessage;
#ifdef __AEGON_THEME_SWITCHER
	strMessage.Format( "%s %s\n", lpszJoinName, prj.GetText(TID_GAME_LOGIN));
#else //__AEGON_THEME_SWITCHER
	strMessage.Format("#cff0000ff%s %s\n", lpszJoinName, prj.GetText(TID_GAME_LOGIN));
#endif //__AEGON_THEME_SWITCHER

#ifdef __AEGON_THEME_SWITCHER
	pWndText->AddString(strMessage, dwColorHighlightBlue);
#else //__AEGON_THEME_SWITCHER
	pWndText->AddString( strMessage );
#endif //__AEGON_THEME_SWITCHER
	m_timer.Reset();
}

void CWndInstantMsg::AddPostMessage( LPCTSTR lpszSendName )
{
#ifdef __fmod
	SoundSystemClass::playSound(SND_INF_MESSENGERRING);
#else
	PLAYSND( SND_INF_MESSENGERRING );
#endif
	
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	pWndText->AddString( lpszSendName );
	m_timer.Reset();
}
//////////////////////////////////////////////////////////

CWndMessage::CWndMessage() 
{ 
} 
CWndMessage::~CWndMessage() 
{

} 
void CWndMessage::OnDraw( C2DRender* p2DRender ) 
{ 
#ifdef __AEGON_QOL_STUFF
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT);
	if (pWndEdit) 
	{
		CRect wndRect = pWndEdit->GetWndRect();
		int nRange = 0;
		int nPage = 2;

		nRange = pWndEdit->m_string.GetLineCount();

		if (!nRange)
			nRange = 1;

		pWndEdit->m_wndScrollBar.SetScrollRange(0, nRange);
		pWndEdit->m_wndScrollBar.SetScrollPage(nPage);
		pWndEdit->m_wndScrollBar.EnableScrollBar(TRUE);
	}
	
#endif
} 
void CWndMessage::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT );
	pWndEdit->AddWndStyle( EBS_AUTOVSCROLL );
#ifdef __GAMEGUARD
	pWndEdit->AddWndStyle(EBS_AUTOHSCROLL);
#endif // __GAMEGUARD
	pWndEdit->SetNativeMode();

	pWndEdit->SetFocus();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

#ifdef __AEGON_QOL_STUFF
	OnSize(0, rectWindow.Width(), rectWindow.Height());
#endif
} 
void CWndMessage::InitSize( void )
{
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndMessage::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MESSAGE, WBS_THICKFRAME, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndMessage::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndMessage::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	AdjustMinRect( &rectWnd, 16 * 10, 16 * 10 );
//	AdjustMaxRect( &rectWnd, 16 * 12, 16 * 12 );
	CWndNeuz::SetWndRect( rectWnd, bOnSize );
}
void CWndMessage::OnSize( UINT nType, int cx, int cy ) \
{ 
	CRect rect = GetClientRect();//GetWndRect();
	
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT );
	CWndButton* pWndSend = (CWndButton*)GetDlgItem( WIDC_SEND );

	rect.top += 4;
	rect.left += 4;
	rect.right -= 4;
	rect.bottom -= 50;
	pWndText->SetWndRect( rect );

	rect = GetClientRect();//GetWndRect();
	rect.top = rect.bottom - 45;
	rect.left += 4;
	rect.right -= 80;
	rect.bottom -= 4;
	pWndEdit->SetWndRect( rect );
	
	rect = GetClientRect();//GetWndRect();
	rect.top = rect.bottom - 45;
	rect.left = rect.right - 74;
	rect.right -= 4;
	rect.bottom -= 4;
	//pWndSend->SetWndRect( rect );
	pWndSend->Move( rect.TopLeft() );//rect.right + 4, rect.top );

	//pAdd->Move( rect.left + 5, rect.bottom + 2 );
	//pFind->Move( rect.left + 25, rect.bottom + 2 );
	
	//m_wndPlace.Move( CPoint( 0, 0 ) );
	//m_wndZoomIn.Move( CPoint(0, cy - 32 ) );
	//m_wndZoomOut.Move( CPoint(0, cy - 16 ) );
	
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndMessage::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndMessage::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndMessage::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWndEdit* pWndText = (CWndEdit*)GetDlgItem( WIDC_EDIT );
	switch(nID)
	{
		case WIDC_EDIT: // 본문 
			if( message == EN_RETURN)
			{
				if( pWndText->m_string.IsEmpty() == FALSE )
				{
					CString strFormat = pWndText->m_string;					
					pWndText->m_string.GetTextFormat(strFormat);
					CString string;
					string.Format( "/say \"%s\" %s", m_strPlayer.GetString(), strFormat.GetString() );
					
					g_DPlay.SendChat( string );
					pWndText->Empty();
#ifdef __AEGON_QOL_STUFF
					pWndText->m_wndScrollBar.SetScrollPos(0);
#endif
				}
				//				pWndText->ResetString();
			}
#ifdef __AEGON_QOL_STUFF
			else if(message == EN_CHANGE) //auto scrolling edit field
			{
				CPoint ptCaret = pWndText->GetCaretPos();
				DWORD dwLineCount = pWndText->m_string.GetLineCount();
				DWORD dwFontMaxHeight = pWndText->GetFont()->GetMaxHeight();

				DWORD dwCaretLineOffset = pWndText->m_nLineSpace + dwFontMaxHeight;
				int nCaretLine = 0;
				if(dwCaretLineOffset > 0)
					nCaretLine = ptCaret.y / dwCaretLineOffset;

				int nMaxScrollPos = pWndText->m_wndScrollBar.GetMaxScrollPos() - pWndText->m_wndScrollBar.GetScrollPage();
				int nMinScrollPos = pWndText->m_wndScrollBar.GetMinScrollPos();

				if (nCaretLine > dwLineCount)
					nCaretLine = dwLineCount;

				if (nCaretLine >= nMaxScrollPos) 
				{
					pWndText->m_wndScrollBar.SetMaxScrollPos();
				}
				else 
				{
					if (nCaretLine < nMinScrollPos)
						nCaretLine = nMinScrollPos;
					pWndText->m_wndScrollBar.SetScrollPos(nCaretLine);
				}
			}
#endif
			break;
		case WIDC_SEND: // 본문 
			{
				CString str = pWndText->m_string;
				if( str.IsEmpty() == FALSE )
				{
					CString strFormat = pWndText->m_string;

					pWndText->m_string.GetTextFormat(strFormat);

					CString string;
					string.Format( "/say \"%s\" %s", m_strPlayer.GetString(), strFormat.GetString() );					
					
					g_DPlay.SendChat( string );
					pWndText->Empty();
	//				pWndText->ResetString();
#ifdef __AEGON_QOL_STUFF
					pWndText->m_wndScrollBar.SetScrollPos(0);
#endif
				}
			}
			break;

	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndMessage::AddMessage( LPCTSTR lpszFrom, LPCTSTR lpszMessage )
{
	char szPlayerName[MAX_NAME] = "error";
	
	if (IsValidObj(g_pPlayer))
		strcpy(szPlayerName, g_pPlayer->GetName());

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	CString strMessage;

#ifdef __LEESE_MESSENGER_TIMER
	CString timeMessage;

	CTime time = CTime::GetCurrentTime();
	if (!strcmp(lpszFrom, szPlayerName))
		timeMessage = time.Format("#cffff0000[%H:%M]#nc ");
	else
		timeMessage = time.Format("#cff0000ff[%H:%M]#nc ");

	if (!strcmp(lpszFrom, szPlayerName))
		strMessage.Format("#cffff0000%s%s:#nc\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage);
	else
		strMessage.Format("#cff0000ff%s%s:#nc\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage);

	if (g_Option.m_bTimeStamp) {
		pWndText->AddString(timeMessage + strMessage);
	}
	else {
		pWndText->AddString(strMessage);
	}
	pWndText->m_wndScrollBar.SetMaxScrollPos();
#else //__LEESE_MESSENGER_TIMER
	if( !strcmp( lpszFrom, g_pPlayer->GetName() ) )
		strMessage.Format( "#cffff0000%s%s :#nc\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage );
	else
		strMessage.Format( "#cff0000ff%s%s :#nc\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage );
	
	pWndText->AddString( strMessage );
	pWndText->m_wndScrollBar.SetMaxScrollPos();
#endif //__LEESE_MESSENGER_TIMER
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		쪽지창
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/****************************************************
WndId : APP_MESSAGE_NOTE - 쪽지보내기창
CtrlId : WIDC_EDIT1 - 
CtrlId : WIDC_BUTTON1 - Button
CtrlId : WIDC_TEXT1 - 
****************************************************/

CWndMessageNote::CWndMessageNote() 
{ 
} 
CWndMessageNote::~CWndMessageNote() 
{ 
	memset(m_szName, 0, 64);
} 
void CWndMessageNote::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndMessageNote::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
//	m_pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	m_pEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	m_pEdit->AddWndStyle( EBS_WANTRETURN );//| WBS_VSCROLL  );
	m_pEdit->SetNativeMode();
	m_pEdit->SetFocus();
	CString strTitle;
	strTitle.Format( "%s - %s", GetTitle().GetString(), m_szName );
	SetTitle( strTitle );
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

#ifdef __AEGON_MESSENGER
	if (strlen(m_szName) > 0)
		g_WndMng.OpenMessage(m_szName);
	Destroy();
#endif
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndMessageNote::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MESSAGE_NOTE, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
직접 윈도를 열때 사용 
BOOL CWndMessageNote::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
CRect rectWindow = m_pWndRoot->GetWindowRect(); 
CRect rect( 50 ,50, 300, 300 ); 
SetTitle( _T( "title" ) ); 
return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndMessageNote::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndMessageNote::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndMessageNote::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndMessageNote::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndMessageNote::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 )
	{
		LPSTR lpSendMessage = (LPSTR)m_pEdit->GetString();
		int adf = strlen( lpSendMessage );
		if( strlen( lpSendMessage ) <= 255 )
		{
			g_DPlay.SendMessageNote( m_dwUserId, (LPSTR)m_pEdit->GetString() );
		}
		else
		{
			// 에러 메세지 
			g_WndMng.PutString( prj.GetText( TID_GAME_MESSFULLMSG ), NULL, prj.GetTextColor( TID_GAME_MESSFULLMSG ) );
		}
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/****************************************************
WndId : APP_MESSENGER_NOTE - 쪽지창
CtrlId : WIDC_TEXT1 - 
****************************************************/

CWndMessengerNote::CWndMessengerNote() 
{ 
} 
CWndMessengerNote::~CWndMessengerNote() 
{ 
} 
void CWndMessengerNote::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndMessengerNote::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	for( int i = 0 ; i < g_Neuz.m_nTagCount ; i++ )
	{
		CWndText * pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
		pWndText->AddWndStyle( EBS_WANTRETURN );
		CString strMessage;
		strMessage.Format( "%s - %d / %02d / %02d", g_Neuz.m_strTagName[ i ], g_Neuz.m_dwtegDate[ i ] / 10000, ( g_Neuz.m_dwtegDate[ i ] % 10000 ) / 100,  ( g_Neuz.m_dwtegDate[ i ] % 10000 ) % 100 );
		pWndText->AddString( strMessage, 0xff804000 );
		pWndText->AddString( "\n  " );
		pWndText->AddString( g_Neuz.m_strTagMessage[ i ] );
		pWndText->AddString( "\n\n" );
		
//		g_Neuz.m_dwtegDate[ i ]
//		g_Neuz.m_strTagMessage[ i ]
	}
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndMessengerNote::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MESSENGER_NOTE, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
직접 윈도를 열때 사용 
BOOL CWndMessengerNote::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
CRect rectWindow = m_pWndRoot->GetWindowRect(); 
CRect rect( 50 ,50, 300, 300 ); 
SetTitle( _T( "title" ) ); 
return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndMessengerNote::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndMessengerNote::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndMessengerNote::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndMessengerNote::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndMessengerNote::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


