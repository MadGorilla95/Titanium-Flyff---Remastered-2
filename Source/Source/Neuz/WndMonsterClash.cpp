#include "StdAfx.h"

#ifdef __SYS_MONSTER_CLASH
#include "MsgHdr.h"
#include "ResData.h"
#include "WndManager.h"
#include "DPClient.h"
extern CDPClient g_DPlay;
#include "defineText.h"
#include "AppDefine.h"
#include "defineObj.h"

CWndMonsterClashRegister::CWndMonsterClashRegister()
{
}

CWndMonsterClashRegister::~CWndMonsterClashRegister()
{
}

BOOL CWndMonsterClashRegister::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_MONSTERCLASH_REGISTER, 0, CPoint(0, 0), pWndParent);
}

void CWndMonsterClashRegister::OnDraw(C2DRender* p2DRender)
{
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);
	CString strTitle;
	strTitle.Format("Monster Clash starts every day at 19:00 CEST. Please note, participation costs 100 mill penya. The winning guild will receive Monster Clash Fragments, which can be spent in the Monster Clash shop for fun stuff!");
	pWndText->SetString(strTitle);

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC);
	pWndStatic->SetTitle("WARNING: Application cost 100M!");
	pWndStatic->m_dwColor = COLOR_ERROR;
}

void CWndMonsterClashRegister::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	MoveParentCenter();
}

BOOL CWndMonsterClashRegister::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		if (nID == WIDC_BUTTON1)
		{
			//Send Register
			g_DPlay.SendHdr(PACKETTYPE_MONSTER_CLASH_REGISTER);
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndMonsterClashRegister::AddGuild(DWORD dwGuildId, CString strGuild)
{
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	int nIdx = pWndListBox->AddString(strGuild);

	pWndListBox->SetItemData(nIdx, dwGuildId);
}

void CWndMonsterClashRegister::RemoveGuild(DWORD dwGuildId)
{
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);

	int nIdx = pWndListBox->GetItemIndex(dwGuildId);
	pWndListBox->DeleteString(nIdx);
}

CWndMonsterClashRegisterMember::CWndMonsterClashRegisterMember()
{
}

CWndMonsterClashRegisterMember::~CWndMonsterClashRegisterMember()
{
}

BOOL CWndMonsterClashRegisterMember::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_MONSTERCLASH_MEMBER, 0, CPoint(0, 0), pWndParent);
}

void CWndMonsterClashRegisterMember::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	MoveParentCenter();
}

void CWndMonsterClashRegisterMember::AddMemberL(u_long idPlayer, CString strPlayer)
{
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX2);

	int nIdx = pWndListBox->GetItemIndex(idPlayer);
	if (nIdx > -1)
		return;//already inserted

	nIdx = pWndListBox->AddString(strPlayer);
	pWndListBox->SetItemData(nIdx, idPlayer);
}

void CWndMonsterClashRegisterMember::AddMemberR(u_long idPlayer, CString strPlayer)
{
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);

	int nIdx = pWndListBox->GetItemIndex(idPlayer);
	if (nIdx > -1)
		return;//already inserted

	nIdx = pWndListBox->AddString(strPlayer);
	pWndListBox->SetItemData(nIdx, idPlayer);
}

void CWndMonsterClashRegisterMember::RemoveMemberL(u_long idPlayer, CString strPlayer)
{
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX2);

	int nIdx = pWndListBox->GetItemIndex(idPlayer);
	if (nIdx == -1)
		return;//not existing

	pWndListBox->DeleteString(nIdx);
}

void CWndMonsterClashRegisterMember::RemoveMemberR(u_long idPlayer, CString strPlayer)
{
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);

	int nIdx = pWndListBox->GetItemIndex(idPlayer);
	if (nIdx == -1)
		return;//already inserted

	pWndListBox->DeleteString(nIdx);
}

BOOL CWndMonsterClashRegisterMember::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		if (nID == WIDC_BUTTON1)
		{
			//Send Remove
			CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX2);
			if (pWndListBox->GetCurSel() >= 0)
			{
				auto playerid = pWndListBox->GetItemData(pWndListBox->GetCurSel());
				g_DPlay.SendMonsterClashUnRegisterMember(playerid);
			}
		}
		else if (nID == WIDC_BUTTON2)
		{
			//Send Add
			CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
			if (pWndListBox->GetCurSel() >= 0)
			{
				auto playerid = pWndListBox->GetItemData(pWndListBox->GetCurSel());
				g_DPlay.SendMonsterClashRegisterMember(playerid);
			}
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

CWndMonsterClashRank::CWndMonsterClashRank()
	: nMaxKillCount(0),
	bActive(FALSE),
	nFinalStage(-1)
{

}

CWndMonsterClashRank* CWndMonsterClashRank::GetInstance()
{
	static CWndMonsterClashRank sRank;
	return &sRank;
}

bool SortClashRank(CWndMonsterClashRank::GUILD_RANK& rank1, CWndMonsterClashRank::GUILD_RANK& rank2)
{
	if (rank1.time > 0 && rank2.time == 0)
		return true;
	if (rank1.time > 0 && rank2.time > 0)
		return rank1.time > rank2.time;
	if (rank1.time == 0 && rank2.time > 0)
		return rank1.time < rank2.time;
	return rank1.nKillCount > rank2.nKillCount;
}

void CWndMonsterClashRank::Sort()
{
	std::sort(vecGuild.begin(), vecGuild.end(), SortClashRank);
}

void CWndMonsterClashRank::Serialize(CAr& ar)
{
	/*
	states
	0 = killcount
	1 = activate mc & Guilds
	2 = deactivate mc
	3 = mc state
	*/
	//load
	BYTE b;
	ar >> b;
	if (b == 0)
	{
		DWORD dwGuild;
		ar >> dwGuild;
		int nKillCount;
		ar >> nKillCount;
		time_t tTime;
		ar >> tTime;
		int nState;
		ar >> nState;

		auto it = std::find_if(vecGuild.begin(), vecGuild.end(), [&](const GUILD_RANK& rank) {
			return rank.dwGuild == dwGuild;
			});
		if (it != vecGuild.end())
		{
			int nOldState = it->nStage;

			it->nKillCount = nKillCount;
			it->time = tTime;
			it->nStage = nState;
			Sort();

			if (nState == nFinalStage && nOldState != nState && dwGuild == g_pPlayer->m_idGuild)
			{
				CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
				g_Caption1.AddCaption(GETTEXT(TID_GAME_MONSTERCLASH_FINAL_STAGE_SHOUT), pWndWorld->m_pFontAPITitle);
			}
		}
	}
	else if (b == 1)
	{
		SetActive(TRUE);
		vecGuild.clear();

		int nSize;
		ar >> nSize;
		for (int i = 0; i < nSize; i++)
		{
			GUILD_RANK sRank;
			ar >> sRank.dwGuild;

			TCHAR szGuild[16];
			ar.ReadString(szGuild, 16);
			sRank.strGuild = szGuild;

			sRank.nKillCount = 0;
			sRank.time = 0;

			vecGuild.push_back(sRank);
		}
		ar >> nMaxKillCount;
		ar >> nFinalStage;
	}
	else if (b == 2)
	{
		SetActive(FALSE);
		vecGuild.clear();
	}
	else if (b == 3)
	{
		ar >> nState;
		ar >> tTime;
	}
}
////////new dashboard
void CWndMonsterClashRank::DrawMonsterClashGuildInfo(C2DRender* p2DRender, const BOOL bIsMyGuild, const int nRank, const GUILD_RANK sGuild, const CPoint ptRank, const CPoint ptLogo,
	const CPoint ptGName, const CPoint ptHypoon, CPoint ptState, const float fLogoScaleX, const float fLogoScaleY, const CRect rectBg)
{
	CString strFormat;
	DWORD dwColor = 0xFFFFFFFF;
	// Draw Rank
	strFormat.Format("%d.", nRank + 1);
	p2DRender->TextOut(ptRank.x, ptRank.y, strFormat, 0xFFFFFFFF);

	// Draw Logo
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
	if (g_GuildMng.GetGuild(sGuild.dwGuild)->m_dwLogo > 0)
		pWndWorld->m_pTextureLogo[g_GuildMng.GetGuild(sGuild.dwGuild)->m_dwLogo - 1].RenderScal(p2DRender, ptLogo, 255, fLogoScaleX, fLogoScaleY);

	// Draw Guild Name
	strFormat = g_GuildMng.GetGuild(sGuild.dwGuild)->m_szGuild;
	if (strFormat.GetLength() > 14)
	{
		int	nReduceCount = 0;

		for (nReduceCount = 0; nReduceCount < 14; )
		{
			if (IsDBCSLeadByte(strFormat[nReduceCount]))
				nReduceCount += 2;
			else
				nReduceCount++;
		}
		strFormat = strFormat.Left(nReduceCount);
		strFormat += "...";
	}
	p2DRender->TextOut(ptGName.x, ptGName.y, strFormat, 0xFFFEBB1B);

	// Draw Hypoon
	if (sGuild.nStage >= nFinalStage)
		p2DRender->TextOut(ptHypoon.x - 3, ptHypoon.y, "vs", 0xFFFFFFFF);
	else
		p2DRender->TextOut(ptHypoon.x, ptHypoon.y, "-", 0xFFFFFFFF);

	// Draw State
	if (sGuild.nStage >= nFinalStage)
	{
		DrawOutLineLamp(p2DRender, rectBg, 0x00F67608, 0xBEF67608, sGuild.nStage, nRank, bIsMyGuild);
		strFormat.Format("%s", prj.GetText(TID_GAME_MONSTERCLASH_FINAL_STAGE));
		dwColor = 0xFFF67608;
		p2DRender->TextOut(ptState.x, ptState.y, strFormat, dwColor);
	}
	else
	{
		if (bIsMyGuild)
		{
			DrawMyGuildKillCount(p2DRender, sGuild, ptState, nMaxKillCount);
			DrawOutLineFlash(p2DRender, rectBg, 0x00FFFFFF, 0xBEFFFFFF);
		}
		else
		{
			strFormat.Format("%03d", sGuild.nKillCount);

			p2DRender->TextOut(ptState.x, ptState.y, strFormat, dwColor);

			ptState.x += 21;
			p2DRender->TextOut(ptState.x, ptState.y, "/", 0xFFC2D1FF);

			strFormat.Format("%03d", nMaxKillCount);
			ptState.x += 7;
			p2DRender->TextOut(ptState.x, ptState.y, strFormat, dwColor);
		}
	}
}

void CWndMonsterClashRank::DrawMyGuildKillCount(C2DRender* p2DRender, const GUILD_RANK sGuild, CPoint ptState, const int nMax)
{
	CString strFormat;
	int i;
	int nCipher = 0;
	char strTempKillCount[MAX_KILLCOUNT_CIPHERS + 1];
	char strMyGuildKillCount[MAX_KILLCOUNT_CIPHERS + 1];
	DWORD dwColor = 0xFFFFFFFF;

	sprintf(strTempKillCount, "%03d", sGuild.nKillCount);

	for (i = 0; i < MAX_KILLCOUNT_CIPHERS; i++)
	{
		strMyGuildKillCount[i] = m_stKillCountCiphers[i].szMyGuildKillCount;
		if (strTempKillCount[i] > '0' && nCipher == 0)
			nCipher = MAX_KILLCOUNT_CIPHERS - i;
	}

	strMyGuildKillCount[MAX_KILLCOUNT_CIPHERS] = NULL;
	int nMyGuildKillCount = atoi(strMyGuildKillCount);

	if (nMyGuildKillCount != sGuild.nKillCount)
		m_bFlashBackground = TRUE;

	for (i = 0; i < MAX_KILLCOUNT_CIPHERS; i++)
	{
		if (nMyGuildKillCount != sGuild.nKillCount)
		{
			if (MAX_KILLCOUNT_CIPHERS - i <= nCipher)
			{
				m_stKillCountCiphers[i].bDrawMyGuildKillCount = FALSE;
				m_stKillCountCiphers[i].fScaleX = 2.5f;
				m_stKillCountCiphers[i].fScaleY = 2.5f;
			}

			m_stKillCountCiphers[i].szMyGuildKillCount = strTempKillCount[i];
		}

		m_stKillCountCiphers[i].ptPos = ptState;

		if (m_stKillCountCiphers[i].bDrawMyGuildKillCount)
		{
			strFormat.Format("%c", m_stKillCountCiphers[i].szMyGuildKillCount);
			p2DRender->TextOut(ptState.x, ptState.y, strFormat, dwColor);
		}

		ptState.x += 9;
	}

	p2DRender->TextOut(ptState.x, ptState.y, "/", 0xFFC2D1FF);

	ptState.x += 7;
	strFormat.Format("%03d", nMax);
	p2DRender->TextOut(ptState.x, ptState.y, strFormat, dwColor);

	// Kill Count Animation
	for (i = 0; i < MAX_KILLCOUNT_CIPHERS; i++)
	{
		if (m_stKillCountCiphers[i].bDrawMyGuildKillCount == FALSE)
		{
			m_stKillCountCiphers[i].fScaleX -= 0.15f;
			m_stKillCountCiphers[i].fScaleY -= 0.15f;

			if (m_stKillCountCiphers[i].fScaleX <= 1.0f && m_stKillCountCiphers[i].fScaleY <= 1.0f)
			{
				m_stKillCountCiphers[i].fScaleX = 1.0f;
				m_stKillCountCiphers[i].fScaleY = 1.0f;
				m_stKillCountCiphers[i].bDrawMyGuildKillCount = TRUE;
			}

			strFormat.Format("%c", m_stKillCountCiphers[i].szMyGuildKillCount);
			int nPosx = (int)(m_stKillCountCiphers[i].ptPos.x - (6 * (m_stKillCountCiphers[i].fScaleX - 1.0f)));
			int nPosy = (int)(m_stKillCountCiphers[i].ptPos.y - (8 * (m_stKillCountCiphers[i].fScaleY - 1.0f)));
			dwColor = D3DCOLOR_ARGB((m_stKillCountCiphers[i].nAlpha -= 10), 255, 255, 255);

			p2DRender->TextOut(nPosx, nPosy, m_stKillCountCiphers[i].fScaleX,
				m_stKillCountCiphers[i].fScaleY, strFormat, dwColor);
		}
	}
}

void CWndMonsterClashRank::DrawOutLineLamp(C2DRender* p2DRender, const CRect rectBg, const DWORD dwColorstart, const DWORD dwColorend, const int nState, const int nRank, const BOOL bIsMyGuild) const
{
	CPoint Point = CPoint(rectBg.TopLeft().x, rectBg.TopLeft().y);
	LONG thick;
	float fVar = 0.01f;

	if (bIsMyGuild)
		thick = 4;
	else
		thick = 2;

	if (nState >= nFinalStage)
		fVar = 0.04f;

	D3DXCOLOR dwColorDest2 = dwColorstart;
	D3DXCOLOR dwColor = dwColorend;
	D3DXCOLOR dwColor1 = D3DCOLOR_ARGB(0, 255, 255, 255);
	D3DXCOLOR dwColor2 = D3DCOLOR_ARGB(0, 255, 255, 255);

	static BOOL  bReverse[8] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };
	static FLOAT fLerp[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	if (bReverse[nRank])
		fLerp[nRank] -= fVar;
	else
		fLerp[nRank] += fVar;

	if (fLerp[nRank] > 1.0f)
	{
		bReverse[nRank] = TRUE;
		fLerp[nRank] = 1.0f;
	}
	else if (fLerp[nRank] < 0.0f)
	{
		bReverse[nRank] = FALSE;
		fLerp[nRank] = 0.0f;
	}

	D3DXColorLerp(&dwColor2, &dwColor, &dwColorDest2, fLerp[nRank]);

	CRect Rect = CRect(0, 0, rectBg.right - rectBg.left, rectBg.bottom - rectBg.top);

	p2DRender->RenderFillRect(CRect((Point.x + Rect.left) - thick - 1, Point.y + Rect.top - (thick / 2), (Point.x + Rect.left), Point.y + Rect.bottom + (thick / 2)),
		dwColor1, dwColor2, dwColor1, dwColor2);

	p2DRender->RenderFillRect(CRect((Point.x + Rect.left), (Point.y + Rect.top) - thick - 1, (Point.x + Rect.right), Point.y + Rect.top),
		dwColor1, dwColor1, dwColor2, dwColor2);

	p2DRender->RenderFillRect(CRect((Point.x + Rect.right), Point.y + Rect.top - (thick / 2), (Point.x + Rect.right) + thick, Point.y + Rect.bottom + (thick / 2)),
		dwColor2, dwColor1, dwColor2, dwColor1);

	p2DRender->RenderFillRect(CRect((Point.x + Rect.left), (Point.y + Rect.bottom), (Point.x + Rect.right), (Point.y + Rect.bottom) + thick),
		dwColor2, dwColor2, dwColor1, dwColor1);
}

void CWndMonsterClashRank::DrawOutLineFlash(C2DRender* p2DRender, const CRect rectBg, const DWORD dwColorstart, const DWORD dwColorend)
{
	if (m_bFlashBackground)
	{
		CPoint Point = CPoint(rectBg.TopLeft().x, rectBg.TopLeft().y);
		LONG thick = 6;

		D3DXCOLOR dwColorDest2 = dwColorstart;
		D3DXCOLOR dwColor = dwColorend;
		D3DXCOLOR dwColor1 = D3DCOLOR_ARGB(0, 255, 255, 255);
		D3DXCOLOR dwColor2 = D3DCOLOR_ARGB(0, 255, 255, 255);

		static BOOL  bReverse = FALSE;
		static FLOAT fLerp = 0.0f;

		if (bReverse)
			fLerp -= 0.15f;
		else
			fLerp += 0.15f;

		if (fLerp > 1.0f)
		{
			bReverse = TRUE;
			fLerp = 1.0f;
			m_bFlashBackground = FALSE;
		}
		else if (fLerp < 0.0f)
		{
			bReverse = FALSE;
			fLerp = 0.0f;
		}

		D3DXColorLerp(&dwColor2, &dwColor, &dwColorDest2, fLerp);

		CRect Rect = CRect(0, 0, rectBg.right - rectBg.left, rectBg.bottom - rectBg.top);

		p2DRender->RenderFillRect(CRect((Point.x + Rect.left) - thick, Point.y + Rect.top - (thick / 2), (Point.x + Rect.left), Point.y + Rect.bottom + (thick / 2)),
			dwColor1, dwColor2, dwColor1, dwColor2);

		p2DRender->RenderFillRect(CRect((Point.x + Rect.left), (Point.y + Rect.top) - thick, (Point.x + Rect.right), Point.y + Rect.top),
			dwColor1, dwColor1, dwColor2, dwColor2);

		p2DRender->RenderFillRect(CRect((Point.x + Rect.right), Point.y + Rect.top - (thick / 2), (Point.x + Rect.right) + thick, Point.y + Rect.bottom + (thick / 2)),
			dwColor2, dwColor1, dwColor2, dwColor1);

		p2DRender->RenderFillRect(CRect((Point.x + Rect.left), (Point.y + Rect.bottom), (Point.x + Rect.right), (Point.y + Rect.bottom) + thick),
			dwColor2, dwColor2, dwColor1, dwColor1);
	}
}

void CWndMonsterClashRank::DrawMonsterClashinfo(C2DRender* p2DRender)
{
	if (!IsActive())
		return;

	CPoint cPoint;
	CRect crBoard, crMyGuildBoard;
	DWORD dwBgColor;

	// Background Board Setting
	crBoard.left = 20;
	crBoard.top = 120;
	crBoard.right = crBoard.left + 270;
	crBoard.bottom = crBoard.top + 20;

	crMyGuildBoard.left = 10;
	crMyGuildBoard.top = 120;
	crMyGuildBoard.right = crMyGuildBoard.left + 305;
	crMyGuildBoard.bottom = crMyGuildBoard.top;

	CString strFormat;
	CD3DFont* pOldFont = p2DRender->GetFont();

	if (!g_pPlayer->GetGuild())
		return;

	int nRank = 0;
	for (const auto& GuildRank : vecGuild)
	{
		if (GuildRank.dwGuild == g_pPlayer->GetGuild()->m_idGuild)
		{
			dwBgColor = D3DCOLOR_ARGB(100, 0, 100, 255);

			p2DRender->SetFont(CWndBase::m_Theme.m_pFontSRMyGiuld);

			crMyGuildBoard.top = crBoard.top + 18;
			crMyGuildBoard.bottom = crMyGuildBoard.top + 25;

			crBoard.top += 15;
			crBoard.bottom += 15;

			cPoint.x = crMyGuildBoard.left + 6;
			cPoint.y = crMyGuildBoard.top + 4;

			p2DRender->RenderFillRect(crMyGuildBoard, dwBgColor);

			DrawMonsterClashGuildInfo(p2DRender, TRUE, nRank, GuildRank, CPoint(cPoint.x, cPoint.y), CPoint(crBoard.left + 16, crMyGuildBoard.top),
				CPoint(cPoint.x + 50, cPoint.y), CPoint(cPoint.x + 210, cPoint.y), CPoint(cPoint.x + 230, cPoint.y), 1.0f, 1.0f, crMyGuildBoard);
		}
		else
		{
			dwBgColor = D3DCOLOR_ARGB(100, 245, 204, 176);

			p2DRender->SetFont(CWndBase::m_Theme.m_pFontSRGiuld);

			crBoard.top += 25;
			crBoard.bottom += 25;

			cPoint.x = crBoard.left + 4;
			cPoint.y = crBoard.top + 4;

			p2DRender->RenderFillRect(crBoard, dwBgColor);

			DrawMonsterClashGuildInfo(p2DRender, FALSE, nRank, GuildRank, CPoint(cPoint.x, cPoint.y), CPoint(crBoard.left + 18, crBoard.top),
				CPoint(cPoint.x + 36, cPoint.y), CPoint(cPoint.x + 180, cPoint.y), CPoint(cPoint.x + 200, cPoint.y), 0.8f, 0.8f, crBoard);
		}
		p2DRender->SetFont(pOldFont);
		++nRank;
	}
}
#endif //__SYS_MONSTER_CLASH