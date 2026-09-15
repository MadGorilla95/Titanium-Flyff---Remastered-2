#pragma once
#include "party.h"
#ifdef __SYS_PARTYFINDER
class CWndPartyFind : public CWndNeuz
{
private:
	int						m_nSelected;
	CWndListBox* pWndListBox;
	CWndText* pWndText;

public:
	CWndPartyFind();
	~CWndPartyFind();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	vector<PARTYFINDER_LIST>	GetPartyList() const;
	void RefreshItemList();
	vector<PARTYFINDER_LIST>	m_mapItem;
};
#endif //__SYS_PARTYFINDER