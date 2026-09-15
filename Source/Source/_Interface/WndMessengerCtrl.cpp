#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "DPClient.h"
#include "party.h"
#include "guild.h"
#include "WndMessengerCtrl.h"
#include "WndMessenger.h"
#include "WndManager.h"
#if __VER >= 13 // __HOUSING
#include "Housing.h"
#endif // __HOUSING
#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA
#if __VER >= 15 // __CAMPUS
#include "Campus.h"
#include "CampusHelper.h"
#endif // __CAMPUS

#ifdef __PARTYLIST_MESSENGER
#include "eveschool.h"
#include "defineQuest.h"
#include "WndPartyChangeTroup.h"
extern CGuildCombat g_GuildCombatMng;
#endif // __PARTYLIST_MESSENGER

extern CDPClient	g_DPlay;
extern CParty		g_Party;

#if __VER >= 11 // __CSC_VER11_4
//////////////////////////////////////////////////////////////////////////
// Common Local Func.
//////////////////////////////////////////////////////////////////////////
bool prLevelAsce(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	int nPlayer1JobType, nPlayer2JobType;

	nPlayer1JobType = prj.m_aJob[ player1.m_nJob ].dwJobType;
	nPlayer2JobType = prj.m_aJob[ player2.m_nJob ].dwJobType;

	if(nPlayer1JobType > nPlayer2JobType)
		rtn_val = true;
	else if(nPlayer1JobType == nPlayer2JobType)
	{
		if(player1.m_nLevel > player2.m_nLevel)
			rtn_val = true;
	}
	
	return rtn_val;
}

bool prLevelDesc(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	int nPlayer1JobType, nPlayer2JobType;

	nPlayer1JobType = prj.m_aJob[ player1.m_nJob ].dwJobType;
	nPlayer2JobType = prj.m_aJob[ player2.m_nJob ].dwJobType;

	if(nPlayer1JobType < nPlayer2JobType)
		rtn_val = true;
	else if(nPlayer1JobType == nPlayer2JobType)
	{
		if(player1.m_nLevel < player2.m_nLevel)
			rtn_val = true;
	}
	
	return rtn_val;
}

bool prJobAsce(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;

	int nPlayer1JobType, nPlayer2JobType;

	nPlayer1JobType = prj.m_aJob[ player1.m_nJob ].dwJobType;
	nPlayer2JobType = prj.m_aJob[ player2.m_nJob ].dwJobType;

	if(nPlayer1JobType > nPlayer2JobType)
		rtn_val = true;
	else if(nPlayer1JobType == nPlayer2JobType)
	{
		if(player1.m_nJob > player2.m_nJob)
			rtn_val = true;
	}
	
	return rtn_val;
}

bool prJobDesc(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	int nPlayer1JobType, nPlayer2JobType;

	nPlayer1JobType = prj.m_aJob[ player1.m_nJob ].dwJobType;
	nPlayer2JobType = prj.m_aJob[ player2.m_nJob ].dwJobType;

	if(nPlayer1JobType < nPlayer2JobType)
		rtn_val = true;
	else if(nPlayer1JobType == nPlayer2JobType)
	{
		if(player1.m_nJob < player2.m_nJob)
			rtn_val = true;
	}
	
	return rtn_val;
}

bool prStatusAsce(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	int nPlayer1Status, nPlayer2Status;

	nPlayer1Status = player1.m_dwStatus;
	nPlayer2Status = player2.m_dwStatus;

	// offline 상태가 맨 마지막상태
	if(nPlayer1Status == FRS_OFFLINE)
		nPlayer1Status = 12;
	if(nPlayer2Status == FRS_OFFLINE)
		nPlayer2Status = 12;

	if(nPlayer1Status < nPlayer2Status)
		rtn_val = true;
	
	return rtn_val;
}

bool prStatusDesc(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	int nPlayer1Status, nPlayer2Status;

	nPlayer1Status = player1.m_dwStatus;
	nPlayer2Status = player2.m_dwStatus;

	// offline 상태가 맨 마지막상태
	if(nPlayer1Status == FRS_OFFLINE)
		nPlayer1Status = 12;
	if(nPlayer2Status == FRS_OFFLINE)
		nPlayer2Status = 12;

	if(nPlayer1Status > nPlayer2Status)
		rtn_val = true;
	
	return rtn_val;
	
	return rtn_val;
}

bool prChannelAsce(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;

	if(player1.m_nChannel > player2.m_nChannel)
		rtn_val = true;
	
	return rtn_val;
}

bool prChannelDesc(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;

	if(player1.m_nChannel < player2.m_nChannel)
		rtn_val = true;
	
	return rtn_val;
}

bool prNameAsce(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	CString strplayer1Name, strplayer2Name;

	strplayer1Name.Format("%s", player1.m_szName);
	strplayer2Name.Format("%s", player2.m_szName);

	if(strplayer1Name > strplayer2Name)
		rtn_val = true;
	
	return rtn_val;
}

bool prNameDesc(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	CString strplayer1Name, strplayer2Name;

	strplayer1Name.Format("%s", player1.m_szName);
	strplayer2Name.Format("%s", player2.m_szName);

	if(strplayer1Name < strplayer2Name)
		rtn_val = true;
	
	return rtn_val;
}

#if __VER >= 15 // __CAMPUS
//-----------------------------------------------------------------------------
__MESSENGER_PLAYER::__MESSENGER_PLAYER( void ) : 
m_nChannel( 0 ),
m_dwStatus( 0 ),
m_nLevel( 0 ), 
m_nJob( 0 ), 
m_dwPlayerId( 0 ), 
m_bBlock( FALSE ), 
m_bVisitAllowed( FALSE )
{
	ZeroMemory( m_szName, sizeof( m_szName ) );
}
//-----------------------------------------------------------------------------
void __MESSENGER_PLAYER::Initialize( void )
{
	m_nChannel = 0;
	m_dwStatus = 0;
	m_nLevel = 0;
	m_nJob = 0;
	m_dwPlayerId = 0;
	m_bBlock = FALSE;
	m_bVisitAllowed = FALSE;
	ZeroMemory( m_szName, sizeof( m_szName ) );
}
//-----------------------------------------------------------------------------
#endif // __CAMPUS

//////////////////////////////////////////////////////////////////////////
// Messenger Friend Tab Ctrl
//////////////////////////////////////////////////////////////////////////
#ifdef __MESSENGER_CTRL_EXTENTED
#define MAX_MEMBER_LIST_FRIEND 10
#endif // __MESSENGER_CTRL_EXTENTED
CWndFriendCtrlEx::CWndFriendCtrlEx() 
{
	m_nCurSelect = -1;
	m_nFontHeight = 20;
	m_nDrawCount = 0;
	m_bSortbyChannel = TRUE;
	m_bSortbyStatus = FALSE;
	m_bSortbyLevel = TRUE;
	m_bSortbyJob = TRUE;
	m_bSortbyName = TRUE;
	m_nCurSort = SORT_BY_STATUS;

	m_vPlayerList.clear();
}

CWndFriendCtrlEx::~CWndFriendCtrlEx()
{
}

void CWndFriendCtrlEx::Create( RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	CWndBase::Create( WBS_CHILD, rect, pParentWnd, nID );
}

void CWndFriendCtrlEx::OnInitialUpdate()
{
	CRect rect = GetWindowRect();

	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );

	m_menu.CreateMenu( this );

	UpdatePlayerList();
}

void CWndFriendCtrlEx::UpdatePlayerList()
{
	// Set Friends List
	m_vPlayerList.clear();
	__MESSENGER_PLAYER stPlayer;
#if __VER >= 13 // __HOUSING
	vector<DWORD> vecTemp;
	CHousing::GetInstance()->GetVisitAllow( vecTemp );
#endif // __HOUSING
#ifdef __RT_1025
	for( map<u_long, Friend>::iterator i	= g_WndMng.m_RTMessenger.begin(); i != g_WndMng.m_RTMessenger.end(); ++i )
	{
		u_long idPlayer	= i->first;
		Friend* pFriend		= &i->second;
		PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( idPlayer );
		int nJob	= pPlayerData->data.nJob;
		int nLevel	= pPlayerData->data.nLevel;
		u_long uLogin	= pPlayerData->data.uLogin;
		LPCSTR lpszPlayer	= pPlayerData->szPlayer;
		DWORD dwState	= pFriend->dwState;
#else	// __RT_1025
	C2FriendPtr::iterator iter = g_WndMng.m_Messenger.m_aFriend.begin();
	for( ; iter != g_WndMng.m_Messenger.m_aFriend.end(); ++iter )
	{
		LPFRIEND lpFriend = (LPFRIEND)iter->second;
		u_long idPlayer	= lpFriend->dwUserId;
		DWORD dwState	= lpFriend->dwState;
		int nJob	= lpFriend->nJob;
		int nLevel	= 0;
		u_long uLogin	= lpFriend->m_uIdofMulti;
		LPCSTR lpszPlayer	= lpFriend->szName;
#endif	// __RT_1025
		stPlayer.m_dwPlayerId	= idPlayer;
		stPlayer.m_dwStatus		= dwState;
		stPlayer.m_nJob		= nJob;
		stPlayer.m_nLevel	= nLevel;
#ifdef __RT_1025
		stPlayer.m_bBlock	= pFriend->bBlock;
#endif	// __RT_1025
#if __VER >= 13 // __HOUSING
		stPlayer.m_bVisitAllowed = FALSE;
		vector<DWORD>::iterator iterV = vecTemp.begin();
		for( ; iterV != vecTemp.end(); ++iterV)
		{
			if(idPlayer == *iterV)
				stPlayer.m_bVisitAllowed = TRUE;
		}
#endif // __HOUSING
		if( stPlayer.m_dwStatus == FRS_OFFLINE )
			stPlayer.m_nChannel	= 100;
		else
			stPlayer.m_nChannel	= uLogin;

		lstrcpy( stPlayer.m_szName, lpszPlayer );

		m_vPlayerList.push_back( stPlayer );
	}

	switch(m_nCurSort)
	{
		case SORT_BY_CHANNEL:
			SortbyChannel(FALSE);
			break;
		case SORT_BY_STATUS:
			SortbyStatus(FALSE);
			break;
		case SORT_BY_LEVEL:
			SortbyLevel(FALSE);
			break;
		case SORT_BY_JOB:
			SortbyJob(FALSE);
			break;
		case SORT_BY_NAME:
			SortbyName(FALSE);
			break;
	}
}

void CWndFriendCtrlEx::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor3 );
}

void CWndFriendCtrlEx::OnDraw( C2DRender* p2DRender ) 
{
	CPoint pt( 3, 3 );
	m_nDrawCount = 0;

	if( NULL == g_pPlayer )
		return;

#ifdef __AEGON_THEME_SWITCHER
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	DWORD dwColorHighlightGreen = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_GREEN);
#endif

	int nMax = GetDrawCount();
	m_nDrawCount = m_wndScrollBar.GetScrollPos();

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[ 6 * 4 * nMax ];
	TEXTUREVERTEX2* pVertices = pVertex;
	
	vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
	
	for(int i=0; iter<m_vPlayerList.end(); i++, iter++)
	{
		CString strFormat;

		if( i < m_nDrawCount )
			continue;
		if( i >= nMax )
		{
			iter = m_vPlayerList.end();
			continue;
		}
		
		__MESSENGER_PLAYER stPlayer = *(iter);
		
		// Draw Channel Icon
		if( stPlayer.m_nChannel != 100 && stPlayer.m_nChannel > 0 && stPlayer.m_nChannel < 11)
		{
#ifdef __MESSENGER_CTRL_EXTENTED
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 15, pt.y ), 34 + stPlayer.m_nChannel - 1, &pVertices, 0xffffffff );
#else // __MESSENGER_CTRL_EXTENTED
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 20, pt.y ), 34 + stPlayer.m_nChannel - 1, &pVertices, 0xffffffff );
#endif // __MESSENGER_CTRL_EXTENTED
		}

		// Draw Status Icon
		DWORD dwMyState;
		if( stPlayer.m_dwStatus == FRS_AUTOABSENT )
			dwMyState = FRS_ABSENT;
		else if( stPlayer.m_dwStatus == FRS_ONLINE )
			dwMyState = 2;
		else if( stPlayer.m_dwStatus == FRS_OFFLINE )
			dwMyState = 8;
		else
			dwMyState = stPlayer.m_dwStatus;
#ifdef __MESSENGER_CTRL_EXTENTED
#if __VER >= 15 // __CAMPUS
		pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(65, pt.y), 7 + (dwMyState - 2), &pVertices, 0xffffffff);
#else // __CAMPUS
		pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(63, pt.y), 7 + (dwMyState - 2), &pVertices, 0xffffffff);
#endif // __CAMPUS
#else // __MESSENGER_CTRL_EXTENTED
#if __VER >= 15 // __CAMPUS
		pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(76, pt.y), 7 + (dwMyState - 2), &pVertices, 0xffffffff);
#else // __CAMPUS
		pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(74, pt.y), 7 + (dwMyState - 2), &pVertices, 0xffffffff);
#endif // __CAMPUS
#endif // __MESSENGER_CTRL_EXTENTED

		// Draw Level
		strFormat.Format("%d", stPlayer.m_nLevel);
#if __VER >= 15 // __CAMPUS
#ifdef __MESSENGER_CTRL_EXTENTED
		static const int LEVEL_TEXT_X = 114;
#else // __MESSENGER_CTRL_EXTENTED
		static const int LEVEL_TEXT_X = 127;
#endif // __MESSENGER_CTRL_EXTENTED
#ifdef __AEGON_THEME_SWITCHER

		if (stPlayer.m_bVisitAllowed)
			p2DRender->TextOut(LEVEL_TEXT_X, pt.y + 3, strFormat, dwColorHighlightGreen);
		else
			p2DRender->TextOut(LEVEL_TEXT_X, pt.y + 3, strFormat, dwColorText);
#else
		if (stPlayer.m_bVisitAllowed)
			p2DRender->TextOut(LEVEL_TEXT_X, pt.y + 3, strFormat, 0xff00ff00);
		else
			p2DRender->TextOut(LEVEL_TEXT_X, pt.y + 3, strFormat, 0xff000000);
#endif
		
#else // __CAMPUS
#if __VER >= 13 // __HOUSING
		if(stPlayer.m_bVisitAllowed)
		{
#ifdef __MESSENGER_CTRL_EXTENTED
			p2DRender->TextOut( LEVEL_TEXT_X-5, pt.y + 3, strFormat, 0xff00ff00 );	// 초대허용한 사람은 녹색으로
#else // __MESSENGER_CTRL_EXTENTED
			p2DRender->TextOut( 122, pt.y + 3, strFormat, 0xff00ff00 );	// 초대허용한 사람은 녹색으로
#endif // __MESSENGER_CTRL_EXTENTED
		}
		else
#endif // __HOUSING
		{
#ifdef __MESSENGER_CTRL_EXTENTED
		p2DRender->TextOut(LEVEL_TEXT_X - 5, pt.y + 3, strFormat, 0xff000000);
#else // __MESSENGER_CTRL_EXTENTED
		p2DRender->TextOut( 122, pt.y + 3, strFormat, 0xff000000 );
#endif // __MESSENGER_CTRL_EXTENTED
		}
#endif // __CAMPUS

		// Draw Job Icon
#if __VER >= 15 // __CAMPUS
#ifdef __MESSENGER_CTRL_EXTENTED
		static const int JOB_TYPE_ICON_X = 157;
#else // __MESSENGER_CTRL_EXTENTED
		static const int JOB_TYPE_ICON_X = 174;
#endif // __MESSENGER_CTRL_EXTENTED
		static const int JOB_ICON_X = JOB_TYPE_ICON_X + 20;
		if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_PRO )
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 6 ), &pVertices, 0xffffffff );
		else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_MASTER )
		{
			int nMasterIndex = 27;
			if( stPlayer.m_nLevel < 70 )
				nMasterIndex = 27;
			else if( stPlayer.m_nLevel >= 70 && stPlayer.m_nLevel < 80 )
				nMasterIndex = 28;
			else if( stPlayer.m_nLevel >= 80 && stPlayer.m_nLevel < 90 )
				nMasterIndex = 29;
			else if( stPlayer.m_nLevel >= 90 && stPlayer.m_nLevel < 100 )
				nMasterIndex = 30;
			else if( stPlayer.m_nLevel >= 100 && stPlayer.m_nLevel < 110 )
				nMasterIndex = 31;
			else if( stPlayer.m_nLevel >= 110 && stPlayer.m_nLevel <= 120 )
				nMasterIndex = 32;

			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  nMasterIndex, &pVertices, 0xffffffff );
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 16 ), &pVertices, 0xffffffff );
		}
		else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_HERO )
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  33, &pVertices, 0xffffffff );
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 24 ), &pVertices, 0xffffffff );
		}
#ifdef __3RD_LEGEND16
		else if (prj.m_aJob[stPlayer.m_nJob].dwJobType == JTYPE_LEGEND_HERO)
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(JOB_TYPE_ICON_X, pt.y), 33, &pVertices, 0xffffffff);
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(JOB_ICON_X, pt.y), (19 + stPlayer.m_nJob - 32), &pVertices, 0xffffffff);
		}
#endif // __3RD_LEGEND16
		else
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  14 + stPlayer.m_nJob, &pVertices, 0xffffffff );
#else // __CAMPUS
		if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_PRO )			
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 188, pt.y ),  ( 19 + stPlayer.m_nJob - 6 ), &pVertices, 0xffffffff );
		else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_MASTER )
		{
			int nMasterIndex = 27;
			if(/*m_nLevel >= 60 && */stPlayer.m_nLevel < 70) //Level Down될 경우를 생각해서 주석처리.
				nMasterIndex = 27;
			else if(stPlayer.m_nLevel >= 70 && stPlayer.m_nLevel < 80)
				nMasterIndex = 28;
			else if(stPlayer.m_nLevel >= 80 && stPlayer.m_nLevel < 90)
				nMasterIndex = 29;
			else if(stPlayer.m_nLevel >= 90 && stPlayer.m_nLevel < 100)
				nMasterIndex = 30;
			else if(stPlayer.m_nLevel >= 100 && stPlayer.m_nLevel < 110)
				nMasterIndex = 31;
			else if(stPlayer.m_nLevel >= 110 && stPlayer.m_nLevel <= 120)
				nMasterIndex = 32;

			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 166, pt.y ),  nMasterIndex, &pVertices, 0xffffffff );
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 188, pt.y ),  ( 19 + stPlayer.m_nJob - 16 ), &pVertices, 0xffffffff );
		}
		else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_HERO )
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 166, pt.y ),  33, &pVertices, 0xffffffff );
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 188, pt.y ),  ( 19 + stPlayer.m_nJob - 24 ), &pVertices, 0xffffffff );
		}
#ifdef __3RD_LEGEND16
		else if (prj.m_aJob[stPlayer.m_nJob].dwJobType == JTYPE_LEGEND_HERO)
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(166, pt.y), 33, &pVertices, 0xffffffff);
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(188, pt.y), (19 + stPlayer.m_nJob - 32), &pVertices, 0xffffffff);
		}
#endif // __3RD_LEGEND16
		else
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 188, pt.y ),  14 + stPlayer.m_nJob, &pVertices, 0xffffffff );
#endif // __CAMPUS

		// Draw Name
#ifdef __AEGON_THEME_SWITCHER
		DWORD dwColor = dwColorText;
#else
		DWORD dwColor = 0xff000000;
#endif
		if( i == m_nCurSelect )
#ifdef __AEGON_THEME_SWITCHER
			dwColor = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE);
#else
			dwColor = 0xff6060ff;
#endif
		
#ifdef __RT_1025
		if( stPlayer.m_bBlock )
#else	// __RT_1025
		if( stPlayer.m_dwStatus == FRS_OFFLINEBLOCK )
#endif	// __RT_1025
		{
			dwColor = COLOR_ERROR;
		}
		
		strFormat.Format("%s", stPlayer.m_szName);
#ifdef __MESSENGER_CTRL_EXTENTED
		if( strFormat.GetLength() > 9 ) 
#else // __MESSENGER_CTRL_EXTENTED
		if( strFormat.GetLength() > 10 ) 
#endif //  __MESSENGER_CTRL_EXTENTED
		{
			int	nReduceCount = 0;

#ifdef __MESSENGER_CTRL_EXTENTED
			for (nReduceCount = 0; nReduceCount < 9; )
#else // __MESSENGER_CTRL_EXTENTED
			for (nReduceCount = 0; nReduceCount < 10; )
#endif // __MESSENGER_CTRL_EXTENTED
			{
				if( IsDBCSLeadByte( strFormat[ nReduceCount ] ) )
					nReduceCount+=2;
				else
					nReduceCount++;
			}

			strFormat = strFormat.Left( nReduceCount );
			strFormat += "...";
		}
#if __VER >= 15 // __CAMPUS
#ifdef __MESSENGER_CTRL_EXTENTED
		const int NAME_TEXT_X = 211;
#else // __MESSENGER_CTRL_EXTENTED
		const NAME_TEXT_X = 226;
#endif // __MESSENGER_CTRL_EXTENTED
		const int NAME_TEXT_Y = pt.y + 3;
		if( stPlayer.m_bVisitAllowed )
#ifdef __AEGON_THEME_SWITCHER
			p2DRender->TextOut(NAME_TEXT_X, NAME_TEXT_Y, strFormat, dwColorHighlightGreen);
#else
			p2DRender->TextOut( NAME_TEXT_X, NAME_TEXT_Y, strFormat, 0xff00ff00 );
#endif
		else
			p2DRender->TextOut( NAME_TEXT_X, NAME_TEXT_Y, strFormat, dwColor );
#else // __CAMPUS
#if __VER >= 13 // __HOUSING
		if (stPlayer.m_bVisitAllowed)
		{
#ifdef __MESSENGER_CTRL_EXTENTED
			p2DRender->TextOut(205, pt.y + 3, strFormat, 0xff00ff00);	// 초대허용한 사람은 녹색으로
#else // __MESSENGER_CTRL_EXTENTED
			p2DRender->TextOut(220, pt.y + 3, strFormat, 0xff00ff00);	// 초대허용한 사람은 녹색으로
#endif // __MESSENGER_CTRL_EXTENTED
		}
		else
#endif // __HOUSING
		{
#ifdef __MESSENGER_CTRL_EXTENTED
			p2DRender->TextOut(205, pt.y + 3, strFormat, dwColor);
#else // __MESSENGER_CTRL_EXTENTED
			p2DRender->TextOut(220, pt.y + 3, strFormat, dwColor);
#endif // __MESSENGER_CTRL_EXTENTED
		}
#endif // __CAMPUS
		
		pt.y += m_nFontHeight;
	}
#ifdef _WIN64
	pWndWorld->m_texPlayerDataIcon.Render(m_pApp->m_pd3dDevice, pVertex, (int)(((__int64)pVertices - (__int64)pVertex) / sizeof(TEXTUREVERTEX2)));
#else
	pWndWorld->m_texPlayerDataIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
#endif
	SAFE_DELETE_ARRAY( pVertex );
}

void CWndFriendCtrlEx::OnMouseMove(UINT nFlags, CPoint point)
{
#ifdef __MESSENGER_CTRL_EXTENTED
	CPoint pt(173, 3);
#else // __MESSENGER_CTRL_EXTENTED
	CPoint pt(188, 3);
#endif // __MESSENGER_CTRL_EXTENTED
	CRect rect;

	int nCount = m_vPlayerList.size();
	if(nCount > 10)
		nCount = 10;

	for( int j=0; j<nCount; j++)
	{
		rect.SetRect( pt.x, pt.y, pt.x + 24, pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
			int nPos = m_wndScrollBar.GetScrollPos();
			iter += j + nPos;
			__MESSENGER_PLAYER stPlayer	= *(iter);
			ClientToScreen( &point );
			ClientToScreen( &rect );
			g_toolTip.PutToolTip( 100, prj.m_aJob[ stPlayer.m_nJob ].szName, rect, point, 3 );
			j = m_vPlayerList.size();
		}

#ifdef __MESSENGER_CTRL_EXTENTED
		CRect rect2;
		rect2.SetRect(pt.x + 40, pt.y, pt.x + 105, pt.y + m_nFontHeight);
		if (rect2.PtInRect(point))
		{
			vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
			int nPos = m_wndScrollBar.GetScrollPos();
			iter += j + nPos;
			__MESSENGER_PLAYER stPlayer = *(iter);
			CPoint point2 = point;
			ClientToScreen(&point2);
			ClientToScreen(&rect2);

			int nPlayerId = stPlayer.m_dwPlayerId;
			CString str;

#ifdef __AEGON_LAST_ONLINE
			str.Format("%s", CAegonLastOnlineMng::GetInstance()->GetLastOnlineString(nPlayerId).GetString() );
#endif // __AEGON_LAST_ONLINE

			g_toolTip.PutToolTip(m_nIdWnd, str, rect2, point2);
			j = m_vPlayerList.size();
		}
#endif // __MESSENGER_CTRL_EXTENTED

		pt.y += m_nFontHeight;
	}
}

void CWndFriendCtrlEx::OnLButtonUp( UINT nFlags, CPoint point )
{
	CPoint pt( 3, 3 );
	CRect rect;

	for( int j=0; j<(int)( m_vPlayerList.size() ); j++)
	{
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			int nPos = m_wndScrollBar.GetScrollPos();
			m_nCurSelect = j + nPos;
			return;
		}
		pt.y += m_nFontHeight;
	}
	
	g_WorldMng()->SetObjFocus(NULL);
}

void CWndFriendCtrlEx::OnLButtonDblClk( UINT nFlags, CPoint point )
{
#ifdef __RT_1025
	u_long idPlayer;
	Friend* pFriend = NULL;
	int nSelect		= GetSelect( point, idPlayer, &pFriend );
	if( nSelect != -1 && pFriend )
#else	// __RT_1025
	LPFRIEND lpFriend = NULL;
	int nSelect = GetSelect( point, &lpFriend );
	if( nSelect != -1 && lpFriend)
#endif	// __RT_1025
	{

#ifdef __AEGON_MESSENGER
		g_WndMng.OpenMessage(CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer));
#else

#ifdef __RT_1025
		DWORD dwState	= pFriend->dwState;
		if( dwState != FRS_OFFLINE && !pFriend->bBlock )
#else	// __RT_1025
		u_long idPlayer		= lpFriend->dwUserId;
		DWORD dwState	= lpFriend->dwState;
		if( dwState != FRS_OFFLINE && dwState != FRS_BLOCK && dwState != FRS_OFFLINEBLOCK )
#endif	// __RT_1025
		{
			m_nCurSelect = nSelect;
#if __VER >= 11 // __SYS_PLAYER_DATA
			CWndMessage* pWndMessage	= g_WndMng.OpenMessage( CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer ) );
#else	// __SYS_PLAYER_DATA
			CWndMessage* pWndMessage	= g_WndMng.OpenMessage( lpFriend->szName );
#endif	// __SYS_PLAYER_DATA
		}
		else
		{
			CString szMessage;
			if( dwState == FRS_OFFLINE )
				szMessage = prj.GetText(TID_GAME_NOTLOGIN);                               //"??? 님은 접속되어 있지 않습니다";
			else
#if __VER >= 11 // __SYS_PLAYER_DATA
				szMessage.Format( prj.GetText(TID_GAME_MSGBLOCKCHR), CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer ) );  //"??? 님은 차단되어 있어 메세지를 보낼수 없습니다";
#else	// __SYS_PLAYER_DATA
				szMessage.Format( prj.GetText(TID_GAME_MSGBLOCKCHR), lpFriend->szName );  //"??? 님은 차단되어 있어 메세지를 보낼수 없습니다";
#endif	// __SYS_PLAYER_DATA
			g_WndMng.PutString( szMessage, NULL, prj.GetTextColor( TID_GAME_NOTLOGIN ) );		
		}
#endif
	}	

}

#ifdef __RT_1025
void	CWndFriendCtrlEx::GetSelectFriend( int SelectCount, u_long & idPlayer, Friend** ppFriend )
#else	// __RT_1025
void CWndFriendCtrlEx::GetSelectFriend( int SelectCount, LPFRIEND* lppFriend )
#endif	// __RT_1025
{
	*ppFriend	= NULL;
	vector < __MESSENGER_PLAYER >::iterator iter	= m_vPlayerList.begin();
	iter	+= SelectCount;
	__MESSENGER_PLAYER stPlayer	= *(iter);
#ifdef __RT_1025
	idPlayer	= stPlayer.m_dwPlayerId;
	*ppFriend	= g_WndMng.m_RTMessenger.GetFriend( stPlayer.m_dwPlayerId );
#else	// __RT_1025
	*lppFriend = g_WndMng.m_Messenger.GetFriend( stPlayer.m_dwPlayerId );
#endif	// __RT_1025
}

u_long CWndFriendCtrlEx::GetSelectId( int SelectCount )
{
	vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
	iter += SelectCount;
	__MESSENGER_PLAYER stPlayer = *(iter);
	
	return stPlayer.m_dwPlayerId;
}

#ifdef __RT_1025
int	CWndFriendCtrlEx::GetSelect( CPoint point, u_long &idPlayer, Friend** ppFriend )
#else	// __RT_1025
int CWndFriendCtrlEx::GetSelect( CPoint point, LPFRIEND* lppFriend )
#endif	// __RT_1025
{
	CPoint pt( 3, 3 );
	CRect rect;
	int rtn_val = -1;
	
	for( int j=0; j<(int)( m_vPlayerList.size() ); j++)
	{
		rtn_val++;
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
			int nPos = m_wndScrollBar.GetScrollPos();
			iter += j + nPos;
			rtn_val += nPos;
			__MESSENGER_PLAYER stPlayer	= *(iter);
#ifdef __RT_1025
			idPlayer = stPlayer.m_dwPlayerId;
			*ppFriend = g_WndMng.m_RTMessenger.GetFriend( stPlayer.m_dwPlayerId );
#else	// __RT_1025
			*lppFriend = g_WndMng.m_Messenger.GetFriend( stPlayer.m_dwPlayerId );
#endif	// __RT_1025
			j = m_vPlayerList.size();
		}
		pt.y += m_nFontHeight;
	}

	return rtn_val;
}

#ifdef __MESSENGER_CTRL_EXTENTED
int	CWndFriendCtrlEx::GetSelectGuildMember(CPoint point, u_long& idPlayer, CGuildMember** lppGuildMember)
{
	CPoint pt(3, 3);
	CRect rect;
	int rtn_val = -1;

	CGuild* pGuild = g_pPlayer->GetGuild();
	if (pGuild)
	{
		for (int j = 0; j < (int)(m_vPlayerList.size()); j++)
		{
			rtn_val++;
			rect.SetRect(pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight);
			if (rect.PtInRect(point))
			{
				vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
				int nPos = m_wndScrollBar.GetScrollPos();
				iter += j + nPos;
				rtn_val += nPos;
				__MESSENGER_PLAYER stPlayer = *(iter);
				idPlayer = stPlayer.m_dwPlayerId;
				CGuildMember* lpGuildMember = pGuild->GetMember(idPlayer);
				if (lpGuildMember)
					*lppGuildMember = lpGuildMember;
				j = m_vPlayerList.size();
			}
			pt.y += m_nFontHeight;
		}
	}
	return rtn_val;
}
#endif // __MESSENGER_CTRL_EXTENTED

BOOL CWndFriendCtrlEx::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	m_menu.SetVisible(FALSE);

#ifdef __RT_1025
	u_long idPlayer;
#endif	// __RT_1025

	switch( nID )
	{
	case 0:		// 메시지
		{
#ifdef __RT_1025
			Friend* pFriend		= NULL;
			GetSelectFriend( m_nCurSelect, idPlayer, &pFriend );
			if( pFriend )
				CWndMessage* pWndMessage	= g_WndMng.OpenMessage( CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer ) );
#else	// __RT_1025
			LPFRIEND lpFriend = NULL;
			GetSelectFriend( m_nCurSelect, &lpFriend );
			if( lpFriend )
				CWndMessage* pWndMessage = g_WndMng.OpenMessage( lpFriend->szName );				
#endif	// __RT_1025
		}
		break;
	case 1:		// 차단 / 차단해제
		{
			u_long uidPlayer = GetSelectId( m_nCurSelect );
			if( uidPlayer != -1 )
				g_DPlay.SendFriendInterceptState( uidPlayer );		
		}
		break;
	case 2:		// 삭제
		{
			u_long uidPlayer = GetSelectId( m_nCurSelect );
			if( uidPlayer != -1 )
				g_DPlay.SendRemoveFriend( uidPlayer );
		}
		break;
	case 3:		// 이동
		{
#ifdef __MESSENGER_CTRL_EXTENTED
		Friend* pFriend = NULL;
		GetSelectFriend(m_nCurSelect, idPlayer, &pFriend);
		if (pFriend)
		{
			CString string;
			string.Format("/teleport \"%s\"", CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer));
			ParsingCommand(string.LockBuffer(), g_pPlayer);
			string.UnlockBuffer();
		}
#else // __MESSENGER_CTRL_EXTENTED
#ifdef __RT_1025
			Friend* pFriend		= NULL;
			GetSelectFriend( m_nCurSelect, idPlayer, &pFriend );
			if( pFriend )
			{
				CString string;
				string.Format( "/teleport \"%s\"", CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer ) );
				ParsingCommand( string.LockBuffer(), g_pPlayer );
				string.UnlockBuffer();
			}
#else	// __RT_1025
			LPFRIEND lpFriend = NULL;
			GetSelectFriend( m_nCurSelect, &lpFriend );
			if( lpFriend )
			{
				CString string;
				string.Format( "/teleport \"%s\"", CPlayerDataCenter::GetInstance()->GetPlayerString( lpFriend->dwUserId ) );
				ParsingCommand( string.LockBuffer(), g_pPlayer );
				string.UnlockBuffer();			
			}
#endif	// __RT_1025
#endif // __MESSENGER_CTRL_EXTENTED
		}
		break;
	case 4:		// 극단 초청
		{
			u_long uidPlayer = GetSelectId( m_nCurSelect );
			if( uidPlayer != -1 )
			{
				CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
				if( pWndWorld )
					pWndWorld->InviteParty( uidPlayer );
			}
		}
		break;

#ifdef __MESSENGER_CTRL_EXTENTED
	case 5:
	{
		u_long uidPlayer = GetSelectId(m_nCurSelect);
		if (uidPlayer != -1)
		{
			CString string;
			string.Format("/summon \"%s\"", CPlayerDataCenter::GetInstance()->GetPlayerString(uidPlayer));
			ParsingCommand(string.LockBuffer(), g_pPlayer);
			string.UnlockBuffer();
		}
	}
	break;
#endif // __MESSENGER_CTRL_EXTENTED

	case 6:		// 쪽지 보내기
		{
#ifdef __RT_1025
			Friend* pFriend		= NULL;
			GetSelectFriend( m_nCurSelect, idPlayer, &pFriend );
			if( pFriend )
			{
				SAFE_DELETE( g_WndMng.m_pWndMessageNote );
				g_WndMng.m_pWndMessageNote	= new CWndMessageNote;
				strcpy( g_WndMng.m_pWndMessageNote->m_szName, CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer ) );
				g_WndMng.m_pWndMessageNote->m_dwUserId	= idPlayer;
				g_WndMng.m_pWndMessageNote->Initialize();
			}
#else	// __RT_1025
			LPFRIEND lpFriend = NULL;
			GetSelectFriend( m_nCurSelect, &lpFriend );
			if( lpFriend )
			{
				SAFE_DELETE( g_WndMng.m_pWndMessageNote );
				g_WndMng.m_pWndMessageNote = new CWndMessageNote;
				strcpy( g_WndMng.m_pWndMessageNote->m_szName, lpFriend->szName );
				g_WndMng.m_pWndMessageNote->m_dwUserId = lpFriend->dwUserId;
				g_WndMng.m_pWndMessageNote->Initialize();		
			}
#endif	// __RT_1025
		}
		break;
#if __VER >= 13 // __HOUSING
		case 7 :	// 입장허가를 취소한다
			{
				u_long uidPlayer = GetSelectId( m_nCurSelect );
				if(uidPlayer > 0)
					g_DPlay.SendHousingReqSetVisitAllow(uidPlayer, FALSE);
			}
			break;
		case 8:		// 입장을 허가한다
			{
				u_long uidPlayer = GetSelectId( m_nCurSelect );
				if(uidPlayer > 0)
					g_DPlay.SendHousingReqSetVisitAllow(uidPlayer, TRUE);
			}
			break;
#endif // __HOUSING

#ifdef __MESSENGER_CTRL_EXTENTED
		case 9:
		{
			Friend* pFriend = NULL;
			GetSelectFriend(m_nCurSelect, idPlayer, &pFriend);
			if (pFriend)
			{
				CString string;
				string.Format("/GuildInvite \"%s\"", CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer));
				ParsingCommand(string.LockBuffer(), g_pPlayer);
				string.UnlockBuffer();
			}
		}
		break;
#endif // __MESSENGER_CTRL_EXTENTED

	}
	return -1; 
} 

void CWndFriendCtrlEx::OnRButtonUp( UINT nFlags, CPoint point )
{
#ifdef __RT_1025
	u_long idPlayer;
	Friend* pFriend		= NULL;
	int nSelect		= GetSelect( point, idPlayer, &pFriend );
#else	// __RT_1025
	LPFRIEND lpFriend = NULL;
	int nSelect = GetSelect( point, &lpFriend );
#endif	// __RT_1025

#ifdef __RT_1025
	if( nSelect != -1 && pFriend != NULL )
#else //__RT_1025
	if( nSelect != -1 )
#endif //__RT_1025
	{
#ifdef __RT_1025
		DWORD dwState	= pFriend->dwState;
#else	// __RT_1025
		u_long idPlayer	= lpFriend->dwUserId;
		DWORD dwState	= lpFriend->dwState;
#endif	// __RT_1025
		m_nCurSelect	= nSelect;
		ClientToScreen( &point );
		m_menu.DeleteAllMenu();
#ifdef __RT_1025
		if( dwState != FRS_OFFLINE && !pFriend->bBlock )
#else	// __RT_1025
		if( dwState != FRS_OFFLINE && dwState != FRS_BLOCK && dwState != FRS_OFFLINEBLOCK )
#endif	// __RT_1025
		{
#ifdef __MESSENGER_CTRL_EXTENTED
			m_menu.AppendMenu(0, 0, prj.GetText(TID_MESSENGER_GENERAL_MESSAGE));
#else // __MESSENGER_CTRL_EXTENTED
			m_menu.AppendMenu(0, 0, _T(prj.GetText(TID_APP_MESSAGE)));
#endif // __MESSENGER_CTRL_EXTENTED
		}
#ifdef __RT_1025
		if( pFriend->bBlock )
#else	// __RT_1025
		if( dwState == FRS_BLOCK || dwState == FRS_OFFLINEBLOCK )
#endif	// __RT_1025
		{
#ifdef __MESSENGER_CTRL_EXTENTED
			m_menu.AppendMenu(0, 1, prj.GetText(TID_MESSENGER_FRIEND_BLOCKRESTORE));
#else // __MESSENGER_CTRL_EXTENTED
			m_menu.AppendMenu(0, 1, _T(prj.GetText(TID_FRS_BLOCKRESTORE)));
#endif // __MESSENGER_CTRL_EXTENTED
		}
		else
		{
#ifdef __MESSENGER_CTRL_EXTENTED
			m_menu.AppendMenu(0, 1, prj.GetText(TID_MESSENGER_FRIEND_BLOCK));
#else // __MESSENGER_CTRL_EXTENTED
			m_menu.AppendMenu(0, 1, _T(prj.GetText(TID_FRS_BLOCK)));
#endif // __MESSENGER_CTRL_EXTENTED
		}

#ifdef __MESSENGER_CTRL_EXTENTED
		m_menu.AppendMenu(0, 2, prj.GetText(TID_MESSENGER_FRIEND_DELETE));
#else // __MESSENGER_CTRL_EXTENTED
		m_menu.AppendMenu(0, 2, _T(prj.GetText(TID_FRS_DELETE)));
#endif // __MESSENGER_CTRL_EXTENTED

#ifdef __RT_1025
		if( dwState != FRS_OFFLINE && !pFriend->bBlock )
#else	// __RT_1025
		if( dwState != FRS_OFFLINE && dwState != FRS_OFFLINEBLOCK )
#endif	// __RT_1025
		{	
#ifdef __MESSENGER_CTRL_EXTENTED
			if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
			{
				m_menu.AppendMenu(0, 3, prj.GetText(TID_MESSENGER_ADMIN_APPROCHER));
				m_menu.AppendMenu(0, 5, prj.GetText(TID_MESSENGER_ADMIN_RAMENER));
			}
#else // __MESSENGER_CTRL_EXTENTED
			if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
				m_menu.AppendMenu( 0, 3 ,_T( prj.GetText( TID_FRS_MOVE2 ) ) );
#endif // __MESSENGER_CTRL_EXTENTED
#ifdef __RT_1025
			if( !pFriend->bBlock )
#else	// __RT_1025
			if( dwState != FRS_BLOCK )
#endif	// __RT_1025
			{
				if( g_Party.IsMember( idPlayer ) == FALSE )
				{
#ifdef __MESSENGER_CTRL_EXTENTED
					m_menu.AppendMenu(0, 4, prj.GetText(TID_MESSENGER_PARTY_INVIT));
#else // __MESSENGER_CTRL_EXTENTED
					m_menu.AppendMenu(0, 4, _T(prj.GetText(TID_MMI_INVITE_PARTY)));
#endif // __MESSENGER_CTRL_EXTENTED
				}
			}
		}
#ifdef __RT_1025
		if( dwState == FRS_OFFLINE || pFriend->bBlock )
#else	// __RT_1025
		if( dwState == FRS_OFFLINE || dwState == FRS_BLOCK || dwState == FRS_OFFLINEBLOCK )
#endif	// __RT_1025
		{
#ifdef __MESSENGER_CTRL_EXTENTED

#ifndef __AEGON_MESSENGER
			m_menu.AppendMenu(0, 6, prj.GetText(TID_MESSENGER_GENERAL_OFFLINEMESSAGE));
#endif

#else // __MESSENGER_CTRL_EXTENTED
			m_menu.AppendMenu(0, 6, _T(prj.GetText(TID_GAME_TAGSEND)));
#endif // __MESSENGER_CTRL_EXTENTED
		}

#if __VER >= 13 // __HOUSING
		vector<__MESSENGER_PLAYER>::iterator iter = m_vPlayerList.begin();
		int		nCount		= 0;
		BOOL	bIsAllowed	= FALSE;
		for(; iter != m_vPlayerList.end(); ++iter)
		{
			if(idPlayer == iter->m_dwPlayerId)
			{
				if(iter->m_bVisitAllowed)	
					bIsAllowed = TRUE;
			}
		}
#ifdef __MESSENGER_CTRL_EXTENTED
		if (bIsAllowed)
			m_menu.AppendMenu(0, 7, prj.GetText(TID_MESSENGER_STUDIO_REFUSE));
		else
			m_menu.AppendMenu(0, 8, prj.GetText(TID_MESSENGER_STUDIO_ACCEPT));
#else // __MESSENGER_CTRL_EXTENTED
		if (bIsAllowed)
			m_menu.AppendMenu(0, 7, _T(prj.GetText(TID_GAME_PROHIBIT_VISIT)));
		else
			m_menu.AppendMenu(0, 8, _T(prj.GetText(TID_GAME_ALLOW_VISIT)));
#endif // __MESSENGER_CTRL_EXTENTED
#endif // __HOUSING

#ifdef __MESSENGER_CTRL_EXTENTED
		if (dwState != FRS_OFFLINE && !pFriend->bBlock)
		{
			CGuild* pGuild = g_pPlayer->GetGuild();
			if (pGuild)
			{
				CGuildMember* pMember = pGuild->GetMember(g_pPlayer->m_idPlayer);
				if (pMember)
				{
					if (pGuild->m_adwPower[pMember->m_nMemberLv] & PF_INVITATION)
						m_menu.AppendMenu(0, 9, prj.GetText(TID_MESSENGER_GUILDE_INVITE));
				}
			}
		}
#endif // __MESSENGER_CTRL_EXTENTED

		m_menu.Move( point );
		m_menu.SetVisible( TRUE );
		m_menu.SetFocus();
	}
}

void CWndFriendCtrlEx::SetScrollBar()
{
	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
#ifdef __RT_1025
	nRange	= g_WndMng.m_RTMessenger.size();
#else	// __RT_1025
	nRange = g_WndMng.m_Messenger.m_aFriend.size();
#endif	// __RT_1025
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
}

void CWndFriendCtrlEx::ScrollBarPos( int nPos )
{
	m_wndScrollBar.SetScrollPos( nPos );
}

int CWndFriendCtrlEx::GetDrawCount( void )
{
#ifdef __RT_1025
	int nMax	= g_WndMng.m_RTMessenger.size();
#else	// __RT_1025
	int nMax = g_WndMng.m_Messenger.m_aFriend.size();
#endif	// __RT_1025
	if( nMax - m_wndScrollBar.GetScrollPos() > m_wndScrollBar.GetScrollPage() )
		nMax = m_wndScrollBar.GetScrollPage() + m_wndScrollBar.GetScrollPos();
	if( nMax < m_wndScrollBar.GetScrollPos() )
		nMax = 0;

	return nMax;
}

void CWndFriendCtrlEx::OnSize( UINT nType, int cx, int cy )
{
	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;
	m_wndScrollBar.SetWndRect( rect );

	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
#ifdef __RT_1025
	nRange	= g_WndMng.m_RTMessenger.size();
#else	// __RT_1025
	nRange = g_WndMng.m_Messenger.m_aFriend.size();
#endif	// __RT_1025
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
	
	CWndBase::OnSize( nType, cx, cy);
}
void CWndFriendCtrlEx::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 3, 3 );

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}

#ifdef __MESSENGER_CTRL_EXTENTED
BOOL CWndFriendCtrlEx::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int		nZoom = 64;
	int nCount = m_vPlayerList.size();

	if (zDelta > 0)
		m_nCurrentList -= 1;
	else
		m_nCurrentList += 1;

	if (m_nCurrentList < 0)
		m_nCurrentList = 0;

	if ((m_nCurrentList + MAX_MEMBER_LIST_FRIEND - 1) >= nCount)
	{
		m_nCurrentList = nCount - MAX_MEMBER_LIST_FRIEND;
		if (m_nCurrentList < 0)
			m_nCurrentList = 0;
	}

	m_wndScrollBar.SetScrollPos(m_nCurrentList);
	return TRUE;
}
#endif // __MESSENGER_CTRL_EXTENTED

void CWndFriendCtrlEx::SortbyChannel(BOOL bCheckbefore)
{
	if(!bCheckbefore)
		m_bSortbyChannel = !m_bSortbyChannel;

	if(m_bSortbyChannel)
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prChannelAsce );
		m_bSortbyChannel = FALSE;
	}
	else
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prChannelDesc );
		m_bSortbyChannel = TRUE;
	}
	m_nCurSort = SORT_BY_CHANNEL;
}

void CWndFriendCtrlEx::SortbyStatus(BOOL bCheckbefore)
{
	if(!bCheckbefore)
		m_bSortbyStatus = !m_bSortbyStatus;

	if(m_bSortbyStatus)
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prStatusAsce );
		m_bSortbyStatus = FALSE;
	}
	else
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prStatusDesc );
		m_bSortbyStatus = TRUE;
	}
	m_nCurSort = SORT_BY_STATUS;
}

void CWndFriendCtrlEx::SortbyLevel(BOOL bCheckbefore)
{
	if(!bCheckbefore)
		m_bSortbyLevel = !m_bSortbyLevel;

	if(m_bSortbyLevel)
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prLevelAsce );
		m_bSortbyLevel = FALSE;
	}
	else
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prLevelDesc );
		m_bSortbyLevel = TRUE;
	}
	m_nCurSort = SORT_BY_LEVEL;
}

void CWndFriendCtrlEx::SortbyJob(BOOL bCheckbefore)
{
	if(!bCheckbefore)
		m_bSortbyJob = !m_bSortbyJob;

	if(m_bSortbyJob)
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prJobAsce );
		m_bSortbyJob = FALSE;
	}
	else
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prJobDesc );
		m_bSortbyJob = TRUE;
	}
	m_nCurSort = SORT_BY_JOB;
}

void CWndFriendCtrlEx::SortbyName(BOOL bCheckbefore)
{
	if(!bCheckbefore)
		m_bSortbyName = !m_bSortbyName;

	if(m_bSortbyName)
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prNameAsce );
		m_bSortbyName = FALSE;
	}
	else
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prNameDesc );
		m_bSortbyName = TRUE;
	}
	m_nCurSort = SORT_BY_NAME;
}

#ifdef __PARTYLIST_MESSENGER
//////////////////////////////////////////////////////////////////////////
// Messenger Party Tab Ctrl
//////////////////////////////////////////////////////////////////////////
#define MAX_MEMBER_LIST_PARTY 7

CWndPartyCtrlEx::CWndPartyCtrlEx() 
{
	m_nCurSelect = -1;
	m_bDrag = FALSE;
	m_pVBGauge = NULL;
	m_nFontHeight = 30;
}

CWndPartyCtrlEx::~CWndPartyCtrlEx()
{
	DeleteDeviceObjects();
}

void CWndPartyCtrlEx::Create( DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	m_dwListCtrlStyle = dwListCtrlStyle;
	CWndBase::Create( WBS_CHILD, rect, pParentWnd, nID );
}

void CWndPartyCtrlEx::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 2, 2 );

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}

HRESULT CWndPartyCtrlEx::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();

	if( m_pVBGauge == NULL )
		return m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	
	return S_OK;
}
HRESULT CWndPartyCtrlEx::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();

	m_texGauEmptyNormal.Invalidate();
	m_texGauFillNormal.Invalidate();
	
    SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
HRESULT CWndPartyCtrlEx::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();

	m_texGauEmptyNormal.DeleteDeviceObjects();
	m_texGauFillNormal.DeleteDeviceObjects();
	SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}

void CWndPartyCtrlEx::OnInitialUpdate()
{

	m_menu.CreateMenu( this );	

	CRect rect = GetWindowRect();
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );

	RestoreDeviceObjects();
	m_texGauEmptyNormal.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "GauEmptySmall.bmp"), COLOR_ITEM_BG, TRUE);
	m_texGauFillNormal.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "GauFillSmall.bmp"), COLOR_ITEM_BG, TRUE);

}

void CWndPartyCtrlEx::OnDraw( C2DRender* p2DRender ) 
{
	if( NULL == g_pPlayer )
		return;

#ifdef __AEGON_THEME_SWITCHER
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
#endif

	CPoint pt( 2, 2 );

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );

	int nMax = g_Party.m_nSizeofMember;

	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[ 6 * 4 * nMax ];
	TEXTUREVERTEX2* pVertices = pVertex;

	for( int i = m_wndScrollBar.GetScrollPos(); i < nMax; i++ ) 
	{

		CMover* pObjMember = prj.GetUserByID( g_Party.m_aMember[i].m_uPlayerId );
		CString strMember;
		CString strLevel;

		PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( g_Party.m_aMember[i].m_uPlayerId );
		int nJob				= pPlayerData->data.nJob;
		int nLvl				= pPlayerData->data.nLevel;
		int nLogin				= pPlayerData->data.uLogin;
		u_long  nLeadMember		= g_Party.m_aMember[i].m_uPlayerId;
		int nMyState;

		if( nLogin > 0 && nLogin < 11 )
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 15, pt.y ), 34 + ( nLogin - 1 ), &pVertices, 0xffffffff );
			nMyState = 2;
		}
		else
			nMyState = 8;

		pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 65, pt.y ), 7 + ( nMyState - 2 ), &pVertices, 0xffffffff );
#ifdef __AEGON_THEME_SWITCHER
		DWORD dwColor = dwColorText;
#else
		DWORD dwColor = 0xff000000;
#endif

		if( IsValidObj( pObjMember ))
		{
			if( pObjMember->GetHitPoint() == 0 ) 
				dwColor = COLOR_ERROR;
			else if( ((FLOAT)pObjMember->GetHitPoint()) / ((FLOAT)pObjMember->GetMaxHitPoint()) < 0.1f ) 
				dwColor = 0xffffff00;
		}
		else
		{
#ifdef __AEGON_THEME_SWITCHER
			DWORD dwColor = dwColorText;
#else
			DWORD dwColor = 0xff000000;
#endif

			if( g_Party.m_aMember[ i ].m_bRemove ) 
				dwColor = 0xff878787;
		}

		if( i == m_nCurSelect )
#ifdef __AEGON_THEME_SWITCHER
			dwColor = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE);
#else
			dwColor = 0xff6060ff;
#endif

		if(g_Party.IsLeader(nLeadMember))
			dwColor = 0xff1fb72d;

		strLevel.Format( "%d", nLvl );
		p2DRender->TextOut( 114, pt.y + 3, strLevel, dwColor ); 
		strMember.Format( "%s", pPlayerData->szPlayer );
		if( strMember.GetLength() > 9 ) 
		{
			int	nReduceCount = 0;

			for( nReduceCount = 0; nReduceCount < 9; )
			{
				if( IsDBCSLeadByte( strMember[ nReduceCount ] ) )
					nReduceCount += 2;
				else
					nReduceCount++;
			}

			strMember = strMember.Left( nReduceCount );
			strMember += "...";
		}
		p2DRender->TextOut( 211, pt.y + 3, strMember, dwColor ); 
		int x = 0, nWidth = m_rectClient.Width() - 10;// - 1;
		CRect rect( x, pt.y, x + nWidth, pt.y + m_nFontHeight );
		rect.SetRect( x + 6, pt.y + 18, x + nWidth - 10, pt.y + 30 ); 
		nWidth	= pObjMember ? pObjMember->GetHitPointPercent( rect.Width() ) : 0;
		
		CRect rectTemp = rect; 
		rectTemp.right = rectTemp.left + nWidth;
		if( rect.right < rectTemp.right )
			rectTemp.right = rect.right;

		m_pTheme->RenderGauge( p2DRender, &rect, 0xffffffff, m_pVBGauge, &m_texGauEmptyNormal );
		m_pTheme->RenderGauge( p2DRender, &rectTemp, 0x64ff0000, m_pVBGauge, &m_texGauFillNormal );

		rect.SetRect( x + 3, pt.y, x + 3 + 30, pt.y + 6 + 30 ); 

		static const int JOB_TYPE_ICON_X = 157;
		static const int JOB_ICON_X = JOB_TYPE_ICON_X + 20;
		if( prj.m_aJob[ nJob ].dwJobType == JTYPE_PRO )
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + nJob - 6 ), &pVertices, 0xffffffff );
		else if( prj.m_aJob[ nJob ].dwJobType == JTYPE_MASTER )
		{
			int nMasterIndex = 27;
			if( nLvl < 70 )
				nMasterIndex = 27;
			else if( nLvl >= 70 && nLvl < 80 )
				nMasterIndex = 28;
			else if( nLvl >= 80 && nLvl < 90 )
				nMasterIndex = 29;
			else if( nLvl >= 90 && nLvl < 100 )
				nMasterIndex = 30;
			else if( nLvl >= 100 && nLvl < 110 )
				nMasterIndex = 31;
			else if( nLvl >= 110 && nLvl <= 120 )
				nMasterIndex = 32;

			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  nMasterIndex, &pVertices, 0xffffffff );
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + nJob - 16 ), &pVertices, 0xffffffff );
		}
		else if( prj.m_aJob[ nJob ].dwJobType == JTYPE_HERO )
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  33, &pVertices, 0xffffffff );
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + nJob - 24 ), &pVertices, 0xffffffff );
		}
#ifdef __3RD_LEGEND16
		else if( prj.m_aJob[ nJob ].dwJobType == JTYPE_LEGEND_HERO )
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  33, &pVertices, 0xffffffff );
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + nJob - 32 ), &pVertices, 0xffffffff );
		}
#endif // __3RD_LEGEND16
		else
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  14 + nJob, &pVertices, 0xffffffff );
		pt.y += m_nFontHeight;
	}
#ifdef _WIN64
	pWndWorld->m_texPlayerDataIcon.Render(m_pApp->m_pd3dDevice, pVertex, (int)(((__int64)pVertices - (__int64)pVertex) / sizeof(TEXTUREVERTEX2)));
#else
	pWndWorld->m_texPlayerDataIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
#endif
	SAFE_DELETE_ARRAY( pVertex );
}

void CWndPartyCtrlEx::OnLButtonUp( UINT nFlags, CPoint point )
{
	m_bDrag = FALSE;

	CPoint pt( 2, 2 );
	CRect rect;

	pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
	for( int i = 0; i < g_Party.m_nSizeofMember; i++ ) 
	{
		CMover* pObjMember = prj.GetUserByID( g_Party.m_aMember[i].m_uPlayerId );
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			CMover* pObjMember = prj.GetUserByID( g_Party.m_aMember[ i ].m_uPlayerId );
			m_nCurSelect = i;
			if( g_pPlayer != pObjMember ) 
			{
				if( IsValidObj(pObjMember ) ) 
				{
					g_WorldMng()->SetObjFocus( pObjMember );
					return;
				}
			}
		}
		pt.y += m_nFontHeight;
	}
	g_WorldMng()->SetObjFocus(NULL);
}

void CWndPartyCtrlEx::OnRButtonUp( UINT nFlags, CPoint point )
{
	CPoint pt( 1, 1 );
	CRect rect;
	u_long idPlayer;
	Friend* pFriend		= NULL;
	
	int nMax = g_Party.m_nSizeofMember;

	if( nMax - m_wndScrollBar.GetScrollPos() > m_wndScrollBar.GetScrollPage() )
		nMax = m_wndScrollBar.GetScrollPage();

	for( int i = m_wndScrollBar.GetScrollPos(); i < nMax; i++ ) 
	{

		CMover* pObjMember = prj.GetUserByID( g_Party.m_aMember[i].m_uPlayerId );
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );

		if( rect.PtInRect( point ) )
		{
			m_nCurSelect = i;

			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &rect );
			m_menu.DeleteAllMenu();
			
			CMover* pObjMember = prj.GetUserByID( g_Party.m_aMember[ i ].m_uPlayerId );
			PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( g_Party.m_aMember[i].m_uPlayerId );

			if( pPlayerData && pPlayerData->data.uLogin > 0 )
			{	
				if( g_pPlayer != pObjMember )
				{
					int nSelectFriend	= GetSelectFriend( point, idPlayer, &pFriend );
					if( nSelectFriend != -1 && pFriend != NULL )
					{
						if( !pFriend->bBlock )
						{
							m_menu.AppendMenu(0, 0, prj.GetText(TID_MESSENGER_GENERAL_MESSAGE));
							m_menu.AppendMenu(0, 1, prj.GetText(TID_MESSENGER_FRIEND_BLOCK));
						}
						else
						{
							m_menu.AppendMenu(0, 1, prj.GetText(TID_MESSENGER_FRIEND_BLOCKRESTORE));
						}
					}
					
					else
					{
						m_menu.AppendMenu(0, 0, prj.GetText(TID_MESSENGER_GENERAL_MESSAGE));
					}

					if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
					{
						m_menu.AppendMenu(0, 3, prj.GetText(TID_MESSENGER_ADMIN_APPROCHER));
						m_menu.AppendMenu(0, 4, prj.GetText(TID_MESSENGER_ADMIN_RAMENER));
					}

					CGuild* pGuild	= g_pPlayer->GetGuild();
					if( pGuild )
					{
						CGuildMember* pMember	= pGuild->GetMember( g_pPlayer->m_idPlayer );
						if( pMember )
						{
							if( pGuild->m_adwPower[pMember->m_nMemberLv] & PF_INVITATION )
								m_menu.AppendMenu(0, 9, prj.GetText(TID_MESSENGER_GUILDE_INVITE));
						}
					}

					if ( g_Party.IsLeader( g_pPlayer->m_idPlayer ) )
					{
						m_menu.AppendMenu(0, 10, prj.GetText(TID_MESSENGER_PARTY_LEAVEOTHER));
						m_menu.AppendMenu(0, 11, prj.GetText(TID_MESSENGER_PARTY_GIVELEAD));
					}
				}
				else if( g_pPlayer == pObjMember )
				{
					m_menu.AppendMenu(0, 10, prj.GetText(TID_MESSENGER_PARTY_LEAVEMYSELF));
				}
			}
			else
			{
				Friend* pFriend		= NULL;
				int nSelectFriend	= GetSelectFriend( point, idPlayer, &pFriend );

				if( nSelectFriend != -1 && pFriend != NULL )
				{
					if( !pFriend->bBlock )
					{					
#ifndef __AEGON_MESSENGER
						m_menu.AppendMenu(0, 5, prj.GetText(TID_MESSENGER_GENERAL_OFFLINEMESSAGE));
#endif
						m_menu.AppendMenu(0, 1, prj.GetText(TID_MESSENGER_FRIEND_BLOCK));
					}
					else
					{
						m_menu.AppendMenu(0, 1, prj.GetText(TID_MESSENGER_FRIEND_BLOCKRESTORE));
					}
				}
				else if(nSelectFriend == -1 || pFriend == NULL )
				{
#ifndef __AEGON_MESSENGER
					m_menu.AppendMenu(0, 5, prj.GetText(TID_MESSENGER_GENERAL_OFFLINEMESSAGE));
#endif
				}
			}
			m_menu.Move( point2 );
			m_menu.SetVisible( TRUE );
			m_menu.SetFocus();
		}
		pt.y += m_nFontHeight + 1;
	}
}

void CWndPartyCtrlEx::OnLButtonDown( UINT nFlags, CPoint point )
{

}

void CWndPartyCtrlEx::OnRButtonDown( UINT nFlags, CPoint point )
{

}

void CWndPartyCtrlEx::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	CPoint pt( 2, 2 );
	CRect rect;
	u_long idPlayer;
	
	int nMax = g_Party.m_nSizeofMember;
	// 눈에 보이는 갯수가 페이지라인수 보다 크면 보이는 갯수를 페이지라인수로 조정 
	if( nMax - m_wndScrollBar.GetScrollPos() > m_wndScrollBar.GetScrollPage() )
		nMax = m_wndScrollBar.GetScrollPage();
	
	for( int i = m_wndScrollBar.GetScrollPos(); i < nMax; i++ ) 
	{
		CMover* pObjMember = prj.GetUserByID( g_Party.m_aMember[i].m_uPlayerId );
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			m_nCurSelect = i;

			PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( g_Party.m_aMember[i].m_uPlayerId );
#ifdef __AEGON_MESSENGER
			if(pPlayerData)
#else
			if( pPlayerData && pPlayerData->data.uLogin > 0 )
#endif
			{	
				if( g_pPlayer != pObjMember )
				{
					Friend* pFriend		= NULL;
					int nSelectFriend	= GetSelectFriend( point, idPlayer, &pFriend );

					if( nSelectFriend != -1 && pFriend)
					{
#ifdef __AEGON_MESSENGER
						g_WndMng.OpenMessage(CPlayerDataCenter::GetInstance()->GetPlayerString(g_Party.m_aMember[i].m_uPlayerId));
#else
						if( !pFriend->bBlock )
						{
							CWndMessage* pWndMessage = g_WndMng.OpenMessage( CPlayerDataCenter::GetInstance()->GetPlayerString( g_Party.m_aMember[ i ].m_uPlayerId ) );
						}
						else
						{
							DWORD dwState	= pFriend->dwState;
							CString szMessage;
							if( dwState == FRS_OFFLINE )
								szMessage = prj.GetText(TID_GAME_NOTLOGIN);   
							else
								szMessage.Format( prj.GetText(TID_GAME_MSGBLOCKCHR), CPlayerDataCenter::GetInstance()->GetPlayerString( g_Party.m_aMember[ i ].m_uPlayerId ) );

							g_WndMng.PutString( szMessage, NULL, prj.GetTextColor( TID_GAME_NOTLOGIN ) );
						}
#endif
					}
					else
					{
						CWndMessage* pWndMessage = g_WndMng.OpenMessage( CPlayerDataCenter::GetInstance()->GetPlayerString( g_Party.m_aMember[ i ].m_uPlayerId ) );
					}
				}
				else if( g_pPlayer == pObjMember )
				{
					return;
				}
			}
			else
			{
				CString szMessage;
				szMessage = prj.GetText( TID_GAME_NOTLOGIN );                               
				g_WndMng.PutString( szMessage, NULL, prj.GetTextColor(TID_GAME_NOTLOGIN) );
			}
		}
		pt.y += m_nFontHeight;
	}
}

void CWndPartyCtrlEx::OnRButtonDblClk( UINT nFlags, CPoint point)
{
}

BOOL CWndPartyCtrlEx::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	m_menu.SetVisible(FALSE);


	switch( nID )
	{
		case 0:
		{	

			u_long uidPlayer = GetSelectId( m_nCurSelect );
			if( uidPlayer != -1 )
					CWndMessage* pWndMessage	= g_WndMng.OpenMessage( CPlayerDataCenter::GetInstance()->GetPlayerString( uidPlayer ) );
		}
		break;
		case 1:
		{
			u_long uidPlayer = GetSelectId( m_nCurSelect );
			if( uidPlayer != -1 )
				g_DPlay.SendFriendInterceptState( uidPlayer );	
		}
		break;

		case 3: 
		{
			u_long uidPlayer = GetSelectId( m_nCurSelect );
			if( uidPlayer != -1 )
			{
				CString string;
				string.Format( "/teleport \"%s\"", CPlayerDataCenter::GetInstance()->GetPlayerString( uidPlayer ) );
				ParsingCommand( string.LockBuffer(), g_pPlayer );
				string.UnlockBuffer();
			}
		}
		break;
		case 4: 
		{
			u_long uidPlayer = GetSelectId( m_nCurSelect );
			if( uidPlayer != -1 )
			{
				CString string;
				string.Format( "/summon \"%s\"", CPlayerDataCenter::GetInstance()->GetPlayerString( uidPlayer ) );
				ParsingCommand( string.LockBuffer(), g_pPlayer );
				string.UnlockBuffer();
			}
		}
		break;
		case 5: 
		{
			u_long uidPlayer = GetSelectId( m_nCurSelect );
			if( uidPlayer != -1 )
			{
				SAFE_DELETE( g_WndMng.m_pWndMessageNote );
				g_WndMng.m_pWndMessageNote	= new CWndMessageNote;
				strcpy( g_WndMng.m_pWndMessageNote->m_szName, CPlayerDataCenter::GetInstance()->GetPlayerString( uidPlayer ) );
				g_WndMng.m_pWndMessageNote->m_dwUserId	= uidPlayer;
				g_WndMng.m_pWndMessageNote->Initialize();
			}
		}
		break;

		case 9:
		{
			u_long uidPlayer = GetSelectId( m_nCurSelect );
			if( uidPlayer != -1 )
			{
				CString string;
				string.Format( "/GuildInvite \"%s\"", CPlayerDataCenter::GetInstance()->GetPlayerString( uidPlayer ) );
				ParsingCommand( string.LockBuffer(), g_pPlayer );
				string.UnlockBuffer();
			}
		}
		break;

		case 10:
		{
			u_long uidPlayer = GetSelectId( m_nCurSelect );

			if( g_pPlayer->m_nDuel == 2 )
			{
				DWORD dwText = TID_GAME_PPVP_LEAVE;
				g_WndMng.PutString( prj.GetText( dwText ), NULL, prj.GetTextColor( dwText ) );
			} 
			else
			{
				if(uidPlayer != -1) 
				{
					if( g_Party.m_nSizeofMember != 0 )
					{
						CWndPartyLeaveConfirm* pWndPartyLeaveConfirm = (CWndPartyLeaveConfirm*)g_WndMng.GetWndBase( APP_PARTYLEAVE_CONFIRM );
						if( pWndPartyLeaveConfirm == NULL )
						{
							g_WndMng.m_pWndPartyLeaveConfirm = new CWndPartyLeaveConfirm;
							g_WndMng.m_pWndPartyLeaveConfirm->Initialize( NULL, APP_PARTYLEAVE_CONFIRM );
							if( g_Party.m_aMember[0].m_uPlayerId == g_pPlayer->m_idPlayer )
								g_WndMng.m_pWndPartyLeaveConfirm->SetLeaveId( uidPlayer );
							else
								g_WndMng.m_pWndPartyLeaveConfirm->SetLeaveId( g_pPlayer->m_idPlayer );
						}
					}
				}
				else
				{
					CWndPartyLeaveConfirm* pWndPartyLeaveConfirm = (CWndPartyLeaveConfirm*)g_WndMng.GetWndBase( APP_PARTYLEAVE_CONFIRM );
					if( pWndPartyLeaveConfirm == NULL )
					{
						g_WndMng.m_pWndPartyLeaveConfirm = new CWndPartyLeaveConfirm;
						g_WndMng.m_pWndPartyLeaveConfirm->Initialize( NULL, APP_PARTYLEAVE_CONFIRM );
						if( g_Party.m_nSizeofMember != 0 )
							g_WndMng.m_pWndPartyLeaveConfirm->SetLeaveId( g_pPlayer->m_idPlayer );
					}
				}
				uidPlayer =- 1;
			}
		}
		break;

		case 11:
		{

			u_long uidPlayer = GetSelectId( m_nCurSelect );

			if( g_Party.m_nSizeofMember != 0 )
			{
				if( g_Party.IsLeader( g_pPlayer->m_idPlayer ) && uidPlayer != -1 )
				{
					if( g_Party.m_aMember[uidPlayer].m_bRemove == FALSE )
					{
						if( g_Party.IsLeader( uidPlayer ) == FALSE )
						{
							if( g_pPlayer->m_nDuel == 2 )
								g_WndMng.PutString( prj.GetText( TID_PK_NOCHANGE_PARTYLEADER ), NULL, prj.GetTextColor( TID_PK_NOCHANGE_PARTYLEADER ) );
							else
								g_DPlay.SendPartyChangeLeader( g_pPlayer->m_idPlayer, uidPlayer );						
						}
						else
						{
							g_WndMng.PutString( prj.GetText( TID_GAME_PARTY_ALREADYMASTER ), NULL, prj.GetTextColor( TID_GAME_PARTY_ALREADYMASTER ) ); // "이미 단장을 하고 입니다"
						}
					}
					else
					{
						g_WndMng.PutString( prj.GetText( TID_GAME_MASTER_AWAY ), NULL, prj.GetTextColor( TID_GAME_MASTER_AWAY ) ); // "단장이 아니거나 극단창에서 단원을 선택을 해야 합니다"					
					}
				}
				else
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_PARTY_ISNOTMASTER ), NULL, prj.GetTextColor( TID_GAME_PARTY_ISNOTMASTER ) ); // "단장이 아니거나 극단창에서 단원을 선택을 해야 합니다"
				}
			}

		}
		break;

	}
	return -1; 
}

void CWndPartyCtrlEx::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint pt( 173, 3 );
	CRect rect;

	for( int i = 0; i < g_Party.m_nSizeofMember; i++ ) 
	{
	
		CMover* pObjMember = prj.GetUserByID( g_Party.m_aMember[i].m_uPlayerId );
		PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(g_Party.m_aMember[i].m_uPlayerId);
		rect.SetRect( pt.x, pt.y, pt.x + 24, pt.y + 24 );
		if( rect.PtInRect( point ) )
		{
			ClientToScreen( &point );
			ClientToScreen( &rect );
			g_toolTip.PutToolTip( 100, prj.m_aJob[ pPlayerData->data.nJob ].szName, rect, point, 3 );
			i = g_Party.m_nSizeofMember;
		}

#ifdef __MESSENGER_CTRL_EXTENTED
		CRect rect2;
		rect2.SetRect(pt.x + 40, pt.y, pt.x + 105, pt.y + m_nFontHeight);
		if (rect2.PtInRect(point))
		{
			vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
			int nPos = m_wndScrollBar.GetScrollPos();
			iter += i + nPos;
			__MESSENGER_PLAYER stPlayer = *(iter);
			CPoint point2 = point;
			ClientToScreen(&point2);
			ClientToScreen(&rect2);

			CString str;

#ifdef __PARTY_ASSISTANT
			CString PartyRank;
			u_long  nPartyAssist = g_Party.m_aMember[i].m_uPlayerId;
			BOOL nAssist = pPlayerData->data.nAssist;
			
			if (g_Party.IsLeader(nPartyAssist))
			{
				PartyRank = "Party Leader";
			}
			else
			{
				if (nAssist == 1)
				{
					PartyRank = "Party Assist";
				}
				else
				{
					PartyRank = "Party Member";
				}
			}
#endif //__PARTY_ASSISTANT

#ifdef __AEGON_LAST_ONLINE
			str.Format("%s\n%s", PartyRank, CAegonLastOnlineMng::GetInstance()->GetLastOnlineString(g_Party.m_aMember[i].m_uPlayerId).GetString());
#endif // __AEGON_LAST_ONLINE

			g_toolTip.PutToolTip(m_nIdWnd, str, rect2, point2);
			i = g_Party.m_nSizeofMember;
		}
#endif // __MESSENGER_CTRL_EXTENTED

		pt.y += m_nFontHeight;
	}
}

BOOL CWndPartyCtrlEx::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	int		nZoom = 64;
	int nCount = g_Party.m_nSizeofMember;
	
	if( zDelta > 0 )
		m_nCurrentList -= 1;
	else
		m_nCurrentList += 1;

	if( m_nCurrentList < 0 )
		m_nCurrentList = 0;
	if( (m_nCurrentList + MAX_MEMBER_LIST_PARTY - 1) >= nCount )
	{
		m_nCurrentList = nCount - MAX_MEMBER_LIST_PARTY;
		if( m_nCurrentList < 0 )
			m_nCurrentList = 0;
	}

	m_wndScrollBar.SetScrollPos( m_nCurrentList );
	return TRUE;
}

void CWndPartyCtrlEx::OnSize( UINT nType, int cx, int cy )
{
	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;
	m_wndScrollBar.SetWndRect( rect );

	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
	nRange = g_Party.m_nSizeofMember;
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );

	CWndBase::OnSize( nType, cx, cy);
}

void CWndPartyCtrlEx::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );		//D3DCOLOR_TEMP( 255,   0,   0,  50 );
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );	//D3DCOLOR_TEMP( 255,  80,  80, 120 );
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );	//D3DCOLOR_TEMP( 255,  80,  80, 120 );

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 2 , 2 );
	p2DRender->RenderRect( rect, dwColor2 );
	rect.DeflateRect( 2 , 2 );
	p2DRender->RenderRect( rect, dwColor3 );

	int nPage, nRange;
	if( 1 ) 
	{
		m_nFontHeight = 30;
		nPage = GetClientRect().Height() / m_nFontHeight;
		nRange = g_Party.m_nSizeofMember;
	}
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
}


void CWndPartyCtrlEx::SetScrollBar()
{
	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
	nRange	= g_Party.m_nSizeofMember;
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
}

void CWndPartyCtrlEx::ScrollBarPos( int nPos )
{
	m_wndScrollBar.SetScrollPos( nPos );
}

u_long CWndPartyCtrlEx::GetSelectId( int SelectCount )
{

	int nMax = g_Party.m_nSizeofMember;
	int nGetScrollPos = m_wndScrollBar.GetScrollPos();

	if( nMax - m_wndScrollBar.GetScrollPos() > m_wndScrollBar.GetScrollPage() )
		nMax = m_wndScrollBar.GetScrollPage();

	if( nGetScrollPos <= nMax )
	{
		nGetScrollPos += SelectCount;
		return g_Party.m_aMember[nGetScrollPos].m_uPlayerId ;
	}
	else
	{
		nGetScrollPos = SelectCount + nMax;
		return g_Party.m_aMember[nGetScrollPos].m_uPlayerId ;
	}

}

int	CWndPartyCtrlEx::GetSelectGuildMember ( CPoint point, u_long & idPlayer, CGuildMember** ppGuildMember )
{
	CPoint pt( 2, 2 );
	CRect rect;
	int rtn_val = -1;
	
	CGuild* pGuild = g_pPlayer->GetGuild();

	if(pGuild)
	{
		int nMax = g_Party.m_nSizeofMember;
		for( int i = 0; i < nMax; i++ ) 
		{
			rtn_val++;
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
			if( rect.PtInRect( point ) )
			{
				int nPos = m_wndScrollBar.GetScrollPos();
				rtn_val += nPos;
				idPlayer = g_Party.m_aMember[i].m_uPlayerId;
				*ppGuildMember = pGuild->GetMember( g_Party.m_aMember[i].m_uPlayerId );
				i = m_vPlayerList.size();
			}
			pt.y += m_nFontHeight;
		}
	}
	return rtn_val;
}

int	CWndPartyCtrlEx::GetSelectFriend( CPoint point, u_long &idPlayer, Friend** ppFriend )
{
	CPoint pt( 2, 2 );
	CRect rect;
	int rtn_val = -1;

	int nMax = g_Party.m_nSizeofMember;
	for( int i = 0; i < nMax; i++ ) 
	{
		rtn_val++;
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			int nPos = m_wndScrollBar.GetScrollPos();
			rtn_val += nPos;
			idPlayer = g_Party.m_aMember[i].m_uPlayerId;
			*ppFriend = g_WndMng.m_RTMessenger.GetFriend( g_Party.m_aMember[i].m_uPlayerId );
			i = g_Party.m_nSizeofMember;
		}
		pt.y += m_nFontHeight;
	}

	return rtn_val;
}
#endif // __PARTYLIST_MESSENGER

//////////////////////////////////////////////////////////////////////////
// Messenger Guild Tab Ctrl
//////////////////////////////////////////////////////////////////////////
#ifdef __MESSENGER_CTRL_EXTENTED
#define MAX_MEMBER_LIST_GUILD 10
#endif // __MESSENGER_CTRL_EXTENTED
CWndGuildCtrlEx::CWndGuildCtrlEx() 
{
	m_nCurSelect = -1;
	m_nFontHeight = 20;
	m_nDrawCount = 0;
	m_bSortbyChannel = TRUE;
	m_bSortbyStatus = FALSE;
	m_bSortbyLevel = TRUE;
	m_bSortbyJob = TRUE;
	m_bSortbyName = TRUE;
	m_nCurSort = SORT_BY_STATUS;

	m_vPlayerList.clear();
}

CWndGuildCtrlEx::~CWndGuildCtrlEx()
{
}

void CWndGuildCtrlEx::Create( RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	CWndBase::Create( WBS_CHILD, rect, pParentWnd, nID );
}

void CWndGuildCtrlEx::OnInitialUpdate()
{
	CRect rect = GetWindowRect();

	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );

#ifdef	__MESSENGER_CTRL_EXTENTED
	m_menu.CreateMenu(this);
#endif // __MESSENGER_CTRL_EXTENTED

	UpdatePlayerList();
}

void CWndGuildCtrlEx::UpdatePlayerList()
{
	// Set GuildMemeber List
	m_vPlayerList.clear();
	CGuild* pGuild = g_pPlayer->GetGuild();
	if( pGuild )
	{
		map<u_long, CGuildMember*>::iterator iter = pGuild->m_mapPMember.begin();

		for( ; iter != pGuild->m_mapPMember.end() ; ++iter )
		{
			__MESSENGER_PLAYER stPlayer;
			CGuildMember* pGuildMember = (CGuildMember*)iter->second;
#if __VER >= 11 // __SYS_PLAYER_DATA
			PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( pGuildMember->m_idPlayer );
			stPlayer.m_nJob	= pPlayerData->data.nJob;
			stPlayer.m_nLevel = pPlayerData->data.nLevel;
			stPlayer.m_dwPlayerId = pGuildMember->m_idPlayer;
			stPlayer.m_nChannel = pPlayerData->data.uLogin;
			if( pPlayerData->data.uLogin > 0 )
				stPlayer.m_dwStatus = FRS_ONLINE;
			else
				stPlayer.m_dwStatus = FRS_OFFLINE;
#else	// __SYS_PLAYER_DATA
			stPlayer.m_nJob = pGuildMember->m_nJob;
			stPlayer.m_nLevel = pGuildMember->m_nLevel;
			stPlayer.m_dwPlayerId = pGuildMember->m_idPlayer;
			stPlayer.m_dwStatus = 0; //Guild Member는 상태를 알 수 없다???
			stPlayer.m_nChannel = pGuildMember->m_nMultiNo;
#endif	// __SYS_PLAYER_DATA
#if __VER >= 11 // __SYS_PLAYER_DATA
			lstrcpy( stPlayer.m_szName, pPlayerData->szPlayer );
#else	// __SYS_PLAYER_DATA
			strcpy(stPlayer.m_szName, prj.GetPlayerString( pGuildMember->m_idPlayer ));
#endif	// __SYS_PLAYER_DATA
			m_vPlayerList.push_back(stPlayer);
		}
	}

	switch(m_nCurSort)
	{
		case SORT_BY_CHANNEL:
			SortbyChannel(FALSE);
			break;
		case SORT_BY_STATUS:
			SortbyStatus(FALSE);
			break;
		case SORT_BY_LEVEL:
			SortbyLevel(FALSE);
			break;
		case SORT_BY_JOB:
			SortbyJob(FALSE);
			break;
		case SORT_BY_NAME:
			SortbyName(FALSE);
			break;
	}
}

#ifdef __MESSENGER_CTRL_EXTENTED
BOOL CWndGuildCtrlEx::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	m_menu.SetVisible(FALSE);

	if (g_pPlayer->m_idWar)
	{
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDWARERRORMEMBER));
		return FALSE;
	}

	switch (nID)
	{
	case 0:
	{
		u_long uidPlayer = GetSelectId(m_nCurSelect);
		if (uidPlayer != -1)
			CWndMessage* pWndMessage = g_WndMng.OpenMessage(CPlayerDataCenter::GetInstance()->GetPlayerString(uidPlayer));
	}
	break;

	case 1:
	{
		u_long uidPlayer = GetSelectId(m_nCurSelect);
		if (uidPlayer != -1)
			g_DPlay.SendFriendInterceptState(uidPlayer);
	}
	break;

	case 2:
	{
		u_long uidPlayer = GetSelectId(m_nCurSelect);
		if (uidPlayer != -1)
		{
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet(APP_WORLD);
			if (pWndWorld)
				pWndWorld->InviteParty(uidPlayer);
		}
	}
	break;

	case 3:
	{
		u_long uidPlayer = GetSelectId(m_nCurSelect);
		if (uidPlayer != -1)
		{
			CString string;
			string.Format("/teleport \"%s\"", CPlayerDataCenter::GetInstance()->GetPlayerString(uidPlayer));
			ParsingCommand(string.LockBuffer(), g_pPlayer);
			string.UnlockBuffer();
		}
	}
	break;

	case 4:
	{
		u_long uidPlayer = GetSelectId(m_nCurSelect);
		if (uidPlayer != -1)
		{
			CString string;
			string.Format("/summon \"%s\"", CPlayerDataCenter::GetInstance()->GetPlayerString(uidPlayer));
			ParsingCommand(string.LockBuffer(), g_pPlayer);
			string.UnlockBuffer();
		}
	}
	break;

	case 5:
	{
		u_long uidPlayer = GetSelectId(m_nCurSelect);
		if (uidPlayer != -1)
		{
			SAFE_DELETE(g_WndMng.m_pWndMessageNote);
			g_WndMng.m_pWndMessageNote = new CWndMessageNote;
			strcpy(g_WndMng.m_pWndMessageNote->m_szName, CPlayerDataCenter::GetInstance()->GetPlayerString(uidPlayer));
			g_WndMng.m_pWndMessageNote->m_dwUserId = uidPlayer;
			g_WndMng.m_pWndMessageNote->Initialize();
		}
	}
	break;

	case 12:
	{
		u_long uidPlayer = GetSelectId(m_nCurSelect);
		CGuild* pGuild = g_pPlayer->GetGuild();
		CGuildMember* pMember = pGuild->GetMember(uidPlayer);
		g_DPlay.SendGuildMemberLv(pGuild->m_idMaster, pMember->m_idPlayer, pMember->m_nMemberLv - 1);
	}
	break;

	case 13:
	{
		u_long uidPlayer = GetSelectId(m_nCurSelect);
		CGuild* pGuild = g_pPlayer->GetGuild();
		CGuildMember* pMember = pGuild->GetMember(uidPlayer);
		g_DPlay.SendGuildMemberLv(pGuild->m_idMaster, pMember->m_idPlayer, pMember->m_nMemberLv + 1);
	}
	break;

	case 14:
	{
		u_long uidPlayer = GetSelectId(m_nCurSelect);
		CGuild* pGuild = g_pPlayer->GetGuild();
		CGuildMember* pMember = pGuild->GetMember(uidPlayer);
		g_DPlay.SendGuildClass(pGuild->m_idMaster, pMember->m_idPlayer, 1);
	}
	break;

	case 15:
	{
		u_long uidPlayer = GetSelectId(m_nCurSelect);
		CGuild* pGuild = g_pPlayer->GetGuild();
		CGuildMember* pMember = pGuild->GetMember(uidPlayer);
		g_DPlay.SendGuildClass(pGuild->m_idMaster, pMember->m_idPlayer, 0);
	}
	break;

	case 16:
	{
		u_long uidPlayer = GetSelectId(m_nCurSelect);
		CGuild* pGuild = g_pPlayer->GetGuild();
		CGuildMember* pMember = pGuild->GetMember(uidPlayer);
		if (pMember->m_idPlayer != g_pPlayer->m_idPlayer)
		{
			PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pMember->m_idPlayer);

			if (pPlayerData && pPlayerData->data.uLogin > 0)
				g_DPlay.SendChgMaster(pMember->m_idPlayer, pMember->m_idPlayer);
			else
				g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_NOTLOGIN), MB_OK, this);
		}
	}
	break;

	case 17:
	{
		u_long uidPlayer = GetSelectId(m_nCurSelect);
		CGuild* pGuild = g_pPlayer->GetGuild();
		CGuildMember* pMember = pGuild->GetMember(uidPlayer);
		if (pGuild && pGuild->IsMaster(g_pPlayer->m_idPlayer))
		{
			if (10 > pGuild->m_nLevel)
			{
				g_WndMng.OpenMessageBox(_T(prj.GetText(TID_GAME_GUILDNOTLEVEL)));
			}
			else
			{
				SAFE_DELETE(g_WndMng.m_pWndGuildNickName);
				g_WndMng.m_pWndGuildNickName = new CWndGuildNickName;
				g_WndMng.m_pWndGuildNickName->m_idPlayer = pMember->m_idPlayer;
				g_WndMng.m_pWndGuildNickName->Initialize(&g_WndMng, APP_GUILD_NICKNAME);
			}
		}
		else
		{
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDONLYMASTER), MB_OK, this);
		}

	}
	break;

	case 18:
	{
		u_long uidPlayer = GetSelectId(m_nCurSelect);
		CGuild* pGuild = g_pPlayer->GetGuild();
		CGuildMember* pMember = pGuild->GetMember(uidPlayer);
		if (pGuild->GetQuest(QUEST_WARMON_LV1) != NULL && pGuild->GetQuest(QUEST_WARMON_LV1)->nState == QS_BEGIN)	// 클락워크 퀘스트면 길드 해체 안됨
		{
			QuestProp* pQuestProp = prj.m_aPropQuest.GetAt(QUEST_WARMON_LV1);
			if (pQuestProp)
			{
				g_WndMng.OpenMessageBox(prj.GetText(TID_GUILD_QUEST_LEAVEERROR), MB_OK, this);
			}
			return FALSE;
		}
		else
		{
			if (g_GuildCombatMng.m_bRequest && g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE && g_GuildCombatMng.m_nGCState != CGuildCombat::WAR_CLOSE_STATE)
				g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_NOT_LEAVE_GUILD));

			else if (g_GuildCombat1to1Mng.m_nState != CGuildCombat1to1Mng::GC1TO1_CLOSE)
				g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT1TO1_NOTLEAVEGUILD));
			else
			{
				g_DPlay.SendRemoveGuildMember(pMember->m_idPlayer, pMember->m_idPlayer);
			}
		}
	}
	break;
	}
	return -1;
}
#endif // __MESSENGER_CTRL_EXTENTED

void CWndGuildCtrlEx::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor3 );
}

void CWndGuildCtrlEx::OnDraw( C2DRender* p2DRender ) 
{
	CPoint pt( 3, 3 );
	m_nDrawCount = 0;

	if( NULL == g_pPlayer )
		return;

#ifdef __AEGON_THEME_SWITCHER
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
#endif

	int nMax = GetDrawCount();
	m_nDrawCount = m_wndScrollBar.GetScrollPos();

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[ 6 * 4 * nMax ];
	TEXTUREVERTEX2* pVertices = pVertex;
	
	vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
	
	for(int i=0; iter<m_vPlayerList.end(); i++, iter++)
	{
		CString strFormat;

		if( i < m_nDrawCount )
			continue;
		if( i >= nMax )
		{
			iter = m_vPlayerList.end();
			continue;
		}
		
		__MESSENGER_PLAYER stPlayer = *(iter);
		
		// Draw Channel Icon
		if (stPlayer.m_nChannel != 100 && stPlayer.m_nChannel > 0 && stPlayer.m_nChannel < 11)
		{
#ifdef __MESSENGER_CTRL_EXTENTED
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(15, pt.y), 34 + stPlayer.m_nChannel - 1, &pVertices, 0xffffffff);
#else // __MESSENGER_CTRL_EXTENTED
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(20, pt.y), 34 + stPlayer.m_nChannel - 1, &pVertices, 0xffffffff);
#endif // __MESSENGER_CTRL_EXTENTED
		}

		// Draw Status Icon
		DWORD dwMyState;
		if( stPlayer.m_dwStatus == FRS_AUTOABSENT )
			dwMyState = FRS_ABSENT;
		else if( stPlayer.m_dwStatus == FRS_ONLINE )
			dwMyState = 2;
		else if( stPlayer.m_dwStatus == FRS_OFFLINE )
			dwMyState = 8;
		else
			dwMyState = stPlayer.m_dwStatus;
		
#ifdef __MESSENGER_CTRL_EXTENTED
#if __VER >= 15 // __CAMPUS
		pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(65, pt.y), 7 + (dwMyState - 2), &pVertices, 0xffffffff);
#else // __CAMPUS
		pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(63, pt.y), 7 + (dwMyState - 2), &pVertices, 0xffffffff);
#endif // __CAMPUS
#else // __MESSENGER_CTRL_EXTENTED
#if __VER >= 15 // __CAMPUS
		pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(76, pt.y), 7 + (dwMyState - 2), &pVertices, 0xffffffff);
#else // __CAMPUS
		pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(74, pt.y), 7 + (dwMyState - 2), &pVertices, 0xffffffff);
#endif // __CAMPUS
#endif // __MESSENGER_CTRL_EXTENTED

		// Draw Level
		strFormat.Format("%d", stPlayer.m_nLevel);
#if __VER >= 15 // __CAMPUS
#ifdef __MESSENGER_CTRL_EXTENTED
#ifdef __AEGON_THEME_SWITCHER
		p2DRender->TextOut(114, pt.y + 3, strFormat, dwColorText);
#else
		p2DRender->TextOut(114, pt.y + 3, strFormat, 0xff000000);
#endif
#else // __MESSENGER_CTRL_EXTENTED
		p2DRender->TextOut(127, pt.y + 3, strFormat, 0xff000000);
#endif // __MESSENGER_CTRL_EXTENTED
#else // __CAMPUS
#ifdef __MESSENGER_CTRL_EXTENTED
		p2DRender->TextOut(109, pt.y + 3, strFormat, 0xff000000);
#else // __MESSENGER_CTRL_EXTENTED
		p2DRender->TextOut(122, pt.y + 3, strFormat, 0xff000000);
#endif // __MESSENGER_CTRL_EXTENTED
#endif // __CAMPUS

		// Draw Job Icon
#if __VER >= 15 // __CAMPUS
#ifdef __MESSENGER_CTRL_EXTENTED
		static const int JOB_TYPE_ICON_X = 157;
#else // __MESSENGER_CTRL_EXTENTED
		static const int JOB_TYPE_ICON_X = 174;
#endif // __MESSENGER_CTRL_EXTENTED
		static const int JOB_ICON_X = JOB_TYPE_ICON_X + 20;
		if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_PRO )
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 6 ), &pVertices, 0xffffffff );
		else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_MASTER )
		{
			int nMasterIndex = 27;
			if( stPlayer.m_nLevel < 70 )
				nMasterIndex = 27;
			else if( stPlayer.m_nLevel >= 70 && stPlayer.m_nLevel < 80 )
				nMasterIndex = 28;
			else if( stPlayer.m_nLevel >= 80 && stPlayer.m_nLevel < 90 )
				nMasterIndex = 29;
			else if( stPlayer.m_nLevel >= 90 && stPlayer.m_nLevel < 100 )
				nMasterIndex = 30;
			else if( stPlayer.m_nLevel >= 100 && stPlayer.m_nLevel < 110 )
				nMasterIndex = 31;
			else if( stPlayer.m_nLevel >= 110 && stPlayer.m_nLevel <= 120 )
				nMasterIndex = 32;

			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  nMasterIndex, &pVertices, 0xffffffff );
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 16 ), &pVertices, 0xffffffff );
		}
		else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_HERO )
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  33, &pVertices, 0xffffffff );
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 24 ), &pVertices, 0xffffffff );
		}
#ifdef __3RD_LEGEND16
		else if (prj.m_aJob[stPlayer.m_nJob].dwJobType == JTYPE_LEGEND_HERO)
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(JOB_TYPE_ICON_X, pt.y), 33, &pVertices, 0xffffffff);
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(JOB_ICON_X, pt.y), (19 + stPlayer.m_nJob - 32), &pVertices, 0xffffffff);
		}
#endif // __3RD_LEGEND16
		else
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( JOB_ICON_X, pt.y ),  14 + stPlayer.m_nJob, &pVertices, 0xffffffff );
#else // __CAMPUS
		if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_PRO )			
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 188, pt.y ),  ( 19 + stPlayer.m_nJob - 6 ), &pVertices, 0xffffffff );
		else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_MASTER )
		{
			int nMasterIndex = 27;
			if(/*m_nLevel >= 60 && */stPlayer.m_nLevel < 70) //Level Down될 경우를 생각해서 주석처리.
				nMasterIndex = 27;
			else if(stPlayer.m_nLevel >= 70 && stPlayer.m_nLevel < 80)
				nMasterIndex = 28;
			else if(stPlayer.m_nLevel >= 80 && stPlayer.m_nLevel < 90)
				nMasterIndex = 29;
			else if(stPlayer.m_nLevel >= 90 && stPlayer.m_nLevel < 100)
				nMasterIndex = 30;
			else if(stPlayer.m_nLevel >= 100 && stPlayer.m_nLevel < 110)
				nMasterIndex = 31;
			else if(stPlayer.m_nLevel >= 110 && stPlayer.m_nLevel <= 120)
				nMasterIndex = 32;

			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 166, pt.y ),  nMasterIndex, &pVertices, 0xffffffff );
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 188, pt.y ),  ( 19 + stPlayer.m_nJob - 16 ), &pVertices, 0xffffffff );
		}
		else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_HERO )
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 166, pt.y ),  33, &pVertices, 0xffffffff );
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 188, pt.y ),  ( 19 + stPlayer.m_nJob - 24 ), &pVertices, 0xffffffff );
		}
#ifdef __3RD_LEGEND16
		else if (prj.m_aJob[stPlayer.m_nJob].dwJobType == JTYPE_LEGEND_HERO)
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(166, pt.y), 33, &pVertices, 0xffffffff);
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(188, pt.y), (19 + stPlayer.m_nJob - 32), &pVertices, 0xffffffff);
		}
#endif // __3RD_LEGEND16
		else
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 188, pt.y ),  14 + stPlayer.m_nJob, &pVertices, 0xffffffff );
#endif // __CAMPUS

		// Draw Name
#ifdef __AEGON_THEME_SWITCHER
		DWORD dwColor = dwColorText;
#else
		DWORD dwColor = 0xff000000;
#endif
		if( i == m_nCurSelect )
#ifdef __AEGON_THEME_SWITCHER
			dwColor = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE);
#else
			dwColor = 0xff6060ff;
#endif
		
		strFormat.Format("%s", stPlayer.m_szName);
	
		if( strFormat.GetLength() > 9 ) 
		{
			int	nReduceCount = 0;

			for( nReduceCount = 0; nReduceCount < 9; )
			{
				if( IsDBCSLeadByte( strFormat[ nReduceCount ] ) )
					nReduceCount+=2;
				else
					nReduceCount++;
			}

			strFormat = strFormat.Left( nReduceCount );
			strFormat += "...";
		}

#ifdef __MESSENGER_CTRL_EXTENTED
#if __VER >= 15 // __CAMPUS
		p2DRender->TextOut(211, pt.y + 3, strFormat, dwColor);
#else // __CAMPUS
		p2DRender->TextOut(205, pt.y + 3, strFormat, dwColor);
#endif // __CAMPUS
#else // __MESSENGER_CTRL_EXTENTED
#if __VER >= 15 // __CAMPUS
		p2DRender->TextOut(226, pt.y + 3, strFormat, dwColor);
#else // __CAMPUS
		p2DRender->TextOut(220, pt.y + 3, strFormat, dwColor);
#endif // __CAMPUS
#endif // __MESSENGER_CTRL_EXTENTED
		
		pt.y += m_nFontHeight;
	}
#ifdef _WIN64
	pWndWorld->m_texPlayerDataIcon.Render(m_pApp->m_pd3dDevice, pVertex, (int)((__int64)pVertices - (__int64)pVertex) / sizeof(TEXTUREVERTEX2));
#else
	pWndWorld->m_texPlayerDataIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
#endif
	SAFE_DELETE_ARRAY( pVertex );
}

void CWndGuildCtrlEx::OnMouseMove(UINT nFlags, CPoint point)
{
#ifdef __MESSENGER_CTRL_EXTENTED
	CPoint pt(173, 3);
#else // __MESSENGER_CTRL_EXTENTED
	CPoint pt(188, 3);
#endif // __MESSENGER_CTRL_EXTENTED
	CRect rect;

	int nCount = m_vPlayerList.size();
	if(nCount > 10)
		nCount = 10;

	for( int j=0; j<nCount; j++)
	{
		rect.SetRect( pt.x, pt.y, pt.x + 24, pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
			int nPos = m_wndScrollBar.GetScrollPos();
			iter += j + nPos;
			__MESSENGER_PLAYER stPlayer	= *(iter);
			ClientToScreen( &point );
			ClientToScreen( &rect );
			g_toolTip.PutToolTip( 100, prj.m_aJob[ stPlayer.m_nJob ].szName, rect, point, 3 );
			j = m_vPlayerList.size();
		}

#ifdef __MESSENGER_CTRL_EXTENTED
		CRect rect2;
		rect2.SetRect(pt.x + 40, pt.y, pt.x + 105, pt.y + m_nFontHeight);
		if (rect2.PtInRect(point))
		{
			vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
			int nPos = m_wndScrollBar.GetScrollPos();
			iter += j + nPos;
			__MESSENGER_PLAYER stPlayer = *(iter);
			CPoint point2 = point;
			ClientToScreen(&point2);
			ClientToScreen(&rect2);

			CGuild* pGuild = g_pPlayer->GetGuild();
			CGuildMember* pMember = pGuild->GetMember(stPlayer.m_dwPlayerId);

			int nPlayerId = stPlayer.m_dwPlayerId;
			CString str;

#ifdef __AEGON_LAST_ONLINE
			str.Format("%s %d\n%s %d\n%s %d\n%s",
				prj.GetText(TID_GAME_TOOLTIP_EXPMERIT), pMember->m_dwGivePxpCount,
				prj.GetText(TID_GAME_TOOLTIP_PENYAMERIT), pMember->m_nGiveGold,
				prj.GetText(TID_GAME_TOOLTIP_GIVEUPNUM), pMember->m_nLose,
				CAegonLastOnlineMng::GetInstance()->GetLastOnlineString(pMember->m_idPlayer).GetString()
			);
#else // __AEGON_LAST_ONLINE
			str.Format("%s %d\n%s %d\n%s %d",
				prj.GetText(TID_GAME_TOOLTIP_EXPMERIT), pMember->m_dwGivePxpCount,
				prj.GetText(TID_GAME_TOOLTIP_PENYAMERIT), pMember->m_nGiveGold,
				prj.GetText(TID_GAME_TOOLTIP_GIVEUPNUM), pMember->m_nLose);
#endif // __AEGON_LAST_ONLINE

			g_toolTip.PutToolTip(m_nIdWnd, str, rect2, point2);
			g_toolTip.SetGuildRankToolTip(m_nIdWnd, nPlayerId);
			j = m_vPlayerList.size();
		}
#endif // __MESSENGER_CTRL_EXTENTED

		pt.y += m_nFontHeight;
	}
}

void CWndGuildCtrlEx::OnLButtonUp( UINT nFlags, CPoint point )
{
	CPoint pt( 3, 3 );
	CRect rect;

	for( int j=0; j<(int)( m_vPlayerList.size() ); j++)
	{
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			int nPos = m_wndScrollBar.GetScrollPos();
			m_nCurSelect = j + nPos;
			return;
		}
		pt.y += m_nFontHeight;
	}
	
	g_WorldMng()->SetObjFocus(NULL);
}

void CWndGuildCtrlEx::OnLButtonDblClk( UINT nFlags, CPoint point )
{
#ifdef __MESSENGER_CTRL_EXTENTED
	u_long idPlayer;
	CGuildMember* pGuildMember = NULL;
	int nSelect = GetSelect(point, idPlayer, &pGuildMember);
	if (nSelect != -1 && pGuildMember)
	{
		PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pGuildMember->m_idPlayer);
		Friend* pFriend = NULL;
#ifdef __AEGON_MESSENGER
		if(pPlayerData)
#else
		if (pPlayerData && pPlayerData->data.uLogin > 0)
#endif
		{
			if (pGuildMember->m_idPlayer != g_pPlayer->m_idPlayer)
			{
				int nSelectFriend = GetSelectFriend(point, idPlayer, &pFriend);
				if (nSelectFriend != -1 && pFriend)
				{
					DWORD dwState = pFriend->dwState;
#ifdef __AEGON_MESSENGER
					m_nCurSelect = nSelect;
					g_WndMng.OpenMessage(CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer));
#else
					if (dwState != FRS_BLOCK && dwState != FRS_OFFLINEBLOCK)
					{
						m_nCurSelect = nSelect;
						CWndMessage* pWndMessage = g_WndMng.OpenMessage(CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer));
					}
					else if (dwState == FRS_BLOCK)
					{
						CString szMessage;
						szMessage = prj.GetText(TID_GAME_MSGBLOCKCHR);
						g_WndMng.PutString(szMessage, NULL, prj.GetTextColor(TID_GAME_MSGBLOCKCHR));
					}
					else if (dwState == FRS_OFFLINEBLOCK)
					{
						CString szMessage;
						szMessage = prj.GetText(TID_GAME_NOTLOGIN);
						g_WndMng.PutString(szMessage, NULL, prj.GetTextColor(TID_GAME_NOTLOGIN));
					}
#endif
				}
				else if (nSelect != -1 && pFriend == NULL)
				{
					m_nCurSelect = nSelect;
					CWndMessage* pWndMessage = g_WndMng.OpenMessage(CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer));
				}
			}
			else if (pGuildMember->m_idPlayer == g_pPlayer->m_idPlayer)
			{
				m_nCurSelect = nSelect;
				return;
			}
		}
		else
		{
			CString szMessage;
			szMessage = prj.GetText(TID_GAME_NOTLOGIN);
			g_WndMng.PutString(szMessage, NULL, prj.GetTextColor(TID_GAME_NOTLOGIN));
		}
	}
#else // __MESSENGER_CTRL_EXTENTED
#ifdef __RT_1025
	u_long idPlayer;
	CGuildMember* pGuildMember = NULL;
	int nSelect	= GetSelect( point, idPlayer, &pGuildMember );
	if( nSelect != -1 && pGuildMember )
	{
		PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( pGuildMember->m_idPlayer );

		if( pPlayerData && pPlayerData->data.uLogin > 0 ) // state online
		{
			m_nCurSelect = nSelect;
			CWndMessage* pWndMessage = g_WndMng.OpenMessage( CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer ) );
		}
#else	// __RT_1025
	LPFRIEND lpFriend = NULL;
	int nSelect = GetSelect( point, &lpFriend );
	if( nSelect != -1 && lpFriend )
	{
		u_long idPlayer	= lpFriend->dwUserId;
		DWORD dwState	= lpFriend->dwState;
		if( dwState != FRS_OFFLINE && dwState != FRS_BLOCK && dwState != FRS_OFFLINEBLOCK )
		{
			m_nCurSelect	= nSelect;
			CWndMessage* pWndMessage = g_WndMng.OpenMessage( lpFriend->szName );
		}
#endif	// __RT_1025
		else
		{
			CString szMessage;
//			if( dwState == FRS_OFFLINE )
				szMessage = prj.GetText( TID_GAME_NOTLOGIN );                               //"??? 님은 접속되어 있지 않습니다";
/*			else
#if __VER >= 11 // __SYS_PLAYER_DATA
				szMessage.Format( prj.GetText(TID_GAME_MSGBLOCKCHR), CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer ) );  //"??? 님은 차단되어 있어 메세지를 보낼수 없습니다";
#else	// __SYS_PLAYER_DATA
				szMessage.Format( prj.GetText(TID_GAME_MSGBLOCKCHR), lpFriend->szName );  //"??? 님은 차단되어 있어 메세지를 보낼수 없습니다";
#endif	// __SYS_PLAYER_DATA
*/
			g_WndMng.PutString( szMessage, NULL, prj.GetTextColor(TID_GAME_NOTLOGIN) );		
		}
	}
#endif // __MESSENGER_CTRL_EXTENTED
}

#ifdef __MESSENGER_CTRL_EXTENTED
void CWndGuildCtrlEx::OnRButtonUp( UINT nFlags, CPoint point )
{
	u_long idPlayer;
	CGuildMember* pGuildMember	= NULL;
	int nSelect					= GetSelect( point, idPlayer, &pGuildMember );

	if( nSelect != -1 && pGuildMember )
	{
		Friend* pFriend				= NULL;
		int nSelectFriend			= GetSelectFriend( point, idPlayer, &pFriend );
		m_nCurSelect	= nSelect;
		ClientToScreen( &point );
		m_menu.DeleteAllMenu();

		PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( pGuildMember->m_idPlayer );
		if( pPlayerData && pPlayerData->data.uLogin > 0  )
		{
			if( nSelectFriend != -1 && pFriend != NULL )
			{
				DWORD dwState	= pFriend->dwState;
				if( !pFriend->bBlock )
				{

					m_menu.AppendMenu(0, 0, prj.GetText(TID_MESSENGER_GENERAL_MESSAGE));
					m_menu.AppendMenu(0, 1, prj.GetText(TID_MESSENGER_FRIEND_BLOCK));
	
					m_menu.AppendMenu( 0, 12,_T( prj.GetText( TID_GAME_GUILD_APPELLATIONUP ) ) );
					m_menu.AppendMenu( 0, 13,_T( prj.GetText( TID_GAME_GUILD_APPELLATIONDOWN ) ) );
					m_menu.AppendMenu( 0, 14,_T( prj.GetText( TID_GAME_GUILD_CLASSUP ) ) );
					m_menu.AppendMenu( 0, 15,_T( prj.GetText( TID_GAME_GUILD_CLASSDOWN ) ) );
					m_menu.AppendMenu( 0, 17,_T( prj.GetText( TID_GAME_GUILD_NICKNAME ) ) );
					m_menu.AppendMenu( 0, 18,_T( prj.GetText( TID_GAME_GUILD_LEAVE ) ) );

					CGuild* pGuild = g_pPlayer->GetGuild();
					if( pGuild && pGuild->IsMaster( g_pPlayer->m_idPlayer ) )
						m_menu.AppendMenu( 0, 16,_T( prj.GetText( TID_GAME_CHG_MASTER ) ) );

					if( g_Party.IsMember( idPlayer ) == FALSE )
					{
						m_menu.AppendMenu(0, 2, prj.GetText(TID_MESSENGER_PARTY_INVIT));
					}
				}
				else if( pFriend->bBlock )
				{
					m_menu.AppendMenu(0, 1, prj.GetText(TID_MESSENGER_FRIEND_BLOCKRESTORE));
				}
			}
			else
			{
				if ( pGuildMember->m_idPlayer == g_pPlayer->m_idPlayer )
				{
					return;
				}
				m_menu.AppendMenu(0, 0, prj.GetText(TID_MESSENGER_GENERAL_MESSAGE));
				m_menu.AppendMenu(0, 2, prj.GetText(TID_MESSENGER_PARTY_INVIT));

				m_menu.AppendMenu( 0, 12,_T( prj.GetText( TID_GAME_GUILD_APPELLATIONUP ) ) );
				m_menu.AppendMenu( 0, 13,_T( prj.GetText( TID_GAME_GUILD_APPELLATIONDOWN ) ) );
				m_menu.AppendMenu( 0, 14,_T( prj.GetText( TID_GAME_GUILD_CLASSUP ) ) );
				m_menu.AppendMenu( 0, 15,_T( prj.GetText( TID_GAME_GUILD_CLASSDOWN ) ) );
				m_menu.AppendMenu( 0, 17,_T( prj.GetText( TID_GAME_GUILD_NICKNAME ) ) );
				m_menu.AppendMenu( 0, 18,_T( prj.GetText( TID_GAME_GUILD_LEAVE ) ) );

				CGuild* pGuild = g_pPlayer->GetGuild();
				if( pGuild && pGuild->IsMaster( g_pPlayer->m_idPlayer ) )
					m_menu.AppendMenu( 0, 16,_T( prj.GetText( TID_GAME_CHG_MASTER ) ) );

			}

			if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
			{
				m_menu.AppendMenu(0, 3, prj.GetText(TID_MESSENGER_ADMIN_APPROCHER));
				m_menu.AppendMenu(0, 4, prj.GetText(TID_MESSENGER_ADMIN_RAMENER));
			}
		}
		else
		{
			if( nSelectFriend != -1 && pFriend != NULL )
			{
				DWORD dwState	= pFriend->dwState;
				if( !pFriend->bBlock )
				{
					m_menu.AppendMenu(0, 1, prj.GetText(TID_MESSENGER_FRIEND_BLOCK));
#ifndef __AEGON_MESSENGER
					m_menu.AppendMenu(0, 5, prj.GetText(TID_MESSENGER_GENERAL_OFFLINEMESSAGE));
#endif
					m_menu.AppendMenu( 0, 12,_T( prj.GetText( TID_GAME_GUILD_APPELLATIONUP ) ) );
					m_menu.AppendMenu( 0, 13,_T( prj.GetText( TID_GAME_GUILD_APPELLATIONDOWN ) ) );
					m_menu.AppendMenu( 0, 14,_T( prj.GetText( TID_GAME_GUILD_CLASSUP ) ) );
					m_menu.AppendMenu( 0, 15,_T( prj.GetText( TID_GAME_GUILD_CLASSDOWN ) ) );
					m_menu.AppendMenu( 0, 17,_T( prj.GetText( TID_GAME_GUILD_NICKNAME ) ) );
					m_menu.AppendMenu( 0, 18,_T( prj.GetText( TID_GAME_GUILD_LEAVE ) ) );

					CGuild* pGuild = g_pPlayer->GetGuild();
					if( pGuild && pGuild->IsMaster( g_pPlayer->m_idPlayer ) )
						m_menu.AppendMenu( 0, 16,_T( prj.GetText( TID_GAME_CHG_MASTER ) ) );


				}
				else if( pFriend->bBlock )
				{
					m_menu.AppendMenu(0, 1, prj.GetText(TID_MESSENGER_FRIEND_BLOCKRESTORE));
				}

			}
			else
			{
#ifndef __AEGON_MESSENGER
					m_menu.AppendMenu(0, 5, prj.GetText(TID_MESSENGER_GENERAL_OFFLINEMESSAGE));
#endif
					m_menu.AppendMenu( 0, 12,_T( prj.GetText( TID_GAME_GUILD_APPELLATIONUP ) ) );
					m_menu.AppendMenu( 0, 13,_T( prj.GetText( TID_GAME_GUILD_APPELLATIONDOWN ) ) );
					m_menu.AppendMenu( 0, 14,_T( prj.GetText( TID_GAME_GUILD_CLASSUP ) ) );
					m_menu.AppendMenu( 0, 15,_T( prj.GetText( TID_GAME_GUILD_CLASSDOWN ) ) );
					m_menu.AppendMenu( 0, 17,_T( prj.GetText( TID_GAME_GUILD_NICKNAME ) ) );
					m_menu.AppendMenu( 0, 18,_T( prj.GetText( TID_GAME_GUILD_LEAVE ) ) );

					CGuild* pGuild = g_pPlayer->GetGuild();
					if( pGuild && pGuild->IsMaster( g_pPlayer->m_idPlayer ) )
						m_menu.AppendMenu( 0, 16,_T( prj.GetText( TID_GAME_CHG_MASTER ) ) );
			}
		}


		m_menu.Move( point );
		m_menu.SetVisible( TRUE );
		m_menu.SetFocus();
	}
}

BOOL CWndGuildCtrlEx::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	int		nZoom = 64;
	int nCount = m_vPlayerList.size();
	
	if( zDelta > 0 )
		m_nCurrentList -= 1;
	else
		m_nCurrentList += 1;

	if( m_nCurrentList < 0 )
		m_nCurrentList = 0;
	if( (m_nCurrentList + MAX_MEMBER_LIST_GUILD - 1) >= nCount )
	{
		m_nCurrentList = nCount - MAX_MEMBER_LIST_GUILD;
		if( m_nCurrentList < 0 )
			m_nCurrentList = 0;
	}

	m_wndScrollBar.SetScrollPos( m_nCurrentList );
	return TRUE;
}

int	CWndGuildCtrlEx::GetSelectFriend( CPoint point, u_long &idPlayer, Friend** ppFriend )
{
	CPoint pt( 3, 3 );
	CRect rect;
	int rtn_val = -1;
	
	for( int j=0; j<(int)( m_vPlayerList.size() ); j++)
	{
		rtn_val++;
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
			int nPos = m_wndScrollBar.GetScrollPos();
			iter += j + nPos;
			rtn_val += nPos;
			__MESSENGER_PLAYER stPlayer	= *(iter);
			idPlayer = stPlayer.m_dwPlayerId;
			*ppFriend = g_WndMng.m_RTMessenger.GetFriend( stPlayer.m_dwPlayerId );
			j = m_vPlayerList.size();
		}
		pt.y += m_nFontHeight;
	}

	return rtn_val;
}
#endif // __MESSENGER_CTRL_EXTENTED

/*
#ifdef __RT_1025
void	CWndGuildCtrlEx::GetSelectFriend( int SelectCount, u_long & idPlayer, Friend** ppFriend )
#else	// __RT_1025
void CWndGuildCtrlEx::GetSelectFriend( int SelectCount, LPFRIEND* lppFriend )
#endif	// __RT_1025
{
	vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
	iter += SelectCount;
	__MESSENGER_PLAYER stPlayer	= *(iter);
#ifdef __RT_1025
	idPlayer	= stPlayer.m_dwPlayerId;
	*ppFriend	= g_WndMng.m_RTMessenger.GetFriend( stPlayer.m_dwPlayerId );
#else	// __RT_1025
	*lppFriend = g_WndMng.m_Messenger.GetFriend( stPlayer.m_dwPlayerId );
#endif	// __RT_1025
}
*/
	
u_long CWndGuildCtrlEx::GetSelectId( int SelectCount )
{
	vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
	iter += SelectCount;
	__MESSENGER_PLAYER stPlayer = *(iter);
	
	return stPlayer.m_dwPlayerId;
}

#ifdef __RT_1025
int	CWndGuildCtrlEx::GetSelect( CPoint point, u_long & idPlayer, CGuildMember** lppGuildMember )
#else	// __RT_1025
int CWndGuildCtrlEx::GetSelect( CPoint point, LPFRIEND* lppFriend )
#endif	// __RT_1025
{
	CPoint pt( 3, 3 );
	CRect rect;
	int rtn_val = -1;
	
	CGuild* pGuild = g_pPlayer->GetGuild();

	if(pGuild)
	{
		for( int j=0; j<(int)( m_vPlayerList.size() ); j++)
		{
			rtn_val++;
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
			if( rect.PtInRect( point ) )
			{
				vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
				int nPos = m_wndScrollBar.GetScrollPos();
				iter += j + nPos;
				rtn_val += nPos;
				__MESSENGER_PLAYER stPlayer = *(iter);
#ifdef __RT_1025
				idPlayer = stPlayer.m_dwPlayerId;
				CGuildMember * lpGuildMember = pGuild->GetMember( idPlayer );
				if( lpGuildMember )
					*lppGuildMember = lpGuildMember;
#else	// __RT_1025
				*lppFriend = g_WndMng.m_Messenger.GetFriend( stPlayer.m_dwPlayerId );
#endif	// __RT_1025
				j = m_vPlayerList.size();
			}
			pt.y += m_nFontHeight;
		}
	}

	return rtn_val;
}

void CWndGuildCtrlEx::SetScrollBar()
{
	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
#ifdef __RT_1025
	nRange	= g_WndMng.m_RTMessenger.size();
#else	// __RT_1025
	nRange = g_WndMng.m_Messenger.m_aFriend.size();
#endif	// __RT_1025
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
}

void CWndGuildCtrlEx::ScrollBarPos( int nPos )
{
	m_wndScrollBar.SetScrollPos( nPos );
}

int CWndGuildCtrlEx::GetDrawCount( void )
{
	CGuild * pGuild = g_pPlayer->GetGuild();
	int nMax = 0;
	if( pGuild )
		nMax = pGuild->GetSize();

	if( nMax - m_wndScrollBar.GetScrollPos() > m_wndScrollBar.GetScrollPage() )
		nMax = m_wndScrollBar.GetScrollPage() + m_wndScrollBar.GetScrollPos();
	if( nMax < m_wndScrollBar.GetScrollPos() )
		nMax = 0;

	return nMax;
}

void CWndGuildCtrlEx::OnSize( UINT nType, int cx, int cy )
{
	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;
	m_wndScrollBar.SetWndRect( rect );

	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
	CGuild * pGuild = g_pPlayer->GetGuild();
	if( pGuild )
	{
		nRange = pGuild->GetSize();
		m_wndScrollBar.SetScrollRange( 0, nRange );
		m_wndScrollBar.SetScrollPage( nPage );
	}
	
	CWndBase::OnSize( nType, cx, cy);
}
void CWndGuildCtrlEx::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 3, 3 );

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}

void CWndGuildCtrlEx::SortbyChannel(BOOL bCheckbefore)
{
	if(!bCheckbefore)
		m_bSortbyChannel = !m_bSortbyChannel;

	if(m_bSortbyChannel)
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prChannelAsce );
		m_bSortbyChannel = FALSE;
	}
	else
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prChannelDesc );
		m_bSortbyChannel = TRUE;
	}
	m_nCurSort = SORT_BY_CHANNEL;
}

void CWndGuildCtrlEx::SortbyStatus(BOOL bCheckbefore)
{
	if(!bCheckbefore)
		m_bSortbyStatus = !m_bSortbyStatus;

	if(m_bSortbyStatus)
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prStatusAsce );
		m_bSortbyStatus = FALSE;
	}
	else
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prStatusDesc );
		m_bSortbyStatus = TRUE;
	}
	m_nCurSort = SORT_BY_STATUS;
}

void CWndGuildCtrlEx::SortbyLevel(BOOL bCheckbefore)
{
	if(!bCheckbefore)
		m_bSortbyLevel = !m_bSortbyLevel;

	if(m_bSortbyLevel)
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prLevelAsce );
		m_bSortbyLevel = FALSE;
	}
	else
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prLevelDesc );
		m_bSortbyLevel = TRUE;
	}
	m_nCurSort = SORT_BY_LEVEL;
}

void CWndGuildCtrlEx::SortbyJob(BOOL bCheckbefore)
{
	if(!bCheckbefore)
		m_bSortbyJob = !m_bSortbyJob;

	if(m_bSortbyJob)
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prJobAsce );
		m_bSortbyJob = FALSE;
	}
	else
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prJobDesc );
		m_bSortbyJob = TRUE;
	}
	m_nCurSort = SORT_BY_JOB;
}

void CWndGuildCtrlEx::SortbyName(BOOL bCheckbefore)
{
	if(!bCheckbefore)
		m_bSortbyName = !m_bSortbyName;

	if(m_bSortbyName)
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prNameAsce );
		m_bSortbyName = FALSE;
	}
	else
	{
		std::sort( m_vPlayerList.begin(), m_vPlayerList.end(), prNameDesc );
		m_bSortbyName = TRUE;
	}
	m_nCurSort = SORT_BY_NAME;
}

#if __VER >= 15 // __CAMPUS
//-----------------------------------------------------------------------------
CWndCampus::CWndCampus( void ) : 
m_bSortbyChannel( TRUE ), 
m_bSortbyStatus( FALSE ), 
m_bSortbyLevel( TRUE ), 
m_bSortbyJob( TRUE ), 
m_bSortbyName( TRUE ), 
m_nCurSort( SORT_BY_STATUS ), 
m_bCurSelectedMaster( FALSE ), 
m_nCurSelectedDisciple( -1 ), 
m_nFontHeight( 20 ), 
m_MasterPlayer()
{
	m_vDisciplePlayer.clear();
}
//-----------------------------------------------------------------------------
CWndCampus::~CWndCampus( void )
{
}
//-----------------------------------------------------------------------------
BOOL CWndCampus::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MESSENGER_TAB_CAMPUS, 0, CPoint( 0, 0 ), pWndParent );
}
//-----------------------------------------------------------------------------
BOOL CWndCampus::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
//-----------------------------------------------------------------------------
void CWndCampus::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	CWndListBox* pWndListBox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX_MASTER );
	assert( pWndListBox );
	pWndListBox->EnableWindow( FALSE );
	pWndListBox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX_DISCIPLE );
	assert( pWndListBox );
	pWndListBox->EnableWindow( FALSE );

	m_Menu.CreateMenu( this );

	UpdatePlayerList();
}
//-----------------------------------------------------------------------------
BOOL CWndCampus::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	m_Menu.SetVisible( FALSE );
	switch( nID )
	{
	case 0:
		{
			CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus();
			if( pCampus == NULL )
				return FALSE;
			if( pCampus->IsMaster( g_pPlayer->m_idPlayer ) == TRUE )
			{
				__MESSENGER_PLAYER* pstDisciplePlayer = GetSelectedDiscipleID( m_nCurSelectedDisciple );
				if( !pstDisciplePlayer )
					return FALSE;
				if( pstDisciplePlayer->m_dwPlayerId != -1 )
				{
					if( g_WndMng.m_pWndCampusSeveranceConfirm )
						SAFE_DELETE( g_WndMng.m_pWndCampusSeveranceConfirm );
					g_WndMng.m_pWndCampusSeveranceConfirm = new CWndCampusSeveranceConfirm( static_cast< u_long >( pstDisciplePlayer->m_dwPlayerId ), 
																							pstDisciplePlayer->m_szName );
					g_WndMng.m_pWndCampusSeveranceConfirm->Initialize( NULL );
				}
			}
			else if( pCampus->IsPupil( g_pPlayer->m_idPlayer ) == TRUE )
			{
				if( m_MasterPlayer.m_dwPlayerId != -1 )
				{
					if( g_WndMng.m_pWndCampusSeveranceConfirm )
						SAFE_DELETE( g_WndMng.m_pWndCampusSeveranceConfirm );
					g_WndMng.m_pWndCampusSeveranceConfirm = new CWndCampusSeveranceConfirm( static_cast< u_long >( m_MasterPlayer.m_dwPlayerId ), 
																							m_MasterPlayer.m_szName );
					g_WndMng.m_pWndCampusSeveranceConfirm->Initialize( NULL );
				}
			}
			break;
		}
	}
	return TRUE;
}
//-----------------------------------------------------------------------------
void CWndCampus::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor3 );
}
//-----------------------------------------------------------------------------
void CWndCampus::OnDraw( C2DRender* p2DRender )
{
	if( NULL == g_pPlayer )
		return;

#ifdef __AEGON_THEME_SWITCHER
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();;
#endif

	CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus();
	if( pCampus == NULL )
		return;

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	CPoint pt( 3, 3 );

	if( pCampus->IsMaster( g_pPlayer->m_idPlayer ) == TRUE )
	{
		pt.y = DISCIPLE_RENDERING_POSITION;
		int nDiscipleMax = GetDiscipleDrawCount();
		vector < __MESSENGER_PLAYER >::iterator iter = m_vDisciplePlayer.begin();
		for( int i = 0; iter < m_vDisciplePlayer.end(); ++i, ++iter )
		{
			CString strFormat = _T( "" );
			if( i >= nDiscipleMax )
			{
				iter = m_vDisciplePlayer.end();
				continue;
			}

			__MESSENGER_PLAYER stPlayer = *iter;

			// Draw Channel Icon
			if( stPlayer.m_nChannel != 100 && stPlayer.m_nChannel > 0 && stPlayer.m_nChannel < 11 )
			{
				pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( 20, pt.y ), 34 + stPlayer.m_nChannel - 1, 0xffffffff );
			}

			// Draw Status Icon
			DWORD dwMyState = 0;
			if( stPlayer.m_dwStatus == FRS_AUTOABSENT )
				dwMyState = FRS_ABSENT;
			else if( stPlayer.m_dwStatus == FRS_ONLINE )
				dwMyState = 2;
			else if( stPlayer.m_dwStatus == FRS_OFFLINE )
				dwMyState = 8;
			else
				dwMyState = stPlayer.m_dwStatus;

#ifdef __MESSENGER_CTRL_EXTENTED
			pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(50, pt.y), 7 + (dwMyState - 2), 0xffffffff);
#else // __MESSENGER_CTRL_EXTENTED
			pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(76, pt.y), 7 + (dwMyState - 2), 0xffffffff);
#endif // __MESSENGER_CTRL_EXTENTED

			// Draw Level
			strFormat.Format("%d", stPlayer.m_nLevel);

#ifdef __MESSENGER_CTRL_EXTENTED
#ifdef __AEGON_THEME_SWITCHER
			p2DRender->TextOut(100, pt.y + 3, strFormat, dwColorText);
#else
			p2DRender->TextOut(100, pt.y + 3, strFormat, 0xff000000);
#endif
#else // __MESSENGER_CTRL_EXTENTED
			p2DRender->TextOut(127, pt.y + 3, strFormat, 0xff000000);
#endif // __MESSENGER_CTRL_EXTENTED

			// Draw Job Icon
#ifdef __MESSENGER_CTRL_EXTENTED
			static const int JOB_TYPE_ICON_X = 150;
#else // __MESSENGER_CTRL_EXTENTED
			static const int JOB_TYPE_ICON_X = 174;
#endif // __MESSENGER_CTRL_EXTENTED
			static const int JOB_ICON_X = JOB_TYPE_ICON_X + 20;
			if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_PRO )
				pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 6 ), 0xffffffff );
			else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_MASTER )
			{
				int nMasterIndex = 27;
				if( stPlayer.m_nLevel < 70 )
					nMasterIndex = 27;
				else if( stPlayer.m_nLevel >= 70 && stPlayer.m_nLevel < 80 )
					nMasterIndex = 28;
				else if( stPlayer.m_nLevel >= 80 && stPlayer.m_nLevel < 90 )
					nMasterIndex = 29;
				else if( stPlayer.m_nLevel >= 90 && stPlayer.m_nLevel < 100 )
					nMasterIndex = 30;
				else if( stPlayer.m_nLevel >= 100 && stPlayer.m_nLevel < 110 )
					nMasterIndex = 31;
				else if( stPlayer.m_nLevel >= 110 && stPlayer.m_nLevel <= 120 )
					nMasterIndex = 32;
				pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  nMasterIndex, 0xffffffff );
				pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 16 ), 0xffffffff );
			}
			else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_HERO )
			{
				pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  33, 0xffffffff );
				pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 24 ), 0xffffffff );
			}
#ifdef __3RD_LEGEND16
			else if (prj.m_aJob[stPlayer.m_nJob].dwJobType == JTYPE_LEGEND_HERO)
			{
				pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(JOB_TYPE_ICON_X, pt.y), 33, 0xffffffff);
				pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(JOB_ICON_X, pt.y), (19 + stPlayer.m_nJob - 32), 0xffffffff);
			}
#endif // __3RD_LEGEND16
			else
				pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_ICON_X, pt.y ),  14 + stPlayer.m_nJob, 0xffffffff );

			// Draw Name
#ifdef __AEGON_THEME_SWITCHER
			DWORD dwSelectColor = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE);
			DWORD dwColor = (i == m_nCurSelectedDisciple) ? dwSelectColor : dwColorText;
#else
			DWORD dwColor = ( i == m_nCurSelectedDisciple ) ? 0xff6060ff : 0xff000000;
#endif
			strFormat.Format( "%s", stPlayer.m_szName );

#ifdef __MESSENGER_CTRL_EXTENTED
			if (strFormat.GetLength() > 8)
#else // __MESSENGER_CTRL_EXTENTED
			if (strFormat.GetLength() > 10)
#endif // __MESSENGER_CTRL_EXTENTED
			{
				int	nReduceCount = 0;

#ifdef __MESSENGER_CTRL_EXTENTED
				for (nReduceCount = 0; nReduceCount < 8; )
#else // __MESSENGER_CTRL_EXTENTED
				for (nReduceCount = 0; nReduceCount < 10; )
#endif // __MESSENGER_CTRL_EXTENTED
				{
					if( IsDBCSLeadByte( strFormat[ nReduceCount ] ) )
						nReduceCount += 2;
					else
						++nReduceCount;
				}

				strFormat = strFormat.Left( nReduceCount );
				strFormat += "...";
			}
#ifdef __MESSENGER_CTRL_EXTENTED
			p2DRender->TextOut(200, pt.y + 3, strFormat, dwColor);
#else // __MESSENGER_CTRL_EXTENTED
			p2DRender->TextOut(226, pt.y + 3, strFormat, dwColor);
#endif // __MESSENGER_CTRL_EXTENTED
			pt.y += m_nFontHeight;
		}
	}
	else if( pCampus->IsPupil( g_pPlayer->m_idPlayer ) == TRUE )
	{
		pt.y = MASTER_RENDERING_POSITION;
		CString strFormat = _T( "" );
		__MESSENGER_PLAYER stPlayer = m_MasterPlayer;

		// Draw Channel Icon
		if( stPlayer.m_nChannel != 100 && stPlayer.m_nChannel > 0 && stPlayer.m_nChannel < 11 )
			pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( 20, pt.y ), 34 + stPlayer.m_nChannel - 1, 0xffffffff );

		// Draw Status Icon
		DWORD dwMyState = 0;
		if( stPlayer.m_dwStatus == FRS_AUTOABSENT )
			dwMyState = FRS_ABSENT;
		else if( stPlayer.m_dwStatus == FRS_ONLINE )
			dwMyState = 2;
		else if( stPlayer.m_dwStatus == FRS_OFFLINE )
			dwMyState = 8;
		else
			dwMyState = stPlayer.m_dwStatus;

#ifdef __MESSENGER_CTRL_EXTENTED
		pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(50, pt.y), 7 + (dwMyState - 2), 0xffffffff);
#else // __MESSENGER_CTRL_EXTENTED
		pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(76, pt.y), 7 + (dwMyState - 2), 0xffffffff);
#endif // __MESSENGER_CTRL_EXTENTED

		// Draw Level
		strFormat.Format("%d", stPlayer.m_nLevel);

#ifdef __MESSENGER_CTRL_EXTENTED
#ifdef __AEGON_THEME_SWITCHER
		p2DRender->TextOut(100, pt.y + 3, strFormat, dwColorText);
#else
		p2DRender->TextOut(100, pt.y + 3, strFormat, 0xff000000);
#endif
#else // __MESSENGER_CTRL_EXTENTED
		p2DRender->TextOut(127, pt.y + 3, strFormat, 0xff000000);
#endif // __MESSENGER_CTRL_EXTENTED

		// Draw Job Icon
#ifdef __MESSENGER_CTRL_EXTENTED
		static const int JOB_TYPE_ICON_X = 150;
#else // __MESSENGER_CTRL_EXTENTED
		static const int JOB_TYPE_ICON_X = 174;
#endif // __MESSENGER_CTRL_EXTENTED

		static const int JOB_ICON_X = JOB_TYPE_ICON_X + 20;
		if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_PRO )
			pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 6 ), 0xffffffff );
		else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_MASTER )
		{
			int nMasterIndex = 27;
			if( stPlayer.m_nLevel < 70 )
				nMasterIndex = 27;
			else if( stPlayer.m_nLevel >= 70 && stPlayer.m_nLevel < 80 )
				nMasterIndex = 28;
			else if( stPlayer.m_nLevel >= 80 && stPlayer.m_nLevel < 90 )
				nMasterIndex = 29;
			else if( stPlayer.m_nLevel >= 90 && stPlayer.m_nLevel < 100 )
				nMasterIndex = 30;
			else if( stPlayer.m_nLevel >= 100 && stPlayer.m_nLevel < 110 )
				nMasterIndex = 31;
			else if( stPlayer.m_nLevel >= 110 && stPlayer.m_nLevel <= 120 )
				nMasterIndex = 32;
			pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  nMasterIndex, 0xffffffff );
			pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 16 ), 0xffffffff );
		}
		else if( prj.m_aJob[ stPlayer.m_nJob ].dwJobType == JTYPE_HERO )
		{
			pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_TYPE_ICON_X, pt.y ),  33, 0xffffffff );
			pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_ICON_X, pt.y ),  ( 19 + stPlayer.m_nJob - 24 ), 0xffffffff );
		}
#ifdef __3RD_LEGEND16
		else if (prj.m_aJob[stPlayer.m_nJob].dwJobType == JTYPE_LEGEND_HERO)
		{
			pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(JOB_TYPE_ICON_X, pt.y), 33, 0xffffffff);
			pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(JOB_ICON_X, pt.y), (19 + stPlayer.m_nJob - 32), 0xffffffff);
		}
#endif // __3RD_LEGEND16
		else
			pWndWorld->m_texPlayerDataIcon.Render( p2DRender, CPoint( JOB_ICON_X, pt.y ),  14 + stPlayer.m_nJob, 0xffffffff );

		// Draw Name
#ifdef __AEGON_THEME_SWITCHER
		DWORD dwSelectColor = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE);
		DWORD dwColor = (m_bCurSelectedMaster == TRUE) ? dwSelectColor : dwColorText;
#else
		DWORD dwColor = ( m_bCurSelectedMaster == TRUE ) ? 0xff6060ff : 0xff000000;
#endif
		strFormat.Format( "%s", stPlayer.m_szName );

#ifdef __MESSENGER_CTRL_EXTENTED
		if (strFormat.GetLength() > 8)
#else // __MESSENGER_CTRL_EXTENTED
		if (strFormat.GetLength() > 10)
#endif // __MESSENGER_CTRL_EXTENTED
		{
			int	nReduceCount = 0;

#ifdef __MESSENGER_CTRL_EXTENTED
			for (nReduceCount = 0; nReduceCount < 8; )
#else // __MESSENGER_CTRL_EXTENTED
			for (nReduceCount = 0; nReduceCount < 10; )
#endif // __MESSENGER_CTRL_EXTENTED
			{
				if( IsDBCSLeadByte( strFormat[ nReduceCount ] ) )
					nReduceCount += 2;
				else
					++nReduceCount;
			}
			strFormat = strFormat.Left( nReduceCount );
			strFormat += "...";
		}
#ifdef __MESSENGER_CTRL_EXTENTED
		p2DRender->TextOut(200, pt.y + 3, strFormat, dwColor);
#else // __MESSENGER_CTRL_EXTENTED
		p2DRender->TextOut(226, pt.y + 3, strFormat, dwColor);
#endif // __MESSENGER_CTRL_EXTENTED
	}
}
//-----------------------------------------------------------------------------
void CWndCampus::OnLButtonUp( UINT nFlags, CPoint point )
{
	CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus();
	if( pCampus == NULL )
		return;
	if( pCampus->IsMaster( g_pPlayer->m_idPlayer ) == TRUE )
		GetSelectedDiscipleID( point );
	else if( pCampus->IsPupil( g_pPlayer->m_idPlayer ) == TRUE )
		GetSelectedMasterID( point );
		
}
//-----------------------------------------------------------------------------
void CWndCampus::OnRButtonUp( UINT nFlags, CPoint point )
{
	CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus();
	if( pCampus == NULL )
		return;
	if( pCampus->IsMaster( g_pPlayer->m_idPlayer ) == TRUE )
	{
		if( GetSelectedDiscipleID( point ) == -1 )
			return;
	}
	else if( pCampus->IsPupil( g_pPlayer->m_idPlayer ) == TRUE )
	{
		if( GetSelectedMasterID( point ) == -1 )
			return;
	}
	ClientToScreen( &point );
	m_Menu.DeleteAllMenu();
	m_Menu.AppendMenu( 0, 0, prj.GetText( TID_GAME_MENU_CAMPUS_SEVERANCE ) );
	m_Menu.Move( point );
	m_Menu.SetVisible( TRUE );
	m_Menu.SetFocus();
}
//-----------------------------------------------------------------------------
void CWndCampus::OnMouseMove( UINT nFlags, CPoint point )
{
	CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus();
	if( pCampus == NULL )
		return;

	CPoint pt( 188, 3 );
	if( pCampus->IsMaster( g_pPlayer->m_idPlayer ) == TRUE )
	{
		pt.y = DISCIPLE_RENDERING_POSITION;
		CRect rect( 0, 0, 0, 0 );
		for( int i = 0; i < (int)( m_vDisciplePlayer.size() ); ++i )
		{
			rect.SetRect( pt.x, pt.y, pt.x + 24, pt.y + m_nFontHeight );
			if( rect.PtInRect( point ) == TRUE )
			{
				vector< __MESSENGER_PLAYER >::iterator DiscipleIterator = m_vDisciplePlayer.begin();
				DiscipleIterator += i;
				__MESSENGER_PLAYER stPlayer	= *DiscipleIterator;
				ClientToScreen( &point );
				ClientToScreen( &rect );
				g_toolTip.PutToolTip( 100, prj.m_aJob[ stPlayer.m_nJob ].szName, rect, point, 3 );
				i = m_vDisciplePlayer.size();
			}
			pt.y += m_nFontHeight;
		}
	}
	else if( pCampus->IsPupil( g_pPlayer->m_idPlayer ) == TRUE )
	{
		pt.y = MASTER_RENDERING_POSITION;
		CRect rect( pt.x, pt.y, pt.x + 24, pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) == TRUE )
		{
			ClientToScreen( &point );
			ClientToScreen( &rect );
			g_toolTip.PutToolTip( 100, prj.m_aJob[ m_MasterPlayer.m_nJob ].szName, rect, point, 3 );
		}
	}
}
//-----------------------------------------------------------------------------
void CWndCampus::UpdatePlayerList( void )
{
	m_MasterPlayer.Initialize();
	m_vDisciplePlayer.clear();
	CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus();
	if( pCampus == NULL )
		return;
	if( pCampus->IsMaster( g_pPlayer->m_idPlayer ) )
	{
		typedef vector< u_long > DiscipleVector;
		DiscipleVector vecPupil = pCampus->GetPupilPlayerId();
		for( DiscipleVector::iterator itor = vecPupil.begin(); itor != vecPupil.end(); ++itor )
		{
			u_long idDisciplePlayer = *itor;
			PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData( idDisciplePlayer );
			if( pPlayerData == NULL )
				continue;
			__MESSENGER_PLAYER stDisciplePlayer;
			stDisciplePlayer.m_dwPlayerId = idDisciplePlayer;
			stDisciplePlayer.m_dwStatus = ( pPlayerData->data.uLogin > 0 ) ? FRS_ONLINE : FRS_OFFLINE;
			stDisciplePlayer.m_nChannel	= pPlayerData->data.uLogin;
			stDisciplePlayer.m_nJob = pPlayerData->data.nJob;
			stDisciplePlayer.m_nLevel = pPlayerData->data.nLevel;
			lstrcpy( stDisciplePlayer.m_szName, pPlayerData->szPlayer );
			m_vDisciplePlayer.push_back( stDisciplePlayer );
		}
	}
	else if( pCampus->IsPupil( g_pPlayer->m_idPlayer ) )
	{
		u_long idMasterPlayer = pCampus->GetMaster();
		PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData( idMasterPlayer );
		if( pPlayerData == NULL )
			return;
		m_MasterPlayer.m_dwPlayerId = idMasterPlayer;
		m_MasterPlayer.m_dwStatus = ( pPlayerData->data.uLogin > 0 ) ? FRS_ONLINE : FRS_OFFLINE;
		m_MasterPlayer.m_nChannel = pPlayerData->data.uLogin;
		m_MasterPlayer.m_nJob = pPlayerData->data.nJob;
		m_MasterPlayer.m_nLevel = pPlayerData->data.nLevel;
		lstrcpy( m_MasterPlayer.m_szName, pPlayerData->szPlayer );
	}
	switch( m_nCurSort )
	{
		case SORT_BY_CHANNEL:
			SortbyChannel(FALSE);
			break;
		case SORT_BY_STATUS:
			SortbyStatus(FALSE);
			break;
		case SORT_BY_LEVEL:
			SortbyLevel(FALSE);
			break;
		case SORT_BY_JOB:
			SortbyJob(FALSE);
			break;
		case SORT_BY_NAME:
			SortbyName(FALSE);
			break;
	}
}
//-----------------------------------------------------------------------------
int CWndCampus::GetDiscipleDrawCount( void ) const
{
	return m_vDisciplePlayer.size();
}
//-----------------------------------------------------------------------------
__MESSENGER_PLAYER* CWndCampus::GetSelectedDiscipleID( int nSelectedNumber )
{
//	vector < __MESSENGER_PLAYER >::iterator iter = m_vDisciplePlayer.begin();
//	iter += nSelectedNumber;
	//	BEGINTEST100113	??????????
//	return	( ( __MESSENGER_PLAYER* )( &iter ) );
	if( nSelectedNumber >= (int)( m_vDisciplePlayer.size() ) || nSelectedNumber < 0 )	//gmpbigsun: with => || nSelectedNumber < 0 )
		return NULL;
	return	&m_vDisciplePlayer[nSelectedNumber];
	//	ENDTEST100113	??????????
}
//-----------------------------------------------------------------------------
u_long CWndCampus::GetSelectedMasterID( CPoint point )
{
	CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus();
	if( pCampus == NULL )
		return -1;
	if( pCampus->IsPupil( g_pPlayer->m_idPlayer ) == TRUE )
	{
		CPoint pt( 3, MASTER_RENDERING_POSITION );
		CRect rect( pt.x, pt.y, pt.x + m_rectWindow.Width(), pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) == TRUE )
		{
			m_bCurSelectedMaster = TRUE;
			return static_cast< u_long >( m_MasterPlayer.m_dwPlayerId );
		}
	}
	return -1;
}
//-----------------------------------------------------------------------------
u_long CWndCampus::GetSelectedDiscipleID( CPoint point )
{
	CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus();
	if( pCampus == NULL )
		return -1;
	if( pCampus->IsMaster( g_pPlayer->m_idPlayer ) == TRUE )
	{
		CPoint pt( 3, DISCIPLE_RENDERING_POSITION );
		CRect rect( 0, 0, 0, 0 );
		for( int i = 0; i < (int)( m_vDisciplePlayer.size() ); ++i )
		{
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width(), pt.y + m_nFontHeight );
			if( rect.PtInRect( point ) == TRUE )
			{
				m_nCurSelectedDisciple = i;
				return static_cast< u_long >( ( ( __MESSENGER_PLAYER )( m_vDisciplePlayer[i] ) ).m_dwPlayerId );
			}
			pt.y += m_nFontHeight;
		}
	}
	return -1;
}
//-----------------------------------------------------------------------------
void CWndCampus::SortbyChannel( BOOL bCheckbefore )
{
	if(!bCheckbefore)
		m_bSortbyChannel = !m_bSortbyChannel;

	if(m_bSortbyChannel)
	{
		std::sort( m_vDisciplePlayer.begin(), m_vDisciplePlayer.end(), prChannelAsce );
		m_bSortbyChannel = FALSE;
	}
	else
	{
		std::sort( m_vDisciplePlayer.begin(), m_vDisciplePlayer.end(), prChannelDesc );
		m_bSortbyChannel = TRUE;
	}
	m_nCurSort = SORT_BY_CHANNEL;
}
//-----------------------------------------------------------------------------
void CWndCampus::SortbyStatus( BOOL bCheckbefore )
{
	if(!bCheckbefore)
		m_bSortbyStatus = !m_bSortbyStatus;

	if(m_bSortbyStatus)
	{
		std::sort( m_vDisciplePlayer.begin(), m_vDisciplePlayer.end(), prStatusAsce );
		m_bSortbyStatus = FALSE;
	}
	else
	{
		std::sort( m_vDisciplePlayer.begin(), m_vDisciplePlayer.end(), prStatusDesc );
		m_bSortbyStatus = TRUE;
	}
	m_nCurSort = SORT_BY_STATUS;
}
//-----------------------------------------------------------------------------
void CWndCampus::SortbyLevel( BOOL bCheckbefore )
{
	if(!bCheckbefore)
		m_bSortbyLevel = !m_bSortbyLevel;

	if(m_bSortbyLevel)
	{
		std::sort( m_vDisciplePlayer.begin(), m_vDisciplePlayer.end(), prLevelAsce );
		m_bSortbyLevel = FALSE;
	}
	else
	{
		std::sort( m_vDisciplePlayer.begin(), m_vDisciplePlayer.end(), prLevelDesc );
		m_bSortbyLevel = TRUE;
	}
	m_nCurSort = SORT_BY_LEVEL;
}
//-----------------------------------------------------------------------------
void CWndCampus::SortbyJob( BOOL bCheckbefore )
{
	if(!bCheckbefore)
		m_bSortbyJob = !m_bSortbyJob;

	if(m_bSortbyJob)
	{
		std::sort( m_vDisciplePlayer.begin(), m_vDisciplePlayer.end(), prJobAsce );
		m_bSortbyJob = FALSE;
	}
	else
	{
		std::sort( m_vDisciplePlayer.begin(), m_vDisciplePlayer.end(), prJobDesc );
		m_bSortbyJob = TRUE;
	}
	m_nCurSort = SORT_BY_JOB;
}
//-----------------------------------------------------------------------------
void CWndCampus::SortbyName( BOOL bCheckbefore )
{
	if(!bCheckbefore)
		m_bSortbyName = !m_bSortbyName;

	if(m_bSortbyName)
	{
		std::sort( m_vDisciplePlayer.begin(), m_vDisciplePlayer.end(), prNameAsce );
		m_bSortbyName = FALSE;
	}
	else
	{
		std::sort( m_vDisciplePlayer.begin(), m_vDisciplePlayer.end(), prNameDesc );
		m_bSortbyName = TRUE;
	}
	m_nCurSort = SORT_BY_NAME;
}
//-----------------------------------------------------------------------------
#endif // __CAMPUS

#endif //__CSC_VER11_4