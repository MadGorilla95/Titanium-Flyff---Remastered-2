#include "stdafx.h"
#include "resData.h"
#include "WndAegonSeasonPass.h"
#include "DPClient.h"
#include "defineText.h"
#include "WndManager.h"
#include "AegonSeasonPass.h"
#include "WndManager.h"
#include "Model.h"
#include "ModelMng.h"
#include "ModelObject.h"
#include "ModelGlobal.h"
#include "math.h"

extern CDPClient g_DPlay;

#ifdef __AEGON_SEASON_PASS

///////////////////////////
///////QUEST WINDOW////////
///////////////////////////
CWndSeasonPassRewards::CWndSeasonPassRewards()
{
	m_rewardBox[0][0] = NULL;
	m_rewardBox[0][1] = NULL;
	m_rewardBox[0][2] = NULL;
	m_rewardBox[1][0] = NULL;
	m_rewardBox[1][1] = NULL;
	m_rewardBox[1][2] = NULL;
	m_rewardBox[2][0] = NULL;
	m_rewardBox[2][1] = NULL;
	m_rewardBox[2][2] = NULL;

	m_numberField[0] = NULL;
	m_numberField[1] = NULL;
	m_numberField[2] = NULL;

	m_pTextureLevelConnection[0] = NULL;
	m_pTextureLevelConnection[1] = NULL;

	m_glowTexture = NULL;


	for (int i = 0; i < 6; i++) 
	{
		m_rewardBoxRect[i] = CRect(0, 0, 0, 0);
		m_rewardNumRect[i] = CRect(0, 0, 0, 0);
		m_rewardFields[i] = CRect(0, 0, 0, 0);

		m_itemTextures[i] = NULL;
	}

	m_selectedReward = -1;

	m_nRewardBoxOffset = 0;
	m_nScrollStep = 1;
	m_nScrollFactor = 1;
	m_reachedLevel = 0;
}
CWndSeasonPassRewards::~CWndSeasonPassRewards()
{
}
void CWndSeasonPassRewards::Create(DWORD dwCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	CWndBase::Create(dwCtrlStyle, rect, pParentWnd, nID);
}
void CWndSeasonPassRewards::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	CRect rect = GetWindowRect();

	AddWndStyle(WBS_HSCROLL);

	m_wndScrollBar.Create(WBS_DOCKING | WBS_HORI, rect, this, 1000);	

	m_rewardBox[0][0] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Free_Box.png"), COLOR_ITEM_BG, TRUE);
	m_rewardBox[0][1] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Free_Box.png"), COLOR_ITEM_BG, TRUE);
	m_rewardBox[0][2] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Free_Box_Done.png"), COLOR_ITEM_BG, TRUE);

	m_rewardBox[1][0] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Normal_Box_Closed.png"), COLOR_ITEM_BG, TRUE);
	m_rewardBox[1][1] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Normal_Box_Open.png"), COLOR_ITEM_BG, TRUE);
	m_rewardBox[1][2] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Normal_Box_Open_Done.png"), COLOR_ITEM_BG, TRUE);

	m_rewardBox[2][0] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Premium_Box_Closed.png"), COLOR_ITEM_BG, TRUE);
	m_rewardBox[2][1] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Premium_Box_Done.png"), COLOR_ITEM_BG, TRUE);
	m_rewardBox[2][2] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Premium_Box_Open_Done.png"), COLOR_ITEM_BG, TRUE);

	m_numberField[0] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Icon_Level_Open.png"), COLOR_ITEM_BG, TRUE);
	m_numberField[1] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Icon_Level_Current.png"), COLOR_ITEM_BG, TRUE);
	m_numberField[2] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Icon_Level_Done.png"), COLOR_ITEM_BG, TRUE);

	m_pTextureLevelConnection[0] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Num_Connection01.png"), COLOR_ITEM_BG, TRUE);
	m_pTextureLevelConnection[1] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Num_Connection02.png"), COLOR_ITEM_BG, TRUE);

	m_glowTexture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "glow.png"), COLOR_ITEM_BG, TRUE);


	m_nRewardBoxOffset = (GetWindowRect().Width() - (5 * m_rewardBox[0][0]->m_size.cx)) / (5);
	m_nScrollFactor = m_rewardBox[0][0]->m_size.cx + m_nRewardBoxOffset;

	int fieldSize = m_rewardBox[0][0]->m_size.cx + (GetWindowRect().Width() - (5 * m_rewardBox[0][0]->m_size.cx)) / (5);
	m_nScrollStep = fieldSize / 4;
	m_wndScrollBar.SetScrollStep(m_nScrollStep);
	
	m_wndScrollBar.SetButton1Texture(MakePath(DIR_THEME_SEASON_PASS, _T("ButtHScrLeft_ex.tga")));
	m_wndScrollBar.SetButton2Texture(MakePath(DIR_THEME_SEASON_PASS, _T("ButtHScrRight_ex.tga")));
	m_wndScrollBar.OffsetButt1Pos(5, 0);
	m_wndScrollBar.OffsetButt2Pos(-5, 0);

	m_wndScrollBar.m_pTexButtHScrBar = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "ButtHScrBar_ex.tga"), COLOR_ITEM_BG);
	m_wndScrollBar.m_pTexButtHScrPLeft = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "ButtHScrPLeft_ex.tga"), COLOR_ITEM_BG);
	m_wndScrollBar.m_pTexButtHScrPRight = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "ButtHScrPRight_ex.tga"), COLOR_ITEM_BG);
	m_wndScrollBar.m_pTexButtHScrPBar = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "ButtHScrPBar_ex.tga"), COLOR_ITEM_BG);


}
void CWndSeasonPassRewards::OnDraw(C2DRender* p2DRender)
{
	__SEASONPASS_UNIT* selectedUnit = ((CWndSeasonPassMain*)GetParentWnd())->m_selectedUnit;
	

	if (!selectedUnit)
		return;

	__SEASONPASS_PROGRESS_UNIT* progressUnit = g_pPlayer->m_SeasonpassProgress.GetProgressUnit(selectedUnit->passID);

	if (!progressUnit)
		return;
	
	// scrollbar

	int nPage = 5 * m_nScrollFactor;
	int nRange = selectedUnit->rewards.size() * m_nScrollFactor;
	int scrollPos = m_wndScrollBar.GetScrollPos();

	if (nRange <= 0)
		nRange = 1;

	if (IsWndStyle(WBS_HSCROLL))
	{
		m_wndScrollBar.SetVisible(TRUE);
		m_wndScrollBar.SetScrollRange(0, nRange);
		m_wndScrollBar.SetScrollPage(nPage);
	}
	else
		m_wndScrollBar.SetVisible(FALSE);

	if (scrollPos > m_wndScrollBar.GetMaxScrollPos() - nPage && scrollPos > nPage)
		m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetMaxScrollPos() - nPage);
	else if (scrollPos < m_wndScrollBar.GetMinScrollPos())
		m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetMinScrollPos());
	// ~scrollbar




	CPoint point = GetWindowRect().TopLeft();

	int nIndex = 0;
	
	int numFieldOffset = 0;

	CString strTmp;
	CEditString strEdit;

	point.x -= (scrollPos) % m_nScrollFactor;
	for (unsigned i = (unsigned)scrollPos; i < (unsigned)scrollPos + nPage + m_nScrollFactor; i+= m_nScrollFactor)
	{
		int nPos = i / m_nScrollFactor;


		point.x += m_nRewardBoxOffset / 2;


		if (nPos > selectedUnit->rewards.size() - 1)
		{
			break;
		}
		

		__SEASONPASS_REWARD* rewardUnit = &selectedUnit->rewards[nPos];
		if (!rewardUnit)
			continue;

		int hasPurchasedPass = progressUnit->boughtPremium;
		
		m_rewardBoxRect[nIndex] = CRect(point.x, point.y, point.x + m_rewardBox[rewardUnit->rewardRarity][hasPurchasedPass]->m_size.cx, point.y + m_rewardBox[rewardUnit->rewardRarity][hasPurchasedPass]->m_size.cy);
		m_rewardNumRect[nIndex] = CRect(m_rewardBoxRect[nIndex].CenterPoint().x - (m_numberField[0]->m_size.cx / 2), m_rewardBoxRect[nIndex].bottom + numFieldOffset, m_rewardBoxRect[nIndex].CenterPoint().x + (m_numberField[0]->m_size.cx / 2), m_rewardBoxRect[nIndex].bottom + numFieldOffset + m_numberField[0]->m_size.cy);

		m_rewardFields[nIndex] = CRect(m_rewardBoxRect[nIndex].left - (m_nRewardBoxOffset / 2), m_rewardBoxRect[nIndex].top, m_rewardBoxRect[nIndex].right + (m_nRewardBoxOffset / 2), GetWindowRect().bottom - m_wndScrollBar.GetWndRect().Height());


		//number field
		int currentLevel = selectedUnit->GetCurrentLevel(progressUnit->progressPoints);
		int numTexture = 0;
		if (currentLevel < nPos)
		{
			numTexture = 0;
		}
		else if (currentLevel == nPos)
		{
			numTexture = 1;
		}
		else if (currentLevel > nPos)
		{
			numTexture = 2;
		}


		if (nPos < selectedUnit->rewards.size() - 1) 
		{
			if (nPos + 1 > currentLevel) 
			{
				if(m_pTextureLevelConnection[0])
					p2DRender->RenderTexture(CPoint(m_rewardNumRect[nIndex].right - 15, m_rewardNumRect[nIndex].CenterPoint().y - (m_pTextureLevelConnection[0]->m_size.cy / 2) + 1), m_pTextureLevelConnection[0]);
				
			}
			else 
			{
				if(m_pTextureLevelConnection[1])
					p2DRender->RenderTexture(CPoint(m_rewardNumRect[nIndex].right - 15, m_rewardNumRect[nIndex].CenterPoint().y - (m_pTextureLevelConnection[1]->m_size.cy / 2) + 1), m_pTextureLevelConnection[1]);
			}
		}


		//selected reward
		if (m_selectedReward == nPos)
		{
			if (m_glowTexture) 
			{
				p2DRender->RenderTexture(m_rewardFields[nIndex].TopLeft(), m_glowTexture);
			}
			
			

			for (int i = 0; i < m_rewardFields[nIndex].Height(); i++) 
			{
				CRect tmpRect = m_rewardFields[nIndex];
				tmpRect.top += i;
				tmpRect.bottom = tmpRect.top + 1;

				int alpha = ((float)i / (float)m_rewardFields[nIndex].Height()) * 50;
				p2DRender->RenderFillRect(tmpRect, D3DCOLOR_ARGB(alpha, 255, 255, 255));
			}
		}
		//~selected reward


		//rewardBox
		p2DRender->RenderTexture(m_rewardBoxRect[nIndex].TopLeft(), m_rewardBox[rewardUnit->rewardRarity][hasPurchasedPass]);

		m_itemTextures[nIndex] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, prj.GetItemProp(rewardUnit->itemID)->szIcon), COLOR_ITEM_BG);
		if (m_itemTextures[nIndex])
		{
			p2DRender->RenderTexture(CPoint(m_rewardBoxRect[nIndex].CenterPoint().x - (m_itemTextures[nIndex]->m_size.cx / 2), m_rewardBoxRect[nIndex].CenterPoint().y - (m_itemTextures[nIndex]->m_size.cy / 2)), m_itemTextures[nIndex]);
		}

		if (easy_find(progressUnit->rewardsClaimed, rewardUnit->rewardID))
		{
			p2DRender->RenderTexture(m_rewardBoxRect[nIndex].TopLeft(), m_rewardBox[rewardUnit->rewardRarity][2]);
		}
		//~rewardBox


		CD3DFont* oldFont = p2DRender->GetFont();
		p2DRender->SetFont(m_Theme.m_pFontSeasonPassNumField);
		strEdit.Init(m_Theme.m_pFontSeasonPassNumField, &m_rewardNumRect[nIndex]);
		p2DRender->RenderTexture(m_rewardNumRect[nIndex].TopLeft(), m_numberField[numTexture]);
		strTmp.Format("%d", nPos);
		strEdit.SetParsingString(strTmp);
		strEdit.SetColor(COLOR_WHITE);
		strEdit.SetStyle(ESSTY_BOLD);
		p2DRender->TextOut_EditString(m_rewardNumRect[nIndex].CenterPoint().x - (p2DRender->GetFont()->GetTextExtent(strEdit.GetString()).cx / 2), m_rewardNumRect[nIndex].CenterPoint().y - (p2DRender->GetFont()->GetTextExtent(strEdit.GetString()).cy / 2) + 2, strEdit, 0, 0, 2);
		strEdit.ClearStyle(ESSTY_BOLD);
		p2DRender->SetFont(oldFont);
		//~number field



		point.x += m_rewardBox[rewardUnit->rewardRarity][hasPurchasedPass]->m_size.cx;
		point.x += m_nRewardBoxOffset / 2;
		nIndex++;
	}
	
}
void CWndSeasonPassRewards::OnLButtonDown(UINT nFlags, CPoint point)
{
	for (int i = 0; i < 6; i++)
	{
		if (m_rewardFields[i].PtInRect(point))
		{
			m_selectedReward = (m_wndScrollBar.GetScrollPos() / m_nScrollFactor) + i;
			break;
		}
	}
}
void CWndSeasonPassRewards::OnMouseWndSurface(CPoint pt)
{

}
BOOL CWndSeasonPassRewards::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	if (m_wndScrollBar.GetScrollPage() >= m_wndScrollBar.GetMaxScrollPos())
		return TRUE;
#endif // __IMPROVE_QUEST_INTERFACE
	if (zDelta < 0)
	{
		if (m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos())
			m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetScrollPos() + m_nScrollStep);
		else
			m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage());
	}
	else
	{
		if (m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos())
			m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetScrollPos() - m_nScrollStep);
		else
			m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetMinScrollPos());
	}

	return TRUE;
}
BOOL CWndSeasonPassRewards::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndBase::OnChildNotify(message, nID, pLResult);
}
void CWndSeasonPassRewards::OnSelectedPassChanged()
{
	SetSelectedRewardCurrentLevel();
	SetScrollPosReward(m_selectedReward);
}
void CWndSeasonPassRewards::SetSelectedRewardCurrentLevel()
{

	__SEASONPASS_UNIT* selectedUnit = ((CWndSeasonPassMain*)GetParentWnd())->m_selectedUnit;
	if (!selectedUnit)
		return;
	__SEASONPASS_PROGRESS_UNIT* progressUnit = g_pPlayer->m_SeasonpassProgress.GetProgressUnit(selectedUnit->passID);
	if (!progressUnit)
		return;

	m_selectedReward = selectedUnit->GetCurrentLevel(progressUnit->progressPoints);
	m_reachedLevel = m_selectedReward;

}
void CWndSeasonPassRewards::SetScrollPosReward(int nReward)
{
	int maxPos = m_wndScrollBar.GetMaxScrollPos();
	int minPos = m_wndScrollBar.GetMinScrollPos();


	int newScrollPos = (nReward - 2) * m_nScrollFactor;

	m_wndScrollBar.SetScrollPos(newScrollPos);
}
void CWndSeasonPassRewards::OnProgressRefresh() 
{
	__SEASONPASS_PROGRESS_UNIT* progUnit = g_pPlayer->m_SeasonpassProgress.GetProgressUnit(((CWndSeasonPassMain*)GetParentWnd())->m_selectedUnit->passID);
	if (progUnit) 
	{
		int currentLevel = ((CWndSeasonPassMain*)GetParentWnd())->m_selectedUnit->GetCurrentLevel(progUnit->progressPoints);
		if (currentLevel > m_reachedLevel) 
		{
			m_reachedLevel = currentLevel;
			m_selectedReward = currentLevel;
			SetScrollPosReward(currentLevel);
		}
	}

}
///////////////////////////
///////MAIN WINDOW/////////
///////////////////////////
CWndSeasonPassMain::CWndSeasonPassMain()
{
	m_selectedUnit = NULL;

	m_pWndRewards = NULL;

	m_pWndStaticPassBox = NULL;

	m_pWndButtonClaim = NULL;
	m_pWndButtonUpgrade = NULL;

	m_pveSelected = TRUE;

	m_pTexturePvESelect = NULL;
	m_pTexturePvPSelect = NULL;

	m_pTextureItemPreviewBox = NULL;

	m_pTextureItemPreviewField[0] = NULL;
	m_pTextureItemPreviewField[1] = NULL;

	m_pTextureSelReward = NULL;



	m_rectPvESelect = CRect(0, 0, 0, 0);
	m_rectPvPSelect = CRect(0, 0, 0, 0);

	m_rectCountdown = CRect(0, 0, 0, 0);
	m_rectLevelProg = CRect(0, 0, 0, 0);
	m_rectProgBar = CRect(0, 0, 0, 0);
	m_rectItemPreview = CRect(0, 0, 0, 0);
	m_rectItemDescr = CRect(0, 0, 0, 0);
}
CWndSeasonPassMain::~CWndSeasonPassMain()
{

}
void CWndSeasonPassMain::OnDraw(C2DRender* p2DRender)
{
	if (!m_selectedUnit)
		return;		

	__SEASONPASS_PROGRESS_UNIT* progUnit = g_pPlayer->m_SeasonpassProgress.GetProgressUnit(m_selectedUnit->passID);
	if (!progUnit)
		return;

	CD3DFont* oldFont = p2DRender->GetFont();
	CString strTmp;
	CEditString strEditOut;

	int maxProgBarLength = m_rectProgBar.Width() - 4;
	int nSelectedLevel = 0;
	if (m_pWndRewards)
		nSelectedLevel = m_pWndRewards->m_selectedReward;

	__SEASONPASS_REWARD* rewardUnit = &m_selectedUnit->rewards[nSelectedLevel];
	if (!rewardUnit)
		return;

	int neededLevelPoints = m_selectedUnit->rewards[nSelectedLevel].passPointsNeeded;
	int hasLevelPoints = progUnit->progressPoints - m_selectedUnit->rewards[nSelectedLevel].pointsToReach;
	if (hasLevelPoints > neededLevelPoints)
		hasLevelPoints = neededLevelPoints;
	else if (hasLevelPoints < 0)
		hasLevelPoints = 0;




	if (hasLevelPoints < neededLevelPoints || (!progUnit->boughtPremium && !rewardUnit->IsFreeReward()))
	{
		m_pWndButtonClaim->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, _T("Button_Claim2.tga")), TRUE);
	}
	else 
	{
		m_pWndButtonClaim->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, _T("Button_Claim.tga")), TRUE);
	}

	if (progUnit->IsClaimedReward(nSelectedLevel) || hasLevelPoints < neededLevelPoints || (!progUnit->boughtPremium && !rewardUnit->IsFreeReward()) )
	{
		m_pWndButtonClaim->EnableWindow(FALSE);
	}
	else 
	{
		m_pWndButtonClaim->EnableWindow(TRUE);
	}

	float progPercent = (float)hasLevelPoints / (float)neededLevelPoints;

	if (m_pTexturePvESelect) 
	{
		p2DRender->RenderTexture(m_rectPvESelect.TopLeft(), m_pTexturePvESelect);
	}
	if (m_pTexturePvPSelect)
	{
		p2DRender->RenderTexture(m_rectPvPSelect.TopLeft(), m_pTexturePvPSelect);
	}

	

	if (progUnit->boughtPremium)
	{
		m_pWndButtonUpgrade->SetVisible(FALSE);
	}
	else
	{
		m_pWndButtonUpgrade->SetVisible(TRUE);
	}

	if (m_pTextureProgBar[0])
	{
		p2DRender->RenderTexture(m_rectProgBar.TopLeft(), m_pTextureProgBar[0]);

		if (m_pTextureProgBar[1])
		{
			m_pTextureProgBar[1]->m_size.cx = maxProgBarLength * progPercent;
			p2DRender->RenderTexture(CPoint(m_rectProgBar.left + 2, m_rectProgBar.top + 2), m_pTextureProgBar[1]);
		}
	}

	if (m_pTextureItemPreviewBox) 
	{
		p2DRender->RenderTexture(m_rectItemPreview.TopLeft(), m_pTextureItemPreviewBox);
	}

	if (m_pWndRewards) 
	{
		if (m_pWndRewards->m_selectedReward >= 0 && m_pWndRewards->m_selectedReward < m_selectedUnit->rewards.size()) 
		{
			if (m_selectedUnit->rewards[m_pWndRewards->m_selectedReward].IsFreeReward()) 
			{
				if (m_pTextureItemPreviewField[0]) 
				{
					p2DRender->RenderTexture(m_rectItemDescr.TopLeft(), m_pTextureItemPreviewField[0]);
				}
			}
			else 
			{
				if (m_pTextureItemPreviewField[1])
				{
					p2DRender->RenderTexture(m_rectItemDescr.TopLeft(), m_pTextureItemPreviewField[1]);
				}
			}

			m_pTextureSelReward = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, prj.GetItemProp(m_selectedUnit->rewards[m_pWndRewards->m_selectedReward].itemID)->szIcon), COLOR_ITEM_BG);
			if (m_pTextureSelReward) 
			{
				p2DRender->RenderTexture(CPoint(m_rectItemPreview.CenterPoint().x - (m_pTextureSelReward->m_size.cx / 2), m_rectItemPreview.CenterPoint().y - (m_pTextureSelReward->m_size.cy / 2)), m_pTextureSelReward);
			}
		}
	}

	//countdown
	time_t timeCountdown = m_selectedUnit->GetTimeRemaining();
	int days = 0;
	int hours = 0;
	int minutes = 0;
	int seconds = 0;
	if (timeCountdown == 0) 
	{
		strTmp.Format("The event is over!");
	}
	else if (timeCountdown > 2*(60 * 60 * 24)) 
	{
		days = timeCountdown / (60 * 60 * 24);
		strTmp.Format("%d days left", days);
	}
	else if (timeCountdown > 60 * 60 * 24)
	{
		days = timeCountdown / (60 * 60 * 24);
		hours = (timeCountdown - (days * 60 * 60 * 24)) / (60 * 60);
		strTmp.Format("%d day %d hours left", days, hours);
	}
	else if (timeCountdown > 60*60)
	{
		hours = timeCountdown / (60 * 60);
		minutes = (timeCountdown - (hours * 60 * 60)) / 60;
		strTmp.Format("%d hours %d minutes left", hours, minutes);
	}
	else if (timeCountdown > 60)
	{
		minutes = timeCountdown / 60;
		seconds = (timeCountdown - (minutes * 60));
		strTmp.Format("%d minutes %d seconds left", minutes, seconds);
	}
	else 
	{
		strTmp.Format("%d seconds left", timeCountdown);
	}	
	p2DRender->SetFont(m_Theme.m_pFontSeasonPassCountdown);
	strEditOut.SetParsingString(strTmp);
	strEditOut.SetColor(D3DCOLOR_ARGB(255, 217, 182, 113));
	p2DRender->TextOut_EditString(m_rectCountdown.left, m_rectCountdown.CenterPoint().y - (p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy / 2), strEditOut, 0, 0, 2);
	
	//~countdown

	
	if (m_pWndRewards) 
	{
		//level prog
		p2DRender->SetFont(m_Theme.m_pFontSeasonPassProgLevel);
		strEditOut.Init(m_Theme.m_pFontSeasonPassProgLevel, &m_rectLevelProg);
		strTmp.Format("Lv.%d", nSelectedLevel);
		strEditOut.SetParsingString(strTmp);		
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 240, 230, 210));
		p2DRender->TextOut_EditString(m_rectLevelProg.left, m_rectLevelProg.bottom - (p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy), strEditOut, 0, 0, 2);



		p2DRender->SetFont(m_Theme.m_pFontSeasonPassProg);
		strEditOut.Init(m_Theme.m_pFontSeasonPassProg, &m_rectLevelProg);
		strTmp.Format("%d / %d", hasLevelPoints, neededLevelPoints);
		CString strHasPoints, strNeededPoints;
		strHasPoints.Format("%d", hasLevelPoints);
		strNeededPoints.Format("%d", neededLevelPoints);
		int lengthDiff = strNeededPoints.GetLength() - strHasPoints.GetLength();		
		strEditOut.SetParsingString(strTmp);
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 240, 230, 210));
		strEditOut.SetColor(0, strHasPoints.GetLength(), D3DCOLOR_ARGB(255, 8, 153, 185));
		p2DRender->TextOut_EditString(m_rectLevelProg.CenterPoint().x + (p2DRender->GetFont()->GetTextExtent("0").cx * lengthDiff), m_rectLevelProg.bottom - (p2DRender->GetFont()->GetTextExtent(strTmp.GetString()).cy), strEditOut, 0, 0, 2);
		//~level prog


		//item descr
		CRect tmpRect = m_rectItemDescr;
		tmpRect.right -= 10;

		p2DRender->SetFont(m_Theme.m_pFontSeasonPassDescrHeader);
		strEditOut.Init(m_Theme.m_pFontSeasonPassDescrHeader, &tmpRect);
		strTmp.Format("%s", prj.GetItemProp(m_selectedUnit->rewards[m_pWndRewards->m_selectedReward].itemID)->szName );
		strEditOut.SetParsingString(strTmp);
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 240, 230, 210));
		p2DRender->TextOut_EditString(tmpRect.left + 85, tmpRect.top, strEditOut, 0, 0, 2);

		p2DRender->SetFont(m_Theme.m_pFontSeasonPassDescrLevel);
		strEditOut.Init(m_Theme.m_pFontSeasonPassDescrLevel, &tmpRect);
		strTmp.Format("Level %d", nSelectedLevel);
		strEditOut.SetParsingString(strTmp);
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 200, 155, 60));
		p2DRender->TextOut_EditString(tmpRect.left + 5, tmpRect.top + 15, strEditOut, 0, 0, 2);

		p2DRender->SetFont(m_Theme.m_pFontSeasonPassDescr);
		strEditOut.Init(m_Theme.m_pFontSeasonPassDescr, &tmpRect);
		strTmp.Format("%s", prj.GetItemProp(m_selectedUnit->rewards[m_pWndRewards->m_selectedReward].itemID)->szCommand);
		strEditOut.SetParsingString(strTmp);
		strEditOut.SetColor(D3DCOLOR_ARGB(255, 240, 230, 210));
		p2DRender->TextOut_EditString(tmpRect.left + 5, tmpRect.top + 28, strEditOut, 0, 2, -4);

		//~itemdescr
	}
	p2DRender->SetFont(oldFont);
}
void CWndSeasonPassMain::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	//Move Window to center
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();

	m_pWndStaticPassBox = (CWndStatic*)GetDlgItem(WIDC_STATIC1);

	m_pWndButtonClaim = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	m_pWndButtonUpgrade = (CWndButton*)GetDlgItem(WIDC_BUTTON2);

	m_pWndRewards = new CWndSeasonPassRewards;
	m_pWndRewards->Create(WBS_HSCROLL | WBS_CHILD | WBS_NOFOCUS | WBS_NOFRAME | WBS_NODRAWFRAME, m_pWndStaticPassBox->GetWndRect(), this, APP_SEASON_PASS_REWARDS);
	
	CWndButton* pWndButtExit = m_wndTitleBar.GetTitleBarButton(WTBID_CLOSE);

	if (pWndButtExit) 
	{
		CRect titleBarRect = m_wndTitleBar.GetWindowRect();
		CRect buttonRect = pWndButtExit->GetWndRect();

		titleBarRect.top = 0;
		titleBarRect.right += 10;
		titleBarRect.bottom += 10;
		m_wndTitleBar.SetWndRect(titleBarRect);

		pWndButtExit->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "ButtChatExit.tga"), TRUE);
		pWndButtExit->FitTextureSize();
		pWndButtExit->Move(CPoint(titleBarRect.left, titleBarRect.top));
	}
		
	m_rectPvESelect = CRect(650, 8, 680, 38);
	m_rectPvPSelect = CRect(650, 45, 680, 75);

	m_rectCountdown = CRect(65, 35, 250, 62);
	m_rectLevelProg = CRect(20, 290, 200, 320);
	m_rectProgBar = CRect(20, 320, 223, 328);
	m_rectItemPreview = CRect(300, 15, 347, 62);
	m_rectItemDescr = CRect(360, 5, 620, 65);


	m_pTextureProgBar[0] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Box_EXP_Clean.png"), COLOR_ITEM_BG, TRUE);
	m_pTextureProgBar[1] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Box_Progressbar_Mid02.png"), COLOR_ITEM_BG, TRUE);

	m_pTextureItemPreviewBox = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Box_Item_Preview.png"), COLOR_ITEM_BG, TRUE);

	m_pTextureItemPreviewField[0] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Box_Item_Description_Free.png"), COLOR_ITEM_BG, TRUE);
	m_pTextureItemPreviewField[1] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Box_Item_Description.png"), COLOR_ITEM_BG, TRUE);	

	SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, _T("SeasonPassMainWnd.png")), TRUE);

	m_pWndButtonClaim->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, _T("Button_Claim.tga")), TRUE);

	m_pWndButtonUpgrade->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, _T("Button_Upgrade.tga")), TRUE);

	g_DPlay.SendLoadSeasonPassProgress();

	__SEASONPASS_UNIT* pveUnit = CAegonSeasonPassMng::GetInstance()->GetCurrentUnit(TRUE);
	__SEASONPASS_UNIT* pvpUnit = CAegonSeasonPassMng::GetInstance()->GetCurrentUnit(FALSE);

	if((pveUnit && pveUnit->timeEnd == 0) || (pvpUnit && pvpUnit->timeEnd == 0))
		g_DPlay.SendGetSeasonPassCountdown();

	OnSetPassType(TRUE);

	CRect rectTitleBar = m_wndTitleBar.GetWndRect();
	rectTitleBar.top = 0;
	rectTitleBar.bottom = 20;
	m_wndTitleBar.SetWndRect(rectTitleBar);

}
BOOL CWndSeasonPassMain::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SEASON_PASS, 0, CPoint(0, 0), pWndParent);
}
void CWndSeasonPassMain::OnDestroy()
{
}
BOOL CWndSeasonPassMain::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndSeasonPassMain::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndSeasonPassMain::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndSeasonPassMain::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_rectPvESelect.PtInRect(point)) 
	{
		OnSetPassType(TRUE);
	}
	else if (m_rectPvPSelect.PtInRect(point)) 
	{
		OnSetPassType(FALSE);
	}
}
BOOL CWndSeasonPassMain::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_BUTTON1: //claim
	{
		if (!m_selectedUnit)
			break;


		__SEASONPASS_PROGRESS_UNIT* progUnit = g_pPlayer->m_SeasonpassProgress.GetProgressUnit(m_selectedUnit->passID);
		if (!progUnit)
			break;

		if (m_pWndRewards->m_selectedReward >= m_selectedUnit->rewards.size())
			break;
	
		if (progUnit->progressPoints < m_selectedUnit->rewards[m_pWndRewards->m_selectedReward].GetTotalPointsToFinish())
			break;
		
		g_DPlay.SendClaimSeasonPassReward(m_selectedUnit->passID, m_selectedUnit->rewards[m_pWndRewards->m_selectedReward].rewardID);
		break;
	}
	case WIDC_BUTTON2: //upgrade
	{
		if (!m_selectedUnit)
			break;

		__SEASONPASS_PROGRESS_UNIT* progUnit = g_pPlayer->m_SeasonpassProgress.GetProgressUnit(m_selectedUnit->passID);
		if (!progUnit)
			break;
		if (progUnit->boughtPremium == 0) 
		{

			CWndIngameDPPurchaseConfirm* pWndConfirmPurchase = g_WndMng.OpenDPConfirmPurchase();
			if (pWndConfirmPurchase)
			{
				pWndConfirmPurchase->m_nCommand = INGAMEDP_BUYSEASONPASS;
				pWndConfirmPurchase->m_bPvE = m_selectedUnit->IsPvEPass();
				pWndConfirmPurchase->m_nPassID = m_selectedUnit->passID;

				CString strConfirmPurchase;
				strConfirmPurchase.Format("Do you want to purchase this Season Pass for %d DP?", m_selectedUnit->nPriceDP);
				pWndConfirmPurchase->SetInfo("", m_selectedUnit->nPriceDP);
			}


			
		}
			
		break;
	}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndSeasonPassMain::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}
BOOL CWndSeasonPassMain::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	return TRUE;
}
BOOL CWndSeasonPassMain::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_pWndRewards)
	{
		return m_pWndRewards->OnMouseWheel(nFlags, zDelta, pt);
	}
	return TRUE;
}
void CWndSeasonPassMain::OnMouseWndSurface(CPoint point)
{
	if (m_selectedUnit && m_pWndRewards && m_rectItemPreview.PtInRect(point))
	{
		if (m_pWndRewards->m_selectedReward < 0 || m_pWndRewards->m_selectedReward >= m_selectedUnit->rewards.size())
			return;

		CPoint pointMouse = point;
		CRect rect = m_rectItemPreview;

		CItemElem itemElem;
		itemElem.m_dwItemId = m_selectedUnit->rewards[m_pWndRewards->m_selectedReward].itemID;
		itemElem.m_nItemNum = m_selectedUnit->rewards[m_pWndRewards->m_selectedReward].itemNum;

		ClientToScreen(&pointMouse);
		ClientToScreen(&rect);
		g_WndMng.PutToolTip_Item(&itemElem, pointMouse, &rect);
	}
}

void CWndSeasonPassMain::OnSetPassType(BOOL bPvE) 
{		
	m_pveSelected = bPvE;

	m_selectedUnit = CAegonSeasonPassMng::GetInstance()->GetCurrentUnit(m_pveSelected);

	if (bPvE) 
	{
		m_pTexturePvESelect = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Button_PvE_Normal.png"), COLOR_ITEM_BG, TRUE);
		m_pTexturePvPSelect = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Button_PvP_Press.png"), COLOR_ITEM_BG, TRUE);
	}
	else 
	{
		m_pTexturePvESelect = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Button_PvE_Press.png"), COLOR_ITEM_BG, TRUE);
		m_pTexturePvPSelect = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME_SEASON_PASS, "Button_PvP_Normal.png"), COLOR_ITEM_BG, TRUE);
	}

	if(m_pWndRewards)
		m_pWndRewards->OnSelectedPassChanged();
}

void CWndSeasonPassMain::OnProgressRefresh() 
{
	if (m_pWndRewards)
		m_pWndRewards->OnProgressRefresh();
}
#endif // __AEGON_SEASON_PASS