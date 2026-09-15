#include "stdafx.h"
#include "AegonShopFinder.h"
#include "defineObj.h"
#include "defineItem.h"

#ifdef __WORLDSERVER
#include "User.h"
#include "defineText.h"
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

#ifdef __LEEAEG_SHOP_FINDER
CAegonShopFinderMng::CAegonShopFinderMng()
{
	m_mapShopItems.clear();
#ifdef __WORLDSERVER
#endif // __WORLDSERVER
}

CAegonShopFinderMng::~CAegonShopFinderMng()
{
}

CAegonShopFinderMng* CAegonShopFinderMng::GetInstance(void)
{
	static CAegonShopFinderMng sShopFinderMng;
	return &sShopFinderMng;
}
#ifdef __WORLDSERVER
void CAegonShopFinderMng::TeleportToItem(CUser* pUser, DWORD itemIndex)
{
	map<DWORD, __SHOP_ITEM>::iterator it = m_mapShopItems.find(itemIndex);

	if (pUser && pUser->IsValid() && it != m_mapShopItems.end())
	{
		pUser->REPLACE(g_uIdofMulti, it->second.worldID, it->second.vPos, REPLACE_NORMAL, nDefaultLayer);
	}
}
void CAegonShopFinderMng::AddItem(CItemElem* itemElem, LPCTSTR sellUserName, D3DXVECTOR3 shopPos, DWORD shopWorldID, LPCTSTR shopLocation)
{

	DWORD itemIndex = GetNextMapIndex();
	
	__SHOP_ITEM tmpShopItem;

	tmpShopItem.itemElem = *itemElem;
	tmpShopItem.itemElem.m_nCost = itemElem->m_nCost;
	tmpShopItem.itemElem.m_nBuyMode = itemElem->m_nBuyMode;

	sprintf(tmpShopItem.nameSeller, sellUserName);
	tmpShopItem.worldID = shopWorldID;
	tmpShopItem.vPos = shopPos;
	tmpShopItem.itemElem.m_shopItemIndex = itemIndex;

	tmpShopItem.itemElem.b_shopFinderItem = TRUE;

	sprintf(tmpShopItem.location, shopLocation);

	if (m_mapShopItems.find(itemIndex) == m_mapShopItems.end())
	{
		m_mapShopItems.insert(make_pair(itemIndex, tmpShopItem));
		itemElem->m_shopItemIndex = itemIndex;
	}	
}
void CAegonShopFinderMng::RemoveItem(CItemElem* tmpItemElem)
{
	map<DWORD, __SHOP_ITEM>::iterator it = m_mapShopItems.find(tmpItemElem->m_shopItemIndex);
	

	if (it != m_mapShopItems.end())
	{
		it->second.itemElem.b_shopFinderItem = FALSE;
		m_mapShopItems.erase(tmpItemElem->m_shopItemIndex);
		tmpItemElem->m_shopItemIndex = 0;
	}
}
void CAegonShopFinderMng::UpdateItem(CItemElem* tmpItemElem, int nRemain)
{
	if (m_mapShopItems.find(tmpItemElem->m_shopItemIndex) != m_mapShopItems.end())
	{
		if (nRemain > 0)
		{
			m_mapShopItems.find(tmpItemElem->m_shopItemIndex)->second.itemElem.m_nItemNum = nRemain;
		}
		else
		{
			RemoveItem(tmpItemElem);
		}

	}
}
DWORD CAegonShopFinderMng::GetNextMapIndex()
{
	for (int i = 1; i < m_mapShopItems.size() + 1; i++)
	{
		if (m_mapShopItems.find(i) == m_mapShopItems.end())
		{
			return i;
		}
	}
}
#endif // __WORLDSERVER

void CAegonShopFinderMng::WriteMap(CAr& ar)
{

	DWORD mapSize = m_mapShopItems.size();
	ar << mapSize;

	map<DWORD, __SHOP_ITEM>::iterator it = m_mapShopItems.begin();
	for (; it != m_mapShopItems.end(); it++)
	{
		ar << it->first;
		it->second.itemElem.Serialize(ar);
		ar.WriteString(it->second.nameSeller);
		ar << it->second.vPos;
		ar << it->second.worldID;
		ar.WriteString(it->second.location);
	}

}
void CAegonShopFinderMng::ReadMap(CAr& ar)
{
	DWORD mapSize;
	ar >> mapSize;

	m_mapShopItems.clear();
	for (int i = 0; i < mapSize; i++)
	{
		DWORD itemIndexAR = 0;
		CItemElem	itemElemAR;
		D3DXVECTOR3 vPosAR;
		DWORD		worldIDAR;
		char		nameSellerAR[MAX_NAME];
		char		shopLocationAR[64];

		ar >> itemIndexAR;
		itemElemAR.Serialize(ar);
		ar.ReadString(nameSellerAR, MAX_NAME);
		ar >> vPosAR;
		ar >> worldIDAR;
		ar.ReadString(shopLocationAR, 64);

		if (itemIndexAR != 0)
		{

			AddSerializedItem(itemIndexAR, &itemElemAR, (LPCTSTR)nameSellerAR, vPosAR, worldIDAR, (LPCTSTR)shopLocationAR);
		}



	}
}

void CAegonShopFinderMng::AddSerializedItem(DWORD itemIndex, CItemElem* itemElem, LPCTSTR sellUserName, D3DXVECTOR3 shopPos, DWORD shopWorldID, LPCTSTR shopLocation)
{
	__SHOP_ITEM tmpShopItem;

	tmpShopItem.itemElem = *itemElem;
	tmpShopItem.itemElem.m_nCost = itemElem->m_nCost;
	tmpShopItem.itemElem.m_nBuyMode = itemElem->m_nBuyMode;

	sprintf(tmpShopItem.nameSeller, sellUserName);
	tmpShopItem.worldID = shopWorldID;
	tmpShopItem.vPos = shopPos;
	tmpShopItem.itemElem.m_shopItemIndex = itemIndex;

	tmpShopItem.itemElem.b_shopFinderItem = TRUE;

	sprintf(tmpShopItem.location, shopLocation);

	if (m_mapShopItems.find(itemIndex) == m_mapShopItems.end())
	{
		m_mapShopItems.insert(make_pair(itemIndex, tmpShopItem));
		itemElem->m_shopItemIndex = itemIndex;
	}
}
#endif // __LEEAEG_SHOP_FINDER