#include "stdafx.h"

#ifdef __WIKIPEDIA_ITEMS
#include "WndWikiItems.h"
#include "resdata.h"
#include <sstream>
#include "DPClient.h"

#ifdef __MODEL_VIEW
#include "WndModelView.h"
#endif //__MODEL_VIEW

extern CDPClient g_DPlay;

DWORD WINAPI __SortThread(LPVOID lpParam)
{
	CWndWikiItems* pWiki = (CWndWikiItems*)lpParam;
	pWiki->Sort();
	return 0;
}

CWndWikiItems::CWndWikiItems()
{
	m_vecItems.clear();
	m_bUpperCase = false;
	m_cSortType = 0;
	m_hSortThread = INVALID_HANDLE_VALUE;
	m_pCreate = NULL;
#ifdef __FL_FARM_STAT
	bWikiSelectMode = FALSE;
#endif //__FL_FARM_STAT
}

CWndWikiItems::~CWndWikiItems()
{
	m_vecItems.clear();
	TerminateThread(m_hSortThread, 0);
	SAFE_DELETE(m_pCreate);
}

BOOL CWndWikiItems::Initialize(CWndBase* pWndParent, DWORD dwStyle)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_WIKI_ITEMS, 0, 0, pWndParent);
}

void CWndWikiItems::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pListItems = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	m_pComboJob = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	m_pEditFilter = (CWndEdit*)GetDlgItem(WIDC_EDIT1);

#ifdef __PLACEHOLDER_TEXT
	m_pEditFilter->SetPlaceholderString("Search...");
#endif //__PLACEHOLDER_TEXT

#ifdef __AEGON_FIXES
	CString tmpStr = "";
#endif
	for (int i = 0; i < MAX_JOB; ++i)
	{
		if (i == 5 || i == 14 || i == 15)
			continue;

#ifdef __AEGON_FIXES
		tmpStr = "";
		if (prj.m_aJob[i].dwJobType == JTYPE_MASTER)
			tmpStr += "Master ";
		else if (prj.m_aJob[i].dwJobType == JTYPE_HERO)
			tmpStr += "Hero ";
		tmpStr += prj.m_aJob[i].szName;
		m_pComboJob->AddString(tmpStr.GetString());
#else
		m_pComboJob->AddString(prj.m_aJob[i].szName);
#endif
	}
	m_pComboJob->SetCurSel(0);
	m_pComboJob->AddWndStyle(EBS_READONLY);

	m_pFilterSex = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);
	m_pFilterSex->SetString("All");
	m_pFilterSex->AddString("All");
	m_pFilterSex->AddString("Male");
	m_pFilterSex->AddString("Female");
	m_pFilterSex->SetCurSel(0);
	m_pFilterSex->AddWndStyle(EBS_READONLY);

	//Level Filters
	m_pFilterLvlLow = (CWndEdit*)GetDlgItem(WIDC_EDIT2);
	m_pFilterLvlHigh = (CWndEdit*)GetDlgItem(WIDC_EDIT3);

	m_pFilterLvlLow->AddWndStyle(EBS_NUMBER);
	m_pFilterLvlLow->SetString("1");

	m_pFilterLvlHigh->AddWndStyle(EBS_NUMBER);
	m_pFilterLvlHigh->SetString("175");

	//Clear Filters
	ResetFilters();

	CWndTreeCtrl* pWndTreeCtrl = (CWndTreeCtrl*)GetDlgItem(WIDC_TREECTRL);
	pWndTreeCtrl->LoadTreeScript(MakePath(DIR_CLIENT, "WikiItems.inc"));

	UpdateList();

	MoveParentCenter();
}
BOOL CWndWikiItems::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_TREECTRL)
	{
		if (message == WNM_DBLCLK)
			return FALSE;
		LPTREEELEM lpTreeElem = (LPTREEELEM)pLResult;
		if (lpTreeElem)
		{
			m_strKeyword = lpTreeElem->m_strKeyword;
			if (Sel == m_strKeyword)
				return FALSE;
			Sel = Select(m_strKeyword);
			UpdateList();
		}
	}
	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case WIDC_BUTTON2:
		{
			TerminateThread(m_hSortThread, 0);
			m_cSortType = 0;
			m_bUpperCase = !m_bUpperCase;
			m_hSortThread = CreateThread(0, 0, __SortThread, this, 0, 0);
		}
		break;
		case WIDC_BUTTON3:
		{
			TerminateThread(m_hSortThread, 0);
			m_cSortType = 1;
			m_bUpperCase = !m_bUpperCase;
			m_hSortThread = CreateThread(0, 0, __SortThread, this, 0, 0);
		}
		break;
		case WIDC_BUTTON4:
		{
			TerminateThread(m_hSortThread, 0);
			m_cSortType = 2;
			m_bUpperCase = !m_bUpperCase;
			m_hSortThread = CreateThread(0, 0, __SortThread, this, 0, 0);
		}
		break;
		case WIDC_BUTTON1:
		{
			ResetFilters(); // Clear Filters
		}
		break;
		}
	}
	else if (message == WNM_SELCHANGE)
	{
		switch (nID)
		{
			case WIDC_COMBOBOX1: // m_pComboJob
			{
				UpdateList();
			}
			break;
			case WIDC_COMBOBOX2: // m_pFilterSex
			{
				UpdateList();
			}
			break;
		}
	}
	else if (message == EN_CHANGE)
	{
		switch (nID)
		{
			case WIDC_EDIT1:
			{
				UpdateList();
			}
			break;
			case WIDC_EDIT2:
			{
				UpdateList();
			}
			break;
			case WIDC_EDIT3:
			{
				UpdateList();
			}
			break;
		}
	}
	else if (message == WNM_DBLCLK)
	{
		if (nID == WIDC_LISTBOX1)
		{
			if (nID == WIDC_LISTBOX1)
			{
#ifdef __FL_FARM_STAT
				if (bWikiSelectMode)
				{
					if (m_pListItems->GetCurSel() != -1)
					{
						CWndFarmCounter* pWndFC = (CWndFarmCounter*)g_WndMng.GetWndBase(APP_FARM_COUNTER);
						{
							if (pWndFC)
							{
								ItemProp* pProp = m_vecItems[m_pListItems->GetCurSel()];
								if (pProp)
								{
									pWndFC->DoSelectItem(pProp);
									//Destroy();
								}
							}
							else
							{
								Destroy();
							}
						}
					}
					return CWndNeuz::OnChildNotify(message, nID, pLResult);
				}
#endif //__FL_FARM_STAT
#ifdef __MODEL_VIEW
				if (m_pListItems->GetCurSel() != -1)
				{
#ifdef __BLOCK_STAFF_ACTIONS
					if (g_pPlayer->m_dwAuthorization >= AUTH_BALANCE && GetAsyncKeyState(VK_SHIFT) & 0x8000)
#else //__BLOCK_STAFF_ACTIONS
					if (g_pPlayer->m_dwAuthorization >= AUTH_ADMINISTRATOR && GetAsyncKeyState(VK_SHIFT) & 0x8000)
#endif //__BLOCK_STAFF_ACTIONS
					{
						SAFE_DELETE(m_pCreate);
						m_pCreate = new CWndCreateItems();
						m_pCreate->Initialize(this);
						m_pCreate->SetItem(m_vecItems[m_pListItems->GetCurSel()]);
					}
#ifdef __AEGON_DB_PETFILTER
					else if (g_WndMng.GetApplet(APP_PETFILTER_MAIN))
					{
						CWndPetfilterMain* pWndPetfilterMain = (CWndPetfilterMain*)g_WndMng.GetApplet(APP_PETFILTER_MAIN);
						if (pWndPetfilterMain) 
						{
							int nCurSel = m_pListItems->GetCurSel();
							if(nCurSel >= 0 && nCurSel < m_vecItems.size())
								pWndPetfilterMain->OnAddItemToFilterList(m_vecItems[nCurSel]->dwID);
						}
					}
#endif
					else
					{
						CItemElem itemElem;
						itemElem.m_dwItemId = m_vecItems[m_pListItems->GetCurSel()]->dwID;
						PPACKITEMELEM pPackItemElem = CPackItem::GetInstance()->Open(itemElem.m_dwItemId);

						if (pPackItemElem)
						{
							for (int i = 0; i < pPackItemElem->nSize; i++)
							{
								CItemElem* itemElem2 = new CItemElem;
								itemElem2->m_dwItemId = pPackItemElem->adwItem[i];
								CWndModelView* pWndViewer = (CWndModelView*)g_WndMng.GetApplet(APP_MODEL_VIEW);
								pWndViewer = (CWndModelView*)g_WndMng.CreateApplet(APP_MODEL_VIEW);
								if (pWndViewer)
								{
									pWndViewer->SetPart(&itemElem);
								}									
							}
						}
						ItemProp *pProp = m_vecItems[m_pListItems->GetCurSel()];
						if (pProp)
						{
							CItemElem itemElem;
							itemElem.m_dwItemId = pProp->dwID;
							CWndModelView* pWndViewer = (CWndModelView*)g_WndMng.GetApplet(APP_MODEL_VIEW);
							if (!pWndViewer)
							{
								pWndViewer = (CWndModelView*)g_WndMng.CreateApplet(APP_MODEL_VIEW);
								if (pWndViewer)
								{
									pWndViewer->AddWndStyle(WBS_TOPMOST);
									if (!pWndViewer->SetPart(&itemElem))
										pWndViewer->Destroy();
								}
							}
							else
								pWndViewer->SetPart(&itemElem);
						}
					}
#ifdef __GIFTBOX_LIST_ITEMS
#ifdef __BLOCK_STAFF_ACTIONS
					if (g_pPlayer->m_dwAuthorization >= AUTH_BALANCE)
#else //__BLOCK_STAFF_ACTIONS
					if (g_pPlayer->m_dwAuthorization >= AUTH_ADMINISTRATOR)
#endif //__BLOCK_STAFF_ACTIONS
					{
						if (m_vecItems[m_pListItems->GetCurSel()]->dwItemKind1 == IK1_SYSTEM && m_vecItems[m_pListItems->GetCurSel()]->dwItemKind2 == IK2_SYSTEM && !GetAsyncKeyState(VK_SHIFT))
							g_DPlay.SendQueryGBPreview(m_vecItems[m_pListItems->GetCurSel()]->dwID);
					}
					else
					{
						if (m_vecItems[m_pListItems->GetCurSel()]->dwItemKind1 == IK1_SYSTEM && m_vecItems[m_pListItems->GetCurSel()]->dwItemKind2 == IK2_SYSTEM)
							g_DPlay.SendQueryGBPreview(m_vecItems[m_pListItems->GetCurSel()]->dwID);
					}
#endif //__GIFTBOX_LIST_ITEMS
				}
#else
				if (g_pPlayer->m_dwAuthorization >= AUTH_ADMINISTRATOR)
				{
					if (m_pListItems->GetCurSel() != -1)
					{
						SAFE_DELETE(m_pCreate);
						m_pCreate = new CWndCreateItems();
						m_pCreate->Initialize(this);
						m_pCreate->SetItem(m_vecItems[m_pListItems->GetCurSel()]);
					}
				}
#endif
			}
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#ifdef __AEGON_FULLSCREEN_AND_RESIZE
void CWndWikiItems::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (m_pCreate == pWndChild)
	{
		SAFE_DELETE(m_pCreate);
		pWndChild = NULL;
	}
}
#endif
void CWndWikiItems::UpdateList()
{
	const static DWORD dwItems[] = 
	{ 
		II_SYS_SYS_SCR_PERIN, II_GOLD_SEED1, II_GOLD_SEED2, II_GOLD_SEED3, II_GOLD_SEED4, II_GOLD_REWARD, 
		II_WEA_HAN_HAND, II_RID_RID_BOR_LADOLF, II_RID_RID_BOR_JLADOLF, II_RID_RID_BOR_JLADOLF_S, II_RID_RID_BOR_LADOLF_S,
		II_TESTITEM1, II_TESTITEM2,	II_TESTITEM3, II_TESTITEM4, II_TESTITEM5
	};

	CString strFilter = m_pEditFilter->GetString();

	m_vecItems.clear();
	m_pListItems->ResetContent();
	for (int i = 0; i < prj.m_aPropItem.GetSize(); ++i)
	{
		ItemProp* pProp = prj.m_aPropItem.GetAt(i);
		if (pProp)
		{
#pragma region TreeCtrl
			if (m_strKeyword != "" && m_strKeyword != "All Items")
			{
#pragma region Wea
				if (m_strKeyword == "Weapons")
				{
					if (pProp->dwItemKind1 != IK1_WEAPON)
						continue;
				}
				else if (m_strKeyword == "1H Sword")
				{
					if (pProp->dwItemKind3 != IK3_SWD || pProp->dwHanded != HD_ONE)
						continue;
				}
				else if (m_strKeyword == "2H Sword")
				{
					if (pProp->dwItemKind3 != IK3_SWD || pProp->dwHanded != HD_TWO)
						continue;
				}
				else if (m_strKeyword == "1H Axe")
				{
					if (pProp->dwItemKind3 != IK3_AXE)
						continue;
					if (pProp->dwHanded != HD_ONE)
						continue;
				}
				else if (m_strKeyword == "2H Axe")
				{
					if (pProp->dwItemKind3 != IK3_AXE)
						continue;
					if (pProp->dwHanded != HD_TWO)
						continue;
				}
				else if (m_strKeyword == "Knuckle")
				{
					if (pProp->dwItemKind3 != IK3_KNUCKLEHAMMER)
						continue;
				}
				else if (m_strKeyword == "Stick")
				{
					if (pProp->dwItemKind3 != IK3_CHEERSTICK)
						continue;
				}
				else if (m_strKeyword == "Bow")
				{
					if (pProp->dwItemKind3 != IK3_BOW && pProp->dwItemKind3 != IK3_CROSSBOW)
						continue;
				}
				else if (m_strKeyword == "Yo-Yo")
				{
					if (pProp->dwItemKind3 != IK3_YOYO)
						continue;
				}
				else if (m_strKeyword == "Wand")
				{
					if (pProp->dwItemKind3 != IK3_WAND)
						continue;
				}
				else if (m_strKeyword == "Staff")
				{
					if (pProp->dwItemKind3 != IK3_STAFF)
						continue;
				}
#pragma endregion
#pragma region Arm
				else if (m_strKeyword == "Sets")
				{
					if ((pProp->dwItemKind1 != IK1_ARMOR || (pProp->dwItemKind2 != IK2_ARMORETC && pProp->dwItemKind2 != IK2_ARMOR)) &&
						(pProp->dwItemKind3 != IK3_HELMET || pProp->dwItemKind3 != IK3_SUIT || pProp->dwItemKind3 != IK3_GAUNTLET ||
							pProp->dwItemKind3 != IK3_BOOTS || pProp->dwItemKind3 != IK3_SHIELD || pProp->dwItemKind3 != IK3_MAGICBARUNA))
						continue;
				}
				else if (m_strKeyword == "Helmet")
				{
					if (pProp->dwItemKind3 != IK3_HELMET)
						continue;
				}
				else if (m_strKeyword == "Suit")
				{
					if (pProp->dwItemKind3 != IK3_SUIT)
						continue;
				}
				else if (m_strKeyword == "Gauntlet")
				{
					if (pProp->dwItemKind3 != IK3_GAUNTLET)
						continue;
				}
				else if (m_strKeyword == "Boots")
				{
					if (pProp->dwItemKind3 != IK3_BOOTS)
						continue;
				}
				else if (m_strKeyword == "Shield")
				{
					if (pProp->dwItemKind3 != IK3_SHIELD && pProp->dwItemKind3 != IK3_MAGICBARUNA)
						continue;
				}
#pragma endregion
#pragma region Jew
				else if (m_strKeyword == "Jewelry")
				{
					if (pProp->dwItemKind2 != IK2_JEWELRY)
						continue;
				}
				else if (m_strKeyword == "Ring")
				{
					if (pProp->dwItemKind3 != IK3_RING)
						continue;
				}
				else if (m_strKeyword == "Earring")
				{
					if (pProp->dwItemKind3 != IK3_EARRING)
						continue;
				}
				else if (m_strKeyword == "Necklace")
				{
					if (pProp->dwItemKind3 != IK3_NECKLACE)
						continue;
				}
#pragma endregion
#pragma region Shop
				else if (m_strKeyword == "CS Shop")
				{
					if ((pProp->dwItemKind2 != IK2_CLOTH && pProp->dwItemKind2 != IK2_CLOTHETC) &&
						(pProp->dwItemKind3 != IK3_HAT || pProp->dwItemKind3 != IK3_CLOTH || pProp->dwItemKind3 != IK3_GLOVE 
						|| pProp->dwItemKind3 != IK3_SHOES || pProp->dwItemKind3 != IK3_CLOAK 
						|| pProp->dwItemKind3 != IK3_MASK || pProp->dwItemKind3 != IK3_WING
					  /*|| pProp->dwItemKind3 != IK3_ANI_WING*/
							))
						continue;
				}
				else if (m_strKeyword == "Hat")
				{
					if (pProp->dwItemKind3 != IK3_HAT && pProp->dwParts != PARTS_HAT)
						continue;
				}
				else if (m_strKeyword == "Body")
				{
					if (pProp->dwItemKind3 != IK3_CLOTH && pProp->dwParts != PARTS_CLOTH)
						continue;
				}
				else if (m_strKeyword == "Gloves")
				{
					if (pProp->dwItemKind3 != IK3_GLOVE && pProp->dwParts != PARTS_GLOVE)
						continue;
				}
				else if (m_strKeyword == "Shoes")
				{
					if (pProp->dwItemKind3 != IK3_SHOES && pProp->dwParts != PARTS_BOOTS)
						continue;
				}
				else if (m_strKeyword == "Cloaks")
				{
					if (pProp->dwItemKind3 != IK3_CLOAK && pProp->dwParts != PARTS_CLOAK)
						continue;
				}
				else if (m_strKeyword == "Masks")
				{
					if (pProp->dwItemKind3 != IK3_MASK && pProp->dwParts != PARTS_MASK)
						continue;
				}
#ifdef __ANI_WING
				else if (m_strKeyword == "Wings")
				{
					if (pProp->dwItemKind3 != IK3_WING || pProp->dwItemKind1 == IK1_RIDE)
						continue;
				}
#endif //__ANI_WING
#pragma endregion
				else if (m_strKeyword == "Other CS")
				{
					if (pProp->dwItemKind1 != IK1_RIDE 
#ifdef __MOUNT
						&& pProp->dwItemKind3 != IK3_MOUNT 
#endif //__MOUNT
#ifdef __BADGE
						&& pProp->dwItemKind3 != IK3_BADGE 
#endif //__BADGE
#ifdef __ANARCHY_SYSTEM
						&& pProp->dwItemKind3 != IK3_ANARCHY
#endif //__ANARCHY_SYSTEM
						)
					continue;
				}
#pragma region Ride
				else if (m_strKeyword == "Ride")
				{
					if (pProp->dwItemKind1 != IK1_RIDE)
						continue;
				}
#pragma endregion
#ifdef __MOUNT
				else if (m_strKeyword == "Mounts")
				{
					if (pProp->dwItemKind3 != IK3_MOUNT)
						continue;
				}
#endif //__MOUNT
#ifdef __BADGE
				else if (m_strKeyword == "Badges")
				{
				if (pProp->dwItemKind3 != IK3_BADGE)
					continue;
				}
#endif //__BADGE
#ifdef __ANARCHY_SYSTEM
				else if (m_strKeyword == "Anarchy")
				{
				if (pProp->dwItemKind3 != IK3_ANARCHY)
					continue;
				}
#endif //__ANARCHY_SYSTEM
#ifdef __LNB_ANIMATED_HATS
				else if (m_strKeyword == "Animated Hats")
				{
				if (pProp->dwItemKind3 != IK3_ANIHAT)
					continue;
				}
#endif //__LNB_ANIMATED_HATS
#ifdef __ANI_WING
				else if (m_strKeyword == "Animated Wings")
				{
				if (pProp->dwItemKind3 != IK3_ANI_WING)
					continue;
				}
#endif //__ANI_WING
#ifdef __AEGON_IMPROVED_SHOPDESIGN
				else if (m_strKeyword == "Shop Design")
				{
					if (pProp->dwItemKind3 != IK3_SHOPDESIGN)
						continue;
				}
#endif
				else if (m_strKeyword == "Titles")
				{
				if (pProp->dwItemKind3 != IK3_HONOR)
					continue;
				}
#pragma endregion
#pragma region Pets
				else if (m_strKeyword == "Pets")
				{
					if (pProp->dwItemKind3 != IK3_PET && pProp->dwItemKind3 != IK3_EGG)
						continue;
				}
				else if (m_strKeyword == "Stat Pets")
				{
					if (pProp->dwItemKind3 != IK3_EGG)
						continue;
				}
				else if (m_strKeyword == "CS Pets")
				{
					if (pProp->dwItemKind3 != IK3_PET || pProp->dwReferStat1 != NULL_ID)
						continue;
				}
				else if (m_strKeyword == "Buff Pets")
				{
					if (pProp->dwItemKind3 != IK3_PET || pProp->dwReferStat1 != PET_VIS)
						continue;
				}
#pragma endregion
#pragma region Ctrl
				else if (m_strKeyword == "Furniture")
				{
					if (pProp->dwItemKind1 != IK1_HOUSING)
						continue;
				}
				else if (m_strKeyword == "Private")
				{
					if (pProp->dwItemKind2 != IK2_FURNITURE && pProp->dwItemKind2 != IK2_PAPERING)
						continue;
				}
				else if (m_strKeyword == "Guild")
				{
					if (pProp->dwItemKind2 != IK2_GUILDHOUSE_FURNITURE && pProp->dwItemKind2 != IK2_GUILDHOUSE_PAPERING)
						continue;
				}
#pragma endregion
#pragma region Others
				else if (m_strKeyword == "Other")
				{
					if (pProp->dwID == II_GEN_MAT_ELE_TOUCH || pProp->dwID == II_GEN_MAT_ELE_LAKE || pProp->dwID == II_GEN_MAT_ELE_VOLTAGE ||
						pProp->dwID == II_GEN_MAT_ELE_STONE || pProp->dwID == II_GEN_MAT_ELE_GALE)
						continue;
					if (pProp->dwItemKind3 != IK3_SOCKETCARD && pProp->dwItemKind3 != IK3_SOCKETCARD2 && pProp->dwItemKind3 != IK3_ELECARD &&
						pProp->dwItemKind3 != IK3_VIS && pProp->dwItemKind2 != IK2_GEM && pProp->dwItemKind3 != IK3_TICKET)
						continue;
				}
				else if (m_strKeyword == "Cards")
				{
					if (pProp->dwID == II_GEN_MAT_ELE_TOUCH || pProp->dwID == II_GEN_MAT_ELE_LAKE || pProp->dwID == II_GEN_MAT_ELE_VOLTAGE ||
						pProp->dwID == II_GEN_MAT_ELE_STONE || pProp->dwID == II_GEN_MAT_ELE_GALE)
						continue;
					if (pProp->dwItemKind3 != IK3_SOCKETCARD && pProp->dwItemKind3 != IK3_SOCKETCARD2 && pProp->dwItemKind3 != IK3_ELECARD)
						continue;
				}
				else if (m_strKeyword == "Beads")
				{
					if (pProp->dwItemKind3 != IK3_VIS)
						continue;
				}
				else if (m_strKeyword == "Gems")
				{
					if (pProp->dwItemKind2 != IK2_GEM)
						continue;
				}
				else if (m_strKeyword == "Tickets")
				{
					if (pProp->dwItemKind3 != IK3_TICKET)
						continue;
				}
				else if (m_strKeyword == "Power-Up")
				{
					if (pProp->dwItemKind3 != IK3_SCROLL)
					continue;

					if (pProp->dwItemKind2 == IK2_SYSTEM)
					continue;
				}
				else if (m_strKeyword.Compare("Food") == 0) 
				{
					if (pProp->dwItemKind2 != IK2_FOOD)
						continue;

					if (pProp->dwItemKind3 == IK3_BADGE)
						continue;
				}
#pragma endregion
			}
#pragma endregion
			bool bInArray = false;
			
			if (!g_pPlayer->IsAuthHigher(AUTH_OPERATOR))
			{
				for (int j = 0; j < sizeof(dwItems) / sizeof(dwItems[0]); ++j)
				{
					if (pProp->dwID == dwItems[j])
					{
						bInArray = true;
						break;
					}
				}


				if (strstr(pProp->szName, "[STAFF]"))
					continue;

				if (easy_find(prj.vWikiBlacklist, pProp->dwID))
					continue;
			}
			
			if (strstr(pProp->szName, "?"))
				continue;

			if (pProp->dwLimitLevel1 > MAX_3RD_LEGEND_LEVEL && pProp->dwLimitLevel1 != NULL_ID)
				continue;

			if (strstr(pProp->szName, "IDS_PROPITEM_TXT_"))
				continue;

			if (!strlen(pProp->szName))
				continue;

			if (!strlen(pProp->szIcon))
				continue;

#ifndef __NEW_ITEM_VARUNA
			if (pProp->dwItemKind3 == IK3_MAGICBARUNA
				|| pProp->dwItemKind3 == IK3_ZEMBARUNA
				|| pProp->dwItemKind3 == IK3_SHILDBARUNA
				|| pProp->dwItemKind3 == IK3_BARUNA
				|| pProp->dwItemKind3 == IK3_BARUNA_PROTECTION
				|| pProp->dwItemKind3 == IK3_BARUNA_PROTECTION_KEEP
				|| pProp->dwItemKind3 == IK3_BARUNAWEA_PIERCE_RUNE
				|| pProp->dwItemKind3 == IK3_BARUNAAMO_PIERCE_RUNE
				|| pProp->dwItemKind3 == IK3_BARUNA_PROTECTION_RUNE
				|| pProp->dwItemKind3 == IK3_BARUNA_PIERCE_RATE
				|| pProp->dwItemKind3 == IK3_BARUNA_RANDOMOPTION_INIT
				|| pProp->dwItemKind3 == IK3_BARUNA_ELE_PROPTECTION
				|| pProp->dwItemKind3 == IK3_BARUNA_ELE_PROPTECTION_KEEP
				|| pProp->dwItemKind3 == IK3_BARUNA_ELE_PROP_INCREASE
				|| pProp->dwItemKind3 == IK3_BARUNA_ENCHANT_RATE
				|| pProp->dwItemKind3 == IK3_BARUNA_GEN_PIC_MAT_CANCEL
				)
				continue;

			if (strstr(pProp->szName, "Baruna"))
				continue;
#endif //__NEW_ITEM_VARUNA

			if (bInArray)
				continue;

			if (m_pComboJob->GetCurSel() != 0)
			{
				DWORD dwJob = JOB_VAGRANT;
				switch (m_pComboJob->GetCurSel())
				{
				case 1: dwJob = JOB_MERCENARY; break;
				case 2: dwJob = JOB_ACROBAT; break;
				case 3: dwJob = JOB_ASSIST; break;
				case 4: dwJob = JOB_MAGICIAN; break;
				case 5: dwJob = JOB_KNIGHT; break;
				case 6: dwJob = JOB_BLADE; break;
				case 7: dwJob = JOB_JESTER; break;
				case 8: dwJob = JOB_RANGER; break;
				case 9: dwJob = JOB_RINGMASTER; break;
				case 10: dwJob = JOB_BILLPOSTER; break;
				case 11: dwJob = JOB_PSYCHIKEEPER; break;
				case 12: dwJob = JOB_ELEMENTOR; break;
				case 13: dwJob = JOB_KNIGHT_MASTER; break;
				case 14: dwJob = JOB_BLADE_MASTER; break;
				case 15: dwJob = JOB_JESTER_MASTER; break;
				case 16: dwJob = JOB_RANGER_MASTER; break;
				case 17: dwJob = JOB_RINGMASTER_MASTER; break;
				case 18: dwJob = JOB_BILLPOSTER_MASTER; break;
				case 19: dwJob = JOB_PSYCHIKEEPER_MASTER; break;
				case 20: dwJob = JOB_ELEMENTOR_MASTER; break;
				case 21: dwJob = JOB_KNIGHT_HERO; break;
				case 22: dwJob = JOB_BLADE_HERO; break;
				case 23: dwJob = JOB_JESTER_HERO; break;
				case 24: dwJob = JOB_RANGER_HERO; break;
				case 25: dwJob = JOB_RINGMASTER_HERO; break;
				case 26: dwJob = JOB_BILLPOSTER_HERO; break;
				case 27: dwJob = JOB_PSYCHIKEEPER_HERO; break;
				case 28: dwJob = JOB_ELEMENTOR_HERO; break;
				case 29: dwJob = JOB_LORDTEMPLER_HERO; break;
				case 30: dwJob = JOB_STORMBLADE_HERO; break;
				case 31: dwJob = JOB_WINDLURKER_HERO; break;
				case 32: dwJob = JOB_CRACKSHOOTER_HERO; break;
				case 33: dwJob = JOB_FLORIST_HERO; break;
				case 34: dwJob = JOB_FORCEMASTER_HERO; break;
				case 35: dwJob = JOB_MENTALIST_HERO; break;
				case 36: dwJob = JOB_ELEMENTORLORD_HERO; break;
				default: dwJob = JOB_VAGRANT; break;
				}
				if (pProp->dwItemJob != dwJob)
					continue;
			}
			
			//sex filter
			if (m_pFilterSex)
			{
				CString strSexFilter;
				strSexFilter.Format("%s", m_pFilterSex->GetString());
				if (pProp->dwItemSex == SEX_SEXLESS || strSexFilter.CompareNoCase("All") == 0)
				{
				}
				else if (pProp->dwItemSex == SEX_MALE && strSexFilter.CompareNoCase("Female") == 0)
				{
					continue;
				}
				else if (pProp->dwItemSex == SEX_FEMALE && strSexFilter.CompareNoCase("Male") == 0)
				{
					continue;
				}
			}
			//~sex filter

			//level filter
			int nLevelFilterLow = 0;
			int nLevelFilterHigh = 175;

			std::stringstream strToIntLevelLow(m_pFilterLvlLow->GetString());
			strToIntLevelLow >> nLevelFilterLow;

			if (!strToIntLevelLow)
			{
				nLevelFilterLow = 0;
			}

			std::stringstream strToIntLevelHigh(m_pFilterLvlHigh->GetString());
			strToIntLevelHigh >> nLevelFilterHigh;

			if (!strToIntLevelHigh)
			{
				nLevelFilterHigh = 175;
			}

			if (pProp->dwLimitLevel1 == 0xFFFFFFFF)
			{
			}
			else if (pProp->dwLimitLevel1 < nLevelFilterLow || pProp->dwLimitLevel1 > nLevelFilterHigh)
			{
				continue;
			}
			//~level filter

			if (strFilter.GetLength() > 0)
			{
				CString strName = pProp->szName;
				if (!strstr(strName.MakeLower(), strFilter.MakeLower()))
					continue;
			}
			m_vecItems.push_back(pProp);
			m_pListItems->AddString("");
		}
	}
	//TerminateThread(m_hSortThread, 0);
	//m_cSortType = 2;
	//m_bUpperCase = TRUE;
	//m_hSortThread = CreateThread(0, 0, __SortThread, this, 0, 0);
}

void CWndWikiItems::ResetFilters()
{
	m_pEditFilter->SetString("");
	m_pEditFilter->SetFocus();

	m_pFilterLvlLow->SetString("1");
	m_pFilterLvlHigh->SetString("175");
	m_pFilterSex->SetCurSel(0);
	m_pComboJob->SetCurSel(0);
		
	UpdateList();
}

bool SortByNameUpper(const ItemProp* pPropLeft, const ItemProp* pPropRight)
{
	std::string strLeft = pPropLeft->szName;
	std::string strRight = pPropRight->szName;
	return (strLeft < strRight);
}
bool SortByNameLower(const ItemProp* pPropLeft, const ItemProp* pPropRight)
{
	std::string strLeft = pPropLeft->szName;
	std::string strRight = pPropRight->szName;
	return (strLeft > strRight);
}
bool SortByLevelUpper(const ItemProp* pPropLeft, const ItemProp* pPropRight)
{
	int nLevelLeft = (pPropLeft->dwLimitLevel1 == NULL_ID ? 1 : pPropLeft->dwLimitLevel1);
	int nLevelRight = (pPropRight->dwLimitLevel1 == NULL_ID ? 1 : pPropRight->dwLimitLevel1);
	return (nLevelLeft < nLevelRight);
}
bool SortByLevelLower(const ItemProp* pPropLeft, const ItemProp* pPropRight)
{
	int nLevelLeft = (pPropLeft->dwLimitLevel1 == NULL_ID ? 1 : pPropLeft->dwLimitLevel1);
	int nLevelRight = (pPropRight->dwLimitLevel1 == NULL_ID ? 1 : pPropRight->dwLimitLevel1);
	return (nLevelLeft > nLevelRight);
}
bool SortByJobUpper(const ItemProp* pPropLeft, const ItemProp* pPropRight)
{
	return (pPropLeft->dwItemJob < pPropRight->dwItemJob);
}
bool SortByJobLower(const ItemProp* pPropLeft, const ItemProp* pPropRight)
{
	return (pPropLeft->dwItemJob > pPropRight->dwItemJob);
}

void CWndWikiItems::SortByName()
{
	if (m_bUpperCase)
		std::sort(m_vecItems.begin(), m_vecItems.end(), SortByNameUpper);
	else
		std::sort(m_vecItems.begin(), m_vecItems.end(), SortByNameLower);
}

void CWndWikiItems::SortByLevel()
{
	if (m_bUpperCase)
		std::sort(m_vecItems.begin(), m_vecItems.end(), SortByLevelUpper);
	else
		std::sort(m_vecItems.begin(), m_vecItems.end(), SortByLevelLower);
}

void CWndWikiItems::SortByJob()
{
	if (m_bUpperCase)
		std::sort(m_vecItems.begin(), m_vecItems.end(), SortByJobUpper);
	else
		std::sort(m_vecItems.begin(), m_vecItems.end(), SortByJobLower);
}

void CWndWikiItems::Sort()
{
	m_pListItems->EnableWindow(FALSE);
	switch (m_cSortType)
	{
		case 0: SortByName(); break;
		case 1: SortByJob(); break;
		case 2: SortByLevel(); break;
	}
	m_pListItems->EnableWindow(TRUE);
}
CWndCreateItems::CWndCreateItems()
{
	m_pPropCreate = NULL;
}
BOOL CWndCreateItems::Initialize(CWndBase* pWndParent, DWORD dwStyle)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_WIKI_ITEMS_CRE, 0, 0, pWndParent);
}
void CWndCreateItems::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	RestoreDeviceObjects();
	MoveParentCenter();

	m_pStcName = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	m_pDrawCtrl = GetWndCtrl(WIDC_STATIC4);
	m_pEditAmount = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	m_pEditAmount->AddWndStyle(EBS_NUMBER);
	m_pEditAmount->SetString("1");
	m_pEditName = (CWndEdit*)GetDlgItem(WIDC_EDIT2);

	if (g_pPlayer->m_dwAuthorization < AUTH_ADMINISTRATOR)
	{
		m_pEditName->AddWndStyle(EBS_READONLY);
		m_pEditName->SetPlaceholderString("Admin Only.");
	}

	pComboBox[0] = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX);	// Upgrade
	pComboBox[1] = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);	// Element
	pComboBox[2] = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);	// Piercing
	pComboBox[3] = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX3);	// Element Upgrade

	for (int i = 0; i < sizeof(pComboBox) / sizeof(pComboBox[0]); i++)
		pComboBox[i]->ResetContent();

	pComboBox[1]->AddString("Flame");
	pComboBox[1]->AddString("Water");
	pComboBox[1]->AddString("Lightning");
	pComboBox[1]->AddString("Wind");
	pComboBox[1]->AddString("Earth");

	CString str;
	for (int i = 0; i <= 20; i++)
	{
		str.Format("+%d", i);
		pComboBox[0]->AddString(str);
		pComboBox[3]->AddString(str);
		if (i <= 10)
		{
			str.Format("(0/%d)", i);
			pComboBox[2]->AddString(str);
		}
	}
	for (int i = 0; i < sizeof(pComboBox) / sizeof(pComboBox[0]); i++)
		pComboBox[i]->AddWndStyle(EBS_READONLY);

	pComboBox[0]->SetString("- None -", COLOR_PLACEHOLDER);	// Upgrade
	pComboBox[1]->SetString("- None -", COLOR_PLACEHOLDER);	// Element
	pComboBox[2]->SetString("- None -", COLOR_PLACEHOLDER);	// Piercing
	pComboBox[3]->SetString("- None -", COLOR_PLACEHOLDER);	// Element Upgrade

}
void CWndCreateItems::OnDraw(C2DRender* p2DRender)
{
	CWndNeuz::OnDraw(p2DRender);
	if (m_pPropCreate)
	{
		CTexture* pTex = CWndBase::m_textureMng.AddTexture(p2DRender->m_pd3dDevice, MakePath(DIR_ITEM, m_pPropCreate->szIcon), COLOR_ITEM_BG);
		if (pTex)
			pTex->Render(p2DRender, CPoint(m_pDrawCtrl->rect.left + 6, m_pDrawCtrl->rect.top + 6));
	}
}
BOOL CWndCreateItems::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	CWndButton* pButton2 = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
	switch (nID)
	{
	case WIDC_BUTTON1:
		for (int i = 0; i < sizeof(pComboBox) / sizeof(pComboBox[0]); i++)
			m_Select[i] = pComboBox[i]->GetCurSel();
		if (m_pPropCreate)
		{
			int nAmount = atoi(m_pEditAmount->GetString());
			if (nAmount > 9999)
				nAmount = 9999;

			CItemElem itemElem;
			itemElem.m_dwItemId = m_pPropCreate->dwID;
			itemElem.m_nItemNum = (short)nAmount;
			itemElem.SetSerialNumber();
			if (m_Select[0] > -1)
			{
				itemElem.SetAbilityOption(m_Select[0]);
				if (m_pPropCreate->dwReferStat1 == WEAPON_ULTIMATE)
				{
					if (m_Select[0] > 5 && m_Select[0] <= 10)
						itemElem.SetUltimatePiercingSize(m_Select[0] - 5);
				}
			}
			if (m_Select[2] > -1)
				itemElem.SetPiercingSize(m_Select[2]);
			if (m_Select[1] > -1)
				itemElem.m_bItemResist = m_Select[1] + 1;
			if (m_Select[3] > -1)
				itemElem.m_nResistAbilityOption = m_Select[3];
			g_DPlay.SendWikiCreateItem(&itemElem, m_pEditName->GetString());
			for (int i = 0; i < sizeof(pComboBox) / sizeof(pComboBox[0]); i++)
				pComboBox[i]->ResetContent();
			Destroy();
		}
		break;
	case WIDC_BUTTON2: Destroy(); break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndCreateItems::OnMouseWndSurface(CPoint point)
{
	if (m_pDrawCtrl->rect.PtInRect(point))
	{
		if (m_pPropCreate)
		{
			CItemElem itemElem;
			CRect rect = m_pDrawCtrl->rect;
			itemElem.m_dwItemId = m_pPropCreate->dwID;
			ClientToScreen(&point);
			ClientToScreen(&rect);
			g_WndMng.PutToolTip_Item(&itemElem, point, &rect);
		}
	}
}
void CWndCreateItems::SetItem(ItemProp* pPropCreate)
{
	m_pPropCreate = pPropCreate;
	if (m_pPropCreate)
		m_pStcName->SetTitle(m_pPropCreate->szName);
	else
		m_pStcName->SetTitle("");
}
#endif