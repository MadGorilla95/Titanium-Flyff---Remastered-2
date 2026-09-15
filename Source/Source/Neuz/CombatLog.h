#pragma once

#ifdef __COMBAT_LOG
enum EventType {
	ET_DAMAGE = 0,
	ET_HEALING = 1,
};

struct CombatLogEvent {
	EventType type;
	int value;
	int attacker_id;
	int defender_id;
	CString attacker_name;
	CString defender_name;
};

class CombatLog
{
public:
	CombatLog();
	~CombatLog();

	static CombatLog* GetInstance();

	void AddEvent(EventType type, int value, int Attacker, CString AttackerName, int Defender, CString DefenderName);
	void Clear(void);
	void Process();
	vector<CombatLogEvent>* GetLog() { return &log; }
	u_long GetPlayerDamage(int id);
	float GetPlayerDPS(int id);
	void ClearLog(void);
	void PrintLog(void);
	map<int, u_long> GetObjects() const;

private:
	vector<CombatLogEvent> log;
	time_t last_combat_tick;
	time_t first_combat_tick;
	map<int, u_long> player_damage;
	map<int, u_long> player_healing;
};
#endif //__COMBAT_LOG
