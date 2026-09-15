#include "stdafx.h"
#include "AegonThemeSwitcherMng.h"
#include "defineObj.h"
#include "defineItem.h"
#include "ResData.h"
#include "d3d9types.h"
#include "WndManager.h"
#include "DPClient.h"
#include "AppDefine.h"
#include "WndBagEx.h"

#ifdef __AEGON_THEME_SWITCHER

CAegonThemeSwitcherMng::CAegonThemeSwitcherMng()
{
	m_bScriptLoaded = LoadScript(m_scriptFile);
	if (!m_bScriptLoaded)
	{

	}
	m_bScriptLoaded = LoadScript(MakePath(DIR_THEME_SWITCHER, "CustomThemes.inc"));
	if (!m_bScriptLoaded)
	{

	}

	if (g_Option.m_nSelectedThemeID >= m_mapThemes.size())
		g_Option.m_nSelectedThemeID = 0;

}

CAegonThemeSwitcherMng::~CAegonThemeSwitcherMng()
{
}

CAegonThemeSwitcherMng* CAegonThemeSwitcherMng::GetInstance(void)
{
	static CAegonThemeSwitcherMng sAegonThemeSwitcherMng;
	return &sAegonThemeSwitcherMng;
}

BOOL CAegonThemeSwitcherMng::LoadScript(LPCSTR szFileName)
{
	CScript s;
	__AEGON_THEME_UNIT tmpTheme;

	if (s.Load(szFileName))
	{
		s.GetToken();
		while (s.tok != FINISHED)
		{
			if (s.Token.Compare("Theme") == 0)
			{
				tmpTheme.Init();
				tmpTheme.nThemeID = m_mapThemes.size();
				s.GetToken(); // {

				while (s.Token.Compare("}") != 0)
				{
					s.GetToken();
					if (s.Token.Compare("themeFolder") == 0)
					{
						s.GetToken();
						strcpy(tmpTheme.szThemeFolder, s.Token);
					}
					else if (s.Token.Compare("themeName") == 0)
					{

						s.GetToken();
						strcpy(tmpTheme.szThemeName, s.Token);
					}
					else if (s.Token.Compare("themeType") == 0)
					{
						tmpTheme.nThemeType = s.GetNumber();
					}
					else if (s.Token.Compare("wndColor") == 0) 
					{
						tmpTheme.dwColorWnd = s.GetHex();
					}
					else if (s.Token.Compare("wndColorFrame") == 0)
					{
						tmpTheme.dwColorFrameWnd = s.GetHex();
					}
					else if (s.Token.Compare("textColor") == 0) 
					{
						tmpTheme.dwTextColor = s.GetHex();
					}
					else if (s.Token.Compare("fontTextColor") == 0)
					{
						tmpTheme.dwFontTextColor = s.GetHex();
					}
					else if (s.Token.Compare("fontTitleColor") == 0)
					{
						tmpTheme.dwFontTitleColor = s.GetHex();
					}
					else if (s.Token.Compare("staticColor") == 0)
					{
						tmpTheme.dwStaticColor = s.GetHex();
					}
					else if (s.Token.Compare("highlightColor") == 0)
					{
						int nIndex = s.GetNumber();
						DWORD dwColor = s.GetHex();
						if (nIndex >= 0 && nIndex < MAX_HIGHLIGHT_COLOR)
							tmpTheme.dwHighlightColor[nIndex] = dwColor;
					}
					else if (s.Token.Compare("nameColor") == 0)
					{
						int nIndex = s.GetNumber();
						DWORD dwColor = s.GetHex();
						if (nIndex >= 0 && nIndex < MAX_NAME_COLOR)
							tmpTheme.dwNameColor[nIndex] = dwColor;
					}
					else if (s.Token.Compare("RenderFillRect") == 0)
					{
						tmpTheme.dwRenderFillRectColor = s.GetHex();
					}
					else if (s.Token.Compare("RenderRect") == 0)
					{
						tmpTheme.dwRenderRectColor = s.GetHex();
					}
					else if (s.Token.Compare("titleDrawOffset") == 0)
					{
						int nWndID = s.GetNumber();
						int nOffset = s.GetNumber();
						if (nWndID == 0) 
						{
							tmpTheme.nTitleOffsetAll = nOffset;
						}
						else 
						{
							if (tmpTheme.m_mTitleOffsets.find((DWORD)nWndID) == tmpTheme.m_mTitleOffsets.end())
								tmpTheme.m_mTitleOffsets.insert(make_pair((DWORD)nWndID, nOffset));
						}
						
					}
					else if (s.Token.Compare("pickUpHeight") == 0)
					{
						int nWndID = s.GetNumber();
						int nHeight = s.GetNumber();
						if (nWndID == 0)
						{
							tmpTheme.nPickUpHeightAll = nHeight;
						}
						else 
						{
							if (tmpTheme.m_mPickUpHeights.find((DWORD)nWndID) == tmpTheme.m_mPickUpHeights.end())
								tmpTheme.m_mPickUpHeights.insert(make_pair((DWORD)nWndID, nHeight));
						}
						
					}
					else if (s.Token.Compare("titleBarYOffset") == 0)
					{
						tmpTheme.m_nTitleBarYOffset = s.GetNumber();
					}
					else if (s.Token.Compare("titleBarPosOffset") == 0)
					{
						tmpTheme.m_sizeTitleBarPosOffset.cx = s.GetNumber();
						tmpTheme.m_sizeTitleBarPosOffset.cy = s.GetNumber();
					}
					else if (s.Token.Compare("wndTitleBarPosOffset") == 0)
					{
						DWORD dwWndID = s.GetNumber();
						CSize tmpSize;
						tmpSize.cx = s.GetNumber();
						tmpSize.cy = s.GetNumber();

						tmpTheme.m_mapWndTitleBarPosOffsets.insert(make_pair(dwWndID, tmpSize));
					}
					else if (s.Token.Compare("scrollBarOffset") == 0)
					{
						tmpTheme.m_nScrollBarOffset = s.GetNumber();
					}
					else if (s.Token.Compare("maxSlotAppletOffset") == 0)
					{
						tmpTheme.m_nMaxSlotAppletOffset = s.GetNumber();
					}
					else if (s.Token.Compare("offsetLoopIncrease") == 0)
					{
						tmpTheme.m_nOffsetLoopIncrease = s.GetNumber();
					}
					else if (s.Token.Compare("offsetLoopIncreaseQueue") == 0)
					{
						tmpTheme.m_nOffsetLoopIncreaseQueue = s.GetNumber();
					}
					else if (s.Token.Compare("queueIconOffsetPoint") == 0)
					{
						tmpTheme.m_nQueueIconOffsetPoint = s.GetNumber();
					}	
					else if (s.Token.Compare("inventoryCtrlOffsetXY") == 0)
					{
						tmpTheme.m_nInventoryCtrlOffsetX = s.GetNumber();
						tmpTheme.m_nInventoryCtrlOffsetY = s.GetNumber();
					}
					else if(s.Token.Compare("buttonFont") == 0 || s.Token.Compare("buttonFontHover") == 0 || s.Token.Compare("messengerFontTitle") == 0)
					{
						__AEGON_THEME_FONT* pFont = NULL;
						if(s.Token.Compare("buttonFontHover") == 0)
							pFont = &tmpTheme.m_sFontButtonHover;
						else if(s.Token.Compare("buttonFont") == 0)
							pFont = &tmpTheme.m_sFontButton;
						else /*if(s.Token.Compare("messengerFontTitle") == 0)*/
							pFont = &tmpTheme.m_sFontMessengerTitle;

						s.GetToken();
						strcpy(pFont->m_szButtonFont, s.Token);

						pFont->m_dwButtonFontColor = s.GetHex();
						pFont->m_dwButtonFontBGColor = s.GetHex();
						pFont->m_dwButtonFontSize = s.GetNumber();
						pFont->m_dwButtonOutline = s.GetNumber();
						pFont->m_dwFlags = s.GetNumber();
						pFont->m_dwFontFlags = s.GetNumber();

					}
					else if (s.Token.Compare("}") == 0)
					{
						if (tmpTheme.nThemeType == 1) 
						{
							tmpTheme.d3dFormat = D3DFMT_A8R8G8B8;
						}
						if (m_mapThemes.find(tmpTheme.nThemeID) == m_mapThemes.end())
						{
							m_mapThemes.insert(make_pair(tmpTheme.nThemeID, tmpTheme));
						}
					}
				}
			}
			else if (s.Token.Compare("restartWindows") == 0) 
			{
				s.GetToken(); // {
				while (s.Token.Compare("}") != 0) 
				{
					s.GetToken();
					if (s.Token.Compare("addWnd") == 0) 
					{
						int tmpID = s.GetNumber();
						if (!easy_find(m_vecRestartWindows, tmpID) && tmpID > 0)
							m_vecRestartWindows.push_back(tmpID);
					}
					
				}
			}
			s.GetToken();
		}
	}
	else
	{
		Error("failed to Load %s", szFileName);
		return FALSE;
	}
	return TRUE;
}

BOOL CAegonThemeSwitcherMng::ThemeTypeHasChanged(int nPreviousThemeID, int nNewThemeID)
{
	map<int, __AEGON_THEME_UNIT>::iterator itPrevious = m_mapThemes.find(nPreviousThemeID);
	map<int, __AEGON_THEME_UNIT>::iterator itNew = m_mapThemes.find(nNewThemeID);
	if (itPrevious != m_mapThemes.end() && itNew != m_mapThemes.end()) 
	{
		if (itPrevious->second.nThemeType != itNew->second.nThemeType)
			return TRUE;
	}
	return FALSE;
}

BOOL CAegonThemeSwitcherMng::WindowNeedsRestart(int wndID) 
{
	if (easy_find(m_vecRestartWindows, wndID))
		return TRUE;
	return FALSE;
}
int CAegonThemeSwitcherMng::GetThemeType(int nThemeID) 
{
	map<int, __AEGON_THEME_UNIT>::iterator it = m_mapThemes.find(nThemeID);
	if (it != m_mapThemes.end())
	{
		return it->second.nThemeType;
	}
	return 0;
}

__AEGON_THEME_UNIT* CAegonThemeSwitcherMng::GetThemeUnit(int nThemeID)
{
	map<int, __AEGON_THEME_UNIT>::iterator it = m_mapThemes.find(nThemeID);
	if (it != m_mapThemes.end())
	{
		return &it->second;
	}
	return NULL;
}

int CAegonThemeSwitcherMng::GetInventoryCtrlOffsetX(int x) 
{
	__AEGON_THEME_UNIT* pThemeUnit = GetThemeUnit(g_Option.m_nSelectedThemeID);
	if (!pThemeUnit)
		return 0;

	return pThemeUnit->m_nInventoryCtrlOffsetX * x;
}
int CAegonThemeSwitcherMng::GetInventoryCtrlOffsetY(int y) 
{
	__AEGON_THEME_UNIT* pThemeUnit = GetThemeUnit(g_Option.m_nSelectedThemeID);
	if (!pThemeUnit)
		return 0;

	return pThemeUnit->m_nInventoryCtrlOffsetY * y;
}

void CAegonThemeSwitcherMng::ThemeSwitchLoadTexture(CTexture* pTexture, LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pFileName, D3DCOLOR d3dKeyColor, BOOL bMyLoader)
{
	pTexture->DeleteDeviceObjects();
	SAFE_DELETE(pTexture->m_pTexture);

	map<int, __AEGON_THEME_UNIT>::iterator it = m_mapThemes.find(g_Option.m_nSelectedThemeID);
	BOOL bSuccess = FALSE;
	if (it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end())
	{
		bSuccess = pTexture->LoadTexture(pd3dDevice, MakePath(it->second.szThemeFolder, pFileName), d3dKeyColor, bMyLoader, FALSE);
	}
	if (!bSuccess)
	{
		pTexture->LoadTexture(pd3dDevice, MakePath(DIR_THEME, pFileName), d3dKeyColor, bMyLoader);
	}
}
CTexture* CAegonThemeSwitcherMng::ThemeSwitchAddTexture(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pFileName, D3DCOLOR d3dKeyColor, BOOL bMyLoader)
{
	CTexture* tmpTexture;
	map<int, __AEGON_THEME_UNIT>::iterator it = m_mapThemes.find(g_Option.m_nSelectedThemeID);
	if (it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end())
	{
		tmpTexture = CWndBase::m_textureMng.AddTexture(pd3dDevice, MakePath(it->second.szThemeFolder, pFileName), d3dKeyColor, bMyLoader, FALSE);
	}
	if (!tmpTexture)
	{
		tmpTexture = CWndBase::m_textureMng.AddTexture(pd3dDevice, MakePath(DIR_THEME, pFileName), d3dKeyColor, bMyLoader);
	}
	return tmpTexture;
}

void CAegonThemeSwitcherMng::ThemeSwitchSetTexture(CWndBase* pWndBase, LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pFileName, BOOL bMyLoader)
{
	map<int, __AEGON_THEME_UNIT>::iterator it = m_mapThemes.find(g_Option.m_nSelectedThemeID);
	BOOL bSuccess = FALSE;
	if (it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end())
	{
		bSuccess = pWndBase->SetTexture(pd3dDevice, MakePath(it->second.szThemeFolder, pFileName), bMyLoader, FALSE);
	}

	if (!bSuccess)
	{
		pWndBase->SetTexture(pd3dDevice, MakePath(DIR_THEME, pFileName), bMyLoader);
	}
}

void CAegonThemeSwitcherMng::ThemeSwitchLoadTGA(LPIMAGE image, LPCTSTR pFileName)
{
	map<int, __AEGON_THEME_UNIT>::iterator it = m_mapThemes.find(g_Option.m_nSelectedThemeID);
	BOOL bSuccess = FALSE;
	if (it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end())
	{
		bSuccess = LoadTGA(MakePath(it->second.szThemeFolder, pFileName), image, FALSE);
	}

	if (!bSuccess)
	{
		LoadTGA(MakePath(DIR_THEME, pFileName), image);
	}
}

DWORD CAegonThemeSwitcherMng::GetTextColor() 
{
	DWORD dwColor = 0xff000000;
	map<int, __AEGON_THEME_UNIT>::iterator it = m_mapThemes.find(g_Option.m_nSelectedThemeID);
	if (it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end())
	{
		return it->second.dwFontTextColor;
	}
	return dwColor;
}

DWORD CAegonThemeSwitcherMng::GetHighlightColor(int nIndex)
{
	DWORD dwColor = 0xffffffff;

	if (nIndex < 0 || nIndex >= MAX_HIGHLIGHT_COLOR)
		return dwColor;

	map<int, __AEGON_THEME_UNIT>::iterator it = m_mapThemes.find(g_Option.m_nSelectedThemeID);
	if (it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end())
	{
		return it->second.dwHighlightColor[nIndex];
	}
	return dwColor;
}
DWORD CAegonThemeSwitcherMng::GetNameColor(int nIndex)
{
	DWORD dwColor = 0xffffffff;

	if (nIndex < 0 || nIndex >= MAX_NAME_COLOR)
		return dwColor;

	map<int, __AEGON_THEME_UNIT>::iterator it = m_mapThemes.find(g_Option.m_nSelectedThemeID);
	if (it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end())
	{
		return it->second.dwNameColor[nIndex];
	}
	return dwColor;
}

void CAegonThemeSwitcherMng::ReplaceColorInString(CString* str, DWORD dwColorToReplace, DWORD dwNewColor) 
{
	if (!str)
		return;

	CString strReplace, strNew;
	strReplace.Format("#c%08x", dwColorToReplace);
	strNew.Format("#c%08x", dwNewColor);

	str->Replace(strReplace.GetString(), strNew.GetString());
}

DWORD CAegonThemeSwitcherMng::GetRenderFillRectColor()
{
	DWORD dwColor = 0xff000000;
	map<int, __AEGON_THEME_UNIT>::iterator it = m_mapThemes.find(g_Option.m_nSelectedThemeID);
	if (it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end())
	{
		return it->second.dwRenderFillRectColor;
	}
	return dwColor;
}

DWORD CAegonThemeSwitcherMng::GetRenderRectColor()
{
	DWORD dwColor = 0xff000000;
	map<int, __AEGON_THEME_UNIT>::iterator it = m_mapThemes.find(g_Option.m_nSelectedThemeID);
	if (it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end())
	{
		return it->second.dwRenderRectColor;
	}
	return dwColor;
}

DWORD CAegonThemeSwitcherMng::GetStaticColor()
{
	DWORD dwColor = 0xff000000;
	map<int, __AEGON_THEME_UNIT>::iterator it = m_mapThemes.find(g_Option.m_nSelectedThemeID);
	if (it != CAegonThemeSwitcherMng::GetInstance()->m_mapThemes.end())
	{
		return it->second.dwStaticColor;
	}
	return dwColor;
}

int CAegonThemeSwitcherMng::GetPickUpSpaceHeight(DWORD dwWndID) 
{
	int nOut = 0;
	__AEGON_THEME_UNIT* pThemeUnit = GetThemeUnit(g_Option.m_nSelectedThemeID);
	if (pThemeUnit) 
	{
		nOut = pThemeUnit->nPickUpHeightAll;
		auto it = pThemeUnit->m_mPickUpHeights.find(dwWndID);
		if (it != pThemeUnit->m_mPickUpHeights.end())
			nOut = it->second;
	}
	return nOut;
}
int CAegonThemeSwitcherMng::GetTitleDrawOffset(DWORD dwWndID) 
{
	int nOut = 0;
	__AEGON_THEME_UNIT* pThemeUnit = GetThemeUnit(g_Option.m_nSelectedThemeID);
	if (pThemeUnit)
	{
		nOut = pThemeUnit->nTitleOffsetAll;
		auto it = pThemeUnit->m_mTitleOffsets.find(dwWndID);
		if (it != pThemeUnit->m_mTitleOffsets.end())
			nOut = it->second;
	}
	return nOut;
}
int CAegonThemeSwitcherMng::GetTitleBarYOffset()
{
	int nOut = 0;
	__AEGON_THEME_UNIT* pThemeUnit = GetThemeUnit(g_Option.m_nSelectedThemeID);
	if (pThemeUnit)
	{
		nOut = pThemeUnit->m_nTitleBarYOffset;
	}
	return nOut;
}
CSize CAegonThemeSwitcherMng::GetTitleBarPosOffset(DWORD dwWndID)
{
	__AEGON_THEME_UNIT* pThemeUnit = GetThemeUnit(g_Option.m_nSelectedThemeID);
	if (pThemeUnit)
	{
		if (dwWndID > 0) 
		{
			map<DWORD, CSize>::iterator it = pThemeUnit->m_mapWndTitleBarPosOffsets.find(dwWndID);
			if (it != pThemeUnit->m_mapWndTitleBarPosOffsets.end())
			{
				return it->second;
			}
		}
		return pThemeUnit->m_sizeTitleBarPosOffset;
	}
	return CSize(0, 0);
}
int CAegonThemeSwitcherMng::GetScrollBarOffset()
{
	int nOut = 0;
	__AEGON_THEME_UNIT* pThemeUnit = GetThemeUnit(g_Option.m_nSelectedThemeID);
	if (pThemeUnit)
	{
		nOut = pThemeUnit->m_nScrollBarOffset;
	}
	return nOut;
}
#endif // __AEGON_THEME_SWITCHER