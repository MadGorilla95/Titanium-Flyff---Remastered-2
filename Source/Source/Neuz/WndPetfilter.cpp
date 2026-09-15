#include "stdafx.h"
#include "WndPetfilter.h"
#include "ResData.h"
#include "defineFilter.h"
#include "DPClient.h"
extern CDPClient g_DPlay;

#ifdef __SYS_PETFILTER
//pet filter tab
CWndPetfilter::CWndPetfilter()
{
	dwPetfilter = 0;
	m_bInitialized = FALSE;
}
CWndPetfilter::~CWndPetfilter()
{
}
void CWndPetfilter::OnDraw(C2DRender* p2DRender)
{
}
void CWndPetfilter::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndComboBox* pWndComboWeapon = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	CWndComboBox* pWndComboArmor = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);

	if (pWndComboWeapon)
	{
		pWndComboWeapon->AddString("Disabled");
		pWndComboWeapon->AddString("Common");
		pWndComboWeapon->AddString("Precious");
		pWndComboWeapon->AddString("Rare");
		pWndComboWeapon->AddString("Epic");
		pWndComboWeapon->AddString("Legendary");
		pWndComboWeapon->AddString("Mystique");
	}


	if (pWndComboArmor)
	{
		pWndComboArmor->AddString("Disabled");
		pWndComboArmor->AddString("Common");
		pWndComboArmor->AddString("Precious");
		pWndComboArmor->AddString("Rare");
		pWndComboArmor->AddString("Epic");
		pWndComboArmor->AddString("Legendary");
		pWndComboArmor->AddString("Mystique");
	}	

#ifdef __AEGON_DB_PETFILTER
	DoInitCheck();
#else
	dwPetfilter = g_Option.m_dwPetfilter;
#endif

	SetButtonStatus();
	MoveParentCenter();
}
void CWndPetfilter::SetButtonStatus()
{
	if (!DoInitCheck())
		return;
	CWndButton* pWndCheck1 = (CWndButton*)GetDlgItem(WIDC_PF_HPFOOD);
	CWndButton* pWndCheck2 = (CWndButton*)GetDlgItem(WIDC_PF_MPFOOD);
	CWndButton* pWndCheck3 = (CWndButton*)GetDlgItem(WIDC_PF_FPFOOD);
	CWndButton* pWndCheck4 = (CWndButton*)GetDlgItem(WIDC_PF_BWEAPON);
	CWndButton* pWndCheck5 = (CWndButton*)GetDlgItem(WIDC_PF_GWEAPON);
	CWndButton* pWndCheck6 = (CWndButton*)GetDlgItem(WIDC_PF_BSET);
	CWndButton* pWndCheck7 = (CWndButton*)GetDlgItem(WIDC_PF_GSET);
	CWndButton* pWndCheck8 = (CWndButton*)GetDlgItem(WIDC_PF_NECKLACE);
	CWndButton* pWndCheck9 = (CWndButton*)GetDlgItem(WIDC_PF_EARRING);
	CWndButton* pWndCheck10 = (CWndButton*)GetDlgItem(WIDC_PF_RING);
	CWndButton* pWndCheck11 = (CWndButton*)GetDlgItem(WIDC_PF_EGG);
	CWndButton* pWndCheck12 = (CWndButton*)GetDlgItem(WIDC_PF_SUNSTONE);
	CWndButton* pWndCheck13 = (CWndButton*)GetDlgItem(WIDC_PF_MOONSTONE);
	CWndButton* pWndCheck14 = (CWndButton*)GetDlgItem(WIDC_PF_QUESTS);
	CWndButton* pWndCheck15 = (CWndButton*)GetDlgItem(WIDC_PF_ELECARD);
	CWndButton* pWndCheck16 = (CWndButton*)GetDlgItem(WIDC_PF_4PCARD);
	CWndButton* pWndCheck17 = (CWndButton*)GetDlgItem(WIDC_PF_7PCARD);
	CWndButton* pWndCheck18 = (CWndButton*)GetDlgItem(WIDC_PF_BPIECARD);
	CWndButton* pWndCheck19 = (CWndButton*)GetDlgItem(WIDC_PF_APIECARD);
	CWndButton* pWndCheck20 = (CWndButton*)GetDlgItem(WIDC_CHECK);
	CWndButton* pWndCheck21 = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	CWndButton* pWndCheck22 = (CWndButton*)GetDlgItem(WIDC_CHECK2); //crafting mat
	CWndButton* pWndCheck23 = (CWndButton*)GetDlgItem(WIDC_CHECK3); //weapon other classes
	CWndButton* pWndCheck24 = (CWndButton*)GetDlgItem(WIDC_CHECK4); //armor other classes
	CWndButton* pWndCheck25 = (CWndButton*)GetDlgItem(WIDC_CHECK5); //ultimate weapon
	CWndButton* pWndCheck26 = (CWndButton*)GetDlgItem(WIDC_CHECK6); //jewels

	CWndComboBox* pWndComboWeapon = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	CWndComboBox* pWndComboArmor = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);



	if (dwPetfilter & FILTER_HPFOOD)	{ pWndCheck1->SetCheck(TRUE); } else { pWndCheck1->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_MPFOOD)	{ pWndCheck2->SetCheck(TRUE); } else { pWndCheck2->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_FPFOOD)	{ pWndCheck3->SetCheck(TRUE); } else { pWndCheck3->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_BWEAPON)	{ pWndCheck4->SetCheck(TRUE); } else { pWndCheck4->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_GWEAPON)	{ pWndCheck5->SetCheck(TRUE); } else { pWndCheck5->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_BSET)		{ pWndCheck6->SetCheck(TRUE); } else { pWndCheck6->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_GSET)		{ pWndCheck7->SetCheck(TRUE); } else { pWndCheck7->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_NECKLACE)	{ pWndCheck8->SetCheck(TRUE); } else { pWndCheck8->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_EARRING)	{ pWndCheck9->SetCheck(TRUE); } else { pWndCheck9->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_RING)		{ pWndCheck10->SetCheck(TRUE); } else { pWndCheck10->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_EGG)		{ pWndCheck11->SetCheck(TRUE); } else { pWndCheck11->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_SUNSTONE)	{ pWndCheck12->SetCheck(TRUE); } else { pWndCheck12->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_MOONSTONE) { pWndCheck13->SetCheck(TRUE); } else { pWndCheck13->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_QUESTITEM) { pWndCheck14->SetCheck(TRUE); } else { pWndCheck14->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_ELECARDS)	{ pWndCheck15->SetCheck(TRUE); } else { pWndCheck15->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_4PCARD)	{ pWndCheck16->SetCheck(TRUE); } else { pWndCheck16->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_7PCARD)	{ pWndCheck17->SetCheck(TRUE); } else { pWndCheck17->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_BPIECARD)	{ pWndCheck18->SetCheck(TRUE); } else { pWndCheck18->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_APIECARD)	{ pWndCheck19->SetCheck(TRUE); } else { pWndCheck19->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_MALE)		{ pWndCheck20->SetCheck(TRUE); } else { pWndCheck20->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_FEMALE)	{ pWndCheck21->SetCheck(TRUE); } else { pWndCheck21->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_CRAFT_MAT) { pWndCheck22->SetCheck(TRUE); } else { pWndCheck22->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_OTHER_WEAP) { pWndCheck23->SetCheck(TRUE); } else { pWndCheck23->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_OTHER_ARM) { pWndCheck24->SetCheck(TRUE); } else { pWndCheck24->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_ULT_WEAP) { pWndCheck25->SetCheck(TRUE); } else { pWndCheck25->SetCheck(FALSE); }
	if (dwPetfilter & FILTER_JEWELS) { pWndCheck26->SetCheck(TRUE); } else { pWndCheck26->SetCheck(FALSE); }

	int nWeapRaritySel = 0;
	if (dwPetfilter & FILTER_WEAP_RAR0)
	{ 
		nWeapRaritySel = 1;
	} 
	else if(dwPetfilter & FILTER_WEAP_RAR1)
	{ 
		nWeapRaritySel = 2;
	}
	else if (dwPetfilter & FILTER_WEAP_RAR2)
	{
		nWeapRaritySel = 3;
	}
	else if (dwPetfilter & FILTER_WEAP_RAR3)
	{
		nWeapRaritySel = 4;
	}
	else if (dwPetfilter & FILTER_WEAP_RAR4)
	{
		nWeapRaritySel = 5;
	}
	else if (dwPetfilter & FILTER_WEAP_RAR5)
	{
		nWeapRaritySel = 6;
	}
	if (pWndComboWeapon)
	{
		pWndComboWeapon->SetCurSel(nWeapRaritySel);
	}


	int nArmRaritySel = 0;
	if (dwPetfilter & FILTER_ARM_RAR0)
	{
		nArmRaritySel = 1;
	}
	else if (dwPetfilter & FILTER_ARM_RAR1)
	{
		nArmRaritySel = 2;
	}
	else if (dwPetfilter & FILTER_ARM_RAR2)
	{
		nArmRaritySel = 3;
	}
	else if (dwPetfilter & FILTER_ARM_RAR3)
	{
		nArmRaritySel = 4;
	}
	else if (dwPetfilter & FILTER_ARM_RAR4)
	{
		nArmRaritySel = 5;
	}
	else if (dwPetfilter & FILTER_ARM_RAR5)
	{
		nArmRaritySel = 6;
	}
	if (pWndComboArmor)
	{
		pWndComboArmor->SetCurSel(nArmRaritySel);
	}	
}

BOOL CWndPetfilter::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PETFILTER, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndPetfilter::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndPetfilter::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndPetfilter::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndPetfilter::OnLButtonDown(UINT nFlags, CPoint point)
{
}

BOOL CWndPetfilter::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (!DoInitCheck())
		return CWndNeuz::OnChildNotify(message, nID, pLResult);

	switch (nID)
	{
		case WIDC_BUTTON1:
		{
#ifdef __AEGON_DB_PETFILTER
			if (IsValidObj(g_pPlayer)) 
			{
				g_pPlayer->m_dwPetfilter = dwPetfilter;
				g_DPlay.SendPlayerPetfilter();
			}
#else
			g_Option.m_dwPetfilter = dwPetfilter;
			g_Option.Save("Neuz.ini");
			g_DPlay.SendPlayerPetfilter(dwPetfilter);
			g_WndMng.PutString("Petfilter saved successfully.", NULL, COLOR_SUCCESS);
			Destroy();
#endif
		}
		break;
		case WIDC_BUTTON2:
		{
			AddPetFilterMode(FILTER_HPFOOD);
			AddPetFilterMode(FILTER_MPFOOD);
			AddPetFilterMode(FILTER_FPFOOD);
			AddPetFilterMode(FILTER_BWEAPON);
			AddPetFilterMode(FILTER_GWEAPON);
			AddPetFilterMode(FILTER_BSET);
			AddPetFilterMode(FILTER_GSET);
			AddPetFilterMode(FILTER_NECKLACE);
			AddPetFilterMode(FILTER_EARRING);
			AddPetFilterMode(FILTER_RING);
			AddPetFilterMode(FILTER_EGG);
			AddPetFilterMode(FILTER_SUNSTONE);
			AddPetFilterMode(FILTER_MOONSTONE);
			AddPetFilterMode(FILTER_QUESTITEM);
			AddPetFilterMode(FILTER_ELECARDS);
			AddPetFilterMode(FILTER_4PCARD);
			AddPetFilterMode(FILTER_7PCARD);
			AddPetFilterMode(FILTER_BPIECARD);
			AddPetFilterMode(FILTER_APIECARD);
			AddPetFilterMode(FILTER_MALE);
			AddPetFilterMode(FILTER_FEMALE);
			AddPetFilterMode(FILTER_CRAFT_MAT);
			AddPetFilterMode(FILTER_OTHER_WEAP);
			AddPetFilterMode(FILTER_OTHER_ARM);
			AddPetFilterMode(FILTER_ULT_WEAP);
			AddPetFilterMode(FILTER_JEWELS);
			SetButtonStatus();
		}
		break;
		case WIDC_BUTTON3:
		{
			RemovePetFilterMode(FILTER_HPFOOD);
			RemovePetFilterMode(FILTER_MPFOOD);
			RemovePetFilterMode(FILTER_FPFOOD);
			RemovePetFilterMode(FILTER_BWEAPON);
			RemovePetFilterMode(FILTER_GWEAPON);
			RemovePetFilterMode(FILTER_BSET);
			RemovePetFilterMode(FILTER_GSET);
			RemovePetFilterMode(FILTER_NECKLACE);
			RemovePetFilterMode(FILTER_EARRING);
			RemovePetFilterMode(FILTER_RING);
			RemovePetFilterMode(FILTER_EGG);
			RemovePetFilterMode(FILTER_SUNSTONE);
			RemovePetFilterMode(FILTER_MOONSTONE);
			RemovePetFilterMode(FILTER_QUESTITEM);
			RemovePetFilterMode(FILTER_ELECARDS);
			RemovePetFilterMode(FILTER_4PCARD);
			RemovePetFilterMode(FILTER_7PCARD);
			RemovePetFilterMode(FILTER_BPIECARD);
			RemovePetFilterMode(FILTER_APIECARD);
			RemovePetFilterMode(FILTER_MALE);
			RemovePetFilterMode(FILTER_FEMALE);
			RemovePetFilterMode(FILTER_CRAFT_MAT);
			RemovePetFilterMode(FILTER_OTHER_WEAP);
			RemovePetFilterMode(FILTER_OTHER_ARM);
			RemovePetFilterMode(FILTER_ULT_WEAP);
			RemovePetFilterMode(FILTER_JEWELS);

			RemovePetFilterMode(FILTER_WEAP_RAR0);
			RemovePetFilterMode(FILTER_WEAP_RAR1);
			RemovePetFilterMode(FILTER_WEAP_RAR2);
			RemovePetFilterMode(FILTER_WEAP_RAR3);
			RemovePetFilterMode(FILTER_WEAP_RAR4);
			RemovePetFilterMode(FILTER_WEAP_RAR5);

			RemovePetFilterMode(FILTER_ARM_RAR0);
			RemovePetFilterMode(FILTER_ARM_RAR1);
			RemovePetFilterMode(FILTER_ARM_RAR2);
			RemovePetFilterMode(FILTER_ARM_RAR3);
			RemovePetFilterMode(FILTER_ARM_RAR4);
			RemovePetFilterMode(FILTER_ARM_RAR5);

			SetButtonStatus();
		}
		break;
		case WIDC_PF_HPFOOD:
		case WIDC_PF_MPFOOD:
		case WIDC_PF_FPFOOD:
		case WIDC_PF_BWEAPON:
		case WIDC_PF_GWEAPON:
		case WIDC_PF_BSET:
		case WIDC_PF_GSET:
		case WIDC_PF_NECKLACE:
		case WIDC_PF_EARRING:
		case WIDC_PF_RING:
		case WIDC_PF_EGG:
		case WIDC_PF_SUNSTONE:
		case WIDC_PF_MOONSTONE:
		case WIDC_PF_QUESTS:
		case WIDC_PF_ELECARD:
		case WIDC_PF_4PCARD:
		case WIDC_PF_7PCARD:
		case WIDC_PF_BPIECARD:
		case WIDC_PF_APIECARD:
		case WIDC_CHECK:
		case WIDC_CHECK1:
		case WIDC_CHECK2:
		case WIDC_CHECK3:
		case WIDC_CHECK4:
		case WIDC_CHECK5:
		case WIDC_CHECK6:		
		case WIDC_COMBOBOX1:
		case WIDC_COMBOBOX2:
			ApplyFilters();
			break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndPetfilter::ApplyFilters()
{
	if (!DoInitCheck())
		return;

	CWndButton* pWndCheck1 = (CWndButton*)GetDlgItem(WIDC_PF_HPFOOD);
	CWndButton* pWndCheck2 = (CWndButton*)GetDlgItem(WIDC_PF_MPFOOD);
	CWndButton* pWndCheck3 = (CWndButton*)GetDlgItem(WIDC_PF_FPFOOD);
	CWndButton* pWndCheck4 = (CWndButton*)GetDlgItem(WIDC_PF_BWEAPON);
	CWndButton* pWndCheck5 = (CWndButton*)GetDlgItem(WIDC_PF_GWEAPON);
	CWndButton* pWndCheck6 = (CWndButton*)GetDlgItem(WIDC_PF_BSET);
	CWndButton* pWndCheck7 = (CWndButton*)GetDlgItem(WIDC_PF_GSET);
	CWndButton* pWndCheck8 = (CWndButton*)GetDlgItem(WIDC_PF_NECKLACE);
	CWndButton* pWndCheck9 = (CWndButton*)GetDlgItem(WIDC_PF_EARRING);
	CWndButton* pWndCheck10 = (CWndButton*)GetDlgItem(WIDC_PF_RING);
	CWndButton* pWndCheck11 = (CWndButton*)GetDlgItem(WIDC_PF_EGG);
	CWndButton* pWndCheck12 = (CWndButton*)GetDlgItem(WIDC_PF_SUNSTONE);
	CWndButton* pWndCheck13 = (CWndButton*)GetDlgItem(WIDC_PF_MOONSTONE);
	CWndButton* pWndCheck14 = (CWndButton*)GetDlgItem(WIDC_PF_QUESTS);
	CWndButton* pWndCheck15 = (CWndButton*)GetDlgItem(WIDC_PF_ELECARD);
	CWndButton* pWndCheck16 = (CWndButton*)GetDlgItem(WIDC_PF_4PCARD);
	CWndButton* pWndCheck17 = (CWndButton*)GetDlgItem(WIDC_PF_7PCARD);
	CWndButton* pWndCheck18 = (CWndButton*)GetDlgItem(WIDC_PF_BPIECARD);
	CWndButton* pWndCheck19 = (CWndButton*)GetDlgItem(WIDC_PF_APIECARD);
	CWndButton* pWndCheck20 = (CWndButton*)GetDlgItem(WIDC_CHECK);
	CWndButton* pWndCheck21 = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	CWndButton* pWndCheck22 = (CWndButton*)GetDlgItem(WIDC_CHECK2);
	CWndButton* pWndCheck23 = (CWndButton*)GetDlgItem(WIDC_CHECK3); //weapon other classes
	CWndButton* pWndCheck24 = (CWndButton*)GetDlgItem(WIDC_CHECK4); //armor other classes
	CWndButton* pWndCheck25 = (CWndButton*)GetDlgItem(WIDC_CHECK5); //ultimate weapon
	CWndButton* pWndCheck26 = (CWndButton*)GetDlgItem(WIDC_CHECK6); //jewels

	CWndComboBox* pWndComboWeapon = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	CWndComboBox* pWndComboArmor = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);

	SetPetFilterMode(pWndCheck1->GetCheck(), FILTER_HPFOOD);
	SetPetFilterMode(pWndCheck2->GetCheck(), FILTER_MPFOOD);
	SetPetFilterMode(pWndCheck3->GetCheck(), FILTER_FPFOOD);
	SetPetFilterMode(pWndCheck4->GetCheck(), FILTER_BWEAPON);
	SetPetFilterMode(pWndCheck5->GetCheck(), FILTER_GWEAPON);
	SetPetFilterMode(pWndCheck6->GetCheck(), FILTER_BSET);
	SetPetFilterMode(pWndCheck7->GetCheck(), FILTER_GSET);
	SetPetFilterMode(pWndCheck8->GetCheck(), FILTER_NECKLACE);
	SetPetFilterMode(pWndCheck9->GetCheck(), FILTER_EARRING);
	SetPetFilterMode(pWndCheck10->GetCheck(), FILTER_RING);
	SetPetFilterMode(pWndCheck11->GetCheck(), FILTER_EGG);
	SetPetFilterMode(pWndCheck12->GetCheck(), FILTER_SUNSTONE);
	SetPetFilterMode(pWndCheck13->GetCheck(), FILTER_MOONSTONE);
	SetPetFilterMode(pWndCheck14->GetCheck(), FILTER_QUESTITEM);
	SetPetFilterMode(pWndCheck15->GetCheck(), FILTER_ELECARDS);

	SetPetFilterMode(pWndCheck16->GetCheck(), FILTER_4PCARD);
	SetPetFilterMode(pWndCheck17->GetCheck(), FILTER_7PCARD);
	SetPetFilterMode(pWndCheck18->GetCheck(), FILTER_BPIECARD);
	SetPetFilterMode(pWndCheck19->GetCheck(), FILTER_APIECARD);
	SetPetFilterMode(pWndCheck20->GetCheck(), FILTER_MALE);
	SetPetFilterMode(pWndCheck21->GetCheck(), FILTER_FEMALE);
	SetPetFilterMode(pWndCheck22->GetCheck(), FILTER_CRAFT_MAT);

	SetPetFilterMode(pWndCheck23->GetCheck(), FILTER_OTHER_WEAP);
	SetPetFilterMode(pWndCheck24->GetCheck(), FILTER_OTHER_ARM);
	SetPetFilterMode(pWndCheck25->GetCheck(), FILTER_ULT_WEAP);
	SetPetFilterMode(pWndCheck26->GetCheck(), FILTER_JEWELS);

	int nWeaponRarity = pWndComboWeapon->GetCurSel();
	RemovePetFilterMode(FILTER_WEAP_RAR0);
	RemovePetFilterMode(FILTER_WEAP_RAR1);
	RemovePetFilterMode(FILTER_WEAP_RAR2);
	RemovePetFilterMode(FILTER_WEAP_RAR3);
	RemovePetFilterMode(FILTER_WEAP_RAR4);
	RemovePetFilterMode(FILTER_WEAP_RAR5);

	switch (nWeaponRarity) 
	{
	case 1:
		AddPetFilterMode(FILTER_WEAP_RAR0);
		break;
	case 2:
		AddPetFilterMode(FILTER_WEAP_RAR1);
		break;
	case 3:
		AddPetFilterMode(FILTER_WEAP_RAR2);
		break;
	case 4:
		AddPetFilterMode(FILTER_WEAP_RAR3);
		break;
	case 5:
		AddPetFilterMode(FILTER_WEAP_RAR4);
		break;
	case 6:
		AddPetFilterMode(FILTER_WEAP_RAR5);
		break;
	case 0:
	default:
		break;
	}

	int nArmorRarity = pWndComboArmor->GetCurSel();
	RemovePetFilterMode(FILTER_ARM_RAR0);
	RemovePetFilterMode(FILTER_ARM_RAR1);
	RemovePetFilterMode(FILTER_ARM_RAR2);
	RemovePetFilterMode(FILTER_ARM_RAR3);
	RemovePetFilterMode(FILTER_ARM_RAR4);
	RemovePetFilterMode(FILTER_ARM_RAR5);

	switch (nArmorRarity)
	{
	case 1:
		AddPetFilterMode(FILTER_ARM_RAR0);
		break;
	case 2:
		AddPetFilterMode(FILTER_ARM_RAR1);
		break;
	case 3:
		AddPetFilterMode(FILTER_ARM_RAR2);
		break;
	case 4:
		AddPetFilterMode(FILTER_ARM_RAR3);
		break;
	case 5:
		AddPetFilterMode(FILTER_ARM_RAR4);
		break;
	case 6:
		AddPetFilterMode(FILTER_ARM_RAR5);
		break;
	case 0:
	default:
		break;
	}

	SetButtonStatus();
}
void CWndPetfilter::SetPetFilterMode(BOOL bAdd, LONGLONG dwMode)
{
	if (bAdd)
		AddPetFilterMode(dwMode);
	else
		RemovePetFilterMode(dwMode);
}
void CWndPetfilter::AddPetFilterMode(LONGLONG dwMode)
{
	dwPetfilter |= dwMode;
}
void CWndPetfilter::RemovePetFilterMode(LONGLONG dwMode)
{
	dwPetfilter &= ~dwMode;
}
BOOL CWndPetfilter::DoInitCheck() 
{
	if (!m_bInitialized && IsValidObj(g_pPlayer))
	{
		dwPetfilter = g_pPlayer->m_dwPetfilter;
		m_bInitialized = TRUE;
	}

	return m_bInitialized;
}

//pet filter main
CWndPetfilterMain::CWndPetfilterMain()
{

}
CWndPetfilterMain::~CWndPetfilterMain()
{
	m_pWndFilter.ApplyFilters();
	g_DPlay.SendPlayerPetfilter();
}
void CWndPetfilterMain::OnDraw(C2DRender* p2DRender)
{
}
void CWndPetfilterMain::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;

	m_pWndFilter.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_PETFILTER);

	m_pWndLists.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_PETFILTER_LISTS);

	WTCITEM tabTabItem;
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;

	tabTabItem.pszText = _T("Filters");
	tabTabItem.pWndBase = &m_pWndFilter;
	pWndTabCtrl->InsertItem(0, &tabTabItem);

	tabTabItem.pszText = _T("Lists");
	tabTabItem.pWndBase = &m_pWndLists;
	pWndTabCtrl->InsertItem(1, &tabTabItem);

	MoveParentCenter();
}

BOOL CWndPetfilterMain::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PETFILTER_MAIN, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndPetfilterMain::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndPetfilterMain::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndPetfilterMain::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndPetfilterMain::OnLButtonDown(UINT nFlags, CPoint point)
{
}

BOOL CWndPetfilterMain::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	
	switch (nID)
	{
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndPetfilterMain::OnAddItemToFilterList(DWORD dwItemID)
{
	m_pWndLists.OnAddItemToFilterList(dwItemID);
}
//pet filter lists
CWndPetfilterLists::CWndPetfilterLists()
{
	m_pWndWhitelistItems = NULL;
	m_pWndBlacklistItems = NULL;
	m_bAddToWhitelist = TRUE;
}
CWndPetfilterLists::~CWndPetfilterLists()
{
}
void CWndPetfilterLists::OnDraw(C2DRender* p2DRender)
{
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	CRect rect = GetWndRect();
	if (m_pWndWhitelistItems && m_pWndBlacklistItems) 
	{
		int xPos = m_pWndWhitelistItems->GetWndRect().right + (m_pWndBlacklistItems->GetWndRect().left - m_pWndWhitelistItems->GetWndRect().right) / 2;
		p2DRender->RenderLine(CPoint(xPos, rect.top), CPoint(xPos, rect.bottom), dwColorText);
	}

	if (m_pWndWhitelistItems && !m_pWndWhitelistItems->m_pItemList && IsValidObj(g_pPlayer))
	{
		m_pWndWhitelistItems->m_pItemList = &g_pPlayer->m_sPetfilterLists.sWhitelist;
	}
	if (m_pWndBlacklistItems && !m_pWndBlacklistItems->m_pItemList && IsValidObj(g_pPlayer))
	{
		m_pWndBlacklistItems->m_pItemList = &g_pPlayer->m_sPetfilterLists.sBlacklist;
	}
}
void CWndPetfilterLists::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	LPWNDCTRL whitelistCtrl = GetWndCtrl(WIDC_CUSTOM1);
	if (whitelistCtrl)
	{
		SAFE_DELETE(m_pWndWhitelistItems);
		m_pWndWhitelistItems = new CWndPetfilterItemList;
		m_pWndWhitelistItems->m_byWndType = WTYPE_LISTBOX;
		m_pWndWhitelistItems->m_bTile = true;
		m_pWndWhitelistItems->m_bVisible = true;
		m_pWndWhitelistItems->m_strTexture = "WndEditTile00.tga";
		m_pWndWhitelistItems->Create(WBS_CHILD | WBS_VSCROLL, whitelistCtrl->rect, this, WIDC_LISTBOX1);
	}

	LPWNDCTRL blacklistCtrl = GetWndCtrl(WIDC_CUSTOM2);
	if (blacklistCtrl)
	{
		SAFE_DELETE(m_pWndBlacklistItems);
		m_pWndBlacklistItems = new CWndPetfilterItemList;
		m_pWndBlacklistItems->m_byWndType = WTYPE_LISTBOX;
		m_pWndBlacklistItems->m_bTile = true;
		m_pWndBlacklistItems->m_bVisible = true;
		m_pWndBlacklistItems->m_strTexture = "WndEditTile00.tga";
		m_pWndBlacklistItems->Create(WBS_CHILD | WBS_VSCROLL, blacklistCtrl->rect, this, WIDC_LISTBOX2);
	}

	//AdjustWndBase();

	MoveParentCenter();
}

BOOL CWndPetfilterLists::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PETFILTER_MAIN, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndPetfilterLists::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndPetfilterLists::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndPetfilterLists::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndPetfilterLists::OnLButtonDown(UINT nFlags, CPoint point)
{
}

BOOL CWndPetfilterLists::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{

	switch (nID)
	{
	case WIDC_BUTTON1: //Whitelist add
	case WIDC_BUTTON3: //Blacklist add
		{
			if (nID == WIDC_BUTTON1)
				m_bAddToWhitelist = TRUE;
			else
				m_bAddToWhitelist = FALSE;

			CWndBase* pWndBaseItemWiki = g_WndMng.GetApplet(APP_WIKI_ITEMS);
			if (!pWndBaseItemWiki)
			{
				g_WndMng.CreateApplet(APP_WIKI_ITEMS);
			}
		}
		break;
	case WIDC_BUTTON2: //Whitelist Delete
		if (m_pWndWhitelistItems) 
		{
			if (IsValidObj(g_pPlayer) && m_pWndWhitelistItems->m_nSelectedItem >= 0 && m_pWndWhitelistItems->m_nSelectedItem < g_pPlayer->m_sPetfilterLists.sWhitelist.m_vdwItemList.size())
			{
				g_pPlayer->m_sPetfilterLists.sWhitelist.m_vdwItemList.erase(g_pPlayer->m_sPetfilterLists.sWhitelist.m_vdwItemList.begin() + m_pWndWhitelistItems->m_nSelectedItem);
			}
		}
		break;
	case WIDC_BUTTON4: //Blacklist Delete
		if (m_pWndBlacklistItems)
		{
			if (IsValidObj(g_pPlayer) && m_pWndBlacklistItems->m_nSelectedItem >= 0 && m_pWndBlacklistItems->m_nSelectedItem < g_pPlayer->m_sPetfilterLists.sBlacklist.m_vdwItemList.size())
			{
				g_pPlayer->m_sPetfilterLists.sBlacklist.m_vdwItemList.erase(g_pPlayer->m_sPetfilterLists.sBlacklist.m_vdwItemList.begin() + m_pWndBlacklistItems->m_nSelectedItem);
			}
		}
		break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndPetfilterLists::OnAddItemToFilterList(DWORD dwItemID)
{
	__AEGON_PET_FILTER_LIST_UNIT* pItemList = NULL;
	if (m_bAddToWhitelist)
	{
		if (m_pWndWhitelistItems && m_pWndWhitelistItems->m_pItemList)
			pItemList = m_pWndWhitelistItems->m_pItemList;
	}
	else 
	{
		if (m_pWndBlacklistItems && m_pWndBlacklistItems->m_pItemList)
			pItemList = m_pWndBlacklistItems->m_pItemList;
	}

	if (pItemList )
	{
		if (!easy_find(pItemList->m_vdwItemList, dwItemID)) 
		{
			pItemList->m_vdwItemList.push_back(dwItemID);
		}
		else 
		{
			g_WndMng.PutString("The selected item is already in the list.", NULL, COLOR_ERROR);

		}
		
	}
}
//pet filter item list
CWndPetfilterItemList::CWndPetfilterItemList()
{
	m_nRowHeight = 40;
	m_nSelectedItem = -1;
	m_pItemList = NULL;
}
CWndPetfilterItemList::~CWndPetfilterItemList()
{
}
void CWndPetfilterItemList::OnDraw(C2DRender* p2DRender)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	if (!m_pItemList)
		return;

	CRect wndRect = GetWindowRect();
	DWORD dwColorText = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
	CPoint point = CPoint(0, 0);
	CString str;

	int nRange = 0;
	int nPage = wndRect.Height() / m_nRowHeight;

	nRange = m_pItemList->m_vdwItemList.size();

	if (!nRange)
		nRange = 1;

	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);
	m_wndScrollBar.EnableScrollBar(TRUE);
	m_wndScrollBar.SetScrollStep(1);

	if (m_wndScrollBar.GetScrollPos() > (m_pItemList->m_vdwItemList.size() - nPage))
		m_wndScrollBar.SetScrollPos(m_pItemList->m_vdwItemList.size() - nPage);

	int nIndex = 0;
	for (int i = m_wndScrollBar.GetScrollPos(); i < m_pItemList->m_vdwItemList.size(); i++, nIndex++)
	{
		point.x = 10;
		point.y = nIndex * m_nRowHeight;

		if (i > m_wndScrollBar.GetScrollPos() + nPage)
			break;


		if (point.y + m_nRowHeight >= m_nRowHeight)
			p2DRender->RenderLine(CPoint(0, point.y + m_nRowHeight), CPoint(wndRect.Width() - 30, point.y + m_nRowHeight), dwColorText);

#ifdef __AEGON_THEME_SWITCHER
		DWORD RenderFillRect = CAegonThemeSwitcherMng::GetInstance()->GetRenderFillRectColor();
		DWORD RenderRect = CAegonThemeSwitcherMng::GetInstance()->GetRenderRectColor();
#endif //__AEGON_THEME_SWITCHER

		if (m_nSelectedItem == i)
		{
			CRect rectSelected = CRect(0, point.y, wndRect.right - 30, point.y + m_nRowHeight);
#ifdef __AEGON_THEME_SWITCHER
			p2DRender->RenderFillRect(rectSelected, RenderFillRect);
			p2DRender->RenderRect(rectSelected, RenderRect);
#else //__AEGON_THEME_SWITCHER
			p2DRender->RenderFillRect(rectSelected, 0xFFf8e6cc);
			p2DRender->RenderRect(rectSelected, 0xFFf28818);
#endif //__AEGON_THEME_SWITCHER
		}

		ItemProp* tmpProp = prj.GetItemProp(m_pItemList->m_vdwItemList[i]);
		CTexture* pActionTexture = NULL;
		if (tmpProp) 
		{
			pActionTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, tmpProp->szIcon).GetString(), COLOR_ITEM_BG);
			str.Format("%s", tmpProp->szName);
		
			if (pActionTexture)
			{
				p2DRender->RenderTexture(CPoint(point.x, point.y + (m_nRowHeight / 2) - (pActionTexture->m_size.cy / 2)), pActionTexture);

				point.x += pActionTexture->m_size.cx + 5;
			}

			CSize strExtent = p2DRender->GetFont()->GetTextExtent(str.GetString());
			p2DRender->TextOut(point.x, point.y + (m_nRowHeight / 2) - (strExtent.cy / 2), str.GetString(), dwColorText);
		}
	}


}
void CWndPetfilterItemList::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	CRect rect = GetWindowRect();

	m_wndScrollBar.Create(WBS_DOCKING | WBS_VERT, rect, this, 1000);
	m_wndScrollBar.SetVisible(IsWndStyle(WBS_VSCROLL));

}
BOOL CWndPetfilterItemList::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndBase::OnChildNotify(message, nID, pLResult);
}
BOOL CWndPetfilterItemList::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndBase::OnCommand(nID, dwMessage, pWndBase);
}
void CWndPetfilterItemList::OnSize(UINT nType, int cx, int cy)
{
	CWndBase::OnSize(nType, cx, cy);
}
void CWndPetfilterItemList::OnLButtonUp(UINT nFlags, CPoint point)
{

}
void CWndPetfilterItemList::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetSelectedItem(point);
}

void CWndPetfilterItemList::OnMouseWndSurface(CPoint point)
{
	int nScrollPos = m_wndScrollBar.GetScrollPos();
	CRect wndRect = GetWndRect();
	int nRowCount = ceil((float)wndRect.Height() / (float)m_nRowHeight);

	for (int i = 0; i < nRowCount; i++)
	{
		if (nScrollPos + i >= m_pItemList->m_vdwItemList.size())
			continue;

		int nSelectedItem = nScrollPos + i;
		if (nSelectedItem < 0 || nSelectedItem >= m_pItemList->m_vdwItemList.size())
			continue;

		ItemProp* pProp = prj.GetItemProp(m_pItemList->m_vdwItemList[nSelectedItem]);

		if (!pProp)
			continue;

		SIZE texSize;
		texSize.cx = 32;
		texSize.cy = 32;


		CRect tmpRect;
		tmpRect.left = 10;
		tmpRect.top = 0 + i * m_nRowHeight;
		tmpRect.right = tmpRect.left + texSize.cx;
		tmpRect.bottom = tmpRect.top + m_nRowHeight;

		CRect rectIcon = CRect(tmpRect.left, tmpRect.CenterPoint().y - texSize.cy / 2, tmpRect.right, tmpRect.CenterPoint().y + texSize.cy / 2);


		if (!rectIcon.PtInRect(point))
			continue;


		CRect hitrect = rectIcon;

		CPoint point2 = point;
		ClientToScreen(&point2);
		ClientToScreen(&hitrect);

		CItemElem itemElem;
		itemElem.m_dwItemId = pProp->dwID;
		itemElem.m_nItemNum = 1;

		g_WndMng.PutToolTip_Item(&itemElem, point2, &hitrect);

		break;

	}
}
void CWndPetfilterItemList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}
void CWndPetfilterItemList::OnRButtonUp(UINT nFlags, CPoint point)
{
}
void CWndPetfilterItemList::SetSelectedItem(CPoint point)
{
	if (IsInvalidObj(g_pPlayer))
		return;

	if (!m_pItemList)
		return;

	int nScrollPos = m_wndScrollBar.GetScrollPos();
	CRect wndRect = GetWndRect();
	int nRowCount = ceil((float)wndRect.Height() / (float)m_nRowHeight);

	for (int i = 0; i < nRowCount; i++)
	{
		if (nScrollPos + i >= m_pItemList->m_vdwItemList.size())
			break;

		CRect tmpRect;
		tmpRect.left = 0;
		tmpRect.top = 0 + i * m_nRowHeight;
		tmpRect.right = wndRect.Width() - 30;
		tmpRect.bottom = tmpRect.top + m_nRowHeight;

		if (tmpRect.PtInRect(point))
		{
			m_nSelectedItem = nScrollPos + i;
			return;
		}
	}
	m_nSelectedItem = -1;
}
#endif //__SYS_PETFILTER