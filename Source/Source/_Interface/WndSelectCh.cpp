#include "stdafx.h"
#include "resData.h"

#ifdef __AZRIA_1023
#include "defineText.h"
#include "AppDefine.h"

#include "WndSelectCh.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __V18TICKET
CWndSelectCh::CWndSelectCh(const unsigned long mixVar, const unsigned long propId) : pWndListBox(nullptr), pWndListBox2(nullptr), propItemId(propId)
, mixedVar(mixVar)
{
}
void CWndSelectCh::createWorldListBox()
{
	for (std::vector<const TicketProp*>::const_iterator it = worldList.begin(); it != worldList.end(); ++it)
	{
		if (!*it) continue;
#ifndef __CHANNELSWITCHER
		if (g_pPlayer->GetWorld()->GetID() != (*it)->dwWorldId)
#endif //__CHANNELSWITCHER
		{
			const int nIndex = pWndListBox2->AddString(g_WorldMng.m_aWorld.GetAt((*it)->dwWorldId)->m_szWorldName);
			unsigned long* newData = new unsigned long;
			*newData = (*it)->dwWorldId;
			pWndListBox2->SetItemDataPtr(nIndex, newData);
		}
	}
}
void CWndSelectCh::createChannelListBox(const unsigned long index) const
{
	pWndListBox->ResetContent();
	unsigned long* worldId = reinterpret_cast<unsigned long*>(pWndListBox2->GetItemDataPtr(index));
	if (worldId)
	{
		const int expand = CTicketProperty::GetInstance()->getExpandedLayer(*worldId) + 1;
		std::string tmp = prj.GetText(TID_GAME_CHAR_SERVERNAME);
		tmp += " ";

		for (int i = 0; i < expand; ++i)
		{
			std::string loopStr = tmp + std::to_string(i + 1);
			pWndListBox->AddString(loopStr.c_str());
		}
	}
}

bool CWndSelectCh::checkWorld()
{
#ifndef __CHANNELSWITCHER
	for (std::vector<const TicketProp*>::const_iterator it = worldList.begin(); it != worldList.end(); ++it)
	{
		if (g_pPlayer->GetWorld()->GetID() == (*it)->dwWorldId)
		{
			pWndListBox2->ResetContent();
			pWndListBox->ResetContent();

			Destroy();

			SAFE_DELETE(g_WndMng.m_pWndCommItemDlg);
			g_WndMng.m_pWndCommItemDlg = new CWndCommItemDlg;
			g_WndMng.m_pWndCommItemDlg->Initialize(&g_WndMng, APP_COMMITEM_DIALOG);
			g_WndMng.m_pWndCommItemDlg->SetItem(TID_GAME_TICKET_DESC, HIWORD(mixedVar), propItemId);
			return true;
		}
	}
#endif //__CHANNELSWITCHER
	return false;
}

#else //__V18TICKET
CWndSelectCh::CWndSelectCh(int nItemId, int nChCount) 
{ 
	m_nItemId  = nItemId;
	m_nChCount = nChCount + 1;	// 0일때 1개이므로 실제갯수대로 맞춰준다
} 
#endif //__V18TICKET

CWndSelectCh::~CWndSelectCh() 
{ 
#ifdef __V18TICKET
	pWndListBox2->ResetContent();
	pWndListBox->ResetContent();
#endif //__V18TICKET
}

void CWndSelectCh::OnDraw( C2DRender* p2DRender ) 
{ 
}

void CWndSelectCh::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
#ifdef __V18TICKET
	pWndListBox = static_cast<CWndListBox*>(GetDlgItem(WIDC_LB_CHANNEL));
	pWndListBox2 = static_cast<CWndListBox*>(GetDlgItem(WIDC_LB_WORLD));

	CTicketProperty* tickInst = CTicketProperty::GetInstance();

	if (tickInst->isMultiWorldTicket(propItemId))
	{
		worldList = tickInst->getMultiworldTicketProp(propItemId);
		for (unsigned int i = 0; i < worldList.size(); ++i)
		{
			if (worldList[i] == nullptr)
			{
				Error("MultiWorld Ticket Null i = %d", i);
			}
		}
	}
	else
	{
		const TicketProp* tmpTicket = tickInst->getTicketProp(propItemId);
		if (tmpTicket == nullptr)
			Error("This ticket seems to be a nullptr");
		worldList.push_back(tmpTicket);
	}

	if (checkWorld())
		return;

	createWorldListBox();
	createChannelListBox();

	pWndListBox->SetCurSel(0);
	pWndListBox2->SetCurSel(0);

#else //__V18TICKET
	const TicketProp* tmpTicket = tickInst->GetTicketProp(propItemId);
	worldList.push_back(tmpTicket);
	pWndListBox2->AddString(g_WorldMng.m_aWorld.GetAt((*it)->dwWorldId)->m_szWorldName);
	int m_nChCount = tickInst->GetExpanedLayer(tmpTicket->dwWorldId) + 1;
	CString strTitle;
	for (int i = 0; i < m_nChCount; ++i)
	{
		strTitle.Format("%s	%d", prj.GetText(TID_GAME_CHAR_SERVERNAME), i + 1);
		pWndListBox->AddString(strTitle);
	}

	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	pWndListBox->ResetContent();
	CString strTitle;
	for(int i=0; i < m_nChCount; ++i)
	{
		strTitle.Format( "%s	%d", prj.GetText(TID_GAME_CHAR_SERVERNAME), i+1);
		pWndListBox->AddString(strTitle);
	}
#endif //__V18TICKET

#ifndef __V18TICKET
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
#endif //__V18TICKET

	MoveParentCenter();
} 

BOOL CWndSelectCh::Initialize( CWndBase* pWndParent, DWORD  ) 
{ 
#ifdef __V18TICKET
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_WORLD_FREETICKET, 0, CPoint(0, 0), pWndParent);
#else //__V18TICKET
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SELECT_CHANNEL, 0, CPoint( 0, 0 ), pWndParent );
#endif //__V18TICKET
} 

BOOL CWndSelectCh::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndSelectCh::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

#ifdef __V18TICKET
BOOL CWndSelectCh::OnChildNotify(const UINT message, const UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
		case WIDC_LB_WORLD:
		{
			if (pWndListBox2->GetCurSel() >= 0)
				createChannelListBox(pWndListBox2->GetCurSel());
			pWndListBox->SetCurSel(0);
		}
		break;
		case WIDC_BT_MOVE:
		{
			unsigned long sendVar = pWndListBox->GetCurSel();
			if (static_cast<long>(sendVar) == -1)
				break;

			if (pWndListBox2->GetCurSel() >= 0)
			{
				unsigned long* worldId = reinterpret_cast<unsigned long*>(pWndListBox2->GetItemDataPtr(pWndListBox2->GetCurSel()));
				if (worldId)
					sendVar = (*worldId << 16) | sendVar;
			}

			std::string var = std::to_string(sendVar);
			g_DPlay.SendDoUseItemInput(mixedVar, const_cast<char*>(var.c_str()));
			pWndListBox2->ResetContent();
			pWndListBox->ResetContent();

			Destroy();
		}
		break;
#ifdef __CHANNELSWITCHER
		case WIDC_BUTTON:
		{
			g_DPlay.SendDoUseItemInput(mixedVar, const_cast<char*>(std::to_string(1 << 16).c_str()));
			pWndListBox2->ResetContent();
			pWndListBox->ResetContent();
			Destroy();
		}
		break;
#endif //__CHANNELSWITCHER
#ifdef __CHECKLOAD
		case WIDC_BT_CANCEL:
		{
			if (pWndListBox2->GetCurSel() >= 0)
			{
				const int layer = pWndListBox->GetCurSel();
				if (layer == -1)
					break;

				unsigned long* worldId = reinterpret_cast<unsigned long*>(pWndListBox2->GetItemDataPtr(pWndListBox2->GetCurSel()));
				if (worldId)
					g_DPlay.GetCheckLoad(*worldId, layer * -1);
			}
		}
		break;
#endif //__CHECKLOAD
		default:
		{
			break;
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#else //__V18TICKET
BOOL CWndSelectCh::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	int nSelect = 0;

	switch (nID)
	{
	case WIDC_LISTBOX1: // view ctrl
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
		char strTemp[8];
		nSelect = pWndListBox->GetCurSel() * -1;
		_itoa(nSelect, strTemp, 10);
		g_DPlay.SendDoUseItemInput(m_nItemId, strTemp);
		Destroy();
	}
	break;
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#endif //__V18TICKET

#ifndef __V18TICKET
void CWndSelectCh::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndSelectCh::OnLButtonDown(UINT nFlags, CPoint point)
{
}
#endif //__V18TICKET

#endif //__AZRIA_1023