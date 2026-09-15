#ifndef __PLAYERWAR_H__
#define __PLAYERWAR_H__
#ifdef __ROYAL_RUMBLE

#define MAX_JOBS 4
#ifndef __DBSERVER
struct WAR_PLAYER
{
	TCHAR m_szName[MAX_PLAYER];
	u_long m_idPlayer;
	int m_nKills;
	int m_nPoints;
	int m_nDeaths;
	int m_nJob;
	DWORD m_dwRespawnTime;
	WAR_PLAYER()
	{
		m_idPlayer = m_nKills = m_nDeaths = m_nPoints = m_dwRespawnTime = 0;
		memset(m_szName, 0, sizeof(TCHAR));
	}
};
#ifdef __WORLDSERVER
typedef struct tagWAR_TIME
{
	short nMin;
	short nHour;
}WAR_TIME;

struct WAR_INFO
{
	DWORD m_dwItemID;
	int m_nMaxPlayerLevel;
	int m_nMinPlayerLevel;
	int m_nNum;
	int m_nFightTime;
	int m_nWaitTime;
#ifdef __RUMBLE_GIVEUP
	int m_nMinWarPlayersGiveUp;
#endif //__RUMBLE_GIVEUP
	int m_nMinWarPlayers;
	int m_nMaxWarPlayers;
	int m_nRespawnTime;
	int m_nPreparationTime;
};
#endif // __WORLDSERVER
#endif // __DBSERVER

struct SIEGE_RANK
{
	u_long	uidPlayer;
	TCHAR szName[MAX_PLAYER];
	int	nKills;
	int nDeaths;
	int nPoints;
	int nJob;
	SIEGE_RANK()
	{
		uidPlayer = nKills = nDeaths = nPoints = 0;
		memset(szName, 0, sizeof(TCHAR));
	}
};

class CPlayerWar
{
public:
	CPlayerWar();
	virtual ~CPlayerWar();

	// DB & Neuz & WS
	vector<SIEGE_RANK> m_vecRankSiege;
#ifndef __DBSERVER
	vector<WAR_PLAYER> m_vecWarPlayers;
	int m_nState;
	// Only WorldServer & Client
	void SortUsers();
	enum { OPEN_STATE, CLOSE_STATE, WAR_STATE, FIGHT_STATE, WAIT_STATE };
	void SetState(int nState);
	int	GetState();
	bool NextState();
#endif // __DBSERVER
#ifdef __CLIENT
	// Only Client ( ScoreBoard )
	void Paint(C2DRender* p2DRender);
	void PaintTime(C2DRender* p2DRender);
#endif // __CLIENT

#ifdef __WORLDSERVER
	// Only WorldServer
	WAR_INFO WARINFO;
	DWORD m_dwWaitTime;
	time_t _tLastRanking;
	DWORD m_dwUserWaitTime;
	u_long idLeader;
	D3DXVECTOR3 vPos[10];
	char szMsg[120];
	bool m_bMsg;
	bool m_bWar;
	bool m_bStart;
	DWORD StartTime;

	void EndWar();
	void OpenWar();
	void Process();
	void ProcessJoinWar();
	int  NewUser(u_long idPlayer);
	void RejoinSiegeFight(CUser* pUser);
	void TeleportAllUser(int nState);
	void TeleportToWaitArea(CUser* pDefender);
	void Kill(CUser* pAttacker, CUser* pDefender);
	void UpdateUser(u_long idPlayer, WAR_PLAYER pInfo);
#ifdef __RUMBLE_GIVEUP
	void UserGiveUp(u_long idPlayer);
#endif //__RUMBLE_GIVEUP

	BOOL CheckJob(CUser* pUser);
	BOOL CheckIP(CUser* pUser);
	BOOL IsLeader(u_long idPlayer);
	BOOL GetNextWar();
	BOOL CreateItem(CUser* pAttacker, int nNum);
	BOOL LoadScript(LPCSTR lpszFileName);
	BOOL IsSiegeUser(u_long idPlayer);

	SIEGE_RANK GetRankingUser(u_long idPlayer);
	WAR_PLAYER GetUser(u_long idPlayer);
#ifdef __TIMER
	DWORD GetNextPlayerWarTime();
#endif // __TIMER
#endif // __WORLDSERVER
	BOOL RemoveSiegeUser(u_long idPlayer);
	BOOL CheckUsers(u_long idPlayer, BOOL bRemove);
};
#endif // __ROYAL_RUMBLE
#endif // __PLAYERWAR_H__