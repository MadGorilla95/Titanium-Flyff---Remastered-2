#include "stdafx.h"
#include "WndPartyFind.h"
#include "ResData.h"
#include "DPClient.h"
#include "Party.h"
extern CDPClient g_DPlay;

#ifdef __SYS_PARTYFINDER
CWndPartyFind::CWndPartyFind()
{
	pWndListBox = NULL;
	pWndText = NULL;
	m_mapItem.clear();
	m_nSelected = 0;
}

CWndPartyFind::~CWndPartyFind()
{
	pWndListBox = NULL;
	pWndText = NULL;
	m_mapItem.clear();
	m_nSelected = 0;
}

void CWndPartyFind::RefreshItemList()
{
	for (vector<PARTYFINDER_LIST>::const_iterator itr = m_mapItem.begin(); itr != m_mapItem.end(); ++itr)
		pWndListBox->AddString(" ");
	m_nSelected = 0;
}

void CWndPartyFind::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	if (pWndListBox)
	{
		pWndListBox->ResetContent();
		g_DPlay.JoinParty();
	}
	pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	RefreshItemList();

	MoveParentCenter();
}

BOOL CWndPartyFind::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PARTY_FIND, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndPartyFind::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndPartyFind::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndPartyFind::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	int	nLoop = 0;
	switch (nID)
	{
	case WIDC_LISTBOX1:
	{
		pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
		m_nSelected = pWndListBox->GetCurSel() + 1;
		for (vector<PARTYFINDER_LIST>::const_iterator itr = m_mapItem.begin(); itr != m_mapItem.end(); ++itr)
		{
			++nLoop;
			if (m_nSelected > nLoop)
				continue;
			else if (m_nSelected == nLoop)
			{
				if (itr->m_sPartyList)
#ifdef __AEGON_THEME_SWITCHER
				pWndText->SetString(itr->m_sPartyList, CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
#else //__AEGON_THEME_SWITCHER
				pWndText->SetString(itr->m_sPartyList, 0xff000000);
#endif //__AEGON_THEME_SWITCHER
			}
		}
	}
	break;
	case WIDC_BUTTON1:
	{
		pWndListBox->ResetContent();
		g_DPlay.JoinParty();
	}
	break;
	case WIDC_BUTTON2:
	{
		for (vector<PARTYFINDER_LIST>::const_iterator itr = m_mapItem.begin(); itr != m_mapItem.end(); ++itr)
		{
			++nLoop;
			if (m_nSelected > nLoop)
				continue;
			else if (m_nSelected == nLoop)
			{
				g_DPlay.SendAddPartyMember(itr->m_nLeaderId, 0, 0, 0, 0, 0, 0, 0);
				Destroy();
			}
		}
	}
	break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

vector<PARTYFINDER_LIST> CWndPartyFind::GetPartyList() const
{
	return m_mapItem;
}
#endif // __SYS_PARTYFINDER