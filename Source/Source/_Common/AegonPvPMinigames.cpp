#include "stdafx.h"
#include "AegonPvPMinigames.h"
#include "defineObj.h"
#include "defineItem.h"

#ifdef __WORLDSERVER
#include "User.h"
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
extern	CUserMng g_UserMng;
#endif // __WORLDSERVER


#ifdef __AEGON_PVP_MINIGAMES

//Minigame Base
CAegonPvPMinigameBase::CAegonPvPMinigameBase()
{
	m_nGameID = 0;
	m_uTurn = 0;
	m_tPlayerTimeLimit = 0;
	m_dwPlayerMoveLimit = 0;
	m_dwMinPlayers = 2;
	m_dwMaxPlayers = 2;

	m_n64TotalBets = 0;

	m_bAllowJoining = FALSE;

	m_eGameState = GameState::state_preparation;

	m_nUpdateMode = 0;

	m_vecWinners.clear();
}
CAegonPvPMinigameBase::~CAegonPvPMinigameBase()
{
}

void CAegonPvPMinigameBase::WriteToBuffer(CAr& ar, u_long uIDReveiver)
{
	ar << m_uTurn;
	ar << m_dwMinPlayers;
	ar << m_dwMaxPlayers;

	ar << m_eGameState;

	ar << m_dwPlayerMoveLimit;
	ar << m_tPlayerTimeLimit;

	ar << m_n64TotalBets;

	ar << m_bAllowJoining;

	ar << m_nUpdateMode;

	ar << static_cast<unsigned long>(m_vecsPlayers.size());
	for (int i = 0; i < m_vecsPlayers.size(); i++)
	{
		m_vecsPlayers[i]->WriteToBuffer(ar, uIDReveiver);
	}

	ar << static_cast<unsigned long>(m_vecWinners.size());
	for (int i = 0; i < m_vecWinners.size(); i++) 
	{
		ar << m_vecWinners[i];
	}

}
#ifdef __CLIENT
void CAegonPvPMinigameBase::ReadUpdateFromBuffer(CAr& ar)
{
	ReadFromBuffer(ar);
}
#endif
void CAegonPvPMinigameBase::ReadFromBuffer(CAr& ar)
{
	ar >> m_uTurn;
	ar >> m_dwMinPlayers;
	ar >> m_dwMaxPlayers;

	ar >> m_eGameState;

	ar >> m_dwPlayerMoveLimit;
	ar >> m_tPlayerTimeLimit;

	ar >> m_n64TotalBets;

	ar >> m_bAllowJoining;

	ar >> m_nUpdateMode;

	u_long nSize = 0;
	ar >> nSize;
	DeletePlayers();
	for (int i = 0; i < nSize; i++)
	{
		__MINIGAME_PLAYER* pTmpPlayer = CreateNewPlayer();
		pTmpPlayer->ReadFromBuffer(ar);
		m_vecsPlayers.push_back(pTmpPlayer);
	}


	ar >> nSize;
	m_vecWinners.clear();
	for (int i = 0; i < nSize; i++)
	{
		u_long uWinnerID;
		ar >> uWinnerID;
		m_vecWinners.push_back(uWinnerID);
	}
}
BOOL CAegonPvPMinigameBase::HasPlayerWon(u_long uPlayerID, vector<u_long>* pVecWinners)
{
	return FALSE;
}
BOOL CAegonPvPMinigameBase::IsDraw()
{
	return FALSE;
}
BOOL CAegonPvPMinigameBase::IsValidMove(int nPlayerMove)
{
	return FALSE;
}
__MINIGAME_PLAYER* CAegonPvPMinigameBase::CreateNewPlayer()
{
	__MINIGAME_PLAYER* pOut = new __MINIGAME_PLAYER;
	return pOut;
}
void CAegonPvPMinigameBase::DeletePlayers()
{
	for (int i = 0; i < m_vecsPlayers.size(); i++)
	{
		SAFE_DELETE(m_vecsPlayers[i]);
	}
	m_vecsPlayers.clear();
}
BOOL CAegonPvPMinigameBase::DeleteSinglePlayer(u_long uIDPlayer)
{
	for (int i = 0; i < m_vecsPlayers.size(); i++)
	{
		if (uIDPlayer == m_vecsPlayers[i]->m_uPlayerID) 
		{
			SAFE_DELETE(m_vecsPlayers[i]);
			m_vecsPlayers.erase(m_vecsPlayers.begin() + i);
			return TRUE;
		}
	}
	return FALSE;
}

__MINIGAME_PLAYER* CAegonPvPMinigameBase::GetPlayerByID(u_long uPlayerID) 
{
	for (int i = 0; i < m_vecsPlayers.size(); i++) 
	{
		if (m_vecsPlayers[i]->m_uPlayerID == uPlayerID)
			return m_vecsPlayers[i];
	}
	return NULL;
}
#ifdef __WORLDSERVER
void CAegonPvPMinigameBase::OnGameStart()
{

}
ProcessResult CAegonPvPMinigameBase::Process()
{
	int nPlayerCount = m_vecsPlayers.size();

	switch (m_eGameState) 
	{
	case GameState::state_preparation: //betting, inviting, etc.
	{
		if (nPlayerCount == 0)
		{
			return ProcessResult::finished;
		}
		else if (nPlayerCount >= m_dwMinPlayers && m_dwMinPlayers > 0)
		{
			BOOL bAllReady = TRUE;
			for (int i = 0; i < nPlayerCount; i++)
			{
				if (!m_vecsPlayers[i]->m_bReady)
				{
					bAllReady = FALSE;
					break;
				}
			}
			if (bAllReady)
			{
				
				int nRand = rand() % nPlayerCount;
				if (nRand < 0 || nRand > nPlayerCount)
					nRand = 0;

				m_uTurn = m_vecsPlayers[nRand]->m_uPlayerID;
				m_tPlayerTimeLimit = time_null() + m_dwPlayerMoveLimit;
				
				CString strMessage;
				strMessage.Format("The game started.");
				AddSystemMessage(&strMessage);

				SetGameState(GameState::state_in_progress);
				OnGameStart();
				SendStateRefresh(AEG_MINIGAME_UPDATE_START);
			}
		}
		break;
	}
	case GameState::state_in_progress: //game in progress
	{
		if (DoPlayerCountCheck())
		{
			SendStateRefresh(AEG_MINIGAME_UPDATE_NONE);
		}
		else if (m_tPlayerTimeLimit < time_null())
		{
			CUser* pAfkUser = g_UserMng.GetUserByPlayerID(m_uTurn);
			if (IsValidObj(pAfkUser)) 
			{
				CString strTmp; 
				strTmp.Format("Time limit exceeded. You have been kicked from the minigame.");
				pAfkUser->AddText(strTmp.GetString(), COLOR_ERROR);

				CString strMessage;
				strMessage.Format("Player '%s' was kicked from the game due to inactivity.", pAfkUser->GetName());
				AddSystemMessage(&strMessage);
			}
			KickPlayer(m_uTurn);
		}
		break;
	}
	case GameState::state_reward_players: //distribute rewards
	{
		m_uTurn = 0;
		if (m_vecWinners.size() > 0)
		{
			OnWinnerEnd();
		}
		else
		{
			OnDrawEnd();
		}
		SetGameState(GameState::state_game_over);
		SendStateRefresh(AEG_MINIGAME_UPDATE_END);
		break;
	}
	case GameState::state_game_over: //game over
	{	
		if (nPlayerCount <= 1) 
		{
			CString strMessage;
			strMessage.Format("The game lobby was closed.");
			AddSystemMessage(&strMessage);
			return ProcessResult::finished;
		}
		else
			return ProcessResult::running;
	}
	}	

	return ProcessResult::running;
}
void CAegonPvPMinigameBase::SendStateRefresh(int nUpdateMode)
{
	m_nUpdateMode = nUpdateMode;
	for (int i = 0; i < m_vecsPlayers.size(); i++)
	{
		CUser* pUser = g_UserMng.GetUserByPlayerID(m_vecsPlayers[i]->m_uPlayerID);
		if (IsInvalidObj(pUser))
			continue;

		pUser->AddPvPMinigameRefresh();
	}
}
void CAegonPvPMinigameBase::SetNextTurn()
{
	for (int i = 0; i < m_vecsPlayers.size(); i++)
	{
		if (m_vecsPlayers[i]->m_uPlayerID == m_uTurn)
		{
			if (i == m_vecsPlayers.size() - 1)
				m_uTurn = m_vecsPlayers[0]->m_uPlayerID;
			else
				m_uTurn = m_vecsPlayers[i + 1]->m_uPlayerID;

			m_tPlayerTimeLimit = time_null() + m_dwPlayerMoveLimit;
			if (easy_find(m_vecWinners, m_uTurn))
			{
				if (DoPlayerCountCheck())
					return;
				else
					SetNextTurn();
			}
			return;
		}
	}	
}
BOOL CAegonPvPMinigameBase::DoPlayerMove(int nPlayerMove, BOOL* bSetNextTurn)
{
	return FALSE;
}
BOOL CAegonPvPMinigameBase::DoPlayerCountCheck()
{
	if (m_eGameState == GameState::state_in_progress)
	{
		int nPlayersRemaining = 0;

		for (int i = 0; i < m_vecsPlayers.size(); i++)
		{
			if (!easy_find(m_vecWinners, m_vecsPlayers[i]->m_uPlayerID)) 
			{
				++nPlayersRemaining;
			}
		}

		if (nPlayersRemaining < m_dwMinPlayers)
		{
			EndWithRemainingAsWinners();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CAegonPvPMinigameBase::KickPlayer(u_long uPlayerID)
{
	if (uPlayerID == m_uTurn)
		SetNextTurn();

	BOOL bSuccess = DeleteSinglePlayer(uPlayerID);

	if (bSuccess)
	{
		DoPlayerCountCheck();
	}

	return bSuccess;
}

//Commands
void CAegonPvPMinigameBase::OnPlayerCommand(CUser* pUser, __MINIGAME_COMMAND sCommand, char* szName) 
{
	if (IsInvalidObj(pUser))
		return;

	if (m_vecsPlayers.size() == 0)
		return;

	BOOL bUpdateState = FALSE;
	int nUpdateMode = AEG_MINIGAME_UPDATE_NONE;

	switch (sCommand.dwCommandID)
	{
	case AEG_MINIGAME_COMMAND_INVITE:
	{
		nUpdateMode = AEG_MINIGAME_UPDATE_INVITE;
		bUpdateState = OnPlayerInvite(pUser, sCommand.dwCommandValue, szName);
		break;
	}
	case AEG_MINIGAME_COMMAND_JOIN:
	{
		nUpdateMode = AEG_MINIGAME_UPDATE_JOIN;
		bUpdateState = OnPlayerJoin(pUser, sCommand.dwCommandValue2);
		break;
	}
	case AEG_MINIGAME_COMMAND_KICK:
	{
		nUpdateMode = AEG_MINIGAME_UPDATE_KICK;
		bUpdateState = OnPlayerKick(pUser, sCommand.dwCommandValue);
		break;
	}
	case AEG_MINIGAME_COMMAND_BET:
	{
		nUpdateMode = AEG_MINIGAME_UPDATE_BET;
		bUpdateState = OnPlayerBet(pUser, sCommand.dwCommandValue);
		break;
	}
	case AEG_MINIGAME_COMMAND_RESET_BET:
	{
		nUpdateMode = AEG_MINIGAME_UPDATE_RESET;
		bUpdateState = OnPlayerResetBet(pUser);
		break;
	}
	case AEG_MINIGAME_COMMAND_READY:
	{
		nUpdateMode = AEG_MINIGAME_UPDATE_READY;
		bUpdateState = OnPlayerReady(pUser);
		break;
	}
	case AEG_MINIGAME_COMMAND_MOVE:
	{
		nUpdateMode = AEG_MINIGAME_UPDATE_MOVE;
		bUpdateState = OnPlayerMove(pUser, sCommand.dwCommandValue);
		break;
	}
	case AEG_MINIGAME_COMMAND_QUIT:
	{
		nUpdateMode = AEG_MINIGAME_UPDATE_QUIT;
		bUpdateState = OnPlayerQuit(pUser);
		break;
	}
	case AEG_MINIGAME_COMMAND_T_JOIN:
	{
		nUpdateMode = AEG_MINIGAME_UPDATE_TOGGLE;
		bUpdateState = OnPlayerToggleJoin(pUser);
		break;
	}
	case AEG_MINIGAME_COMMAND_REFRESH:
	{
		nUpdateMode = AEG_MINIGAME_UPDATE_NONE;
		pUser->AddPvPMinigameRefresh();
		break;
	}
	default:
		return;
	}

	if (bUpdateState) 
		SendStateRefresh(nUpdateMode);

}
BOOL CAegonPvPMinigameBase::OnPlayerInvite(CUser* pUser, DWORD dwValue, char* szName)
{
	if (m_vecInvitedPlayers.size() > 20)
		return FALSE;

	CUser* pInvited = NULL;
	
	pInvited = g_UserMng.GetUserByName(szName);
	
	if (IsInvalidObj(pInvited))
		pInvited = g_UserMng.GetUserByPlayerID(dwValue);

	if (IsInvalidObj(pInvited))
		return FALSE;

	CAegonPvPMinigameBase* pGame = CAegonPvPMinigamesMng::GetInstance()->GetGameByPlayerID(pInvited->m_idPlayer);
	if (pGame)
		return FALSE;

	if (!easy_find(m_vecInvitedPlayers, pInvited->m_idPlayer))
		m_vecInvitedPlayers.push_back(pInvited->m_idPlayer);

	pInvited->AddPvPMinigameInvitation(pUser->m_idPlayer, m_nGameID, pUser->GetName());
	return TRUE;
}
BOOL CAegonPvPMinigameBase::OnPlayerJoin(CUser* pUser, DWORD dwValue)
{
	if (m_eGameState != GameState::state_preparation)
		return FALSE;

	CAegonPvPMinigameBase* pGame = CAegonPvPMinigamesMng::GetInstance()->GetGameByPlayerID(pUser->m_idPlayer);
	if (pGame)
		return FALSE;

	BOOL bInvited = FALSE;

	if (!m_bAllowJoining)
	{
		if (!easy_find(m_vecInvitedPlayers, pUser->m_idPlayer))
		{
			return FALSE;
		}
		else
			bInvited = TRUE;
	}

	if (dwValue == 0 && bInvited) //Refused invite
	{
		if (m_vecsPlayers.size() > 0) 
		{
			CUser* pLeader = g_UserMng.GetUserByPlayerID(m_vecsPlayers[0]->m_uPlayerID);
			if (IsValidObj(pLeader)) 
			{
				CString strTmp;
				strTmp.Format("%s declined the game invitation.", pUser->GetName());
				pLeader->AddText(strTmp.GetString(), COLOR_ERROR);
			}
		}

		RemoveInvite(pUser->m_idPlayer);
		return TRUE;
	}

	if (m_vecsPlayers.size() >= m_dwMaxPlayers)
		return FALSE;

	if (bInvited) 
	{
		RemoveInvite(pUser->m_idPlayer);
	}

	__MINIGAME_PLAYER* psNewPlayer = CreateNewPlayer();
	psNewPlayer->m_uPlayerID = pUser->m_idPlayer;
	sprintf(psNewPlayer->m_szName, pUser->GetName());
	psNewPlayer->m_szName[MAX_NAME - 1] = '\0';
	
	m_vecsPlayers.push_back(psNewPlayer);

	pUser->AddPvPMinigameRefresh();

	return TRUE;
}
BOOL CAegonPvPMinigameBase::OnPlayerKick(CUser* pUser, DWORD dwValue)
{
	if (m_eGameState != GameState::state_preparation)
		return FALSE;

	if (pUser->m_idPlayer != m_vecsPlayers[0]->m_uPlayerID)
		return FALSE;

	if (pUser->m_idPlayer == dwValue)
		return FALSE;

	if (m_vecsPlayers.size() <= 1)
		return FALSE;

	__MINIGAME_PLAYER* pSPlayer = GetPlayerByID(dwValue);
	if (!pSPlayer)
		return FALSE;

	CString strMessage;	
	strMessage.Format("Player '%s' was kicked from the game.", pSPlayer->m_szName);
	AddSystemMessage(&strMessage);

	if(ReturnBetMoney(dwValue))
		KickPlayer(dwValue);

	return TRUE; 
}
BOOL CAegonPvPMinigameBase::OnPlayerBet(CUser* pUser, DWORD dwValue)
{
	if (m_eGameState != GameState::state_preparation)
		return FALSE;

	int nGoldBet = dwValue;
	__int64 nCheckValue = 0;

	if (IsInvalidObj(pUser))
		return FALSE;

	if (nGoldBet <= 0)
		return FALSE;

	if (pUser->GetGold() < nGoldBet)
		return FALSE;

	for (int i = 0; i < m_vecsPlayers.size(); i++) 
	{
		if (m_vecsPlayers[i]->m_uPlayerID == pUser->m_idPlayer)
		{
			nCheckValue = m_vecsPlayers[i]->m_nMoneyBet;
			nCheckValue += nGoldBet;
			if (nCheckValue > INT_MAX)
				return FALSE;

			if (pUser->AddGold(-nGoldBet))
			{
				m_vecsPlayers[i]->m_nMoneyBet += nGoldBet;
				m_n64TotalBets += nGoldBet;
				return TRUE;
			}
		}
	}

	return FALSE;
}
BOOL CAegonPvPMinigameBase::OnPlayerResetBet(CUser* pUser)
{
	if (IsInvalidObj(pUser))
		return FALSE;

	if (m_eGameState != GameState::state_preparation)
		return FALSE;

	for (int i = 0; i < m_vecsPlayers.size(); i++)
	{
		m_vecsPlayers[i]->m_bReady = FALSE;
	}

	return ReturnBetMoney(pUser->m_idPlayer);
}
BOOL CAegonPvPMinigameBase::OnPlayerReady(CUser* pUser)
{
	if (m_eGameState != GameState::state_preparation)
		return FALSE;

	for (int i = 0; i < m_vecsPlayers.size(); i++) 
	{
		if (m_vecsPlayers[i]->m_uPlayerID == pUser->m_idPlayer)
		{
			m_vecsPlayers[i]->m_bReady = !m_vecsPlayers[i]->m_bReady;
			return TRUE;
		}
	}
	return FALSE;
}
BOOL CAegonPvPMinigameBase::OnPlayerMove(CUser* pUser, DWORD dwValue)
{
	if (m_eGameState != GameState::state_in_progress)
		return FALSE;

	if (m_uTurn != pUser->m_idPlayer)
		return FALSE;

	BOOL bSetNextTurn = TRUE;
	BOOL bSuccess = DoPlayerMove(dwValue, &bSetNextTurn);

	if (bSuccess)
	{
		if (HasPlayerWon(m_uTurn, &m_vecWinners))
		{
			if (!DoPlayerCountCheck())
				SetNextTurn();
		}
		else if (IsDraw()) 
		{
			m_vecWinners.clear();
			SetGameState(GameState::state_reward_players);
		}
		else 
		{
			if(bSetNextTurn)
				SetNextTurn();
		}

		return TRUE;
	}
	return FALSE;
}
BOOL CAegonPvPMinigameBase::OnPlayerQuit(CUser* pUser)
{
	if (IsInvalidObj(pUser))
		return FALSE;

	CString strMessage;

	BOOL bBetsChanged = FALSE;
	BOOL bKicked = FALSE;
	if(m_eGameState == GameState::state_preparation) 
	{
		bBetsChanged = ReturnBetMoney(pUser->m_idPlayer);
	}

	bKicked = KickPlayer(pUser->m_idPlayer);

	strMessage.Format("Player '%s' quit the game.", pUser->GetName());
	AddSystemMessage(&strMessage);

		
	return (bBetsChanged || bKicked);
}
BOOL CAegonPvPMinigameBase::OnPlayerToggleJoin(CUser* pUser)
{
	if (m_eGameState != GameState::state_preparation)
		return FALSE;

	if (m_vecsPlayers.size() <= 0)
		return FALSE;

	if (pUser->m_idPlayer != m_vecsPlayers[0]->m_uPlayerID)
		return FALSE;

	m_bAllowJoining = !m_bAllowJoining;
	return TRUE;
}
//~Commands
BOOL CAegonPvPMinigameBase::ReturnBetMoney(u_long uPlayerID)
{
	for (int i = 0; i < m_vecsPlayers.size(); i++)
	{
		if (m_vecsPlayers[i]->m_uPlayerID == uPlayerID)
		{
			int nGold = m_vecsPlayers[i]->m_nMoneyBet;
			if (nGold > 0)
			{
				if (RewardPlayer(uPlayerID, nGold))
				{
					m_vecsPlayers[i]->m_nMoneyBet = 0;
					m_n64TotalBets -= nGold;
				}		
			}
			return TRUE;
		}
	}
	return FALSE;
}
void CAegonPvPMinigameBase::OnPlayerChat(CUser* pUser, char* szMessage)
{
	if (IsInvalidObj(pUser))
		return;

	if (strlen(szMessage) <= 0)
		return;

	CString strMessage;
	strMessage.Format("%s: %s\n", pUser->GetName(), szMessage);

	AddPlayerMessage(&strMessage);
}
void CAegonPvPMinigameBase::AddPlayerMessage(CString* strMessage)
{
	for (int i = 0; i < m_vecsPlayers.size(); i++) 
	{
		CUser* pUser = g_UserMng.GetUserByPlayerID(m_vecsPlayers[i]->m_uPlayerID);
		if (IsInvalidObj(pUser))
			continue;

		pUser->AddPvPMinigameChat(strMessage);
	}
}
void CAegonPvPMinigameBase::AddSystemMessage(CString* strMessage)
{
	strMessage->Format("[SYSTEM] %s\n", strMessage->GetString());
	for (int i = 0; i < m_vecsPlayers.size(); i++)
	{
		CUser* pUser = g_UserMng.GetUserByPlayerID(m_vecsPlayers[i]->m_uPlayerID);
		if (IsInvalidObj(pUser))
			continue;

		pUser->AddPvPMinigameChat(strMessage, TRUE);
	}
}
void CAegonPvPMinigameBase::RemoveInvite(u_long nPlayerID)
{
	for (int i = 0; i < m_vecInvitedPlayers.size(); i++)
	{
		if (m_vecInvitedPlayers[i] == nPlayerID)
		{
			m_vecInvitedPlayers.erase(m_vecInvitedPlayers.begin() + i);
			return;
		}
	}
}
void CAegonPvPMinigameBase::PrintWinnersMessage()
{
	if (m_vecWinners.size() <= 0)
		return;

	CString strMessage;
	__MINIGAME_PLAYER* tmpPlayer = GetPlayerByID(m_vecWinners[0]);

	if (!tmpPlayer)
		return;

	strMessage.Format("Game over! Winner: %s", tmpPlayer->m_szName);

	AddSystemMessage(&strMessage);
}
void CAegonPvPMinigameBase::OnWinnerEnd() // same reward for all winners
{
	PrintWinnersMessage();

	if (m_vecWinners.size() <= 0)
		return;

	u_long uWinnerID = m_vecWinners[0];

	CUser* pWinner = g_UserMng.GetUserByPlayerID(uWinnerID);

	if (IsValidObj(pWinner))
	{
#ifdef __AEGON_ACHIEVEMENT_SYSTEM
		CAegonAchievementMng::GetInstance()->AddAchievementProgressForType(pWinner, AEGACH_MINIGWIN, 1);
#endif // __AEGON_ACHIEVEMENT_SYSTEM
	}

	if (m_n64TotalBets <= 0)
		return;

	BOOL bRewarded = RewardPlayer(uWinnerID, m_n64TotalBets);
	if (bRewarded)
	{
		if (IsValidObj(pWinner))
		{
				CString strMoney;
				strMoney.Format("%I64d", m_n64TotalBets);
				strMoney = GetNumberFormatEx(strMoney.GetString());
				CString strTmp;
				strTmp.Format("You won! You received the pot of %s penya.", strMoney.GetString());
				pWinner->AddText(strTmp.GetString(), COLOR_SUCCESS);
		}
	}
	else 
	{
		Error("%s %s %d - failed to send reward - playerID: %d - money: %I64d", __FILE__, __FUNCTION__, __LINE__, pWinner->m_idPlayer, m_n64TotalBets);
	}
}
void CAegonPvPMinigameBase::OnDrawEnd()
{
	for (int i = 0; i < m_vecsPlayers.size(); i++) 
	{
		CUser* pUser = g_UserMng.GetUserByPlayerID(m_vecsPlayers[i]->m_uPlayerID);

		if ( m_vecsPlayers[i]->m_nMoneyBet > 0)
		{
			BOOL bRewarded = RewardPlayer(m_vecsPlayers[i]->m_uPlayerID, m_vecsPlayers[i]->m_nMoneyBet);
			if (bRewarded) 
			{
				if(IsValidObj(pUser))
				{
					CString strMoney;
					strMoney.Format("%d", m_vecsPlayers[i]->m_nMoneyBet);
					strMoney = GetNumberFormatEx(strMoney.GetString());
					CString strTmp;
					strTmp.Format("Draw! You received your bet of %s penya.", strMoney.GetString());
					pUser->AddText(strTmp.GetString(), COLOR_SUCCESS);
				}
			}
			else 
			{
				Error("%s %s %d - failed to send reward - playerID: %d - money: %d", __FILE__, __FUNCTION__, __LINE__, m_vecsPlayers[i]->m_uPlayerID, m_vecsPlayers[i]->m_nMoneyBet);
			}
			m_vecsPlayers[i]->m_nMoneyBet = 0;
		}
	}

	CString strMessage;
	strMessage.Format("Game Over! It is a Draw.");
	AddSystemMessage(&strMessage);
}

BOOL CAegonPvPMinigameBase::RewardPlayer(u_long uPlayerID, __int64 nGold)
{
	if (nGold == 0)
		return TRUE;

	CUser* pUser = g_UserMng.GetUserByPlayerID(uPlayerID);

	if (IsInvalidObj(pUser))
	{
		return RewardPlayerByMail(uPlayerID, nGold);
	}


	if(uPlayerID == 0 || nGold < 0)
	{
		Error("%s %s %d uPlayerID: %d nGold: %I64d", __FILE__, __FUNCTION__, __LINE__, uPlayerID, nGold);
		return FALSE;
	}

	

	if (nGold + pUser->GetGold() > INT_MAX)
	{
		if (pUser->GetGold() > PERIN_VALUE)
			pUser->AutoConvertPerin();

		while (nGold > 0)
		{
			int nMaxAddValue = INT_MAX - pUser->GetGold();

			if (nMaxAddValue > nGold)
				nMaxAddValue = nGold;

			if (pUser->AddGold(nMaxAddValue)) 
			{
				nGold -= nMaxAddValue;
			}
			else 
			{
				return RewardPlayerByMail(uPlayerID, nGold);
			}

			if (nGold > 0)
				pUser->AutoConvertPerin();
		}
		return TRUE;
	}
	else
	{
		if (pUser->AddGold(nGold))
		{
			return TRUE;
		}
		else
			return RewardPlayerByMail(uPlayerID, nGold);
	}
}

BOOL CAegonPvPMinigameBase::RewardPlayerByMail(u_long uPlayerID, __int64 nGold)
{
	if (uPlayerID == 0 || nGold < 0)
	{
		Error("%s %s %d uPlayerID: %d nGold: %I64d", __FILE__, __FUNCTION__, __LINE__, uPlayerID, nGold);
		return FALSE;
	}

	CItemElem itemElem;
	itemElem.m_dwItemId = 0;
	itemElem.m_nItemNum = 0;
	itemElem.m_bCharged = FALSE;

	if (nGold > PERIN_VALUE)
	{
		itemElem.m_dwItemId = II_SYS_SYS_SCR_PERIN;

		while (nGold > PERIN_VALUE)
		{
			itemElem.m_nItemNum += 1;
			nGold -= PERIN_VALUE;
		}
	}

	g_dpDBClient.SendQueryPostMail(uPlayerID, 0, itemElem, nGold, "Minigame Reward", "");
}

void CAegonPvPMinigameBase::SetGameState(GameState eState)
{
	m_eGameState = eState;
}
void CAegonPvPMinigameBase::OnPlayerLogout(CUser* pUser)
{
	if (IsInvalidObj(pUser))
		return;

	BOOL bBetsChanged = FALSE;
	BOOL bKicked = FALSE;
	if (m_eGameState == GameState::state_preparation)
	{
		bBetsChanged = ReturnBetMoney(pUser->m_idPlayer);
	}

	bKicked = KickPlayer(pUser->m_idPlayer);


	CString strMessage;
	strMessage.Format("Player '%s' quit the game.", pUser->GetName());
	AddSystemMessage(&strMessage);

	if (bBetsChanged || bKicked) 
	{
		SendStateRefresh(AEG_MINIGAME_UPDATE_QUIT);
	}
	
}
void CAegonPvPMinigameBase::EndWithRemainingAsWinners()
{
	for (int i = 0; i < m_vecsPlayers.size(); i++) 
	{
		u_long uTmpID = m_vecsPlayers[i]->m_uPlayerID;
		if (!easy_find(m_vecWinners, uTmpID))
		{
			m_vecWinners.push_back(uTmpID);
		}
	}
	m_eGameState = GameState::state_reward_players;
}
#endif
//TicTacToe
CAegonPvPMinigameTicTacToe::CAegonPvPMinigameTicTacToe()
{
	m_nGameID = AEG_MINIGAME_TICTACTOE;

	for (int i = 0; i < 9; i++)
		m_nTicTacToeField[i] = 0;

	m_dwPlayerMoveLimit = 60;
}
CAegonPvPMinigameTicTacToe::~CAegonPvPMinigameTicTacToe()
{
}

void CAegonPvPMinigameTicTacToe::WriteToBuffer(CAr& ar, u_long uIDReveiver)
{
	CAegonPvPMinigameBase::WriteToBuffer(ar, uIDReveiver);

	for (int i = 0; i < 9; i++)
		ar << m_nTicTacToeField[i];
}
void CAegonPvPMinigameTicTacToe::ReadFromBuffer(CAr& ar)
{
	CAegonPvPMinigameBase::ReadFromBuffer(ar);

	for (int i = 0; i < 9; i++)
		ar >> m_nTicTacToeField[i];
}
#ifdef __CLIENT
void CAegonPvPMinigameTicTacToe::ReadUpdateFromBuffer(CAr& ar)
{
	ReadFromBuffer(ar);
}
#endif
BOOL CAegonPvPMinigameTicTacToe::HasPlayerWon(u_long uPlayerID, vector<u_long>* pVecWinners)
{
	BOOL bPlayerWon = FALSE;
	//check rows
	for (int i = 0; i < 3; i++) 
	{
		if (m_nTicTacToeField[i] == uPlayerID && m_nTicTacToeField[i + 3] == uPlayerID && m_nTicTacToeField[i + 6] == uPlayerID) 
		{
			bPlayerWon = TRUE;
			break;
		}
	}
	//check columns
	if(!bPlayerWon)
		for (int i = 0; i < 9; i+=3) 
		{
			if (m_nTicTacToeField[i] == uPlayerID && m_nTicTacToeField[i + 1] == uPlayerID && m_nTicTacToeField[i + 2] == uPlayerID)
			{
				bPlayerWon = TRUE;
				break;
			}
		}
	//diagonal
	if (!bPlayerWon)
		if (m_nTicTacToeField[4] == uPlayerID) 
		{
			if (m_nTicTacToeField[0] == uPlayerID && m_nTicTacToeField[8] == uPlayerID)
			{
				bPlayerWon = TRUE;
			}
			else if (m_nTicTacToeField[2] == uPlayerID && m_nTicTacToeField[6] == uPlayerID)
			{
				bPlayerWon = TRUE;
			}
		}

	if (pVecWinners && bPlayerWon)
		pVecWinners->push_back(uPlayerID);

	return bPlayerWon;
}
BOOL CAegonPvPMinigameTicTacToe::IsDraw()
{
	for (int i = 0; i < 9; i++) 
		if (m_nTicTacToeField[i] == 0)
			return FALSE;

	return TRUE;
}
BOOL CAegonPvPMinigameTicTacToe::IsValidMove(int nPlayerMove)
{
	if (nPlayerMove < 0 || nPlayerMove > 8)
		return FALSE;

	if (m_nTicTacToeField[nPlayerMove] != 0)
		return FALSE;

	return TRUE;
}
#ifdef __WORLDSERVER
void CAegonPvPMinigameTicTacToe::OnGameStart()
{

}
ProcessResult CAegonPvPMinigameTicTacToe::Process()
{
	ProcessResult prOut = CAegonPvPMinigameBase::Process();

	return prOut;
}
BOOL CAegonPvPMinigameTicTacToe::DoPlayerMove(int nPlayerMove, BOOL* bSetNextTurn)
{
	if (!IsValidMove(nPlayerMove))
		return FALSE;

	m_nTicTacToeField[nPlayerMove] = m_uTurn;
	return TRUE;
}

#endif
//Connect Four
CAegonPvPMinigameConnectFour::CAegonPvPMinigameConnectFour()
{
	m_nGameID = AEG_MINIGAME_CONNECTFOUR;

	m_dwPlayerMoveLimit = 60;

	for (int i = 0; i < 6; i++)
		for (int j = 0; j < 7; j++)
			m_nConnectFourField[i][j] = 0;
}
CAegonPvPMinigameConnectFour::~CAegonPvPMinigameConnectFour()
{
}

void CAegonPvPMinigameConnectFour::WriteToBuffer(CAr& ar, u_long uIDReveiver)
{
	CAegonPvPMinigameBase::WriteToBuffer(ar, uIDReveiver);

	for (int i = 0; i < 6; i++)
		for (int j = 0; j < 7; j++)
			ar << m_nConnectFourField[i][j];
}
void CAegonPvPMinigameConnectFour::ReadFromBuffer(CAr& ar)
{
	CAegonPvPMinigameBase::ReadFromBuffer(ar);

	for (int i = 0; i < 6; i++)
		for (int j = 0; j < 7; j++)
			ar >> m_nConnectFourField[i][j];
}
#ifdef __CLIENT
void CAegonPvPMinigameConnectFour::ReadUpdateFromBuffer(CAr& ar)
{
	CAegonPvPMinigameBase* tmpGame = CAegonPvPMinigamesMng::GetInstance()->CreateNewGame(m_nGameID);
	tmpGame->ReadFromBuffer(ar);

	if (g_WndMng.m_pWndAegonPvPGameMain) 
	{
		g_WndMng.m_pWndAegonPvPGameMain->m_vGameUpdateState.push_back(tmpGame);
	}
	
}
#endif
BOOL CAegonPvPMinigameConnectFour::HasPlayerWon(u_long uPlayerID, vector<u_long>* pVecWinners)
{
	BOOL bPlayerWon = FALSE;
	//check rows
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (bPlayerWon)
				break;
			if (m_nConnectFourField[i][j] == uPlayerID && m_nConnectFourField[i][j + 1] == uPlayerID && m_nConnectFourField[i][j + 2] == uPlayerID && m_nConnectFourField[i][j + 3] == uPlayerID) 
			{
				bPlayerWon = TRUE;
				break;
			}
				
		}
	}
	//check columns
	for (int j = 0; j < 7; j++)
	{
		if (bPlayerWon)
			break;
		for (int i = 0; i < 3; i++)
		{
			if (m_nConnectFourField[i][j] == uPlayerID && m_nConnectFourField[i + 1][j] == uPlayerID && m_nConnectFourField[i + 2][j] == uPlayerID && m_nConnectFourField[i + 3][j] == uPlayerID) 
			{
				bPlayerWon = TRUE;
				break;
			}
		}
	}
	//diagonal
	for (int i = 0; i < 3; i++)
	{
		if (bPlayerWon)
			break;

		for (int j = 0; j < 4; j++)
		{
			//check diagonal left -> right up
			if (m_nConnectFourField[i + 3][j] == uPlayerID && m_nConnectFourField[i + 2][j + 1] == uPlayerID && m_nConnectFourField[i + 1][j + 2] == uPlayerID && m_nConnectFourField[i][j + 3] == uPlayerID)
			{
				bPlayerWon = TRUE;
				break;
			}
			//check diagonal left -> right down
			if (m_nConnectFourField[i][j] == uPlayerID && m_nConnectFourField[i + 1][j + 1] == uPlayerID && m_nConnectFourField[i + 2][j + 2] == uPlayerID && m_nConnectFourField[i + 3][j + 3] == uPlayerID) 
			{
				bPlayerWon = TRUE;
				break;
			}
		}
	}

	if (pVecWinners && bPlayerWon) 
	{
		pVecWinners->push_back(uPlayerID);
	}

	return bPlayerWon;
}
BOOL CAegonPvPMinigameConnectFour::IsDraw()
{
	for (int i = 0; i < 6; i++) 
		for (int j = 0; j < 7; j++) 
			if (m_nConnectFourField[i][j] == 0) 
				return FALSE;
		
	return TRUE;
}
BOOL CAegonPvPMinigameConnectFour::IsValidMove(int nPlayerMove)
{
	if (nPlayerMove < 0 || nPlayerMove > 6)
		return FALSE;

	if (m_nConnectFourField[0][nPlayerMove] != 0)
		return FALSE;

	return TRUE;
}
#ifdef __WORLDSERVER
void CAegonPvPMinigameConnectFour::OnGameStart()
{

}
ProcessResult CAegonPvPMinigameConnectFour::Process()
{
	ProcessResult prOut = CAegonPvPMinigameBase::Process();

	return prOut;
}
BOOL CAegonPvPMinigameConnectFour::DoPlayerMove(int nPlayerMove, BOOL* bSetNextTurn)
{
	if (!IsValidMove(nPlayerMove))
		return FALSE;

	int row = 0;
	for(int i = 5; i >= 0; i--) 
	{
		if (m_nConnectFourField[i][nPlayerMove] == 0) 
		{
			row = i;
			break;
		}
	}

	m_nConnectFourField[row][nPlayerMove] = m_uTurn;

	return TRUE;
}
#endif
//fluno
CAegonPvPMinigameFluno::CAegonPvPMinigameFluno()
{
	m_nGameID = AEG_MINIGAME_FLUNO;

	m_dwPlayerMoveLimit = 60;

	m_dwMaxPlayers = 4;

	m_vCardTakeStack.clear();
	m_vCardPutStack.clear();

	m_bClockwise = TRUE;
	m_nDrawCards = 0;
	m_bSelectColor = FALSE;
	m_bPassTurn = FALSE;

#ifdef __WORLDSERVER
	m_bSkip = FALSE;
#endif
}
CAegonPvPMinigameFluno::~CAegonPvPMinigameFluno()
{
}

void CAegonPvPMinigameFluno::WriteToBuffer(CAr& ar, u_long uIDReveiver)
{
	CAegonPvPMinigameBase::WriteToBuffer(ar, uIDReveiver);

	ar << static_cast<unsigned int>(m_vCardTakeStack.size());
	for (int i = 0; i < m_vCardTakeStack.size(); i++) 
	{
		ar << m_vCardTakeStack[i];
	}

	ar << static_cast<unsigned int>(m_vCardPutStack.size());
	for (int i = 0; i < m_vCardPutStack.size(); i++)
	{
		ar << m_vCardPutStack[i];
	}

	ar << m_bClockwise;
	ar << m_nDrawCards;
	ar << m_bSelectColor;
	ar << m_bPassTurn;
}
void CAegonPvPMinigameFluno::ReadFromBuffer(CAr& ar)
{
	CAegonPvPMinigameBase::ReadFromBuffer(ar);

	m_vCardTakeStack.clear();
	m_vCardPutStack.clear();

	unsigned int nSize = 0;
	ar >> nSize;
	for (int i = 0; i < nSize; i++)
	{
		__FLUNO_CARD sTmpCard;
		ar >> sTmpCard;
		m_vCardTakeStack.push_back(sTmpCard);
	}

	ar >> nSize;
	for (int i = 0; i < nSize; i++)
	{
		__FLUNO_CARD sTmpCard;
		ar >> sTmpCard;
		m_vCardPutStack.push_back(sTmpCard);
	}

	ar >> m_bClockwise;
	ar >> m_nDrawCards;
	ar >> m_bSelectColor;
	ar >> m_bPassTurn;
}
#ifdef __CLIENT
void CAegonPvPMinigameFluno::ReadUpdateFromBuffer(CAr& ar)
{
	CAegonPvPMinigameBase* tmpGame = CAegonPvPMinigamesMng::GetInstance()->CreateNewGame(m_nGameID);
	tmpGame->ReadFromBuffer(ar);

	if (g_WndMng.m_pWndAegonPvPGameMain)
	{
		g_WndMng.m_pWndAegonPvPGameMain->m_vGameUpdateState.push_back(tmpGame);
	}
}
#endif
BOOL CAegonPvPMinigameFluno::HasPlayerWon(u_long uPlayerID, vector<u_long>* pVecWinners)
{
	BOOL bPlayerWon = FALSE;

	__MINIGAME_PLAYER_FLUNO* pPlayer = (__MINIGAME_PLAYER_FLUNO*)GetPlayerByID(uPlayerID);
	if (!pPlayer)
		return FALSE;

	if (pPlayer->m_vCards.size() == 0)
		bPlayerWon = TRUE;

	if (pVecWinners && bPlayerWon)
	{
		pVecWinners->push_back(uPlayerID);
	}

	return bPlayerWon;
}
BOOL CAegonPvPMinigameFluno::IsDraw()
{
	return FALSE; //no draw possible
}
BOOL CAegonPvPMinigameFluno::IsValidMove(int nPlayerMove)
{
	__MINIGAME_PLAYER_FLUNO* pPlayer = (__MINIGAME_PLAYER_FLUNO*)GetPlayerByID(m_uTurn);
	if (!pPlayer)
		return FALSE;

	if (m_bSelectColor) 
	{
		if (nPlayerMove >= FLUNO_MOVE_CHOOSE_COLOR_GREEN && nPlayerMove <= FLUNO_MOVE_CHOOSE_COLOR_RED)
		{
			return TRUE;
		}
		return FALSE;
	}
	else if (m_bPassTurn) 
	{
		if (nPlayerMove == FLUNO_MOVE_PASS_MOVE)
			return TRUE;
		else
			return IsValidCardMove(pPlayer, nPlayerMove);
	}
	else if (nPlayerMove == FLUNO_MOVE_TAKE_CARD)
	{
		return TRUE;
	}
	else if (nPlayerMove == FLUNO_MOVE_CALL_UNO) 
	{
		if (pPlayer->m_bCalledUno)
			return FALSE;

		return TRUE;
	}
	else if (nPlayerMove >= 0 && nPlayerMove < pPlayer->m_vCards.size()) //put card
	{
		return IsValidCardMove(pPlayer, nPlayerMove);
	}
	return FALSE;
}
__MINIGAME_PLAYER* CAegonPvPMinigameFluno::CreateNewPlayer()
{
	__MINIGAME_PLAYER_FLUNO* pOut = new __MINIGAME_PLAYER_FLUNO;
	return pOut;
}
BOOL CAegonPvPMinigameFluno::IsValidCardMove(__MINIGAME_PLAYER_FLUNO* pPlayer, int nPlayerMove)
{
	if (!pPlayer)
		return FALSE;

	if (nPlayerMove < 0 || nPlayerMove >= pPlayer->m_vCards.size())
		return FALSE;

	__FLUNO_CARD* pSelectedCard = &pPlayer->m_vCards[nPlayerMove];

	int nIndexLastCard = m_vCardPutStack.size() - 1;
	if (nIndexLastCard < 0)
		return FALSE;

	__FLUNO_CARD* pLastPlayedCard = &m_vCardPutStack[nIndexLastCard];

	if (!pSelectedCard || !pLastPlayedCard)
		return FALSE;


	return pSelectedCard->CanPlayCard(pLastPlayedCard);
}

#ifdef __WORLDSERVER
void CAegonPvPMinigameFluno::OnGameStart()
{
	m_vCardTakeStack.clear();

	//fill card deck
	for (int i = 0; i < 4; i++)
	{
		//Regular Cards
		{
			__FLUNO_CARD sAddCard(0, (CardColor)i, CardType::regular);
			m_vCardTakeStack.push_back(sAddCard);

			for (int j = 1; j < 10; j++)
			{
				__FLUNO_CARD sAddCard2(j, (CardColor)i, CardType::regular);
				m_vCardTakeStack.push_back(sAddCard2);
				m_vCardTakeStack.push_back(sAddCard2);
			}
		}

		//draw 2 cards
		{
			__FLUNO_CARD sAddCard(-1, (CardColor)i, CardType::draw_2);
			m_vCardTakeStack.push_back(sAddCard);
			m_vCardTakeStack.push_back(sAddCard);
		}

		//reverse cards
		{
			__FLUNO_CARD sAddCard(-1, (CardColor)i, CardType::reverse);
			m_vCardTakeStack.push_back(sAddCard);
			m_vCardTakeStack.push_back(sAddCard);
		}

		//skip cards
		{
			__FLUNO_CARD sAddCard(-1, (CardColor)i, CardType::skip);
			m_vCardTakeStack.push_back(sAddCard);
			m_vCardTakeStack.push_back(sAddCard);
		}
	}

	//wild cards
	{
		__FLUNO_CARD sAddCard(-1, CardColor::wild, CardType::wild_card);
		for (int j = 0; j < 4; j++)
			m_vCardTakeStack.push_back(sAddCard);
	}

	//wild draw 4 cards
	{
		__FLUNO_CARD sAddCard(-1, CardColor::wild, CardType::wild_draw_4);
		for (int j = 0; j < 4; j++)
			m_vCardTakeStack.push_back(sAddCard);
	}


	//shuffle cards
	std::random_shuffle(m_vCardTakeStack.begin(), m_vCardTakeStack.end());

	//distribute cards
	for (int i = 0; i < m_vecsPlayers.size(); i++) 
	{
		__MINIGAME_PLAYER_FLUNO* pPlayer = ((__MINIGAME_PLAYER_FLUNO*)m_vecsPlayers[i]);
		for (int j = 0; j < 7; j++) 
			TakeCard(pPlayer);
	}

	//place first card
	int nFirstIndex = 0;

	//regular card to start
	while (m_vCardTakeStack[nFirstIndex].m_eCardType != CardType::regular)
		nFirstIndex++;

	if (nFirstIndex < 0 || nFirstIndex >= m_vCardTakeStack.size())
		nFirstIndex = 0;

	if (m_vCardTakeStack.size() == 0) 
	{
		__FLUNO_CARD sAddCard(0, (CardColor)0, CardType::regular);
		m_vCardPutStack.push_back(sAddCard);
	}
	else 
	{
		m_vCardPutStack.push_back(m_vCardTakeStack[nFirstIndex]);
		m_vCardTakeStack.erase(m_vCardTakeStack.begin() + nFirstIndex);
	}
}
ProcessResult CAegonPvPMinigameFluno::Process()
{
	ProcessResult prOut = CAegonPvPMinigameBase::Process();

	return prOut;
}
void CAegonPvPMinigameFluno::SetNextTurn()
{
	for (int i = 0; i < m_vecsPlayers.size(); i++)
	{
		if (m_vecsPlayers[i]->m_uPlayerID == m_uTurn)
		{
			if (m_bClockwise) 
			{
				if (i == m_vecsPlayers.size() - 1)
				{
					m_uTurn = m_vecsPlayers[0]->m_uPlayerID;
				}
				else
				{
					m_uTurn = m_vecsPlayers[i + 1]->m_uPlayerID;
				}
			}
			else 
			{
				if (i == 0) 
				{
					int nIndexLast = m_vecsPlayers.size() - 1;
					m_uTurn = m_vecsPlayers[nIndexLast]->m_uPlayerID;
				}
				else 
				{
					m_uTurn = m_vecsPlayers[i - 1]->m_uPlayerID;
				}
			}
			m_tPlayerTimeLimit = time_null() + m_dwPlayerMoveLimit;
			if (easy_find(m_vecWinners, m_uTurn))
			{
				if (DoPlayerCountCheck())
					return;
				else
					SetNextTurn();
			}
			else if (m_bSkip) 
			{
				m_bSkip = FALSE;
				SetNextTurn();
			}
			return;
		}
	}	
}
BOOL CAegonPvPMinigameFluno::DoPlayerMove(int nPlayerMove, BOOL* bSetNextTurn)
{
	if (!IsValidMove(nPlayerMove))
		return FALSE;

	__MINIGAME_PLAYER_FLUNO* pPlayer = (__MINIGAME_PLAYER_FLUNO*)GetPlayerByID(m_uTurn);
	if (!pPlayer)
		return FALSE;

	if (nPlayerMove >= FLUNO_MOVE_CHOOSE_COLOR_GREEN && nPlayerMove <= FLUNO_MOVE_CHOOSE_COLOR_RED) //choose color after wildcard
	{
		CardColor tmpColor = CardColor::blue;
		switch (nPlayerMove)
		{
		case FLUNO_MOVE_CHOOSE_COLOR_BLUE:
			tmpColor = CardColor::blue;
			break;
		case FLUNO_MOVE_CHOOSE_COLOR_GREEN:
			tmpColor = CardColor::green;
			break;
		case FLUNO_MOVE_CHOOSE_COLOR_RED:
			tmpColor = CardColor::red;
			break;
		case FLUNO_MOVE_CHOOSE_COLOR_YELLOW:
			tmpColor = CardColor::yellow;
			break;
		default:
			break;
		}
		int nLastCard = m_vCardPutStack.size() - 1;
		if (nLastCard < 0)
			return FALSE;

		__FLUNO_CARD* sLastCard = &m_vCardPutStack[nLastCard];

		if (sLastCard->m_eCardType != CardType::wild_draw_4 && m_nDrawCards > 0)
			DrawPunishCards(pPlayer);

		sLastCard->m_eCardColor = tmpColor;
		m_bSelectColor = FALSE;
		return TRUE;
	}	
	else if (nPlayerMove == FLUNO_MOVE_PASS_MOVE) //pass on putting a card
	{
			m_bPassTurn = FALSE;

			if (m_nDrawCards > 0)
				DrawPunishCards(pPlayer);

			return TRUE;
	}
	else if (nPlayerMove == FLUNO_MOVE_TAKE_CARD) //take card
	{
		BOOL bTake = TakeCard(pPlayer);
		if (!bTake) 
		{
			CString strMessage;
			strMessage.Format("No cards left to draw.");

			AddSystemMessage(&strMessage);

			return FALSE;
		}
			
		*bSetNextTurn = FALSE;
		m_bPassTurn = TRUE;
		return TRUE;
	}
	else if (nPlayerMove == FLUNO_MOVE_CALL_UNO) //Call last card
	{
		pPlayer->m_bCalledUno = TRUE;
		*bSetNextTurn = FALSE;
		CString strMessage;
		strMessage.Format("%s called Fluno.", pPlayer->m_szName);
		AddSystemMessage(&strMessage);
		return TRUE;
	} 
	else if (nPlayerMove >= 0 && nPlayerMove < pPlayer->m_vCards.size()) //put card
	{
		m_bPassTurn = FALSE;
		if (pPlayer->m_vCards.size() == 1 && !pPlayer->m_bCalledUno)
		{
			BOOL bTake1 = TakeCard(pPlayer);
			BOOL bTake2 = TakeCard(pPlayer);

			CString strMessage;
			strMessage.Format("%s forgot to call Fluno, Draw 2.", pPlayer->m_szName);

			if (!bTake1 && !bTake2) 
			{
				strMessage.Format("%s forgot to call Fluno, but no cards to draw.", pPlayer->m_szName);
			}

			AddSystemMessage(&strMessage);
		}
		int nLastCard = m_vCardPutStack.size() - 1;
		__FLUNO_CARD sLastCard = m_vCardPutStack[nLastCard];
		__FLUNO_CARD sPutCard = pPlayer->m_vCards[nPlayerMove];

		pPlayer->m_vCards.erase(pPlayer->m_vCards.begin() + nPlayerMove);

		m_vCardPutStack.push_back(sPutCard);

		BOOL bDrawCards = FALSE;
		if (m_nDrawCards > 0) 
		{
			if (sLastCard.m_eCardType == CardType::wild_draw_4)
			{
				if (sPutCard.m_eCardType != CardType::wild_draw_4) 
				{
					bDrawCards = TRUE;
				}
			}
			else 
			{
				if (sPutCard.m_eCardType != CardType::wild_draw_4 && sPutCard.m_eCardType != CardType::draw_2)
				{
					bDrawCards = TRUE;
				}
			}
		}

		if (bDrawCards)
			DrawPunishCards(pPlayer);

		switch (sPutCard.m_eCardType) 
		{
		case CardType::draw_2:
		{
			m_nDrawCards += 2;
			break;
		}
		case CardType::reverse:
		{
			m_bClockwise = !m_bClockwise;
			break;
		}
		case CardType::skip:
		{
			m_bSkip = TRUE;
			break;
		}
		case CardType::wild_card:
		{
			*bSetNextTurn = FALSE;
			m_bSelectColor = TRUE;
			break;
		}
		case CardType::wild_draw_4:
		{
			*bSetNextTurn = FALSE;
			m_nDrawCards += 4;
			m_bSelectColor = TRUE;
			break;
		}
		default:
			break;
		}

		pPlayer->m_bCalledUno = FALSE;

		return TRUE;
	}
	
	return FALSE;
}
void CAegonPvPMinigameFluno::PrintWinnersMessage()
{
	int nWinnerCount = m_vecWinners.size() - 1;
	if (nWinnerCount <= 0)
		return;

	CString strMessage;
	if (nWinnerCount == 1) 
	{
		strMessage.Format("Game over! Winner:");
	}
	else 
	{
		strMessage.Format("Game over! Winners:");
	}

	for (int i = 0; i < nWinnerCount; i++)
	{
		__MINIGAME_PLAYER* tmpPlayer = GetPlayerByID(m_vecWinners[i]);
		if (!tmpPlayer)
			continue;

		CString strTmp;
		strTmp.Format(" %s", tmpPlayer->m_szName);

		if (i == nWinnerCount - 1)
			strTmp.Append(".");
		else
			strTmp.Append(",");

		strMessage.Append(strTmp);
	}
	AddSystemMessage(&strMessage);
}
void CAegonPvPMinigameFluno::OnWinnerEnd()
{
	PrintWinnersMessage();

	int nPlayers = m_vecWinners.size();

	if (nPlayers <= 0)
		return;

	DOUBLE dPercentage[4] = {0.0, 0.0, 0.0, 0.0};
	switch (nPlayers) 
	{
	case 4:
		dPercentage[0] = 0.60;
		dPercentage[1] = 0.30;
		dPercentage[2] = 0.10;
		break;
	case 3:
		dPercentage[0] = 0.70;
		dPercentage[1] = 0.30;
		break;
	case 2:
		dPercentage[0] = 1.00;
		break;
	}

	__int64 nPotRemaining = m_n64TotalBets;

	for (int i = 0; i < nPlayers; i++)
	{
		if (dPercentage[i] <= 0)
			continue;

		__int64 nReward = floor(m_n64TotalBets * dPercentage[i]);
		u_long uWinnerID = m_vecWinners[i];

		CUser* pWinner = g_UserMng.GetUserByPlayerID(uWinnerID);

		if (IsValidObj(pWinner))
		{
#ifdef __AEGON_ACHIEVEMENT_SYSTEM
			if(i == 0) //only first place gets achievement process
				CAegonAchievementMng::GetInstance()->AddAchievementProgressForType(pWinner, AEGACH_MINIGWIN, 1);
#endif // __AEGON_ACHIEVEMENT_SYSTEM
		}

		if (nReward <= 0)
			continue;

		BOOL bRewarded = RewardPlayer(uWinnerID, nReward);
		if (bRewarded)
		{
			if (IsValidObj(pWinner))
			{
				CString strMoney;
				strMoney.Format("%I64d", nReward);
				strMoney = GetNumberFormatEx(strMoney.GetString());
				CString strTmp;
				strTmp.Format("You won! You received %s penya.", strMoney.GetString());
				pWinner->AddText(strTmp.GetString(), COLOR_SUCCESS);
			}
		}
		else
		{
			Error("%s %s %d - failed to send reward - playerID: %d - money: %I64d", __FILE__, __FUNCTION__, __LINE__, pWinner->m_idPlayer, nReward);
		}
	}
}
void CAegonPvPMinigameFluno::RefillTakeStack()
{
	int nPutStackSize = m_vCardPutStack.size();
	if (nPutStackSize <= 1)
		return;
	int nIndexLastPut = nPutStackSize - 1;
	__FLUNO_CARD sLastPlayedCard = m_vCardPutStack[nIndexLastPut];
	m_vCardPutStack.erase(m_vCardPutStack.begin() + nIndexLastPut);

	nPutStackSize = m_vCardPutStack.size();
	for (int i = 0; i < nPutStackSize; i++) 
	{
		m_vCardTakeStack.push_back(m_vCardPutStack[i]);
	}
	m_vCardPutStack.clear();
	m_vCardPutStack.push_back(sLastPlayedCard);

	//shuffle cards
	std::random_shuffle(m_vCardTakeStack.begin(), m_vCardTakeStack.end());
}
BOOL CAegonPvPMinigameFluno::TakeCard(__MINIGAME_PLAYER_FLUNO* pPlayer)
{
	if (!pPlayer)
		return FALSE;

	if (m_vCardTakeStack.size() == 0)
	{
		RefillTakeStack();
		CString strMessage;
		strMessage.Format("Played cards got added back to the deck.");
		AddSystemMessage(&strMessage);
	}

	if (m_vCardTakeStack.size() > 0)
	{
		pPlayer->m_vCards.push_back(m_vCardTakeStack[0]);
		m_vCardTakeStack.erase(m_vCardTakeStack.begin());
		return TRUE;
	}
	else
		return FALSE;
}
void CAegonPvPMinigameFluno::DrawPunishCards(__MINIGAME_PLAYER_FLUNO* pPlayer)
{
	if (!pPlayer)
		return;

	int nCount = 0;
	for (int i = 0; i < m_nDrawCards; i++) 
	{
		if (!TakeCard(pPlayer))
			break;
		nCount++;
	}

	CString strMessage;
	if (m_nDrawCards == nCount) 
	{
		strMessage.Format("%s drew %d cards.", pPlayer->m_szName, nCount);
	}
	else 
	{
		strMessage.Format("%s drew %d of %d cards. No cards left.", pPlayer->m_szName, nCount, m_nDrawCards);
	}	
	AddSystemMessage(&strMessage);

	m_nDrawCards = 0;
}
void CAegonPvPMinigameFluno::OnPlayerChat(CUser* pUser, char* szMessage)
{
	CAegonPvPMinigameBase::OnPlayerChat(pUser, szMessage);

	if (pUser->m_idPlayer == m_uTurn) 
	{
		CString strFluno = "fluno";
		if (strFluno.CompareNoCase(szMessage) == 0)
		{
			BOOL bSetNextTurn = FALSE;
			BOOL bSuccess = DoPlayerMove(FLUNO_MOVE_CALL_UNO, &bSetNextTurn);
			if (bSuccess) 
			{
				SendStateRefresh(AEG_MINIGAME_UPDATE_MOVE);
			}
		}
	}
}
#endif
//Minigames Mng
CAegonPvPMinigamesMng::CAegonPvPMinigamesMng()
{
#ifdef __CLIENT
	m_pActiveGame = NULL;
#endif

#ifdef __WORLDSERVER
	m_dwLastGameProc = 0;
#endif
}
CAegonPvPMinigamesMng::~CAegonPvPMinigamesMng()
{
}
CAegonPvPMinigamesMng* CAegonPvPMinigamesMng::GetInstance(void)
{
	static CAegonPvPMinigamesMng sPvPMinigamesMng;
	return &sPvPMinigamesMng;
}

BOOL CAegonPvPMinigamesMng::IsValidMinigameID(int nGameID) 
{
	if (nGameID <= 0 || nGameID > AEG_MINIGAME_MAX_ID)
		return FALSE;

	return TRUE;
}

BOOL CAegonPvPMinigamesMng::IsPlaying(u_long uPlayerID) 
{
#ifdef __WORLDSERVER
	CAegonPvPMinigameBase* pGame = GetGameByPlayerID(uPlayerID);

	if (pGame)
		return TRUE;

	return FALSE;
#endif

#ifdef __CLIENT
	if (m_pActiveGame)
		return TRUE;

	return FALSE;
#endif
}

void CAegonPvPMinigamesMng::GetGameNameByID(CString* szName, int nGameID)
{
	switch (nGameID) 
	{
	case AEG_MINIGAME_TICTACTOE:
		szName->Format("Tic-Tac-Toe");
		break;
	case AEG_MINIGAME_CONNECTFOUR:
		szName->Format("Connect Four");
		break;
	case AEG_MINIGAME_FLUNO:
		szName->Format("Fluno");
		break;
	default:
		szName->Format("Error");
		break;
	}
}
CAegonPvPMinigameBase* CAegonPvPMinigamesMng::CreateNewGame(int nGameID)
{
	CAegonPvPMinigameBase* pNewGame = NULL;

	switch (nGameID)
	{
	case AEG_MINIGAME_TICTACTOE:
	{
		CAegonPvPMinigameTicTacToe* pTicTacToe = new CAegonPvPMinigameTicTacToe;
		pNewGame = pTicTacToe;
	}
	break;
	case AEG_MINIGAME_CONNECTFOUR:
	{
		CAegonPvPMinigameConnectFour* pConnectFour = new CAegonPvPMinigameConnectFour;
		pNewGame = pConnectFour;
	}
	break;
	case AEG_MINIGAME_FLUNO:
	{
		CAegonPvPMinigameFluno* pFluno = new CAegonPvPMinigameFluno;
		pNewGame = pFluno;
	}
	break;
	default:
		return NULL;
	}

	return pNewGame;
}

void CAegonPvPMinigamesMng::WriteMinigameList(CAr& ar, u_long uIDReveiver)
{
	unsigned int nSize = 0;
	for (int i = 0; i < m_vecActiveGames.size(); i++)
	{
		if (m_vecActiveGames[i]->m_bAllowJoining && m_vecActiveGames[i]->m_eGameState == GameState::state_preparation)
			nSize++;
	}

	ar << nSize;
	for (int i = 0; i < m_vecActiveGames.size(); i++) 
	{
		if (m_vecActiveGames[i]->m_bAllowJoining && m_vecActiveGames[i]->m_eGameState == GameState::state_preparation)
		{
			ar << m_vecActiveGames[i]->m_nGameID;
			m_vecActiveGames[i]->WriteToBuffer(ar, uIDReveiver);
		}
	}
}
#ifdef __CLIENT
void CAegonPvPMinigamesMng::DeleteMinigameList()
{
	for (int i = m_vecActiveGames.size() - 1; i >= 0; i--)
	{
		CAegonPvPMinigameBase* pGame = m_vecActiveGames[i];
		m_vecActiveGames.erase(m_vecActiveGames.begin() + i);
		SAFE_DELETE(pGame);
	}
	m_vecActiveGames.clear();
}
void CAegonPvPMinigamesMng::ReadMinigameList(CAr& ar)
{
	DeleteMinigameList();

	unsigned int nSize = 0;
	ar >> nSize;
	for (int i = 0; i < nSize; i++) 
	{
		int nGameID;
		ar >> nGameID;

		CAegonPvPMinigameBase* pGame = CAegonPvPMinigamesMng::GetInstance()->CreateNewGame(nGameID);
		pGame->ReadFromBuffer(ar);
		m_vecActiveGames.push_back(pGame);
	}
}
void CAegonPvPMinigamesMng::SetActiveGame(CAegonPvPMinigameBase* pGame)
{
	m_pActiveGame = pGame;
}
CAegonPvPMinigameBase* CAegonPvPMinigamesMng::GetActiveGame()
{
	return m_pActiveGame;
}
void CAegonPvPMinigamesMng::RemoveActiveGame() 
{
	SAFE_DELETE(m_pActiveGame);
}
#endif
#ifdef __WORLDSERVER
CAegonPvPMinigameBase* CAegonPvPMinigamesMng::GetGameByPlayerID(u_long uPlayerID)
{
	for (int i = 0; i < m_vecActiveGames.size(); i++)
	{

		if(std::find_if(m_vecActiveGames[i]->m_vecsPlayers.begin(), m_vecActiveGames[i]->m_vecsPlayers.end(), find_player_id(uPlayerID)) != m_vecActiveGames[i]->m_vecsPlayers.end())
		{
			return m_vecActiveGames[i];
		}
	}
	return NULL;
}
void CAegonPvPMinigamesMng::StartMinigame(CUser* pUser, int nGameID)
{
	if (IsInvalidObj(pUser))
		return;

	if (!IsValidMinigameID(nGameID))
		return;

	CAegonPvPMinigameBase* pGameTest = GetGameByPlayerID(pUser->m_idPlayer);
	if (pGameTest)
		return;

	CAegonPvPMinigameBase* pGame = CreateNewGame(nGameID);
	if (!pGame)
		return;

	__MINIGAME_PLAYER* tmpPlayer = pGame->CreateNewPlayer();
	tmpPlayer->m_uPlayerID = pUser->m_idPlayer;
	sprintf(tmpPlayer->m_szName, pUser->GetName());
	tmpPlayer->m_szName[MAX_NAME - 1] = '\0';
	pGame->m_vecsPlayers.push_back(tmpPlayer);

	m_vecActiveGames.push_back(pGame);

	pUser->AddPvPMinigameRefresh();
}
void CAegonPvPMinigamesMng::Process()
{
	ProcessGames();
}

void CAegonPvPMinigamesMng::ProcessGames()
{
	if (m_vecActiveGames.size() == 0)
		return;

	if (m_dwLastGameProc >= m_vecActiveGames.size())
	{
		m_dwLastGameProc = 0;
	}

	ProcessResult nResult = m_vecActiveGames[m_dwLastGameProc]->Process();

	if (nResult == ProcessResult::finished)
	{
		CAegonPvPMinigameBase* pGame = m_vecActiveGames[m_dwLastGameProc];
		m_vecActiveGames.erase(m_vecActiveGames.begin() + m_dwLastGameProc);
		SAFE_DELETE(pGame);
	}

	m_dwLastGameProc++;
}
void CAegonPvPMinigamesMng::OnPlayerCommand(CUser* pUser, __MINIGAME_COMMAND sCommand, char* szName)
{
	if (IsInvalidObj(pUser))
		return;

	if (!sCommand.IsValid())
		return;

	CAegonPvPMinigameBase* pGame = NULL;
	if (sCommand.dwCommandID == AEG_MINIGAME_COMMAND_JOIN) 
	{
		pGame = GetGameByPlayerID(sCommand.dwCommandValue);
	}
	else 
	{
		pGame = GetGameByPlayerID(pUser->m_idPlayer);
	}

	if (!pGame)
		return;

	pGame->OnPlayerCommand(pUser, sCommand, szName);
}
void CAegonPvPMinigamesMng::OnPlayerChat(CUser* pUser, char* szMessage)
{
	if (IsInvalidObj(pUser))
		return;

	CAegonPvPMinigameBase* pGame = GetGameByPlayerID(pUser->m_idPlayer);
	if (!pGame)
		return;

	pGame->OnPlayerChat(pUser, szMessage);
}
void CAegonPvPMinigamesMng::OnPlayerLogout(CUser* pUser)
{
	if (IsInvalidObj(pUser))
		return;

	CAegonPvPMinigameBase* pGame = GetGameByPlayerID(pUser->m_idPlayer);
	if (!pGame)
		return;

	pGame->OnPlayerLogout(pUser);

}

#endif // __WORLDSERVER
#endif // __AEGON_PVP_MINIGAMES