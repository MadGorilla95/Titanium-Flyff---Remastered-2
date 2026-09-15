#pragma once
#include "AegonPvPMinigames.h"
#ifdef __AEGON_PVP_MINIGAMES

#define FLUNO_SIZE_X	500
#define FLUNO_SIZE_Y	400

#define FLUNO_CARD_Y_OFFSET 30
#define FLUNO_CARD_X_OFFSET 20

#define __MINIGAME_MAX_ANI_TIME	10000 //max animation time before abort

#define __FLUNO_OTHER_CARD_SCALE	0.8

class CWndPvPMinigameBase;

class CMinigameAnimationBase
{
public:
	CWndPvPMinigameBase* m_pParentWnd;

	DWORD m_dwStartTime;
	BOOL m_bEnded;
	DWORD m_dwLastStep;
	DWORD m_dwFrameTime;

	BOOL m_bDeleteable;

	CMinigameAnimationBase();
	virtual ~CMinigameAnimationBase();

	void SetParent(CWndPvPMinigameBase* pParent) { m_pParentWnd = pParent; };

	virtual void Initialize();
	virtual void DoAniStep();
	virtual void RenderAni(C2DRender* p2DRender, int nPos);
};

class CMinigameMultipleAnimations : public CMinigameAnimationBase
{
public:
	int m_nCurrent;
	BOOL m_bRenderPrevious;
	vector<CMinigameAnimationBase*> m_vecpAnimations;

	CMinigameMultipleAnimations();
	virtual ~CMinigameMultipleAnimations();

	void Initialize() override;
	void DoAniStep() override;
	void RenderAni(C2DRender* p2DRender, int nPos) override;

	void AddAnimation(CMinigameAnimationBase* pAnimation);
	size_t GetAniCount();
};

class CMinigameAnimationC4Move : public CMinigameAnimationBase
{
public:
	CPoint m_ptAni;
	CPoint m_ptAniEnd;
	u_long m_nMoveAniValue[3];

	CMinigameAnimationC4Move();
	virtual ~CMinigameAnimationC4Move();

	void Initialize() override;
	void DoAniStep() override;
	void RenderAni(C2DRender* p2DRender, int nPos) override;

	void SetValues(int nRow, int nCollumn, u_long uTurn);
};

class CMinigameAnimationFlunoInit : public CMinigameAnimationBase
{
public:


	CMinigameAnimationFlunoInit();
	virtual ~CMinigameAnimationFlunoInit();

	void Initialize() override;
	void DoAniStep() override;
	void RenderAni(C2DRender* p2DRender, int nPos) override;
};

class CMinigameAnimationFlunoMoveCard : public CMinigameAnimationBase
{
public:

	int    m_nPercentDone;

	CPoint m_ptStart;
	CPoint m_ptEnd;
	
	FLOAT m_fRotateStart;
	FLOAT m_fRotateEnd;

	CTexture* m_pCardTexture;

	float m_fScaleStart;
	float m_fScaleEnd;

	CMinigameAnimationFlunoMoveCard();
	virtual ~CMinigameAnimationFlunoMoveCard();

	void Initialize() override;
	void DoAniStep() override;
	void RenderAni(C2DRender* p2DRender, int nPos) override;

	void SetValues(CPoint ptStart, CPoint ptEnd, FLOAT fRotateStart, FLOAT fRotateEnd, CTexture* pCardTexture, float fScaleStart = 1.0f, float fScaleEnd = 1.0f);
};

class CWndAegonPvPGameCreate : public CWndNeuz
{
public:

	CWndAegonPvPGameCreate();
	~CWndAegonPvPGameCreate();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);

	void SendCreateGame();
};

class CWndAegonPvPGameInvitation : public CWndNeuz
{
	u_long m_uChallengerID;
public:
	CWndAegonPvPGameInvitation();
	virtual ~CWndAegonPvPGameInvitation();

	CString m_strGame;
	int m_nGameID;

	void OnSetInfo(const char* szName, OBJID objid, DWORD nGameID);

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};
class CWndMinigamePlayerList : public CWndListBox
{
	int m_nSelectedItem;

public:
	int m_nRowHeight;

	CWndMinigamePlayerList();
	virtual ~CWndMinigamePlayerList();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();

	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface(CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);


	void SetSelectedItem(CPoint point);

	int GetSelectedItemIndex() { return m_nSelectedItem; };

};

class CWndPvPMinigameBase : public CWndBase
{

public:
	int m_nRenderHoverMove;
	int m_nClickMove;

	BOOL m_bUpdateInProgress;

	CAegonPvPMinigameBase* m_pUpdateGame;

	CMinigameAnimationBase* m_pAnimation;

	CWndPvPMinigameBase();
	virtual ~CWndPvPMinigameBase();
	void Create(DWORD m_dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnMouseWndSurface(CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);

	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void PaintFrame(C2DRender* p2DRender);
	virtual void SetWndRect(CRect rectWnd, BOOL bOnSize);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);

	virtual void OnGameRefresh();

	void UpdateGameState();
	void UpdateCurrentGame();

	virtual	void OnBaseAni();
	virtual void OnGameSpecificAni();
	
	void ProcessAni(C2DRender* p2DRender, int nPos);
	virtual void OnAniEnd();

#ifdef __AEGON_THEME_SWITCHER
	virtual void OnThemeSelectionChanged();
#endif
};

class CWndPvPMinigameTikTakToe : public CWndPvPMinigameBase
{
	CTexture* m_pTextureField;
	CTexture* m_pTextureMoveMe;
	CTexture* m_pTextureMoveEnemy;

	CRect m_pRectFields[9];

public:
	CWndPvPMinigameTikTakToe();
	virtual ~CWndPvPMinigameTikTakToe();
	void OnInitialUpdate() override;
	void OnDraw(C2DRender* p2DRender) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult) override;
	void OnMouseWndSurface(CPoint point) override;
	void OnLButtonUp(UINT nFlags, CPoint point) override;
	void OnLButtonDown(UINT nFlags, CPoint point) override;

	void OnGameRefresh() override;

	void OnBaseAni() override;
	void OnGameSpecificAni() override;

#ifdef __AEGON_THEME_SWITCHER
	virtual void OnThemeSelectionChanged();
#endif
};

class CWndPvPMinigameConnectFour : public CWndPvPMinigameBase
{
public:
	CTexture* m_pTextureBGField;
	CTexture* m_pTextureFGField;

	CTexture* m_pTextureRedPiece;
	CTexture* m_pTextureYellowPiece;

	CRect m_rectGameRows[6];
	CRect m_rectGameColumns[7];

	CRect m_rectHoles[6][7];

	CWndPvPMinigameConnectFour();
	virtual ~CWndPvPMinigameConnectFour();
	void OnInitialUpdate() override;
	void OnDraw(C2DRender* p2DRender) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult) override;
	void OnMouseWndSurface(CPoint point) override;
	void OnLButtonUp(UINT nFlags, CPoint point) override;
	void OnLButtonDown(UINT nFlags, CPoint point) override;

	void OnGameRefresh() override;

	void OnBaseAni() override;
	void OnGameSpecificAni() override;

	void CalculateRects();

#ifdef __AEGON_THEME_SWITCHER
	virtual void OnThemeSelectionChanged();
#endif
};

class CWndPvPMinigameFluno : public CWndPvPMinigameBase
{
	CPoint m_pointTakeStack;
	CRect m_rectTakeStack;

	CPoint m_pointPutStack;
	CRect m_rectPutStack;

	CRect m_rectPlayerFieldMe;
	CRect m_rectPlayerFields[3];

	CTexture* m_pTextureCardBackside;

	CTexture* m_pTextureColorPicker[4];

	CRect m_rectColorPicker[4];

	int m_nColorPickerOffsetY[4];

	DWORD m_dwLastColorPickerIncrease;

	__AEGON_2D_TRIANGLE m_tColorPicker[4];

	int m_nNoDrawOwn;

public:
	CWndPvPMinigameFluno();
	virtual ~CWndPvPMinigameFluno();
	void OnInitialUpdate() override;
	void OnDraw(C2DRender* p2DRender) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult) override;
	void OnMouseWndSurface(CPoint point) override;
	void OnLButtonUp(UINT nFlags, CPoint point) override;
	void OnLButtonDown(UINT nFlags, CPoint point) override;

	void OnGameRefresh() override;

	void OnBaseAni() override;
	void OnGameSpecificAni() override;

	CTexture* GetCardTexture(__FLUNO_CARD* pCard);

	CPoint GetOwnHandPointByIndex(CTexture* pCardTexture, int nIndex, BOOL bHoverMoveOffset);
	CPoint GetOtherHandPointByIndex(CAegonPvPMinigameFluno* pGame, int nPlayerSize, u_long uPlayerID, int nIndexCard);

	void DrawDeck(C2DRender* p2DRender, CPoint ptCenter, int nCount);

	void DrawColorPicker(C2DRender* p2DRender, BOOL bMyTurn);
	void DrawColorWish(C2DRender* p2DRender, CardColor sCardColor);

	void OnAniEnd() override;
#ifdef __AEGON_THEME_SWITCHER
	virtual void OnThemeSelectionChanged();
#endif
};

class CWndAegonPvPGameMain : public CWndNeuz
{
	CWndMinigamePlayerList* m_pWndPlayerList;

public:

	CWndAegonPvPGameMain();
	~CWndAegonPvPGameMain();

	int m_nGameID;

	CRect m_rectGameField;
	int nIncreasedWidth;
	int nIncreasedHeight;

	CWndPvPMinigameBase* m_pWndMinigame;

	vector<CAegonPvPMinigameBase*> m_vGameUpdateState;

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);

	void AdjustWindowForGame();

	void SendChat();
	void AddChatMessage(char* szMessage, BOOL bSystem = FALSE);
	void QuitGame();
	void OnGameRefresh();
	void SetButtons();
};
class CWndMinigameGameList : public CWndListBox
{
	int m_nSelectedItem;

public:
	int m_nRowHeight;

	CWndMinigameGameList();
	virtual ~CWndMinigameGameList();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();

	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface(CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);

	void SetSelectedItem(CPoint point);
	int GetSelectedItemIndex() { return m_nSelectedItem; };

};
class CWndAegonPvPGameFinder : public CWndNeuz
{
	CWndMinigameGameList* m_pWndGameList;
public:

	CWndAegonPvPGameFinder();
	~CWndAegonPvPGameFinder();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);

	void JoinSelectedGame();
};
#endif // __AEGON_PVP_MINIGAMES