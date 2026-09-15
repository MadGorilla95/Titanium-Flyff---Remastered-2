#include "stdafx.h"
#include "resData.h"
#include "defineText.h"

#ifdef __CLIENT
#ifdef __GUILDFINDER
#include "WndGuildFinder.h"
#include "DPClient.h"
#include "Guild.h"
#include "MsgHdr.h"

extern	CDPClient	g_DPlay;

bool CompType_Greater(GUILDFINDER_LIST& first, GUILDFINDER_LIST& second)
{
	return first.m_nSizeofMember > second.m_nSizeofMember;
}
bool CompType_Smaller(GUILDFINDER_LIST& first, GUILDFINDER_LIST& second)
{
	return first.m_nSizeofMember < second.m_nSizeofMember;
}

bool CompDeploy_Greater(GUILDFINDER_LIST& first, GUILDFINDER_LIST& second)
{
	return first.m_nLevel > second.m_nLevel;
}
bool CompDeploy_Smaller(GUILDFINDER_LIST& first, GUILDFINDER_LIST& second)
{
	return first.m_nLevel < second.m_nLevel;
}

bool CompName_Greater(GUILDFINDER_LIST& first, GUILDFINDER_LIST& second)
{
	if (strcmp(first.m_sGuild, second.m_sGuild) < 0)
		return true;
	else
		return false;
}
bool CompName_Smaller(GUILDFINDER_LIST& first, GUILDFINDER_LIST& second)
{
	if (strcmp(first.m_sGuild, second.m_sGuild) > 0)
		return true;
	else
		return false;
}

CWndGuildFinder::CWndGuildFinder()
{
	m_mapItem.clear();
	m_nSortType = WIDC_STATIC2;
	m_bIsGreater = TRUE;
	m_nSelected = 0;
}

CWndGuildFinder::~CWndGuildFinder()
{
}

void CWndGuildFinder::SerializeRegInfo(CAr& ar, DWORD& dwVersion)
{
	CWndNeuz::SerializeRegInfo(ar, dwVersion);
}

void CWndGuildFinder::OnDraw(C2DRender* p2DRender)
{
	if (m_mapItem.size())
	{
		vector<GUILDFINDER_LIST>::iterator iter;
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
		LPWNDCTRL		pCustom = NULL;
		DWORD			dwColor;
		int				nIndex = 0;

		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

		int nListFontHeight = pWndListBox->GetFontHeight() + 1;

		pCustom = GetWndCtrl(WIDC_LISTBOX1);

#ifdef __AEGON_THEME_SWITCHER
		dwColor = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE);
#else
		dwColor = D3DCOLOR_ARGB(255, 0, 0, 255);
#endif

		int nDrawCount = 0;
		int nLine = 0;

		for (iter = m_mapItem.begin(); iter != m_mapItem.end(); ++iter)
		{
			if (m_nSelected != 0 && (m_nSelected - 1) == nLine)
#ifdef __AEGON_THEME_SWITCHER
				dwColor = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE);
#else
				dwColor = D3DCOLOR_ARGB(255, 0, 0, 255);
#endif
			else
#ifdef __AEGON_THEME_SWITCHER
				dwColor = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
#else
				dwColor = D3DCOLOR_ARGB(255, 0, 0, 255);
#endif

			if (nLine < pWndListBox->GetScrollPos())
			{
				nLine++;
				continue;
			}
			else
				nLine++;

			if (nDrawCount >= 8) continue;

			PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(iter->m_nLeaderId);
			p2DRender->TextOut(pCustom->rect.left + 25, pCustom->rect.top + 8 + (nIndex)*nListFontHeight, iter->m_sGuild, dwColor);

			if (pPlayerData)
				p2DRender->TextOut(pCustom->rect.left + 225, pCustom->rect.top + 8 + (nIndex)*nListFontHeight, pPlayerData->szPlayer, dwColor);

			p2DRender->TextOut(pCustom->rect.left + 380, pCustom->rect.top + 8 + (nIndex)*nListFontHeight, iter->m_nLevel, dwColor);
			CString str;
			str.Format("%d/%d", iter->m_nSizeofMember, iter->m_nMaxSize);
			p2DRender->TextOut(pCustom->rect.left + 450, pCustom->rect.top + 8 + (nIndex)*nListFontHeight, str, dwColor);

#ifdef __WBQT_GUILD_BUFFS
			CString GuildBuffs;
			if (iter->m_nLevel == 50) {
				GuildBuffs = "5 / 5";
			}
			else if (iter->m_nLevel >= 40) {
				GuildBuffs = "4 / 5";
			}
			else if (iter->m_nLevel >= 30) {
				GuildBuffs = "3 / 5";
			}
			else if (iter->m_nLevel >= 20) {
				GuildBuffs = "2 / 5";
			}
			else if (iter->m_nLevel >= 10) {
				GuildBuffs = "1 / 5";
			}
			else {
				GuildBuffs = "0 / 5";
			}
			p2DRender->TextOut(pCustom->rect.left + 530, pCustom->rect.top + 8 + (nIndex)*nListFontHeight, GuildBuffs, dwColor);
#endif //__WBQT_GUILD_BUFFS

			++nIndex;
			++nDrawCount;
		}
	}
}

void CWndGuildFinder::RefreshItemList()
{
	int	nIndex = 1;

	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	pWndListBox->ResetContent();

	for (vector<GUILDFINDER_LIST>::iterator iter = m_mapItem.begin(); iter != m_mapItem.end(); ++iter)
		pWndListBox->AddString(" ");

	Sort();
	m_nSelected = 0;
}

void CWndGuildFinder::Sort()
{
	if (m_mapItem.size() > 1)
	{
		switch (m_nSortType)
		{
		case WIDC_STATIC2:
			if (m_bIsGreater)
				std::sort(m_mapItem.begin(), m_mapItem.end(), CompType_Greater);
			else
				std::sort(m_mapItem.begin(), m_mapItem.end(), CompType_Smaller);
			break;
		case WIDC_STATIC3:
			if (m_bIsGreater)
				std::sort(m_mapItem.begin(), m_mapItem.end(), CompName_Greater);
			else
				std::sort(m_mapItem.begin(), m_mapItem.end(), CompName_Smaller);
			break;
		case WIDC_STATIC5:
			if (m_bIsGreater)
				std::sort(m_mapItem.begin(), m_mapItem.end(), CompDeploy_Greater);
			else
				std::sort(m_mapItem.begin(), m_mapItem.end(), CompDeploy_Smaller);
			break;
		};
	}
}

void CWndGuildFinder::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	g_DPlay.SendHdr(PACKETTYPE_GUILD_LIST);

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndGuildFinder::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILDFINDER, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndGuildFinder::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndGuildFinder::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndGuildFinder::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return TRUE;
}

void CWndGuildFinder::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndGuildFinder::OnLButtonDown(UINT nFlags, CPoint point)
{
}

BOOL CWndGuildFinder::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	int				nLoop = 0;

	switch (nID)
	{
	case WIDC_LISTBOX1:
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
		CWndButton* pWndButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		CWndButton* pWndButton2 = (CWndButton*)GetDlgItem(WIDC_BUTTON2);

		m_nSelected = pWndListBox->GetCurSel() + 1;
	}
	break;

	case WIDC_BUTTON1:
	{
		g_DPlay.SendHdr(PACKETTYPE_GUILD_LIST);
	}
	break;

	case WIDC_BUTTON2:
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);

		if (g_pPlayer->GetGuild())
		{
			g_WndMng.OpenMessageBox("You are already in a guild.");
			return CWndNeuz::OnChildNotify(message, nID, pLResult);
		}

		for (vector<GUILDFINDER_LIST>::iterator iter = m_mapItem.begin(); iter != m_mapItem.end(); ++iter)
		{
			++nLoop;
			if (m_nSelected > nLoop)	continue;
			else if (m_nSelected == nLoop)
			{
				g_DPlay.OnGuildRequest(iter->m_uGuildId, iter->m_nLeaderId);
				g_WndMng.PutString("Request send successfully.", NULL, COLOR_SUCCESS);
			}
		}
	}
	break;
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

CWndGuildFinderReq::CWndGuildFinderReq(LONG idPlayer, int nLevel, int nClass, BYTE cCheckhero)
{
	m_idPlayer = idPlayer;
	m_nLevel = nLevel;
	m_nClass = nClass;
	m_byCheckhero = cCheckhero;
}

CWndGuildFinderReq::~CWndGuildFinderReq()
{
}

void CWndGuildFinderReq::OnDraw(C2DRender* p2DRender)
{
}

void CWndGuildFinderReq::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();

	CString str;
	CWndStatic* pWndStaticName = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	CWndStatic* pWndStaticLevel = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	CWndStatic* pWndStaticClass = (CWndStatic*)GetDlgItem(WIDC_STATIC3);

	PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(m_idPlayer);
	if (pPlayerData)
	{
		if (pWndStaticName)
		{
			str.Format("Name: %s", pPlayerData->szPlayer);
			pWndStaticName->SetTitle(str);
		}
	}
	if (pWndStaticLevel)
	{
		str.Format("Level: %d", m_nLevel);
		pWndStaticLevel->SetTitle(str);
	}
	if (pWndStaticClass)
	{
		str.Format("Job: %s", prj.m_aJob[m_nClass].szName);
		pWndStaticClass->SetTitle(str);
	}
}

BOOL CWndGuildFinderReq::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILDFINDER_REQ, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndGuildFinderReq::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndGuildFinderReq::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndGuildFinderReq::OnLButtonUp(UINT nFlags, CPoint point)
{

}
void CWndGuildFinderReq::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndGuildFinderReq::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_BUTTON1:
	{
		GUILD_MEMBER_INFO	info;
		info.idPlayer = m_idPlayer;
		g_DPlay.SendAddGuildMember(g_pPlayer->m_idPlayer, info, g_pPlayer->IsAuthorization(AUTH_GAMEMASTER));
		Destroy();
	}
	break;

	case WIDC_BUTTON2:
	{
		Destroy();
	}
	break;
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#endif //__GUILDFINDER
#endif //__CLIENT
