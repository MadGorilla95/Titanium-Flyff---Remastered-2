#ifndef __WNDSELECTCH__H
#define __WNDSELECTCH__H

#ifdef __V18TICKET
#include "ticket.h"
#endif //__V18TICKET

#ifdef __AZRIA_1023
class CWndSelectCh : public CWndNeuz
{
#ifdef __V18TICKET
	CWndListBox* pWndListBox;
	CWndListBox* pWndListBox2;

	unsigned long propItemId;
	unsigned long mixedVar;

	std::vector<const TicketProp*> worldList;

	void createWorldListBox();
	void createChannelListBox(const unsigned long index = 0) const;
	bool checkWorld();
#else //__V18TICKET
private:
	int m_nItemId;
	int m_nChCount;
#endif //__V18TICKET

public:
#ifdef __V18TICKET
	CWndSelectCh(const unsigned long mixVar, const unsigned long propId);
#else //__V18TICKET
	CWndSelectCh(int nItemId, int nChCount);
#endif //__V18TICKET
	~CWndSelectCh();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
#ifndef __V18TICKET
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
#endif //__V18TICKET
};
#endif //__AZRIA_1023

#endif //__WNDSELECTCH__H