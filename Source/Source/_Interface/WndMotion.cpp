#include "stdafx.h"
#include "AppDefine.h"
#include "WndMotion.h"
#include "WndManager.h"
#include "defineText.h"

#ifdef __MODEL_VIEW
#include "WndModelView.h"
#endif //__MODEL_VIEW


CWndMotion::CWndMotion() 
{ 
} 
CWndMotion::~CWndMotion() 
{ 
} 
void CWndMotion::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndMotion::OnMouseWndSurface( CPoint point )
{
}	
void CWndMotion::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 


	CRect rect = GetClientRect();
	rect.left = 0;
	rect.top = 0;
	CWndTabCtrl* lpTapCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	m_wndMotion1.Create( WBS_CHILD | WBS_NODRAWFRAME , rect, lpTapCtrl, 100000 );
#ifdef __AEGON_EMOTE_SYSTEM
	BOOL bAdminEmotes = FALSE;
	if (IsValidObj(g_pPlayer) && g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
		bAdminEmotes = TRUE;

	m_wndAegonEmoticon.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, lpTapCtrl, 1000001);
	if(bAdminEmotes)
		m_wndAegonEmoticonAdmin.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, lpTapCtrl, 1000002);
#else //__AEGON_EMOTE_SYSTEM
	m_wndEmoticon.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, lpTapCtrl, 1000001);
#endif //__AEGON_EMOTE_SYSTEM

	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_MOTION);//"기본";
	tabTabItem.pWndBase = &m_wndMotion1;
	lpTapCtrl->InsertItem( 0, &tabTabItem );
	
#ifdef __AEGON_EMOTE_SYSTEM
	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_EMOTICON);//"자세히";
	tabTabItem.pWndBase = &m_wndAegonEmoticon;
	lpTapCtrl->InsertItem(1, &tabTabItem);

	if (bAdminEmotes)
	{
		tabTabItem.pszText = "GM";//"자세히";
		tabTabItem.pWndBase = &m_wndAegonEmoticonAdmin;
		lpTapCtrl->InsertItem(2, &tabTabItem);
	}
#else //__AEGON_EMOTE_SYSTEM
	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_EMOTICON);//"자세히";
	tabTabItem.pWndBase = &m_wndEmoticon;
	lpTapCtrl->InsertItem(1, &tabTabItem);
#endif //__AEGON_EMOTE_SYSTEM

	lpTapCtrl->SetCurSel( 0 );
	
#ifdef __AEGON_EMOTE_SYSTEM
	int nButtonLength = 0;
	int nWidth = lpTapCtrl->GetClientRect().Width() - 32;
	int nTabs = 2;

	if (bAdminEmotes)
		nTabs = 3;

	nButtonLength = nWidth / nTabs;

	lpTapCtrl->SetButtonLength(nButtonLength);
#else //__AEGON_EMOTE_SYSTEM
	lpTapCtrl->SetButtonLength( 130 );
#endif //__AEGON_EMOTE_SYSTEM	

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CPoint point( rectRoot.left, rectRoot.top  );
	
	CRect rectWindow = GetWindowRect();
	Move( point );
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndMotion::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MOTION, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndMotion::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndMotion::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndMotion::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndMotion::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndMotion::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndMotion::OnMouseMove( UINT nFlags, CPoint point )
{
}

CWndMotion1::CWndMotion1()
{
	m_pSelectMotion = NULL;
	m_nSelect = 0;	
#ifdef __AEGON_PREM_MOTIONS
	m_pTextureLock = NULL;
#endif // __AEGON_PREM_MOTIONS
}
CWndMotion1::~CWndMotion1()
{
}
void CWndMotion1::OnDraw(C2DRender* p2DRender)
{
	MotionProp* pMotionProp = NULL;
	int j = 0;
	int ny;
#ifdef __AEGON_PREM_MOTIONS
	int drawOffsetY = m_wndScrollBar.GetScrollPos() * 32;

	if (!g_pPlayer)
		return;
	//BOOL bPremiumUser = g_pPlayer->IsPremium();
#endif



	for( int i = 0; i < m_motionArray.GetSize(); i++ )
	{
		pMotionProp = (MotionProp*)m_motionArray.GetAt( i );
		if( pMotionProp )
		{
			ny = j / 8;
#ifdef __AEGON_PREM_MOTIONS
			if (pMotionProp->dwUnlockItemID == 0 || (g_pPlayer && (/*bPremiumUser ||*/ easy_find(g_pPlayer->m_vUnlockedMotions, pMotionProp->dwUnlockItemID))))
			{
				p2DRender->RenderTexture(CPoint((j - (ny * 8)) * 32 + 3, 7 + (ny * 32) - drawOffsetY), pMotionProp->pTexture);
			}
			else 
			{
				p2DRender->RenderTexture2(CPoint((j - (ny * 8)) * 32 + 3, 7 + (ny * 32) - drawOffsetY), pMotionProp->pTexture, 1.0f, 1.0f, D3DCOLOR_ARGB(255, 210, 90, 90));
				if (m_pTextureLock)
				{
					p2DRender->RenderTexture(CPoint((j - (ny * 8)) * 32 + 3, 7 + (ny * 32) - drawOffsetY), m_pTextureLock);
				}
			}
			
#else // __AEGON_PREM_MOTIONS
			p2DRender->RenderTexture(CPoint((j - (ny * 8)) * 32 + 3, 7 + (ny * 32)), pMotionProp->pTexture);
#endif // __AEGON_PREM_MOTIONS
			j++;
		}
	}	
}
void CWndMotion1::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	MotionProp* pMotionProp = NULL;
	int j = 0;
	for( int i = 0; i < prj.m_aPropMotion.GetSize(); i++ )
	{
		pMotionProp = prj.GetMotionProp( i );
		if( pMotionProp )
		{
			m_motionArray.SetAtGrow( j, pMotionProp ); j++;
			pMotionProp->pTexture = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pMotionProp->szIconName ), COLOR_ITEM_BG );
		}
	}
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );

#ifdef __AEGON_PREM_MOTIONS
	m_pTextureLock = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(g_Neuz.m_pd3dDevice, "lock.png", COLOR_ITEM_BG, TRUE);


	CRect rect = rectWindow;
	rect.right -= 10;
	rect.bottom -= 55;
	m_wndScrollBar.AddWndStyle(WBS_DOCKING);
	m_wndScrollBar.Create(WBS_VERT, rect, this, 1000);
	int nRange = 0;
	int nPage = rect.Height() / 32;

	nRange = ceil((double)j / 8.0f);

	if (!nRange)
		nRange = 1;

	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);
	m_wndScrollBar.EnableScrollBar(TRUE);
	SetScrollBarVisible(TRUE);
#endif // __AEGON_PREM_MOTIONS
}
BOOL CWndMotion1::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 240, 0, 240 + 330, 255 - 135 ); 
	return CWndBase::Create(WBS_THICKFRAME|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);
}
BOOL CWndMotion1::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{ 
	return CWndBase::OnChildNotify( message, nID, pLResult );
}
BOOL CWndMotion1::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndBase::OnCommand(nID,dwMessage,pWndBase);
}
void CWndMotion1::OnSize(UINT nType, int cx, int cy)
{
	/*
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize( 70, 25);

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	CWndBase::OnSize(nType,cx,cy);
	*/
}
void CWndMotion1::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_pSelectMotion) 
	{
#ifdef __AEGON_PREM_MOTIONS
		if (!g_pPlayer)
			return;

		//BOOL bPremiumUser = g_pPlayer->IsPremium();

		if ( (m_pSelectMotion->dwUnlockItemID > 0 /*&& !bPremiumUser*/ && !easy_find(g_pPlayer->m_vUnlockedMotions, m_pSelectMotion->dwUnlockItemID)))
		{
			CWndModelView* pWndViewer = (CWndModelView*)g_WndMng.GetApplet(APP_MODEL_VIEW);
			if (!pWndViewer) 
			{
				pWndViewer = (CWndModelView*)g_WndMng.CreateApplet(APP_MODEL_VIEW);
			}
			if (pWndViewer) 
			{
				CItemElem tmpItem;
				tmpItem.m_dwItemId = m_pSelectMotion->dwUnlockItemID;
				pWndViewer->SetPart(&tmpItem);
			}
		}
		else
#endif // __AEGON_PREM_MOTIONS
		g_WndMng.ObjectExecutor(SHORTCUT_MOTION, m_pSelectMotion->dwID);
	}
		
			
}
void CWndMotion1::OnLButtonDown(UINT nFlags, CPoint point)
{
	CPoint pt;
	MotionProp* pMotionProp = NULL;
	int j = 0;
	int ny;

#ifdef __AEGON_PREM_MOTIONS
	int drawOffsetY = m_wndScrollBar.GetScrollPos() * 32;
#endif

	for( int i = 0; i < m_motionArray.GetSize(); i++ )
	{
		pMotionProp = (MotionProp*)m_motionArray.GetAt( i );
		if( pMotionProp )
		{
			ny = j / 8;
#ifdef __AEGON_PREM_MOTIONS
			pt = CPoint((j - (ny * 8)) * 32 + 3, 7 + (ny * 32) - drawOffsetY);
#else
			pt = CPoint( ( j - ( ny * 8 ) ) * 32 + 3, 7 + ( ny * 32 ) );
#endif
			CRect rect( pt.x, pt.y, pt.x+32, pt.y+32 );
			
			CPoint pt2 = point;
			
			ClientToScreen( &pt2 );
			ClientToScreen( &rect );

			if( PtInRect(&rect, pt2))
			{
				m_pSelectMotion = pMotionProp;
				m_nSelect = i;
				break;
			}
			else
			{
				m_pSelectMotion = NULL;
				m_nSelect = 0;
			}
			
			j++;
		}
	}
}

BOOL CWndMotion1::Process( void )
{
	if( !IsFocusChild() )
		return FALSE;
	
	CPoint point = GetMousePoint();
	CPoint pt;
	MotionProp* pMotionProp = NULL;
	int j = 0;
	int ny;

#ifdef __AEGON_PREM_MOTIONS
	int drawOffsetY = m_wndScrollBar.GetScrollPos() * 32;
	if (!g_pPlayer)
		return FALSE;
	/*BOOL bPremiumUser = g_pPlayer->IsPremium();*/
#endif


	for( int i = 0; i < m_motionArray.GetSize(); i++ )
	{
		pMotionProp = (MotionProp*)m_motionArray.GetAt( i );
		if( pMotionProp  )
		{
			if( pMotionProp->dwID != MOT_BASE_CHEER )
			{
				j++;
				continue;
			}

			ny = j / 8;
#ifdef __AEGON_PREM_MOTIONS
			pt = CPoint((j - (ny * 8)) * 32 + 3, 7 + (ny * 32) - drawOffsetY);
#else
			pt = CPoint( ( j - ( ny * 8 ) ) * 32 + 3, 7 + ( ny * 32 ) );
#endif
			CRect rect( pt.x, pt.y, pt.x+32, pt.y+32 );
			
			CPoint pt2 = point;
			
			ClientToScreen( &pt2 );
			ClientToScreen( &rect );
			
			TCHAR szDesc [ 1024 ] = { 0 };
			_tcscpy( szDesc, pMotionProp->szDesc );


			
			if( g_pPlayer )
			{

#ifdef __AEGON_PREM_MOTIONS
				if (pMotionProp->dwUnlockItemID > 0)
				{
					if (/*!bPremiumUser &&*/ !easy_find(g_pPlayer->m_vUnlockedMotions, pMotionProp->dwUnlockItemID))
					{
						_tcscat(szDesc, "\n(locked)");
					}
				}
#endif // __AEGON_PREM_MOTIONS

				TCHAR szChar[ 256 ] = { 0 };
				_stprintf( szChar, prj.GetText(TID_CHEER_MESSAGE1), g_pPlayer->m_nCheerPoint );
				_tcscat( szDesc, "\n" );
				_tcscat( szDesc, szChar );	
				
				if( g_pPlayer->m_nCheerPoint < MAX_CHEERPOINT )
				{
					CTimeSpan ct( ((g_pPlayer->m_dwTickCheer - GetTickCount()) / 1000) );		// 남은시간을 초단위로 변환해서 넘겨줌
					
					TCHAR szChar2[ 256 ] = { 0 };
					if( ct.GetMinutes() <= 0 )
					{
						_stprintf( szChar2, prj.GetText(TID_PK_LIMIT_SECOND), ct.GetSeconds() );
					}
					else
					{
						_stprintf( szChar2, prj.GetText(TID_PK_LIMIT_MINUTE), ct.GetMinutes() );
					}
					
					_tcscat( szDesc, "\n" );
					_tcscat( szDesc, prj.GetText(TID_CHEER_MESSAGE2) );	
					_tcscat( szDesc, szChar2 );	
				}
				
				g_toolTip.PutToolTip( pt.x + 10000, szDesc, rect, pt2, 0 );
				
				break;
			}
			j++;
		}
	}

	return TRUE;
}

void CWndMotion1::OnMouseWndSurface( CPoint point )
{
	CPoint pt;
	MotionProp* pMotionProp = NULL;
	int j = 0;
	int ny;

#ifdef __AEGON_PREM_MOTIONS
	int drawOffsetY = m_wndScrollBar.GetScrollPos() * 32;
	if (!g_pPlayer)
		return;
	//BOOL bPremiumUser = g_pPlayer->IsPremium();
#endif

	for( int i = 0; i < m_motionArray.GetSize(); i++ )
	{
		pMotionProp = (MotionProp*)m_motionArray.GetAt( i );
		if( pMotionProp )
		{
			ny = j / 8;
#ifdef __AEGON_PREM_MOTIONS
			pt = CPoint((j - (ny * 8)) * 32 + 3, 7 + (ny * 32) - drawOffsetY);
#else
			pt = CPoint( ( j - ( ny * 8 ) ) * 32 + 3, 7 + ( ny * 32 ) );
#endif
			CRect rect( pt.x, pt.y, pt.x+32, pt.y+32 );
	
			CPoint pt2 = point;
			
			ClientToScreen( &pt2 );
			ClientToScreen( &rect );
			
			TCHAR szDesc [ 1024 ] = { 0 };
			_tcscpy( szDesc, pMotionProp->szDesc );

#ifdef __AEGON_PREM_MOTIONS
			if (g_pPlayer && pMotionProp->dwUnlockItemID > 0)
			{
				if (/*!bPremiumUser &&*/ !easy_find(g_pPlayer->m_vUnlockedMotions, pMotionProp->dwUnlockItemID))
				{
					_tcscat(szDesc, "\n(Locked)");
				}
			}
#endif // __AEGON_PREM_MOTIONS
			if( pMotionProp->dwID != MOT_BASE_CHEER )		
				g_toolTip.PutToolTip( pt.x + 10000, szDesc, rect, pt2, 0 );
			
			j++;
		}
	}
}	
void CWndMotion1::OnMouseMove( UINT nFlags, CPoint point )
{
#ifdef __AEGON_PREM_MOTIONS
	if (!g_pPlayer)
		return;
	//BOOL bPremiumUser = g_pPlayer->IsPremium();
#endif

	if( m_pSelectMotion && IsPush()
#ifdef __AEGON_FIXES
		&& m_nSelect != -1
#endif
		)
	{
#ifdef __AEGON_PREM_MOTIONS
		if (m_pSelectMotion->dwUnlockItemID > 0 /*&& !bPremiumUser*/ && !easy_find(g_pPlayer->m_vUnlockedMotions, m_pSelectMotion->dwUnlockItemID))
			return;
#endif // __AEGON_PREM_MOTIONS
		m_GlobalShortcut.m_pFromWnd   = this;
		m_GlobalShortcut.m_dwShortcut = SHORTCUT_MOTION;
		m_GlobalShortcut.m_dwIndex    = m_nSelect;
		m_GlobalShortcut.m_dwType     = 0;
		m_GlobalShortcut.m_dwId       = m_pSelectMotion->dwID;
		m_GlobalShortcut.m_pTexture   = m_pSelectMotion->pTexture;
#ifdef _WIN64
		m_GlobalShortcut.m_dwData = (ULONG_PTR)m_pSelectMotion;
#else
		m_GlobalShortcut.m_dwData     = (DWORD) m_pSelectMotion;
#endif
	}
}
#ifdef __AEGON_PREM_MOTIONS
BOOL CWndMotion1::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta < 0)
	{
		if (m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos())
			m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetScrollPos() + 1);
		else
			m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage());
	}
	else
	{
		if (m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos())
			m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetScrollPos() - 1);
		else
			m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetMinScrollPos());
	}

	return TRUE;
}
#endif
CWndEmoticon::CWndEmoticon()
{
}
CWndEmoticon::~CWndEmoticon()
{
}
void CWndEmoticon::OnDraw(C2DRender* p2DRender)
{
	int j = 0;
	int ny = 0;
	
	for( int i=0; i < (int)( g_DialogMsg.m_texEmoticonUser.GetNumber() ); i++ )
	{
		CTexture *pTexture = g_DialogMsg.m_texEmoticonUser.GetAt(i);
		if( pTexture )
		{
			ny = j / 8;
			p2DRender->RenderTexture( CPoint( ( j - ( ny * 8 ) ) * 32 + 3, 7 + ( ny * 32 ) ), pTexture );
			j++;
		}
	}
}
void CWndEmoticon::OnInitialUpdate()
{
	m_nSelect = -1;

	CWndBase::OnInitialUpdate();
}
BOOL CWndEmoticon::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 240, 0, 240 + 330, 255 - 135 ); 
	return CWndBase::Create(WBS_THICKFRAME|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);
}
BOOL CWndEmoticon::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{ 
	return CWndBase::OnChildNotify( message, nID, pLResult );
}
BOOL CWndEmoticon::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndBase::OnCommand(nID,dwMessage,pWndBase);
}
void CWndEmoticon::OnSize(UINT nType, int cx, int cy)
{
	CWndBase::OnSize( nType, cx, cy );
}
void CWndEmoticon::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_pSelectTexture 
#ifdef __AEGON_FIXES
		&& m_nSelect != -1
#endif
		)
		g_WndMng.ObjectExecutor( SHORTCUT_EMOTICON, m_nSelect );	

#ifdef __AEGON_FIXES
	m_pSelectTexture = NULL;
#endif
}
void CWndEmoticon::OnLButtonDown(UINT nFlags, CPoint point)
{
	CPoint pt;
	MotionProp* pMotionProp = NULL;
	int j = 0;
	int ny;
	m_nSelect = -1;

	for( int i=0; i < (int)( g_DialogMsg.m_texEmoticonUser.GetNumber() ); i++ )
	{
		CTexture *pTexture = g_DialogMsg.m_texEmoticonUser.GetAt(i);
		if( pTexture )
		{
			ny = j / 8;
			pt = CPoint( ( j - ( ny * 8 ) ) * 32 + 3, 7 + ( ny * 32 ) );
			CRect rect( pt.x, pt.y, pt.x+32, pt.y+32 );
			
			CPoint pt2 = point;
			
			ClientToScreen( &pt2 );
			ClientToScreen( &rect );

			if( PtInRect(&rect, pt2))
			{
				m_pSelectTexture  = pTexture;
				m_nSelect = i;
				break;
			}
			
			j++;
		}
	}
}

void CWndEmoticon::OnMouseWndSurface( CPoint point )
{
	CPoint pt;
	int j = 0;
	int ny;
	for( int i = 0; i < (int)( g_DialogMsg.m_texEmoticonUser.GetNumber() ) ; i++ )
	{
		CTexture *pTexture = g_DialogMsg.m_texEmoticonUser.GetAt(i);
		if( pTexture )
		{
			ny = j / 8;
			pt = CPoint( ( j - ( ny * 8 ) ) * 32 + 3, 7 + ( ny * 32 ) );
			CRect rect( pt.x, pt.y, pt.x+32, pt.y+32 );
			
			CPoint pt2 = point;
			
			ClientToScreen( &pt2 );
			ClientToScreen( &rect );
			
			if( PtInRect( &rect, pt2 ) )
			{
				TCHAR szCmd[1024] = { 0 };

				_tcscat( szCmd, "/" );
				_tcscat( szCmd, g_DialogMsg.m_EmiticonCmd[i].m_szCommand );

				g_toolTip.PutToolTip( pt.x + 10000, szCmd, rect, pt2, 0 );
			}
			
			j++;
		}
	}
	
	
}	
void CWndEmoticon::OnMouseMove( UINT nFlags, CPoint point )
{
	if( m_pSelectTexture && IsPush()
#ifdef __AEGON_FIXES
		&& m_nSelect != -1
#endif
		)
	{
		m_GlobalShortcut.m_pFromWnd   = this;
		m_GlobalShortcut.m_dwShortcut = SHORTCUT_EMOTICON;
		m_GlobalShortcut.m_dwIndex    = m_nSelect;
		m_GlobalShortcut.m_dwType     = 0;
		m_GlobalShortcut.m_dwId       = m_nSelect;
		m_GlobalShortcut.m_pTexture   = m_pSelectTexture;
#ifdef _WIN64
		m_GlobalShortcut.m_dwData = (ULONG_PTR)m_pSelectTexture;
#else
		m_GlobalShortcut.m_dwData     = (DWORD) m_pSelectTexture;
#endif

		TCHAR szCmd[1024] = { 0 };
		
		_tcscat( szCmd, "/" );
		_tcscat( szCmd, g_DialogMsg.m_EmiticonCmd[m_nSelect].m_szCommand );
		
		_tcscpy( m_GlobalShortcut.m_szString, szCmd  );
	}
}

#ifdef __AEGON_EMOTE_SYSTEM
CWndAegonEmoticon::CWndAegonEmoticon()
{
	m_pTextureLock = NULL;

	m_pEmoteMap = &CAegonEmoticonMng::GetInstance()->m_mapEmotes;
	if (!m_pEmoteMap)
	{
		Destroy();
		return;
	}
}
CWndAegonEmoticon::~CWndAegonEmoticon()
{
}
void CWndAegonEmoticon::OnDraw(C2DRender* p2DRender)
{
	if (!m_pEmoteMap)
		return;

	int j = 0;
	int ny = 0;

	int drawOffsetY = m_wndScrollBar.GetScrollPos() * 32;

	auto it = m_pEmoteMap->begin();
	
	for (; it != m_pEmoteMap->end(); it++)
	{
		BOOL bRenderNormal = FALSE;
		if (it->second.m_pIconTexture)
		{
			ny = j / 8;


			if (it->second.m_vUnlockItems.size() > 0)
			{
				BOOL bEmoteOwned = CAegonEmoticonMng::GetInstance()->PlayerOwnsEmote(g_pPlayer, it->second.nEmoteID, it->second.GetUnitType());

				if (bEmoteOwned)
				{
					bRenderNormal = TRUE;
				}
				else
				{
					
					p2DRender->RenderTexture2(CPoint((j - (ny * 8)) * 32 + 3, 7 + (ny * 32) - drawOffsetY), it->second.m_pIconTexture, 1.0f, 1.0f, D3DCOLOR_ARGB(255, 210, 90, 90));
					if (m_pTextureLock)
					{
						p2DRender->RenderTexture(CPoint((j - (ny * 8)) * 32 + 3, 7 + (ny * 32) - drawOffsetY), m_pTextureLock);
					}
				}
			}
			else
				bRenderNormal = TRUE;

			if(bRenderNormal)
				p2DRender->RenderTexture(CPoint((j - (ny * 8)) * 32 + 3, 7 + (ny * 32) - drawOffsetY), it->second.m_pIconTexture);

			j++;
		}
	}
}
void CWndAegonEmoticon::OnInitialUpdate()
{
	selectedUnit = NULL;

	CWndBase::OnInitialUpdate();

	CRect rect = GetWindowRect();
	rect.right -= 10;
	rect.bottom -= 55;
	m_wndScrollBar.AddWndStyle(WBS_DOCKING);
	m_wndScrollBar.Create(WBS_VERT, rect, this, 1000);

	int nRange = 0;
	int nPage = rect.Height() / 32;

	nRange = ceil((double)m_pEmoteMap->size() / 8.0f);
	
	if (!nRange)
		nRange = 1;

	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);
	m_wndScrollBar.EnableScrollBar(TRUE);
	SetScrollBarVisible(TRUE);

	m_pTextureLock = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(g_Neuz.m_pd3dDevice, "lock.png", COLOR_ITEM_BG, TRUE);

}
BOOL CWndAegonEmoticon::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect(240, 0, 240 + 330, 255 - 135);
	return CWndBase::Create(WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_EXTENSION, rect, pWndParent, dwWndId);
}
BOOL CWndAegonEmoticon::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndBase::OnChildNotify(message, nID, pLResult);
}
BOOL CWndAegonEmoticon::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndBase::OnCommand(nID, dwMessage, pWndBase);
}
void CWndAegonEmoticon::OnSize(UINT nType, int cx, int cy)
{
	CWndBase::OnSize(nType, cx, cy);
}
void CWndAegonEmoticon::OnLButtonUp(UINT nFlags, CPoint point)
{
	BOOL bEmoteOwned = FALSE;
	if (selectedUnit)
		bEmoteOwned = CAegonEmoticonMng::GetInstance()->PlayerOwnsEmote(g_pPlayer, selectedUnit->nEmoteID, selectedUnit->GetUnitType());

	if (m_pSelectTexture && selectedUnit && bEmoteOwned)
		g_WndMng.ObjectExecutor(SHORTCUT_EMOTICON, selectedUnit->nEmoteID, selectedUnit->GetUnitType());

	m_pSelectTexture = NULL;
}
void CWndAegonEmoticon::OnLButtonDown(UINT nFlags, CPoint point)
{
	CPoint pt;
	MotionProp* pMotionProp = NULL;
	int j = 0;
	int ny;
	selectedUnit = NULL;

	int drawOffsetY = m_wndScrollBar.GetScrollPos() * 32;

	auto it = m_pEmoteMap->begin();
	for (; it != m_pEmoteMap->end(); it++)
	{
		if (it->second.m_pIconTexture) 
		{
			ny = j / 8;
			pt = CPoint((j - (ny * 8)) * 32 + 3, 7 + (ny * 32) - drawOffsetY);
			CRect rect(pt.x, pt.y, pt.x + 32, pt.y + 32);

			CPoint pt2 = point;

			ClientToScreen(&pt2);
			ClientToScreen(&rect);

			if (PtInRect(&rect, pt2))
			{
				BOOL bOwnedEmote = CAegonEmoticonMng::GetInstance()->PlayerOwnsEmote(g_pPlayer, it->second.nEmoteID, it->second.GetUnitType());
				if (bOwnedEmote)
				{
					m_pSelectTexture = it->second.m_pIconTexture;
					selectedUnit = &it->second;
				}
				else 
				{
					CWndIngameDPPurchaseConfirm* pWndConfirmPurchase = g_WndMng.OpenDPConfirmPurchase();
					if (pWndConfirmPurchase) 
					{
						pWndConfirmPurchase->m_nCommand = INGAMEDP_BUYEMOTE;
						pWndConfirmPurchase->dwID = it->second.nEmoteID;

						pWndConfirmPurchase->SetInfo(it->second.szEmoteName, it->second.m_dwPriceDP);
					}
				}
				break;
			}
			j++;
		}
	}
}

void CWndAegonEmoticon::OnMouseWndSurface(CPoint point)
{
	CPoint pt;
	int j = 0;
	int ny;

	int drawOffsetY = m_wndScrollBar.GetScrollPos() * 32;

	auto it = m_pEmoteMap->begin();
	for (; it != m_pEmoteMap->end(); it++)
	{
		if (it->second.m_pIconTexture) 
		{
			ny = j / 8;
			pt = CPoint((j - (ny * 8)) * 32 + 3, 7 + (ny * 32) - drawOffsetY);
			CRect rect(pt.x, pt.y, pt.x + 32, pt.y + 32);

			CPoint pt2 = point;

			ClientToScreen(&pt2);
			ClientToScreen(&rect);

			if (PtInRect(&rect, pt2))
			{
				TCHAR szCmd[1024] = { 0 };
				CAegonEmoticonMng::GetInstance()->MakeEmoticonTooltipText(szCmd, it->second.nEmoteID, it->second.GetUnitType());

				g_toolTip.PutToolTip(pt.x + 10000, szCmd, rect, pt2, 0);
			}

			j++;
		}
	}
}
void CWndAegonEmoticon::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_pSelectTexture && IsPush() && selectedUnit)
	{
		m_GlobalShortcut.m_pFromWnd = this;
		m_GlobalShortcut.m_dwShortcut = SHORTCUT_EMOTICON;
		m_GlobalShortcut.m_dwIndex = selectedUnit->nEmoteID;
		m_GlobalShortcut.m_dwType = selectedUnit->GetUnitType();
		m_GlobalShortcut.m_dwId = selectedUnit->nEmoteID;
		m_GlobalShortcut.m_pTexture = m_pSelectTexture;
#ifdef _WIN64
		m_GlobalShortcut.m_dwData = (ULONG_PTR)m_pSelectTexture;
#else
		m_GlobalShortcut.m_dwData = (DWORD)m_pSelectTexture;
#endif

		TCHAR szCmd[1024] = { 0 };

		CAegonEmoticonMng::GetInstance()->MakeEmoticonTooltipText(szCmd, selectedUnit->nEmoteID, m_GlobalShortcut.m_dwType);

		_tcscpy(m_GlobalShortcut.m_szString, szCmd);
	}
}
BOOL CWndAegonEmoticon::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta < 0)
	{
		if (m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos())
			m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetScrollPos() + 1);
		else
			m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage());
	}
	else
	{
		if (m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos())
			m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetScrollPos() - 1);
		else
			m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetMinScrollPos());
	}

	return TRUE;
}

//ADMIN EMOTES
CWndAegonEmoticonAdmin::CWndAegonEmoticonAdmin()
{
	m_pTextureLock = NULL;

	m_pEmoteMap = &CAegonEmoticonMng::GetInstance()->m_mapAdminEmotes;
	if (!m_pEmoteMap)
	{
		Destroy();
		return;
	}
}
CWndAegonEmoticonAdmin::~CWndAegonEmoticonAdmin()
{
}
#endif