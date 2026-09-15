#pragma once

#ifdef __SYS_MONSTER_CLASH
#define STATE_CLOSED  0
#define STATE_REGISTER 1
#define STATE_WAIT 2
#define STATE_CLASH 3
#include "guild.h"
class CUser;
class CMonsterClashInstance;
class CMonsterClashMng
{
public:
	//Public Structs
	struct STATE
	{
		struct MONSTER
		{
			DWORD dwMonsterId;
			D3DXVECTOR3 vPos;
			BOOL bAggro;
			MONSTER()
				:dwMonsterId(0),
				bAggro(FALSE)
			{

			}		
		};
		vector<MONSTER> vecMonster;
		size_t GetStateCount() const { return vecMonster.size(); }
	};
	struct GUILD
	{
		DWORD dwGuildId;
		map<u_long, CUser*> aRegisteredMember;
		GUILD()
			: dwGuildId(0)
		{

		}
		GUILD(DWORD dwGuildId)
			: dwGuildId(dwGuildId)
		{

		}
		size_t RegisteredMemberCount() const { return aRegisteredMember.size(); }
		void AddUser(CUser* pUser);
		void RemoveUser(CUser* pUser);
		void RemoveUser()
		{
			aRegisteredMember.clear();
		}
	};
private:
	//Private Structs
	struct CLASH_PROP
	{
		struct CLASH_TIME
		{
			int nDayOfWeek;
			int nHour;
			int nMinute;
			CLASH_TIME()
				: nDayOfWeek(0),
				nHour(0),
				nMinute(0)
			{

			}
		};
		struct CLASH_REWARD
		{
			map<DWORD, int> ItemReward;
		};
		int nMaxRegisterdMember;
		int nRegisterMinutes;
		int nFightMinutes;
		int nReadyMinutes;
		int nMemberRankRegister;
		int nRegisterCost;

		DWORD dwClashMapId;

		D3DXVECTOR3 endPortPos;
		DWORD endPortMapId;

		D3DXVECTOR3 waitPortPos;
		D3DXVECTOR3 fightPortPos;

		map<int, STATE> aStateProp;

		vector<CLASH_TIME> vecClashTimes;
		map<int, CLASH_REWARD> aClashRewards;
		CLASH_PROP()
			: nMaxRegisterdMember(5),
			nRegisterMinutes(10),
			nFightMinutes(15),
			nReadyMinutes(5),
			nMemberRankRegister(GUD_KINGPIN),
			nRegisterCost(100000000),
			dwClashMapId(0)
		{

		}
	};

	//Class Member
	map<int, CMonsterClashInstance> aClashInstance;
	map<DWORD, GUILD>aRegisteredGuild;
	int nState;
	CLASH_PROP sProp;

	time_t Timer;
public:
	CMonsterClashMng();
	~CMonsterClashMng();

	static CMonsterClashMng* GetInstance();

	//Setter Getter
	int GetState() const { return nState; }
	void SetState(const int State) { nState = State; }

	CLASH_PROP& GetProp() { return sProp; }
	int GetPropMemberRegisterRank() const { return sProp.nMemberRankRegister; }
	DWORD GetPropTeleportOutMapId() const { return sProp.endPortMapId; }
	D3DXVECTOR3 GetPropTeleportOutMapPos() const { return sProp.endPortPos; }

	D3DXVECTOR3 GetPropTeleportWaitMapPos() const { return sProp.waitPortPos; }
	D3DXVECTOR3 GetPropTeleportFightMapPos() const { return sProp.fightPortPos; }
	DWORD GetClashMapId() const { return sProp.dwClashMapId; }

	STATE* GetStateProp(const int nState);

	int GetMaxStateKillCount() const;
	int GetMaxState() const;
	//Class Functions
	void ReadScript(const LPCTSTR szFile);
	GUILD* GetRegisteredGuild(const DWORD dwGuildId);
	GUILD* GetRegisteredUser(CUser* pUser);
	void RemoveUser(CUser* pUser);
	void RemoveGuild(const DWORD dwGuildId) { aRegisteredGuild.erase(dwGuildId); }
	void AddGuild(const GUILD sGuild) { aRegisteredGuild.insert(make_pair(sGuild.dwGuildId, sGuild)); }
	size_t GetRegisteredGuildCount() const { return aRegisteredGuild.size(); }
	void TeleportUserOut(CUser* pUser) const;
	void RemoveRegisteredGuild(const DWORD dwGuildId);
	

	void AddClashInstance(const int nLayer);
	void RemoveClashInstance(const int nLayer);

	CMonsterClashInstance* GetClashInstance(const int nLayer);

	BOOL IsClash(CUser* pUser) const;
	void Process();
	/*REGISTER*/
	void StartRegister();
	BOOL IsRegisterTime() const { return GetState() == STATE_REGISTER; }
	BOOL IsRegisterRank(CUser* pUser) const;
	BOOL RegisterGuild(CUser* pUser);
	BOOL RegisterGuildMember(CUser* pUser, CUser* pMember);
	BOOL UnregisterGuildMember(CUser* pUser, CUser* pMember);
	/*REGISTER END*/

	/*CLASH*/
	BOOL IsClashTime() const { return (GetState() == STATE_WAIT || GetState() == STATE_CLASH); }
	//Wait
	BOOL IsWaitTime() const { return GetState() == STATE_WAIT; }
	void StartWait();
	BOOL CreateClashLayer(const DWORD dwGuildId);
	void TeleportAllToWait() const;

	//Clash
	BOOL IsClashFightTime() const { return GetState() == STATE_CLASH; }
	void StartFight();
	void StartAllInstance();
	void TeleportAllToFight() const;
	void EndClash();

	void KillMonster(CUser* pUser, const DWORD dwMonsterId);
	/*CLASH END*/
	/*Ranking*/
	void GiveItems();
	/*Ranking End*/

	/*Packet*/
	void SerializeRegisterWindow(CAr& ar)const;
	void SerializeRegisterMemberWindow(DWORD dwGuild, CAr& ar);
	/*Packet End*/
};
class CMonsterClashInstance
{
private:
	int nStateKillCount;
	int nKillCount;
	int nLayer;
	int nState;
	DWORD dwWorldId;

	time_t tTime;
public:
	CMonsterClashInstance();
	~CMonsterClashInstance();

	//setter getter
	void SetLayer(const int nLayer) { this->nLayer = nLayer; }
	int GetLayer() const { return nLayer; }

	void SetNextState();
	int GetState() const { return nState; }

	void SetWorldId(const DWORD dwWorldId) { this->dwWorldId = dwWorldId; }
	DWORD GetWorldId() const { return dwWorldId; }

	void SetKillCount(const int nKill) { nKillCount = nKill; }
	int GetKillCount() const { return nKillCount; }
	void AddKillCount() { ++nKillCount; }

	int GetStateKillCount() const { return nStateKillCount; }
	void SetStateKillCount(const int nKill) { nStateKillCount = nKill; }
	void AddStateKillCount() { ++nStateKillCount; }

	time_t GetTime()const { return tTime; }
	void SetTime(const time_t time) { tTime = time;}

	//class Function
	void AddMonster(const DWORD dwMonsterId, const D3DXVECTOR3 vPos, const BOOL bAggro) const;
	void KillMonster(const DWORD dwMonsterId);
	void End(const time_t Time);//sum up leftovers for ranking
};
#endif //__SYS_MONSTER_CLASH