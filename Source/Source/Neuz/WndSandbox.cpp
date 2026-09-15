#include "stdafx.h"

#ifdef __SYS_SANDBOX
#include "resdata.h"
#include "DPClient.h"
#include "defineObj.h"
extern CDPClient g_DPlay;
CWndSandbox::CWndSandbox()
{

}
CWndSandbox::~CWndSandbox()
{

}
void CWndSandbox::FillCombo()
{
	if (m_pWndComboBox)
	{
		const static vector<DWORD> blacklist = {
		MI_BRIGADIER, MI_LIEUTENANT, MI_SPIKETAIL, MI_GLYPHAXZ, MI_EVENT01, MI_EVENT02, MI_EVENT03, MI_EVENT04, MI_IMP01, MI_IMP02, MI_IMP03,
		MI_CAITSITH04_1, MI_HARPY04_1, MI_POLEVIK04_1, MI_ABRAXAS04_1, MI_HAG04_1, MI_THOTH04_1, MI_KHNEMU04_1, MI_DANTALIAN04_1, MI_GANESA04_1,
		MI_ASURA04_1, MI_DREADSTONE01, MI_DREADSTONE02, MI_DREADSTONE03, MI_DREADSTONE04, MI_DREADSTONE05, MI_DREADSTONE06, MI_RUSTIACRASHGATE01,
		MI_RUSTIACRASHGATE02, MI_RUSTIACRASHGATE03, MI_RUSTIACRASHGATE04, MI_BEHESTATUE01, MI_KALGASEGG01, MI_KALGASSTELE01, MI_KALGASSTELE02,
		MI_CLOCKWORK1, MI_LOADCLOCKWORK, MI_RANGDA01, MI_RANGDA02, MI_RANGDA03, MI_RANGDA04};

		CString strMover;

		for (int i = 0; i < prj.m_nMoverPropSize; i++)
		{
			MoverProp* pMoverProp = prj.m_pPropMover + i;

			if (pMoverProp->dwID == 0)
				continue;
			if (pMoverProp->dwClass != RANK_SUPER)
				continue;

			DWORD dwID = pMoverProp->dwID;
			bool bValid = true;

			if (std::find(blacklist.begin(), blacklist.end(), pMoverProp->dwID) != blacklist.end())
			{
				continue;
			}

			if (bValid)
			{
				strMover.Format("%s [Lv. %d]", pMoverProp->szName, pMoverProp->dwLevel);
				m_pWndComboBox->AddString(strMover);
			}
		}
	}
}
BOOL CWndSandbox::Initialize(CWndBase* pWndParent, DWORD nType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SANDBOX, 0, CPoint(0, 0), pWndParent);
}
void CWndSandbox::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	if (IsInvalidObj(g_pPlayer) || !g_pPlayer->IsWorldSandBox())
		Destroy();

	CWndButton* pWndButton[2] = { nullptr, };
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_RADIO1);
	pWndButton[1] = (CWndButton*)GetDlgItem(WIDC_RADIO10);

	pWndButton[0]->SetGroup(TRUE);
	pWndButton[0]->SetCheck(TRUE);

	m_pWndComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	FillCombo();
	m_pWndComboBox->SetCurSel(0);
	m_pWndComboBox->AddWndStyle(EBS_READONLY);

	MoveParentCenter();
}
BOOL CWndSandbox::Process()
{
	if (IsInvalidObj(g_pPlayer) || !g_pPlayer->IsWorldSandBox())
		Destroy();
	return TRUE;
}
BOOL CWndSandbox::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_BUTTON1:
		CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_RADIO1);
		g_DPlay.SendSandboxSpawn(m_pWndComboBox->GetCurSel(), pWndButton->GetCheck());
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#endif //__SYS_SANDBOX