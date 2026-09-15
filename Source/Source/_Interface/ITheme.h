// Theme.h: interface for the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THEME) //_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)
#define AFX_THEME //_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWndButton;

// WallPaper Type

#define WPT_STRETCH        1
#define WPT_CENTER         2
#define WPT_CENTERSTRETCH  3
#define WPT_TILE           4

#include "AegonThemeSwitcherMng.h"

class CWndBase;

class CTheme
{
	LPDIRECT3DDEVICE9 m_pd3dDevice; 
#ifdef __AEGON_FONT_SWITCH
public:
#endif // __AEGON_FONT_SWITCH
	CMapStringToPtr m_mapFont;

#ifdef __FLYFF_INITPAGE_EXT
public:
	CWorld* m_pTitleWorld;
	BOOL m_bLoadTerrainScript;
	BOOL m_bRenderTitleWorld;

	CTexture* m_pTitaniumLogo;

	DWORD m_dwTexturAlpha1;
	DWORD m_dwTexturAlpha2;

	DWORD m_dwStartTime;
	DWORD m_dwEndTime;
	BOOL m_bStartCameraWork;
#endif //__FLYFF_INITPAGE_EXT

public:
	void SetVersion( int Lang );
	//D3DSURFACE_DESC* m_pd3dsdBackBuffer;   // 
	//CWndBase* m_pWndMng;
	CWndBase* m_pActiveDesktop;

	CD3DFont* m_pFontText;
//	CD3DFont* m_pFontTextMin;
	CD3DFont* m_pFontGameTitle;
	CD3DFont* m_pFontWndTitle;

	CD3DFont* m_pFontChat;
	CD3DFont* m_pFontCaption;
#ifdef __ZONE_AFFICHAGE
	CD3DFont* m_pFontZoneText;
	CD3DFont* m_pFontZoneTextSub;
#endif //__ZONE_AFFICHAGE

	CD3DFont* m_pFontWorld;
	CD3DFont* m_pFontStatus;
	CD3DFont* m_pFontGuildCombatText;
#if __VER >= 12 // __SECRET_ROOM
	CD3DFont* m_pFontSRMyGiuld;
	CD3DFont* m_pFontSRGiuld;
#endif //__SECRET_ROOM

#ifdef __AEGON_TOURNAMENT
	CD3DFont* m_pFontAegonTournamentBracket;
	CD3DFont* m_pFontAegonTournamentTop10;
#endif // __AEGON_TOURNAMENT

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
	CD3DFont* m_pFontAegonAchievementNotice;
#endif // __AEGON_ACHIEVEMENT_SYSTEM

#ifdef __AEGON_ADVANCED_ANNOUNCEMENT
	CD3DFont* m_pFontAegonCaption1;
#endif // __AEGON_ADVANCED_ANNOUNCEMENT

#ifdef __AEGON_SEASON_PASS
	CD3DFont* m_pFontSeasonPassNumField;
	CD3DFont* m_pFontSeasonPassCountdown;
	CD3DFont* m_pFontSeasonPassDescrHeader;
	CD3DFont* m_pFontSeasonPassDescrLevel;
	CD3DFont* m_pFontSeasonPassDescr;
	CD3DFont* m_pFontSeasonPassProgLevel;
	CD3DFont* m_pFontSeasonPassProg;
#endif // __AEGON_SEASON_PASS

#ifdef __BUTTONS_V19
	CD3DFont* m_pFontButton;
	CD3DFont* m_pFontButtonPushed;
#endif //__BUTTONS_V19

#ifdef __FL_FARM_STAT
	CD3DFont* m_pFontFarmStat;
#endif //__FL_FARM_STAT

#ifdef __LEESE_RENDER_ITEMCOUNT
	CD3DFont* m_pFontItemCount;
#endif //__LEESE_RENDER_ITEMCOUNT

#ifdef __AEGON_MESSENGER
	CD3DFont* m_pFontMessengerNotification;
	CD3DFont* m_pFontMessengerTitle;
#endif

	//CD3DFontAPI* m_pFontAPICaption;
	//CD3DFontAPI* m_pFontAPITitle;

	LPDIRECT3DVERTEXBUFFER9 m_pVBTexture;
	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;

	// Background Color
	D3DCOLOR m_d3dcBackground     ;

	// Window
	D3DCOLOR m_d3dcWndBkgr        ;
	D3DCOLOR m_d3dcWndBolder      ;
	D3DCOLOR m_d3dcWndFrame       ;
	D3DCOLOR m_d3dcWndTitle       ;
	D3DCOLOR m_d3dcWndDeAct       ;

	// TextBox
	D3DCOLOR m_d3dcTextBkgr       ;
	D3DCOLOR m_d3dcTextBolder     ;
	D3DCOLOR m_d3dcTextFrame      ;

	// Button
	D3DCOLOR m_d3dcButtonBkgr     ;
	D3DCOLOR m_d3dcButtonBolder   ;
	D3DCOLOR m_d3dcButtonFrame    ; 
	D3DCOLOR m_d3dcFont           ;
	D3DCOLOR m_d3dcHighLightFont  ;
	D3DCOLOR m_d3dcPushFont       ;
	D3DCOLOR m_d3dcHighLightBar   ;

	// MenuItem
	D3DCOLOR m_d3dcMenuItemBkgr   ;
	D3DCOLOR m_d3dcMenuItemBolder ;
	D3DCOLOR m_d3dcMenuItemFrame  ;

	// Chat or Message
	D3DCOLOR m_d3dcChatMyFont     ;
	D3DCOLOR m_d3dcMsgFont        ;
	D3DCOLOR m_d3dcMsgFontWarning ; 
	D3DCOLOR m_d3dcMsgFontEffect  ;
	D3DCOLOR m_d3dcMsgFontNotice  ;

	// ScrollBar
	D3DCOLOR m_d3dcSBarPad;
	D3DCOLOR m_d3dcSBarStick;

	// WallPaper
	CTexture  m_texWallPaper;
	CTexture  m_texWndPaper;
	DWORD     m_dwWallPaperType;

	BOOL m_bNudeSkin;

	CTheme();
	~CTheme();

#ifdef __AEGON_THEME_SWITCHER
	void OnThemeChanged();
	void SwitchButtonFont();
	void SwitchThemeFonts();
#endif

#ifdef __AEGON_CUSTOM_FONTS
	vector<CString> m_CustomFontMap;
	void LoadSingleFont(CString strFont);
	BOOL LoadCustomFonts();
	BOOL RemoveCustomFonts();
	void AegonOnSwitchFont();

	CD3DFont* SwitchSingleFont(__AEGON_THEME_FONT* pFontUnit);
	CD3DFont* SwitchSingleFont(LPCSTR szFont, DWORD nHeight, DWORD nOutline = 0, DWORD dwColor = COLOR_WHITE, DWORD dwBgColor = COLOR_BLACK, DWORD dwFlags = 0, DWORD dwFontFlags = 0);
#endif // __AEGON_CUSTOM_FONTS

	BOOL LoadTheme( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR lpszFileName );
	BOOL SaveTheme( LPCTSTR lpszFileName );

	void DeleteTheme();
	void RenderTitle( C2DRender* p2DRender );
	void RenderDesktop( C2DRender* p2DRender );

	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT FrameMove();
	HRESULT RestoreDeviceObjects();

	// GUI 랜더에 필요한 함수들 
	void  GradationRect( C2DRender* p2DRender, CRect* pRect, DWORD dwColor1t, DWORD dwColor1b, DWORD dwColor2b, int nMidPercent = 40 );
	DWORD GetButtonFontColor( CWndButton* pWndButton );
	POINT GetButtonTextPos( C2DRender* p2DRender, CWndButton* pWndButton );

	// 윈도 윈터페이스 랜더 시작 
	void RenderEdge              ( C2DRender* p2DRender, CRect* pRect, BOOL bClient = TRUE );
#ifdef __AEGON_THEME_SWITCHER
	void RenderWndBaseTitleBar(C2DRender* p2DRender, CRect* pRect, LPCTSTR lpszTitle, DWORD dwColor = 0xff000000, int nTitleDrawOffsetY = 0);
#else
	void RenderWndBaseTitleBar   ( C2DRender* p2DRender, CRect* pRect, LPCTSTR lpszTitle, DWORD dwColor = 0xff000000 );
#endif
	void RenderWndBaseFrame      ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndBaseBkgr       ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndTextFrame      ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndTextBkgr       ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndEditFrame      ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndEditBkgr       ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndTreeFrame      ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndTreeBkgr       ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndListBoxFrame   ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndListBoxBkgr    ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndComboBoxFrame  ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndComboBoxBkgr   ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndComboBoxButton ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndButton         ( C2DRender* p2DRender, CWndButton* pWndButton );
	void RenderWndButtonRadio    ( C2DRender* p2DRender, CWndButton* pWndButton );
	void RenderWndButtonCheck    ( C2DRender* p2DRender, CWndButton* pWndButton );
	void RenderWndButtonText     ( C2DRender* p2DRender, CWndButton* pWndButton );

	void RenderWndButton_4Texture( C2DRender* p2DRender, CWndButton* pWndButton );
	void RenderWndButton_6Texture( C2DRender* p2DRender, CWndButton* pWndButton );

	void RenderWndTaskBar        ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndMenuTask       ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndMenu           ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndMenuItem       ( C2DRender* p2DRender, CWndButton* pWndButton );

	void RenderWndGauge          ( C2DRender* p2DRender, CRect* pRect, DWORD dwColor );
	void RenderWndGauge2         ( C2DRender* p2DRender, CRect* pRect, DWORD dwColor );

	BOOL MakeGaugeVertex         ( LPDIRECT3DDEVICE9 pd3dDevice, CRect* pRect, DWORD dwColor, LPDIRECT3DVERTEXBUFFER9 pVB, CTexture* pTexture );
	void RenderGauge             ( LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECT3DVERTEXBUFFER9 pVB, CTexture* pTexture );
	void RenderGauge             ( C2DRender* p2DRender, CRect* pRect, DWORD dwColor, LPDIRECT3DVERTEXBUFFER9 pVBGauge, CTexture* pTexture );

#ifdef __FLYFF_INITPAGE_EXT
	void ReadTitleWorld();
	void DestoryTitleWorld();
#endif //__FLYFF_INITPAGE_EXT
};
#endif // !defined(AFX_THEME_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)

