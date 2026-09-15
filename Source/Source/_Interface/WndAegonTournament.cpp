#include "stdafx.h"
#include "resData.h"
#include "WndAegonTournament.h"
#include "DPClient.h"
#include "defineText.h"
#include "WndManager.h"
#include "AegonTournament.h"
#include "WndManager.h"
#include "Model.h"
#include "ModelMng.h"
#include "ModelObject.h"
#include "ModelGlobal.h"
#include "math.h"
#include <ctime>
#include "defineObj.h"

#ifdef __AEGON_TOURNAMENT
extern CDPClient g_DPlay;
AegonTournamentWndHelper g_AegonTournamentWndHelper;

///////////////////////////
///////LIST WINDOW/////////
///////////////////////////
CWndTournamentRewardList::CWndTournamentRewardList()
{
	m_nRowHeight = 40;
	m_nSelectedItem = -1;
}
CWndTournamentRewardList::~CWndTournamentRewardList()
{
}
void CWndTournamentRewardList::OnDraw(C2DRender* p2DRender)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	CRect wndRect = GetWindowRect();
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	CPoint point = CPoint(0, 0);
	CString str;

	int nRange = 0;
	int nPage = wndRect.Height() / m_nRowHeight;

	nRange = m_tournamentRewards.m_vRewards.size();

	if (!nRange)
		nRange = 1;

	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);
	m_wndScrollBar.EnableScrollBar(TRUE);
	m_wndScrollBar.SetScrollStep(1);


	int nIndex = 0;
	for (int i = m_wndScrollBar.GetScrollPos(); i < m_tournamentRewards.m_vRewards.size(); i++, nIndex++)
	{
		point.x = 10;
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

		ItemProp* tmpProp = prj.GetItemProp(m_tournamentRewards.m_vRewards[i].m_dwItemID);
		CTexture* pActionTexture = NULL;
		if (tmpProp)
			pActionTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, tmpProp->szIcon).GetString(), COLOR_ITEM_BG);

		if (pActionTexture)
		{
			p2DRender->RenderTexture(CPoint(point.x, point.y + (m_nRowHeight / 2) - (pActionTexture->m_size.cy / 2)), pActionTexture);

			point.x += pActionTexture->m_size.cx + 10;
		}

		
		if (tmpProp) 
		{
			CString strAmount;
			strAmount.Format(" (%dx)", m_tournamentRewards.m_vRewards[i].m_dwItemNum);


			int nMaxLength = GetWndRect().Width() - p2DRender->GetFont()->GetTextExtent(strAmount.GetString()).cx - p2DRender->GetFont()->GetTextExtent("...").cx - 40;
			CString strName;
			strName.Format("%s", tmpProp->szName);
			CSize strExtentName = p2DRender->GetFont()->GetTextExtent(strName.GetString());
			if(strExtentName.cx > nMaxLength)
			{
				LONG letterLength = p2DRender->GetFont()->GetTextExtent("A").cx;
				int nMaxLetters = 0;
				if(letterLength != 0)
					nMaxLetters = floor((double)nMaxLength / (double)letterLength);

				if (nMaxLetters > strName.GetLength())
					nMaxLetters = strName.GetLength();

				strName = strName.Mid(0, nMaxLetters);
				strName += "...";
			}


			str.Format("%s%s", strName.GetString(), strAmount);
		}
		else
			str.Format("error");

		CSize strExtent = p2DRender->GetFont()->GetTextExtent(str.GetString());
		p2DRender->TextOut(point.x, point.y + (m_nRowHeight / 2) - (strExtent.cy / 2), str.GetString(), dwColorText);


		
	}


}
void CWndTournamentRewardList::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	CRect rect = GetWindowRect();

	m_wndScrollBar.Create(WBS_DOCKING | WBS_VERT, rect, this, 1000);
	m_wndScrollBar.SetVisible(IsWndStyle(WBS_VSCROLL));

}
BOOL CWndTournamentRewardList::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndBase::OnChildNotify(message, nID, pLResult);
}
BOOL CWndTournamentRewardList::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndBase::OnCommand(nID, dwMessage, pWndBase);
}
void CWndTournamentRewardList::OnSize(UINT nType, int cx, int cy)
{
	CWndBase::OnSize(nType, cx, cy);
}
void CWndTournamentRewardList::OnLButtonUp(UINT nFlags, CPoint point)
{

}
void CWndTournamentRewardList::OnLButtonDown(UINT nFlags, CPoint point)
{

}

void CWndTournamentRewardList::OnMouseWndSurface(CPoint point)
{
	int nScrollPos = m_wndScrollBar.GetScrollPos();
	CRect wndRect = GetWndRect();
	int nRowCount = ceil((float)wndRect.Height() / (float)m_nRowHeight);

	for (int i = 0; i < nRowCount; i++)
	{
		if (nScrollPos + i >= m_tournamentRewards.m_vRewards.size())
			continue;

		int nSelectedItem = nScrollPos + i;
		if (nSelectedItem < 0 || nSelectedItem >= m_tournamentRewards.m_vRewards.size())
			continue;

		__TOURNAMENT_SINGLE_REWARD* pUnit = &m_tournamentRewards.m_vRewards[nSelectedItem];

		if (!pUnit)
			continue;
		
		SIZE texSize;
		texSize.cx = 32;
		texSize.cy = 32;
		

		CRect tmpRect;
		tmpRect.left = 10;
		tmpRect.top = 0 + i * m_nRowHeight;
		tmpRect.right = tmpRect.left + texSize.cx;
		tmpRect.bottom = tmpRect.top + m_nRowHeight;

		CRect rectIcon = CRect(tmpRect.left, tmpRect.CenterPoint().y - texSize.cy / 2, tmpRect.right, tmpRect.CenterPoint().y + texSize.cy / 2);


		if (!rectIcon.PtInRect(point))
			continue;

		
		CRect hitrect = rectIcon;

		CPoint point2 = point;
		ClientToScreen(&point2);
		ClientToScreen(&hitrect);

		CItemElem itemElem;
		itemElem.m_dwItemId = pUnit->m_dwItemID;
		itemElem.m_nItemNum = pUnit->m_dwItemNum;

		g_WndMng.PutToolTip_Item(&itemElem, point2, &hitrect);
		
		break;

	}
}
void CWndTournamentRewardList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}
void CWndTournamentRewardList::OnRButtonUp(UINT nFlags, CPoint point)
{
}
void CWndTournamentRewardList::SetTournamentRewards(__TOURNAMENT_REWARD* pTournamentRewards)
{
	m_tournamentRewards = *pTournamentRewards;
}

///////////////////////////
/////CREATE TOURNAMENT//////
///////////////////////////
CWndAegonCreateTournament::CWndAegonCreateTournament()
{
	m_pWndComboBoxPresets = NULL;

	m_pWndTeamSize = NULL;
	m_pWndRounds = NULL;
	m_pWndRoundTime = NULL;

	m_pWndStartTimeHour = NULL;
	m_pWndStartTimeMin = NULL;
	m_pWndStartTimeDay = NULL;
	m_pWndStartTimeMonth = NULL;
	m_pWndStartTimeYear = NULL;

	m_pWndFriendlyFire = NULL;

	m_pWndLevelRangeMin = NULL;
	m_pWndLevelRangeMax = NULL;

	m_pWndClasses = NULL;
	m_pWndTeamReq = NULL;

	m_pWndRewardList = NULL;
	m_pWndRewardID = NULL;
	m_pWndRewardNum = NULL;

	m_pWndRewardParticipation = NULL;
	m_pWndRewardFirstPlace = NULL;
	m_pWndRewardSecondPlace = NULL;

	sTournamentUnit.Init();

}
CWndAegonCreateTournament::~CWndAegonCreateTournament()
{
}
void CWndAegonCreateTournament::OnDraw(C2DRender* p2DRender)
{
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();

	CWndStatic* pWndStaticStartTime = (CWndStatic*)GetDlgItem(WIDC_STATIC5);
	CWndButton* pWndButtonCreate = (CWndButton*)GetDlgItem(WIDC_BUTTON1);


	if (!m_pWndComboBoxPresets || !pWndStaticStartTime || !m_pWndStartTimeYear || !pWndButtonCreate || !m_pWndFriendlyFire)
		return;


	CPoint p1, p2;
	CRect tmpRect;

	p1 = CPoint(0, m_pWndComboBoxPresets->GetWndRect().bottom + 5);
	p2 = CPoint(GetWndRect().Width(), m_pWndComboBoxPresets->GetWndRect().bottom + 5);
	p2DRender->RenderLine(p1, p2, dwColorText);


	tmpRect = pWndStaticStartTime->GetWndRect();
	tmpRect.top = tmpRect.bottom + 5;
	tmpRect.bottom = m_pWndStartTimeYear->GetWndRect().bottom + 5;
	p2DRender->RenderRect(tmpRect, dwColorText);
	

	p1 = CPoint(0, pWndButtonCreate->GetWndRect().top - 10);
	p2 = CPoint(GetWndRect().Width(), pWndButtonCreate->GetWndRect().top - 10);
	p2DRender->RenderLine(p1, p2, dwColorText);
	

	p1 = CPoint(m_pWndFriendlyFire->GetWndRect().right + 10, m_pWndComboBoxPresets->GetWndRect().bottom + 5);
	p2 = CPoint(m_pWndFriendlyFire->GetWndRect().right + 10, pWndButtonCreate->GetWndRect().top - 10);
	p2DRender->RenderLine(p1, p2, dwColorText);

}
void CWndAegonCreateTournament::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	//Move Window to center
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();


	m_pWndComboBoxPresets = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	if (m_pWndComboBoxPresets) 
	{
		m_pWndComboBoxPresets->AddWndStyle(EBS_READONLY);


		vector<__TOURNAMENT_UNIT>* pvPresets = &CAegonTournamentMng::GetInstance()->m_vTournamentPresets;

		for(int i = 0; i < pvPresets->size(); i++)
		{
			CString tmpStr;
			CString strClasses;
			
			if ((*pvPresets)[i].m_dwAllowedClasses == 0)
			{
				strClasses.Format("All Classes");
			}
			else 
			{
				if ((*pvPresets)[i].m_dwAllowedClasses < MAX_JOB)
				{
					strClasses.Format("%s", prj.m_aJob[(*pvPresets)[i].m_dwAllowedClasses].szName);
				}
			}

			tmpStr.Format("%dvs%d - %s", (*pvPresets)[i].m_dwTeamSize, (*pvPresets)[i].m_dwTeamSize, strClasses.GetString());
			m_pWndComboBoxPresets->AddString(tmpStr.GetString());
			m_pWndComboBoxPresets->SetItemData(m_pWndComboBoxPresets->m_wndListBox.GetCount() - 1, i);
		}
		if (m_pWndComboBoxPresets->m_wndListBox.GetCount() > 0)
			m_pWndComboBoxPresets->SetCurSel(0);
	}

	m_pWndTeamSize = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	if (m_pWndTeamSize) 
	{
		m_pWndTeamSize->AddWndStyle(EBS_NUMBER);
	}

	m_pWndRounds = (CWndEdit*)GetDlgItem(WIDC_EDIT2);
	if (m_pWndRounds) 
	{
		m_pWndRounds->AddWndStyle(EBS_NUMBER);
	}

	m_pWndRoundTime = (CWndEdit*)GetDlgItem(WIDC_EDIT3);
	if (m_pWndRoundTime)
	{
		m_pWndRounds->AddWndStyle(EBS_NUMBER);
	}

	m_pWndStartTimeHour = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);
	if (m_pWndStartTimeHour)
	{
		m_pWndStartTimeHour->AddWndStyle(EBS_READONLY);
		CString tmpStr;
		for (int i = 0; i < 24; i++) 
		{
			tmpStr.Format("%d", i);
			m_pWndStartTimeHour->AddString(tmpStr.GetString());
			m_pWndStartTimeHour->SetItemData(m_pWndStartTimeHour->m_wndListBox.GetCount() - 1, i);
		}
	}
	m_pWndStartTimeMin = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX3);
	if (m_pWndStartTimeMin)
	{
		m_pWndStartTimeMin->AddWndStyle(EBS_READONLY);
		CString tmpStr;
		for (int i = 0; i < 60; i++)
		{
			tmpStr.Format("%d", i);
			m_pWndStartTimeMin->AddString(tmpStr.GetString());
			m_pWndStartTimeMin->SetItemData(m_pWndStartTimeMin->m_wndListBox.GetCount() - 1, i);
		}
	}
	m_pWndStartTimeDay = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX4);
	if (m_pWndStartTimeDay)
	{
		m_pWndStartTimeDay->AddWndStyle(EBS_READONLY);
		CString tmpStr;
		for (int i = 1; i < 32; i++)
		{
			tmpStr.Format("%d", i);
			m_pWndStartTimeDay->AddString(tmpStr.GetString());
			m_pWndStartTimeDay->SetItemData(m_pWndStartTimeDay->m_wndListBox.GetCount() - 1, i);
		}
	}
	m_pWndStartTimeMonth = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX5);
	if (m_pWndStartTimeMonth)
	{
		m_pWndStartTimeMonth->AddWndStyle(EBS_READONLY);
		CString tmpStr;
		for (int i = 1; i < 13; i++)
		{
			tmpStr.Format("%d", i);
			m_pWndStartTimeMonth->AddString(tmpStr.GetString());
			m_pWndStartTimeMonth->SetItemData(m_pWndStartTimeMonth->m_wndListBox.GetCount() - 1, i - 1);
		}
	}
	m_pWndStartTimeYear = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX6);
	if (m_pWndStartTimeYear)
	{
		m_pWndStartTimeYear->AddWndStyle(EBS_READONLY);
		CString tmpStr;
		for (int i = 2021; i < 2031; i++)
		{
			tmpStr.Format("%d", i);
			m_pWndStartTimeYear->AddString(tmpStr.GetString());
			m_pWndStartTimeYear->SetItemData(m_pWndStartTimeYear->m_wndListBox.GetCount() - 1, i - 1900);
		}
	}

	m_pWndFriendlyFire = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX7);
	if (m_pWndFriendlyFire)
	{
		m_pWndFriendlyFire->AddWndStyle(EBS_READONLY);

		m_pWndFriendlyFire->AddString("On");
		m_pWndFriendlyFire->SetItemData(m_pWndFriendlyFire->m_wndListBox.GetCount() - 1, 1);

		m_pWndFriendlyFire->AddString("Off");
		m_pWndFriendlyFire->SetItemData(m_pWndFriendlyFire->m_wndListBox.GetCount() - 1, 0);
	}

	m_pWndLevelRangeMin = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX8);
	if (m_pWndLevelRangeMin) 
	{
		m_pWndLevelRangeMin->AddWndStyle(EBS_READONLY);
		CString tmpStr;
		for (int i = 0; i < MAX_CHARACTER_LEVEL; i++) 
		{
			tmpStr.Format("%d", i + 1);
			m_pWndLevelRangeMin->AddString(tmpStr.GetString());
			m_pWndLevelRangeMin->SetItemData(m_pWndFriendlyFire->m_wndListBox.GetCount() - 1, i + 1);
		}
	}
	m_pWndLevelRangeMax = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX9);
	if (m_pWndLevelRangeMax)
	{
		m_pWndLevelRangeMax->AddWndStyle(EBS_READONLY);
		CString tmpStr;
		for (int i = 0; i < MAX_CHARACTER_LEVEL; i++)
		{
			tmpStr.Format("%d", i + 1);
			m_pWndLevelRangeMax->AddString(tmpStr.GetString());
			m_pWndLevelRangeMax->SetItemData(m_pWndFriendlyFire->m_wndListBox.GetCount() - 1, i + 1);
		}
	}

	m_pWndClasses = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX10);
	if (m_pWndClasses) 
	{
		m_pWndClasses->AddWndStyle(EBS_READONLY);
		m_pWndClasses->AddString("All");
		m_pWndClasses->SetItemData(m_pWndFriendlyFire->m_wndListBox.GetCount() - 1, 0);
		for (int i = 32; i < MAX_JOB; i++)
		{
			m_pWndClasses->AddString(prj.m_aJob[i].szName);
			m_pWndClasses->SetItemData(m_pWndClasses->m_wndListBox.GetCount() - 1, i);
			
		}
	}
	m_pWndTeamReq = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX11);
	if (m_pWndTeamReq)
	{
		m_pWndTeamReq->AddWndStyle(EBS_READONLY);
		CString tmpStr;
		for (int i = 2; i < 9; i++)
		{
			tmpStr.Format("%d", i);
			m_pWndTeamReq->AddString(tmpStr.GetString());
			m_pWndTeamReq->SetItemData(m_pWndFriendlyFire->m_wndListBox.GetCount() - 1, i);

		}
	}

	m_pWndRewardList = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX12);
	if (m_pWndRewardList) 
	{
		m_pWndRewardList->AddWndStyle(EBS_READONLY);

		m_pWndRewardList->AddString("All Participants");

		m_pWndRewardList->AddString("First Place");

		m_pWndRewardList->AddString("Second Place");

		if (m_pWndRewardList->m_wndListBox.GetCount() > 0)
			m_pWndRewardList->SetCurSel(0);
	}
	m_pWndRewardID = (CWndEdit*)GetDlgItem(WIDC_EDIT4);
	if (m_pWndRewardID) 
	{
		m_pWndRewardID->AddWndStyle(EBS_NUMBER);
		m_pWndRewardID->SetMaxStringNumber(9);

	}
	m_pWndRewardNum = (CWndEdit*)GetDlgItem(WIDC_EDIT5);
	if (m_pWndRewardNum)
	{
		m_pWndRewardNum->AddWndStyle(EBS_NUMBER);
		m_pWndRewardNum->SetMaxStringNumber(4);
	}

	m_pWndRewardParticipation = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	m_pWndRewardFirstPlace = (CWndListBox*)GetDlgItem(WIDC_LISTBOX2);
	m_pWndRewardSecondPlace = (CWndListBox*)GetDlgItem(WIDC_LISTBOX3);

	FillPresetInfo();
}
BOOL CWndAegonCreateTournament::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_AEGON_CREATE_TOURNAMENT, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndAegonCreateTournament::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndAegonCreateTournament::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndAegonCreateTournament::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndAegonCreateTournament::OnLButtonDown(UINT nFlags, CPoint point)
{

}
BOOL CWndAegonCreateTournament::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_BUTTON1: //Create Tournament
	{
		CreateTournament();
		break;
	}
	case WIDC_BUTTON2: //Add Reward
	{
		AddTournamentReward();
		
		break;
	}
	case WIDC_BUTTON3: //Clear Reward List
	{
		ClearRewardList();
		break;
	}
	case WIDC_COMBOBOX1:
	{
		FillPresetInfo();
		break;
	}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndAegonCreateTournament::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}
BOOL CWndAegonCreateTournament::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	return TRUE;
}
void CWndAegonCreateTournament::FillPresetInfo() 
{
	if (m_pWndComboBoxPresets && m_pWndComboBoxPresets->m_wndListBox.GetCount() > 0)
	{
		DWORD dwPresetID = m_pWndComboBoxPresets->GetItemData(m_pWndComboBoxPresets->GetCurSel());

		if (dwPresetID >= CAegonTournamentMng::GetInstance()->m_vTournamentPresets.size())
			return;

		__TOURNAMENT_UNIT* pSelectedPreset = &CAegonTournamentMng::GetInstance()->m_vTournamentPresets[dwPresetID];
		
		if(pSelectedPreset)
		{
			CString tmpStr;
			int nSelect;
			tmpStr.Format("%d", pSelectedPreset->m_dwTeamSize);
			m_pWndTeamSize->SetString(tmpStr.GetString());

			tmpStr.Format("%d", pSelectedPreset->m_dwRounds);
			m_pWndRounds->SetString(tmpStr.GetString());

			tmpStr.Format("%d", pSelectedPreset->m_dwRoundTime);
			m_pWndRoundTime->SetString(tmpStr.GetString());

			tmpStr.Format("%d", pSelectedPreset->m_sStartTime.tm_hour);
			nSelect = GetComboBoxStringIndex(m_pWndStartTimeHour, tmpStr.GetString(), TRUE);
			if(nSelect >= 0 && nSelect < m_pWndStartTimeHour->m_wndListBox.GetCount())
				m_pWndStartTimeHour->SetCurSel(nSelect);
			else if(m_pWndStartTimeHour->m_wndListBox.GetCount() > 0)
				m_pWndStartTimeHour->SetCurSel(0);
			

			tmpStr.Format("%d", pSelectedPreset->m_sStartTime.tm_min);
			nSelect = GetComboBoxStringIndex(m_pWndStartTimeMin, tmpStr.GetString(), TRUE);
			if (nSelect >= 0 && nSelect < m_pWndStartTimeMin->m_wndListBox.GetCount())
				m_pWndStartTimeMin->SetCurSel(nSelect);
			else if (m_pWndStartTimeMin->m_wndListBox.GetCount() > 0)
				m_pWndStartTimeMin->SetCurSel(0);

			tmpStr.Format("%d", pSelectedPreset->m_sStartTime.tm_mday);
			nSelect = GetComboBoxStringIndex(m_pWndStartTimeDay, tmpStr.GetString(), TRUE);
			if (nSelect >= 0 && nSelect < m_pWndStartTimeDay->m_wndListBox.GetCount())
				m_pWndStartTimeDay->SetCurSel(nSelect);
			else if (m_pWndStartTimeDay->m_wndListBox.GetCount() > 0)
				m_pWndStartTimeDay->SetCurSel(0);


			tmpStr.Format("%d", pSelectedPreset->m_sStartTime.tm_mon + 1);
			nSelect = GetComboBoxStringIndex(m_pWndStartTimeMonth, tmpStr.GetString(), TRUE);
			if (nSelect >= 0 && nSelect < m_pWndStartTimeMonth->m_wndListBox.GetCount())
				m_pWndStartTimeMonth->SetCurSel(nSelect);
			else if (m_pWndStartTimeMonth->m_wndListBox.GetCount() > 0)
				m_pWndStartTimeMonth->SetCurSel(0);

			tmpStr.Format("%d", pSelectedPreset->m_sStartTime.tm_year + 1900);
			nSelect = GetComboBoxStringIndex(m_pWndStartTimeYear, tmpStr.GetString(), TRUE);
			if (nSelect >= 0 && nSelect < m_pWndStartTimeYear->m_wndListBox.GetCount())
				m_pWndStartTimeYear->SetCurSel(nSelect);
			else if (m_pWndStartTimeYear->m_wndListBox.GetCount() > 0)
				m_pWndStartTimeYear->SetCurSel(0);

			if(pSelectedPreset->m_bFriendlyFire)
				tmpStr.Format("On");
			else
				tmpStr.Format("Off");
			nSelect = GetComboBoxStringIndex(m_pWndFriendlyFire, tmpStr.GetString(), TRUE);
			if (nSelect >= 0 && nSelect < m_pWndFriendlyFire->m_wndListBox.GetCount())
				m_pWndFriendlyFire->SetCurSel(nSelect);
			else if (m_pWndFriendlyFire->m_wndListBox.GetCount() > 0)
				m_pWndFriendlyFire->SetCurSel(0);


			tmpStr.Format("%d", pSelectedPreset->m_dwLevelReqMin);
			nSelect = GetComboBoxStringIndex(m_pWndLevelRangeMin, tmpStr.GetString(), TRUE);
			if (nSelect >= 0 && nSelect < m_pWndLevelRangeMin->m_wndListBox.GetCount())
				m_pWndLevelRangeMin->SetCurSel(nSelect);
			else if (m_pWndLevelRangeMin->m_wndListBox.GetCount() > 0)
				m_pWndLevelRangeMin->SetCurSel(0);

			tmpStr.Format("%d", pSelectedPreset->m_dwLevelReqMax);
			nSelect = GetComboBoxStringIndex(m_pWndLevelRangeMax, tmpStr.GetString(), TRUE);
			if (nSelect >= 0 && nSelect < m_pWndLevelRangeMax->m_wndListBox.GetCount())
				m_pWndLevelRangeMax->SetCurSel(nSelect);
			else if (m_pWndLevelRangeMax->m_wndListBox.GetCount() > 0)
				m_pWndLevelRangeMax->SetCurSel(0);

			if (pSelectedPreset->m_dwAllowedClasses == 0)
				tmpStr.Format("All");
			else
				tmpStr.Format("%s", prj.m_aJob[pSelectedPreset->m_dwAllowedClasses].szName);
			nSelect = GetComboBoxStringIndex(m_pWndClasses, tmpStr.GetString(), TRUE);
			if (nSelect >= 0 && nSelect < m_pWndClasses->m_wndListBox.GetCount())
				m_pWndClasses->SetCurSel(nSelect);
			else if (m_pWndClasses->m_wndListBox.GetCount() > 0)
				m_pWndClasses->SetCurSel(0);

			
			tmpStr.Format("%d", pSelectedPreset->m_dwTeamReqNum);
			nSelect = GetComboBoxStringIndex(m_pWndTeamReq, tmpStr.GetString(), TRUE);
			if (nSelect >= 0 && nSelect < m_pWndTeamReq->m_wndListBox.GetCount())
				m_pWndTeamReq->SetCurSel(nSelect);
			else if (m_pWndTeamReq->m_wndListBox.GetCount() > 0)
				m_pWndTeamReq->SetCurSel(0);


			sTournamentUnit.sRewardAll = pSelectedPreset->sRewardAll;
			sTournamentUnit.sRewardFirstPlace = pSelectedPreset->sRewardFirstPlace;
			sTournamentUnit.sRewardSecondPlace = pSelectedPreset->sRewardSecondPlace;
		}
	}

	UpdateRewardLists();
}
int CWndAegonCreateTournament::GetComboBoxStringIndex(CWndComboBox* pComboBox, LPCSTR szString, BOOL bNoCase)
{
	int nOut = -1;
	for (int i = 0; i < pComboBox->m_wndListBox.GetCount(); i++) 
	{
		if (bNoCase)
			if (pComboBox->m_wndListBox.GetString(i).CompareNoCase(szString) == 0) 
			{
				nOut = i;
				break;
			}
		else
			if (pComboBox->m_wndListBox.GetString(i).Compare(szString) == 0)
			{
				nOut = i;
				break;
			}
	}
	return nOut;
}
void CWndAegonCreateTournament::AddTournamentReward() 
{
	if (!m_pWndRewardList || !m_pWndRewardID || !m_pWndRewardNum)
		return;

	__TOURNAMENT_REWARD* pList = NULL;
	int dwList = m_pWndRewardList->GetCurSel();
	DWORD dwItemID = atoi(m_pWndRewardID->GetString());
	int nItemNum = atoi(m_pWndRewardNum->GetString());

	if (dwItemID >= prj.m_aPropItem.GetSize()) 
	{
		g_WndMng.PutString("Invalid itemID", 0, COLOR_ERROR);
		return;
	}
		

	if (nItemNum < 1 || nItemNum > 9999)
		return;

	ItemProp* pProp = prj.GetItemProp(dwItemID);
	if (!pProp) 
	{
		g_WndMng.PutString("item not found.", 0, COLOR_ERROR);
		return;
	}
		
	
	switch (dwList) 
	{
	case 0:
		pList = &sTournamentUnit.sRewardAll;
		break;
	case 1:
		pList = &sTournamentUnit.sRewardFirstPlace;
		break;
	case 2:
		pList = &sTournamentUnit.sRewardSecondPlace;
		break;
	}
	if (!pList)
		return;
	__TOURNAMENT_SINGLE_REWARD tmpReward;
	tmpReward.m_dwItemID = dwItemID;
	tmpReward.m_dwItemNum = nItemNum;
	pList->m_vRewards.push_back(tmpReward);
	

	UpdateRewardLists();
}
void CWndAegonCreateTournament::UpdateRewardLists()
{
	if (!m_pWndRewardParticipation || !m_pWndRewardFirstPlace || !m_pWndRewardSecondPlace)
		return;

	for (int i = m_pWndRewardParticipation->GetCount() - 1; i >= 0; i--)
	{
		m_pWndRewardParticipation->DeleteString(i);
	}
	for (int i = m_pWndRewardFirstPlace->GetCount() - 1; i >= 0; i--)
	{
		m_pWndRewardFirstPlace->DeleteString(i);
	}
	for (int i = m_pWndRewardSecondPlace->GetCount() - 1; i >= 0; i--)
	{
		m_pWndRewardSecondPlace->DeleteString(i);
	}

	CString strTmp;
	for (int i = 0; i < sTournamentUnit.sRewardAll.m_vRewards.size(); i++) 
	{
		ItemProp* tmpProp = prj.GetItemProp(sTournamentUnit.sRewardAll.m_vRewards[i].m_dwItemID);
		if (!tmpProp) 
		{
			sTournamentUnit.sRewardAll.m_vRewards.clear();
			break;
		}

		strTmp.Format("'%s' (%dx)", tmpProp->szName, sTournamentUnit.sRewardAll.m_vRewards[i].m_dwItemNum);
		m_pWndRewardParticipation->AddString(strTmp.GetString());
	}
	for (int i = 0; i < sTournamentUnit.sRewardFirstPlace.m_vRewards.size(); i++)
	{
		ItemProp* tmpProp = prj.GetItemProp(sTournamentUnit.sRewardFirstPlace.m_vRewards[i].m_dwItemID);
		if (!tmpProp)
		{
			sTournamentUnit.sRewardFirstPlace.m_vRewards.clear();
			break;
		}

		strTmp.Format("'%s' (%dx)", tmpProp->szName, sTournamentUnit.sRewardFirstPlace.m_vRewards[i].m_dwItemNum);
		m_pWndRewardFirstPlace->AddString(strTmp.GetString());
	}
	for (int i = 0; i < sTournamentUnit.sRewardSecondPlace.m_vRewards.size(); i++)
	{
		ItemProp* tmpProp = prj.GetItemProp(sTournamentUnit.sRewardSecondPlace.m_vRewards[i].m_dwItemID);
		if (!tmpProp)
		{
			sTournamentUnit.sRewardSecondPlace.m_vRewards.clear();
			break;
		}

		strTmp.Format("'%s' (%dx)", tmpProp->szName, sTournamentUnit.sRewardSecondPlace.m_vRewards[i].m_dwItemNum);
		m_pWndRewardSecondPlace->AddString(strTmp.GetString());
	}
}
void CWndAegonCreateTournament::ClearRewardList()
{
	if (!m_pWndRewardList)
		return;

	__TOURNAMENT_REWARD* pList = NULL;
	int dwList = m_pWndRewardList->GetCurSel();
	switch (dwList)
	{
	case 0:
		pList = &sTournamentUnit.sRewardAll;
		break;
	case 1:
		pList = &sTournamentUnit.sRewardFirstPlace;
		break;
	case 2:
		pList = &sTournamentUnit.sRewardSecondPlace;
		break;
	}
	if (!pList)
		return;
	pList->m_vRewards.clear();

	UpdateRewardLists();
}
void CWndAegonCreateTournament::CreateTournament() 
{
	FillTournamentUnit();
	g_DPlay.SendCreateTournament(&sTournamentUnit);
}
void CWndAegonCreateTournament::FillTournamentUnit() 
{
	if (!m_pWndTeamSize || !m_pWndRounds || !m_pWndRoundTime || !m_pWndStartTimeHour || !m_pWndStartTimeMin || !m_pWndStartTimeDay 
		|| !m_pWndStartTimeMonth || !m_pWndStartTimeYear || !m_pWndFriendlyFire || !m_pWndLevelRangeMin || !m_pWndLevelRangeMax 
		|| !m_pWndClasses || !m_pWndTeamReq)
	{
		g_WndMng.PutString("Error - a window element failed to load", 0, COLOR_ERROR);
		return;
	}

	sTournamentUnit.m_dwTeamSize = atoi(m_pWndTeamSize->GetString());
	sTournamentUnit.m_dwRounds = atoi(m_pWndRounds->GetString());
	sTournamentUnit.m_dwRoundTime = atoi(m_pWndRoundTime->GetString());
	sTournamentUnit.m_sStartTime.tm_hour = m_pWndStartTimeHour->GetCurSel();
	sTournamentUnit.m_sStartTime.tm_min = m_pWndStartTimeMin->GetCurSel();
	sTournamentUnit.m_sStartTime.tm_mday = m_pWndStartTimeDay->GetCurSel() + 1;
	sTournamentUnit.m_sStartTime.tm_mon = m_pWndStartTimeMonth->GetCurSel();
	sTournamentUnit.m_sStartTime.tm_year = m_pWndStartTimeYear->GetItemData(m_pWndStartTimeYear->GetCurSel());
	sTournamentUnit.m_sStartTime.tm_isdst = 0;
	sTournamentUnit.m_bFriendlyFire = m_pWndFriendlyFire->GetItemData(m_pWndFriendlyFire->GetCurSel()) == 0 ? FALSE : TRUE;
	sTournamentUnit.m_dwLevelReqMin = m_pWndLevelRangeMin->GetCurSel() + 1;
	sTournamentUnit.m_dwLevelReqMax = m_pWndLevelRangeMax->GetCurSel() + 1;
	sTournamentUnit.m_dwAllowedClasses = m_pWndClasses->GetItemData(m_pWndClasses->GetCurSel());
	sTournamentUnit.m_dwTeamReqNum = atoi(m_pWndTeamReq->GetString());
}
////////////////////////////
//////TOURNAMENT LIST///////
///////////////////////////
CWndAegonTournamentList::CWndAegonTournamentList()
{
	m_nRowHeight = 40;
	m_nSelectedItem = -1;
}
CWndAegonTournamentList::~CWndAegonTournamentList()
{
}
void CWndAegonTournamentList::OnDraw(C2DRender* p2DRender)
{
	CRect rect = GetWindowRect();

	int nRange = 0;
	int nPage = rect.Height() / m_nRowHeight;

	nRange = CAegonTournamentMng::GetInstance()->m_mQueuedTournaments.size();

	if (!nRange)
		nRange = 1;

	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);
	m_wndScrollBar.EnableScrollBar(TRUE);
	m_wndScrollBar.SetScrollStep(1);


	CRect wndRect = GetWndRect();
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	CPoint point = CPoint(0, 0);
	int drawOffsetY = m_wndScrollBar.GetScrollPos() * m_nRowHeight;
	CString str;

#ifdef __AEGON_THEME_SWITCHER
	DWORD RenderFillRect = CAegonThemeSwitcherMng::GetInstance()->GetRenderFillRectColor();
	DWORD RenderRect = CAegonThemeSwitcherMng::GetInstance()->GetRenderRectColor();
#endif //__AEGON_THEME_SWITCHER



	map<DWORD, __TOURNAMENT_UNIT>::iterator it = CAegonTournamentMng::GetInstance()->m_mQueuedTournaments.begin();
	int nIndex = 0;
	for (; it != CAegonTournamentMng::GetInstance()->m_mQueuedTournaments.end(); it++, nIndex++)
	{
		point.x = 10;
		point.y = nIndex * m_nRowHeight;



		if (point.y + m_nRowHeight - drawOffsetY >= m_nRowHeight)
			p2DRender->RenderLine(CPoint(0, point.y + m_nRowHeight - drawOffsetY), CPoint(wndRect.Width() - 30, point.y + m_nRowHeight - drawOffsetY), dwColorText);

		if (m_nSelectedItem == nIndex + m_wndScrollBar.GetScrollPos())
		{
			CRect rectSelected = CRect(0, point.y, wndRect.Width() - 30, point.y + m_nRowHeight);
#ifdef __AEGON_THEME_SWITCHER
			p2DRender->RenderFillRect(rectSelected, RenderFillRect);
			p2DRender->RenderRect(rectSelected, RenderRect);
#else //__AEGON_THEME_SWITCHER
			p2DRender->RenderFillRect(rectSelected, 0xFFf8e6cc);
			p2DRender->RenderRect(rectSelected, 0xFFf28818);
#endif //__AEGON_THEME_SWITCHER
		}



		CString strClasses;
		if (it->second.m_dwAllowedClasses == 0)
			strClasses.Format("All Classes");
		else
			strClasses.Format("%s", prj.m_aJob[it->second.m_dwAllowedClasses].szName);
		str.Format("%dvs%d %s", it->second.m_dwTeamSize, it->second.m_dwTeamSize, strClasses.GetString());
		CSize strExtent = p2DRender->GetFont()->GetTextExtent(str.GetString());
		p2DRender->TextOut(point.x, point.y - drawOffsetY + (m_nRowHeight / 2) - (strExtent.cy / 2), str.GetString(), dwColorText);

		str.Format("%02d.%02d.%04d", it->second.m_sStartTime.tm_mday, it->second.m_sStartTime.tm_mon + 1, it->second.m_sStartTime.tm_year + 1900);
		strExtent = p2DRender->GetFont()->GetTextExtent(str.GetString());
		p2DRender->TextOut(wndRect.Width() - 32 - strExtent.cx, point.y - drawOffsetY + (m_nRowHeight / 2) - (strExtent.cy / 2), str.GetString(), dwColorText);


		
	}


}
void CWndAegonTournamentList::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();
	CWndListBox::OnInitialUpdate();

	CRect rect = GetWindowRect();
	//m_wndScrollBar.AddWndStyle(WBS_DOCKING);
	//m_wndScrollBar.Create(WBS_VERT, rect, this, 1000);

}
BOOL CWndAegonTournamentList::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return FALSE;
}
BOOL CWndAegonTournamentList::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndBase::OnChildNotify(message, nID, pLResult);
}
BOOL CWndAegonTournamentList::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndBase::OnCommand(nID, dwMessage, pWndBase);
}
void CWndAegonTournamentList::OnSize(UINT nType, int cx, int cy)
{
	CWndBase::OnSize(nType, cx, cy);
}
void CWndAegonTournamentList::OnLButtonUp(UINT nFlags, CPoint point)
{

}
void CWndAegonTournamentList::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nScrollPos = m_wndScrollBar.GetScrollPos();
	CRect wndRect = GetWndRect();
	int nRowCount = ceil((float)wndRect.Height() / (float)m_nRowHeight);

	for (int i = 0; i < nRowCount; i++)
	{
		if (nScrollPos + i >= CAegonTournamentMng::GetInstance()->m_mQueuedTournaments.size())
			break;

		CRect tmpRect;
		tmpRect.left = 0;
		tmpRect.top = 0 + i * m_nRowHeight;
		tmpRect.right = wndRect.Width() - 30;
		tmpRect.bottom = tmpRect.top + m_nRowHeight;

		if (tmpRect.PtInRect(point))
		{
			m_nSelectedItem = nScrollPos + i;
			SelectionChanged();
			return;
		}
	}
	m_nSelectedItem = -1;
	SelectionChanged();

}

void CWndAegonTournamentList::OnMouseWndSurface(CPoint point)
{
	int nScrollPos = m_wndScrollBar.GetScrollPos();
	CRect wndRect = GetWndRect();
	int nRowCount = ceil((float)wndRect.Height() / (float)m_nRowHeight);

	for (int i = 0; i < nRowCount; i++)
	{
		if (nScrollPos + i >= CAegonTournamentMng::GetInstance()->m_mQueuedTournaments.size())
			break;

		CRect tmpRect;
		tmpRect.left = 0;
		tmpRect.top = 0 + i * m_nRowHeight;
		tmpRect.right = wndRect.Width() - 30;
		tmpRect.bottom = tmpRect.top + m_nRowHeight;

		if (tmpRect.PtInRect(point))
		{
			int nSelectedItem = nScrollPos + i;
			if (nSelectedItem >= 0 && nSelectedItem < CAegonTournamentMng::GetInstance()->m_mQueuedTournaments.size())
			{
				CRect hitrect = tmpRect;

				CPoint point2 = point;
				ClientToScreen(&point2);
				ClientToScreen(&hitrect);

				//g_WndMng.PutToolTip_ShopPackage(m_vItemList[nSelectedItem].m_dwItemUnitID, point2, &hitrect);
			}
			return;
		}
	}
}
void CWndAegonTournamentList::SelectionChanged() 
{
	CWndAegonTournament* pWndParent = (CWndAegonTournament*)GetParentWnd();
	if (pWndParent) 
	{
		pWndParent->SelectedTournamentChanged();
	}
}
////////////////////////////
////////TOURNAMENT/////////
///////////////////////////
CWndAegonTournament::CWndAegonTournament()
{
	m_wndTournamentList = NULL;

	m_pWndParticipationRewards = NULL;
	m_pWndFirstPlaceRewards = NULL;
	m_pWndSecondPlaceRewards = NULL;
}
CWndAegonTournament::~CWndAegonTournament()
{
}
void CWndAegonTournament::OnDraw(C2DRender* p2DRender)
{
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	DWORD dwColorStatic = CAegonThemeSwitcherMng::GetInstance()->GetStaticColor();
	CRect rectDraw;
	CWndStatic* pWndHeader = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	
	if (pWndHeader) 
	{
		pWndHeader->SetTitle("");
		rectDraw.top = pWndHeader->GetWndRect().bottom + 5;
		rectDraw.left = pWndHeader->GetWndRect().left;
		rectDraw.right = pWndHeader->GetWndRect().right;
		rectDraw.bottom = m_wndTournamentList->GetWndRect().bottom;
	}
	p2DRender->RenderRect(rectDraw, dwColorText);

	if (m_wndTournamentList && m_wndTournamentList->m_nSelectedItem >= 0 && m_wndTournamentList->m_nSelectedItem < CAegonTournamentMng::GetInstance()->m_mQueuedTournaments.size())
	{
		if (pWndHeader)
		{
			pWndHeader->SetTitle("Tournament Info");
		}



		__TOURNAMENT_UNIT* pTournamentUnit = CAegonTournamentMng::GetInstance()->GetTournamentByIndex(m_wndTournamentList->m_nSelectedItem);
		if (pTournamentUnit)
		{
			CString strTmp;
			CEditString strEdit;
			CPoint pt = CPoint(rectDraw.left + 10, rectDraw.top + 10);
			DWORD dwColorInfo = dwColorStatic;

			{
				char szTime[256] = { 0, };
				strftime(szTime, 255, "%d.%m.%Y - %R", &pTournamentUnit->m_sStartTime);
				szTime[255] = '\0';
				strTmp.Format("Starttime: %s", szTime);
			}

			strEdit.SetParsingString(strTmp.GetString(), dwColorInfo);
			strEdit.SetStyle(ESSTY_BOLD);
			p2DRender->TextOut_EditString(pt.x, pt.y, strEdit);

			pt.y += 20;


			time_t now = time_null();
			tm start_time_tm;
			memcpy(&start_time_tm, &pTournamentUnit->m_sStartTime, sizeof(tm)); // = pTournamentUnit->m_sStartTime;
			time_t start_time = mktime(&start_time_tm);
			start_time += (time_t)g_WndMng.m_nTimeDiff * 3600;
			CTimeSpan diff = start_time - now;

			strTmp.Format("Countdown: %I64d days %ld hours %ld min %ld sec", diff.GetDays(), diff.GetHours(), diff.GetMinutes(), diff.GetSeconds());
			strEdit.SetParsingString(strTmp.GetString(), dwColorInfo);
			strEdit.SetStyle(ESSTY_BOLD);
			p2DRender->TextOut_EditString(pt.x, pt.y, strEdit);

			pt.y += 20;

			if(pTournamentUnit->m_dwAllowedClasses > 0 && pTournamentUnit->m_dwAllowedClasses < MAX_JOB)
			{
				strTmp.Format("Classes: %s", prj.m_aJob[pTournamentUnit->m_dwAllowedClasses].szName);
			}
			else
			{
				strTmp.Format("Classes: All");
			}
			strEdit.SetParsingString(strTmp.GetString(), dwColorInfo);
			strEdit.SetStyle(ESSTY_BOLD);
			p2DRender->TextOut_EditString(pt.x, pt.y, strEdit);

			pt.y += 20;

			strTmp.Format("Level: %d-%d", pTournamentUnit->m_dwLevelReqMin, pTournamentUnit->m_dwLevelReqMax);
			strEdit.SetParsingString(strTmp.GetString(), dwColorInfo);
			strEdit.SetStyle(ESSTY_BOLD);
			p2DRender->TextOut_EditString(pt.x, pt.y, strEdit);

			pt.y += 20;

			strTmp.Format("Teamsize: %d", pTournamentUnit->m_dwTeamSize);
			strEdit.SetParsingString(strTmp.GetString(), dwColorInfo);
			strEdit.SetStyle(ESSTY_BOLD);
			p2DRender->TextOut_EditString(pt.x, pt.y, strEdit);

			pt.y += 20;

			strTmp.Format("Friendly fire: %s", pTournamentUnit->m_bFriendlyFire ? "On" : "Off");
			strEdit.SetParsingString(strTmp.GetString(), dwColorInfo);
			strEdit.SetStyle(ESSTY_BOLD);
			p2DRender->TextOut_EditString(pt.x, pt.y, strEdit);

			pt.y += 20;

			strTmp.Format("Rounds: %d", pTournamentUnit->m_dwRounds);
			strEdit.SetParsingString(strTmp.GetString(), dwColorInfo);
			strEdit.SetStyle(ESSTY_BOLD);
			p2DRender->TextOut_EditString(pt.x, pt.y, strEdit);

			pt.y += 20;

			strTmp.Format("Roundtime: %d seconds", pTournamentUnit->m_dwRoundTime);
			strEdit.SetParsingString(strTmp.GetString(), dwColorInfo);
			strEdit.SetStyle(ESSTY_BOLD);
			p2DRender->TextOut_EditString(pt.x, pt.y, strEdit);

			pt.y += 20;

			strTmp.Format("Minimum teams: %d", pTournamentUnit->m_dwTeamReqNum);
			strEdit.SetParsingString(strTmp.GetString(), dwColorInfo);
			strEdit.SetStyle(ESSTY_BOLD);
			p2DRender->TextOut_EditString(pt.x, pt.y, strEdit);

		}
		
	}
	else 
	{
		//no tournament selected
		if (pWndHeader)
		{
			pWndHeader->SetTitle("Tournament Guide");
		}
	}

}
void CWndAegonTournament::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	//Move Window to center
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();

	g_DPlay.SendGetTournamentMap();
	g_DPlay.SendUpdateTimeDiff();

	LPWNDCTRL listCtrl = GetWndCtrl(WIDC_CUSTOM1);

	if (listCtrl)
	{
		SAFE_DELETE(m_wndTournamentList);
		m_wndTournamentList = new CWndAegonTournamentList;
		m_wndTournamentList->Create(WBS_CHILD | WBS_VSCROLL, listCtrl->rect, this, 100000);
		m_wndTournamentList->m_byWndType = WTYPE_LISTBOX;
		m_wndTournamentList->m_bTile = true;
		m_wndTournamentList->m_bVisible = true;
		m_wndTournamentList->m_strTexture = "WndEditTile00.tga";

		AdjustWndBase();
	}

	CWndButton* pWndButtonJoin = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	if (pWndButtonJoin) 
	{
		pWndButtonJoin->SetVisible(FALSE);
	}

	CWndButton* pWndButtonDelete = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
	if (pWndButtonDelete)
	{
		pWndButtonDelete->SetVisible(FALSE);
	}
}
BOOL CWndAegonTournament::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_AEGON_TOURNAMENT, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndAegonTournament::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndAegonTournament::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndAegonTournament::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndAegonTournament::OnLButtonDown(UINT nFlags, CPoint point)
{

}
BOOL CWndAegonTournament::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_BUTTON1: //Join
	{
		break;
	}
	case WIDC_BUTTON2: //Delete
	{
		if (!g_pPlayer->IsAuthHigher(AUTH_ADMINISTRATOR))
			break;

		if (m_wndTournamentList) 
		{
			__TOURNAMENT_UNIT* tUnit = CAegonTournamentMng::GetInstance()->GetTournamentByIndex(m_wndTournamentList->m_nSelectedItem);
			if (tUnit) 
			{
				g_DPlay.SendDeleteTournament(tUnit->m_dwTournamentID);
			}
		}
		break;
	}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndAegonTournament::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}
BOOL CWndAegonTournament::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	return TRUE;
}
void CWndAegonTournament::CreateRewardLists()
{

	CWndStatic* pWndParticipationTitle = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	CWndStatic* pWndFirstTitle = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
	CWndStatic* pWndSecondTitle = (CWndStatic*)GetDlgItem(WIDC_STATIC4);

	if (pWndParticipationTitle)
	{
		pWndParticipationTitle->SetTitle("Participation Rewards");
	}
	if (pWndFirstTitle)
	{
		pWndFirstTitle->SetTitle("First Place Rewards");
	}
	if (pWndSecondTitle)
	{
		pWndSecondTitle->SetTitle("Second Place Rewards");
	}

	CWndButton* pWndButtonJoin = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	if (pWndButtonJoin)
	{
		pWndButtonJoin->SetVisible(TRUE);
	}

	if (g_pPlayer->IsAuthHigher(AUTH_ADMINISTRATOR)) 
	{
		CWndButton* pWndButtonDelete = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
		if (pWndButtonDelete)
		{
			pWndButtonDelete->SetVisible(TRUE);
		}
	}

	LPWNDCTRL listCtrlParticipation = GetWndCtrl(WIDC_CUSTOM2);
	if (listCtrlParticipation)
	{
		SAFE_DELETE(m_pWndParticipationRewards);
		m_pWndParticipationRewards = new CWndTournamentRewardList;
		m_pWndParticipationRewards->Create(WBS_CHILD | WBS_VSCROLL, listCtrlParticipation->rect, this, 100001);
		m_pWndParticipationRewards->m_byWndType = WTYPE_LISTBOX;
		m_pWndParticipationRewards->m_bTile = true;
		m_pWndParticipationRewards->m_bVisible = true;
		m_pWndParticipationRewards->m_strTexture = "WndEditTile00.tga";
	}

	LPWNDCTRL listCtrlFirst = GetWndCtrl(WIDC_CUSTOM3);
	if (listCtrlFirst)
	{
		SAFE_DELETE(m_pWndFirstPlaceRewards);
		m_pWndFirstPlaceRewards = new CWndTournamentRewardList;
		m_pWndFirstPlaceRewards->Create(WBS_CHILD | WBS_VSCROLL, listCtrlFirst->rect, this, 100002);
		m_pWndFirstPlaceRewards->m_byWndType = WTYPE_LISTBOX;
		m_pWndFirstPlaceRewards->m_bTile = true;
		m_pWndFirstPlaceRewards->m_bVisible = true;
		m_pWndFirstPlaceRewards->m_strTexture = "WndEditTile00.tga";
	}

	LPWNDCTRL listCtrlSecond = GetWndCtrl(WIDC_CUSTOM4);
	if (listCtrlSecond)
	{
		SAFE_DELETE(m_pWndSecondPlaceRewards);
		m_pWndSecondPlaceRewards = new CWndTournamentRewardList;
		m_pWndSecondPlaceRewards->Create(WBS_CHILD | WBS_VSCROLL, listCtrlSecond->rect, this, 100003);
		m_pWndSecondPlaceRewards->m_byWndType = WTYPE_LISTBOX;
		m_pWndSecondPlaceRewards->m_bTile = true;
		m_pWndSecondPlaceRewards->m_bVisible = true;
		m_pWndSecondPlaceRewards->m_strTexture = "WndEditTile00.tga";
	}
	m_wndTournamentList->SetFocus();
	AdjustWndBase();
}
void CWndAegonTournament::FillRewardLists() 
{
	if (m_wndTournamentList) 
	{
		int nSelected = m_wndTournamentList->m_nSelectedItem;
		__TOURNAMENT_UNIT* pUnit = CAegonTournamentMng::GetInstance()->GetTournamentByIndex(nSelected);

		if (pUnit) 
		{
			if(m_pWndParticipationRewards)
				m_pWndParticipationRewards->SetTournamentRewards(&pUnit->sRewardAll);
			if(m_pWndFirstPlaceRewards)
				m_pWndFirstPlaceRewards->SetTournamentRewards(&pUnit->sRewardFirstPlace);
			if(m_pWndSecondPlaceRewards)
				m_pWndSecondPlaceRewards->SetTournamentRewards(&pUnit->sRewardSecondPlace);
		}
	}
}
void CWndAegonTournament::RemoveRewardLists()
{
	CWndStatic* pWndParticipationTitle = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	CWndStatic* pWndFirstTitle = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
	CWndStatic* pWndSecondTitle = (CWndStatic*)GetDlgItem(WIDC_STATIC4);

	if (pWndParticipationTitle) 
	{
		pWndParticipationTitle->SetTitle("");
	}
	if (pWndFirstTitle)
	{
		pWndFirstTitle->SetTitle("");
	}
	if (pWndSecondTitle)
	{
		pWndSecondTitle->SetTitle("");
	}

	CWndButton* pWndButtonJoin = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	if (pWndButtonJoin)
	{
		pWndButtonJoin->SetVisible(FALSE);
	}

	CWndButton* pWndButtonDelete = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
	if (pWndButtonDelete)
	{
		pWndButtonDelete->SetVisible(FALSE);
	}
	
	if (m_pWndParticipationRewards) 
	{
		RemoveWnd(m_pWndParticipationRewards);
		SAFE_DELETE(m_pWndParticipationRewards);
	}
	if (m_pWndFirstPlaceRewards)
	{
		RemoveWnd(m_pWndFirstPlaceRewards);
		SAFE_DELETE(m_pWndFirstPlaceRewards);
	}
	if (m_pWndSecondPlaceRewards)
	{
		RemoveWnd(m_pWndSecondPlaceRewards);
		SAFE_DELETE(m_pWndSecondPlaceRewards);
	}

	AdjustWndBase();
}
void CWndAegonTournament::SelectedTournamentChanged()
{
	if (!m_wndTournamentList)
		return;

	int nSelected = m_wndTournamentList->m_nSelectedItem;
	__TOURNAMENT_UNIT* pUnit = CAegonTournamentMng::GetInstance()->GetTournamentByIndex(nSelected);
	if (pUnit) 
	{
		RemoveRewardLists();
		CreateRewardLists();
		FillRewardLists();
	}
	else 
	{
		RemoveRewardLists();
	}

}
BOOL CWndAegonTournament::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return FALSE;
}
void CWndAegonTournament::OnGetTournamentMap()
{
	if (m_wndTournamentList) 
	{
		if (m_wndTournamentList->m_nSelectedItem >= 0) 
		{
			if (m_wndTournamentList->m_nSelectedItem >= CAegonTournamentMng::GetInstance()->m_mQueuedTournaments.size()) 
			{
				m_wndTournamentList->m_nSelectedItem = CAegonTournamentMng::GetInstance()->m_mQueuedTournaments.size() - 1;
				m_wndTournamentList->SelectionChanged();
			}
		}
	}
}
///////////////////////////
////////SCOREBOARD/////////
///////////////////////////
CWndAegonTournamentScoreBoard::CWndAegonTournamentScoreBoard()
{
	strMainTexture = "";
	for (int i = 0; i < 10; i++)
	{
		top10Name[i] = "";
		top10Team[i] = "";
		top10Kill[i] = 0;
		top10Death[i] = 0;
	}
}
CWndAegonTournamentScoreBoard::~CWndAegonTournamentScoreBoard()
{

}
void CWndAegonTournamentScoreBoard::OnDraw(C2DRender* p2DRender)
{
	CRect rect = g_WndMng.m_rectClient;
	if (GetWndRect() != rect)
		SetWndRect(g_WndMng.m_rectClient);

	CString strTmp;
	CEditString strEditOut;

	//main
	mainTexture.Render(p2DRender, rect.TopLeft(), rect.BottomRight());
	//~main

	//caption
	captionTexture.m_size = CSize(captionRect.Width(), captionRect.Height());
	p2DRender->RenderTexture(captionRect.TopLeft(), &captionTexture, 255);
	//p2DRender->RenderRect(captionRect, COLOR_ERROR);
	//~caption

	//top10
	top10Texture.m_size = CSize(top10Rect.Width(), top10Rect.Height());
	p2DRender->RenderTexture(top10Rect.TopLeft(), &top10Texture, 255);
	//p2DRender->RenderRect(top10Rect, COLOR_ERROR);

	p2DRender->SetFont(CWndBase::m_Theme.m_pFontAegonTournamentTop10);
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			//p2DRender->RenderRect(top10PlayerRect[i][j], COLOR_ERROR);
		}
		if (top10Name[i].Compare("") == 0)
			continue;
		//rank
		strTmp.Format("%d", i + 1);
		strEditOut.SetParsingString(strTmp);
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		strEditOut.SetStyle(ESSTY_BOLD);
		p2DRender->TextOut_EditString(top10PlayerRect[i][0].left + (top10PlayerRect[i][0].Width() / 2) - (p2DRender->GetFont()->GetTextExtent(strEditOut.GetString()).cx / 2), top10PlayerRect[i][0].top + (top10PlayerRect[i][0].Height() / 2) - (p2DRender->GetFont()->GetTextExtent(strEditOut.GetString()).cy / 2), strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD);
		//~rank

		//name
		strTmp.Format("%s", top10Name[i].GetString());
		strEditOut.SetParsingString(strTmp);
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		strEditOut.SetStyle(ESSTY_BOLD);
		p2DRender->TextOut_EditString(top10PlayerRect[i][1].left + (top10PlayerRect[i][1].Width() / 2) - (p2DRender->GetFont()->GetTextExtent(strEditOut.GetString()).cx / 2), top10PlayerRect[i][1].top + (top10PlayerRect[i][1].Height() / 2) - (p2DRender->GetFont()->GetTextExtent(strEditOut.GetString()).cy / 2), strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD);
		//~name

		//team
		strTmp.Format("%s", top10Team[i].GetString());
		strEditOut.SetParsingString(strTmp);
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		strEditOut.SetStyle(ESSTY_BOLD);
		p2DRender->TextOut_EditString(top10PlayerRect[i][2].left + (top10PlayerRect[i][2].Width() / 2) - (p2DRender->GetFont()->GetTextExtent(strEditOut.GetString()).cx / 2), top10PlayerRect[i][2].top + (top10PlayerRect[i][2].Height() / 2) - (p2DRender->GetFont()->GetTextExtent(strEditOut.GetString()).cy / 2), strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD);
		//~team

		//KD
		strTmp.Format("%d/%d", top10Kill[i], top10Death[i]);
		strEditOut.SetParsingString(strTmp);
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		strEditOut.SetStyle(ESSTY_BOLD);
		p2DRender->TextOut_EditString(top10PlayerRect[i][3].left + (top10PlayerRect[i][3].Width() / 2) - (p2DRender->GetFont()->GetTextExtent(strEditOut.GetString()).cx / 2), top10PlayerRect[i][3].top + (top10PlayerRect[i][3].Height() / 2) - (p2DRender->GetFont()->GetTextExtent(strEditOut.GetString()).cy / 2), strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD);
		//~KD

	}
	p2DRender->SetFont(CWndBase::m_Theme.m_pFontText);

	//~top10

	//arrows - background 
	arrowTextureRoundOne[0].m_size = CSize(arrowRoundOne[0].Width(), arrowRoundOne[0].Height());
	arrowTextureRoundOne[1].m_size = CSize(arrowRoundOne[1].Width(), arrowRoundOne[1].Height());
	arrowTextureRoundTwo[0].m_size = CSize(arrowRoundTwo[0].Width(), arrowRoundTwo[0].Height());
	arrowTextureRoundTwo[1].m_size = CSize(arrowRoundTwo[1].Width(), arrowRoundTwo[1].Height());
	arrowTextureRoundThree[0].m_size = CSize(arrowRoundThree[0].Width(), arrowRoundThree[0].Height());
	arrowTextureRoundThree[1].m_size = CSize(arrowRoundThree[1].Width(), arrowRoundThree[1].Height());

	overlayRedTexture.m_size = CSize(fieldsRoundOne[0].Width(), fieldsRoundOne[0].Height());
	overlayGreenTexture.m_size = CSize(fieldsRoundOne[0].Width(), fieldsRoundOne[0].Height());
	overlayGoldTexture.m_size = CSize(fieldsRoundOne[0].Width(), fieldsRoundOne[0].Height());



	//first round
	for (int i = 0; i < 8; i += 2)
	{
		p2DRender->RenderRect(arrowRoundOne[i], COLOR_ERROR);
	}
	for (int i = 1; i < 8; i += 2)
	{
		p2DRender->RenderRect(arrowRoundOne[i], COLOR_ERROR);
	}
	//~first round
	//second round
	for (int i = 0; i < 4; i += 2)
	{
		p2DRender->RenderRect(arrowRoundTwo[i], COLOR_ERROR);
	}
	for (int i = 1; i < 4; i += 2)
	{
		p2DRender->RenderRect(arrowRoundTwo[i], COLOR_ERROR);
	}
	//~second round
	//third round
	p2DRender->RenderRect(arrowRoundThree[0], COLOR_ERROR);
	p2DRender->RenderRect(arrowRoundThree[1], COLOR_ERROR);
	p2DRender->RenderTexture(fieldsRoundThree[0].TopLeft(), &overlayGreenTexture, 255);
	p2DRender->RenderTexture(fieldsRoundThree[1].TopLeft(), &overlayRedTexture, 255);
	//~third round

	CString tmpComp = "";

	//if (tmpComp.Compare(m_tournamentLog.winningTeam.teamName) != 0)
	//	p2DRender->RenderTexture(trophyRect.TopLeft(), &overlayGoldTexture, 255);

	//p2DRender->RenderRect(arrowRoundThree[0], COLOR_ERROR);
	//p2DRender->RenderRect(arrowRoundThree[1], COLOR_ERROR);
	//~arrows - background 

	//team rects
	p2DRender->SetFont(CWndBase::m_Theme.m_pFontAegonTournamentBracket);
	for (int i = 0; i < AT_MAX_TEAMS; i++)
	{
		//p2DRender->RenderRect(fieldsRoundOne[i], COLOR_ERROR);
		strTmp.Format("%s", "team1");
		strEditOut.SetParsingString(strTmp);
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		strEditOut.SetStyle(ESSTY_BOLD);
		p2DRender->TextOut_EditString(fieldsRoundOne[i].left + (fieldsRoundOne[i].Width() / 2) - (p2DRender->GetFont()->GetTextExtent(strEditOut.GetString()).cx / 2), fieldsRoundOne[i].top + (fieldsRoundOne[i].Height() / 2) - (p2DRender->GetFont()->GetTextExtent(strEditOut.GetString()).cy / 2), strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD);
	}
	for (int i = 0; i < AT_MAX_TEAMS / 2; i++)
	{
		//p2DRender->RenderRect(fieldsRoundTwo[i], COLOR_ERROR);
		strTmp.Format("%s", "team2");
		strEditOut.SetParsingString(strTmp);
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		strEditOut.SetStyle(ESSTY_BOLD);
		p2DRender->TextOut_EditString(fieldsRoundTwo[i].left + (fieldsRoundTwo[i].Width() / 2) - (p2DRender->GetFont()->GetTextExtent(strEditOut.GetString()).cx / 2), fieldsRoundTwo[i].top + (fieldsRoundTwo[i].Height() / 2) - (p2DRender->GetFont()->GetTextExtent(strEditOut.GetString()).cy / 2), strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD);
	}
	for (int i = 0; i < (AT_MAX_TEAMS / 2) / 2; i++)
	{
		//p2DRender->RenderRect(fieldsRoundThree[i], COLOR_ERROR);
		strTmp.Format("%s", "team3");
		strEditOut.SetParsingString(strTmp);
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		strEditOut.SetStyle(ESSTY_BOLD);
		p2DRender->TextOut_EditString(fieldsRoundThree[i].left + (fieldsRoundThree[i].Width() / 2) - (p2DRender->GetFont()->GetTextExtent(strEditOut.GetString()).cx / 2), fieldsRoundThree[i].top + (fieldsRoundThree[i].Height() / 2) - (p2DRender->GetFont()->GetTextExtent(strEditOut.GetString()).cy / 2), strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD);
	}
	//p2DRender->RenderRect(fieldWinner, COLOR_ERROR);
	strTmp.Format("%s", "team4");
	strEditOut.SetParsingString(strTmp);
	strEditOut.SetColor(D3DCOLOR_ARGB(255, 255, 255, 255));
	strEditOut.SetStyle(ESSTY_BOLD);
	p2DRender->TextOut_EditString(fieldWinner.left + (fieldWinner.Width() / 2) - (p2DRender->GetFont()->GetTextExtent(strEditOut.GetString()).cx / 2), fieldWinner.top + (fieldWinner.Height() / 2) - (p2DRender->GetFont()->GetTextExtent(strEditOut.GetString()).cy / 2), strEditOut, 0, 0, 2);
	strEditOut.ClearStyle(ESSTY_BOLD);
	p2DRender->SetFont(CWndBase::m_Theme.m_pFontText);

	//~team rects



	//icons
	trophyTexture.m_size = CSize(trophyRect.Width(), trophyRect.Height());
	p2DRender->RenderTexture(trophyRect.TopLeft(), &trophyTexture, 255);
	//p2DRender->RenderRect(trophyRect, COLOR_ERROR);
	//~icons





}
void CWndAegonTournamentScoreBoard::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	CRect rect = g_WndMng.m_rectClient;
	SetWndRect(rect);

	//Move Window to center
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();

	//main
	strMainTexture = MakePath(DIR_THEME, _T("tournamentMainTex.tga"));
	mainTexture.LoadTexture(m_pApp->m_pd3dDevice, strMainTexture, COLOR_ITEM_BG, TRUE);
	//~main

	//caption
	strCaption = MakePath(DIR_THEME, _T("tournamentCaptionTex.tga"));
	captionTexture.LoadTexture(m_pApp->m_pd3dDevice, strCaption, COLOR_ITEM_BG, TRUE);
	captionRect.SetRect(rect.right * 688 / 1920, rect.bottom * 15 / 1080, rect.right * 1232 / 1920, rect.bottom * 194 / 1080);
	//~caption

	//top10
	strTop10 = MakePath(DIR_THEME, _T("tournamentTop10Tex.tga"));
	top10Texture.LoadTexture(m_pApp->m_pd3dDevice, strTop10, COLOR_ITEM_BG, TRUE);
	top10Rect.SetRect(rect.right * 1488 / 1920, rect.bottom * 301 / 1080, rect.right * 1849 / 1920, rect.bottom * 852 / 1080);

	int yOffset = 102;
	int yOffset2 = 128;

	for (int i = 0; i < 10; i++)
	{
		int y1 = top10Rect.top + top10Rect.Height() * yOffset / 551;
		int y2 = top10Rect.top + top10Rect.Height() * yOffset2 / 551;
		for (int j = 0; j < 4; j++)
		{
			int x1 = 0;
			int x2 = 0;
			switch (j)
			{
			case 0: //rank
			{
				x1 = top10Rect.left + top10Rect.Width() * 14 / 361;
				x2 = top10Rect.left + top10Rect.Width() * 25 / 361;
				break;
			}
			case 1: //name
			{
				x1 = top10Rect.left + top10Rect.Width() * 44 / 361;
				x2 = top10Rect.left + top10Rect.Width() * 201 / 361;
				break;
			}
			case 2: //team
			{
				x1 = top10Rect.left + top10Rect.Width() * 212 / 361;
				x2 = top10Rect.left + top10Rect.Width() * 302 / 361;
				break;
			}
			case 3: //kd
			{
				x1 = top10Rect.left + top10Rect.Width() * 312 / 361;
				x2 = top10Rect.left + top10Rect.Width() * 351 / 361;
				break;
			}

			}
			top10PlayerRect[i][j].SetRect(x1, y1, x2, y2);
		}
		yOffset += 39;
		yOffset2 += 39;
	}
	//~top10

	//team fields

	fieldsRoundOne[0].SetRect(rect.right * 96 / 1920, rect.bottom * 179 / 1080, rect.right * 367 / 1920, rect.bottom * 233 / 1080);
	fieldsRoundOne[1].SetRect(rect.right * 96 / 1920, rect.bottom * 274 / 1080, rect.right * 367 / 1920, rect.bottom * 328 / 1080);
	fieldsRoundOne[2].SetRect(rect.right * 96 / 1920, rect.bottom * 394 / 1080, rect.right * 367 / 1920, rect.bottom * 448 / 1080);
	fieldsRoundOne[3].SetRect(rect.right * 96 / 1920, rect.bottom * 489 / 1080, rect.right * 367 / 1920, rect.bottom * 543 / 1080);
	fieldsRoundOne[4].SetRect(rect.right * 96 / 1920, rect.bottom * 609 / 1080, rect.right * 367 / 1920, rect.bottom * 663 / 1080);
	fieldsRoundOne[5].SetRect(rect.right * 96 / 1920, rect.bottom * 704 / 1080, rect.right * 367 / 1920, rect.bottom * 758 / 1080);
	fieldsRoundOne[6].SetRect(rect.right * 96 / 1920, rect.bottom * 824 / 1080, rect.right * 367 / 1920, rect.bottom * 878 / 1080);
	fieldsRoundOne[7].SetRect(rect.right * 96 / 1920, rect.bottom * 919 / 1080, rect.right * 367 / 1920, rect.bottom * 973 / 1080);

	fieldsRoundTwo[0].SetRect(rect.right * 420 / 1920, rect.bottom * 226 / 1080, rect.right * 691 / 1920, rect.bottom * 280 / 1080);
	fieldsRoundTwo[1].SetRect(rect.right * 420 / 1920, rect.bottom * 441 / 1080, rect.right * 691 / 1920, rect.bottom * 495 / 1080);
	fieldsRoundTwo[2].SetRect(rect.right * 420 / 1920, rect.bottom * 657 / 1080, rect.right * 691 / 1920, rect.bottom * 711 / 1080);
	fieldsRoundTwo[3].SetRect(rect.right * 420 / 1920, rect.bottom * 873 / 1080, rect.right * 691 / 1920, rect.bottom * 927 / 1080);

	fieldsRoundThree[0].SetRect(rect.right * 748 / 1920, rect.bottom * 334 / 1080, rect.right * 1019 / 1920, rect.bottom * 388 / 1080);
	fieldsRoundThree[1].SetRect(rect.right * 748 / 1920, rect.bottom * 764 / 1080, rect.right * 1019 / 1920, rect.bottom * 818 / 1080);

	fieldWinner.SetRect(rect.right * 1130 / 1920, rect.bottom * 549 / 1080, rect.right * 1343 / 1920, rect.bottom * 603 / 1080);
	//~team fields

	//arrows
	strArrowTextureRoundOne[0] = MakePath(DIR_THEME, _T("tournamentArrow1-1.tga"));
	strArrowTextureRoundOne[1] = MakePath(DIR_THEME, _T("tournamentArrow1-2.tga"));
	strArrowTextureRoundTwo[0] = MakePath(DIR_THEME, _T("tournamentArrow2-1.tga"));
	strArrowTextureRoundTwo[1] = MakePath(DIR_THEME, _T("tournamentArrow2-2.tga"));
	strArrowTextureRoundThree[0] = MakePath(DIR_THEME, _T("tournamentArrow3-1.tga"));
	strArrowTextureRoundThree[1] = MakePath(DIR_THEME, _T("tournamentArrow3-2.tga"));

	arrowTextureRoundOne[0].LoadTexture(m_pApp->m_pd3dDevice, strArrowTextureRoundOne[0], COLOR_ITEM_BG, TRUE);
	arrowTextureRoundOne[1].LoadTexture(m_pApp->m_pd3dDevice, strArrowTextureRoundOne[1], COLOR_ITEM_BG, TRUE);
	arrowTextureRoundTwo[0].LoadTexture(m_pApp->m_pd3dDevice, strArrowTextureRoundTwo[0], COLOR_ITEM_BG, TRUE);
	arrowTextureRoundTwo[1].LoadTexture(m_pApp->m_pd3dDevice, strArrowTextureRoundTwo[1], COLOR_ITEM_BG, TRUE);
	arrowTextureRoundThree[0].LoadTexture(m_pApp->m_pd3dDevice, strArrowTextureRoundThree[0], COLOR_ITEM_BG, TRUE);
	arrowTextureRoundThree[1].LoadTexture(m_pApp->m_pd3dDevice, strArrowTextureRoundThree[1], COLOR_ITEM_BG, TRUE);

	arrowRoundOne[0].SetRect(rect.right * 376 / 1920, rect.bottom * 202 / 1080, rect.right * 555 / 1920, rect.bottom * 219 / 1080);
	arrowRoundOne[2].SetRect(rect.right * 376 / 1920, rect.bottom * 417 / 1080, rect.right * 555 / 1920, rect.bottom * 434 / 1080);
	arrowRoundOne[4].SetRect(rect.right * 376 / 1920, rect.bottom * 633 / 1080, rect.right * 555 / 1920, rect.bottom * 650 / 1080);
	arrowRoundOne[6].SetRect(rect.right * 376 / 1920, rect.bottom * 849 / 1080, rect.right * 555 / 1920, rect.bottom * 866 / 1080);
	arrowRoundOne[1].SetRect(rect.right * 376 / 1920, rect.bottom * 288 / 1080, rect.right * 555 / 1920, rect.bottom * 306 / 1080);
	arrowRoundOne[3].SetRect(rect.right * 376 / 1920, rect.bottom * 503 / 1080, rect.right * 555 / 1920, rect.bottom * 521 / 1080);
	arrowRoundOne[5].SetRect(rect.right * 376 / 1920, rect.bottom * 719 / 1080, rect.right * 555 / 1920, rect.bottom * 737 / 1080);
	arrowRoundOne[7].SetRect(rect.right * 376 / 1920, rect.bottom * 935 / 1080, rect.right * 555 / 1920, rect.bottom * 953 / 1080);

	arrowRoundTwo[0].SetRect(rect.right * 700 / 1920, rect.bottom * 260 / 1080, rect.right * 881 / 1920, rect.bottom * 327 / 1080);
	arrowRoundTwo[2].SetRect(rect.right * 700 / 1920, rect.bottom * 690 / 1080, rect.right * 881 / 1920, rect.bottom * 757 / 1080);
	arrowRoundTwo[1].SetRect(rect.right * 700 / 1920, rect.bottom * 397 / 1080, rect.right * 881 / 1920, rect.bottom * 464 / 1080);
	arrowRoundTwo[3].SetRect(rect.right * 700 / 1920, rect.bottom * 826 / 1080, rect.right * 881 / 1920, rect.bottom * 893 / 1080);

	arrowRoundThree[0].SetRect(rect.right * 1029 / 1920, rect.bottom * 362 / 1080, rect.right * 1227 / 1920, rect.bottom * 542 / 1080);
	arrowRoundThree[1].SetRect(rect.right * 1029 / 1920, rect.bottom * 612 / 1080, rect.right * 1227 / 1920, rect.bottom * 792 / 1080);
	//~arrows

	//icons
	strTrophy = MakePath(DIR_THEME, _T("tournamentTrophy.tga"));
	trophyTexture.LoadTexture(m_pApp->m_pd3dDevice, strTrophy, COLOR_ITEM_BG, TRUE);
	trophyRect.SetRect(rect.right * 1072 / 1920, rect.bottom * 549 / 1080, rect.right * 1126 / 1920, rect.bottom * 603 / 1080);
	//~icons

	//team overlay
	strOverlayRed = MakePath(DIR_THEME, _T("tournamentOverlayRed.tga"));
	strOverlayGreen = MakePath(DIR_THEME, _T("tournamentOverlayGreen.tga"));
	strOverlayGold = MakePath(DIR_THEME, _T("tournamentOverlayGold.tga"));

	overlayRedTexture.LoadTexture(m_pApp->m_pd3dDevice, strOverlayRed, COLOR_ITEM_BG, TRUE);
	overlayGreenTexture.LoadTexture(m_pApp->m_pd3dDevice, strOverlayGreen, COLOR_ITEM_BG, TRUE);
	overlayGoldTexture.LoadTexture(m_pApp->m_pd3dDevice, strOverlayGold, COLOR_ITEM_BG, TRUE);
	//~team overlay
}

BOOL CWndAegonTournamentScoreBoard::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_AEGON_TOURNAMENT_SCORE, WBS_TOPMOST | WBS_NOFRAME | WBS_NOFOCUS, CPoint(0, 0), pWndParent);
	DelWndStyle(WBS_MOVE);
}

void CWndAegonTournamentScoreBoard::OnDestroy()
{
}
void CWndAegonTournamentScoreBoard::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndAegonTournamentScoreBoard::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndAegonTournamentScoreBoard::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return FALSE;
}
BOOL CWndAegonTournamentScoreBoard::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return FALSE;
}
void CWndAegonTournamentScoreBoard::SetTop10()
{

	/*for (int i = 0; i < AT_MAX_TEAMS; i++)
	{
		for (int j = 0; j < AT_MAX_PLAYERS; j++)
		{
			if (m_tournamentLog.teamsRoundOne[i].playerIDs[j] != 0 && top10Name[9].Compare("") == 0 || m_tournamentLog.teamsRoundOne[i].playerKills[j] > top10Kill[9])
			{
				AddToTop10(i, j);
			}
		}

	}*/
}
void CWndAegonTournamentScoreBoard::SortTop10()
{
	BOOL changed = TRUE;
	CString tmpName = "";
	CString tmpTeam = "";
	DWORD	tmpKills = 0;
	DWORD	tmpDeaths = 0;

	while (changed)
	{
		changed = FALSE;
		for (int i = 0; i < 9; i++)
		{
			if (top10Kill[i] < top10Kill[i + 1])
			{
				tmpName.Format("%s", top10Name[i].GetString());
				tmpTeam.Format("%s", top10Team[i].GetString());
				tmpKills = top10Kill[i];
				tmpDeaths = top10Death[i];

				top10Name[i].Format("%s", top10Name[i + 1].GetString());
				top10Team[i].Format("%s", top10Team[i + 1].GetString());
				top10Kill[i] = top10Kill[i + 1];
				top10Death[i] = top10Death[i + 1];


				top10Name[i + 1].Format("%s", tmpName.GetString());
				top10Team[i + 1].Format("%s", tmpTeam.GetString());
				top10Kill[i + 1] = tmpKills;
				top10Death[i + 1] = tmpDeaths;

				changed = TRUE;
			}
		}
	}

}
void CWndAegonTournamentScoreBoard::AddToTop10(int teamIndex, int playerIndex)
{
	/*int freeIndex = -1;
	for (int i = 0; i < 10; i++)
	{
		if (top10Name[i].Compare("") == 0)
		{
			freeIndex = i;
			i = 10;
		}
	}
	if (freeIndex != -1)
	{

		top10Name[freeIndex].Format("%s", CPlayerDataCenter::GetInstance()->GetPlayerString(m_tournamentLog.teamsRoundOne[teamIndex].playerIDs[playerIndex]));
		top10Team[freeIndex].Format("%s", m_tournamentLog.teamsRoundOne[teamIndex].teamName);
		top10Kill[freeIndex] = m_tournamentLog.teamsRoundOne[teamIndex].playerKills[playerIndex];
		top10Death[freeIndex] = m_tournamentLog.teamsRoundOne[teamIndex].playerDeaths[playerIndex];
	}
	else
	{
		top10Name[9].Format("%s", CPlayerDataCenter::GetInstance()->GetPlayerString(m_tournamentLog.teamsRoundOne[teamIndex].playerIDs[playerIndex]));
		top10Team[9].Format("%s", m_tournamentLog.teamsRoundOne[teamIndex].teamName);
		top10Kill[9] = m_tournamentLog.teamsRoundOne[teamIndex].playerKills[playerIndex];
		top10Death[9] = m_tournamentLog.teamsRoundOne[teamIndex].playerDeaths[playerIndex];
	}

	SortTop10();*/
}
#endif // __AEGON_TOURNAMENT