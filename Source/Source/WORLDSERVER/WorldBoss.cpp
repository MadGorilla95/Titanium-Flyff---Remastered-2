#include "stdafx.h"

#ifdef __SYS_WORLDBOSS
#include "WorldBoss.h"
#include "worldmng.h"
#include "defineObj.h"
extern	CWorldMng	g_WorldMng;
#include "DPCoreClient.h"
extern CDPCoreClient g_DPCoreClient;
#include "../XML/XMLWrapper.h"

//////////// WORLD BOSS CONTROLLER ///////////
CWorldBossController::CWorldBossController()
{
}
CWorldBossController::~CWorldBossController()
{
	m_WorldBosses.clear();
}
CWorldBossController* CWorldBossController::Instance(void)
{
	static CWorldBossController sWorldBossControl;
	return &sWorldBossControl;
}
BOOL CWorldBossController::LoadScript(const char* szFile)
{
	auto doc = new XMLWrapper(szFile, "WorldBossProp");

	bool bFound = false;

	for (auto WB : doc->Get())
	{
		vWBProp prop;
		prop.dwMonsterID = WB.GetID("ID", bFound);
		if (!bFound)
		{
			Error("CWorldBossController::LoadScript - Invalid Boss-ID");
			return FALSE;
		}
		prop.bActiveAttack = WB.GetInt("Aggro");
		prop.dwWorld = WB.GetID("World", bFound);
		if (!bFound)
		{
			Error("CWorldBossController::LoadScript - Invalid World ID");
			return FALSE;
		}
		auto SpawnPosition = WB.GetVector(bFound);
		if (!bFound)
		{
			Error("CWorldBossController::LoadScript - Invalid Spawn Position");
			return FALSE;
		}
		prop.vPos_x = SpawnPosition.at(0);
		prop.vPos_y = SpawnPosition.at(1);
		prop.vPos_z = SpawnPosition.at(2);

		auto SpawnTimes = WB.SelectChild("Spawns").GetChildren("Spawn");
		for (auto Spawn : SpawnTimes)
		{
			WBSpawnTime spawnTime;
			int start = 0;
			CString str = Spawn.GetString("Time");
			std::string TimeString = str;
			spawnTime.SetDay(Spawn.GetInt("Day"));
			spawnTime.SetHour(stoi(TimeString.substr(0, 2)));
			spawnTime.SetMinute(stoi(TimeString.substr(3, 5)));
			spawnTime.SetSecond(stoi(TimeString.substr(6, 8)));
			prop.vecSpawnTimes.push_back(spawnTime);
		}

		auto LootTable = WB.SelectChild("LootTable").GetChildren("Loot");
		for (auto Item : LootTable)
		{
			prop.vecLoot.insert(make_pair(
				Item.GetID("ID", bFound),
				Item.GetInt("Count")
			));
			if (!bFound)
			{
				Error("CWorldBossController::LoadScript - Invalid Loot ID");
				return FALSE;
			}
		}

		m_WorldBosses.push_back(prop);
	}
	delete doc;
	return TRUE;
}
void CWorldBossController::Process(void)
{
	for (auto it = m_WorldBosses.begin(); it != m_WorldBosses.end(); it++)
	{
		if (IsSpawning(*it) && !IsSpawned(it->dwMonsterID))
			Spawn(*it);
	}
	for (auto spawn : m_aSpawnedBosses)
	{
		CTime spawnTime = spawn.second.ctSpawnTime;
		
#ifdef __AEGON_FIXES
		if (((spawnTime + CTimeSpan(0, 1, 0, 0)) < CTime::GetCurrentTime()) && spawn.second.pMover)
		{
			DeleteSpawn(spawn.second);
			break;
		}
#else
		if (((spawnTime + CTimeSpan(0, 1, 0, 0)) < CTime::GetCurrentTime()) && spawn.second.pMover)
			DeleteSpawn(spawn.second);
#endif

	}
}
BOOL CWorldBossController::IsSpawning(vWBProp pProp)
{
	CTime ctime = CTime::GetCurrentTime();
	for (auto spawn : pProp.vecSpawnTimes)
	{
		if (spawn.GetDay() == ctime.GetDayOfWeek())
			if (spawn.GetHour() == ctime.GetHour())
				if (spawn.GetMinute() == ctime.GetMinute())
					if (spawn.GetSecond() == ctime.GetSecond())
						return TRUE;
	}
	return FALSE;
}
void CWorldBossController::Spawn(vWBProp pProp)
{
	CWorld* pWorld = g_WorldMng.GetWorld(pProp.dwWorld);
	if (!pWorld)
		return;
	CMover* pMonster = static_cast<CMover*>(::CreateObj(D3DDEVICE, OT_MOVER, pProp.dwMonsterID));
	D3DXVECTOR3 pos;
	pos.x = (float)pProp.vPos_x;
	pos.y = (float)pProp.vPos_y;
	pos.z = (float)pProp.vPos_z;
	if (pMonster)
	{
		pMonster->m_bActiveAttack = pProp.bActiveAttack;
		pMonster->SetPos(pos);
		pMonster->InitMotion(MTI_STAND);
		pMonster->UpdateLocalMatrix();
		pMonster->AddItToGlobalId();
		pMonster->m_bIsWorldBoss = TRUE;
		pWorld->AddObj(pMonster, FALSE, nDefaultLayer);

		WBSPAWN mobSpawn;
		mobSpawn.mobID = pProp.dwMonsterID;
		mobSpawn.ctSpawnTime = CTime::GetCurrentTime();
		mobSpawn.pMover = pMonster;
		m_aSpawnedBosses.insert(make_pair(pProp.dwMonsterID, mobSpawn));

		CString spawnString;
		spawnString.Format("World Boss %s has spawned in %s.", pMonster->GetName(), g_WorldMng.GetWorld(pProp.dwWorld)->m_szWorldName);
#ifdef __AEGON_ADVANCED_ANNOUNCEMENT
		g_DPCoreClient.SendSystem(spawnString, 3, 0);	// World Event - Default Shout Font
#else //__AEGON_ADVANCED_ANNOUNCEMENT
		g_DPCoreClient.SendSystem(spawnString);
#endif //__AEGON_ADVANCED_ANNOUNCEMENT
	}
}
WBLOOT CWorldBossController::GetLoot(DWORD dwMonsterID)
{
	for (auto it = m_WorldBosses.begin(); it != m_WorldBosses.end(); it++)
	{
		if (it->dwMonsterID == dwMonsterID)
			return (*it).vecLoot;
	}
	return WBLOOT();
}
bool CWorldBossController::IsSpawned(DWORD dwMonsterID)
{
	for (auto spawn : m_aSpawnedBosses)
	{
		if (spawn.first == dwMonsterID)
			return true;
	}

	return false;
}
void CWorldBossController::DeleteSpawn(WBSPAWN spawn)
{
	if (!spawn.pMover) return;
	spawn.pMover->Delete();
	m_aSpawnedBosses.erase(spawn.mobID);
	MoverProp* moverProp = prj.GetMoverProp(spawn.mobID);
	if (moverProp)
	{
		CString despawnStr;
		despawnStr.Format("World Boss %s has vanished.", moverProp->szName);
#ifdef __AEGON_ADVANCED_ANNOUNCEMENT
		g_DPCoreClient.SendSystem(despawnStr, 3, 0);	// World Event - Default Shout Font
#else //__AEGON_ADVANCED_ANNOUNCEMENT
		g_DPCoreClient.SendSystem(despawnStr);
#endif //__AEGON_ADVANCED_ANNOUNCEMENT
	}
}
void CWorldBossController::BossDeath(CMover* mover) {
	if (!mover->IsNPC()) return;
	if (!mover->GetProp()) return;

	for (auto const& spawn : m_aSpawnedBosses) 
	{
		if (spawn.first == mover->GetProp()->dwID) 
		{
			m_aSpawnedBosses.erase(spawn.first);
			CString announce;
			announce.Format("World Boss %s has been defeated.", mover->GetProp()->szName);
#ifdef __AEGON_ADVANCED_ANNOUNCEMENT
			g_DPCoreClient.SendSystem(announce, 3, 0);	// World Event - Default Shout Font
#else //__AEGON_ADVANCED_ANNOUNCEMENT
			g_DPCoreClient.SendSystem(announce);
#endif //__AEGON_ADVANCED_ANNOUNCEMENT
			return;
		}
	}
}
#endif // __SYS_WORLDBOSS
