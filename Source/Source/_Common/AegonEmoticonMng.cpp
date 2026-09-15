#include "stdafx.h"
#include "AegonEmoticonMng.h"
#include "defineObj.h"
#include "defineItem.h"
#include "ResData.h"
#include "d3d9types.h"




#ifdef __CLIENT
#include "DPClient.h"
#include "AppDefine.h"
#include "WndManager.h"
#endif

#define ICON_SIZE 32

#ifdef __AEGON_EMOTE_SYSTEM
CAegonEmoticonMng::CAegonEmoticonMng()
{
	m_bScriptLoaded = LoadScript();
	if (!m_bScriptLoaded)
	{
		m_mapEmotes.clear();
		m_mapAdminEmotes.clear();

	}
	else 
	{
	}
}

CAegonEmoticonMng::~CAegonEmoticonMng()
{
}

CAegonEmoticonMng* CAegonEmoticonMng::GetInstance(void)
{
	static CAegonEmoticonMng sAegonEmoticonMng;
	return &sAegonEmoticonMng;
}

BOOL CAegonEmoticonMng::LoadScript()
{
	CScript s;
	__AEGON_EMOTE_UNIT tmpUnit;

	if (s.Load(m_scriptFile))
	{
		s.GetToken();
		while (s.tok != FINISHED)
		{
			if (s.Token.CompareNoCase("Emote") == 0)
			{
				tmpUnit.Init();
				
				s.GetToken(); // {

				while (s.Token.Compare("}") != 0)
				{
					s.GetToken();
					if (s.Token.CompareNoCase("name") == 0)
					{
						s.GetToken();
						strcpy(tmpUnit.szEmoteName, s.Token);
					}
					else if (s.Token.CompareNoCase("animated") == 0)
					{
						tmpUnit.bAnimated = s.GetNumber() == 0 ? FALSE : TRUE;
					}
					else if (s.Token.CompareNoCase("fileName") == 0)
					{
						s.GetToken();
						strcpy(tmpUnit.szFileName, s.Token);
					}
					else if (s.Token.CompareNoCase("command") == 0)
					{
						s.GetToken();
						strcpy(tmpUnit.szCommand, s.Token);
					}
					else if (s.Token.CompareNoCase("frameTime") == 0)
					{
						tmpUnit.nTimeBetweenFrames = s.GetNumber();
					}
					else if (s.Token.CompareNoCase("mainUnlock") == 0)
					{
						tmpUnit.m_dwSingleUnlockItem = s.GetNumber();
						tmpUnit.m_vUnlockItems.push_back(tmpUnit.m_dwSingleUnlockItem);
					}
					else if (s.Token.CompareNoCase("unlockItem") == 0)
					{
						DWORD dwTmp = s.GetNumber();
						tmpUnit.m_vUnlockItems.push_back(dwTmp);
					}
					else if (s.Token.CompareNoCase("priceDP") == 0)
					{
						tmpUnit.m_dwPriceDP = s.GetNumber();
					}
					else if (s.Token.CompareNoCase("iconFrame") == 0)
					{
						tmpUnit.m_dwIconFrame = s.GetNumber();
					}
					else if (s.Token.CompareNoCase("repeating") == 0)
					{
						tmpUnit.m_bRepeat = s.GetNumber() == 0 ? FALSE : TRUE;
					}
					else if (s.Token.CompareNoCase("adminOnly") == 0)
					{
						tmpUnit.m_bAdminOnly = s.GetNumber() == 0 ? FALSE : TRUE;
					}
					else if (s.Token.Compare("}") == 0)
					{
						if (tmpUnit.m_bAdminOnly) 
						{
							tmpUnit.nEmoteID = m_mapAdminEmotes.size();
							m_mapAdminEmotes.insert(make_pair(tmpUnit.nEmoteID, tmpUnit));
						}
						else 
						{
							tmpUnit.nEmoteID = m_mapEmotes.size();
							m_mapEmotes.insert(make_pair(tmpUnit.nEmoteID, tmpUnit));
						}
					}
				}
			}
			s.GetToken();
		}
	}
	else
	{
		Error("failed to Load %s", m_scriptFile);
		return FALSE;
	}
	return TRUE;
}

#ifdef __CLIENT
void CAegonEmoticonMng::LoadSingleEmote(__AEGON_EMOTE_UNIT* unit)
{
	CString strName;
	strName.Format("aegon_emote_%d", unit->nEmoteID);
	if (unit->bAnimated) 
	{
		CAnimatedTexture* tmpAniTexture = CWndBase::m_animatedTextureMng.AddAnimatedTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_APNG, unit->szFileName));
		if (tmpAniTexture) 
		{
			CAnimatedTexture* newAniTexture = new CAnimatedTexture;
			if (newAniTexture) 
			{
				*newAniTexture = *tmpAniTexture;
				unit->m_pAniTexture = CWndBase::m_animatedTextureMng.AddAnimatedTexture(strName.GetString(), newAniTexture);

				if (unit->m_pAniTexture) 
				{
					if (!unit->m_bRepeat) 
					{
						unit->m_pAniTexture->m_bLoopAnimation = FALSE;
					}

					if (unit->m_dwIconFrame >= unit->m_pAniTexture->GetFrameCount())
						unit->m_dwIconFrame = 0;

					if (unit->m_pAniTexture->GetFrameCount() > 0) 
					{
						CTexture* tmpFrame = unit->m_pAniTexture->GetFrame(unit->m_dwIconFrame);
						if (tmpFrame->m_size.cx != ICON_SIZE || tmpFrame->m_size.cy != ICON_SIZE) 
						{
							CTexture* newTexture = new CTexture;
							if (newTexture)
							{
								*newTexture = *tmpFrame;
								newTexture->m_size.cx = ICON_SIZE;
								newTexture->m_size.cy = ICON_SIZE;

								CString strIconName;
								strIconName.Format("%s_icon", strName.GetString());
								unit->m_pIconTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, strIconName.GetString(), newTexture);
							}
						}
						else
							unit->m_pIconTexture = unit->m_pAniTexture->GetFrame(unit->m_dwIconFrame);
					}
					
					if (unit->nTimeBetweenFrames > 0) 
					{
						unit->m_pAniTexture->m_dwTimeBetweenFrames = unit->nTimeBetweenFrames;
					}
				}
			}
		}
	}
	else 
	{
		CTexture* tmpTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_EMOTE, unit->szFileName), COLOR_WHITE, TRUE);
		if (tmpTexture) 
		{
			CTexture* newTexture = new CTexture;
			if (newTexture) 
			{
				*newTexture = *tmpTexture;
				unit->m_pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, strName.GetString(), newTexture);

				unit->m_pIconTexture = unit->m_pTexture;
			}
		}
	}
}

void CAegonEmoticonMng::LoadAllEmotes()
{
	map<int, __AEGON_EMOTE_UNIT>::iterator it = m_mapEmotes.begin();
	for (; it != m_mapEmotes.end(); it++)
	{
		LoadSingleEmote(&it->second);
	}

	it = m_mapAdminEmotes.begin();
	for (; it != m_mapAdminEmotes.end(); it++)
	{
		LoadSingleEmote(&it->second);
	}
}
void CAegonEmoticonMng::MakeEmoticonTooltipText(char* szText, int nEmoteID, DWORD dwType)
{
	map<int, __AEGON_EMOTE_UNIT>* pMap = GetEmoteMap(dwType);
	if (!pMap)
		return;

	if (!IsValidObj(g_pPlayer))
		return;

	if (!szText || nEmoteID < 0 || nEmoteID >= pMap->size())
		return;

	
	BOOL bPremiumUser = g_pPlayer->IsPremium();


	/*
	* BOOL bPremiumUser = g_pPlayer->IsPremium();
	if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
		bPremiumUser = TRUE;
		*/
	map<int, __AEGON_EMOTE_UNIT>::iterator it = pMap->find(nEmoteID);
	if (it != pMap->end())
	{
		BOOL bEmoteOwned = FALSE;
		if (it->second.m_vUnlockItems.size() > 0)
		{
			for (int i = 0; i < it->second.m_vUnlockItems.size(); i++)
			{
				if (g_pPlayer && (/*bPremiumUser ||*/ easy_find(g_pPlayer->m_vUnlockedEmotes, it->second.m_vUnlockItems[i])))
				{
					bEmoteOwned = TRUE;
					break;
				}
			}
		}
		else
			bEmoteOwned = TRUE;


		if (it->second.bAnimated)
			_tcscat(szText, "Animated Emote");
		else
			_tcscat(szText, "Emote");

		if (!bEmoteOwned)
			_tcscat(szText, " (Locked)");

		_tcscat(szText, "\n");
		_tcscat(szText, "/");
		_tcscat(szText, it->second.szCommand);
	}

}
CTexture* CAegonEmoticonMng::GetIconTexture(int nIndex, DWORD dwType)
{
	map<int, __AEGON_EMOTE_UNIT>* pMap = GetEmoteMap(dwType);
	if (!pMap)
		return NULL;

	map<int, __AEGON_EMOTE_UNIT>::iterator it = pMap->find(nIndex);


	if (it != pMap->end())
	{
		return it->second.m_pIconTexture;
	}
	return NULL;
}
#endif

BOOL CAegonEmoticonMng::IsEmoticonCommand(LPCSTR szCommand, DWORD dwType)
{
	map<int, __AEGON_EMOTE_UNIT>* pMap = GetEmoteMap(dwType);
	if (!pMap)
		return FALSE;

	map<int, __AEGON_EMOTE_UNIT>::iterator it = pMap->begin();
	for (; it != pMap->end(); it++)
	{
		if (stricmp(&(szCommand[1]), it->second.szCommand) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}
LPCSTR CAegonEmoticonMng::GetCommand(int nIndex, DWORD dwType)
{
	map<int, __AEGON_EMOTE_UNIT>* pMap = GetEmoteMap(dwType);
	if (!pMap)
		return "";

	map<int, __AEGON_EMOTE_UNIT>::iterator it = pMap->find(nIndex);

	if(it != pMap->end())
	{
		return it->second.szCommand;
	}
	return "";
}



__AEGON_EMOTE_UNIT* CAegonEmoticonMng::GetUnitByCommand(LPCSTR szCommand, DWORD dwType) 
{
	map<int, __AEGON_EMOTE_UNIT>* pMap = GetEmoteMap(dwType);
	if (!pMap)
		return NULL;

	map<int, __AEGON_EMOTE_UNIT>::iterator it = pMap->begin();
	for (; it != pMap->end(); it++)
	{
		if (stricmp(&(szCommand[1]), it->second.szCommand) == 0)
		{
			return &it->second;
		}
	}

	return NULL;
}
__AEGON_EMOTE_UNIT* CAegonEmoticonMng::GetUnitByID(int nEmoteID, DWORD dwType)
{
	map<int, __AEGON_EMOTE_UNIT>* pMap = GetEmoteMap(dwType);
	if (!pMap)
		return NULL;

	map<int, __AEGON_EMOTE_UNIT>::iterator it = pMap->find(nEmoteID);
	if (it != pMap->end())
	{
		return &it->second;
	}
	return NULL;
}


BOOL CAegonEmoticonMng::PlayerOwnsEmote(CMover* pMover, int nEmoteID, DWORD dwType) 
{
	map<int, __AEGON_EMOTE_UNIT>* pMap = GetEmoteMap(dwType);
	if (!pMap)
		return NULL;

	if (!IsValidObj(pMover))
		return FALSE;

	if (nEmoteID < 0 || nEmoteID >= pMap->size())
		return FALSE;

	/*
	BOOL bPremiumUser = pMover->IsPremium();

	if (pMover->IsAuthHigher(AUTH_GAMEMASTER))
		bPremiumUser = TRUE;
	
	if (bPremiumUser)
		return TRUE;
		*/

	map<int, __AEGON_EMOTE_UNIT>::iterator it = pMap->find(nEmoteID);
	if (it != pMap->end())
	{
		if (it->second.m_vUnlockItems.size() <= 0)
			return TRUE;

		if (pMover->m_vUnlockedEmotes.size() <= 0)
			return FALSE;

		for (int i = 0; i < pMover->m_vUnlockedEmotes.size(); i++) 
		{
			if (easy_find(it->second.m_vUnlockItems, pMover->m_vUnlockedEmotes[i]))
				return TRUE;
		}
	}
	return FALSE;
}

map<int, __AEGON_EMOTE_UNIT>* CAegonEmoticonMng::GetEmoteMap(DWORD dwType)
{
	map<int, __AEGON_EMOTE_UNIT>* pMap = NULL;
	if (dwType == 2)
		pMap = &m_mapAdminEmotes;
	else
		pMap = &m_mapEmotes;

	return pMap;
}
#endif //__AEGON_EMOTE_SYSTEM