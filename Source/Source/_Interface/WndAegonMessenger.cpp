#include "stdafx.h"
#include "resData.h"
#include "WndAegonMessenger.h"
#include "DPClient.h"
#include "defineText.h"
#include "WndManager.h"
#include "WndManager.h"
#include "Model.h"
#include "ModelMng.h"
#include "ModelObject.h"
#include "ModelGlobal.h"
#include "math.h"
#include <ctime>
#include "defineObj.h"
#include "defineSound.h"

#ifdef __AEGON_MESSENGER
extern CDPClient g_DPlay;

///////////////////////////
////////MNG CLASS//////////
///////////////////////////
CAegonMessengerMng::CAegonMessengerMng()
{
	m_idPlayer = 0;
}
CAegonMessengerMng::~CAegonMessengerMng()
{
}
CAegonMessengerMng* CAegonMessengerMng::GetInstance(void)
{
	static CAegonMessengerMng sAegonMessengerMng;
	return &sAegonMessengerMng;
}
vector<__AEGON_CHAT_UNIT*>* CAegonMessengerMng::GetOpenChats()
{
	return &m_vOpenChats;
}
vector<__AEGON_CHAT_UNIT>* CAegonMessengerMng::GetAllChats()
{
	return &m_vAllChats;
}
vector<__AEGON_CHAT_MESSAGE>* CAegonMessengerMng::GetMessageQueue()
{
	return &m_vMessageQueue;
}
__AEGON_CHAT_UNIT* CAegonMessengerMng::GetChatUnitFromAll(char* szName)
{
	for (int i = 0; i < m_vAllChats.size(); i++)
	{
		if (m_vAllChats[i].m_strName.Compare(szName) == 0)
			return &m_vAllChats[i];
	}
	return NULL;
}
__AEGON_CHAT_UNIT* CAegonMessengerMng::AddChatUnit(__AEGON_CHAT_UNIT sChatUnit)
{
	__AEGON_CHAT_UNIT* pOut = NULL;
	BOOL bSwitchUnit = FALSE;
	CString strName;
	CWndAegonMessenger* pWndMessenger = (CWndAegonMessenger*)g_WndMng.GetApplet(APP_AEGON_MESSENGER);
	if (pWndMessenger && pWndMessenger->m_sSelectedUnit)
	{
		strName = pWndMessenger->m_sSelectedUnit->m_strName;
		pWndMessenger->m_sSelectedUnit = NULL;
		bSwitchUnit = TRUE;
	}

	__AEGON_CHAT_UNIT* pChatUnit = GetChatUnitFromAll((char*)sChatUnit.m_strName.GetString());
	if (pChatUnit) 
	{
		pChatUnit->m_bVisible = TRUE;
		pOut = pChatUnit;
	}
	else 
	{
		m_vAllChats.push_back(sChatUnit);
		pOut = &m_vAllChats[m_vAllChats.size() - 1];
	}

	UpdateOpenChats();

	if (pWndMessenger && bSwitchUnit)
		pWndMessenger->SwitchToChat(strName);

	g_Option.SaveMessengerChats();

	return pOut;
}
void CAegonMessengerMng::RemoveChatUnit(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_vOpenChats.size())
		return;

	BOOL bSwitchUnit = FALSE;
	CString strNameSwap;
	CWndAegonMessenger* pWndMessenger = (CWndAegonMessenger*)g_WndMng.GetApplet(APP_AEGON_MESSENGER);
	if (pWndMessenger && pWndMessenger->m_sSelectedUnit)
	{
		strNameSwap = pWndMessenger->m_sSelectedUnit->m_strName;
		pWndMessenger->m_sSelectedUnit = NULL;
		bSwitchUnit = TRUE;
	}

	CString strName = m_vOpenChats[nIndex]->m_strName;

	for (int i = 0; i < m_vAllChats.size(); i++)
	{
		if (strName.Compare(m_vAllChats[i].m_strName.GetString()) == 0) 
		{
			m_vAllChats[i].m_bVisible = FALSE;
			break;
		}
	}
	UpdateOpenChats();

	
	if (pWndMessenger && bSwitchUnit) 
	{
		pWndMessenger->SwitchToChat(strNameSwap);
	}

	g_Option.SaveMessengerChats();
}
void CAegonMessengerMng::UpdateOpenChats()
{

	CWndAegonMessenger* pWndMessenger = (CWndAegonMessenger*)g_WndMng.GetApplet(APP_AEGON_MESSENGER);
	if (pWndMessenger) 
		pWndMessenger->m_sSelectedUnit = NULL;

	m_vOpenChats.clear();
	for (int i = 0; i < m_vAllChats.size(); i++) 
	{
		if (m_vAllChats[i].m_bVisible)
			m_vOpenChats.push_back(&m_vAllChats[i]);
	}

	g_DPlay.SendAddMessengerStatus();
}
void CAegonMessengerMng::ClearAll()
{
	CWndAegonMessenger* pWndMessenger = (CWndAegonMessenger*)g_WndMng.GetApplet(APP_AEGON_MESSENGER);
	if (pWndMessenger)
		pWndMessenger->m_sSelectedUnit = NULL;

	m_vOpenChats.clear();
	m_vAllChats.clear();

	m_idPlayer = 0;
}
void CAegonMessengerMng::SetLoadedId(u_long idCurrent)
{
	m_idPlayer = idCurrent;
}
BOOL CAegonMessengerMng::IsInitialized(u_long idCurrent)
{
	return m_idPlayer == idCurrent ? TRUE : FALSE;

}
///////////////////////////
///////LIST WINDOW/////////
///////////////////////////
CWndMessengerList::CWndMessengerList()
{
	m_nRowHeight = 60;
	m_nSelectedItem = -1;
	m_dwFlashAlpha = 0;
	m_nTotalUnreadMessages = 0;
}
CWndMessengerList::~CWndMessengerList()
{
}
void CWndMessengerList::OnDraw(C2DRender* p2DRender)
{
	vector<__AEGON_CHAT_UNIT*>* pOpenChats = CAegonMessengerMng::GetInstance()->GetOpenChats();
	if (!pOpenChats)
		return;

	SetButtonCount(pOpenChats->size());

	if (IsInvalidObj(g_pPlayer))
		return;

	CRect wndRect = GetWindowRect();
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	CPoint point = CPoint(0, 0);
	CString str;

	int nRange = 0;
	int nPage = wndRect.Height() / m_nRowHeight;

	nRange = pOpenChats->size();

	if (!nRange)
		nRange = 1;

	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);
	m_wndScrollBar.EnableScrollBar(TRUE);
	m_wndScrollBar.SetScrollStep(1);

	m_dwFlashAlpha += 5;
	if (m_dwFlashAlpha > 255*2)
		m_dwFlashAlpha = 0;

	DWORD dwAlpha = m_dwFlashAlpha;
	if (dwAlpha > 255)
	{
		dwAlpha = 255 - (dwAlpha - 255);
	}
	dwAlpha = dwAlpha << 24;

	int nIndex = 0;
	for (int i = m_wndScrollBar.GetScrollPos(); i < pOpenChats->size(); i++, nIndex++)
	{
		point.x = 5;
		point.y = nIndex * m_nRowHeight;

		if (i > m_wndScrollBar.GetScrollPos() + nPage)
			break;


		if (point.y + m_nRowHeight >= m_nRowHeight)
			p2DRender->RenderLine(CPoint(0, point.y + m_nRowHeight), CPoint(wndRect.Width() - 26, point.y + m_nRowHeight), dwColorText);

#ifdef __AEGON_THEME_SWITCHER
		DWORD dwColorRenderFillRect = CAegonThemeSwitcherMng::GetInstance()->GetRenderFillRectColor();
		DWORD dwColorRenderRect = CAegonThemeSwitcherMng::GetInstance()->GetRenderRectColor();
		DWORD dwColorFlash = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_YELLOW);
#endif //__AEGON_THEME_SWITCHER

		if (m_nSelectedItem == i)
		{
			CRect rectSelected = CRect(0, point.y, wndRect.right - 26, point.y + m_nRowHeight);
#ifdef __AEGON_THEME_SWITCHER
			p2DRender->RenderFillRect(rectSelected, dwColorRenderFillRect);
			p2DRender->RenderRect(rectSelected, dwColorRenderRect);
#else //__AEGON_THEME_SWITCHER
			p2DRender->RenderFillRect(rectSelected, 0xFFf8e6cc);
			p2DRender->RenderRect(rectSelected, 0xFFf28818);
#endif //__AEGON_THEME_SWITCHER
		}
		else 
		{
			if ((*pOpenChats)[i]->m_nUnreadMessages > 0)
			{
				CRect rectSelected = CRect(0, point.y, wndRect.right - 26, point.y + m_nRowHeight);

				DWORD dwColor = (dwColorFlash & ~(0xff000000)) | dwAlpha;
				p2DRender->RenderFillRect(rectSelected, dwColor);
			}
		}

		point.y += 2;

		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont(m_Theme.m_pFontMessengerTitle);

		str.Format("%s", (*pOpenChats)[i]->m_strName.GetString());
		CSize strExtentTitle = p2DRender->GetFont()->GetTextExtent(str.GetString());
		p2DRender->TextOut(point.x, point.y, str.GetString());

		p2DRender->SetFont(pOldFont);

		CWndAegonMessenger* pWndParent = (CWndAegonMessenger*)GetParentWnd();
		if (pWndParent) 
		{
			str.Format("(%s)", pWndParent->GetStatusString((*pOpenChats)[i]->m_nLastStatus).GetString());
			int nTmpX = point.x + strExtentTitle.cx + 5;
			CSize strExtent = p2DRender->GetFont()->GetTextExtent(str.GetString());
			int nYOffset = (strExtentTitle.cy - strExtent.cy) / 2;
			p2DRender->TextOut(nTmpX, point.y + nYOffset, str.GetString(), pWndParent->GetStatusColor((*pOpenChats)[i]->m_nLastStatus));
		}
		


		CRect rectEditStr = CRect(point.x, point.y + strExtentTitle.cy, GetWndRect().right - 35, point.y + m_nRowHeight);
		point.y += strExtentTitle.cy;

		CEditString strEdit;
		strEdit.Init(p2DRender->GetFont(), &rectEditStr);
		str.Format("%s", (*pOpenChats)[i]->m_strLastMessage.GetString());
		strEdit.SetParsingString(str.GetString(), dwColorText);

		p2DRender->TextOut_EditString(point.x, point.y, strEdit, 0, 2, 2);

	}
}
void CWndMessengerList::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	CRect rect = GetWindowRect();

	m_wndScrollBar.Create(WBS_DOCKING | WBS_VERT, rect, this, 1000);
	m_wndScrollBar.SetVisible(IsWndStyle(WBS_VSCROLL));

	CreateButtons();
	SetButtonCount(CAegonMessengerMng::GetInstance()->GetOpenChats()->size());
}
BOOL CWndMessengerList::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID >= 0 && nID < __AEG_MAX_LIST_BUTTONS) 
	{
		RemoveChat(nID);
	}
	return CWndBase::OnChildNotify(message, nID, pLResult);
}
BOOL CWndMessengerList::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndBase::OnCommand(nID, dwMessage, pWndBase);
}
void CWndMessengerList::OnLButtonUp(UINT nFlags, CPoint point)
{

}
void CWndMessengerList::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	SetSelectedItem(point);

	vector<__AEGON_CHAT_UNIT*>* pOpenChats = CAegonMessengerMng::GetInstance()->GetOpenChats();

	if (!pOpenChats || m_nSelectedItem < 0 || m_nSelectedItem >= pOpenChats->size())
		return;

	ToggleSelectedUnit();
}

void CWndMessengerList::OnMouseWndSurface(CPoint point)
{
}
void CWndMessengerList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}
void CWndMessengerList::OnRButtonUp(UINT nFlags, CPoint point)
{
	SetSelectedItem(point);
	ToggleSelectedUnit();
}
void CWndMessengerList::SetSelectedItem(CPoint point)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	vector<__AEGON_CHAT_UNIT*>* pOpenChats = CAegonMessengerMng::GetInstance()->GetOpenChats();
	if (!pOpenChats)
		return;

	int nScrollPos = m_wndScrollBar.GetScrollPos();
	CRect wndRect = GetWndRect();
	int nRowCount = ceil((float)wndRect.Height() / (float)m_nRowHeight);

	for (int i = 0; i < nRowCount; i++)
	{
		if (nScrollPos + i >= pOpenChats->size())
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
void CWndMessengerList::ToggleSelectedUnit()
{
	vector<__AEGON_CHAT_UNIT*>* pOpenChats = CAegonMessengerMng::GetInstance()->GetOpenChats();
	if (!pOpenChats)
		return;

	if (m_nSelectedItem < 0 || m_nSelectedItem >= pOpenChats->size())
	{
		return;
	}

	CWndAegonMessenger* pWndParent = (CWndAegonMessenger*)GetParentWnd();
	if (pWndParent && pWndParent->m_nMinimizedMode != __AEG_MINIMIZE_MODE_TEX)
	{
		pWndParent->m_sSelectedUnit = (*pOpenChats)[m_nSelectedItem];
		if (pWndParent->m_sSelectedUnit->m_nUnreadMessages > 0) 
		{
			m_nTotalUnreadMessages -= pWndParent->m_sSelectedUnit->m_nUnreadMessages;
			OnUpdateUnreadMessagesCount();
		}
		pWndParent->m_sSelectedUnit->m_nUnreadMessages = 0;
		pWndParent->RefreshTextField();

	}
}
void CWndMessengerList::AddChatUnit(CString strName, int nStatus)
{
	vector<__AEGON_CHAT_UNIT*>* pOpenChats = CAegonMessengerMng::GetInstance()->GetOpenChats();
	if (!pOpenChats)
		return;

	__AEGON_CHAT_UNIT* pChatUnit = NULL;
	for (int i = 0; i < pOpenChats->size(); i++) //find chat unit
	{
		if (strName.Compare((*pOpenChats)[i]->m_strName.GetString()) == 0)
		{
			pChatUnit = (*pOpenChats)[i];
			break;
		}
	}

	if (!pChatUnit) 
	{
		__AEGON_CHAT_UNIT tmpUnit;
		tmpUnit.m_strName = strName;
		tmpUnit.m_nLastStatus = nStatus;
		tmpUnit.m_nUnreadMessages = 0;

		CAegonMessengerMng::GetInstance()->AddChatUnit(tmpUnit);
		SetButtonCount(pOpenChats->size());
	}
}
void CWndMessengerList::OnChatMessage(CString strFrom, CString strTo, LPCSTR szMessage, int nMessageType, DWORD dwTimeStamp)
{
	if (strFrom.GetLength() <= 0 || strTo.GetLength() <= 0)
		return;

	__AEGON_CHAT_MESSAGE tmpMessage;
	tmpMessage.m_strFrom = strFrom;
	tmpMessage.m_strTo = strTo;
	tmpMessage.m_strMessage = szMessage;
	tmpMessage.m_nMessageType = nMessageType;
	tmpMessage.m_dwTimeStamp = dwTimeStamp;

	CAegonMessengerMng::GetInstance()->GetMessageQueue()->push_back(tmpMessage);
}
void CWndMessengerList::ProcessMessageQueue()
{
	if (CAegonMessengerMng::GetInstance()->GetMessageQueue()->size() <= 0)
		return;

	if (IsInvalidObj(g_pPlayer))
		return;

	if (!CAegonMessengerMng::GetInstance()->IsInitialized(g_pPlayer->m_idPlayer))
		return;

	vector<__AEGON_CHAT_UNIT*>* pOpenChats = CAegonMessengerMng::GetInstance()->GetOpenChats();
	if (!pOpenChats)
		return;

	BOOL bMessageAdded = FALSE;

	__AEGON_CHAT_MESSAGE* pSelectedMessage = &((*CAegonMessengerMng::GetInstance()->GetMessageQueue())[0]);

	CString strFindName = "";
	if (pSelectedMessage->m_strFrom.Compare(g_pPlayer->GetName()) == 0)
	{
		strFindName = pSelectedMessage->m_strTo;
	}
	else 
	{
		strFindName = pSelectedMessage->m_strFrom;
	}

	if (strFindName.GetLength() <= 0) 
	{
		CAegonMessengerMng::GetInstance()->GetMessageQueue()->erase(CAegonMessengerMng::GetInstance()->GetMessageQueue()->begin());
		return;
	}

	__AEGON_CHAT_UNIT* pChatUnit = NULL;
	for (int i = 0; i < pOpenChats->size(); i++) //find chat unit
	{
		if (strFindName.Compare((*pOpenChats)[i]->m_strName.GetString()) == 0)
		{
			pChatUnit = (*pOpenChats)[i];
			break;
		}
	}
	
	if (!pChatUnit) //create new chat unit
	{
		__AEGON_CHAT_UNIT tmpUnit;
		tmpUnit.m_strName = strFindName;

		tmpUnit.m_nLastStatus = __AEG_CHAT_STATUS_OFFLINE;

		pChatUnit = CAegonMessengerMng::GetInstance()->AddChatUnit(tmpUnit);
		SetButtonCount(pOpenChats->size());
	}

	if (pChatUnit && pChatUnit->m_strName.Compare(strFindName.GetString()) == 0)
	{
		switch (pSelectedMessage->m_nMessageType)
		{
		case __AEG_MESSAGE_TYPE_REG:
			pChatUnit->m_nLastStatus = __AEG_CHAT_STATUS_ONLINE;
			break;
		case __AEG_MESSAGE_TYPE_OFFLINE:
			pChatUnit->m_nLastStatus = __AEG_CHAT_STATUS_OFFLINE;
			break;
		default:
			break;
		}

		if(pSelectedMessage->m_strMessage.GetLength() > 0)
			pChatUnit->m_strLastMessage = pSelectedMessage->m_strMessage;

		CString strTotalMessage = "";
		CString strMessage = "";
#ifdef __LEESE_MESSENGER_TIMER
		CString timeMessage;
		DWORD dwColorTime = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
		CTime time = CTime::GetCurrentTime();

		DWORD dwColorName = 0;

		if (strcmp(pSelectedMessage->m_strFrom.GetString(), g_pPlayer->GetName()) != 0)
		{
			dwColorName = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_RED);
		}
		else 
		{
			dwColorName = CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE);
		}

		CString strTime;
		strTime = time.Format("[%H:%M]");
		timeMessage.Format("#c%08x%s#nc ", dwColorName, strTime.GetString());
		strMessage.Format("#c%08x%s:#nc\n  %s\n", dwColorName, pSelectedMessage->m_strFrom.GetString(), pSelectedMessage->m_strMessage.GetString());

		if (pSelectedMessage->m_nMessageType == __AEG_MESSAGE_TYPE_OFFLINE)
		{
			if (pSelectedMessage->m_dwTimeStamp > 0)
			{
				time_t timestamp = pSelectedMessage->m_dwTimeStamp;
				tm* timestruct = localtime(&timestamp);

				strTime.Format("%02d.%02d.%04d [%02d:%02d]", timestruct->tm_mday, timestruct->tm_mon + 1, timestruct->tm_year + 1900, timestruct->tm_hour, timestruct->tm_min);
				timeMessage.Format("#c%08x%s#nc ", dwColorName, strTime.GetString());
			}
			
			strMessage.Format("#c%08x%s (Offline Message):#nc\n  %s\n", dwColorName, pSelectedMessage->m_strFrom.GetString(), pSelectedMessage->m_strMessage.GetString());
			strTotalMessage = timeMessage + strMessage;
		}
		else if (g_Option.m_bTimeStamp)
		{
			strTotalMessage = timeMessage + strMessage;
		}
		else 
		{
			strTotalMessage = strMessage;
		}
#endif
		if (pSelectedMessage->m_strMessage.GetLength() > 0)
		{
			pChatUnit->m_strEditString.AddParsingString(strTotalMessage.GetString(), CAegonThemeSwitcherMng::GetInstance()->GetTextColor());
			bMessageAdded = TRUE;

		}
	}

	CAegonMessengerMng::GetInstance()->GetMessageQueue()->erase(CAegonMessengerMng::GetInstance()->GetMessageQueue()->begin());

	CWndAegonMessenger* pWndParent = (CWndAegonMessenger*)GetParentWnd();
	if (pWndParent )
	{
		if (bMessageAdded)
		{
			pChatUnit->m_nUnreadMessages++;
			m_nTotalUnreadMessages++;
		}

		if (pWndParent->m_sSelectedUnit && pChatUnit) 
		{
			if (pWndParent->m_sSelectedUnit == pChatUnit)
			{
				pWndParent->RefreshTextField();

				if (bMessageAdded && pWndParent->m_nMinimizedMode != __AEG_MINIMIZE_MODE_TEX)
				{
					pChatUnit->m_nUnreadMessages--;
					m_nTotalUnreadMessages--;
				}
			}
		}

		if (bMessageAdded) 
		{
			OnUpdateUnreadMessagesCount();
			g_Option.SaveMessengerChats();
		}
	}
}
void CWndMessengerList::SwitchToChatUnit(CString strName)
{
	vector<__AEGON_CHAT_UNIT*>* pOpenChats = CAegonMessengerMng::GetInstance()->GetOpenChats();
	if (!pOpenChats)
		return;

	for (int i = 0; i < pOpenChats->size(); i++) //find chat unit
	{
		if (strName.Compare((*pOpenChats)[i]->m_strName.GetString()) == 0)
		{
			m_nSelectedItem = i;
			break;
		}
	}

	ToggleSelectedUnit();
}
void CWndMessengerList::SetButtonCount(int nCount)
{
	for (int i = 1; i < __AEG_MAX_LIST_BUTTONS + 1; i++)
	{
		if (i <= nCount) 
		{
			m_pWndButtons[i - 1].SetVisible(TRUE);
			m_pWndButtons[i - 1].EnableWindow(TRUE);
		}
		else
		{
			m_pWndButtons[i - 1].SetVisible(FALSE);
			m_pWndButtons[i - 1].EnableWindow(FALSE);
		}
	}
}
void CWndMessengerList::CreateButtons()
{
	for (int i = 0; i < __AEG_MAX_LIST_BUTTONS; i++)
	{
		CTexture* pTextureClose = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "ButtWndExit.tga", COLOR_ITEM_BG, TRUE);

		int nSizeX = 16;
		if (pTextureClose)
			nSizeX = pTextureClose->m_size.cx / 4;

		int nXPos = GetWndRect().Width() - 30 - nSizeX;

		m_pWndButtons[i].Create(_T(""), 0, CRect(nXPos, i * m_nRowHeight + 2, nXPos + nSizeX, i * m_nRowHeight + nSizeX + 2), this, i);
		CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchSetTexture((CWndBase*)&m_pWndButtons[i], m_pApp->m_pd3dDevice, "ButtWndExit.tga", TRUE);
		m_pWndButtons[i].FitTextureSize();
	}	
}
void CWndMessengerList::OnThemeSelectionChanged()
{
	CWndListBox::OnThemeSelectionChanged();
	for (int i = 0; i < __AEG_MAX_LIST_BUTTONS; i++)
	{
		CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchSetTexture((CWndBase*)&m_pWndButtons[i], m_pApp->m_pd3dDevice, "ButtWndExit.tga", TRUE);
		m_pWndButtons[i].FitTextureSize();
	}
}
void CWndMessengerList::RemoveChat(int nPos)
{
	vector<__AEGON_CHAT_UNIT*>* pOpenChats = CAegonMessengerMng::GetInstance()->GetOpenChats();
	if (!pOpenChats)
		return;

	int nRemoveIndex = m_wndScrollBar.GetScrollPos() + nPos;
	if (nRemoveIndex < 0 || nRemoveIndex >= pOpenChats->size())
		return;

	CWndAegonMessenger* pWndParent = (CWndAegonMessenger*)GetParentWnd();
	if (pWndParent && pWndParent->m_sSelectedUnit) 
	{
		if (pWndParent->m_sSelectedUnit == (*pOpenChats)[nRemoveIndex])
		{
			pWndParent->m_sSelectedUnit = NULL;
			m_nSelectedItem = -1;
			pWndParent->RefreshTextField();
		}
	}

	CAegonMessengerMng::GetInstance()->RemoveChatUnit(nRemoveIndex);
	SetButtonCount(pOpenChats->size());
	
	m_wndScrollBar.SetScrollPos(0);
}
void CWndMessengerList::OnStatusUpdate(int nIndex, int nStatus)
{
	vector<__AEGON_CHAT_UNIT*>* pOpenChats = CAegonMessengerMng::GetInstance()->GetOpenChats();
	if (!pOpenChats)
		return;

	if (nIndex < 0 || nIndex >= pOpenChats->size())
		return;

	(*pOpenChats)[nIndex]->m_nLastStatus = nStatus;
}
void CWndMessengerList::OnUpdateUnreadMessagesCount()
{
	CWndAegonMessenger* pWndParent = (CWndAegonMessenger*)GetParentWnd();
	if (pWndParent)
	{
		pWndParent->OnUpdateUnreadMessagesCount(m_nTotalUnreadMessages);
	}
}
////////////////////////////
//////Messenger MAIN////////
///////////////////////////
CWndAegonMessenger::CWndAegonMessenger()
{
	m_pWndMessengerList = NULL;
	m_pWndMessengerAddChat = NULL;
	m_sSelectedUnit = NULL;
	m_dwLastStatusUpdate = 0;
	m_nTotalUnreadMessages = 0;
}
CWndAegonMessenger::~CWndAegonMessenger()
{
	if (m_pWndMessengerAddChat)
		SAFE_DELETE(m_pWndMessengerAddChat);
}
BOOL CWndAegonMessenger::Process()
{
	CWndNeuz::Process();
	if (m_pWndMessengerList && CAegonMessengerMng::GetInstance()->GetMessageQueue()->size() > 0)
		m_pWndMessengerList->ProcessMessageQueue();

	if (!m_nMinimizedMode && GetTickCount() > m_dwLastStatusUpdate + __AEG_STATUS_UPDATE_INTERVAL)
	{
		g_DPlay.SendAddMessengerStatus();
		m_dwLastStatusUpdate = GetTickCount();
	}
	return TRUE;
}
void CWndAegonMessenger::OnDraw(C2DRender* p2DRender)
{
	if (m_nMinimizedMode != __AEG_MINIMIZE_MODE_NONE)
	{
		return;
	}

	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT);
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);

#ifdef __AEGON_QOL_STUFF
	if (pWndEdit)
	{
		CRect wndRect = pWndEdit->GetWndRect();
		int nRange = 0;
		int nPage = 3;

		nRange = pWndEdit->m_string.GetLineCount();

		if (!nRange)
			nRange = 1;

		pWndEdit->m_wndScrollBar.SetScrollRange(0, nRange);
		pWndEdit->m_wndScrollBar.SetScrollPage(nPage);
		pWndEdit->m_wndScrollBar.EnableScrollBar(TRUE);
	}

#endif

	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont(m_Theme.m_pFontMessengerTitle);

	if (m_pWndMessengerList) 
	{
		CPoint ptTextList = m_pWndMessengerList->GetWndRect().TopLeft();
		CString strTmp = "Latest Messages";

		CSize sizeText = p2DRender->GetFont()->GetTextExtent(strTmp);
		ptTextList.y -= sizeText.cy;
		p2DRender->TextOut(ptTextList.x, ptTextList.y, strTmp.GetString());
	}

	BOOL bChatOpened = FALSE;
	CString strTmp;
	if (m_sSelectedUnit)
	{
		strTmp.Format("%s", m_sSelectedUnit->m_strName.GetString());
		bChatOpened = TRUE;
	}
	else
	{
		strTmp.Format("No chat selected.");
	}

	CPoint ptRender = pWndText->GetWndRect().TopLeft();
	CSize sizeText = p2DRender->GetFont()->GetTextExtent(strTmp);
	ptRender.y -= sizeText.cy;
	p2DRender->TextOut(ptRender.x, ptRender.y, strTmp.GetString());


	p2DRender->SetFont(pOldFont);

	if (bChatOpened )
	{
		ptRender.x += sizeText.cx + 5;
		

		if (m_sSelectedUnit)
		{
			strTmp.Format("(%s)", GetStatusString(m_sSelectedUnit->m_nLastStatus).GetString());
			CSize textExtent = p2DRender->GetFont()->GetTextExtent(strTmp.GetString());
			int nYOffset = (sizeText.cy - textExtent.cy) / 2;
			p2DRender->TextOut(ptRender.x, ptRender.y + nYOffset, strTmp.GetString(), GetStatusColor(m_sSelectedUnit->m_nLastStatus));
		}
	}
	
}
void CWndAegonMessenger::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	LPWNDCTRL listCtrl = GetWndCtrl(WIDC_CUSTOM1);
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT);

	if (pWndEdit) 
	{
		pWndEdit->AddWndStyle(EBS_AUTOVSCROLL);
		pWndEdit->AddWndStyle(EBS_AUTOHSCROLL);
		pWndEdit->SetNativeMode();
		pWndEdit->SetFocus();
		pWndEdit->SetWndRect(pWndEdit->GetWndRect()); //to make new styles work properly  ?? idk shit code

		pWndEdit->SetMaxStringNumber(__AEG_MAX_MESSAGE_LENGTH);
	}

	if (listCtrl)
	{
		SAFE_DELETE(m_pWndMessengerList);
		m_pWndMessengerList = new CWndMessengerList;
		m_pWndMessengerList->Create(WBS_CHILD | WBS_VSCROLL, listCtrl->rect, this, 100000);
		m_pWndMessengerList->m_byWndType = WTYPE_LISTBOX;
		m_pWndMessengerList->m_bTile = true;
		m_pWndMessengerList->m_bVisible = true;
		m_pWndMessengerList->m_strTexture = "WndEditTile00.tga";
	}


	//Move Window to center
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}
BOOL CWndAegonMessenger::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_AEGON_MESSENGER, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndAegonMessenger::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndAegonMessenger::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndAegonMessenger::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndAegonMessenger::OnLButtonDown(UINT nFlags, CPoint point)
{

}
BOOL CWndAegonMessenger::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT);

	switch (nID)
	{
	case WIDC_BUTTON1:
	{
		SAFE_DELETE(m_pWndMessengerAddChat);
		m_pWndMessengerAddChat = new CWndAegonMessengerAddChat;
		m_pWndMessengerAddChat->Initialize(this);
		break;
	}
	case WIDC_SEND:
		DoSendMessage();
		break;
	case WIDC_EDIT:
		if (message == EN_RETURN)
		{
			DoSendMessage();
		}
#ifdef __AEGON_QOL_STUFF
		else if (message == EN_CHANGE) //auto scrolling edit field
		{
			CPoint ptCaret = pWndEdit->GetCaretPos();
			DWORD dwLineCount = pWndEdit->m_string.GetLineCount();
			DWORD dwFontMaxHeight = pWndEdit->GetFont()->GetMaxHeight();

			DWORD dwCaretLineOffset = pWndEdit->m_nLineSpace + dwFontMaxHeight;
			int nCaretLine = 0;
			if (dwCaretLineOffset > 0)
				nCaretLine = ptCaret.y / dwCaretLineOffset;

			int nMaxScrollPos = pWndEdit->m_wndScrollBar.GetMaxScrollPos() - pWndEdit->m_wndScrollBar.GetScrollPage();
			int nMinScrollPos = pWndEdit->m_wndScrollBar.GetMinScrollPos();

			if (nCaretLine > dwLineCount)
				nCaretLine = dwLineCount;

			if (nCaretLine >= nMaxScrollPos)
			{
				pWndEdit->m_wndScrollBar.SetMaxScrollPos();
			}
			else
			{
				if (nCaretLine < nMinScrollPos)
					nCaretLine = nMinScrollPos;
				pWndEdit->m_wndScrollBar.SetScrollPos(nCaretLine);
			}
		}
#endif
		break;

	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndAegonMessenger::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_nMinimizedMode != __AEG_MINIMIZE_MODE_NONE)
		OnMinimizeToggle();
}
void CWndAegonMessenger::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_pWndMessengerAddChat) 
	{
		SAFE_DELETE(m_pWndMessengerAddChat);
		pWndChild = NULL;
	}
}
void CWndAegonMessenger::PaintFrame(C2DRender* p2DRender)
{
	CWndNeuz::PaintFrame(p2DRender);

	if (m_nMinimizedMode == __AEG_MINIMIZE_MODE_TEX)
	{
		int nUnreadMessages = ((CWndAegonMessenger*)this)->m_nTotalUnreadMessages;
		if (nUnreadMessages > 0)
		{
			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont(CWndBase::m_Theme.m_pFontMessengerNotification);

			CString strCount;
			if(nUnreadMessages > 99)
				strCount.Format("99");
			else
				strCount.Format("%d", nUnreadMessages);

			CRect rectRender;

			rectRender.left = (double)m_pTexture->m_size.cx * ((double)21 / (double)38);
			rectRender.right = (double)m_pTexture->m_size.cx * ((double)37 / (double)38);
			rectRender.top = 0;
			rectRender.bottom = (double)m_pTexture->m_size.cy * ((double)16 / (double)38);

			CPoint point = rectRender.CenterPoint();

			CSize size = p2DRender->m_pFont->GetTextExtent(strCount);

			p2DRender->TextOut(point.x - size.cx / 2, point.y - size.cy / 2, strCount, COLOR_WHITE);

			p2DRender->SetFont(pOldFont);
		}
	}
}
void CWndAegonMessenger::OnMinimizeToggle()
{
	if (m_nMinimizedMode != __AEG_MINIMIZE_MODE_NONE)
	{
		CWndNeuz::OnMinimizeToggle();
	}
	else 
	{
		if(m_nTotalUnreadMessages > 0)
			MinimizeToTexture("Icon_MessengerNotification.tga");
		else
			MinimizeToTexture("Icon_Messenger.tga");
	}
}
void CWndAegonMessenger::DoSendMessage()
{
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT);
	
	CString strPlayer = "";

	if (m_sSelectedUnit)
		strPlayer = m_sSelectedUnit->m_strName;
	else
	{
		g_WndMng.PutString("No chat selected.", 0, COLOR_ERROR);
		return;
	}

	if (pWndEdit->m_string.IsEmpty() == FALSE)
	{
		if (pWndEdit->m_string.GetLength() > __AEG_MAX_MESSAGE_LENGTH)
		{
			g_WndMng.PutString("The Message is %d characters too long.", 0, COLOR_ERROR);
			return;
		}
		CString strFormat = pWndEdit->m_string;
		pWndEdit->m_string.GetTextFormat(strFormat);
		CString string;
		string.Format("/say \"%s\" %s", strPlayer.GetString(), strFormat.GetString());

		g_DPlay.SendChat(string);
		pWndEdit->Empty();
#ifdef __AEGON_QOL_STUFF
		pWndEdit->m_wndScrollBar.SetScrollPos(0);
#endif
	}
}
CString CWndAegonMessenger::GetStatusString(int nStatus)
{
	CString strOut = "searching";
	switch (nStatus) 
	{
	case __AEG_CHAT_STATUS_ONLINE:
		strOut = "Online";
		break;
	case __AEG_CHAT_STATUS_OFFLINE:
		strOut = "Offline";
		break;
	case __AEG_CHAT_STATUS_AFK:
		strOut = "AFK";
		break;
	}
	return strOut;
}

DWORD CWndAegonMessenger::GetStatusColor(int nStatus) 
{
	switch (nStatus)
	{
	case __AEG_CHAT_STATUS_ONLINE:
		return CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_GREEN);
		break;
	case __AEG_CHAT_STATUS_OFFLINE:
		return CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_RED);
		break;
	case __AEG_CHAT_STATUS_AFK:
		return CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_YELLOW);
		break;
	}
	return CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_PURPLE);
}
void CWndAegonMessenger::RefreshTextField()
{
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);

	if (m_sSelectedUnit && pWndText) 
	{
		CRect rect = pWndText->GetWndRect();
		rect.right -= 30;
		m_sSelectedUnit->m_strEditString.Init(CWndBase::m_Theme.m_pFontText, &rect);
		pWndText->ForceSetEditString(m_sSelectedUnit->m_strEditString);
		pWndText->m_wndScrollBar.SetMaxScrollPos();
	}
	else if (pWndText)
	{
		CEditString strTmp;
		CRect rect = pWndText->GetWndRect();
		rect.right -= 30;
		strTmp.Init(CWndBase::m_Theme.m_pFontText, &rect);

		pWndText->ForceSetEditString(strTmp);
	}
}
void CWndAegonMessenger::AddChatUnit(CString strName, int nStatus)
{
	if (m_pWndMessengerList)
		m_pWndMessengerList->AddChatUnit(strName, nStatus);
}
void CWndAegonMessenger::OnChatMessage(CString strFrom, CString strTo, LPCSTR szMessage, int nMessageType, DWORD dwTimeStamp)
{
	if (m_pWndMessengerList)
		m_pWndMessengerList->OnChatMessage(strFrom, strTo, szMessage, nMessageType, dwTimeStamp);
}
void CWndAegonMessenger::SwitchToChat(CString strName)
{
	if (m_pWndMessengerList)
	{
		m_pWndMessengerList->SwitchToChatUnit(strName);
	}
}
void CWndAegonMessenger::OnStatusUpdate(int nIndex, int nStatus)
{
	if (m_pWndMessengerList)
	{
		m_pWndMessengerList->OnStatusUpdate(nIndex, nStatus);
	}
}
void CWndAegonMessenger::OnUpdateUnreadMessagesCount(int nCount)
{
	if (m_nTotalUnreadMessages != nCount) 
	{
		if (m_nMinimizedMode == __AEG_MINIMIZE_MODE_TEX)
		{
			if (m_nTotalUnreadMessages == 0 && nCount > 0)
			{
				m_strTexture = "Icon_MessengerNotification.tga";
				AdjustWndBase();
			}
			else if (m_nTotalUnreadMessages > 0 && nCount == 0)
			{
				m_strTexture = "Icon_Messenger.tga";
				AdjustWndBase();
			}
		}
		m_nTotalUnreadMessages = nCount;
	}
}
void CWndAegonMessenger::MoveTopLeft()
{
	CPoint ptMove = CPoint(g_WndMng.m_rectClient.left + 20, g_WndMng.m_rectClient.top + 200);
	Move(ptMove);
}
////////////////////////////
//////Messenger Add////////
///////////////////////////
CWndAegonMessengerAddChat::CWndAegonMessengerAddChat()
{
}
CWndAegonMessengerAddChat::~CWndAegonMessengerAddChat()
{
}
void CWndAegonMessengerAddChat::OnDraw(C2DRender* p2DRender)
{
}
void CWndAegonMessengerAddChat::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndComboBox* pWndComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	if (pWndComboBox) 
	{
		for (map<u_long, Friend>::iterator it = g_WndMng.m_RTMessenger.begin(); it != g_WndMng.m_RTMessenger.end(); it++)
		{
			u_long idPlayer = it->first;
			const char* szPlayerName = CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer);
			if(szPlayerName)
				pWndComboBox->AddString(szPlayerName);
		}
	}

	//Move Window to center
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}
BOOL CWndAegonMessengerAddChat::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_AEGON_MESSENGER_ADD, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndAegonMessengerAddChat::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndAegonMessengerAddChat::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndAegonMessengerAddChat::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndAegonMessengerAddChat::OnLButtonDown(UINT nFlags, CPoint point)
{

}
BOOL CWndAegonMessengerAddChat::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);

	switch (nID)
	{
	case WIDC_BUTTON1:
	{
		DoAddChat();
		break;
	}
	case WIDC_COMBOBOX1:
		if (message == EN_RETURN)
			DoAddChat();
		break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndAegonMessengerAddChat::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}
void CWndAegonMessengerAddChat::DoAddChat()
{
	CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);

	if (pWndCombo)
	{
		if (strlen(pWndCombo->GetString()) > 0)
		{
			if (strlen(pWndCombo->GetString()) < MAX_PLAYER)
			{
				g_DPlay.SendAddMessengerChatUnit((char*)pWndCombo->GetString());
				Destroy();
			}
			else
			{
				g_WndMng.PutString("Invalid Name.", 0, COLOR_ERROR);
			}
		}
		else
		{
			g_WndMng.PutString("Invalid Name.", 0, COLOR_ERROR);
		}

	}
}
#endif