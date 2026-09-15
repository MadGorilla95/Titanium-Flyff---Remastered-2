#pragma once
#ifdef __LEEAEG_SHOP_FINDER

typedef struct __SHOP_ITEM
{
	CItemElem	itemElem;
	D3DXVECTOR3 vPos;
	DWORD		worldID;
	char		nameSeller[MAX_NAME];
	char		location[64];

}__SHOP_ITEM;

class CAegonShopFinderMng
{
public:
	CAegonShopFinderMng();
	virtual ~CAegonShopFinderMng();

	static CAegonShopFinderMng* GetInstance(void);

	map<DWORD, __SHOP_ITEM>	m_mapShopItems;

#ifdef __WORLDSERVER	
	void	TeleportToItem(CUser* pUser, DWORD itemIndex);
	void	RemoveItem(CItemElem* tmpItem);
	void	UpdateItem(CItemElem* tmpItem, int nRemain);
	DWORD	GetNextMapIndex();
	void	AddItem(CItemElem* itemElem, LPCTSTR sellUserName, D3DXVECTOR3 shopPos, DWORD shopWorldID, LPCTSTR shopLocation);
#endif // __WORLDSERVER

	void	WriteMap(CAr& ar);
	void	ReadMap(CAr& ar);
	void	AddSerializedItem(DWORD itemIndex, CItemElem* itemElem, LPCTSTR sellUserName, D3DXVECTOR3 shopPos, DWORD shopWorldID, LPCTSTR shopLocation);

};
#endif // __LEEAEG_SHOP_FINDER