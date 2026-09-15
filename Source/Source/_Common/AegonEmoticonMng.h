#pragma once
#ifndef __AEGEMO__H
#define __AEGEMO__H
#ifdef __AEGON_EMOTE_SYSTEM

#define MAX_COMMAND_STR	32
#define MAX_NAME_STR	64


typedef struct __AEGON_EMOTE_UNIT
{
	int nEmoteID;
	char szEmoteName[MAX_NAME_STR];
	char szFileName[MAX_NAME_STR];
	char szCommand[MAX_COMMAND_STR];
	BOOL bAnimated;
	int nTimeBetweenFrames;
#ifdef __CLIENT
	CTexture* m_pTexture;
	CTexture* m_pIconTexture;
	CAnimatedTexture* m_pAniTexture;
#endif

	DWORD m_dwSingleUnlockItem;
	vector<DWORD> m_vUnlockItems;
	DWORD m_dwPriceDP;
	DWORD m_dwIconFrame;
	BOOL m_bRepeat;
	BOOL m_bAdminOnly;
	

	__AEGON_EMOTE_UNIT()
	{
		Init();
	}

	void Init() 
	{
		nEmoteID = -1;
		strcpy(szEmoteName, "err_name");
		strcpy(szFileName, "err_file");
		strcpy(szCommand, "err_command");
		bAnimated = FALSE;
		nTimeBetweenFrames = -1;
#ifdef __CLIENT
		m_pTexture = NULL;
		m_pIconTexture = NULL;
		m_pAniTexture = NULL;
#endif
		m_dwSingleUnlockItem = 0;
		m_vUnlockItems.clear();
		m_dwPriceDP = 0;
		m_dwIconFrame = 0;

		m_bRepeat = TRUE;

		m_bAdminOnly = FALSE;
	}

	DWORD GetUnitType() 
	{
		DWORD dwOut = 1;
		if (m_bAdminOnly)
			dwOut = 2;

		return dwOut;
	}
}__AEGON_EMOTE_UNIT;



class CAegonEmoticonMng
{
public:

	LPCSTR m_scriptFile = "AegonEmoticon.inc";
	BOOL m_bScriptLoaded;

	map<int, __AEGON_EMOTE_UNIT> m_mapEmotes;

	map<int, __AEGON_EMOTE_UNIT> m_mapAdminEmotes;

	CAegonEmoticonMng();
	virtual ~CAegonEmoticonMng();

	static CAegonEmoticonMng* GetInstance(void);

	BOOL LoadScript();

#ifdef __CLIENT
	void LoadSingleEmote(__AEGON_EMOTE_UNIT* unit);
	void LoadAllEmotes();
	void MakeEmoticonTooltipText(char* szText, int nEmoteID, DWORD dwType);
	CTexture* GetIconTexture(int nIndex, DWORD dwType);
#endif
	BOOL IsEmoticonCommand(LPCSTR szCommand, DWORD dwType);
	LPCSTR GetCommand(int nIndex, DWORD dwType);
	
	__AEGON_EMOTE_UNIT* GetUnitByCommand(LPCSTR szCommand, DWORD dwType);
	__AEGON_EMOTE_UNIT* GetUnitByID(int nEmoteID, DWORD dwType);
	
	BOOL PlayerOwnsEmote(CMover* pMover, int nEmoteID, DWORD dwType);

	map<int, __AEGON_EMOTE_UNIT>* GetEmoteMap(DWORD dwType);
};
#endif // __AEGON_EMOTE_SYSTEM

#endif //__AEGEMO__H