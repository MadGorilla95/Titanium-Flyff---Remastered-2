#include "stdafx.h"
#include "resData.h"
#include "WndAegonSystemShout.h"
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

#ifdef __AEGON_ADVANCED_ANNOUNCEMENT


///////////////////////////
///////MAIN WINDOW/////////
///////////////////////////
CWndAegonSystemShout::CWndAegonSystemShout()
{
	m_pWndTextField = NULL;
	m_pWndComboBoxSelectImage = NULL;
	m_pWndComboBoxSelectFont = NULL;
	m_pWndStaticComboBoxHeader = NULL;
	m_pWndStaticPreview = NULL;
	m_pWndCheckName = NULL;
	m_pWndCheckAuth = NULL;

	m_bgTexture = NULL;
	m_texResizeFactor = 1.0f;
}
CWndAegonSystemShout::~CWndAegonSystemShout()
{

}
void CWndAegonSystemShout::OnDraw(C2DRender* p2DRender)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	if (m_pWndStaticPreview) 
	{
		p2DRender->RenderRect(m_pWndStaticPreview->GetWndRect(), COLOR_BLACK);
		p2DRender->RenderFillRect(m_pWndStaticPreview->GetWndRect(), COLOR_BLACK);


		if (m_bgTexture)
		{
			p2DRender->RenderTexture(m_pWndStaticPreview->GetWndRect().TopLeft(), m_bgTexture, 255, m_texResizeFactor, m_texResizeFactor);
		}

		if (m_CaptionFont)
		{
			CD3DFont* oldFont = p2DRender->GetFont();
			p2DRender->SetFont(m_CaptionFont);

			BOOL bShowName = m_pWndCheckName->GetCheck() == 0 ? FALSE : TRUE;
			BOOL bShowAuth = m_pWndCheckAuth->GetCheck() == 0 ? FALSE : TRUE;

			if (!g_pPlayer->IsAuthHigher(AUTH_OPERATOR))
				bShowName = TRUE;

			CString completeString;

			if (bShowName && bShowAuth) 
			{
				completeString.Format("[%s] [%s] %s", g_pPlayer->GetAuthString().GetString(), g_pPlayer->GetName(), m_pWndTextField->GetString());
			}
			else if (bShowName) 
			{
				completeString.Format("[%s] %s", g_pPlayer->GetName(), m_pWndTextField->GetString());
			}
			else if (bShowAuth)
			{
				completeString.Format("[%s] %s", g_pPlayer->GetAuthString().GetString(), m_pWndTextField->GetString());
			}
			else 
			{
				completeString.Format("%s", m_pWndTextField->GetString());
			}

			CPoint point = m_pWndStaticPreview->GetWndRect().TopLeft();
			if (m_bgTexture) 
			{
				point.x += ((m_bgTexture->m_size.cx / 2.0f) * m_texResizeFactor);
			}
			else 
			{
				point.x += m_pWndStaticPreview->GetWndRect().Width() / 2;
			}

			point.x -= (p2DRender->GetFont()->GetTextExtent(completeString.GetString()).cx / 2.0f)* m_texResizeFactor;
			
			point.y = m_pWndStaticPreview->GetWndRect().top;

			if (m_bgTexture) 
			{
				
				point.y += (m_bgTexture->m_size.cy / 2) * m_texResizeFactor;
			}
			
			p2DRender->TextOutA(point.x, point.y, m_texResizeFactor, m_texResizeFactor, completeString.GetString());

			p2DRender->SetFont(oldFont);
		}
	}
}
void CWndAegonSystemShout::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	//Move Window to center
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();


	m_pWndTextField = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	m_pWndComboBoxSelectImage = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	m_pWndComboBoxSelectFont = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);
	m_pWndStaticComboBoxHeader = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	m_pWndStaticPreview = (CWndStatic*)GetDlgItem(WIDC_STATIC2);

	m_pWndCheckName = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	m_pWndCheckAuth = (CWndButton*)GetDlgItem(WIDC_CHECK2);

	m_pWndComboBoxSelectImage->AddWndStyle(EBS_READONLY);
	m_pWndComboBoxSelectFont->AddWndStyle(EBS_READONLY);

	m_pWndCheckName->SetCheck(1);
	m_pWndCheckAuth->SetCheck(1);

	m_pWndComboBoxSelectImage->AddString("No image");
	m_pWndComboBoxSelectImage->AddString("Announcement");
	m_pWndComboBoxSelectImage->AddString("Winner");
	m_pWndComboBoxSelectImage->AddString("Notification");
	m_pWndComboBoxSelectImage->AddString("Battlefield");
	m_pWndComboBoxSelectImage->AddString("Discovery");
	m_pWndComboBoxSelectImage->AddString("Level-Up");
	
	if(m_pWndComboBoxSelectImage->m_wndListBox.GetCount() > 0)
		m_pWndComboBoxSelectImage->SetCurSel(0);

	OnImageSelChanged();


	m_pWndComboBoxSelectFont->AddString("Default");
	m_pWndComboBoxSelectFont->AddString("Zone Text");
	m_pWndComboBoxSelectFont->AddString("Zone Subtext");
	m_pWndComboBoxSelectFont->AddString("Caption1");


#ifdef __AEGON_TOURNAMENT
	m_pWndComboBoxSelectFont->AddString("tournament");
#endif // __AEGON_TOURNAMENT


	if (m_pWndComboBoxSelectFont->m_wndListBox.GetCount() > 0)
		m_pWndComboBoxSelectFont->SetCurSel(0);

	OnFontSelChanged();

}
BOOL CWndAegonSystemShout::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_AEGON_SYSTEM_SHOUT, 0, CPoint(0, 0), pWndParent);
}
void CWndAegonSystemShout::OnDestroy()
{
}
BOOL CWndAegonSystemShout::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndAegonSystemShout::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndAegonSystemShout::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndAegonSystemShout::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndAegonSystemShout::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_COMBOBOX1:
	{
		OnImageSelChanged();
		break;
	}
	case WIDC_COMBOBOX2:
	{
		OnFontSelChanged();
		break;
	}
	case WIDC_BUTTON1: //sys shout
	{
		if (m_pWndTextField) 
		{
			if (strlen(m_pWndTextField->GetString()) > 0)
				g_DPlay.SendAegonSystemShout(m_pWndTextField->GetString(), m_pWndCheckName->GetCheck() == 0 ? FALSE : TRUE, m_pWndCheckAuth->GetCheck() == 0 ? FALSE : TRUE, m_pWndComboBoxSelectImage->GetCurSel(), m_pWndComboBoxSelectFont->GetCurSel());
			else
				g_WndMng.PutString("no text entered", 0, COLOR_ERROR);
		}
			
		break;
	}
	case WIDC_BUTTON2: //cancel
	{
		Destroy();
		break;
	}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndAegonSystemShout::SetShoutString(char* szString)
{
	if (m_pWndTextField)
		m_pWndTextField->SetString(szString);
}
void CWndAegonSystemShout::OnImageSelChanged() 
{
	int nCurSel = m_pWndComboBoxSelectImage->GetCurSel();
	if (nCurSel == 0) 
	{
		m_bgTexture = NULL;
		m_texResizeFactor = 1;
		return;
	}

	m_bgTexture = g_Caption1.GetSystemImage(nCurSel);

	if (m_bgTexture)
		FitImageSize();

}
void CWndAegonSystemShout::FitImageSize() 
{
	if (m_bgTexture) 
	{
		m_texResizeFactor = 1.0f;

		int nWidth = m_pWndStaticPreview->GetWndRect().Width();
		int nHeight = m_pWndStaticPreview->GetWndRect().Height();
		if (m_bgTexture->m_size.cx > nWidth)
		{
			float tmpFloat = (float)nWidth / (float)m_bgTexture->m_size.cx;
			if (tmpFloat < m_texResizeFactor)
				m_texResizeFactor = tmpFloat;
		}	
		if (m_bgTexture->m_size.cy > nHeight)
		{
			float tmpFloat = (float)nHeight / (float)m_bgTexture->m_size.cy;
			if(tmpFloat < m_texResizeFactor)
				m_texResizeFactor = tmpFloat;
		}
	}
	else 
	{
		m_texResizeFactor = 1.0f;
	}
}

void CWndAegonSystemShout::OnFontSelChanged()
{
	m_CaptionFont = g_Caption1.GetSystemFont(m_pWndComboBoxSelectFont->GetCurSel());
}
#endif // __AEGON_ADVANCED_ANNOUNCEMENT