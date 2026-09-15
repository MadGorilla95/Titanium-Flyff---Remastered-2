#include "stdafx.h"
#include "Discord.h"

#ifdef __DISCORD_RPC
#define APP_ID "785155744517324841"

void CDiscord::Init() {
	DiscordEventHandlers Handle;
	memset(&Handle, 0, sizeof(Handle));
	//memset(m_szCharacterName, 0, sizeof(char) * 50);
	//memset(m_szWorldName, 0, sizeof(char) * 50);

	Discord_Initialize(APP_ID, &Handle, true, NULL);

	SetCharacterName("Logging in...");
	SetWorldName("-");
	m_nJob = -1;
	m_nLevel = 0;
#ifdef __SYS_REBIRTH
	m_nRebirthLevel = 0;
#endif //__SYS_REBIRTH
}

void CDiscord::SetBaseValues() {
	memset(&discordPresence, 0, sizeof(discordPresence));

	CString charString;
	CString worldString;
	CString jobIcon;
	CString jobString;

	//Level
#ifdef __SYS_REBIRTH
	if (m_nLevel > 0 && m_nRebirthLevel > 0)
		charString.Format("%s | Lvl. %d - RB %d", m_szCharacterName, m_nLevel, m_nRebirthLevel);
	else if (m_nLevel > 0)
		charString.Format("%s | Lvl. %d", m_szCharacterName, m_nLevel);
#else //__SYS_REBIRTH
	if (m_nLevel > 0)
		charString.Format("%s | Lvl. %d", m_szCharacterName, m_nLevel);
#endif //__SYS_REBIRTH
	else
		charString.Format("%s", m_szCharacterName);

	//World
	worldString.Format("Location: %s", m_szWorldName);


	if (m_nJob >= JOB_VAGRANT && m_nJob < MAX_JOB) 
	{
		jobIcon.Format("%s", prj.m_aPropJob[m_nJob].szJobIcon);
		jobString.Format("%s", prj.m_aJob[m_nJob].szName);
	}
	else 
	{
		jobIcon.Format("logo");
		jobString.Format("titanium-mmo.com");
	}

	discordPresence.details = charString;
	discordPresence.state = worldString;
	discordPresence.startTimestamp = time(nullptr);
	discordPresence.largeImageKey = "logo";
	discordPresence.largeImageText = "Titanium MMO";
	discordPresence.smallImageKey = jobIcon;
	discordPresence.smallImageText = jobString;
	Discord_UpdatePresence(&discordPresence);
}

void CDiscord::Update(LPCTSTR character, LPCTSTR world, int job, int level
#ifdef __SYS_REBIRTH
	, int rebirth
#endif //__SYS_REBIRTH
) {
	SetCharacterName(character);
	SetWorldName(world);

	if (level != -1)
		SetLevel(level);

#ifdef __SYS_REBIRTH
	if (rebirth != -1)
		SetRebirthLevel(rebirth);
#endif //__SYS_REBIRTH

	if (job != -1)
		SetCharJob(job);

	CString charString;
	CString worldString;
	CString jobIcon;
	CString jobString;

	//Level
#ifdef __SYS_REBIRTH
	if (m_nLevel > 0 && m_nRebirthLevel > 0)
		charString.Format("%s | Lvl. %d - RB %d", m_szCharacterName, m_nLevel, m_nRebirthLevel);
	else if (m_nLevel > 0)
		charString.Format("%s | Lvl. %d", m_szCharacterName, m_nLevel);
#else //__SYS_REBIRTH
	if (m_nLevel > 0)
		charString.Format("%s | Lvl. %d", m_szCharacterName, m_nLevel);
#endif //__SYS_REBIRTH
	else
		charString.Format("%s", m_szCharacterName);

	//World
	worldString.Format("Location: %s", m_szWorldName);


	if (m_nJob >= JOB_VAGRANT && m_nJob < MAX_JOB)
	{
		jobIcon.Format("%s", prj.m_aPropJob[m_nJob].szJobIcon);
		jobString.Format("%s", prj.m_aJob[m_nJob].szName);
	}
	else
	{
		jobIcon.Format("logo");
		jobString.Format("titanium-mmo.com");
	}

	discordPresence.smallImageKey = jobIcon;
	discordPresence.smallImageText = jobString;

	discordPresence.details = charString;
	discordPresence.state = worldString;
	Discord_UpdatePresence(&discordPresence);
}
#endif //__DISCORD_RPC