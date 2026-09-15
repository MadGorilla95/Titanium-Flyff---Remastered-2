
#ifdef __SYS_WORLDBOSS
typedef map<DWORD, DWORD> WBLOOT;
struct WBSPAWN
{
	DWORD mobID;
	CMover* pMover;
	CTime ctSpawnTime;
};

struct WBSpawnTime {
	DWORD hour;
	DWORD minute;
	DWORD second;
	DWORD day;

	WBSpawnTime()
	{
		hour = 0;
		minute = 0;
		second = 0;
		day = 0;
	}

	void SetDay(DWORD sDay)
	{
		day = sDay;
	}

	void SetHour(DWORD sHour)
	{
		hour = sHour;
	}

	void SetMinute(DWORD sMinute)
	{
		minute = sMinute;
	}

	void SetSecond(DWORD sSecond)
	{
		second = sSecond;
	}

	DWORD GetDay()
	{
		return day;
	}

	DWORD GetHour()
	{
		return hour;
	}

	DWORD GetMinute()
	{
		return minute;
	}

	DWORD GetSecond()
	{
		return second;
	}
};

struct vWBProp {
	DWORD dwMonsterID;
	DWORD dwWorld;
	int vPos_x;
	int vPos_y;
	int vPos_z;
	BOOL bActiveAttack;
	WBLOOT vecLoot;
	vector<WBSpawnTime> vecSpawnTimes;
	BOOL bIsSpawned;

	vWBProp()
	{
		dwMonsterID = 0;
		dwWorld = 1;
		vPos_x = 0;
		vPos_y = 0;
		vPos_z = 0;
		bIsSpawned = FALSE;
	}
};

typedef vector<vWBProp> WBV;

class CWorldBossController
{
public:
	virtual ~CWorldBossController();
	static CWorldBossController* Instance(void);
	BOOL LoadScript(const char* szFile);
	void Process();
	WBLOOT GetLoot(DWORD dwMonsterID);
	void BossDeath(CMover* mover);
private:
	CWorldBossController();
	WBV m_WorldBosses;
	BOOL IsSpawning(vWBProp pProp);
	bool IsSpawned(DWORD mobID);
	void Spawn(vWBProp pProp);
	void DeleteSpawn(WBSPAWN spawn);
	map<DWORD, WBSPAWN> m_aSpawnedBosses;
};

#endif //__SYS_WORLDBOSS