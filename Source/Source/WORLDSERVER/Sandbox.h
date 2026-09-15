#pragma once

#ifdef __SYS_SANDBOX
#include "Mover.h"
class CSandboxMover : public CMover
{
public:
	CSandboxMover() { CMover(); m_bDamage = FALSE; }

	BOOL m_bDamage;
};

struct SANDBOX
{
	vector<u_long> m_vecPlayer;
	CSandboxMover* pMonster;
	SANDBOX()
	{
		pMonster = nullptr;
		m_vecPlayer.clear();
	}
};

struct QUEUELIST
{
	int m_nLayer;
	vector<u_long> m_vecPlayer;
	DWORD m_dwWaitTime;
	QUEUELIST()
	{
		m_nLayer = -1;
		m_vecPlayer.clear();
		m_dwWaitTime = 0;
	}
};

class CSandbox
{
private:
	map<int, SANDBOX> m_aLayermap;
	vector<QUEUELIST> m_vecQueue;
	D3DXVECTOR3 m_vecSpawn;
public:
	CSandbox();
	~CSandbox();

	static CSandbox* GetInstance(void);
	void EnterSandbox(CMover* pMover, D3DXVECTOR3 vecPos);
	void InsertMover(int nLayer, CMover* pMover);
	void DeleteMover(int nLayer, CMover* pMover);
	void InsertQueue(int nLayer, CMover* pMover);
	void DeleteQueue(int nLayer, CMover* pMover);
	BOOL CreateLayer(int nLayer);
	void DeleteLayer(int nLayer);
	void SpawnMonster(int nLayer, DWORD dwIndex, BOOL bDamage);
	void ClearMonster(int nLayer);
	BOOL Process();
	BOOL ProcessQueueList(QUEUELIST* queue);
	SANDBOX* GetLayerMap(int nLayer);
};
#endif //__SYS_SANDBOX