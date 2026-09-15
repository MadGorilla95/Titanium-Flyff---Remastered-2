// Theme.cpp: implementation of the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include  "version.h"

#ifdef __LANG_1013
#include "langman.h"
#endif	// __LANG_1013

#ifdef __FLYFF_INITPAGE_EXT
#include "ResData.h"
#endif //__FLYFF_INITPAGE_EXT

char		g_szVersion[128];	// file scope global
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTheme::CTheme()
{
	m_pVBTexture = NULL;
	m_pVBGauge = NULL;
	m_bNudeSkin = FALSE;
	//m_pActiveDesktop = NULL;
	//m_pd3dsdBackBuffer = NULL;
	m_pd3dDevice =NULL;

	m_pFontStatus = NULL;

#ifdef __FLYFF_INITPAGE_EXT
	m_pTitleWorld = NULL;
	m_bLoadTerrainScript = FALSE;
	m_bRenderTitleWorld = FALSE;
	m_dwTexturAlpha1 = 0;
	m_dwTexturAlpha2 = 0;
	m_dwStartTime = 0;
	m_dwEndTime = 0;
	m_bStartCameraWork = FALSE;

	m_pTitaniumLogo = NULL;
#endif //__FLYFF_INITPAGE_EXT

//	m_pFontAPICaption = NULL;
//	m_pFontAPITitle = NULL; 

#ifdef __AEGON_CUSTOM_FONTS
	m_CustomFontMap.clear();
#endif // __AEGON_CUSTOM_FONTS
}
CTheme::~CTheme()
{
	DeleteTheme();
}

#ifdef __AEGON_THEME_SWITCHER
void CTheme::OnThemeChanged()
{
	SwitchButtonFont();
	SwitchThemeFonts();
}
void CTheme::SwitchButtonFont()
{
	__AEGON_THEME_UNIT* pUnit = CAegonThemeSwitcherMng::GetInstance()->GetThemeUnit(g_Option.m_nSelectedThemeID);
	if (!pUnit)
		return;

	DWORD dwFontFlags;

	m_pFontButton = SwitchSingleFont(&pUnit->m_sFontButton);
	m_pFontButtonPushed = SwitchSingleFont(&pUnit->m_sFontButtonHover);

}
void CTheme::SwitchThemeFonts()
{
	__AEGON_THEME_UNIT* pUnit = CAegonThemeSwitcherMng::GetInstance()->GetThemeUnit(g_Option.m_nSelectedThemeID);
	if (!pUnit)
		return;

	m_pFontMessengerTitle = SwitchSingleFont(&pUnit->m_sFontMessengerTitle);
}
#endif

#ifdef __AEGON_CUSTOM_FONTS
void CTheme::LoadSingleFont(CString strFont)
{
	if (!strFont)
		return;

	int nResult = AddFontResourceEx(strFont.GetString(), FR_PRIVATE, 0);
	if (nResult > 0)
		m_CustomFontMap.push_back(strFont);
}
BOOL CTheme::LoadCustomFonts() 
{
	CString strTmp;

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(MakePath(DIR_THEME_FONT, "*"), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	do {
		if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) //not directory
		{
			strTmp.Format("%s", MakePath(DIR_THEME_FONT, FindFileData.cFileName).GetString());
			LoadSingleFont(strTmp);
		}
	} while (FindNextFile(hFind, &FindFileData) != 0);

	FindClose(hFind);

	prj.LoadAvailFont();

	return TRUE;
}
BOOL CTheme::RemoveCustomFonts()
{

	int result = 0;
	for (int i = 0; i < m_CustomFontMap.size(); i++) 
	{
		if (RemoveFontResourceExA(m_CustomFontMap[i].GetString(), 0, 0))
			result++;
	}

	return TRUE;
}
#ifdef __AEGON_FONT_SWITCH
CD3DFont* CTheme::SwitchSingleFont(__AEGON_THEME_FONT* pFontUnit)
{
	if (!pFontUnit)
		return NULL;

	return SwitchSingleFont((LPCSTR)pFontUnit->m_szButtonFont, pFontUnit->m_dwButtonFontSize, pFontUnit->m_dwButtonOutline,
		pFontUnit->m_dwButtonFontColor, pFontUnit->m_dwButtonFontBGColor, pFontUnit->m_dwFlags, pFontUnit->m_dwFontFlags);
}
CD3DFont* CTheme::SwitchSingleFont(LPCSTR szFont, DWORD nHeight, DWORD dwOutline, DWORD dwColor, DWORD dwBgColor, DWORD dwFlags, DWORD dwFontFlags)
{
	CD3DFont* pFont = NULL;
	CString strTmp;
	strTmp.Format("%s-%lu-%lu-%lu-%lu-%lu-%lu", szFont, nHeight, dwOutline, dwColor, dwBgColor, dwFlags, dwFontFlags);

	if (!m_mapFont.Lookup(strTmp.GetString(), (void*&)pFont))
	{
		pFont = new CD3DFont(szFont, nHeight, dwFontFlags);
		pFont->m_nOutLine = dwOutline;
		pFont->m_dwColor = dwColor;
		pFont->m_dwBgColor = dwBgColor;
		pFont->m_dwFlags = dwFlags;

		pFont->InitDeviceObjects(m_pd3dDevice);
		pFont->RestoreDeviceObjects();
		m_mapFont.SetAt(strTmp.GetString(), pFont);
	}
	return pFont;
}
void CTheme::AegonOnSwitchFont()
{
	map<int, __AEGON_THEME_UNIT>::iterator it = CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.find(g_Option.m_nSelectedThemeID);
	PLANG_DATA pLangData = CLangMan::GetInstance()->GetLangData(::GetLanguage());

#ifdef __AEGON_THEME_SWITCHER
	BOOL bFontsSet = FALSE;
	if (it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end())
	{
		if (it->second.nThemeType != 0) 
		{
			DWORD dwColor = it->second.dwTextColor;

			m_pFontText = SwitchSingleFont(g_Option.m_strFont.GetString(), 9, 0, dwColor);
			m_pFontChat = SwitchSingleFont(g_Option.m_strFont.GetString(), 9, 0, dwColor);
			m_pFontStatus = SwitchSingleFont(g_Option.m_strFont.GetString(), 8, 0, dwColor);
			m_pFontWndTitle = SwitchSingleFont(g_Option.m_strFont.GetString(), 9, pLangData->font.afi[3].nOutLine, it->second.dwFontTitleColor, COLOR_BLACK);
			m_pFontWorld = SwitchSingleFont(g_Option.m_strFont.GetString(), 9, pLangData->font.afi[4].nOutLine, 0xffffffff, pLangData->font.afi[4].dwBgColor);
			m_pFontCaption = SwitchSingleFont(g_Option.m_strFont.GetString(), 15, pLangData->font.afi[5].nOutLine, dwColor, COLOR_BLACK, pLangData->font.afi[5].dwFlags);
			m_pFontGuildCombatText = SwitchSingleFont(g_Option.m_strFont.GetString(), 13, 0, dwColor);
			m_pFontSRMyGiuld = SwitchSingleFont(g_Option.m_strFont.GetString(), 11, 1, dwColor);
			m_pFontSRGiuld = SwitchSingleFont(g_Option.m_strFont.GetString(), 9, 1, dwColor);
			bFontsSet = TRUE;
		}
	}
	if(!bFontsSet)
	{
		m_pFontText = SwitchSingleFont(g_Option.m_strFont.GetString(), 9);
		m_pFontChat = SwitchSingleFont(g_Option.m_strFont.GetString(), 9);
		m_pFontStatus = SwitchSingleFont(g_Option.m_strFont.GetString(), 8);
		m_pFontWndTitle = SwitchSingleFont(g_Option.m_strFont.GetString(), 9, pLangData->font.afi[3].nOutLine, 0xffffffff, pLangData->font.afi[3].dwBgColor);
		m_pFontWorld = SwitchSingleFont(g_Option.m_strFont.GetString(), 9, pLangData->font.afi[4].nOutLine, 0xffffffff, pLangData->font.afi[4].dwBgColor);
		m_pFontCaption = SwitchSingleFont(g_Option.m_strFont.GetString(), 15, pLangData->font.afi[5].nOutLine, 0xffffffff, pLangData->font.afi[5].dwBgColor, pLangData->font.afi[5].dwFlags);
		m_pFontGuildCombatText = SwitchSingleFont(g_Option.m_strFont.GetString(), 13);
		m_pFontSRMyGiuld = SwitchSingleFont(g_Option.m_strFont.GetString(), 11, 1);
		m_pFontSRGiuld = SwitchSingleFont(g_Option.m_strFont.GetString(), 9, 1);
	}

#else // __AEGON_THEME_SWITCHER
	CWndBase::m_Theme.m_pFontText = SwitchSingleFont(g_Option.m_strFont.GetString(), 9);
	CWndBase::m_Theme.m_pFontChat = SwitchSingleFont(g_Option.m_strFont.GetString(), 9);
	CWndBase::m_Theme.m_pFontStatus = SwitchSingleFont(g_Option.m_strFont.GetString(), 8);
	CWndBase::m_Theme.m_pFontWndTitle = SwitchSingleFont(g_Option.m_strFont.GetString(), 9, pLangData->font.afi[3].nOutLine, 0xffffffff, pLangData->font.afi[3].dwBgColor);
	CWndBase::m_Theme.m_pFontWorld = SwitchSingleFont(g_Option.m_strFont.GetString(), 9, pLangData->font.afi[4].nOutLine, 0xffffffff, pLangData->font.afi[4].dwBgColor);
	CWndBase::m_Theme.m_pFontCaption = SwitchSingleFont(g_Option.m_strFont.GetString(), 15, pLangData->font.afi[5].nOutLine, 0xffffffff, pLangData->font.afi[5].dwBgColor, pLangData->font.afi[5].dwFlags);
	CWndBase::m_Theme.m_pFontGuildCombatText = SwitchSingleFont(g_Option.m_strFont.GetString(), 13);
	CWndBase::m_Theme.m_pFontSRMyGiuld = SwitchSingleFont(g_Option.m_strFont.GetString(), 11, 1);
	CWndBase::m_Theme.m_pFontSRGiuld = SwitchSingleFont(g_Option.m_strFont.GetString(), 9, 1);
#endif // __AEGON_THEME_SWITCHER
}
#endif // __AEGON_FONT_SWITCH
#endif // __AEGON_CUSTOM_FONTS
BOOL CTheme::LoadTheme( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR lpszFileName)
{
	m_pd3dDevice = pd3dDevice;

	//DeleteTheme();

	CScanner scanner;
	if(scanner.Load( MakePath( DIR_THEME, lpszFileName ) )==FALSE)
		return FALSE;

	scanner.GetToken(); // subject or FINISHED
	while(scanner.tok!=FINISHED)
	{
		if( scanner.Token == "m_d3dcBackground" )
		{
			scanner.GetToken();
			m_d3dcBackground = scanner.GetHex();
		}
		else
		if( scanner.Token == "m_texWallPaper" )
		{
			scanner.GetToken();
			scanner.GetToken();

			m_texWallPaper.LoadTexture( m_pd3dDevice, MakePath( DIR_THEME, scanner.token ), 0xff0000 );
			m_texWndPaper.LoadTexture( m_pd3dDevice, MakePath( DIR_THEME, "WindField.bmp" ), 0xff0000 );
		}	
		else
		if( scanner.Token == "m_dwWallPaperType" )
		{
			scanner.GetToken();
			m_dwWallPaperType = scanner.GetNumber();
		}
		scanner.GetToken();
	}

#ifdef __AEGON_THEME_SWITCHER
	map<int, __AEGON_THEME_UNIT>::iterator it = CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.find(g_Option.m_nSelectedThemeID);
#endif // __AEGON_THEME_SWITCHER

#ifdef __LANG_1013
	PLANG_DATA pLangData	= CLangMan::GetInstance()->GetLangData( ::GetLanguage() );
	CD3DFont* pFont;
	pFont = new CD3DFont( pLangData->font.afi[0].szFont, 9);
#ifdef __AEGON_THEME_SWITCHER
	if (it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end())
	{
		if (it->second.nThemeType != 0)
		{
			pFont->m_dwColor = it->second.dwTextColor;
			pFont->m_dwBgColor = COLOR_BLACK;
		}
	}
#endif // __AEGON_THEME_SWITCHER
	m_mapFont.SetAt( _T( "gulim9"), pFont );
	pFont = new CD3DFont( pLangData->font.afi[1].szFont, 8);
	m_mapFont.SetAt( _T( "gulim8"), pFont );
	pFont = new CD3DFont( pLangData->font.afi[2].szFont, 13);
	m_mapFont.SetAt( _T( "gulim13"), pFont );
	pFont = new CD3DFont( pLangData->font.afi[3].szFont, 9);
	pFont->m_nOutLine	= pLangData->font.afi[3].nOutLine;
#ifdef __AEGON_THEME_SWITCHER

	pFont->m_dwColor = 0xffffffff;
	pFont->m_dwBgColor = pLangData->font.afi[3].dwBgColor;
	if (it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end())
	{
		if (it->second.nThemeType != 0)
		{
			pFont->m_dwColor = it->second.dwFontTitleColor;
			pFont->m_dwBgColor = COLOR_BLACK;
		}
	}
#else // __AEGON_THEME_SWITCHER
	pFont->m_dwColor = 0xffffffff;
	pFont->m_dwBgColor = pLangData->font.afi[3].dwBgColor;
#endif // __AEGON_THEME_SWITCHER
	
	
	m_mapFont.SetAt( _T( "Arial Black9"), pFont );
	pFont = new CD3DFont( pLangData->font.afi[4].szFont, 9);
	pFont->m_nOutLine	= pLangData->font.afi[4].nOutLine;
#ifdef __AEGON_THEME_SWITCHER
	pFont->m_dwColor = 0xffffffff;
	pFont->m_dwBgColor = pLangData->font.afi[4].dwBgColor;
	if (it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end())
	{
		if (it->second.nThemeType != 0)
		{
			pFont->m_dwColor = 0xffffffff;
			pFont->m_dwBgColor = pLangData->font.afi[4].dwBgColor;
		}
	}
#else // __AEGON_THEME_SWITCHER
	pFont->m_dwColor	= 0xffffffff;
	pFont->m_dwBgColor = pLangData->font.afi[4].dwBgColor;
#endif // __AEGON_THEME_SWITCHER
	
	m_mapFont.SetAt( _T( "FontWorld"), pFont );
	pFont = new CD3DFont( pLangData->font.afi[5].szFont, 15);
	pFont->m_nOutLine = pLangData->font.afi[5].nOutLine;
#ifdef __AEGON_THEME_SWITCHER
	pFont->m_dwColor = 0xffffffff;
	pFont->m_dwBgColor = pLangData->font.afi[5].dwBgColor;
	if (it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end())
	{
		if (it->second.nThemeType != 0)
		{
			pFont->m_dwColor = it->second.dwTextColor;
			pFont->m_dwBgColor = COLOR_BLACK;
		}
	}
#else // __AEGON_THEME_SWITCHER
	pFont->m_dwColor	= 0xffffffff;
	pFont->m_dwBgColor = pLangData->font.afi[5].dwBgColor;
#endif // __AEGON_THEME_SWITCHER
	pFont->m_dwFlags	= pLangData->font.afi[5].dwFlags;
	m_mapFont.SetAt( _T( "gulim20"), pFont );
#if __VER >= 12 // __SECRET_ROOM
	pFont = new CD3DFont( pLangData->font.afi[2].szFont, 11, D3DFONT_BOLD );
	pFont->m_nOutLine = 1;
	m_mapFont.SetAt( _T( "gulim11"), pFont );
	pFont = new CD3DFont( pLangData->font.afi[2].szFont, 9, D3DFONT_BOLD );
	pFont->m_nOutLine = 1;
	m_mapFont.SetAt( _T( "gulim9_2"), pFont );
#endif //__SECRET_ROOM
#else	// __LANG_1013
	if( ::GetLanguage() == LANG_KOR )
	{
		CD3DFont* pFont;
		// 폰트 로드 생성 
		pFont = new CD3DFont( _T("gulim"), 9 );//, D3DFONT_BOLD );
		//pFont->m_nOutLine = 3;
		m_mapFont.SetAt( _T( "gulim9"), pFont );
		
		pFont = new CD3DFont( _T("gulim"), 13 );//, D3DFONT_BOLD );
		//pFont->m_nOutLine = 3;
		m_mapFont.SetAt( _T( "gulim13"), pFont );

		pFont = new CD3DFont( _T("gulim"), 8 );//, D3DFONT_BOLD );
		//pFont->m_nOutLine = 3;
		m_mapFont.SetAt( _T( "gulim8"), pFont );
		
		pFont = new CD3DFont( _T("Arial Black"), 9);//, D3DFONT_BOLD );
		//pFont = new CD3DFont( _T("gulim"), 9 );//, D3DFONT_BOLD );
		pFont->m_nOutLine = 2;
		pFont->m_dwColor = 0xffffffff;
		pFont->m_dwBgColor = D3DCOLOR_ARGB( 255, 217, 91, 51);
		m_mapFont.SetAt( _T( "Arial Black9"), pFont );
		
		pFont = new CD3DFont( _T("Arial Black"), 9);//, D3DFONT_BOLD );
		//pFont = new CD3DFont( _T("gulim"), 9 );//, D3DFONT_BOLD );
		pFont->m_nOutLine = 2;
		pFont->m_dwColor = 0xffffffff;
		pFont->m_dwBgColor = D3DCOLOR_ARGB( 255, 60, 60, 60 );
		m_mapFont.SetAt( _T( "FontWorld"), pFont );
		
		pFont = new CD3DFont( _T("가을체"), 15 );//, D3DFONT_BOLD );
		pFont->m_nOutLine = 2;
		pFont->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
		pFont->m_dwBgColor = D3DCOLOR_ARGB( 255, 40, 100, 220 );
		pFont->m_dwFlags = D3DFONT_FILTERED;
		m_mapFont.SetAt( _T( "gulim20"), pFont );

		/*
		m_pFontAPICaption = new CD3DFontAPI( _T("휴먼매직체"), rectClient.Width() / 50 );//, D3DFONT_BOLD );
		m_pFontAPICaption->m_nOutLine = 2;
		m_pFontAPICaption->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
		m_pFontAPICaption->m_dwBgColor = D3DCOLOR_ARGB( 255, 40, 100, 220 );
		m_pFontAPICaption->m_dwFlags = D3DFONT_FILTERED;
		m_pFontAPICaption->InitDeviceObjects( m_pApp->m_pd3dDevice );

		m_pFontAPITitle = new CD3DFontAPI( _T("휴먼매직체"), rectClient.Width() / 20 );//, D3DFONT_BOLD );
		m_pFontAPITitle->m_nOutLine = 2;
		m_pFontAPITitle->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
		m_pFontAPITitle->m_dwBgColor = D3DCOLOR_ARGB( 255, 40, 100, 220 );
		m_pFontAPITitle->m_dwFlags = D3DFONT_FILTERED;
		m_pFontAPITitle->InitDeviceObjects( m_pApp->m_pd3dDevice );
		*/
	}
	else	
	if( ::GetLanguage() == LANG_JAP )
	{
		CD3DFont* pFont;
		// 폰트 로드 생성 
		pFont = new CD3DFont( _T("MS Gothic"), 9 );//, D3DFONT_BOLD );
		//pFont->m_nOutLine = 3;
		m_mapFont.SetAt( _T( "gulim9"), pFont );
		
		pFont = new CD3DFont( _T("MS Gothic"), 8 );//, D3DFONT_BOLD );
		//pFont->m_nOutLine = 3;
		m_mapFont.SetAt( _T( "gulim8"), pFont );
		
		pFont = new CD3DFont( _T("gulim"), 13 );//, D3DFONT_BOLD );
		//pFont->m_nOutLine = 3;
		m_mapFont.SetAt( _T( "gulim13"), pFont );
		
		
		pFont = new CD3DFont( _T("MS Gothic"), 9);//, D3DFONT_BOLD );
		pFont->m_nOutLine = 2;
		pFont->m_dwColor = 0xffffffff;
		pFont->m_dwBgColor = D3DCOLOR_ARGB( 255, 217, 91, 51);
		m_mapFont.SetAt( _T( "Arial Black9"), pFont );
		
		pFont = new CD3DFont( _T("MS Gothic"), 9);//, D3DFONT_BOLD );
		pFont->m_nOutLine = 2;
		pFont->m_dwColor = 0xffffffff;
		pFont->m_dwBgColor = D3DCOLOR_ARGB( 255, 60, 60, 60 );
		m_mapFont.SetAt( _T( "FontWorld"), pFont );
		
		pFont = new CD3DFont( _T("MS Gothic"), 15 );//, D3DFONT_BOLD );
		pFont->m_nOutLine = 2;
		pFont->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
		pFont->m_dwBgColor = D3DCOLOR_ARGB( 255, 40, 100, 220 );
		pFont->m_dwFlags = D3DFONT_FILTERED;
		m_mapFont.SetAt( _T( "gulim20"), pFont );

		/*
		m_pFontAPICaption = new CD3DFontAPI( _T("휴먼매직체"), rectClient.Width() / 50 );//, D3DFONT_BOLD );
		m_pFontAPICaption->m_nOutLine = 2;
		m_pFontAPICaption->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
		m_pFontAPICaption->m_dwBgColor = D3DCOLOR_ARGB( 255, 40, 100, 220 );
		m_pFontAPICaption->m_dwFlags = D3DFONT_FILTERED;
		m_pFontAPICaption->InitDeviceObjects( m_pApp->m_pd3dDevice );

		m_pFontAPITitle = new CD3DFontAPI( _T("휴먼매직체"), rectClient.Width() / 20 );//, D3DFONT_BOLD );
		m_pFontAPITitle->m_nOutLine = 2;
		m_pFontAPITitle->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
		m_pFontAPITitle->m_dwBgColor = D3DCOLOR_ARGB( 255, 40, 100, 220 );
		m_pFontAPITitle->m_dwFlags = D3DFONT_FILTERED;
		m_pFontAPITitle->InitDeviceObjects( m_pApp->m_pd3dDevice );
		*/
	}
	else	
	if( ::GetLanguage() == LANG_THA )
	{
		CD3DFont* pFont;
		// 폰트 로드 생성 
		pFont = new CD3DFont( _T("MS Sans Serif"), 9 );//, D3DFONT_BOLD );
		//pFont->m_nOutLine = 3;
		m_mapFont.SetAt( _T( "gulim9"), pFont );
		
		pFont = new CD3DFont( _T("MS Sans Serif"), 8 );//, D3DFONT_BOLD );
		//pFont->m_nOutLine = 3;
		m_mapFont.SetAt( _T( "gulim8"), pFont );

		pFont = new CD3DFont( _T("MS Sans Serif"), 13 );//, D3DFONT_BOLD );
		//pFont->m_nOutLine = 3;
		m_mapFont.SetAt( _T( "gulim13"), pFont );
		
		pFont = new CD3DFont( _T("MS Sans Serif"), 9);//, D3DFONT_BOLD );
		pFont->m_nOutLine = 2;
		pFont->m_dwColor = 0xffffffff;
		pFont->m_dwBgColor = D3DCOLOR_ARGB( 255, 217, 91, 51);
		m_mapFont.SetAt( _T( "Arial Black9"), pFont );
		
		pFont = new CD3DFont( _T("MS Sans Serif"), 9);//, D3DFONT_BOLD );
		pFont->m_nOutLine = 2;
		pFont->m_dwColor = 0xffffffff;
		pFont->m_dwBgColor = D3DCOLOR_ARGB( 255, 60, 60, 60 );
		m_mapFont.SetAt( _T( "FontWorld"), pFont );
		
		pFont = new CD3DFont( _T("MS Sans Serif"), 13 );//, D3DFONT_BOLD );
		pFont->m_nOutLine = 2;
		pFont->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
		pFont->m_dwBgColor = D3DCOLOR_ARGB( 255, 40, 100, 250 );
		pFont->m_dwFlags = D3DFONT_FILTERED;
		m_mapFont.SetAt( _T( "gulim20"), pFont );

		/*
		m_pFontAPICaption = new CD3DFontAPI( _T("휴먼매직체"), rectClient.Width() / 50 );//, D3DFONT_BOLD );
		m_pFontAPICaption->m_nOutLine = 2;
		m_pFontAPICaption->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
		m_pFontAPICaption->m_dwBgColor = D3DCOLOR_ARGB( 255, 40, 100, 220 );
		m_pFontAPICaption->m_dwFlags = D3DFONT_FILTERED;
		m_pFontAPICaption->InitDeviceObjects( m_pApp->m_pd3dDevice );

		m_pFontAPITitle = new CD3DFontAPI( _T("휴먼매직체"), rectClient.Width() / 20 );//, D3DFONT_BOLD );
		m_pFontAPITitle->m_nOutLine = 2;
		m_pFontAPITitle->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
		m_pFontAPITitle->m_dwBgColor = D3DCOLOR_ARGB( 255, 40, 100, 220 );
		m_pFontAPITitle->m_dwFlags = D3DFONT_FILTERED;
		m_pFontAPITitle->InitDeviceObjects( m_pApp->m_pd3dDevice );

  */
	}
	else
	if( ::GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_HK )
	{
		CD3DFont* pFont;
		// 폰트 로드 생성 
		pFont = new CD3DFont( _T("MingLiU"), 9 );//, D3DFONT_BOLD );
		//pFont->m_nOutLine = 3;
		m_mapFont.SetAt( _T( "gulim9"), pFont );
		
		pFont = new CD3DFont( _T("MingLiU"), 8 );//, D3DFONT_BOLD );
		//pFont->m_nOutLine = 3;
		m_mapFont.SetAt( _T( "gulim8"), pFont );

		pFont = new CD3DFont( _T("MingLiU"), 13 );//, D3DFONT_BOLD );
		//pFont->m_nOutLine = 3;
		m_mapFont.SetAt( _T( "gulim13"), pFont );
		
		pFont = new CD3DFont( _T("MingLiU"), 9);//, D3DFONT_BOLD );
		pFont->m_nOutLine = 2;
		pFont->m_dwColor = 0xffffffff;
		pFont->m_dwBgColor = D3DCOLOR_ARGB( 255, 217, 91, 51);
		m_mapFont.SetAt( _T( "Arial Black9"), pFont );
		
		pFont = new CD3DFont( _T("MingLiU"), 9);//, D3DFONT_BOLD );
		pFont->m_nOutLine = 2;
		pFont->m_dwColor = 0xffffffff;
		pFont->m_dwBgColor = D3DCOLOR_ARGB( 255, 60, 60, 60 );
		m_mapFont.SetAt( _T( "FontWorld"), pFont );
		
		pFont = new CD3DFont( _T("MingLiU"), 13 );//, D3DFONT_BOLD );
		pFont->m_nOutLine = 2;
		pFont->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
		pFont->m_dwBgColor = D3DCOLOR_ARGB( 255, 40, 100, 250 );
		pFont->m_dwFlags = D3DFONT_FILTERED;
		m_mapFont.SetAt( _T( "gulim20"), pFont );
	}			
	else	
	if( ::GetLanguage() == LANG_CHI )
	{
		CD3DFont* pFont;
		// 폰트 로드 생성 
		pFont = new CD3DFont( _T("MingLiU"), 9 );//, D3DFONT_BOLD );
		//pFont->m_nOutLine = 3;
		m_mapFont.SetAt( _T( "gulim9"), pFont );
		
		pFont = new CD3DFont( _T("MingLiU"), 8 );//, D3DFONT_BOLD );
		//pFont->m_nOutLine = 3;
		m_mapFont.SetAt( _T( "gulim8"), pFont );

		pFont = new CD3DFont( _T("MingLiU"), 13 );//, D3DFONT_BOLD );
		//pFont->m_nOutLine = 3;
		m_mapFont.SetAt( _T( "gulim13"), pFont );
		
		pFont = new CD3DFont( _T("MingLiU"), 9);//, D3DFONT_BOLD );
		pFont->m_nOutLine = 2;
		pFont->m_dwColor = 0xffffffff;
		pFont->m_dwBgColor = D3DCOLOR_ARGB( 255, 217, 91, 51);
		m_mapFont.SetAt( _T( "Arial Black9"), pFont );
		
		pFont = new CD3DFont( _T("MingLiU"), 9);//, D3DFONT_BOLD );
		pFont->m_nOutLine = 2;
		pFont->m_dwColor = 0xffffffff;
		pFont->m_dwBgColor = D3DCOLOR_ARGB( 255, 60, 60, 60 );
		m_mapFont.SetAt( _T( "FontWorld"), pFont );
		
		pFont = new CD3DFont( _T("MingLiU"), 13 );//, D3DFONT_BOLD );
		pFont->m_nOutLine = 2;
		pFont->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
		pFont->m_dwBgColor = D3DCOLOR_ARGB( 255, 40, 100, 250 );
		pFont->m_dwFlags = D3DFONT_FILTERED;
		m_mapFont.SetAt( _T( "gulim20"), pFont );

		/*
		m_pFontAPICaption = new CD3DFontAPI( _T("휴먼매직체"), rectClient.Width() / 50 );//, D3DFONT_BOLD );
		m_pFontAPICaption->m_nOutLine = 2;
		m_pFontAPICaption->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
		m_pFontAPICaption->m_dwBgColor = D3DCOLOR_ARGB( 255, 40, 100, 220 );
		m_pFontAPICaption->m_dwFlags = D3DFONT_FILTERED;
		m_pFontAPICaption->InitDeviceObjects( m_pApp->m_pd3dDevice );

		m_pFontAPITitle = new CD3DFontAPI( _T("휴먼매직체"), rectClient.Width() / 20 );//, D3DFONT_BOLD );
		m_pFontAPITitle->m_nOutLine = 2;
		m_pFontAPITitle->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
		m_pFontAPITitle->m_dwBgColor = D3DCOLOR_ARGB( 255, 40, 100, 220 );
		m_pFontAPITitle->m_dwFlags = D3DFONT_FILTERED;
		m_pFontAPITitle->InitDeviceObjects( m_pApp->m_pd3dDevice );
		*/
	}
	else	
	if( ::GetLanguage() == LANG_ENG )
	{
		CD3DFont* pFont;
		// 폰트 로드 생성 
		pFont = new CD3DFont( _T("Arial"), 9 );//, D3DFONT_BOLD );
		//pFont->m_nOutLine = 3;
		m_mapFont.SetAt( _T( "gulim9"), pFont );
		
		pFont = new CD3DFont( _T("Arial"), 8 );//, D3DFONT_BOLD );
		//pFont->m_nOutLine = 3;
		m_mapFont.SetAt( _T( "gulim8"), pFont );

		pFont = new CD3DFont( _T("Arial"), 13 );//, D3DFONT_BOLD );
		//pFont->m_nOutLine = 3;
		m_mapFont.SetAt( _T( "gulim13"), pFont );
		
		
		pFont = new CD3DFont( _T("Arial"), 9);//, D3DFONT_BOLD );
		pFont->m_nOutLine = 2;
		pFont->m_dwColor = 0xffffffff;
		pFont->m_dwBgColor = D3DCOLOR_ARGB( 255, 217, 91, 51);
		m_mapFont.SetAt( _T( "Arial Black9"), pFont );
		
		pFont = new CD3DFont( _T("Arial"), 9);//, D3DFONT_BOLD );
		pFont->m_nOutLine = 2;
		pFont->m_dwColor = 0xffffffff;
		pFont->m_dwBgColor = D3DCOLOR_ARGB( 255, 60, 60, 60 );
		m_mapFont.SetAt( _T( "FontWorld"), pFont );
		
		pFont = new CD3DFont( _T("Arial"), 15 );//, D3DFONT_BOLD );
		pFont->m_nOutLine = 2;
		pFont->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
		pFont->m_dwBgColor = D3DCOLOR_ARGB( 255, 40, 100, 220 );
		pFont->m_dwFlags = D3DFONT_FILTERED;
		m_mapFont.SetAt( _T( "gulim20"), pFont );
	}
#endif	// __LANG_1013

#ifdef __LEESE_RENDER_ITEMCOUNT
	pFont = new CD3DFont(_T("Verdana"), 8, D3DFONT_BOLD);
	pFont->m_nOutLine = 2;
	pFont->m_dwColor = D3DCOLOR_ARGB(255, 255, 221, 102);
	pFont->m_dwBgColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	m_mapFont.SetAt(_T("ItemCount"), pFont);
#endif //__LEESE_RENDER_ITEMCOUNT

#ifdef __ZONE_AFFICHAGE
	pFont = new CD3DFont(_T("Gill Sans Nova Light"), 20, D3DFONT_BOLD);
	pFont->m_dwColor   = D3DCOLOR_ARGB( 255, 217, 182, 113 );
	pFont->m_nOutLine = 1;
	pFont->m_dwBgColor = COLOR_BLACK;
	m_mapFont.SetAt( _T( "ZoneText"), pFont );

	pFont = new CD3DFont(_T("Myriad Pro"), 14, D3DFONT_BOLD);
	pFont->m_dwColor = D3DCOLOR_ARGB(255, 217, 182, 113);
	pFont->m_nOutLine = 1;
	pFont->m_dwBgColor = COLOR_BLACK;
	m_mapFont.SetAt(_T("ZoneTextSub"), pFont);
#endif //__ZONE_AFFICHAGE

#ifdef __AEGON_TOURNAMENT
	pFont = new CD3DFont(_T("Verdana"), 14, D3DFONT_BOLD);
	pFont->m_nOutLine = 2;
	pFont->m_dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);
	pFont->m_dwBgColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	m_mapFont.SetAt(_T("AegonTournamentBracket"), pFont);

	pFont = new CD3DFont(_T("Verdana"), 8, D3DFONT_BOLD);
	pFont->m_nOutLine = 1;
	pFont->m_dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);
	pFont->m_dwBgColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	m_mapFont.SetAt(_T("AegonTournamentTop10"), pFont);
#endif // __AEGON_TOURNAMENT

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
	pFont = new CD3DFont(_T("Atomic Alice Bold"), 12, D3DFONT_BOLD);
	pFont->m_nOutLine = 1;
	pFont->m_dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);
	pFont->m_dwBgColor = D3DCOLOR_ARGB(255, 43, 43, 42);
	m_mapFont.SetAt(_T("AegonAchievementNotice"), pFont);
#endif // __AEGON_ACHIEVEMENT_SYSTEM

#ifdef __AEGON_ADVANCED_ANNOUNCEMENT
	pFont = new CD3DFont(_T("Artifakt Element"), 16);
	pFont->m_dwColor = D3DCOLOR_ARGB(255, 217, 182, 113);
	pFont->m_nOutLine = 1;
	pFont->m_dwBgColor = COLOR_BLACK;
	m_mapFont.SetAt(_T("AegonCaption1"), pFont);
#endif // __AEGON_ADVANCED_ANNOUNCEMENT

#ifdef __AEGON_SEASON_PASS
	//pFont = new CD3DFont(_T("Friz Quadrata Std"), 12);
	//pFont = new CD3DFont(_T("Myriad Pro"), 12);
	pFont = new CD3DFont(_T("Verdana"), 12);
	pFont->m_dwColor = COLOR_WHITE;
	pFont->m_nOutLine = 0;
	pFont->m_dwBgColor = COLOR_BLACK;
	pFont->m_dwAegonSpacing = 1;
	m_mapFont.SetAt(_T("AegonSeasonPassNumField"), pFont);

	//pFont = new CD3DFont(_T("Friz Quadrata Std"), 10);
	//pFont = new CD3DFont(_T("Myriad Pro"), 10);
	pFont = new CD3DFont(_T("Verdana"), 10);

	pFont->m_dwColor = D3DCOLOR_ARGB(255, 217, 182, 113);
	pFont->m_nOutLine = 1;
	pFont->m_dwBgColor = COLOR_BLACK;
	pFont->m_dwAegonSpacing = 1;
	m_mapFont.SetAt(_T("AegonSeasonPassCountdown"), pFont);

	//pFont = new CD3DFont(_T("Friz Quadrata Std"), 12);
	//pFont = new CD3DFont(_T("Myriad Pro"), 12);
	pFont = new CD3DFont(_T("Verdana"), 10);

	pFont->m_dwColor = COLOR_WHITE;
	pFont->m_nOutLine = 1;
	pFont->m_dwBgColor = COLOR_BLACK;
	pFont->m_dwAegonSpacing = 0;
	m_mapFont.SetAt(_T("AegonSeasonPassDescrHeader"), pFont);

	//pFont = new CD3DFont(_T("Friz Quadrata Std"), 10);
	//pFont = new CD3DFont(_T("Myriad Pro"), 10);
	pFont = new CD3DFont(_T("Verdana"), 10);

	pFont->m_dwColor = COLOR_WHITE;
	pFont->m_nOutLine = 1;
	pFont->m_dwBgColor = COLOR_BLACK;
	pFont->m_dwAegonSpacing = 1;
	m_mapFont.SetAt(_T("AegonSeasonPassDescrLevel"), pFont);

	//pFont = new CD3DFont(_T("Arial Nova"), 8);
	//pFont = new CD3DFont(_T("Myriad Pro"), 8);
	pFont = new CD3DFont(_T("Verdana"), 8);

	pFont->m_dwColor = COLOR_WHITE;
	pFont->m_nOutLine = 1;
	pFont->m_dwBgColor = COLOR_BLACK;
	pFont->m_dwAegonSpacing = 1;
	m_mapFont.SetAt(_T("AegonSeasonPassDescr"), pFont);

	//pFont = new CD3DFont(_T("Friz Quadrata Std"), 16);
	//pFont = new CD3DFont(_T("Myriad Pro"), 16);
	pFont = new CD3DFont(_T("Verdana"), 14);

	pFont->m_dwColor = COLOR_WHITE;
	pFont->m_nOutLine = 1;
	pFont->m_dwBgColor = COLOR_BLACK;
	pFont->m_dwAegonSpacing = 1;
	m_mapFont.SetAt(_T("AegonSeasonPassProgLevel"), pFont);

	//pFont = new CD3DFont(_T("Friz Quadrata Std"), 8);
	//pFont = new CD3DFont(_T("Myriad Pro"), 8);
	pFont = new CD3DFont(_T("Verdana"), 8);

	pFont->m_dwColor = COLOR_WHITE;
	pFont->m_nOutLine = 1;
	pFont->m_dwBgColor = COLOR_BLACK;
	pFont->m_dwAegonSpacing = 1;
	m_mapFont.SetAt(_T("AegonSeasonPassProg"), pFont);
#endif // __AEGON_SEASON_PASS

#ifdef __BUTTONS_V19
	pFont = new CD3DFont(_T("Verdana"), 10, D3DFONT_BOLD);
	pFont->m_nOutLine = 1;
	pFont->m_dwColor = 0xffffffff;
	pFont->m_dwBgColor = D3DCOLOR_ARGB(255, 135, 135, 135);
	m_mapFont.SetAt(_T("OptButton"), pFont);

	pFont = new CD3DFont(_T("Verdana"), 10, D3DFONT_BOLD);
	pFont->m_nOutLine = 1;
	pFont->m_dwColor = 0xffffffff;
	pFont->m_dwBgColor = D3DCOLOR_ARGB(255, 199, 107, 75);
	m_mapFont.SetAt(_T("OptButtonPushed"), pFont);
#endif// __BUTTONS_V19

#ifdef __FL_FARM_STAT
	pFont = new CD3DFont(_T("Verdana"), 8, D3DFONT_BOLD);
	pFont->m_nOutLine = 2;
	m_mapFont.SetAt(_T("FarmStat"), pFont);
#endif //__FL_FARM_STAT

#ifdef __AEGON_MESSENGER
	pFont = new CD3DFont(_T("Verdana"), 7);
	pFont->m_dwColor = COLOR_WHITE;
	//pFont->m_nOutLine = 0;
	pFont->m_dwBgColor = COLOR_WHITE;
	//pFont->m_dwAegonSpacing = 1;
	m_mapFont.SetAt(_T("AegonMessengerNotification"), pFont);


	pFont = new CD3DFont(_T("Verdana"), 12);
	pFont->m_dwColor = COLOR_WHITE;
	pFont->m_nOutLine = 1;
	pFont->m_dwBgColor = COLOR_BLACK;
	m_mapFont.SetAt(_T("AegonMessengerTitle"), pFont);
#endif

/*
// Font creation flags
#define D3DFONT_BOLD        0x0001
#define D3DFONT_ITALIC      0x0002
#define D3DFONT_ZENABLE     0x0004
*/


	//SetVersion( ::GetLanguage() ); 

	// 폰트 세팅 
	m_mapFont.Lookup( _T("gulim9"), (void*&)m_pFontChat );
	m_mapFont.Lookup( _T("gulim9"), (void*&)m_pFontText );
	m_mapFont.Lookup( _T("gulim8"), (void*&)m_pFontStatus );
	m_mapFont.Lookup( _T("Arial Black9"), (void*&)m_pFontWndTitle );
	m_mapFont.Lookup( _T("FontWorld"), (void*&)m_pFontWorld );
	m_mapFont.Lookup( _T("gulim20"), (void*&)m_pFontCaption );
	m_mapFont.Lookup( _T("gulim13"), (void*&)m_pFontGuildCombatText );
#if __VER >= 12 // __SECRET_ROOM
	m_mapFont.Lookup( _T("gulim11"), (void*&)m_pFontSRMyGiuld );
	m_mapFont.Lookup( _T("gulim9_2"), (void*&)m_pFontSRGiuld );	
#endif //__SECRET_ROOM
	
#ifdef __ZONE_AFFICHAGE
	m_mapFont.Lookup( _T("ZoneText"), (void*&)m_pFontZoneText );
	m_mapFont.Lookup( _T("ZoneTextSub"), (void*&)m_pFontZoneTextSub);
#endif //__ZONE_AFFICHAGE

#ifdef __AEGON_TOURNAMENT
	m_mapFont.Lookup(_T("AegonTournamentBracket"), (void*&)m_pFontAegonTournamentBracket);
	m_mapFont.Lookup(_T("AegonTournamentTop10"), (void*&)m_pFontAegonTournamentTop10);
#endif // __AEGON_TOURNAMENT

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
	m_mapFont.Lookup(_T("AegonAchievementNotice"), (void*&)m_pFontAegonAchievementNotice);
#endif // __AEGON_ACHIEVEMENT_SYSTEM

#ifdef __AEGON_ADVANCED_ANNOUNCEMENT
	m_mapFont.Lookup(_T("AegonCaption1"), (void*&)m_pFontAegonCaption1);
#endif // __AEGON_ADVANCED_ANNOUNCEMENT
	
#ifdef __AEGON_SEASON_PASS
	m_mapFont.Lookup(_T("AegonSeasonPassNumField"), (void*&)m_pFontSeasonPassNumField);
	m_mapFont.Lookup(_T("AegonSeasonPassCountdown"), (void*&)m_pFontSeasonPassCountdown);
	m_mapFont.Lookup(_T("AegonSeasonPassDescrHeader"), (void*&)m_pFontSeasonPassDescrHeader);
	m_mapFont.Lookup(_T("AegonSeasonPassDescrLevel"), (void*&)m_pFontSeasonPassDescrLevel);
	m_mapFont.Lookup(_T("AegonSeasonPassDescr"), (void*&)m_pFontSeasonPassDescr);
	m_mapFont.Lookup(_T("AegonSeasonPassProgLevel"), (void*&)m_pFontSeasonPassProgLevel);
	m_mapFont.Lookup(_T("AegonSeasonPassProg"), (void*&)m_pFontSeasonPassProg);	
#endif // __AEGON_SEASON_PASS
	
#ifdef __BUTTONS_V19
	m_mapFont.Lookup(_T("OptButton"), (void*&)m_pFontButton);
	m_mapFont.Lookup(_T("OptButtonPushed"), (void*&)m_pFontButtonPushed);
#endif// __BUTTONS_V19

#ifdef __FL_FARM_STAT
	m_mapFont.Lookup(_T("FarmStat"), (void*&)m_pFontFarmStat);
#endif //__FL_FARM_STAT

#ifdef __LEESE_RENDER_ITEMCOUNT
	m_mapFont.Lookup(_T("ItemCount"), (void*&)m_pFontItemCount);
#endif //__LEESE_RENDER_ITEMCOUNT


#ifdef __AEGON_MESSENGER
	m_mapFont.Lookup(_T("AegonMessengerNotification"), (void*&)m_pFontMessengerNotification);
	m_mapFont.Lookup(_T("AegonMessengerTitle"), (void*&)m_pFontMessengerTitle);
#endif

	return TRUE;
}
BOOL CTheme::SaveTheme(LPCTSTR lpszFileName)
{
	return TRUE;
}
void CTheme::DeleteTheme()
{
	DeleteDeviceObjects();
} 
HRESULT CTheme::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	POSITION pos = m_mapFont.GetStartPosition();
	CString strFont; CD3DFont* pFont;
	while(pos)
	{
		m_mapFont.GetNextAssoc( pos, strFont, (void*&)pFont );
		pFont->InitDeviceObjects( pd3dDevice );
	}
//	g_mesh.InitDeviceObjects( m_pd3dDevice );
	//g_mesh.OpenMesh( _T("obj_rideferriswheel.mes") );

//	g_mesh.OpenMesh( _T("obj_starship.mes") );
	
	return TRUE;//m_pFontGameTitle->InitDeviceObjects( pd3dDevice );
}
HRESULT CTheme::InvalidateDeviceObjects()
{
#ifdef __YDEBUG
	m_texWallPaper.Invalidate();
	m_texWndPaper.Invalidate();
#endif //__YDEBUG

	HRESULT h = S_OK;
    SAFE_RELEASE( m_pVBTexture );
	SAFE_RELEASE( m_pVBGauge );
	//g_mesh.InvalidateDeviceObjects();
	POSITION pos = m_mapFont.GetStartPosition();
	CString strFont; CD3DFont* pFont;
	while(pos)
	{
		m_mapFont.GetNextAssoc( pos, strFont, (void*&)pFont );
		pFont->InvalidateDeviceObjects();
	}
	return h;
}
HRESULT CTheme::DeleteDeviceObjects()
{
	HRESULT h = S_OK;
	m_texWallPaper.DeleteDeviceObjects();
	m_texWndPaper.DeleteDeviceObjects();

	POSITION pos = m_mapFont.GetStartPosition();
	CString strFont; CD3DFont* pFont;
	while(pos)
	{
		m_mapFont.GetNextAssoc( pos, strFont, (void*&)pFont );
		pFont->DeleteDeviceObjects();
		SAFE_DELETE( pFont );
	}
	m_mapFont.RemoveAll();

	return h;
}
HRESULT CTheme::RestoreDeviceObjects( )
{
#ifdef __YDEBUG
	m_texWallPaper.SetInvalidate(m_pd3dDevice);
	m_texWndPaper.SetInvalidate(m_pd3dDevice);
#endif //__YDEBUG
	
	//if( m_pVBTexture ) 
		//return S_OK;
	//g_mesh.RestoreDeviceObjects();
	POSITION pos = m_mapFont.GetStartPosition();
	CString strFont; CD3DFont* pFont;
	while(pos)
	{
		m_mapFont.GetNextAssoc( pos, strFont, (void*&)pFont );
		pFont->RestoreDeviceObjects();
	}
	HRESULT hr = S_OK;
	m_pd3dDevice->CreateVertexBuffer( sizeof(TEXTUREVERTEX)*24, D3DUSAGE_WRITEONLY| D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX, D3DPOOL_SYSTEMMEM, &m_pVBTexture      , NULL);
	m_pd3dDevice->CreateVertexBuffer( sizeof(DRAWVERTEX)*42, D3DUSAGE_WRITEONLY| D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX, D3DPOOL_SYSTEMMEM, &m_pVBGauge       , NULL );
	return hr;
}

#ifdef __FLYFF_INITPAGE_EXT
void CTheme::ReadTitleWorld()
{
	if (!m_pTitleWorld)
	{
		if (!m_bLoadTerrainScript)
			m_bLoadTerrainScript = prj.m_terrainMng.LoadScript("terrain.inc");

		if (m_bLoadTerrainScript)
		{
			m_pTitleWorld = g_WorldMng.Open(m_pd3dDevice, WI_WORLD_LOGIN);
			if(m_pTitleWorld)
			{					
				D3DXVECTOR3 vecWorld(67.78f, 109.83f, 2.5f);
				m_pTitleWorld->ReadWorld(vecWorld);
				D3DXVECTOR3 vecPos(67.78f, 109.83f, 2.5f);
				D3DXVECTOR3 vecLookat(67.78f, 109.83f, 2.5f + 1.0f);

				CCamera* pCamera = new CCamera;
				pCamera->SetPos(vecPos);
				pCamera->m_vLookAt = vecLookat;
				m_pTitleWorld->SetCamera(pCamera);
				m_pTitaniumLogo = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pd3dDevice, "logotitanium.png", COLOR_ITEM_BG, TRUE);
				m_bRenderTitleWorld = TRUE;
			}
		}
	}
}

void CTheme::DestoryTitleWorld()
{
	CWorld* pCurrentWorld = g_WorldMng.Get();

	if (m_pTitleWorld && pCurrentWorld) 
	{
		if (m_pTitleWorld == pCurrentWorld) 
		{
			g_WorldMng.DestroyCurrentWorld();
		}
	}
	m_pTitleWorld = NULL;
	m_bRenderTitleWorld = FALSE;
	m_dwTexturAlpha1 = 0;
	m_dwTexturAlpha2 = 0;
}
#endif //__FLYFF_INITPAGE_EXT

HRESULT CTheme::FrameMove()
{
#ifdef __FLYFF_INITPAGE_EXT
	if(m_pTitleWorld)
	{
		m_pTitleWorld->Process();

		if(!m_bStartCameraWork)
		{
			static const DWORD MAX_ALPHA = 255;
			static const DWORD EVENT_ALPHA = 155;
			static const DWORD EFFECT_ALPHA = 100;
			static const float ALPHA_FRAME = 2.0f;

			static bool bEffect = false;

			if( bEffect == false )
			{
				if(m_dwTexturAlpha1 < MAX_ALPHA)
				{
					m_dwTexturAlpha1 += static_cast<DWORD>(ALPHA_FRAME);
					if(m_dwTexturAlpha1 > MAX_ALPHA)
						m_dwTexturAlpha1 = MAX_ALPHA;
				}

				if(m_dwTexturAlpha1 > EVENT_ALPHA && m_dwTexturAlpha2 < MAX_ALPHA)
				{
					m_dwTexturAlpha2 += static_cast<DWORD>(ALPHA_FRAME);
					if(m_dwTexturAlpha2 > MAX_ALPHA)
						m_dwTexturAlpha2 = MAX_ALPHA;
				}
			}
			else
			{
				if(m_dwTexturAlpha1 > EFFECT_ALPHA)
				{
					m_dwTexturAlpha1 -= static_cast<DWORD>(ALPHA_FRAME);
					if(m_dwTexturAlpha1 < EFFECT_ALPHA)
						m_dwTexturAlpha1 = EFFECT_ALPHA;
				}

				if(m_dwTexturAlpha2 > EFFECT_ALPHA)
				{
					m_dwTexturAlpha2 -= static_cast<DWORD>(ALPHA_FRAME);
					if(m_dwTexturAlpha2 < EFFECT_ALPHA)
						m_dwTexturAlpha2 = EFFECT_ALPHA;
				}
			}

			if( m_dwTexturAlpha1 == MAX_ALPHA && m_dwTexturAlpha2 == MAX_ALPHA )
			{
				bEffect = true;
			}

			if( m_dwTexturAlpha1 == EFFECT_ALPHA && m_dwTexturAlpha2 == EFFECT_ALPHA )
			{
				bEffect = false;
			}
		}
	}
#endif //__FLYFF_INITPAGE_EXT
	return S_OK;
}
void CTheme::RenderTitle( C2DRender* p2DRender )
{
}
void CTheme::RenderDesktop( C2DRender* p2DRender )
{
#ifdef __FLYFF_INITPAGE_EXT
	if (g_Option.m_nLoginVideo == FALSE)
	{
		D3DVIEWPORT9 viewport;
		viewport.X = 0;
		viewport.Y = 0;
		viewport.Width = 1360;
		viewport.Height = 768;
		//viewport.Width = g_WndMng.resizeWidth;
		//viewport.Height = g_WndMng.resizeHeight;
		viewport.MinZ = 0.0f;
		viewport.MaxZ = 1.0f;

		FLOAT fAspect = (FLOAT)viewport.Width / (FLOAT)viewport.Height;
		float fFov = D3DX_PI / 4.0f;
		float fNear = CWorld::m_fNearPlane;

		D3DXMatrixPerspectiveFovLH(&m_pTitleWorld->m_matProj, fFov, fAspect, fNear - 0.01f, CWorld::m_fFarPlane);
		p2DRender->m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_pTitleWorld->m_matProj);

		DWORD dwColor = CWorld::GetDiffuseColor();
		p2DRender->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, dwColor, 1.0f, 0);

		if (m_pTitleWorld)
		{
			static const float START_X = 67.78f;
			static const float START_Y = 109.83f;
			static const float START_Z = 2.53f;

			static const float END_X = 7210.0f;
			static const float START_TO_END_X = END_X - START_X;
			
			static const float END_Y = 100.0f;
			static const float START_TO_END_Y = END_Y - START_Y;
			
			static const float END_Z = 3500.0f;
			static const float START_TO_END_Z = END_Z - START_Z;

			static const float SPEED_RATE = 3500.0f;

			static float fCameraPositionX = START_X;
			static float fCameraPositionY = START_Y;
			static float fCameraPositionZ = START_Z;

			/*
			static float fCameraSpeedX = START_TO_END_X / SPEED_RATE;
			if (fCameraPositionX <= END_X)
			{
				fCameraPositionX += fCameraSpeedX;
			}
			
			static float fCameraSpeedY = START_TO_END_Y / SPEED_RATE;
			if (fCameraPositionY <= END_Y)
			{
				fCameraPositionY += fCameraSpeedY;
			}
			
			static float fCameraSpeedZ = START_TO_END_Z / SPEED_RATE;
			if (fCameraPositionZ <= END_Z)
			{
				fCameraPositionZ += fCameraSpeedZ;
			}
			*/

			D3DXVECTOR3 vecPos(fCameraPositionX, fCameraPositionY, fCameraPositionZ);
			D3DXVECTOR3 vecLookat(fCameraPositionX - 0.25f, fCameraPositionY + 1, fCameraPositionZ + 12);
			//D3DXVECTOR3 vecLookat(67.78f, 109.83f, 1.0f);

			/*
			CString strPos, strLookAt;

			strPos.Format("cam: x: %f - y: %f - z: %f", vecPos.x, vecPos.y, vecPos.z);
			strLookAt.Format("cam: x: %f - y: %f - z: %f", vecLookat.x, vecLookat.y, vecLookat.z);

			g_WndMng.PutString(strPos.GetString(), 0, COLOR_ERROR);
			g_WndMng.PutString(strLookAt.GetString(), 0, COLOR_ERROR);
			*/

			CCamera* pCamera = m_pTitleWorld->GetCamera();
			if (!pCamera) 
			{
				pCamera = new CCamera;
				m_pTitleWorld->SetCamera(pCamera);
			}
			pCamera->SetPos(vecPos);
			pCamera->m_vLookAt = vecLookat;
			
			m_pTitleWorld->Render(p2DRender->m_pd3dDevice, m_pFontWorld);

			p2DRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			p2DRender->m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			p2DRender->m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			p2DRender->m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
			p2DRender->m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
			p2DRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			p2DRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			p2DRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0x08);

			if (m_pTitaniumLogo)
			{
				CPoint ptPos;
				int nWidth = g_WndMng.GetClientRect().Width();
				int nHeight = g_WndMng.GetClientRect().Height();
				ptPos.x = (nWidth / 2) - (m_pTitaniumLogo->m_size.cx / 2);
				ptPos.y = nHeight / 20;
				m_pTitaniumLogo->Render(p2DRender, ptPos, m_dwTexturAlpha1);
			}
		}
	}
	else
	{
#if __VER >= 9 // __CSC_VER9_RESOLUTION
		int xOffset = 0;
		int rectWidth = 0;
		BOOL isWide = FALSE;
#endif //__CSC_VER9_RESOLUTION
		CTexture texture = m_texWallPaper;
		texture.SetAutoFree(FALSE);
		p2DRender->m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		CRect rectWindow = p2DRender->m_clipRect;
		if (m_dwWallPaperType == WPT_STRETCH) // 전체 늘리기 
		{
#if __VER >= 9 // __CSC_VER9_RESOLUTION
			if (rectWindow.Width() == 1280 && (rectWindow.Height() == 720 || rectWindow.Height() == 768 || rectWindow.Height() == 800)) //Wide
			{
				rectWidth = 960;
				isWide = TRUE;
			}
			else
			{
				switch (rectWindow.Width())
				{
				case 1360:
					rectWidth = 1024;
					isWide = TRUE;
					break;
				case 1440:
					rectWidth = 1200;
					isWide = TRUE;
					break;
				case 1680:
					rectWidth = 1400;
					isWide = TRUE;
					break;
				}
			}

			if (isWide)
			{
				texture.m_size.cx = rectWidth;
				texture.m_size.cy = rectWindow.Height();
				xOffset = (rectWindow.Width() - rectWidth) / 2;
			}
			else
			{
				texture.m_size.cx = rectWindow.Width();
				texture.m_size.cy = rectWindow.Height();
			}

			p2DRender->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, m_d3dcBackground, 1.0f, 0);
			p2DRender->RenderTexture(CPoint(xOffset, 0), &texture);
#else //__CSC_VER9_RESOLUTION
			texture.m_size.cx = rectWindow.Width();
			texture.m_size.cy = rectWindow.Height();
			p2DRender->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, m_d3dcBackground, 1.0f, 0);
			p2DRender->RenderTexture(CPoint(0, 0), &texture);
#endif //__CSC_VER9_RESOLUTION
		}
		else if (m_dwWallPaperType == WPT_CENTER) // 중앙 정렬 
		{
			CPoint pt((rectWindow.Width() / 2) - (texture.m_size.cx / 2), (rectWindow.Height() / 2) - (texture.m_size.cy / 2));
			p2DRender->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, m_d3dcBackground, 1.0f, 0);
			p2DRender->RenderTexture(pt, &texture);
		}
		else if (m_dwWallPaperType == WPT_CENTERSTRETCH) // 중앙 늘리기  
		{
			if (((int)rectWindow.Width() - texture.m_size.cx) < ((int)rectWindow.Height() - texture.m_size.cy))
			{
				// texture.m_size.cx : m_pd3dsdBackBuffer->Width = texture.m_size.cy : y;
				texture.m_size.cy = rectWindow.Width() * texture.m_size.cy / texture.m_size.cx;
				texture.m_size.cx = rectWindow.Width();
			}
			else
			{
				// texture.m_size.cy : m_pd3dsdBackBuffer->Height = texture.m_size.cx : x;
				texture.m_size.cx = rectWindow.Height() * texture.m_size.cx / texture.m_size.cy;
				texture.m_size.cy = rectWindow.Height();
			}

			CPoint pt((rectWindow.Width() / 2) - (texture.m_size.cx / 2), (rectWindow.Height() / 2) - (texture.m_size.cy / 2));
			p2DRender->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, m_d3dcBackground, 1.0f, 0);
			p2DRender->RenderTexture(pt, &texture);
		}
		else if (m_dwWallPaperType == WPT_TILE) // 타일 정렬 
		{
			FLOAT fu = (FLOAT)rectWindow.Width() / texture.m_size.cx;
			FLOAT fv = (FLOAT)rectWindow.Height() / texture.m_size.cy;
			texture.m_size.cx = rectWindow.Width();
			texture.m_size.cy = rectWindow.Height();
			texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
			texture.m_fuRT = fu; texture.m_fvRT = 0.0f;
			texture.m_fuLB = 0.0f; texture.m_fvLB = fv;
			texture.m_fuRB = fu; texture.m_fvRB = fv;
			p2DRender->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, m_d3dcBackground, 1.0f, 0);
			p2DRender->RenderTexture(CPoint(0, 0), &texture);
		}
		p2DRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		p2DRender->m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
#if __VER >= 9 // __CSC_VER9_RESOLUTION
		p2DRender->TextOut(1 + xOffset, 1, "", 0xffffffff);
		p2DRender->TextOut(50 + xOffset, 1, g_szVersion, 0xffffffff);
#else //__CSC_VER9_RESOLUTION
		p2DRender->TextOut(1, 1, "", 0xffffffff);
		p2DRender->TextOut(50, 1, g_szVersion, 0xffffffff);
#endif //__CSC_VER9_RESOLUTION
	}
#else //__FLYFF_INITPAGE_EXT
#if __VER >= 9 // __CSC_VER9_RESOLUTION
	int xOffset = 0;
	int rectWidth = 0;
	BOOL isWide = FALSE;
#endif //__CSC_VER9_RESOLUTION
	CTexture texture
		= m_texWallPaper;
	texture.SetAutoFree( FALSE );
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	CRect rectWindow = p2DRender->m_clipRect;
	if( m_dwWallPaperType == WPT_STRETCH ) // 전체 늘리기 
	{
#if __VER >= 9 // __CSC_VER9_RESOLUTION
		if(rectWindow.Width() == 1280 && (rectWindow.Height() == 720 || rectWindow.Height() == 768 || rectWindow.Height() == 800)) //Wide
		{
			rectWidth = 960;
			isWide = TRUE;
		}
		else
		{
			switch(rectWindow.Width())
			{
			case 1360:
				rectWidth = 1024;
				isWide = TRUE;
				break;
			case 1440:
				rectWidth = 1200;
				isWide = TRUE;
				break;
			case 1680:
				rectWidth = 1400;
				isWide = TRUE;
			    break;
			}
		}

		if(isWide)
		{
			texture.m_size.cx = rectWidth;
			texture.m_size.cy = rectWindow.Height();
			xOffset = (rectWindow.Width() - rectWidth) / 2;
		}
		else
		{
			texture.m_size.cx = rectWindow.Width();
			texture.m_size.cy = rectWindow.Height();
		}

		p2DRender->m_pd3dDevice->Clear(0, NULL,  D3DCLEAR_TARGET, m_d3dcBackground, 1.0f, 0 ) ;
		p2DRender->RenderTexture( CPoint( xOffset, 0 ), &texture );
#else //__CSC_VER9_RESOLUTION
		texture.m_size.cx = rectWindow.Width();
		texture.m_size.cy = rectWindow.Height();
		p2DRender->m_pd3dDevice->Clear(0, NULL,  D3DCLEAR_TARGET, m_d3dcBackground, 1.0f, 0 ) ;
		p2DRender->RenderTexture( CPoint( 0, 0 ), &texture );
#endif //__CSC_VER9_RESOLUTION
	}
	else
	if( m_dwWallPaperType == WPT_CENTER ) // 중앙 정렬 
	{
		CPoint pt( ( rectWindow.Width() / 2 ) - ( texture.m_size.cx / 2 ), ( rectWindow.Height() / 2 ) - ( texture.m_size.cy / 2 ) );
		p2DRender->m_pd3dDevice->Clear(0, NULL,  D3DCLEAR_TARGET, m_d3dcBackground, 1.0f, 0 ) ;
		p2DRender->RenderTexture( pt, &texture );
	}
	else
	if( m_dwWallPaperType == WPT_CENTERSTRETCH ) // 중앙 늘리기  
	{
		if(( (int) rectWindow.Width() - texture.m_size.cx ) < ( (int)rectWindow.Height() - texture.m_size.cy ) )
		{
			// texture.m_size.cx : m_pd3dsdBackBuffer->Width = texture.m_size.cy : y;
			texture.m_size.cy = rectWindow.Width() * texture.m_size.cy / texture.m_size.cx;
			texture.m_size.cx = rectWindow.Width();
		}
		else
		{
			// texture.m_size.cy : m_pd3dsdBackBuffer->Height = texture.m_size.cx : x;
			texture.m_size.cx = rectWindow.Height() * texture.m_size.cx / texture.m_size.cy;
			texture.m_size.cy = rectWindow.Height();
		}

		CPoint pt( ( rectWindow.Width() / 2 ) - ( texture.m_size.cx / 2 ), ( rectWindow.Height() / 2 ) - ( texture.m_size.cy / 2 ) );
		p2DRender->m_pd3dDevice->Clear(0, NULL,  D3DCLEAR_TARGET, m_d3dcBackground, 1.0f, 0 ) ;
		p2DRender->RenderTexture( pt, &texture );
	}
	else
	if( m_dwWallPaperType == WPT_TILE ) // 타일 정렬 
	{
		FLOAT fu = (FLOAT)rectWindow.Width()  / texture.m_size.cx;
		FLOAT fv = (FLOAT)rectWindow.Height() / texture.m_size.cy;
		texture.m_size.cx = rectWindow.Width();
		texture.m_size.cy = rectWindow.Height();
		texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
		texture.m_fuRT = fu  ; texture.m_fvRT = 0.0f;
		texture.m_fuLB = 0.0f; texture.m_fvLB = fv  ;
		texture.m_fuRB = fu  ; texture.m_fvRB = fv  ;
		p2DRender->m_pd3dDevice->Clear(0, NULL,  D3DCLEAR_TARGET, m_d3dcBackground, 1.0f, 0 ) ;
		p2DRender->RenderTexture( CPoint( 0, 0), &texture );
	}
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
#if __VER >= 9 // __CSC_VER9_RESOLUTION
	p2DRender->TextOut( 1 + xOffset, 1, "", 0xffffffff  );
	p2DRender->TextOut( 50 + xOffset, 1, g_szVersion, 0xffffffff  );
#else //__CSC_VER9_RESOLUTION
	p2DRender->TextOut(  1, 1, "", 0xffffffff  );
	p2DRender->TextOut( 50, 1, g_szVersion, 0xffffffff  );
#endif //__CSC_VER9_RESOLUTION
#endif //__FLYFF_INITPAGE_EXT
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTheme::GradationRect( C2DRender* p2DRender, CRect* pRect, DWORD dwColor1t, DWORD dwColor1b, DWORD dwColor2b, int nMidPercent )
{
	int nFirstHeight = pRect->Height() * nMidPercent / 100;
	
	CRect rect1 = *pRect; rect1.bottom = rect1.top + nFirstHeight;
	CRect rect2 = *pRect; rect2.top    = rect2.top + nFirstHeight;

	p2DRender->RenderFillRect( rect1, dwColor1t, dwColor1t, dwColor1b, dwColor1b );
	p2DRender->RenderFillRect( rect2, dwColor1b, dwColor1b, dwColor2b, dwColor2b );
}
#ifdef __AEGON_THEME_SWITCHER
void CTheme::RenderWndBaseTitleBar(C2DRender* p2DRender, CRect* pRect, LPCTSTR lpszTitle, DWORD dwColor, int nTitleDrawOffsetY)
#else
void CTheme::RenderWndBaseTitleBar( C2DRender* p2DRender, CRect* pRect, LPCTSTR lpszTitle, DWORD dwColor  )
#endif
{
	//DWORD dwColor1 = D3DCOLOR_ARGB( 250, 255, 255, 255 );//D3DCOLOR_ARGB( 255, 130, 130, 230 );//
	//DWORD dwColor2 = D3DCOLOR_ARGB( 50,    0,   0,  00 );//D3DCOLOR_ARGB( 255,  50,  50, 100 );//
	//DWORD dwColor3 = D3DCOLOR_ARGB( 200, 150, 150, 150 );//D3DCOLOR_ARGB( 255, 180, 180, 220 );//
	DWORD dwColor1 = D3DCOLOR_ARGB( 255, 255, 255, 255 );//D3DCOLOR_ARGB( 255, 130, 130, 230 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255,  150, 150, 150 );//D3DCOLOR_ARGB( 255,  50,  50, 100 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 255, 230, 230, 230 );//D3DCOLOR_ARGB( 255, 180, 180, 220 );//

	GradationRect( p2DRender, pRect, dwColor1 ,dwColor2, dwColor3 );
	//p2DRender->RenderLine( CPoint( pRect->left, pRect->top ), CPoint( pRect->right, pRect->top ), dwColor3 );

#ifdef __AEGON_THEME_SWITCHER
	p2DRender->TextOut(17, 7 + nTitleDrawOffsetY, lpszTitle, dwColor);
#else
	p2DRender->TextOut( 17, 7, lpszTitle, dwColor);
#endif
	//p2DRender->TextOut( 16, 6, lpszTitle, 0xffffffff);


}
void CTheme::RenderWndBaseFrame( C2DRender* p2DRender, CRect* pRect )
{
	//DWORD dwColor1 = D3DCOLOR_ARGB( 100,   0,   0,   0 );//D3DCOLOR_ARGB( 255,  50,  50, 100 );//
	//DWORD dwColor2 = D3DCOLOR_ARGB( 100,  80,  80,  80 );//D3DCOLOR_ARGB( 255, 180, 180, 220 );//
	//DWORD dwColor3 = D3DCOLOR_ARGB( 150, 160, 160, 160 );//D3DCOLOR_ARGB( 255, 180, 180, 220 );//
	//DWORD dwColor4 = D3DCOLOR_ARGB( 100,   0,   0,   0 );//D3DCOLOR_ARGB( 255,  50,  50, 150 );//
	DWORD dwColor1 = D3DCOLOR_ARGB( 255,   0,   0,   0 );//D3DCOLOR_ARGB( 255,  50,  50, 100 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255,  80,  80,  80 );//D3DCOLOR_ARGB( 255, 180, 180, 220 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 255, 160, 160, 160 );//D3DCOLOR_ARGB( 255, 180, 180, 220 );//
	DWORD dwColor4 = D3DCOLOR_ARGB( 255,   0,   0,   0 );//D3DCOLOR_ARGB( 255,  50,  50, 150 );//

	//CRect rectTemp = *pRect;
	//rectTemp.DeflateRect(2,2);
	//rectTemp.bottom = 20;
	if( m_bNudeSkin == FALSE )
	{
		CTexture texture
			= m_texWndPaper;
		texture.SetAutoFree( FALSE );

		FLOAT fu, fv;


	//	p2DRender->RenderTexture( CPoint( 0, 0), &texture, 0 );

		//////////////////////////////////

		CRect rect, rectOrg = *pRect;
		rectOrg += p2DRender->m_ptOrigin;
		
		
		TEXTUREVERTEX* pVertices;
		m_pVBTexture->Lock( 0, sizeof(TEXTUREVERTEX) * 24, (void**) &pVertices, 0 );

		// horizon

		rect.SetRect( rectOrg.left, rectOrg.top, rectOrg.right, rectOrg.top + 16 + 4 );
		fu = (FLOAT)rect.Width()  / texture.m_size.cx;
		fv = (FLOAT)rect.Height() / texture.m_size.cy;
		texture.m_size.cx = rect.Width();
		texture.m_size.cy = rect.Height();
		texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
		texture.m_fuRT = fu  ; texture.m_fvRT = 0.0f;
		texture.m_fuLB = 0.0f; texture.m_fvLB = fv  ;
		texture.m_fuRB = fu  ; texture.m_fvRB = fv  ;

		SetTextureVertex( pVertices, (FLOAT)rect.left,  (FLOAT)rect.top,    texture. m_fuLT + 0.000001f, texture.m_fvLT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.right, (FLOAT)rect.top,    texture. m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.left,  (FLOAT)rect.bottom ,texture. m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f ); pVertices++;

		SetTextureVertex( pVertices, (FLOAT)rect.right,  (FLOAT)rect.top,    texture. m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.left,   (FLOAT)rect.bottom, texture. m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.right,  (FLOAT)rect.bottom ,texture. m_fuRB + 0.000001f, texture.m_fvRB + 0.000001f ); pVertices++;
		
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLT + 0.000001f; pVertices->v = texture.m_fvLT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;

		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRB + 0.000001f; pVertices->v = texture.m_fvRB + 0.000001f; pVertices++;

		texture	= m_texWndPaper;
		texture.SetAutoFree( FALSE );
		rect.SetRect( rectOrg.left, rectOrg.bottom - 4, rectOrg.right, rectOrg.bottom );
		fu = (FLOAT)rect.Width()  / texture.m_size.cx;
		fv = (FLOAT)rect.Height() / texture.m_size.cy;
		texture.m_size.cx = rect.Width();
		texture.m_size.cy = rect.Height();
		texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
		texture.m_fuRT = fu  ; texture.m_fvRT = 0.0f;
		texture.m_fuLB = 0.0f; texture.m_fvLB = fv  ;
		texture.m_fuRB = fu  ; texture.m_fvRB = fv  ;

		SetTextureVertex( pVertices, (FLOAT)rect.left,  (FLOAT)rect.top,    texture. m_fuLT + 0.000001f, texture.m_fvLT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.right, (FLOAT)rect.top,    texture. m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.left,  (FLOAT)rect.bottom ,texture. m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f ); pVertices++;
		
		SetTextureVertex( pVertices, (FLOAT)rect.right,  (FLOAT)rect.top,    texture. m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.left,   (FLOAT)rect.bottom, texture. m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.right,  (FLOAT)rect.bottom ,texture. m_fuRB + 0.000001f, texture.m_fvRB + 0.000001f ); pVertices++;
		
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLT + 0.000001f; pVertices->v = texture.m_fvLT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;

		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRB + 0.000001f; pVertices->v = texture.m_fvRB + 0.000001f; pVertices++;

		// vertical
		texture	= m_texWndPaper;
		texture.SetAutoFree( FALSE );
		rect.SetRect( rectOrg.left, rectOrg.top + 16 + 4, rectOrg.left + 4, rectOrg.bottom - 4 );
		fu = (FLOAT)rect.Width()  / texture.m_size.cx;
		fv = (FLOAT)rect.Height() / texture.m_size.cy;
		texture.m_size.cx = rect.Width();
		texture.m_size.cy = rect.Height();
		texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
		texture.m_fuRT = fu  ; texture.m_fvRT = 0.0f;
		texture.m_fuLB = 0.0f; texture.m_fvLB = fv  ;
		texture.m_fuRB = fu  ; texture.m_fvRB = fv  ;

		SetTextureVertex( pVertices, (FLOAT)rect.left,  (FLOAT)rect.top,    texture. m_fuLT + 0.000001f, texture.m_fvLT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.right, (FLOAT)rect.top,    texture. m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.left,  (FLOAT)rect.bottom ,texture. m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f ); pVertices++;
		
		SetTextureVertex( pVertices, (FLOAT)rect.right,  (FLOAT)rect.top,    texture. m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.left,   (FLOAT)rect.bottom, texture. m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.right,  (FLOAT)rect.bottom ,texture. m_fuRB + 0.000001f, texture.m_fvRB + 0.000001f ); pVertices++;
		
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLT + 0.000001f; pVertices->v = texture.m_fvLT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;

		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRB + 0.000001f; pVertices->v = texture.m_fvRB + 0.000001f; pVertices++;

		texture	= m_texWndPaper;
		texture.SetAutoFree( FALSE );
		rect.SetRect( rectOrg.right - 4, rectOrg.top + 16 + 4, rectOrg.right, rectOrg.bottom - 4 );
		fu = (FLOAT)rect.Width()  / texture.m_size.cx;
		fv = (FLOAT)rect.Height() / texture.m_size.cy;
		texture.m_size.cx = rect.Width();
		texture.m_size.cy = rect.Height();
		texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
		texture.m_fuRT = fu  ; texture.m_fvRT = 0.0f;
		texture.m_fuLB = 0.0f; texture.m_fvLB = fv  ;
		texture.m_fuRB = fu  ; texture.m_fvRB = fv  ;


		SetTextureVertex( pVertices, (FLOAT)rect.left,  (FLOAT)rect.top,    texture. m_fuLT + 0.000001f, texture.m_fvLT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.right, (FLOAT)rect.top,    texture. m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.left,  (FLOAT)rect.bottom ,texture. m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f ); pVertices++;
		
		SetTextureVertex( pVertices, (FLOAT)rect.right,  (FLOAT)rect.top,    texture. m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.left,   (FLOAT)rect.bottom, texture. m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.right,  (FLOAT)rect.bottom ,texture. m_fuRB + 0.000001f, texture.m_fvRB + 0.000001f ); pVertices++;

		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLT + 0.000001f; pVertices->v = texture.m_fvLT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;

		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRB + 0.000001f; pVertices->v = texture.m_fvRB + 0.000001f; pVertices++;

		m_pVBTexture->Unlock(); 

		
		//p2DRender->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, 1 );
		//p2DRender->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, 1 );
		//p2DRender->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_NONE );
		//p2DRender->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_NONE );
		p2DRender->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		p2DRender->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		p2DRender->m_pd3dDevice->SetTexture( 0, texture.m_pTexture );
		p2DRender->m_pd3dDevice->SetFVF( D3DFVF_TEXTUREVERTEX );
		p2DRender->m_pd3dDevice->SetStreamSource( 0, m_pVBTexture, 0,sizeof( TEXTUREVERTEX ) );
		p2DRender->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 8);

	//
	}
	///////////////////////////////////////////////
//return;
	// 테두리 박스 
	p2DRender->RenderRoundRect( *pRect, dwColor1 );
	pRect->DeflateRect(1,1);
	p2DRender->RenderRoundRect( *pRect, dwColor3 );
	pRect->DeflateRect(1,1);
	p2DRender->RenderRect( *pRect, 0xffffffff );
	pRect->DeflateRect(1,1);
	p2DRender->RenderRect( *pRect, dwColor3 );


	/*
	p2DRender->RenderRoundRect( *pRect, dwColor1 );
	pRect->DeflateRect(1,1);
	p2DRender->RenderRoundRect( *pRect, dwColor2 );
	pRect->DeflateRect(1,1);
	p2DRender->RenderRoundRect( *pRect, dwColor3 );
	pRect->DeflateRect(1,1);
	p2DRender->RenderRoundRect( *pRect, dwColor4 );
	*/
}
void CTheme::RenderEdge( C2DRender* p2DRender, CRect* pRect, BOOL bClient )
{
	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//

	if( bClient )
		p2DRender->RenderFillRect ( *pRect, dwColor1 );
	p2DRender->RenderRoundRect( *pRect, dwColor2 );
	pRect->DeflateRect( 1 , 1 );
	p2DRender->RenderRect( *pRect, dwColor2 );
	pRect->DeflateRect( 1 , 1 );
	p2DRender->RenderRect( *pRect, dwColor3 );

}
void CTheme::RenderWndBaseBkgr( C2DRender* p2DRender, CRect* pRect  )
{
	if( m_bNudeSkin  == FALSE )
	{
		// 테두리 박스 
		DWORD dwColor1t = D3DCOLOR_ARGB( 155, 100, 100, 100 );//D3DCOLOR_ARGB( 255, 250, 250, 255 );//
		DWORD dwColor1b = D3DCOLOR_ARGB( 155,  70,  70,  70 );//D3DCOLOR_ARGB( 255, 200, 200, 210 );//
		DWORD dwColor2b = D3DCOLOR_ARGB( 155,  43,  73, 45 );//D3DCOLOR_ARGB( 255, 143, 173, 245 );//
		//adationRect( p2DRender, pRect, dwColor1t, dwColor1b, dwColor2b ); 

		CTexture texture
			= m_texWndPaper;
		texture.SetAutoFree( FALSE );

		FLOAT fu = (FLOAT)pRect->Width()  / texture.m_size.cx;
		FLOAT fv = (FLOAT)pRect->Height() / texture.m_size.cy;
		texture.m_size.cx = pRect->Width();
		texture.m_size.cy = pRect->Height();
		texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
		texture.m_fuRT = fu  ; texture.m_fvRT = 0.0f;
		texture.m_fuLB = 0.0f; texture.m_fvLB = fv  ;
		texture.m_fuRB = fu  ; texture.m_fvRB = fv  ;
		p2DRender->RenderTexture( CPoint( 0, 0), &texture );
	//	p2DRender->RenderFillRect( *pRect, 0xa0ffffff );
	}
	else
	{
		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		p2DRender->RenderFillRect( *pRect, D3DCOLOR_ARGB( 100, 0, 0, 0 ) );
	}
}
void CTheme::RenderWndEditFrame( C2DRender* p2DRender, CRect* pRect  )
{
#ifdef __IMPROVE_MAP_SYSTEM
	DWORD dwColor1 = D3DCOLOR_ARGB( 255, 200, 200, 200 );//D3DCOLOR_ARGB(255,200,200,250);//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 255, 255, 255 );//D3DCOLOR_ARGB(255,150,150,200);//

#ifdef __AEGON_THEME_SWITCHER
	map<int, __AEGON_THEME_UNIT>::iterator it = CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.find(g_Option.m_nSelectedThemeID);

	if (it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end())
	{
		if (it->second.dwColorFrameWnd != 0) 
		{
			dwColor1 = it->second.dwColorFrameWnd;
		}

		if (it->second.dwColorWnd != 0)
		{
			dwColor2 = it->second.dwColorWnd;
		}
	}
#endif // __AEGON_THEME_SWITCHER

#else __IMPROVE_MAP_SYSTEM
	DWORD dwColor1 = D3DCOLOR_ARGB( 255, 255, 255, 255 );//D3DCOLOR_ARGB(255,200,200,250);//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 200, 200, 200 );//D3DCOLOR_ARGB(255,150,150,200);//
#endif // __IMPROVE_MAP_SYSTEM

	p2DRender->RenderRoundRect( *pRect, dwColor1 );
	pRect->DeflateRect(1,1);
	p2DRender->RenderFillRect( *pRect, dwColor2 );
}
void CTheme::RenderWndTextFrame( C2DRender* p2DRender, CRect* pRect  )
{
	//DWORD dwColor1 = D3DCOLOR_ARGB(  88,  50,  50,  50 );//D3DCOLOR_ARGB( 255,   0,   0,  50 );//
	DWORD dwColor1 = D3DCOLOR_ARGB( CWndBase::m_nAlpha - 32, 255,255,255 );//D3DCOLOR_ARGB( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( CWndBase::m_nAlpha - 32, 226,198,181 );//D3DCOLOR_ARGB( 255,  80,  80, 120 );//

	p2DRender->RenderFillRect ( *pRect, dwColor1 );
	p2DRender->RenderRoundRect( *pRect, dwColor2 );

}
//////////////////////////

DWORD CTheme::GetButtonFontColor( CWndButton* pWndButton  )
{
	if( pWndButton->IsWindowEnabled() )
	{
		if( pWndButton->IsHighLight() )
			return pWndButton->GetHighLightColor();
		return pWndButton->IsPush() ? pWndButton->GetPushColor() : pWndButton->GetFontColor();
	}
	return pWndButton->GetDisableColor();
}
POINT CTheme::GetButtonTextPos( C2DRender* p2DRender, CWndButton* pWndButton  )
{
	if( ! ( pWndButton->GetStyle() & WBS_NOCENTER ) )
		return pWndButton->GetStrCenter( p2DRender, pWndButton->GetTitle() );
	return CPoint( 0, 0);
}
void CTheme::RenderWndButton( C2DRender* p2DRender, CWndButton* pWndButton  )
{
	int   nCheck      = pWndButton->GetCheck();
	BOOL  bHighLight  = pWndButton->IsHighLight();
	BOOL  bEnable     = pWndButton->IsWindowEnabled();
	BOOL  bPush       = pWndButton->IsPush();
	DWORD dwColor     = GetButtonFontColor( pWndButton );
	POINT pt          = GetButtonTextPos( p2DRender, pWndButton );

//#ifdef __NEWINTERFACE
	if( pWndButton->m_pTexture )
	{
		RenderWndButton_4Texture( p2DRender, pWndButton );
#ifdef __BUTTONS_V19
		RenderWndButtonText(p2DRender, pWndButton);
#endif// __BUTTONS_V19
		return;
	}
	if( nCheck )
	{
		dwColor = D3DCOLOR_ARGB( 0, 0, 0, 0);

	}
	//DWORD dwColor1 = ( bHighLight ?	D3DCOLOR_ARGB( 255, 200, 200, 250 ) : D3DCOLOR_ARGB( 255, 150, 150, 250 ) );
	//DWORD dwColor2 = ( bHighLight ?	D3DCOLOR_ARGB( 255, 130, 130, 200 ) : D3DCOLOR_ARGB( 255,  30,  30, 100 ) );
	//DWORD dwColor1 = ( bHighLight ?	D3DCOLOR_ARGB( 100, 100, 100, 150 ) : D3DCOLOR_ARGB( 200,  200,  200, 200 ) );
	//DWORD dwColor2 = ( bHighLight ?	D3DCOLOR_ARGB(  55,  30,  30, 100 ) : D3DCOLOR_ARGB(  55,   0,   0,  00 ) );


	//DWORD dwColor1 = D3DCOLOR_ARGB( 250, 255, 255, 255 );//D3DCOLOR_ARGB( 255, 130, 130, 230 );//
	//DWORD dwColor2 = D3DCOLOR_ARGB( 50,    0,   0,  00 );//D3DCOLOR_ARGB( 255,  50,  50, 100 );//
	//DWORD dwColor3 = D3DCOLOR_ARGB( 200, 150, 150, 150 );//D3DCOLOR_ARGB( 255, 180, 180, 220 );//

	DWORD dwColor1 = ( bPush ?	D3DCOLOR_ARGB( 255,   100, 255, 255 ) : D3DCOLOR_ARGB( 250, 255, 255, 255 ) );
	DWORD dwColor2 = ( bPush ?	D3DCOLOR_ARGB(  50,     0,   0,   0 ) : D3DCOLOR_ARGB(  50,   0,   0,   0 ) );
	DWORD dwColor3 = ( bPush ?	D3DCOLOR_ARGB( 200,     0, 150, 150 ) : D3DCOLOR_ARGB( 200, 150, 150, 150 ) );
//	DWORD dwColor  = ( bHighLight ? D3DCOLOR_ARGB( 250, 0, 255, 255 ) : D3DCOLOR_ARGB( 250, 255, 255, 255 ) );

	//p2DRender->TextOut( 17, 7, lpszTitle, 0xff000000);
	//p2DRender->TextOut( 16, 6, lpszTitle, 0xffffffff);

	p2DRender->TextOut( pt.x+1, pt.y+1, pWndButton->GetTitle(), 0xff000000 );
	p2DRender->TextOut( pt.x, pt.y, pWndButton->GetTitle(), dwColor );

//	if( pWndButton->IsWndStyle( WBS_NODRAWFRAME ) )
//		return;

	CRect rect = pWndButton->GetClientRect();
	rect.DeflateRect( 2, 2);
	GradationRect( p2DRender, &rect, dwColor1 ,dwColor2, dwColor3 );
	p2DRender->RenderLine( CPoint( rect.left, rect.top ), CPoint( rect.right, rect.top ), dwColor3 );

	//CRect rect = pWndButton->GetClientRect();
	rect.InflateRect(1,1);
	p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB(  155, 200, 200, 200 ) );
	rect.InflateRect(1,1);
	p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB(  155,  50,  50,  50 ) );
/*
	if( bPush )
		p2DRender->RenderFillRect( rect, dwColor2, dwColor2, dwColor1, dwColor1 );
	else
		p2DRender->RenderFillRect( rect, dwColor1, dwColor1, dwColor2, dwColor2 );
*/
}
void CTheme::RenderWndButton_4Texture( C2DRender* p2DRender, CWndButton* pWndButton  )
{
	int   nCheck      = pWndButton->GetCheck();
	BOOL  bHighLight  = pWndButton->IsHighLight();
	BOOL  bEnable     = pWndButton->IsWindowEnabled();
	BOOL  bPush       = pWndButton->IsPush();
	DWORD dwColor     = GetButtonFontColor( pWndButton );
//	POINT pt          = GetButtonTextPos( p2DRender, pWndButton );

	if( pWndButton->m_pTexture == FALSE )
		return;
	
	CTexture* pTexture = pWndButton->m_pTexture;
	CSize sizeOld = pTexture->m_size;
	pTexture->m_size.cx /= 4;
	
	if( bEnable == FALSE )
	{
		pTexture->m_fuLT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 3.0f;
		pTexture->m_fvLT = 0.0f;

		pTexture->m_fuRT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 4.0f;
		pTexture->m_fvRT = 0.0f;

		pTexture->m_fuLB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 3.0f;
		pTexture->m_fvLB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;

		pTexture->m_fuRB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 4.0f;
		pTexture->m_fvRB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;
	}
	else
	if( bPush == FALSE && nCheck == 0)
	{
		if( bHighLight )
		{
			pTexture->m_fuLT = 0.0f;
			pTexture->m_fvLT = 0.0f;
			
			pTexture->m_fuRT = (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx;
			pTexture->m_fvRT = 0.0f;
			
			pTexture->m_fuLB = 0.0f;
			pTexture->m_fvLB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;
			
			pTexture->m_fuRB = (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx;
			pTexture->m_fvRB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;
		}
		else
		{
			pTexture->m_fuLT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 1.0f;
			pTexture->m_fvLT = 0.0f;
			
			pTexture->m_fuRT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 2.0f;
			pTexture->m_fvRT = 0.0f;
			
			pTexture->m_fuLB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 1.0f;
			pTexture->m_fvLB = ( (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy );
			
			pTexture->m_fuRB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 2.0f;
			pTexture->m_fvRB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;
		}
	}
	else
	{

		pTexture->m_fuLT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 2.0f;
		pTexture->m_fvLT = 0.0f;

		pTexture->m_fuRT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 3.0f;
		pTexture->m_fvRT = 0.0f;

		pTexture->m_fuLB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 2.0f;
		pTexture->m_fvLB = ( (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy );

		pTexture->m_fuRB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 3.0f;
		pTexture->m_fvRB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;

	}
	pTexture->Render( p2DRender, CPoint( 0, 0 ), pWndButton->m_nAlphaCount );
	pTexture->m_size = sizeOld;
}

void CTheme::RenderWndButton_6Texture( C2DRender* p2DRender, CWndButton* pWndButton  )
{
	int   nCheck      = pWndButton->GetCheck();
	BOOL  bHighLight  = pWndButton->IsHighLight();
	BOOL  bEnable     = pWndButton->IsWindowEnabled();
	BOOL  bPush       = pWndButton->IsPush();
	DWORD dwColor     = GetButtonFontColor( pWndButton );
//	POINT pt          = GetButtonTextPos( p2DRender, pWndButton );

	if( pWndButton->m_pTexture )
	{
		CTexture* pTexture = pWndButton->m_pTexture;
		CSize sizeOld = pTexture->m_size;
		pTexture->m_size.cx /= 6;
		
		if( bEnable == FALSE )
		{
			if( bPush == FALSE && nCheck == 0 ) 
			{
				pTexture->m_fuLT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 4.0f;
				pTexture->m_fvLT = 0.0f;

				pTexture->m_fuRT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 5.0f;
				pTexture->m_fvRT = 0.0f;

				pTexture->m_fuLB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 4.0f;
				pTexture->m_fvLB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;

				pTexture->m_fuRB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 5.0f;
				pTexture->m_fvRB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;
			}
			else
			{
				pTexture->m_fuLT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 5.0f;
				pTexture->m_fvLT = 0.0f;

				pTexture->m_fuRT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 6.0f;
				pTexture->m_fvRT = 0.0f;

				pTexture->m_fuLB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 5.0f;
				pTexture->m_fvLB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;

				pTexture->m_fuRB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 6.0f;
				pTexture->m_fvRB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;
			}
		}
		else
		if( bPush == FALSE && nCheck == 0 )
		{
			if( bHighLight )
			{
				pTexture->m_fuLT = 0.0f;
				pTexture->m_fvLT = 0.0f;
				
				pTexture->m_fuRT = (FLOAT) pTexture->m_size.cx / (FLOAT)pTexture->m_sizePitch.cx;
				pTexture->m_fvRT = 0.0f;
				
				pTexture->m_fuLB = 0.0f;
				pTexture->m_fvLB = (FLOAT) pTexture->m_size.cy / (FLOAT)pTexture->m_sizePitch.cy;
				
				pTexture->m_fuRB = (FLOAT) pTexture->m_size.cx / (FLOAT)pTexture->m_sizePitch.cx;
				pTexture->m_fvRB = (FLOAT) pTexture->m_size.cy / (FLOAT)pTexture->m_sizePitch.cy;
			}
			else
			{
				pTexture->m_fuLT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 1.0f;
				pTexture->m_fvLT = 0.0f;
				
				pTexture->m_fuRT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 2.0f;
				pTexture->m_fvRT = 0.0f;
				
				pTexture->m_fuLB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 1.0f;
				pTexture->m_fvLB = ( (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy );
				
				pTexture->m_fuRB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 2.0f;
				pTexture->m_fvRB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;
			}
		}
		else
		{
			if( bHighLight )
			{
				pTexture->m_fuLT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 2.0f;
				pTexture->m_fvLT = 0.0f;

				pTexture->m_fuRT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 3.0f;
				pTexture->m_fvRT = 0.0f;

				pTexture->m_fuLB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 2.0f;
				pTexture->m_fvLB = ( (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy );

				pTexture->m_fuRB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 3.0f;
				pTexture->m_fvRB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;
			}
			else
			{
				pTexture->m_fuLT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 3.0f;
				pTexture->m_fvLT = 0.0f;

				pTexture->m_fuRT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 4.0f;
				pTexture->m_fvRT = 0.0f;

				pTexture->m_fuLB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 3.0f;
				pTexture->m_fvLB = ( (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy );

				pTexture->m_fuRB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 4.0f;
				pTexture->m_fvRB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;
			}
		}
		pTexture->Render( p2DRender, CPoint( 0, 0 ), pWndButton->m_nAlphaCount );
		pTexture->m_size = sizeOld;
	}

}

void CTheme::RenderWndButtonCheck( C2DRender* p2DRender, CWndButton* pWndButton  )
{
	int nFontHeight = pWndButton->GetFontHeight();
	DWORD dwColor = GetButtonFontColor( pWndButton );
	if( pWndButton->m_pTexture ) // pWndButton->IsWndStyle( WBS_PUSHLIKE ) )
	{
		RenderWndButton_6Texture( p2DRender, pWndButton );
		p2DRender->TextOut( pWndButton->m_pTexture->m_size.cx / 6 + 2, p2DRender->m_clipRect.Height() / 2 - nFontHeight / 2, pWndButton->GetTitle(), dwColor );
	}
	else
	{
		BOOL  bHighLight  = pWndButton->IsHighLight();
		BOOL  bEnable     = pWndButton->IsWindowEnabled();
		BOOL  bPush       = pWndButton->IsPush();
//		POINT pt          = GetButtonTextPos( p2DRender, pWndButton );

		int nFontColor = p2DRender->GetTextColor();

		p2DRender->RenderRect( CRect( 0, 0, 10, 10 ), dwColor );
		if( pWndButton->GetCheck() ) 
		{
			p2DRender->RenderLine( CPoint( 2,  2), CPoint( 5, 8), dwColor );
			p2DRender->RenderLine( CPoint( 2,  2), CPoint( 6, 8), dwColor );
			p2DRender->RenderLine( CPoint( 5,  8), CPoint( 8, 3), dwColor );
		}
		p2DRender->TextOut( nFontHeight + 5, 0, pWndButton->GetTitle(), dwColor );
	}
}
void CTheme::RenderWndButtonRadio( C2DRender* p2DRender, CWndButton* pWndButton  )
{
	BOOL  bHighLight  = pWndButton->IsHighLight();
	BOOL  bEnable     = pWndButton->IsWindowEnabled();
	BOOL  bPush       = pWndButton->IsPush();
	DWORD dwColor     = GetButtonFontColor( pWndButton );
//	POINT pt          = GetButtonTextPos( p2DRender, pWndButton );

	int nFontHeight = pWndButton->GetFontHeight();
	int nFontColor = p2DRender->GetTextColor();
	if( pWndButton->m_pTexture ) // pWndButton->IsWndStyle( WBS_PUSHLIKE ) )
	{
		RenderWndButton_6Texture( p2DRender, pWndButton );
		p2DRender->TextOut( pWndButton->m_pTexture->m_size.cx / 6 + 2, p2DRender->m_clipRect.Height() / 2 - nFontHeight / 2, pWndButton->GetTitle(), dwColor );
	}
	else
	{
		CRect rect( 0, 0, 10, 10 );
		
		p2DRender->RenderRoundRect( rect, dwColor );
		if( pWndButton->GetCheck() ) 
		{
			rect.DeflateRect( 2, 2, 2, 2 );
			p2DRender->RenderFillRect( rect, dwColor );
		}
		p2DRender->TextOut( nFontHeight + 5, 0, pWndButton->GetTitle(), dwColor );
	}
		/*
		if(m_bCheck && m_bEnable == TRUE)
			;//p2DRender->PaintImage(lpRadioBitmap2,CPtSz(1,1,14,14),0);
		else
			;//p2DRender->PaintImage(lpRadioBitmap1,CPtSz(1,1,14,14),0);
		p2DRender->TextOut( nFontHeight, 0, m_strTitle );
		*/

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTheme::RenderWndButtonText( C2DRender* p2DRender, CWndButton* pWndButton  )
{
	BOOL  bHighLight  = pWndButton->IsHighLight();
	BOOL  bEnable     = pWndButton->IsWindowEnabled();
	BOOL  bPush       = pWndButton->IsPush();
	DWORD dwColor     = GetButtonFontColor( pWndButton );
	POINT pt;

#ifdef __BUTTONS_V19
	CD3DFont* pFont = p2DRender->GetFont();
	p2DRender->SetFont(CWndBase::m_Theme.m_pFontButton);
	pt = GetButtonTextPos(p2DRender, pWndButton);
	pt.x -= 1;

	if (pt.y < 0)
		pt.y = 0;

	if (pWndButton->IsPush())
	{
		p2DRender->TextOut(pt.x, pt.y + 1, pWndButton->GetTitle(), 0xffffffff);
		p2DRender->TextOut(pt.x, pt.y + 1, pWndButton->GetTitle(), dwColor);
	}
	else if (pWndButton->IsHighLight())
	{
		p2DRender->SetFont(CWndBase::m_Theme.m_pFontButtonPushed);
		pt = GetButtonTextPos(p2DRender, pWndButton);
		if (pt.y < 0)
			pt.y = 0;
		pt.x -= 1;
		p2DRender->TextOut(pt.x, pt.y, pWndButton->GetTitle(), 0xffffffff);
		p2DRender->TextOut(pt.x , pt.y, pWndButton->GetTitle(), dwColor);
	}
	else if (pWndButton->IsWindowEnabled() == FALSE)
	{
		p2DRender->TextOut(pt.x , pt.y, pWndButton->GetTitle(), dwColor);
	}
	else
	{
		p2DRender->TextOut(pt.x, pt.y, pWndButton->GetTitle(), 0xffffffff);
		p2DRender->TextOut(pt.x, pt.y, pWndButton->GetTitle(), dwColor);
	}
	p2DRender->SetFont(pFont);
#else //__BUTTONS_V19
	p2DRender->TextOut( pt.x, pt.y, pWndButton->GetTitle(), dwColor);
#endif// __BUTTONS_V19
}

void CTheme::RenderWndTaskBar( C2DRender* p2DRender, CRect* pRect )
{
	CRect rect = *pRect;

	CTexture texture
		= m_texWndPaper;
	texture.SetAutoFree( FALSE );

	FLOAT fu = (FLOAT)pRect->Width()  / texture.m_size.cx;
	FLOAT fv = (FLOAT)pRect->Height() / texture.m_size.cy;
	texture.m_size.cx = pRect->Width();
	texture.m_size.cy = pRect->Height();
	texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
	texture.m_fuRT = fu  ; texture.m_fvRT = 0.0f;
	texture.m_fuLB = 0.0f; texture.m_fvLB = fv  ;
	texture.m_fuRB = fu  ; texture.m_fvRB = fv  ;
	p2DRender->RenderTexture( CPoint( 0, 0), &texture );

	// 테두리 박스 
	/*
	DWORD dwColor1t = D3DCOLOR_ARGB( 255,  91, 104, 205 );
	DWORD dwColor2t = D3DCOLOR_ARGB( 255, 116, 128, 220 );
	DWORD dwColor3t = D3DCOLOR_ARGB( 255, 143, 173, 245 );
	DWORD dwColor1b = D3DCOLOR_ARGB( 255,  41, 104, 155 );
	DWORD dwColor2b = D3DCOLOR_ARGB( 255,  66,  78, 170 );
	DWORD dwColor3b = D3DCOLOR_ARGB( 255,   3,  33, 105 );
*/
	DWORD dwColor0 = D3DCOLOR_ARGB( 150, 100, 100, 100 );
	DWORD dwColor1 = D3DCOLOR_ARGB( 250, 255, 255, 255 );//D3DCOLOR_ARGB( 255, 130, 130, 230 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 50,    0,   0,  00 );//D3DCOLOR_ARGB( 255,  50,  50, 100 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 200, 150, 150, 150 );//D3DCOLOR_ARGB( 255, 180, 180, 220 );//

	//DWORD dwColor1 = D3DCOLOR_ARGB( 100,   0,   0,   0 );
	//DWORD dwColor2 = D3DCOLOR_ARGB( 150, 255, 255, 255 );
	//DWORD dwColor3 = D3DCOLOR_ARGB( 100,  00,  00,  00 );

	GradationRect( p2DRender, &rect, dwColor1 ,dwColor2, dwColor3, 40 );
	//p2DRender->RenderLine( CPoint( rect.left, rect.top ), CPoint( rect.right, rect.top ), dwColor3);
	
	rect = *pRect;//GetWindowRect();
	rect.DeflateRect( 3, 3);
	rect.left = 65;
	//rect.right = 240;
	rect.right -= 90;
	//p2DRender->RenderRoundRect( rect, dwColor0 );
	rect.DeflateRect( 1, 1);

//	rect = *pRect;//GetWindowRect();
	for( int i = 0; i < rect.Width() / 32; i++ )
		;//p2DRender->RenderTriangle( CPoint( rect.left+ 16 + i * 32, 3 ), CPoint( rect.left + 3 + i * 32, 32 - 3 ), CPoint( rect.left + 32 - 3 + i * 32, 32 - 3 ), D3DCOLOR_ARGB( 100, 200, 170, 170 ) );

	//GradationRect( p2DRender, &rect, dwColor3 ,dwColor2, dwColor1, 80 );
}
void CTheme::RenderWndMenuTask( C2DRender* p2DRender, CRect* pRect )
{
}
void CTheme::RenderWndMenu( C2DRender* p2DRender, CRect* pRect )
{
	RenderWndBaseFrame( p2DRender, pRect );
	/*
	CRect rect = *pRect;
	// 테두리 박스 
	p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB( 155, 100, 100, 200 ) );
	rect.DeflateRect(1,1);
	p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB( 155, 180, 180, 220 ) );
	rect.DeflateRect(1,1);
	p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB( 155, 180, 180, 220 ) );
	rect.DeflateRect(1,1);
	p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB( 155, 50, 50, 150 ) );
	*/
}
void CTheme::RenderWndMenuItem( C2DRender* p2DRender, CWndButton* pWndButton )
{
}
BOOL CTheme::MakeGaugeVertex( LPDIRECT3DDEVICE9 pd3dDevice, CRect* pRect, DWORD dwColor, LPDIRECT3DVERTEXBUFFER9 pVB, CTexture* pTexture )
{
	CPoint pt = pRect->TopLeft();
	CPoint ptCenter = pTexture->m_ptCenter;
	pt -= ptCenter;

	///////////////////////////////////////////////////////////////
	int nTexWidth = ( pTexture->m_size.cx ) / 3;
	int nTexHeight = pTexture->m_size.cy;

	FLOAT left   = (FLOAT)( pt.x );
	FLOAT top    = (FLOAT)( pt.y );
	FLOAT right  = (FLOAT)( pt.x + nTexWidth );//( pTexture->m_size.cx );
	FLOAT bottom = (FLOAT)( pt.y + nTexHeight );//( pTexture->m_size.cy );

	int nWidth = ( pRect->Width() / nTexWidth );// - 2;

	// 기본 패턴으로 완성될 수 있는건 2이다. 2보다 작으면 이미지가 깨질 수 있으니 리턴.
	if( nWidth < 2 )
		return FALSE;
	
	int nTileNum = 3;
	int nVertexNum = 3 * 6;
	
	TEXTUREVERTEX2* pVertices,* pVertices_;
	HRESULT hr = pVB->Lock( 0, sizeof(TEXTUREVERTEX2) * nVertexNum, (void**) &pVertices_, D3DLOCK_DISCARD );
	if(hr != D3D_OK) return FALSE;
	{
		SIZE size = pTexture->m_size;//
		SIZE sizePitch = pTexture->m_sizePitch;//
		size.cx /= 3;
		
		pVertices = pVertices_;
		for( int i = 0; i < nVertexNum; i++ )
		{
			pVertices->vec.z = 0;
			pVertices->rhw = 1.0f;
			pVertices->color = dwColor;
			pVertices++;
		}
		pVertices = pVertices_;

		left   = (FLOAT)( pt.x );
		right  = (FLOAT)( pt.x + nTexWidth );
			
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;

		//////////////////////////////////
		
		left   = (FLOAT)( pt.x + nTexWidth );
		right  = (FLOAT)( pt.x + nTexWidth + ( ( nWidth - 2 ) * nTexWidth ) );
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 1.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 1.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 1.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;

		//////////////////////////////////
		left   = (FLOAT)( pt.x + ( ( nWidth - 1 ) * nTexWidth ) );
		right  = (FLOAT)( pt.x + ( ( nWidth ) * nTexWidth ) );

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = 0.0f;//(FLOAT)size.cy / sizePitch.cy;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 3.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 3.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 3.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;

	}
	pVB->Unlock(); 
	return TRUE;
}				
void CTheme::RenderGauge( LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECT3DVERTEXBUFFER9 pVB, CTexture* pTexture )
{
	///////////////////////////////////////////////////////////////

	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, 1 );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, 1 );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		

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

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR  );

	//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
	//pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
	//pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
	//pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
	//pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
	//pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
	//pd3dDevice->SetRenderState( D3DRS_CLIPPING,         TRUE );
	//pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
	//pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND,      D3DVBF_DISABLE );
	//pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
	//pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
	//pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE,
	//	D3DCOLORWRITEENABLE_RED  | D3DCOLORWRITEENABLE_GREEN |
	//	D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
	
	int nVertexNum = 3 * 6;
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetTexture( 0, pTexture->m_pTexture );
	pd3dDevice->SetFVF( D3DFVF_TEXTUREVERTEX2 );
	pd3dDevice->SetStreamSource( 0, pVB, 0,sizeof( TEXTUREVERTEX2 ) );
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, nVertexNum / 3 );
}				

void CTheme::RenderGauge( C2DRender* p2DRender, CRect* pRect, DWORD dwColor, LPDIRECT3DVERTEXBUFFER9 pVB, CTexture* pTexture )
{
//	pTexture = &m_texGauEmptyNormal;
    LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice; 
	CPoint pt = pRect->TopLeft();
	pt += p2DRender->m_ptOrigin;
	CPoint ptCenter = pTexture->m_ptCenter;
	pt -= ptCenter;

	///////////////////////////////////////////////////////////////
	int nTexWidth = ( pTexture->m_size.cx ) / 3;
	int nTexHeight = pTexture->m_size.cy;

	FLOAT left   = (FLOAT)( pt.x );
	FLOAT top    = (FLOAT)( pt.y );
	FLOAT right  = (FLOAT)( pt.x + nTexWidth );//( pTexture->m_size.cx );
	FLOAT bottom = (FLOAT)( pt.y + nTexHeight );//( pTexture->m_size.cy );

	int nWidth = ( pRect->Width() / nTexWidth );// - 2;

	// 기본 패턴으로 완성될 수 있는건 2이다. 2보다 작으면 이미지가 깨질 수 있으니 리턴.
	if( nWidth < 2 )
		return;
	
	int nTileNum = 3;
	int nVertexNum = 3 * 6;
	
	TEXTUREVERTEX2* pVertices,* pVertices_;
	HRESULT hr = pVB->Lock( 0, sizeof(TEXTUREVERTEX2) * nVertexNum, (void**) &pVertices_, D3DLOCK_DISCARD );
	if(hr != D3D_OK) return;
	{
		SIZE size = pTexture->m_size;//
		SIZE sizePitch = pTexture->m_sizePitch;//
		size.cx /= 3;
		
		pVertices = pVertices_;
		for( int i = 0; i < nVertexNum; i++ )
		{
			pVertices->vec.z = 0;
			pVertices->rhw = 1.0f;
			pVertices->color = dwColor;
			pVertices++;
		}
		pVertices = pVertices_;

		left   = (FLOAT)( pt.x );
		right  = (FLOAT)( pt.x + nTexWidth );
			
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;

		//////////////////////////////////
		
		left   = (FLOAT)( pt.x + nTexWidth );
		right  = (FLOAT)( pt.x + nTexWidth + ( ( nWidth - 2 ) * nTexWidth ) );
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 1.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 1.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 1.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;

		//////////////////////////////////
		left   = (FLOAT)( pt.x + ( ( nWidth - 1 ) * nTexWidth ) );
		right  = (FLOAT)( pt.x + ( ( nWidth ) * nTexWidth ) );

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = 0.0f;//(FLOAT)size.cy / sizePitch.cy;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 3.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 3.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 3.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;

	}
	pVB->Unlock(); 

	///////////////////////////////////////////////////////////////

	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, 1 );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, 1 );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		

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

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR  );

	//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
	//pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
	//pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
	//pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
	//pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
	//pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
	//pd3dDevice->SetRenderState( D3DRS_CLIPPING,         TRUE );
	//pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
	//pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND,      D3DVBF_DISABLE );
	//pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
	//pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
	//pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE,
	//	D3DCOLORWRITEENABLE_RED  | D3DCOLORWRITEENABLE_GREEN |
	//	D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
	
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetTexture( 0, pTexture->m_pTexture );
	pd3dDevice->SetFVF( D3DFVF_TEXTUREVERTEX2 );
	pd3dDevice->SetStreamSource( 0, pVB, 0,sizeof( TEXTUREVERTEX2 ) );
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, nVertexNum / 3 );
}				

void CTheme::SetVersion(int Lang)
{

}
