#include "StdAfx.h"

#if __VER >= 15 // __GUILD_HOUSE
#include "WndGuildTabBuffs.h"
#include "AppDefine.h"
#include "DPClient.h"

extern	CDPClient	g_DPlay;

CWndGuildTabBuffs::CWndGuildTabBuffs()
{
}

CWndGuildTabBuffs::~CWndGuildTabBuffs()
{
}

void CWndGuildTabBuffs::OnDraw(C2DRender* p2DRender)
{
	// Main Info
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);
	CString strTitle;
	strTitle.Format("Every 10 levels a new guild buff unlocks! When your guild is level 50 the maximum amount of buffs are reached. Hover over the icons below to see the buff and its effect.");
	pWndText->SetString(strTitle);

	CTexture* pTexture;

	// Buffs
	ItemProp* GuildBuff1 = prj.GetItemProp(II_GUILD_BUFF_01);
	ItemProp* GuildBuff2 = prj.GetItemProp(II_GUILD_BUFF_02);
	ItemProp* GuildBuff3 = prj.GetItemProp(II_GUILD_BUFF_03);
	ItemProp* GuildBuff4 = prj.GetItemProp(II_GUILD_BUFF_04);
	ItemProp* GuildBuff5 = prj.GetItemProp(II_GUILD_BUFF_05);

	// Ctrls
	LPWNDCTRL pWndCtrl1 = GetWndCtrl(WIDC_STATIC);
	LPWNDCTRL pWndCtrl2 = GetWndCtrl(WIDC_STATIC1);
	LPWNDCTRL pWndCtrl3 = GetWndCtrl(WIDC_STATIC2);
	LPWNDCTRL pWndCtrl4 = GetWndCtrl(WIDC_STATIC3);
	LPWNDCTRL pWndCtrl5 = GetWndCtrl(WIDC_STATIC4);

	if (GuildBuff1 != NULL) {
		pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, GuildBuff1->szIcon), COLOR_ITEM_BG);
		if (pTexture != NULL)
			pTexture->Render(p2DRender, CPoint(pWndCtrl1->rect.left, pWndCtrl1->rect.top));
	}

	if (GuildBuff2 != NULL) {
		pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, GuildBuff2->szIcon), COLOR_ITEM_BG);
		if (pTexture != NULL)
			pTexture->Render(p2DRender, CPoint(pWndCtrl2->rect.left, pWndCtrl2->rect.top));
	}

	if (GuildBuff3 != NULL) {
		pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, GuildBuff3->szIcon), COLOR_ITEM_BG);
		if (pTexture != NULL)
			pTexture->Render(p2DRender, CPoint(pWndCtrl3->rect.left, pWndCtrl3->rect.top));
	}

	if (GuildBuff4 != NULL) {
		pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, GuildBuff4->szIcon), COLOR_ITEM_BG);
		if (pTexture != NULL)
			pTexture->Render(p2DRender, CPoint(pWndCtrl4->rect.left, pWndCtrl4->rect.top));
	}

	if (GuildBuff5 != NULL) {
		pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, GuildBuff5->szIcon), COLOR_ITEM_BG);
		if (pTexture != NULL)
			pTexture->Render(p2DRender, CPoint(pWndCtrl5->rect.left, pWndCtrl5->rect.top));
	}

	CPoint pointMouse = GetMousePoint();
	CWndStatic* Static1 = (CWndStatic*)GetDlgItem(WIDC_STATIC);
	CWndStatic* Static2 = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	CWndStatic* Static3 = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	CWndStatic* Static4 = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
	CWndStatic* Static5 = (CWndStatic*)GetDlgItem(WIDC_STATIC4);

	CRect rect1 = Static1->GetWndRect();
	if (rect1.PtInRect(pointMouse)) {
		CItemElem Item;
		Item.m_dwItemId = GuildBuff1->dwID;

		ClientToScreen(&pointMouse);
		ClientToScreen(&rect1);
		g_WndMng.PutToolTip_Item(&Item, pointMouse, &rect1);
	}

	CRect rect2 = Static2->GetWndRect();
	if (rect2.PtInRect(pointMouse)) {
		CItemElem Item;
		Item.m_dwItemId = GuildBuff2->dwID;

		ClientToScreen(&pointMouse);
		ClientToScreen(&rect2);
		g_WndMng.PutToolTip_Item(&Item, pointMouse, &rect2);
	}

	CRect rect3 = Static3->GetWndRect();
	if (rect3.PtInRect(pointMouse)) {
		CItemElem Item;
		Item.m_dwItemId = GuildBuff3->dwID;

		ClientToScreen(&pointMouse);
		ClientToScreen(&rect3);
		g_WndMng.PutToolTip_Item(&Item, pointMouse, &rect3);
	}

	CRect rect4 = Static4->GetWndRect();
	if (rect4.PtInRect(pointMouse)) {
		CItemElem Item;
		Item.m_dwItemId = GuildBuff4->dwID;

		ClientToScreen(&pointMouse);
		ClientToScreen(&rect4);
		g_WndMng.PutToolTip_Item(&Item, pointMouse, &rect4);
	}

	CRect rect5 = Static5->GetWndRect();
	if (rect5.PtInRect(pointMouse)) {
		CItemElem Item;
		Item.m_dwItemId = GuildBuff5->dwID;

		ClientToScreen(&pointMouse);
		ClientToScreen(&rect5);
		g_WndMng.PutToolTip_Item(&Item, pointMouse, &rect5);
	}
}

void CWndGuildTabBuffs::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	UpdateData();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndGuildTabBuffs::Initialize(CWndBase* pWndParent, DWORD)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILD_TAB_BUFFS, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndGuildTabBuffs::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndGuildTabBuffs::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndGuildTabBuffs::UpdateData()
{
	CGuild* pGuild = g_pPlayer->GetGuild();
	if (pGuild)
	{
	}
	else
	{

	}
}

void CWndGuildTabBuffs::SetData(DWORD dwPower[])
{
}

BOOL CWndGuildTabBuffs::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CGuild* pGuild = g_pPlayer->GetGuild();

	if (!pGuild)
		return FALSE;

	CGuildMember* pGuildMember = pGuild->GetMember(g_pPlayer->m_idPlayer);

	if (!pGuildMember)
		return FALSE;

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

#endif //__GUILD_HOUSE