#include "stdafx.h"
#include "resData.h"
#include "defineText.h"
#include "DPClient.h"
extern CDPClient g_DPlay;

#ifdef __REPORT_SYSTEM
#include "WndReport.h"
CWndReport::CWndReport()
{
}

CWndReport::~CWndReport()
{
}
void CWndReport::OnDraw(C2DRender* p2DRender)
{
}

void CWndReport::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pComboKind = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	m_pComboKind->AddString(prj.GetText(TID_REPORT_TEXT_06));
	m_pComboKind->AddString(prj.GetText(TID_REPORT_TEXT_07));
	m_pComboKind->AddString(prj.GetText(TID_REPORT_TEXT_08));
	m_pComboKind->AddString(prj.GetText(TID_REPORT_TEXT_09));
	m_pComboKind->AddWndStyle(EBS_READONLY);
#ifdef __PLACEHOLDER_TEXT
	m_pComboKind->SetString("Please select a category.", COLOR_PLACEHOLDER);
#endif //__PLACEHOLDER_TEXT

	CWndEdit* pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT);
	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC3);

	//Default
	CString str;
	str.Format(prj.GetText(TID_REPORT_TEXT_14));
	pStatic->m_dwColor = COLOR_BLACK;
	pStatic->SetTitle(str);

	CRect rect = pEdit->GetWndRect();
	rect.left = 8;
	pEdit->SetWndRect(rect, TRUE);
	MoveParentCenter();
}

BOOL CWndReport::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_REPORT, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndReport::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndReport::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndReport::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndReport::OnLButtonDown(UINT nFlags, CPoint point)
{
}

BOOL CWndReport::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT);
	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC3);

	if (nID == WIDC_OK)
	{
		if (pWndEdit)
		{
			if (m_pComboKind->GetCurSel() >= 0)
			{
				if (strlen(pWndEdit->GetString()) >= 1 && strlen(pWndEdit->GetString()) <= 256)
				{
					g_DPlay.SendReport(m_pComboKind->GetCurSel(), (char*)pWndEdit->GetString());
					Destroy();
				}
				else
					g_WndMng.PutString(prj.GetText(TID_REPORT_TEXT_11), NULL, COLOR_ERROR);
			}
			else
				g_WndMng.PutString(prj.GetText(TID_REPORT_TEXT_10), NULL, COLOR_ERROR);
		}
	}
	if (nID == WIDC_EDIT)
	{
		if (pWndEdit && pStatic)
		{
			CString str;
			str.Format(prj.GetText(TID_REPORT_TEXT_12), strlen(pWndEdit->GetString()));

			if (strlen(pWndEdit->GetString()) > 256)
				pStatic->m_dwColor = COLOR_ERROR;
			else
				pStatic->m_dwColor = COLOR_BLACK;

			pStatic->SetTitle(str);
		}
	}
	else if (nID == WIDC_CANCEL)
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#endif // __REPORT_SYSTEM