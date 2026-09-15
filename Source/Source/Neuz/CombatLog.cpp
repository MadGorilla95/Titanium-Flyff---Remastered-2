#include "stdafx.h"
#include "CombatLog.h"
#include "WndManager.h"
#include "ResData.h"

#ifdef __COMBAT_LOG
CombatLog::CombatLog()
{
	Clear();
}


CombatLog::~CombatLog()
{
	Clear();
}


CombatLog* CombatLog::GetInstance() {
	static CombatLog inst;
	return &inst;
}

void CombatLog::Process() {
	if (last_combat_tick + SEC(15) < GetTickCount() && (player_damage.size() > 0 || player_healing.size() > 0)) {
		Clear();
	}
	if (last_combat_tick + MIN(15) < GetTickCount() && log.size() > 0)
		ClearLog();
}

void CombatLog::AddEvent(EventType type, int value, int attacker_id, CString attacker_name, int defender_id, CString defender_name) {
	if (player_damage.size() == 0 && player_healing.size() == 0)
		first_combat_tick = GetTickCount();

	CombatLogEvent ev;

	ev.type = type;
	ev.value = value;
	ev.attacker_id = attacker_id;
	ev.attacker_name = attacker_name;
	ev.defender_id = defender_id;
	ev.defender_name = defender_name;

	log.push_back(ev);

	if (type == ET_DAMAGE) {
		auto find_player = player_damage.find(attacker_id);
		if (find_player != player_damage.end())
			find_player->second += value;
		else
			player_damage.insert(make_pair(attacker_id, value));
	}
	else {
		auto find_player = player_healing.find(attacker_id);
		if (find_player != player_healing.end())
			find_player->second += value;
		else
			player_healing.insert(make_pair(attacker_id, value));
	}

	last_combat_tick = GetTickCount();
}

u_long CombatLog::GetPlayerDamage(int id) {

	auto find_player = player_damage.find(id);
	if (find_player != player_damage.end())
		return find_player->second;
	else
		return 0;
}

float CombatLog::GetPlayerDPS(int id) {
	auto dmg = GetPlayerDamage(id);
	if (dmg == 0) return 0.0f;

	float time_passed = (GetTickCount() - first_combat_tick) / 1000;

	if (time_passed > 0.0f)
		return (float)(dmg / time_passed);

	return 0.0f;
}

void CombatLog::Clear(void) {
	player_damage.clear();
	player_healing.clear();
}
void CombatLog::ClearLog(void) {
	log.clear();
}

void CombatLog::PrintLog(void) {
	auto fp = fopen(MakeFileNameDate("CombatLog.txt"), "a");
	if (fp == NULL)
		return;

	char szLog[1000];

	for (auto eve : log) {
		CString type;
		if (eve.type == ET_DAMAGE) {
			type = "damaged";
		}
		else {
			type = "healed";
		}
		sprintf(szLog, "%s %s %s for %d.\n",
			eve.attacker_name,
			type,
			eve.defender_name,
			eve.value
		);
		fprintf(fp, szLog);
	}
	fclose(fp);
}
map<int, u_long> CombatLog::GetObjects() const {
	return player_damage;
}
#endif //__COMBAT_LOG