#include "stdafx.h"
#include "AegonIngameShop.h"
#include "defineObj.h"
#include "defineItem.h"
#include "defineText.h"

#ifdef __WORLDSERVER
#include "User.h"
#include "DPCoreClient.h"
extern  CDPCoreClient g_DPCoreClient;
#include "DPDatabaseClient.h"
extern	CDPDatabaseClient g_dpDBClient;
extern	CGuildMng	g_GuildMng;
#include "worldmng.h"
extern	CWorldMng	g_WorldMng;
#include "DPSrvr.h"
extern CDPSrvr g_DPSrvr;
extern	CUserMng g_UserMng;
#endif // __WORLDSERVER

#ifdef __CLIENT
#include "ResData.h"
#include "WndManager.h"

#include "DPClient.h"
extern CDPClient g_DPlay;
#endif // __CLIENT

#ifdef __AEGON_INGAME_SHOP

CAegonIngameShopMng::CAegonIngameShopMng()
{

	m_mapShopItems.clear();

#ifdef __WORLDSERVER
	g_dpDBClient.SendInitLoadIngameShop();
#endif // __WORLDSERVER
}

CAegonIngameShopMng::~CAegonIngameShopMng()
{
}

CAegonIngameShopMng* CAegonIngameShopMng::GetInstance(void)
{
	static CAegonIngameShopMng sAegonIngameShopMng;
	return &sAegonIngameShopMng;
}
void CAegonIngameShopMng::WriteCategoryMap(CAr& ar)
{
#if _WIN64
	ar << static_cast<unsigned long>(m_mapCategories.size());
#else
	ar << m_mapCategories.size();
#endif

	map<DWORD, __AEGIGNSHP_CATEGORY>::iterator it = m_mapCategories.begin();
	for (; it != m_mapCategories.end(); it++)
	{
		it->second.Write(ar);
	}
}
void CAegonIngameShopMng::ReadCategoryMap(CAr& ar)
{
	m_mapCategories.clear();

	unsigned long nSize;
	ar >> nSize;
	__AEGIGNSHP_CATEGORY tmpUnit;
	for (int i = 0; i < nSize; i++)
	{
		
		tmpUnit.Init();
		tmpUnit.Read(ar);
		m_mapCategories.insert(make_pair(tmpUnit.m_nPosition, tmpUnit));
	}
}
void CAegonIngameShopMng::WriteItemMap(CAr& ar)
{
#if _WIN64
	ar << static_cast<unsigned long>(m_mapShopItems.size());
#else
	ar << m_mapShopItems.size();
#endif

	map<DWORD, __AEGIGNSHP_ITEM_UNIT>::iterator it = m_mapShopItems.begin();
	for (; it != m_mapShopItems.end(); it++)
	{
		it->second.Write(ar);
	}
}
void CAegonIngameShopMng::ReadItemMap(CAr& ar)
{
	m_mapShopItems.clear();

	unsigned long nSize;
	ar >> nSize;
	__AEGIGNSHP_ITEM_UNIT tmpUnit;
	for (int i = 0; i < nSize; i++)
	{

		tmpUnit.Init();

		tmpUnit.Read(ar);

		m_mapShopItems.insert(make_pair(tmpUnit.m_dwItemUnitID, tmpUnit));
	}
}
__AEGIGNSHP_ITEM_UNIT* CAegonIngameShopMng::GetShopUnitByID(int nUnitID)
{
	map<DWORD, __AEGIGNSHP_ITEM_UNIT>::iterator it = m_mapShopItems.find(nUnitID);
	if (it != m_mapShopItems.end())
	{
		return &it->second;
	}
	return NULL;
}
#ifdef __WORLDSERVER
void CAegonIngameShopMng::IngameShopOnJoin(CUser* pUser)
{
	if (IsValidObj(pUser))
	{
		pUser->AddIngameShopInfo();
	}
}
void CAegonIngameShopMng::OnShopUpdate() 
{
	g_UserMng.AddIngameShopInfo();
}
#endif // __WORLDSERVER

#endif // __AEGON_INGAME_SHOP