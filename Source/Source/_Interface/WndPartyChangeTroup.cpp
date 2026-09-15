#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndPartyChangeTroup.h"
#include "WndManager.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;

#include "party.h"
extern	CParty g_Party;

CWndPartyChangeName::CWndPartyChangeName() 
{ 
} 
CWndPartyChangeName::~CWndPartyChangeName() 
{ 
} 
void CWndPartyChangeName::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndPartyChangeName::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	if( 0 == strlen( g_pPlayer->m_szPartyName ) )
	{
		strcpy( m_sParty, prj.GetText(TID_GAME_PARTY_NAME) );
	}
	else
	{
		strcpy( m_sParty, g_pPlayer->m_szPartyName );
	}

	CWndEdit *pEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	pEdit->SetString( m_sParty );
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 

BOOL CWndPartyChangeName::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PARTYCHANGENAME, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndPartyChangeName::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPartyChangeName::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPartyChangeName::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPartyChangeName::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndPartyChangeName::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
	{
		CWndEdit *pEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		CString PartyName = pEdit->GetString();

		PartyName.TrimLeft();
		PartyName.TrimRight();
		LPCTSTR lpszString = PartyName;		
		if( PartyName.IsEmpty() == FALSE )
		{
			CHAR c = PartyName[ 0 ];
#ifdef __RULE_0615
			if( PartyName.GetLength() < 4 || PartyName.GetLength() > 16 )
#else	// __RULE_0615
			if( PartyName.GetLength() < 3 || PartyName.GetLength() > 16 )
#endif	// __RULE_0615
			{
#ifdef __RULE_0615
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_RULE_0) ) );
#else	// __RULE_0615
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0011) ) );
#endif	// __RULE_0615
				return TRUE;
			}
			else
			if( IsDBCSLeadByte( c ) == FALSE && isdigit2( c ) ) 
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0012) ) );
				return TRUE;
			}
			else
			{

				for( int i = 0; i < PartyName.GetLength(); i++ )
				{
					c = PartyName[ i ];
					if( IsDBCSLeadByte( c ) == TRUE ) 
					{
						CHAR c2 = PartyName[ ++i ];
						WORD word = ( ( c << 8 ) & 0xff00 ) | ( c2 & 0x00ff );
						if( ::GetLanguage() == LANG_KOR )
						{
							if( IsHangul( word ) == FALSE ) 
							{
								g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0014) ) );
								return TRUE;
							}
						}
					}
					else
					if( ::GetLanguage() != LANG_THA )
					{
						if( !IsCyrillic( c ) && ( isalnum( c ) == FALSE || iscntrl( c ) )  )
						{
							g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0013) ) );
							return TRUE;
						}
					}
				}
			}

			if( prj.IsInvalidName( PartyName ) 
#ifdef __RULE_0615
				|| prj.IsAllowedLetter( PartyName ) == FALSE
#endif	// __RULE_0615
				)
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0020) ) );
				return TRUE;
			}

			strcpy( m_sParty, PartyName );
			if( 0 == strcmp( m_sParty, prj.GetText(TID_GAME_PARTY_NAME ) ) )
			{
			}
			else
			{
				g_DPlay.SendChangeTroup( TRUE, m_sParty );
				Destroy();			
			}
			
		}
		else
		{
#ifdef __RULE_0615
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_RULE_0) ) );
#else	// __RULE_0615
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0011) ) );
#endif	// __RULE_0615
			return TRUE;
		}
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

//--------------------------------------------------------------------------------------------------------------------

CWndPartyChangeTroup::CWndPartyChangeTroup() 
{ 
	m_WndPartyChangeName = NULL;
} 
CWndPartyChangeTroup::~CWndPartyChangeTroup() 
{ 
#ifdef __FIX_WND_1109
	SAFE_DELETE( m_WndPartyChangeName );
#endif	// __FIX_WND_1109
} 

void CWndPartyChangeTroup::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndPartyChangeTroup::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 

BOOL CWndPartyChangeTroup::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CHANGETROUP, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndPartyChangeTroup::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPartyChangeTroup::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPartyChangeTroup::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPartyChangeTroup::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndPartyChangeTroup::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
	{
		SAFE_DELETE( m_WndPartyChangeName );
		m_WndPartyChangeName = new CWndPartyChangeName;
#ifdef __FIX_WND_1109
		m_WndPartyChangeName->Initialize( );
#else	// __FIX_WND_1109
		m_WndPartyChangeName->Initialize( );
#endif	// __FIX_WND_1109
		Destroy();		
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

CWndPartyLeaveConfirm::CWndPartyLeaveConfirm() 
{ 
} 
CWndPartyLeaveConfirm::~CWndPartyLeaveConfirm() 
{ 
} 
void CWndPartyLeaveConfirm::SetLeaveId( u_long uidPlayer )
{
	uLeaveId = uidPlayer;

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	if( g_pPlayer->m_idPlayer == uLeaveId )
	{
		pWndEdit->SetString( prj.GetText( TID_DIAG_0084 ) );
	}
	else
	{
		pWndEdit->SetString( prj.GetText( TID_DIAG_0085 ) );
	}
	pWndEdit->EnableWindow( FALSE );
}
void CWndPartyLeaveConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndPartyLeaveConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
BOOL CWndPartyLeaveConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PARTYLEAVE_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndPartyLeaveConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPartyLeaveConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPartyLeaveConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPartyLeaveConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndPartyLeaveConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_NO || nID == WTBID_CLOSE ) // Ãë¼Ò 
	{
		Destroy( TRUE );
	}
	else if ( nID == WIDC_YES )
	{
		g_DPlay.SendRemovePartyMember( g_pPlayer->m_idPlayer, uLeaveId );
		Destroy( TRUE );
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
