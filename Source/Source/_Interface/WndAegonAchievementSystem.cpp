#include "stdafx.h"
#include "resData.h"
#include "WndAegonAchievementSystem.h"
#include "DPClient.h"
#include "defineText.h"
#include "WndManager.h"
#include "AegonAchievementSystem.h"
#include "WndManager.h"
#include "Model.h"
#include "ModelMng.h"
#include "ModelObject.h"
#include "ModelGlobal.h"
#include "math.h"

extern CDPClient g_DPlay;

#ifdef __AEGON_ACHIEVEMENT_SYSTEM

///////////////////////////
///////MAIN WINDOW/////////
///////////////////////////
CWndAchievementSystemMain::CWndAchievementSystemMain()
{
	m_pListItems = NULL;
	m_pImageWnd = NULL;
	m_pProgressWnd = NULL;
	m_pTextWnd = NULL;
	m_pProgressBarWnd = NULL;
	m_achievementImage = NULL;
	m_pMainGaugeBack = NULL;
	m_pMainGaugeFront = NULL;
	m_pSideGaugeBack = NULL;
	m_pSideGaugeFront = NULL;

	m_mainProgressGaugeFront = NULL;
	m_sideProgressGaugeFront = NULL;
	m_sideProgressGaugeFront2 = NULL;
	

	nCurListSelection = -1;


}
CWndAchievementSystemMain::~CWndAchievementSystemMain()
{
	SAFE_RELEASE(m_pMainGaugeBack);
	SAFE_RELEASE(m_pMainGaugeFront);

	SAFE_RELEASE(m_pSideGaugeBack);
	SAFE_RELEASE(m_pSideGaugeFront);
}
void CWndAchievementSystemMain::OnDraw(C2DRender* p2DRender)
{
	m_pMainProgressBarGaugeRect = m_pProgressBarWnd->GetWndRect();
	m_pSideProgressBarGaugeRect = m_pProgressWnd->GetWndRect();

	if (m_pImageWnd && m_achievementImage)
	{
		m_achievementImage->m_size = m_pImageWnd->GetWndRect().Size();
		m_achievementImage->Render(p2DRender, m_pImageWnd->GetWndRect().TopLeft(), (DWORD)g_Option.m_nWindowAlpha);
	}

	CString strTmp;
	CEditString strEditOut;
	CPoint point = m_pProgressBarWnd->GetWndRect().TopLeft();

#ifdef __AEGON_THEME_SWITCHER
	DWORD dwStaticColor = CAegonThemeSwitcherMng::GetInstance()->GetStaticColor();
#endif //__AEGON_THEME_SWITCHER

	//Main Gauge
	int finished = CAegonAchievementMng::GetInstance()->GetFinishedCount();
	int total = CAegonAchievementMng::GetInstance()->m_mapAchievements.size();
	if (!m_pMainGaugeBack)
	{
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pMainGaugeBack, NULL);
	}
	else
	{
		m_mainProgressGaugeBack->m_sizePitch.cy = m_mainProgressGaugeBack->m_size.cy = AEGACH_MAIN_GAUGE_HEIGHT;
		m_Theme.RenderGauge(p2DRender, &m_pMainProgressBarGaugeRect, D3DCOLOR_ARGB(255, 255, 255, 255), m_pMainGaugeBack, m_mainProgressGaugeBack);
	}
	if (!m_pMainGaugeFront)
	{
		
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pMainGaugeFront, NULL);
	}
	else
	{
		double progress;
		if (total != 0)
			progress = ((double)finished / (double)total);
		else
			progress = 0;

		if (progress < 0)
			progress = 0;

		if (progress > 1)
			progress = 1;

		m_mainProgressGaugeFront->m_sizePitch.cy = m_mainProgressGaugeFront->m_size.cy = AEGACH_MAIN_GAUGE_HEIGHT;

		m_pMainProgressBarGaugeRect.right = m_pMainProgressBarGaugeRect.left + (((double)m_pMainProgressBarGaugeRect.right - (double)m_pMainProgressBarGaugeRect.left) * progress);

		int diff = m_pSideProgressBarGaugeRect.right - m_pSideProgressBarGaugeRect.left;
		if (diff > 0 && diff < m_sideProgressGaugeFront2->m_size.cx)
			m_pSideProgressBarGaugeRect.right = m_pSideProgressBarGaugeRect.left + m_sideProgressGaugeFront2->m_size.cx;

		m_Theme.RenderGauge(p2DRender, &m_pMainProgressBarGaugeRect, D3DCOLOR_ARGB(255, 255, 255, 255), m_pMainGaugeFront, m_mainProgressGaugeFront);
	}
	strTmp.Format("%d / %d", finished, total);
	strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
	strEditOut.SetColor(COLOR_BLACK);
#else //__AEGON_THEME_SWITCHER
	strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
	strEditOut.SetStyle(ESSTY_BOLD);
	point.x = m_pProgressBarWnd->GetWndRect().left + (m_pProgressBarWnd->GetWndRect().right - m_pProgressBarWnd->GetWndRect().left) / 2;
	p2DRender->TextOut_EditString(point.x - (p2DRender->m_pFont->GetTextExtent(strEditOut).cx / 2), point.y, strEditOut, 0, 0, 2);
	strEditOut.ClearStyle(ESSTY_BOLD);
	//~Main Gauge

/*Styles
ESSTY_UNDERLINE
ESSTY_BOLD
ESSTY_STRIKETHROUGH
ESSTY_SHADOW
ESSTY_BLOCK
*/
	

	//Progress Window
	//CD3DFont* progressWndFont = CWndBase::m_Theme.m_pFontGuildCombatText;
	//CD3DFont* oldFont = p2DRender->GetFont();
	//p2DRender->SetFont(progressWndFont);
	point = m_pProgressWnd->GetWndRect().TopLeft();
	
	if (nCurListSelection == -1) //TODO category progress
	{
		strTmp.Format("Category - %s", m_strSelectedKeyword.GetString());
		strEditOut.SetParsingString(strTmp);

#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		strEditOut.SetStyle(ESSTY_BOLD);
		point.x = m_pProgressWnd->GetWndRect().left + (m_pProgressWnd->GetWndRect().right - m_pProgressWnd->GetWndRect().left) / 2;
		p2DRender->TextOut_EditString(point.x - (p2DRender->m_pFont->GetTextExtent(strEditOut).cx / 2), point.y, strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD);
		point.y += p2DRender->m_pFont->GetTextExtent(strEditOut).cy + 5;

		m_pSideProgressBarGaugeRect.top = point.y;

		int finished = CAegonAchievementMng::GetInstance()->GetFinishedCount(m_strSelectedKeyword);
		int total = CAegonAchievementMng::GetInstance()->GetCategoryAchievementCount(m_strSelectedKeyword);
		if (!m_pSideGaugeBack)
		{
			m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pSideGaugeBack, NULL);
		}
		else
		{
			m_sideProgressGaugeBack->m_sizePitch.cy = m_mainProgressGaugeBack->m_size.cy = AEGACH_SIDE_GAUGE_HEIGHT;
			m_Theme.RenderGauge(p2DRender, &m_pSideProgressBarGaugeRect, D3DCOLOR_ARGB(255, 255, 255, 255), m_pSideGaugeBack, m_sideProgressGaugeBack);
		}
		if (!m_pSideGaugeFront)
		{

			m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pSideGaugeFront, NULL);
		}
		else
		{
			double progress;
			if (total != 0)
				progress = ((double)finished / (double)total);
			else
				progress = 0;

			if (progress < 0)
				progress = 0;

			if (progress > 1)
				progress = 1;

			m_sideProgressGaugeFront2->m_sizePitch.cy = m_sideProgressGaugeFront2->m_size.cy = AEGACH_SIDE_GAUGE_HEIGHT - 2;

			m_pSideProgressBarGaugeRect.right = m_pSideProgressBarGaugeRect.left + ceil(((double)m_pSideProgressBarGaugeRect.right - (double)m_pSideProgressBarGaugeRect.left) * progress);

			int diff = m_pSideProgressBarGaugeRect.right - m_pSideProgressBarGaugeRect.left;
			if (diff > 0 && diff < m_sideProgressGaugeFront2->m_size.cx)
				m_pSideProgressBarGaugeRect.right = m_pSideProgressBarGaugeRect.left + m_sideProgressGaugeFront2->m_size.cx;

			m_Theme.RenderGauge(p2DRender, &m_pSideProgressBarGaugeRect, D3DCOLOR_ARGB(255, 255, 255, 255), m_pSideGaugeFront, m_sideProgressGaugeFront2);

		}
		strTmp.Format("%d / %d", finished, total);
		strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
		strEditOut.SetColor(COLOR_BLACK);
#else //__AEGON_THEME_SWITCHER
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
		strEditOut.SetStyle(ESSTY_BOLD);
		point.x = m_pProgressWnd->GetWndRect().left + (m_pProgressWnd->GetWndRect().right - m_pProgressWnd->GetWndRect().left) / 2;
		p2DRender->TextOut_EditString(point.x - (p2DRender->m_pFont->GetTextExtent(strEditOut).cx / 2), point.y, strEditOut, 0, 0, 2);
		strEditOut.ClearStyle(ESSTY_BOLD);
		point.y += p2DRender->m_pFont->GetTextExtent(strEditOut).cy + 5;
		
	}
	else //single achievement progress
	{
		map<int, __ACHIEVEMENT_UNIT>::iterator it = CAegonAchievementMng::GetInstance()->m_mapAchievements.find(m_vecItems[nCurListSelection]->achievementID);

		if (it != CAegonAchievementMng::GetInstance()->m_mapAchievements.end())
		{

			__ACHIEVEMENT_PROGRESS_UNIT* unit = g_pPlayer->GetAchievementProgress()->GetUnit(it->second.achievementID);


			strTmp.Format("%s", it->second.achievementName);
			strEditOut.SetParsingString(strTmp);

#ifdef __AEGON_THEME_SWITCHER
			strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
			strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
			strEditOut.SetStyle(ESSTY_BOLD);
			point.x = m_pProgressWnd->GetWndRect().left + (m_pProgressWnd->GetWndRect().right - m_pProgressWnd->GetWndRect().left) / 2;
			p2DRender->TextOut_EditString(point.x - (p2DRender->m_pFont->GetTextExtent(strEditOut).cx / 2), point.y, strEditOut, 0, 0, 2);
			strEditOut.ClearStyle(ESSTY_BOLD);
			point.y += p2DRender->m_pFont->GetTextExtent(strEditOut).cy + 5;


			m_pSideProgressBarGaugeRect.top = point.y;

			if (!m_pSideGaugeBack)
			{
				m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pSideGaugeBack, NULL);
			}
			else
			{
				m_sideProgressGaugeBack->m_sizePitch.cy = m_mainProgressGaugeBack->m_size.cy = AEGACH_SIDE_GAUGE_HEIGHT;
				m_Theme.RenderGauge(p2DRender, &m_pSideProgressBarGaugeRect, D3DCOLOR_ARGB(255, 255, 255, 255), m_pSideGaugeBack, m_sideProgressGaugeBack);
			}
			if (!m_pSideGaugeFront)
			{

				m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pSideGaugeFront, NULL);
			}
			else
			{
				double progress = 0;
				if(unit)
					progress = unit->GetProgressPercent();

				if (progress < 0)
					progress = 0;

				if (progress > 1)
					progress = 1;

				m_sideProgressGaugeFront->m_sizePitch.cy = m_sideProgressGaugeFront->m_size.cy = AEGACH_SIDE_GAUGE_HEIGHT;

				m_pSideProgressBarGaugeRect.right = m_pSideProgressBarGaugeRect.left + ceil(((double)m_pSideProgressBarGaugeRect.right - (double)m_pSideProgressBarGaugeRect.left) * progress);

				int diff = m_pSideProgressBarGaugeRect.right - m_pSideProgressBarGaugeRect.left;
				if(diff > 0 && diff < m_sideProgressGaugeFront->m_size.cx)
					m_pSideProgressBarGaugeRect.right = m_pSideProgressBarGaugeRect.left + m_sideProgressGaugeFront->m_size.cx;
				
				m_Theme.RenderGauge(p2DRender, &m_pSideProgressBarGaugeRect, D3DCOLOR_ARGB(255, 255, 255, 255), m_pSideGaugeFront, m_sideProgressGaugeFront);

			}

			int progressCount = 0;
			if (unit)
				progressCount = unit->nProgressCount;

			strTmp.Format("%d / %d", progressCount, it->second.maxProgress);
			strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
			strEditOut.SetColor(COLOR_BLACK);
#else //__AEGON_THEME_SWITCHER
			strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
			strEditOut.SetStyle(ESSTY_BOLD);
			point.x = m_pProgressWnd->GetWndRect().left + (m_pProgressWnd->GetWndRect().right - m_pProgressWnd->GetWndRect().left) / 2;
			p2DRender->TextOut_EditString(point.x - (p2DRender->m_pFont->GetTextExtent(strEditOut).cx / 2), point.y, strEditOut, 0, 0, 2);
			strEditOut.ClearStyle(ESSTY_BOLD);
			point.y += p2DRender->m_pFont->GetTextExtent(strEditOut).cy + 5;


			strTmp.Format("Reward: %dx %s", it->second.rewardNum, prj.GetItemProp(it->second.reward)->szName);
			strEditOut.SetParsingString(strTmp);
#ifdef __AEGON_THEME_SWITCHER
			strEditOut.SetColor(dwStaticColor);
#else //__AEGON_THEME_SWITCHER
			strEditOut.SetColor(D3DCOLOR_ARGB(255, 46, 112, 169));
#endif //__AEGON_THEME_SWITCHER
			strEditOut.SetStyle(ESSTY_BOLD);
			point.x = m_pProgressWnd->GetWndRect().left + (m_pProgressWnd->GetWndRect().right - m_pProgressWnd->GetWndRect().left) / 2;
			p2DRender->TextOut_EditString(point.x - (p2DRender->m_pFont->GetTextExtent(strEditOut).cx / 2), point.y, strEditOut, 0, 0, 2);
			strEditOut.ClearStyle(ESSTY_BOLD);
			point.y += p2DRender->m_pFont->GetTextExtent(strEditOut).cy + 5;
			
		}
	}
	//p2DRender->SetFont(oldFont);
	//~Progress Window
}
void CWndAchievementSystemMain::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	//Move Window to center
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();

	m_pListItems = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	m_pImageWnd = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	m_pProgressWnd = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
	m_pTextWnd = (CWndText*)GetDlgItem(WIDC_TEXT1);
	m_pProgressBarWnd = (CWndStatic*)GetDlgItem(WIDC_STATIC7);
	m_pWndTreeCtrl = (CWndTreeCtrl*)GetDlgItem(WIDC_TREECTRL);

	m_CollectButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	m_FavoriteCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);

	m_pWndTreeCtrl->LoadTreeScript(MakePath(DIR_CLIENT, "AchievementCategories.inc"));
	if (m_pWndTreeCtrl->GetTreeItemsNumber() > 0)
	{
		m_pWndTreeCtrl->SetCurSel((DWORD)0);
		OnChildNotify(0, WIDC_TREECTRL, (LRESULT*)m_pWndTreeCtrl->GetCurSel());
	}

	m_mainProgressGaugeBack = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_ACHIEVEMENT, "EmptyMainGaugeBar.bmp"), COLOR_ITEM_BG, TRUE);
	m_mainProgressGaugeFront = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_ACHIEVEMENT, "MainGaugeBar.tga"), COLOR_ITEM_BG, TRUE);

	m_sideProgressGaugeBack = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_ACHIEVEMENT, "EmptyMainGaugeBar.bmp"), COLOR_ITEM_BG, TRUE);
	m_sideProgressGaugeFront = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_ACHIEVEMENT, "AchievementGaugeBar.tga"), COLOR_ITEM_BG, TRUE);
	m_sideProgressGaugeFront2 = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_ACHIEVEMENT, "CategoryGaugeBar.tga"), COLOR_ITEM_BG, TRUE);
	
	UpdateList();

	g_DPlay.SendLoadAchievementProgress();

	m_CollectButton->SetVisible(FALSE);
	m_FavoriteCheck->SetVisible(FALSE);


}
BOOL CWndAchievementSystemMain::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_AEGON_ACHIEVEMENT_SYSTEM, 0, CPoint(0, 0), pWndParent);
}
void CWndAchievementSystemMain::OnDestroy()
{
	Destroy();
}
BOOL CWndAchievementSystemMain::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndAchievementSystemMain::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndAchievementSystemMain::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndAchievementSystemMain::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndAchievementSystemMain::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{

	if (nID == WIDC_LISTBOX1)
	{
		if (message == WNM_SELCHANGE)
		{
			nCurListSelection = m_pListItems->GetCurSel();
			ListBoxSelChanged();
		}
	}
	else if (nID == WIDC_TREECTRL)
	{
		if (message == WNM_DBLCLK)
			return FALSE;

		LPTREEELEM lpTreeElem = (LPTREEELEM)pLResult;
		BOOL selChanged = FALSE;
		

		if (lpTreeElem)
		{

			m_strKeyword = lpTreeElem->m_strKeyword;

			//if (m_strSelectedKeyword.Compare(m_strKeyword) != 0)
			{
				m_strSelectedKeyword = m_strKeyword;
				m_dwSelectedData = lpTreeElem->m_dwData;
				UpdateList();
			}					
		}
	}
	else if (nID == WIDC_BUTTON1) 
	{
		if(nCurListSelection >= 0 && nCurListSelection < m_vecItems.size())
			g_DPlay.SendCollectAchievement(m_vecItems[nCurListSelection]->achievementID);
	}
	else if (nID == WIDC_CHECK1) 
	{
		if (nCurListSelection >= 0 && nCurListSelection < m_vecItems.size())
			g_DPlay.SendAchievementFavorite(m_vecItems[nCurListSelection]->achievementID);
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndAchievementSystemMain::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}
BOOL CWndAchievementSystemMain::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	return TRUE;
}
void CWndAchievementSystemMain::UpdateList() 
{
	BOOL hasParent = FALSE;
	BOOL addElem = FALSE;

	m_vecItems.clear();
	m_pListItems->ResetContent();

	

	map<int, __ACHIEVEMENT_UNIT>::iterator it = CAegonAchievementMng::GetInstance()->m_mapAchievements.begin();

	for (; it != CAegonAchievementMng::GetInstance()->m_mapAchievements.end(); it++)
	{
		if (it->second.IsInCategory(m_strSelectedKeyword) )
		{
			m_vecItems.push_back(&it->second);
			m_pListItems->AddString("");
			continue;
		}		
		if (m_strSelectedKeyword.Compare("Favorites") == 0)
		{
			if (easy_find(g_pPlayer->m_AchievementFavorites, it->second.achievementID)) 
			{
				m_vecItems.push_back(&it->second);
				m_pListItems->AddString("");
				continue;
			}
		}
		if (m_strSelectedKeyword.Compare("Claimable") == 0)
		{
			__ACHIEVEMENT_PROGRESS_UNIT* unit = g_pPlayer->GetAchievementProgress()->GetUnit(it->second.achievementID);
			if (unit && unit->nRewardSent == 0 && !easy_find(g_pPlayer->m_AchievementProgress.unfinishedUnit, it->second.achievementID))
			{
				m_vecItems.push_back(&it->second);
				m_pListItems->AddString("");
				continue;
			}
		}
	}

	

	nCurListSelection = m_pListItems->GetCurSel();
	ListBoxSelChanged();
}

void CWndAchievementSystemMain::ListBoxSelChanged()
{
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	RefreshButtonState();
	if (nCurListSelection < 0 || nCurListSelection >= m_vecItems.size()) 
	{
		DWORD categoryDescriptionTID = CAegonAchievementMng::GetInstance()->GetCategoryDescription(m_strSelectedKeyword, m_dwSelectedData);
		CString strCategoryImage = CAegonAchievementMng::GetInstance()->GetCategoryImage(m_strSelectedKeyword, m_dwSelectedData);


		if (categoryDescriptionTID)
		{
			m_pTextWnd->SetString(prj.GetText(categoryDescriptionTID), dwColorText);
		}
		else 
		{
			m_pTextWnd->SetString("default category text", dwColorText);
		}
		m_achievementImage = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_ACHIEVEMENT, strCategoryImage.GetString()), COLOR_ITEM_BG, TRUE);
		
		
		return;
	}
		

	__ACHIEVEMENT_UNIT* activeUnit = m_vecItems[nCurListSelection];

	m_achievementImage = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_ACHIEVEMENT, activeUnit->imageFile), COLOR_ITEM_BG, TRUE);


	CEditString strEdit;
	strEdit.Init(m_pTextWnd->m_pFont, &m_pTextWnd->GetClientRect());
	CAegonAchievementMng::GetInstance()->GetAchievementDescriptionEditString(&strEdit, activeUnit->achievementID);
	m_pTextWnd->ForceSetEditString(strEdit);

}
void CWndAchievementSystemMain::OpenAchievement(int achievementID) 
{
	map<int, __ACHIEVEMENT_UNIT>::iterator it = CAegonAchievementMng::GetInstance()->m_mapAchievements.find(achievementID);

	if(it != CAegonAchievementMng::GetInstance()->m_mapAchievements.end())
	{
		CString strTmp = "";
		CString strCategory;

		strCategory.Format("%s", it->second.categoryName);
		LPTREEELEM treeElem = m_pWndTreeCtrl->SetCurSel(strCategory);
		if (treeElem) 
		{
			treeElem->m_bOpen = TRUE;
			m_dwSelectedData = treeElem->m_dwData;
		}
			

		for (int i = 0; i < it->second.subcategories.size(); i++)
		{
			if (strTmp.Compare(it->second.subcategories[i]) != 0)
			{
				m_strSelectedKeyword = it->second.subcategories[i];

				LPTREEELEM treeElem = m_pWndTreeCtrl->SetCurSel(m_strSelectedKeyword);
				if (treeElem) 
				{
					treeElem->m_bOpen = TRUE;
				}
					
			}
		}

		LPTREEELEM result = m_pWndTreeCtrl->SetCurSel(m_strSelectedKeyword);
		if (result) 
		{
			result->m_bOpen = TRUE;
		}
			

		UpdateList();

		for (int i = 0; i < m_vecItems.size(); i++)
		{
			if (m_vecItems[i]->achievementID == achievementID)
			{
				m_pListItems->SetCurSel(i);
				nCurListSelection = i;
				ListBoxSelChanged();
				return;
			}
		}
	}
}

void CWndAchievementSystemMain::RefreshButtonState() 
{
	if (nCurListSelection >= 0 && nCurListSelection < m_vecItems.size()) 
	{
		m_CollectButton->SetVisible(TRUE);
		m_FavoriteCheck->SetVisible(TRUE);

		if (easy_find(g_pPlayer->m_AchievementFavorites, m_vecItems[nCurListSelection]->achievementID)) 
		{
			m_FavoriteCheck->SetCheck(TRUE);
			
		}
		else 
		{
			m_FavoriteCheck->SetCheck(FALSE);
		}

		__ACHIEVEMENT_PROGRESS_UNIT* unit = g_pPlayer->GetAchievementProgress()->GetUnit(m_vecItems[nCurListSelection]->achievementID);
		if (unit && unit->IsFinished() && unit->nRewardSent == 0) 
		{
			m_CollectButton->EnableWindow(TRUE);
		}
		else
		{
			m_CollectButton->EnableWindow(FALSE);
		}
		
	}
	else
	{
		m_CollectButton->SetVisible(FALSE);
		m_FavoriteCheck->SetVisible(FALSE);
	}
}
void CWndAchievementSystemMain::OnFavoritesRefresh() 
{
	if(m_strSelectedKeyword.Compare("Favorites") == 0)
		UpdateList();

	RefreshButtonState();
}
void CWndAchievementSystemMain::OnProgressRefresh()
{
	if (m_strSelectedKeyword.Compare("Claimable") == 0)
		UpdateList();

	RefreshButtonState();
}

HRESULT CWndAchievementSystemMain::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	return S_OK;
}

HRESULT CWndAchievementSystemMain::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE(m_pMainGaugeBack);
	SAFE_RELEASE(m_pMainGaugeFront);
	SAFE_RELEASE(m_pSideGaugeBack);
	SAFE_RELEASE(m_pSideGaugeFront);
	return S_OK;
}

HRESULT CWndAchievementSystemMain::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}
#endif // __AEGON_ACHIEVEMENT_SYSTEM