#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndFriendConFirm.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;

#include "WndManager.h"

/****************************************************
  WndId : APP_FRIEND_CONFIRM - Confirm
****************************************************/

CWndFriendConFirm::CWndFriendConFirm() 
{
	m_uLeader = m_uMember = 0;
	m_nLeaderSex= 0;
	m_szLeaderName[ 0 ] = '\0';
} 
CWndFriendConFirm::~CWndFriendConFirm() 
{ 
} 

void CWndFriendConFirm::SetMember( u_long uLeader, u_long uMember, LONG nLeaderJob, BYTE nLeaderSex, char * szLeadName )
{
	m_uLeader = uLeader;
	m_uMember = uMember;
	m_nLeaderSex = nLeaderSex;
	m_nLeaderJob = nLeaderJob;
	strcpy( m_szLeaderName, szLeadName );
}
void CWndFriendConFirm::OnDraw( C2DRender* p2DRender ) 
{ 
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	//	CWndStatic* pWndStatic;
	//	pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	CString strTemp;
	strTemp.Format(_T( prj.GetText(TID_DIAG_0071) ),m_szLeaderName); // 메시지 바꾸려면 이걸 바꾸시오
//	strTemp.Format(_T("%s 님이 친구 추가 요청이 들어왔습니다. 승낙하시겠습니까?"),m_szLeaderName); // 메시지 바꾸려면 이걸 바꾸시오
	pWndText->SetString( strTemp );
} 
void CWndFriendConFirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	

	// 윈도를 중앙으로 옮기는 부분.
	/*
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
	*/
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndFriendConFirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_FRIEND_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndFriendConFirm::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndFriendConFirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndFriendConFirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndFriendConFirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndFriendConFirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndFriendConFirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES ) 
	{
		// 여기다가 승락하는 처리 추가하시오
		g_DPlay.SendAddFriend( m_uLeader, m_nLeaderJob, m_nLeaderSex );
		Destroy();	// 수동파괴로 바꿈. -xuzhu- 09/16
	}
	else 
	if( nID == WIDC_NO )
	{
		// 여기다가 거부하는 처리 추가하시오
		g_DPlay.SendFriendCancel( m_uLeader, m_uMember );
		Destroy();	// 수동파괴로 바꿈. -xuzhu- 09/16
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/****************************************************
  WndId : APP_ADDFRIEND - 친구 추가
  CtrlId : WIDC_STATIC1 - 추가할 이름을 입력하세요
  CtrlId : WIDC_EDIT1 - 
  CtrlId : WIDC_STATIC2 - 이  름 :
  CtrlId : WIDC_OK - 확인
  CtrlId : WIDC_CANCEL - 취소
****************************************************/

CWndAddFriend::CWndAddFriend() 
{ 
} 
CWndAddFriend::~CWndAddFriend() 
{ 
} 
void CWndAddFriend::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndAddFriend::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	pWndEdit->SetFocus();

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndAddFriend::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_ADDFRIEND, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndAddFriend::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndAddFriend::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndAddFriend::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndAddFriend::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndAddFriend::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndAddFriend::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK ) 
	{
		LPCTSTR szAddName;
		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		szAddName = pWndEdit->GetString();

#ifdef __GAMEGUARD
		if (prj.IsInvalidName(szAddName))
		{
			pWndEdit->SetString("");
			g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0020)));
		}
		else if (!prj.IsAllowedLetter(szAddName))
		{
			pWndEdit->SetString("");
			g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0020)));
		}
		else if (strlen(szAddName) > MAX_NAME)
		{
			pWndEdit->SetString("");
			g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0057)));
		}
		else if (strcmp(szAddName, g_pPlayer->GetName()) == 0)
		{
			pWndEdit->SetString("");
			g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0056)));
		}
		else if (g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_GUILDWAR)
		{
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_FRIENDADD));
			Destroy();
		}
#ifdef __COLOSSEUM
#ifndef __CORESERVER
		else if (g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_COLOSSEUM)
		{
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_FRIENDADD));
			Destroy();
		}
#endif // __CORESERVER
#endif // __COLOSSEUM
#ifdef __ROYAL_RUMBLE
		else if (g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_FWC)
		{
			g_WndMng.PutString(prj.GetText(TID_PLAYER_WAR_35), NULL, 0xffff0000);
			Destroy();
		}
#endif	// __ROYAL_RUMBLE
		else
		{
			g_DPlay.SendAddFriendNameReqest(szAddName);
			Destroy();

			CString str;
			str.Format(prj.GetText(TID_GAME_MSGINVATE), szAddName);
			g_WndMng.PutString(str, NULL, prj.GetTextColor(TID_GAME_MSGINVATE));
		}
#else // __GAMEGUARD
		if( strlen( szAddName ) < MAX_NAME )
		{
			if( strcmp( szAddName, g_pPlayer->GetName() ) != 0 )
			{
				if( g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
				{
					g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_GUILDCOMBAT_CANNOT_FRIENDADD ) );	// "수정해야함 : 길드대전장에는 친구추가를 할수 없습니다" );
				}
				else
				{
					g_DPlay.SendAddFriendNameReqest( szAddName );
					CString str;
					str.Format( prj.GetText(TID_GAME_MSGINVATE), szAddName );
					g_WndMng.PutString( str, NULL, prj.GetTextColor(TID_GAME_MSGINVATE) );
				}
				Destroy( );	
			}
			else
			{
				pWndEdit->SetString("");
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0056) ) );
			}			
		}
		else
		{
			pWndEdit->SetString("");
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0057) ) );
		}
#endif // __GAMEGUARD
	}
	else 
	if( nID == WIDC_CANCEL )
	{
		Destroy( );
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
