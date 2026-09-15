#include "stdafx.h"
#include "AegonSeasonPass.h"
#include "defineObj.h"
#include "defineItem.h"

#ifdef __WORLDSERVER
#include "User.h"
extern CUserMng g_UserMng;
#include "defineText.h"
#include "DPCoreClient.h"
extern  CDPCoreClient g_DPCoreClient;
#include "DPDatabaseClient.h"
extern	CDPDatabaseClient g_dpDBClient;
extern	CGuildMng	g_GuildMng;
#include "worldmng.h"
extern	CWorldMng	g_WorldMng;
#include "DPSrvr.h"
extern CDPSrvr g_DPSrvr;

#include "InstanceDungeonParty.h"

#ifdef	__COLOSSEUM
#include "Colosseum.h"
#endif // __COLOSSEUM



#include "eveschool.h"
extern	CGuildCombat	g_GuildCombatMng;

#include "party.h"
extern	CPartyMng g_PartyMng;

#endif // __WORLDSERVER

#ifdef __AEGON_SEASON_PASS

CAegonSeasonPassMng::CAegonSeasonPassMng()
{
	activePvEPass = -1;
	activePvPPass = -1;

	m_mapSeasonPassPvE.clear();
	m_mapSeasonPassPvP.clear();

	m_bScriptLoaded = LoadScript();
	if (!m_bScriptLoaded) 
	{
		m_mapSeasonPassPvE.clear();
		m_mapSeasonPassPvP.clear();
	}

#ifdef __WORLDSERVER
	if (activePvEPass < 0 || activePvPPass < 0) 
	{
		Error("Active season pass missing. pveID: %d - pvpID: %d", activePvEPass, activePvPPass);
	}
#endif // __WORLDSERVER

}

CAegonSeasonPassMng::~CAegonSeasonPassMng()
{
}

CAegonSeasonPassMng* CAegonSeasonPassMng::GetInstance(void)
{
	static CAegonSeasonPassMng sAegonSeasonPassMng;
	return &sAegonSeasonPassMng;
}

BOOL CAegonSeasonPassMng::LoadScript()
{
	CScript s;
	__SEASONPASS_UNIT tmpSeasonPass;

	if (s.Load(m_scriptFile))
	{
		s.GetToken();
		while (s.tok != FINISHED)
		{
			if (s.Token.Compare("SeasonPass") == 0)
			{
				tmpSeasonPass.Init();
				tmpSeasonPass.passID = s.GetNumber();
				s.GetToken(); // {

				while (s.Token.Compare("}") != 0)
				{
					s.GetToken();
					if (s.Token.Compare("name") == 0)
					{
						s.GetToken();
						strcpy(tmpSeasonPass.passName, s.Token);
					}
					else if (s.Token.Compare("pvePass") == 0)
					{
						tmpSeasonPass.nPvEPass = s.GetNumber();
					}
					else if (s.Token.Compare("reward") == 0)
					{
						__SEASONPASS_REWARD tmpReward;
						tmpReward.Init();
						tmpReward.itemID = s.GetNumber();
						tmpReward.itemNum = s.GetNumber();
						tmpReward.passPointsNeeded = s.GetNumber();
						tmpReward.rewardRarity = s.GetNumber();
						tmpSeasonPass.AddRewardUnit(tmpReward);
					}
					else if (s.Token.Compare("endTime") == 0)
					{
						tmpSeasonPass.endTimeStruct.tm_year = s.GetNumber() - 1900;
						tmpSeasonPass.endTimeStruct.tm_mon = s.GetNumber() - 1;
						tmpSeasonPass.endTimeStruct.tm_mday = s.GetNumber();
						tmpSeasonPass.endTimeStruct.tm_hour = s.GetNumber();
						tmpSeasonPass.endTimeStruct.tm_min = 0;
						tmpSeasonPass.endTimeStruct.tm_sec = 0;
						mktime(&tmpSeasonPass.endTimeStruct);
#ifdef __WORLDSERVER
						tmpSeasonPass.SetEndTime();
#endif // __WORLDSERVER
					}
					else if (s.Token.Compare("passVisible") == 0)
					{
						tmpSeasonPass.bVisible = s.GetNumber() == 0 ? FALSE : TRUE;
					}
					else if (s.Token.Compare("priceDP") == 0)
					{
						tmpSeasonPass.nPriceDP = s.GetNumber();
					}
					else if (s.Token.Compare("}") == 0)
					{
						if (tmpSeasonPass.IsPvEPass()) 
						{
							if (m_mapSeasonPassPvE.find(tmpSeasonPass.passID) == m_mapSeasonPassPvE.end() && tmpSeasonPass.IsValid())
							{
								m_mapSeasonPassPvE.insert(make_pair(tmpSeasonPass.passID, tmpSeasonPass));
							}
						}
						else 
						{
							if (m_mapSeasonPassPvP.find(tmpSeasonPass.passID) == m_mapSeasonPassPvP.end() && tmpSeasonPass.IsValid())
							{
								m_mapSeasonPassPvP.insert(make_pair(tmpSeasonPass.passID, tmpSeasonPass));
							}
						}
						
					}
				}
			}
			else if (s.Token.Compare("ActivePvEPass") == 0)
			{
				activePvEPass = s.GetNumber();
			}
			else if (s.Token.Compare("ActivePvPPass") == 0)
			{
				activePvPPass = s.GetNumber();
			}

			s.GetToken();
		}
	}
	else
	{
		Error("failed to Load AegonSeasonPass.inc");
		return FALSE;
	}

	return TRUE;
}

__SEASONPASS_UNIT* CAegonSeasonPassMng::GetCurrentUnit(BOOL pve) 
{
	if (pve) 
	{
		map<DWORD, __SEASONPASS_UNIT>::iterator it = m_mapSeasonPassPvE.find(activePvEPass);
		if (it != m_mapSeasonPassPvE.end())
		{
			return &it->second;
		}


	}
	else 
	{
		map<DWORD, __SEASONPASS_UNIT>::iterator it = m_mapSeasonPassPvP.find(activePvPPass);
		if (it != m_mapSeasonPassPvP.end())
		{
			return &it->second;
		}
	}
		
	return NULL;
}
__SEASONPASS_UNIT* CAegonSeasonPassMng::GetUnit(int passID) 
{
	if (passID < 0)
		return NULL;

	map<DWORD, __SEASONPASS_UNIT>::iterator itPvE = m_mapSeasonPassPvE.find(passID);
	if (itPvE != m_mapSeasonPassPvE.end())
	{
		return &itPvE->second;
	}

	map<DWORD, __SEASONPASS_UNIT>::iterator itPvP = m_mapSeasonPassPvP.find(passID);
	if (itPvP != m_mapSeasonPassPvE.end())
	{
		return &itPvP->second;
	}

	return NULL;
}

#ifdef __WORLDSERVER
BOOL CAegonSeasonPassMng::SafeAddProgUnit(CUser* pUser, int passID)
{
	BOOL out = FALSE;
	if (IsValidObj(pUser))
	{
		if (pUser->m_SeasonpassProgress.GetProgressUnit(passID) == NULL)
		{
			__SEASONPASS_PROGRESS_UNIT tmpUnit;
			tmpUnit.Init();
			tmpUnit.passID = passID;

			pUser->m_SeasonpassProgress.AddUnit(tmpUnit);
			out = TRUE;
		}
	}
	return out;
}
void CAegonSeasonPassMng::SeasonPassOnJoin(CUser* pUser)
{
	if (IsValidObj(pUser))
	{
		BOOL pveSave = FALSE;
		BOOL pvpSave = FALSE;
		//pve
		map<DWORD, __SEASONPASS_UNIT>::iterator itPvE = m_mapSeasonPassPvE.begin();
		for (; itPvE != m_mapSeasonPassPvE.end(); itPvE++)
		{
			pveSave = SafeAddProgUnit(pUser, itPvE->second.passID);
		}
		//pvp
		map<DWORD, __SEASONPASS_UNIT>::iterator itPvP = m_mapSeasonPassPvP.begin();
		for (; itPvP != m_mapSeasonPassPvP.end(); itPvP++)
		{
			pvpSave = SafeAddProgUnit(pUser, itPvP->second.passID);
		}


		if(pUser->GetWorld() && (pveSave || pvpSave))
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
	}
}
BOOL CAegonSeasonPassMng::SafeAddPoints(BOOL pve, CUser* pUser, int nPoints) 
{
	if (!IsValidObj(pUser) || nPoints <= 0)
		return FALSE;
	__SEASONPASS_UNIT* passUnit = GetCurrentUnit(pve);
	if (!passUnit) 
	{
		Error("Failed to get Current seasonpass unit. pve: %s - playerID: %d - nPoints: %d", pve ? "TRUE" : "FALSE", pUser->m_idPlayer, nPoints);
		return FALSE;
	}		
	if (!passUnit->GetTimeRemaining() > 0)
	{
		if(pve)
			activePvEPass = -1;
		else
			activePvPPass = -1;

		return FALSE;
	}

	__SEASONPASS_PROGRESS_UNIT* progUnit = pUser->m_SeasonpassProgress.GetProgressUnit(passUnit->passID);
	if (!progUnit) 
	{
		Error("Failed to get seasonpass progress unit. pve: %s - playerID: %d - nPoints: %d", pve ? "TRUE" : "FALSE", pUser->m_idPlayer, nPoints);
		return FALSE;
	}

	int maxPoints = passUnit->rewards[passUnit->rewards.size() - 1].GetTotalPointsToFinish();

	if (progUnit->progressPoints >= maxPoints)//max points reached
		return FALSE;

	if (progUnit->progressPoints + nPoints > maxPoints)
		nPoints = maxPoints - progUnit->progressPoints;

	if (nPoints <= 0)
		return FALSE;

	progUnit->progressPoints += nPoints;
	return TRUE;
}
void CAegonSeasonPassMng::OnPvEKill(CMover* pMoverDied, CMover* pAttacker)
{
	if (IsInvalidObj(pMoverDied) || IsInvalidObj(pAttacker) || pMoverDied->IsPlayer() || !pAttacker->IsPlayer())
		return;

#ifdef __SYS_SANDBOX
	if (pAttacker->IsWorldSandBox())
		return;

	if (pMoverDied->IsWorldSandBox())
		return;
#endif

	if (!pMoverDied->GetWorld() || !pAttacker->GetWorld())
		return;

	MoverProp* pMoverDiedProp = pMoverDied->GetPropA();
	if (!pMoverDiedProp)
		return;

	int nLevel = pMoverDiedProp->dwLevel;
	int nExp = 0;
	int nRegionBonus = 0;
	BOOL bDungeonBoss = FALSE;
	BOOL bDungeonMidBoss = FALSE;

	CColosseumMatch* pColosseumMatch = CColosseumMng::GetInstance()->GetMatchByMoverID(pMoverDied->GetId());
	if (pColosseumMatch) 
	{
		nRegionBonus = 10;
	}
	else if (CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon(pMoverDied->GetWorld()->GetID()))
	{
		ID_INFO* dungeonInfo = CInstanceDungeonParty::GetInstance()->GetDungeonInfo(static_cast<DWORD>(pMoverDied->GetLayer()), pMoverDied->GetWorld()->GetID());
		
		
		int mobState = CInstanceDungeonParty::GetInstance()->GetMonsterState(pMoverDied->GetWorld()->GetID(), static_cast<DWORD>(pMoverDied->GetLayer()), pMoverDiedProp->dwID);
		switch (mobState) 
		{
		case ID_MIDBOSS:
			
			bDungeonMidBoss = TRUE;
			nRegionBonus = 5;
			break;
		case ID_BOSS:
			if (pMoverDiedProp->dwID == MI_BESIBIGFOOT01) // wilds non-master
			{
				nExp = 40;
			}
			else if (pMoverDiedProp->dwID == MI_BESIBIGFOOT02) // wilds master
			{
				nExp = 55;
			}
			else if (pMoverDiedProp->dwID == MI_DREAMQEEN01) // euphresia non-master
			{
				nExp = 75;
			}
			else if (pMoverDiedProp->dwID == MI_DREAMQEEN01_1) // euphresia master
			{
				nExp = 90;
			}
			else if (pMoverDiedProp->dwID == MI_HERNKRAKEN01) // herneos non-master
			{
				nExp = 115;
			}
			else if (pMoverDiedProp->dwID == MI_HERNKRAKEN01_1) // herneos master
			{
				nExp = 130;
			}
			else if (pMoverDiedProp->dwID == MI_SHIPHARPINEES) // sanpres non-master
			{
				nExp = 150;
			}
			else if (pMoverDiedProp->dwID == MI_SHIPHARPINEES_1) // sanpres master
			{
				nExp = 165;
			}
			else if (pMoverDiedProp->dwID == MI_VEMPAIN01) // aminus non-master
			{
				nExp = 200;
			}
			else if (pMoverDiedProp->dwID == MI_VEMPAIN01_1) // aminus master
			{
				nExp = 220;
			}
			else if (pMoverDiedProp->dwID == MI_SKELDEVIL) // catacombs
			{
				nExp = 240;
			}

			else if (pMoverDiedProp->dwID == MI_BEHEMOTH) // behemoth (ankou)
			{
				nExp = 265;
			}

			else if (pMoverDiedProp->dwID == MI_KALGASBOSS) // kalgas
			{
				nExp = 300;
			}

			bDungeonBoss = TRUE;
			break;
		default:
			nRegionBonus = 5;
			break;
		}

	}

	

	if (bDungeonBoss) // dungeonboss gives exp to all participants
	{
		nExp += nRegionBonus;

		DWORD dwDungeonId = static_cast<DWORD>(pMoverDied->GetLayer());
		CParty* pParty = g_PartyMng.GetParty(dwDungeonId);
		if (pParty)
		{
			DWORD dwWorldId = pMoverDied->GetWorld()->GetID();
			for (int i = 0; i < pParty->GetSizeofMember(); ++i)
			{
				CUser* pPartyMember = g_UserMng.GetUserByPlayerID(pParty->GetPlayerId(i));
				if (IsValidObj(pPartyMember))
				{
					CWorld* pWorld = pPartyMember->GetWorld();
					if (pWorld)
					{
						if (pWorld->GetID() == dwWorldId && pPartyMember->GetLayer() == dwDungeonId)
						{
							if (SafeAddPoints(TRUE, pPartyMember, nExp))
								pPartyMember->m_bUpdateSeasonPass = TRUE;
						}
					}
				}
			}
		}
		else 
		{
			if (SafeAddPoints(TRUE, ((CUser*)pAttacker), nExp))
				pAttacker->m_bUpdateSeasonPass = TRUE;
		}
		
	}
	else 
	{
		int nLevelRange = nLevel / 30;

		if (pMoverDiedProp->dwClass == RANK_BOSS || pMoverDiedProp->dwClass == RANK_MIDBOSS || pMoverDiedProp->dwClass == RANK_SUPER || bDungeonMidBoss) // giant //midboss //boss
		{
			switch (nLevelRange)
			{
			case 0:
				nExp = 4;
				break;
			case 1:
				nExp = 8;
				break;
			case 2:
				nExp = 17;
				break;
			case 3:
				nExp = 27;
				break;
			case 4:
				nExp = 38;
				break;
			case 5:
				nExp = 50;
				if (nLevel >= 155)
					nExp = 63;
				break;
			default:
				nExp = 1;
				break;
			}
		}
		else //normal mobs
		{
			switch (nLevelRange)
			{
			case 0:
				nExp = 1;
				break;
			case 1:
				nExp = 2;
				break;
			case 2:
				nExp = 6;
				break;
			case 3:
				nExp = 10;
				break;
			case 4:
				nExp = 20;
				break;
			case 5:
				nExp = 30;
				if (nLevel >= 155)
					nExp = 45;
				break;
			default:
				nExp = 1;
				break;
			}
		}

		nExp += nRegionBonus;
		if (SafeAddPoints(TRUE, ((CUser*)pAttacker), nExp)) //exp added
			pAttacker->m_bUpdateSeasonPass = TRUE;
	}

	

	
	
	

}
void CAegonSeasonPassMng::OnPvPKill(CMover* pMoverDied, CMover* pAttacker)
{
	if (IsInvalidObj(pMoverDied) || IsInvalidObj(pAttacker) || !pMoverDied->IsPlayer() || !pAttacker->IsPlayer() || !pMoverDied->GetWorld() || !pAttacker->GetWorld())
		return;

	int nExp = 0;
	
	
	if (strcmp(((CUser*)pMoverDied)->m_playAccount.lpAddr, ((CUser*)pAttacker)->m_playAccount.lpAddr) == 0 && strlen(((CUser*)pMoverDied)->m_playAccount.lpAddr) > 0 && strlen(((CUser*)pAttacker)->m_playAccount.lpAddr) > 0)
	{
		//no points for same ip
		return;
	}
		

	PVP_MODE mode = pMoverDied->GetPVPCase(pAttacker);

	switch (mode)
	{
	case PVP_MODE_GUILDWAR:
		if (pMoverDied->GetWorld()->GetID() == WI_WORLD_GUILDWAR && g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE) // regular guildwar
		{
			//((CUser*)pAttacker)->AddText("PVP_MODE_GUILDWAR REGULAR", COLOR_ERROR);

			nExp = 100;

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
			if (pAttacker->m_AchievementProgress.isUnfinishedType(AEGACH_GWKILLS))
			{
				CAegonAchievementMng::GetInstance()->AddAchievementProgressForType((CUser*)pAttacker, AEGACH_GWKILLS, 1);
			}
#endif // __AEGON_ACHIEVEMENT_SYSTEM

#ifdef __AEGON_PVP_RANKING

			__PVPRANKING_PROGRESS_UNIT* tmpUnitAttacker = CAegonPvPRankingMng::GetInstance()->GetStatUnit(pAttacker->m_idPlayer);
			if (tmpUnitAttacker) 
			{
				tmpUnitAttacker->m_Guildwar.m_nKills++;
				tmpUnitAttacker->CalculatePoints();
				CAegonPvPRankingMng::GetInstance()->SaveStatUnit(pAttacker->m_idPlayer);

			}

			__PVPRANKING_PROGRESS_UNIT* tmpUnitDefender = CAegonPvPRankingMng::GetInstance()->GetStatUnit(pMoverDied->m_idPlayer);
			if (tmpUnitDefender)
			{
				tmpUnitDefender->m_Guildwar.m_nDeaths++;
				tmpUnitDefender->CalculatePoints();
				CAegonPvPRankingMng::GetInstance()->SaveStatUnit(pMoverDied->m_idPlayer);
			}
#endif // __AEGON_PVP_RANKING
		}
		else if (g_GuildCombat1to1Mng.IsPossibleUser((CUser*)pMoverDied)) // 1vs1 guildwar
		{
			//((CUser*)pAttacker)->AddText("PVP_MODE_GUILDWAR 1VS1", COLOR_ERROR);
		}
		else 
		{
			//((CUser*)pAttacker)->AddText("PVP_MODE_GUILDWAR UNKNOWN", COLOR_ERROR);
		}
		break;
	case PVP_MODE_PK:
		if (pMoverDied->GetWorld()->IsArena() && pAttacker->GetWorld()->IsArena()) 
		{
			//((CUser*)pAttacker)->AddText("PVP_MODE_PK ARENA", COLOR_ERROR);
#ifdef __AEGON_PVP_RANKING

			__PVPRANKING_PROGRESS_UNIT* tmpUnitAttacker = CAegonPvPRankingMng::GetInstance()->GetStatUnit(pAttacker->m_idPlayer);
			if (tmpUnitAttacker)
			{
				tmpUnitAttacker->m_Arena.m_nKills++;
				tmpUnitAttacker->CalculatePoints();
				CAegonPvPRankingMng::GetInstance()->SaveStatUnit(pAttacker->m_idPlayer);

			}

			__PVPRANKING_PROGRESS_UNIT* tmpUnitDefender = CAegonPvPRankingMng::GetInstance()->GetStatUnit(pMoverDied->m_idPlayer);
			if (tmpUnitDefender)
			{
				tmpUnitDefender->m_Arena.m_nDeaths++;
				tmpUnitDefender->CalculatePoints();
				CAegonPvPRankingMng::GetInstance()->SaveStatUnit(pMoverDied->m_idPlayer);
			}
#endif // __AEGON_PVP_RANKING
		}
		else 
		{
			//((CUser*)pAttacker)->AddText("PVP_MODE_PK", COLOR_ERROR);
		}
		
		break;
	case PVP_MODE_DUEL:
	{
		//((CUser*)pAttacker)->AddText("PVP_MODE_DUEL", COLOR_ERROR);

#ifdef __AEGON_PVP_RANKING

		__PVPRANKING_PROGRESS_UNIT* tmpUnitAttacker = CAegonPvPRankingMng::GetInstance()->GetStatUnit(pAttacker->m_idPlayer);
		if (tmpUnitAttacker)
		{
			tmpUnitAttacker->m_Duel.m_nKills++;
			tmpUnitAttacker->m_Duel.m_nParticipations++;
			tmpUnitAttacker->m_Duel.m_nWins++;
			tmpUnitAttacker->CalculatePoints();
			CAegonPvPRankingMng::GetInstance()->SaveStatUnit(pAttacker->m_idPlayer);

		}

		__PVPRANKING_PROGRESS_UNIT* tmpUnitDefender = CAegonPvPRankingMng::GetInstance()->GetStatUnit(pMoverDied->m_idPlayer);
		if (tmpUnitDefender)
		{
			tmpUnitDefender->m_Duel.m_nDeaths++;
			tmpUnitDefender->m_Duel.m_nParticipations++;
			tmpUnitDefender->CalculatePoints();
			CAegonPvPRankingMng::GetInstance()->SaveStatUnit(pMoverDied->m_idPlayer);
		}
#endif // __AEGON_PVP_RANKING
		break;
	}

#ifdef __AEGON_TOURNAMENT
	case PVP_MODE_TOURNAMENT:
		if (0) //TODO: check if player is in tournament
		{
			//((CUser*)pAttacker)->AddText("PVP_MODE_TOURNAMENT", COLOR_ERROR);

			nExp = 100;

#ifdef __AEGON_PVP_RANKING

			__PVPRANKING_PROGRESS_UNIT* tmpUnitAttacker = CAegonPvPRankingMng::GetInstance()->GetStatUnit(pAttacker->m_idPlayer);
			if (tmpUnitAttacker)
			{
				tmpUnitAttacker->m_Tournament.m_nKills++;
				tmpUnitAttacker->CalculatePoints();
				CAegonPvPRankingMng::GetInstance()->SaveStatUnit(pAttacker->m_idPlayer);

			}

			__PVPRANKING_PROGRESS_UNIT* tmpUnitDefender = CAegonPvPRankingMng::GetInstance()->GetStatUnit(pMoverDied->m_idPlayer);
			if (tmpUnitDefender)
			{
				tmpUnitDefender->m_Tournament.m_nDeaths++;
				tmpUnitDefender->CalculatePoints();
				CAegonPvPRankingMng::GetInstance()->SaveStatUnit(pMoverDied->m_idPlayer);
			}
#endif // __AEGON_PVP_RANKING
		}
		break;
#endif // __AEGON_TOURNAMENT
	default:
		return;
	}


	if (nExp > 0 && SafeAddPoints(FALSE, ((CUser*)pAttacker), nExp)) //exp added
		pAttacker->m_bUpdateSeasonPass = TRUE;
}
#endif // __WORLDSERVER
#endif // __AEGON_SEASON_PASS