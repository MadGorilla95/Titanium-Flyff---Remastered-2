#pragma once
#ifndef __AEGINGSHP__H
#define __AEGINGSHP__H
#ifdef __AEGON_INGAME_SHOP


typedef struct __AEGIGNSHP_UNIT_COMMAND
{
	DWORD m_dwItemID;
	char m_szItemName[64];
	int m_dwItemNum;

	__AEGIGNSHP_UNIT_COMMAND()
	{
		Init();
	}
	void Init()
	{
		m_dwItemID = 0;
		strncpy(m_szItemName, "", 64);
		m_dwItemNum = 0;
	}
	void Write(CAr& ar)
	{
		ar << m_dwItemID;
		ar.WriteString(m_szItemName);
		ar << m_dwItemNum;
	}
	void Read(CAr& ar)
	{
		ar >> m_dwItemID;
		ar.ReadString(m_szItemName, 64);
		ar >> m_dwItemNum;
	}
	BOOL IsValid() 
	{
		if (m_dwItemID == 0)
			return FALSE;
		if (m_dwItemNum <= 0)
			return FALSE;
		return TRUE;
	}
}__AEGIGNSHP_UNIT_COMMAND;

typedef struct __AEGIGNSHP_CATEGORY
{
	DWORD m_dwCategoryID;
	char m_szName[32];
	DWORD m_nPosition;
	int m_nParentID;
	BOOL m_bCumulatePurchases;
	BOOL m_bIsEnabled;
	
	__AEGIGNSHP_CATEGORY() 
	{
		Init();
	}
	void Init() 
	{
		m_dwCategoryID = 0;
		strncpy(m_szName, "", 32);
		m_nPosition = 0;
		m_nParentID = -1;
		m_bCumulatePurchases = 0;
		m_bIsEnabled = 0;
	}
	void Write(CAr& ar)
	{
		ar << m_dwCategoryID;
		ar.WriteString(m_szName);
		ar << m_nPosition;
		ar << m_nParentID;
		ar << m_bCumulatePurchases;
		ar << m_bIsEnabled;
	}
	void Read(CAr& ar)
	{
		ar >> m_dwCategoryID;
		ar.ReadString(m_szName, 32);
		ar >> m_nPosition;
		ar >> m_nParentID;
		ar >> m_bCumulatePurchases;
		ar >> m_bIsEnabled;
	}
	

}__AEGIGNSHP_CATEGORY;

typedef struct __AEGIGNSHP_ITEM_UNIT
{
	DWORD m_dwItemUnitID;
	DWORD m_dwCategoryID;
	char m_szName[128];
	//char m_szShortDescr[32];
	//char m_szDescr[128];
	int m_nPosition;
	int m_nPrice;
	BOOL m_bIsEnabled;

	vector<__AEGIGNSHP_UNIT_COMMAND> m_vCommandUnits;

	__AEGIGNSHP_ITEM_UNIT()
	{
		Init();
	}
	void Init()
	{
		m_dwItemUnitID = 0;
		m_dwCategoryID = 0;
		memset(m_szName, 0, 128);
		strncpy(m_szName, "error", 128);
		//strncpy(m_szShortDescr, "", 32);
		//strncpy(m_szDescr, "", 128);
		m_nPosition = 0;
		m_nPrice = 0;
		m_bIsEnabled = 0;
		m_vCommandUnits.clear();
	}
	void Write(CAr& ar)
	{
		ar << m_dwItemUnitID;
		ar << m_dwCategoryID;
		ar.WriteString(m_szName);
		//ar.WriteString(m_szShortDescr);
		//ar.WriteString(m_szDescr);
		ar << m_nPosition;
		ar << m_nPrice;
		ar << m_bIsEnabled;

		ar << static_cast<unsigned long>(m_vCommandUnits.size());
		for (int i = 0; i < m_vCommandUnits.size(); i++) 
		{
			m_vCommandUnits[i].Write(ar);
		}
	}
	void Read(CAr& ar)
	{
		ar >> m_dwItemUnitID;
		ar >> m_dwCategoryID;
		ar.ReadString(m_szName, 128);
		//ar.ReadString(m_szShortDescr, 32);
		//ar.ReadString(m_szDescr, 128);
		ar >> m_nPosition;
		ar >> m_nPrice;
		ar >> m_bIsEnabled;

		__AEGIGNSHP_UNIT_COMMAND tmpCommandUnit;
		unsigned long nCommandSize;
		ar >> nCommandSize;
		for (int i = 0; i < nCommandSize; i++) 
		{
			tmpCommandUnit.Init();
			tmpCommandUnit.Read(ar);
			m_vCommandUnits.push_back(tmpCommandUnit);
		}
	}

}__AEGIGNSHP_ITEM_UNIT;



class CAegonIngameShopMng
{
public:

	map<DWORD, __AEGIGNSHP_CATEGORY> m_mapCategories;
	map<DWORD, __AEGIGNSHP_ITEM_UNIT> m_mapShopItems;

	CAegonIngameShopMng();
	virtual ~CAegonIngameShopMng();

	static CAegonIngameShopMng* GetInstance(void);

	void WriteCategoryMap(CAr& ar);
	void ReadCategoryMap(CAr& ar);
	void WriteItemMap(CAr& ar);
	void ReadItemMap(CAr& ar);

	__AEGIGNSHP_ITEM_UNIT* GetShopUnitByID(int nUnitID);

#ifdef __WORLDSERVER
	void IngameShopOnJoin(CUser* pUser);
	void OnShopUpdate();
#endif // __WORLDSERVER

};

#endif // __AEGON_INGAME_SHOP
#endif // __AEGINGSHP__H