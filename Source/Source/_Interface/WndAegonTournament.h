#pragma once
#include "AegonTournament.h"
#ifdef __AEGON_TOURNAMENT
#define AT_LEADER_BUTTONS 5
class AegonTournamentWndHelper
{
public:


};
class CWndTournamentRewardList : public CWndListBox
{
	int m_nSelectedItem;

	__TOURNAMENT_REWARD m_tournamentRewards;

public:
	int m_nRowHeight;

	CWndTournamentRewardList();
	virtual ~CWndTournamentRewardList();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	// message
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface(CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);

	void SetTournamentRewards(__TOURNAMENT_REWARD* pTournamentRewards);



};
class CWndAegonCreateTournament : public CWndNeuz
{

private:
	CWndComboBox* m_pWndComboBoxPresets;

	CWndEdit* m_pWndTeamSize;
	CWndEdit* m_pWndRounds;
	CWndEdit* m_pWndRoundTime;

	CWndComboBox* m_pWndStartTimeHour;
	CWndComboBox* m_pWndStartTimeMin;
	CWndComboBox* m_pWndStartTimeDay;
	CWndComboBox* m_pWndStartTimeMonth;
	CWndComboBox* m_pWndStartTimeYear;

	CWndComboBox* m_pWndFriendlyFire;

	CWndComboBox* m_pWndLevelRangeMin;
	CWndComboBox* m_pWndLevelRangeMax;

	CWndComboBox* m_pWndClasses;
	CWndComboBox* m_pWndTeamReq;

	CWndComboBox* m_pWndRewardList;

	CWndEdit* m_pWndRewardID;
	CWndEdit* m_pWndRewardNum;

	__TOURNAMENT_UNIT sTournamentUnit;

	CWndListBox* m_pWndRewardParticipation;
	CWndListBox* m_pWndRewardFirstPlace;
	CWndListBox* m_pWndRewardSecondPlace;

public:

	CWndAegonCreateTournament();
	~CWndAegonCreateTournament();


	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point);

	void FillPresetInfo();
	int GetComboBoxStringIndex(CWndComboBox* pComboBox, LPCSTR szString, BOOL bNoCase);
	void AddTournamentReward();
	void UpdateRewardLists();
	void ClearRewardList();
	void CreateTournament();
	void FillTournamentUnit();

};
class CWndAegonTournamentList : public CWndListBox
{
	int m_nRowHeight;

public:
	int m_nSelectedItem;

	CWndAegonTournamentList();
	virtual ~CWndAegonTournamentList();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface(CPoint point);

	void SelectionChanged();

};
class CWndAegonTournament : public CWndNeuz
{
	CWndAegonTournamentList* m_wndTournamentList;

	CWndTournamentRewardList* m_pWndParticipationRewards;
	CWndTournamentRewardList* m_pWndFirstPlaceRewards;
	CWndTournamentRewardList* m_pWndSecondPlaceRewards;
public:

	CWndAegonTournament();
	~CWndAegonTournament();


	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point);
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	void CreateRewardLists();
	void FillRewardLists();
	void RemoveRewardLists();

	void SelectedTournamentChanged();

	void OnGetTournamentMap();

};
class CWndAegonTournamentScoreBoard : public CWndNeuz
{
public:
	//main texture
	CString		strMainTexture;
	CTexture	mainTexture;
	//~main texture

	//caption
	CString		strCaption;
	CTexture	captionTexture;
	CRect		captionRect;
	//~caption

	//top10
	CString		strTop10;
	CTexture	top10Texture;
	CRect		top10Rect;
	CRect		top10PlayerRect[10][4];

	CString		top10Name[10];
	CString		top10Team[10];
	DWORD		top10Kill[10];
	DWORD		top10Death[10];
	//~top10

	//team fields
	CRect		fieldsRoundOne[8];
	CRect		fieldsRoundTwo[4];
	CRect		fieldsRoundThree[2];
	CRect		fieldWinner;
	//~team fields

	//icons
	CString		strTrophy;
	CTexture	trophyTexture;
	CRect		trophyRect;
	//~icons

	//arrows
	CString		strArrowTextureRoundOne[2];
	CString		strArrowTextureRoundTwo[2];
	CString		strArrowTextureRoundThree[2];

	CTexture	arrowTextureRoundOne[2];
	CTexture	arrowTextureRoundTwo[2];
	CTexture	arrowTextureRoundThree[2];

	CRect		arrowRoundOne[8];
	CRect		arrowRoundTwo[4];
	CRect		arrowRoundThree[2];
	//~arrows

	//team overlay
	CString		strOverlayRed;
	CTexture	overlayRedTexture;

	CString		strOverlayGreen;
	CTexture	overlayGreenTexture;

	CString		strOverlayGold;
	CTexture	overlayGoldTexture;
	//~team overlay

	CWndAegonTournamentScoreBoard();
	~CWndAegonTournamentScoreBoard();


	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnDestroy();
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	void SetTop10();
	void SortTop10();
	void AddToTop10(int teamIndex, int playerIndex);

};
#endif // __AEGON_TOURNAMENT