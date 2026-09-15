#include "stdafx.h"
#include "resData.h"
#include "WndAegonPvPMinigames.h"
#include "DPClient.h"
#include "defineText.h"
#include "WndManager.h"
#include "AegonPvPMinigames.h"
#include "WndManager.h"
#include "Model.h"
#include "ModelMng.h"
#include "ModelObject.h"
#include "ModelGlobal.h"
#include "math.h"
#include <ctime>
#include "defineObj.h"

extern CDPClient g_DPlay;

#ifdef __AEGON_PVP_MINIGAMES
///////////////////////////
/////////Ani Base//////////
///////////////////////////
CMinigameAnimationBase::CMinigameAnimationBase()
{
	m_dwStartTime = 0;
	m_bEnded = FALSE;
	m_dwLastStep = 0;
	m_pParentWnd = NULL;
	m_dwFrameTime = 10;

	m_bDeleteable = FALSE;
}
CMinigameAnimationBase::~CMinigameAnimationBase()
{
}
void CMinigameAnimationBase::Initialize()
{
}
void CMinigameAnimationBase::DoAniStep()
{
}
void CMinigameAnimationBase::RenderAni(C2DRender* p2DRender, int nPos)
{
}
///////////////////////////
///////Multiple Anis////////
///////////////////////////
CMinigameMultipleAnimations::CMinigameMultipleAnimations()
{
	m_nCurrent = 0;
	m_bRenderPrevious = FALSE;
}
CMinigameMultipleAnimations::~CMinigameMultipleAnimations()
{
	for (int i = 0; i < m_vecpAnimations.size(); i++) 
	{
		SAFE_DELETE(m_vecpAnimations[i]);
	}
}
void CMinigameMultipleAnimations::Initialize()
{
	m_dwStartTime = GetTickCount();
	m_bEnded = FALSE;
	m_nCurrent = 0;

	if (m_vecpAnimations.size() <= 0) 
	{
		m_bEnded = TRUE;
		return;
	}
	m_vecpAnimations[0]->Initialize();

}
void CMinigameMultipleAnimations::DoAniStep()
{
	if (m_bEnded)
		return;

	//check end
	if (m_nCurrent < 0 || m_nCurrent >= m_vecpAnimations.size()) 
	{
		m_bEnded = TRUE;
		return;
	}

	CMinigameAnimationBase* pCurrentAni = m_vecpAnimations[m_nCurrent];
	if (!pCurrentAni || pCurrentAni->m_bEnded) 
	{
		++m_nCurrent;
		if (m_nCurrent >= 0 && m_nCurrent < m_vecpAnimations.size() && m_vecpAnimations[m_nCurrent])
			m_vecpAnimations[m_nCurrent]->Initialize();
		else
			m_nCurrent = m_vecpAnimations.size() - 1;


		if (m_nCurrent == m_vecpAnimations.size() - 1 && m_vecpAnimations[m_nCurrent]->m_bEnded)
		{
			m_bEnded = TRUE;
		}

		return;
	}

	pCurrentAni->DoAniStep();
}
void CMinigameMultipleAnimations::RenderAni(C2DRender* p2DRender, int nPos)
{
	if (!p2DRender || m_nCurrent >= m_vecpAnimations.size())
		return;

	if (m_bRenderPrevious) 
	{
		for (int i = 0; i <= m_nCurrent; i++) 
		{
			CMinigameAnimationBase* pCurrentAni = m_vecpAnimations[i];
			if (pCurrentAni)
				pCurrentAni->RenderAni(p2DRender, nPos);
		}
	}
	else 
	{
		CMinigameAnimationBase* pCurrentAni = m_vecpAnimations[m_nCurrent];
		if (pCurrentAni)
			pCurrentAni->RenderAni(p2DRender, nPos);
	}
	
}
void CMinigameMultipleAnimations::AddAnimation(CMinigameAnimationBase* pAnimation)
{
	if (pAnimation)
	{
		m_vecpAnimations.push_back(pAnimation);
	}
}
size_t CMinigameMultipleAnimations::GetAniCount() 
{
	return m_vecpAnimations.size();
}

///////////////////////////
////////Ani C4 Move////////
///////////////////////////
CMinigameAnimationC4Move::CMinigameAnimationC4Move()
{
	for(int i = 0; i < 3; i++)
		m_nMoveAniValue[i] = 0;
}
CMinigameAnimationC4Move::~CMinigameAnimationC4Move()
{
}
void CMinigameAnimationC4Move::Initialize()
{
	int nRow = m_nMoveAniValue[0];
	int nColumn = m_nMoveAniValue[1];

	m_dwStartTime = GetTickCount();
	m_bEnded = FALSE;

	if (m_pParentWnd)
	{
		CWndPvPMinigameConnectFour* pWndParent = ((CWndPvPMinigameConnectFour*)m_pParentWnd);
		if (pWndParent && pWndParent->m_pTextureRedPiece)
		{
			m_ptAni = CPoint(pWndParent->m_rectHoles[0][nColumn].CenterPoint().x - (pWndParent->m_pTextureRedPiece->m_size.cx / 2), pWndParent->m_rectHoles[0][nColumn].CenterPoint().y - pWndParent->m_pTextureRedPiece->m_size.cy);
			m_ptAniEnd = CPoint(pWndParent->m_rectHoles[nRow][nColumn].CenterPoint().x - (pWndParent->m_pTextureRedPiece->m_size.cx / 2), pWndParent->m_rectHoles[nRow][nColumn].CenterPoint().y - (pWndParent->m_pTextureRedPiece->m_size.cy / 2));
		}
	}
	else
		m_bEnded = TRUE;
}
void CMinigameAnimationC4Move::DoAniStep()
{
	if (m_bEnded)
		return;

	int nRow = m_nMoveAniValue[0];
	int nColumn = m_nMoveAniValue[1];

	if (nColumn >= 7 || nRow >= 6)
		return;

	if (GetTickCount() > m_dwLastStep + m_dwFrameTime)
	{
		m_ptAni.y += 10;
		m_dwLastStep = GetTickCount();
	}

	if (m_ptAni.y > m_ptAniEnd.y)
	{
		m_ptAni.y = m_ptAniEnd.y;
		m_bEnded = TRUE;
		return;
	}
}
void CMinigameAnimationC4Move::RenderAni(C2DRender* p2DRender, int nPos)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	if (!m_pParentWnd) 
	{
		m_bEnded = TRUE;
		return;
	}

	switch (nPos)
	{
	case 0: //between bg and fg
	{
		CTexture* pAniPiece = NULL;
		if (m_nMoveAniValue[2] == g_pPlayer->m_idPlayer)
		{
			pAniPiece = ((CWndPvPMinigameConnectFour*)m_pParentWnd)->m_pTextureRedPiece;
		}
		else
		{
			pAniPiece = ((CWndPvPMinigameConnectFour*)m_pParentWnd)->m_pTextureYellowPiece;
		}

		if (pAniPiece)
			pAniPiece->Render(p2DRender, m_ptAni);
		break;
	}
	}


	
}
void CMinigameAnimationC4Move::SetValues(int nRow, int nCollumn, u_long uTurn)
{
	m_nMoveAniValue[0] = nRow;
	m_nMoveAniValue[1] = nCollumn;
	m_nMoveAniValue[2] = uTurn;
}
///////////////////////////
///////Ani Fluno Init///////
///////////////////////////
CMinigameAnimationFlunoInit::CMinigameAnimationFlunoInit()
{

}
CMinigameAnimationFlunoInit::~CMinigameAnimationFlunoInit()
{
}
void CMinigameAnimationFlunoInit::Initialize()
{

	m_dwStartTime = GetTickCount();
	m_bEnded = FALSE;

	if (m_pParentWnd)
	{
		CWndPvPMinigameFluno* pWndParent = ((CWndPvPMinigameFluno*)m_pParentWnd);

	}
	else
		m_bEnded = TRUE;

}
void CMinigameAnimationFlunoInit::DoAniStep()
{
	if (m_bEnded)
		return;


	if (GetTickCount() > m_dwLastStep + m_dwFrameTime)
	{
		//do next step
	}

	if (1)//check for end
	{
		m_bEnded = TRUE;
		return;
	}
}
void CMinigameAnimationFlunoInit::RenderAni(C2DRender* p2DRender, int nPos)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	if (!m_pParentWnd)
	{
		m_bEnded = TRUE;
		return;
	}

	switch (nPos) 
	{
	case 0:
	{
		break;
	}
	}
	//TODO render
}
///////////////////////////
////Ani Fluno Move Card////
///////////////////////////
CMinigameAnimationFlunoMoveCard::CMinigameAnimationFlunoMoveCard()
{
	m_nPercentDone = 0;

	m_fRotateStart = 0.0;
	m_fRotateEnd = 0.0;

	m_pCardTexture = NULL;

	m_fScaleStart = 0.0;
	m_fScaleEnd = 0.0;
}
CMinigameAnimationFlunoMoveCard::~CMinigameAnimationFlunoMoveCard()
{
}
void CMinigameAnimationFlunoMoveCard::Initialize()
{

	m_dwStartTime = GetTickCount();
	m_bEnded = FALSE;

	if (m_pParentWnd)
	{
		CWndPvPMinigameFluno* pWndParent = ((CWndPvPMinigameFluno*)m_pParentWnd);
	}
	else
		m_bEnded = TRUE;

}
void CMinigameAnimationFlunoMoveCard::DoAniStep()
{
	if (m_bEnded)
		return;


	if (GetTickCount() > m_dwLastStep + m_dwFrameTime)
	{
		//do next step
		
		m_nPercentDone += 5;
		if (m_nPercentDone > 100)
			m_nPercentDone = 100;
		
		m_dwLastStep = GetTickCount();
	}

	if (m_nPercentDone == 100)//check for end
	{
		m_bEnded = TRUE;
		return;
	}
}
void CMinigameAnimationFlunoMoveCard::RenderAni(C2DRender* p2DRender, int nPos)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	if (!m_pParentWnd)
	{
		m_bEnded = TRUE;
		return;
	}

	switch (nPos)
	{
	case 0:
	{
		CPoint ptRender;
		ptRender.x = m_ptStart.x + (m_ptEnd.x - m_ptStart.x) * ((double)m_nPercentDone / (double)100);
		ptRender.y = m_ptStart.y + (m_ptEnd.y - m_ptStart.y) * ((double)m_nPercentDone / (double)100);

		float fRotateCurrent = m_fRotateStart + (m_fRotateEnd - m_fRotateStart) * ((double)m_nPercentDone / (double)100);
		float fRadian = DEGREETORADIAN(fRotateCurrent);

		float fScale = m_fScaleStart + (m_fScaleEnd - m_fScaleStart) * ((double)m_nPercentDone / (double)100);			

		if (m_pCardTexture) 
			m_pCardTexture->RenderRotate(p2DRender, CPoint(ptRender.x, ptRender.y), fRadian, (BOOL)TRUE, (DWORD)255, fScale, fScale);
		break;
	}
	}
	//TODO render
}
void CMinigameAnimationFlunoMoveCard::SetValues(CPoint ptStart, CPoint ptEnd, FLOAT fRotateStart, FLOAT fRotateEnd, CTexture* pCardTexture, float fScaleStart, float fScaleEnd)
{

	m_ptStart = ptStart;
	m_ptEnd = ptEnd;
	m_fRotateStart = fRotateStart;
	m_fRotateEnd = fRotateEnd;
	m_pCardTexture = pCardTexture;
	m_fScaleStart = fScaleStart;
	m_fScaleEnd = fScaleEnd;

}
///////////////////////////
//////Invite WINDOW////////
///////////////////////////
CWndAegonPvPGameCreate::CWndAegonPvPGameCreate()
{
}
CWndAegonPvPGameCreate::~CWndAegonPvPGameCreate()
{
}
void CWndAegonPvPGameCreate::OnDraw(C2DRender* p2DRender)
{


}
BOOL CWndAegonPvPGameCreate::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_BUTTON1)
	{
		SendCreateGame();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

BOOL CWndAegonPvPGameCreate::Initialize(CWndBase* pWndParent, DWORD)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PVP_MINIGAME_CREATE, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndAegonPvPGameCreate::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndAegonPvPGameCreate::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndAegonPvPGameCreate::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndAegonPvPGameCreate::OnLButtonDown(UINT nFlags, CPoint point)
{
}
void CWndAegonPvPGameCreate::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndComboBox* pWndComboGame = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);
	pWndComboGame->SetTabStop(TRUE);

	pWndComboGame->AddString(_T("Tik-Tak-Toe"));
	pWndComboGame->AddString(_T("Connect Four"));
	pWndComboGame->AddString(_T("Fluno"));

	pWndComboGame->SetCurSel(0);

	pWndComboGame->AddWndStyle(EBS_READONLY);
	MoveParentCenter();
}
void CWndAegonPvPGameCreate::SendCreateGame()
{
	if (IsInvalidObj(g_pPlayer))
		return;

	CWndStatic* pWndInfo = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	CWndComboBox* pWndComboGame = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);
	int nGameSelection = pWndComboGame->GetCurSel();
	int nGameID;

	CString strGame = pWndComboGame->GetString();
	size_t gameLength = strGame.GetLength();


	if (nGameSelection == 0)
	{
		nGameID = AEG_MINIGAME_TICTACTOE;
	}
	else if (nGameSelection == 1)
	{
		nGameID = AEG_MINIGAME_CONNECTFOUR;
	}
	else if (nGameSelection == 2)
	{
		nGameID = AEG_MINIGAME_FLUNO;
	}
	else
		nGameID = 0;

	if (gameLength == 0)
	{
		pWndInfo->m_dwColor = COLOR_ERROR;
		pWndInfo->SetTitle("Choose a Game");
	}
	else if (nGameID == 0)
	{
		pWndInfo->m_dwColor = COLOR_ERROR;
		pWndInfo->SetTitle("Invalid Game");
	}
	else
	{
		pWndInfo->m_dwColor = COLOR_SUCCESS;

		g_DPlay.SendPvPMinigameCreate(nGameID);
	}
	
}
//Invitation window 
CWndAegonPvPGameInvitation::CWndAegonPvPGameInvitation()
{
	m_uChallengerID = 0;
	m_strGame = "";
	m_nGameID = 0;
}
CWndAegonPvPGameInvitation::~CWndAegonPvPGameInvitation()
{
}
void CWndAegonPvPGameInvitation::OnSetInfo(const char* szName, OBJID objid, DWORD nGameID)
{
	m_uChallengerID = objid;
	CWndStatic* m_pName = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	CString strText;
	m_nGameID = nGameID;

	CAegonPvPMinigamesMng::GetInstance()->GetGameNameByID(&m_strGame, m_nGameID);

	strText.Format("%s invited you to play %s.", szName, m_strGame.GetString());
	m_pName->SetTitle(strText);
}
void CWndAegonPvPGameInvitation::OnDraw(C2DRender* p2DRender)
{
}
void CWndAegonPvPGameInvitation::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point((rectRoot.right - rectWindow.Width()) / 2, 70);
	Move(point);
}

BOOL CWndAegonPvPGameInvitation::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PVP_MINIGAME_INVITATION, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndAegonPvPGameInvitation::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndAegonPvPGameInvitation::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndAegonPvPGameInvitation::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndAegonPvPGameInvitation::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndAegonPvPGameInvitation::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	__MINIGAME_COMMAND tmpCommand;
	tmpCommand.dwCommandID = AEG_MINIGAME_COMMAND_JOIN;
	tmpCommand.dwCommandValue = m_uChallengerID;

	if (nID == WIDC_OK)
	{
		tmpCommand.dwCommandValue2 = 1;

		g_DPlay.SendPvPMinigameCommand(tmpCommand);
		Destroy();
	}
	else if (nID == WIDC_CANCEL || nID == WTBID_CLOSE)
	{
		tmpCommand.dwCommandValue2 = 0;

		g_DPlay.SendPvPMinigameCommand(tmpCommand);
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

///////////////////////////
///////MAIN WINDOW/////////
///////////////////////////
CWndAegonPvPGameMain::CWndAegonPvPGameMain()
{
	m_nGameID = 0;
	m_pWndPlayerList = NULL;
	m_rectGameField = CRect(0, 0, 0, 0);
	nIncreasedWidth = 0;
	nIncreasedHeight = 0;
	m_pWndMinigame = NULL;
}
CWndAegonPvPGameMain::~CWndAegonPvPGameMain()
{
	CAegonPvPMinigamesMng::GetInstance()->RemoveActiveGame();
	SAFE_DELETE(m_pWndMinigame);
	SAFE_DELETE(g_WndMng.m_pWndAegonPvPGameMessageBox);
}
void CWndAegonPvPGameMain::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndComboBox* pWndComboInvite = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	LPWNDCTRL listCtrl = GetWndCtrl(WIDC_CUSTOM1);
	CAegonPvPMinigameBase* pGame = CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();

	if (pGame)
	{
		m_nGameID = pGame->m_nGameID;
	}


	if (listCtrl)
	{
		SAFE_DELETE(m_pWndPlayerList);
		m_pWndPlayerList = new CWndMinigamePlayerList;
		m_pWndPlayerList->Create(WBS_CHILD | WBS_VSCROLL, listCtrl->rect, this, 100000);
		m_pWndPlayerList->m_byWndType = WTYPE_LISTBOX;
		m_pWndPlayerList->m_bTile = true;
		m_pWndPlayerList->m_bVisible = true;
		m_pWndPlayerList->m_strTexture = "WndEditTile00.tga";
	}

	//fill combobox with friend list
	for (map<u_long, Friend>::iterator it = g_WndMng.m_RTMessenger.begin(); it != g_WndMng.m_RTMessenger.end(); ++it)
	{
		const char* tmpName = CPlayerDataCenter::GetInstance()->GetPlayerString(it->first);
		if (tmpName != NULL)
			pWndComboInvite->AddString(tmpName);
	}
	if (pWndComboInvite->m_wndListBox.GetCount() > 0)
		pWndComboInvite->SetCurSel(0);

	switch (m_nGameID)
	{
	case AEG_MINIGAME_TICTACTOE:
		m_pWndMinigame = new CWndPvPMinigameTikTakToe;
		break;
	case AEG_MINIGAME_CONNECTFOUR:
		m_pWndMinigame = new CWndPvPMinigameConnectFour;
		break;
	case AEG_MINIGAME_FLUNO:
		m_pWndMinigame = new CWndPvPMinigameFluno;
		break;
	}

	AdjustWindowForGame();

	if (m_pWndMinigame)
	{
		m_pWndMinigame->Create(WBS_CHILD | WBS_NODRAWFRAME, m_rectGameField, this, APP_PVP_MINIGAME_SUBWINDOW);
	}

	MoveParentCenter();
}
void CWndAegonPvPGameMain::OnDraw(C2DRender* p2DRender)
{
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	DWORD dwColorHighlightBlue = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE);

	CAegonPvPMinigameBase* pGame = CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();

	CRect rectWnd = GetWndRect();

	CWndStatic* pWndStaticInvite = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	CWndComboBox* pWndComboInvite = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	CWndButton* pWndButtonInvite = (CWndButton*)GetDlgItem(WIDC_BUTTON1);

	CWndStatic* pWndStaticBet = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	CWndEdit* pWndEditBet = (CWndComboBox*)GetDlgItem(WIDC_EDIT2);
	CWndButton* pWndButtonBet = (CWndButton*)GetDlgItem(WIDC_BUTTON5);
	CWndButton* pWndButtonBetReset = (CWndButton*)GetDlgItem(WIDC_BUTTON6);

	CWndText* pWndChatText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	CWndStatic* pWndStaticPot = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
	CWndEdit* pWndEditPenyaPot = (CWndComboBox*)GetDlgItem(WIDC_EDIT3);

	CWndStatic* pWndStaticTurnCount = (CWndStatic*)GetDlgItem(WIDC_STATIC7);

	CWndStatic* pWndStaticLobby = (CWndStatic*)GetDlgItem(WIDC_STATIC8);
	CWndButton* pWndButtonLobbyToggle = (CWndButton*)GetDlgItem(WIDC_BUTTON7);

	CString strTmp;

	if (!pGame)
		return;

	if(pWndEditBet && pWndEditBet->m_wndScrollBar.IsVisible())
		pWndEditBet->m_wndScrollBar.SetVisible(FALSE);

	if (pWndStaticInvite && pWndComboInvite && pWndButtonInvite) 
	{
		CRect rectInvite = CRect(pWndStaticInvite->GetWndRect().left - 8, pWndStaticInvite->GetWndRect().top - 8, pWndStaticInvite->GetWndRect().right + 8, pWndButtonInvite->GetWndRect().bottom + 8);
		p2DRender->RenderRect(rectInvite, dwColorText);
	}
	
	if (pWndStaticBet && pWndEditBet && pWndButtonBetReset)
	{
		CRect rectInvite = CRect(pWndStaticBet->GetWndRect().left - 8, pWndStaticBet->GetWndRect().top - 8, pWndStaticBet->GetWndRect().right + 8, pWndButtonBetReset->GetWndRect().bottom + 8);
		p2DRender->RenderRect(rectInvite, dwColorText);
	}

	if (m_pWndPlayerList) 
	{
		CPoint pt1 = CPoint(m_pWndPlayerList->GetWndRect().right + 8, 8);
		CPoint pt2 = CPoint(m_pWndPlayerList->GetWndRect().right + 8, rectWnd.Height() - 35);

		p2DRender->RenderLine(pt1, pt2, dwColorText);
	}

	if (pWndChatText) 
	{
		CPoint pt1 = CPoint(pWndChatText->GetWndRect().right + 8, 8);
		CPoint pt2 = CPoint(pWndChatText->GetWndRect().right + 8, rectWnd.Height() - 35);

		p2DRender->RenderLine(pt1, pt2, dwColorText);

		pt1 = CPoint(m_pWndPlayerList->GetWndRect().right + 8, pWndChatText->GetWndRect().top - 8);
		pt2 = CPoint(pWndChatText->GetWndRect().right + 8, pWndChatText->GetWndRect().top - 8);

		p2DRender->RenderLine(pt1, pt2, dwColorText);

		if (pWndEditPenyaPot)
		{
			CString strMoney;
			strMoney.Format("%I64d", pGame->m_n64TotalBets);
			strMoney = GetNumberFormatEx(strMoney);
			pWndEditPenyaPot->SetString(strMoney.GetString());

			pt1.y = pWndEditPenyaPot->GetWndRect().bottom + 8;
			pt2.y = pt1.y;

			p2DRender->RenderLine(pt1, pt2, dwColorText);
		}
	}

	if (pWndStaticLobby && pWndButtonLobbyToggle) 
	{
		CRect rectInvite = CRect(pWndStaticLobby->GetWndRect().left - 8, pWndStaticLobby->GetWndRect().top - 8, pWndStaticLobby->GetWndRect().right + 8, pWndButtonLobbyToggle->GetWndRect().bottom + 8);
		p2DRender->RenderRect(rectInvite, dwColorText);

		CString strLobby;
		DWORD dwColorLobby;
		if (pGame->m_bAllowJoining) 
		{
			strLobby.Format("Open");
			dwColorLobby = COLOR_SUCCESS;
		}
		else 
		{
			strLobby.Format("Closed");
			dwColorLobby = COLOR_ERROR;
		}
		p2DRender->TextOut(pWndStaticLobby->GetWndRect().CenterPoint().x - p2DRender->GetFont()->GetTextExtent(strLobby.GetString()).cx / 2, pWndStaticLobby->GetWndRect().bottom + 4, strLobby.GetString(), dwColorLobby);
	}


	//time limit countdown
	int nTimeLeft = pGame->m_tPlayerTimeLimit - (time_null() + (((time_t)g_WndMng.m_nTimeDiff) * (time_t)60 * (time_t)60));
	if (nTimeLeft < 0)
		nTimeLeft = 0;

	if (pGame->m_eGameState != GameState::state_in_progress)
		nTimeLeft = 0;

	strTmp.Format("%d", nTimeLeft);
	CEditString tmpEditStr;
	tmpEditStr.SetParsingString(strTmp.GetString(), COLOR_ERROR);

	CD3DFont* oldFont = p2DRender->GetFont();
	p2DRender->SetFont(CWndBase::m_pTheme->m_pFontCaption);
	CSize sizeTextExtent = p2DRender->GetFont()->GetTextExtent_EditString(tmpEditStr);

	CRect rectTurnCount = pWndStaticTurnCount->GetWndRect();
	CRect rectTimeLimit = CRect(rectTurnCount.left - 8, rectTurnCount.top - 8, rectTurnCount.right + 8, 0);
	rectTimeLimit.bottom = rectTimeLimit.top  + 8 + rectTurnCount.Height() + 8 + sizeTextExtent.cy + 8;
	p2DRender->RenderRect(rectTimeLimit, dwColorText);

	p2DRender->TextOut_EditString(rectTimeLimit.CenterPoint().x - sizeTextExtent.cx / 2, rectTurnCount.bottom + 8, tmpEditStr, 0, 0, 2);
	p2DRender->SetFont(oldFont);

}
BOOL CWndAegonPvPGameMain::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID) 
	{
	case WTBID_CLOSE:
	case WIDC_CANCEL:
	{
		CAegonPvPMinigameBase* pGame = CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
		if (pGame && (pGame->m_eGameState == GameState::state_in_progress || pGame->m_eGameState == GameState::state_reward_players)) 
		{
			SAFE_DELETE(g_WndMng.m_pWndAegonPvPGameMessageBox);
			g_WndMng.m_pWndAegonPvPGameMessageBox = new CWndMessageBox;
			g_WndMng.m_pWndAegonPvPGameMessageBox->Initialize("Are you sure you want to quit the minigame?", this, MB_YESNO);
			break;
		}
		QuitGame();
		break;
	}
	case WIDC_BUTTON1: // invite
	{
		CWndComboBox* pWndComboInvite = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);

		__MINIGAME_COMMAND sCommand;
		sCommand.dwCommandID = AEG_MINIGAME_COMMAND_INVITE;


		CString strName = pWndComboInvite->GetString();
		if (strName.GetLength() > MAX_NAME - 1) 
		{
			g_WndMng.PutString("Name too long.", 0, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
			break;
		}

		char szName[MAX_NAME];
		sprintf(szName, strName.GetString());



		g_DPlay.SendPvPMinigameCommand(sCommand, szName);
		break;
	}
	case WIDC_BUTTON2: // send
	{
		SendChat();
		break;
	}
	case WIDC_BUTTON3: // kick
	{
		CAegonPvPMinigameBase* pGame = CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();

		if (!m_pWndPlayerList || !pGame)
			break;

		int nSelectedPlayer = m_pWndPlayerList->GetSelectedItemIndex();

		if (nSelectedPlayer < 0 || nSelectedPlayer >= pGame->m_vecsPlayers.size())
			break;

		__MINIGAME_COMMAND sCommand;
		sCommand.dwCommandID = AEG_MINIGAME_COMMAND_KICK;
		sCommand.dwCommandValue = pGame->m_vecsPlayers[nSelectedPlayer]->m_uPlayerID;

		g_DPlay.SendPvPMinigameCommand(sCommand);
		break;
	}
	case WIDC_BUTTON4: // ready
	{
		__MINIGAME_COMMAND sCommand;
		sCommand.dwCommandID = AEG_MINIGAME_COMMAND_READY;
		g_DPlay.SendPvPMinigameCommand(sCommand);
		break;
	}
	case WIDC_BUTTON5: // bet
	{
		CWndEdit* pWndEditBet = (CWndComboBox*)GetDlgItem(WIDC_EDIT2);

		__MINIGAME_COMMAND sCommand;
		sCommand.dwCommandID = AEG_MINIGAME_COMMAND_BET;
		__int64 nValue = 0;
		try 
		{
			nValue = strtoull(pWndEditBet->GetString(), NULL, 10);
		
		}
		catch (...) 
		{
			nValue = 0;
			pWndEditBet->Empty();
			break;
		}
	
		if (nValue <= 0 || nValue > INT_MAX)
			break;

		if (nValue > g_pPlayer->GetGold()) 
		{
			CString strError;
			strError.Format("Not enough penya.");
			g_WndMng.PutString(strError.GetString(), 0, COLOR_ERROR);
			break;
		}

		sCommand.dwCommandValue = nValue;

		g_DPlay.SendPvPMinigameCommand(sCommand);
		break;
	}
	case WIDC_BUTTON6: // reset bet
	{
		__MINIGAME_COMMAND sCommand;
		sCommand.dwCommandID = AEG_MINIGAME_COMMAND_RESET_BET;
		g_DPlay.SendPvPMinigameCommand(sCommand);

		break;
	}
	case WIDC_BUTTON7: // toggle lobby join
	{
		__MINIGAME_COMMAND sCommand;
		sCommand.dwCommandID = AEG_MINIGAME_COMMAND_T_JOIN;
		g_DPlay.SendPvPMinigameCommand(sCommand);
		break;
	}
	case WIDC_EDIT1: // º»¹® 
	{
		if (message == EN_RETURN)
		{
			SendChat();			
		}
		else if (message == EN_CHANGE) //auto scrolling edit field
		{
			CWndEdit* pWndChatEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);

			CPoint ptCaret = pWndChatEdit->GetCaretPos();
			DWORD dwLineCount = pWndChatEdit->m_string.GetLineCount();
			DWORD dwFontMaxHeight = pWndChatEdit->GetFont()->GetMaxHeight();

			DWORD dwCaretLineOffset = pWndChatEdit->m_nLineSpace + dwFontMaxHeight;
			int nCaretLine = 0;
			if (dwCaretLineOffset > 0)
				nCaretLine = ptCaret.y / dwCaretLineOffset;

			int nMaxScrollPos = pWndChatEdit->m_wndScrollBar.GetMaxScrollPos() - pWndChatEdit->m_wndScrollBar.GetScrollPage();
			int nMinScrollPos = pWndChatEdit->m_wndScrollBar.GetMinScrollPos();

			if (nCaretLine > dwLineCount)
				nCaretLine = dwLineCount;

			if (nCaretLine >= nMaxScrollPos)
			{
				pWndChatEdit->m_wndScrollBar.SetMaxScrollPos();
			}
			else
			{
				if (nCaretLine < nMinScrollPos)
					nCaretLine = nMinScrollPos;
				pWndChatEdit->m_wndScrollBar.SetScrollPos(nCaretLine);
			}
		}
		break;
	}
	case WIDC_EDIT2: 
	{
		if (message == EN_CHANGE) //auto scrolling edit field
		{
			__int64 nValue = 0;
			CWndEdit* pWndEditBet = (CWndComboBox*)GetDlgItem(WIDC_EDIT2);
			CAegonPvPMinigameBase* pGame = CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();

			try
			{
				nValue = strtoull(pWndEditBet->GetString(), NULL, 10);
			}
			catch (...)
			{
				nValue = 0;
				pWndEditBet->Empty();
				break;
			}
			if (nValue < 0) 
			{
				pWndEditBet->SetString("0");
			}
			else if (nValue > g_pPlayer->GetGold())
			{
				nValue = g_pPlayer->GetGold();
				CString strTmp;
				strTmp.Format("%I64d", nValue);
				pWndEditBet->SetString(strTmp.GetString());
				
			}

			if (pGame && IsValidObj(g_pPlayer)) 
			{
				for (int i = 0; i < pGame->m_vecsPlayers.size(); i++) 
				{
					if (pGame->m_vecsPlayers[i]->m_uPlayerID == g_pPlayer->m_idPlayer)
					{
						__int64 nCheckValue = pGame->m_vecsPlayers[i]->m_nMoneyBet;
						nCheckValue += nValue;
						if (nCheckValue > INT_MAX)
						{
							nValue = (__int64)INT_MAX - pGame->m_vecsPlayers[i]->m_nMoneyBet;
							CString strTmp;
							strTmp.Format("%I64d", nValue);
							pWndEditBet->SetString(strTmp.GetString());
						}
					}
				}
			}

		}
	}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

BOOL CWndAegonPvPGameMain::Initialize(CWndBase* pWndParent, DWORD)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PVP_MINIGAME_MAIN, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndAegonPvPGameMain::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	if (pWndBase == g_WndMng.m_pWndAegonPvPGameMessageBox)
	{
		switch (nID) 
		{
		case IDYES:
			QuitGame();
			break;
		case IDNO:
			break;
		}
	}
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndAegonPvPGameMain::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndAegonPvPGameMain::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndAegonPvPGameMain::OnLButtonDown(UINT nFlags, CPoint point)
{
}
void CWndAegonPvPGameMain::AdjustWindowForGame()
{
	CWndButton* pWndButtonCancel = (CWndButton*)GetDlgItem(WIDC_CANCEL);
	CWndText* pWndChatText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	CWndEdit* pWndChatEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	CWndButton* pWndButtonSend = (CWndButton*)GetDlgItem(WIDC_BUTTON2);

	CWndStatic* pWndStaticInvite = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	CWndComboBox* pWndComboInvite = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	CWndButton* pWndButtonInvite = (CWndButton*)GetDlgItem(WIDC_BUTTON1);

	CWndStatic* pWndStaticBet = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	CWndEdit* pWndEditBet = (CWndComboBox*)GetDlgItem(WIDC_EDIT2);
	CWndButton* pWndButtonBet = (CWndButton*)GetDlgItem(WIDC_BUTTON5);
	CWndButton* pWndButtonBetReset = (CWndButton*)GetDlgItem(WIDC_BUTTON6);


	CWndButton* pWndButtonKick = (CWndButton*)GetDlgItem(WIDC_BUTTON3);
	CWndButton* pWndButtonReady = (CWndButton*)GetDlgItem(WIDC_BUTTON4);

	CWndStatic* pWndStaticPot = (CWndStatic*)GetDlgItem(WIDC_STATIC6);

	CWndEdit* pWndEditPenyaPot = (CWndComboBox*)GetDlgItem(WIDC_EDIT3);

	CWndStatic* pWndStaticTurnCount = (CWndStatic*)GetDlgItem(WIDC_STATIC7);

	CWndStatic* pWndStaticLobby = (CWndStatic*)GetDlgItem(WIDC_STATIC8);
	CWndButton* pWndButtonLobbyToggle = (CWndButton*)GetDlgItem(WIDC_BUTTON7);

	CSize sizeGameField;
	switch (m_nGameID)
	{
	case AEG_MINIGAME_TICTACTOE:
	{
		CTexture* pTexGameField = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "tictactoe-field.png", COLOR_ITEM_BG, TRUE);
		if(pTexGameField)
			sizeGameField = pTexGameField->m_size;
		break;
	}
		
	case AEG_MINIGAME_CONNECTFOUR:
	{
		CTexture* pTexGameField = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "connectfour-bg-field.png", COLOR_ITEM_BG, TRUE);
		if(pTexGameField)
			sizeGameField = pTexGameField->m_size;
		break;
	}
	case AEG_MINIGAME_FLUNO:
		sizeGameField.cx = FLUNO_SIZE_X;
		sizeGameField.cy = FLUNO_SIZE_Y;
		break;
	}

	m_rectGameField.left = m_pWndPlayerList->GetWndRect().right + 16;
	m_rectGameField.top = pWndEditPenyaPot->GetWndRect().bottom + 16;
	m_rectGameField.right = m_rectGameField.left + sizeGameField.cx + 6;
	m_rectGameField.bottom = m_rectGameField.top + sizeGameField.cy + 6;
	
	nIncreasedWidth = m_rectGameField.Width() - 180;
	nIncreasedHeight = m_rectGameField.Height() - 157;

	CRect rectWnd = GetWndRect();
	rectWnd.right += nIncreasedWidth - 110;
	rectWnd.bottom += nIncreasedHeight;
	SetWndRect(rectWnd);

	if (pWndButtonKick)
	{
		CRect rectOld = pWndButtonKick->GetWndRect();
		CRect rectNew = rectOld;
		rectNew.bottom = rectWnd.bottom - 35;
		rectNew.top = rectNew.bottom - rectOld.Height();
		pWndButtonKick->SetWndRect(rectNew);
	}

	if (pWndButtonReady)
	{
		CRect rectOld = pWndButtonReady->GetWndRect();
		CRect rectNew = rectOld;
		rectNew.bottom = rectWnd.bottom - 35;
		rectNew.top = rectNew.bottom - rectOld.Height();
		pWndButtonReady->SetWndRect(rectNew);
	}

	if (pWndChatEdit)
	{
		CRect oldRect = pWndChatEdit->GetWndRect();
		CRect newRect = oldRect;
		newRect.left = m_pWndPlayerList->GetWndRect().right + 16;
		newRect.right = newRect.left + 100 + nIncreasedWidth;
		newRect.bottom = rectWnd.bottom - 35;
		newRect.top = newRect.bottom - oldRect.Height();
		pWndChatEdit->SetWndRect(newRect);

		if (pWndButtonSend)
		{
			CRect oldRectSend = pWndButtonSend->GetWndRect();
			CRect newRectButton = oldRectSend;
			newRectButton.left = newRect.right + 8;
			newRectButton.right = newRectButton.left + oldRectSend.Width();
			newRectButton.bottom = rectWnd.bottom - 35;
			newRectButton.top = newRectButton.bottom - oldRectSend.Height();
			pWndButtonSend->SetWndRect(newRectButton);
		}
	}
	if (pWndChatText)
	{
		CRect oldRect = pWndChatText->GetWndRect();
		CRect newRect = oldRect;
		newRect.left = m_pWndPlayerList->GetWndRect().right + 16;
		newRect.right = newRect.left + 180 + nIncreasedWidth;
		newRect.bottom = pWndChatEdit->GetWndRect().top;
		newRect.top = newRect.bottom - oldRect.Height();
		pWndChatText->SetWndRect(newRect);

	}

	if (pWndEditPenyaPot)
	{
		CRect oldRect = pWndEditPenyaPot->GetWndRect();
		CRect newRect = oldRect;

		int nSpace = (pWndChatText->GetWndRect().right + 8) - (m_pWndPlayerList->GetWndRect().right + 8);

		newRect.left = (m_pWndPlayerList->GetWndRect().right + 8) + (nSpace - oldRect.Width()) / 2;
		newRect.right = newRect.left + oldRect.Width();
		pWndEditPenyaPot->SetWndRect(newRect);
	}



	if (pWndStaticLobby)
	{
		CRect oldRect = pWndStaticLobby->GetWndRect();
		CRect newRect = oldRect;
		newRect.left = pWndButtonSend->GetWndRect().right + 24;
		newRect.right = newRect.left + oldRect.Width();
		newRect.top = pWndStaticPot->GetWndRect().top;
		newRect.bottom = newRect.top + oldRect.Height();
		pWndStaticLobby->SetWndRect(newRect, TRUE);
	}
	if (pWndButtonLobbyToggle)
	{
		CRect oldRect = pWndButtonLobbyToggle->GetWndRect();
		CRect newRect = oldRect;
		newRect.left = pWndStaticLobby->GetWndRect().left + (pWndStaticLobby->GetWndRect().Width() - oldRect.Width()) / 2;
		newRect.right = newRect.left + oldRect.Width();
		newRect.top = pWndStaticLobby->GetWndRect().bottom + 24;
		newRect.bottom = newRect.top + oldRect.Height();
		pWndButtonLobbyToggle->SetWndRect(newRect);
	}

	if (pWndStaticInvite)
	{
		CRect oldRect = pWndStaticInvite->GetWndRect();
		CRect newRect = oldRect;
		newRect.left = pWndButtonSend->GetWndRect().right + 24;
		newRect.right = newRect.left + oldRect.Width();
		newRect.top = pWndButtonLobbyToggle->GetWndRect().bottom + 24;
		newRect.bottom = newRect.top + oldRect.Height();
		pWndStaticInvite->SetWndRect(newRect, TRUE);
	}

	if (pWndComboInvite)
	{
		CRect oldRect = pWndComboInvite->GetWndRect();
		CRect newRect = oldRect;
		newRect.left = pWndStaticInvite->GetWndRect().left;
		newRect.right = newRect.left + oldRect.Width();
		newRect.top = pWndStaticInvite->GetWndRect().bottom + 6;
		newRect.bottom = newRect.top + oldRect.Height();
		pWndComboInvite->SetWndRect(newRect);
	}
	if (pWndButtonInvite)
	{
		CRect oldRect = pWndButtonInvite->GetWndRect();
		CRect newRect = oldRect;
		newRect.left = pWndComboInvite->GetWndRect().left + (pWndComboInvite->GetWndRect().Width() - oldRect.Width()) / 2;
		newRect.right = newRect.left + oldRect.Width();
		newRect.top = pWndComboInvite->GetWndRect().bottom + 6;
		newRect.bottom = newRect.top + oldRect.Height();
		pWndButtonInvite->SetWndRect(newRect);
	}

	if (pWndStaticBet)
	{
		CRect oldRect = pWndStaticBet->GetWndRect();
		CRect newRect = oldRect;
		newRect.left = pWndStaticInvite->GetWndRect().left;
		newRect.right = newRect.left + oldRect.Width();
		newRect.top = pWndButtonInvite->GetWndRect().bottom + 24;
		newRect.bottom = newRect.top + oldRect.Height();
		pWndStaticBet->SetWndRect(newRect, FALSE);
	}
	if (pWndEditBet)
	{
		CRect oldRect = pWndEditBet->GetWndRect();
		CRect newRect = oldRect;
		newRect.left = pWndStaticInvite->GetWndRect().left;
		newRect.right = newRect.left + oldRect.Width();
		newRect.top = pWndStaticBet->GetWndRect().bottom + 6;
		newRect.bottom = newRect.top + oldRect.Height();
		pWndEditBet->SetWndRect(newRect);

		pWndEditBet->AddWndStyle(EBS_NUMBER);
		pWndEditBet->m_wndScrollBar.SetVisible(FALSE);
	}
	if (pWndButtonBet)
	{
		CRect oldRect = pWndButtonBet->GetWndRect();
		CRect newRect = oldRect;
		newRect.left = pWndEditBet->GetWndRect().left + (pWndEditBet->GetWndRect().Width() - oldRect.Width()) / 2;
		newRect.right = newRect.left + oldRect.Width();
		newRect.top = pWndEditBet->GetWndRect().bottom + 6;
		newRect.bottom = newRect.top + oldRect.Height();
		pWndButtonBet->SetWndRect(newRect);
	}
	if (pWndButtonBetReset)
	{
		CRect oldRect = pWndButtonBetReset->GetWndRect();
		CRect newRect = pWndButtonBet->GetWndRect();;
		newRect.top = pWndButtonBet->GetWndRect().bottom + 6;
		newRect.bottom = newRect.top + oldRect.Height();
		pWndButtonBetReset->SetWndRect(newRect);
	}

	if (m_pWndPlayerList)
	{
		CRect newRectList = m_pWndPlayerList->GetWndRect();
		newRectList.bottom = pWndButtonReady->GetWndRect().top - 8;
		m_pWndPlayerList->SetWndRect(newRectList);
	}

	if (pWndButtonCancel)
	{
		CRect oldRect = pWndButtonCancel->GetWndRect();
		CRect newRectCancel = pWndButtonInvite->GetWndRect();

		newRectCancel.bottom = pWndButtonSend->GetWndRect().bottom;
		newRectCancel.top = newRectCancel.bottom - oldRect.Height();
		pWndButtonCancel->SetWndRect(newRectCancel);
	}

	if (pWndStaticPot)
	{
		CRect newRect = pWndStaticPot->GetWndRect();
		newRect.left = m_pWndPlayerList->GetWndRect().right + 8;
		newRect.right = pWndChatText->GetWndRect().right + 8;
		pWndStaticPot->SetWndRect(newRect, TRUE);
	}

	if (pWndStaticTurnCount)
	{
		CRect rectEditBet = pWndEditBet->GetWndRect();
		CRect rectResetButt = pWndButtonBetReset->GetWndRect();

		CRect oldRect = pWndStaticTurnCount->GetWndRect();
		CRect newRect = oldRect;

		newRect.left = rectEditBet.left;
		newRect.right = rectEditBet.right;
		newRect.top = rectResetButt.bottom + 24;
		newRect.bottom = newRect.top + oldRect.Height();

		pWndStaticTurnCount->SetWndRect(newRect, TRUE);
	}
	pWndComboInvite->SetTabStop(TRUE);
}
void CWndAegonPvPGameMain::SendChat() 
{
	CWndEdit* pWndChatEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);

	CString strMessage = pWndChatEdit->GetString();
	if (strMessage.GetLength() <= 0)
		return;

	g_DPlay.SendPvPMinigameChat(&strMessage);

	pWndChatEdit->Empty();
	pWndChatEdit->m_wndScrollBar.SetScrollPos(0);
}
void CWndAegonPvPGameMain::AddChatMessage(char* szMessage, BOOL bSystem)
{
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();

	CWndText* pWndChatText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	if(bSystem)
		pWndChatText->AddString(szMessage, COLOR_ERROR);
	else
		pWndChatText->AddString(szMessage, dwColorText);

	pWndChatText->m_wndScrollBar.SetScrollPos(pWndChatText->m_wndScrollBar.GetMaxScrollPos() - pWndChatText->m_wndScrollBar.GetScrollPage());
}
void CWndAegonPvPGameMain::QuitGame() 
{
	__MINIGAME_COMMAND sCommand;
	sCommand.dwCommandID = AEG_MINIGAME_COMMAND_QUIT;

	g_DPlay.SendPvPMinigameCommand(sCommand);
	Destroy();
}
void CWndAegonPvPGameMain::OnGameRefresh()
{
	if (m_pWndMinigame) 
	{
		m_pWndMinigame->OnGameRefresh();
	}
	SetButtons();
}
void CWndAegonPvPGameMain::SetButtons() 
{
	if (IsInvalidObj(g_pPlayer))
		return;

	CAegonPvPMinigameBase* pGame = CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	if (pGame->m_vecsPlayers.size() <= 0)
		return;

	CWndButton* pWndButtonCancel = (CWndButton*)GetDlgItem(WIDC_CANCEL);

	CWndButton* pWndButtonInvite = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	CWndButton* pWndButtonSendChat = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
	CWndButton* pWndButtonKick = (CWndButton*)GetDlgItem(WIDC_BUTTON3);
	CWndButton* pWndButtonReady = (CWndButton*)GetDlgItem(WIDC_BUTTON4);
	CWndButton* pWndButtonBet = (CWndButton*)GetDlgItem(WIDC_BUTTON5);
	CWndButton* pWndButtonBetReset = (CWndButton*)GetDlgItem(WIDC_BUTTON6);
	CWndButton* pWndButtonLobbyToggle = (CWndButton*)GetDlgItem(WIDC_BUTTON7);

	CWndComboBox* pWndComboInvite = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);

	CWndEdit* pWndEditBet = (CWndComboBox*)GetDlgItem(WIDC_EDIT2);

	CWndEdit* pWndChatEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);

	__MINIGAME_PLAYER* sPlayer = pGame->GetPlayerByID(g_pPlayer->m_idPlayer);

	BOOL bReady = FALSE;
	BOOL bLeader = pGame->m_vecsPlayers[0]->m_uPlayerID == g_pPlayer->m_idPlayer ? TRUE : FALSE;;

	if (sPlayer) 
	{
		bReady = sPlayer->m_bReady;
	}

	pWndButtonCancel->EnableWindow(TRUE);

	pWndButtonSendChat->EnableWindow(TRUE);

	pWndButtonKick->EnableWindow(FALSE);

	pWndButtonLobbyToggle->EnableWindow(FALSE);

	if (pGame->m_eGameState == GameState::state_preparation) 
	{
		pWndButtonInvite->EnableWindow(TRUE);
		pWndComboInvite->EnableWindow(TRUE);

		if (bLeader) 
		{
			pWndButtonKick->EnableWindow(TRUE);
			pWndButtonLobbyToggle->EnableWindow(TRUE);
		}

		pWndButtonReady->EnableWindow(TRUE);

		pWndButtonBet->EnableWindow(TRUE);
		pWndEditBet->EnableWindow(TRUE);

		pWndButtonBetReset->EnableWindow(TRUE);

		if (bReady) 
		{
			pWndButtonInvite->EnableWindow(FALSE);
			pWndComboInvite->EnableWindow(FALSE);

			pWndButtonBet->EnableWindow(FALSE);
			pWndEditBet->EnableWindow(FALSE);
		}
	}
	else 
	{
		pWndButtonInvite->EnableWindow(FALSE);
		pWndComboInvite->EnableWindow(FALSE);

		pWndButtonReady->EnableWindow(FALSE);

		pWndButtonBet->EnableWindow(FALSE);
		pWndEditBet->EnableWindow(FALSE);

		pWndButtonBetReset->EnableWindow(FALSE);
	}
}
///////////////////////////
///////LIST WINDOW/////////
///////////////////////////
CWndMinigamePlayerList::CWndMinigamePlayerList()
{
	m_nRowHeight = 40;
	m_nSelectedItem = -1;
}
CWndMinigamePlayerList::~CWndMinigamePlayerList()
{
}
void CWndMinigamePlayerList::OnDraw(C2DRender* p2DRender)
{
	CAegonPvPMinigameBase* pGame = CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	CRect wndRect = GetWindowRect();
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	CPoint point = CPoint(0, 0);
	CString str;

	int nRange = 0;
	int nPage = wndRect.Height() / m_nRowHeight;

	nRange = pGame->m_vecsPlayers.size();

	if (!nRange)
		nRange = 1;

	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);
	m_wndScrollBar.EnableScrollBar(TRUE);
	m_wndScrollBar.SetScrollStep(1);

	int nIndex = 0;
	for (int i = m_wndScrollBar.GetScrollPos(); i < pGame->m_vecsPlayers.size(); i++, nIndex++)
	{
		point.x = 5;
		point.y = nIndex * m_nRowHeight;

		if (i > m_wndScrollBar.GetScrollPos() + nPage)
			break;


		if (point.y + m_nRowHeight >= m_nRowHeight)
			p2DRender->RenderLine(CPoint(0, point.y + m_nRowHeight), CPoint(wndRect.Width() - 30, point.y + m_nRowHeight), dwColorText);

#ifdef __AEGON_THEME_SWITCHER
		DWORD RenderFillRect = CAegonThemeSwitcherMng::GetInstance()->GetRenderFillRectColor();
		DWORD RenderRect = CAegonThemeSwitcherMng::GetInstance()->GetRenderRectColor();
#endif //__AEGON_THEME_SWITCHER

		if (m_nSelectedItem == i)
		{
			CRect rectSelected = CRect(0, point.y, wndRect.right - 30, point.y + m_nRowHeight);
#ifdef __AEGON_THEME_SWITCHER
			p2DRender->RenderFillRect(rectSelected, RenderFillRect);
			p2DRender->RenderRect(rectSelected, RenderRect);
#else //__AEGON_THEME_SWITCHER
			p2DRender->RenderFillRect(rectSelected, 0xFFf8e6cc);
			p2DRender->RenderRect(rectSelected, 0xFFf28818);
#endif //__AEGON_THEME_SWITCHER
		}


		int nMaxLength = 90 - point.x - p2DRender->GetFont()->GetTextExtent("...").cx;
		str.Format("%s", pGame->m_vecsPlayers[i]->m_szName);
		CSize strExtent = p2DRender->GetFont()->GetTextExtent(str.GetString());
		if (strExtent.cx > nMaxLength)
		{
			LONG letterLength = p2DRender->GetFont()->GetTextExtent("W").cx;
			int nMaxLetters = 0;
			if (letterLength != 0)
				nMaxLetters = floor((double)nMaxLength / (double)letterLength);

			if (nMaxLetters > str.GetLength())
				nMaxLetters = str.GetLength();

			str = str.Mid(0, nMaxLetters);
			str += "...";
		}
		strExtent = p2DRender->GetFont()->GetTextExtent(str.GetString());
		p2DRender->TextOut(point.x, point.y + (m_nRowHeight / 2) - (strExtent.cy / 2), str.GetString(), dwColorText);

		str.Format("%d", pGame->m_vecsPlayers[i]->m_nMoneyBet);
		str = GetNumberFormatEx(str);
		strExtent = p2DRender->GetFont()->GetTextExtent(str.GetString());
		p2DRender->TextOut(point.x + 85, point.y + (m_nRowHeight / 2) - (strExtent.cy / 2), str.GetString(), dwColorText);

		CAnimatedTexture* pTextureStatus = NULL;
		switch (pGame->m_eGameState) 
		{
		case  GameState::state_preparation:
		{
			if (pGame->m_vecsPlayers[i]->m_bReady)
			{
				pTextureStatus = m_animatedTextureMng.AddAnimatedTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME_APNG, "Ready_Icon.png"));
			}
			else 
			{
				pTextureStatus = m_animatedTextureMng.AddAnimatedTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME_APNG, "NotReady_Icon.png"));
			}
			break;
		}
		case  GameState::state_in_progress:
		{
			if (pGame->m_vecsPlayers[i]->m_uPlayerID == pGame->m_uTurn)
			{
				pTextureStatus = m_animatedTextureMng.AddAnimatedTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME_APNG, "Zug_Icon.png"));
			}
			else if (easy_find(pGame->m_vecWinners, pGame->m_vecsPlayers[i]->m_uPlayerID))
			{
				pTextureStatus = m_animatedTextureMng.AddAnimatedTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME_APNG, "Winner_Icon.png"));
			}
			else 
			{
				pTextureStatus = NULL;
			}
			break;
		}
		case  GameState::state_game_over:
		{
			if (pGame->m_vecWinners.size() > 0 && pGame->m_vecWinners[0] == pGame->m_vecsPlayers[i]->m_uPlayerID)
			{
				pTextureStatus = m_animatedTextureMng.AddAnimatedTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME_APNG, "Winner_Icon.png"));
			}
			else 
			{
				pTextureStatus = NULL;
			}
			break;
		}
		default:
			pTextureStatus = NULL;
			break;
		}
		if (pTextureStatus)
		{
			pTextureStatus->Render(p2DRender, CPoint(point.x + 175, point.y + (m_nRowHeight / 2) - (pTextureStatus->GetSize().cy / 2)));
		}

	}


}
void CWndMinigamePlayerList::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	CRect rect = GetWindowRect();

	m_wndScrollBar.Create(WBS_DOCKING | WBS_VERT, rect, this, 1000);
	m_wndScrollBar.SetVisible(IsWndStyle(WBS_VSCROLL));

}
BOOL CWndMinigamePlayerList::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndBase::OnChildNotify(message, nID, pLResult);
}
BOOL CWndMinigamePlayerList::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndBase::OnCommand(nID, dwMessage, pWndBase);
}
void CWndMinigamePlayerList::OnLButtonUp(UINT nFlags, CPoint point)
{

}
void CWndMinigamePlayerList::OnLButtonDown(UINT nFlags, CPoint point)
{
	CAegonPvPMinigameBase* pGame = CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	SetSelectedItem(point);

	if (m_nSelectedItem < 0 || m_nSelectedItem >= pGame->m_vecsPlayers.size())
		return;

	CWndAegonPvPGameMain* pParentWnd = (CWndAegonPvPGameMain*)GetParentWnd();
	if (pParentWnd)
	{
	}
}

void CWndMinigamePlayerList::OnMouseWndSurface(CPoint point)
{
}
void CWndMinigamePlayerList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}
void CWndMinigamePlayerList::OnRButtonUp(UINT nFlags, CPoint point)
{
	SetSelectedItem(point);
}
void CWndMinigamePlayerList::SetSelectedItem(CPoint point)
{
	CAegonPvPMinigameBase* pGame = CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	int nScrollPos = m_wndScrollBar.GetScrollPos();
	CRect wndRect = GetWndRect();
	int nRowCount = ceil((float)wndRect.Height() / (float)m_nRowHeight);

	for (int i = 0; i < nRowCount; i++)
	{
		if (nScrollPos + i >= pGame->m_vecsPlayers.size())
			break;

		CRect tmpRect;
		tmpRect.left = 0;
		tmpRect.top = 0 + i * m_nRowHeight;
		tmpRect.right = wndRect.Width() - 30;
		tmpRect.bottom = tmpRect.top + m_nRowHeight;

		if (tmpRect.PtInRect(point))
		{
			m_nSelectedItem = nScrollPos + i;
			return;
		}
	}
	m_nSelectedItem = -1;
}

///////////////////////////
//////Minigame Base////////
///////////////////////////
CWndPvPMinigameBase::CWndPvPMinigameBase()
{
	m_nRenderHoverMove = -1;
	m_nClickMove = -1;
	m_bUpdateInProgress = FALSE;
	m_pUpdateGame = NULL;
	m_pAnimation = NULL;
}
CWndPvPMinigameBase::~CWndPvPMinigameBase()
{
	SAFE_DELETE(m_pAnimation);
}
void CWndPvPMinigameBase::Create(DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	CWndBase::Create(WBS_CHILD, rect, pParentWnd, nID);
}
void CWndPvPMinigameBase::OnInitialUpdate()
{
}
void CWndPvPMinigameBase::OnDraw(C2DRender* p2DRender)
{
}
BOOL CWndPvPMinigameBase::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndBase::OnChildNotify(message, nID, pLResult);
}
void CWndPvPMinigameBase::OnMouseWndSurface(CPoint point)
{
}
void CWndPvPMinigameBase::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndPvPMinigameBase::OnLButtonDown(UINT nFlags, CPoint point)
{
}
void CWndPvPMinigameBase::OnSize(UINT nType, int cx, int cy)
{
}
void CWndPvPMinigameBase::PaintFrame(C2DRender* p2DRender)
{
}
void CWndPvPMinigameBase::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect(3, 3);

	if (bOnSize)
		OnSize(0, m_rectClient.Width(), m_rectClient.Height());
}
void CWndPvPMinigameBase::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}
BOOL CWndPvPMinigameBase::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndBase::OnCommand(nID, dwMessage, pWndBase);
}
void CWndPvPMinigameBase::OnGameRefresh()
{
	CAegonPvPMinigameBase* pGame = CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	if (IsInvalidObj(g_pPlayer))
		return;

	if (pGame->m_uTurn != g_pPlayer->m_idPlayer)
		m_nClickMove = -1;
}
void CWndPvPMinigameBase::UpdateGameState()
{
	if (m_bUpdateInProgress || m_pUpdateGame)
		return;

	CAegonPvPMinigameBase* pGame = CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	if (!g_WndMng.m_pWndAegonPvPGameMain)
		return;

	if (g_WndMng.m_pWndAegonPvPGameMain->m_vGameUpdateState.size() <= 0)
		return;

	m_bUpdateInProgress = TRUE;

	m_pUpdateGame = g_WndMng.m_pWndAegonPvPGameMain->m_vGameUpdateState[0];
	g_WndMng.m_pWndAegonPvPGameMain->m_vGameUpdateState.erase(g_WndMng.m_pWndAegonPvPGameMain->m_vGameUpdateState.begin());

	switch (m_pUpdateGame->m_nUpdateMode)
	{
	case AEG_MINIGAME_UPDATE_INVITE:
	case AEG_MINIGAME_UPDATE_TOGGLE:
	case AEG_MINIGAME_UPDATE_JOIN:
	case AEG_MINIGAME_UPDATE_BET:
	case AEG_MINIGAME_UPDATE_RESET:
	case AEG_MINIGAME_UPDATE_READY:
	case AEG_MINIGAME_UPDATE_KICK:
	case AEG_MINIGAME_UPDATE_START:
	case AEG_MINIGAME_UPDATE_QUIT:
	case AEG_MINIGAME_UPDATE_END:
		OnBaseAni();
		return;
	case AEG_MINIGAME_UPDATE_MOVE:
		OnGameSpecificAni();
		return;
	}

	UpdateCurrentGame();
}
void CWndPvPMinigameBase::UpdateCurrentGame()
{
	CAegonPvPMinigameBase* pGame = CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();

	if (!m_bUpdateInProgress)
		return;

	if (!m_pUpdateGame) 
	{
		m_bUpdateInProgress = FALSE;
		return;
	}

	CAegonPvPMinigamesMng::GetInstance()->SetActiveGame(m_pUpdateGame);
	SAFE_DELETE(pGame);
	m_pUpdateGame = NULL;
	m_bUpdateInProgress = FALSE;

	if (g_WndMng.m_pWndAegonPvPGameMain) 
		g_WndMng.m_pWndAegonPvPGameMain->SetButtons();
}
void CWndPvPMinigameBase::OnBaseAni()
{
	if (!m_pUpdateGame) 
	{
		m_bUpdateInProgress = FALSE;
		return;
	}

	//TODO base anis
	/*switch (m_pUpdateGame->m_nUpdateMode) 
	{
	case AEG_MINIGAME_UPDATE_INVITE:
		UpdateCurrentGame();
		return;
	case AEG_MINIGAME_UPDATE_TOGGLE:
		UpdateCurrentGame();
		return;
	case AEG_MINIGAME_UPDATE_JOIN:
		UpdateCurrentGame();
		return;
	case AEG_MINIGAME_UPDATE_BET:
		UpdateCurrentGame();
		return;
	case AEG_MINIGAME_UPDATE_RESET:
		UpdateCurrentGame();
		return;
	case AEG_MINIGAME_UPDATE_READY:
		UpdateCurrentGame();
		return;
	case AEG_MINIGAME_UPDATE_KICK:
		UpdateCurrentGame();
		return;
	case AEG_MINIGAME_UPDATE_START:
		UpdateCurrentGame();
		return;
	case AEG_MINIGAME_UPDATE_QUIT:
		UpdateCurrentGame();
		return;
	case AEG_MINIGAME_UPDATE_END:
		UpdateCurrentGame();
		return;
	}*/

	UpdateCurrentGame();
}
void CWndPvPMinigameBase::OnGameSpecificAni()
{
	UpdateCurrentGame();
}
void CWndPvPMinigameBase::ProcessAni(C2DRender* p2DRender, int nPos)
{
	if (g_WndMng.m_pWndAegonPvPGameMain->m_vGameUpdateState.size() > 0)
		UpdateGameState();

	if (m_pAnimation)
	{
		if (m_pAnimation->m_bEnded || GetTickCount() > m_pAnimation->m_dwStartTime + __MINIGAME_MAX_ANI_TIME)
		{	
			m_pAnimation->RenderAni(p2DRender, nPos);
			if (m_pAnimation->m_bDeleteable)
			{
				SAFE_DELETE(m_pAnimation);
			}
			else
			{
				UpdateCurrentGame();
				m_pAnimation->m_bDeleteable = TRUE;
				OnAniEnd();
			}
		}
		else
		{
			m_pAnimation->DoAniStep();
			m_pAnimation->RenderAni(p2DRender, nPos);
		}
	}
}
void CWndPvPMinigameBase::OnAniEnd()
{
}
#ifdef __AEGON_THEME_SWITCHER
void CWndPvPMinigameBase::OnThemeSelectionChanged()
{
	CWndBase::OnThemeSelectionChanged();
}
#endif

///////////////////////////
///////Minigame TTT////////
///////////////////////////
CWndPvPMinigameTikTakToe::CWndPvPMinigameTikTakToe()
{
	m_pTextureField = NULL;
	m_pTextureMoveMe = NULL;
	m_pTextureMoveEnemy = NULL;
}
CWndPvPMinigameTikTakToe::~CWndPvPMinigameTikTakToe()
{
	SAFE_DELETE(m_pAnimation);
}
void CWndPvPMinigameTikTakToe::OnInitialUpdate()
{
	m_pTextureField = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "tictactoe-field.png", COLOR_ITEM_BG, TRUE);
	m_pTextureMoveMe = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "tictactoe-move-me.png", COLOR_ITEM_BG, TRUE);
	m_pTextureMoveEnemy = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "tictactoe-move-enemy.png", COLOR_ITEM_BG, TRUE);



	//includes texture size so m_pTextureField can be scaled down/up
	m_pRectFields[0] = CRect(0,		0,		63,		63);
	m_pRectFields[1] = CRect(68,	0,		133,	63);
	m_pRectFields[2] = CRect(138,	0,		200,	63);
	m_pRectFields[3] = CRect(0,		68,		63,		133);
	m_pRectFields[4] = CRect(68,	68,		133,	133);
	m_pRectFields[5] = CRect(138,	68,		200,	133);
	m_pRectFields[6] = CRect(0,		138,	63,		200);
	m_pRectFields[7] = CRect(68,	138,	133,	200);
	m_pRectFields[8] = CRect(138,	138,	200,	200);

}
void CWndPvPMinigameTikTakToe::OnDraw(C2DRender* p2DRender)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	CAegonPvPMinigameTicTacToe* pGame = (CAegonPvPMinigameTicTacToe*)CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	if (m_pTextureField) 
	{
		m_pTextureField->Render(p2DRender, CPoint(0, 0));
	}

	for (int i = 0; i < 9; i++)
	{
		CPoint pt = m_pRectFields[i].CenterPoint();

		int nFieldValue = pGame->m_nTicTacToeField[i];
		if (nFieldValue == 0) 
		{
			//free field
		}
		else if (nFieldValue == g_pPlayer->m_idPlayer)
		{
			//X
			if (m_pTextureMoveMe) 
			{
				pt.x -= m_pTextureMoveMe->m_size.cx / 2;
				pt.y -= m_pTextureMoveMe->m_size.cy / 2;

				m_pTextureMoveMe->Render(p2DRender, pt);
			}
		}
		else 
		{
			//O
			if (m_pTextureMoveEnemy) 
			{
				pt.x -= m_pTextureMoveEnemy->m_size.cx / 2;
				pt.y -= m_pTextureMoveEnemy->m_size.cy / 2;
				m_pTextureMoveEnemy->Render(p2DRender, pt);
			}
		}
	}
	if (m_nRenderHoverMove != -1 && g_pPlayer->m_idPlayer == pGame->m_uTurn)
	{
		if (m_nRenderHoverMove >= 0 && m_nRenderHoverMove < 9)
		{
			if (m_pTextureMoveMe)
			{ 
				CPoint pt = m_pRectFields[m_nRenderHoverMove].CenterPoint();
				pt.x -= m_pTextureMoveMe->m_size.cx / 2;
				pt.y -= m_pTextureMoveMe->m_size.cy / 2;
				m_pTextureMoveMe->Render(p2DRender, pt, 150);
			}
		}
	}
}
BOOL CWndPvPMinigameTikTakToe::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndBase::OnChildNotify(message, nID, pLResult);
}
void CWndPvPMinigameTikTakToe::OnMouseWndSurface(CPoint point)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	CAegonPvPMinigameTicTacToe* pGame = (CAegonPvPMinigameTicTacToe*)CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	if (pGame->m_uTurn != g_pPlayer->m_idPlayer) 
	{
		m_nRenderHoverMove = -1;
		return;
	}

	for (int i = 0; i < 9; i++)
	{
		if (pGame->m_nTicTacToeField[i] != 0)
			continue;

		if (m_pRectFields[i].PtInRect(point))
		{
			m_nRenderHoverMove = i;
			return;
		}
	}
	m_nRenderHoverMove = -1;
}
void CWndPvPMinigameTikTakToe::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	CAegonPvPMinigameTicTacToe* pGame = (CAegonPvPMinigameTicTacToe*)CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	if (pGame->m_uTurn != g_pPlayer->m_idPlayer)
		return;

	for (int i = 0; i < 9; i++)
	{
		if (m_pRectFields[i].PtInRect(point))
		{
			if (m_nClickMove == i) 
			{
				__MINIGAME_COMMAND tmpCommand;
				tmpCommand.dwCommandID = AEG_MINIGAME_COMMAND_MOVE;
				tmpCommand.dwCommandValue = i;

				g_DPlay.SendPvPMinigameCommand(tmpCommand);
				m_nRenderHoverMove = -1;
			}
			m_nClickMove = -1;
			return;
		}
	}
	m_nClickMove = -1;
}
void CWndPvPMinigameTikTakToe::OnLButtonDown(UINT nFlags, CPoint point)
{
	CAegonPvPMinigameTicTacToe* pGame = (CAegonPvPMinigameTicTacToe*)CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	for (int i = 0; i < 9; i++)
	{
		if (m_pRectFields[i].PtInRect(point))
		{
			m_nClickMove = i;
			return;
		}
	}
	m_nClickMove = -1;
}
void CWndPvPMinigameTikTakToe::OnGameRefresh()
{
	CWndPvPMinigameBase::OnGameRefresh();

	CAegonPvPMinigameTicTacToe* pGame = (CAegonPvPMinigameTicTacToe*)CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	if (IsInvalidObj(g_pPlayer))
		return;

}
void CWndPvPMinigameTikTakToe::OnBaseAni()
{
	if (!m_pUpdateGame)
	{
		m_bUpdateInProgress = FALSE;
		return;
	}

	//override base anis here
	switch (m_pUpdateGame->m_nUpdateMode)
	{
	default:
		break;
	}

	CWndPvPMinigameBase::OnBaseAni();
}
void CWndPvPMinigameTikTakToe::OnGameSpecificAni()
{
	UpdateCurrentGame();
}
#ifdef __AEGON_THEME_SWITCHER
void CWndPvPMinigameTikTakToe::OnThemeSelectionChanged()
{
	CWndPvPMinigameBase::OnThemeSelectionChanged();

	m_pTextureField = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "tictactoe-field.png", COLOR_ITEM_BG, TRUE);
	m_pTextureMoveMe = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "tictactoe-move-me.png", COLOR_ITEM_BG, TRUE);
	m_pTextureMoveEnemy = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "tictactoe-move-enemy.png", COLOR_ITEM_BG, TRUE);
}
#endif
///////////////////////////
///////Minigame C4/////////
///////////////////////////
CWndPvPMinigameConnectFour::CWndPvPMinigameConnectFour()
{
	m_pTextureBGField = NULL;
	m_pTextureFGField = NULL;
	m_pTextureRedPiece = NULL;
	m_pTextureYellowPiece = NULL;
}
CWndPvPMinigameConnectFour::~CWndPvPMinigameConnectFour()
{
	SAFE_DELETE(m_pAnimation);
}
void CWndPvPMinigameConnectFour::OnInitialUpdate()
{
	m_pTextureBGField = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "connectfour-bg-field.png", COLOR_ITEM_BG, TRUE);
	m_pTextureFGField = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "connectfour-fg-field.png", COLOR_ITEM_BG, TRUE);

	m_pTextureRedPiece = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "connectfour-red.png", COLOR_ITEM_BG, TRUE);
	m_pTextureYellowPiece = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "connectfour-yellow.png", COLOR_ITEM_BG, TRUE);

	CalculateRects();
}
void CWndPvPMinigameConnectFour::OnDraw(C2DRender* p2DRender)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	CAegonPvPMinigameConnectFour* pGame = (CAegonPvPMinigameConnectFour*)CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	if (!g_WndMng.m_pWndAegonPvPGameMain)
		return;

	//background
	m_pTextureBGField->Render(p2DRender, CPoint(0, 0));

	for (int i = 0; i < 6; i++) 
	{
		for (int j = 0; j < 7; j++) 
		{
			if (pGame->m_nConnectFourField[i][j] == 0) 
			{
				//free space
			}
			else if (pGame->m_nConnectFourField[i][j] == g_pPlayer->m_idPlayer)
			{
				//render my piece
				m_pTextureRedPiece->Render(p2DRender, CPoint(m_rectHoles[i][j].CenterPoint().x - (m_pTextureRedPiece->m_size.cx / 2), m_rectHoles[i][j].CenterPoint().y - (m_pTextureRedPiece->m_size.cy / 2)));
			}
			else
			{
				//render enemy piece
				m_pTextureYellowPiece->Render(p2DRender, CPoint(m_rectHoles[i][j].CenterPoint().x - (m_pTextureYellowPiece->m_size.cx / 2), m_rectHoles[i][j].CenterPoint().y - (m_pTextureYellowPiece->m_size.cy / 2)));
			}
		}
	}

	//hover move
	if (m_nRenderHoverMove != -1 && g_pPlayer->m_idPlayer == pGame->m_uTurn)
	{
		if (m_nRenderHoverMove >= 0 && m_nRenderHoverMove < 7)
		{
			if (m_pTextureRedPiece)
			{
				CPoint pt = CPoint(m_rectHoles[0][m_nRenderHoverMove].CenterPoint().x - (m_pTextureRedPiece->m_size.cx / 2), m_rectHoles[0][m_nRenderHoverMove].CenterPoint().y - m_pTextureRedPiece->m_size.cy);
				m_pTextureRedPiece->Render(p2DRender, pt);
			}
		}
	}

	ProcessAni(p2DRender, 0);
		
	//foreground
	m_pTextureFGField->Render(p2DRender, CPoint(0, 0));
}
BOOL CWndPvPMinigameConnectFour::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndBase::OnChildNotify(message, nID, pLResult);
}
void CWndPvPMinigameConnectFour::OnMouseWndSurface(CPoint point)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	if (m_pAnimation)
		return;

	CAegonPvPMinigameConnectFour* pGame = (CAegonPvPMinigameConnectFour*)CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	if (pGame->m_uTurn != g_pPlayer->m_idPlayer)
	{
		m_nRenderHoverMove = -1;
		return;
	}

	for (int i = 0; i < 7; i++)
	{
		if (pGame->m_nConnectFourField[0][i] != 0)
			continue;

		if (m_rectGameColumns[i].PtInRect(point))
		{
			m_nRenderHoverMove = i;
			return;
		}
	}
	m_nRenderHoverMove = -1;
}
void CWndPvPMinigameConnectFour::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	CAegonPvPMinigameConnectFour* pGame = (CAegonPvPMinigameConnectFour*)CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	if (pGame->m_uTurn != g_pPlayer->m_idPlayer)
		return;

	for (int i = 0; i < 7; i++) 
	{
		if (m_rectGameColumns[i].PtInRect(point))
		{
			if (m_nClickMove == i)
			{
				__MINIGAME_COMMAND tmpCommand;
				tmpCommand.dwCommandID = AEG_MINIGAME_COMMAND_MOVE;
				tmpCommand.dwCommandValue = i;

				g_DPlay.SendPvPMinigameCommand(tmpCommand);
				m_nRenderHoverMove = -1;
			}
			m_nClickMove = -1;
			return;
		}
	}
	
	m_nClickMove = -1;
}
void CWndPvPMinigameConnectFour::OnLButtonDown(UINT nFlags, CPoint point)
{
	CAegonPvPMinigameConnectFour* pGame = (CAegonPvPMinigameConnectFour*)CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	for (int i = 0; i < 7; i++)
	{
		if (m_rectGameColumns[i].PtInRect(point))
		{
			m_nClickMove = i;
			return;
		}
	}
	m_nClickMove = -1;
}
void CWndPvPMinigameConnectFour::OnGameRefresh()
{
	CWndPvPMinigameBase::OnGameRefresh();

	CAegonPvPMinigameConnectFour* pGame = (CAegonPvPMinigameConnectFour*)CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	if (IsInvalidObj(g_pPlayer))
		return;
}
void CWndPvPMinigameConnectFour::OnBaseAni()
{
	if (!m_pUpdateGame)
	{
		m_bUpdateInProgress = FALSE;
		return;
	}

	//override base anis here
	switch (m_pUpdateGame->m_nUpdateMode)
	{
	default:
		break;
	}

	CWndPvPMinigameBase::OnBaseAni();
}
void CWndPvPMinigameConnectFour::OnGameSpecificAni()
{
	CAegonPvPMinigameConnectFour* pGame = (CAegonPvPMinigameConnectFour*)CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame) 
	{
		UpdateCurrentGame();
		return;
	}

	if (!m_pUpdateGame) 
	{
		m_bUpdateInProgress = FALSE;
		return;
	}

	switch(m_pUpdateGame->m_nUpdateMode)
	{
	case AEG_MINIGAME_UPDATE_MOVE:
	{
		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 7; j++)
			{
				if (pGame->m_nConnectFourField[i][j] == 0 && ((CAegonPvPMinigameConnectFour*)m_pUpdateGame)->m_nConnectFourField[i][j] != 0) 
				{
					SAFE_DELETE(m_pAnimation);
					m_pAnimation = new CMinigameAnimationC4Move;
					m_pAnimation->SetParent(this);
					((CMinigameAnimationC4Move*)m_pAnimation)->SetValues(i, j, pGame->m_uTurn);

					m_nRenderHoverMove = -1;
					m_pAnimation->Initialize();
					return;
				}
			}
		}
		break;
	}
	}
	UpdateCurrentGame();
}
void CWndPvPMinigameConnectFour::CalculateRects()
{
	int nHoledWidth = 42;
	int nHoleHeight = 41;

	for (int i = 0; i < 7; i++)
	{
		CRect rect;
		rect.left = 4 + i * (6 + nHoledWidth);
		rect.top = 0;
		rect.right = rect.left + nHoledWidth;
		rect.bottom = m_pTextureFGField->m_size.cy;

		m_rectGameColumns[i] = rect;
	}

	for (int i = 0; i < 6; i++)
	{
		CRect rect;
		rect.left = 0;

		rect.top = 26 + i * (7 + nHoleHeight);
		rect.right = m_pTextureFGField->m_size.cx;
		rect.bottom = rect.top + nHoleHeight;

		m_rectGameRows[i] = rect;
	}

	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			CRect rectHole = m_rectGameRows[i];
			rectHole.left = m_rectGameColumns[j].left;
			rectHole.right = m_rectGameColumns[j].right;

			m_rectHoles[i][j] = rectHole;
		}
	}
}
#ifdef __AEGON_THEME_SWITCHER
void CWndPvPMinigameConnectFour::OnThemeSelectionChanged()
{
	CWndPvPMinigameBase::OnThemeSelectionChanged();

	m_pTextureBGField = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "connectfour-bg-field.png", COLOR_ITEM_BG, TRUE);
	m_pTextureFGField = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "connectfour-fg-field.png", COLOR_ITEM_BG, TRUE);

	m_pTextureRedPiece = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "connectfour-red.png", COLOR_ITEM_BG, TRUE);
	m_pTextureYellowPiece = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "connectfour-yellow.png", COLOR_ITEM_BG, TRUE);

	CalculateRects();
}
#endif
///////////////////////////
//////Minigame Fluno///////
///////////////////////////
CWndPvPMinigameFluno::CWndPvPMinigameFluno()
{
	m_pTextureCardBackside = NULL;

	for (int i = 0; i < 4; i++) 
	{
		m_pTextureColorPicker[i] = NULL;
		m_nColorPickerOffsetY[i] = 0;
	}

	m_nNoDrawOwn = -1;
}
CWndPvPMinigameFluno::~CWndPvPMinigameFluno()
{
	SAFE_DELETE(m_pAnimation);
}
void CWndPvPMinigameFluno::OnInitialUpdate()
{
	m_pTextureCardBackside = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME_MINIGAMES_UNO, "Uno_Backside.png"), COLOR_ITEM_BG, TRUE);

	m_pTextureColorPicker[0] = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME_MINIGAMES_UNO, "uno_green.png"), COLOR_ITEM_BG, TRUE);
	m_pTextureColorPicker[0]->m_size.cx = ceil(404.f / 4.f);
	m_pTextureColorPicker[0]->m_size.cy = ceil(256.f / 4.f);

	m_pTextureColorPicker[1] = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME_MINIGAMES_UNO, "uno_blue.png"), COLOR_ITEM_BG, TRUE);
	m_pTextureColorPicker[1]->m_size.cx = ceil(286.f / 4.f);
	m_pTextureColorPicker[1]->m_size.cy = ceil(339.f / 4.f);

	m_pTextureColorPicker[2] = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME_MINIGAMES_UNO, "uno_yellow.png"), COLOR_ITEM_BG, TRUE);
	m_pTextureColorPicker[2]->m_size.cx = ceil(286.f / 4.f);
	m_pTextureColorPicker[2]->m_size.cy = ceil(339.f / 4.f);

	m_pTextureColorPicker[3] = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME_MINIGAMES_UNO, "uno_red.png"), COLOR_ITEM_BG, TRUE);
	m_pTextureColorPicker[3]->m_size.cx = ceil(404.f / 4.f);
	m_pTextureColorPicker[3]->m_size.cy = ceil(256.f / 4.f);


	

	m_pointTakeStack = CPoint(FLUNO_SIZE_X - m_pTextureCardBackside->m_size.cy / 2 - 20, 0); //center of take stack
	m_pointPutStack = CPoint(FLUNO_SIZE_X / 2, FLUNO_SIZE_Y / 2); // center of put stack

	//player field
	m_rectPlayerFieldMe.left = 0;
	m_rectPlayerFieldMe.top = m_pointPutStack.y + m_pTextureCardBackside->m_size.cy / 2 + 20;
	m_rectPlayerFieldMe.right = m_pointTakeStack.x - m_pTextureCardBackside->m_size.cy / 2 - 20;
	m_rectPlayerFieldMe.bottom = FLUNO_SIZE_Y;

	m_pointTakeStack.y = m_rectPlayerFieldMe.CenterPoint().y;

	CSize sizePlayerField = CSize(150, 100);

	//top player
	m_rectPlayerFields[1].left = FLUNO_SIZE_X / 2 - sizePlayerField.cx / 2;
	m_rectPlayerFields[1].top = 20;
	m_rectPlayerFields[1].right = FLUNO_SIZE_X / 2 + sizePlayerField.cx / 2;
	m_rectPlayerFields[1].bottom = m_rectPlayerFields[1].top + sizePlayerField.cy;

	//left player
	m_rectPlayerFields[0].left = 0;
	m_rectPlayerFields[0].top = m_rectPlayerFields[1].bottom - 20;
	m_rectPlayerFields[0].right = m_rectPlayerFields[0].left + sizePlayerField.cx;
	m_rectPlayerFields[0].bottom = m_rectPlayerFields[0].top + sizePlayerField.cy;

	//right player
	m_rectPlayerFields[2].left = FLUNO_SIZE_X - sizePlayerField.cx;
	m_rectPlayerFields[2].top = m_rectPlayerFields[1].bottom - 20;
	m_rectPlayerFields[2].right = FLUNO_SIZE_X;
	m_rectPlayerFields[2].bottom = m_rectPlayerFields[2].top + sizePlayerField.cy;

	m_rectTakeStack.left = m_pointTakeStack.x - m_pTextureCardBackside->m_size.cy / 2 - 10;
	m_rectTakeStack.top = m_pointTakeStack.y - m_pTextureCardBackside->m_size.cy / 2 - 30;
	m_rectTakeStack.right = m_pointTakeStack.x + m_pTextureCardBackside->m_size.cy / 2 + 10;
	m_rectTakeStack.bottom = m_pointTakeStack.y + m_pTextureCardBackside->m_size.cy / 2 + 10;

	CPoint ptColorPickerCenter = m_pointPutStack;
	ptColorPickerCenter.y -= 20;

	m_rectColorPicker[0].left = ptColorPickerCenter.x - m_pTextureColorPicker[0]->m_size.cx / 2;
	m_rectColorPicker[0].top = (ptColorPickerCenter.y - m_pTextureColorPicker[0]->m_size.cy + (double)((double)53 / (double)256) * m_pTextureColorPicker[0]->m_size.cy);
	m_rectColorPicker[0].right = m_rectColorPicker[0].left + m_pTextureColorPicker[0]->m_size.cx;
	m_rectColorPicker[0].bottom = m_rectColorPicker[0].top + m_pTextureColorPicker[0]->m_size.cy;

	m_rectColorPicker[1].left = ptColorPickerCenter.x - m_pTextureColorPicker[1]->m_size.cx;
	m_rectColorPicker[1].top = (ptColorPickerCenter.y - m_pTextureColorPicker[1]->m_size.cy / 2 + (double)((double)27 / (double)339) * m_pTextureColorPicker[1]->m_size.cy);
	m_rectColorPicker[1].right = m_rectColorPicker[1].left + m_pTextureColorPicker[1]->m_size.cx;
	m_rectColorPicker[1].bottom = m_rectColorPicker[1].top + m_pTextureColorPicker[1]->m_size.cy;

	m_rectColorPicker[2].left = ptColorPickerCenter.x;
	m_rectColorPicker[2].top = (ptColorPickerCenter.y - m_pTextureColorPicker[2]->m_size.cy / 2 + (double)((double)27 / (double)339) * m_pTextureColorPicker[2]->m_size.cy);
	m_rectColorPicker[2].right = m_rectColorPicker[2].left + m_pTextureColorPicker[2]->m_size.cx;
	m_rectColorPicker[2].bottom = m_rectColorPicker[2].top + m_pTextureColorPicker[2]->m_size.cy;

	m_rectColorPicker[3].left = ptColorPickerCenter.x - m_pTextureColorPicker[3]->m_size.cx / 2;
	m_rectColorPicker[3].top = ptColorPickerCenter.y;
	m_rectColorPicker[3].right = m_rectColorPicker[3].left + m_pTextureColorPicker[3]->m_size.cx;
	m_rectColorPicker[3].bottom = m_rectColorPicker[3].top + m_pTextureColorPicker[3]->m_size.cy;

	m_tColorPicker[0] = __AEGON_2D_TRIANGLE(CPoint(m_rectColorPicker[1].left, m_rectColorPicker[0].top), CPoint(m_rectColorPicker[0].right, m_rectColorPicker[0].top), CPoint(m_rectColorPicker[0].CenterPoint().x, m_rectColorPicker[3].top));
	m_tColorPicker[1] = __AEGON_2D_TRIANGLE(CPoint(m_rectColorPicker[1].left, m_rectColorPicker[1].bottom), CPoint(m_rectColorPicker[1].left, m_rectColorPicker[0].top), CPoint(m_rectColorPicker[1].right, m_rectColorPicker[3].top));
	m_tColorPicker[2] = __AEGON_2D_TRIANGLE(CPoint(m_rectColorPicker[2].right, m_rectColorPicker[0].top), CPoint(m_rectColorPicker[2].right, m_rectColorPicker[2].bottom), CPoint(m_rectColorPicker[2].left, m_rectColorPicker[3].top));
	m_tColorPicker[3] = __AEGON_2D_TRIANGLE(CPoint(m_rectColorPicker[2].right, m_rectColorPicker[3].bottom), CPoint(m_rectColorPicker[1].left, m_rectColorPicker[3].bottom), CPoint(m_rectColorPicker[1].right, m_rectColorPicker[3].top));

	m_rectPutStack = CRect(m_pointPutStack.x - m_pTextureCardBackside->m_size.cy / 2, m_pointPutStack.y - m_pTextureCardBackside->m_size.cy / 2, m_pointPutStack.x + m_pTextureCardBackside->m_size.cy / 2, m_pointPutStack.y + m_pTextureCardBackside->m_size.cy / 2);
}
void CWndPvPMinigameFluno::OnDraw(C2DRender* p2DRender)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	CAegonPvPMinigameFluno* pGame = (CAegonPvPMinigameFluno*)CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	if (!g_WndMng.m_pWndAegonPvPGameMain)
		return;

	__MINIGAME_PLAYER_FLUNO* pPlayerMe = (__MINIGAME_PLAYER_FLUNO*)pGame->GetPlayerByID(g_pPlayer->m_idPlayer);
	if (!pPlayerMe)
		return;

	//Render rects for testing
/*	if (m_pTextureCardBackside) 
	{
		p2DRender->RenderRect(m_rectTakeStack, COLOR_ERROR);
		p2DRender->RenderRect(m_rectPutStack, COLOR_SUCCESS);
	}

	p2DRender->RenderRect(m_rectPlayerFieldMe, COLOR_ERROR);

	p2DRender->RenderRect(m_rectPlayerFields[0], COLOR_ERROR);
	p2DRender->RenderRect(m_rectPlayerFields[1], COLOR_ERROR);
	p2DRender->RenderRect(m_rectPlayerFields[2], COLOR_ERROR);
	*/
	if (pGame->m_eGameState != GameState::state_preparation) 
	{
		//other players
		int nPlayerSize = pGame->m_vecsPlayers.size();
		for (int i = 0; i < nPlayerSize; i++)
		{
			if (g_pPlayer->m_idPlayer == pGame->m_vecsPlayers[i]->m_uPlayerID)
			{
				int nIndex = i;
				for (int j = 0; j < nPlayerSize - 1; j++)
				{
					nIndex = (nIndex + 1) % (nPlayerSize);

					__MINIGAME_PLAYER_FLUNO* pPlayer = (__MINIGAME_PLAYER_FLUNO*)pGame->m_vecsPlayers[nIndex];

					if (!pPlayer)
						continue;

					if (pPlayer->m_uPlayerID == g_pPlayer->m_idPlayer)
						continue;

					if (m_pTextureCardBackside)
					{
						CPoint ptRender = m_rectPlayerFields[j].TopLeft();

						CString strName = pPlayer->m_szName;
						CSize nameExtent = p2DRender->GetFont()->GetTextExtent(strName.GetString());
						p2DRender->TextOut(ptRender.x, ptRender.y - nameExtent.cy, strName.GetString(), CAegonThemeSwitcherMng::GetInstance()->GetTextColor());

						for (int k = 0; k < pPlayer->m_vCards.size(); k++)
						{
							if (ptRender.x + m_pTextureCardBackside->m_size.cx * __FLUNO_OTHER_CARD_SCALE > m_rectPlayerFields[j].right)
							{
								ptRender.y += FLUNO_CARD_Y_OFFSET * __FLUNO_OTHER_CARD_SCALE;
								ptRender.x = m_rectPlayerFields[j].left;
							}
							m_pTextureCardBackside->RenderScal(p2DRender, ptRender, 255, __FLUNO_OTHER_CARD_SCALE, __FLUNO_OTHER_CARD_SCALE);
							ptRender.x += FLUNO_CARD_X_OFFSET* __FLUNO_OTHER_CARD_SCALE;
						}
					}
				}
				break;
			}
		}

		//draw takestack
		DrawDeck(p2DRender, m_pointTakeStack, pGame->m_vCardTakeStack.size());

		//draw putstack
		for (int i = 0; i < pGame->m_vCardPutStack.size(); i++)
		{
			CTexture* pTextureCard = GetCardTexture(&pGame->m_vCardPutStack[i]);
			if (pTextureCard)
			{
				CPoint ptPut = m_pointPutStack;
				ptPut.x -= pTextureCard->m_size.cx / 2;
				ptPut.y -= pTextureCard->m_size.cy / 2;

				ptPut.x += i % 3;
				ptPut.y += i % 4;

				FLOAT fDegree, fRadian;
				fDegree = (i * 12345) % 360;

				fRadian = DEGREETORADIAN(fDegree);

				pTextureCard->RenderRotate(p2DRender, ptPut, fRadian, (BOOL)TRUE);
			}
		}

		//Draw own cards
		CPoint ptCard = m_rectPlayerFieldMe.TopLeft();
		for (int i = 0; i < pPlayerMe->m_vCards.size(); i++)
		{
			if (m_nNoDrawOwn != -1 && m_nNoDrawOwn == i)
				continue;

			CTexture* pTextureCard = GetCardTexture(&pPlayerMe->m_vCards[i]);
			if (pTextureCard)
			{
				if (ptCard.x + pTextureCard->m_size.cx > m_rectPlayerFieldMe.right)
				{
					ptCard.y += FLUNO_CARD_Y_OFFSET;
					ptCard.x = m_rectPlayerFieldMe.left;
				}

				if (m_nRenderHoverMove == i)
				{
					pTextureCard->Render(p2DRender, CPoint(ptCard.x, ptCard.y - 20));
				}
				else
					pTextureCard->Render(p2DRender, ptCard);

				ptCard.x += FLUNO_CARD_X_OFFSET;
			}
		}

		if (pGame->m_bSelectColor) 
		{
			DrawColorPicker(p2DRender, pGame->m_uTurn == g_pPlayer->m_idPlayer ? TRUE : FALSE);
		}
		else if (pGame->m_vCardPutStack.size() > 0 && (pGame->m_vCardPutStack[pGame->m_vCardPutStack.size() - 1].m_eCardType == CardType::wild_card || pGame->m_vCardPutStack[pGame->m_vCardPutStack.size() - 1].m_eCardType == CardType::wild_draw_4))
		{
			DrawColorWish(p2DRender, pGame->m_vCardPutStack[pGame->m_vCardPutStack.size() - 1].m_eCardColor);
		}
	}
	else
		DrawDeck(p2DRender, m_pointTakeStack, 108);

	//color picker ani
	if (m_nRenderHoverMove >= FLUNO_MOVE_CHOOSE_COLOR_GREEN && m_nRenderHoverMove <= FLUNO_MOVE_CHOOSE_COLOR_RED)
	{
		if (GetTickCount() > m_dwLastColorPickerIncrease + 20)
		{
			if (m_nColorPickerOffsetY[m_nRenderHoverMove - FLUNO_MOVE_CHOOSE_COLOR_GREEN] < 20)
				m_nColorPickerOffsetY[m_nRenderHoverMove - FLUNO_MOVE_CHOOSE_COLOR_GREEN]++;

			m_dwLastColorPickerIncrease = GetTickCount();
		}
	}

	//State anis
	ProcessAni(p2DRender, 0);
}
BOOL CWndPvPMinigameFluno::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndBase::OnChildNotify(message, nID, pLResult);
}
void CWndPvPMinigameFluno::OnMouseWndSurface(CPoint point)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	if (m_pAnimation)
		return;

	CAegonPvPMinigameFluno* pGame = (CAegonPvPMinigameFluno*)CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	__MINIGAME_PLAYER_FLUNO* pPlayer = (__MINIGAME_PLAYER_FLUNO*)pGame->GetPlayerByID(g_pPlayer->m_idPlayer);
	if (!pPlayer)
		return;

	if (pGame->m_uTurn != g_pPlayer->m_idPlayer)
	{
		m_nRenderHoverMove = -1;
		return;
	}

	if (!m_pTextureCardBackside)
		return;

	if (pGame->m_bSelectColor)
	{
		for (int i = 0; i < 4; i++) 
		{
			if (m_tColorPicker[i].IsInTriangle(point)) 
			{
				int nMoveID = i + FLUNO_MOVE_CHOOSE_COLOR_GREEN;
				//set others 0
				for (int j = 0; j < 4; j++) 
				{
					if (j == i)
						continue;
					m_nColorPickerOffsetY[j] = 0;
				}

				if (m_nRenderHoverMove != nMoveID)
					m_dwLastColorPickerIncrease = 0;

				m_nRenderHoverMove = nMoveID;
				return;
			}
		}

		for (int i = 0; i < 4; i++)
			m_nColorPickerOffsetY[i] = 0;


		m_nRenderHoverMove = -1;
		return;
	}


	//deck
	if (m_rectTakeStack.PtInRect(point)) 
	{
		m_nRenderHoverMove = FLUNO_MOVE_TAKE_CARD;
		return;
	}

	//own cards
	int nCards = pPlayer->m_vCards.size();
	CPoint ptTopLeft = m_rectPlayerFieldMe.TopLeft();
	CRect rectHittest;
	int nCardVSOffsetCount = ceil(m_pTextureCardBackside->m_size.cx / FLUNO_CARD_X_OFFSET);
	int nCardsEachRow = floor(m_rectPlayerFieldMe.Width() / FLUNO_CARD_X_OFFSET) - nCardVSOffsetCount;	
	for (int i = 0; i < nCards; i++) 
	{
			if (ptTopLeft.x + m_pTextureCardBackside->m_size.cx > m_rectPlayerFieldMe.right)
			{
				ptTopLeft.y += FLUNO_CARD_Y_OFFSET;
				ptTopLeft.x = m_rectPlayerFieldMe.left;
			}

			rectHittest = CRect(ptTopLeft.x, ptTopLeft.y, ptTopLeft.x + m_pTextureCardBackside->m_size.cx, ptTopLeft.y + m_pTextureCardBackside->m_size.cy);

			if (i < (nCards - 1)) //i not last card?
			{
				if (i == 0 || i % (nCardsEachRow - 1) != 0) //not last card in row?
				{
					//decrease rect x
					rectHittest.right = rectHittest.left + FLUNO_CARD_X_OFFSET;
				}
			}

			if (nCards > nCardsEachRow) //multiple rows?
			{
				if (i + nCardsEachRow < nCards + nCardVSOffsetCount) //card rendered under current?
				{
					//decrease rect y
					rectHittest.bottom = rectHittest.top + FLUNO_CARD_Y_OFFSET;
				}
			}

			if (rectHittest.PtInRect(point))
			{
				m_nRenderHoverMove = i;
				return;
			}

			ptTopLeft.x += FLUNO_CARD_X_OFFSET;	
	}

	

	m_nRenderHoverMove = -1;
}
void CWndPvPMinigameFluno::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	CAegonPvPMinigameFluno* pGame = (CAegonPvPMinigameFluno*)CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	if (pGame->m_uTurn != g_pPlayer->m_idPlayer)
		return;


	if (m_nRenderHoverMove != -1 && m_nClickMove == m_nRenderHoverMove)
	{
		//TODO replace with image for pass move
		if (pGame->m_bPassTurn && m_nClickMove == FLUNO_MOVE_TAKE_CARD)
			m_nClickMove = FLUNO_MOVE_PASS_MOVE;

		__MINIGAME_COMMAND tmpCommand;
		tmpCommand.dwCommandID = AEG_MINIGAME_COMMAND_MOVE;
		tmpCommand.dwCommandValue = m_nClickMove;

		g_DPlay.SendPvPMinigameCommand(tmpCommand);
		m_nRenderHoverMove = -1;
		m_nClickMove = -1;
		return;
	}
	m_nClickMove = -1;
}
void CWndPvPMinigameFluno::OnLButtonDown(UINT nFlags, CPoint point)
{
	CAegonPvPMinigameFluno* pGame = (CAegonPvPMinigameFluno*)CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	if (m_nRenderHoverMove != -1) 
	{
		m_nClickMove = m_nRenderHoverMove;
		return;
	}
	m_nClickMove = -1;
}
void CWndPvPMinigameFluno::OnGameRefresh()
{
	CWndPvPMinigameBase::OnGameRefresh();

	CAegonPvPMinigameFluno* pGame = (CAegonPvPMinigameFluno*)CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame)
		return;

	if (IsInvalidObj(g_pPlayer))
		return;

}
void CWndPvPMinigameFluno::OnBaseAni()
{
	if (!m_pUpdateGame)
	{
		m_bUpdateInProgress = FALSE;
		return;
	}

	//override base anis here
	switch (m_pUpdateGame->m_nUpdateMode)
	{
	default:
		break;
	}

	CWndPvPMinigameBase::OnBaseAni();
}
void CWndPvPMinigameFluno::OnGameSpecificAni()
{
	CAegonPvPMinigameFluno* pGame = (CAegonPvPMinigameFluno*)CAegonPvPMinigamesMng::GetInstance()->GetActiveGame();
	if (!pGame || IsInvalidObj(g_pPlayer))
	{
		UpdateCurrentGame();
		return;
	}

	if (!m_pUpdateGame)
	{
		m_bUpdateInProgress = FALSE;
		return;
	}

	CAegonPvPMinigameFluno* pUpdateGame = (CAegonPvPMinigameFluno*)m_pUpdateGame;

	switch (pUpdateGame->m_nUpdateMode)
	{
	case AEG_MINIGAME_UPDATE_START:
	{
		SAFE_DELETE(m_pAnimation);
		m_pAnimation = new CMinigameAnimationFlunoInit;
		m_pAnimation->SetParent(this);

		m_nRenderHoverMove = -1;
		m_pAnimation->Initialize();
		return;
	}
	case AEG_MINIGAME_UPDATE_MOVE:
	{
		//create animation container
		SAFE_DELETE(m_pAnimation);
		m_pAnimation = new CMinigameMultipleAnimations;
		((CMinigameMultipleAnimations*)m_pAnimation)->m_bRenderPrevious = TRUE;
		if (!m_pAnimation) 
		{
			UpdateCurrentGame();
			return;
		}
		m_pAnimation->SetParent(this);

		//add animations
		__MINIGAME_PLAYER_FLUNO* pPlayerMoved = (__MINIGAME_PLAYER_FLUNO*)pGame->GetPlayerByID(pGame->m_uTurn);
		__MINIGAME_PLAYER_FLUNO* pPlayerMovedFuture = (__MINIGAME_PLAYER_FLUNO*)pUpdateGame->GetPlayerByID(pGame->m_uTurn);

		BOOL bPutCard = FALSE;
		if (pGame->m_vCardPutStack.size() > 0 && pUpdateGame->m_vCardPutStack.size() > 0) 
			if(pGame->m_vCardPutStack.size() != pUpdateGame->m_vCardPutStack.size())
				bPutCard = pGame->m_vCardPutStack[pGame->m_vCardPutStack.size() - 1] != pUpdateGame->m_vCardPutStack[pUpdateGame->m_vCardPutStack.size() - 1] ? TRUE : FALSE;

		int nCardDiff = pPlayerMovedFuture->m_vCards.size() - pPlayerMoved->m_vCards.size();

		if (bPutCard) //move card on put stack
		{
			nCardDiff++;

			CMinigameAnimationBase* pNewAni = new CMinigameAnimationFlunoMoveCard;
			pNewAni->SetParent(this);

			CPoint ptStart;
			int nCardIndex = pUpdateGame->m_vCardPutStack.size() - 1;
			FLOAT fRotateEnd = (nCardIndex * 12345) % 360;
			
			__FLUNO_CARD* pCard = &pUpdateGame->m_vCardPutStack[nCardIndex];
			CTexture* pCardTexture = GetCardTexture(pCard);
			//me
			if (pPlayerMovedFuture->m_uPlayerID == g_pPlayer->m_idPlayer)
			{
				//get hand index
				int nHandIndex = 0;
				for (int i = 0; i < pPlayerMoved->m_vCards.size(); i++) 
				{
					if (*pCard == pPlayerMoved->m_vCards[i]) 
					{
						nHandIndex = i;
						break;
					}
				}

				m_nNoDrawOwn = nHandIndex;

				//calculate start point

				CPoint ptStart = GetOwnHandPointByIndex(pCardTexture, nHandIndex, TRUE);
				((CMinigameAnimationFlunoMoveCard*)pNewAni)->SetValues(ptStart, CPoint(m_pointPutStack.x - m_pTextureCardBackside->m_size.cx / 2, m_pointPutStack.y - m_pTextureCardBackside->m_size.cy / 2) , 0, fRotateEnd, pCardTexture);
			}
			else //other player
			{
				//calculate start point
				int nPlayerSize = pGame->m_vecsPlayers.size();
				ptStart = GetOtherHandPointByIndex(pGame, nPlayerSize, pPlayerMovedFuture->m_uPlayerID, pPlayerMoved->m_vCards.size() - 1);
				((CMinigameAnimationFlunoMoveCard*)pNewAni)->SetValues(ptStart, CPoint(m_pointPutStack.x - m_pTextureCardBackside->m_size.cx / 2, m_pointPutStack.y - m_pTextureCardBackside->m_size.cy / 2), 0, fRotateEnd, pCardTexture, __FLUNO_OTHER_CARD_SCALE, 1.0f);
			}
			((CMinigameMultipleAnimations*)m_pAnimation)->AddAnimation(pNewAni);
		}

		if (nCardDiff > 0) //take card
		{
			for (int i = nCardDiff - 1; i >= 0; i--)
			{
				int nIndex = pPlayerMovedFuture->m_vCards.size() - 1 - i;
				if (nIndex < 0 || nIndex >= pPlayerMovedFuture->m_vCards.size())
					continue;

				__FLUNO_CARD* pCard = &pPlayerMovedFuture->m_vCards[nIndex];
				if (!pCard)
					continue;

				CMinigameAnimationBase* pNewAni = new CMinigameAnimationFlunoMoveCard;
				pNewAni->SetParent(this);

				CPoint ptStart = CPoint(m_pointTakeStack.x - m_pTextureCardBackside->m_size.cx / 2, m_pointTakeStack.y - m_pTextureCardBackside->m_size.cy / 2);
				
				float fRotateEnd = 0.0;
				float fRotateStart = 315.0;
				if (pPlayerMovedFuture->m_uPlayerID == g_pPlayer->m_idPlayer)
				{
					//TODO ani own draw
					CTexture* pCardTexture = GetCardTexture(pCard);
					//calculate end point
					CPoint ptEnd = GetOwnHandPointByIndex(pCardTexture, nIndex + 1, FALSE);
					
					((CMinigameAnimationFlunoMoveCard*)pNewAni)->SetValues(ptStart, ptEnd, fRotateStart, fRotateEnd, pCardTexture);
				}
				else
				{
					//TODO other draw
					CTexture* pCardTexture = m_pTextureCardBackside;
					//calculate end point
					CPoint ptEnd = GetOtherHandPointByIndex(pGame, pGame->m_vecsPlayers.size(), pPlayerMovedFuture->m_uPlayerID, nIndex + 1);

					((CMinigameAnimationFlunoMoveCard*)pNewAni)->SetValues(ptStart, ptEnd, fRotateStart, fRotateEnd, pCardTexture, 1.0f, __FLUNO_OTHER_CARD_SCALE);
				}
				((CMinigameMultipleAnimations*)m_pAnimation)->AddAnimation(pNewAni);

			}
		}
		//start animation
		m_pAnimation->Initialize();
		return;
	}
	}

	UpdateCurrentGame();
}
CTexture* CWndPvPMinigameFluno::GetCardTexture(__FLUNO_CARD* pCard)
{
	CString strFileName;
	pCard->GetCardFileName(&strFileName);
	return CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_MINIGAMES_UNO, strFileName.GetString()), COLOR_ITEM_BG, TRUE);
}
CPoint CWndPvPMinigameFluno::GetOwnHandPointByIndex(CTexture* pCardTexture, int nIndex, BOOL bHoverMoveOffset)
{
	if (!pCardTexture)
		return CPoint(0, 0);
	CPoint ptCard = m_rectPlayerFieldMe.TopLeft();

	int nOffset = 0;

	if (bHoverMoveOffset)
		nOffset = 20;

	CPoint ptOut = CPoint(ptCard.x, ptCard.y - nOffset);

	for (int i = 0; i < nIndex; i++)
	{
		if (ptCard.x + pCardTexture->m_size.cx > m_rectPlayerFieldMe.right)
		{
			ptCard.y += FLUNO_CARD_Y_OFFSET;
			ptCard.x = m_rectPlayerFieldMe.left;
		}
		ptOut = CPoint(ptCard.x, ptCard.y - nOffset);
		ptCard.x += FLUNO_CARD_X_OFFSET;
	}
	return ptOut;
}
CPoint CWndPvPMinigameFluno::GetOtherHandPointByIndex(CAegonPvPMinigameFluno* pGame, int nPlayerSize, u_long uPlayerID, int nIndexCard)
{
	if (!pGame)
		return CPoint(0, 0);

	CPoint ptOut = CPoint(0, 0);
	for (int i = 0; i < nPlayerSize; i++)
	{
		if (g_pPlayer->m_idPlayer == pGame->m_vecsPlayers[i]->m_uPlayerID)
		{
			int nIndex = i;
			for (int j = 0; j < nPlayerSize - 1; j++)
			{
				nIndex = (nIndex + 1) % (nPlayerSize);

				__MINIGAME_PLAYER_FLUNO* pPlayer = (__MINIGAME_PLAYER_FLUNO*)pGame->m_vecsPlayers[nIndex];

				if (!pPlayer || pPlayer->m_uPlayerID != uPlayerID)
					continue;

				CPoint ptRender = m_rectPlayerFields[j].TopLeft();
				ptOut = ptRender;

				for (int k = 0; k < nIndexCard; k++)
				{
					if (ptRender.x + m_pTextureCardBackside->m_size.cx * __FLUNO_OTHER_CARD_SCALE > m_rectPlayerFields[j].right)
					{
						ptRender.y += FLUNO_CARD_Y_OFFSET * __FLUNO_OTHER_CARD_SCALE;
						ptRender.x = m_rectPlayerFields[j].left;
					}
					ptOut = ptRender;
					ptRender.x += FLUNO_CARD_X_OFFSET * __FLUNO_OTHER_CARD_SCALE;
				}
			}
			break;
		}
	}
	return ptOut;
}
void CWndPvPMinigameFluno::DrawDeck(C2DRender* p2DRender, CPoint ptCenter, int nCount)
{
	if (!m_pTextureCardBackside)
		return;

	int nDrawCount = ceil(nCount / 10);
	if (nCount <= 1)
		nDrawCount = 1;
	else if (nDrawCount <= 1 && nCount > 1)
		nDrawCount = 2;

	for (int i = 0; i < nDrawCount; i++)
	{
		CPoint ptTake = ptCenter;

		//no rotation
		//ptTake.y += (-1 * i) - m_pTextureCardBackside->m_size.cy / 2;
		//ptTake.x += (i) - m_pTextureCardBackside->m_size.cx / 2;
		//m_pTextureCardBackside->Render(p2DRender, ptTake);

		//facing middle //315
		ptTake.y -= m_pTextureCardBackside->m_size.cy / 2;
		ptTake.x -= m_pTextureCardBackside->m_size.cx / 2;
		//ptTake.x -= i;
		ptTake.y -= i*3; // i

		//facing left // 270
		//ptTake.y -= m_pTextureCardBackside->m_size.cy / 2;
		//ptTake.x -= m_pTextureCardBackside->m_size.cx / 2;
		//ptTake.x -= i;
		//ptTake.y -= i;

		if (i == (nDrawCount - 1) && m_nRenderHoverMove == FLUNO_MOVE_TAKE_CARD)
		{
			m_pTextureCardBackside->RenderRotate(p2DRender, CPoint(ptTake.x + 5, ptTake.y - 15), DEGREETORADIAN(315), (BOOL)TRUE);
		}
		else
			m_pTextureCardBackside->RenderRotate(p2DRender, ptTake, DEGREETORADIAN(315), (BOOL)TRUE);
	}
}
void CWndPvPMinigameFluno::DrawColorPicker(C2DRender* p2DRender, BOOL bMyTurn)
{
	for (int i = 0; i < 4; i++) 
	{
		CPoint ptRender = m_rectColorPicker[i].TopLeft();
		if(bMyTurn)
			ptRender.y -= m_nColorPickerOffsetY[i];
		m_pTextureColorPicker[i]->Render(p2DRender, ptRender);
	}
}
void CWndPvPMinigameFluno::DrawColorWish(C2DRender* p2DRender, CardColor sCardColor)
{
	CTexture* pWishTexture = NULL;
	CPoint ptRender;

	switch (sCardColor) 
	{
	case CardColor::green:
	{
		pWishTexture = m_pTextureColorPicker[0];
		ptRender.x = m_rectPutStack.CenterPoint().x - pWishTexture->m_size.cx / 2;
		ptRender.y = m_rectPutStack.top - pWishTexture->m_size.cy;
		break;
	}
	case CardColor::blue:
	{
		pWishTexture = m_pTextureColorPicker[1];
		ptRender.x = m_rectPutStack.left - pWishTexture->m_size.cx;
		ptRender.y = m_rectPutStack.CenterPoint().y - pWishTexture->m_size.cy / 2;
		break;
	}
	case CardColor::yellow:
	{
		pWishTexture = m_pTextureColorPicker[2];
		ptRender.x = m_rectPutStack.right;
		ptRender.y = m_rectPutStack.CenterPoint().y - pWishTexture->m_size.cy / 2;
		break;
	}
	case CardColor::red:
	{
		pWishTexture = m_pTextureColorPicker[3];
		ptRender.x = m_rectPutStack.CenterPoint().x - pWishTexture->m_size.cx / 2;
		ptRender.y = m_rectPutStack.bottom;
		break;
	}
	}

	if (pWishTexture) 
	{
		pWishTexture->Render(p2DRender, ptRender);
	}
}
void CWndPvPMinigameFluno::OnAniEnd()
{
	m_nNoDrawOwn = -1;
	m_nRenderHoverMove = -1;
}
#ifdef __AEGON_THEME_SWITCHER
void CWndPvPMinigameFluno::OnThemeSelectionChanged()
{
	CWndPvPMinigameBase::OnThemeSelectionChanged();
}
#endif
///////////////////////////
/////GAME LIST WINDOW//////
///////////////////////////
CWndMinigameGameList::CWndMinigameGameList()
{
	m_nRowHeight = 40;
	m_nSelectedItem = 0;
}
CWndMinigameGameList::~CWndMinigameGameList()
{
}
void CWndMinigameGameList::OnDraw(C2DRender* p2DRender)
{
	CRect wndRect = GetWindowRect();
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	CPoint point = CPoint(0, 0);
	CString str;

	int nRange = 0;
	int nPage = wndRect.Height() / m_nRowHeight;

	nRange = CAegonPvPMinigamesMng::GetInstance()->m_vecActiveGames.size();

	if (!nRange)
		nRange = 1;

	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);
	m_wndScrollBar.EnableScrollBar(TRUE);
	m_wndScrollBar.SetScrollStep(1);

	int nIndex = 0;
	for (int i = m_wndScrollBar.GetScrollPos(); i < CAegonPvPMinigamesMng::GetInstance()->m_vecActiveGames.size(); i++, nIndex++)
	{
		CAegonPvPMinigameBase* pGame = CAegonPvPMinigamesMng::GetInstance()->m_vecActiveGames[i];
		if (!pGame)
			continue;

		point.x = 5;
		point.y = nIndex * m_nRowHeight;

		if (i > m_wndScrollBar.GetScrollPos() + nPage)
			break;


		if (point.y + m_nRowHeight >= m_nRowHeight)
			p2DRender->RenderLine(CPoint(0, point.y + m_nRowHeight), CPoint(wndRect.Width() - 30, point.y + m_nRowHeight), dwColorText);

#ifdef __AEGON_THEME_SWITCHER
		DWORD RenderFillRect = CAegonThemeSwitcherMng::GetInstance()->GetRenderFillRectColor();
		DWORD RenderRect = CAegonThemeSwitcherMng::GetInstance()->GetRenderRectColor();
#endif //__AEGON_THEME_SWITCHER

		if (m_nSelectedItem == i)
		{
			CRect rectSelected = CRect(0, point.y, wndRect.right - 30, point.y + m_nRowHeight);
#ifdef __AEGON_THEME_SWITCHER
			p2DRender->RenderFillRect(rectSelected, RenderFillRect);
			p2DRender->RenderRect(rectSelected, RenderRect);
#else //__AEGON_THEME_SWITCHER
			p2DRender->RenderFillRect(rectSelected, 0xFFf8e6cc);
			p2DRender->RenderRect(rectSelected, 0xFFf28818);
#endif //__AEGON_THEME_SWITCHER
		}

		int nMaxLength = 100 - point.x - p2DRender->GetFont()->GetTextExtent("...").cx;
		str.Format("%s", pGame->m_vecsPlayers[0]->m_szName);
		CSize strExtent = p2DRender->GetFont()->GetTextExtent(str.GetString());
		if (strExtent.cx > nMaxLength)
		{
			LONG letterLength = p2DRender->GetFont()->GetTextExtent("W").cx;
			int nMaxLetters = 0;
			if (letterLength != 0)
				nMaxLetters = floor((double)nMaxLength / (double)letterLength);

			if (nMaxLetters > str.GetLength())
				nMaxLetters = str.GetLength();

			str = str.Mid(0, nMaxLetters);
			str += "...";
		}
		strExtent = p2DRender->GetFont()->GetTextExtent(str.GetString());
		p2DRender->TextOut(point.x, point.y + (m_nRowHeight / 2) - (strExtent.cy / 2), str.GetString(), dwColorText);

		CAegonPvPMinigamesMng::GetInstance()->GetGameNameByID(&str, pGame->m_nGameID);
		strExtent = p2DRender->GetFont()->GetTextExtent(str.GetString());
		p2DRender->TextOut(point.x + 100, point.y + (m_nRowHeight / 2) - (strExtent.cy / 2), str.GetString(), dwColorText);

		str.Format("%d/%d", static_cast<int>(pGame->m_vecsPlayers.size()), pGame->m_dwMaxPlayers);
		strExtent = p2DRender->GetFont()->GetTextExtent(str.GetString());
		p2DRender->TextOut(point.x + 220, point.y + (m_nRowHeight / 2) - (strExtent.cy / 2), str.GetString(), dwColorText);

	}


}
void CWndMinigameGameList::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	CRect rect = GetWindowRect();

	m_wndScrollBar.Create(WBS_DOCKING | WBS_VERT, rect, this, 1000);
	m_wndScrollBar.SetVisible(IsWndStyle(WBS_VSCROLL));

}
BOOL CWndMinigameGameList::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndBase::OnChildNotify(message, nID, pLResult);
}
BOOL CWndMinigameGameList::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndBase::OnCommand(nID, dwMessage, pWndBase);
}
void CWndMinigameGameList::OnLButtonUp(UINT nFlags, CPoint point)
{

}
void CWndMinigameGameList::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetSelectedItem(point);

	if (m_nSelectedItem < 0 || m_nSelectedItem >= CAegonPvPMinigamesMng::GetInstance()->m_vecActiveGames.size())
		return;

	CWndAegonPvPGameFinder* pParentWnd = (CWndAegonPvPGameFinder*)GetParentWnd();
	if (pParentWnd)
	{
	}
}

void CWndMinigameGameList::OnMouseWndSurface(CPoint point)
{
}
void CWndMinigameGameList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	SetSelectedItem(point);
	CWndAegonPvPGameFinder* pParentWnd = (CWndAegonPvPGameFinder*)GetParentWnd();
	if (pParentWnd)
	{
		pParentWnd->JoinSelectedGame();
	}
}
void CWndMinigameGameList::OnRButtonUp(UINT nFlags, CPoint point)
{
	SetSelectedItem(point);
}
void CWndMinigameGameList::SetSelectedItem(CPoint point)
{
	int nScrollPos = m_wndScrollBar.GetScrollPos();
	CRect wndRect = GetWndRect();
	int nRowCount = ceil((float)wndRect.Height() / (float)m_nRowHeight);

	for (int i = 0; i < nRowCount; i++)
	{
		if (nScrollPos + i >= CAegonPvPMinigamesMng::GetInstance()->m_vecActiveGames.size())
			break;

		CRect tmpRect;
		tmpRect.left = 0;
		tmpRect.top = 0 + i * m_nRowHeight;
		tmpRect.right = wndRect.Width() - 30;
		tmpRect.bottom = tmpRect.top + m_nRowHeight;

		if (tmpRect.PtInRect(point))
		{
			m_nSelectedItem = nScrollPos + i;
			return;
		}
	}
	m_nSelectedItem = -1;
}

///////////////////////////
//////Finder Window////////
///////////////////////////
CWndAegonPvPGameFinder::CWndAegonPvPGameFinder()
{
	m_pWndGameList = NULL;
}
CWndAegonPvPGameFinder::~CWndAegonPvPGameFinder()
{
	CAegonPvPMinigamesMng::GetInstance()->DeleteMinigameList();
}
void CWndAegonPvPGameFinder::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	LPWNDCTRL listCtrl = GetWndCtrl(WIDC_CUSTOM1);
	if (listCtrl)
	{
		SAFE_DELETE(m_pWndGameList);
		m_pWndGameList = new CWndMinigameGameList;
		m_pWndGameList->Create(WBS_CHILD | WBS_VSCROLL, listCtrl->rect, this, 100000);
		m_pWndGameList->m_byWndType = WTYPE_LISTBOX;
		m_pWndGameList->m_bTile = true;
		m_pWndGameList->m_bVisible = true;
		m_pWndGameList->m_strTexture = "WndEditTile00.tga";
	}

	g_DPlay.SendPvPMinigameGetList();
	MoveParentCenter();
}
void CWndAegonPvPGameFinder::OnDraw(C2DRender* p2DRender)
{
}
BOOL CWndAegonPvPGameFinder::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_BUTTON1: //join
	{
		JoinSelectedGame();
		break;
	}
	case WIDC_BUTTON2: //refresh
	{
		g_DPlay.SendPvPMinigameGetList();
		break;
	}
	case WIDC_BUTTON3: //close
		Destroy();
		break;
	default:
		break;
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

BOOL CWndAegonPvPGameFinder::Initialize(CWndBase* pWndParent, DWORD)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PVP_MINIGAME_FINDER, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndAegonPvPGameFinder::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndAegonPvPGameFinder::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndAegonPvPGameFinder::JoinSelectedGame()
{
	int nSelected = m_pWndGameList->GetSelectedItemIndex();
	if (nSelected < 0 || nSelected >= CAegonPvPMinigamesMng::GetInstance()->m_vecActiveGames.size())
		return;
	if (CAegonPvPMinigamesMng::GetInstance()->m_vecActiveGames[nSelected]->m_vecsPlayers.size() <= 0)
		return;

	__MINIGAME_COMMAND sCommand;
	sCommand.dwCommandID = AEG_MINIGAME_COMMAND_JOIN;
	sCommand.dwCommandValue = CAegonPvPMinigamesMng::GetInstance()->m_vecActiveGames[nSelected]->m_vecsPlayers[0]->m_uPlayerID;	//leader player id
	sCommand.dwCommandValue2 = 1;	//accept

	g_DPlay.SendPvPMinigameCommand(sCommand);
}
#endif // __AEGON_PVP_MINIGAMES