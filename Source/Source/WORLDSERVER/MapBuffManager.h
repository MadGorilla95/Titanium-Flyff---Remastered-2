#pragma once

#ifdef __SYS_MAP_BUFFS
struct MapBuffDate {
	int day;
	int month;

	MapBuffDate() {
		day = -1;
		month = -1;
	}

	MapBuffDate(int d, int m) {
		day = d;
		month = m;
	}
};

struct MapBuffProp {
	DWORD buff_id;
	UINT min_player_level;
	UINT max_player_level;
	DWORD job_exclusive;
	MapBuffDate start_date;
	MapBuffDate end_date;

	MapBuffProp(DWORD buffid) {
		buff_id = buffid;
		min_player_level = 0;
		max_player_level = 0;
		job_exclusive = 0;
	}

	void SetLevelRange(int min_level, int max_level) {
		min_player_level = min_level;
		max_player_level = max_level;
	}

	void SetJob(DWORD job) {
		job_exclusive = job;
	}
};

class CMapBuffManager
{
public:
	CMapBuffManager();
	~CMapBuffManager();

	static CMapBuffManager* GetInstance();
	void Load();
	void Reload(bool message = true);
	void Handle(CMover* mover, DWORD world_id, bool is_player = true);
private:
	map<DWORD, vector<MapBuffProp>> map_buffs;
};
#endif //__SYS_MAP_BUFFS