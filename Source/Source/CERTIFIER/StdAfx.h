// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

/* VS2010 */
#define POINTER_64 __ptr64

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define __AEGON_CHANGE_WIN32_WINNT	//| lose winxp support, gain new features like GetTickCount64

#ifdef __AEGON_CHANGE_WIN32_WINNT
#define _WIN32_WINNT 0x0600
#define	WINVER	0x0600
#else // __AEGON_CHANGE_WIN32_WINNT
/* VS2010 */
#define _WIN32_WINNT 0x0501
#define	WINVER	0x0501
#endif // __AEGON_CHANGE_WIN32_WINNT

//#define _WIN32_WINNT 0x0500
//#define	WINVER	0x0500

#define __CERTIFIER

#include <afxwin.h>
#include <mmsystem.h>

#pragma warning(disable:4786)
#include <list>
#include <map>
#include <vector>
using namespace std;

#include "VersionCommon.h"
#include "Titanium.h"
#include "memtrace.h"

#include "DefineCommon.h"
#include "CmnHdr.h"
#include "DXUtil.h"
#include "data.h"
#include "Query.h"
#include "debug.h"
#include "file.h"
#include "scanner.h"
#include "d3dfont.h"
#include "timer.h"
#include "xutil.h"
#include "vutil.h"

#include <afxdisp.h>        // MFC Automation classes

extern	BOOL	g_fInternal;



//////////////////////////////////////////////////////////////////////////
//	mulcom	BEGIN100218	패스워드 암호화.
#include "../_Common/Rijndael.h"
//	mulcom	END100218	패스워드 암호화.
//////////////////////////////////////////////////////////////////////////


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
