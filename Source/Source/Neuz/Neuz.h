#ifndef __NEUZAPP_H 
#define __NEUZAPP_H

#define WM_LOGOUT ( WM_USER + 10 )


#define CUR_ATTACK       0
#define CUR_BASE         1
#define CUR_CAMERA       2
#define CUR_CONTROL      3
#define CUR_DELAY        4
#define CUR_EDIT         5
#define CUR_GETITEM      6
#define CUR_HELP         7
#define CUR_NO           8
#define CUR_NPC          9
#define CUR_SELECT       10
#define CUR_RESIZE_HORI  11
#define CUR_RESIZE_VERT  12
#define CUR_RESIZE_HV1   13
#define CUR_RESIZE_HV2   14
#define CUR_DOOR	     15
#define CUR_GATE	     16
#define CUR_LEVER        17
#define CUR_HAMMER       18

struct LOGIN_STEP_INFO
{
	LPCTSTR		pszAccount;			// account
	LPCTSTR		pszPassword;		// password
	DWORD		dwID;				// pServerDesc->dwID 서버번호 
	int			nCount;				// 
};

#include "NeuzEnemy.h"



class CNeuzApp : public CD3DApplication 
{
public:
	CNeuzApp();

public:
#ifdef __NEWCUR
	void SetMouseCursor(DWORD dwCursor);
#endif //__NEWCUR
#ifdef __FIX_TICK
	DWORD					m_nSoundTick;
#endif //__FIX_TICK
	DWORD					m_dwTimeOutDis;
	int						m_nTexAlpha;
	BOOL					m_bTexLoad;
	BOOL					m_bTexLoadAlpha;
	CTexture				m_TexLoading;
	BOOL					m_bActiveNeuz;
	CNeuzEnemy				m_NeuzEnemy;
	DWORD					m_dwTimeReSkillPoint;
	BOOL					m_bQuit;
	CTimer					m_timerQuit;//.Set( SEC( 1 ) );	
#ifdef __VENDING_NPC
	map<OBJID, DWORD>		m_mapVisitedShops;
#endif // __VENDING_NPC
	CTexturePack			m_texQuestEmoticon;
	CTexture				m_texChar;
	C2DRender				m_2DRender;
	HCURSOR					m_ahCursor[ 20 ];
	TCHAR					m_szCaptureFileName[ 64 ];
	BOOL					m_bCapture;
	int						m_nCaptureCnt;
	CTimer					m_timerConnect;
	DWORD					m_dwTempMessage;
	BOOL					m_bLButtonDown;
	BOOL					m_bRButtonDown;
	CBackCamera				m_camera;
	BOOL					m_bFrameSkip;			// 프레임 스키핑 플래그
	CTexture				m_TextureGauge[5];
	CTexture				m_TextureTurboGauge[2];
	CTexture				m_TextureCastingGauge[2];
	CTexturePack			m_TexturePackPVP;
	int						m_nPVPCount;
	DWORD					m_dwPVPTime;
	CStringArray			m_strArray;
	TCHAR					m_lpCertifierAddr[64];				// 서버 IP Address
	TCHAR					m_lpCacheAddr[16];
	u_short					m_uCachePort;						// 15400, 5400
	LOGIN_STEP_INFO			m_loginSI;
	TCHAR					m_szAccount[MAX_ACCOUNT];			// account
	TCHAR					m_szPassword[MAX_PASSWORD];			// password

#ifdef __CHANGE_HOT_KEY
	bool	m_bKeyListening;
#endif // __CHANGE_HOT_KEY

#ifdef __SYS_WORLDBOSS
	char	m_WinGUID[38];
#endif //__SYS_WORLDBOSS

#ifdef __GPAUTH_01
	BOOL	m_bGPotatoAuth;
	TCHAR	m_szGPotatoNo[10];
#ifdef __GPAUTH_02
	TCHAR	m_szCheck[255];
#endif	// __GPAUTH_02
#endif	// __GPAUTH_01
	DWORD					m_dwSys;
	DWORD					m_dwAuthKey;
	CMover*					m_apPlayer[MAX_CHARACTER_LIST];
	DWORD					m_adwWorldID[MAX_CHARACTER_LIST];
	int						m_nCharacterBlock[MAX_CHARACTER_LIST];
#ifdef __RT_1025
	CRTMessenger	m_aRTMessenger[MAX_CHARACTER_LIST];
#else	// __RT_1025
	CMessenger				m_Messenger[MAX_CHARACTER_LIST];
#endif	// __RT_1025
#ifdef __THROUGHPORTAL0810
	BOOL	m_bThroughPortal;
#else	// __THROUGHPORTAL0810
	BOOL					m_bHanGame;
#endif	// __THROUGHPORTAL0810
#ifdef __TWN_LOGIN0816
	TCHAR	m_szSessionPwd[MAX_SESSION_PWD];
#endif	// __TWN_LOGIN0816
	BOOL					m_bEncryptPWD;
	u_long					m_idPlayer;
	int						m_nTagCount;
	BOOL					m_bCheckTag;
	char					m_strTagName[ 20 ][ 65 ];
	DWORD					m_dwtegDate[ 20 ];
	char					m_strTagMessage[ 20 ][ 256 ];
	DWORD					m_dwPingTime;
	short					m_nPingCount;
	BYTE					m_cbAccountFlag;	// 18세이상, 학교대항전, 24시간플레이 플래그 
	DWORD					m_dwCurTick;
	DWORD					m_dwTimeLeft;		// 플레이할 수 있게 남은 시간 (태국의 경우)
	UINT					m_nLeftTimeNotifyStatus;
	DWORD					m_dwShoutLimitSecond;
	int						m_nShoutLimitCount;
	u_long					m_uIdofMulti;
	BOOL					m_b18Server;		
	HANDLE					m_hThread;
	DWORD					m_dwSummonPartyData;
	OBJID					m_dwSummonPartyObjid;
	char					m_szSummonPartyWorldName[128];
	CTexture				m_TextureHPGauge[3];
	CTexture*				m_pMasterIcon[6];
	CTexture*				m_pHeroIcon;
	CTexture*				m_pLegendIcon;
#ifdef __SYS_REBIRTH
	CTexture*				m_pRebIcon[10];
#endif //__SYS_REBIRTH

#if __VER >= 11 //	__SYS_COLLECTING
	CTexture				m_TexCltGauge[2];
#endif
#if __VER >= 11 // __MOD_VENDOR
	CItemBase				m_aSavedInven[MAX_VENDITEM];
#endif
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	int						m_n2ndPasswordNumber;
#endif // __2ND_PASSWORD_SYSTEM

#if __VER >= 8 //__CSC_VER8_5
	CTexture				m_TextureAngelGauge[2];
#endif //__CSC_VER8_5

	TCHAR					m_szWhisperName[MAX_NAME];			// 귓속말 보낼 이름
	
#if __VER >= 12 // __ITEMCREATEMON_S0602
	D3DXVECTOR3				m_vCursorPos;
	CItemElem*				m_pCreateMonItem;
#endif // __ITEMCREATEMON_S0602

#ifdef __VENDING_NPC
	char                    m_szVendingNpcTitle[MAX_VENDORNAME];
	CItemElem* m_pVendingNpcItem;
#endif // __VENDING_NPC

#ifdef __YENV
	LPD3DXEFFECT			m_pEffect = nullptr;
	D3DXHANDLE				m_hTechnique;
	D3DXHANDLE				m_hmWVP;
	D3DXHANDLE				m_hvLightDir;

	D3DXHANDLE				m_hTexDiffuse;
	D3DXHANDLE				m_hTexBump;
	D3DXHANDLE				m_hTexSpecular;

	D3DXHANDLE				m_hvEyePos;
	D3DXHANDLE				m_hvFog;
	D3DXHANDLE				m_hvEye;
	D3DXHANDLE				m_hvDiffuse;
	D3DXHANDLE				m_hvSpecual;	
	D3DXHANDLE				m_hvAmbient;
#ifdef __dx9c
	D3DXHANDLE m_hvmatProj, m_hvmatView;
	std::array<D3DXHANDLE, 28> m_hvBones;
	D3DXHANDLE m_hvTexEnvi;
#endif
	
	LPDIRECT3DTEXTURE9		m_pEnvTex;
#endif //__YENV	

	CWndBase* pOldModalWindow;

	struct
	{
		char	chLeft;		// 'A'
		char	chUp;		// 'W'
		char	chTrace;	// 'Z'
		char	chQuest;	// 'Q'
		char	chWalk;	// 'X'
#ifdef __CHANGE_HOT_KEY
		char	chRight;	// D
		char	chBack;		// S
#endif // __CHANGE_HOT_KEY
#ifdef __DPS_METER
		char	chDpsMeter;	// 'C'
#endif
	}	Key;

#ifdef __SHOPS_VISITED
	private:
		vector<OBJID> _visitedShops;
#endif //__SHOPS_VISITED

#ifdef __NEWCUR
public:
	void UpdateCursor();
#endif //__NEWCUR

//protected:
	HRESULT		OneTimeSceneInit();
	HRESULT		InitDeviceObjects();
	HRESULT		RestoreDeviceObjects();
	HRESULT		InvalidateDeviceObjects();
	HRESULT		DeleteDeviceObjects();
	HRESULT		FinalCleanup();
	HRESULT		Render(); 
	HRESULT		FrameMove();
	HRESULT		ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior, 
								D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat );
	void		NotifyLeftTime();
	void		NotifyLeftMinute( UINT type, int nMin );
	void		HashMD5( char* szResult, LPCTSTR szPWD );

public:

#if __VER >= 15
	void		ResetStaticValues( );	//gmpbigsun(100429)
#endif

#ifdef __SHOPS_VISITED
	bool		IsVisitedShop(OBJID moverId) { return easy_find(_visitedShops, moverId); }
	void		AddVisitedShop(OBJID moverId) {
		if (!easy_find(_visitedShops, moverId))
			_visitedShops.push_back(moverId);
	}
	void		ResetVisistedShops() { _visitedShops.clear(); }
	void		RemoveShopFromList(OBJID moverId) {
		vector<OBJID>::iterator position = find(_visitedShops.begin(), _visitedShops.end(), moverId);
		if (position != _visitedShops.end())
			_visitedShops.erase(position);
	}
#endif //__SHOPS_VISITED

#if __VER >= 8 //__Y_GAMMA_CONTROL_8
	void Drv_SetGamma(HWND hWnd, float gamma, int overbright, float fContrast);
#endif //__Y_GAMMA_CONTROL_8	

	LRESULT		MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
	void		SetLeftTime( long lTimeLeft );
	HRESULT		CaptureBitmap();	   
#ifdef __TWN_LOGIN0816
	void		SetAccountInfo( LPCTSTR lpszAccount, LPCTSTR lpszpw, LPCTSTR lpszSessionPwd = "" );
#else	// __TWN_LOGIN0816
	void		SetAccountInfo( LPCTSTR lpszAccount, LPCTSTR lpszpw );
#endif	// __TWN_LOGIN0816
	LPCTSTR		GetDeleteKey( LPCTSTR szNo );
	BOOL		LoadOption();
	BOOL		KillWindow( int wParam );
	void		BeginLoadThread();	
	void		WaitLoading();

#ifdef __DEBUG_NEUZ
	BOOL		LoadAllModel();
	BOOL		LoadAllTerrain();
	BOOL		LoadAllItemIcon();
	BOOL		LoadAllIcon();
#endif //__DEBUG_NEUZ

#ifdef __AUTO_HELPER_THREAD
	CWinThread*		m_AutoHelperThread;
	static UINT		AutoHelperThreadLoop(LPVOID param);
	void			StartAutoHelperThread();
#endif // __AUTO_HELPER_THREAD

#ifdef __CRASH_REPORT
	BOOL m_bSendCrashReport;
#endif

};

extern LPCTSTR GetProtocolVersion();
#ifndef __NEWCUR
extern void SetMouseCursor( DWORD dwCursor);
#endif //__NEWCUR

extern CNeuzApp g_Neuz;
#ifdef __LINK_PORTAL
extern BOOL	g_bBuddyFlag;
#endif //__LINK_PORTAL

#endif