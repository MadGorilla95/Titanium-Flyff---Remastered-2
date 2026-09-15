#include "stdafx.h"
#include "AegonTeleportMng.h"
#include "defineObj.h"
#include "defineItem.h"
#include "ResData.h"
#include "d3d9types.h"

#ifdef __CLIENT
#include "DPClient.h"
#include "AppDefine.h"
#include "WndManager.h"
#endif

#ifdef __AEGON_TELEPORTER
///////////////////////////
////////MNG CLASS//////////
///////////////////////////
CAegonTeleporterMng::CAegonTeleporterMng()
{
	m_bScriptLoaded = LoadScript();
	if (!m_bScriptLoaded)
	{
		Error("failed to load teleporter script");
		m_vUnits.clear();
	}
	else
	{
	}
}
CAegonTeleporterMng::~CAegonTeleporterMng()
{
}
CAegonTeleporterMng* CAegonTeleporterMng::GetInstance(void)
{
	static CAegonTeleporterMng sAegonTeleporterMng;
	return &sAegonTeleporterMng;
}
BOOL CAegonTeleporterMng::LoadScript()
{
	CScript s;
	__AEGON_TELEPORT_UNIT tmpUnit;

	if (s.Load(m_scriptFile))
	{
		s.GetToken();
		while (s.tok != FINISHED)
		{
			if (s.Token.CompareNoCase("Teleport") == 0)
			{
				tmpUnit.Init();

				s.GetToken(); // {

				while (s.Token.Compare("}") != 0)
				{
					s.GetToken();
					if (s.Token.CompareNoCase("nameList") == 0)
					{
						s.GetToken();
						strcpy(tmpUnit.m_szNameList, s.Token);
					}
					else if (s.Token.CompareNoCase("nameHeader") == 0)
					{
						s.GetToken();
						strcpy(tmpUnit.m_szNameHeader, s.Token);
					}
					else if (s.Token.CompareNoCase("item") == 0)
					{
						DWORD dwItemID = s.GetNumber();
#ifdef __CLIENT
						tmpUnit.m_vItemIDs.push_back(dwItemID);
#endif
					}
					else if (s.Token.CompareNoCase("teleporterWnd") == 0)
					{
						tmpUnit.m_dwTeleporterWnd = s.GetNumber();
					}
					else if (s.Token.CompareNoCase("type") == 0)
					{
						tmpUnit.m_dwUnitType = s.GetNumber();
					}
					else if (s.Token.CompareNoCase("cost") == 0)
					{
						tmpUnit.m_nCost = s.GetNumber();
					}
					else if (s.Token.CompareNoCase("descrTID") == 0)
					{
						tmpUnit.m_dwDescrTid = s.GetNumber();
					}
					else if (s.Token.CompareNoCase("worldID") == 0)
					{
						tmpUnit.m_dwWorldID = s.GetNumber();
					}
					else if (s.Token.CompareNoCase("pos") == 0)
					{
						tmpUnit.m_telePos.x = s.GetFloat();
						tmpUnit.m_telePos.y = s.GetFloat();
						tmpUnit.m_telePos.z = s.GetFloat();
					}
					else if (s.Token.CompareNoCase("fAngle") == 0)
					{
						tmpUnit.m_fAngle = s.GetFloat();
					}
					else if (s.Token.CompareNoCase("worldDungeon") == 0)
					{
						tmpUnit.m_dwWorldDungeon = s.GetNumber();
					}
					else if (s.Token.CompareNoCase("shortcutTex") == 0)
					{
						s.GetToken();
						strcpy(tmpUnit.m_szNameShortcutTex, s.Token);
					}
					else if (s.Token.CompareNoCase("nameImage") == 0)
					{
						s.GetToken();
						strcpy(tmpUnit.m_szNameImage, s.Token);
					}
					else if (s.Token.Compare("}") == 0)
					{
						tmpUnit.m_nID = m_vUnits.size();
						m_vUnits.push_back(tmpUnit);
					}
				}
			}
			s.GetToken();
		}
	}
	else
	{
		Error("failed to Load %s", m_scriptFile);
		return FALSE;
	}
	return TRUE;
}
LPCSTR CAegonTeleporterMng::GetTypeString(int nType)
{
	switch (nType)
	{
	case 0:
		return "Town";
	case 1:
		return "Collecting";
	case 2:
		return "Dungeon";
	}
	return "error";
}
CString	CAegonTeleporterMng::GetTeleportString(int nUnitID)
{
	CString strOut = "error";

	for (int i = 0; i < m_vUnits.size(); i++) 
	{
		if (m_vUnits[i].m_nID == nUnitID) 
		{
			return GetTeleportString(&m_vUnits[i]);
		}
	}

	return strOut;
}
CString	CAegonTeleporterMng::GetTeleportString(__AEGON_TELEPORT_UNIT* pUnit)
{
	CString strOut = "error";

	if (!pUnit)
		return strOut;

	if(pUnit->m_dwTeleporterWnd == 0)
		strOut.Format("[%s] %s", GetTypeString(pUnit->m_dwUnitType), pUnit->m_szNameList);
	else
		strOut.Format("%s", pUnit->m_szNameList);


	return strOut;
}
CString	CAegonTeleporterMng::GetTeleportLandName(int nUnitID)
{
	CString strOut = "error";

	for (int i = 0; i < m_vUnits.size(); i++)
	{
		if (m_vUnits[i].m_nID == nUnitID)
		{
			return GetTeleportString(&m_vUnits[i]);
		}
	}

	return strOut;
}
CString	CAegonTeleporterMng::GetTeleportLandName(__AEGON_TELEPORT_UNIT* pUnit)
{
	CString strOut = "error";

	if (!pUnit)
		return strOut;

	strOut.Format("%s", pUnit->m_szNameHeader);


	return strOut;
}

CString	CAegonTeleporterMng::GetTeleportShortCutString(int nUnitID)
{
	CString strOut = "error";

	for (int i = 0; i < m_vUnits.size(); i++)
	{
		if (m_vUnits[i].m_nID == nUnitID)
		{
			return GetTeleportString(&m_vUnits[i]);
		}
	}

	return strOut;
}
CString	CAegonTeleporterMng::GetTeleportShortCutString(__AEGON_TELEPORT_UNIT* pUnit)
{
	CString strOut = "error";

	if (!pUnit)
		return strOut;

	strOut.Format("Teleporter-Shortcut\n%s", GetTeleportString(pUnit).GetString());

	return strOut;
}
__AEGON_TELEPORT_UNIT* CAegonTeleporterMng::GetTeleUnitByID(int nUnitID)
{
	for (int i = 0; i < m_vUnits.size(); i++)
	{
		if (m_vUnits[i].m_nID == nUnitID)
			return &m_vUnits[i];
	}
	return NULL;
}
#endif