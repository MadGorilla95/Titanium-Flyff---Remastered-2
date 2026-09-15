#pragma once
#ifndef __WNDAEGTELSYS__H
#define __WNDAEGTELSYS__H

#ifdef __AEGON_TELEPORTER

typedef struct __AEGON_TELEPORT_UNIT
{
	int m_nID;
	char m_szNameList[64];
	char m_szNameHeader[64];
	char m_szNameShortcutTex[64];
	char m_szNameImage[64];

	DWORD m_dwTeleporterWnd;
	DWORD m_dwUnitType;
	int m_nCost;
	DWORD m_dwDescrTid;
	DWORD m_dwWorldID;
	DWORD m_dwWorldDungeon;
	D3DXVECTOR3 m_telePos;
	FLOAT m_fAngle;

	vector<DWORD> m_vItemIDs;

	__AEGON_TELEPORT_UNIT()
	{
		Init();
	}
	void Init()
	{
		m_nID = -1;
		sprintf(m_szNameList, "error");
		sprintf(m_szNameHeader, "error");
		sprintf(m_szNameShortcutTex, "error");
		sprintf(m_szNameImage, "error");

		m_dwDescrTid = 0;
		m_dwTeleporterWnd = 0;
		m_dwUnitType = 0;
		m_nCost = 0;
		m_dwWorldID = 0;
		m_dwWorldDungeon = 0;
		m_vItemIDs.clear();
		m_telePos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_fAngle = 0.0f;
	}

}__AEGON_TELEPORT_UNIT;

class CAegonTeleporterMng
{
	LPCSTR m_scriptFile = "AegonTeleporter.inc";
	BOOL m_bScriptLoaded;
public:

	vector<__AEGON_TELEPORT_UNIT> m_vUnits;

	CAegonTeleporterMng();
	virtual ~CAegonTeleporterMng();

	static CAegonTeleporterMng* GetInstance(void);

	BOOL LoadScript();

	LPCSTR GetTypeString(int nType);

	CString	GetTeleportString(int nUnitID);
	CString	GetTeleportString(__AEGON_TELEPORT_UNIT* pUnit);

	CString	GetTeleportLandName(int nUnitID);
	CString GetTeleportLandName(__AEGON_TELEPORT_UNIT* pUnit);

	CString GetTeleportShortCutString(int nUnitID);
	CString GetTeleportShortCutString(__AEGON_TELEPORT_UNIT* pUnit);

	__AEGON_TELEPORT_UNIT* GetTeleUnitByID(int nUnitID);
};

#endif

#endif