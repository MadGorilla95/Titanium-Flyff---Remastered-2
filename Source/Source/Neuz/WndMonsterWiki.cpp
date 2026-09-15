#include "stdafx.h"
#include "WndMonsterWiki.h"
#include "ResData.h"
#include "defineText.h"
#include "DPClient.h"
extern CDPClient g_DPlay;

#ifdef __WIKIPEDIA_MONSTER
CWndWikipediaMonster::CWndWikipediaMonster(void)
{
	m_vecMover.clear();
	m_pCreate = NULL;

	m_pListbox = NULL;
	m_pEdit = NULL;

#ifdef __WIKIPEDIA_MONSTER_DROPLIST
	m_pWndDropInfo = NULL;
#endif //__WIKIPEDIA_MONSTER_DROPLIST
}

CWndWikipediaMonster::~CWndWikipediaMonster(void)
{
	m_vecMover.clear();
	SAFE_DELETE(m_pCreate);
#ifdef __WIKIPEDIA_MONSTER_DROPLIST
	SAFE_DELETE(m_pWndDropInfo);
#endif //__WIKIPEDIA_MONSTER_DROPLIST
}

BOOL CWndWikipediaMonster::Initialize(CWndBase* pWndParent, DWORD dwType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_WIKIPEDIA_MONSTER, 0, CPoint(0, 0), pWndParent);
}

void CWndWikipediaMonster::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();

	m_pListbox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	m_pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT);

#ifdef __PLACEHOLDER_TEXT
	m_pEdit->SetPlaceholderString("Search...");
#endif //__PLACEHOLDER_TEXT

	UpdateList();
	MoveParentCenter();
}

void CWndWikipediaMonster::OnDraw(C2DRender* p2DRender)
{
	CWndNeuz::OnDraw(p2DRender);
}

void CWndWikipediaMonster::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndWikipediaMonster::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

BOOL CWndWikipediaMonster::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_EDIT && message == EN_CHANGE)
	{
		UpdateList();
	}
#ifdef __WIKIPEDIA_MONSTER_DROPLIST
	else if (nID == WIDC_LISTBOX1)
	{
		if (message == WNM_DBLCLK)
		{
			if (g_pPlayer->m_dwAuthorization >= AUTH_GAMEMASTER3 && GetAsyncKeyState(VK_SHIFT) & 0x8000)
			{
				SAFE_DELETE(m_pCreate);
				m_pCreate = new CWndWikipediaMonsterCreate();
				m_pCreate->Initialize(this);
				m_pCreate->SetItem(m_vecMover[m_pListbox->GetCurSel()]);
			}
			else
			{
				DWORD pMoverSelect = m_vecMover[m_pListbox->GetCurSel()]->dwID;
				CRect tmpRect(0, 0, 0, 0);
				BOOL bSetWndRect = FALSE;

				if (m_pWndDropInfo) 
				{
					tmpRect = m_pWndDropInfo->GetWndRect();
					bSetWndRect = TRUE;
				}
				


				SAFE_DELETE(m_pWndDropInfo);
				m_pWndDropInfo = new CWndDropInfo();
				m_pWndDropInfo->SetMonsterID(pMoverSelect);
				m_pWndDropInfo->Initialize(this);
				
				if(bSetWndRect)
					m_pWndDropInfo->SetWndRect(tmpRect);

			}
		}
	}
#else //__WIKIPEDIA_MONSTER_DROPLIST
	else if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER3))
	{
		SAFE_DELETE(m_pCreate);
		m_pCreate = new CWndWikipediaMonsterCreate();
		m_pCreate->Initialize(this);
		m_pCreate->SetItem(m_vecMover[m_pListbox->GetCurSel()]);
	}
#endif //__WIKIPEDIA_MONSTER_DROPLIST
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndWikipediaMonster::UpdateList()
{
	if (m_pListbox)
	{
		m_pListbox->ResetContent();
		m_vecMover.clear();

		for (int i = 0; i < prj.m_nMoverPropSize; i++)
		{
			MoverProp* pMoverProp = prj.m_pPropMover + i;
			if (pMoverProp)
			{
				if (pMoverProp->dwAI != AII_MONSTER
					&& pMoverProp->dwAI != AII_AGGRO_NORMAL
					&& pMoverProp->dwAI != AII_PARTY_AGGRO_SUB
					&& pMoverProp->dwAI != AII_PARTY_AGGRO_LEADER
					&& pMoverProp->dwAI != AII_ARENA_REAPER
					&& pMoverProp->dwAI != AII_CLOCKWORKS
					&& pMoverProp->dwAI != AII_BIGMUSCLE
					&& pMoverProp->dwAI != AII_KRRR
					&& pMoverProp->dwAI != AII_BEAR
					&& pMoverProp->dwAI != AII_METEONYKER
					)
					continue;

				if (easy_find(prj.vMonsterBlacklist, pMoverProp->dwID))
					continue;

				if (strstr(pMoverProp->szName, "?"))
					continue;

				if (!strlen(pMoverProp->szName))
					continue;

				if (CString(m_pEdit->GetString()).GetLength())
				{
					if (!strstr(CString(pMoverProp->szName).MakeLower(), CString(m_pEdit->GetString()).MakeLower()))
						continue;
				}
				m_pListbox->AddString("");
				m_vecMover.push_back(pMoverProp);
			}
		}
	}
}

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
void CWndWikipediaMonster::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (m_pCreate == pWndChild)
	{
		SAFE_DELETE(m_pCreate);
		pWndChild = NULL;
	}
	else if (m_pWndDropInfo == pWndChild)
	{
		SAFE_DELETE(m_pWndDropInfo);
		pWndChild = NULL;
	}
}
#endif

CWndWikipediaMonsterCreate::CWndWikipediaMonsterCreate()
{
	m_pMoverPropCreate = NULL;
	m_pEdit = NULL;
	m_pButtOk = NULL;
	m_pCheck1 = NULL;
	m_pCheck2 = NULL;
	m_pStatic = NULL;
}

CWndWikipediaMonsterCreate::~CWndWikipediaMonsterCreate()
{
}

BOOL CWndWikipediaMonsterCreate::Initialize(CWndBase* pWndParent, DWORD dwStyle)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_WIKIPEDIA_MONSTER_CREATE, WBS_MODAL, 0, pWndParent);
}

void CWndWikipediaMonsterCreate::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	m_pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	m_pButtOk = (CWndButton*)GetDlgItem(WIDC_BT_OK);
	m_pCheck1 = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	m_pCheck2 = (CWndButton*)GetDlgItem(WIDC_CHECK2);
	m_pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	MoveParentCenter();
}

void CWndWikipediaMonsterCreate::OnDraw(C2DRender* p2DRender)
{
	CWndNeuz::OnDraw(p2DRender);
	if (m_pButtOk)
		m_pButtOk->EnableWindow(atoi(m_pEdit->GetString()) > 0);
}

BOOL CWndWikipediaMonsterCreate::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_BT_OK)
	{
		if (m_pMoverPropCreate)
		{
			CString str;
			str.Format("/cn %d %d %d", m_pMoverPropCreate->dwID, atoi(m_pEdit->GetString()), m_pCheck1->GetCheck());
			g_DPlay.SendChat(str);

			if (m_pCheck2->GetCheck())
			{
				if (atoi(m_pEdit->GetString()) == 1)
					str.Format("/sys %d %s has been summoned.", atoi(m_pEdit->GetString()), m_pMoverPropCreate->szName);
				else
					str.Format("/sys %d %s were summoned.", atoi(m_pEdit->GetString()), m_pMoverPropCreate->szName);

				g_DPlay.SendChat(str);
			}
			Destroy();
		}
	}
	else if (nID == WIDC_BT_CANCEL)
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndWikipediaMonsterCreate::SetItem(MoverProp* pMoverProp)
{
	m_pMoverPropCreate = pMoverProp;
	if (m_pMoverPropCreate)
	{
		m_pStatic->SetTitle(m_pMoverPropCreate->szName);
		m_pEdit->AddWndStyle(EBS_NUMBER);
		m_pEdit->SetString("1");
	}
}
#endif // __WIKIPEDIA_MONSTER

