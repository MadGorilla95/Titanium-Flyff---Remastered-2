#pragma once
#ifndef __AEGLOOBOX__H
#define __AEGLOOBOX__H
#ifdef __AEGON_LOOT_BOX

#define LOOTBOXITEMS 10
#define LOOTBOXTICKETS 5
#define MAXBOXNAME 32
#define LOOTBOXCHANCESUM	10000

#define LOOTBOX_BUY_TICKET	0
#define LOOTBOX_BUY_DP		1

typedef struct __LOOTBOX_UNIT
{
	int boxID;
	char boxName[MAXBOXNAME];
	DWORD neededTicket;
	DWORD itemIDs[LOOTBOXITEMS];
	DWORD itemNums[LOOTBOXITEMS];
	DWORD itemChance[LOOTBOXITEMS];
	DWORD ticketIDs[LOOTBOXTICKETS];
	int itemCount;
	int ticketCount;
	DWORD dpPrice;
	


	void Init()
	{
		boxID = -1;
		neededTicket = 0;
		strcpy(boxName, "");
		itemCount = 0;
		ticketCount = 0;
		dpPrice = 0;
		for(int i = 0; i < LOOTBOXITEMS; i++)
		{
			itemIDs[i] = 0;
			itemChance[i] = 0;
			
		}
		for(int i = 0; i < LOOTBOXTICKETS; i++)
		{
			ticketIDs[i] = 0;
		}
		
	}
	void AddItem(DWORD itemID, DWORD itemNum, DWORD chance)
	{
		for(int i = 0; i < LOOTBOXITEMS; i++)
		{
			if (itemIDs[i] == 0)
			{
				itemIDs[i] = itemID;
				itemNums[i] = itemNum;
				itemChance[i] = chance;
				return;
			}
		}
	}
	BOOL valid()
	{
		DWORD chanceSum = 0;
		DWORD itemSum = 0;
		DWORD ticketSum = 0;
		for(int i = 0; i < LOOTBOXITEMS; i++)
		{
			chanceSum += itemChance[i];
			if (itemIDs[i] != 0)
			{
				if (itemNums[i] == 0)
					itemIDs[i] = 0;
				else
					itemSum++;
				
			}
				
		}

		for(int i = 0; i < LOOTBOXTICKETS; i++)
		{
			if(ticketIDs[i] != 0)
				ticketSum++;
		}

		if (boxID == -1)
			return FALSE;

		if(chanceSum != LOOTBOXCHANCESUM)
			return FALSE;

		if (itemSum <= 1)
			return FALSE;
		else
			itemCount = itemSum;

		if (ticketSum > 0)
			ticketCount = ticketSum;
		else if(ticketSum <= 0 && dpPrice == 0)
			return FALSE;

		if (dpPrice <= 0)
			return FALSE;
			
		return TRUE;
	}
	void AddTicket(DWORD ticketID)
	{
		for (int i = 0; i < LOOTBOXTICKETS; i++)
		{
			if (ticketIDs[i] == 0)
			{
				ticketIDs[i] = ticketID;
				return;
			}
		}
	}
	BOOL IsValidTicket(CItemElem* pTicketElem)
	{
		for (int i = 0; i < LOOTBOXTICKETS; i++)
		{
			if (ticketIDs[i] == pTicketElem->m_dwItemId)
				return TRUE;
		}
		
		return FALSE;
	}
	void GetRandomResult(__LOOTBOX_RESULT* result)
	{
		result->Init();

		int r = rand() % LOOTBOXCHANCESUM;
		int chanceCount = 0;
		
		for(int i = 0; i < itemCount; i++)
		{
			chanceCount += itemChance[i];
			if (r < chanceCount)
			{
				if (itemIDs[i] > 0 && itemNums[i] > 0)
				{
					result->receiveItem = itemIDs[i];
					result->receiveNum = itemNums[i];
					result->itemIndex = i;
					result->chance = itemChance[i];
					result->validResult = TRUE;
					return;
				}
				else
				{
					result->receiveItem = 0;
					result->receiveNum = 0;
					result->itemIndex = 0;
					result->chance = itemChance[i];
					result->validResult = FALSE;
					Error("__LOOTBOX_UNIT GetRandomResult failed, check config file");
					return;
				}
					
				
			}
		}
	}
}__LOOTBOX_UNIT;



class CAegonLootBoxMng
{
public:

	LPCSTR m_scriptFile = "AegonLootBox.inc";
	map<DWORD, __LOOTBOX_UNIT> m_mapLootBoxes;
	BOOL m_bScriptLoaded;

	__LOOTBOX_UNIT tmpLootBox;

	CAegonLootBoxMng();
	virtual ~CAegonLootBoxMng();

	static CAegonLootBoxMng* GetInstance(void);

	BOOL LoadScript();


};
#endif // __AEGON_LOOT_BOX

#endif //__AEGLOOBOX__H