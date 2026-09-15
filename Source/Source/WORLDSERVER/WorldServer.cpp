#include "StdAfx.h"
#include "Resource.h"
#include "GlobalTime.h"
#include "DPSrvr.h"
#include "DPCoreClient.h"
#include "DPDatabaseClient.h"
#include "User.h"
#include "WorldMng.h"
#include "WorldServer.h"
#include "ThreadMng.h"
#include "AIBear.h"
#include "AIBigMuscle.h"
#include "AIClockWorks.h"
#include "AIGuard.h"
#include "AIKrrr.h"
#include "AIPet.h"
#include "guild.h"
#include "guildwar.h"
#include "party.h"
#include "Chatting.h"
#include "MiniDumper.h"
#include "spevent.h"
#include "ItemScript.h"
#include "WorldDialog.h"
#include "WScript.h"

#ifdef __VM_0819
#include "vmlog.h"
#endif	// __VM_0819

#if __VER >= 13 // __COUPLE_1117
#include "couple.h"
#endif	// __COUPLE_1117
#if __VER >= 13 // __HOUSING
#include "housing.h"
#endif	// __HOUSING

#if __VER >= 12 // __ITEMCREATEMON_S0602
#if __VER < 12 // __NEW_ITEMCREATEMON_SERVER
#include "CreateMonster.h"
#endif // __NEW_ITEMCREATEMON_SERVER
#endif // __ITEMCREATEMON_S0602

#if __VER >= 12 // __LORD
#include "slord.h"
#endif	// __LORD

#ifdef __NEW_PROFILE
#include "ProfileInfo.h"
#endif // __NEW_PROFILE

#ifdef __LANG_1013
#include "langman.h"
#endif	// __LANG_1013

#ifdef __SYS_MAP_BUFFS
#include "MapBuffManager.h"
#endif //__SYS_MAP_BUFFS

LRESULT	CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
BOOL				Script(LPCSTR lpszFileName);
void				ExitInstance(void);
void				SetTitle();
extern BOOL			LoadAIScript();
#if __VER >= 12 // __ITEMCREATEMON_S0602
#if __VER < 12 // __NEW_ITEMCREATEMON_SERVER
BOOL				LoadScriptCreateMonster(void);
#endif // __NEW_ITEMCREATEMON_SERVER
#endif // __ITEMCREATEMON_S0602

const int			MAX_LOADSTRING = 100;
static TCHAR		g_szTitle[MAX_LOADSTRING];
static TCHAR		g_szWindowClass[MAX_LOADSTRING];
static HWND			g_hMainWnd;
static char			g_szBuffer[LOGTYPE_MAX][256] = { 0, };
char				g_szCoreAddr[16];
/*
#ifdef __GIFTBOX0213
static char			s_szAccountAddr[16];
#endif
*/

#ifdef __IDC
//static char  		g_szINI[] = "..\\script\\WorldServer.ini";
static char  		g_szINI[] = "WorldServer.ini";
#else	
static char  		g_szINI[] = "WorldServer.ini";
#endif	

extern	CGuildMng			g_GuildMng;
extern	CGuildWarMng		g_GuildWarMng;
extern	CChattingMng		g_ChattingMng;
extern	CPartyMng			g_PartyMng;
extern	char				g_szDBAddr[16];
extern	char				g_sHeartbeat[32];
extern	CDPSrvr				g_DPSrvr;
extern	CDPCoreClient		g_DPCoreClient;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CUserMng			g_UserMng;
extern	CWorldMng			g_WorldMng;
extern CGuildCombat			g_GuildCombatMng;
#if __VER >= 12 // __ITEMCREATEMON_S0602
#if __VER < 12 // __NEW_ITEMCREATEMON_SERVER
extern	CCreateMonster		g_CreateMonster;
#endif // __NEW_ITEMCREATEMON_SERVER
#endif 


#if !defined( __MINIDUMP )
#ifndef _DEBUG
#pragma comment(lib, "../_UnhandledException/neuzd.lib") 
#endif  // _DEBUG
#endif // !__MINIDUMP

#if defined(__LUASCRIPT060908)
#if _WIN64
#pragma comment(lib, "../lua/lib64/lua53.lib" )
#else
#pragma comment(lib, "../lua/lib/lua53.lib" )
#endif
#endif

void InitLanguageFromResource(HINSTANCE hInstance)
{
	char szLang[16], szSubLang[16];
	LoadString(hInstance, IDS_LANG, szLang, 16);
	LoadString(hInstance, IDS_SUBLANG, szSubLang, 16);

	char szCodePage[16];
	LoadString(hInstance, IDS_CODEPAGE, szCodePage, 16);
	g_codePage = atoi(szCodePage);

	::SetLanguageInfo(atoi(szLang), atoi(szSubLang));
}

extern void md5(char* out, const char* in);


int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{

	LoadString(hInstance, IDS_APP_TITLE, g_szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WORLDSERVER, g_szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	InitLanguageFromResource(hInstance);
	if (::GetLanguage() != LANG_KOR)
		g_eLocal.SetState(EVE_18, 1);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		ExitInstance();
		return FALSE;
	}

	MSG msg;
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ExitInstance();

	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_WORLDSERVER);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = (LPCSTR)IDC_WORLDSERVER;
	wcex.lpszClassName = g_szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

void SetLogInfo(LOGTYPE type, LPCTSTR lpszFormat, ...)
{
	if (type == LOGTYPE_REDRAW)
	{
		InvalidateRect(g_hMainWnd, NULL, TRUE);
		return;
	}

	va_list args;
	va_start(args, lpszFormat);

	_vsntprintf(g_szBuffer[type], 255, lpszFormat, args);

	va_end(args);
}

#ifdef __LEESE_WORLDSERVER_OPT
static std::string g_listOfWorlds = "";

void SetServerList(std::string serverList) 
{
	g_listOfWorlds = std::move(serverList);
}
#endif //__LEESE_WORLDSERVER_OPT

void OnPaint(HDC& hDC)
{
	int x, y;
	x = 10;
	y = 10;

	auto hFont = CreateFont(22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Tahoma");
	auto hTmp = (HFONT)SelectObject(hDC, hFont);

	for (int i = 0; i < LOGTYPE_MAX; ++i)
	{
		SetTextColor(hDC, RGB(0x41, 0x41, 0x41));
		TextOut(hDC, x, y, g_szBuffer[i], strlen(g_szBuffer[i]));
		y += 24;
	}

#ifdef __LEESE_WORLDSERVER_OPT
	RECT rcCli;
	GetClientRect(WindowFromDC(hDC), &rcCli);

	rcCli.top = y;
	DrawText(hDC, g_listOfWorlds.c_str(), -1, &rcCli, DT_WORDBREAK);
#endif //__LEESE_WORLDSERVER_OPT
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
#ifdef __WORLSERVER_MENU
	case WM_CREATE:
	{
		CreateWindow(_T("edit"), "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 300, 5, 450, 35, hWnd, (HMENU)6365, NULL, NULL);
		CreateWindow(_T("button"), "Send", BS_TEXT | WS_CHILD | WS_VISIBLE, 755, 5, 80, 35, hWnd, (HMENU)6367, NULL, NULL);
		break;
	}
#endif // __WORLSERVER_MENU
	case WM_TIMER:
	{
		WORD wTimerID = wParam;
		switch (wTimerID)
		{
		case 0:
			g_dpDBClient.SendPing();
			g_DPCoreClient.SendPing();
			break;
		case 1:
			// g_UserMng.OutputStatistics();
			break;
		}
		break;
	}
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
#ifdef __WORLSERVER_MENU
		case 6367:
		{
			char line[255] = { 0, };
			GetDlgItemText(hWnd, 6365, line, 255);
			CScanner scanner;
			scanner.SetProg((LPTSTR)line);
			scanner.GetToken();
			if (scanner.Token == _T("system") || scanner.Token == _T("sys")) {
				CHAR szString[512] = "[System] ";
				scanner.GetLastFull();

				if (strlen(scanner.token) >= 512)
					return TRUE;

				CString output;
				output.Format("%s%s", szString, scanner.token);
				g_DPCoreClient.SendSystem(output);
				AfxMessageBox("System Message Sent");
			}
			else if (scanner.Token == _T("outall")) {
				try
				{
					g_UserMng.SaveAllPlayers();
					g_DPCoreClient.SendSystem(prj.GetText(TID_WORLDSRVR_MENU_01));
					g_UserMng.RemoveAllUsers();
					AfxMessageBox("All players removed!");
				}
				catch (...)
				{
					AfxMessageBox(prj.GetText(TID_WORLDSRVR_MENU_03));
				}
			}
			else if (scanner.Token == _T("mapbuff")) {
				try {
					CMapBuffManager::GetInstance()->Reload();
					AfxMessageBox("Map Buffs Reloaded!");
				}
				catch (...) {
				}
			}
			else if (scanner.Token == _T("lua")) {
				scanner.GetToken();
				scanner.Token.MakeLower();

				g_dpDBClient.SendEventLuaChanged();
				AfxMessageBox("Event Lua Reloaded!");
			}
			else if (scanner.Token == _T("message")) {
				scanner.GetToken();
				u_long idPlayer = CPlayerDataCenter::GetInstance()->GetPlayerId(scanner.token);

				CUser* pUserTarget = g_UserMng.GetUserByPlayerID(idPlayer);
				if (idPlayer > 0)
				{
					if (IsValidObj(pUserTarget))
					{
						scanner.GetLastFull();
						CString szMsg;
						szMsg.Format("[CONSOLE] %s", scanner.token);
						pUserTarget->AddDiagText(szMsg);
						AfxMessageBox("Message Sent to Ingame Player!");
					}
				}
			}
			else
			{
				AfxMessageBox("Please enter the right commands!");
			}
		}
		break;
#endif // __WORLSERVER_MENU
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
#ifdef __WORLSERVER_MENU
		case ID_BLOCK_EXCHANGES_OFF:
		{
			HMENU hMenu = GetMenu(hWnd);
			CheckMenuItem(hMenu, ID_BLOCK_EXCHANGES_OFF, MF_CHECKED);
			CheckMenuItem(hMenu, ID_BLOCK_EXCHANGES_ON, MF_UNCHECKED);
			CDPSrvr::GetInstance()->m_bExchange = TRUE;
			CDPSrvr::GetInstance()->SendExchangeSetApply(DPID_ALLPLAYERS);
		}
		break;
		case ID_BLOCK_EXCHANGES_ON:
		{
			HMENU hMenu = GetMenu(hWnd);
			CheckMenuItem(hMenu, ID_BLOCK_EXCHANGES_ON, MF_CHECKED);
			CheckMenuItem(hMenu, ID_BLOCK_EXCHANGES_OFF, MF_UNCHECKED);
			CDPSrvr::GetInstance()->m_bExchange = FALSE;
			CDPSrvr::GetInstance()->SendExchangeSetApply(DPID_ALLPLAYERS);
		}
		break;
		case ID_BLOCK_CHAT_OFF:
		{
			HMENU hMenu = GetMenu(hWnd);
			CheckMenuItem(hMenu, ID_BLOCK_CHAT_OFF, MF_CHECKED);
			CheckMenuItem(hMenu, ID_BLOCK_CHAT_ON, MF_UNCHECKED);
			CDPSrvr::GetInstance()->m_bChat = TRUE;
			CDPSrvr::GetInstance()->SendChatSetApply(DPID_ALLPLAYERS);
		}
		break;
		case ID_BLOCK_CHAT_ON:
		{
			HMENU hMenu = GetMenu(hWnd);
			CheckMenuItem(hMenu, ID_BLOCK_CHAT_ON, MF_CHECKED);
			CheckMenuItem(hMenu, ID_BLOCK_CHAT_OFF, MF_UNCHECKED);
			CDPSrvr::GetInstance()->m_bChat = FALSE;
			CDPSrvr::GetInstance()->SendChatSetApply(DPID_ALLPLAYERS);
		}
		break;
		case ID_BLOCK_PK_OFF:
		{
			HMENU hMenu = GetMenu(hWnd);
			CheckMenuItem(hMenu, ID_BLOCK_PK_OFF, MF_CHECKED);
			CheckMenuItem(hMenu, ID_BLOCK_PK_ON, MF_UNCHECKED);
			CDPSrvr::GetInstance()->m_bPk = TRUE;
			CDPSrvr::GetInstance()->SendPkSetApply(DPID_ALLPLAYERS);
		}
		break;
		case ID_BLOCK_PK_ON:
		{
			HMENU hMenu = GetMenu(hWnd);
			CheckMenuItem(hMenu, ID_BLOCK_PK_ON, MF_CHECKED);
			CheckMenuItem(hMenu, ID_BLOCK_PK_OFF, MF_UNCHECKED);
			CDPSrvr::GetInstance()->m_bPk = FALSE;
			CDPSrvr::GetInstance()->SendPkSetApply(DPID_ALLPLAYERS);
		}
		break;
#endif // __WORLSERVER_MENU
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		OnPaint(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		CRunObject::GetInstance()->ToggleProfiling();
#ifdef __MEM_TRACE
#ifndef _DEBUG
		CMemTrace::Instance()->Print();
#endif	// _DEBUG
#endif	// __MEM_TRACE
#ifdef __VM_0819
		CVMLog::Instance()->Log();
#endif	// __VM_0819
		break;
	case WM_RBUTTONDOWN:
#ifdef __NEW_PROFILE
		CProfileInfo::GetInstance()->SetToggleProfiling();
#endif // __NEW_PROFILE
		break;
	case WM_DESTROY:
		KillTimer(g_hMainWnd, 0);
		KillTimer(g_hMainWnd, 1);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// float point 에러가 exception을 내게 한다.
void EnableFloatException()
{
	int cw = _controlfp(0, 0);
	cw &= ~(EM_OVERFLOW | EM_UNDERFLOW | EM_ZERODIVIDE | EM_DENORMAL);
	_controlfp(cw, MCW_EM);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	g_hMainWnd = hWnd = CreateWindow(g_szWindowClass, g_szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	int x = 20, y = 470;
	SetWindowPos(hWnd, NULL, x, y, 940, 350, SWP_SHOWWINDOW);

	if (!hWnd)
		return FALSE;

	SetTimer(g_hMainWnd, 0, SEC(60), NULL);	// 60
	SetTimer(g_hMainWnd, 1, MIN(60), NULL);	// 3600

	CTime time = CTime::GetCurrentTime();
	SetLogInfo(LOGTYPE_TIME, "%s", time.Format("[%d/%m/%Y] [%H:%M:%S]"));

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	EnableFloatException();

	if (InitializeNetLib() == FALSE)
		return FALSE;

	if (InitializeScriptLib() == FALSE)
		return FALSE;

	xSRand(timeGetTime());	// 속도를 요하는 랜덤은 xRandom()을 쓰도록...

	if (Script(g_szINI) == TRUE)
	{
		if (!prj.OpenProject("Masquerade.prj"))
		{
			Error("OpenProject ERROR check Masquerade.prj");
			return FALSE;
		}

		SetLogInfo(LOGTYPE_REDRAW, "");
		SetTitle();

		if (CompileItemScript("ItemScript.lua") == FALSE)
			return FALSE;

#if defined(__REMOVE_SCIRPT_060712)
		if (CWorldDialog::GetInstance().Init() == FALSE)
		{
			OutputDebugString("WorldDialog.DLL load failed\n");
			return FALSE;
		}
#endif
		return CRunObject::GetInstance()->Init();
	}
	else
	{
		char szBuffer[300] = { 0, };
		char szDir[255] = { 0, };
		GetCurrentDirectory(255, szDir);
		sprintf(szBuffer, "[CRITICAL] Missing File -> %s\\WorldServer.ini", szDir);
		AfxMessageBox(szBuffer);
		return FALSE;
	}
}

void ExitInstance(void)
{
	CRunObject::GetInstance()->Close();

	g_DPSrvr.DeleteDPObject();
	g_DPCoreClient.DeleteDPObject();
	g_dpDBClient.DeleteDPObject();

	g_WorldMng.Free();

#ifndef __VM_0820
#ifndef __MEM_TRACE
	SAFE_DELETE(CUser::m_pPool);
	SAFE_DELETE(CMover::m_pPool);
	SAFE_DELETE(CActionMover::m_pPool);
	SAFE_DELETE(CAIMonster::m_pPool);
	SAFE_DELETE(CItem::m_pPool);
#endif	// __MEM_TRACE
#endif	// __VM_0820

	g_PartyMng.Clear();
#ifndef __VM_0820
#ifndef __MEM_TRACE
	SAFE_DELETE(CParty::m_pPool);
#endif	// __MEM_TRACE
#endif	// __VM_0820
	g_ChattingMng.Clear();
#ifndef __VM_0820
#ifndef __MEM_TRACE
	SAFE_DELETE(CChatting::m_pPool);
#endif	// __MEM_TRACE
#endif	// __VM_0820
	g_GuildWarMng.Clear();

	g_GuildMng.Clear();

#if __VER >= 13 // __HOUSING
	CHousingMng::GetInstance()->Clear();
#endif	// __HOUSING
#ifndef __VM_0820
#ifndef __MEM_TRACE
	SAFE_DELETE(CGuildMember::sm_pPool);
	SAFE_DELETE(CGuild::sm_pPool);
	SAFE_DELETE(CAIPet::m_pPool);
	SAFE_DELETE(CAIKrrr::m_pPool);
	SAFE_DELETE(CAIGuard::m_pPool);
	SAFE_DELETE(CAIClockWorks::m_pPool);
	SAFE_DELETE(CAIBigMuscle::m_pPool);
	SAFE_DELETE(CAIBear::m_pPool);
#endif	// __MEM_TRACE
#endif	// __VM_0820
	UninitializeNetLib();
	UninitializeScriptLib();

#ifndef __VM_0820
#ifndef __MEM_TRACE
	SAFE_DELETE(CAIPet::m_pPool);
#endif	// __MEM_TRACE
#endif	// __VM_0820
	g_Object3DMng.Destroy();
	prj.m_modelMng.DeleteDeviceObjects();

#if __VER >= 12 // __LORD
	CSLord::Instance()->DestroyColleagues();
#endif	// __LORD
}


void SetTitle()
{
	char szLang[MAX_PATH] = { 0, };
#ifdef __LANG_1013
	sprintf(szLang, CLangMan::GetInstance()->GetLangData(::GetLanguage())->szTitle);
#else	// __LANG_1013
	switch (::GetLanguage())
	{
	case 0:	sprintf(szLang, "Korean");	break;
	case 1:	sprintf(szLang, "English");	break;
	case 2:	sprintf(szLang, "Japanese");	break;
	case 3:	sprintf(szLang, "Chinese");	break;
	case 4:	sprintf(szLang, "Thailand");	break;
	case 5:	sprintf(szLang, "Taiwan");	break;
	default:	break;
	}
#endif	// __LANG_1013

	char szTitle[256];
	sprintf(szTitle, "%s", g_szTitle, g_uKey, szLang);
	SetWindowText(g_hMainWnd, szTitle);
}

BOOL Script(LPCSTR lpszFileName)
{
	g_eLocal.SetState(EVE_RESPAWN, 1);

	CScanner s;
	if (s.Load(lpszFileName) == FALSE)
	{
		Error("Fail to open file - %s", lpszFileName);
		return FALSE;
	}

	s.GetToken();
	while (s.tok != FINISHED)
	{
		if (s.Token == "ResourceFolder")
		{
			s.GetToken();
			if (!SetCurrentDirectory(s.Token))
			{
				Error("[CRITICAL] Missing Folder -> Resource, %s", s.Token);
				return FALSE;
			}
		}
		else if (s.Token == "Key")
		{
			g_uKey = (DWORD)s.GetNumber();
#ifdef __S8_SERVER_PORT
			g_uIdofMulti = g_uKey % 100;
#else // __S8_SERVER_PORT
			g_uIdofMulti = g_uKey / 100;
#endif // __S8_SERVER_PORT
		}
		else if (s.Token == "Core")
		{
			s.GetToken();
			strcpy(g_szCoreAddr, s.Token);
		}
		else if (s.Token == "DB")
		{
			s.GetToken();
			strcpy(g_szDBAddr, s.Token);
		}
		else if (s.Token == "Account")
		{
			s.GetToken();
		}
		else if (s.Token == "Heartbeat")
		{
			s.GetToken();
			strcpy(g_sHeartbeat, s.Token);
		}
		else if (s.Token == "18")
		{
			g_eLocal.SetState(EVE_18, 1);		// 18세 서버 
		}
		else if (s.Token == "PK")
		{
			g_eLocal.SetState(EVE_PK, 1);
		}
		else if (s.Token == "PKCOST")
		{
			g_eLocal.SetState(EVE_PKCOST, 1);
		}
		else if (s.Token == "STEAL")
		{
			g_eLocal.SetState(EVE_STEAL, 1);
		}
		else if (s.Token == "GUILDBANK")
		{
			g_eLocal.SetState(ENABLE_GUILD_INVENTORY, 1);
		}
		else if (s.Token == "GUILDWAR")
		{
			g_eLocal.SetState(EVE_GUILDWAR, 1);
		}
		else if (s.Token == "DROPITEM_REMOVE")
		{
			g_eLocal.SetState(EVE_DROPITEMREMOVE, 1);
		}
		else if (s.Token == "SCHOOL")
		{
			//g_eLocal.SetState( EVE_SCHOOL, 1 );
		}
		else if (s.Token == "GUILDCOMBAT")
		{
			g_eLocal.SetState(EVE_GUILDCOMBAT, 1);
		}
		else if (s.Token == "WORMON")
		{
			g_eLocal.SetState(EVE_WORMON, 1);
		}
		else if (s.Token == "respawn")
		{
			g_eLocal.SetState(EVE_RESPAWN, (BYTE)s.GetNumber());
		}
#ifdef __S_RECOMMEND_EVE
		else if (s.Token == "RECOMMEND")
		{
			g_eLocal.SetState(EVE_RECOMMEND, 1);
		}
#endif // __S_RECOMMEND_EVE

#if __VER >= 11 // __GUILD_COMBAT_1TO1
		else if (s.Token == "GUILDCOMBAT1TO1")
		{
			g_eLocal.SetState(EVE_GUILDCOMBAT1TO1, 1);
		}
#endif // __GUILD_COMBAT_1TO1

#ifdef __JEFF_11_4
		else if (s.Token == "ARENA")
		{
			g_eLocal.SetState(EVE_ARENA, 1);
		}
#endif // __JEFF_11_4

#if __VER >= 12 // __SECRET_ROOM
		else if (s.Token == "SECRETROOM")
		{
			g_eLocal.SetState(EVE_SECRETROOM, 1);
		}
#endif // __SECRET_ROOM
#if __VER >= 13 // __RAINBOW_RACE
		else if (s.Token == "RAINBOWRACE")
		{
			g_eLocal.SetState(EVE_RAINBOWRACE, 1);
		}
#endif // __RAINBOW_RACE

		s.GetToken();
	}
	return TRUE;
}
