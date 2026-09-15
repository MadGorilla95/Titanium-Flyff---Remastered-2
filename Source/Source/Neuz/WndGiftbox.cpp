#include "stdafx.h"
#include "ResData.h"
#ifdef __GIFTBOX_LIST_ITEMS
#include "WndGiftbox.h"

CWndGiftbox::CWndGiftbox()
{
}

CWndGiftbox::~CWndGiftbox()
{
	m_mapItems.clear();
}

BOOL CWndGiftbox::Initialize(CWndBase* pWndParent, DWORD nType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GIFTBOX, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndGiftbox::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndGiftbox::OnDraw(C2DRender* p2DRender)
{

}
void CWndGiftbox::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();
}
BOOL CWndGiftbox::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

bool CWndGiftbox::Serialize(CAr& ar)
{
	m_mapItems.clear();
#if _WIN64
	unsigned long mapsize;
#else
	size_t mapsize;
#endif
	ar >> mapsize;

	DWORD id, count, prob;
	for (int i = 0; i < mapsize; i++)
	{
		id = prob = 0;
		ar >> id >> count >> prob;
		if (id != 0 && prob != 0)
			m_mapItems.push_back(make_pair(id, make_pair(count, prob)));
	}

	if (m_mapItems.size() == 0) return false;

	return true;
}

vector<pair<DWORD, pair<DWORD, FLOAT>>> CWndGiftbox::Get()
{
	sort(m_mapItems.begin(), m_mapItems.end(), less_second<DWORD, pair<DWORD, FLOAT>>());

	return m_mapItems;
}
#endif //__GIFTBOX_LIST_ITEMS