#pragma once

#ifdef __DISCORD_RPC
#include <discord_register.h>
#include <discord_rpc.h>
#include <Discord.h>
#include <Windows.h>

class CDiscord
{
public:
	void Init();
	void SetBaseValues();
	void SetCharacterName(LPCTSTR name) { this->m_szCharacterName = name; }
	void SetWorldName(LPCTSTR name) { this->m_szWorldName = name; }
	void SetCharJob(int job) { m_nJob = job; }
	void SetLevel(int level) { m_nLevel = level; }
#ifdef __SYS_REBIRTH
	void SetRebirthLevel(int rebirth) { m_nRebirthLevel = rebirth; }
#endif //__SYS_REBIRTH
	void Update(LPCTSTR character, LPCTSTR world, int job = -1, int level = -1
#ifdef __SYS_REBIRTH
		, int rebirth = -1
#endif //__SYS_REBIRTH
	);
private:
	LPCTSTR		m_szCharacterName;
	LPCTSTR		m_szWorldName;
	int			m_nJob;
	int			m_nLevel;
#ifdef __SYS_REBIRTH
	int			m_nRebirthLevel;
#endif //__SYS_REBIRTH
	DiscordRichPresence discordPresence;
};
#endif //__DISCORD_RPC