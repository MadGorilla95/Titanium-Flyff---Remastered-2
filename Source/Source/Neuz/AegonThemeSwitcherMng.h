#pragma once
#ifndef __AEGTHESW__H
#define __AEGTHESW__H
#ifdef __AEGON_THEME_SWITCHER

#define HIGHLIGHT_COLOR_BLUE 0
#define HIGHLIGHT_COLOR_GREEN 1
#define HIGHLIGHT_COLOR_PURPLE 2
#define HIGHLIGHT_COLOR_YELLOW 3
#define HIGHLIGHT_COLOR_RED 4
#define HIGHLIGHT_COLOR_GREY 5
#define HIGHLIGHT_COLOR_ORANGE 6


#define MAX_HIGHLIGHT_COLOR 7

#define MAX_NAME_COLOR	8

#define AEG_DEFAULT_THEME "- Default -"

typedef struct __AEGON_THEME_FONT
{
	char m_szButtonFont[128];
	DWORD m_dwButtonFontColor;
	DWORD m_dwButtonFontBGColor;
	DWORD m_dwButtonFontSize;
	DWORD m_dwButtonOutline;
	DWORD m_dwFlags;
	DWORD m_dwFontFlags;

	void Init(LPCSTR szName, DWORD dwFontColor, DWORD dwFontColorBG, DWORD dwButtonFontSize, DWORD dwButtonOutline, DWORD dwFlags, DWORD dwFontFlags)
	{
		strcpy(m_szButtonFont, szName);
		m_dwButtonFontColor = dwFontColor;
		m_dwButtonFontBGColor = dwFontColorBG;
		m_dwButtonFontSize = dwButtonFontSize;
		m_dwButtonOutline = dwButtonOutline;
		m_dwFlags = dwFlags;
		m_dwFontFlags = dwFontFlags;
	}
	

}__AEGON_THEME_FONT;

typedef struct __AEGON_THEME_UNIT
{
	int nThemeID;
	char szThemeName[64];
	char szThemeFolder[128];
	int nThemeType;
	D3DFORMAT d3dFormat;
	DWORD dwColorFrameWnd;
	DWORD dwColorWnd;
	DWORD dwTextColor;
	DWORD dwFontTextColor;
	DWORD dwFontTitleColor;
	DWORD dwStaticColor;
	DWORD dwHighlightColor[MAX_HIGHLIGHT_COLOR];
	DWORD dwNameColor[MAX_NAME_COLOR];
	DWORD dwRenderFillRectColor;
	DWORD dwRenderRectColor;

	int m_nScrollBarOffset;
	int m_nTitleBarYOffset;
	CSize m_sizeTitleBarPosOffset;
	map<DWORD, CSize> m_mapWndTitleBarPosOffsets;

	int nTitleOffsetAll;
	map<DWORD, int>m_mTitleOffsets;
	int nPickUpHeightAll;
	map<DWORD, int>m_mPickUpHeights;

	int m_nMaxSlotAppletOffset;
	int m_nOffsetLoopIncrease;
	int m_nOffsetLoopIncreaseQueue;
	int m_nQueueIconOffsetPoint;

	int m_nInventoryCtrlOffsetX;
	int m_nInventoryCtrlOffsetY;

	__AEGON_THEME_FONT m_sFontButton;
	__AEGON_THEME_FONT m_sFontButtonHover;
	__AEGON_THEME_FONT m_sFontMessengerTitle;


	void Init() 
	{
		nThemeID = 0;
		strcpy(szThemeFolder, "Data\\Theme\\Default\\");
		nThemeType = 0;
		d3dFormat = D3DFMT_A4R4G4B4;
		dwColorFrameWnd = 0;
		dwColorWnd = 0;
		dwTextColor = 0xff000000;
		dwFontTextColor = 0xff000000;
		dwFontTitleColor = 0xffffffff;
		dwStaticColor = 0xff2e70a9;
		dwHighlightColor[HIGHLIGHT_COLOR_BLUE] = 0xff0000ff;
		dwHighlightColor[HIGHLIGHT_COLOR_GREEN] = 0xff005d00;
		dwHighlightColor[HIGHLIGHT_COLOR_PURPLE] = 0xff884dff;
		dwHighlightColor[HIGHLIGHT_COLOR_YELLOW] = 0xffbd8033;
		dwHighlightColor[HIGHLIGHT_COLOR_RED] = 0xffff0000;
		dwHighlightColor[HIGHLIGHT_COLOR_GREY] = 0xffb2b2b2;
		dwHighlightColor[HIGHLIGHT_COLOR_ORANGE] = 0xffff9d00;

		dwNameColor[0] = D3DCOLOR_XRGB(46, 112, 169);
		dwNameColor[1] = D3DCOLOR_XRGB(0, 93, 0);
		dwNameColor[2] = D3DCOLOR_XRGB(128, 64, 0);
		dwNameColor[3] = D3DCOLOR_XRGB(127, 0, 255);
		dwNameColor[4] = D3DCOLOR_XRGB(0, 0, 255);
		dwNameColor[5] = D3DCOLOR_XRGB(0, 0, 255);
		dwNameColor[6] = D3DCOLOR_XRGB(189, 128, 51);
		dwNameColor[7] = D3DCOLOR_XRGB(31, 38, 147);

		dwRenderFillRectColor = 0xFFf8e6cc;
		dwRenderRectColor = 0xFFf28818;

		m_nScrollBarOffset = 0;
		m_nTitleBarYOffset = 0;
		m_sizeTitleBarPosOffset = CSize(0, 0);
		m_mapWndTitleBarPosOffsets.clear();

		nTitleOffsetAll = 0;
		m_mTitleOffsets.clear();
		nPickUpHeightAll = 20;
		m_mPickUpHeights.clear();

		m_nMaxSlotAppletOffset = 0;
		m_nOffsetLoopIncrease = 0;
		m_nOffsetLoopIncreaseQueue = 0;
		m_nQueueIconOffsetPoint = 0;

		m_nInventoryCtrlOffsetX = 0;
		m_nInventoryCtrlOffsetY = 0;

		m_sFontButton.Init("Verdana", 0xffffffff, D3DCOLOR_ARGB(255, 135, 135, 135), 10, 1, 0, D3DFONT_BOLD);
		m_sFontButtonHover.Init("Verdana", 0xffffffff, D3DCOLOR_ARGB(255, 199, 107, 75), 10, 1, 0, D3DFONT_BOLD);
		m_sFontMessengerTitle.Init("Verdana", 0xff000000, 0xff000000, 12, 0, 0, 0);


	}
}__AEGON_THEME_UNIT;



class CAegonThemeSwitcherMng
{
public:

	LPCSTR m_scriptFile = "AegonThemeSwitcher.inc";
	BOOL m_bScriptLoaded;

	map<int, __AEGON_THEME_UNIT> m_mapThemes;
	vector<int> m_vecRestartWindows;



	CAegonThemeSwitcherMng();
	virtual ~CAegonThemeSwitcherMng();

	static CAegonThemeSwitcherMng* GetInstance(void);

	BOOL LoadScript(LPCSTR szFileName);

	BOOL ThemeTypeHasChanged(int nPreviousThemeID, int nNewThemeID);
	BOOL WindowNeedsRestart(int wndID);
	int GetThemeType(int nThemeID);
	__AEGON_THEME_UNIT* GetThemeUnit(int nThemeID);

	int GetInventoryCtrlOffsetX(int x);
	int GetInventoryCtrlOffsetY(int y);

	void ThemeSwitchLoadTexture(CTexture* pTexture, LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pFileName, D3DCOLOR d3dKeyColor, BOOL bMyLoader = FALSE);
	CTexture* ThemeSwitchAddTexture(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pFileName, D3DCOLOR d3dKeyColor, BOOL bMyLoader = FALSE);
	void ThemeSwitchSetTexture(CWndBase* pWndBase, LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pFileName, BOOL bMyLoader = FALSE);
	void ThemeSwitchLoadTGA(LPIMAGE image, LPCTSTR pFileName);

	DWORD GetTextColor();
	DWORD GetHighlightColor(int nIndex);
	DWORD GetNameColor(int nIndex);
	void ReplaceColorInString(CString* str, DWORD dwColorToReplace, DWORD dwNewColor);
	DWORD GetRenderFillRectColor();
	DWORD GetRenderRectColor();
	DWORD GetStaticColor();

	int GetPickUpSpaceHeight(DWORD dwWndID);
	int GetTitleDrawOffset(DWORD dwWndID);
	int GetTitleBarYOffset();
	CSize GetTitleBarPosOffset(DWORD dwWndID);
	int GetScrollBarOffset();

};
#endif // __AEGON_THEME_SWITCHER

#endif //__AEGTHESW__H