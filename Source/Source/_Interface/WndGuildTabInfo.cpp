#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndGuildTabInfo.h"
#include "WndManager.h"
#include "DPClient.h"
#include "MsgHdr.h"
#include "defineQuest.h"
extern	CDPClient	g_DPlay;

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#include "guild.h"
extern	CGuildMng	g_GuildMng;

#include "eveschool.h"
extern CGuildCombat g_GuildCombatMng;

/****************************************************
  WndId : APP_GUILD_DISMISS
****************************************************/
CWndGuildDisMiss::CWndGuildDisMiss() 
{ 
} 
CWndGuildDisMiss::~CWndGuildDisMiss() 
{ 
} 
void CWndGuildDisMiss::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndGuildDisMiss::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

#ifdef __AEGON_FIXES
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	if (pWndStatic)
	{
		pWndStatic->SetVisible(FALSE);
	}
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	if (pWndEdit) 
	{
		pWndEdit->AddWndStyle(EBS_READONLY);
		if (pWndStatic) 
		{
			pWndEdit->SetString(pWndStatic->GetTitle().GetString(), CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
		}
	}
	
	
#endif
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildDisMiss::Initialize( CWndBase* pWndParent ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_GUILD_DISMISS );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );
	return CWndNeuz::Create( /*WBS_THICKFRAME |*/ WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_GUILD_DISMISS ); 
} 

BOOL CWndGuildDisMiss::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildDisMiss::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildDisMiss::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildDisMiss::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildDisMiss::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case WIDC_YES:
			{
				if( g_GuildCombatMng.m_bRequest )
					g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_GUILDCOMBAT_NOT_DISSOLVE_GUILD ) );	// "수정해야함 : 길드대전에 신청한길드는 길드해체를 할수 없습니다" );
				else
					g_DPlay.SendDestroyGuild( g_pPlayer->m_idPlayer );
				Destroy();
			}
			break;
		case WIDC_NO:
			{
				Destroy();
			}
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 



/****************************************************
  WndId : APP_GUILD_NOTICE
****************************************************/
CWndGuildNotice::CWndGuildNotice() 
{ 
} 
CWndGuildNotice::~CWndGuildNotice() 
{ 
} 
void CWndGuildNotice::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndGuildNotice::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	// 여기에 코딩하세요
	CGuild* pGuild = g_pPlayer->GetGuild();
	if( pGuild )
	{
		CWndEdit* pNotice = (CWndEdit*) GetDlgItem( WIDC_EDIT1 );
		pNotice->AddWndStyle( EBS_WANTRETURN );
		pNotice->AddWndStyle( EBS_AUTOVSCROLL );
		
		LPWNDCTRL pWndCtrl = GetWndCtrl( WIDC_EDIT1 );
		if(pWndCtrl)
			pNotice->SetWndRect(pWndCtrl->rect);

		pNotice->SetString( pGuild->m_szNotice );		// 공지 출력.
	}
		
/*	pNotice->EnableModeChange( FALSE );
	//pNotice->AddWndStyle( WBS_NODRAWFRAME );
	pNotice->SetTabStop( TRUE );
	pNotice->SetString( "공지돠" );
	pNotice->SetFocus(); */
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildNotice::Initialize( CWndBase* pWndParent ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_GUILD_NOTICE );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );
	return CWndNeuz::Create( /*WBS_THICKFRAME |*/ WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_GUILD_NOTICE ); 
} 

BOOL CWndGuildNotice::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildNotice::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildNotice::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildNotice::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildNotice::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWndEdit* pNotice = (CWndEdit*) GetDlgItem( WIDC_EDIT1 );

	switch( nID )
	{
	case WIDC_BUTTON2:		// 공지 개시
		{
			LPCTSTR szNotice = pNotice->GetString();

			if( strlen(szNotice) < MAX_BYTE_NOTICE )
			{
				g_DPlay.SendGuildNotice( szNotice );
				Destroy();		// 창 닫음.
			}
			else
			{
				g_WndMng.OpenMessageBox( prj.GetText( TID_GUILD_NOTICE_ERROR ) );
			}
			break;
		}
	case WIDC_BUTTON1:		// 새공지.
		pNotice->SetString( "" );		// 에디트 박스 내용을 클리어
		break;
	case WIDC_BUTTON3:
		pNotice->SetFocus();			// 내용은 건드리지 않고 포커스만 에디트 박스로 이동.
		break;
	}
		
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


/****************************************************
  WndId : APP_GUILD_SETLOGO
****************************************************/
CWndGuildSetLogo::CWndGuildSetLogo() 
{ 
} 
CWndGuildSetLogo::~CWndGuildSetLogo() 
{ 
} 
void CWndGuildSetLogo::OnDraw( C2DRender* p2DRender ) 
{ 
	{
		if( m_nSelectLogo )
		{
			CRect rect = m_rect[m_nSelectLogo-1];
			
			rect.left -= 1;
			rect.top  -= 1;
			rect.right  += 1;
			rect.bottom += 1;
			
			p2DRender->RenderRect( rect, D3DCOLOR_XRGB( 255, 0, 255) );
			
			rect.left -= 1;
			rect.top  -= 1;
			rect.right  += 1;
			rect.bottom += 1;
			
			p2DRender->RenderRect( rect, D3DCOLOR_XRGB( 255, 0, 255) );
		}
		
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
		
		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
		
		if( pWndWorld )
		{
			for( int i=0; i<CUSTOM_LOGO_MAX - 7; i++ )  // GM Guild Log 출력 막음
			{
				CPoint pt = CPoint( m_rect[i].left, m_rect[i].top );
				p2DRender->RenderTexture( pt, &pWndWorld->m_pTextureLogo[i] );
			}
		}
		
		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		
	}
} 
void CWndGuildSetLogo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	m_nSelectLogo = 0;
	
	{
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );

		if( pWndWorld == NULL )
			return;

		// 출력위치 지정
		CPoint pt;
		int index = 0;
		
		for( int y=0; y<4; y++ )
		{
			for( int x=0; x<5; x++ )
			{
				pt = CPoint( x+24, y+18 );
				
				pt.x += (x*(pWndWorld->m_pTextureLogo[index].m_size.cx+5));
				pt.y += (y*(pWndWorld->m_pTextureLogo[index].m_size.cy+5));
				
				SetRect( m_rect[index], pt.x, pt.y, pt.x+pWndWorld->m_pTextureLogo[index].m_size.cx, pt.y+pWndWorld->m_pTextureLogo[index].m_size.cy );
				index++;
			}
		}

		CGuild* pGuild = g_pPlayer->GetGuild();
		
		if( pGuild )
			m_nSelectLogo = pGuild->m_dwLogo;
	}
	// Check Game Master
	if(!g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
	{
		CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		if(pWndButton)
		{
			pWndButton->SetVisible(FALSE);
			pWndButton->EnableWindow(FALSE);
		}
		CRect rectWindow = GetWindowRect();
		rectWindow.bottom = 208;
		SetWndRect(rectWindow);
	}
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
	
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildSetLogo::Initialize( CWndBase* pWndParent ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_GUILD_SETLOGO );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );

	return CWndNeuz::Create( /*WBS_THICKFRAME |*/ WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_GUILD_SETLOGO ); 
} 

BOOL CWndGuildSetLogo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildSetLogo::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildSetLogo::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	{
		for( int i=0; i<CUSTOM_LOGO_MAX - 7; i++ ) // GM Guild Log 선택 막음
		{
			if( PtInRect( &m_rect[i], point) )
			{
				m_nSelectLogo = i+1;
				return;
			}
		}
	}
} 
void CWndGuildSetLogo::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildSetLogo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case WIDC_OK:
		{
			CGuild* pGuild = g_pPlayer->GetGuild();
			
			if( pGuild )
			{
				g_DPlay.SendGuildLogo( m_nSelectLogo );
			}
			
			Destroy();
			break;
		}
		case WIDC_CANCEL:
		{
			Destroy();
			break;
		}
		case WIDC_BUTTON1:
		{
			CGuild* pGuild = g_pPlayer->GetGuild();
			
			if( pGuild && g_WndMng.m_pWndWorld )
			{
				if(g_WndMng.m_pWndWorld->GetGMLogoIndex() != -1)
					g_DPlay.SendGuildLogo( g_WndMng.m_pWndWorld->GetGMLogoIndex() );
				else
					g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_DONOTHAVE_GMLOGO));
			}
			
			Destroy();
			break;
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 



/****************************************************
  WndId : APP_GUILD_TABINFO
****************************************************/
CWndGuildTabInfo::CWndGuildTabInfo() 
{ 
	m_pwndGuildName = NULL;
	m_pwndGuildNotice = NULL;
	m_pwndGuildSetLogo = NULL;
	m_pwndGuildDisMiss = NULL;
	
#ifdef _DEBUG
	m_nMx = m_nMy = 0;
#endif

} 
CWndGuildTabInfo::~CWndGuildTabInfo() 
{ 
	SAFE_DELETE(m_pwndGuildName);
	SAFE_DELETE(m_pwndGuildNotice);
	SAFE_DELETE(m_pwndGuildSetLogo);
	SAFE_DELETE(m_pwndGuildDisMiss);
} 
void CWndGuildTabInfo::OnDraw( C2DRender* p2DRender ) 
{ 
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	
	CGuild* pGuild = g_pPlayer->GetGuild();

	if( pWndWorld && pGuild )
	{
		if( pGuild->m_dwLogo != 0 )
		{
			LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );

			CPoint point = lpWndCtrl->rect.TopLeft();

			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
			
			pWndWorld->m_pTextureLogo[pGuild->m_dwLogo-1].Render( &g_Neuz.m_2DRender, point, 255 );

			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			
		}
	}
} 

void CWndGuildTabInfo::UpdateData()
{
	CGuild* pGuild = g_pPlayer->GetGuild();
	if( pGuild )
	{
		CString strText, strExpPerc, strGoldPerc;
		CWndBase *pWndText;
		
		pWndText = GetDlgItem( WIDC_GUILDNAME );
		pWndText->SetTitle( pGuild->m_szGuild );
		pWndText = GetDlgItem( WIDC_GUILDLEVEL );
		strText.Format( "%d", pGuild->m_nLevel );
		pWndText->SetTitle( strText );
		pWndText = GetDlgItem( WIDC_GUILDMASTER );
#if __VER >= 11 // __SYS_PLAYER_DATA
		LPCTSTR szMaster	= CPlayerDataCenter::GetInstance()->GetPlayerString( pGuild->m_idMaster );
#else	// __SYS_PLAYER_DATA
		LPCTSTR szMaster = prj.GetPlayerString( pGuild->m_idMaster );
#endif	// __SYS_PLAYER_DATA
		
#if __VER >= 11 // __SYS_PLAYER_DATA
		pWndText->SetTitle( szMaster );
#else	// __SYS_PLAYER_DATA
		if( szMaster )
			pWndText->SetTitle( szMaster );		// 길드 장.
		else
			g_DPlay.SendQueryPlayerString( pGuild->m_idMaster, QPS_GUILD_MASTER);
#endif	// __SYS_PLAYER_DATA

		pWndText = GetDlgItem( WIDC_GUILDNUMBER );
		
		strText.Format( "%d / %d", pGuild->GetSize(), CGuildTable::GetInstance().GetMaxMemeber(pGuild->m_nLevel) );	
		pWndText->SetTitle( strText );
		
		// Static EXP
		pWndText = GetDlgItem(WIDC_GUILDEXPMERIT);
		strText.Format( "%u", pGuild->m_dwContributionPxp);
		pWndText->SetTitle(GetNumberFormatEx(strText));

		// EXP Percentage Tooltip
		float nEPercent = 0.2f;
		float nContri = (float)(pGuild->m_dwContributionPxp);
		float nENext = (float)(CGuildTable::GetInstance().GetPxpCount(pGuild->m_nLevel + 1));
		nEPercent = (float)((nContri / nENext) * 100.0f);
		if (nEPercent > 100.0f)
			nEPercent = 100.0f;
		strExpPerc.Format("Exp Percentage - %.2f%%", nEPercent);
		pWndText->SetToolTip(strExpPerc);

		// Statis Gold
		pWndText = GetDlgItem( WIDC_GUILDPENYAMERIT );
		strText.Format("%u", pGuild->m_nGoldGuild);
		pWndText->SetTitle(GetNumberFormatEx(strText));

		// Gold Percentage Tooltip
		float nGPercent = 0.2f;
		float nGold = (float)(pGuild->m_nGoldGuild);
		float nGNext = (float)(CGuildTable::GetInstance().GetPenya(pGuild->m_nLevel + 1));
		nGPercent = (float)((nGold / nGNext) * 100.0f);
		if (nGPercent > 100.0f)
			nGPercent = 100.0f;
		strGoldPerc.Format("Penya Percentage - %.2f%%", nGPercent);
		pWndText->SetToolTip(strGoldPerc);

		CWndText* pNotice = (CWndText*) GetDlgItem( WIDC_TEXT1 );
		if( pNotice )
			pNotice->SetString( pGuild->m_szNotice );

#ifdef __WBQT_GUILD_BUFFS
		CWndStatic* pBuffs = (CWndStatic*)GetDlgItem(WIDC_STATIC73);
		CString pBuffsAmount;
		if (pBuffs)
		{
			if (pGuild->m_nLevel == 50) {
				pBuffsAmount = "5 / 5";
			}
			else if (pGuild->m_nLevel >= 40) {
				pBuffsAmount = "4 / 5";
			}
			else if (pGuild->m_nLevel >= 30) {
				pBuffsAmount = "3 / 5";
			}
			else if (pGuild->m_nLevel >= 20) {
				pBuffsAmount = "2 / 5";
			}
			else if (pGuild->m_nLevel >= 10) {
				pBuffsAmount = "1 / 5";
			}
			else {
				pBuffsAmount = "0 / 5";
			}
			pBuffs->SetTitle(pBuffsAmount);
		}
#endif //__WBQT_GUILD_BUFFS
	}
	else
	{
		CString strText;
		CWndBase *pWndText;
		
		pWndText = GetDlgItem( WIDC_GUILDNAME );
		pWndText->SetTitle( "" );
		pWndText = GetDlgItem( WIDC_GUILDLEVEL );
		pWndText->SetTitle( "" );
		pWndText = GetDlgItem( WIDC_GUILDMASTER );
		pWndText->SetTitle( "" );
		pWndText = GetDlgItem( WIDC_GUILDNUMBER );
		pWndText->SetTitle( "" );
		pWndText = GetDlgItem( WIDC_GUILDEXPMERIT );
		pWndText->SetTitle( "" );
		pWndText = GetDlgItem( WIDC_GUILDPENYAMERIT );
		pWndText->SetTitle( "" );
		
		CWndText* pNotice = (CWndText*) GetDlgItem( WIDC_TEXT1 );
		if( pNotice )
			pNotice->SetString( "" );

#ifdef __WBQT_GUILD_BUFFS
		CWndStatic* pBuffs = (CWndStatic*)GetDlgItem(WIDC_STATIC73);
		CString pBuffsAmount = "";
		if (pBuffs)
		{
			pBuffs->SetTitle(pBuffsAmount);
		}
#endif //__WBQT_GUILD_BUFFS

	}
#ifdef __GUILDFINDER
	CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_BUTTON12);
	if (pWndCheck)
	{
		pWndCheck->EnableWindow(FALSE);
		pWndCheck->SetCheck(FALSE);
		if (g_pPlayer && g_pPlayer->GetGuild())
		{
			pWndCheck->SetCheck(g_pPlayer->GetGuild()->m_bGuildFinder);
			if (g_pPlayer->GetGuild()->m_idMaster == g_pPlayer->m_idPlayer)
				pWndCheck->EnableWindow(TRUE);
		}
	}
#endif //__GUILDFINDER
}
void CWndGuildTabInfo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	UpdateData();

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildTabInfo::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD_TABINFO, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndGuildTabInfo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildTabInfo::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildTabInfo::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildTabInfo::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildTabInfo::OnMouseMove(UINT nFlags, CPoint point )
{
}


BOOL CWndGuildTabInfo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CGuild* pGuild = g_pPlayer->GetGuild();

	if( pGuild == NULL )
		return FALSE;
		
	if( !pGuild->IsMaster(g_pPlayer->m_idPlayer) )
		return FALSE;

	switch( nID )
	{
	case WIDC_BUTTON4:		// 길드 이름 변경
		{
			if( strlen(pGuild->m_szGuild) == 0 )
			{
#ifdef __AEGON_FULLSCREEN_AND_RESIZE
				if (!g_WndMng.m_pWndGuildName)
				{
					g_WndMng.m_pWndGuildName = new CWndGuildName;
					g_WndMng.m_pWndGuildName->Initialize(&g_WndMng);
				}
				g_WndMng.m_pWndGuildName->SetData();
#else
				CWndGuildName* pWndGuildName = (CWndGuildName*)g_WndMng.GetWndBase(APP_GUILDNAME);
				if (!pWndGuildName)
				{
					pWndGuildName = new CWndGuildName;
					pWndGuildName->Initialize(&g_WndMng);
				}
				pWndGuildName->SetData();
#endif
				
			}
			else
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDNOTCHGNAME ), MB_OK, this );
			
		}
		break;
	case WIDC_BUTTON1:		// 공지
		{
			SAFE_DELETE(m_pwndGuildNotice);
			m_pwndGuildNotice = new CWndGuildNotice;
			m_pwndGuildNotice->Initialize( this );
		}
		break;
	case WIDC_BUTTON2:		// 로고 지정
		{
			if( g_pPlayer->m_idWar )
			{
				// 전쟁중엔 로고 못바꿈다.
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDWARERRORLOGO), MB_OK, this );
				return FALSE;
			}
			if( pGuild->m_dwLogo )
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDSTILLLOGO), MB_OK, this );
				return FALSE;
			}
			if( pGuild->m_nLevel < 2 )
			{
				g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_GUILDNOTLEVEL ), MB_OK, this );
				return FALSE;
			}

			SAFE_DELETE(m_pwndGuildSetLogo);
			m_pwndGuildSetLogo = new CWndGuildSetLogo;
			m_pwndGuildSetLogo->Initialize( this );
		}
		break;
#ifdef __GUILDFINDER
	case WIDC_BUTTON12:
	{
		if (g_pPlayer->GetGuild() && g_pPlayer->GetGuild()->m_idMaster == g_pPlayer->m_idPlayer)
		{
			CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_BUTTON12);
			if (pWndCheck)
				g_DPlay.SendGuildFinder(pWndCheck->GetCheck());
		}
	}
	break;
#endif //__GUILDFINDER
	case WIDC_BUTTON3:		// 길드 뽀개기.
		{
			if( g_WndMng.m_pWndGuildBank )
			{
				return FALSE;
			}
			if( g_pPlayer->m_idWar )
			{
				// 전쟁중엔 길드 못뽀갭니다.
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDWARERRORDISBAND), MB_OK, this );
				return FALSE;
			} else

			if( pGuild->GetQuest( QUEST_WARMON_LV1 ) != NULL && pGuild->GetQuest( QUEST_WARMON_LV1 )->nState == QS_BEGIN )	// 클락워크 퀘스트면 길드 해체 안됨
			{
				QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( QUEST_WARMON_LV1 );
				if( pQuestProp )
				{
					g_WndMng.OpenMessageBox(prj.GetText( TID_GUILD_QUEST_LEAVEERROR ), MB_OK, this );
				}
				return FALSE;
			} 
			else
			{
				SAFE_DELETE(m_pwndGuildDisMiss);
				m_pwndGuildDisMiss = new CWndGuildDisMiss;
				m_pwndGuildDisMiss->Initialize( this );
			}
		}
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
#ifdef __AEGON_FULLSCREEN_AND_RESIZE
void CWndGuildTabInfo::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (m_pwndGuildName == pWndChild)
	{
		SAFE_DELETE(m_pwndGuildName);
		pWndChild = NULL;
	}
	else if (m_pwndGuildNotice == pWndChild)
	{
		SAFE_DELETE(m_pwndGuildNotice);
		pWndChild = NULL;
	}
	else if (m_pwndGuildSetLogo == pWndChild)
	{
		SAFE_DELETE(m_pwndGuildSetLogo);
		pWndChild = NULL;
	}
	else if (m_pwndGuildDisMiss == pWndChild)
	{
		SAFE_DELETE(m_pwndGuildDisMiss);
		pWndChild = NULL;
	}
}
#endif
	
