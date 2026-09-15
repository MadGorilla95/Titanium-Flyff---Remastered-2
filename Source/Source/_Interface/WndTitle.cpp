// WndArcane.cpp: implementation of the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "defineObj.h"
#include "defineText.h"
#include "AppDefine.h"
#include "DPLoginClient.h"
#include "DPClient.h"
#include "dpCertified.h"
#include "..\_Common\Debug.h"
#include "webbox.h"

#include "WndManager.h"
#include "Network.h"

#ifdef __AEGON_ADVANCED_LOGIN
#include "tools.h"
#endif // __AEGON_ADVANCED_LOGIN

extern	CDPLoginClient	g_dpLoginClient;
extern	CDPClient	g_DPlay;
extern	CDPCertified	g_dpCertified;

extern BYTE  nMaleHairColor[10][3];
extern BYTE  nFeMaleHairColor[10][3];

#ifdef __CERTIFIER_COLLECTING_SYSTEM
#include "DPCollectClient.h"
#endif // __CERTIFIER_COLLECTING_SYSTEM

BOOL GetIePath( LPSTR lpPath )
{	
	LONG result;
	HKEY hKey;
	DWORD dwType; 
	char data[MAX_PATH];
	DWORD dataSize = MAX_PATH+1;

	result = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, "\\Software\\Clients\\StartMenuInternet\\", 0, KEY_QUERY_VALUE, &hKey );
	if (result == ERROR_SUCCESS) 
	{
		result = ::RegQueryValueEx ( hKey, "Path", NULL, &dwType, (unsigned char *)data, &dataSize );
		strcpy( lpPath, data );
		lpPath[lstrlen( lpPath )-1]	= '\0';
	}
	else
		return FALSE;

	RegCloseKey( hKey );
	return TRUE;
}

BOOL CWndConnectingBox::Initialize( CWndBase* pWndParent, DWORD nType  ) 
{
	CRect rect = m_pWndRoot->MakeCenterRect( 250, 130 );
/*
	Create( _T( "매시지 박스" ), MB_CANCEL, rect, APP_MESSAGEBOX );
	m_wndText.SetString( _T( "접속중입니다. 잠시만 기다려 주십시오." ) );
*/
	Create( _T( prj.GetText(TID_DIAG_0068) ), /*MB_CANCEL*/0xFFFFFFFF, rect, APP_MESSAGEBOX );
	m_wndText.SetString( _T( prj.GetText(TID_DIAG_0064) ) );
	
	m_wndText.ResetString();
	return CWndMessageBox::Initialize( pWndParent, 0 );
}
BOOL CWndConnectingBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	return TRUE;
}

BOOL CWndCharBlockBox::Initialize( CWndBase* pWndParent, DWORD nType  ) 
{
	CRect rect = m_pWndRoot->MakeCenterRect( 250, 130 );
/*
	Create( _T( "매시지 박스" ), MB_CANCEL, rect, APP_MESSAGEBOX );
	m_wndText.SetString( _T( "사용할수 없는 캐릭터 입니다" ) );
*/
	Create( _T( prj.GetText(TID_DIAG_0068) ), MB_CANCEL, rect, APP_MESSAGEBOX );
	m_wndText.SetString( _T( prj.GetText(TID_DIAG_0073) ) );

	m_wndText.ResetString();
	return CWndMessageBox::Initialize( pWndParent, 0 );
}
BOOL CWndCharBlockBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if( message == WNM_CLICKED  )
	{
		switch(nID)
		{
		case IDCANCEL:   
			//Destroy(); 
			break;
		}
	}
	return CWndMessageBox::OnChildNotify( message, nID, pLResult );
}
BOOL CWndAllCharBlockBox::Initialize( CWndBase* pWndParent, DWORD nType  ) 
{
    CRect rect = m_pWndRoot->MakeCenterRect( 250, 130 );
/*
	Create( _T( "매시지 박스" ), MB_CANCEL, rect, APP_MESSAGEBOX );
	m_wndText.SetString( _T( "접속할수 없는 계정입니다" ) );
*/
	Create( _T( prj.GetText(TID_DIAG_0068) ), MB_CANCEL, rect, APP_MESSAGEBOX );
	m_wndText.SetString( _T( prj.GetText(TID_DIAG_0074) ) );

	m_wndText.ResetString();
	return CWndMessageBox::Initialize( pWndParent, 0 );
}
BOOL CWndAllCharBlockBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if( message == WNM_CLICKED  )
	{
		switch(nID)
		{
		case IDCANCEL:   
			{
				g_dpLoginClient.DeleteDPObject();
				CWndSelectChar* pWndSelectChar 
					= (CWndSelectChar*)g_WndMng.GetWndBase( APP_SELECT_CHAR );
				if( pWndSelectChar )
				{
					pWndSelectChar->Destroy();
				}
				g_dpCertified.SendCertify();
#ifdef __AEGON_ADVANCED_LOGIN
				g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_AEGON_LOGIN);
				CWndBase* pWndBase = g_WndMng.GetWndBase(APP_AEGON_LOGIN);
#else // __AEGON_ADVANCED_LOGIN
				g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_LOGIN);
				CWndBase* pWndBase = g_WndMng.GetWndBase(APP_LOGIN);
#endif // __AEGON_ADVANCED_LOGIN				
			}
			//Destroy(); 
			break;
		}
	}
	return CWndMessageBox::OnChildNotify( message, nID, pLResult );
}

//////////////////////////////////////////////////////////////////////////////////
// Login
//////////////////////////////////////////////////////////////////////////////////

CWndLogin::CWndLogin()
{
	m_bNoCloseButton = TRUE;
	SetPutRegInfo( FALSE );
#ifdef __NPKCRYPT
	m_hKCrypt	= NULL;
	TCHAR	szFileName[MAX_PATH]	= {0};
	TCHAR	szGameFileName[MAX_PATH]	= {0};
	BOOL	bStatus	= FALSE;
	char szPath[MAX_PATH];
	if( GetCurrentDirectory( MAX_PATH, szPath ) == 0 ) 
		return;
	
	sprintf( (char*)szGameFileName, "%s\\neuz.exe", szPath );
	if( NPKGetAppCompatFlag( szGameFileName ) != apcfNone )
    {
		OutputDebugString("--> ReStart Program\n");
    }
    else
	{
		OutputDebugString("--> Load KeyCrypt\n");
		bStatus		= NPKGetLoadStartup();

		GetModuleFileName( NULL, szFileName, sizeof(szFileName) );
		char *ptr = strrchr( szFileName, '\\' );
		if( ptr != NULL )
			*ptr	= 0;

		NPKSetDrvPath( szFileName );
		m_hKCrypt	= NPKOpenDriver();

		if(m_hKCrypt <= 0)
		{
			HLOCAL	hlocal = NULL;
			DWORD	dwError = GetLastError();
			CString	strErrMsg = "";
			
			BOOL fOk = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 
				NULL, 
				dwError, 
				MAKELANGID(LANG_KOREAN, SUBLANG_ENGLISH_US), 
				(PTSTR) &hlocal, 
				0, 
				NULL);
			
			if(fOk)
			{
				strErrMsg = (PCTSTR) LocalLock(hlocal);
				::LocalFree(hlocal);
			}
			else
			{
				switch(dwError)
				{
				case NPK_ERROR_NOTADMIN:
					break;

				case NPK_ERROR_DRIVERVERSION:
//					strErrMsg.LoadString(IDS_ERR_DRIVERVERSION);				
					break;

				case NPK_ERROR_VERIFYVERSION:
//					strErrMsg.LoadString(IDS_ERR_VERIFYVERSION);				
					break;
				}
			}

//			::MessageBox(GetSafeHwnd(), strErrMsg, "nProtect KeyCrypt", MB_OK | MB_ICONERROR);
//			PostQuitMessage(0);
		} else {

			NPKLoadAtStartup(TRUE);
			NPKRegCryptMsg( m_hKCrypt, g_Neuz.GetSafeHwnd(), WM_USER + 1094 );
		}
	}
	
#endif	// __NPKCRYPT
}
CWndLogin::~CWndLogin()
{
#ifdef __NPKCRYPT
	if( m_hKCrypt > 0 )
		NPKCloseDriver( m_hKCrypt );
#endif	// __NPKCRYPT
}
void CWndLogin::OnDraw( C2DRender* p2DRender )
{
	//CRect rect = GetClientRect();
	//CSize size = m_pTheme->m_pFontGameTitle->GetTextExtent( _T( "CLOCKWORKS" ) );
	//p2DRender->m_pFont = m_pTheme->m_pFontGameTitle;
	//p2DRender->TextOut( rect.Width() / 2 - size.cx / 2,20, "CLOCKWORKS", 0xffffffff );

	//p2DRender->RenderTexture( CPoint( 120, 0 ), &m_Texture );

	//size = m_pTheme->m_pFontText->GetTextExtent( _T( "Copyright (C) 2002~2003 Allrights Reserved AEONSOFT Inc." ) );
	//p2DRender->m_pFont = m_pTheme->m_pFontText;
	//p2DRender->TextOut( rect.Width() / 2 - size.cx / 2, rect.top + 120,"Copyright (C) 2002~2003 Allrights Reserved AEONSOFT Inc.", 0xffffffff  );
	//p2DRender->RenderLine( CPoint( 5, 140 ), CPoint( rect.right - 5, 140 ), 0x70ffffff);

	//p2DRender->TextOut( 105, 160, _T( "Account" ) );
	//p2DRender->TextOut( 105, 185, _T( "Password" ) );


	//p2DRender->RenderRoundRect(CRect(4,     4,128*2+6,       96+6),D3DCOLOR_TEMP(255,150,150,250));
/*
	CRect rect = CRect( 4, 96 + 6 + 4, 128 * 2 + 6, 96 + 6 + 4 + 96 + 6 );
	p2DRender->RenderRoundRect( rect, D3DCOLOR_TEMP( 255, 150, 150, 250 ) );
	rect.DeflateRect( 1, 1 );
	p2DRender->RenderFillRect( rect, D3DCOLOR_TEMP( 255, 200, 200, 240 ) );

	CRect rect = CRect( 4, 96 + 6 + 4, 128 * 2 + 6, 96 + 6 + 4 + 96 + 6 );
	p2DRender->RenderRoundRect( rect, D3DCOLOR_TEMP( 255, 150, 150, 250 ) );
	rect.DeflateRect( 1, 1 );
	p2DRender->RenderFillRect( rect, D3DCOLOR_TEMP( 255, 200, 200, 240 ) );
*/
	//p2DRender->TextOut(10,60,"aaaa",D3DCOLOR_TEMP(255,100,100,200));
}

BOOL CWndLogin::Process()
{
#ifdef __AEGON_LOGOUT_CHAR_SELECT
	if (g_WndMng.m_bLogoutToCharSelect && !g_dpCertified.IsConnected())
	{
		CWndEdit* pAccount = (CWndEdit*)GetDlgItem(WIDC_ACCOUNT);
		CWndEdit* pPassword = (CWndEdit*)GetDlgItem(WIDC_PASSWORD);

		pAccount->SetString(g_WndMng.m_szLogoutName);
		pPassword->SetString(g_WndMng.m_szLogoutPw);

		DoLoginProcedure();

		memset(g_WndMng.m_szLogoutName, 0, MAX_PLAYER);
		memset(g_WndMng.m_szLogoutPw, 0, MAX_SESSION_PWD);
		g_WndMng.m_bLogoutToCharSelect = FALSE;
		g_WndMng.m_bSkipServerSelect = TRUE;
		return TRUE;
	}
#endif

	if( g_Neuz.m_dwTimeOutDis < GetTickCount() )
	{
		g_Neuz.m_dwTimeOutDis = 0xffffffff;
		g_dpCertified.DeleteDPObject();
		g_dpLoginClient.DeleteDPObject();
		g_DPlay.DeleteDPObject();
		
		g_WndMng.OpenMessageBoxUpper(  _T( prj.GetText(TID_DIAG_0043) ) );
		
		m_bDisconnect = TRUE;
	}

	if( g_WndMng.m_pWndMessageBoxUpper == NULL && m_bDisconnect )
	{
		m_bDisconnect = FALSE;
		g_WndMng.CloseMessageBox();
		g_dpCertified.DeleteDPObject();
		CWndButton* pButton	= (CWndButton*)GetDlgItem( WIDC_OK );
		pButton->EnableWindow( TRUE );
	}
	
	return 1;
}

void CWndLogin::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	CRect rect = GetClientRect();

#ifdef __REG
	m_wndRegist. Create( "Registration", 0, CRect( 0, 0, 100, 20 ), this, 1001 ); rect.OffsetRect( 120,0 );
#endif

	CWndEdit* pAccount = (CWndEdit*) GetDlgItem( WIDC_ACCOUNT );
	CWndEdit* pPassword = (CWndEdit*) GetDlgItem( WIDC_PASSWORD );
	CWndButton* pSaveAccount = (CWndButton*) GetDlgItem( WIDC_CHECK1 );
#ifndef __AEGON_ADVANCED_LOGIN
	if( ::GetLanguage() == LANG_FRE )
	{
		CWndStatic	* pWndStatic2	= (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
		CRect rc	= pWndStatic2->GetWndRect();
		rc.right	+= 24;
		pWndStatic2->SetWndRect( rc, TRUE );

		CRect rc1	= pAccount->GetWndRect();
		CRect rc2	= pPassword->GetWndRect();
		rc1.left	+= 12;
		rc1.right	= rc2.right;
		pAccount->SetWndRect( rc1, TRUE );
		rc2.left	+= 12;
		pPassword->SetWndRect( rc2, TRUE );
	}
#endif //  __AEGON_ADVANCED_LOGIN
	pAccount->EnableModeChange( FALSE );
	pAccount->SetTabStop( TRUE );

	pPassword->AddWndStyle( EBS_PASSWORD );
	pPassword->SetTabStop( TRUE );
	pPassword->EnableModeChange( FALSE );

	CWndButton* pOk   = (CWndButton*)GetDlgItem( WIDC_OK );
	CWndButton* pQuit = (CWndButton*)GetDlgItem( WIDC_QUIT );

	pOk->SetDefault( TRUE );

#ifdef __GAMEGUARD
	pSaveAccount->SetCheck(g_Option.m_bSaveAccount);

	if (g_Option.m_bSaveAccount == 1)
		pAccount->SetString(g_Option.m_szAccount);
	else
	{
		pAccount->SetString("");
		strcpy(g_Option.m_szAccount, pAccount->GetString());
	}
#else // __GAMEGUARD
	pAccount->SetString(g_Option.m_szAccount);
	pSaveAccount->SetCheck(g_Option.m_bSaveAccount);
#endif // __GAMEGUARD

	if( g_Option.m_szAccount[ 0 ] )
		pPassword->SetFocus();
	else
		pAccount->SetFocus();

	MoveParentCenter();

#ifndef __AEGON_ADVANCED_LOGIN
		CWndButton* pAbout   = (CWndButton*)GetDlgItem( WIDC_ABOUT );
		pAbout->EnableWindow(FALSE);
		pAbout->SetVisible(FALSE);	

		CWndButton* pPrologue   = (CWndButton*)GetDlgItem( WIDC_PROLOGUE );
		pPrologue->EnableWindow(FALSE);
		pPrologue->SetVisible(FALSE);
	
		pOk->Move( 72, 105 );
		pQuit->Move( 72, 135 );
#endif // __AEGON_ADVANCED_LOGIN
		
/*#endif //__FOR_PROLOGUE_UPDATE
	}*/
#ifdef __THROUGHPORTAL0810
	if( g_Neuz.m_bThroughPortal )
	{
		CRect HanrectWindow = GetWindowRect( TRUE );
		SetWndRect( CRect( HanrectWindow.left, HanrectWindow.top, HanrectWindow.right - 120, HanrectWindow.bottom - 115 ) );
		CRect rectLayout = m_pWndRoot->GetLayoutRect();
		Move( (int)( rectLayout.Width() / 2 - m_rectWindow.Width() / 2 ), (int)( rectLayout.Height() * 0.65 ) );
	}
#endif	// __THROUGHPORTAL0810

	if( ::GetLanguage() == LANG_KOR )
	{
		CWndButton* pPrologue   = (CWndButton*)GetDlgItem( WIDC_PROLOGUE );
		pPrologue->SetVisible(FALSE);
	}

	switch( ::GetLanguage() )
	{
		case LANG_KOR:
		case LANG_FRE:
		case LANG_GER:
			{
				CRect rectRoot = m_pWndRoot->GetLayoutRect();
				CRect rectWindow = GetWindowRect( TRUE );
				rectWindow.top = 400 * rectRoot.Height() / 768;
				Move( rectWindow.TopLeft() );
				break;
			}
	}


#ifdef __THROUGHPORTAL0810
	if( g_Neuz.m_bThroughPortal )
#else	// __THROUGHPORTAL0810
	if( g_Neuz.m_bHanGame )
#endif	// __THROUGHPORTAL0810
	{
		CWndStatic* pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
		pStatic->SetVisible( FALSE );
		pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
		pStatic->SetVisible( FALSE );
		CWndEdit* pEdit = (CWndEdit*)GetDlgItem( WIDC_ACCOUNT );
		pEdit->SetVisible( FALSE );
		pEdit->Move( 800, 800 );
		pEdit = (CWndEdit*)GetDlgItem( WIDC_PASSWORD );
		pEdit->SetVisible( FALSE );
		pEdit->Move( 800, 800 );
		CWndButton* pButton = (CWndButton*) GetDlgItem( WIDC_CHECK1 );
		pButton->SetVisible( FALSE );
		pButton = (CWndButton*) GetDlgItem( WIDC_OK );
		CRect rectButton = pButton->GetWndRect();
		pButton->Move( 13, 10 );
#ifndef __AEGON_ADVANCED_LOGIN
		pButton = (CWndButton*) GetDlgItem( WIDC_ABOUT );
		pButton->Move( 13, 35 );
		pButton = (CWndButton*) GetDlgItem( WIDC_PROLOGUE );
		pButton->Move( 13, 60 );
#endif // __AEGON_ADVANCED_LOGIN
		pButton = (CWndButton*) GetDlgItem( WIDC_QUIT );
		pButton->Move( 13, 85 );
	}
	g_Neuz.m_dwTimeOutDis = 0xffffffff;
	m_bDisconnect = FALSE;

#ifdef __AEGON_ADVANCED_LOGIN
	CWndListBox* pListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	CString tmpCompareAccount;
	tmpCompareAccount.Format("%s", pAccount->GetString());
	for (int i = 0; i < g_Option.m_vecLoginManager.size(); i++)
	{
		if (tmpCompareAccount.GetLength() > 0 && tmpCompareAccount.CompareNoCase(g_Option.m_vecLoginManager[i].szAccount) == 0)
		{
			pPassword->SetString(g_Option.m_vecLoginManager[i].szPassword);
		}
		pListBox->AddString(g_Option.m_vecLoginManager[i].szAccount);
	}
#endif // __AEGON_ADVANCED_LOGIN

#ifdef __FLYFF_INITPAGE_EXT
	CWndButton* pLoginVideo = (CWndButton*)GetDlgItem(WIDC_CHECK2);
	if (pLoginVideo)
	{
		pLoginVideo->SetCheck(g_Option.m_nLoginVideo);
	}
#endif //__FLYFF_INITPAGE_EXT

}
BOOL CWndLogin::Initialize(CWndBase* pWndParent,DWORD dwStyle)
{
#ifdef __AEGON_ADVANCED_LOGIN
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_AEGON_LOGIN, WBS_KEY, CPoint(0, 0), pWndParent);
#else // __AEGON_ADVANCED_LOGIN
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_LOGIN, WBS_KEY, CPoint(0, 0), pWndParent);
#endif // __AEGON_ADVANCED_LOGIN	
}

void CWndLogin::Connected( long lTimeSpan )
{
	g_WndMng.CloseMessageBox();
	g_WndMng.ObjectExecutor( SHORTCUT_APPLET, APP_SELECT_SERVER );
#ifdef __BILLING0712
	if( lTimeSpan )	// 1일 미만?
	{
		CTimeSpan span = (time_t)lTimeSpan;		
		char szMsg[256];			

		if( span.GetTotalMinutes() > 60 )	// 1시간 0분 은 표시하지 않고 60분 남았음으로 
		{
			// %d시간 %분 남았습니다.
			sprintf( szMsg, prj.GetText(TID_DIAG_EXPIRYDAY), span.GetHours(), span.GetMinutes() );
		}
		else
		{
			// %분 남았습니다. (최소 1분으로 표시)
			int nMM = span.GetTotalMinutes();
			sprintf( szMsg, prj.GetText(TID_DIAG_EXPIRYDAYMIN ), max(nMM, 1) );
		}

		g_WndMng.CloseMessageBox();
		g_WndMng.OpenMessageBox( szMsg );
	}
#endif //__BILLING0712
	Destroy();
}

BOOL CWndLogin::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	if (message == WNM_DBLCLK)
	{
		CWndListBox* pListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
		if (pListBox->m_nIdWnd == nID) 
		{
			nID = WIDC_BUTTON2; //load acc with dbl click
		}
	}

#ifdef __FLYFF_INITPAGE_EXT
	CWndButton* pLoginVideo = (CWndButton*)GetDlgItem(WIDC_CHECK2);
#endif //__FLYFF_INITPAGE_EXT

	switch(nID)
	{
#ifdef __AEGON_ADVANCED_LOGIN
		case WIDC_BUTTON1: //Save Login
		{
			__LOGIN_ACCOUNT tmpLoginData;
			memset(&tmpLoginData, 0, sizeof(__LOGIN_ACCOUNT));

			CString strAccount, strPassword;
			BOOL alreadyExists = FALSE;
			BOOL addToVec = TRUE;

			CWndEdit* pAccount = (CWndEdit*)GetDlgItem(WIDC_ACCOUNT);
			CWndEdit* pPassword = (CWndEdit*)GetDlgItem(WIDC_PASSWORD);
			CWndListBox* pListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
			strAccount.Format("%s", pAccount->GetString());
			strPassword.Format("%s", pPassword->GetString());

			for (int i = 0; i < g_Option.m_vecLoginManager.size(); i++) 
			{
				if (strAccount.CompareNoCase(g_Option.m_vecLoginManager[i].szAccount) == 0 )
				{
					if(strPassword.Compare(g_Option.m_vecLoginManager[i].szPassword) != 0)
					{
						//change password of existing
						if (strlen(pPassword->GetString()) == 32)
							strcpy(tmpLoginData.szPassword, pPassword->GetString());
						else
						{
							CString strHash = NEUZ_PHASH;
							strHash += pPassword->GetString();
							md5(tmpLoginData.szPassword, strHash);
						}

						sprintf(g_Option.m_vecLoginManager[i].szPassword, tmpLoginData.szPassword);
						g_Option.WriteLoginManager();

					}
					alreadyExists = TRUE;
				}	
			}

			if (alreadyExists || strAccount.GetLength() < 1)
			{
				addToVec = FALSE;
			}
				
			if (addToVec)
			{
				sprintf(tmpLoginData.szAccount, strAccount.GetString());

				if (strlen(pPassword->GetString()) == 32) 
				{
					strcpy(tmpLoginData.szPassword, pPassword->GetString());
				}
				else
				{
					CString strHash = NEUZ_PHASH;
					strHash += pPassword->GetString();
					md5(tmpLoginData.szPassword, strHash);
				}

				//fill chars after '\0' with random shit
				BOOL bFillWithShit = FALSE;
				for (int i = 0; i < 64; i++) 
				{
					if (bFillWithShit) 
					{
						tmpLoginData.szAccount[i] = (char)(rand() % 128);
						continue;
					}
					else if (tmpLoginData.szAccount[i] == '\0') 
					{
						bFillWithShit = TRUE;
					}
				}
				bFillWithShit = FALSE;
				for (int i = 0; i < 128; i++)
				{
					if (bFillWithShit)
					{
						tmpLoginData.szPassword[i] = (char)(rand() % 128);
						continue;
					}
					else if (tmpLoginData.szPassword[i] == '\0')
					{
						bFillWithShit = TRUE;
					}
				}

				if(g_Option.m_vecLoginManager.size() < 100)
					g_Option.m_vecLoginManager.push_back(tmpLoginData);
				else 
					g_WndMng.PutString("100 Accounts limit reached.", 0, COLOR_ERROR);

				pListBox->SetCurSel(pListBox->AddString(strAccount.GetString()));

				g_Option.WriteLoginManager();
			}
		}
		break;
		case WIDC_BUTTON2: //Load
		{
			CWndEdit* pAccount = (CWndEdit*)GetDlgItem(WIDC_ACCOUNT);
			CWndEdit* pPassword = (CWndEdit*)GetDlgItem(WIDC_PASSWORD);
			CWndListBox* pListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);

			if (pListBox->GetCurSel() != -1 && g_Option.m_vecLoginManager.size() > pListBox->GetCurSel())
			{
				pAccount->SetString(g_Option.m_vecLoginManager[pListBox->GetCurSel()].szAccount);
				pPassword->SetString(g_Option.m_vecLoginManager[pListBox->GetCurSel()].szPassword);

				OnChildNotify(message, WIDC_OK, pLResult);
			}
		}
		break;
		case WIDC_BUTTON3: //Delete
		{
			CWndListBox* pListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
			if (pListBox->GetCurSel() != -1 && g_Option.m_vecLoginManager.size() > pListBox->GetCurSel())
			{
				g_Option.m_vecLoginManager.erase(g_Option.m_vecLoginManager.begin() + pListBox->GetCurSel());
				pListBox->DeleteString(pListBox->GetCurSel());
				g_Option.WriteLoginManager();

				if (g_Option.m_vecLoginManager.size() > 0)
					pListBox->SetCurSel(0);
			}
		}
		break;
#endif // __AEGON_ADVANCED_LOGIN
		case WIDC_CHECK1:
			{
				g_Option.m_bSaveAccount = !g_Option.m_bSaveAccount;
				CWndEdit* pAccount = (CWndEdit*) GetDlgItem( WIDC_ACCOUNT );
				strcpy( g_Option.m_szAccount, pAccount->GetString() );
			}
			break;
#ifdef __FLYFF_INITPAGE_EXT
		case WIDC_CHECK2:
		{
			if (pLoginVideo->GetCheck())
			{
				g_Option.m_nLoginVideo = TRUE;
			}
			else
			{
				g_Option.m_nLoginVideo = FALSE;
			}
		}
		break;
#endif //__FLYFF_INITPAGE_EXT
		case WIDC_ACCOUNT:
		case WIDC_PASSWORD:
			//if( message != EN_CHANGE )
				break;
#ifndef __AEGON_ADVANCED_LOGIN
		case WIDC_ABOUT:
			{
				if( GetLanguage() == LANG_FRE )
				{
					STARTUPINFO si;
					PROCESS_INFORMATION pi;
					ZeroMemory( &si, sizeof(si) );
					si.cb	= sizeof(si);
					// Start the child process. 
					char lpPath[MAX_PATH]	= { 0, };
					char lpCommandLine[MAX_PATH]	= { 0,};
					if( !GetIePath( lpPath ) )
						break;
					sprintf( lpCommandLine, "%s\\IEXPLORE.EXE http://flyff.gpotato.eu", lpPath );
					
					if( !CreateProcess( NULL,
					lpCommandLine,
					NULL,
					NULL,
					FALSE,
					0,
					NULL,
					NULL,
					&si,
					&pi )
					) 
					{
					}
					// Close process and thread handles.
					CloseHandle( pi.hProcess );
					CloseHandle( pi.hThread );
				}
				else
				{
					CWndAbout* pWndAbout = new CWndAbout;
					pWndAbout->Initialize();
				}
			}
			break;
#endif // __AEGON_ADVANCED_LOGIN
		case WIDC_OK: // 접속 
			{
				int ret = DoLoginProcedure();
				switch (ret) 
				{
				case 1:
					return TRUE;
				default:
					break;
				}
				break;
			}

		case WIDC_QUIT: // 종료 
		case WTBID_CLOSE:
			::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, 0, 0 );
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndLogin::OnCommand(UINT nID,DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndLogin::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndLogin::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;
}

void CWndLogin::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;
}

#ifdef __CON_AUTO_LOGIN
void CWndLogin::SetAccountAndPassword( const CString& account, const CString& pass )
{
	CWndEdit* pAccount = (CWndEdit*) GetDlgItem( WIDC_ACCOUNT );
	CWndEdit* pPassword = (CWndEdit*) GetDlgItem( WIDC_PASSWORD );

	if( pAccount && pPassword )
	{
		pAccount->SetString( account );
		pPassword->SetString( pass );
	}
}
#endif

#ifdef __AEGON_LOGOUT_CHAR_SELECT
int CWndLogin::DoLoginProcedure()
{
#ifdef __CRC
	if (!g_dpCertified.ConnectToServer(g_Neuz.m_lpCertifierAddr, PN_CERTIFIER, TRUE, CSock::crcWrite))
#else	// __CRC
	if (!g_dpCertified.ConnectToServer(g_Neuz.m_lpCertifierAddr, PN_CERTIFIER, TRUE))
#endif	// __CRC
	{
		// Can't connect to server
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0043)));
		//					g_WndMng.OpenMessageBox( _T( "접속할 수 없습니다. 네트워크 상태를 확인하십시오." ) );
		CNetwork::GetInstance().OnEvent(CERT_CONNECT_FAIL);
		return 0;
	}
	CNetwork::GetInstance().OnEvent(CERT_CONNECTED);

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
	pButton->EnableWindow(FALSE);

	CWndEdit* pAccount = (CWndEdit*)GetDlgItem(WIDC_ACCOUNT);
	CWndEdit* pPassword = (CWndEdit*)GetDlgItem(WIDC_PASSWORD);

	CString strAccount, strPassword;
	strAccount = pAccount->GetString();
	strPassword = pPassword->GetString();

	if (IsAcValid(pAccount->GetString()) == FALSE)
	{
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0005)));
		//					g_WndMng.OpenMessageBox( _T( "계정은 3~16자 영어, 숫자를 사용할 수 있고, 숫자로 시작할 수 없습니다." ) );
		pButton->EnableWindow(TRUE);
		return 1;
	}
	if (IsPwdValid(pPassword->GetString()) == FALSE)
	{
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0030)));
		//					g_WndMng.OpenMessageBox( _T( "암호는 3~16자 영어, 숫자를 사용할 수 있습니다." ) );
		pButton->EnableWindow(TRUE);
		return 1;
	}

#ifdef __GAMEGUARD
	if (g_Option.m_bSaveAccount == 1)
#endif // __GAMEGUARD
	{
		strcpy(g_Option.m_szAccount, pAccount->GetString());
	}
#ifdef __THROUGHPORTAL0810
	if (g_Neuz.m_bThroughPortal == FALSE)
#else	// __THROUGHPORTAL0810
	if (g_Neuz.m_bHanGame == FALSE)	// 한게임 유저는 이미 세팅되어 있음, 두번 하면 문제생김 
#endif	// __THROUGHPORTAL0810
		g_Neuz.SetAccountInfo(pAccount->GetString(), pPassword->GetString());
	g_dpCertified.SendCertify();

	g_WndMng.OpenCustomBox(NULL, new CWndConnectingBox);
}
#endif

/////////////////////////////////////////////////////////////////////////////////////
// Select Server
/////////////////////////////////////////////////////////////////////////////////////

CWndSelectServer::CWndSelectServer()
{
	m_atexPannel = NULL;
	m_dwChangeBannerTime = g_tmCurrent+SEC(10);
	m_vecStrBanner.clear();
	SetPutRegInfo( FALSE );
}
CWndSelectServer::~CWndSelectServer()
{
	if( m_atexPannel )
	{
		SAFE_DELETE( m_atexPannel );
	}
}
void CWndSelectServer::OnDraw( C2DRender* p2DRender )
{
	//p2DRender->TextOut( 5, 225, _T( "URL" ) );
	//p2DRender->TextOut( 5,  5, _T( "Clockworks Server List" ) );
}

void CWndSelectServer::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rect = GetClientRect();

	//m_wndURL.Create( g_Neuz.GetSafeHwnd(), 0, CRect( 30, 220, 230, 240 ), this, 1000);
	//m_wndSearch.Create( _T( "Search" ), 0,  CRect( 235, 220, 290, 240 ), this, 100 ); 

	//m_wndServerList.Create( 0, CRect( rect.left + 5, rect.top + 40, rect.right - 5, rect.bottom - 35), this, 200 );


	CWndButton* pNext = (CWndButton*)GetDlgItem( WIDC_NEXT );
	pNext->SetDefault( TRUE );

	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_CONTROL0 );
	CRect ReRect = pWndList->GetWindowRect(TRUE);
	ReRect.bottom -= 5;
	pWndList->SetWndRect(ReRect);

	CWndListBox* pWndListMulti	= (CWndListBox*)GetDlgItem( WIDC_CONTROL1 );
	ReRect = pWndListMulti->GetWindowRect(TRUE);
	ReRect.bottom -= 5;
	pWndListMulti->SetWndRect(ReRect);

#ifdef __AEGON_THEME_SWITCHER
	pWndListMulti->m_nFontColor = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	pWndListMulti->m_nSelectColor = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE);

	pWndList->m_nFontColor = pWndListMulti->m_nFontColor;
	pWndList->m_nSelectColor = pWndListMulti->m_nSelectColor;
#else // __AEGON_THEME_SWITCHER
	pWndListMulti->m_nFontColor = 0xff000000;
	pWndListMulti->m_nSelectColor = 0xff0000ff;

	//pWndList->AddWndStyle( WBS_NODRAWFRAME );
	pWndList->m_nFontColor = 0xff000000;
	pWndList->m_nSelectColor = 0xff0000ff;
#endif // __AEGON_THEME_SWITCHER

	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(70,25);//m_pSprPack->GetAt(9)->GetSize();

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

//	m_wndServerList.Create( WLVS_REPORT, CRect( rect.left + 5, rect.top + 20, rect.right - 5, rect.bottom - 65), this, 11 );

	TCHAR szTitle[3][10] = {_T("Server"), _T("Ping"), _T("Max") };
	
	BOOL bSeveServer = FALSE;
	for( int j = 0; j < (int)( g_dpCertified.m_dwSizeofServerset ); j++ )
	{
		char lpString[MAX_PATH]	= { 0, };
		char lpStrtmp[32]	= { 0, };
		long lCount	= 0;
		long lMax	= 0;
		if( g_dpCertified.m_aServerset[j].dwParent == NULL_ID )
		{
			if( g_dpCertified.m_aServerset[j].lEnable != 0L )
			{
				int nIndex	= pWndList->AddString( g_dpCertified.m_aServerset[j].lpName );
#ifdef _WIN64
				pWndList->SetItemData(nIndex, reinterpret_cast<INT_PTR>(&g_dpCertified.m_aServerset[j]));
#else
				pWndList->SetItemData( nIndex, (DWORD)&g_dpCertified.m_aServerset[j] );
#endif
				
				if( nIndex == g_Option.m_nSer )
				{
					bSeveServer = TRUE;
					pWndListMulti->ResetContent();
				}
			}
		}
		else if( g_dpCertified.m_aServerset[j].lEnable != 0L )
		{
			if( pWndList->GetCount() > 0 )
			{
				LPSERVER_DESC pServerDesc;
				if( bSeveServer )
				{
					pServerDesc = (LPSERVER_DESC)pWndList->GetItemData( g_Option.m_nSer );
				}
				else
				{
					pServerDesc	= (LPSERVER_DESC)pWndList->GetItemData( 0 );
				}
				if( g_dpCertified.m_aServerset[j].dwParent == pServerDesc->dwID )
				{
					lCount	= g_dpCertified.m_aServerset[j].lCount;
					lMax	= g_dpCertified.m_aServerset[j].lMax;

					long lBusy	= (long)( lMax * 0.8 );

					if( lCount < lBusy )
					{
						strcpy( lpStrtmp, prj.GetText(TID_GAME_NORMAL));
					}
					else 
					if( lCount < lMax )
					{
						strcpy( lpStrtmp, prj.GetText(TID_GAME_BUSY));
					}
					else	
					{
						strcpy( lpStrtmp, prj.GetText(TID_GAME_FULL) );
					}
					sprintf( lpString, "%s (%s)", g_dpCertified.m_aServerset[j].lpName, lpStrtmp );
					int nIndex	= pWndListMulti->AddString( lpString );
#ifdef _WIN64
					pWndListMulti->SetItemData(nIndex, reinterpret_cast<INT_PTR>(&g_dpCertified.m_aServerset[j]));
#else
					pWndListMulti->SetItemData( nIndex, (DWORD)&g_dpCertified.m_aServerset[j] );
#endif
				}
			}
		}
	}
	if( pWndListMulti->GetCount() )
		pWndListMulti->SetCurSel( 0 );

	if( pWndList->GetCount() )
		pWndList->SetCurSel( 0 );

	if( bSeveServer == FALSE )
	{
		g_Option.m_nSer = 0;
		g_Option.m_nMSer = 0;
	}
	else
	{
		if( g_Option.m_nMSer >= pWndListMulti->GetCount() )
		{
			g_Option.m_nMSer = 0;
		}
	}
	if( pWndList->GetCount() > 0 )
		pWndList->SetCurSel( g_Option.m_nSer );
	if( pWndListMulti->GetCount() > 0 )
		pWndListMulti->SetCurSel( g_Option.m_nMSer );

//	if( ::GetLanguage() == LANG_TWN )
//	{
//		CWndButton* pWndBack = (CWndButton*)GetDlgItem( WIDC_BACK );
//		pWndBack->EnableWindow( FALSE );
//	}
	
	MoveParentCenter();

	if( ::GetLanguage() == LANG_JAP )
	{
		CRect rect2 = m_pWndRoot->GetLayoutRect();
		
		int width = (rect2.right-rect2.left) / 2;
		
		Move( width, m_rectWindow.top );
	}	

	pNext->SetFocus();

/////////////////////////////////////////////////////////////////////////////////////////
	int nCount = 0;
#ifndef __NO_AFTER_SKIN
	CScript script;
	if( script.Load(MakePath(DIR_THEME, "TexBannerList.inc" )) )
	{
		int nLang;
		nLang = script.GetNumber();
		do 
		{
			if( nLang == ::GetLanguage() )
			{
				script.GetToken();
				
				nCount = atoi( script.token );
				
				script.GetToken();				
				
				for( int i=0; i<nCount; i++ )
				{
					CString addStr = script.token;
					m_vecStrBanner.push_back( addStr );
					script.GetToken();
				}
				
				if( nCount <= 0 )
				{
					Error( "TexBannerList.inc의 갯수가 0이다" );
					return;
				}
				
				break;
			}
			else
				script.GetLastFull();
			
			nLang = script.GetNumber();
		} while( script.tok != FINISHED );		
	}
#endif // __NO_AFTER_SKIN	
	if( nCount > 0 )
	{
		SAFE_DELETE( m_atexPannel );
		m_atexPannel = new IMAGE;
		LoadImage( MakePath( DIR_THEME, m_vecStrBanner[xRandom(nCount)] ), m_atexPannel );
		AdjustWndBase();	
	}
/////////////////////////////////////////////////////////////////////////////////////////
}
BOOL CWndSelectServer::Process()
{
#ifdef __AEGON_LOGOUT_CHAR_SELECT
	if (g_WndMng.m_bSkipServerSelect)
	{
		g_WndMng.m_bSkipServerSelect = FALSE;
		OnChildNotify(WNM_CLICKED, WIDC_NEXT, NULL);
		return TRUE;
	}
#endif

#ifndef __NO_AFTER_SKIN
	if( g_tmCurrent > m_dwChangeBannerTime )
	{
		m_dwChangeBannerTime = g_tmCurrent+SEC(10);

		SAFE_DELETE( m_atexPannel );
		m_atexPannel = new IMAGE;
		LoadImage( MakePath( DIR_THEME, m_vecStrBanner[xRandom(m_vecStrBanner.size())] ), m_atexPannel );
		AdjustWndBase();	
	}
#endif // __NO_AFTER_SKIN
	return TRUE;
}
BOOL CWndSelectServer::Initialize(CWndBase* pWndParent,DWORD dwStyle)
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SELECT_SERVER, WBS_KEY, CPoint( 0, 0 ), pWndParent );
}

void CWndSelectServer::AfterSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat )
{
#ifdef __NO_AFTER_SKIN
	return;
#else // __NO_AFTER_SKIN
	CPoint pt;
	LPWNDCTRL lpWndCtrl;
	
	CPoint pt2 = m_rectClient.TopLeft() - m_rectWindow.TopLeft();
	
	lpWndCtrl = GetWndCtrl( WIDC_COMMER_BANNER2 );
	pt = lpWndCtrl->rect.TopLeft() + pt2;
	
	if( m_atexPannel ) 
		PaintTexture( pDest, m_atexPannel, pt, size );	
#endif // __NO_AFTER_SKIN
}

void CWndSelectServer::Connected()
{
#if defined(_DEBUG)
	g_Neuz.WaitLoading();
#endif

	g_WndMng.CloseMessageBox();
	g_WndMng.ObjectExecutor( SHORTCUT_APPLET, APP_SELECT_CHAR );
	Destroy();
}

void CWndSelectServer::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWndListBox* pWndListServer		= (CWndListBox*)GetDlgItem( WIDC_CONTROL0 );
	CWndListBox* pWndListMulti	= (CWndListBox*)GetDlgItem( WIDC_CONTROL1 );

	if( nChar == VK_UP )	
	{
		DWORD dwIndex = pWndListMulti->GetCurSel();

		if( dwIndex > 0 )
			pWndListMulti->SetCurSel(--dwIndex);
	}
	else
	if( nChar == VK_DOWN )	
	{
		DWORD dwIndex = pWndListMulti->GetCurSel();
		
		if( (int)( dwIndex ) < pWndListMulti->GetCount()-1 )
			pWndListMulti->SetCurSel(++dwIndex);
	}
}
BOOL CWndSelectServer::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	if (message == WNM_DBLCLK)
	{
		CWndListBox* pListBox = (CWndListBox*)GetDlgItem(WIDC_CONTROL1);
		if (pListBox->m_nIdWnd == nID)
		{
			nID = WIDC_NEXT; //Load Channel with Dbl Click
		}
	}

	if( message == WNM_SELCHANGE )
	{
		switch( nID )
		{
			case 11:
				{
					TCHAR szTemp[32];// =  _T( "nnp://" );
					_tcscpy( szTemp, m_wndServerList.GetItemText( m_wndServerList.GetCurSel(), 0 ) );
					m_wndURL.SetString( szTemp );
					break;
				}
			case 188:
				{
					char lpString[MAX_PATH]	= { 0, };
					char lpStrtmp[32]	= { 0, };
					long lCount	= 0;
					long lMax	= 0;
					CWndListBox* pWndListServer		= (CWndListBox*)GetDlgItem( WIDC_CONTROL0 );
					CWndListBox* pWndListMulti	= (CWndListBox*)GetDlgItem( WIDC_CONTROL1 );

					pWndListMulti->ResetContent();
					LPSERVER_DESC pServerDesc	= (LPSERVER_DESC)pWndListServer->GetItemData( pWndListServer->GetCurSel() );
					for( int j = 0; j < (int)( g_dpCertified.m_dwSizeofServerset ); j++ )
					{
						if( g_dpCertified.m_aServerset[j].dwParent == pServerDesc->dwID && ( g_dpCertified.m_aServerset[j].lEnable != 0L ) )
						{
							lCount	= g_dpCertified.m_aServerset[j].lCount;
							lMax	= g_dpCertified.m_aServerset[j].lMax;

							long lBusy	= (long)( lMax * 0.8 );

							if( lCount < lBusy )	
								strcpy( lpStrtmp, prj.GetText(TID_GAME_NORMAL));	//"정상"
							else if( lCount < lMax )		
								strcpy( lpStrtmp, prj.GetText(TID_GAME_BUSY));		//"혼잡"
							else	
								lstrcpy( lpStrtmp, prj.GetText(TID_GAME_FULL) );

							sprintf( lpString, "%s (%s)", g_dpCertified.m_aServerset[j].lpName, lpStrtmp );
							int nIndex	= pWndListMulti->AddString( lpString );
#ifdef _WIN64
							pWndListMulti->SetItemData(nIndex, reinterpret_cast<INT_PTR>(&g_dpCertified.m_aServerset[j]));
#else
							pWndListMulti->SetItemData( nIndex, (DWORD)&g_dpCertified.m_aServerset[j] );
#endif
						}
					}
					if( pWndListMulti->GetCount() )
						pWndListMulti->SetCurSel( 0 );
					break;
				}
		}
	}
	else
	switch(nID)
	{
		case 10000: // close msg
		case WIDC_BACK: // Back
#ifdef __AEGON_ADVANCED_LOGIN
			g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_AEGON_LOGIN);
#else // __AEGON_ADVANCED_LOGIN
			g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_LOGIN);
#endif // __AEGON_ADVANCED_LOGIN
			
			Destroy();
			g_dpCertified.DeleteDPObject();
			break;
		case WIDC_NEXT: // accept
			{
				CWndListBox* pWnd	= (CWndListBox*)GetDlgItem( WIDC_CONTROL1 );
				if( pWnd->GetCount() <= 0 )
					break;
				LPSERVER_DESC pDesc		= (LPSERVER_DESC)pWnd->GetItemData( pWnd->GetCurSel() );
				if( pDesc )
				{
					if( !( g_Neuz.m_cbAccountFlag & ACCOUNT_FLAG_SCHOOLEVENT ) && pDesc->lCount > pDesc->lMax )
					{
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0041) ) );
//						g_WndMng.OpenMessageBox( _T( "사용자가 너무 많습니다." ) );
						break;
					}
				}
			}
			if( ::GetLanguage() != LANG_THA )
			{
				CWndListBox* pWnd	= (CWndListBox*)GetDlgItem( WIDC_CONTROL1 );
				LPSERVER_DESC pDesc		= (LPSERVER_DESC)pWnd->GetItemData( pWnd->GetCurSel() );
				if( pDesc )
				{
					if( pDesc->b18 && !( g_Neuz.m_cbAccountFlag & ACCOUNT_FLAG_18 ) )
					{
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0058) ) );   // 18세미만 사용자는 접속할 수 없습니다.
						break;
					}
				}
			}

			if( FALSE == g_dpCertified.IsConnected() )
			{
				CNetwork::GetInstance().OnEvent( LOGIN_CONNECT_STEP_ERROR );

#ifdef __AEGON_ADVANCED_LOGIN
				g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_AEGON_LOGIN);
#else // __AEGON_ADVANCED_LOGIN			
				g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_LOGIN);
#endif // __AEGON_ADVANCED_LOGIN

				Destroy();
				g_dpCertified.DeleteDPObject();
				g_dpLoginClient.DeleteDPObject();	// 2004^04^19
				break;
			}
			g_WndMng.OpenCustomBox( NULL, new CWndConnectingBox );

			CWndListBox* pWndList	= (CWndListBox*)GetDlgItem( WIDC_CONTROL0 );
			LPSERVER_DESC pServerDesc	= (LPSERVER_DESC)pWndList->GetItemData( pWndList->GetCurSel() );

			g_Option.m_nSer = pWndList->GetCurSel();
			g_Neuz.m_dwSys	= pServerDesc->dwID;
			LPCSTR lpAddr	= pServerDesc->lpAddr;
			pWndList	= (CWndListBox*)GetDlgItem( WIDC_CONTROL1 );
			pServerDesc	= (LPSERVER_DESC)pWndList->GetItemData( pWndList->GetCurSel() );
			g_Option.m_nMSer = pWndList->GetCurSel();
			g_Neuz.m_uIdofMulti		= pServerDesc->dwID;
			g_Neuz.m_b18Server		= pServerDesc->b18;
			

			if( pServerDesc->dwParent != g_Neuz.m_dwSys )
			{
				CWndListBox* pWndListBox	= (CWndListBox*)GetDlgItem( WIDC_CONTROL0 );
				for( int i = 0; i < pWndListBox->GetCount(); i++ )
				{
					LPSERVER_DESC ptr	= (LPSERVER_DESC)pWndListBox->GetItemData( i );
					if( ptr && ptr->dwID == pServerDesc->dwParent )
					{
						pWndListBox->SetCurSel( i );
						g_Option.m_nSer	= i;
						g_Neuz.m_dwSys	= ptr->dwID;
						lpAddr	= ptr->lpAddr;
						break;
					}
				}
			}

#		ifdef __CRC
			if( !g_dpLoginClient.ConnectToServer( lpAddr, PN_LOGINSRVR, TRUE, CSock::crcWrite ) )
#		else	__CRC
			if( !g_dpLoginClient.ConnectToServer( lpAddr, PN_LOGINSRVR, TRUE ) )
#		endif	// __CRC
			{
				// Can't connect to server
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0043) ) );
//				g_WndMng.OpenMessageBox( _T( "접속할 수 없습니다. 네트워크 상태를 확인하십시오." ) );
				CNetwork::GetInstance().OnEvent( LOGIN_CONNECT_FAIL );
				break;
			}
			CNetwork::GetInstance().OnEvent( LOGIN_CONNECTED );

			g_dpLoginClient.QueryTickCount();
#ifdef __GPAUTH_01
			g_dpLoginClient.SendGetPlayerList( pServerDesc->dwID, g_Neuz.m_bGPotatoAuth? g_Neuz.m_szGPotatoNo: g_Neuz.m_szAccount, g_Neuz.m_szPassword );
#else	// __GPAUTH_01
			g_dpLoginClient.SendGetPlayerList( pServerDesc->dwID, g_Neuz.m_szAccount, g_Neuz.m_szPassword );
#endif	// __GPAUTH_01
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

BOOL CWndSelectServer::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	/*
	switch(nID)
	{
	case 100:
		g_WndMng.OpenField();
		break;
	case 101:
		break;
	case 102:
		//g_WndMng.OpenCustomBox("종료하시겠습니까?",new CWndExitBox);
		break;
	case 1000:
		break;
	case 1001:
		if(dwMessage == WM_KEYDOWN)
		{
			m_wndText.m_string += g_Neuz.m_pPlayer->m_szName;
			m_wndText.m_string += " :\n  ";
			m_wndText.m_string += m_wndChat.m_string;
			m_wndText.m_string += '\n';
			m_wndText.m_string.Reset( g_2DRender.m_pFont, &m_wndText.GetClientRect() );
			m_wndText.UpdateScrollBar();
			m_wndText.m_wndScrollBar.SetMaxScrollPos();
			m_wndChat.Empty();
		}
		break;
	}
	*/
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}
void CWndSelectServer::OnSize(UINT nType, int cx, int cy)
{
	/*
	CRect rect = GetClientRect();
	rect.bottom = rect.bottom - 40; //20;
	rect.right -= 50; 
	rect.DeflateRect( 1, 1 );
	m_wndText.SetWndRect( rect );

	rect = GetClientRect();
	rect.top = rect.bottom - 37; //20;
	rect.right -= 50; 
	rect.DeflateRect( 1, 1 );
	m_wndChat.SetWndRect( rect );

	rect = GetClientRect();
	rect.left = rect.right - 47; rect.right -= 3;	rect.top += 3; rect.bottom = rect.top + 20;
	m_wndLogin.SetWndRect( rect ); rect.OffsetRect( 0, 25 );
	m_wndRegist.SetWndRect( rect ); rect.OffsetRect( 0, 25 );
	m_wndQuit.SetWndRect( rect );
*/
	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndSelectServer::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;

	//if(IsWndStyle(WBS_CAPTION) && m_bPickup)
	{//
	//	m_wndTitleBar.m_wndMinimize.SetVisible(TRUE);
		//m_wndTitleBar.m_wndMaximize.SetVisible(TRUE);
	}
}
void CWndSelectServer::OnLButtonDown(UINT nFlags, CPoint point)
{
//	CWndBase::OnLButtonDown(nFlags,point
	if(IsWndRoot())
		return;
//	return;

}

/////////////////////////////////////////////////////////////////////////////////////
// Delete Character
/////////////////////////////////////////////////////////////////////////////////////

CWndDeleteChar::CWndDeleteChar() 
{ 
	nDeletePressCount = 0;
} 
CWndDeleteChar::~CWndDeleteChar() 
{ 
} 
void CWndDeleteChar::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndDeleteChar::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	if (pWndText) 
	{
		pWndText->m_string.SetColor(CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
	}

}

void CWndDeleteChar::AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat )
{
	CWndEdit *WndEdit   = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	WndEdit->SetVisible( FALSE );
	WndEdit->EnableWindow( FALSE );

	CWndNeuz::AdditionalSkinTexture( pDest, sizeSurface, d3dFormat );
}

BOOL CWndDeleteChar::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
#ifdef __AEGON_FIXES
	InitDialog( g_Neuz.GetSafeHwnd(), APP_DELETE_CHAR, WBS_MODAL, 0, pWndParent );
#else
	InitDialog(g_Neuz.GetSafeHwnd(), APP_DELETE_CHAR, WBS_MODAL);
#endif
	CWndEdit *WndEdit   = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	if( WndEdit )
	{
		WndEdit->AddWndStyle( EBS_PASSWORD|EBS_AUTOHSCROLL );
		WndEdit->SetFocus();
	}

	MoveParentCenter();
	return TRUE;
}

BOOL CWndDeleteChar::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndDeleteChar::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndDeleteChar::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndDeleteChar::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndDeleteChar::DeletePlayer( int nSelect, LPCTSTR szNo )
{
	g_dpLoginClient.SendDeletePlayer( nSelect, szNo );
	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_OK );
	pWndButton->EnableWindow( FALSE );
	pWndButton	= (CWndButton*)GetDlgItem( WIDC_CANCEL );
	pWndButton->EnableWindow( FALSE );								
}

BOOL CWndDeleteChar::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
	{
		if (nDeletePressCount < 3) 
		{
			CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);
			if (pWndText)
			{
				CString strTmp;
				DWORD dwColor = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
				switch (nDeletePressCount) 
				{
				case 0:
					strTmp.Format("\nAre you sure?");
					break;
				case 1:
					strTmp.Format(" Like 100%% certain?");
					break;
				case 2:
					strTmp.Format("\nIm not gonna ask again!\nDeleted characters are gone forever!");
					dwColor = COLOR_ERROR;
					break;

				}
				pWndText->m_string.AddParsingString(strTmp, dwColor);
			}
			nDeletePressCount++;
			return TRUE;
		}

		CWndSelectChar* pWnd = (CWndSelectChar *)g_WndMng.GetWndBase( APP_SELECT_CHAR );
		if( pWnd == NULL )
			return CWndNeuz::OnChildNotify( message, nID, pLResult ); 

		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		if( pWndEdit == NULL )
			return CWndNeuz::OnChildNotify( message, nID, pLResult ); 

		BOOL bOK = FALSE;
		LPCTSTR szNo = pWndEdit->GetString();

		szNo = g_Neuz.m_szPassword;
		if (0 < strlen(szNo) && strlen(szNo) < 64)
		{
			DeletePlayer(pWnd->m_nSelectCharacter, szNo);
			bOK = TRUE;
		}

		if( bOK == FALSE )
		{
			pWndEdit->SetString( "" );
#ifdef __THROUGHPORTAL0810
			if( g_Neuz.m_bThroughPortal == FALSE || GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_KOR )
#else	// __THROUGHPORTAL0810
			if( g_Neuz.m_bHanGame == FALSE )
#endif	// __THROUGHPORTAL0810
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0044) ) ); // "주민번호 숫자 7자리로 넣어야 합니다. 다시 입력해주세요"
		}
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/////////////////////////////////////////////////////////////////////////////////////
// Select Character
/////////////////////////////////////////////////////////////////////////////////////

int CWndSelectChar::m_nSelectCharacter = 0;

CWndSelectChar::CWndSelectChar()
{
	m_pWndDeleteChar = NULL;
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	m_pWnd2ndPassword = NULL;
#endif // __2ND_PASSWORD_SYSTEM
	ZeroMemory( m_pBipedMesh, sizeof( m_pBipedMesh ) );	
	m_dwMotion[ 0 ] = MTI_SITSTAND;
	m_dwMotion[ 1 ] = MTI_SITSTAND;
	m_dwMotion[ 2 ] = MTI_SITSTAND;
	SetPutRegInfo( FALSE );
	m_CreateApply = TRUE; //서버통합 관련 특정 기간 캐릭터 생성 금지.
#ifdef __LECTURE_V17 // Lecture v17 chracter
    m_bFlash        = TRUE;
    m_nFlashAlpha    = 0;
    m_nChars        = 0;
#endif //__LECTURE_V17

#ifdef __ANI_WING
	for (short i = 0; i < MAX_CHARACTER_LIST; i++)
		m_pMask[i] = nullptr;
#endif

}
CWndSelectChar::~CWndSelectChar()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		SAFE_DELETE( m_pBipedMesh[ i ] );
	}
	SAFE_DELETE( m_pWndDeleteChar );
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	SAFE_DELETE( m_pWnd2ndPassword );
#endif // __2ND_PASSWORD_SYSTEM

#ifdef __ANI_WING
	for (short i = 0; i < MAX_CHARACTER_LIST; i++)
		SAFE_DELETE(m_pMask[i]);
#endif
}

void CWndSelectChar::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( !g_Neuz.m_timerConnect.Over() )
		return;

	int nSelectCharBuf = m_nSelectCharacter;
	if( nChar == VK_LEFT )
	{
		--nSelectCharBuf;
		if( 0 > nSelectCharBuf )
		{
			nSelectCharBuf = MAX_CHARACTER_LIST - 1;
		}
		SelectCharacter( nSelectCharBuf );
	}
	else if( nChar == VK_RIGHT )
	{
		++nSelectCharBuf;
		if( nSelectCharBuf >= MAX_CHARACTER_LIST )
		{
			nSelectCharBuf = 0;
		}
		SelectCharacter( nSelectCharBuf );
	}
}

void CWndSelectChar::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if (pWndChild == m_pWndDeleteChar) 
	{
		SAFE_DELETE(m_pWndDeleteChar);
		pWndChild = NULL;
	}
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	if (pWndChild == m_pWnd2ndPassword) 
	{
		SAFE_DELETE(m_pWnd2ndPassword);
		pWndChild = NULL;
	}
#endif // __2ND_PASSWORD_SYSTEM
}
HRESULT CWndSelectChar::InitDeviceObjects()
{
	CWndBase::InitDeviceObjects();
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		if( m_pBipedMesh[ i ] )
			m_pBipedMesh[ i ]->InitDeviceObjects( m_pApp->m_pd3dDevice );
	}

	return S_OK;
}
HRESULT CWndSelectChar::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		if( m_pBipedMesh[ i ] )
			m_pBipedMesh[ i ]->RestoreDeviceObjects();
	}
	return S_OK;
}
HRESULT CWndSelectChar::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		if( m_pBipedMesh[ i ] )
			m_pBipedMesh[ i ]->InvalidateDeviceObjects();
	}
	return S_OK;
}
HRESULT CWndSelectChar::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		if( m_pBipedMesh[ i ] )
			m_pBipedMesh[ i ]->DeleteDeviceObjects();
	}
	return S_OK;
}
BOOL CWndSelectChar::Process()
{
#ifdef __ANI_WING
	for (short i = 0; i < MAX_CHARACTER_LIST; i++)
	{
		if (m_pMask[i])
			m_pMask[i]->FrameMove();
	}
#endif
	/*
	 *	

	ANILOOP_1PLAY			(0x00000001)			// 한번 플레이후 끝.
	ANILOOP_CONT			(0x00000002)			// 한번 플레이후 마지막 동작으로
	ANILOOP_LOOP			(0x00000004)			// 반복
	ANILOOP_RETURN			(0x00000008)			// 왕복 - 사용되지 않음.
	ANILOOP_BACK			(0x00000010)			// 뒤에서 부터. - 사용되지 않음

	 */
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		CRect rect = m_aRect[ i ];
		CModelObject* pModel = (CModelObject*)m_pBipedMesh[ i ];
		CMover* pMover = g_Neuz.m_apPlayer[ i ];
	
		if( g_Neuz.m_apPlayer[i] != NULL && pModel )
		{

#ifdef __LNB_ANIMATED_HATS
			if (pMover->GainAniHatInfo())
			{
				pMover->GainAniHatInfo()->FrameMove(0, 1);
			}
#endif //__LNB_ANIMATED_HATS

			int nMover = (pMover->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
			if( m_nSelectCharacter == i )
			{
				// 완전히 일어났나? 그렇다면 MTI_STAND로 변경 
				if( m_dwMotion[ i ] == MTI_GETUP )
				{
					if( pModel->IsEndFrame() && pModel->m_nLoop == ANILOOP_1PLAY )
					{
						SetMotion( pModel, nMover, MTI_STAND, ANILOOP_LOOP, 0 );
						m_dwMotion[ i ] = MTI_STAND;
					}
				}
			}
			else
			{
				// 앉아 있는게 아닌가? 그렇다면 무조건 앉아라. MTI_SIT으로 변경
				if( m_dwMotion[ i ] != MTI_SITSTAND )
				{
					if( pModel->IsEndFrame() && pModel->m_nLoop == ANILOOP_1PLAY )
					{
						SetMotion( pModel, nMover, MTI_SITSTAND, ANILOOP_LOOP, 0 );
						m_dwMotion[ i ] = MTI_SITSTAND;
					}
					else
					if( m_dwMotion[ i ] != MTI_SIT )
					{
						SetMotion( pModel, nMover, MTI_SIT, ANILOOP_1PLAY, 0 );
						m_dwMotion[ i ] = MTI_SIT;
					}
				}
				// 완전히 앉았나? 그렇다면 MTI_SITSTAND로 변경 
			}
			pModel->FrameMove();
		}
	}

#ifndef _DEBUG			
	if( g_Neuz.m_dwTimeOutDis < GetTickCount() )
	{
		g_Neuz.m_dwTimeOutDis = 0xffffffff;
		g_dpCertified.DeleteDPObject();
		g_dpLoginClient.DeleteDPObject();
		g_DPlay.DeleteDPObject();
		
		g_WndMng.OpenMessageBoxUpper(  _T( prj.GetText(TID_DIAG_0043) ) );

		m_bDisconnect = TRUE;
	}

	if( g_WndMng.m_pWndMessageBoxUpper == NULL && m_bDisconnect )
	{
		Destroy();
		g_WndMng.CloseMessageBox();
		g_dpCertified.SendCertify();
#ifdef __AEGON_ADVANCED_LOGIN
		g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_AEGON_LOGIN);
#else // __AEGON_ADVANCED_LOGIN
		g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_LOGIN);
#endif // __AEGON_ADVANCED_LOGIN
		
#ifdef __AEGON_ADVANCED_LOGIN
		CWndBase* pWndBase = g_WndMng.GetWndBase(APP_AEGON_LOGIN);
#else // __AEGON_ADVANCED_LOGIN
		CWndBase* pWndBase = g_WndMng.GetWndBase(APP_LOGIN);
#endif // __AEGON_ADVANCED_LOGIN
		
		return 0;	
	}
#endif //_DEBUG			
	
#ifdef __LECTURE_V17
    if( m_bFlash == TRUE )
    {
        m_nFlashAlpha += 1;

        if( m_nFlashAlpha > 40 )
        {
            m_nFlashAlpha = 40;
            m_bFlash = FALSE;
        }
    }
    else
    {
        m_nFlashAlpha -= 1;

        if( m_nFlashAlpha < 0 )
        {
            m_nFlashAlpha = 0;
            m_bFlash = TRUE;
        }
    }

    m_nChars = 0;

    if( g_Neuz.m_apPlayer[0] != NULL )
        m_nChars ++;
    if( g_Neuz.m_apPlayer[1] != NULL )
        m_nChars ++;
    if( g_Neuz.m_apPlayer[2] != NULL )
        m_nChars ++;

#ifndef __TITANIUM_THEME
    CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_ST_CHARCOUNT );
    char cStatic[10];
    sprintf( cStatic, "%d", m_nChars );
    pWndStatic->SetTitle( cStatic );
#endif //__TITANIUM_THEME

#endif //__LECTURE_V17
	
	return CWndNeuz::Process();
}

void CWndSelectChar::OnDraw( C2DRender* p2DRender )
{
	CWndButton* pWndAccept = (CWndButton*)GetDlgItem( WIDC_ACCEPT );
	CWndButton* pWndCreate = (CWndButton*)GetDlgItem( WIDC_CREATE );
	CWndButton* pWndDelete = (CWndButton*)GetDlgItem( WIDC_DELETE );

#ifdef __LECTURE_V17
    CWndStatic* pWndStaticChar[4];
    pWndStaticChar[1] = (CWndStatic*)GetDlgItem( WIDC_ST_CHARNAME01 );
    pWndStaticChar[2] = (CWndStatic*)GetDlgItem( WIDC_ST_CHARNAME02 );
    pWndStaticChar[3] = (CWndStatic*)GetDlgItem( WIDC_ST_CHARNAME03 );

#ifndef __TITANIUM_THEME
    CWndButton*    pWndButton1 = (CWndButton*)GetDlgItem( WIDC_BT_LEFT );
    CWndButton*    pWndButton2 = (CWndButton*)GetDlgItem( WIDC_BT_RIGHT );
#endif //__TITANIUM_THEME

    LPWNDCTRL lpCustom[3];
    lpCustom[0] = GetWndCtrl( WIDC_CUSTOM1 );
    lpCustom[1] = GetWndCtrl( WIDC_CUSTOM2 );
    lpCustom[2] = GetWndCtrl( WIDC_CUSTOM3 );
    LPWNDCTRL lpStatic[3];
    lpStatic[0] = GetWndCtrl( WIDC_ST_CHARNAME01 );
    lpStatic[1] = GetWndCtrl( WIDC_ST_CHARNAME02 );
    lpStatic[2] = GetWndCtrl( WIDC_ST_CHARNAME03 );

#ifndef __TITANIUM_THEME
    pWndButton1->EnableWindow( false );
    pWndButton2->EnableWindow( false );
    pWndButton1->SetVisible( false );
    pWndButton2->SetVisible( false );
#endif //__TITANIUM_THEME

#endif //__LECTURE_V17

	CRect rect;
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		rect = m_aRect[ i ];

		if( g_Neuz.m_apPlayer[i] != NULL )
		{
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
			if( g_WndMng.GetWndBase( APP_2ND_PASSWORD_NUMBERPAD ) == NULL )
			{
				POINT point = GetMousePoint();
				if( m_aRect[ i ].PtInRect( point ) )
				{
					CRect rectHittest = m_aRect[ i ];
					CPoint point2 = point;
					ClientToScreen( &point2 );
					ClientToScreen( rectHittest );
					g_WndMng.PutToolTip_Character( i, point2, &rectHittest );
				}
			}
#else // __2ND_PASSWORD_SYSTEM
			POINT point = GetMousePoint();
			if( m_aRect[ i ].PtInRect( point ) )
			{
				CRect rectHittest = m_aRect[ i ];
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( rectHittest );
				g_WndMng.PutToolTip_Character( i, point2, &rectHittest );
			}
#endif // __2ND_PASSWORD_SYSTEM
			if( m_nSelectCharacter == i )
			{
#ifdef __LECTURE_V17
                CRect rectNew;
                CRect rectFlash;

                rectNew.top        = lpCustom[i]->rect.top;
                rectNew.left    = lpCustom[i]->rect.left;
                rectNew.bottom    = lpCustom[i]->rect.bottom;
                rectNew.right    = lpCustom[i]->rect.right;

                rectFlash.top        = lpStatic[i]->rect.top + 3;
                rectFlash.left        = lpStatic[i]->rect.left + 2;
                rectFlash.bottom    = lpStatic[i]->rect.bottom - 3;
                rectFlash.right        = lpStatic[i]->rect.right - 2;

                // Background
                p2DRender->RenderFillRect(rectNew, D3DCOLOR_ARGB( 20, 80, 250, 80 ) );
                // Flash
                p2DRender->RenderFillRect(rectFlash, D3DCOLOR_ARGB( m_nFlashAlpha, 80, 250, 80 ) );
#else //__LECTURE_V17
				CRect rectTemp = rect;

                rectTemp.top += 10;
                rectTemp.bottom -= 10;
				rectTemp.left += i;
				rectTemp.right += i;
                p2DRender->RenderFillRect(rectTemp, D3DCOLOR_ARGB( 20, 80, 250, 80 ) );
#endif //__LECTURE_V17
				
#ifdef __LECTURE_V17
                pWndStaticChar[i+1]->SetTitle( g_Neuz.m_apPlayer[i]->GetName() );
					
                switch( i )
                {
#ifdef __AEGON_THEME_SWITCHER
				case 0:	pWndStaticChar[1]->m_dwColor = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE), pWndStaticChar[2]->m_dwColor = 0xFF666666, pWndStaticChar[3]->m_dwColor = 0xFF666666; break;
				case 1:	pWndStaticChar[1]->m_dwColor = 0xFF666666, pWndStaticChar[2]->m_dwColor = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE), pWndStaticChar[3]->m_dwColor = 0xFF666666; break;
				case 2:	pWndStaticChar[1]->m_dwColor = 0xFF666666, pWndStaticChar[2]->m_dwColor = 0xFF666666, pWndStaticChar[3]->m_dwColor = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE); break;
#else //__AEGON_THEME_SWITCHER
				case 0:	pWndStaticChar[1]->m_dwColor = 0xFF3173C6, pWndStaticChar[2]->m_dwColor = 0xFF666666, pWndStaticChar[3]->m_dwColor = 0xFF666666; break;
				case 1:	pWndStaticChar[1]->m_dwColor = 0xFF666666, pWndStaticChar[2]->m_dwColor = 0xFF3173C6, pWndStaticChar[3]->m_dwColor = 0xFF666666; break;
				case 2:	pWndStaticChar[1]->m_dwColor = 0xFF666666, pWndStaticChar[2]->m_dwColor = 0xFF666666, pWndStaticChar[3]->m_dwColor = 0xFF3173C6; break;
#endif //__AEGON_THEME_SWITCHER
                }
#else //__LECTURE_V17
                p2DRender->TextOut( rect.left, rect.bottom + 10, g_Neuz.m_apPlayer[i]->GetName(), COLOR_ERROR );
                p2DRender->TextOut( rect.left + 1, rect.bottom + 10, g_Neuz.m_apPlayer[i]->GetName(), COLOR_ERROR );
#endif //__LECTURE_V17
			}
			else
            {
                
#ifdef __LECTURE_V17
                pWndStaticChar[i+1]->SetTitle( g_Neuz.m_apPlayer[i]->GetName() );
#else //__LECTURE_V17
				p2DRender->TextOut( rect.left, rect.bottom + 10, g_Neuz.m_apPlayer[i]->GetName(), 0xff505050 );
#endif //__LECTURE_V17
            }

			CModelObject* pModel = (CModelObject*)m_pBipedMesh[ i ];
			LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

#ifdef __ANISOTROPIC
			if (g_Option.m_nAnisotropic == TRUE)
			{
				pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, g_pD3dApp->m_d3dCaps.MaxAnisotropy);
				pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
				pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
				pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
			}
			else
#endif // __ANISOTROPIC
			{
				pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
				pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			}

			pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255, 255,255,255) );
			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

			D3DVIEWPORT9 viewport;

#ifdef __LECTURE_V17
            viewport.X      = p2DRender->m_ptOrigin.x + lpCustom[i]->rect.left;
            viewport.Y      = p2DRender->m_ptOrigin.y + lpCustom[i]->rect.top;    
#else //__LECTURE_V17
			viewport.X      = p2DRender->m_ptOrigin.x + rect.left;
			viewport.Y      = p2DRender->m_ptOrigin.y + rect.top;
#endif //__LECTURE_V17

			viewport.Width  = rect.Width();
			viewport.Height = rect.Height();
			viewport.MinZ   = 0.0f;
			viewport.MaxZ   = 1.0f;
			pd3dDevice->SetViewport(&viewport);
			pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

			CRect rectViewport( 0, 0, viewport.Width, viewport.Height );

			// 프로젝션 
			D3DXMATRIX matProj;
			D3DXMatrixIdentity( &matProj );
			FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;
			D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
			pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

			// 카메라 
			D3DXMATRIX  matView;
			D3DXVECTOR3 vecLookAt( 0.0f, 0.0f, 1.0f );
			D3DXVECTOR3 vecPos(  0.0f, 0.5f, -3.5f );

			D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );

			pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

			// 월드 
			D3DXMATRIXA16 matWorld;
			D3DXMATRIXA16 matScale;
			D3DXMATRIXA16 matRot;
			D3DXMATRIXA16 matTrans;

			// 초기화 
			D3DXMatrixIdentity(&matScale);
			D3DXMatrixIdentity(&matRot);
			D3DXMatrixIdentity(&matTrans);
			D3DXMatrixIdentity(&matWorld);
			
			D3DXMatrixScaling(&matScale,1.6f,1.6f,1.6f);
			D3DXMatrixTranslation(&matTrans,0.0f,-1.15f,0.0f);

			D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
			D3DXMatrixMultiply(&matWorld, &matWorld,&matRot);
			D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
			pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

			// 랜더링 
			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
			pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );//m_bViewLight );
			::SetLight( FALSE );
			::SetFog( FALSE );
			SetDiffuse( 1.0f, 1.0f, 1.0f );
			SetAmbient( 1.0f, 1.0f, 1.0f );
			
			O3D_ELEMENT *pElem = pModel->GetParts( PARTS_RIDE );
			if( pElem && pElem->m_pObject3D )
				pModel->TakeOffParts( PARTS_RIDE );
			
			pElem = pModel->GetParts( PARTS_HAIR );

			if( pElem && pElem->m_pObject3D )
			{
				pElem->m_pObject3D->m_fAmbient[0] = g_Neuz.m_apPlayer[i]->m_fHairColorR;
				pElem->m_pObject3D->m_fAmbient[1] = g_Neuz.m_apPlayer[i]->m_fHairColorG;
				pElem->m_pObject3D->m_fAmbient[2] = g_Neuz.m_apPlayer[i]->m_fHairColorB;
			}
			
			//pModel->FrameMove();
			D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
#ifdef __YENV
			g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vConst );
#else //__YENV						
			pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
#endif //__YENV
			::SetTransformView( matView );
			::SetTransformProj( matProj );

			g_Neuz.m_apPlayer[i]->OverCoatItemRenderCheck(pModel);
			
			// 헬멧이 머리카락 날려야하는것이냐?  // 인벤이 없는경우
#ifdef __LOOKCHANGE
			DWORD dwId = g_Neuz.m_apPlayer[i]->m_aEquipInfo[PARTS_CAP].isLook() ? g_Neuz.m_apPlayer[i]->m_aEquipInfo[PARTS_CAP].dwLook : g_Neuz.m_apPlayer[i]->m_aEquipInfo[PARTS_CAP].dwId;
#else
			DWORD dwId	= g_Neuz.m_apPlayer[i]->m_aEquipInfo[PARTS_CAP].dwId;
#endif 
			ItemProp* pItemProp = NULL;

#ifdef __LNB_ANIMATED_HATS
			D3DXMATRIX mRide = matWorld;
			if (g_Neuz.m_apPlayer[i]->GainAniHatInfo())
			{
				pModel->SetEffect(PARTS_CAP, XE_HIDE);
				static const int SPINE1_BONE = 6;
				static const int SPINE2_BONE = 6;
				D3DXMATRIX* pmatSpine1Bone = pModel->GetMatrixBone(SPINE1_BONE);
				D3DXMATRIX* pmatSpine2Bone = pModel->GetMatrixBone(SPINE2_BONE);
				if (pmatSpine1Bone && pmatSpine2Bone)
				{
					D3DXMATRIX matEvent;
					D3DXMatrixIdentity(&matEvent);

					D3DXMATRIX matRotation;
					D3DXMatrixIdentity(&matRotation);
					D3DXMatrixRotationX(&matRotation, D3DXToRadian(180));
					D3DXMatrixMultiply(&matEvent, &matRotation, &matEvent);
					D3DXMatrixRotationZ(&matRotation, D3DXToRadian(-90));
					D3DXMatrixMultiply(&matEvent, &matRotation, &matEvent);
					D3DXMatrixMultiply(&matEvent, &matEvent, pmatSpine1Bone);


					matEvent._41 = pmatSpine2Bone->_41;
					matEvent._42 = pmatSpine2Bone->_42;
					matEvent._43 = pmatSpine2Bone->_43;

					D3DXMatrixMultiply(&mRide, &matEvent, &matWorld);

					g_Neuz.m_apPlayer[i]->GainAniHatInfo()->Render(pd3dDevice, &mRide);
					g_Neuz.m_apPlayer[i]->GainAniHatInfo()->FrameMove();
				}
			}
#endif //__LNB_ANIMATED_HATS

			if( dwId != NULL_ID )
			{
				O3D_ELEMENT* pElement = NULL;
				pItemProp = prj.GetItemProp(dwId);
				if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
					pElement = pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );

#ifdef __LOOKCHANGE
				dwId = g_Neuz.m_apPlayer[i]->m_aEquipInfo[PARTS_HAT].isLook() ? g_Neuz.m_apPlayer[i]->m_aEquipInfo[PARTS_HAT].dwLook : g_Neuz.m_apPlayer[i]->m_aEquipInfo[PARTS_HAT].dwId;
#else
				dwId = g_Neuz.m_apPlayer[i]->m_aEquipInfo[PARTS_HAT].dwId;
#endif 
				if( dwId != NULL_ID )
				{
					if( !(g_Neuz.m_apPlayer[i]->m_aEquipInfo[PARTS_HAT].byFlag & CItemElem::expired) )
					{
						pItemProp = prj.GetItemProp( dwId );

#ifdef __LNB_ANIMATED_HATS
						if (pItemProp && pItemProp->dwItemKind3 == IK3_ANIHAT)
						{
							if (pItemProp->dwBasePartsIgnore != -1)
							{
								if (pItemProp->dwBasePartsIgnore != PARTS_HEAD)
								{
									pModel->SetEffectOff(PARTS_HAIR, XE_HIDE);
								}
								pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
							}
						}
						else
#endif // __LNB_ANIMATED_HATS
						if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
						{
							if( pItemProp->dwBasePartsIgnore == PARTS_HEAD )
								pModel->SetEffect(PARTS_HAIR, XE_HIDE );
							
							pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
						}
						else
						{
							if( pElement )
								pElement->m_nEffect &= ~XE_HIDE;
						}
					}
				}
			}
			else
			{
#ifdef __LOOKCHANGE
				dwId = g_Neuz.m_apPlayer[i]->m_aEquipInfo[PARTS_HAT].isLook() ? g_Neuz.m_apPlayer[i]->m_aEquipInfo[PARTS_HAT].dwLook : g_Neuz.m_apPlayer[i]->m_aEquipInfo[PARTS_HAT].dwId;
#else
				dwId = g_Neuz.m_apPlayer[i]->m_aEquipInfo[PARTS_HAT].dwId;
#endif 

				if( dwId != NULL_ID )
				{
					if( !(g_Neuz.m_apPlayer[i]->m_aEquipInfo[PARTS_HAT].byFlag & CItemElem::expired) )
					{						
						pItemProp = prj.GetItemProp( dwId );

#ifdef __LNB_ANIMATED_HATS
						if (pItemProp && pItemProp->dwItemKind3 == IK3_ANIHAT)
						{
							if (pItemProp->dwBasePartsIgnore != -1)
							{
								if (pItemProp->dwBasePartsIgnore != PARTS_HEAD)
								{
									pModel->SetEffectOff(PARTS_HAIR, XE_HIDE);
								}
								pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
							}
						}
						else
#endif // __LNB_ANIMATED_HATS
						if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
						{
							if( pItemProp->dwBasePartsIgnore == PARTS_HEAD )
								pModel->SetEffect(PARTS_HAIR, XE_HIDE );

							pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
						}
					}
				}
			}

#ifdef __YENV
			SetLightVec( D3DXVECTOR3( 0.0f, 0.0f, 1.0f ) );
#endif //__YENV
			
			pModel->Render( p2DRender->m_pd3dDevice, &matWorld );

#ifdef __ANI_WING
			D3DXMATRIX mMask = matWorld;

			if (m_pMask[i])
			{
				m_pMask[i]->m_nNoEffect = pModel->m_nNoEffect;
				static const int SPINE1_BONE = 4;
				static const int SPINE2_BONE = 5;
				D3DXMATRIX* pmatSpine1Bone = pModel->GetMatrixBone(SPINE1_BONE);
				D3DXMATRIX* pmatSpine2Bone = pModel->GetMatrixBone(SPINE2_BONE);
				if (pmatSpine1Bone && pmatSpine2Bone)
				{
					D3DXMATRIX matEvent;
					D3DXMatrixIdentity(&matEvent);

					pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
					D3DXMatrixTranslation(&matEvent, 0.2f, 0.0f, 0.1f);

					D3DXMATRIX matRotation;
					D3DXMatrixIdentity(&matRotation);
					D3DXMatrixRotationX(&matRotation, D3DXToRadian(180));
					D3DXMatrixMultiply(&matEvent, &matRotation, &matEvent);
					D3DXMatrixRotationZ(&matRotation, D3DXToRadian(-90));
					D3DXMatrixMultiply(&matEvent, &matRotation, &matEvent);
					D3DXMatrixMultiply(&matEvent, &matEvent, pmatSpine1Bone);

					D3DXMatrixMultiply(&mMask, &matEvent, &matWorld);
				}
				m_pMask[i]->SetTextureEx(m_pMask[i]->m_pModelElem->m_nTextureEx);

				m_pMask[i]->Render(p2DRender->m_pd3dDevice, &mMask);
				m_pMask[i]->m_nNoEffect = 0;
			}
#endif

			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		
#ifdef __LECTURE_V17
            viewport.X      = p2DRender->m_ptOrigin.x + lpCustom[i]->rect.left;
            viewport.Y      = p2DRender->m_ptOrigin.y + lpCustom[i]->rect.top;
#else //__LECTURE_V17
            viewport.X      = p2DRender->m_ptOrigin.x + 5;
            viewport.Y      = p2DRender->m_ptOrigin.y + 5;
#endif //__LECTURE_V17
			viewport.Width  = p2DRender->m_clipRect.Width();
			viewport.Height = p2DRender->m_clipRect.Height();
			viewport.MinZ   = 0.0f;
			viewport.MaxZ   = 1.0f;
			pd3dDevice->SetViewport(&viewport);

		}
		else
		{
#ifdef __LECTURE_V17
#ifdef __PLACEHOLDER_TEXT
			pWndStaticChar[i + 1]->SetTitle("- Empty -");
#else //__PLACEHOLDER_TEXT
			pWndStaticChar[i + 1]->SetTitle("");
#endif //__PLACEHOLDER_TEXT
#else //__LECTURE_V17
			if( m_nSelectCharacter == i )
				p2DRender->TextOut( rect.left, rect.bottom + 10, prj.GetText( TID_GAME_WND_SELECT_CHARACTER_EMPTY ), 0xff6060ff );
			else
				p2DRender->TextOut( rect.left, rect.bottom + 10, prj.GetText( TID_GAME_WND_SELECT_CHARACTER_EMPTY ), 0xff505050 );
#endif //__LECTURE_V17
		}			

		if( m_nSelectCharacter == i )
		{
			if( g_Neuz.m_apPlayer[i]  )
			{
				pWndCreate->EnableWindow( FALSE );
				pWndAccept->EnableWindow( TRUE );
				pWndDelete->EnableWindow( TRUE );
			}
			else
			{
				pWndCreate->EnableWindow( TRUE );
				pWndAccept->EnableWindow( FALSE );
				pWndDelete->EnableWindow( FALSE );
#ifdef __LECTURE_V17
                switch( i )
                {
#ifdef __AEGON_THEME_SWITCHER
				case 0:	pWndStaticChar[1]->m_dwColor = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE), pWndStaticChar[2]->m_dwColor = 0xFF666666, pWndStaticChar[3]->m_dwColor = 0xFF666666; break;
				case 1:	pWndStaticChar[1]->m_dwColor = 0xFF666666, pWndStaticChar[2]->m_dwColor = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE), pWndStaticChar[3]->m_dwColor = 0xFF666666; break;
				case 2:	pWndStaticChar[1]->m_dwColor = 0xFF666666, pWndStaticChar[2]->m_dwColor = 0xFF666666, pWndStaticChar[3]->m_dwColor = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE); break;
#else //__AEGON_THEME_SWITCHER
                case 0:	pWndStaticChar[1]->m_dwColor = 0xFF6BBDAE,	pWndStaticChar[2]->m_dwColor = 0xFF666666,	pWndStaticChar[3]->m_dwColor = 0xFF666666; break;
                case 1:	pWndStaticChar[1]->m_dwColor = 0xFF666666,	pWndStaticChar[2]->m_dwColor = 0xFF6BBDAE,	pWndStaticChar[3]->m_dwColor = 0xFF666666; break;
                case 2:	pWndStaticChar[1]->m_dwColor = 0xFF666666,	pWndStaticChar[2]->m_dwColor = 0xFF666666,	pWndStaticChar[3]->m_dwColor = 0xFF6BBDAE; break;
#endif //__AEGON_THEME_SWITCHER
                }
#endif //__LECTURE_V17
			}
		}
	}
}
void CWndSelectChar::DeleteCharacter()
{
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		if( m_pBipedMesh[ i ] )
		{
			m_pBipedMesh[ i ]->InvalidateDeviceObjects();
			m_pBipedMesh[ i ]->DeleteDeviceObjects();
			SAFE_DELETE( m_pBipedMesh[ i ] );
		}
	}
}
void CWndSelectChar::UpdateCharacter()
{
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		CMover* pMover = g_Neuz.m_apPlayer[i];
		if( pMover )
		{
			// 장착, 게이지에 나올 캐릭터 오브젝트 설정 
			int nMover = (pMover->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
			m_pBipedMesh[ i ] = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
			if( i == m_nSelectCharacter )
			{
				prj.m_modelMng.LoadMotion( m_pBipedMesh[ i ],  OT_MOVER, nMover, MTI_STAND );
				m_dwMotion[ i ] = MTI_STAND;
			}
			else
			{
				prj.m_modelMng.LoadMotion( m_pBipedMesh[ i ],  OT_MOVER, nMover, MTI_SITSTAND );
				m_dwMotion[ i ] = MTI_SITSTAND;
			}
#ifdef __ANI_WING
			DWORD dwId = NULL_ID;
#ifdef __LOOKCHANGE
			if (pMover->m_aEquipInfo[PARTS_MASK].dwLook != NULL_ID)
				dwId = pMover->m_aEquipInfo[PARTS_MASK].dwLook;
			else
#endif
				dwId = pMover->m_aEquipInfo[PARTS_MASK].dwId;
			if (dwId != NULL_ID)
			{
				ItemProp* pItemProp = prj.GetItemProp(dwId);
				if (pItemProp && pItemProp->dwParts == PARTS_MASK && pItemProp->dwItemKind3 == IK3_ANI_WING)
				{
					m_pMask[i] = (CModelObject*)prj.m_modelMng.LoadModel(D3DDEVICE, OT_ITEM, pItemProp->dwID);

					CString strMotion = m_pMask[i]->GetMotionFileName("stand");
					m_pMask[i]->LoadMotion(strMotion);
				}
				else
				{
					if (pItemProp && pItemProp->dwParts == PARTS_MASK)
						m_pMask[i] = NULL;
				}
			}
#endif
			CMover::UpdateParts( pMover->GetSex(), pMover->m_dwSkinSet, pMover->m_dwFace, pMover->m_dwHairMesh, pMover->m_dwHeadMesh, pMover->m_aEquipInfo, m_pBipedMesh[ i ], NULL/*&pMover->m_Inventory*/ );
		}
	}
}
void CWndSelectChar::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

#ifdef __ANI_WING
	for (short i = 0; i < MAX_CHARACTER_LIST; i++)
		SAFE_DELETE(m_pMask[i]);
#endif

	CRect rect = GetClientRect();

	LPWNDCTRL lpText1 = GetWndCtrl( WIDC_CUSTOM1 );
	LPWNDCTRL lpText2 = GetWndCtrl( WIDC_CUSTOM2 );
	LPWNDCTRL lpText3 = GetWndCtrl( WIDC_CUSTOM3 );
	
	LPWNDCTRL lpCustom[3];
	lpCustom[0] = GetWndCtrl( WIDC_CUSTOM1 );
	lpCustom[1] = GetWndCtrl( WIDC_CUSTOM2 );
	lpCustom[2] = GetWndCtrl( WIDC_CUSTOM3 );

	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_DELETE );

	CWndButton* pWndAccept = (CWndButton*)GetDlgItem( WIDC_ACCEPT );
	pWndAccept->SetDefault( TRUE );
	CWndButton* pWndBack = (CWndButton*)GetDlgItem( WIDC_BACK );
	pWndBack->SetFocus( );

	m_bDisconnect = FALSE;

	rect = CRect( 16, 16, 174, 254 );
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
#ifdef __LECTURE_V17 
        m_aRect[ i ] = lpCustom[i]->rect;
#else //__LECTURE_V17
		m_aRect[ i ] = rect;
		rect.OffsetRect( 170, 0 );
#endif //__LECTURE_V17
	}

	MoveParentCenter();

}

BOOL CWndSelectChar::Initialize(CWndBase* pWndParent,DWORD dwStyle)
{
	CRect rect = m_pWndRoot->MakeCenterRect( 590, 400 );
	SetTitle( _T( "Select Character" ) );
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SELECT_CHAR, WBS_KEY, CPoint( 0, 0 ), pWndParent );
}

void CWndSelectChar::Connected()
{
#ifdef __GAMEGUARD
	if (m_nSelectCharacter < 0 || m_nSelectCharacter >= MAX_CHARACTER_LIST)
#else //__GAMEGUARD
	if( m_nSelectCharacter < 0 || m_nSelectCharacter >= 5 )
#endif //__GAMEGUARD
	{
		LPCTSTR szErr = Error( "CWndSelectChar::Connected : 범위초과 %d", m_nSelectCharacter );
		ADDERRORMSG( szErr );
#ifndef __GAMEGUARD
		int* p = NULL;
		*p = 1;
#endif // __GAMEGUARD
	}
#ifdef __USE_IDPLAYER0519
	#ifdef __GPAUTH_01
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	g_dpLoginClient.SendPreJoin( g_Neuz.m_bGPotatoAuth? g_Neuz.m_szGPotatoNo: g_Neuz.m_szAccount, g_Neuz.m_apPlayer[m_nSelectCharacter]->m_idPlayer, m_nSelectCharacter, g_Neuz.m_n2ndPasswordNumber );
#else __2ND_PASSWORD_SYSTEM
	g_dpLoginClient.SendPreJoin( g_Neuz.m_bGPotatoAuth? g_Neuz.m_szGPotatoNo: g_Neuz.m_szAccount, g_Neuz.m_apPlayer[m_nSelectCharacter]->m_idPlayer, m_nSelectCharacter );
#endif // __2ND_PASSWORD_SYSTEM
	#else	// __GPAUTH_01
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	g_dpLoginClient.SendPreJoin( g_Neuz.m_szAccount, g_Neuz.m_apPlayer[m_nSelectCharacter]->m_idPlayer, m_nSelectCharacter, g_Neuz.m_n2ndPasswordNumber );
#else __2ND_PASSWORD_SYSTEM
	g_dpLoginClient.SendPreJoin( g_Neuz.m_szAccount, g_Neuz.m_apPlayer[m_nSelectCharacter]->m_idPlayer, m_nSelectCharacter );
#endif // __2ND_PASSWORD_SYSTEM
	#endif	// __GPAUTH_01
#else	// __USE_IDPLAYER0519
	#ifdef __GPAUTH_01
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	g_dpLoginClient.SendPreJoin( g_Neuz.m_bGPotatoAuth? g_Neuz.m_szGPotatoNo: g_Neuz.m_szAccount, g_Neuz.m_apPlayer[m_nSelectCharacter]->m_idPlayer, g_Neuz.m_apPlayer[m_nSelectCharacter]->GetName(), m_nSelectCharacter, g_Neuz.m_n2ndPasswordNumber );
#else __2ND_PASSWORD_SYSTEM
	g_dpLoginClient.SendPreJoin( g_Neuz.m_bGPotatoAuth? g_Neuz.m_szGPotatoNo: g_Neuz.m_szAccount, g_Neuz.m_apPlayer[m_nSelectCharacter]->m_idPlayer, g_Neuz.m_apPlayer[m_nSelectCharacter]->GetName(), m_nSelectCharacter );
#endif // __2ND_PASSWORD_SYSTEM
	#else	// __GPAUTH_01
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	g_dpLoginClient.SendPreJoin( g_Neuz.m_szAccount, g_Neuz.m_apPlayer[m_nSelectCharacter]->m_idPlayer, g_Neuz.m_apPlayer[m_nSelectCharacter]->GetName(), m_nSelectCharacter, g_Neuz.m_n2ndPasswordNumber );
#else __2ND_PASSWORD_SYSTEM
	g_dpLoginClient.SendPreJoin( g_Neuz.m_szAccount, g_Neuz.m_apPlayer[m_nSelectCharacter]->m_idPlayer, g_Neuz.m_apPlayer[m_nSelectCharacter]->GetName(), m_nSelectCharacter );
#endif // __2ND_PASSWORD_SYSTEM
	#endif	// __GPAUTH_01
#endif	// __USE_IDPLAYER0519

	CNetwork::GetInstance().OnEvent( LOGIN_REQ_PREJOIN );

	// ata2k - (1)시간 저정
#ifdef __NO_SUB_LANG
	if( ::GetLanguage() == LANG_USA )
#else // __NO_SUB_LANG
	if( ::GetLanguage() == LANG_ENG && ::GetSubLanguage() == LANG_SUB_USA )
#endif // __NO_SUB_LANG
		g_Neuz.m_dwTimeOutDis = GetTickCount() + SEC( 30 );
	else
		g_Neuz.m_dwTimeOutDis = GetTickCount() + SEC( 15 );
}

BOOL CWndSelectChar::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	if( !g_Neuz.m_timerConnect.Over() )
		return CWndNeuz::OnChildNotify( message, nID, pLResult );

	switch(nID)
	{
		case 10000: // close msg
		case WIDC_BACK: // Back
			{
#ifdef __CERTIFIER_COLLECTING_SYSTEM
				DPCollectClient->DeleteDPObject();
#endif // __CERTIFIER_COLLECTING_SYSTEM
				g_dpLoginClient.DeleteDPObject();
				Sleep( 1000 );	// 임시.
#			ifdef __CRC
				if( !g_dpCertified.ConnectToServer( g_Neuz.m_lpCertifierAddr, PN_CERTIFIER, TRUE, CSock::crcWrite ) )
#			else	// __CRC
				if( !g_dpCertified.ConnectToServer( g_Neuz.m_lpCertifierAddr, PN_CERTIFIER, TRUE ) )
#			endif	// __CRC
				{
					// Can't connect to server
					g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0043) ) );
//					g_WndMng.OpenMessageBox( _T( "접속할 수 없습니다. 네트워크 상태를 확인하십시오." ) );
					CNetwork::GetInstance().OnEvent( CERT_CONNECT_FAIL );
					break;
				}
				CNetwork::GetInstance().OnEvent( CERT_CONNECTED );
				Destroy();
				g_dpCertified.SendCertify();

#ifdef __AEGON_ADVANCED_LOGIN
				g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_AEGON_LOGIN);
				CWndBase* pWndBase = g_WndMng.GetWndBase(APP_AEGON_LOGIN);
#else // __AEGON_ADVANCED_LOGIN
				g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_LOGIN);
				CWndBase* pWndBase = g_WndMng.GetWndBase(APP_LOGIN);
#endif // __AEGON_ADVANCED_LOGIN

//				pWndBase->SetVisible( FALSE );
				break;
			}
		case WIDC_CREATE: // Create
			if( m_nSelectCharacter != -1 && g_Neuz.m_apPlayer[ m_nSelectCharacter ] == NULL )
			{
				if(m_CreateApply) //서버통합 관련 특정 기간 캐릭터 생성 금지.
				{	
					u_short uSlot	= (u_short)m_nSelectCharacter;
					Destroy();
					g_WndMng.ObjectExecutor( SHORTCUT_APPLET, APP_CREATE_CHAR );
					CWndCreateChar* pWndCreateChar	= (CWndCreateChar*)g_WndMng.GetWndBase( APP_CREATE_CHAR );
					if( pWndCreateChar ) {
						pWndCreateChar->m_Player.m_uSlot	= uSlot;
					}
				}
				else
				{
//					g_WndMng.OpenMessageBox( _T( prj.GetText(TID_GAME_CREATECHAR_WARNNING) ) ); //Message : 서버통합 관련 특정 기간 캐릭터 생성 금지.
				}
			}
			break;
		case WIDC_DELETE: // Delete
			if( m_nSelectCharacter != -1 && g_Neuz.m_apPlayer[ m_nSelectCharacter ] )
			{
				SAFE_DELETE( m_pWndDeleteChar );
				m_pWndDeleteChar = new CWndDeleteChar;
				m_pWndDeleteChar->Initialize( this, APP_DELETE_CHAR );
			}
			break;
		case WIDC_ACCEPT: // Accept
			{
				switch( m_nSelectCharacter )
				{
					case 0:
						g_Option.m_pGuide = &(g_Option.m_nGuide1);
						break;
					case 1:
						g_Option.m_pGuide = &(g_Option.m_nGuide2);
						break;
					case 2:
						g_Option.m_pGuide = &(g_Option.m_nGuide3);
						break;
					default:
						Error( "선택한 캐릭터 번호가 이상함!! : %d", m_nSelectCharacter );
						return FALSE;
				}
				CWndButton* pWndAccept = (CWndButton*)GetDlgItem( WIDC_ACCEPT );
				pWndAccept->EnableWindow( FALSE );
			}
#ifdef __CERTIFIER_COLLECTING_SYSTEM
			DPCollectClient->DeleteDPObject();
#endif // __CERTIFIER_COLLECTING_SYSTEM
			
			if( g_Neuz.m_nCharacterBlock[m_nSelectCharacter] == 0 )
			{
				g_WndMng.OpenCustomBox( NULL, new CWndCharBlockBox );
			}
			else
			{
				if( FALSE == g_dpLoginClient.IsConnected() )
				{
					CNetwork::GetInstance().OnEvent( CACHE_CONNECT_STEP_ERROR );

#ifdef __AEGON_ADVANCED_LOGIN
					g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_AEGON_LOGIN);
#else // __AEGON_ADVANCED_LOGIN
					g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_LOGIN);
#endif // __AEGON_ADVANCED_LOGIN
					
					Destroy();
					g_dpLoginClient.DeleteDPObject();
					break;
				}

#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
				if( ::IsUse2ndPassWord() == TRUE )
				{
					if( m_pWnd2ndPassword )
						SAFE_DELETE( m_pWnd2ndPassword );
					m_pWnd2ndPassword = new CWnd2ndPassword();
					m_pWnd2ndPassword->Initialize( this, APP_2ND_PASSWORD_NUMBERPAD );
					m_pWnd2ndPassword->SetInformation( g_dpLoginClient.GetNumberPad(), m_nSelectCharacter );
#ifdef __CON_AUTO_LOGIN
					for( int i = 0; i < 4; ++i )
						m_pWnd2ndPassword->InsertPassword( g_Console._nPasswordFigure[ i ] );
					m_pWnd2ndPassword->OnChildNotify( 0, WIDC_BUTTON_OK, 0 );
#endif // __CON_AUTO_LOGIN
				}
				else
				{
					g_WndMng.OpenCustomBox( _T( prj.GetText(TID_DIAG_0064) ), new CWndConnectingBox );
					//g_WndMng.OpenCustomBox( _T( "로딩중입니다. 잠시만 기다려 주십시오." ), new CWndConnectingBox );

					if( g_DPlay.Connect( g_Neuz.m_lpCacheAddr, g_Neuz.m_uCachePort ) )
					{						
						CNetwork::GetInstance().OnEvent( CACHE_CONNECTED );
						if( m_nSelectCharacter != -1 && g_Neuz.m_apPlayer[m_nSelectCharacter] )
						{
#if __VER < 8 // __S8_PK
							// 한국은 2005/11/1 PK서버가 없어지고, 아래의 코드가 있으면 카오인 유저는 모든 서버에 접속 할 수 없으므로 막는다.
							if( ::GetLanguage() != LANG_KOR )		
							{
								if( g_Neuz.m_b18Server == FALSE && g_Neuz.m_apPlayer[m_nSelectCharacter]->IsChaotic() )
								{
									//g_WndMng.OpenMessageBox( "선택된 플레이어는 카르마 수치가 낮아서 PK서버에만 접속 할 수 있습니다." );
									g_WndMng.OpenMessageBox( prj.GetText(TID_PK_REFUSE_CHAOTIC) );
									break;
								}
							}
#endif // __VER < 8 // __S8_PK
							g_Neuz.m_dwTempMessage = 1;
							g_Neuz.m_timerConnect.Set( SEC( 1 ) );
#ifndef __AEGON_DB_PETFILTER
#ifdef __SYS_PETFILTER
							g_DPlay.SendPlayerPetfilter(g_Option.m_dwPetfilter);
#endif //__SYS_PETFILTER
#endif
						}
					}
					else
					{
						CNetwork::GetInstance().OnEvent( CACHE_CONNECT_FAIL );
						TRACE( _T( "Can't connect to server. : %s \n" ), g_Neuz.m_lpCacheAddr );
					}
				}
#else // __2ND_PASSWORD_SYSTEM
				g_WndMng.OpenCustomBox( _T( prj.GetText(TID_DIAG_0064) ), new CWndConnectingBox );
				//g_WndMng.OpenCustomBox( _T( "로딩중입니다. 잠시만 기다려 주십시오." ), new CWndConnectingBox );

				if( g_DPlay.Connect( g_Neuz.m_lpCacheAddr, g_Neuz.m_uCachePort ) )
				{						
					CNetwork::GetInstance().OnEvent( CACHE_CONNECTED );

					if( m_nSelectCharacter != -1 && g_Neuz.m_apPlayer[m_nSelectCharacter] )
					{
#if __VER < 8 // __S8_PK
						// 한국은 2005/11/1 PK서버가 없어지고, 아래의 코드가 있으면 카오인 유저는 모든 서버에 접속 할 수 없으므로 막는다.
						if( ::GetLanguage() != LANG_KOR )		
						{
							if( g_Neuz.m_b18Server == FALSE && g_Neuz.m_apPlayer[m_nSelectCharacter]->IsChaotic() )
							{
								//g_WndMng.OpenMessageBox( "선택된 플레이어는 카르마 수치가 낮아서 PK서버에만 접속 할 수 있습니다." );
								g_WndMng.OpenMessageBox( prj.GetText(TID_PK_REFUSE_CHAOTIC) );
								break;
							}
						}

#endif // __VER < 8 // __S8_PK

						g_Neuz.m_dwTempMessage = 1;
						g_Neuz.m_timerConnect.Set( SEC( 1 ) );
						
					}
				}
				else
				{
					CNetwork::GetInstance().OnEvent( CACHE_CONNECT_FAIL );
					TRACE( _T( "Can't connect to server. : %s \n" ), g_Neuz.m_lpCacheAddr );
				}
#endif // __2ND_PASSWORD_SYSTEM
			}
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndSelectChar::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndSelectChar::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType,cx,cy);
}
BOOL CWndSelectChar::SetMotion( CModelObject* pModel, DWORD dwIndex, DWORD dwMotion, int nLoop, DWORD dwOption )
{
	DWORD	dwOrigMotion = dwMotion;
	prj.m_modelMng.LoadMotion( pModel, OT_MOVER, dwIndex, dwMotion );
	
	pModel->m_bEndFrame = FALSE;
	pModel->SetLoop( nLoop );
	if( dwOption & MOP_NO_TRANS ) pModel->SetMotionBlending( FALSE );
	else pModel->SetMotionBlending( TRUE );
	
	return TRUE;
}
void CWndSelectChar::SelectCharacter( int i )
{
	if( m_nSelectCharacter != i )
	{
		m_nSelectCharacter = i;
		CMover* pMover = g_Neuz.m_apPlayer[ i ];
		if( pMover )
		{
			int nMover = (pMover->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
			CModelObject* pModel = (CModelObject*)m_pBipedMesh[ m_nSelectCharacter ];
			if( pModel )
				SetMotion( pModel, nMover, MTI_GETUP, ANILOOP_1PLAY, 0 );
			m_dwMotion[ i ] = MTI_GETUP;

			if( ::GetLanguage() == LANG_JAP && g_Option.m_bVoice )
			{
#ifdef __fmod
				if (pMover->GetSex() == SEX_MALE)
					SoundSystemClass::playSound("VocM-CharaChoice.wav");
				else
					SoundSystemClass::playSound("VocF-CharaChoice.wav");
#else
				float fVolume = 0;

				fVolume = g_Option.m_fEffectVolume;

				g_SoundMng.m_nSoundVolume = 0;
				if (pMover->GetSex() == SEX_MALE)
					PLAYSND("VocM-CharaChoice.wav");
				else
					PLAYSND("VocF-CharaChoice.wav");
				g_SoundMng.m_nSoundVolume = (int)(-(1.0f - fVolume) * 5000);
#endif
				

			}
		}
	}
}
void CWndSelectChar::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( !g_Neuz.m_timerConnect.Over() )
		return;
		
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		if( /*m_pBipedMesh[ i ] &&*/ m_aRect[ i ].PtInRect( point ) )
		{
			if( g_Neuz.m_nCharacterBlock[i] == 0 )
			{
				g_WndMng.OpenCustomBox( NULL, new CWndCharBlockBox );
					
			}
			else
			{
				SelectCharacter( i );
			}
		}
	}
}

void CWndSelectChar::OnLButtonDown(UINT nFlags, CPoint point)
{
}

/////////////////////////////////////////////////////////////////////////////////////
// Select Character
/////////////////////////////////////////////////////////////////////////////////////
CWndCreateChar::CWndCreateChar()
{
	m_pModel = NULL;
	m_Player.m_bySkinSet = SKINSET_01;
	m_Player.m_byHairMesh = HAIRMESH_01;
	m_Player.m_byHairColor = 0;
	m_Player.m_byHeadMesh = 0;
	m_Player.m_bySex = SEX_FEMALE;
	m_Player.m_byCostume = 0;
	SetPutRegInfo( FALSE );
#ifdef __NEW_COLOR_CHANGE
	memset(m_ColorRect, 0, sizeof(CRect) * 3);
	memset(m_fColor, 0, sizeof(FLOAT) * 3);
	m_bLButtonClick = FALSE;
	m_ChoiceBar = -1;
#endif // __NEW_COLOR_CHANGE
#ifdef __CHAR_CREATE_ROTATE
	m_bLButtonDownRot = FALSE;
	m_OldPos = CPoint(0, 0);
	m_fRot = 0.0f;
#endif // __CREATE_CHAR_ROTATE
}
CWndCreateChar::~CWndCreateChar()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();
	SAFE_DELETE( m_pModel );
}
HRESULT CWndCreateChar::InitDeviceObjects()
{
	CWndBase::InitDeviceObjects();
	if( m_pModel )
		m_pModel->InitDeviceObjects( m_pApp->m_pd3dDevice );
	return S_OK;
}
HRESULT CWndCreateChar::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pModel )
		m_pModel->RestoreDeviceObjects();
	return S_OK;
}
HRESULT CWndCreateChar::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	if( m_pModel )
		m_pModel->InvalidateDeviceObjects();
	return S_OK;
}
HRESULT CWndCreateChar::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	//m_pModel->DeleteDeviceObjects();
	return S_OK;
}
void CWndCreateChar::OnDraw( C2DRender* p2DRender )
{
	CRect rect = GetClientRect();

	CPoint pt( 20, 15 );
	pt = CPoint( 260, 15 );

	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

#ifdef __ANISOTROPIC
	if (g_Option.m_nAnisotropic == TRUE)
	{
		pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, g_pD3dApp->m_d3dCaps.MaxAnisotropy);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	}
	else
#endif // __ANISOTROPIC
	{
		pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	}

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255, 255,255,255) );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	rect = GetClientRect();

	D3DVIEWPORT9 viewport;

	viewport.X      = p2DRender->m_ptOrigin.x + 280;
	viewport.Y      = p2DRender->m_ptOrigin.y + 0;
	viewport.Width  = 550 - 280;
	viewport.Height = 320 - 15;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	pd3dDevice->SetViewport(&viewport);
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

	POINT point = GetMousePoint();

	point.x -= 280;
	point.y -= 15;

	CRect rectViewport( 0, 0, viewport.Width, viewport.Height );

	D3DXMATRIX matProj;
	D3DXMatrixIdentity( &matProj );
	FLOAT fAspect = ( (FLOAT) viewport.Width ) / (FLOAT) viewport.Height ;
	FLOAT fov = D3DX_PI / 4.0f;//796.0f;
	FLOAT h = cos( fov / 2 ) / sin( fov / 2 );
	FLOAT w = h * fAspect;
	D3DXMatrixOrthoLH( &matProj, w, h, 1.0f, 10.0f );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	D3DXMATRIX  matView;
	D3DXVECTOR3 vecLookAt( 0.0f, 0.0f, 3.0f );
	D3DXVECTOR3 vecPos(  0.0f, 0.0f, -5.0f );

	D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );

	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot;
	D3DXMATRIXA16 matTrans;

	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matRot);
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matWorld);
#ifdef __CHAR_CREATE_ROTATE
	D3DXMatrixRotationY(&matRot, D3DXToRadian(m_fRot));
#endif // __CHAR_CREATE_ROTATE

	D3DXMatrixScaling(&matScale,1.4f,1.4f,1.4f);
	D3DXMatrixTranslation(&matTrans,0.0f,-1.1f,0.0f);

	D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld,&matRot);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
	
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	::SetLight( FALSE );
	::SetFog( FALSE );
	SetDiffuse( 1.0f, 1.0f, 1.0f );
	SetAmbient( 1.0f, 1.0f, 1.0f );
	
	m_pModel->FrameMove();
	D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
#ifdef __YENV
	g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vConst );
#else //__YENV						
	pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
#endif //__YENV
	::SetTransformView( matView );
	::SetTransformProj( matProj );

	
	O3D_ELEMENT* pElem = m_pModel->GetParts( PARTS_HAIR );
	
	if( pElem && pElem->m_pObject3D )
	{
#ifdef __NEW_COLOR_CHANGE
		if (m_Player.m_bySex == SEX_MALE || m_Player.m_bySex == SEX_FEMALE)
		{
			pElem->m_pObject3D->m_fAmbient[0] = m_fColor[0];
			pElem->m_pObject3D->m_fAmbient[1] = m_fColor[1];
			pElem->m_pObject3D->m_fAmbient[2] = m_fColor[2];
		}
		else
		{
			pElem->m_pObject3D->m_fAmbient[0] = (nFeMaleHairColor[m_Player.m_byHairMesh][0]) / 255.f;
			pElem->m_pObject3D->m_fAmbient[1] = (nFeMaleHairColor[m_Player.m_byHairMesh][1]) / 255.f;
			pElem->m_pObject3D->m_fAmbient[2] = (nFeMaleHairColor[m_Player.m_byHairMesh][2]) / 255.f;
		}
#else // __NEW_COLOR_CHANGE
		if( m_Player.m_bySex == SEX_MALE )
		{
			pElem->m_pObject3D->m_fAmbient[0] = (nMaleHairColor[m_Player.m_byHairMesh][0])/255.f;
			pElem->m_pObject3D->m_fAmbient[1] = (nMaleHairColor[m_Player.m_byHairMesh][1])/255.f;
			pElem->m_pObject3D->m_fAmbient[2] = (nMaleHairColor[m_Player.m_byHairMesh][2])/255.f;
		}
		else
		{
			pElem->m_pObject3D->m_fAmbient[0] = (nFeMaleHairColor[m_Player.m_byHairMesh][0])/255.f;
			pElem->m_pObject3D->m_fAmbient[1] = (nFeMaleHairColor[m_Player.m_byHairMesh][1])/255.f;
			pElem->m_pObject3D->m_fAmbient[2] = (nFeMaleHairColor[m_Player.m_byHairMesh][2])/255.f;
		}
#endif // __NEW_COLOR_CHANGE
	}
	
	m_pModel->Render( p2DRender->m_pd3dDevice, &matWorld );

	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	viewport.X      = p2DRender->m_ptOrigin.x;// + 5;
	viewport.Y      = p2DRender->m_ptOrigin.y;// + 5;
	viewport.Width  = p2DRender->m_clipRect.Width();
	viewport.Height = p2DRender->m_clipRect.Height();
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	pd3dDevice->SetViewport(&viewport);
#ifdef __NEW_COLOR_CHANGE
	for (int i = 0; i < 3; i++)
	{
		CPoint pt = CPoint(m_ColorScrollBar[i].x - (m_Texture.m_size.cx / 2), m_ColorScrollBar[i].y);
		m_Texture.Render(p2DRender, pt);
	}
#endif // __NEW_COLOR_CHANGE
}
void CWndCreateChar::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_Player.m_byJob = JOB_VAGRANT;

	CWndButton* pWndHairColorLeft  = (CWndButton*) GetDlgItem( WIDC_HAIRCOLOR_LEFT );
	CWndButton* pWndHairColorRight = (CWndButton*) GetDlgItem( WIDC_HAIRCOLOR_RIGHT );
	CWndButton* pWndHairFaceLeft   = (CWndButton*) GetDlgItem( WIDC_FACE_LEFT );
	CWndButton* pWndHairFaceRight  = (CWndButton*) GetDlgItem( WIDC_FACE_RIGHT );

#ifdef __NEW_COLOR_CHANGE
	pWndHairColorLeft->SetVisible(FALSE);
	pWndHairColorRight->SetVisible(FALSE);
#else // __NEW_COLOR_CHANGE
	pWndHairColorLeft->EnableWindow(FALSE);
	pWndHairColorRight->EnableWindow(FALSE);
#endif // __NEW_COLOR_CHANGE

	CWndButton* pWndOk = (CWndButton*)GetDlgItem( WIDC_OK );
	pWndOk->SetDefault( TRUE );

	
	SetSex( m_Player.m_bySex );
	MoveParentCenter();
	
	CWndEdit* pWndName = (CWndEdit*) GetDlgItem( WIDC_NAME );
	pWndName->SetFocus();

	m_Player.m_byHairMesh = (char)( xRandom( 0, MAX_BASE_HAIR ) );
	m_Player.m_byHeadMesh = (char)( xRandom( 0, MAX_DEFAULT_HEAD ) );
	CMover::UpdateParts( m_Player.m_bySex, m_Player.m_bySkinSet, m_Player.m_byFace, m_Player.m_byHairMesh, m_Player.m_byHeadMesh, m_Player.m_aEquipInfo, m_pModel, NULL );

	CWndStatic* pWnd2ndPasswordText =  ( CWndStatic* )GetDlgItem( WIDC_STATIC_2ND_PASSWORD_TEXT );
	assert( pWnd2ndPasswordText );
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	pWnd2ndPasswordText->m_dwColor = D3DCOLOR_ARGB( 255, 255, 0, 0 );

	CWndEdit* pWnd2ndPassword = ( CWndEdit* )GetDlgItem( WIDC_EDIT_2ND_PASSWORD );
	assert( pWnd2ndPassword );
	pWnd2ndPassword->AddWndStyle( EBS_PASSWORD | EBS_AUTOHSCROLL | EBS_NUMBER );
	pWnd2ndPassword->SetMaxStringNumber( MAX_2ND_PASSWORD_NUMBER );
	CWndEdit* pWnd2ndPasswordConfirm = ( CWndEdit* )GetDlgItem( WIDC_EDIT_2ND_PASSWORD_CONFIRM );
	assert( pWnd2ndPasswordConfirm );
	pWnd2ndPasswordConfirm->AddWndStyle( EBS_PASSWORD | EBS_AUTOHSCROLL | EBS_NUMBER );
	pWnd2ndPasswordConfirm->SetMaxStringNumber( MAX_2ND_PASSWORD_NUMBER );
#else // __2ND_PASSWORD_SYSTEM
	pWnd2ndPasswordText->EnableWindow( FALSE );
#endif // __2ND_PASSWORD_SYSTEM

#ifdef __NEW_COLOR_CHANGE
	CRect rect = GetWindowRect(TRUE);
	O3D_ELEMENT* pElem = m_pModel->GetParts(PARTS_HAIR);
	if (pElem && pElem->m_pObject3D)
	{
		if (m_Player.m_bySex == SEX_MALE)
		{
			m_fColor[0] = (nMaleHairColor[m_Player.m_byHairMesh][0]) / 255.f;
			m_fColor[1] = (nMaleHairColor[m_Player.m_byHairMesh][1]) / 255.f;
			m_fColor[2] = (nMaleHairColor[m_Player.m_byHairMesh][2]) / 255.f;
		}
		else
		{
			m_fColor[0] = (nFeMaleHairColor[m_Player.m_byHairMesh][0]) / 255.f;
			m_fColor[1] = (nFeMaleHairColor[m_Player.m_byHairMesh][1]) / 255.f;
			m_fColor[2] = (nFeMaleHairColor[m_Player.m_byHairMesh][2]) / 255.f;
		}
	}

	LPWNDCTRL lpWndCtrl;
	lpWndCtrl = GetWndCtrl(WIDC_CUSTOM_R);
	rect = lpWndCtrl->rect;
	m_ColorRect[0].left = rect.left;
	m_ColorRect[0].top = rect.top;
	m_ColorRect[0].right = rect.right;
	m_ColorRect[0].bottom = rect.bottom;
	m_ColorRect[0] = rect;

	lpWndCtrl = GetWndCtrl(WIDC_CUSTOM_G);
	rect = lpWndCtrl->rect;
	m_ColorRect[1].left = rect.left;
	m_ColorRect[1].top = rect.top;
	m_ColorRect[1].right = rect.right;
	m_ColorRect[1].bottom = rect.bottom;
	m_ColorRect[1] = rect;

	lpWndCtrl = GetWndCtrl(WIDC_CUSTOM_B);
	rect = lpWndCtrl->rect;
	m_ColorRect[2].left = rect.left;
	m_ColorRect[2].top = rect.top;
	m_ColorRect[2].right = rect.right;
	m_ColorRect[2].bottom = rect.bottom;
	m_ColorRect[2] = rect;

	ReSetBar(m_fColor[0], m_fColor[1], m_fColor[2]);
	m_Texture.LoadTexture(g_Neuz.GetDevice(), MakePath(DIR_THEME, "yellowbuttten.tga"), COLOR_ITEM_BG, TRUE);
#endif // __NEW_COLOR_CHANGE

}

void CWndCreateChar::SetSex( int nSex )
{
	m_Player.m_bySex = nSex;

	int nMover = m_Player.m_bySex == SEX_MALE ? MI_MALE : MI_FEMALE;

	SAFE_DELETE( m_pModel );
	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );

	if( nSex == SEX_MALE )
		prj.m_modelMng.LoadMotion( m_pModel,  OT_MOVER, nMover, MTI_STAND );
	else
		prj.m_modelMng.LoadMotion( m_pModel,  OT_MOVER, nMover, MTI_STAND2 ); // 포니테일 앞으로 

	memset( m_Player.m_aEquipInfo, 0, sizeof(EQUIP_INFO) * MAX_HUMAN_PARTS );
	{
		for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
		{
			m_Player.m_aEquipInfo[i].dwId	= NULL_ID;
#ifdef __LOOKCHANGE
			m_Player.m_aEquipInfo[i].dwLook = NULL_ID;
#endif // __LOOKCHANGE
		}
	}

	for( int i = 0; i < MAX_BEGINEQUIP; i++ )
	{
		DWORD dwEquip = prj.m_jobItem[ m_Player.m_byJob ].adwMale[ i ][ m_Player.m_bySex ];
		if( dwEquip != NULL_ID )
		{
			ItemProp* pItemProp = prj.GetItemProp( dwEquip );
			m_Player.m_aEquipInfo[pItemProp->dwParts].dwId = dwEquip;
#ifdef __LOOKCHANGE
			m_Player.m_aEquipInfo[pItemProp->dwParts].dwLook = dwEquip;
#endif // __LOOKCHANGE
		}
	}
	CMover::UpdateParts( m_Player.m_bySex, 0, m_Player.m_bySkinSet, m_Player.m_byHairMesh, m_Player.m_byHeadMesh, m_Player.m_aEquipInfo, m_pModel, NULL );
}

BOOL CWndCreateChar::Initialize( CWndBase* pWndParent, DWORD dwStyle )
{
	CRect rect = m_pWndRoot->MakeCenterRect( 590, 400 );
#ifdef __NEW_COLOR_CHANGE
	m_Texture.LoadTexture(g_Neuz.GetDevice(), MakePath(DIR_THEME, "yellowbuttten.tga"), COLOR_ITEM_BG, TRUE);
#endif // __NEW_COLOR_CHANGE
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CREATE_CHAR, WBS_KEY, CPoint( 0, 0 ), pWndParent );
}


DWORD IsValidPlayerNameTWN( CString& string )
{
	const char* begin	= string;
	const char* end		= begin + string.GetLength();
	const char* iter	= begin;
	char bytes[16];
	
	while( *iter && iter < end ) 
	{ 
		const char* next = CharNext(iter);

		memcpy( bytes, iter, next-iter );
		bytes[next-iter] = 0;

		if( IsMultiByte( iter ) ) 
		{
			wchar_t ch = MAKEWORD( bytes[1], bytes[0] );
			
			if( ch >= 0xA259 && ch <= 0xA261 || ch == 0xA2CD || ch >= 0xA440 && ch <= 0xC67E || ch >= 0xC940 && ch <= 0xF9D5 )
				;
			else
				return TID_DIAG_0014;
		}
		else if( isalnum( bytes[0] ) == FALSE || iscntrl( bytes[0] ) )
			return TID_DIAG_0013;

		iter = next;
	}
	return 0;
}

// return 0   : OK
//        0 > : error
DWORD IsValidPlayerName( CString& strName )
{
	strName.TrimLeft();
	strName.TrimRight();

	LPCTSTR lpszString = strName;
	if( strName.IsEmpty() )
		return TID_DIAG_0031;			// "이름을 입력하십시오."

#ifdef __RULE_0615
	// "이름은 한글 2자 이상, 8자 이하로 입력하십시오."
	// "이름은 영문 4자 이상, 16자 이하로 입력하십시오."
	if( strName.GetLength() < 3 || strName.GetLength() > 16 )
		return TID_DIAG_RULE_0;
#else	// __RULE_0615
	if( strName.GetLength() < 3 || strName.GetLength() > 16 )
		return TID_DIAG_0011;			// "명칭에 3글자 이상, 16글자 이하로 입력 입력하십시오."
#endif	// __RULE_0615

	char c = strName[ 0 ];

	if( ( c >= '0' && c <= '9' ) && !IsMultiByte( lpszString ) )
		return TID_DIAG_0012;			// "명칭에 첫글자를 숫자로 사용할 수 없습니다."

	int j;
	switch( ::GetLanguage() )
	{
	case LANG_THA:
		for( j = 0; j < strName.GetLength(); ++j )
		{
			c = strName[ j ];
			if( IsNative( &lpszString[ j ] ) == FALSE && ( isalnum( c ) == FALSE || iscntrl( c ) ) )		
				return TID_DIAG_0013;	// 명칭에 콘트롤이나 스페이스, 특수 문자를 사용할 수 없습니다.	
		}
		break;
	case LANG_TWN:
	case LANG_HK:
		return IsValidPlayerNameTWN( strName );

	default:
		for( j = 0; j < strName.GetLength(); ++j )
		{
			c = strName[ j ];
			if( IsDBCSLeadByte(c) ) 
			{
				++j;
				if( ::GetLanguage() == LANG_KOR )
				{
					char c2 = strName[ j ];
					WORD word = ( ( c << 8 ) & 0xff00 ) | ( c2 & 0x00ff );
					if( IsHangul( word ) == FALSE ) 
						return TID_DIAG_0014;
				}					
			}
			else if( isalnum( c ) == FALSE || iscntrl( c ) )
			{
				char szLetter[2]	= { c, '\0' };
				if( (		::GetLanguage() == LANG_GER 
						||	::GetLanguage() == LANG_RUS
					) && prj.IsAllowedLetter( szLetter ) )
					continue;
				return TID_DIAG_0013;
			}
		}
		break;
	}

	return 0;
}

BOOL CWndCreateChar::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == WNM_CLICKED )
	{
		switch(nID)
		{
		case 10000:
			return FALSE;
		case WIDC_MALE: // male
			SetSex( SEX_MALE );
			break;
		case WIDC_FEMALE: // female
			SetSex( SEX_FEMALE );
			break;
		case WIDC_HAIRSTYLE_LEFT: // hair
			m_Player.m_byHairMesh--;
			if( m_Player.m_byHairMesh < 0 )
				m_Player.m_byHairMesh = MAX_BASE_HAIR - 1;
			CMover::UpdateParts( m_Player.m_bySex, m_Player.m_bySkinSet, m_Player.m_byFace, m_Player.m_byHairMesh, m_Player.m_byHeadMesh, m_Player.m_aEquipInfo, m_pModel, NULL );
			break;
		case WIDC_HAIRSTYLE_RIGHT: // hair
			m_Player.m_byHairMesh++;
			if( m_Player.m_byHairMesh >= MAX_BASE_HAIR )
				m_Player.m_byHairMesh = 0;
			CMover::UpdateParts( m_Player.m_bySex, m_Player.m_bySkinSet, m_Player.m_byFace, m_Player.m_byHairMesh, m_Player.m_byHeadMesh, m_Player.m_aEquipInfo, m_pModel, NULL );
			break;
		case WIDC_FACE_LEFT: // head
			m_Player.m_byHeadMesh--;
			if( m_Player.m_byHeadMesh < 0 )
				m_Player.m_byHeadMesh = MAX_DEFAULT_HEAD - 1;
			CMover::UpdateParts( m_Player.m_bySex, m_Player.m_bySkinSet, m_Player.m_byFace, m_Player.m_byHairMesh, m_Player.m_byHeadMesh, m_Player.m_aEquipInfo, m_pModel, NULL );
			break;
		case WIDC_FACE_RIGHT: // head
			m_Player.m_byHeadMesh++;

			if( m_Player.m_byHeadMesh >= MAX_DEFAULT_HEAD )
				m_Player.m_byHeadMesh = 0;
			CMover::UpdateParts( m_Player.m_bySex, m_Player.m_bySkinSet, m_Player.m_byFace, m_Player.m_byHairMesh, m_Player.m_byHeadMesh, m_Player.m_aEquipInfo, m_pModel, NULL );
			break;

		case WIDC_CANCEL: // Cancel 
			{
				Destroy();
				g_WndMng.ObjectExecutor( SHORTCUT_APPLET, APP_SELECT_CHAR );
				CWndSelectChar* pWndSelectChar 
					= (CWndSelectChar*)g_WndMng.GetWndBase( APP_SELECT_CHAR );
				if( pWndSelectChar )
					pWndSelectChar->UpdateCharacter();
				break;
			}
		case WIDC_OK: // Create
			{
				CWndEdit* pEdit = (CWndEdit*)GetDlgItem( WIDC_NAME );
				CString string = pEdit->m_string;
				
				DWORD dwError = IsValidPlayerName( string );
				if( dwError > 0 )
				{
					g_WndMng.OpenMessageBox( prj.GetText(dwError) );
					pEdit->SetFocus();
					return TRUE;
				}

				if( prj.IsInvalidName( string )
#ifdef __RULE_0615
					|| prj.IsAllowedLetter( string ) == FALSE
#endif	// __RULE_0615
					)
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_DIAG_0020) );	// "사용할수 없는 이름입니다"
					return TRUE;
				}

#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
				CWndEdit* pWnd2ndPassword = ( CWndEdit* )GetDlgItem( WIDC_EDIT_2ND_PASSWORD );
				assert( pWnd2ndPassword );
				CWndEdit* pWnd2ndPasswordConfirm = ( CWndEdit* )GetDlgItem( WIDC_EDIT_2ND_PASSWORD_CONFIRM );
				assert( pWnd2ndPasswordConfirm );
#ifdef __GAMEGUARD
				if (strcmp(pWnd2ndPassword->GetString(), _T("")) <= 0)
#else // __GAMEGUARD
				if (strcmp(pWnd2ndPassword->GetString(), _T("")) == 0)
#endif // __GAMEGUARD
				{
					g_WndMng.OpenMessageBox(prj.GetText(TID_2ND_PASSWORD_INPUT_ERROR01));	// 2차 비밀번호를 입력하여 주십시오.
					return TRUE;
				}
				if( static_cast< int >( strlen( pWnd2ndPassword->GetString() ) ) < MAX_2ND_PASSWORD_NUMBER )
				{
					CString strError = _T( "" );
					strError.Format( prj.GetText( TID_2ND_PASSWORD_INPUT_ERROR02 ), MAX_2ND_PASSWORD_NUMBER );
					g_WndMng.OpenMessageBox( strError );	// 2차 비밀번호는 숫자 %d자리로만 입력해야 합니다.
					return TRUE;
				}
				if( strcmp( pWnd2ndPassword->GetString(), _T( "0000" ) ) == 0 )
				{
					g_WndMng.OpenMessageBox( prj.GetText( TID_2ND_PASSWORD_INPUT_ERROR03 ) );	// 입력하신 비밀번호는 2차 비밀번호로 사용할 수 없습니다.
					return TRUE;
				}
				if( strcmp( pWnd2ndPasswordConfirm->GetString(), _T( "" ) ) == 0 )
				{
					g_WndMng.OpenMessageBox( prj.GetText( TID_2ND_PASSWORD_INPUT_ERROR04 ) );	// 2차 비밀번호 확인을 입력하여 주십시오.
					return TRUE;
				}
				if( strcmp( pWnd2ndPassword->GetString(), pWnd2ndPasswordConfirm->GetString() ) != 0 )
				{
					g_WndMng.OpenMessageBox( prj.GetText( TID_2ND_PASSWORD_INPUT_ERROR05 ) );	// 2차 비밀번호 확인이 2차 비밀번호와 일치하지 않습니다.
					return TRUE;
				}
#endif // __2ND_PASSWORD_SYSTEM
			
				CWndButton*	pButton	= (CWndButton*)GetDlgItem( WIDC_OK );
				pButton->EnableWindow( FALSE );
				pButton	= (CWndButton*)GetDlgItem( WIDC_CANCEL );
				pButton->EnableWindow( FALSE );
				DWORD dwHairColor = 0xffffffff;
				
#ifdef __NEW_COLOR_CHANGE
				BYTE nColorR = (BYTE)((m_fColor[0] * 255));
				BYTE nColorG = (BYTE)((m_fColor[1] * 255));
				BYTE nColorB = (BYTE)((m_fColor[2] * 255));
				dwHairColor = D3DCOLOR_ARGB(255, nColorR, nColorG, nColorB);
#else // __NEW_COLOR_CHANGE
				if (m_Player.m_bySex == SEX_MALE)
				{
					dwHairColor = D3DCOLOR_ARGB(255, nMaleHairColor[m_Player.m_byHairMesh][0],
						nMaleHairColor[m_Player.m_byHairMesh][1],
						nMaleHairColor[m_Player.m_byHairMesh][2]);
				}
				else
				{
					dwHairColor = D3DCOLOR_ARGB(255, nFeMaleHairColor[m_Player.m_byHairMesh][0],
						nFeMaleHairColor[m_Player.m_byHairMesh][1],
						nFeMaleHairColor[m_Player.m_byHairMesh][2]);
				}
#endif // __NEW_COLOR_CHANGE

#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
				g_dpLoginClient.SendCreatePlayer( (BYTE)( m_Player.m_uSlot ), string, m_Player.m_byFace, m_Player.m_byCostume, m_Player.m_bySkinSet, m_Player.m_byHairMesh, dwHairColor, m_Player.m_bySex, m_Player.m_byJob, m_Player.m_byHeadMesh, atoi( pWnd2ndPassword->GetString() ) );
#else // __2ND_PASSWORD_SYSTEM
				g_dpLoginClient.SendCreatePlayer( (BYTE)( m_Player.m_uSlot ), string, m_Player.m_byFace, m_Player.m_byCostume, m_Player.m_bySkinSet, m_Player.m_byHairMesh, dwHairColor, m_Player.m_bySex, m_Player.m_byJob, m_Player.m_byHeadMesh );
#endif // __2ND_PASSWORD_SYSTEM
			}
			break;
		case 10002: // Accept
			{
			g_WndMng.OpenCustomBox( NULL, new CWndConnectingBox );
			g_Neuz.m_dwTempMessage = 1;
			g_Neuz.m_timerConnect.Set( 1 );
			}
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndCreateChar::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}
void CWndCreateChar::OnSize(UINT nType, int cx, int cy)
{

	CWndNeuz::OnSize(nType,cx,cy);
}

#ifdef __NEW_COLOR_CHANGE
void CWndCreateChar::OnLButtonUp(UINT nFlags, CPoint point)
{
#if __VER >= 8 //__CSC_VER8_4
	m_ChoiceBar = -1;
#endif // __CSC_VER8_4
	m_bLButtonClick = FALSE;
#ifdef __CHAR_CREATE_ROTATE
	m_bLButtonDownRot = FALSE;
#endif // __CHAR_CREATE_ROTATE
}

void CWndCreateChar::OnLButtonDown(UINT nFlags, CPoint point)
{
#if __VER >= 8 // __CSC_VER8_4
	for (int i = 0; i < 3; i++)
	{
		CRect DrawRect = m_ColorRect[i];
		if (DrawRect.PtInRect(point))
			m_ChoiceBar = i;
#ifdef __CHAR_CREATE_ROTATE
		else
			m_bLButtonDownRot = TRUE;
#endif // __CHAR_CREATE_ROTATE
	}
#endif // __CSC_VER8_4
	m_bLButtonClick = TRUE;
}

void CWndCreateChar::OnMouseWndSurface(CPoint point)
{
#if __VER >= 8 // __Y_HAIR_BUG_FIX
	if (m_ChoiceBar != -1 && m_bLButtonClick)
	{
		CRect DrawRect = m_ColorRect[m_ChoiceBar];
		point.x = (point.x > DrawRect.right) ? DrawRect.right : point.x;
		point.x = (point.x < DrawRect.left) ? DrawRect.left : point.x;
		LONG Width = DrawRect.right - DrawRect.left;
		LONG Pos = point.x - DrawRect.left;
		FLOAT p = ((FLOAT)((FLOAT)Pos / (FLOAT)Width));

#ifdef __Y_HAIR_BUG_FIX
		D3DXVECTOR2 vec1 = D3DXVECTOR2(0.0f, 1.0f);
		D3DXVECTOR2 vec2 = D3DXVECTOR2(1.0f, 1.0f);
#else // __Y_HAIR_BUG_FIX
		D3DXVECTOR2 vec1 = D3DXVECTOR2(0.3f, 0.998f);
		D3DXVECTOR2 vec2 = D3DXVECTOR2(0.998f, 0.998f);
#endif // __Y_HAIR_BUG_FIX
		D3DXVECTOR2 vec3;
		D3DXVec2Lerp(&vec3, &vec1, &vec2, p);
		m_fColor[m_ChoiceBar] = vec3.x;
		m_ColorScrollBar[m_ChoiceBar].x = point.x;
	}
#else // __CSC_VER8_4

	for (int i = 0; i < 3; i++)
	{
		CRect DrawRect = m_ColorRect[i];
		DrawRect.top -= 22;
		DrawRect.bottom -= 22;
		if (DrawRect.PtInRect(point) && m_bLButtonClick)
		{
			point.x = (point.x > DrawRect.right) ? DrawRect.right : point.x;
			LONG Width = DrawRect.right - DrawRect.left;
			LONG Pos = point.x - DrawRect.left;
			FLOAT p = ((FLOAT)((FLOAT)Pos / (FLOAT)Width));
#ifdef  __Y_HAIR_BUG_FIX
			D3DXVECTOR2 vec1 = D3DXVECTOR2(0.0f, 1.0f);
			D3DXVECTOR2 vec2 = D3DXVECTOR2(1.0f, 1.0f);
#else // __Y_HAIR_BUG_FIX
			D3DXVECTOR2 vec1 = D3DXVECTOR2(0.3f, 1.0f);
			D3DXVECTOR2 vec2 = D3DXVECTOR2(1.0f, 1.0f);
#endif // __Y_HAIR_BUG_FIX
			D3DXVECTOR2 vec3;
			D3DXVec2Lerp(&vec3, &vec1, &vec2, p);
			m_fColor[i] = vec3.x;
			m_ColorScrollBar[i].x = point.x;
		}
	}
#endif // __CSC_VER8_4
#ifdef __CHAR_CREATE_ROTATE
	else
	{
	if (m_bLButtonDownRot)
	{
		m_fRot += (m_OldPos.x - point.x) * 0.5f;
		m_OldPos = point;
	}
	}
	m_OldPos = point;
#endif // __CHAR_CREATE_ROTATE
}

void CWndCreateChar::ReSetBar(FLOAT r, FLOAT g, FLOAT b)
{
#ifdef __Y_HAIR_BUG_FIX
	FLOAT fR = (r / 1.0f) * 100.0f;
	FLOAT fG = (g / 1.0f) * 100.0f;
	FLOAT fB = (b / 1.0f) * 100.0f;
#else // __Y_HAIR_BUG_FIX
	FLOAT fR = ((r - 0.3f) / (1.0f - 0.3f)) * 100.0f;
	FLOAT fG = ((g - 0.3f) / (1.0f - 0.3f)) * 100.0f;
	FLOAT fB = ((b - 0.3f) / (1.0f - 0.3f)) * 100.0f;
#endif // __Y_HAIR_BUG_FIX

#if __VER >= 8 // __CSC_VER8_4
	m_ColorScrollBar[0].x = (LONG)((((m_ColorRect[0].right - m_ColorRect[0].left) * fR) / 100.0f) + m_ColorRect[0].left);
	m_ColorScrollBar[0].y = m_ColorRect[0].top;
	m_ColorScrollBar[1].x = (LONG)((((m_ColorRect[1].right - m_ColorRect[1].left) * fG) / 100.0f) + m_ColorRect[1].left);
	m_ColorScrollBar[1].y = m_ColorRect[1].top;
	m_ColorScrollBar[2].x = (LONG)((((m_ColorRect[2].right - m_ColorRect[2].left) * fB) / 100.0f) + m_ColorRect[2].left);
	m_ColorScrollBar[2].y = m_ColorRect[2].top;
#else // __CSC_VER8_4
	m_ColorScrollBar[0].x = (LONG)((((m_ColorRect[0].right - m_ColorRect[0].left) * fR) / 100.0f) + m_ColorRect[0].left);
	m_ColorScrollBar[0].y = m_ColorRect[0].top - 20;
	m_ColorScrollBar[1].x = (LONG)((((m_ColorRect[1].right - m_ColorRect[1].left) * fG) / 100.0f) + m_ColorRect[1].left);
	m_ColorScrollBar[1].y = m_ColorRect[1].top - 20;
	m_ColorScrollBar[2].x = (LONG)((((m_ColorRect[2].right - m_ColorRect[2].left) * fB) / 100.0f) + m_ColorRect[2].left);
	m_ColorScrollBar[2].y = m_ColorRect[2].top - 20;
#endif // __CSC_VER8_4
}
#else // __NEW_COLOR_CHANGE
void CWndCreateChar::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndCreateChar::OnLButtonDown(UINT nFlags, CPoint point)
{
}
#endif // __NEW_COLOR_CHANGE