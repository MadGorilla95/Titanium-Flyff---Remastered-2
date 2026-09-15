#include "stdafx.h"
#include <stdio.h>

#ifndef __MINIMIZE_USE_OPTION
	#include "scanner.h"
	#include "lang.h"
#endif

#include "HwOption.h"

#if __VER >= 9 // __CSC_VER9_RESOLUTION
#include "D3D9.h"
#pragma comment	(lib, "d3d9.lib")
#endif //__CSC_VER9_RESOLUTION

#ifdef __AEGON_ADVANCED_LOGIN
#include <fstream>
#endif // __AEGON_ADVANCED_LOGIN


COption		g_Option;




#ifdef __MINIMIZE_USE_OPTION
	// chat style ( copy from \\_Interface\\WndManager.h )
	#define CHATSTY_GENERAL   0x0001
	#define CHATSTY_WHISPER   0x0002
	#define CHATSTY_SHOUT     0x0004
	#define CHATSTY_PARTY     0x0008
	#define CHATSTY_GUILD     0x0010

	#define CHATSTY_HELP          0x0060
	#define CHATSTY_HELP_CLIENT   0x0020
	#define CHATSTY_HELP_CHAT     0x0040
	#define CHATSTY_SYSTEM        0x0180
	#define CHATSTY_SYSTEM_CLIENT 0x0080
	#define CHATSTY_SYSTEM_CHAT   0x0100
	#define CHATSTY_GAME          0x0600
	#define CHATSTY_GAME_CLIENT   0x0200
	#define CHATSTY_GAME_CHAT     0x0400

	#define FINISHED	true
	#define Error		printf
	static const char g_seps[] = " ,\t";

	class CScannerEx
	{
	public:
		CScannerEx();
		~CScannerEx();

		BOOL	Load_FileIO( LPCTSTR szFilename );
		void	GetToken();
		void	GetTokenEx();
		int		GetNumber();
		float	GetFloat();

	public:
		CString	Token;
		bool	tok;

	private:
		char	m_szBuffer[256];
		char*	m_szToken;
		FILE*   m_pFILE;

	};

	CScannerEx::CScannerEx()
	{
		Token.Empty();
		tok = false;
		m_pFILE = NULL;
		m_szToken = NULL;
	}

	CScannerEx::~CScannerEx()
	{
		if( m_pFILE )
			fclose( m_pFILE );
	}

	BOOL CScannerEx::Load_FileIO( LPCTSTR szFilename )
	{
		FILE* fp = fopen( szFilename, "rt" );
		if( fp == NULL )
			return FALSE;
		else
		{
			m_pFILE = fp;
			return TRUE;
		}
	}
	void CScannerEx::GetToken()
	{
		if( fgets( m_szBuffer, sizeof( m_szBuffer ), m_pFILE ) )
		{
			m_szToken = strtok( m_szBuffer, g_seps );
			Token = m_szToken; 
		}
		else
		{
			tok = FINISHED;
			Token = "";
		}
	}

	void CScannerEx::GetTokenEx()
	{
		m_szToken = strtok( NULL, g_seps );
		if( m_szToken[0] == '"' )
		{
			Token = "";
			for( int i=1; m_szToken[i] != '"'; ++i )
				Token += m_szToken[i];
		}
		else
		{
			Token = m_szToken;
		}
	}
	
	int	 CScannerEx::GetNumber()
	{
		m_szToken = strtok( NULL, g_seps );
		if( m_szToken )
			return atoi( m_szToken );
		else
			return 0;
	}

	float CScannerEx::GetFloat()
	{
		m_szToken = strtok( NULL, g_seps );
		if( m_szToken )
			return static_cast< float >( atof( m_szToken ) );
		else
			return 0.0f;
	}
	int GetLanguage()
	{
		return __CURRENT_LANG;	// patchclient의 versioncommon.h에 정의됨 
	}
#else

	#define CScannerEx	CScanner
	#define GetTokenEx	GetToken

#endif

///////////////////////////////////////////////////////////////////
//  COption
///////////////////////////////////////////////////////////////////

#if defined(__AES) && defined(__CLIENT)
AES		COption::m_crypt;
#endif // __AES

COption::COption()
{
	Init();
}

COption::~COption()
{
}

void COption::Init()
{
	memset( m_szFileName, 0, sizeof(m_szFileName) );
	m_nWeatherEffect = TRUE;// FALSE;
	m_bOperator = FALSE;
	m_nResWidth = 1920; //1024;
	m_nResHeight = 1080; //768;
	m_bStartFullScreen = FALSE;		
	m_nTextureQuality = 1;
	m_nViewArea = 0; //1;
	m_nObjectDistant = 0; //1;
	m_nObjectDetail = 0; //1;
	m_nShadow = 0; //2;
	m_nWindowAlpha = 255; 
	m_nWindowEffect = 0;
	m_nSlangWord    = 0;
	m_nChatCommand  = 0;
	m_nInstantHelp  = 0; //1;
	m_nInterface	= 1;	// 디폴트를 신버전으로 해놓자.
	m_bRollEffect = 0;
	
	m_bSaveAccount = 0;
	m_szAccount[ 0 ] = 0;
	m_bNotice = TRUE;
	m_bTip = TRUE;
	m_tNoticeTime = 0;
	m_bTrade = TRUE;
	m_bSay = TRUE;
	m_bParty = TRUE;
	m_bShout = TRUE;
	m_bMessengerJoin = TRUE;
	m_bMessenger = TRUE;
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	m_bBattleBGM = FALSE;
#endif // __IMPROVE_SYSTEM_VER15

	m_nToggleScreen = 0;
	m_nSer = m_nMSer = 0;
	
	m_nBloom = 0;

	m_nMonName = 1;
	m_nToolTipTexture = 0;
	m_nToolTipText = 0;
	
	m_bVoice        = TRUE;
	m_fEffectVolume = 0.0f;// 1.0f;
	m_fBGMVolume = 0.0f;// 0.5f;
	m_nGuide1 = 2;// 0; //2=disable info pang
	m_nGuide2        = 0;
	m_nGuide3        = 0;
#if __VER >= 12 // __MOD_TUTORIAL
	m_nTutorialLv = 0;
#endif	
#if __VER >= 12 // __UPDATE_OPT
	m_bViewMask = TRUE;
#endif
	m_bIsShowReSkillMessage = TRUE;
	m_pGuide = NULL;

	m_dwChatFilter[0] = CHATSTY_GENERAL | CHATSTY_WHISPER | CHATSTY_SHOUT | CHATSTY_PARTY | CHATSTY_GUILD;
	m_dwChatFilter[1] = CHATSTY_GENERAL | CHATSTY_WHISPER ;
	m_dwChatFilter[2] = CHATSTY_GENERAL | CHATSTY_SHOUT ;
	m_dwChatFilter[3] = CHATSTY_GENERAL | CHATSTY_PARTY ;
	m_dwChatFilter[4] = CHATSTY_GENERAL | CHATSTY_GUILD;
	m_dwChatFilterEtc = CHATSTY_HELP_CHAT | CHATSTY_SYSTEM_CHAT | CHATSTY_GAME_CLIENT; 

	m_IPAddress[0] = NULL;
#ifdef __SFX_OPT
	m_nSfxLevel = 5;	
#endif

#ifdef __YENV
	m_bExitChange = FALSE;
	m_bSpecBump = TRUE;
#endif //__YENV

	m_bDamageRender = FALSE;
	
	m_bVisibleBuffTimeRender = TRUE;
	
	m_MouseSpeed = 1;
	m_bSFXRenderOff = FALSE;	

	m_bCameraLock = TRUE;			// 디폴트로 켜져있게 내가 고쳤슴데이~ -xuzhu-
	m_bAutoAttack = TRUE;
	m_nPlayerName		= 1;		    // 자신 이름 On / Off
	m_nOtherPlayerName	= 1;		    // 다른 플레이어 이름 On / Off

#if __VER >= 8 //__Y_GAMMA_CONTROL_8
	m_nOverBright = 10;
	m_fGamma = 1.0f;
	m_fContrast = 1.0f;
#endif //__Y_GAMMA_CONTROL_8

#if __VER >= 8 //__CSC_VER8_3
	m_BuffStatusMode = 0;
#endif //__CSC_VER8_3
	
#if __VER >= 11 // __ADD_ZOOMOPT
	m_bZoomLimit = FALSE;// TRUE;
#endif

#ifdef __AEGON_ADVANCED_LOGIN
	m_vecLoginManager.clear();
#endif // __AEGON_ADVANCED_LOGIN

#ifdef __SYS_PET_NO_CLICK
	petClickOption = 1;
#endif //__SYS_PET_NO_CLICK

#ifdef __NEW_NIGHT
	m_bNewNight = TRUE;//FALSE;
#endif //__NEW_NIGHT

#ifdef __SYS_GROUND_ITEM_TEXT
	groundItemText = TRUE;
#endif //__SYS_GROUND_ITEM_TEXT

#ifdef __MUTEBGSOUNDS //set to true if you want it defaultly saved in
	muteSoundsInBG = false;// true;
	muteMusicInBG = false;// true;
#endif // __MUTEBGSOUNDS

#ifdef __BACKGROUNDRENDER
	inactiveRender = 0;// 1;
#endif // __BACKGROUNDRENDER

#ifdef __ALIASING
	m_nAliasing = 1;// 0;
#endif // __ALIASING

#ifdef __ANISOTROPIC
	m_nAnisotropic = 1;// 0;
#endif // __ANISOTROPIC

#ifdef __AEGON_FONT_SWITCH
	m_strFont = "Verdana";
#endif // __AEGON_FONT_SWITCH

#ifdef __HIDE_NPC_BUBBLES
	m_nNPCBubbles = TRUE;// FALSE;
#endif //__HIDE_NPC_BUBBLES

#ifdef __HIDE_POWER_UPS
	m_nHidePU = FALSE;
#endif //__HIDE_POWER_UPS

#ifdef __HIDE_MONSTER_LEVEL
	m_nMonsterLevel = FALSE;
#endif //__HIDE_MONSTER_LEVEL

#ifdef __ALPHA_CHAT
	alphaChat = false;
#endif //__ALPHA_CHAT

#ifdef __STOP_EXP
	m_bExpStop = 0;
#endif //__STOP_EXP

#ifndef __AEGON_DB_PETFILTER
#ifdef __SYS_PETFILTER
	m_dwPetfilter = 0;
#endif //__SYS_PETFILTER
#endif

#ifdef __SWITCH_DMG_RENDER
	damageRender = 1;
#endif //__SWITCH_DMG_RENDER

#ifdef __SYS_CUSTOM_TITLE
	m_bShowCustomTitle = FALSE;
#endif // __SYS_CUSTOM_TITLE

#ifdef __PREMIUM
	m_bShowPremiumTitle = FALSE;
#endif // __PREMIUM

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
	m_bCenterTaskbar = FALSE;
	m_bWindowMaximized = FALSE;
#endif // __AEGON_FULLSCREEN_AND_RESIZE

#ifdef __SYS_AUTO_SHOUT
	for (size_t i = 0; i < MAX_AUTO_SHOUT; i++)
	{
		m_Shouts[i].Empty();
		m_CheckShouts[i] = false;
	}
#endif // __SYS_AUTO_SHOUT

#ifdef __SINGLE_TASKBAR_SWAP
	m_bTaskbarDisplay = FALSE;
#endif // __SINGLE_TASKBAR_SWAP

#ifdef __NEWCUR
	m_nCurFold = 0;
#endif //__NEWCUR

#ifdef __AEGON_INGAME_DP
	m_bSkipDPConfirmLootbox = FALSE;
#endif // __AEGON_INGAME_DP

#ifdef __AEGON_THEME_SWITCHER
	m_nSelectedThemeID = 2;//0;
#endif // __AEGON_THEME_SWITCHER

#ifdef __NEW_BUFF_FORMAT
	m_Buffs = FALSE;
#endif // __NEW_BUFF_FORMAT

#ifdef __RENDER_SMALL_POWERUPS
	m_nSmallPowerUps = FALSE;
#endif //__RENDER_SMALL_POWERUPS

#ifdef __FLYFF_INITPAGE_EXT
	m_nLoginVideo = FALSE;
#endif //__FLYFF_INITPAGE_EXT

#ifdef __NAVIGATOR_V19
	m_bV19Navigator = TRUE;// FALSE;
#endif // __NAVIGATOR_V19

#ifdef __LEESE_MESSENGER_TIMER
	m_bTimeStamp = TRUE;// FALSE;
#endif // __LEESE_MESSENGER_TIMER

#ifdef __TRAILS
	m_bHideTrails = FALSE;
#endif //__TRAILS

#ifdef __BLOCK_GUILD_INVITES
	m_bBlockGuildInvites = FALSE;
#endif //__BLOCK_GUILD_INVITES

#ifdef __HEAL_SHOW
	m_bHideHeal = TRUE;// FALSE;
#endif //__HEAL_SHOW

#ifdef __AUTO_ATTACK_NEXT_TARGET
	m_bAutoAttackNT = TRUE;
#endif //__AUTO_ATTACK_NEXT_TARGET

#ifdef __AEGON_MONSTER_HUNT
	m_bPrintMHProgress = FALSE;
#endif

#ifdef __LEESE_SAVE_STATUS
	m_nStatusSave = 2;
#endif //__LEESE_SAVE_STATUS

#ifdef __WBQT_GUILD_BUFFS
	m_bHideGuildBuffs = FALSE;
#endif

#ifdef __AEGON_FIXES
#ifdef _WIN64
	m_bStart32Bit = FALSE;
#else
	m_bStart32Bit = TRUE;
#endif
#endif

}

#ifdef __SYS_AUTO_SHOUT
void COption::SaveShout()
{
	FILE* fp = fopen(MakePath(DIR_CLIENT, "Shouts.ini"), "wt");
	if (fp == NULL)
		return;

	for (int i = NULL; i < MAX_AUTO_SHOUT; i++)
	{
		_ftprintf(fp, "\"%s\" %d\n", m_Shouts[i].GetString(), m_CheckShouts[i]);
	}
	fclose(fp);
}
void COption::LoadShout()
{
	CScript s;
	if (!s.Load(MakePath(DIR_CLIENT, "Shouts.ini")))
		return;

	for (int i = NULL; i < MAX_AUTO_SHOUT; i++)
	{
		s.GetTokenEx();
		m_Shouts[i] = s.Token;
		m_CheckShouts[i] = s.GetNumber();
	}
}
#endif // __SYS_AUTO_SHOUT

#if defined( _DEBUG) && !defined(__MINIMIZE_USE_OPTION)
int COption::LoadToolTip( LPCTSTR szFileName )
{
	_tcscpy( m_szFileName, szFileName);
	CScannerEx scan;
	if(scan.Load_FileIO( szFileName ) == FALSE )
		return 0;
	
	m_nToolTipText = 2;
	int nR = 0;
	int nG = 0;
	int nB = 0;

	do
	{
		nR = 0;
		nG = 0;
		nB = 0;
		scan.GetToken();
		if( scan.Token == _T( "아이템이름0" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwName0 = D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "아이템이름1" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwName1 = D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "아이템이름2" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwName2 = D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "아이템이름3" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwName3 = D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "아이템이름4" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwName4 = D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "아이템이름5" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwName5 = D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
#ifdef __PREMIUM
		else if (scan.Token == _T("아이템이름6"))
		{
			nR = scan.GetNumber();
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwName6 = D3DCOLOR_ARGB(255, nR, nG, nB);
		}
#endif //__PREMIUM
#ifdef __ANARCHY_SYSTEM
		else if (scan.Token == _T("아이템이름7"))
		{
			nR = scan.GetNumber();
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwName7 = D3DCOLOR_ARGB(255, nR, nG, nB);
		}
#endif //__ANARCHY_SYSTEM
		else if( scan.Token == _T( "일반" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwGeneral = D3DCOLOR_ARGB( 255, nR, nG, nB );		// 일반	
		}
		else if( scan.Token == _T( "플러스옵션" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwPlusOption = D3DCOLOR_ARGB( 255, nR, nG, nB );	// 플러스 옵션
		}
		else if( scan.Token == _T( "피어싱" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwPiercing = D3DCOLOR_ARGB( 255, nR, nG, nB );	// 피어싱
		}
		else if( scan.Token == _T( "속성일반" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResist = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 일반
		}
		else if( scan.Token == _T( "속성불" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistFire = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 불
		}
		else if( scan.Token == _T( "속성물" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistWater = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 물
		}
		else if( scan.Token == _T( "속성땅" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistEarth = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 땅
		}
		else if( scan.Token == _T( "속성전기" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistElectricity = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 전기
		}
		else if( scan.Token == _T( "속성바람" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistWind = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 바람
		}
		else if( scan.Token == _T( "속성상용화무기" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistSM = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 상용화 무기
		}
		else if( scan.Token == _T( "속성상용화방어구" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistSM1 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 상용화 방어구
		}
		else if( scan.Token == _T( "시간" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwTime = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 시간 및 효과
		}
		else if( scan.Token == _T( "효능0" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwEffective0 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 아이템 효능
		}
		else if( scan.Token == _T( "효능1" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwEffective1 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 아이템 효능1
		}
		else if( scan.Token == _T( "효능2" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwEffective2 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 아이템 효능2
		}
		else if( scan.Token == _T( "효능3" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwEffective3 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 아이템 효능3
		}
		else if( scan.Token == _T( "랜덤옵션" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwRandomOption = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 랜덤 옵션
		}
		else if( scan.Token == _T( "인첸트옵션" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwEnchantOption = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 인첸트 옵션
		}
		else if( scan.Token == _T( "세트이름" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwSetName = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 세트 이름
		}
		else if( scan.Token == _T( "세트목록비착용" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwSetItem0 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 세트 목록(비착용시)
		}
		else if( scan.Token == _T( "세트목록착용" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwSetItem1 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 세트 목록(착용시)
		}
		else if( scan.Token == _T( "세트효과" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwSetEffect = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 세트 효과
		}
		else if( scan.Token == _T( "가격" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwGold = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 가격
		}
		else if( scan.Token == _T( "설명" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwCommand = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 설명
		}
		else if( scan.Token == _T( "사용못함" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwNotUse = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 사용못함	
		}
#if __VER >= 9 // __CSC_VER9_1
		else if( scan.Token == _T( "무기추가옵션1" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAddedOpt1 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 무기추가옵션1				
		}
		else if( scan.Token == _T( "무기추가옵션2" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAddedOpt2 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 무기추가옵션2
		}
		else if( scan.Token == _T( "무기추가옵션3" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAddedOpt3 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 무기추가옵션3
		}
		else if( scan.Token == _T( "무기추가옵션4" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAddedOpt4 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 무기추가옵션4
		}
		else if( scan.Token == _T( "무기추가옵션5" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAddedOpt5 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 무기추가옵션5
		}
		else if( scan.Token == _T( "무기추가옵션6" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAddedOpt6 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 무기추가옵션6
		}
		else if( scan.Token == _T( "무기추가옵션7" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAddedOpt7 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 무기추가옵션7
		}
		else if( scan.Token == _T( "무기추가옵션8" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAddedOpt8 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 무기추가옵션8
		}
		else if( scan.Token == _T( "무기추가옵션9" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAddedOpt9 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 무기추가옵션9
		}
#ifdef __NEW_DIAMONDS
		else if (scan.Token == _T("무기추가옵션10"))
		{
			nR = scan.GetNumber();
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAddedOpt10 = D3DCOLOR_ARGB(255, nR, nG, nB);
		}
#endif //__NEW_DIAMONDS
#endif //__CSC_VER9_1
#if __VER >= 11 // __SYS_IDENTIFY
		else if( scan.Token == _T( "아이템각성" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAwakening	= D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "여신의 축복" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwBlessing	= D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "여신의 축복 경고" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwBlessingWarning	= D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
#endif	// __SYS_IDENTIFY
	} while( scan.tok != FINISHED );

	return 1;
}
#endif // _DEBUG

#if __VER >= 9 // __CSC_VER9_RESOLUTION
void COption::CheckResolution()
{
	IDirect3D9* pD3D9;
	BOOL bResolution = FALSE;
	
	pD3D9 = Direct3DCreate9( D3D_SDK_VERSION );
	//UINT numAdapters = pD3D9->GetAdapterCount();
	
	D3DFORMAT allowedAdapterFormat = D3DFMT_X8R8G8B8;
	UINT numAdapterModes = pD3D9->GetAdapterModeCount( 0, allowedAdapterFormat );
	for (UINT mode = 0; mode < numAdapterModes; mode++)
	{
		D3DDISPLAYMODE displayMode;
		pD3D9->EnumAdapterModes( 0, allowedAdapterFormat, mode, &displayMode );
		if( displayMode.Width == m_nResWidth && displayMode.Height == m_nResHeight )
			bResolution = TRUE;
	}
	

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
	HMONITOR monitor = MonitorFromWindow(g_pD3dApp->m_hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFOEX info;
	info.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(monitor, &info);
	int monitor_width = info.rcMonitor.right - info.rcMonitor.left;
	int monitor_height = info.rcMonitor.bottom - info.rcMonitor.top;
	if (m_nResWidth > monitor_width)
		m_nResWidth = monitor_width;
	if (m_nResHeight > monitor_height)
		m_nResHeight = monitor_height;

	if (m_nResWidth < 100)
		m_nResWidth = 100;
	if (m_nResHeight < 100)
		m_nResHeight = 100;
#else // __AEGON_FULLSCREEN_AND_RESIZE
	if(!bResolution && m_bStartFullScreen) //미지원 해상도의 경우 800x600으로 고정
	{

		Error( "We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight );
		m_nResWidth = 800;
		m_nResHeight = 600;
	}
	else //그래픽카드가 지원하는 해상도 중 사용가능한 해상도 선별
	{
		switch(m_nResWidth)
		{
		case 800: //800x600
			if(m_nResHeight != 600)
			{
				Error( "We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight );
				m_nResHeight = 600;
			}
			break;
		case 1024: //1024x768
			if(m_nResHeight != 768)
			{
				Error( "We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight );
				m_nResHeight = 768;
			}
			break;
		case 1280: //1280x1024 or 1280x720 (WIDE) or 1280x768 (WIDE) or 1280x800 (WIDE)
			if(m_nResHeight != 1024 && m_nResHeight != 720 && m_nResHeight != 768 && m_nResHeight != 800)
			{
				Error( "We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight );
				m_nResWidth = 800;
				m_nResHeight = 600;				
			}
			break;
		case 1600: //1600x1200
			if(m_nResHeight != 1200)
			{
				Error( "We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight );
				m_nResHeight = 1200;
			}
			break;
		case 1360: //1360x768 (WIDE)
			if(m_nResHeight != 768)
			{
				Error( "We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight );
				m_nResHeight = 768;
			}
			break;
		case 1400: //1400x1050
			if(m_nResHeight != 1050)
			{
				Error( "We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight );
				m_nResHeight = 1050;
			}
			break;
		case 1440: //1440x900 (WIDE)
			if(m_nResHeight != 900)
			{
				Error( "We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight );
				m_nResHeight = 900;
			}
			break;
		case 1680: //1680x1050 (WIDE)
			if(m_nResHeight != 1050)
			{
				Error( "We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight );
				m_nResHeight = 1050;
			}
			break;
		case 1920: // 1920x1080 (WIDE)
			if (m_nResHeight != 1080)
			{
				Error("We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight);
				m_nResHeight = 1080;
			}
			break;
#ifdef __AEGON_2K_RES
		case 2560: // 2500x1400 (WIDE)
			if (m_nResHeight != 1400)
			{
				Error("We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight);
				m_nResHeight = 1400;
			}
			break;
#endif // __AEGON_2K_RES
		default:
			Error( "We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight );
			m_nResWidth = 800;
			m_nResHeight = 600;
			break;
		}

	}
	//SAFE_RELEASE( pD3D9 );
	pD3D9->Release();
#endif // __AEGON_FULLSCREEN_AND_RESIZE
}

#endif//__CSC_VER9_RESOLUTION


#ifdef __AEGON_ADVANCED_LOGIN
int COption::LoadLoginManager(LPCTSTR szFileName)
{
	_tcscpy(m_szFileNameLoginManager, szFileName);

	m_vecLoginManager.clear();

	unsigned char szKey[24] = __AEGON_LOGIN_CRYPT;
	m_crypt.SetParameters(192);
	m_crypt.StartDecryption(szKey);

	CFile file;
	if (file.Open(m_szFileNameLoginManager, CFile::modeRead) == FALSE)
		return -1;

	char szCountCrypted[32];
	memset(szCountCrypted, 0, sizeof(char) * 32);

	char szCountUncrypted[32];
	memset(szCountUncrypted, 0, sizeof(char) * 32);

	file.Read(szCountCrypted, sizeof(char) * 16);

	m_crypt.Decrypt(reinterpret_cast<const unsigned char*>(szCountCrypted), reinterpret_cast<unsigned char*>(szCountUncrypted), sizeof(char));

	int nCount = 0;
	memcpy((char*)&nCount, szCountUncrypted, sizeof(int));

	__LOGIN_ACCOUNT tmpLoginData;
	char szCrypted[1024];

	if (nCount < 0 || nCount > 100)
		return -1;

	for(int i = 0; i < nCount; i++)
	{
		memset(&tmpLoginData, 0, sizeof(__LOGIN_ACCOUNT));
		memset(szCrypted, 0, sizeof(char) * 1024);
		file.Read(szCrypted, sizeof(__LOGIN_ACCOUNT));

		m_crypt.Decrypt(reinterpret_cast<const unsigned char*>(szCrypted), reinterpret_cast<unsigned char*>(tmpLoginData.szAccount), sizeof(__LOGIN_ACCOUNT::szAccount) / 16);
		m_crypt.Decrypt(reinterpret_cast<const unsigned char*>(szCrypted + sizeof(__LOGIN_ACCOUNT::szAccount)), reinterpret_cast<unsigned char*>(tmpLoginData.szPassword), sizeof(__LOGIN_ACCOUNT::szPassword) / 16);

		m_vecLoginManager.push_back(tmpLoginData);
	}
	file.Close();
}

void COption::WriteLoginManager()
{

	unsigned char szKey[24] = __AEGON_LOGIN_CRYPT;
	m_crypt.SetParameters(192);
	m_crypt.StartEncryption(szKey);

	size_t bufSize = sizeof(char) * 16 + m_vecLoginManager.size() * sizeof(__LOGIN_ACCOUNT);
	char* szBuf = (char*)malloc(bufSize);

	if (!szBuf)
		return;

	memset(szBuf, 0, bufSize);

	int nIndex = 0;
	size_t nBlocks = 0;

	int nCount = m_vecLoginManager.size();
	char szCount[32];
	memset(szCount, 0, sizeof(char) * 32);

	memcpy(szCount, (char*)&nCount, sizeof(int));

	memcpy(&szBuf[nIndex], szCount, sizeof(char) * 16);
	nIndex += sizeof(char) * 16;
	nBlocks += sizeof(char);

	for (int i = 0; i < nCount; i++)
	{
		memcpy(&szBuf[nIndex], m_vecLoginManager[i].szAccount, sizeof(__LOGIN_ACCOUNT::szAccount));
		nIndex += sizeof(__LOGIN_ACCOUNT::szAccount);
		memcpy(&szBuf[nIndex], m_vecLoginManager[i].szPassword, sizeof(__LOGIN_ACCOUNT::szPassword));
		nIndex += sizeof(__LOGIN_ACCOUNT::szPassword);
	}

	nBlocks += (nCount * sizeof(__LOGIN_ACCOUNT)) / 16;
	size_t sizeEncrypted = 16 * nBlocks;
	char* szBufEncrypted = (char*)malloc(sizeEncrypted);
	if (!szBufEncrypted) 
	{
		std::free(szBuf);
		return;
	}

	memset(szBufEncrypted, 0, sizeEncrypted);

	m_crypt.Encrypt(reinterpret_cast<const unsigned char*>(szBuf), reinterpret_cast<unsigned char*>(szBufEncrypted), nBlocks);

	CFile file;
	if (file.Open(m_szFileNameLoginManager, CFile::modeWrite | CFile::modeCreate) != FALSE) 
	{
		file.Write(szBufEncrypted, sizeEncrypted);
		file.Close();
	}	

	std::free(szBuf);
	std::free(szBufEncrypted);
}
#endif //__AEGON_ADVANCED_LOGIN
#ifdef __AEGON_MESSENGER
void COption::LoadMessengerChats()
{
	CAegonMessengerMng::GetInstance()->ClearAll();

	if (IsInvalidObj(g_pPlayer))
		return;

	vector<__AEGON_CHAT_UNIT>* pAllChats = CAegonMessengerMng::GetInstance()->GetAllChats();
	if (!pAllChats)
		return;

	CString strFile, strFileName;
	strFileName.Format("chat_history_%s.tmmo", g_pPlayer->GetName());
	strFile = MakePath(DIR_MESSENGER_LOG, strFileName.GetString());

	unsigned char szKey[24] = __AEGON_LOGIN_CRYPT;
	m_crypt.SetParameters(192);
	m_crypt.StartDecryption(szKey);



	CFile file;
	if (file.Open(strFile.GetString(), CFile::modeRead) == FALSE)
		return;

	char szCountCrypted[32];
	memset(szCountCrypted, 0, sizeof(char) * 32);

	char szCountUncrypted[32];
	memset(szCountUncrypted, 0, sizeof(char) * 32);

	file.Read(szCountCrypted, sizeof(char) * 16);

	m_crypt.Decrypt(reinterpret_cast<const unsigned char*>(szCountCrypted), reinterpret_cast<unsigned char*>(szCountUncrypted), sizeof(char));

	int nCount = 0;
	memcpy((char*)&nCount, szCountUncrypted, sizeof(int));

	__AEGON_CHAT_UNIT tmpChatUnit;

	for (int i = 0; i < nCount; i++)
	{
		tmpChatUnit.Init();

		//visible
		BOOL bVisible = 0;
		memset(szCountCrypted, 0, sizeof(char) * 32);
		memset(szCountUncrypted, 0, sizeof(char) * 32);
		file.Read(szCountCrypted, sizeof(char) * 16);
		m_crypt.Decrypt(reinterpret_cast<const unsigned char*>(szCountCrypted), reinterpret_cast<unsigned char*>(szCountUncrypted), sizeof(char));
		memcpy((char*)&bVisible, szCountUncrypted, sizeof(BOOL));
		tmpChatUnit.m_bVisible = bVisible;

		//name
		char szNameCrypted[64];
		char szNameUncrypted[64];

		memset(szNameCrypted, 0, sizeof(char) * 64);
		memset(szNameUncrypted, 0, sizeof(char) * 64);

		file.Read(szNameCrypted, sizeof(char) * 64);
		m_crypt.Decrypt(reinterpret_cast<const unsigned char*>(szNameCrypted), reinterpret_cast<unsigned char*>(szNameUncrypted), (sizeof(char) * 64) / 16);
		tmpChatUnit.m_strName = szNameUncrypted;

		//length last msg
		int nLengthLastMsg = 0;
		memset(szCountCrypted, 0, sizeof(char) * 32);
		memset(szCountUncrypted, 0, sizeof(char) * 32);
		file.Read(szCountCrypted, sizeof(char) * 16);
		m_crypt.Decrypt(reinterpret_cast<const unsigned char*>(szCountCrypted), reinterpret_cast<unsigned char*>(szCountUncrypted), sizeof(char));
		memcpy((char*)&nLengthLastMsg, szCountUncrypted, sizeof(int));

		//last msg
		if (nLengthLastMsg > 0) 
		{
			char* szLastMsgCrypted = (char*)malloc(nLengthLastMsg);
			char* szLastMsgUncrypted = (char*)malloc(nLengthLastMsg);
			file.Read(szLastMsgCrypted, sizeof(char) * nLengthLastMsg);
			m_crypt.Decrypt(reinterpret_cast<const unsigned char*>(szLastMsgCrypted), reinterpret_cast<unsigned char*>(szLastMsgUncrypted), (sizeof(char) * nLengthLastMsg) / 16);
			tmpChatUnit.m_strLastMessage = szLastMsgUncrypted;
			std::free(szLastMsgCrypted);
			std::free(szLastMsgUncrypted);
		}
		

		//length edit str
		int nLengthEditStr = 0;
		memset(szCountCrypted, 0, sizeof(char) * 32);
		memset(szCountUncrypted, 0, sizeof(char) * 32);
		file.Read(szCountCrypted, sizeof(char) * 16);
		m_crypt.Decrypt(reinterpret_cast<const unsigned char*>(szCountCrypted), reinterpret_cast<unsigned char*>(szCountUncrypted), sizeof(char));
		memcpy((char*)&nLengthEditStr, szCountUncrypted, sizeof(int));

		//edit str
		if (nLengthEditStr > 0) 
		{
			char* szEditStrCrypted = (char*)malloc(nLengthEditStr);
			char* szEditStrUncrypted = (char*)malloc(nLengthEditStr);
			file.Read(szEditStrCrypted, sizeof(char) * nLengthEditStr);
			m_crypt.Decrypt(reinterpret_cast<const unsigned char*>(szEditStrCrypted), reinterpret_cast<unsigned char*>(szEditStrUncrypted), (sizeof(char) * nLengthEditStr) / 16);
			tmpChatUnit.m_strEditString.SetParsingString(szEditStrUncrypted, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
			std::free(szEditStrCrypted);
			std::free(szEditStrUncrypted);
		}
		
		pAllChats->push_back(tmpChatUnit);
	}
	CAegonMessengerMng::GetInstance()->UpdateOpenChats();
	file.Close();
}
void COption::SaveMessengerChats()
{
	if (IsInvalidObj(g_pPlayer))
		return;

	vector<__AEGON_CHAT_UNIT>* pAllChats = CAegonMessengerMng::GetInstance()->GetAllChats();

	char strWorkingDirectory[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, strWorkingDirectory);
	//GetModuleFileName(NULL, strWorkingDirectory, MAX_PATH);
	CString strFullPath;
	strFullPath.Format("%s\\%s", strWorkingDirectory, DIR_MESSENGER_LOG);


	if(!SHCreateDirectoryEx(NULL, strFullPath.GetString(), NULL) && ERROR_FILE_EXISTS != GetLastError())
		return;

	CString strFile, strFileName;
	strFileName.Format("chat_history_%s.tmmo", g_pPlayer->GetName());
	strFile = MakePath(DIR_MESSENGER_LOG, strFileName.GetString());

	size_t totalEditStringSize = 0;
	size_t totalLastMsgSize = 0;
	int nCount = pAllChats->size();
	if (nCount <= 0)
		return;
	for (int i = 0; i < nCount; i++)
	{
		//length last msg
		int nLength = (*pAllChats)[i].m_strLastMessage.GetLength();
		int nMod = nLength % 16;
		if(nMod > 0)
			nLength += (16 - (nMod));
		totalLastMsgSize += nLength;

		//length edit str
		CString strEditFormat;
		(*pAllChats)[i].m_strEditString.GetTextFormat(strEditFormat);
		nLength = strEditFormat.GetLength();
		nMod = nLength % 16;
		if(nMod > 0)
			nLength += (16 - (nMod));
		totalEditStringSize += nLength;
	}

	unsigned char szKey[24] = __AEGON_LOGIN_CRYPT;
	m_crypt.SetParameters(192);
	m_crypt.StartEncryption(szKey);

	
	//1 + nCount*3 vector length, last msg length, edit str length, visible  //64 char for name //editstr //last msg
	size_t bufSize = (sizeof(char) * 16) * (1 + (size_t)nCount*3) + ((sizeof(char) * 64) * (size_t)nCount) + totalEditStringSize + totalLastMsgSize;

	char* szBuf = (char*)malloc(bufSize);

	if (!szBuf)
		return;

	memset(szBuf, 0, bufSize);

	int nIndex = 0;
	size_t nBlocks = 0;

	char szCount[32];
	memset(szCount, 0, sizeof(char) * 32);

	memcpy(szCount, (char*)&nCount, sizeof(int));

	memcpy(&szBuf[nIndex], szCount, sizeof(char) * 16);
	nIndex += sizeof(char) * 16;
	nBlocks += sizeof(char);

	CString strEditFormat;
	for (int i = 0; i < nCount; i++)
	{
		//visible
		BOOL bVisible = (*pAllChats)[i].m_bVisible;

		memset(szCount, 0, 32);
		memcpy(szCount, (char*)&bVisible, sizeof(BOOL));
		memcpy(&szBuf[nIndex], szCount, sizeof(char) * 16);
		nIndex += sizeof(char) * 16;
		nBlocks += sizeof(char);

		//name
		memcpy(&szBuf[nIndex], (*pAllChats)[i].m_strName.GetString(), sizeof(char) * (*pAllChats)[i].m_strName.GetLength());
		nIndex += (sizeof(char) * 64);
		nBlocks += (sizeof(char) * 64) / 16;

		//length last msg
		int nLength = (*pAllChats)[i].m_strLastMessage.GetLength();
		int nMod = nLength % 16;
		if (nMod > 0)
			nLength += (16 - (nMod));

		memset(szCount, 0, 32);
		memcpy(szCount, (char*)&nLength, sizeof(int));
		memcpy(&szBuf[nIndex], szCount, sizeof(char) * 16);
		nIndex += sizeof(char) * 16;
		nBlocks += sizeof(char);

		//last msg
		if (nLength > 0) 
		{
			memcpy(&szBuf[nIndex], (*pAllChats)[i].m_strLastMessage, sizeof(char) * nLength);
			nIndex += (sizeof(char) * nLength);
			nBlocks += (sizeof(char) * nLength) / 16;
		}

		//length edit string
		(*pAllChats)[i].m_strEditString.GetTextFormat(strEditFormat);
		nLength = strEditFormat.GetLength();
		nMod = nLength % 16;
		if(nMod > 0)
			nLength += (16 - (nMod));

		memset(szCount, 0, 32);
		memcpy(szCount, (char*)&nLength, sizeof(int));
		memcpy(&szBuf[nIndex], szCount, sizeof(char) * 16);
		nIndex += sizeof(char) * 16;
		nBlocks += sizeof(char);

		//edit string
		if (nLength > 0)
		{
			memcpy(&szBuf[nIndex], strEditFormat.GetString(), sizeof(char) * strEditFormat.GetLength());
			nIndex += (sizeof(char) * nLength);
			nBlocks += (sizeof(char) * nLength) / 16;
		}
	}

	size_t sizeEncrypted = 16 * nBlocks;
	char* szBufEncrypted = (char*)malloc(sizeEncrypted);
	if (!szBufEncrypted)
	{
		std::free(szBuf);
		return;
	}

	memset(szBufEncrypted, 0, sizeEncrypted);

	m_crypt.Encrypt(reinterpret_cast<const unsigned char*>(szBuf), reinterpret_cast<unsigned char*>(szBufEncrypted), nBlocks);

	CFile file;
	if (file.Open(strFile.GetString(), CFile::modeWrite | CFile::modeCreate) != FALSE)
	{
		file.Write(szBufEncrypted, sizeEncrypted);
		file.Close();
	}

	std::free(szBuf);
	std::free(szBufEncrypted);
}
#endif

int COption::Load( LPCTSTR szFileName )
{
#ifdef __SYS_AUTO_SHOUT
	LoadShout();
#endif // __SYS_AUTO_SHOUT

	_tcscpy( m_szFileName, szFileName);

	CScannerEx scan;
	if(scan.Load_FileIO( szFileName ) == FALSE )
		return 0;

	do {
		scan.GetToken();
		if( scan.Token == _T( "resolution" ) )
		{
			m_nResWidth = scan.GetNumber();
			m_nResHeight = scan.GetNumber();

			g_WndMng.resizeWidth = m_nResWidth;
			g_WndMng.resizeHeight = m_nResHeight;
#if __VER < 9 // __CSC_VER9_RESOLUTION
			if( m_nResWidth != 800 && m_nResWidth != 1024 && m_nResWidth != 1280 )
			{
				Error( "지원하지않는 해상도 발견!! - Width : %d", m_nResWidth );
				m_nResWidth = 800;
			}

			if( m_nResHeight != 600 && m_nResHeight != 768 && m_nResHeight != 1024 )
			{
				Error( "지원하지않는 해상도 발견!! - Height : %d", m_nResHeight );
				m_nResHeight = 600;
			}
#endif //__CSC_VER9_RESOLUTION	
		}
		else
		if( scan.Token == _T( "fullscreen" ) )
			m_bStartFullScreen = scan.GetNumber();
		else
		if( scan.Token == _T( "texture" ) )
			m_nTextureQuality = scan.GetNumber();
		else
		if( scan.Token == _T( "view" ) )
		{
			m_nViewArea = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "distant" ) )
		{
			m_nObjectDistant = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "shadow" ) )
		{
			m_nShadow = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "bloom" ) )
		{
			m_nBloom = scan.GetNumber();
			m_nBloom	= 0;	// 2008/04/25
		}
		else
		if( scan.Token == _T( "detail" ) )
			m_nObjectDetail = scan.GetNumber();
		else
		if( scan.Token == _T( "hotkey" ) ) 
		{
		}
		else
		if( scan.Token == _T( "WindowAlpha" ) ) 
		{
			m_nWindowAlpha = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "SlangWord" ) ) 
		{
			m_nSlangWord = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "ChatCommand" ) ) 
		{
			m_nChatCommand = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "InstantHelp" ) ) 
		{
			m_nInstantHelp = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Interface" ) ) 
		{
			m_nInterface = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "RollEffect" ) ) 
		{
			m_bRollEffect = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "account" ) ) 
		{
			scan.GetTokenEx();
			strcpy( m_szAccount, scan.Token );
		}
		else
		if( scan.Token == _T( "saveAccount" ) ) 
		{
			m_bSaveAccount = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "notice" ) ) 
		{
			m_bNotice = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "noticeTime" ) ) 
		{
			m_tNoticeTime = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "tip" ) ) 
		{
			m_bTip = scan.GetNumber();
		}				
		else
		if( scan.Token == _T( "Trade") )
		{
			m_bTrade = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Say") )
		{
			m_bSay = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Party") )
		{
			m_bParty = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "sunkist" ) )
		{
			m_bOperator = TRUE;
			m_nToggleScreen = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Shout" ) )
		{
			m_bShout = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "MessengerJoin" ) )
		{
			m_bMessengerJoin = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Messenger") )
		{
			m_bMessenger = scan.GetNumber();
		}
		else
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
		if( scan.Token == _T( "BattleBGM") )
		{
			m_bBattleBGM = scan.GetNumber();
		}
		else
#endif // __IMPROVE_SYSTEM_VER15

		if( scan.Token == _T( "Server" ) )
		{
			m_nSer = scan.GetNumber();
			m_nMSer = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Sound" ) )
		{
			m_bVoice        = scan.GetNumber();
			m_fEffectVolume = scan.GetFloat();
			m_fBGMVolume    = scan.GetFloat();
		}
		else
		if( scan.Token == _T( "Guide" ) )
		{
			m_nGuide1        = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Guide2" ) )
		{
			m_nGuide2        = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Guide3" ) )
		{
			m_nGuide3        = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "SkillShowMessage" ) )
		{
			m_bIsShowReSkillMessage = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "ChatFilter0" ) )
		{
			m_dwChatFilter[0] = scan.GetNumber();
			m_dwChatFilter[0] &= ~( CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME );
		}
		else
		if( scan.Token == _T( "ChatFilter1" ) )
		{
			m_dwChatFilter[1] = scan.GetNumber();
			m_dwChatFilter[1] &= ~( CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME );
		}
		else
		if( scan.Token == _T( "ChatFilter2" ) )
		{
			m_dwChatFilter[2] = scan.GetNumber();
			m_dwChatFilter[2] &= ~( CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME );
		}
		else
		if( scan.Token == _T( "ChatFilter3" ) )
		{
			m_dwChatFilter[3] = scan.GetNumber();
			m_dwChatFilter[3] &= ~( CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME );
		}
		else
		if( scan.Token == _T( "ChatFilter4" ) )
		{
			m_dwChatFilter[4] = scan.GetNumber();
			m_dwChatFilter[4] &= ~( CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME );
		}
		else
		if( scan.Token == _T( "ChatFilterEtc" ) )
		{
#if __VER >= 8 //__Y_CHAT_SYSTEM_8
			m_dwChatFilterEtc = CHATSTY_HELP_CHAT | CHATSTY_SYSTEM_CHAT | CHATSTY_GAME_CLIENT;
#else //__Y_CHAT_SYSTEM_8
			m_dwChatFilterEtc = scan.GetNumber();
#endif //__Y_CHAT_SYSTEM_8
		}
#ifdef __YENV
		else
		if( scan.Token == _T( "SpecBumpEffect" ) )
		{
			m_bSpecBump = scan.GetNumber();
		}
#endif //__YENV
		else
		if( scan.Token == _T( "DamageRender" ) )
		{
			m_bDamageRender = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "BuffTimeRender" ) )
		{
			m_bVisibleBuffTimeRender = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "MouseSpeed" ) )
		{
			m_MouseSpeed = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "SFXRenderOff" ) )
		{
			m_bSFXRenderOff = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "CameraLock" ) )
		{
			m_bCameraLock = scan.GetNumber();
		}	
		else
		if( scan.Token == _T( "AutoAttack" ) )
		{
			m_bAutoAttack = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "PRenderName" ) )
		{
			m_nPlayerName = scan.GetNumber();
		}	
		else
		if( scan.Token == _T( "OPRenderName" ) )
		{
			m_nOtherPlayerName = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "NRenderName" ) )
		{
			m_nMonName = scan.GetNumber();
		}
#if __VER >= 8 //__Y_GAMMA_CONTROL_8
		else
		if( scan.Token == _T( "BRIGHT" ) )
		{
			m_nOverBright = scan.GetNumber();
		}	
		else
		if( scan.Token == _T( "GAMMA" ) )
		{
			m_fGamma = scan.GetFloat();
		}
		else
		if( scan.Token == _T( "CONTRAST" ) )
		{
			m_fContrast = scan.GetFloat();
		}
#endif //__Y_GAMMA_CONTROL_8

#if __VER >= 8 //__CSC_VER8_3
		else if(scan.Token == _T( "BUFFSTATUSMODE" ))
		{
			m_BuffStatusMode = scan.GetNumber();
		}
#endif //__CSC_VER8_3
		
#if __VER >= 11 // __ADD_ZOOMOPT
	else if(scan.Token == _T( "ZOOMLIMIT" ))
	{
		m_bZoomLimit = scan.GetNumber();
	}
#endif
#ifdef __SFX_OPT
	else if(scan.Token == _T( "SFXLEVEL" ))
	{
		m_nSfxLevel = scan.GetNumber();
	}
#endif
	else if(scan.Token == _T( "WEATHEREFFECT" ))
	{
		m_nWeatherEffect = scan.GetNumber();
	}

#if __VER >= 12 // __UPDATE_OPT
	else if(scan.Token == _T( "VIEWMASK" ))
	{
		m_bViewMask = scan.GetNumber();
	}
#endif //__UPDATE_OPT

#ifdef __SYS_PET_NO_CLICK
	else if (scan.Token == _T("petClickOption"))
	{
		petClickOption = scan.GetNumber();
	}
#endif //__SYS_PET_NO_CLICK

#ifdef __NEW_NIGHT			
	else if (scan.Token == _T("NewNight"))
	{
		m_bNewNight = scan.GetNumber();
	}
#endif // __NEW_NIGHT

#ifdef __SYS_GROUND_ITEM_TEXT			
	else if (scan.Token == _T("GroundItem"))
	{
		groundItemText = scan.GetNumber();
	}
#endif // __SYS_GROUND_ITEM_TEXT

#ifdef __MUTEBGSOUNDS
	else if (scan.Token == _T("muteSoundsInBG"))
			muteSoundsInBG = scan.GetNumber();
	else if (scan.Token == _T("muteMusicInBG"))
			muteMusicInBG = scan.GetNumber();
#endif // __MUTEBGSOUNDS

#ifdef __BACKGROUNDRENDER
	else if (scan.Token == _T("inactiveRender"))
			inactiveRender = scan.GetNumber();
#endif // __BACKGROUNDRENDER

#ifdef __ALIASING
	else if (scan.Token == _T("Antialiasing"))
	{
		m_nAliasing = scan.GetNumber();
	}
#endif // __ALIASING

#ifdef __ANISOTROPIC
	else if (scan.Token == _T("Anisotropic"))
	{
		m_nAnisotropic = scan.GetNumber();
	}
#endif // __ANISOTROPIC

#ifdef __AEGON_FONT_SWITCH
	else if (scan.Token == _T("Font"))
	{
		scan.GetToken();
		m_strFont.Format("%s", scan.Token.GetString());
		m_strFont.Replace("_", " ");
	}
#endif // __AEGON_FONT_SWITCH

#ifdef __HIDE_NPC_BUBBLES			
	else if (scan.Token == _T("Bubbles"))
	{
		m_nNPCBubbles = scan.GetNumber();
	}
#endif // __HIDE_NPC_BUBBLES

#ifdef __HIDE_POWER_UPS
	else if (scan.Token == _T("HidePU"))
	{
		m_nHidePU = scan.GetNumber();
	}
#endif //__HIDE_POWER_UPS

#ifdef __HIDE_MONSTER_LEVEL			
	else if (scan.Token == _T("MonsterLv"))
	{
		m_nMonsterLevel = scan.GetNumber();
	}
#endif // __HIDE_MONSTER_LEVEL

#ifdef __ALPHA_CHAT
	else if (scan.Token == _T("alphaChat"))
	{
		alphaChat = scan.GetNumber() != 0;
	}
#endif //__ALPHA_CHAT

#ifdef __STOP_EXP
	else if (scan.Token == _T("ExpStop"))
	{
		m_bExpStop = scan.GetNumber();
	}
#endif //__STOP_EXP

#ifndef __AEGON_DB_PETFILTER
#ifdef __SYS_PETFILTER
	else if (scan.Token == _T("Petfilter"))
	{
		m_dwPetfilter = scan.GetInt64();
	}
#endif //__SYS_PETFILTER
#endif

#ifdef __SWITCH_DMG_RENDER
	else if (scan.Token == _T("damageRender"))
	{
		damageRender = scan.GetNumber();
	}
#endif //__SWITCH_DMG_RENDER

#ifdef __SYS_CUSTOM_TITLE
	else if (scan.Token == _T("ShowCustomTitle"))
	{
		m_bShowCustomTitle = scan.GetNumber();
	}
#endif // __SYS_CUSTOM_TITLE

#ifdef __PREMIUM
	else if (scan.Token == _T("ShowPremiumTitle"))
	{
		m_bShowPremiumTitle = scan.GetNumber();
	}
#endif // __PREMIUM

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
	else if (scan.Token == _T("centerTaskbar"))
	{
		m_bCenterTaskbar = scan.GetNumber();
	}
	else if (scan.Token == _T("bWindowMaximized"))
	{
		m_bWindowMaximized = scan.GetNumber();
	}
#endif // __AEGON_FULLSCREEN_AND_RESIZE

#ifdef __SINGLE_TASKBAR_SWAP
	else if (scan.Token == _T("TaskbarDisplay"))
	{
		m_bTaskbarDisplay = scan.GetNumber();
	}
#endif // __SINGLE_TASKBAR_SWAP

#ifdef __NEWCUR
	else if (scan.Token == _T("nCurFold"))
	{
		m_nCurFold = scan.GetNumber();
	}
#endif // __NEWCUR

#ifdef __AEGON_INGAME_DP
	else if (scan.Token == _T("skipDPConfirmLootbox"))
	{
		int nScan = scan.GetNumber();
		m_bSkipDPConfirmLootbox = nScan == 0 ? FALSE : TRUE;
	}
#endif // __AEGON_INGAME_DP

#ifdef __AEGON_THEME_SWITCHER
	else if (scan.Token == _T("selectedTheme"))
	{
		m_nSelectedThemeID = scan.GetNumber();
	}
#endif // __AEGON_THEME_SWITCHER

#ifdef __NEW_BUFF_FORMAT
	else if (scan.Token == _T("Buffs"))
	{
		m_Buffs = scan.GetNumber();
	}
#endif // __NEW_BUFF_FORMAT

#ifdef __RENDER_SMALL_POWERUPS
	else if (scan.Token == _T("SmallPowerUps"))
	{
		m_nSmallPowerUps = scan.GetNumber();
	}
#endif //__RENDER_SMALL_POWERUPS

#ifdef __FLYFF_INITPAGE_EXT
	else if (scan.Token == _T("LoginVideo"))
	{
		m_nLoginVideo = scan.GetNumber();
	}
#endif //__FLYFF_INITPAGE_EXT

#ifdef __NAVIGATOR_V19
	else if (scan.Token == _T("V19Navigator"))
	{
		m_bV19Navigator = scan.GetNumber();
	}
#endif // __NAVIGATOR_V19

#ifdef __LEESE_MESSENGER_TIMER
	else if (scan.Token == _T("TimeStamp"))
	{
		m_bTimeStamp = scan.GetNumber();
	}
#endif // __LEESE_MESSENGER_TIMER

#ifdef __TRAILS
	else if (scan.Token == _T("HideTrails"))
	{
		m_bHideTrails = scan.GetNumber();
	}
#endif //__TRAILS

#ifdef __BLOCK_GUILD_INVITES
	else if (scan.Token == _T("BlockGuildInvites"))
	{
		m_bBlockGuildInvites = scan.GetNumber();
	}
#endif //__BLOCK_GUILD_INVITES

#ifdef __HEAL_SHOW
	else if (scan.Token == _T("HideHealingNumbers"))
	{
		m_bHideHeal = scan.GetNumber();
	}
#endif //__HEAL_SHOW

#ifdef __AUTO_ATTACK_NEXT_TARGET
	else if (scan.Token == _T("AutoAttackNextTarget"))
	{
		m_bAutoAttackNT = scan.GetNumber();
	}
#endif //__AUTO_ATTACK_NEXT_TARGET

#ifdef __AEGON_MONSTER_HUNT
	else if (scan.Token == _T("PrintMHProgress"))
	{
		m_bPrintMHProgress = scan.GetNumber() == 0 ? FALSE : TRUE;
	}
#endif

#ifdef __LEESE_SAVE_STATUS
	else if (scan.Token == _T("nStatusSave"))
	{
		m_nStatusSave = scan.GetNumber();
	}
#endif //__LEESE_SAVE_STATUS

#ifdef __WBQT_GUILD_BUFFS
	else if (scan.Token == _T("HideGuildBuffs"))
	{
		m_bHideGuildBuffs = scan.GetNumber() == 0 ? FALSE : TRUE;
	}
#endif

#ifdef __AEGON_FIXES
	else if (scan.Token == _T("32bitMode"))
	{
		m_bStart32Bit = scan.GetNumber() == 0 ? FALSE : TRUE;
	}
#endif

	} while( scan.tok != FINISHED );

#if __VER >= 9 // __CSC_VER9_RESOLUTION
	CheckResolution();
#endif //__CSC_VER9_RESOLUTION

#if __VER >= 12 // __UPDATE_OPT
	m_nChatCommand				= 0;
	m_nSlangWord				= 0;
	m_nInterface				= 1;
	m_bRollEffect				= FALSE;
	m_MouseSpeed				= 1;
	m_bVisibleBuffTimeRender	= TRUE;
	m_bShout					= TRUE;
	m_bMessengerJoin   			= TRUE;
#endif

	return 1;
}

int	COption::Save( LPCTSTR szFileName )
{
	FILE *fp = fopen( szFileName, _T( "wt" ) );
	if(fp == NULL) 
		return 0;

	_ftprintf(fp, _T( "// neuz browser ini file\n\n" ) );
	_ftprintf(fp, _T( "// option\n" ) ,0);
	_ftprintf(fp, _T( "resolution %d %d\n" ) ,m_nResWidth, m_nResHeight );
	_ftprintf(fp, _T( "fullscreen %d\n" ) ,m_bStartFullScreen );
	_ftprintf(fp, _T( "texture %d\n" ) ,m_nTextureQuality );
	_ftprintf(fp, _T( "view %d\n" ) ,m_nViewArea );
	_ftprintf(fp, _T( "detail %d\n" ) ,m_nObjectDetail );
	_ftprintf(fp, _T( "distant %d\n" ) ,m_nObjectDistant );
	_ftprintf(fp, _T( "shadow %d\n" ) ,m_nShadow );
	_ftprintf(fp, _T( "bloom %d\n" ) ,m_nBloom );
	_ftprintf(fp, _T( "WindowAlpha %d\n" ) ,m_nWindowAlpha );
	_ftprintf(fp, _T( "SlangWord %d\n" )   ,m_nSlangWord );
	_ftprintf(fp, _T( "ChatCommand %d\n" ) ,m_nChatCommand );
	_ftprintf(fp, _T( "InstantHelp %d\n" ) ,m_nInstantHelp );
	_ftprintf(fp, _T( "Interface %d\n" ) ,m_nInterface );
	_ftprintf(fp, _T( "RollEffect %d\n" ) ,m_bRollEffect );
	_ftprintf(fp, _T( "saveAccount %d\n" ), m_bSaveAccount );

	if( m_bSaveAccount )
		_ftprintf(fp, _T( "account \"%s\"\n" ), m_szAccount );
	else
		_ftprintf(fp, _T( "account \"%s\"\n" ), "" );

	_ftprintf(fp, _T( "notice %d\n" ), m_bNotice );
	_ftprintf(fp, _T( "noticeTime %d\n" ), m_tNoticeTime );
	_ftprintf(fp, _T( "tip %d\n" ), m_bTip );
	_ftprintf(fp, _T( "Trade %d\n" ), m_bTrade );
	_ftprintf(fp, _T( "Say %d\n" ), m_bSay );
	_ftprintf(fp, _T( "Party %d\n" ), m_bParty );
	_ftprintf(fp, _T( "Shout %d\n" ), m_bShout );
	_ftprintf(fp, _T( "MessengerJoin %d\n" ), m_bMessengerJoin );
	_ftprintf(fp, _T( "Messenger %d\n" ), m_bMessenger );
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	_ftprintf(fp, _T( "BattleBGM %d\n" ), m_bBattleBGM );
#endif // __IMPROVE_SYSTEM_VER15

	_ftprintf(fp, _T( "Server %d %d\n" ), m_nSer, m_nMSer );

	if( m_nToggleScreen )
		_ftprintf(fp, _T( "sunkist %d\n" ), m_nToggleScreen );

	_ftprintf(fp, _T( "Sound %d %.1f %.1f\n" ), m_bVoice, m_fEffectVolume, m_fBGMVolume );
	_ftprintf(fp, _T( "Guide %d\n" ), m_nGuide1 );
	_ftprintf(fp, _T( "Guide2 %d\n" ), m_nGuide2 );
	_ftprintf(fp, _T( "Guide3 %d\n" ), m_nGuide3 );
	_ftprintf(fp, _T( "SkillShowMessage %d\n" ), m_bIsShowReSkillMessage );
	_ftprintf(fp, _T( "ChatFilter0 %d\n" ), m_dwChatFilter[0] );
	_ftprintf(fp, _T( "ChatFilter1 %d\n" ), m_dwChatFilter[1] );
	_ftprintf(fp, _T( "ChatFilter2 %d\n" ), m_dwChatFilter[2] );
	_ftprintf(fp, _T( "ChatFilter3 %d\n" ), m_dwChatFilter[3] );
	_ftprintf(fp, _T( "ChatFilter4 %d\n" ), m_dwChatFilter[4] );
	_ftprintf(fp, _T( "ChatFilterEtc %d\n" ), m_dwChatFilterEtc );

#ifdef __YENV
	if( m_bExitChange )
		_ftprintf(fp, _T( "SpecBumpEffect %d\n" ), !m_bSpecBump );
	else
		_ftprintf(fp, _T( "SpecBumpEffect %d\n" ), m_bSpecBump );	
#endif //__YENV

	_ftprintf(fp, _T( "ip \"%s\"\n" ), m_IPAddress);
	_ftprintf(fp, _T( "DamageRender %d\n" ), m_bDamageRender );
	_ftprintf(fp, _T( "BuffTimeRender %d\n" ), m_bVisibleBuffTimeRender );
	_ftprintf(fp, _T( "MouseSpeed %d\n" ), m_MouseSpeed );
	_ftprintf(fp, _T( "SFXRenderOff %d\n" ), m_bSFXRenderOff );
	_ftprintf(fp, _T( "CameraLock %d\n" ), m_bCameraLock );
	_ftprintf(fp, _T( "AutoAttack %d\n" ), m_bAutoAttack );
	_ftprintf(fp, _T( "PRenderName %d\n" ), m_nPlayerName );
	_ftprintf(fp, _T( "OPRenderName %d\n" ), m_nOtherPlayerName );
	_ftprintf(fp, _T( "NRenderName %d\n" ), m_nMonName );

#if __VER >= 8 //__Y_GAMMA_CONTROL_8
	_ftprintf(fp, _T( "BRIGHT %d\n" ),   m_nOverBright );
	_ftprintf(fp, _T( "GAMMA %f\n" ),    m_fGamma );
	_ftprintf(fp, _T( "CONTRAST %f\n" ), m_fContrast );
#endif //__Y_GAMMA_CONTROL_8
			
#if __VER >= 8 //__CSC_VER8_3
	_ftprintf(fp, _T( "BUFFSTATUSMODE %d\n" ), m_BuffStatusMode );
#endif //__CSC_VER8_3
#if __VER >= 11 // __ADD_ZOOMOPT
	_ftprintf(fp, _T( "ZOOMLIMIT %d\n" ), m_bZoomLimit );
#endif
#ifdef __SFX_OPT
	_ftprintf(fp, _T( "SFXLEVEL %d\n" ), m_nSfxLevel );
#endif
	_ftprintf(fp, _T( "WEATHEREFFECT %d\n" ), m_nWeatherEffect );

#if __VER >= 12 // __UPDATE_OPT
	_ftprintf(fp, _T( "VIEWMASK %d\n" ), m_bViewMask );
#endif

#ifdef __SYS_PET_NO_CLICK
	_ftprintf(fp, _T("petClickOption %d\n"), static_cast<int>(petClickOption));
#endif //__SYS_PET_NO_CLICK

#ifdef __NEW_NIGHT
	_ftprintf(fp, _T("NewNight %d\n"), m_bNewNight);
#endif // __NEW_NIGHT

#ifdef __SYS_GROUND_ITEM_TEXT
	_ftprintf(fp, _T("GroundItem %d\n"), groundItemText);
#endif // __SYS_GROUND_ITEM_TEXT

#ifdef __MUTEBGSOUNDS
	_ftprintf(fp, _T("muteSoundsInBG %d\n"), static_cast<int>(muteSoundsInBG));
	_ftprintf(fp, _T("muteMusicInBG %d\n"), static_cast<int>(muteMusicInBG));
#endif // __MUTEBGSOUNDS

#ifdef __BACKGROUNDRENDER
	_ftprintf(fp, _T("inactiveRender %d\n"), inactiveRender);
#endif // __BACKGROUNDRENDER

#ifdef __ALIASING
	_ftprintf(fp, _T("Antialiasing %d\n"), m_nAliasing);
#endif // __ALIASING

#ifdef __ANISOTROPIC
	_ftprintf(fp, _T("Anisotropic %d\n"), m_nAnisotropic);
#endif // __ANISOTROPIC

#ifdef __AEGON_FONT_SWITCH
	CString strFontTmp = m_strFont;
	strFontTmp.Replace(" ", "_");
	_ftprintf(fp, _T("Font %s\n"), strFontTmp.GetString());
#endif // __AEGON_FONT_SWITCH

#ifdef __HIDE_NPC_BUBBLES
	_ftprintf(fp, _T("Bubbles %d\n"), m_nNPCBubbles);
#endif // __HIDE_NPC_BUBBLES

#ifdef __HIDE_POWER_UPS
	_ftprintf(fp, _T("HidePU %d\n"), m_nHidePU);
#endif //__HIDE_POWER_UPS

#ifdef __HIDE_MONSTER_LEVEL
	_ftprintf(fp, _T("MonsterLv %d\n"), m_nMonsterLevel);
#endif // __HIDE_MONSTER_LEVEL

#ifdef __ALPHA_CHAT
	_ftprintf(fp, _T("alphaChat %d\n"), static_cast<int>(alphaChat));
#endif //__ALPHA_CHAT

#ifdef __STOP_EXP
	_ftprintf(fp, _T("ExpStop %d\n"), m_bExpStop);
#endif //__STOP_EXP

#ifndef __AEGON_DB_PETFILTER
#ifdef __SYS_PETFILTER
	_ftprintf(fp, _T("Petfilter %I64d\n"), m_dwPetfilter);
#endif //__SYS_PETFILTER
#endif

#ifdef __SWITCH_DMG_RENDER
	_ftprintf(fp, _T("damageRender %d\n"), static_cast<int>(damageRender));
#endif //__SWITCH_DMG_RENDER

#ifdef __SYS_CUSTOM_TITLE
	_ftprintf(fp, _T("ShowCustomTitle %d\n"), m_bShowCustomTitle);
#endif // __SYS_CUSTOM_TITLE

#ifdef __PREMIUM
	_ftprintf(fp, _T("ShowPremiumTitle %d\n"), m_bShowPremiumTitle);
#endif // __PREMIUM

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
	_ftprintf(fp, _T("centerTaskbar %d\n"), m_bCenterTaskbar);
	_ftprintf(fp, _T("bWindowMaximized %d\n"), m_bWindowMaximized);
#endif // __AEGON_FULLSCREEN_AND_RESIZE

#ifdef __SINGLE_TASKBAR_SWAP
	_ftprintf(fp, _T("TaskbarDisplay %d\n"), m_bTaskbarDisplay);
#endif // __SINGLE_TASKBAR_SWAP

#ifdef __NEWCUR
	_ftprintf(fp, _T("nCurFold %d\n"), m_nCurFold);
#endif // __NEWCUR

#ifdef __AEGON_INGAME_DP
	int nPrint = m_bSkipDPConfirmLootbox ? 1 : 0;
	_ftprintf(fp, _T("skipDPConfirmLootbox %d\n"), nPrint);
#endif // __AEGON_INGAME_DP

#ifdef __AEGON_THEME_SWITCHER
	_ftprintf(fp, _T("selectedTheme %d\n"), m_nSelectedThemeID);
#endif // __AEGON_THEME_SWITCHER

#ifdef __NEW_BUFF_FORMAT
	_ftprintf(fp, _T("Buffs %d\n"), m_Buffs);
#endif // __NEW_BUFF_FORMAT

#ifdef __RENDER_SMALL_POWERUPS
	_ftprintf(fp, _T("SmallPowerUps %d\n"), m_nSmallPowerUps);
#endif //__RENDER_SMALL_POWERUPS

#ifdef __FLYFF_INITPAGE_EXT
	_ftprintf(fp, _T("LoginVideo %d\n"), m_nLoginVideo);
#endif //__FLYFF_INITPAGE_EXT

#ifdef __NAVIGATOR_V19
	_ftprintf(fp, _T("V19Navigator %d\n"), m_bV19Navigator);
#endif // __NAVIGATOR_V19

#ifdef __LEESE_MESSENGER_TIMER
	_ftprintf(fp, _T("TimeStamp %d\n"), m_bTimeStamp);
#endif // __LEESE_MESSENGER_TIMER

#ifdef __TRAILS
	_ftprintf(fp, _T("HideTrails %d\n"), m_bHideTrails);
#endif //__TRAILS

#ifdef __BLOCK_GUILD_INVITES
	_ftprintf(fp, _T("BlockGuildInvites %d\n"), m_bBlockGuildInvites);
#endif //__BLOCK_GUILD_INVITES

#ifdef __HEAL_SHOW
	_ftprintf(fp, _T("HideHealingNumbers %d\n"), m_bHideHeal);
#endif //__HEAL_SHOW

#ifdef __AUTO_ATTACK_NEXT_TARGET
	_ftprintf(fp, _T("AutoAttackNextTarget %d\n"), m_bAutoAttackNT);
#endif //__AUTO_ATTACK_NEXT_TARGET

#ifdef __AEGON_MONSTER_HUNT
	_ftprintf(fp, _T("PrintMHProgress %d\n"), m_bPrintMHProgress);
#endif

#ifdef __LEESE_SAVE_STATUS
	_ftprintf(fp, _T("nStatusSave %d\n"), m_nStatusSave);
#endif //__LEESE_SAVE_STATUS

#ifdef __WBQT_GUILD_BUFFS
	_ftprintf(fp, _T("HideGuildBuffs %d\n"), m_bHideGuildBuffs);
#endif

#ifdef __AEGON_FIXES
	_ftprintf(fp, _T("32bitMode %d\n"), m_bStart32Bit);
#endif

	fclose(fp);
	return 1;
}

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
#include <fstream>
#include <ostream>
void COption::saveBlockedUsers(CString playerName)
{
	std::ofstream f(playerName + "_BlockedList.txt", std::ios::out | std::ios::trunc | std::ios::binary);
	for (std::set<std::string>::const_iterator it = prj.m_setBlockedUserID.begin(); it != prj.m_setBlockedUserID.end(); ++it)
	{
		f.write(it->c_str(), it->length());
		f.write("\n", 1);
	}
	f.close();
}

void COption::saveBlockedUser(CString playerName, const std::set<std::string>::const_iterator& position)
{
	std::ofstream f(playerName + "_BlockedList.txt", std::ios::out | std::ios::app | std::ios::binary);
	f.write(position->c_str(), position->length());
	f.write("\n", 1);
	f.close();
}

void COption::loadBlockedUsers(CString playerName)
{
	std::ifstream f(playerName + "_BlockedList.txt", std::ios::in | std::ios::binary);
	if (f.is_open())
	{
		f.seekg(0, std::ios::end);
		unsigned int end = static_cast<unsigned int>(f.tellg());
		f.seekg(0, std::ios::beg);

		std::string line(end, '\0');
		f.read(&line[0], end);

		size_t pos, oldPos = 0;
		while ((pos = line.find('\n', oldPos)) != std::string::npos)
		{
			if (line[pos-1] == '\r')
				prj.m_setBlockedUserID.insert(line.substr(oldPos, (pos - oldPos) - 1));
			else
				prj.m_setBlockedUserID.insert(line.substr(oldPos, pos - oldPos));				
			oldPos = pos + 1;
		}
	}
	f.close();
}
#endif