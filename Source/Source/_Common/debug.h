#ifndef __DEBUG_H
#define __DEBUG_H


#ifdef __IDC
	extern void DEBUGOUT2( LPCTSTR lpszStr, LPCTSTR szFileName = _T( "..\\error.txt" ) );
#if defined(__WORLDSERVER) && defined(__AEGON_INGAME_DP)
	extern void DEBUGOUTDPPURCHASE(LPCTSTR lpszStr, LPCTSTR szFileName = _T("..\\1. DPPurchaseLog.txt"));
#endif
#else
	extern void DEBUGOUT2( LPCTSTR lpszStr, LPCTSTR szFileName = _T( "error.txt" ) );
#endif

extern void FILEOUT (LPCTSTR lpszFileName,LPCTSTR lpszFormat, ...);
extern void WriteLog( LPCTSTR lpszFormat, ... );
extern void WriteError( LPCTSTR lpszFormat, ... );
#ifdef __GAMEGUARD
extern void GameGuardLog(LPCTSTR lpszStr, LPCTSTR szFileName, int nGameGuard);
#endif // __GAMEGUARD
#ifdef __ROYAL_RUMBLE
extern void LogRumble(LPCTSTR lpszFormat);
#endif //__ROYAL_RUMBLE

extern void OUTPUTDEBUGSTRING( LPCTSTR lpszFormat, ... );

extern const char* MakeFileNameDate( const char* lpszFileName );

#endif