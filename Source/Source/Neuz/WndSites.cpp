#include "stdafx.h"
#include "ResData.h"

#ifdef __TASKBAR_OPEN_SITE

// Homepage URL Applet

CWndHomepage::CWndHomepage()
{
}
CWndHomepage::~CWndHomepage()
{
}
void CWndHomepage::OnDraw(C2DRender* p2DRender)
{
	MoveParentCenter();
}

BOOL CWndHomepage::Initialize(CWndBase* pWndParent, DWORD)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_HOMEPAGE, 0, CPoint(0, 0), pWndParent);
}

void CWndHomepage::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();
}

BOOL CWndHomepage::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_BUTTON:
	{
		ShellExecute(0, TEXT("open"), TEXT("https://titanium-mmo.com/"), 0, 0, 0);
		Destroy();
		break;
	}
	case WIDC_BUTTON1:
	{
		Destroy();
		break;
	}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

// Discord URL Applet

CWndDiscord::CWndDiscord()
{
}
CWndDiscord::~CWndDiscord()
{
}
void CWndDiscord::OnDraw(C2DRender* p2DRender)
{
	MoveParentCenter();
}

BOOL CWndDiscord::Initialize(CWndBase* pWndParent, DWORD)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_DISCORD, 0, CPoint(0, 0), pWndParent);
}

void CWndDiscord::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();
}

BOOL CWndDiscord::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_BUTTON:
	{
		ShellExecute(0, TEXT("open"), TEXT("https://discord.gg/SNz9yAqFWw"), 0, 0, 0);
		Destroy();
		break;
	}
	case WIDC_BUTTON1:
	{
		Destroy();
		break;
	}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#endif //__TASKBAR_OPEN_SITE