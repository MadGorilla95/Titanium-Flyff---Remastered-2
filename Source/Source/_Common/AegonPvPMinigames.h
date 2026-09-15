#pragma once
#ifndef __AEGPVPGAMES__H
#define __AEGPVPGAMES__H
#ifdef __AEGON_PVP_MINIGAMES

//game ids
#define AEG_MINIGAME_TICTACTOE 1
#define AEG_MINIGAME_CONNECTFOUR 2
#define AEG_MINIGAME_FLUNO 3

#define AEG_MINIGAME_MAX_ID	3		//Highest ID

//game commands
#define AEG_MINIGAME_COMMAND_INVITE		1
#define AEG_MINIGAME_COMMAND_JOIN		2
#define AEG_MINIGAME_COMMAND_KICK		3
#define AEG_MINIGAME_COMMAND_BET		4
#define AEG_MINIGAME_COMMAND_RESET_BET	5
#define AEG_MINIGAME_COMMAND_READY		6
#define AEG_MINIGAME_COMMAND_MOVE		7
#define AEG_MINIGAME_COMMAND_QUIT		8
#define AEG_MINIGAME_COMMAND_T_JOIN		9
#define AEG_MINIGAME_COMMAND_REFRESH	10

#define AEG_MINIGAME_COMMAND_MAX		10

//fluno extra moves
#define FLUNO_MOVE_TAKE_CARD 100
#define FLUNO_MOVE_PASS_MOVE 101
#define FLUNO_MOVE_CALL_UNO 102
#define FLUNO_MOVE_CHOOSE_COLOR_GREEN 103
#define FLUNO_MOVE_CHOOSE_COLOR_BLUE 104
#define FLUNO_MOVE_CHOOSE_COLOR_YELLOW 105
#define FLUNO_MOVE_CHOOSE_COLOR_RED 106

//update modes
#define AEG_MINIGAME_UPDATE_NONE	0
#define AEG_MINIGAME_UPDATE_INVITE	1
#define AEG_MINIGAME_UPDATE_TOGGLE	2	
#define AEG_MINIGAME_UPDATE_JOIN	3
#define AEG_MINIGAME_UPDATE_BET		4
#define AEG_MINIGAME_UPDATE_RESET	5
#define AEG_MINIGAME_UPDATE_READY	6
#define AEG_MINIGAME_UPDATE_KICK	7
#define AEG_MINIGAME_UPDATE_START	8
#define AEG_MINIGAME_UPDATE_MOVE	9
#define AEG_MINIGAME_UPDATE_QUIT	10
#define AEG_MINIGAME_UPDATE_END		11


enum class ProcessResult
{
	running,
	finished
};

enum class GameState
{
	state_preparation,
	state_in_progress,
	state_reward_players,
	state_game_over
};

enum class CardColor
{
	blue = 0,
	green = 1,
	red = 2,
	yellow = 3,
	wild = 4
};

enum class CardType
{
	regular,		//regular number/color card
	draw_2,			//draw 2
	reverse,		//reverse direction
	skip,			//skip next player
	wild_card,		//Choose any color
	wild_draw_4,	//draw 4 and choose color

};


typedef struct __FLUNO_CARD
{
	int m_nValue;
	CardColor m_eCardColor;
	CardType m_eCardType;

	__FLUNO_CARD()
	{
	}

	__FLUNO_CARD(int nValue, CardColor eCardColor, CardType eCardType)
	{
		m_nValue = nValue;
		m_eCardColor = eCardColor;
		m_eCardType = eCardType;
	}

	void SetColor(CardColor eCardColor)
	{
		m_eCardColor = eCardColor;
	}

	BOOL CanPlayCard(__FLUNO_CARD* pLastCard)
	{
		if(!pLastCard)
			return FALSE;

		if (m_eCardType != CardType::regular && m_eCardType == pLastCard->m_eCardType)
			return TRUE;

		switch (m_eCardType) 
		{
		case CardType::regular:
		{
			if (m_eCardColor == pLastCard->m_eCardColor)
				return TRUE;

			if (pLastCard->m_eCardType == CardType::regular) 
				if (m_nValue == pLastCard->m_nValue)
					return TRUE;
			break;
		}
		case CardType::draw_2:
		case CardType::reverse:
		case CardType::skip:
		{
			if (m_eCardColor == pLastCard->m_eCardColor)
				return TRUE;
			break;
		}
		case CardType::wild_card:
		case CardType::wild_draw_4:
		{
			return TRUE;
			break;
		}
		default:
			return FALSE;
		}

		return FALSE;
	}

	void GetCardFileName(CString* str) 
	{
		*str = "";

		switch (m_eCardColor) 
		{
		case CardColor::blue:
		{
			*str = "Uno_blue";
			break;
		}
		case CardColor::green:
		{
			*str = "Uno_Green";
			break;
		}
		case CardColor::red:
		{
			*str = "Uno_Red";
			break;
		}
		case CardColor::yellow:
		{
			*str = "Uno_yellow";
			break;
		}
		case CardColor::wild:
		{
			break;
		}
		default:
			break;
		}

		
		CString strTmp;
		switch (m_eCardType)
		{
		case CardType::regular:
		{
			strTmp.Format("%d.png", m_nValue);
			str->Append(strTmp);
			break;
		}
		case CardType::draw_2:
		{
			strTmp.Format("PlusZwei.png");
			str->Append(strTmp);
			break;
		}
		case CardType::reverse:
		{
			strTmp.Format("Switch.png");
			str->Append(strTmp);
			break;
		}
		case CardType::skip:
		{
			strTmp.Format("Block.png");
			str->Append(strTmp);
			break;
		}
		case CardType::wild_card:
		{
			*str = "Uno_ColorChange.png";
			break;
		}
		case CardType::wild_draw_4:
		{
			*str = "Uno_PlusVier.png";
			break;
		}
		}
	}

	inline BOOL __FLUNO_CARD::operator==(_In_ __FLUNO_CARD card) const throw()
	{
		return (m_nValue == card.m_nValue && m_eCardColor == card.m_eCardColor && m_eCardType == card.m_eCardType);
	}

	inline BOOL __FLUNO_CARD::operator!=(_In_ __FLUNO_CARD card) const throw()
	{
		return !(*this == card);
	}

}__FLUNO_CARD;

typedef struct __MINIGAME_PLAYER
{
	u_long	m_uPlayerID;
	int		m_nMoneyBet;
	BOOL	m_bReady;
	char	m_szName[MAX_NAME];

	__MINIGAME_PLAYER()
	{
		Init();
	}

	void Init() 
	{
		m_uPlayerID = 0;
		m_nMoneyBet = 0;
		m_bReady = FALSE;
		sprintf(m_szName, "error");
	}

	BOOL IsValid()
	{
		if (m_uPlayerID == 0 || m_nMoneyBet < 0)
			return FALSE;

		return TRUE;
	}

	virtual void WriteToBuffer(CAr& ar, u_long uIDReveiver) 
	{
		ar << m_uPlayerID;
		ar << m_nMoneyBet;
		ar << m_bReady;
		ar.WriteString(m_szName);
	}

	virtual void ReadFromBuffer(CAr& ar)
	{
		ar >> m_uPlayerID;
		ar >> m_nMoneyBet;
		ar >> m_bReady;
		ar.ReadString(m_szName, MAX_NAME);
	}

}__MINIGAME_PLAYER;

typedef struct __MINIGAME_PLAYER_FLUNO : __MINIGAME_PLAYER
{
	vector<__FLUNO_CARD> m_vCards;
	BOOL m_bCalledUno;

	__MINIGAME_PLAYER_FLUNO() 
	{
		__MINIGAME_PLAYER();
		m_vCards.clear();
		m_bCalledUno = FALSE;
	}

	void WriteToBuffer(CAr& ar, u_long uIDReveiver) override
	{
		__MINIGAME_PLAYER::WriteToBuffer(ar, uIDReveiver);

		__FLUNO_CARD tmpFakeCard(0, CardColor::blue, CardType::regular);

		ar << static_cast<unsigned int>(m_vCards.size());
		for (int i = 0; i < m_vCards.size(); i++) 
		{
			if (uIDReveiver != m_uPlayerID)
				ar << tmpFakeCard;
			else
				ar << m_vCards[i];
		}

		ar << m_bCalledUno;
	}

	void ReadFromBuffer(CAr& ar) override
	{
		__MINIGAME_PLAYER::ReadFromBuffer(ar);

		unsigned int nSize = 0;
		ar >> nSize;
		m_vCards.clear();
		for (int i = 0; i < nSize; i++) 
		{
			__FLUNO_CARD sTmpCard;
			ar >> sTmpCard;
			m_vCards.push_back(sTmpCard);
		}
		ar >> m_bCalledUno;
	}

}__MINIGAME_PLAYER_FLUNO;

struct find_player_id : std::unary_function<__MINIGAME_PLAYER, bool> {
	u_long id;
	find_player_id(u_long id) :id(id) { }
	bool operator()(__MINIGAME_PLAYER* const& m) const {
		return m->m_uPlayerID == id;
	}
};

typedef struct __MINIGAME_COMMAND
{
	DWORD dwCommandID;
	DWORD dwCommandValue;
	DWORD dwCommandValue2;

	__MINIGAME_COMMAND() 
	{
		Init();
	}
	void Init()
	{
		dwCommandID = 0;
		dwCommandValue = 0;
		dwCommandValue2 = 0;
	}

	BOOL IsValid() 
	{
		if (dwCommandID <= 0 || dwCommandID > AEG_MINIGAME_COMMAND_MAX)
			return FALSE;

		return TRUE;
	}
}__MINIGAME_COMMAND;

class CAegonPvPMinigameBase
{
public:
	u_long m_uTurn;
	int m_nGameID;
	DWORD m_dwMinPlayers;
	DWORD m_dwMaxPlayers;

	GameState m_eGameState;

	vector<u_long> m_vecWinners;

	vector<__MINIGAME_PLAYER*> m_vecsPlayers;

	DWORD m_dwPlayerMoveLimit;
	__int64 m_tPlayerTimeLimit;

	__int64 m_n64TotalBets;

	BOOL m_bAllowJoining; // for window with all open game lobbys
	vector<u_long> m_vecInvitedPlayers;

	int m_nUpdateMode;

	CAegonPvPMinigameBase();
	virtual ~CAegonPvPMinigameBase();

	virtual void WriteToBuffer(CAr& ar, u_long uIDReveiver);
#ifdef __CLIENT
	virtual void ReadUpdateFromBuffer(CAr& ar);
#endif
	virtual void ReadFromBuffer(CAr& ar);

	virtual BOOL HasPlayerWon(u_long uPlayerID, vector<u_long>* pVecWinners);
	virtual BOOL IsDraw();

	virtual BOOL IsValidMove(int nPlayerMove);

	
	virtual __MINIGAME_PLAYER* CreateNewPlayer();

	BOOL DeleteSinglePlayer(u_long uIDPlayer);
	void DeletePlayers();
	__MINIGAME_PLAYER* GetPlayerByID(u_long uPlayerID);

#ifdef __WORLDSERVER
	virtual void OnGameStart();
	virtual ProcessResult Process();
	void SendStateRefresh(int nUpdateMode = 0);
	virtual void SetNextTurn();

	virtual BOOL DoPlayerMove(int nPlayerMove, BOOL* bSetNextTurn);

	BOOL DoPlayerCountCheck();

	BOOL KickPlayer(u_long uPlayerID);

	void OnPlayerCommand(CUser* pUser, __MINIGAME_COMMAND sCommand, char* szName);

	BOOL OnPlayerInvite(CUser* pUser, DWORD dwValue, char* szName);
	BOOL OnPlayerJoin(CUser* pUser, DWORD dwValue);
	BOOL OnPlayerKick(CUser* pUser, DWORD dwValue);
	BOOL OnPlayerBet(CUser* pUser, DWORD dwValue);
	BOOL OnPlayerResetBet(CUser* pUser);
	BOOL OnPlayerReady(CUser* pUser);
	BOOL OnPlayerMove(CUser* pUser, DWORD dwValue);
	BOOL OnPlayerQuit(CUser* pUser);
	BOOL OnPlayerToggleJoin(CUser* pUser);

	BOOL ReturnBetMoney(u_long uPlayerID);
	virtual void OnPlayerChat(CUser* pUser, char* szMessage);
	void AddPlayerMessage(CString* strMessage);
	void AddSystemMessage(CString* strMessage);
	void RemoveInvite(u_long nPlayerID);

	virtual void PrintWinnersMessage();

	virtual void OnWinnerEnd();
	void OnDrawEnd();

	BOOL RewardPlayer(u_long uPlayerID, __int64 nGold);
	BOOL RewardPlayerByMail(u_long uPlayerID, __int64 nGold);
	
	void SetGameState(GameState eState);

	void OnPlayerLogout(CUser* pUser);

	void EndWithRemainingAsWinners();
#endif
};

class CAegonPvPMinigameTicTacToe : public CAegonPvPMinigameBase
{
public:
	u_long m_nTicTacToeField[9];

	CAegonPvPMinigameTicTacToe();
	virtual ~CAegonPvPMinigameTicTacToe();

	void WriteToBuffer(CAr& ar, u_long uIDReveiver) override;
#ifdef __CLIENT
	void ReadUpdateFromBuffer(CAr& ar) override;
#endif
	void ReadFromBuffer(CAr& ar) override;

	BOOL HasPlayerWon(u_long uPlayerID, vector<u_long>* pVecWinners) override;
	BOOL IsDraw() override;

	BOOL IsValidMove(int nPlayerMove) override;

#ifdef __WORLDSERVER
	void OnGameStart() override;
	ProcessResult Process() override;
	BOOL DoPlayerMove(int nPlayerMove, BOOL* bSetNextTurn) override;
#endif
};

class CAegonPvPMinigameConnectFour : public CAegonPvPMinigameBase
{
public:
	u_long m_nConnectFourField[6][7];

	CAegonPvPMinigameConnectFour();
	virtual ~CAegonPvPMinigameConnectFour();

	void WriteToBuffer(CAr& ar, u_long uIDReveiver) override;
#ifdef __CLIENT
	void ReadUpdateFromBuffer(CAr& ar) override;
#endif
	void ReadFromBuffer(CAr& ar) override;

	BOOL HasPlayerWon(u_long uPlayerID, vector<u_long>* pVecWinners) override;
	BOOL IsDraw() override;

	BOOL IsValidMove(int nPlayerMove) override;

#ifdef __WORLDSERVER
	void OnGameStart() override;
	ProcessResult Process() override;
	BOOL DoPlayerMove(int nPlayerMove, BOOL* bSetNextTurn) override;
#endif
};

class CAegonPvPMinigameFluno : public CAegonPvPMinigameBase
{
public:
	//TODO "field"
	vector<__FLUNO_CARD> m_vCardTakeStack;
	vector<__FLUNO_CARD> m_vCardPutStack;

	BOOL m_bClockwise;
	int m_nDrawCards;
	BOOL m_bSelectColor;
	BOOL m_bPassTurn;

#ifdef __WORLDSERVER
	BOOL m_bSkip;
#endif

	CAegonPvPMinigameFluno();
	virtual ~CAegonPvPMinigameFluno();

	void WriteToBuffer(CAr& ar, u_long uIDReveiver) override;
#ifdef __CLIENT
	void ReadUpdateFromBuffer(CAr& ar) override;
#endif
	void ReadFromBuffer(CAr& ar) override;

	BOOL HasPlayerWon(u_long uPlayerID, vector<u_long>* pVecWinners) override;
	BOOL IsDraw() override;

	BOOL IsValidMove(int nPlayerMove) override;

	__MINIGAME_PLAYER* CreateNewPlayer() override;

	BOOL IsValidCardMove(__MINIGAME_PLAYER_FLUNO* pPlayer, int nPlayerMove);
#ifdef __WORLDSERVER
	virtual void OnGameStart();
	ProcessResult Process() override;
	void SetNextTurn() override;
	BOOL DoPlayerMove(int nPlayerMove, BOOL* bSetNextTurn) override;

	void PrintWinnersMessage() override;
	void OnWinnerEnd() override;

	void RefillTakeStack();
	BOOL TakeCard(__MINIGAME_PLAYER_FLUNO* pPlayer);

	void DrawPunishCards(__MINIGAME_PLAYER_FLUNO* pPlayer);

	void OnPlayerChat(CUser* pUser, char* szMessage) override;
#endif
};

class CAegonPvPMinigamesMng
{
#ifdef __CLIENT
	CAegonPvPMinigameBase* m_pActiveGame;
#endif

#ifdef __WORLDSERVER
	DWORD m_dwLastGameProc;
#endif

public:
	vector<CAegonPvPMinigameBase*> m_vecActiveGames;

	CAegonPvPMinigamesMng();
	virtual ~CAegonPvPMinigamesMng();

	static CAegonPvPMinigamesMng* GetInstance(void);

	BOOL IsValidMinigameID(int nGameID);
	BOOL IsPlaying(u_long uPlayerID);
	void GetGameNameByID(CString* szName, int nGameID);

	CAegonPvPMinigameBase* CreateNewGame(int nGameID);
	void WriteMinigameList(CAr& ar, u_long uIDReveiver);

#ifdef __CLIENT
	void DeleteMinigameList();
	void ReadMinigameList(CAr& ar);
	void SetActiveGame(CAegonPvPMinigameBase* pGame);
	CAegonPvPMinigameBase* GetActiveGame();
	void RemoveActiveGame();
#endif	

#ifdef __WORLDSERVER	
	CAegonPvPMinigameBase* GetGameByPlayerID(u_long uPlayerID);
	void StartMinigame(CUser* pUser, int nGameID);
	void Process();
	void ProcessGames();

	void OnPlayerCommand(CUser* pUser, __MINIGAME_COMMAND sCommand, char* szName);
	void OnPlayerChat(CUser* pUser, char* szMessage);
	void OnPlayerLogout(CUser* pUser);
#endif // __WORLDSERVER
};

#endif // __AEGON_PVP_MINIGAMES
#endif // __AEGPVPGAMES__H