#include "StdAfx.h"

#ifdef __SYS_MONSTER_CLASH
#include "User.h"
#include "UserMacro.h"
#include "DPCoreClient.h"
#include "DPDatabaseClient.h"
#include "worldmng.h"
#include "Guild.h"
extern	CGuildMng	g_GuildMng;
extern CUserMng g_UserMng;
extern CWorldMng g_WorldMng;
extern CDPCoreClient g_DPCoreClient;
extern CDPDatabaseClient g_dpDBClient;
/*//////////////////////////
// Monster Clash Manager
///////////////////////////*/
CMonsterClashMng::CMonsterClashMng()
	: Timer(0)
{
	nState = STATE_CLOSED;
}

CMonsterClashMng::~CMonsterClashMng()
{
	vector<DWORD> vecGuildId;
	for (const auto& Guild : aRegisteredGuild)//sum up guilds to not destroy the iteration while looping and removing
	{
		vecGuildId.push_back(Guild.first);
	}
	for (const auto& GuildId : vecGuildId)
		RemoveRegisteredGuild(GuildId);
}

void CMonsterClashMng::GUILD::AddUser(CUser* pUser)
{
	aRegisteredMember.insert(make_pair(pUser->m_idPlayer, pUser));
}

void CMonsterClashMng::GUILD::RemoveUser(CUser* pUser)
{
	aRegisteredMember.erase(pUser->m_idPlayer);
}

CMonsterClashMng* CMonsterClashMng::GetInstance()
{
	static CMonsterClashMng sMonsterClashMng;
	return &sMonsterClashMng;
}

void CMonsterClashMng::ReadScript(const LPCTSTR szFile)
{
	CScript s;
	if (!s.Load(szFile))
	{
		Error("CMonsterClashMng::ReadScript File not found %s", szFile);
		return;
	}
	GetProp().aStateProp.clear();
	GetProp().vecClashTimes.clear();
	GetProp().aClashRewards.clear();
	s.GetToken();
	while (s.tok != FINISHED)
	{
		if (s.Token == "SetMaxGuildMember")
		{
			s.GetToken(); // =
			GetProp().nMaxRegisterdMember = s.GetNumber();
		}
		else if (s.Token == "SetRegisterMinutes")
		{
			s.GetToken(); // =
			GetProp().nRegisterMinutes = s.GetNumber();
		}
		else if (s.Token == "SetFightMinutes")
		{
			s.GetToken(); // =
			GetProp().nFightMinutes = s.GetNumber();
		}
		else if (s.Token == "SetWaitMinutes")
		{
			s.GetToken(); // =
			GetProp().nReadyMinutes = s.GetNumber();
		}
		else if (s.Token == "SetRegisterRank")
		{
			s.GetToken();// =
			GetProp().nMemberRankRegister = s.GetNumber();
		}
		else if (s.Token == "SetRegisterCost")
		{
			s.GetToken();// =
			GetProp().nRegisterCost = s.GetNumber();
		}
		else if (s.Token == "SetClashMapId")
		{
			s.GetToken();// =
			GetProp().dwClashMapId = s.GetNumber();
		}
		else if (s.Token == "SetReturnPort")
		{
			s.GetToken();//(
			GetProp().endPortMapId = s.GetNumber();
			s.GetToken();//,
			GetProp().endPortPos.x = s.GetFloat();
			s.GetToken();//,
			GetProp().endPortPos.z = s.GetFloat();
			s.GetToken();//)
		}
		else if (s.Token == "SetWaitPort")
		{
			s.GetToken();//(
			GetProp().waitPortPos.x = s.GetFloat();
			s.GetToken();//,
			GetProp().waitPortPos.z = s.GetFloat();
			s.GetToken();//)
		}
		else if (s.Token == "SetFightPort")
		{
			s.GetToken();//(
			GetProp().fightPortPos.x = s.GetFloat();
			s.GetToken();//,
			GetProp().fightPortPos.z = s.GetFloat();
			s.GetToken();//)
		}
		else if (s.Token == "AddClashTime")
		{
			CLASH_PROP::CLASH_TIME sTime;
			s.GetToken();//(
			sTime.nDayOfWeek = s.GetNumber();
			s.GetToken();//,
			sTime.nHour = s.GetNumber();
			s.GetToken();//,
			sTime.nMinute = s.GetNumber();
			s.GetToken();//)

			GetProp().vecClashTimes.push_back(sTime);
		}
		else if (s.Token == "AddReward")
		{
			s.GetToken();//(
			int nRank = s.GetNumber();
			s.GetToken();//,
			DWORD dwItemid = s.GetNumber();
			s.GetToken();//,
			int nNum = s.GetNumber();
			s.GetToken();//)

			auto it = GetProp().aClashRewards.find(nRank);
			if (it == GetProp().aClashRewards.end())
			{
				CLASH_PROP::CLASH_REWARD sReward;
				sReward.ItemReward.insert(make_pair(dwItemid, nNum));

				GetProp().aClashRewards.insert(make_pair(nRank, sReward));
			}
			else
			{
				it->second.ItemReward.insert(make_pair(dwItemid, nNum));
			}
		}
		else if (s.Token == "AddState")
		{
			int nState = s.GetNumber();
			STATE sState;
			auto it = GetProp().aStateProp.insert(make_pair(nState, sState));


			s.GetNumber();//{
			while (s.Token != "}")
			{
				if (s.Token == "AddMonster")
				{
					STATE::MONSTER sMonster;
					s.GetToken();//(
					sMonster.dwMonsterId = s.GetNumber();
					s.GetToken();//,
					sMonster.vPos.x = s.GetFloat();
					s.GetToken();//,
					sMonster.vPos.z = s.GetFloat();
					s.GetToken();//,
					sMonster.bAggro = s.GetNumber();
					s.GetToken();//)

					it.first->second.vecMonster.push_back(sMonster);
				}
				s.GetToken();
			}
		}
		s.GetToken();
	}
}

CMonsterClashInstance* CMonsterClashMng::GetClashInstance(const int nLayer)
{
	auto it = aClashInstance.find(nLayer);
	if (it == aClashInstance.end())
		return nullptr;

	return &it->second;
}

CMonsterClashMng::STATE* CMonsterClashMng::GetStateProp(const int nState)
{
	auto it = GetProp().aStateProp.find(nState);
	if (it == GetProp().aStateProp.end())
		return nullptr;

	return &it->second;
}

int CMonsterClashMng::GetMaxStateKillCount() const
{
	int nKillCount = 0;
	for (const auto& State : sProp.aStateProp)
	{
		nKillCount += State.second.GetStateCount();
	}
	return nKillCount;
}

int CMonsterClashMng::GetMaxState() const
{
	return sProp.aStateProp.size();
}

CMonsterClashMng::GUILD* CMonsterClashMng::GetRegisteredGuild(const DWORD dwGuildId)
{
	auto it = aRegisteredGuild.find(dwGuildId);
	if (it != aRegisteredGuild.end())
		return &it->second;

	return nullptr;
}

CMonsterClashMng::GUILD* CMonsterClashMng::GetRegisteredUser(CUser* pUser)
{
	for (auto& Guild : aRegisteredGuild)
	{
		for (const auto& user : Guild.second.aRegisteredMember)
		{
			if (user.first == pUser->m_idPlayer)
				return &Guild.second;
		}
	}
	return nullptr;
}

void CMonsterClashMng::RemoveRegisteredGuild(const DWORD dwGuildId)
{
	auto pGuild = GetRegisteredGuild(dwGuildId);
	if (!pGuild)
	{
		Error("CMonsterClashMng::RemoveRegisteredGuild pGuild = nullptr");
		return;
	}

	if(IsClashTime())
		RemoveClashInstance(dwGuildId);

	pGuild->RemoveUser();
	RemoveGuild(dwGuildId);

	if (aRegisteredGuild.size() == 0)
		EndClash();
}

void CMonsterClashMng::RemoveUser(CUser* pUser)
{
	if (!IsValidObj(pUser))
	{
		Error("CMonsterClashMng::RemoveUser pUser = nullptr");
		return;
	}
	auto pGuild = GetRegisteredUser(pUser);
	if (!pGuild)
	{
		Error("CMonsterClashMng::RemoveUser not registed %07d", pUser->GetName());
		return;
	}

	if (IsClashTime() == TRUE)
	{
		//Teleport Out
		TeleportUserOut(pUser);
	}
	//Remove from Registered Guild
	pGuild->RemoveUser(pUser);
	if (pGuild->RegisteredMemberCount() == 0)
	{
		RemoveRegisteredGuild(pGuild->dwGuildId);
	}
	pUser->AddMonsterClashInactive();
}

void CMonsterClashMng::TeleportUserOut(CUser* pUser) const
{
	pUser->Replace(g_uIdofMulti, GetPropTeleportOutMapId(), GetPropTeleportOutMapPos(), REPLACE_FORCE, nDefaultLayer);
}

void CMonsterClashMng::StartRegister()
{
	if (GetState() != STATE_CLOSED)
	{
		Error("CMonsterClashMng::StartRegister State is not closed");
		return;
	}
	SetState(STATE_REGISTER);
	Timer = CTime::GetCurrentTime().GetTime();
	Timer += CTimeSpan(0, 0, GetProp().nRegisterMinutes, 0).GetTimeSpan();
	//Shout
#ifdef __AEGON_ADVANCED_ANNOUNCEMENT
	g_DPCoreClient.SendSystem(prj.GetText(TID_GAME_MONSTERCLASH_REGISTER_OPEN), 3, 0);	// World Event - Default Shout Font
#else //__AEGON_ADVANCED_ANNOUNCEMENT
	g_DPCoreClient.SendSystem(prj.GetText(TID_GAME_MONSTERCLASH_REGISTER_OPEN));
#endif //__AEGON_ADVANCED_ANNOUNCEMENT
}

BOOL CMonsterClashMng::IsRegisterRank(CUser* pUser) const
{
	CGuild* pGuild = pUser->GetGuild();
	if (!pGuild)
		return FALSE;

	CGuildMember* pMember = pGuild->GetMember(pUser->m_idPlayer);
	if (!pMember)
		return FALSE;

	if (pMember->m_nMemberLv > GetPropMemberRegisterRank())
		return FALSE;

	return TRUE;
}

BOOL CMonsterClashMng::RegisterGuild(CUser* pUser)
{
	if (IsRegisterTime() == FALSE)
	{
		pUser->AddDefinedText(TID_GAME_MONSTERCLASH_REGISTER_CLOSED);
		return FALSE;
	}

	if (!IsRegisterRank(pUser))
	{
		pUser->AddDefinedText(TID_GAME_MONSTERCLASH_REGISTER_UNABLE);
		return FALSE;
	}

	if (GetRegisteredGuild(pUser->m_idGuild))
	{
		pUser->AddDefinedText(TID_GAME_MONSTERCLASH_REGISTER_ALREADY);
		return FALSE;
	}

	if (pUser->GetGold() < GetProp().nRegisterCost)
	{
		pUser->AddDefinedText(TID_GAME_MONSTERCLASH_REGISTER_PENYA);
		return FALSE;
	}

	pUser->AddGold(-GetProp().nRegisterCost);
	GUILD sGuild(pUser->m_idGuild);
	sGuild.AddUser(pUser);

	AddGuild(sGuild);

	return TRUE;
}

BOOL CMonsterClashMng::RegisterGuildMember(CUser* pUser, CUser* pMember)
{
	if (IsRegisterTime() == FALSE)
	{
		pUser->AddDefinedText(TID_GAME_MONSTERCLASH_REGISTER_CLOSED);
		return FALSE;
	}

	if (!IsRegisterRank(pUser))
	{
		pUser->AddDefinedText(TID_GAME_MONSTERCLASH_REGISTER_UNABLE);
		return FALSE;
	}

	auto* pGuild = GetRegisteredGuild(pUser->m_idGuild);
	if (!pGuild)
	{
		pUser->AddDefinedText(TID_GAME_MONSTERCLASH_REGISTER_NOT);
		return FALSE;
	}

	if (GetRegisteredUser(pMember))
	{
		pUser->AddDefinedText(TID_GAME_MONSTERCLASH_REGISTER_MEMBER_ALREADY);
		return FALSE;
	}

	pGuild->AddUser(pMember);

	return TRUE;
}

BOOL CMonsterClashMng::UnregisterGuildMember(CUser* pUser, CUser* pMember)
{
	if (IsRegisterTime() == FALSE)
	{
		pUser->AddDefinedText(TID_GAME_MONSTERCLASH_REGISTER_CLOSED);
		return FALSE;
	}

	if (!IsRegisterRank(pUser))
	{
		pUser->AddDefinedText(TID_GAME_MONSTERCLASH_REGISTER_UNABLE);
		return FALSE;
	}

	auto* pGuild = GetRegisteredGuild(pUser->m_idGuild);
	if (!pGuild)
	{
		pUser->AddDefinedText(TID_GAME_MONSTERCLASH_REGISTER_NOT);
		return FALSE;
	}

	if (!GetRegisteredUser(pMember))
	{
		pUser->AddDefinedText(TID_GAME_MONSTERCLASH_REGISTER_MEMBER_NOT);
		return FALSE;
	}

	if (pGuild->RegisteredMemberCount() == 1)
	{
		pUser->AddDefinedText(TID_GAME_MONSTERCLASH_REGISTER_MEMBER_ATLEAST1);
		return FALSE;
	}
	pGuild->RemoveUser(pMember);

	return TRUE;
}

void CMonsterClashMng::AddClashInstance(const int nLayer)
{
	CString strDebug;
	strDebug.Format("Added Clash instance %d\n", nLayer);
	OutputDebugString(strDebug);

	CMonsterClashInstance Instance;
	Instance.SetWorldId(GetProp().dwClashMapId);
	Instance.SetLayer(nLayer);
	aClashInstance.insert(make_pair(nLayer, Instance));
}

BOOL CMonsterClashMng::IsClash(CUser* pUser) const
{
	if (GetState() == STATE_REGISTER)
	{
		for (const auto& Guild : aRegisteredGuild)
		{
			for (const auto& user : Guild.second.aRegisteredMember)
			{
				if (user.first == pUser->m_idPlayer)
					return TRUE;
			}
		}
	}
	else if (IsClashTime())
		return (pUser->GetWorld() && pUser->GetWorld()->GetID() == sProp.dwClashMapId);

	return FALSE;
}

void CMonsterClashMng::RemoveClashInstance(const int nLayer)
{
	auto it = aClashInstance.find(nLayer);
	if (it == aClashInstance.end())
	{
		Error("CMonsterClashMng::RemoveClashInstance Layer not found");
		return;
	}
	CWorld* pWorld = g_WorldMng.GetWorld(GetProp().dwClashMapId);
	if (!pWorld || !pWorld->m_linkMap.GetLinkMap(static_cast<int>(nLayer)))
	{
		Error("CMonsterClashMng::RemoveClashInstance() - LinkMap Not Exist! nLayer:%d", nLayer);
		return;
	}
	auto* pGuild = GetRegisteredGuild(nLayer);
	if (pGuild)
	{
		for (const auto& User : pGuild->aRegisteredMember)
		{
			TeleportUserOut(User.second);
			User.second->AddMonsterClashInactive();
		}
	}
	pWorld->Invalidate(static_cast<int>(nLayer));

	aClashInstance.erase(nLayer);
}

void CMonsterClashMng::StartWait()
{
	if (GetState() != STATE_REGISTER)
	{
		Error("CMonsterClashMng::StartWait State is not Register");
		return;
	}
#ifdef __AEGON_ADVANCED_ANNOUNCEMENT
	g_DPCoreClient.SendSystem(prj.GetText(TID_GAME_MONSTERCLASH_REGISTER_CLOSED), 3, 0);	// World Event - Default Shout Font
#else //__AEGON_ADVANCED_ANNOUNCEMENT
	g_DPCoreClient.SendSystem(prj.GetText(TID_GAME_MONSTERCLASH_REGISTER_CLOSED));
#endif //__AEGON_ADVANCED_ANNOUNCEMENT
	if (GetRegisteredGuildCount() == 0)
	{
		SetState(STATE_CLOSED);
		return;
	}
	SetState(STATE_WAIT);
	Timer = CTime::GetCurrentTime().GetTime();
	Timer += CTimeSpan(0, 0, GetProp().nReadyMinutes, 0).GetTimeSpan();

	vector<DWORD> vecGuildRemove;//sum up failed Guilds to not destroy iteration
	for (auto& Guild : aRegisteredGuild)
	{
		if (!CreateClashLayer(Guild.first))
		{
			Error("Failed Create Clash Layer Guild %d", Guild.first);
			vecGuildRemove.push_back(Guild.first);
		}
		else
			AddClashInstance(Guild.first);
	}
	for(const auto & GuildId : vecGuildRemove)
		RemoveRegisteredGuild(GuildId);

	TeleportAllToWait();
	for (const auto& Guild : aRegisteredGuild)
	{
		for (const auto& Member : Guild.second.aRegisteredMember)
		{
			Member.second->AddMonsterClashState(STATE_WAIT, Timer);
		}
	}
}

BOOL CMonsterClashMng::CreateClashLayer(const DWORD dwGuildId)
{
	CWorld* pWorld = g_WorldMng.GetWorld(GetProp().dwClashMapId);
	if (!pWorld || pWorld->m_linkMap.GetLinkMap(static_cast<int>(dwGuildId)))
	{
		Error("CMonsterClashMng::CreateClashLayer() - LinkMap Exist!, ClashLayer:%d, WorldID:%d", dwGuildId, GetProp().dwClashMapId);
		return FALSE;
	}

	if (!pWorld->CreateLayer(static_cast<int>(dwGuildId)))
	{
		Error("CMonsterClashMng::CreateClashLayer() - CreateLayer Failed!, ClashLayer:%d, WorldID:%d", dwGuildId, GetProp().dwClashMapId);
		return FALSE;
	}
	return TRUE;
}

void CMonsterClashMng::TeleportAllToWait() const
{
	map<DWORD, CString> aGuildAct;
	for (const auto& Guild : aRegisteredGuild)
	{
		CGuild* pGuild = g_GuildMng.GetGuild(Guild.first);
		CString strGuild = pGuild == nullptr ? "Unkown" : pGuild->m_szGuild;

		aGuildAct.insert(make_pair(Guild.first, strGuild));
	}
	int nMaxKillCount = GetMaxStateKillCount();
	int nMaxStage = GetMaxState() - 1;
	for (auto& Guild : aRegisteredGuild)
	{
		for (auto& User : Guild.second.aRegisteredMember)
		{
			User.second->REPLACE(g_uIdofMulti, GetClashMapId(), GetPropTeleportWaitMapPos(), REPLACE_FORCE, Guild.first);
			User.second->AddMonsterClashActive(aGuildAct, nMaxKillCount, nMaxStage);
		}
	}
}

void CMonsterClashMng::StartFight()
{
	if (GetState() != STATE_WAIT)
	{
		Error("CMonsterClashMng::StartFight State is not Wait");
		return;
	}
	if (GetRegisteredGuildCount() == 0)
	{
		SetState(STATE_CLOSED);
		return;
	}
	SetState(STATE_CLASH);
	Timer = CTime::GetCurrentTime().GetTime();
	Timer += CTimeSpan(0, 0, GetProp().nFightMinutes, 0).GetTimeSpan();

	StartAllInstance();
	TeleportAllToFight();

	for (const auto& Guild : aRegisteredGuild)
	{
		for (const auto& Member : Guild.second.aRegisteredMember)
		{
			Member.second->AddMonsterClashState(STATE_CLASH, Timer);
		}
	}
}

void CMonsterClashMng::TeleportAllToFight() const
{
	for (auto& Guild : aRegisteredGuild)
	{
		for (auto& User : Guild.second.aRegisteredMember)
			User.second->REPLACE(g_uIdofMulti, GetClashMapId(), GetPropTeleportFightMapPos(), REPLACE_FORCE, Guild.first);
	}
}

void CMonsterClashMng::StartAllInstance()
{
	for (auto& Instance : aClashInstance)
	{
		Instance.second.SetNextState();
	}
}

void CMonsterClashMng::EndClash()
{
	if (GetState() != STATE_CLASH)
	{
		Error("CMonsterClashMng::EndClash State is not Clash");
		return;
	}
	

	time_t CurrTime = CTime::GetCurrentTime().GetTime();
	for (auto& Instance : aClashInstance)
	{
		Instance.second.End(CurrTime);
	}
	GiveItems();
	vector<DWORD> vecGuilds;
	for (auto& Guild : aRegisteredGuild)//sum up Guilds to not destroy iteration
	{
		vecGuilds.push_back(Guild.first);
	}
	for (auto& GuildId : vecGuilds)
	{
		RemoveRegisteredGuild(GuildId);
	}
	SetState(STATE_CLOSED);
}

bool SortGuildRank(CMonsterClashInstance& Instance1, CMonsterClashInstance& Instance2)
{
	return Instance1.GetTime() < Instance2.GetTime();
}

void CMonsterClashMng::GiveItems()
{
	vector<CMonsterClashInstance> vecInstance;
	for (const auto& Instance : aClashInstance)
	{
		vecInstance.push_back(Instance.second);
	}
	std::sort(vecInstance.begin(), vecInstance.end(), SortGuildRank);

	int nRank = 1;
	for (const auto& Instance : vecInstance)
	{
		auto* pGuild = GetRegisteredGuild(Instance.GetLayer());
		if (pGuild)
		{
			for (const auto& User : pGuild->aRegisteredMember)
			{
				auto RewardIterator = GetProp().aClashRewards.find(nRank);
				if(RewardIterator != GetProp().aClashRewards.end())
				{
					for (auto& Reward : RewardIterator->second.ItemReward)
					{
						CItemElem itemElem;
						itemElem.m_dwItemId = Reward.first;
						itemElem.m_nItemNum = Reward.second;
						if (!User.second->CreateItem(&itemElem))
							g_dpDBClient.SendQueryPostMail(User.second->m_idPlayer, 0, itemElem, 0, "Monster Clash", "");
					}
				}
			}
		}
		++nRank;
	}
}

void CMonsterClashMng::KillMonster(CUser* pUser, const DWORD dwMonsterId)
{
	CString strDebug;
	strDebug.Format("Clash Kill Monster Layer %d\n", pUser->m_idGuild);
	OutputDebugString(strDebug);

	auto Instance = GetClashInstance(pUser->m_idGuild);
	if (!Instance)
		return;

	Instance->KillMonster(dwMonsterId);

	int nKillCount = Instance->GetKillCount();
	time_t tTime = Instance->GetTime();
	int nState = Instance->GetState();
	for (const auto& Guild : aRegisteredGuild)
	{
		for (const auto& Member : Guild.second.aRegisteredMember)
		{
			Member.second->AddMonsterClashKillCount(Guild.first, nKillCount, tTime, nState);
		}
	}
}

void CMonsterClashMng::Process()
{
	CTime CurrTime = CTime::GetCurrentTime();
	if (GetState() == STATE_CLOSED)
	{		
		for (const auto& ClashTime : GetProp().vecClashTimes)
		{
			if (CurrTime.GetDayOfWeek() == ClashTime.nDayOfWeek &&
				CurrTime.GetHour() == ClashTime.nHour &&
				CurrTime.GetMinute() == ClashTime.nMinute)
			{
				StartRegister();
			}
		}
	}
	else if (GetState() == STATE_REGISTER)
	{
		if (CurrTime > Timer)
		{
			StartWait();
		}
	}
	else if (GetState() == STATE_WAIT)
	{
		if (CurrTime > Timer)
		{
			StartFight();
		}
	}
	else if (GetState() == STATE_CLASH)
	{
		if (CurrTime > Timer)
		{
			EndClash();
			Timer = 0;
		}
	}
}

void CMonsterClashMng::SerializeRegisterWindow(CAr& ar)const
{
#if _WIN64
	ar << static_cast<unsigned long>(GetRegisteredGuildCount());
#else
	ar << GetRegisteredGuildCount();
#endif
	for (const auto& Guild : aRegisteredGuild)
	{
		ar << Guild.first;
		CGuild* pGuild = g_GuildMng.GetGuild(Guild.first);
		if (pGuild)
			ar.WriteString(pGuild->m_szGuild);
		else
			ar.WriteString("unkown");
	}
}

void CMonsterClashMng::SerializeRegisterMemberWindow(DWORD dwGuild, CAr& ar)
{
	auto Guild = GetRegisteredGuild(dwGuild);
	if (!Guild)
	{
#if _WIN64
		ar << (unsigned long)0;
		ar << (unsigned long)0;
#else
		ar << (size_t)0;
		ar << (size_t)0;
#endif
	}
	else
	{
#if _WIN64
		ar << static_cast<unsigned long>(Guild->RegisteredMemberCount());
#else
		ar << Guild->RegisteredMemberCount();
#endif
		for (const auto& User : Guild->aRegisteredMember)
		{
			ar << User.first;
			ar.WriteString(User.second->GetName());
		}
		CGuild* pGuild = g_GuildMng.GetGuild(Guild->dwGuildId);
		if (pGuild)
		{
			u_long uOffset = ar.GetOffset();
#if _WIN64
			ar << (unsigned long)0;
			unsigned long nMember = 0;
#else
			ar << (size_t)0;
			size_t nMember = 0;
#endif

			for (const auto& Member : pGuild->m_mapPMember)
			{
				if (Guild->aRegisteredMember.find(Member.first) == Guild->aRegisteredMember.end())
				{
					CUser* pMember = g_UserMng.GetUserByPlayerID(Member.second->m_idPlayer);
					if (IsValidObj(pMember))
					{
						ar << Member.second->m_idPlayer;	
						ar.WriteString(pMember->GetName());				
						++nMember;
					}
				}
			}
			GETBLOCK(ar, lpBlock, nBlockSize);
#if _WIN64
			* (UNALIGNED unsigned long*)(lpBlock + uOffset) = nMember;
#else
			*(UNALIGNED size_t*)(lpBlock + uOffset) = nMember;
#endif
		}
		else
#if _WIN64
			ar << (unsigned long)0;
#else
			ar << (size_t)0;
#endif
	}
}
/*//////////////////////////
// Monster Clash Instance
///////////////////////////*/
CMonsterClashInstance::CMonsterClashInstance()
	: nLayer(0),
	nKillCount(0),
	nState(-1),
	dwWorldId(0),
	nStateKillCount(0),
	tTime(0)
{

}

CMonsterClashInstance::~CMonsterClashInstance()
{

}

void CMonsterClashInstance::SetNextState()
{
	++nState;
	SetStateKillCount(0);
	auto State = CMonsterClashMng::GetInstance()->GetStateProp(GetState());
	if (!State)
	{
		//finished
		tTime = CTime::GetCurrentTime().GetTime();
	}
	else
	{
		for (const auto& Monster : State->vecMonster)
		{
			AddMonster(Monster.dwMonsterId, Monster.vPos, Monster.bAggro);
		}
	}
}

void CMonsterClashInstance::AddMonster(const DWORD dwMonsterId, const D3DXVECTOR3 vPos, const BOOL bAggro)const
{
	CWorld* pWorld = g_WorldMng.GetWorld(GetWorldId());
	if (!pWorld || !pWorld->m_linkMap.GetLinkMap(static_cast<int>(GetLayer())))
		return;

	CObj* pObj = CreateObj(D3DDEVICE, OT_MOVER, dwMonsterId);
	if (NULL == pObj)
		return;

	pObj->SetPos(vPos);
	pObj->InitMotion(MTI_STAND);
	pObj->UpdateLocalMatrix();

	((CMover*)pObj)->m_bActiveAttack = bAggro;
	pWorld->ADDOBJ(pObj, TRUE, GetLayer());
}

void CMonsterClashInstance::KillMonster(const DWORD dwMonsterId)
{
	AddKillCount();
	AddStateKillCount();
	auto State = CMonsterClashMng::GetInstance()->GetStateProp(GetState());
	if (!State)
		return;
	if (GetStateKillCount() >= State->GetStateCount())
	{
		SetNextState();
	}
}

void CMonsterClashInstance::End(const time_t Time)
{
	int nKillCount = CMonsterClashMng::GetInstance()->GetMaxStateKillCount() - GetKillCount();
	if (GetTime() == 0)
	{
		//Clash not finished
		SetTime(Time + nKillCount);
	}
}
#endif //__SYS_MONSTER_CLASH