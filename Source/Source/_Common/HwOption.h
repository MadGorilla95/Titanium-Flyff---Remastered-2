#ifndef __HWOPTION_H__
#define	__HWOPTION_H__

#ifdef __AEGON_ADVANCED_LOGIN
typedef struct __LOGIN_ACCOUNT
{
	TCHAR		szAccount[64];
	TCHAR		szPassword[128];
}__LOGIN_ACCOUNT;
#endif // __AEGON_ADVANCED_LOGIN

class COption
{
public:
	TCHAR	m_szFileName[MAX_PATH];
//	int		m_nSoundEffect;
	int		m_nResWidth;
	int		m_nResHeight;
	BOOL	m_bStartFullScreen;
//	float	m_fMusicVolume;
	int		m_nTextureQuality;		// 텍스쳐 품질 높음0,중간1,낮음2
	int		m_nViewArea;			// 시야범위 멀리0,중간1,가깝게2
	int		m_nObjectDetail;		// 오브젝트 표현수준 높음0,중간1,낮음2
	int     m_nObjectDistant;       // 오브젝트 거리  높음0,중간1,낮음2
	int     m_nShadow; // 그림자 
	int		m_nToggleScreen;		// 알트엔터 가능/못가능.  이건 유저들은 모른다.
	BOOL    m_bOperator;
	DWORD   m_dwChatFilter[5];
	DWORD   m_dwChatFilterEtc;
	int		m_nBloom;				// 뽀샤시 효과.	
	int		m_nMonName;		// 몬스터 이름 On / Off
	int		m_nPlayerName;		    // 자신 이름 On / Off
	int		m_nOtherPlayerName;		// 다른 플레이어 이름 On / Off
	int		m_nToolTipTexture;		// 툴팁 인터페이스 바꾸기
	int		m_nToolTipText;
#ifdef __SFX_OPT
	int		m_nSfxLevel;	
#endif
	BOOL	m_nWeatherEffect;
//#ifdef __YADDSOUND
	BOOL    m_bVoice;
	float	m_fEffectVolume;
	float	m_fBGMVolume;
//#endif //__YADDSOUND

	// 윈도 옵션
	int     m_nWindowAlpha; // 0 ~ 255
	int     m_nWindowEffect; // 0(일반), 1(반투명), 2(사각)

	// 속어 단어 필터
	int     m_nSlangWord; // 0(교체), 1(숨김)

	// 채팅 명령어
	int     m_nChatCommand; // 0(한글 풀문장),1(한글축약어),2(영문풀문장),3(영문축약어)
	
	// 초보자 도움말(채팅창에 나오는 것)
	int     m_nInstantHelp; // 
	
	// 인터페이스 방식
	int		m_nInterface;	// 0 : 구버전 1 : 신버전(우클릭으로 카메라 회전하는..)
	BOOL	m_bRollEffect;	// 비행시 롤링 효과.
	
	BOOL	m_bSay;			// 귀속말 승인 / 거부
	BOOL	m_bTrade;		// 거래 승인 / 거부
	BOOL	m_bParty;		// 파티 승인 / 거부
	BOOL	m_bShout;		// 외치기 표시 ON / OFF
	BOOL	m_bMessengerJoin; // Messenger 알림 표시 On / Off
	BOOL	m_bMessenger;	// Messenger 승인 / 거부
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	BOOL	m_bBattleBGM;	// 전투 음악 ON / OFF
#endif // __IMPROVE_SYSTEM_VER15

	int		m_nSer;		// 이전에 접속했던 서버
	int		m_nMSer;	// 이전에 접속했던 멀티 서버
	
	BOOL    m_bSaveAccount;
	TCHAR   m_szAccount[ 64 ];
	BOOL    m_bNotice;
	BOOL    m_bTip;
	time_t  m_tNoticeTime;
	
	int		m_nGuide1;	
	int		m_nGuide2;	
	int		m_nGuide3;	
#if __VER >= 12 // __MOD_TUTORIAL
	int		m_nTutorialLv;
#endif
#if __VER >= 12 // __UPDATE_OPT
	BOOL	m_bViewMask;
#endif
	BOOL    m_bIsShowReSkillMessage;

	TCHAR	m_IPAddress[32];

#ifdef __SYS_PET_NO_CLICK
	unsigned char petClickOption;
#endif //__SYS_PET_NO_CLICK

#ifdef __YENV
	BOOL	m_bExitChange;
	BOOL	m_bSpecBump;
#endif //__YENV

#ifdef __NEW_NIGHT
	BOOL	m_bNewNight;
#endif // __NEW_NIGHT

#ifdef __SYS_GROUND_ITEM_TEXT
	BOOL	groundItemText;
#endif //__SYS_GROUND_ITEM_TEXT

#ifdef __MUTEBGSOUNDS
	bool muteSoundsInBG;
	bool muteMusicInBG;
#endif // __MUTEBGSOUNDS

#ifdef __BACKGROUNDRENDER
	int inactiveRender;
#endif // __BACKGROUNDRENDER

#ifdef __ALIASING
	BOOL	m_nAliasing;
#endif // __ALIASING

#ifdef __ANISOTROPIC
	BOOL	m_nAnisotropic;
#endif // __ANISOTROPIC

#ifdef __AEGON_FONT_SWITCH
	CString	m_strFont;
#endif // __AEGON_FONT_SWITCH

#ifdef __HIDE_NPC_BUBBLES
	BOOL	m_nNPCBubbles;
#endif //__HIDE_NPC_BUBBLES

#ifdef __HIDE_POWER_UPS
	BOOL	m_nHidePU;
#endif //__HIDE_POWER_UPS

#ifdef __HIDE_MONSTER_LEVEL
	BOOL	m_nMonsterLevel;
#endif //__HIDE_MONSTER_LEVEL

#ifdef __ALPHA_CHAT
	bool	alphaChat;
#endif //__ALPHA_CHAT

#ifdef __STOP_EXP
	BOOL	m_bExpStop;
#endif //__STOP_EXP

#ifndef __AEGON_DB_PETFILTER
#ifdef __SYS_PETFILTER
	LONGLONG m_dwPetfilter;
#endif //__SYS_PETFILTER
#endif

#ifdef __SWITCH_DMG_RENDER
	unsigned char damageRender;
#endif //__SWITCH_DMG_RENDER

#ifdef __SYS_CUSTOM_TITLE
	BOOL	m_bShowCustomTitle;
#endif // __SYS_CUSTOM_TITLE

#ifdef __PREMIUM
	BOOL	m_bShowPremiumTitle;
#endif // __PREMIUM

#ifdef 	__AEGON_FULLSCREEN_AND_RESIZE
	BOOL	m_bCenterTaskbar;
	BOOL	m_bWindowMaximized;
#endif // __AEGON_FULLSCREEN_AND_RESIZE

#ifdef __SINGLE_TASKBAR_SWAP
	BOOL	m_bTaskbarDisplay;
#endif // __SINGLE_TASKBAR_SWAP

#ifdef __NEWCUR
	int		m_nCurFold;
#endif // __NEWCUR

#ifdef __RENDER_SMALL_POWERUPS
	BOOL	m_nSmallPowerUps;
#endif //__RENDER_SMALL_POWERUPS

#ifdef __FLYFF_INITPAGE_EXT
	BOOL	m_nLoginVideo;
#endif //__FLYFF_INITPAGE_EXT

#ifdef __NAVIGATOR_V19
	BOOL	m_bV19Navigator;
#endif // __NAVIGATOR_V19

#ifdef __LEESE_MESSENGER_TIMER
	BOOL	m_bTimeStamp;
#endif // __LEESE_MESSENGER_TIMER

#ifdef __TRAILS
	BOOL	m_bHideTrails;
#endif //__TRAILS

#ifdef __BLOCK_GUILD_INVITES
	BOOL	m_bBlockGuildInvites;
#endif //__BLOCK_GUILD_INVITES

#ifdef __HEAL_SHOW
	BOOL	m_bHideHeal;
#endif //__HEAL_SHOW

#ifdef __AUTO_ATTACK_NEXT_TARGET
	BOOL	m_bAutoAttackNT;
#endif //__AUTO_ATTACK_NEXT_TARGET

#ifdef __LEESE_SAVE_STATUS
	int		m_nStatusSave;
#endif // __LEESE_SAVE_STATUS

#if defined(__AES) && defined(__CLIENT)
	static	AES m_crypt;
#endif

	BOOL	m_bDamageRender;
	BOOL	m_bVisibleBuffTimeRender;
	int		m_MouseSpeed;
	BOOL	m_bSFXRenderOff;	
	BOOL	m_bCameraLock;
	BOOL	m_bAutoAttack;
	int*	m_pGuide;
	
#if __VER >= 8 //__Y_GAMMA_CONTROL_8
	int     m_nOverBright;
	float	m_fGamma;
	float   m_fContrast;
#endif //__Y_GAMMA_CONTROL_8

#if __VER >= 8 //__CSC_VER8_3
	int m_BuffStatusMode;
#endif //__CSC_VER8_3

#if __VER >= 11 // __ADD_ZOOMOPT
	BOOL	m_bZoomLimit;		// 줌 제한 여부
#endif

#ifdef __AEGON_ADVANCED_LOGIN
	TCHAR	m_szFileNameLoginManager[MAX_PATH];
	vector<__LOGIN_ACCOUNT> m_vecLoginManager;
#endif // __AEGON_ADVANCED_LOGIN

#if defined __SYS_AUTO_SHOUT && defined __CLIENT
	CString	m_Shouts[MAX_AUTO_SHOUT];
	BOOL	m_CheckShouts[MAX_AUTO_SHOUT];

	void	LoadShout();
	void	SaveShout();
#endif // __SYS_AUTO_SHOUT

#ifdef __AEGON_INGAME_DP
	BOOL m_bSkipDPConfirmLootbox;
#endif // __AEGON_INGAME_DP

#ifdef __AEGON_THEME_SWITCHER
	int m_nSelectedThemeID;
#endif // __AEGON_THEME_SWITCHER

#ifdef __NEW_BUFF_FORMAT
	BOOL m_Buffs;
#endif // __NEW_BUFF_FORMAT

#ifdef __AEGON_MONSTER_HUNT
	BOOL m_bPrintMHProgress;
#endif

#ifdef __WBQT_GUILD_BUFFS
	BOOL m_bHideGuildBuffs;
#endif

#ifdef __AEGON_FIXES
	BOOL m_bStart32Bit;
#endif

	COption();
	~COption();
	
	void Init();
#ifdef __AEGON_ADVANCED_LOGIN
	int LoadLoginManager(LPCTSTR szFileName);
	void WriteLoginManager();
#endif // __AEGON_ADVANCED_LOGIN
#ifdef __AEGON_MESSENGER
	void LoadMessengerChats();
	void SaveMessengerChats();
#endif
	int Load( LPCTSTR szFileName );
	int Save( LPCTSTR szFileName );
#ifdef __CLIENT
	void saveBlockedUsers(CString playerName); //resave full file / remove user
	void saveBlockedUser(CString playerName, const std::set<std::string>::const_iterator& position); //append user
	void loadBlockedUsers(CString playerName);
#endif
#ifdef _DEBUG
	int LoadToolTip( LPCTSTR szFileName );
#endif // _DEBUG
	
#if __VER >= 9 // __CSC_VER9_RESOLUTION
	void CheckResolution();
#endif //__CSC_VER9_RESOLUTION
};

extern COption		g_Option;

#endif
