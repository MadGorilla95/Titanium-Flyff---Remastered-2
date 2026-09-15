#pragma once
#ifndef __AEGLASTONLSYS__H
#define __AEGLASTONLSYS__H
#ifdef __AEGON_LAST_ONLINE



typedef struct __LAST_ONLINE_UNIT
{
	char m_szName[64];
	u_long playerID;
	__int64 m_nLastOnline;
	BOOL m_bOnline;

	void Init() 
	{
		ZeroMemory(m_szName, 64);
		playerID = 0;
		m_nLastOnline = 0;
		m_bOnline = FALSE;
	}


}__LAST_ONLINE_UNIT;

class CAegonLastOnlineMng
{
public:

	map<u_long, __LAST_ONLINE_UNIT> m_mapLastOnline;


	CAegonLastOnlineMng();
	virtual ~CAegonLastOnlineMng();

	static CAegonLastOnlineMng* GetInstance(void);

	__LAST_ONLINE_UNIT* GetLastOnlineByName(CString playerName);
	__LAST_ONLINE_UNIT* GetLastOnlineByPlayerID(u_long playerID);

#ifdef __DBSERVER
	void WriteMap(CAr& ar);
#endif // __DBSERVER

#ifdef __WORLDSERVER
	void WriteForVec(CAr& ar, vector<u_long> playerIDs);
	void OnUserLogin(CUser* pUser);
	void OnUserLogout(CUser* pUser);
	void DistributeRefreshs(CUser* pUser);
#endif // __WORLDSERVER

#if defined(__CLIENT) || defined(__WORLDSERVER)
	void ReadMap(CAr& ar);
#endif // __CLIENT || __WORLDSERVER

#ifdef __CLIENT
	CString GetLastOnlineString(u_long playerID);
#endif // __CLIENT
};

#endif // __AEGON_LAST_ONLINE

#endif //__AEGLASTONLSYS__H