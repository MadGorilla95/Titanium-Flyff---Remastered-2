#include "stdafx.h"
#include "resData.h"
#include "WndAegonLootBox.h"
#include "DPClient.h"
#include "defineText.h"
#include "WndManager.h"
#include "AegonLootBox.h"
#include "WndManager.h"
#include "Model.h"
#include "ModelMng.h"
#include "ModelObject.h"
#include "ModelGlobal.h"
#include "math.h"




extern CDPClient g_DPlay;
extern char* FindDstString(int nDstParam);
extern BOOL IsDst_Rate(int nDstParam);

#ifdef __AEGON_LOOT_BOX

DWORD WINAPI __LootboxAnimationThread(LPVOID lpParam)
{
	CWndLootBoxMain* pLootBoxMain = (CWndLootBoxMain*)lpParam;
	pLootBoxMain->StartResultAni();
	return 0;
}


///////////////////////////
///////INFO WINDOW/////////
///////////////////////////
CWndLootBoxInfo::CWndLootBoxInfo()
{

}
CWndLootBoxInfo::~CWndLootBoxInfo()
{
}
void CWndLootBoxInfo::OnDraw(C2DRender* p2DRender)
{
}
void CWndLootBoxInfo::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	//Move Window to center
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();

}
BOOL CWndLootBoxInfo::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_LOOTBOX, 0, CPoint(0, 0), pWndParent);
}
void CWndLootBoxInfo::OnDestroy()
{
	Destroy();
}
BOOL CWndLootBoxInfo::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndLootBoxInfo::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndLootBoxInfo::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndLootBoxInfo::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndLootBoxInfo::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CString strOut;
	if (nID == WIDC_CANCEL || nID == WTBID_CLOSE)
	{
		OnDestroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);

}
void CWndLootBoxInfo::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}
BOOL CWndLootBoxInfo::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	return TRUE;
}


///////////////////////////
///////MAIN WINDOW/////////
///////////////////////////
CWndLootBoxMain::CWndLootBoxMain()
{
	m_pTicketElem = NULL;
	m_pTextureTicket = NULL;
	m_nSelectedBox = -1;
	m_bReceivedResult = FALSE;
	m_result.Init();
	m_aniInProgress = FALSE;
	m_aniDelay = 0;
	m_aniStep = 0;
	m_aniNeedsInit = TRUE;

	for(int i = 0; i < LOOTBOXITEMS; i++)
	{
		m_pTextureItems[i] = NULL;
	}

	for (int i = 0; i < LOOTBOXTICKETS; i++)
	{
		m_pTextureTickets[i] = NULL;
	}

	for (int i = 0; i < LOOTBOX_ITEM_BG_TEXTURES; i++)
	{
		m_pItemBGTextures[i] = NULL;
	}

}
CWndLootBoxMain::~CWndLootBoxMain()
{
	TerminateThread(m_hAniThread, 0);
	RemoveTicket(TRUE);	
}
void CWndLootBoxMain::OnDraw(C2DRender* p2DRender)
{
	CWndComboBox* boxesComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	LPWNDCTRL wndCtrlTicket = GetWndCtrl(WIDC_STATIC1);
	CWndStatic* possibleItems = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
	CWndStatic* possibleTickets = (CWndStatic*)GetDlgItem(WIDC_STATIC5);
	CRect rectTicket = wndCtrlTicket->rect;
	CEditString strEditOut;
	CString strOut;
	DWORD colorBlack = D3DCOLOR_ARGB(255, 0, 0, 0);
	CPoint pointMouse = GetMousePoint();

	CWndStatic* pWndStaticArrow = (CWndStatic*)GetDlgItem(WIDC_STATIC8);
	if (pWndStaticArrow)
	{
		CTexture* pWndTextureArrow = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(m_pApp->m_pd3dDevice, "LootboxNewArrow.tga", COLOR_ITEM_BG, TRUE);
		if (pWndTextureArrow)
		{
			CPoint ptRender = pWndStaticArrow->GetWndRect().CenterPoint();
			ptRender.x -= pWndTextureArrow->m_size.cx / 2;
			ptRender.y -= pWndTextureArrow->m_size.cy / 2;
			pWndTextureArrow->Render(p2DRender, ptRender);
		}
	}

	if(m_nSelectedBox != boxesComboBox->GetCurSel())
	{
		m_nSelectedBox = boxesComboBox->GetCurSel();
		OnSelBoxChanged();
	}

	if (m_nSelectedBox < 0)
		return;

	map<DWORD, __LOOTBOX_UNIT>::iterator it = CAegonLootBoxMng::GetInstance()->m_mapLootBoxes.find(m_nSelectedBox);

	if (it == CAegonLootBoxMng::GetInstance()->m_mapLootBoxes.end())
		return;

	
	//p2DRender->RenderRect(possibleTickets->GetWndRect(), COLOR_ERROR);

	for (int i = 0; i < it->second.ticketCount; i++)
	{
		p2DRender->RenderTexture(m_TicketFields[i].GetWndRect().TopLeft(), m_pTextureTickets[i]);

		if(possibleTickets->GetWndRect().PtInRect(pointMouse))
		{
			CRect toolTipRect = m_TicketFields[i].GetWndRect();
			if (toolTipRect.PtInRect(pointMouse))
			{
				ClientToScreen(&pointMouse);
				ClientToScreen(&toolTipRect);
				g_WndMng.PutToolTip_Item((CItemBase*)&m_TicketItems[i], pointMouse, &toolTipRect);
			}
		}
		
		
	}

	//render ticket
	if (m_pTextureTicket)
		m_pTextureTicket->Render(p2DRender, CPoint(wndCtrlTicket->rect.left, wndCtrlTicket->rect.top));

	//ticket tooltip
	if (rectTicket.PtInRect(pointMouse))
	{
		if (m_pTicketElem != NULL)
		{
			ClientToScreen(&pointMouse);
			ClientToScreen(&rectTicket);
			g_WndMng.PutToolTip_Item((CItemBase*)m_pTicketElem, pointMouse, &rectTicket);
		}
		else
		{
			ClientToScreen(&pointMouse);
			ClientToScreen(&rectTicket);
			CString strEmptyTooltip;
			strEmptyTooltip.Format("Place the coupon you want to use here.");
#ifdef _WIN64
			g_toolTip.PutToolTip(reinterpret_cast<ULONG_PTR>(this), strEmptyTooltip, rectTicket, pointMouse);
#else
			g_toolTip.PutToolTip(reinterpret_cast<DWORD>(this), strEmptyTooltip, rectTicket, pointMouse);
#endif
		}
	}

	
	//p2DRender->RenderRect(possibleItems->GetWndRect(), COLOR_ERROR);

	for (int i = 0; i < it->second.itemCount; i++)
	{
		p2DRender->RenderTexture(m_ItemFields[i].GetWndRect().TopLeft(), m_pTextureItems[i]);

		//item num
		strOut.Format("%dx", it->second.itemNums[i]);
		strEditOut.SetParsingString(strOut);
#ifdef __LEESE_RENDER_ITEMCOUNT
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemCount);
#endif //__LEESE_RENDER_ITEMCOUNT
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 255, 185, 18)); // 0 , 150 , 0
		strEditOut.SetStyle(ESSTY_BOLD);
		p2DRender->TextOut_EditString(m_ItemFields[i].GetWndRect().right - p2DRender->m_pFont->GetTextExtent(strEditOut.GetString()).cx, m_ItemFields[i].GetWndRect().bottom - p2DRender->m_pFont->GetTextExtent(strEditOut.GetString()).cy, strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD);
#ifdef __LEESE_RENDER_ITEMCOUNT
		p2DRender->SetFont(pOldFont);
#endif //__LEESE_RENDER_ITEMCOUNT
		//~item num

		//item chance
		strOut.Format("%d%%", it->second.itemChance[i] / 100);
		strEditOut.SetParsingString(strOut);
#ifdef __AEGON_THEME_SWITCHER
		DWORD dwColor = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
		strEditOut.SetColor(dwColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 0, 0, 0));
#endif //__AEGON_THEME_SWITCHER
		strEditOut.SetStyle(ESSTY_BOLD);
		p2DRender->TextOut_EditString(m_ItemFields[i].GetWndRect().CenterPoint().x - (p2DRender->m_pFont->GetTextExtent(strEditOut.GetString()).cx / 2), m_ItemFields[i].GetWndRect().bottom, strEditOut, 0, 0, 2);
		//~item chance

		//p2DRender->RenderRect(m_ItemFields[i].GetWndRect(), COLOR_SUCCESS);

		if(possibleItems->GetWndRect().PtInRect(pointMouse))
		{
			CRect toolTipRect = m_ItemFields[i].GetWndRect();
			if (toolTipRect.PtInRect(pointMouse))
			{
				ClientToScreen(&pointMouse);
				ClientToScreen(&toolTipRect);
				g_WndMng.PutToolTip_Item((CItemBase*)&m_BoxItems[i], pointMouse, &toolTipRect);
			}
		}
		
	}

	RenderAni(p2DRender);
}
void CWndLootBoxMain::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	//Move Window to center
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();

	CWndStatic* possibleItems = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
	LONG firstItemFieldOffset = 0;
	firstItemFieldOffset = ((((possibleItems->GetWndRect().right - possibleItems->GetWndRect().left) / 35) - LOOTBOXITEMS) * 35) / 2;

	for (int i = 0; i < LOOTBOXITEMS; i++)
	{
		CRect tmpRect;
		tmpRect.top = possibleItems->GetWndRect().top;
		tmpRect.bottom = possibleItems->GetWndRect().bottom;
		tmpRect.left = possibleItems->GetWndRect().left + firstItemFieldOffset + (i * 35);
		tmpRect.right = tmpRect.left + 35;
		m_ItemFields[i].Create("", 0, tmpRect, this, i);
	}

	CWndStatic* possibleTickets = (CWndStatic*)GetDlgItem(WIDC_STATIC5);

	for (int i = 0; i < LOOTBOXTICKETS; i++)
	{
		CRect tmpRect;
		tmpRect.top = possibleTickets->GetWndRect().top;
		tmpRect.bottom = possibleTickets->GetWndRect().bottom;
		tmpRect.left = possibleTickets->GetWndRect().left + (i * 35);
		tmpRect.right = tmpRect.left + 35;
		m_TicketFields[i].SetWndRect(tmpRect, TRUE);
	}


	m_pItemBGTextures[0] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "lootbox_bg_platin.tga"), COLOR_ITEM_BG);
	m_pItemBGTextures[1] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "lootbox_bg_gold.tga"), COLOR_ITEM_BG);
	m_pItemBGTextures[2] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "lootbox_bg_silver.tga"), COLOR_ITEM_BG);
	m_pItemBGTextures[3] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "lootbox_bg_bronze.tga"), COLOR_ITEM_BG);
	m_pItemBGTextures[4] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "lootbox_bg_rare.tga"), COLOR_ITEM_BG);
	m_pItemBGTextures[5] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "lootbox_bg_common.tga"), COLOR_ITEM_BG);

	CWndComboBox* boxesComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	boxesComboBox->ResetContent();
	boxesComboBox->AddWndStyle(EBS_READONLY);

	map<DWORD, __LOOTBOX_UNIT>::iterator it = CAegonLootBoxMng::GetInstance()->m_mapLootBoxes.begin();
	for (; it != CAegonLootBoxMng::GetInstance()->m_mapLootBoxes.end(); it++)
	{
		boxesComboBox->AddString(it->second.boxName);
	}
	if (boxesComboBox->GetListBoxSize())
		boxesComboBox->SetCurSel(0);

	OnSelBoxChanged();

	g_DPlay.SendLootBoxCollect();

}
BOOL CWndLootBoxMain::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_LOOTBOX, 0, CPoint(0, 0), pWndParent);
}
void CWndLootBoxMain::OnDestroy()
{
	Destroy();
}
BOOL CWndLootBoxMain::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndLootBoxMain::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndLootBoxMain::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndLootBoxMain::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndLootBoxMain::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CString strOut;
	if (nID == WIDC_CANCEL || nID == WTBID_CLOSE)
	{
		OnDestroy();
	}
	else if(nID == WIDC_BUTTON1)
	{
		if (m_aniNeedsInit)
			InitAni();

		//start
		if (m_nSelectedBox < 0)
		{
			strOut.Format("Error invalid Box!");
			g_WndMng.PutString(strOut.GetString(), NULL, COLOR_ERROR);
		}
		else if(m_pTicketElem)
		{
			g_DPlay.SendLootBoxStart(m_nSelectedBox, m_pTicketElem->m_dwObjId, LOOTBOX_BUY_TICKET);
		}
		else 
		{
			if (g_Option.m_bSkipDPConfirmLootbox) 
			{
				g_DPlay.SendLootBoxStart(m_nSelectedBox, 0, LOOTBOX_BUY_DP);
			}
			else 
			{
				map<DWORD, __LOOTBOX_UNIT>::iterator it = CAegonLootBoxMng::GetInstance()->m_mapLootBoxes.find(m_nSelectedBox);

				if (it != CAegonLootBoxMng::GetInstance()->m_mapLootBoxes.end())
				{
					CWndIngameDPPurchaseConfirm* pWndConfirmPurchase = g_WndMng.OpenDPConfirmPurchase();
					if (pWndConfirmPurchase)
					{
						pWndConfirmPurchase->m_nCommand = INGAMEDP_BUYLOOTBOX;
						pWndConfirmPurchase->m_nSelectedBox = m_nSelectedBox;

						pWndConfirmPurchase->SetInfo("", it->second.dpPrice);
					}
				}
			}			
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);

}
void CWndLootBoxMain::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	LPWNDCTRL wndCtrlTicket = GetWndCtrl(WIDC_STATIC1);
	CRect rectTicket = wndCtrlTicket->rect;

	if (rectTicket.PtInRect(point))
	{
		RemoveTicket();
	}
}
BOOL CWndLootBoxMain::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	LPWNDCTRL wndCtrlTicket = GetWndCtrl(WIDC_STATIC1);
	CRect rectTicket = wndCtrlTicket->rect;

	CItemElem* pTempElem;
	pTempElem = (CItemElem*)g_pPlayer->GetItemId(pShortcut->m_dwId);

	
	if (rectTicket.PtInRect(point))
	{
		SetTicket(pTempElem);
	}

	return TRUE;
}
void CWndLootBoxMain::OnSelBoxChanged()
{
	SetTicket(m_pTicketElem);

	

	CWndComboBox* boxesComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);

	map<DWORD, __LOOTBOX_UNIT>::iterator it = CAegonLootBoxMng::GetInstance()->m_mapLootBoxes.find(boxesComboBox->GetCurSel());

	if(it != CAegonLootBoxMng::GetInstance()->m_mapLootBoxes.end())
	{
		CWndStatic* possibleItems = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
		LONG firstItemFieldOffset = 0;
		firstItemFieldOffset = ((((possibleItems->GetWndRect().right - possibleItems->GetWndRect().left) / 35) - it->second.itemCount) * 35) / 2;

		for (int i = 0; i < it->second.itemCount; i++)
		{
			if (it->second.itemIDs[i] != 0)
			{

				m_BoxItems[i].m_dwItemId = it->second.itemIDs[i];
				m_BoxItems[i].m_nItemNum = it->second.itemNums[i];

				CString strPath;
				strPath.Format(DIR_ITEM  "%s", m_BoxItems[i].GetPropA()->szIcon);
				m_pTextureItems[i] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, strPath, COLOR_ITEM_BG);
			

			}


			CRect tmpRect;
			tmpRect.top = possibleItems->GetWndRect().top;
			tmpRect.bottom = possibleItems->GetWndRect().bottom;
			tmpRect.left = possibleItems->GetWndRect().left + firstItemFieldOffset + (i * 35);
			tmpRect.right = tmpRect.left + 35;
			m_ItemFields[i].SetWndRect(tmpRect, TRUE);
		}

		for(int i = 0; i < it->second.ticketCount; i++)
		{
			m_TicketItems[i].m_dwItemId = it->second.ticketIDs[i];
			m_TicketItems[i].m_nItemNum = 1;

			CString strPath;
			strPath.Format(DIR_ITEM  "%s", m_TicketItems[i].GetPropA()->szIcon);
			m_pTextureTickets[i] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, strPath, COLOR_ITEM_BG);
		}

		RefreshPrice();
	}

	InitAni();
}
void CWndLootBoxMain::ReceiveResult(__LOOTBOX_RESULT result)
{
	if (result.validResult)
	{
		TerminateThread(m_hAniThread, 0);
		m_result = result;
		m_aniInProgress = TRUE;
		m_hAniThread = CreateThread(0, 0, __LootboxAnimationThread, this, 0, 0);
	}	

	RemoveTicket(TRUE);


}
void CWndLootBoxMain::RemoveTicket(BOOL force)
{
	if (m_aniInProgress && !force)
		return;

	if (m_pTicketElem != NULL)
	{
		m_pTicketElem->SetExtra(0);
		m_pTicketElem = NULL;
		m_pTextureTicket = NULL;

		RefreshPrice();
	}
}
void CWndLootBoxMain::SetTicket(CItemElem* pTicketElem)
{
	CString strOut;
	CWndComboBox* boxesComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);

	if (m_aniInProgress)
		return;

	map<DWORD, __LOOTBOX_UNIT>::iterator it = CAegonLootBoxMng::GetInstance()->m_mapLootBoxes.find(boxesComboBox->GetCurSel());

	if (it != CAegonLootBoxMng::GetInstance()->m_mapLootBoxes.end())
	{
		if (!pTicketElem || pTicketElem->m_nItemNum <= 0)
			return;

		if(!it->second.IsValidTicket(pTicketElem))
		{
			if (pTicketElem == m_pTicketElem)
			{
				RemoveTicket();
			}
			else
			{
				strOut.Format("Invalid ticket!");
				g_WndMng.PutString(strOut.GetString(), NULL, COLOR_ERROR);
			}
			return;
		}


		RemoveTicket();

		m_pTicketElem = pTicketElem;
		m_pTicketElem->SetExtra(m_pTicketElem->GetExtra() + 1);
		m_pTextureTicket = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, m_pTicketElem->GetPropA()->szIcon), COLOR_ITEM_BG);
	}

	RefreshPrice();
}
void CWndLootBoxMain::InitAni()
{
	CWndStatic* aniWnd = (CWndStatic*)GetDlgItem(WIDC_STATIC7);
	CWndComboBox* boxesComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);

	map<DWORD, __LOOTBOX_UNIT>::iterator it = CAegonLootBoxMng::GetInstance()->m_mapLootBoxes.find(boxesComboBox->GetCurSel());

	if (it != CAegonLootBoxMng::GetInstance()->m_mapLootBoxes.end())
	{
		for (int i = 0; i < it->second.itemCount; i++)
		{
			CRect rc;
			m_pAniTextures[i] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, prj.GetItemProp(it->second.itemIDs[i])->szIcon), COLOR_ITEM_BG);
		}
		m_aniItemCount = it->second.itemCount;

		for (int i = 0; i < LOOTBOX_ANI_ELEMENTS; i++)
		{
			__LOOTBOX_RESULT result;
			it->second.GetRandomResult(&result);
			m_pAllAniBGTextures[i] = GetItemBGTexture(result.chance);
			m_pAllAniTextures[i] = m_pAniTextures[result.itemIndex];
			
		}
	}

	m_aniPt = aniWnd->GetWndRect().TopLeft();
	m_aniNeedsInit = FALSE;
}

void CWndLootBoxMain::StartResultAni()
{
	CWndStatic* aniWnd = (CWndStatic*)GetDlgItem(WIDC_STATIC7);
	CWndStatic* winArrow = (CWndStatic*)GetDlgItem(WIDC_STATIC8);
	SetButtonMode(1);
	m_aniDelay = 10;
	m_lastAniStep = 0;
	m_aniStep = 20;

	m_pAniWinTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, prj.GetItemProp(m_result.receiveItem)->szIcon), COLOR_ITEM_BG);
	m_pAniWinBGTexture = GetItemBGTexture(m_result.chance);

	m_pAllAniTextures[LOOTBOX_ANI_ELEMENTS - 10] = m_pAniWinTexture;
	m_pAllAniBGTextures[LOOTBOX_ANI_ELEMENTS - 10] = m_pAniWinBGTexture;

	winTextMinx = aniWnd->GetWndRect().left + ((LOOTBOX_ANI_ELEMENTS - 10) * m_pAniWinBGTexture->m_size.cx);

	while(m_aniInProgress)
	{
		if(GetTickCount() > m_lastAniStep + m_aniDelay)
			DoAniStep();

		CWndButton* pWndCheckAni = (CWndButton*)GetDlgItem(WIDC_CHECK1);
		if (pWndCheckAni && pWndCheckAni->GetCheck())
		{
			LONG tmp = ((-1) * winTextMinx + (winArrow->GetWndRect().left + ((winArrow->GetWndRect().right - winArrow->GetWndRect().left) / 2))) - m_pAniWinBGTexture->m_size.cx / 2;
			if (tmp < m_aniPt.x)
				m_aniPt.x = tmp;
		}

		if (m_aniPt.x - (winArrow->GetWndRect().left + ((winArrow->GetWndRect().right - winArrow->GetWndRect().left) / 2))  < ((-1)* winTextMinx) - (m_pAniWinBGTexture->m_size.cx / 3))
		{
			m_aniInProgress = FALSE;
			g_DPlay.SendLootBoxCollect();
			SetButtonMode(0);
			m_aniNeedsInit = TRUE;			
			return;
		}
	}
}
void CWndLootBoxMain::DoAniStep()
{
	CWndStatic* aniWnd = (CWndStatic*)GetDlgItem(WIDC_STATIC7);

	m_aniPt.x -= m_aniStep;
	m_lastAniStep = GetTickCount();

	if (m_aniPt.x < (-1) * (aniWnd->GetWndRect().left + ((LOOTBOX_ANI_ELEMENTS * 0.9) * m_pAniWinBGTexture->m_size.cx)))
		m_aniStep = 2;
	else if (m_aniPt.x < (-1) * (aniWnd->GetWndRect().left + ((LOOTBOX_ANI_ELEMENTS * 0.8) * m_pAniWinBGTexture->m_size.cx)))
		m_aniStep = 4;
	else if (m_aniPt.x < (-1) * (aniWnd->GetWndRect().left + ((LOOTBOX_ANI_ELEMENTS * 0.7) * m_pAniWinBGTexture->m_size.cx)))
		m_aniStep = 8;
	else if (m_aniPt.x < (-1) * (aniWnd->GetWndRect().left + ((LOOTBOX_ANI_ELEMENTS * 0.6) * m_pAniWinBGTexture->m_size.cx)))
		m_aniStep = 16;

}
void CWndLootBoxMain::RenderAni(C2DRender* p2DRender)
{
	CWndStatic* aniWnd = (CWndStatic*)GetDlgItem(WIDC_STATIC7);

	CWndStatic* winArrow = (CWndStatic*)GetDlgItem(WIDC_STATIC8);
	
	for(int i = 0; i < LOOTBOX_ANI_ELEMENTS; i++)
	{
		CPoint tmpPt = m_aniPt;
		tmpPt.x += i * m_pAllAniBGTextures[i]->m_size.cx;

		DWORD alpha = 255;

		if (tmpPt.x + m_pAllAniBGTextures[i]->m_size.cx > 0 && tmpPt.x - m_pAllAniBGTextures[i]->m_size.cx < aniWnd->GetWndRect().right)
		{
			p2DRender->RenderTexture(tmpPt, m_pAllAniBGTextures[i], alpha);
			p2DRender->RenderTexture(CPoint(tmpPt.x + (m_pAllAniBGTextures[i]->m_size.cx / 2) - (m_pAllAniTextures[i]->m_size.cx / 2), tmpPt.y + (m_pAllAniBGTextures[i]->m_size.cy / 2) - (m_pAllAniTextures[i]->m_size.cy / 2)), m_pAllAniTextures[i], alpha);
		}
	}
}
void CWndLootBoxMain::SetButtonMode(int mode) 
{
	CWndComboBox* boxesComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	CWndButton* startButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	CWndButton* cancelButton = (CWndButton*)GetDlgItem(WIDC_CANCEL);

	switch(mode)
	{
	case 0: //all buttons available
		boxesComboBox->EnableWindow(TRUE);
		startButton->EnableWindow(TRUE);
		cancelButton->EnableWindow(TRUE);
		break;
	case 1: //ani in progress
		boxesComboBox->EnableWindow(FALSE);
		startButton->EnableWindow(FALSE);
		cancelButton->EnableWindow(FALSE);
		break;
	}
}

CTexture* CWndLootBoxMain::GetItemBGTexture(DWORD chance) 
{

	if (chance <= 100)
		return m_pItemBGTextures[0];
	else if (chance <= 500)
		return m_pItemBGTextures[1];
	else if (chance <= 1000)
		return m_pItemBGTextures[2];
	else if (chance <= 1500)
		return m_pItemBGTextures[3];
	else if (chance <= 2000)
		return m_pItemBGTextures[4];
	else 
		return m_pItemBGTextures[5];

}

void CWndLootBoxMain::RefreshPrice() 
{
	CWndStatic* priceWnd = (CWndStatic*)GetDlgItem(WIDC_STATIC9);

	CString tmpStr;

	map<DWORD, __LOOTBOX_UNIT>::iterator it = CAegonLootBoxMng::GetInstance()->m_mapLootBoxes.find(m_nSelectedBox);

	if (it == CAegonLootBoxMng::GetInstance()->m_mapLootBoxes.end()) {
		tmpStr.Format("Price: Box Error");
	}
	else if (m_pTicketElem)
		tmpStr.Format("Price: %s", m_pTicketElem->GetName().GetString());
	else {
		tmpStr.Format("Price: %d DP", it->second.dpPrice);
	}

	priceWnd->SetTitle(tmpStr);
}
#endif // __AEGON_LOOT_BOX