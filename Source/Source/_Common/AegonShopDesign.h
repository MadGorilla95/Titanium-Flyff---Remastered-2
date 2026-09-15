#pragma once
#ifndef __AEGSHPDES__H
#define __AEGSHPDES__H
#ifdef __AEGON_IMPROVED_SHOPDESIGN
typedef struct __AEGON_SHOP_DESIGN
{
	char szNameTexLeft[128];
	char szNameTexRight[128];
	char szNameTexCenter[128];

	__AEGON_SHOP_DESIGN() 
	{
		memset(szNameTexLeft, 0, 128);
		memset(szNameTexCenter, 0, 128);
		memset(szNameTexRight, 0, 128);
	}
}__AEGON_SHOP_DESIGN;


class CAegonShopDesignMng
{
public:

	LPCSTR m_scriptFile = "ShopDesign.inc";
	BOOL m_bScriptLoaded;

	map<int, __AEGON_SHOP_DESIGN> m_mapShopDesigns;

	CAegonShopDesignMng();
	virtual ~CAegonShopDesignMng();

	static CAegonShopDesignMng* GetInstance(void);

	BOOL LoadScript();

#ifdef __WORLDSERVER
	void OnPlayerShopDesignBuffChange(CUser* pUser, BOOL bForceRemove = FALSE);
#endif

};
#endif // __AEGON_IMPROVED_SHOPDESIGN

#endif //__AEGSHPDES__H