#include "stdafx.h"
#include "User.h"
extern CUserMng g_UserMng;
#include "DPCoreClient.h"
extern CDPCoreClient g_DPCoreClient;

#ifdef __SYS_MAP_BUFFS
#include "MapBuffManager.h"
#include "../XML/XMLWrapper.h"

CMapBuffManager::CMapBuffManager()
{
}


CMapBuffManager::~CMapBuffManager()
{
	map_buffs.clear();
}

CMapBuffManager* CMapBuffManager::GetInstance()
{
	static CMapBuffManager buff_manager;
	return &buff_manager;
}

void CMapBuffManager::Load()
{
	auto doc = new XMLWrapper("MapBuff.xml", "MapBuffs");
	bool bFound = false;
	vector<MapBuffProp> vec_prop;
	map_buffs.clear();
	for (auto map_buff : doc->Get())
	{
		vec_prop.clear();
		DWORD map_id = map_buff.GetID("ID", bFound);
		if (!bFound) {
			Error("CMapBuffManager::Load - Invalid Map ID");
			return;
		}

		auto buffs = map_buff.GetChildren("Buff");
		for (auto buff : buffs)
		{
			MapBuffProp p(buff.GetID("ID", bFound));
			if (!bFound) {
				Error("CMapBuffManager::Load - Invalid Buff ID %s for Map %s", buff.GetString("ID"), map_buff.GetString("ID"));
				return;
			}
			p.SetLevelRange(
				buff.GetInt("MinPlayerLevel", 0),
				buff.GetInt("MaxPlayerLevel", 0)
			);
			p.SetJob(
				buff.GetID("Job", bFound, 0)
			);
			CString startDate(buff.GetString("Start"));
			p.start_date = MapBuffDate(buff.GetInt("StartDay"), buff.GetInt("StartMonth"));
			p.end_date = MapBuffDate(buff.GetInt("EndDay"), buff.GetInt("EndMonth"));

			vec_prop.push_back(p);
		}

		map_buffs.insert(make_pair(
			map_id,
			vec_prop
		));
	}
	delete doc;
}

void CMapBuffManager::Handle(CMover* mover, DWORD world_id, bool is_player)
{
	if (!IsValidObj(mover))
		return;

	if (is_player && mover->IsPlayer() == FALSE)
		return;

	// Remove all existing map-buffs
	mover->m_buffs.RemoveMapBuff();

	// Check if world exists to prevent crashes
	if (!mover->GetWorld()) return;

	// Get new map-buffs to apply
	auto new_buffs = map_buffs.find(world_id);
	if (new_buffs != map_buffs.end())
	{
		CTime now(time(nullptr));
		auto vec_buffs = map_buffs[world_id];
		for (size_t i = 0; i < vec_buffs.size(); i++)
		{
			if (vec_buffs[i].min_player_level != 0 && mover->GetLevel() < vec_buffs[i].min_player_level) continue;
			if (vec_buffs[i].max_player_level != 0 && mover->GetLevel() > vec_buffs[i].max_player_level) continue;
			if (vec_buffs[i].job_exclusive != 0 && mover->IsInteriorityJob(vec_buffs[i].job_exclusive) == FALSE) continue;
			bool bApply = false;
			auto buff = vec_buffs[i];
			// Check if Start AND End date were set, else apply
			if (buff.start_date.day == -1
				|| buff.start_date.month == -1
				|| buff.end_date.day == -1
				|| buff.end_date.month == -1
				)
				bApply = true;

			if (buff.start_date.month == buff.end_date.month) {
				// If Start & End Date are in the same month, just check days
				if (now.GetDay() <= buff.end_date.day && now.GetDay() >= buff.start_date.day)
					bApply = true;
			}
			else {
				// If the End Date is in the future, check if current Month is lower or equal to end month
				if (now.GetMonth() == buff.end_date.month) {
					// If the current month is the end month, check for valid day
					if (now.GetDay() <= buff.end_date.day)
						bApply = true;
				}
				else {
					// Check if the end month is in the future
					if (now.GetMonth() > buff.end_date.month)
						continue;

					// If the end month is higher than current month, check if the day of the start day is valid
					if (now.GetMonth() <= buff.end_date.month) {
						if (now.GetDay() >= buff.start_date.day && now.GetMonth() >= buff.start_date.month)
							bApply = true;
					}
				}
			}
			if (bApply)
				mover->AddBuff(BUFF_EQUIP, vec_buffs[i].buff_id, 1, 999999999);
		}
	}
}

void CMapBuffManager::Reload(bool message)
{
	Load();

	for (auto user : g_UserMng.m_users)
	{
		if (IsValidObj(user.second) && user.second->GetWorld())
			this->Handle(user.second, user.second->GetWorld()->GetID(), true);
	}

	if (message) {
		for (auto world_iterator = map_buffs.begin(); world_iterator != map_buffs.end(); world_iterator++)
			g_DPCoreClient.SendCaption("New Map Buffs are available!", world_iterator->first);
	}
}
#endif //__SYS_MAP_BUFFS