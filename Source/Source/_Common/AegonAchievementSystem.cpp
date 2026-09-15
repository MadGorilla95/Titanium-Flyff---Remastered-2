#include "stdafx.h"
#include "AegonAchievementSystem.h"
#include "defineObj.h"
#include "defineItem.h"
#include "defineText.h"

#if __VER >= 14 // __NEW_CONTINENT
#include "Continent.h"
#endif // __NEW_CONTINENT

#ifdef __WORLDSERVER
#include "User.h"
#include "DPCoreClient.h"
extern  CDPCoreClient g_DPCoreClient;
#include "DPDatabaseClient.h"
extern	CDPDatabaseClient g_dpDBClient;
extern	CGuildMng	g_GuildMng;
#include "worldmng.h"
extern	CWorldMng	g_WorldMng;
#include "DPSrvr.h"
extern CDPSrvr g_DPSrvr;
#include "Party.h"
extern	CPartyMng			g_PartyMng;
#if __VER >= 14 // __INSTANCE_DUNGEON
#include "InstanceDungeonParty.h"
#endif //  __INSTANCE_DUNGEON
#endif // __WORLDSERVER

#ifdef __CLIENT
#include "ResData.h"
#include "WndManager.h"

#include "DPClient.h"
extern CDPClient g_DPlay;
#endif // __CLIENT

#ifdef __AEGON_ACHIEVEMENT_SYSTEM

CAegonAchievementMng::CAegonAchievementMng()
{
	m_bScriptLoaded = LoadScript();
	if (!m_bScriptLoaded)
		m_mapAchievements.clear();

#ifdef __CLIENT
	m_AchievementNoticeTexture = NULL;
	lastProcessSmall = 0;
	lastProcessMed = 0;
#endif // __CLIENT
}

CAegonAchievementMng::~CAegonAchievementMng()
{
}

CAegonAchievementMng* CAegonAchievementMng::GetInstance(void)
{
	static CAegonAchievementMng sAegonAchievementMng;
	return &sAegonAchievementMng;
}

BOOL CAegonAchievementMng::LoadScript()
{
	CScript s;
	__ACHIEVEMENT_UNIT tmpAchievement;

	if (s.Load(m_scriptFile))
	{
		s.GetToken();
		while (s.tok != FINISHED)
		{

			if (s.Token.Compare("Achievement") == 0)
			{
				tmpAchievement.Init();
				tmpAchievement.achievementID = s.GetNumber();
				s.GetToken(); // {

				while (s.Token.Compare("}") != 0)
				{
					s.GetToken();
					if (s.Token.Compare("name") == 0)
					{
						s.GetToken();
						strcpy(tmpAchievement.achievementName, s.Token);
					}
					else if (s.Token.Compare("category") == 0)
					{
						s.GetToken();
						strcpy(tmpAchievement.categoryName, s.Token);
					}
					else if (s.Token.Compare("subcategory") == 0)
					{
						CString subcategory;
						s.GetToken();
						subcategory.Format("%s", s.token);
						tmpAchievement.AddSubcategory(subcategory);
					}
					else if (s.Token.Compare("descriptionTID") == 0)
					{
						tmpAchievement.descriptionTID = s.GetNumber();
					}
					else if (s.Token.Compare("shortDescriptionTID") == 0)
					{
						tmpAchievement.shortDescriptionTID = s.GetNumber();
					}
					else if (s.Token.Compare("imageFile") == 0)
					{
						s.GetToken();
						strcpy(tmpAchievement.imageFile, s.Token.GetString());
					}
					else if (s.Token.Compare("reward") == 0)
					{
						tmpAchievement.reward = s.GetNumber();
					}
					else if (s.Token.Compare("rewardNum") == 0)
					{
						tmpAchievement.rewardNum = s.GetNumber();
					}
					else if (s.Token.Compare("rewardTradeable") == 0)
					{
						int nNum = s.GetNumber();
						tmpAchievement.bRewardTradeable = nNum == 0 ? FALSE : TRUE;
					}
					else if (s.Token.Compare("maxProgress") == 0)
					{
						tmpAchievement.maxProgress = s.GetNumber();
					}
					else if (s.Token.Compare("achievementType") == 0)
					{
						tmpAchievement.achievementType = s.GetNumber();
					}
					else if (s.Token.Compare("requireAchievement") == 0)
					{
						int requirementID;
						requirementID = s.GetNumber();
						tmpAchievement.AddRequirement(requirementID);
					}
#if defined(__CLIENT) || defined(__WORLDSERVER)
					else if (s.Token.Compare("nTypeVar") == 0)
					{
						tmpAchievement.nTypeVar = s.GetNumber();
					}
#endif // __CLIENT || __WORLDSERVER
					else if (s.Token.Compare("szRegionName") == 0)
					{
						s.GetToken();
						s.Token.Replace("\\n", "\r\n");
						strcpy(tmpAchievement.szRegionTitle, s.Token);
					}
					else if (s.Token.Compare("nTypeExtension") == 0)
					{
						tmpAchievement.nTypeExtension = s.GetNumber();
					}
					else if (s.Token.Compare("}") == 0)
					{
						if (m_mapAchievements.find(tmpAchievement.achievementID) == m_mapAchievements.end() && tmpAchievement.IsValid())
						{
							m_mapAchievements.insert(make_pair(tmpAchievement.achievementID, tmpAchievement));
						}
					}
				}
			}
			s.GetToken();
		}
	}
	else
	{
		Error("failed to Load AegonAchievementSystem.inc");
		return FALSE;
	}

	return TRUE;
}





#if defined(__WORLDSERVER) || defined(__CLIENT)
int CAegonAchievementMng::GetAchievementStatus(int achievementID, CMover* pMover)
{
	map<int, __ACHIEVEMENT_UNIT>::iterator it = m_mapAchievements.find(achievementID);

	if (pMover && it != m_mapAchievements.end()) 
	{
		__ACHIEVEMENT_PROGRESS_UNIT* unit = pMover->GetAchievementProgress()->GetUnit(achievementID);
		if (unit) 
		{
			if (unit->IsFinished()) 
			{
				return AEGACH_STATUS_COMPLETED;
			}
			if (!pMover->GetAchievementProgress()->MeetsRequirements(achievementID)) 
			{
				return AEGACH_STATUS_REQUIREMENTS;
			}
			if (unit->nProgressCount == 0)
			{
				return AEGACH_STATUS_NOTSTARTED;
			}
			if (unit->nProgressCount > 0) 
			{
				return AEGACH_STATUS_STARTED;
			}
		}
	}
	
	return -1;
}
BOOL CAegonAchievementMng::HasRequirements(int achievementID, CUser* pUser)
{
#ifdef __WORLDSERVER
	if (pUser && pUser->IsValid()) 
	{
		map<int, __ACHIEVEMENT_UNIT>::iterator it = m_mapAchievements.find(achievementID);
		if (it != m_mapAchievements.end())
		{
			vector<int>::iterator vecIterator = it->second.requirements.begin();
			for (; vecIterator != it->second.requirements.end(); vecIterator++)
			{
				if (HasFinishedAchievement(pUser, *vecIterator))
				{
					continue;
				}
				return FALSE;
			}
			return TRUE;
		}
	}
	
	return FALSE;
#endif // __WORLDSERVER
#ifdef __CLIENT
	if (IsInvalidObj(g_pPlayer))
		return FALSE;
	map<int, __ACHIEVEMENT_UNIT>::iterator it = m_mapAchievements.find(achievementID);
	if(it != m_mapAchievements.end())
	{
		for (int i = 0; i < it->second.requirements.size(); i++)
		{
			if (g_pPlayer->m_AchievementProgress.isUnfinishedUnit(it->second.requirements[i]))
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
	
#endif // __CLIENT
}
#endif // world || client

#ifdef __WORLDSERVER
BOOL CAegonAchievementMng::HasFinishedAchievement(CUser* pUser, int achievementID)
{
	if (pUser && pUser->IsValid())
	{
		map<int, __ACHIEVEMENT_UNIT>::iterator it = m_mapAchievements.find(achievementID);
		if (it != m_mapAchievements.end())
		{
			__ACHIEVEMENT_PROGRESS_UNIT* ptrUnit = pUser->GetAchievementProgress()->GetUnit(achievementID);
			if (ptrUnit)
			{
				if (ptrUnit->nProgressCount == it->second.maxProgress)
				{
					return TRUE;
				}

			}
		}
	}
	return FALSE;
}
void CAegonAchievementMng::CheckAchievementProgress(CUser* pUser, int achievementID)
{
	if (IsValidObj(pUser)) 
	{
		map<int, __ACHIEVEMENT_UNIT>::iterator it = m_mapAchievements.find(achievementID);
		__ACHIEVEMENT_PROGRESS_UNIT* unit = pUser->m_AchievementProgress.GetUnit(achievementID);

		if (it != m_mapAchievements.end())
		{
			switch (it->second.achievementType)
			{
			case AEGACH_VISITWORLD:
			{
				if (pUser->GetWorld())
				{
					if (it->second.nTypeVar >= 0 && pUser->GetWorld()->GetID() == it->second.nTypeVar)
					{
						AddAchievementProgress(pUser, it->second.achievementID, 1);
					}
				}
				break;
			}
			case AEGACH_VISITTOWN:
			{
				DWORD usrTown = CContinent::GetInstance()->GetTown(pUser->GetPos());
				if (usrTown > CONT_NODATA)
				{
					if (it->second.nTypeVar >= 0 && usrTown == it->second.nTypeVar)
					{
						AddAchievementProgress(pUser, it->second.achievementID, 1);
					}
				}


				break;
			}
			case AEGACH_VISITCONT:
			{
				DWORD usrArea = CContinent::GetInstance()->GetAchievementArea(pUser);
				if (usrArea > CONT_NODATA)
				{
					if (it->second.nTypeVar >= 0 && usrArea == it->second.nTypeVar)
					{
						AddAchievementProgress(pUser, it->second.achievementID, 1);
					}
				}
				break;
			}
			case AEGACH_COLLECTXTIME:
			{
				if (unit && !unit->IsFinished()) 
				{
					int progress = (pUser->m_nCollectingTime / 60 / 60) - unit->nProgressCount;

					if (progress > 0) 
					{
						AddAchievementProgress(pUser, it->second.achievementID, progress);
					}
				}
				break;
			}
			case AEGACH_JOINGUILD:
			{
				if (unit && !unit->IsFinished()) 
				{
					if(pUser->GetGuild() != NULL)
						AddAchievementProgress(pUser, it->second.achievementID, 1);
				}
				break;
			}
			case AEGACH_COLLSTART:
			{
				if (unit && !unit->IsFinished())
				{
					if(pUser->IsCollecting())
						AddAchievementProgress(pUser, it->second.achievementID, 1);
				}
				break;
			}
			case AEGACH_ONLINETIME:
			{
				if (unit && !unit->IsFinished())
				{
					pUser->CalculateNewOnlineTime();

					int progress = (pUser->m_nOnlineTime / 60 / 60) - unit->nProgressCount;

					if (progress > 0)
					{
						AddAchievementProgress(pUser, it->second.achievementID, progress);
					}
				}
				break;
			}
			case AEGACH_JOINPARTY:
			{
				if (unit && !unit->IsFinished())
				{
					if (pUser->m_idparty > 0)
						AddAchievementProgress(pUser, it->second.achievementID, 1);
				}
				break;
			}
			case AEGACH_PARTYLEVEL:
			{
				if (unit && !unit->IsFinished())
				{
					if (g_PartyMng.GetParty(pUser->m_idparty)->GetLevel() >= it->second.nTypeVar)
						AddAchievementProgress(pUser, it->second.achievementID, 1);
				}
				break;
			}
			case AEGACH_PERINNUM:
			{
				if (unit && !unit->IsFinished())
				{
					if(pUser->GetPerinNum() >= it->second.nTypeVar)
						AddAchievementProgress(pUser, it->second.achievementID, 1);
				}
				break;
			}
			case AEGACH_RPNUM:
			{
				if (unit && !unit->IsFinished())
				{
					if (pUser->GetChipRPNum() >= it->second.nTypeVar)
						AddAchievementProgress(pUser, it->second.achievementID, 1);
				}
				break;
			}
			case AEGACH_PTASSIST:
			{
				if (unit && !unit->IsFinished())
				{
					if (pUser->m_bAssistMember)
						AddAchievementProgress(pUser, it->second.achievementID, 1);
				}
				break;
			}
			case AEGACH_PTLEADER:
			{
				if (unit && !unit->IsFinished())
				{
					CParty* userParty = g_PartyMng.GetParty(pUser->GetPartyId());
					if (userParty && userParty->IsLeader(pUser->m_idPlayer))
						AddAchievementProgress(pUser, it->second.achievementID, 1);
				}
				break;
			}
			case AEGACH_XACHIEVES:
			{
				if (unit && !unit->IsFinished())
				{
					int nFinished = pUser->m_AchievementProgress.GetFinishedCount();
					if (nFinished >= it->second.nTypeVar)
						AddAchievementProgress(pUser, it->second.achievementID, 1);
				}
				break;
			}
			case AEGACH_VISITREGION:
			{
				if (unit && !unit->IsFinished() && strlen(it->second.szRegionTitle) > 0)
				{
					CWorld* pWorld = pUser->GetWorld();
					if (!pWorld)
						return;

					int nSize = pWorld->m_aRegion.GetSize();
					for (int i = 0; i < nSize; i++) 
					{
						LPREGIONELEM lpRegionElem = pWorld->m_aRegion.GetAt(i);
						if (!lpRegionElem)
							continue;

						CString strAchievementRegion = it->second.szRegionTitle;
						if (strAchievementRegion.Compare(lpRegionElem->m_szTitle) == 0)
						{
							D3DXVECTOR3 vPos = pUser->GetPos();

							if (lpRegionElem->m_rect.PtInRect(CPoint((int)(vPos.x), (int)(vPos.z))))
							{
								AddAchievementProgress(pUser, it->second.achievementID, 1);
							}
							break;
						}
					}
				}
				break;
			}
			default:
				break;
			}
		}
	}
}
void CAegonAchievementMng::CheckAchievementProgressForType(CUser* pUser, int type)
{
	if (pUser && pUser->IsValid())
	{
		map<int, __ACHIEVEMENT_UNIT>::iterator it = m_mapAchievements.begin();
		for (; it != m_mapAchievements.end(); it++)
		{
			if (it->second.achievementType == type) 
			{
				CheckAchievementProgress(pUser, it->second.achievementID);
			}
		}
	}
}


BOOL CAegonAchievementMng::AddAchievementProgress(CUser* pUser, int achievementID, int value, int nTypeExtension)
{
	BOOL bPointsAdded = FALSE;
	if (pUser && pUser->IsValid())
	{
		map<int, __ACHIEVEMENT_UNIT>::iterator it = m_mapAchievements.find(achievementID);
		if (it != m_mapAchievements.end())
		{
			if(pUser->GetAchievementProgress()->GetUnit(achievementID) == NULL)
			{
				__ACHIEVEMENT_PROGRESS_UNIT tmpUnit;
				tmpUnit.Init();
				tmpUnit.SetUnitAchievement(achievementID);
				tmpUnit.SetUnitProgress(0, 0);
				pUser->GetAchievementProgress()->AddUnit(tmpUnit);
			}

			BOOL bTypeExtensionMatch = FALSE;
			if ((nTypeExtension == -1 || it->second.nTypeExtension == -1) || nTypeExtension == it->second.nTypeExtension)
				bTypeExtensionMatch = TRUE;

			if (HasRequirements(achievementID, pUser) && bTypeExtensionMatch)
			{
				if (pUser->GetAchievementProgress()->SafeAddProgress(achievementID, value, it->second.maxProgress)) 
				{
					if (HasFinishedAchievement(pUser, achievementID))
					{
						pUser->m_AchievementProgress.fillUnfinishedUnit();

						pUser->m_AchievementProgress.fillUnfinishedType();

						pUser->AddFinishedAchievement(it->second.achievementID);
						pUser->AddAchievementLoadProgress();
					}
					bPointsAdded = TRUE;
				}
			}	
		}
	}

	return bPointsAdded;
}
void CAegonAchievementMng::AddAchievementProgressForType(CUser* pUser, int type, int value, int nTypeExtension)
{
	if (pUser && pUser->IsValid())
	{
		map<int, __ACHIEVEMENT_UNIT>::iterator it = m_mapAchievements.begin();
		for (; it != m_mapAchievements.end(); it++)
		{
			if(it->second.achievementType == type)
			{
				if (AddAchievementProgress(pUser, it->second.achievementID, value, nTypeExtension))
				{

				}
			}
		}
	}
}
void CAegonAchievementMng::RewardFinished(CUser* pUser, int achievementID)
{
	char szText[512];
	
	CString strOutput;

	if (pUser && pUser->IsValid())
	{
		__ACHIEVEMENT_PROGRESS_UNIT* ptrUnit = pUser->GetAchievementProgress()->GetUnit(achievementID);

		if (ptrUnit && ptrUnit->nRewardSent == 0)
		{
			map<int, __ACHIEVEMENT_UNIT>::iterator it = m_mapAchievements.find(achievementID);
			if (it != m_mapAchievements.end() && it->second.reward > 0 && it->second.rewardNum > 0)
			{
				CString mailText;
				mailText.Format("Reward for completing the '%s' Achievement!", it->second.achievementName);
				sprintf_s<512>(szText, mailText.GetString());

				DWORD nNum = it->second.rewardNum;

				CItemElem cRewardItem;
				cRewardItem.m_dwItemId = it->second.reward;
				cRewardItem.m_nItemNum = it->second.rewardNum;
				if(!it->second.bRewardTradeable)
					cRewardItem.SetFlag(CItemElem::binds);

				while (nNum > SHRT_MAX)
				{
					cRewardItem.m_nItemNum = SHRT_MAX;
					if (pUser->GetId() == NULL)
					{
						if (pUser->CreateItem(&cRewardItem) == FALSE)
							g_dpDBClient.SendQueryPostMail(pUser->m_idPlayer, 0, cRewardItem, 0, "Achievement Reward", szText);
					}
					else
					{
						if (IsValidObj(pUser))
						{
							if (pUser->CreateItem(&cRewardItem) == FALSE)
								g_dpDBClient.SendQueryPostMail(pUser->m_idPlayer, 0, cRewardItem, 0, "Achievement Reward", szText);
						}
						else
						{
							g_dpDBClient.SendQueryPostMail(pUser->GetId(), 0, cRewardItem, 0, "Achievement Reward", szText);
						}
					}
					nNum -= SHRT_MAX;
				}

				if (IsValidObj(pUser) && nNum > 0)
				{
					if (pUser->CreateItem(&cRewardItem) == FALSE)
						g_dpDBClient.SendQueryPostMail(pUser->m_idPlayer, 0, cRewardItem, 0, "Achievement Reward", szText);
				}
				else
				{
					g_dpDBClient.SendQueryPostMail(pUser->GetId(), 0, cRewardItem, 0, "Achievement Reward", szText);
				}
				ptrUnit->nRewardSent = 1;

				ItemProp* tmpItemProp = prj.GetItemProp(it->second.reward);
	
				if (tmpItemProp)
					strOutput.Format("You received %dx %s for finishing the '%s' Achievement!", it->second.rewardNum, tmpItemProp->szName, it->second.achievementName);
				else
					strOutput.Format("You received a reward for completing an Achievement!");


				g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());

				pUser->AddText(strOutput, COLOR_SUCCESS);
			}
		}
		
	}
}
void CAegonAchievementMng::CollectAchievementReward(CUser* pUser, int achievementID)
{
	if (pUser && pUser->IsValid())
	{
		map<int, __ACHIEVEMENT_UNIT>::iterator it = m_mapAchievements.find(achievementID);
		if (it != m_mapAchievements.end())
		{
			if (HasFinishedAchievement(pUser, achievementID))
			{
				RewardFinished(pUser, achievementID);
				pUser->AddAchievementLoadProgress();
			}
		}
	}
}

void CAegonAchievementMng::SafeAddProgUnit(CUser* pUser, int achievementID)
{
	if (pUser && pUser->IsValid())
	{
		if (pUser->GetAchievementProgress()->GetUnit(achievementID) == NULL)
		{
			__ACHIEVEMENT_PROGRESS_UNIT tmpUnit;
			tmpUnit.Init();
			tmpUnit.SetUnitAchievement(achievementID);
			tmpUnit.SetUnitProgress(0, 0);
			pUser->GetAchievementProgress()->AddUnit(tmpUnit);
		}
	}
}

void CAegonAchievementMng::AddAllProgUnits(CUser* pUser)
{
	if (pUser && pUser->IsValid())
	{
		map<int, __ACHIEVEMENT_UNIT>::iterator it = m_mapAchievements.begin();
		for (; it != m_mapAchievements.end(); it++) 
		{
			SafeAddProgUnit(pUser, it->second.achievementID);
		}
	}
}

void CAegonAchievementMng::CollectOfflineVendorSellCount(CMover* pVendor)
{
	if (IsValidObj(pVendor)) 
	{
		CUser* pUser = (CUser*)prj.GetUserByID(pVendor->m_vtInfo.GetOwner());
		int nCount = pVendor->m_vtInfo.GetAchievementSellCount();
		if (nCount > 0) 
		{
			if (IsValidObj(pUser))
			{
				AddAchievementProgressForType(pUser, AEGACH_VENDSELLS, nCount);
			}
			else
			{
				map<int, __ACHIEVEMENT_UNIT>::iterator it = m_mapAchievements.begin();
				for (; it != m_mapAchievements.end(); it++)
				{
					if (it->second.achievementType == AEGACH_VENDSELLS)
					{
						g_dpDBClient.SendUpdateAchievementOffline(pVendor->m_vtInfo.GetOwner(), it->second.achievementID, nCount);
					}
				}
			}
		}
		pVendor->m_vtInfo.ResetAchievementSellCount();
	}
}
#endif // __WORLDSERVER

#ifdef __CLIENT
void CAegonAchievementMng::ProcessAchievementNotice(C2DRender* p2DRender)
{
	if (!g_pPlayer)
		return;

	if (g_pPlayer->m_AchievementNoticeID.size() > 0)
	{
		CPoint point;
		CString strTmp;
		CEditString strEditOut;

		if (g_pPlayer->m_AchievementNoticeTime == 0)
			g_pPlayer->m_AchievementNoticeTime = GetTickCount();

		if (!m_AchievementNoticeTexture)
		{
			m_AchievementNoticeTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_ACHIEVEMENT, "AchievementNotice.dds"), COLOR_ITEM_BG);
		}

		if (g_pPlayer->m_AchievementNoticeTime + AEGACH_NOTICETIME > GetTickCount())
		{
			if (m_AchievementNoticeTexture)
			{
				int timeElapsed = ((GetTickCount() - g_pPlayer->m_AchievementNoticeTime) * 100) / AEGACH_NOTICETIME; //0-100 percent of time elapsed
				DWORD alpha = 0;
				if ( timeElapsed <= 75)
					alpha = 255;
				else if (timeElapsed > 75)
					alpha = std::abs((timeElapsed - 100) * 10); // fade out

				if (timeElapsed <= 25) 
				{
					point.x = (g_WndMng.GetWndRect().right - g_WndMng.GetWndRect().left) / 2 - (m_AchievementNoticeTexture->m_size.cx / 2);
					point.y = (g_WndMng.GetWndRect().bottom - (g_WndMng.m_pWndTaskBar->GetWndRect().Height()) - m_AchievementNoticeTexture->m_size.cy) + ((std::abs((double)((double)timeElapsed - (double)25) * (double)4) / (double)100) * (double)m_AchievementNoticeTexture->m_size.cy);
				}
				else
				{
					point.x = (g_WndMng.GetWndRect().right - g_WndMng.GetWndRect().left) / 2 - (m_AchievementNoticeTexture->m_size.cx / 2);
					point.y = g_WndMng.GetWndRect().bottom - (g_WndMng.m_pWndTaskBar->GetWndRect().Height()) - m_AchievementNoticeTexture->m_size.cy;
				}

				g_WndMng.m_pWndWorld->m_rectAchievementNotice.top = point.y;
				g_WndMng.m_pWndWorld->m_rectAchievementNotice.left = point.x;
				g_WndMng.m_pWndWorld->m_rectAchievementNotice.right = point.x + m_AchievementNoticeTexture->m_size.cx;
				g_WndMng.m_pWndWorld->m_rectAchievementNotice.bottom = point.y + m_AchievementNoticeTexture->m_size.cy;

				if (alpha < 0)
					alpha = 0;
				if (alpha > 255)
					alpha = 255;

	
				g_WndMng.m_pWndWorld->m_bAchievementNoticeOpen = TRUE;

				

				CD3DFont* oldFont = p2DRender->GetFont();
				p2DRender->SetFont(CWndBase::m_Theme.m_pFontAegonAchievementNotice);

				//point.y = 0;
				p2DRender->RenderTexture(point, m_AchievementNoticeTexture, alpha);

				map<int, __ACHIEVEMENT_UNIT>::iterator it = m_mapAchievements.find(g_pPlayer->m_AchievementNoticeID[0]);
				if (it != m_mapAchievements.end())
				{
					strTmp.Format("%s", it->second.achievementName);
				}
				else
					strTmp.Format("");

				strEditOut.SetParsingString(strTmp);
				strEditOut.SetColor(D3DCOLOR_ARGB(255, 217, 179, 125));
				strEditOut.SetAlpha(alpha);
				strEditOut.SetStyle(ESSTY_BOLD);
				point.x = (g_WndMng.GetWndRect().right - g_WndMng.GetWndRect().left) / 2;
				point.y += m_AchievementNoticeTexture->m_size.cy * 0.6;
				p2DRender->TextOut_EditString(point.x - (p2DRender->m_pFont->GetTextExtent(strEditOut).cx / 2), point.y, strEditOut, 0, 0, 2);

				p2DRender->SetFont(oldFont);
			}

		}
		else
		{
			g_WndMng.m_pWndWorld->m_bAchievementNoticeOpen = FALSE;
			g_pPlayer->m_AchievementNoticeTime = 0;
			g_pPlayer->m_AchievementNoticeID.erase(g_pPlayer->m_AchievementNoticeID.begin());
		}


	}
}
void CAegonAchievementMng::OpenAchievementWnd(int achievementID)
{
	if (!g_WndMng.GetApplet(APP_AEGON_ACHIEVEMENT_SYSTEM)) 
	{
		g_WndMng.CreateApplet(APP_AEGON_ACHIEVEMENT_SYSTEM);
	}

	((CWndAchievementSystemMain*)g_WndMng.GetApplet(APP_AEGON_ACHIEVEMENT_SYSTEM))->OpenAchievement(achievementID);

}
DWORD CAegonAchievementMng::GetCategoryDescription(CString strCategory, DWORD data)
{
	DWORD dwOut = TID_ACHIEVEMENT_CATEGORY_1 + data;

	return dwOut;
}
CString CAegonAchievementMng::GetCategoryImage(CString strCategory, DWORD data)
{
	CString out = "Default.png";

	if(data >= 0)
		out.Format("Achievement_Category_%d.png", data + 1);

	return out;
}

int CAegonAchievementMng::GetFinishedCount(CString strCategory)
{
	if (IsInvalidObj(g_pPlayer))
		return 0;

	int out = 0;
	map<int, __ACHIEVEMENT_UNIT>::iterator it = m_mapAchievements.begin();
	for (; it != m_mapAchievements.end(); it++)
	{
		if (!g_pPlayer->GetAchievementProgress()->GetUnit(it->second.achievementID))
			continue;

		if (g_pPlayer->GetAchievementProgress()->GetUnit(it->second.achievementID)->nProgressCount == it->second.maxProgress && (strCategory.Compare("") == 0 || it->second.IsInCategory(strCategory)))
		{
			out++;
		}
		else if (strCategory.Compare("Favorites") == 0) 
		{
			if (!g_pPlayer->GetAchievementProgress()->isUnfinishedUnit(it->second.achievementID) && easy_find(g_pPlayer->m_AchievementFavorites, it->second.achievementID)) 
			{
				out++;
			}
		}
		else if (strCategory.Compare("Claimable") == 0) 
		{
			if (!g_pPlayer->GetAchievementProgress()->isUnfinishedUnit(it->second.achievementID) && g_pPlayer->GetAchievementProgress()->GetUnit(it->second.achievementID)->nRewardSent == 0)
			{
				out++;
			}
		}
	}
	return out;
}

void CAegonAchievementMng::ClientProcess() 
{

	if (IsInvalidObj(g_pPlayer))
		return;

	if (lastProcessSmall + AEGACH_CLIENT_PROCESS_TIME_SMALL > GetTickCount())
		return;

	ClientAchievementCheck(AEGACH_VISITTOWN, CContinent::GetInstance()->GetTown(g_pPlayer->GetPos()), AEGACH_TVAR_CMP_EQUAL);
	ClientAchievementCheck(AEGACH_VISITCONT, CContinent::GetInstance()->GetAchievementArea(g_pPlayer), AEGACH_TVAR_CMP_EQUAL);
	ClientAchievementCheck(AEGACH_VISITWORLD, g_pPlayer->GetWorld()->GetID(), AEGACH_TVAR_CMP_EQUAL);

	ClientAchievementCheck(AEGACH_PERINNUM, g_pPlayer->GetPerinNum() + 1, AEGACH_TVAR_CMP_BIGGER);
	ClientAchievementCheck(AEGACH_RPNUM, g_pPlayer->GetChipRPNum() + 1 , AEGACH_TVAR_CMP_BIGGER);

	lastProcessSmall = GetTickCount();

	
	if (lastProcessMed + AEGACH_CLIENT_PROCESS_TIME_MED > GetTickCount())
		return;


	__int64 tmpNewOnlineTime = g_pPlayer->m_nOnlineTime;
	__time64_t currentTime;
	_time64(&currentTime);
	tmpNewOnlineTime = (tmpNewOnlineTime + (currentTime - g_pPlayer->m_nLastOnline)) / 60 / 60;
	ClientAchievementCheck(AEGACH_ONLINETIME, tmpNewOnlineTime, AEGACH_PROG_CMP);

	ClientAchievementCheck(AEGACH_XACHIEVES, g_pPlayer->m_AchievementProgress.GetFinishedCount() + 1, AEGACH_TVAR_CMP_BIGGER);
	
	lastProcessMed = GetTickCount();
}

void CAegonAchievementMng::ClientAchievementCheck(int type, int cmpValue, DWORD compareType) 
{
	if (IsInvalidObj(g_pPlayer))
		return;

	if (g_pPlayer->GetWorld() && g_pPlayer->m_AchievementProgress.isUnfinishedType(type))
	{
		map<int, __ACHIEVEMENT_UNIT>::iterator it = m_mapAchievements.begin();
		for (; it != m_mapAchievements.end(); it++)
		{

			__ACHIEVEMENT_PROGRESS_UNIT* unit = g_pPlayer->GetAchievementProgress()->GetUnit(it->second.achievementID);
			if (!unit)
				continue;

			switch (compareType)
			{
			case AEGACH_TVAR_CMP_EQUAL:
			{
				if (it->second.achievementType == type && it->second.nTypeVar == cmpValue)
				{
					if (HasRequirements(it->second.achievementID) && easy_find(g_pPlayer->m_AchievementProgress.unfinishedUnit, it->second.achievementID))
					{
						g_DPlay.SendCheckAchievementProgress(1, type);
						return;
					}
				}
				break;
			}
			case AEGACH_TVAR_CMP_SMALLER:
			{
				if (it->second.achievementType == type && it->second.nTypeVar > cmpValue)
				{
					if (HasRequirements(it->second.achievementID) && easy_find(g_pPlayer->m_AchievementProgress.unfinishedUnit, it->second.achievementID))
					{
						g_DPlay.SendCheckAchievementProgress(1, type);
						return;
					}
				}
				break;
			}
			case AEGACH_TVAR_CMP_BIGGER:
			{
				if (it->second.achievementType == type && it->second.nTypeVar < cmpValue)
				{
					if (HasRequirements(it->second.achievementID) && easy_find(g_pPlayer->m_AchievementProgress.unfinishedUnit, it->second.achievementID))
					{
						g_DPlay.SendCheckAchievementProgress(1, type);
						return;
					}
				}
				break;
			}
			case AEGACH_PROG_CMP:
			{
				if (it->second.achievementType == type && unit && unit->nProgressCount < cmpValue)
				{
					if (HasRequirements(it->second.achievementID) && easy_find(g_pPlayer->m_AchievementProgress.unfinishedUnit, it->second.achievementID))
					{
						g_DPlay.SendCheckAchievementProgress(1, type);
						return;
					}
				}
				break;
			}
			}

		}
	}	
}
int CAegonAchievementMng::GetCategoryAchievementCount(CString strCategory)
{
	if (IsInvalidObj(g_pPlayer))
		return 0;
	int out = 0;
	map<int, __ACHIEVEMENT_UNIT>::iterator it = m_mapAchievements.begin();
	for (; it != m_mapAchievements.end(); it++)
	{
		if (it->second.IsInCategory(strCategory))
		{
			out++;
		}
		else if (strCategory.Compare("Favorites") == 0)
		{
			if (easy_find(g_pPlayer->m_AchievementFavorites, it->second.achievementID))
			{
				out++;
			}
		}
		else if (strCategory.Compare("Claimable") == 0)
		{
			if (!easy_find(g_pPlayer->GetAchievementProgress()->unfinishedUnit, it->second.achievementID) && g_pPlayer->GetAchievementProgress()->GetUnit(it->second.achievementID)->nRewardSent == 0)
			{
				out++;
			}
		}
	}
	return out;
}

void CAegonAchievementMng::GetAchievementDescriptionEditString(CEditString* editString, int achievementID)
{
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	CEditString strTmp;
	if (editString && achievementID >= 0)
	{
		map<int, __ACHIEVEMENT_UNIT>::iterator it = m_mapAchievements.find(achievementID);
		map<int, __ACHIEVEMENT_UNIT>::iterator itRequirement = m_mapAchievements.end();

		if (it != m_mapAchievements.end())
		{

			strTmp.Format("%s\n", prj.GetText(it->second.descriptionTID));
			editString->SetParsingString(strTmp.GetString(), dwColorText);
			
			if (it->second.requirements.size() == 0)
			{
				//editString->AddParsingString("No Requirements");
				//editString->SetStyle(editString->GetLength() - strlen("No Requirements"), strlen("No Requirements"), ESSTY_UNDERLINE);
			}
			else
			{
				editString->AddParsingString("Requirements:", dwColorText);
				editString->SetStyle(editString->GetLength() - strlen("Requirements:"), strlen("Requirements:"), ESSTY_UNDERLINE);
				editString->AddParsingString("\n", dwColorText);

				for (int i = 0; i < it->second.requirements.size(); i++)
				{
					itRequirement = m_mapAchievements.find(it->second.requirements[i]);
					
					if (itRequirement != m_mapAchievements.end())
					{
						editString->AddParsingString("\t", dwColorText);
						editString->AddParsingString(itRequirement->second.achievementName, dwColorText);
						if (IsValidObj(g_pPlayer) && !g_pPlayer->GetAchievementProgress()->isUnfinishedUnit(itRequirement->second.achievementID))
							editString->SetStyle(editString->GetLength() - strlen(itRequirement->second.achievementName), strlen(itRequirement->second.achievementName), ESSTY_STRIKETHROUGH);
						editString->AddParsingString("\n", dwColorText);
					}
				}
			}
		}
	}
}
void CAegonAchievementMng::CheckPlayerRegionAchievement(LPREGIONELEM lpRegionElem)
{
	if (!lpRegionElem)
		return;

	if (IsInvalidObj(g_pPlayer))
		return;

	if (strlen(lpRegionElem->m_szTitle) <= 0)
		return;

	if (!g_pPlayer->m_AchievementProgress.isUnfinishedType(AEGACH_VISITREGION))
		return;

	CString strRegionName = lpRegionElem->m_szTitle;

	DWORD dwLastRegionCheck = 0;
	map<CString, DWORD>::iterator itTimeCheck = m_mapRegionLastCheck.find(strRegionName);
	if (itTimeCheck == m_mapRegionLastCheck.end())
	{
		m_mapRegionLastCheck.insert(make_pair(strRegionName, dwLastRegionCheck));
		itTimeCheck = m_mapRegionLastCheck.find(strRegionName);
	}
	else 
	{
		dwLastRegionCheck = itTimeCheck->second;
	}

	if (itTimeCheck == m_mapRegionLastCheck.end())
		return;

	if (dwLastRegionCheck + AEGACH_CLIENT_PROCESS_TIME_SMALL > GetTickCount())
		return;

	map<int, __ACHIEVEMENT_UNIT>::iterator it = m_mapAchievements.begin();
	for (; it != m_mapAchievements.end(); it++)
	{
		if (it->second.achievementType != AEGACH_VISITREGION || strlen(it->second.szRegionTitle) <= 0)
			continue;

		if (!HasRequirements(it->second.achievementID) || !easy_find(g_pPlayer->m_AchievementProgress.unfinishedUnit, it->second.achievementID))
			continue;

		CString strCurrentRegion = lpRegionElem->m_szTitle;
		if (strCurrentRegion.Compare(it->second.szRegionTitle) == 0) 
		{
			g_DPlay.SendCheckAchievementProgress(0, it->second.achievementID);
		}
	}

	itTimeCheck->second = GetTickCount();
}
#endif // __CLIENT

#endif // __AEGON_ACHIEVEMENT_SYSTEM