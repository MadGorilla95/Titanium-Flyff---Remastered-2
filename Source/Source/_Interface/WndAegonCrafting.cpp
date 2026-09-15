#include "stdafx.h"
#include "resData.h"
#include "WndAegonCrafting.h"
#include "DPClient.h"
#include "defineText.h"
#include "WndManager.h"
#include "AegonCrafting.h"
#include "WndManager.h"
#include "Model.h"
#include "ModelMng.h"
#include "ModelObject.h"
#include "ModelGlobal.h"
#include "math.h"

extern CDPClient g_DPlay;

#ifdef __AEGON_CRAFTING_SYSTEM

DWORD WINAPI __StartCraftingThread(LPVOID lpParam)
{
	CWndCraftingMain* pCraftingMain = (CWndCraftingMain*)lpParam;

	pCraftingMain->m_bCraftingInProgress = TRUE;

	pCraftingMain->CraftingProcess();
	pCraftingMain->OnThreadEnd();


	return 0;
}
/*
DWORD WINAPI __SearchThread(LPVOID lpParam)
{
	CWndCraftingMain* pCraftingMain = (CWndCraftingMain*)lpParam;

	pCraftingMain->m_pWndComboSearch->m_wndListBox.ResetContent();
	CAegonCraftingMng::GetInstance()->FillSearchComboBox(pCraftingMain->m_pWndComboSearch);


	if (!pCraftingMain->m_pWndComboSearch->m_bOpen)
		pCraftingMain->m_pWndComboSearch->OpenListBox();

	return 0;
}
*/
///////////////////////////
///////MAIN WINDOW/////////
///////////////////////////
CWndCraftingMain::CWndCraftingMain()
{
	m_pWndRecipeName = NULL;
	m_pWndDescription = NULL;
	m_pWndResultItemText = NULL;
	m_pWndGauge = NULL;

	m_pWndResultItem = NULL;
	for(int i = 0; i < AEGCRAFT_MAXCOMPONENTS; i++)
		m_pWndComponentItem[i] = NULL;


	m_pWndTreeCtrl = NULL;

	m_pWndCraftButton = NULL;
	m_pWndCancelButton = NULL;

	m_pWndPlusButton = NULL;
	m_pWndMinusButton = NULL;
	m_pWndMaxButton = NULL;

	m_pWndComboSearch = NULL;
	m_pWndCraftNum = NULL;

	m_pItemBgTexture = NULL;
	m_pResultItemTexture = NULL;

	for (int i = 0; i < AEGCRAFT_MAXCOMPONENTS; i++)
	{
		m_pComponentTexture[i] = NULL;
		m_pComponentItem[i] = NULL;
	}

	m_selectedUnit = NULL;

	m_bCraftingInProgress = FALSE;

	m_hCraftingThread = NULL;

	m_nCraftedItems = 0;
	m_bAbortCrafting = FALSE;

	m_pActiveProgressTexture = NULL;
	for (int i = 0; i < AEGCRAFT_PROGRESSELEMENTS; i++)
		m_pProgressTexture[i] = NULL;

	m_pWndSearchButton = NULL;

	m_bRecyclingPage = FALSE;

	m_wndRecyclingList = NULL;
	
}
CWndCraftingMain::~CWndCraftingMain()
{
	TerminateThread(m_hCraftingThread, 0);
}
void CWndCraftingMain::OnDraw(C2DRender* p2DRender)
{
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	CString tmpStr;
	CEditString strEditOut;
	strEditOut.Init(GetFont(), &m_pWndDescription->GetWndRect());

	//description
	CPoint point = m_pWndDescription->GetWndRect().TopLeft();
	m_descriptionString.SetColor(dwColorText);
	p2DRender->TextOut_EditString(point.x, point.y, m_descriptionString, 0, 0, 2);
	//~description

	if (m_bRecyclingPage)
	{
		//item bg
		if (m_pItemBgTexture)
		{
			p2DRender->RenderTexture(m_pWndResultItem->GetWndRect().TopLeft(), m_pItemBgTexture);

			p2DRender->RenderTexture(m_pWndComponentItem[0]->GetWndRect().TopLeft(), m_pItemBgTexture);
		}		
		//~item bg

		if (m_selectedUnit)
		{
			if (m_pComponentItem[0])
			{

				if(m_pComponentTexture[0])
					p2DRender->RenderTexture(m_pWndComponentItem[0]->GetWndRect().TopLeft(), m_pComponentTexture[0]);

				tmpStr.Format("%d/%d\t%s", m_pComponentItem[0]->GetExtra(), m_selectedUnit->components[0].m_itemNum, m_pComponentItem[0]->GetPropA()->szName);
				strEditOut.SetParsingString(tmpStr);

				point.x = m_pWndComponentItem[0]->GetWndRect().right + 10;
				point.y = m_pWndComponentItem[0]->GetWndRect().top + (m_pWndComponentItem[0]->GetWndRect().Height() / 2) - (p2DRender->GetFont()->GetTextExtent(tmpStr.GetString()).cy / 2);
				//strEditOut.SetStyle(0, strEditOut.GetLength(), ESSTY_BOLD);
				strEditOut.SetColor(dwColorText);
				p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
			}
			if (m_pResultItemTexture) 
			{
				p2DRender->RenderTexture(m_pWndResultItem->GetWndRect().TopLeft(), m_pResultItemTexture);
			}

			//result item tooltip
			CRect rect = m_pWndResultItem->GetWndRect();
			CPoint pointMouse = GetMousePoint();
			if (rect.PtInRect(pointMouse))
			{
				CItemElem itemElem;
				itemElem.m_dwItemId = m_selectedUnit->resultItemID;
				itemElem.m_nItemNum = m_selectedUnit->resultItemNum;

				ClientToScreen(&pointMouse);
				ClientToScreen(&rect);
				g_WndMng.PutToolTip_Item(&itemElem, pointMouse, &rect);

			}
			//~result item tooltip
			//component tooltip
			rect = m_pWndComponentItem[0]->GetWndRect();
			pointMouse = GetMousePoint();
			if (rect.PtInRect(pointMouse))
			{
				ClientToScreen(&pointMouse);
				ClientToScreen(&rect);
				g_WndMng.PutToolTip_Item((CItemBase*)m_pComponentItem[0], pointMouse, &rect);
			}
			//~component tooltip
		}
		else 
		{
			tmpStr.Format("Place the item you want to recycle here.");
			strEditOut.SetParsingString(tmpStr);

			point.x = m_pWndComponentItem[0]->GetWndRect().right + 10;
			point.y = m_pWndComponentItem[0]->GetWndRect().top + (m_pWndComponentItem[0]->GetWndRect().Height() / 2) - (p2DRender->GetFont()->GetTextExtent(tmpStr.GetString()).cy / 2);
			//strEditOut.SetStyle(0, strEditOut.GetLength(), ESSTY_BOLD);
			strEditOut.SetColor(dwColorText);
			p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		}
		

	}
	else if (m_selectedUnit)
	{




		//result item and bg
		if (m_pItemBgTexture)
		{
			p2DRender->RenderTexture(m_pWndResultItem->GetWndRect().TopLeft(), m_pItemBgTexture);

			if (m_pResultItemTexture)
			{
				p2DRender->RenderTexture(m_pWndResultItem->GetWndRect().TopLeft(), m_pResultItemTexture);
			}
		}
		//~result item and bg

		//components and bg
		for (int i = 0; i < m_selectedUnit->components.size(); i++) 
		{
			if (m_pItemBgTexture) 
			{
				p2DRender->RenderTexture(m_pWndComponentItem[i]->GetWndRect().TopLeft(), m_pItemBgTexture);
			}
			if (m_pComponentTexture[i] && m_pComponentItem[i])
			{
				p2DRender->RenderTexture(m_pWndComponentItem[i]->GetWndRect().TopLeft(), m_pComponentTexture[i]);
				tmpStr.Format("%d/%d\t%s", m_pComponentItem[i]->GetExtra(), m_selectedUnit->components[i].m_itemNum, prj.GetItemProp(m_selectedUnit->components[i].m_itemID)->szName);
				strEditOut.SetParsingString(tmpStr);
				//if(m_pComponentItem[i]->GetExtra() >= m_selectedUnit->components[i].m_itemNum)
				//	strEditOut.SetStyle(strEditOut.GetLength() - strlen(prj.GetItemProp(m_selectedUnit->components[i].m_itemID)->szName), strlen(prj.GetItemProp(m_selectedUnit->components[i].m_itemID)->szName), ESSTY_STRIKETHROUGH);
			}
			else if (m_pComponentTexture[i]) 
			{
				p2DRender->RenderTexture(m_pWndComponentItem[i]->GetWndRect().TopLeft(), m_pComponentTexture[i], 150);
				tmpStr.Format("%d/%d\t%s", 0, m_selectedUnit->components[i].m_itemNum, prj.GetItemProp(m_selectedUnit->components[i].m_itemID)->szName);
				strEditOut.SetParsingString(tmpStr);
			}
			point.x = m_pWndComponentItem[i]->GetWndRect().right + 10;
			point.y = m_pWndComponentItem[i]->GetWndRect().top + (m_pWndComponentItem[i]->GetWndRect().Height() / 2) - (p2DRender->GetFont()->GetTextExtent(tmpStr.GetString()).cy / 2);
			//strEditOut.SetStyle(0, strEditOut.GetLength(), ESSTY_BOLD);
			strEditOut.SetColor(dwColorText);
			p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		}
		//~components and bg

		



		//result item tooltip
		CRect rect = m_pWndResultItem->GetWndRect();
		CPoint pointMouse = GetMousePoint();
		if (rect.PtInRect(pointMouse))
		{
			CItemElem itemElem;
			itemElem.m_dwItemId = m_selectedUnit->resultItemID;
			itemElem.m_nItemNum = m_selectedUnit->resultItemNum;

			ClientToScreen(&pointMouse);
			ClientToScreen(&rect);
			g_WndMng.PutToolTip_Item(&itemElem, pointMouse, &rect);
			
		}
		//~result item tooltip

		//component tooltip
		for (int i = 0; i < m_selectedUnit->components.size(); i++)
		{

			CRect rect = m_pWndComponentItem[i]->GetWndRect();
			CPoint pointMouse = GetMousePoint();
			if (rect.PtInRect(pointMouse))
			{
				ClientToScreen(&pointMouse);
				ClientToScreen(&rect);
				if (m_pComponentItem[i])
				{
					g_WndMng.PutToolTip_Item((CItemBase*)m_pComponentItem[i], pointMouse, &rect);
				}
				else
				{
					CString strEmptyTooltip;
					strEmptyTooltip.Format("Place the '%s' component here.", prj.GetItemProp(m_selectedUnit->components[i].m_itemID)->szName);
#ifdef _WIN64
					g_toolTip.PutToolTip(reinterpret_cast<ULONG_PTR>(this), strEmptyTooltip, rect, pointMouse);
#else
					g_toolTip.PutToolTip(reinterpret_cast<DWORD>(this), strEmptyTooltip, rect, pointMouse);
#endif
				}
			}
		}
		//~component tooltip


	}
	else 
	{
		//draw logo?
	}

	if (m_bRecyclingPage || m_selectedUnit) 
	{
		//gauge bar
		if (m_pActiveProgressTexture)
			p2DRender->RenderTexture(m_pWndGauge->GetWndRect().TopLeft(), m_pActiveProgressTexture);

		CRect gaugeFrame = m_pWndGauge->GetWndRect();
		gaugeFrame.top -= 1;
		gaugeFrame.left -= 1;
		gaugeFrame.right += 1;
		gaugeFrame.bottom += 1;
		p2DRender->RenderRect(gaugeFrame, dwColorText);
		int craftAmount;
		try
		{
			craftAmount = stoi(m_pWndCraftNum->GetString());
		}
		catch (...)
		{
			craftAmount = 1;
			m_pWndCraftNum->SetTitle("1");
		}
		strEditOut.Init(GetFont(), &m_pWndGauge->GetWndRect());
		tmpStr.Format("%d/%d", m_nCraftedItems, craftAmount);
		strEditOut.SetParsingString(tmpStr);
		point.x = m_pWndGauge->GetWndRect().left - (p2DRender->m_pFont->GetTextExtent(tmpStr.GetString()).cx / 2) + (m_pWndGauge->GetWndRect().Width() / 2);
		point.y = m_pWndGauge->GetWndRect().top - (p2DRender->m_pFont->GetTextExtent(tmpStr.GetString()).cy / 2) + (m_pWndGauge->GetWndRect().Height() / 2);
		strEditOut.SetStyle(0, strEditOut.GetLength(), ESSTY_BOLD);
		strEditOut.SetColor(dwColorText);
		p2DRender->TextOut_EditString(point.x, point.y, strEditOut, 0, 0, 2);
		//~gauge bar
	}
	
	//line
	CPoint pLeft, pRight;
	pLeft.x = m_pWndResultItem->GetWndRect().left;
	pLeft.y = m_pWndMaxButton->GetWndRect().top - 10;
	pRight.x = m_pWndMaxButton->GetWndRect().right;
	pRight.y = pLeft.y;
	p2DRender->RenderLine(pLeft, pRight, dwColorText);
	//~line
}
void CWndCraftingMain::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	//Move Window to center
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();

	m_pWndRecyclingTitle = (CWndStatic*)GetDlgItem(WIDC_STATIC13);
	if (m_pWndRecyclingTitle)
		m_pWndRecyclingTitle->SetTitle("");

	m_pWndRecipeName = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	m_pWndDescription = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
	m_pWndResultItemText = (CWndStatic*)GetDlgItem(WIDC_STATIC4);
	m_pWndGauge = (CWndStatic*)GetDlgItem(WIDC_STATIC5);

	m_pWndResultItem = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
	m_pWndComponentItem[0] = (CWndStatic*)GetDlgItem(WIDC_STATIC7);
	m_pWndComponentItem[1] = (CWndStatic*)GetDlgItem(WIDC_STATIC8);
	m_pWndComponentItem[2] = (CWndStatic*)GetDlgItem(WIDC_STATIC9);
	m_pWndComponentItem[3] = (CWndStatic*)GetDlgItem(WIDC_STATIC10);
	m_pWndComponentItem[4] = (CWndStatic*)GetDlgItem(WIDC_STATIC11);
	m_pWndComponentItem[5] = (CWndStatic*)GetDlgItem(WIDC_STATIC12);


	m_pWndTreeCtrl = (CWndTreeCtrl*)GetDlgItem(WIDC_TREECTRL);

	m_pWndCraftButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	m_pWndCancelButton = (CWndButton*)GetDlgItem(WIDC_BUTTON5);

	m_pWndPlusButton = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
	m_pWndMinusButton = (CWndButton*)GetDlgItem(WIDC_BUTTON3);
	m_pWndMaxButton = (CWndButton*)GetDlgItem(WIDC_BUTTON4);

	m_pWndSearchButton = (CWndButton*)GetDlgItem(WIDC_BUTTON6);

	m_pWndComboSearch = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);

	m_pWndCraftNum = (CWndEdit*)GetDlgItem(WIDC_EDIT1);

	m_pWndCraftNum->AddWndStyle(EBS_NUMBER);
	m_pWndCraftNum->SetString("1");

	m_pWndTreeCtrl->LoadTreeScript(MakePath(DIR_CLIENT, "CraftingCategories.inc"));

#ifdef __AEGON_THEME_SWITCHER
	m_pItemBgTexture = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(g_Neuz.m_pd3dDevice, "WndChgElemItem.tga", COLOR_ITEM_BG, TRUE);
#else // __AEGON_THEME_SWITCHER
	m_pItemBgTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "WndChgElemItem.tga"), COLOR_ITEM_BG, TRUE);
#endif // __AEGON_THEME_SWITCHER

	m_pWndCancelButton->EnableWindow(FALSE);
	m_pWndCancelButton->SetVisible(FALSE);

	for (int i = 1; i < AEGCRAFT_PROGRESSELEMENTS; i++) 
	{
		CString tmpStr;
		tmpStr.Format("craft_bar%d.dds", i);
		m_pProgressTexture[i] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_CRAFTING, tmpStr.GetString()), COLOR_ITEM_BG);
	}

	CAegonCraftingMng::GetInstance()->FillSearchComboBox(m_pWndComboSearch);

	OnCraftingExpRefresh();

	SetEmptyRecipePage();
}
BOOL CWndCraftingMain::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_AEGON_CRAFTING, 0, CPoint(0, 0), pWndParent);
}
void CWndCraftingMain::OnDestroy()
{
	TerminateThread(m_hCraftingThread, 0);
	

	for (int i = 0; i < AEGCRAFT_MAXCOMPONENTS; i++)
	{
		if (m_pComponentItem[i] && !g_pPlayer->m_vtInfo.IsTrading(m_pComponentItem[i]))
		{
			RemoveComponentItem(i);
		}
	}

	m_selectedUnit = NULL;
}
BOOL CWndCraftingMain::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndCraftingMain::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndCraftingMain::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndCraftingMain::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndCraftingMain::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (!m_bCraftingInProgress || nID == WIDC_BUTTON5) 
	{
		switch (nID)
		{
		case WIDC_COMBOBOX1:
		{
			if (message == WNM_SELCHANGE)
			{
				CString searchString = m_pWndComboSearch->GetString();
				OpenRecipe(searchString);
			}
			break;
		}
		case WIDC_BUTTON1:
		{
			CString tmpStr;
			if (m_bRecyclingPage) 
			{
				if (!m_pComponentItem[0]) 
				{
					tmpStr.Format("Place the item you want to recycle.");
					g_WndMng.PutString(tmpStr.GetString(), 0, COLOR_ERROR);
					return FALSE;
				}
			}
			if (m_selectedUnit)
			{
				//check components
				for (int i = 0; i < m_selectedUnit->components.size(); i++)
				{
					if (!m_pComponentItem[i])
					{
						tmpStr.Format("Component '%s' missing.", prj.GetItemProp(m_selectedUnit->components[i].m_itemID)->szName);
						g_WndMng.PutString(tmpStr.GetString(), 0, COLOR_ERROR);
						return FALSE;
					}
					if (!(m_pComponentItem[i]->GetExtra() >= m_selectedUnit->components[i].m_itemNum))
					{
						tmpStr.Format("Too few '%s' components.", prj.GetItemProp(m_selectedUnit->components[i].m_itemID)->szName);
						g_WndMng.PutString(tmpStr.GetString(), 0, COLOR_ERROR);
						return FALSE;
					}
				}
				//check crafting level requirement
				if (CAegonCraftingMng::GetInstance()->GetCraftingLevel(g_pPlayer) < m_selectedUnit->levelRequirement)
				{
					tmpStr.Format("Crafting level too low. Required Level: %d", m_selectedUnit->levelRequirement);
					g_WndMng.PutString(tmpStr.GetString(), 0, COLOR_ERROR);
					return FALSE;
				}
				if (g_pPlayer->m_Inventory.GetEmptyCount() <= 0)
				{
					tmpStr.Format("Not enough room in your inventory.");
					g_WndMng.PutString(tmpStr.GetString(), 0, COLOR_ERROR);
					return FALSE;
				}
				int maxCraft = GetMaxPossibleCrafts();
				int nTmp;
				try
				{
					nTmp = stoi(m_pWndCraftNum->GetString());
				}
				catch (...)
				{
					nTmp = 1;
					m_pWndCraftNum->SetString("1");
					break;
				}
				if (nTmp < 1) 
				{
					nTmp = 1;
					m_pWndCraftNum->SetString("1");
				}
				else if (nTmp > maxCraft) 
				{
					tmpStr.Format("%d", maxCraft);
					m_pWndCraftNum->SetString(tmpStr);
				}
				if (m_pWndCraftButton->IsWindowEnabled())
				{
					m_pWndCraftButton->EnableWindow(FALSE);
					m_pWndCraftButton->SetVisible(FALSE);

					m_pWndCancelButton->EnableWindow(TRUE);
					m_pWndCancelButton->SetVisible(TRUE);

					m_pWndCraftNum->EnableWindow(FALSE);
					m_pWndTreeCtrl->EnableWindow(FALSE);

					m_nCraftedItems = 0;
					m_hCraftingThread = CreateThread(0, 0, __StartCraftingThread, this, 0, 0);



					if (!m_hCraftingThread)
					{
						m_pWndCraftButton->EnableWindow(TRUE);
						m_pWndCraftButton->SetVisible(TRUE);

						m_pWndCancelButton->EnableWindow(FALSE);
						m_pWndCancelButton->SetVisible(FALSE);

						m_pWndCraftNum->EnableWindow(TRUE);
						m_pWndTreeCtrl->EnableWindow(TRUE);
					}
				}
			}
			break;
		}
		case WIDC_BUTTON2: //Plus
		{
			CString tmpStr;
			int maxCraft = GetMaxPossibleCrafts();
			int nTmp;
			try
			{
				nTmp = stoi(m_pWndCraftNum->GetString());
			}
			catch (...)
			{
				break;
			}
			if (++nTmp > maxCraft)
			{
				nTmp = maxCraft;
			}
			if (nTmp < 1)
				nTmp = 1;
			tmpStr.Format("%d", nTmp);
			m_pWndCraftNum->SetString(tmpStr);
			break;
		}
		case WIDC_BUTTON3: //Minus
		{
			CString tmpStr;
			int nTmp;
			try
			{
				nTmp = stoi(m_pWndCraftNum->GetString());
			}
			catch (...)
			{
				break;
			}
			if (--nTmp < 1)
			{
				nTmp = 1;
			}
			tmpStr.Format("%d", nTmp);
			m_pWndCraftNum->SetString(tmpStr);
			break;
		}
		case WIDC_BUTTON4: //Max
		{
			CString tmpStr;
			//TODO calculate max
			int maxCraft = GetMaxPossibleCrafts();
			if (maxCraft < 1)
				maxCraft = 1;
			tmpStr.Format("%d", maxCraft);
			m_pWndCraftNum->SetString(tmpStr);
			break;
		}
		case WIDC_BUTTON5: //Cancel
		{
			if (m_bCraftingInProgress) 
			{
				m_bAbortCrafting = TRUE;
			}
			break;
		}
		case WIDC_BUTTON6: //search
		{
			UpdateSearch();
			break;
		}
		case WIDC_TREECTRL:
		{
			if (message == WNM_DBLCLK)
				return FALSE;

			LPTREEELEM lpTreeElem = (LPTREEELEM)pLResult;
			BOOL selChanged = FALSE;


			if (lpTreeElem)
			{

				m_strKeyword = lpTreeElem->m_strKeyword;

				if (m_strSelectedKeyword.Compare(m_strKeyword) != 0)
				{
					m_strSelectedKeyword = m_strKeyword;
					m_dwSelectedData = lpTreeElem->m_dwData;
					UpdateSelected();
				}
			}
			break;
		}
		}
	}


	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndCraftingMain::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	
	CRect rect;
	if (m_selectedUnit)
	{
		for (int i = 0; i < m_selectedUnit->components.size(); i++)
		{
			rect = m_pWndComponentItem[i]->GetWndRect();

			if (rect.PtInRect(point))
			{
				RemoveComponentItem(i);
				break;
			}
		}
	}

}
BOOL CWndCraftingMain::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	if (m_bCraftingInProgress)
		return FALSE;

	CItemElem* pTempElem;
	pTempElem = (CItemElem*)g_pPlayer->GetItemId(pShortcut->m_dwId);
	int checkSize = 0;

	if (m_selectedUnit)
	{
		for (int i = 0; i < m_selectedUnit->components.size(); i++)
		{
			CRect rect = m_pWndComponentItem[i]->GetWndRect();

			if (rect.PtInRect(point))
			{
				AddComponentItem(pTempElem);
				break;
			}
		}
	}

	if (m_bRecyclingPage) 
	{
		AddComponentItem(pTempElem);
	}
	
	return TRUE;
}

void CWndCraftingMain::SetEmptyRecipePage() 
{
	CString tmpStr;
	tmpStr.Format("Titanium Crafting System");
	m_pWndRecipeName->SetTitle(tmpStr);

	m_pWndDescription->SetTitle("");
	m_pWndResultItemText->SetTitle("");
	m_pWndGauge->SetTitle("");
	

	CRect rect = m_pWndDescription->GetWndRect();
	rect.bottom = GetWndRect().bottom;
	m_descriptionString.Init(GetFont(), &rect);

	tmpStr.Format("%s", prj.GetText(TID_CRAFTING_START_MESSAGE));
	m_descriptionString.SetParsingString(tmpStr);
	m_descriptionString.SetStyle(ESSTY_BOLD);

	m_pWndCraftButton->EnableWindow(FALSE);

}

void CWndCraftingMain::SetSelectedRecipePage()
{
	PrepareRecipeChange();

	CString strTmp;
	strTmp.Format("%s", m_selectedUnit->recipeName);
	m_pWndRecipeName->SetTitle(strTmp);


	strTmp.Format("%s (%dx)", prj.GetItemProp(m_selectedUnit->resultItemID)->szName, m_selectedUnit->resultItemNum);
	m_pWndResultItemText->SetTitle(strTmp);

	m_pResultItemTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, prj.GetItemProp(m_selectedUnit->resultItemID)->szIcon), COLOR_ITEM_BG);

	for (int i = 0; i < AEGCRAFT_MAXCOMPONENTS; i++)
	{
		if (i < m_selectedUnit->components.size())
			m_pComponentTexture[i] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, prj.GetItemProp(m_selectedUnit->components[i].m_itemID)->szIcon), COLOR_ITEM_BG);
	}

	m_descriptionString.Init(GetFont(), &m_pWndDescription->GetWndRect());
	
	strTmp.Format("Description: %s\n", prj.GetText(m_selectedUnit->descriptionTID));
	m_descriptionString.SetParsingString(strTmp);
	m_descriptionString.SetStyle(m_descriptionString.GetLength() - strTmp.GetLength(), strlen("Description:"), ESSTY_UNDERLINE | ESSTY_BOLD);

	strTmp.Format("Required Level: %d\n", m_selectedUnit->levelRequirement);
	m_descriptionString.AddParsingString(strTmp);
	m_descriptionString.SetStyle(m_descriptionString.GetLength() - strTmp.GetLength(), strlen("Required Level:"), ESSTY_UNDERLINE | ESSTY_BOLD);

	strTmp.Format("Exp Reward: %d\n", m_selectedUnit->craftingExp);
	m_descriptionString.AddParsingString(strTmp);
	m_descriptionString.SetStyle(m_descriptionString.GetLength() - strTmp.GetLength(), strlen("Exp Reward:"), ESSTY_UNDERLINE | ESSTY_BOLD);

	strTmp.Format("Crafting Time: %d Sec\n", m_selectedUnit->craftTime);
	m_descriptionString.AddParsingString(strTmp);
	m_descriptionString.SetStyle(m_descriptionString.GetLength() - strTmp.GetLength(), strlen("Crafting Time:"), ESSTY_UNDERLINE | ESSTY_BOLD);

	//m_descriptionString.SetStyle(0, m_descriptionString.GetLength() - 1, ESSTY_BOLD);

	m_pWndCraftButton->EnableWindow(TRUE);
}

void CWndCraftingMain::SetRecyclingPage()
{
	PrepareRecipeChange();
	m_bRecyclingPage = TRUE;

	if (m_pWndRecyclingTitle)
		m_pWndRecyclingTitle->SetTitle("Recycling List");

	CString strTmp;
	strTmp.Format("Recycling");
	m_pWndRecipeName->SetTitle(strTmp);

	SetRecyclingText();

	m_pWndCraftButton->EnableWindow(TRUE);

	LPWNDCTRL recyclingCtrl = GetWndCtrl(WIDC_CUSTOM1);

	if (recyclingCtrl)
	{
		SAFE_DELETE(m_wndRecyclingList);
		m_wndRecyclingList = new CWndRecyclingList;
		m_wndRecyclingList->Create(WBS_CHILD | WBS_VSCROLL, recyclingCtrl->rect, this, 100000);
		m_wndRecyclingList->m_bTile = true;
		m_wndRecyclingList->m_bVisible = true;
		m_wndRecyclingList->m_strTexture = "WndEditTile00.tga";

		AdjustWndBase();
	}
}

void CWndCraftingMain::SetRecyclingText() 
{
	CString strTmp;

	CRect rect = m_pWndDescription->GetWndRect();
	rect.bottom = GetWndRect().bottom;
	m_descriptionString.Init(GetFont(), &rect);

	strTmp.Format("Description: %s\n", prj.GetText(TID_CRAFTING_RECYCLE_DESCR));
	m_descriptionString.SetParsingString(strTmp);
	m_descriptionString.SetStyle(m_descriptionString.GetLength() - strTmp.GetLength(), strlen("Description:"), ESSTY_UNDERLINE | ESSTY_BOLD);

	if (m_selectedUnit) 
	{

		strTmp.Format("%s (%dx)", prj.GetItemProp(m_selectedUnit->resultItemID)->szName, m_selectedUnit->resultItemNum);
		m_pWndResultItemText->SetTitle(strTmp);

		strTmp.Format("Required Level: %d\n", m_selectedUnit->levelRequirement);
		m_descriptionString.AddParsingString(strTmp);
		m_descriptionString.SetStyle(m_descriptionString.GetLength() - strTmp.GetLength(), strlen("Required Level:"), ESSTY_UNDERLINE | ESSTY_BOLD);

		strTmp.Format("Exp Reward: %d\n", m_selectedUnit->craftingExp);
		m_descriptionString.AddParsingString(strTmp);
		m_descriptionString.SetStyle(m_descriptionString.GetLength() - strTmp.GetLength(), strlen("Exp Reward:"), ESSTY_UNDERLINE | ESSTY_BOLD);

		strTmp.Format("Crafting Time: %d Sec\n", m_selectedUnit->craftTime);
		m_descriptionString.AddParsingString(strTmp);
		m_descriptionString.SetStyle(m_descriptionString.GetLength() - strTmp.GetLength(), strlen("Crafting Time:"), ESSTY_UNDERLINE | ESSTY_BOLD);
	}
	else 
	{
		m_pWndResultItemText->SetTitle("");
	}

	

	
}

void CWndCraftingMain::UpdateSelected() 
{
	map<int, __CRAFTING_UNIT>::iterator it = CAegonCraftingMng::GetInstance()->m_mapCraftingUnits.begin();

	if (m_strSelectedKeyword.Compare(AEGCRAFT_RECYCLE_CATEGORY) == 0)
	{
		m_selectedUnit = NULL;
		SetRecyclingPage();
		return;
	}

	for (; it != CAegonCraftingMng::GetInstance()->m_mapCraftingUnits.end(); it++)
	{
		if (m_strSelectedKeyword.Compare(it->second.recipeName) == 0)
		{
			m_selectedUnit = &it->second;
			SetSelectedRecipePage();
			break;
		}
	
	}
}

void CWndCraftingMain::AddComponentItem(CItemElem* pItemElem)
{
	if (!pItemElem || pItemElem->m_nItemNum <= 0)
		return;

	if (m_bRecyclingPage) 
	{
		map<int, __CRAFTING_UNIT>::iterator it = CAegonCraftingMng::GetInstance()->m_mapCraftingUnits.begin();
		for (; it != CAegonCraftingMng::GetInstance()->m_mapCraftingUnits.end(); it++)
		{
			if (it->second.recyclingRecipe && it->second.components[0].m_itemID == pItemElem->GetPropA()->dwID)
			{
				if (m_pComponentItem[0] != NULL)
				{
					RemoveComponentItem(0);
				}
				if (pItemElem->GetExtra() == 0)
				{
					m_selectedUnit = &it->second;

					m_pComponentItem[0] = pItemElem;
					m_pComponentItem[0]->SetExtra(m_pComponentItem[0]->m_nItemNum);
					m_pComponentTexture[0] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemElem->GetPropA()->szIcon), COLOR_ITEM_BG);
					m_pResultItemTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, prj.GetItemProp(it->second.resultItemID)->szIcon), COLOR_ITEM_BG);

					SetRecyclingText();
				}
				return;
			}

		}
		g_WndMng.PutString("This item cannot be recycled.", NULL, COLOR_ERROR);
	}
	else 
	{
		if (!m_selectedUnit)
			return;

		for (int i = 0; i < m_selectedUnit->components.size(); i++)
		{
			if (m_selectedUnit->components[i].m_itemID == pItemElem->GetPropA()->dwID)
			{
				if (m_pComponentItem[i] != NULL)
				{
					RemoveComponentItem(i);
					
				}
				if (pItemElem->GetExtra() == 0)
				{
					m_pComponentItem[i] = pItemElem;
					m_pComponentItem[i]->SetExtra(m_pComponentItem[i]->m_nItemNum);
				}

				return;
			}
		}
		g_WndMng.PutString("Invalid item", NULL, COLOR_ERROR);
	}
	
	
}

void CWndCraftingMain::RemoveComponentItem(int nIndex)
{
	if (nIndex < 0 || nIndex >= AEGCRAFT_MAXCOMPONENTS)
		return;

	if (m_pComponentItem[nIndex]) 
	{
		m_pComponentItem[nIndex]->SetExtra(0);
		m_pComponentItem[nIndex] = NULL;
		if (m_bRecyclingPage) 
		{
			m_pResultItemTexture = NULL;
			m_pWndResultItemText->SetTitle("");
			m_selectedUnit = NULL;
			SetRecyclingText();
		}
	}

}

void CWndCraftingMain::OnCraftingExpRefresh() 
{
	CString strTmp;
	int craftingLevel = CAegonCraftingMng::GetInstance()->GetCraftingLevel(g_pPlayer);
	
	if (craftingLevel == MAXCRAFTINGLEVEL) 
	{
		strTmp.Format("Crafting - Level: %d - MAX Level", craftingLevel);
	}
	else 
	{
		__CRAFTING_LEVEL currentLevel = CAegonCraftingMng::GetInstance()->m_levels[craftingLevel - 1];
		strTmp.Format("Crafting - Level: %d - EXP %d/%d", craftingLevel, g_pPlayer->m_nCraftingExp - currentLevel.m_levelExp, currentLevel.m_levelAdditionalExp);
	}
		
	SetTitle(strTmp);
}

void CWndCraftingMain::OnSuccessFullCraft() 
{
	for (int i = 0; i < AEGCRAFT_MAXCOMPONENTS; i++) 
	{
		if (m_pComponentItem[i]) 
		{

			if (m_pComponentItem[i]->m_dwItemId == 0) 
			{
				RemoveComponentItem(i);
			}
			else if (m_pComponentItem[i]->GetExtra() != m_pComponentItem[i]->m_nItemNum) 
			{
				m_pComponentItem[i]->SetExtra(m_pComponentItem[i]->m_nItemNum);
			}
		}
	}
}

void CWndCraftingMain::CraftingProcess()
{
	int craftCount;
	int maxCraft = GetMaxPossibleCrafts();
	try 
	{
		craftCount = stoi(m_pWndCraftNum->GetString());
	}
	catch (...) 
	{
		craftCount = 1;
		m_pWndCraftNum->SetTitle("1");
	}
	if (craftCount > maxCraft) 
	{
		craftCount = maxCraft;
		CString strTmp;
		strTmp.Format("%d", maxCraft);
		m_pWndCraftNum->SetTitle(strTmp);
	}
		

	
	for (int i = 0; i < craftCount; i++) 
	{
		if (!CraftOnce())
			return;

	}
}
BOOL CWndCraftingMain::CraftOnce()
{
	DWORD stopTime, currentTime;
	stopTime = GetTickCount() + (m_selectedUnit->craftTime * 1000);
	currentTime = 0;
	while (currentTime < stopTime)
	{
		if (m_bAbortCrafting)
			return FALSE;

		currentTime = GetTickCount();

		//TODO set gauge texture
		DWORD timeLeft = stopTime - currentTime;
		DWORD timeElapsed = (m_selectedUnit->craftTime * 1000) - timeLeft;
		double progress = (double)timeElapsed / (double)((double)m_selectedUnit->craftTime * (double)1000);

		double progressPerImage = ((double)100 / (double)(AEGCRAFT_PROGRESSELEMENTS - 1)) / (double)100;
		for (int i = 0; i < AEGCRAFT_PROGRESSELEMENTS; i++) 
		{
			
			if (progress <= progressPerImage * i) 
			{
				m_pActiveProgressTexture = m_pProgressTexture[i];
				break;
			}
		}

	}
	m_nCraftedItems++;
	g_DPlay.SendCraftRecipe(m_selectedUnit, m_pComponentItem);
	return TRUE;
}
void CWndCraftingMain::OnThreadEnd() 
{
	m_bCraftingInProgress = FALSE;

	m_pWndCraftButton->SetVisible(TRUE);
	m_pWndCancelButton->SetVisible(FALSE);

	m_pWndCraftButton->EnableWindow(TRUE);
	m_pWndCancelButton->EnableWindow(FALSE);

	m_pWndCraftNum->EnableWindow(TRUE);
	m_pWndTreeCtrl->EnableWindow(TRUE);

	m_bAbortCrafting = FALSE;
}
int CWndCraftingMain::GetMaxPossibleCrafts() 
{
	if (!m_selectedUnit)
		return 0;

	int out = 0;
	for (int i = 0; i < m_selectedUnit->components.size(); i++) 
	{
		int tmp = 0;
		if (m_pComponentItem[i]) 
		{
			int tmp = floor(m_pComponentItem[i]->GetExtra() / m_selectedUnit->components[i].m_itemNum);
			if (tmp < out || i == 0)
				out = tmp;
		}
	}

	return out;
}

void CWndCraftingMain::UpdateSearch()
{
	//TerminateThread(m_hSearchThread, 0);
	//m_hSearchThread = CreateThread(0, 0, __SearchThread, this, 0, 0);


	m_pWndComboSearch->m_wndListBox.ResetContent();
	CAegonCraftingMng::GetInstance()->FillSearchComboBox(m_pWndComboSearch);


	if (!m_pWndComboSearch->m_bOpen)
		m_pWndComboSearch->OpenListBox();
	
}

void CWndCraftingMain::OpenRecipe(CString recipeString) 
{
	if (recipeString.GetLength() < 1 || recipeString.CompareNoCase("Nothing found") == 0)
		return;

	CString strCategory;
	map<int, __CRAFTING_UNIT>::iterator it = CAegonCraftingMng::GetInstance()->m_mapCraftingUnits.begin();

	for (; it != CAegonCraftingMng::GetInstance()->m_mapCraftingUnits.end(); it++)
	{
		if (recipeString.Compare(it->second.recipeName) == 0) 
		{
			strCategory = it->second.categoryName;
			break;
		}
	}

	LPTREEELEM treeElem = m_pWndTreeCtrl->SetCurSel(strCategory);
	if (treeElem)
	{
		treeElem->m_bOpen = TRUE;
	}

	m_pWndTreeCtrl->SetCurSel(recipeString);
	OnChildNotify(0, WIDC_TREECTRL, (LRESULT*)m_pWndTreeCtrl->GetCurSel());
}

void CWndCraftingMain::PrepareRecipeChange() 
{
	m_bRecyclingPage = FALSE;
	if (m_pWndRecyclingTitle)
		m_pWndRecyclingTitle->SetTitle("");
	RemoveWnd(m_wndRecyclingList);
	SAFE_DELETE(m_wndRecyclingList);
	AdjustWndBase();

	for (int i = 0; i < AEGCRAFT_MAXCOMPONENTS; i++)
	{
		if (m_pComponentItem[i])
		{
			RemoveComponentItem(i);
		}

		m_pComponentTexture[i] = NULL;
	}
	m_pResultItemTexture = NULL;
}

#endif // __AEGON_CRAFTING_SYSTEM

///////////////////////////
//////RECYCLING LIST///////
///////////////////////////
CWndRecyclingList::CWndRecyclingList()
{
	m_nRowHeight = 40;
}
CWndRecyclingList::~CWndRecyclingList()
{
}
void CWndRecyclingList::OnDraw(C2DRender* p2DRender)
{
	CRect wndRect = GetWndRect();
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	CPoint point = CPoint(0, 0);
	int drawOffsetY = m_wndScrollBar.GetScrollPos() * m_nRowHeight;
	CString str;
	
	for (int i = 0; i < m_vRecyclingUnits.size(); i++) 
	{
		if (!m_vRecyclingUnits[i])
			continue;

		point.x = 10;
		point.y = i * m_nRowHeight;

		//str.Format("Recycle %d", i);
		//CSize strExtent = p2DRender->GetFont()->GetTextExtent(str.GetString());
		//p2DRender->TextOut(point.x, point.y - drawOffsetY + (m_nRowHeight / 2) - (strExtent.cy / 2), str.GetString(), dwColorText);
		//point.x += strExtent.cx;

		ItemProp* pRecycleProp = prj.GetItemProp(m_vRecyclingUnits[i]->components[0].m_itemID);
		if (!pRecycleProp)
			return;
		
		CTexture* pRecycleTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pRecycleProp->szIcon), COLOR_ITEM_BG);
		if (pRecycleTexture) 
		{
			p2DRender->RenderTexture(CPoint(point.x, point.y + 4 - drawOffsetY), pRecycleTexture);
			point.x += pRecycleTexture->m_size.cx;
		}
				
		CTexture* pArrow = CAegonThemeSwitcherMng::GetInstance()->ThemeSwitchAddTexture(p2DRender->m_pd3dDevice, "ExchangeArrow.tga", COLOR_ERROR);
		if (pArrow) 
		{
			point.x = (GetWindowRect().Width() - m_wndScrollBar.GetClientRect().Width() - 15) / 2 - pArrow->m_size.cx / 2;
			p2DRender->RenderTexture(CPoint(point.x, point.y + 4 - drawOffsetY), pArrow);
			point.x += pArrow->m_size.cx;
		}


	
		str.Format("(%dx)", m_vRecyclingUnits[i]->components[0].m_itemNum);
		CSize strExtent = p2DRender->GetFont()->GetTextExtent(str.GetString());
		point.x = GetWindowRect().Width() - m_wndScrollBar.GetClientRect().Width() - 15 - strExtent.cx;
		p2DRender->TextOut(point.x, point.y - drawOffsetY + (m_nRowHeight / 2) - (strExtent.cy / 2), str.GetString(), dwColorText);

		ItemProp* pResultProp = prj.GetItemProp(m_vRecyclingUnits[i]->resultItemID);
		if (pResultProp)
		{
			CTexture* pResultTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pResultProp->szIcon), COLOR_ITEM_BG);
			if (pResultTexture)
			{
				point.x -= pResultTexture->m_size.cx;
				p2DRender->RenderTexture(CPoint(point.x, point.y + 4 - drawOffsetY), pResultTexture);
				
			}

		}

		if (point.y + m_nRowHeight - drawOffsetY >= m_nRowHeight)
			p2DRender->RenderLine(CPoint(0, point.y + m_nRowHeight - drawOffsetY), CPoint(wndRect.Width() - 30 , point.y + m_nRowHeight - drawOffsetY), dwColorText);
	}
	
}
void CWndRecyclingList::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	CRect rect = GetWindowRect();
	m_wndScrollBar.AddWndStyle(WBS_DOCKING);
	m_wndScrollBar.Create(WBS_VERT, rect, this, 1000);

	map<int, __CRAFTING_UNIT>::iterator it = CAegonCraftingMng::GetInstance()->m_mapCraftingUnits.begin();
	for (; it != CAegonCraftingMng::GetInstance()->m_mapCraftingUnits.end(); it++) 
	{
		__CRAFTING_UNIT* tmpUnit = &it->second;
		if (tmpUnit && tmpUnit->recyclingRecipe)
		{
			m_vRecyclingUnits.push_back(tmpUnit);
		}
	}

	int nRange = 0;
	int nPage = rect.Height() / 32;

	nRange = m_vRecyclingUnits.size();

	if (!nRange)
		nRange = 1;

	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);
	m_wndScrollBar.EnableScrollBar(TRUE);
	SetScrollBarVisible(TRUE);

}
BOOL CWndRecyclingList::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect(240, 0, 240 + 330, 255 - 135);
	return CWndBase::Create(WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_EXTENSION, rect, pWndParent, dwWndId);
}
BOOL CWndRecyclingList::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndBase::OnChildNotify(message, nID, pLResult);
}
BOOL CWndRecyclingList::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndBase::OnCommand(nID, dwMessage, pWndBase);
}
void CWndRecyclingList::OnSize(UINT nType, int cx, int cy)
{
	CWndBase::OnSize(nType, cx, cy);
}
void CWndRecyclingList::OnLButtonUp(UINT nFlags, CPoint point)
{

}
void CWndRecyclingList::OnLButtonDown(UINT nFlags, CPoint point)
{

}

void CWndRecyclingList::OnMouseWndSurface(CPoint point)
{
	CPoint pointDraw = CPoint(0, 0);
	SIZE sizeTexture;
	sizeTexture.cx = 32;
	sizeTexture.cy = 32;
	int drawOffsetY = m_wndScrollBar.GetScrollPos() * m_nRowHeight;

	for (int i = m_wndScrollBar.GetScrollPos(); i < m_vRecyclingUnits.size(); i++)
	{
		if (!m_vRecyclingUnits[i])
			continue;

		pointDraw.x = 10;
		pointDraw.y = i * m_nRowHeight;


		CRect tmpRect;
		tmpRect.left = pointDraw.x;
		tmpRect.top = pointDraw.y + 4 - drawOffsetY;
		tmpRect.right = tmpRect.left + sizeTexture.cx;
		tmpRect.bottom = tmpRect.top + sizeTexture.cy;

		if (tmpRect.PtInRect(point))
		{

			CRect hitrect = tmpRect;

			CPoint point2 = point;
			ClientToScreen(&point2);
			ClientToScreen(&hitrect);

			CItemElem tmpItem;
			tmpItem.m_dwItemId = m_vRecyclingUnits[i]->components[0].m_itemID;
			tmpItem.m_nItemNum = m_vRecyclingUnits[i]->components[0].m_itemNum;

			g_WndMng.PutToolTip_Item(&tmpItem, point2, &hitrect);
			return;
		}

		CString str;
		str.Format("(%dx)", m_vRecyclingUnits[i]->components[0].m_itemNum);
		CSize strExtent = m_Theme.m_pFontText->GetTextExtent(str.GetString());
		pointDraw.x = GetWindowRect().Width() - m_wndScrollBar.GetClientRect().Width() - 15 - strExtent.cx;
		tmpRect.left = pointDraw.x - sizeTexture.cx;
		tmpRect.right = tmpRect.left + sizeTexture.cx;
		if (tmpRect.PtInRect(point))
		{

			CRect hitrect = tmpRect;

			CPoint point2 = point;
			ClientToScreen(&point2);
			ClientToScreen(&hitrect);

			CItemElem tmpItem;
			tmpItem.m_dwItemId = m_vRecyclingUnits[i]->resultItemID;
			tmpItem.m_nItemNum = m_vRecyclingUnits[i]->resultItemNum;

			g_WndMng.PutToolTip_Item(&tmpItem, point2, &hitrect);
			return;
		}

	}
}
BOOL CWndRecyclingList::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta < 0)
	{
		if (m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos())
			m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetScrollPos() + 1);
		else
			m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage());
	}
	else
	{
		if (m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos())
			m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetScrollPos() - 1);
		else
			m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetMinScrollPos());
	}

	return TRUE;
}
#ifdef __AEGON_THEME_SWITCHER
void CWndRecyclingList::OnThemeSelectionChanged() 
{
	CWndBase::OnThemeSelectionChanged();
	m_wndScrollBar.OnThemeSelectionChanged();
}
#endif