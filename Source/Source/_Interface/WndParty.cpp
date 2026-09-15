#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndParty.h"
#include "WndManager.h"
#include "Party.h"
#include "dpclient.h"
extern CDPClient	g_DPlay;

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA


/****************************************************
  WndId : APP_PARTY - ±Ø´Ü(Party)
  CtrlId : WIDC_TABCTRL1 - TabCtrl
  CtrlId : WIDC_LEAVE - Å»Åð
  CtrlId : WIDC_CHANGE - ±Ø´ÜÀüÈ¯
****************************************************/

CWndParty::CWndParty() 
{ 
#ifdef __PARTY_ASSISTANT
	m_dwLastAssist = 0;
#endif //__PARTY_ASSISTANT
	m_WndPartyChangeTroup = NULL;
#if __VER >= 8 //__CSC_VER8_2
	m_pWndPartyQuick = NULL;
#endif //__CSC_VER8_2
#ifdef __RENAMEPARTY
	m_WndPartyChangeName = NULL;
#endif //__RENAMEPARTY
} 
CWndParty::~CWndParty() 
{ 
#ifdef __FIX_WND_1109
	SAFE_DELETE( m_WndPartyChangeTroup );
#endif	// __FIX_WND_1109
#ifdef __RENAMEPARTY
	SAFE_DELETE(m_WndPartyChangeName);
#endif //__RENAMEPARTY
	DeleteDeviceObjects();
} 
void CWndParty::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
//	Error( "CWndParty::SerializeRegInfo() - IN" );

	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	CWndTabCtrl* lpTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	if( ar.IsLoading() )
	{
		if( dwVersion == 0 )
		{
		}
		else
		if( dwVersion == 1 )
		{
			int nCurSel;
			ar >> nCurSel;
			lpTabCtrl->SetCurSel( nCurSel );
		}
	}
	else
	{
		dwVersion = 1;
		ar << lpTabCtrl->GetCurSel();
	}

//	Error( "CWndParty::SerializeRegInfo() - OUT" );
}
void CWndParty::OnDraw( C2DRender* p2DRender ) 
{ 
#ifdef __AEGON_FIXES
	if (IsInvalidObj(g_pPlayer))
		return;
#endif

	if( g_Party.GetSizeofMember() < 2 )
	{
		m_pWndLeave->EnableWindow( FALSE );
		m_pWndTransfer->EnableWindow( FALSE );
#ifdef __PARTY_ASSISTANT
		assistant->EnableWindow(FALSE);
#endif //__PARTY_ASSISTANT
#if __VER >= 8 //__CSC_VER8_2
		m_pBtnPartyQuick->EnableWindow( FALSE );
#endif //__CSC_VER8_2
	}
	else
	{
		m_pWndLeave->EnableWindow( TRUE );
#ifdef __PARTY_ASSISTANT
		if (g_Party.IsLeader(g_pPlayer->m_idPlayer) == FALSE)
		{
			assistant->EnableWindow(FALSE);
			m_pWndTransfer->EnableWindow(FALSE);
		}
		else
		{
			assistant->EnableWindow(TRUE);
			m_pWndTransfer->EnableWindow(TRUE);
		}
#else //__PARTY_ASSISTANT
		m_pWndTransfer->EnableWindow(TRUE);
#endif //__PARTY_ASSISTANT
#if __VER >= 8 //__CSC_VER8_2
		m_pBtnPartyQuick->EnableWindow( TRUE );
#endif //__CSC_VER8_2
	}

#ifdef __PARTY_DEBUG_0129		// ±Ø´ÜÀå Æ¨±â´Â Çö»ó µð¹ö±ë neuz
	if( g_Party.GetSizeofMember() < 2 && g_Party.m_nLevel > 1 )
	{
		m_pWndLeave->EnableWindow( TRUE );
	}
#endif	// __PARTY_DEBUG_0129		// ±Ø´ÜÀå Æ¨±â´Â Çö»ó µð¹ö±ë neuz

	if( g_Party.GetLevel() >= 10 && g_Party.IsLeader( g_pPlayer->m_idPlayer ) )
		m_pWndChange->EnableWindow( TRUE );
	else
		m_pWndChange->EnableWindow( FALSE );

	// ÆÄÆ¼ Á¤º¸ Ãâ·Â
	CWndStatic* pWndStatic;
	CString strTemp;
	if(g_Party.m_nKindTroup==0)
	{
		strTemp.Format("%s",prj.GetText(TID_GAME_PARTY1));
	}
	else 
	{
		if( 0 == strlen( g_Party.m_sParty ) )
			strTemp.Format("%s",prj.GetText(TID_GAME_PARTY2));
		else
			strTemp.Format("%s",g_Party.m_sParty);
	}
	pWndStatic = (CWndStatic*)GetDlgItem( WIDC_NAME );
	pWndStatic->SetTitle(strTemp);
	strTemp.Format("%d",g_Party.m_nLevel);
	pWndStatic = (CWndStatic*)GetDlgItem( WIDC_LEVEL );
	pWndStatic->SetTitle(strTemp);
	
	if( g_Party.m_nLevel >= 10 )
	{
		strTemp.Format("%.2f %%", (float)g_Party.m_nExp * 100 / (float)( ( ( 50 + g_Party.GetLevel() ) * g_Party.GetLevel() / 13 ) * 10 ));
	}
	else
	{
		strTemp.Format("%.2f %%", (float)g_Party.m_nExp * 100 / (float)prj.m_aExpParty[g_Party.m_nLevel].Exp );
	}
	pWndStatic = (CWndStatic*)GetDlgItem( WIDC_EXP );
	pWndStatic->SetTitle(strTemp);
	strTemp.Format("%d",g_Party.m_nPoint);
	pWndStatic = (CWndStatic*)GetDlgItem( WIDC_POINT );
	pWndStatic->SetTitle(strTemp);
	// ¾ÆÀÌÅÛ °æÇèÄ¡ ºÐ¹è¹æ½Ä »óÅÂ °»½Å
	CWndButton* pWndButton;
	switch( g_Party.m_nTroupsShareExp) 
	{
	case 0:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_EXP_SHARE );
			pWndButton->SetCheck(TRUE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO6 );
			pWndButton->SetCheck(FALSE);
//			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO7 );
//			pWndButton->SetCheck(FALSE);
		}
		break;
	case 1:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_EXP_SHARE );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO6 );
			pWndButton->SetCheck(TRUE);
//			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO7 );
//			pWndButton->SetCheck(FALSE);
		}
		break;
	case 2:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_EXP_SHARE );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO6 );
			pWndButton->SetCheck(FALSE);
//			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO7 );
//			pWndButton->SetCheck(TRUE);
		}
		break;
	}
	switch(g_Party.m_nTroupeShareItem) 
	{
	case 0:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_ITEM_SHARE );
			pWndButton->SetCheck(TRUE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
			pWndButton->SetCheck(FALSE);
		}
		break;
	case 1:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_ITEM_SHARE );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
			pWndButton->SetCheck(TRUE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
			pWndButton->SetCheck(FALSE);
		}
		break;
	case 2:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_ITEM_SHARE );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
			pWndButton->SetCheck(TRUE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
			pWndButton->SetCheck(FALSE);
		}
		break;
	case 3:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_ITEM_SHARE );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
			pWndButton->SetCheck(TRUE);
		}
		break;
	}
} 
void CWndParty::OnInitialUpdate() 
{ 
//	Error( "CWndParty::OnInitialUpdate() - IN" );

	CWndNeuz::OnInitialUpdate(); 
	// ¿©±â¿¡ ÄÚµùÇÏ¼¼¿ä
	
#ifdef __SYS_PARTYFINDER	
	CWndButton* pRadio7 = (CWndButton*)GetDlgItem(WIDC_RADIO7);
	if (pRadio7 && g_Party.GetLeader())
	{
		pRadio7->SetCheck(g_Party.GetLeader()->m_bPartyJoin);
	}
#endif //__SYS_PARTYFINDER

	m_pWndLeave = (CWndButton*)GetDlgItem( WIDC_LEAVE );
	m_pWndChange = (CWndButton*)GetDlgItem( WIDC_CHANGE );
	m_pWndTransfer = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
#ifdef __PARTY_ASSISTANT
	assistant = (CWndButton*)GetDlgItem(WIDC_BUTTON3);
#endif //__PARTY_ASSISTANT
#if __VER >= 8 //__CSC_VER8_2
	m_pBtnPartyQuick = (CWndButton*)GetDlgItem( WIDC_BUTTON2 );
#else
	m_pBtnPartyQuick = (CWndButton*)GetDlgItem( WIDC_BUTTON2 );
	m_pBtnPartyQuick->EnableWindow(FALSE);
	m_pBtnPartyQuick->SetVisible(FALSE);
#endif //__CSC_VER8_2
	if( g_Party.GetSizeofMember() < 2 )
	{
		m_pWndTransfer->EnableWindow( FALSE );
		m_pWndLeave->EnableWindow( FALSE );
#ifdef __PARTY_ASSISTANT
		assistant->EnableWindow(FALSE);
#endif //__PARTY_ASSISTANT
#if __VER >= 8 //__CSC_VER8_2
		m_pBtnPartyQuick->EnableWindow( FALSE );
#endif	//__CSC_VER8_2
	}
	m_pWndChange->EnableWindow( FALSE );
	
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 15;

	//gmpbigsun( 100416 ) : Error Á¦°Å 
	//Error( "CWndParty::OnInitialUpdate() - m_wndPartyInfo.Create - IN" );
	m_wndPartyInfo.Create( WBS_CHILD | WBS_NOFRAME | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_PARTY_INFO );
	//Error( "CWndParty::OnInitialUpdate() - m_wndPartyInfo.Create - OUT" );

	//Error( "CWndParty::OnInitialUpdate() - m_wndPartySkill.Create - IN" );
	m_wndPartySkill.Create( WBS_CHILD | WBS_NOFRAME | WBS_NODRAWFRAME , rect, pWndTabCtrl, APP_PARTY_SKILL );
	//Error( "CWndParty::OnInitialUpdate() - m_wndPartySkill.Create - OUT" );

	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_APP_INFOMATION);//"Á¤º¸";
	tabTabItem.pWndBase = &m_wndPartyInfo;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );
	
	tabTabItem.pszText = prj.GetText(TID_APP_SKILL);//"½ºÅ³";
	tabTabItem.pWndBase = &m_wndPartySkill;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );

	MoveParentCenter();
//	Error( "CWndParty::OnInitialUpdate() - OUT" );
} 
// Ã³À½ ÀÌ ÇÔ¼ö¸¦ ºÎ¸£¸é À©µµ°¡ ¿­¸°´Ù.
BOOL CWndParty::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
//	Error( "CWndParty::Initialize() - IN" );
	BOOL bFlag = CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PARTY, 0, CPoint( 0, 0 ), pWndParent );
//	Error( "CWndParty::Initialize() - OUT" );

	// Daisy¿¡¼­ ¼³Á¤ÇÑ ¸®¼Ò½º·Î À©µµ¸¦ ¿¬´Ù.
	return bFlag;
} 
/*
  Á÷Á¢ À©µµ¸¦ ¿­¶§ »ç¿ë 
BOOL CWndParty::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndParty::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndParty::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndParty::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndParty::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndParty::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_LEAVE ) 
	{ // Å»Åð ´­·È´Ù...
		if( g_pPlayer->m_nDuel == 2 )		// ±Ø´Üµà¾óÁßÀÌ¸é Å»Åð ¾ÈµÊ.
		{
			DWORD dwText = TID_GAME_PPVP_LEAVE;
			g_WndMng.PutString( prj.GetText( dwText ), NULL, prj.GetTextColor( dwText ) );
		} 
		else
		{
			if(m_wndPartyInfo.m_nSelected!=-1) 
			{
				u_long  nLeaveMember = g_Party.m_aMember[m_wndPartyInfo.m_nSelected].m_uPlayerId; //Å»Åð½ÃÅ³³ð ID
				// ¿©±â´Ù°¡ Å»Åð
				if( g_Party.m_nSizeofMember != 0 )
				{
					CWndPartyLeaveConfirm* pWndPartyLeaveConfirm = (CWndPartyLeaveConfirm*)g_WndMng.GetWndBase( APP_PARTYLEAVE_CONFIRM );
					if( pWndPartyLeaveConfirm == NULL )
					{
						g_WndMng.m_pWndPartyLeaveConfirm = new CWndPartyLeaveConfirm;
						g_WndMng.m_pWndPartyLeaveConfirm->Initialize( NULL, APP_PARTYLEAVE_CONFIRM );
						if( g_Party.m_aMember[0].m_uPlayerId == g_pPlayer->m_idPlayer )
							g_WndMng.m_pWndPartyLeaveConfirm->SetLeaveId( nLeaveMember );
						else
							g_WndMng.m_pWndPartyLeaveConfirm->SetLeaveId( g_pPlayer->m_idPlayer );
					}
				}
				else
				{
					// ±Ø´ÜÀÌ ±¸¼ºÀÌ ¾ÈµÇ¾îÀÖÀ½
				}
			}
			else
			{
				CWndPartyLeaveConfirm* pWndPartyLeaveConfirm = (CWndPartyLeaveConfirm*)g_WndMng.GetWndBase( APP_PARTYLEAVE_CONFIRM );
				if( pWndPartyLeaveConfirm == NULL )
				{
					g_WndMng.m_pWndPartyLeaveConfirm = new CWndPartyLeaveConfirm;
					g_WndMng.m_pWndPartyLeaveConfirm->Initialize( NULL, APP_PARTYLEAVE_CONFIRM );
					if( g_Party.m_nSizeofMember != 0 )
						g_WndMng.m_pWndPartyLeaveConfirm->SetLeaveId( g_pPlayer->m_idPlayer );
				}
				// ³» ÀÚ½ÅÀ» ±Ø´Ü¿¡¼­ Å»Åð
			}
			m_wndPartyInfo.m_nSelected=-1;
		}
	}
	// ±Ø´ÜÀå ÀÎ°è.
	else if( nID==WIDC_BUTTON1 )
	{
		if( g_Party.m_nSizeofMember != 0 )
		{
			if( g_Party.IsLeader( g_pPlayer->m_idPlayer ) && m_wndPartyInfo.m_nSelected != -1 )
			{
				if( g_Party.m_aMember[m_wndPartyInfo.m_nSelected].m_bRemove == FALSE )
				{
					u_long  nLeadMember = g_Party.m_aMember[m_wndPartyInfo.m_nSelected].m_uPlayerId;
					if( g_Party.IsLeader( nLeadMember ) == FALSE )
					{
						if( g_pPlayer->m_nDuel == 2 )	// ±Ø´Ü µà¾óÁßÀÏ¶§´Â ±Ø´ÜÀåÀ» ¹Ù²Ü¼ö ¾øÀ½
							g_WndMng.PutString( prj.GetText( TID_PK_NOCHANGE_PARTYLEADER ), NULL, prj.GetTextColor( TID_PK_NOCHANGE_PARTYLEADER ) );
						else
							g_DPlay.SendPartyChangeLeader( g_pPlayer->m_idPlayer, nLeadMember );						
					}
					else
					{
						g_WndMng.PutString( prj.GetText( TID_GAME_PARTY_ALREADYMASTER ), NULL, prj.GetTextColor( TID_GAME_PARTY_ALREADYMASTER ) ); // "ÀÌ¹Ì ´ÜÀåÀ» ÇÏ°í ÀÔ´Ï´Ù"
					}
				}
				else
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_MASTER_AWAY ), NULL, prj.GetTextColor( TID_GAME_MASTER_AWAY ) ); // "´ÜÀåÀÌ ¾Æ´Ï°Å³ª ±Ø´ÜÃ¢¿¡¼­ ´Ü¿øÀ» ¼±ÅÃÀ» ÇØ¾ß ÇÕ´Ï´Ù"					
				}
			}
			else
			{
				g_WndMng.PutString( prj.GetText( TID_GAME_PARTY_ISNOTMASTER ), NULL, prj.GetTextColor( TID_GAME_PARTY_ISNOTMASTER ) ); // "´ÜÀåÀÌ ¾Æ´Ï°Å³ª ±Ø´ÜÃ¢¿¡¼­ ´Ü¿øÀ» ¼±ÅÃÀ» ÇØ¾ß ÇÕ´Ï´Ù"
			}
		}
	}
#ifdef __PARTY_ASSISTANT
	else if (nID == WIDC_BUTTON3)
	{
		if (g_Party.m_nSizeofMember != 0)
		{
			if (g_Party.IsLeader(g_pPlayer->m_idPlayer) && m_wndPartyInfo.m_nSelected != -1)
			{
				if (g_Party.m_aMember[m_wndPartyInfo.m_nSelected].m_bRemove == FALSE)
				{
					u_long AssistantMember = g_Party.m_aMember[m_wndPartyInfo.m_nSelected].m_uPlayerId;
					if (g_Party.IsLeader(AssistantMember) == FALSE)
					{
						if (g_pPlayer->m_nDuel == 2)
							g_WndMng.PutString("Unable to change the party assistants during a party duel.", NULL, prj.GetTextColor(TID_PK_NOCHANGE_PARTYLEADER));
						else
						{
							if (m_dwLastAssist + SEC(2) < GetTickCount())
							{
								g_DPlay.SendSetPartyAssistant(g_pPlayer->m_idPlayer, AssistantMember);
								m_dwLastAssist = GetTickCount();
							}
							else
							{
								g_WndMng.PutString("Please wait few seconds before using it again");
							}
						}
					}
					else
					{
						g_WndMng.PutString("You can't be a leader and an assistant at the same time.", NULL, prj.GetTextColor(TID_GAME_PARTY_ALREADYMASTER));
					}
				}
				else
				{
					g_WndMng.PutString("This player is out of the game, wait until he enters the game.", NULL, prj.GetTextColor(TID_GAME_MASTER_AWAY));
				}
			}
			else
			{
				g_WndMng.PutString("Only the Party Leader is able to transfer the Assistant Status to another member of the party. ", NULL, prj.GetTextColor(TID_GAME_PARTY_ISNOTMASTER));
			}
		}
	}
#endif //__PARTY_ASSISTANT
	else if( nID==WIDC_CHANGE )
	{
#ifdef __RENAMEPARTY
#ifdef __AEGON_FULLSCREEN_AND_RESIZE
		SAFE_DELETE(g_WndMng.m_pWndPartyChangeName);
		g_WndMng.m_pWndPartyChangeName = new CWndPartyChangeName;
		g_WndMng.m_pWndPartyChangeName->Initialize(NULL);	
#else
	SAFE_DELETE(m_WndPartyChangeName);
	m_WndPartyChangeName = new CWndPartyChangeName;
	m_WndPartyChangeName->Initialize(this);
#endif
	
	
#else //__RENAMEPARTY
		if( g_Party.m_nKindTroup == 0 /* && g_Party.m_nLevel == 10 */ )
		{
			SAFE_DELETE( m_WndPartyChangeTroup );
			m_WndPartyChangeTroup = new CWndPartyChangeTroup;
#ifdef __FIX_WND_1109
			m_WndPartyChangeTroup->Initialize( this );
#else	// __FIX_WND_1109
			m_WndPartyChangeTroup->Initialize();
#endif	// __FIX_WND_1109
		}
#endif //__RENAMEPARTY
	}
#if __VER >= 8 //__CSC_VER8_2
	else if(nID == WIDC_BUTTON2)
	{
		if(g_Party.GetSizeofMember() >= 2)
		{
			m_pWndPartyQuick = (CWndPartyQuick*)g_WndMng.GetApplet( APP_PARTY_QUICK );
			if(m_pWndPartyQuick == NULL)
			{
				m_pWndPartyQuick = (CWndPartyQuick*)g_WndMng.CreateApplet( APP_PARTY_QUICK );
				m_pWndPartyQuick->SetActiveMember(g_Party.GetSizeofMember());
			}
			else
			{
				m_pWndPartyQuick->Destroy();
				m_pWndPartyQuick = NULL;
			}
		}
	}
#endif //__CSC_VER8_2
	// ¼øÈ¸±Ø´ÜÀÏ¶§¸¸ ¾ÆÀÌÅÛ °æÇèÄ¡ ºÐ¹è¹æ½Ä °»½Å
	else
		if (g_Party.IsLeader(g_pPlayer->m_idPlayer)
#ifdef __PARTY_ASSISTANT
			|| (g_Party.IsMember(g_pPlayer->m_idPlayer) && g_pPlayer->GetAssist() == TRUE)
#endif //__PARTY_ASSISTANT
			)
		{
		if( g_Party.m_nKindTroup != 0 )
		{
			// °æÇèÄ¡ ºÐ¹è ¹æ½ÄÀº ¼øÈ¸±Ø´Ü ÀÓ
			int nTroupsShareExp = g_Party.m_nTroupsShareExp;
			
			if(nID==WIDC_EXP_SHARE) nTroupsShareExp   = 0;
			else if(nID==WIDC_RADIO6) nTroupsShareExp = 1;
	//		else if(nID==WIDC_RADIO7) nTroupsShareExp = 2;
			
			if( nTroupsShareExp == 2 )
			{
				g_WndMng.PutString(prj.GetText(TID_DIAG_0004), NULL, prj.GetTextColor(TID_DIAG_0004));
			}
			else
			{
				if( nTroupsShareExp != g_Party.m_nTroupsShareExp )
				{
					// Send
					g_DPlay.SendChangeShareExp( nTroupsShareExp );
				}
			}
		}
		// ¾ÆÀÌÅÛ ºÐ¹è ¹æ½ÄÀº ´Ü¸·±Ø´Ü ÀÓ
		int nTroupeShareItem = g_Party.m_nTroupeShareItem;

		if(nID==WIDC_ITEM_SHARE) nTroupeShareItem  = 0;
		else if(nID==WIDC_RADIO2) nTroupeShareItem = 1;
		else if(nID==WIDC_RADIO3) nTroupeShareItem = 2;
		else if(nID==WIDC_RADIO4) nTroupeShareItem = 3;		

		if( nTroupeShareItem != g_Party.m_nTroupeShareItem )
		{
			// Send
			g_DPlay.SendChangeShareItem( nTroupeShareItem );
			
		}
	}
	else
	{
		if( nID != WTBID_CLOSE && nID != WTBID_HELP )
			g_WndMng.PutString(prj.GetText(TID_DIAG_0008), NULL, prj.GetTextColor(TID_DIAG_0008));
	}

#ifdef __SYS_PARTYFINDER
	if (nID == WIDC_RADIO7 && g_Party.IsLeader(g_pPlayer->m_idPlayer))
	{
		CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_RADIO7);
		if (pWndButton)
		{
			pWndButton->SetCheck(!pWndButton->GetCheck());
			g_DPlay.SendPartyJoinAllow(pWndButton->GetCheck());
		}
	}
#endif //__SYS_PARTYFINDER

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

/****************************************************
  WndId : APP_PARTY_INFO - °³ÀÎºÐ¹è
  CtrlId : WIDC_GROUPBOX1 - ±Ø´ÜÁ¤º¸
  CtrlId : WIDC_GROUPBOX3 - °æÇèÄ¡ºÐ¹è
  CtrlId : WIDC_GROUPBOX4 - ¾ÆÀÌÅÛºÐ¹è
  CtrlId : WIDC_ITEM_SHARE - °³ÀÎºÐ¹è
  CtrlId : WIDC_RADIO2 - ¼ø¼­ºÐ¹è
  CtrlId : WIDC_RADIO3 - ¼öµ¿ºÐ¹è
  CtrlId : WIDC_RADIO4 - ·£´øºÐ¹è
  CtrlId : WIDC_EXP_SHARE - µ¿ÀÏºÐ¹è
  CtrlId : WIDC_RADIO6 - ±â¿©µµºÐ¹è
  CtrlId : WIDC_RADIO7 - ÇÇÇØºÐ¹è
  CtrlId : WIDC_STATIC1 - ±Ø´Ü¸íÄª :
  CtrlId : WIDC_STATIC2 - ±Ø´Ü·¹º§ :
  CtrlId : WIDC_STATIC3 - ±Ø´Ü°æÇèÄ¡ :
  CtrlId : WIDC_STATIC4 - ±Ø´ÜÆ÷ÀÎÆ® :
  CtrlId : WIDC_NAME - ¸¶½ºÄ¿·¹ÀÌµå
  CtrlId : WIDC_LEVEL - 1
  CtrlId : WIDC_EXP - 90
  CtrlId : WIDC_POINT - 0
****************************************************/

#ifdef __AEGON_FIXES
#define PARTY_Y_OFFSET 18
#endif

CWndPartyInfo::CWndPartyInfo() 
{ 
	m_nSelected=-1;
	m_pVBGauge = NULL;
} 
CWndPartyInfo::~CWndPartyInfo() 
{ 
	DeleteDeviceObjects();
} 
void CWndPartyInfo::OnDraw( C2DRender* p2DRender ) 
{
#ifdef __AEGON_THEME_SWITCHER
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	DWORD RenderFillRect = CAegonThemeSwitcherMng::GetInstance()->GetRenderFillRectColor();
	DWORD RenderRect = CAegonThemeSwitcherMng::GetInstance()->GetRenderRectColor();
	DWORD dwColorHighlightBlue = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE);
	DWORD dwColorHighlightRed = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_RED);
	DWORD dwColorHighlightYellow = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_YELLOW);
	DWORD dwColorHighlightGreen = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_GREEN);
#endif
	CString strTemp;
	
	if( m_nSelected != -1 ) 
	{
#ifdef __AEGON_FIXES
		CRect rect(5, 9 + m_nSelected * PARTY_Y_OFFSET, 410, 9 + 1 + PARTY_Y_OFFSET + m_nSelected * PARTY_Y_OFFSET);
#ifdef __AEGON_THEME_SWITCHER
		p2DRender->RenderFillRect(rect, RenderFillRect);
		p2DRender->RenderRect(rect, RenderRect);
#else //__AEGON_THEME_SWITCHER
		p2DRender->RenderFillRect(rect, 0xFFf8e6cc);
		p2DRender->RenderRect(rect, 0xFFf28818);
#endif //__AEGON_THEME_SWITCHER
#else
		CRect rect(5, 8 + m_nSelected * 17, 410, 22 + m_nSelected * 17);
		p2DRender->RenderFillRect(rect, 0xFFf8e6cc);
		p2DRender->RenderRect(rect, 0xFFf28818);
#endif
	}
	if( g_Party.GetSizeofMember() < m_nSelected + 1 ) 
	{
		m_nSelected = -1;
	}
	int y = 10;

#if __VER >= 8 //__CSC_VER8_2
	DWORD dwColor;
	for( int i = 0; i < g_Party.m_nSizeofMember; i++ ) 
	{		
#ifdef __AEGON_THEME_SWITCHER
		dwColor = dwColorText;
#else
		dwColor = 0xff000000;
#endif
#else 
	DWORD dwColor = 0xff000000;
	for( int i = 0; i < g_Party.m_nSizeofMember; i++ ) 
	{
#endif //__CSC_VER8_2
		CMover* pObjMember	= prj.GetUserByID( g_Party.m_aMember[ i ].m_uPlayerId );

		CRect rectTemp,rect;
		CString strClass(prj.GetText(TID_GAME_MEMBER));
		CString strName;
#ifdef __PARTY_ASSISTANT
		u_long  nPartyAssist = g_Party.m_aMember[i].m_uPlayerId;
		PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(g_Party.m_aMember[i].m_uPlayerId);
		BOOL nAssist = pPlayerData->data.nAssist;
		if (g_Party.IsLeader(nPartyAssist))
		{
			strClass = "L";
		}
		else
		{
			if (nAssist == 1)
			{
				strClass = "A";
			}
			else
			{
				strClass = "M";
			}
		}
#else //__PARTY_ASSISTANT
		if (i == 0) strClass = prj.GetText(TID_GAME_LEADER);
#endif //__PARTY_ASSISTANT
#ifdef __AEGON_FIXES
		int nRectSize = 10;
		int nYOffset = floor((double)(1 + PARTY_Y_OFFSET - nRectSize) / (double)2);
		rectTemp = CRect(10, y + nYOffset, 20, y + nRectSize + nYOffset);
		p2DRender->RenderFillRect(rectTemp, dwColor);
		rectTemp = CRect(11, y + 1 + nYOffset, 19, y + (nRectSize - 1) + nYOffset);
#else
		rectTemp = CRect( 10, y, 20, y + 10 );
		p2DRender->RenderFillRect( rectTemp, dwColor );
		rectTemp = CRect( 11, y + 1, 19, y + 9 );
#endif
#ifdef __AEGON_THEME_SWITCHER
		DWORD colorStatus = dwColorHighlightBlue;
#else
		DWORD colorStatus = 0xff0000ff; // µðÆúÆ®´Â ÀÌ»ó¾øÀ½
#endif
		if( IsValidObj(pObjMember) )
		{
			if( pObjMember->GetHitPoint() == 0 ) 
#ifdef __AEGON_THEME_SWITCHER
				colorStatus = dwColorHighlightRed;
#else
				colorStatus = COLOR_ERROR; // Á×Àº³ð
#endif
			else 
			{
				if( pObjMember->GetMaxHitPoint() > 0 && ( (FLOAT)pObjMember->GetHitPoint() ) / ( (FLOAT)pObjMember->GetMaxHitPoint() ) <.1f ) 
#ifdef __AEGON_THEME_SWITCHER
					colorStatus = dwColorHighlightYellow;
#else
					colorStatus = 0xffffff00; // HP 10% ÀÌÇÏÀÎ³ð
#endif
			}
		}
		else
		{
			colorStatus = 0xff878787; // µðÆúÆ®´Â ÁÖÀ§¿¡ ¾ø´Â³ð
			if (g_Party.m_aMember[i].m_bRemove)
#ifdef __AEGON_THEME_SWITCHER
				colorStatus = dwColorText;
#else
			colorStatus = 0xff000000; // ¼­¹ö¿¡ ¾ø´Â³ð
#endif
		}
		p2DRender->RenderFillRect( rectTemp, colorStatus );

#if __VER >= 8 //__CSC_VER8_2
		u_long  nLeadMember = g_Party.m_aMember[i].m_uPlayerId;
		if(g_Party.IsLeader(nLeadMember)) //Leader Color Set
#ifdef __AEGON_THEME_SWITCHER
			dwColor = dwColorHighlightGreen;
#else
			dwColor = 0xff1fb72d;
#endif
#ifdef __PARTY_ASSISTANT
		else
		{
			if (nAssist && nAssist == 1)
			{
#ifdef __AEGON_THEME_SWITCHER
				dwColor = dwColorHighlightBlue;
#else
				dwColor = 0xff002bff;
#endif
			}
		}
#endif //__PARTY_ASSISTANT
#endif	//__CSC_VER8_2	
		BOOL bSummary	= FALSE;
		int nLang	= ::GetLanguage();
		switch( nLang )
		{
			case LANG_GER:
			case LANG_FRE:
				bSummary	= TRUE;
				break;
		}

		// ·¹º§ °è±Þ Á÷¾÷
		if( IsValidObj(pObjMember) )
		{
#if __VER >= 10 // __LEGEND
			if( MAX_PROFESSIONAL <= pObjMember->GetJob() && pObjMember->GetJob() < MAX_MASTER )
				strTemp.Format( "%d%s", pObjMember->GetLevel(), prj.GetText( TID_GAME_TOOLTIP_MARK_MASTER ) );
			else if( MAX_MASTER <= pObjMember->GetJob() )
#if __VER >= 15 // __HERO129_VER15 // 15Â÷ È÷¾î·Î ·¹º§È®Àå
				strTemp.Format( "%d%s", pObjMember->GetLevel(), prj.GetText( TID_GAME_TOOLTIP_MARK_HERO ) );
#else // 15Â÷ È÷¾î·Î ·¹º§È®Àå
				strTemp = prj.GetText( TID_GAME_TOOLTIP_MARK_HERO_BEFORE );
#endif // 15Â÷ È÷¾î·Î ·¹º§È®Àå
			else 
				strTemp.Format( "%d", pObjMember->GetLevel() );
			p2DRender->TextOut( 30, y, 1.0f, 1.0f, strTemp.GetString(), dwColor );
			if( !bSummary )
			{
				strTemp.Format( "%s", strClass.GetString() );
				p2DRender->TextOut( 80, y, 1.0f, 1.0f, strTemp, dwColor );
				strTemp.Format( "%s", pObjMember->GetJobString() );
#ifdef __SYS_PARTYFINDER
				p2DRender->TextOut(210, y, 1.0f, 1.0f, strTemp, dwColor);
#else // __SYS_PARTYFINDER
				p2DRender->TextOut(120, y, 1.0f, 1.0f, strTemp, dwColor);
#endif // __SYS_PARTYFINDER
			}
			else
			{
				strTemp.Format( "%s", pObjMember->GetJobString() );
				p2DRender->TextOut( 210, y, 1.0f, 1.0f, strTemp, dwColor );
			}
			strName		= pObjMember->GetName();
#else //__LEGEND
			strTemp.Format( "%d", pObjMember->GetLevel() );
			p2DRender->TextOut( 30, y, 1.0f, 1.0f, strTemp, dwColor );
			if( !bSummary )
			{
				strTemp.Format( "%s", strClass );
				p2DRender->TextOut( 50, y, 1.0f, 1.0f, strTemp, dwColor );
				strTemp.Format( "%s", pObjMember->GetJobString() );
				p2DRender->TextOut( 80, y, 1.0f, 1.0f, strTemp, dwColor );
			}
			else
			{
				strTemp.Format( "%s", pObjMember->GetJobString() );
				p2DRender->TextOut( 50, y, 1.0f, 1.0f, strTemp, dwColor );
			}
			strName = pObjMember->GetName();
#endif //__LEGEND
#ifdef __LEESE_PARTY_LOCATION
			CString strPartyLocation;
			strPartyLocation.Format("%s", pObjMember->GetWorld()->m_szWorldName);
			p2DRender->TextOut(305, y, 1.0f, 1.0f, strPartyLocation, dwColor);
#endif //__LEESE_PARTY_LOCATION
		}
		else
		{
#if __VER >= 10 // __LEGEND
#if __VER >= 11 // __SYS_PLAYER_DATA
			PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( g_Party.m_aMember[i].m_uPlayerId );
			int nLevel	= pPlayerData->data.nLevel;
			int nJob	= pPlayerData->data.nJob;
#else	// __SYS_PLAYER_DATA
			int nLevel	= g_Party.m_aMember[i].m_nLevel;
			int nJob	= g_Party.m_aMember[i].m_nJob;
#endif	// __SYS_PLAYER_DATA
			if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER )
				strTemp.Format( "%d%s", nLevel, prj.GetText( TID_GAME_TOOLTIP_MARK_MASTER ) );
			else if( MAX_MASTER <= nJob )
#if __VER >= 15 // __HERO129_VER15 // 15Â÷ È÷¾î·Î ·¹º§È®Àå
				strTemp.Format( "%d%s", nLevel, prj.GetText( TID_GAME_TOOLTIP_MARK_HERO ) );
#else // 15Â÷ È÷¾î·Î ·¹º§È®Àå
				strTemp = prj.GetText( TID_GAME_TOOLTIP_MARK_HERO_BEFORE );
#endif // 15Â÷ È÷¾î·Î ·¹º§È®Àå
			else 
				strTemp.Format( "%d", nLevel );
			p2DRender->TextOut(30,y,1.0f,1.0f,strTemp,dwColor);
			if( !bSummary )
			{
				strTemp.Format("%s",strClass.GetString());
				p2DRender->TextOut(80,y,1.0f,1.0f,strTemp,dwColor);
				strTemp.Format("%s",prj.m_aJob[nJob].szName);
#ifdef __SYS_PARTYFINDER
				p2DRender->TextOut(210, y, 1.0f, 1.0f, strTemp, dwColor);
#else // __SYS_PARTYFINDER
				p2DRender->TextOut(120, y, 1.0f, 1.0f, strTemp, dwColor);
#endif // __SYS_PARTYFINDER
			}
			else
			{
				strTemp.Format( "%s", prj.m_aJob[nJob].szName );
				p2DRender->TextOut(210, y, 1.0f, 1.0f, strTemp, dwColor );
			}
#if __VER >= 11 // __SYS_PLAYER_DATA
			strName		= pPlayerData->szPlayer;
#else	// __SYS_PLAYER_DATA
			strName		= g_Party.m_aMember[i].m_szName;
#endif	// __SYS_PLAYER_DATA
#else //__LEGEND
			strTemp.Format("%d",g_Party.m_aMember[i].m_nLevel);
			p2DRender->TextOut(30,y,1.0f,1.0f,strTemp,dwColor);
			if( !bSummary )
			{
				strTemp.Format("%s",strClass);
				p2DRender->TextOut(50,y,1.0f,1.0f,strTemp,dwColor);
				strTemp.Format("%s",prj.m_aJob[g_Party.m_aMember[i].m_nJob].szName);
				p2DRender->TextOut(80,y,1.0f,1.0f,strTemp,dwColor);
			}
			else
			{
				strTemp.Format( "%s", prj.m_aJob[g_Party.m_aMember[i].m_nJob].szName );
				p2DRender->TextOut( 50,y,1.0f,1.0f,strTemp,dwColor );
			}
			strName		= g_Party.m_aMember[i].m_szName;
#endif //__LEGEND
#ifdef __LEESE_PARTY_LOCATION
			if (pPlayerData)
			{
				CString strPartyLocation;
				auto playerLocation = g_WorldMng.GetWorldStruct(pPlayerData->data.nLocation);
				if (playerLocation)
				{
					strPartyLocation.Format("%s", playerLocation->m_szWorldName);
					p2DRender->TextOut(305, y, 1.0f, 1.0f, strPartyLocation, dwColor);
				}
			}
#endif //__LEESE_PARTY_LOCATION
		}
		// ÀÌ¸§Àº 10¹ÙÀÌÆ®Â°¿¡¼­ Â¥¸¥´Ù
#ifndef __SYS_PARTYFINDER
		if (strName.GetLength() > 8)
		{
			int	nReduceCount = 0;

			for (nReduceCount = 0; nReduceCount < 8; )
			{
				if (IsDBCSLeadByte(strName[nReduceCount]))
					nReduceCount += 2;
				else
					nReduceCount++;
			}

			strName = strName.Left(nReduceCount);
			strName += "...";
		}
#endif // __SYS_PARTYFINDER
		strTemp.Format("%s",strName.GetString());

		p2DRender->TextOut(130, y, 1.0f, 1.0f, strTemp, dwColor);
		
#ifdef __PARTY_ASSISTANT
		CPoint cpAdd = CPoint(100, y);
		if (!g_Party.IsLeader(nPartyAssist))
		{
			if (nAssist == TRUE)
			{
				if (pTexture1)
				{
					p2DRender->RenderTexture(cpAdd, pTexture1);
				}
			}
			else
			{
				if (pTexture2)
				{
					p2DRender->RenderTexture(cpAdd, pTexture2);
				}
			}
		}
		else
		{
			if (pTexture3)
			{
				p2DRender->RenderTexture(cpAdd, pTexture3);
			}
		}
#endif //__PARTY_ASSISTANT

#ifndef __LEESE_PARTY_LOCATION
		int nWidth	= ( IsValidObj( pObjMember ) ? 110 * pObjMember->GetHitPoint() / pObjMember->GetMaxHitPoint() : 0 );
		if( !bSummary )
			rect.SetRect( 295, y, 295 + 110, y + 12 );
		else
			rect.SetRect( 310, y, 310 + 110, y + 12 );

		rectTemp = rect; 
		rectTemp.right = rectTemp.left + nWidth;

		m_pTheme->RenderGauge( p2DRender, &rect, 0xffffffff, m_pVBGauge, &m_texGauEmptyNormal );
		if( IsValidObj(pObjMember) )
			m_pTheme->RenderGauge( p2DRender, &rectTemp, 0x64ff0000, m_pVBGauge, &m_texGauFillNormal );
#endif //__LEESE_PARTY_LOCATION
#ifdef __AEGON_FIXES
		y+= PARTY_Y_OFFSET;
#else
		y += 15;
#endif
	}
} 
HRESULT CWndPartyInfo::RestoreDeviceObjects()
{
	CWndNeuz::RestoreDeviceObjects();
	if( m_pVBGauge == NULL )
		return m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	return S_OK;
}
HRESULT CWndPartyInfo::InvalidateDeviceObjects()
{
	CWndNeuz::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
HRESULT CWndPartyInfo::DeleteDeviceObjects()
{
	CWndNeuz::DeleteDeviceObjects();
	InvalidateDeviceObjects();
	return S_OK;
}

void CWndPartyInfo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ¿©±â¿¡ ÄÚµùÇÏ¼¼¿ä

	RestoreDeviceObjects();

#ifdef __PARTY_ASSISTANT
	pTexture1 = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "Icon_GuildGeneral.dds"), COLOR_ITEM_BG);
	pTexture2 = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "Icon_GuildRecrue.dds"), COLOR_ITEM_BG);
	pTexture3 = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "Icon_GuildLeader.dds"), COLOR_ITEM_BG);
#endif //__PARTY_ASSISTANT

	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), COLOR_ITEM_BG, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), COLOR_ITEM_BG, TRUE );
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 

BOOL CWndPartyInfo::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy¿¡¼­ ¼³Á¤ÇÑ ¸®¼Ò½º·Î À©µµ¸¦ ¿¬´Ù.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PARTY_INFO, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndPartyInfo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPartyInfo::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPartyInfo::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPartyInfo::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
#if __VER >= 11 // __CSC_VER11_2
	CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
	if(((CWndWorld*)g_WndMng.m_pWndWorld)->m_bAutoAttack || pTaskBar->m_nExecute != 0)
		return;
#endif //__CSC_VER11_2
	
	// ÆÄÆ¼Ã¢¿¡¼­ ¾î¶²³ð ´­·¶´ÂÁö »öÃâ
#ifdef __AEGON_FIXES
	CRect rect(10, 10, 365, 10 + g_Party.GetSizeofMember() * PARTY_Y_OFFSET);
	if (rect.PtInRect(point))
		m_nSelected = (point.y - 10) / PARTY_Y_OFFSET;
#else
	CRect rect( 10, 10, 365, 10 + g_Party.GetSizeofMember() * 15 );
	if( rect.PtInRect( point ) ) 
		m_nSelected = ( point.y - 10 ) / 15;
#endif
	else 
		m_nSelected = -1;
	if( m_nSelected != -1 ) 
	{ // ¼±ÅÃµÈ³ð ÀÖÀ¸¸é
		// ±×³ðÀ» Å¸°ÙÀ¸·Î
#if __VER >= 8 //__CSC_VER8_2
		((CWndWorld*)g_WndMng.m_pWndWorld)->m_pSelectRenderObj = NULL; //¸¶¿ì½º¿¡ °É·Á ±×·ÁÁø³ðÀ» Áö¿ì°í Å¸°ÙÀ» Àâ¾Æ¾ß ¾È°ãÄ£´Ù..
#endif //__CSC_VER8_2
		CMover* pObjMember = prj.GetUserByID( g_Party.m_aMember[m_nSelected].m_uPlayerId );
		if(g_pPlayer!=pObjMember) 
		{
			if(IsValidObj(pObjMember)) 
			{ // È­¸é¿¡ ¾ø´Â³ðÀº ÆÐ½º...
				g_WorldMng()->SetObjFocus(pObjMember);
				CWndWorld* pWndWorld = g_WndMng.m_pWndWorld;
				if(pWndWorld)
					pWndWorld->m_pRenderTargetObj = NULL;
			}
		}
#if __VER >= 8 //__CSC_VER8_2
		else
			g_WorldMng()->SetObjFocus( NULL );
#endif //__CSC_VER8_2
	}
	else 
	{
		// ¾øÀ¸¸é Å¸°Ù Áö¿î´Ù
		g_WorldMng()->SetObjFocus(NULL);
	}
} 
BOOL CWndPartyInfo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/****************************************************
  WndId : APP_PARTY_SKILL - ±Ø´Ü½ºÅ³
****************************************************/

CWndPartySkill::CWndPartySkill() 
{ 
	ZeroMemory( m_atexSkill, sizeof( m_atexSkill ) );
	m_nSkillSelect = -1;
} 
CWndPartySkill::~CWndPartySkill() 
{ 
} 
void CWndPartySkill::OnDraw( C2DRender* p2DRender ) 
{ 
#ifdef __AEGON_THEME_SWITCHER
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
#endif
//	if( g_Party.m_nKindTroup )
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 3;
		int nHeight = rect.Height() / 3;
		int nCount = 0;
		for( int i = 0; i < 3; i++ )
		{
			for( int j = 0; j < 3; j++, nCount++ )
			{
				ItemProp* pItemProp =  prj.GetPartySkill( nCount + 1 );
				if( pItemProp ) //&& g_Party.m_nSizeofMember >= 2 )
				{
					m_atexSkill[ i * 3 + j ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pItemProp->szIcon/*pItemBase->GetProp()->szIcon*/), COLOR_ITEM_BG, FALSE );
					CPoint point1 = CPoint( j * nWidth + 35 , i * nHeight + 6 );
					CPoint point2 = CPoint( j * nWidth + 3, i * nHeight + 3 );
					
					if( g_Party.m_nKindTroup && int(g_Party.GetLevel() - pItemProp->dwReqDisLV) >= 0 )
					{
						if( (g_Party.GetPoint() - pItemProp->dwExp) >= 0 )
						{
							// »ç¿ë°¡´ÉÇÑ ±â¼ú
#ifdef __AEGON_THEME_SWITCHER
							p2DRender->TextOut(point1.x, point1.y, pItemProp->szName, dwColorText);
#else
							p2DRender->TextOut(point1.x, point1.y, pItemProp->szName, 0xff000000);
#endif
							//m_aSlotQueue[nIndex].m_pTexture = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, lpSkill->GetProp()->szIcon/*pItemBase->GetProp()->szIcon*/), COLOR_ITEM_BG, FALSE );
							p2DRender->RenderTexture( point2, m_atexSkill[ i * 3 + j ] );
							
						}
						else
						{
							// ±â¼úÀº ÀÍÇûÀ¸³ª Æ÷ÀÎÆ®°¡ ¸ðÀÚ¶ó »ç¿ëÀ» ¸øÇÔ
							p2DRender->TextOut(point1.x, point1.y, pItemProp->szName, 0xff0000ff);
							p2DRender->RenderTexture( point2, m_atexSkill[ i * 3 + j ] );
							
						}
					}
					else
					{
						// ±â¼úÀ» ¸øÀÍÇûÀ½
						p2DRender->TextOut( point1.x, point1.y, pItemProp->szName, COLOR_ERROR );
						p2DRender->RenderTexture( point2, m_atexSkill[ i * 3 + j ], 54 );		
					}
				}
			}
		}
	}
} 
void CWndPartySkill::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 3;
	int nHeight = rect.Height() / 3;
	point.x /= nWidth; 
	point.y /= nHeight;
	int nSkill = point.y * 3 + point.x;
	
	ItemProp* pItemProp =  prj.GetPartySkill( nSkill + 1 );
	if( pItemProp )
	{
		m_nSkillSelect = nSkill;
		return;
	}
	m_nSkillSelect = -1;
} 
void CWndPartySkill::OnMouseWndSurface( CPoint point )
{
	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 3;
	int nHeight = rect.Height() / 3;
	int nCount = 0;
	for( int i = 0; i < 3; i++ )
	{
		for( int j = 0; j < 3; j++, nCount++ )
		{
			ItemProp* pItemProp =  prj.GetPartySkill( nCount + 1 );
			if( pItemProp ) //&& g_Party.m_nSizeofMember >= 2 )
			{
				CRect rect( j * nWidth + 3, i * nHeight + 3, j * nWidth + 3 + nWidth, i * nHeight + 3 + nHeight);
				if( rect.PtInRect( point ) )
				{
					CPoint point2 = point;
					ClientToScreen( &point2 );
					ClientToScreen( &rect );
					
					g_WndMng.PutToolTip_Troupe( i * 3 + j + 1, point2, &rect );
				}
			}
		}
	}
}	
void CWndPartySkill::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	m_nSkillSelect = -1;
}

void CWndPartySkill::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_nSkillSelect == -1 || IsPush() == FALSE )
		return;
	//	TRACE( " hello \n" );
	CPoint pt( 3, 3 );
	CRect rect;
	//if( m_rect.PtInRect( point ) )
	//{
	//DWORD dwSkill = m_apJobSkill[ m_nCurSelect ].dwSkill;
	ItemProp* pItemProp =  prj.GetPartySkill( m_nSkillSelect + 1 );
	if( pItemProp ) //&& g_Party.m_nSizeofMember >= 2 )
	{
		if( int(g_Party.GetLevel() - pItemProp->dwReqDisLV) >= 0 )
		{
			//CItemElem* pItemElem = &m_pItemContainer->m_apItem[ m_pItemContainer->m_apIndex[ m_nCurSelect ] ] ;
			m_GlobalShortcut.m_dwShortcut  = SHORTCUT_SKILL;
			m_GlobalShortcut.m_dwType  = 2;//m_nJob;//(DWORD)pItemElem;//->m_dwItemId;
			m_GlobalShortcut.m_dwIndex = m_nSkillSelect + 1;//m_nSkillSelect;//m_nCurSelect;//(DWORD)pItemElem;//->m_dwItemId;
			m_GlobalShortcut.m_dwData  = 0;//pItemElem->m_dwObjId;//(DWORD)pItemElem;
			m_GlobalShortcut.m_dwId     = m_nSkillSelect + 1; // ÄÃ·±Æ® ¼¿·ºÆ®°¡ °ð ID³ª ¸¶Âù°¡ÁöÀÓ.
			m_GlobalShortcut.m_pTexture = m_atexSkill[ m_nSkillSelect ];//L;//pItemElem->m_pTexture;
			_tcscpy( m_GlobalShortcut.m_szString, pItemProp->szName);
			//}
		}
	}
}

void CWndPartySkill::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ¿©±â¿¡ ÄÚµùÇÏ¼¼¿ä
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// Ã³À½ ÀÌ ÇÔ¼ö¸¦ ºÎ¸£¸é À©µµ°¡ ¿­¸°´Ù.
BOOL CWndPartySkill::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy¿¡¼­ ¼³Á¤ÇÑ ¸®¼Ò½º·Î À©µµ¸¦ ¿¬´Ù.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PARTY_SKILL, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  Á÷Á¢ À©µµ¸¦ ¿­¶§ »ç¿ë 
BOOL CWndPartySkill::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndPartySkill::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPartySkill::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPartySkill::OnLButtonDblClk( UINT nFlags, CPoint point)
{
//	if( g_Party.m_nSizeofMember >= 2 && g_Party.m_nKindTroup == 1 )
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 3;
		int nHeight = rect.Height() / 3;
		point.x /= nWidth; 
		point.y /= nHeight;
		int nSkill = point.y * 3 + point.x;
		
		ItemProp* pItemProp =  prj.GetPartySkill( nSkill + 1 );
		if( pItemProp )
		{
			g_WndMng.ObjectExecutor( SHORTCUT_SKILL, nSkill + 1, 2 );
		}
	}
}
BOOL CWndPartySkill::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
