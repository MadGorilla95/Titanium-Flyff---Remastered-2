#pragma once

#ifdef __SYS_MONSTER_CLASH
#define STATE_CLOSED  0
#define STATE_REGISTER 1
#define STATE_WAIT 2
#define STATE_CLASH 3

class CWndMonsterClashRegister : public CWndNeuz
{
private:

public:
	CWndMonsterClashRegister();
	~CWndMonsterClashRegister();

	void AddGuild(DWORD dwGuildId, CString strGuild);
	void RemoveGuild(DWORD dwGuildId);

	virtual void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
};

class CWndMonsterClashRegisterMember : public CWndNeuz
{
private:

public:
	CWndMonsterClashRegisterMember();
	~CWndMonsterClashRegisterMember();

	virtual void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);

	void AddMemberL(u_long idPlayer, CString strPlayer);
	void AddMemberR(u_long idPlayer, CString strPlayer);

	void RemoveMemberL(u_long idPlayer, CString strPlayer);
	void RemoveMemberR(u_long idPlayer, CString strPlayer);
};

class CWndMonsterClashRank
{
public:
	struct GUILD_RANK
	{
		CString strGuild;
		int nKillCount;
		int nStage;
		time_t time;
		DWORD dwGuild;
		GUILD_RANK()
			: strGuild(""),
			nKillCount(0),
			time(0),
			dwGuild(0),
			nStage(-1)
		{

		}
	};
private:
	vector<GUILD_RANK> vecGuild;
	int nMaxKillCount;
	BOOL bActive;
	int nState;
	time_t tTime;
	int nFinalStage;

public:
	CWndMonsterClashRank();

	static CWndMonsterClashRank* GetInstance();

	void SetMaxKillCount(const int nKillCount) { nMaxKillCount = nKillCount; }
	int GetMaxKillCount()const { return nMaxKillCount; }

	BOOL IsActive() const { return bActive; }
	void SetActive(const BOOL active) { bActive = active; }

	void SetState(const int State) { nState = State; }
	int GetState() const { return nState; }

	void SetTime(const time_t time) { tTime = time; }
	time_t GetTime() const { return tTime; }

	void Serialize(CAr& ar);
	void Sort();

	char m_szSecretRoomStr[256];
	__KILLCOUNTCIPHERS m_stKillCountCiphers[MAX_KILLCOUNT_CIPHERS];
	BOOL m_bFlashBackground;

	void DrawMonsterClashGuildInfo(C2DRender* p2DRender, const BOOL bIsMyGuild, const int nRank, const GUILD_RANK sGuild, const CPoint ptRank, const CPoint ptLogo,
		const CPoint ptGName, const CPoint ptHypoon, CPoint ptState, const float fLogoScaleX, const float fLogoScaleY, const CRect rectBg);
	void DrawMonsterClashinfo(C2DRender* p2DRender);
	void DrawMyGuildKillCount(C2DRender* p2DRender, const GUILD_RANK sGuild, CPoint ptState, const int nMax);
	void DrawOutLineLamp(C2DRender* p2DRender, const CRect rectBg, const DWORD dwColorstart, const DWORD dwColorend, const int nState, const int nRank, const BOOL bIsMyGuild) const;
	void DrawOutLineFlash(C2DRender* p2DRender, const CRect rectBg, const DWORD dwColorstart, const DWORD dwColorend);
};
#endif //__SYS_MONSTER_CLASH