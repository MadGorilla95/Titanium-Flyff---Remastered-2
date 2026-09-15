#include "stdafx.h"
#include "mover.h"
#include "defineText.h"
#ifdef __WORLDSERVER
#include "user.h"
extern	CUserMng			g_UserMng;

#ifdef __CHIP_PLAYERSHOP
#include "DPDatabaseClient.h"
extern  CDPDatabaseClient   g_dpDBClient;
#endif //__CHIP_PLAYERSHOP

#ifdef __LEEAEG_SHOP_FINDER
#include "AegonShopFinder.h"
#endif // __LEEAEG_SHOP_FINDER

#ifdef __VENDING_NPC
#include "defineObj.h"
#include "DPCoreClient.h"
extern  CDPCoreClient       g_DPCoreClient;
#endif //__VENDING_NPC

#endif	// WORLDSERVER


// pc, npc의 분리 
// m_nCost삭제 

// 거래 상대방을 얻는다.
CMover* CVTInfo::GetOther()
{
//	return m_pOther;
	if( m_objId == NULL_ID )
		return NULL;

	return prj.GetMover( m_objId );
}

// 거래 상대방을 정한다.
void CVTInfo::SetOther( CMover* pMover )
{
//	m_pOther = pMover;
	if( pMover )
	{
		m_objId = pMover->GetId();
	}
	else
	{
		m_objId = NULL_ID;
	}
}

#ifdef __VENDING_NPC
void CVTInfo::SetOwner(CMover* pMover)
{
	if (pMover)
	{
		m_idOwner = pMover->m_idPlayer;
	}
	else
	{
		m_idOwner = NULL_ID;
	}
}
#endif // __VENDING_NPC

CItemBase* CVTInfo::GetItem( BYTE i )
{
	return m_apItem_VT[i];
}

void CVTInfo::SetItem( BYTE i, CItemBase* pItemBase )
{
	m_apItem_VT[i] = pItemBase;
}

LPCTSTR	CVTInfo::GetTitle()
{
	return m_strTitle.c_str();
}

void CVTInfo::SetTitle( LPCTSTR szTitle )
{
	m_strTitle = szTitle;
}

BOOL CVTInfo::IsVendorOpen()
{
	return (m_strTitle.empty() != true);
}

void CVTInfo::Init( CMover* pOwner )
{
#ifdef __VENDING_NPC
	m_tmOpened = 0;
	m_idOwner = NULL;
	m_pVendor = pOwner;
#else // __VENDING_NPC
	m_pOwner = pOwner;
#endif // __VENDING_NPC

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
	m_dwAchievementSellCount = 0;
#endif // __AEGON_ACHIEVEMENT_SYSTEM

	ZeroMemory( m_apItem_VT, sizeof( m_apItem_VT ) );
	TradeSetGold( 0 );							// raiders.2006.11.28 
	TradeClear();
	m_strTitle = "";
}

void CVTInfo::TradeClear()
{
	SetOther( NULL );
	for( int i = 0; i < MAX_TRADE; i++ )
	{
		if( m_apItem_VT[i] )
		{
#ifdef __LEEAEG_SHOP_FINDER
#ifdef __WORLDSERVER
			CAegonShopFinderMng::GetInstance()->RemoveItem((CItemElem*)m_apItem_VT[i]);
#endif // __WORLDSERVER
#endif // __LEEAEG_SHOP_FINDER
			m_apItem_VT[i]->SetExtra( 0 );
			m_apItem_VT[i] = NULL;
		}
	}
#ifdef __WORLDSERVER	
	int nGold = TradeGetGold();
#ifdef __VENDING_NPC
	if (nGold > 0 && m_pVendor)
		m_pVendor->AddGold(nGold);
#else //__VENDING_NPC
	if (nGold > 0 && m_pOwner)
		m_pOwner->AddGold(nGold);
#endif //__VENDING_NPC
#endif //__WORLDSERVER

	TradeSetGold( 0 );
	TradeSetState( TRADE_STEP_ITEM );
}

void CVTInfo::TradeSetGold( DWORD dwGold )
{
	m_dwTradeGold = dwGold;
}

int CVTInfo::TradeGetGold()
{
	return m_dwTradeGold;
}

void CVTInfo::TradeSetItem( BYTE nId, BYTE i, short nItemNum )
{
#ifdef __VENDING_NPC
	CItemBase* pItemBase = m_pVendor->GetItemId(nId);
#else // __VENDING_NPC
	CItemBase* pItemBase = m_pOwner->GetItemId(nId);
#endif // __VENDING_NPC
	if( pItemBase )
	{
		m_apItem_VT[i] = pItemBase;
		pItemBase->SetExtra( nItemNum );
	}
}

BOOL CVTInfo::TradeClearItem( BYTE i )
{
	CItemBase* pItemBase = m_apItem_VT[i];
	if( IsUsingItem( pItemBase ) )
	{
		pItemBase->SetExtra( 0 );		// clear - using flag 
		m_apItem_VT[i] = NULL;

		return TRUE;
	}
	else
		return FALSE;
}

//raiders.2006.11.28  계산과정 변경 ( 인벤돈 = 인벤돈 + 상대방 거래창 돈 )
BOOL CVTInfo::TradeConsent()
{
	CMover* pTrader	= GetOther();
	if( pTrader == NULL )
		return FALSE;

	int cbI	= 0, cbYou	= 0;
	CItemContainer<CItemElem> a;
	a.SetItemContainer( ITYPE_ITEM, MAX_TRADE );
			
	CItemBase* pItemBase;
	for( int i = 0; i < MAX_TRADE; i++ )
	{
		pItemBase = m_apItem_VT[i];
		if( !pItemBase )
			continue;

		m_apItem_VT[i] = NULL;
		CItemElem* pItemElem = ( CItemElem* )pItemBase;
		if( pItemElem->GetProp()->dwPackMax > 1 )
		{
			short nTradeNum = pItemElem->m_nItemNum - (short)pItemBase->GetExtra();
			pItemElem->m_nItemNum = pItemBase->GetExtra();
			a.Add( pItemElem );
			pItemElem->m_nItemNum = nTradeNum;
			pItemElem->SetExtra( 0 );
			if( nTradeNum == 0 )
#ifdef __VENDING_NPC
				m_pVendor->m_Inventory.RemoveAtId(pItemBase->m_dwObjId);	// 제거
#else // __VENDING_NPC
				m_pOwner->m_Inventory.RemoveAtId(pItemBase->m_dwObjId);	// 제거
#endif // __VENDING_NPC
		}
		else
		{
			a.Add( pItemElem );
#ifdef __VENDING_NPC
			m_pVendor->m_Inventory.RemoveAtId(pItemBase->m_dwObjId);
#else // __VENDING_NPC
			m_pOwner->m_Inventory.RemoveAtId(pItemBase->m_dwObjId);
#endif // __VENDING_NPC
		}
	}
		
	for( int i = 0; i < MAX_TRADE; i++ )
	{
		pItemBase = pTrader->m_vtInfo.GetItem( i );
		if( pItemBase == NULL )
			continue;

		pTrader->m_vtInfo.SetItem( i, NULL );
		CItemElem* pItemElem = ( CItemElem* )pItemBase;
		if( pItemElem->GetProp()->dwPackMax > 1 )
		{
			short nTradeNum = pItemElem->m_nItemNum - (short)pItemBase->GetExtra();
			pItemElem->m_nItemNum = pItemBase->GetExtra();
			
#ifdef __VENDING_NPC
			m_pVendor->m_Inventory.Add(pItemElem);
#else // __VENDING_NPC
			m_pOwner->m_Inventory.Add(pItemElem);
#endif // __VENDING_NPC

			pItemElem->m_nItemNum = nTradeNum;
			pItemElem->SetExtra( 0 );
			if( nTradeNum == 0 )
				pTrader->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );	// 제거
		}
		else
		{
#ifdef __VENDING_NPC
			m_pVendor->m_Inventory.Add(pItemElem);
#else // __VENDING_NPC
			m_pOwner->m_Inventory.Add(pItemElem);
#endif // __VENDING_NPC
			pTrader->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );
		}
	}

	cbI		= a.GetCount();
	for( int i = 0; i < cbI; i++ )
	{
		pItemBase	= a.GetAtId( i );
		pTrader->m_Inventory.Add( (CItemElem*)pItemBase );
	}
	
	// step1. 줄돈과 뺄돈을 구해둔다.
	int nThisGold = pTrader->m_vtInfo.TradeGetGold();
	int nTraderGold = TradeGetGold();

	// step2. m_dwTradeGold를 clear
	TradeSetGold( 0 );						// 원복 안되게 
	TradeClear();
	pTrader->m_vtInfo.TradeSetGold( 0 );	// 원복 안되게 
	pTrader->m_vtInfo.TradeClear();

	// step3. 돈을 더한다.
#ifdef __VENDING_NPC
	m_pVendor->AddGold(nThisGold, FALSE);
#else // __VENDING_NPC
	m_pOwner->AddGold(nThisGold, FALSE);
#endif // __VENDING_NPC								
	pTrader->AddGold( nTraderGold, FALSE );

	return TRUE;
}

DWORD CVTInfo::TradeSetItem2( BYTE nId, BYTE i, short & nItemNum )
{
#ifdef __VENDING_NPC
	CItemBase* pItemBase = m_pVendor->GetItemId(nId);
#else // __VENDING_NPC
	CItemBase* pItemBase = m_pOwner->GetItemId(nId);
#endif // __VENDING_NPC
	if( IsUsableItem( pItemBase ) == FALSE || m_apItem_VT[i] != NULL )  
		return (DWORD)TID_GAME_CANNOTTRADE_ITEM;

#ifdef __VENDING_NPC
	if (m_pVendor->m_Inventory.IsEquip(pItemBase->m_dwObjId))
#else // __VENDING_NPC
	if (m_pOwner->m_Inventory.IsEquip(pItemBase->m_dwObjId))
#endif // __VENDING_NPC
		return (DWORD)TID_GAME_CANNOTTRADE_ITEM;
	
	if( ( (CItemElem*)pItemBase )->IsQuest() )
		return (DWORD)TID_GAME_CANNOTTRADE_ITEM;

	if( ( (CItemElem*)pItemBase )->IsBinds() )
		return (DWORD)TID_GAME_CANNOTTRADE_ITEM;

#ifdef __VENDING_NPC
	if (m_pVendor->IsUsing((CItemElem*)pItemBase))
#else // __VENDING_NPC
	if (m_pOwner->IsUsing((CItemElem*)pItemBase))
#endif // __VENDING_NPC
		return (DWORD)TID_GAME_CANNOT_DO_USINGITEM;

	if( pItemBase->GetProp()->dwItemKind3 == IK3_CLOAK && ( (CItemElem*)pItemBase )->m_idGuild != 0 )
		return (DWORD)TID_GAME_CANNOTTRADE_ITEM;
	
	if( pItemBase->GetProp()->dwParts == PARTS_RIDE && pItemBase->GetProp()->dwItemJob == JOB_VAGRANT )
		return (DWORD)TID_GAME_CANNOTTRADE_ITEM;
	
	if( nItemNum < 1)
		nItemNum = 1;
	if( nItemNum > ( (CItemElem*)pItemBase )->m_nItemNum )
		nItemNum = ( (CItemElem*)pItemBase )->m_nItemNum;
	
	TradeSetItem( nId, i, nItemNum );
	return 0;
}

void TradeLog( CAr & ar, CItemBase* pItemBase, short nItemCount )
{
	ar << nItemCount;
	ar << ( (CItemElem*)pItemBase )->GetAbilityOption();
	ar << int(( (CItemElem*)pItemBase )->m_bItemResist);
	ar << int(( (CItemElem*)pItemBase )->m_nResistAbilityOption);
	ar << ( (CItemElem*)pItemBase )->m_nHitPoint;
	ar << ( (CItemElem*)pItemBase )->m_nRepair;
	ar << ( (CItemElem*)pItemBase )->m_bCharged;
	ar << ( (CItemElem*)pItemBase )->m_dwKeepTime;
#if __VER >= 12 // __EXT_PIERCING
	ar << ( (CItemElem*)pItemBase )->GetPiercingSize();
	for( int i=0; i<( (CItemElem*)pItemBase )->GetPiercingSize(); i++ )
		ar << ( (CItemElem*)pItemBase )->GetPiercingItem( i );
	ar << ( (CItemElem*)pItemBase )->GetUltimatePiercingSize();
	for( int i=0; i<( (CItemElem*)pItemBase )->GetUltimatePiercingSize(); i++ )
		ar << ( (CItemElem*)pItemBase )->GetUltimatePiercingItem( i );
#else // __EXT_PIERCING
	ar << ( (CItemElem*)pItemBase )->GetPiercingSize();
	ar << ( (CItemElem*)pItemBase )->GetPiercingItem( 0 );
	ar << ( (CItemElem*)pItemBase )->GetPiercingItem( 1 );
	ar << ( (CItemElem*)pItemBase )->GetPiercingItem( 2 );
	ar << ( (CItemElem*)pItemBase )->GetPiercingItem( 3 );
#if __VER >= 9 // __ULTIMATE
	ar << ( (CItemElem*)pItemBase )->GetPiercingItem( 4 );
#endif // __ULTIMATE
#endif // __EXT_PIERCING
	ar << ( (CItemElem*)pItemBase )->GetRandomOptItemId();
#if __VER >= 9 // __PET_0410
		if( ((CItemElem*)pItemBase)->m_pPet )
		{
			CPet* pPet = ((CItemElem*)pItemBase)->m_pPet;

			ar << pPet->GetKind();
			ar << pPet->GetLevel();
			ar << pPet->GetExp();
			ar << pPet->GetEnergy();
			ar << pPet->GetLife();
			ar << pPet->GetAvailLevel( PL_D );
			ar << pPet->GetAvailLevel( PL_C );
			ar << pPet->GetAvailLevel( PL_B );
			ar << pPet->GetAvailLevel( PL_A );
			ar << pPet->GetAvailLevel( PL_S );
		}
		else
		{
			// mirchang_100514 TransformVisPet_Log
#if __VER >= 15 // __PETVIS
			if( ((CItemElem*)pItemBase)->IsTransformVisPet() == TRUE )
			{
				ar << (BYTE)100;
			}
			else
			{
				ar << (BYTE)0;
			}
#else // __PETVIS
			ar << (BYTE)0;
#endif // __PETVIS
			// mirchang_100514 TransformVisPet_Log
			ar << (BYTE)0;
			ar << (DWORD)0;
			ar << (WORD)0;
			ar << (WORD)0;
			ar << (BYTE)0;
			ar << (BYTE)0;
			ar << (BYTE)0;
			ar << (BYTE)0;
			ar << (BYTE)0;
		}
#endif // __PET_0410
#ifdef __UPGRADE_SUCCESS
		ar << static_cast<CItemElem*>(pItemBase)->m_nUpgradeTries;
#endif // __UPGRADE_SUCCESS
}

// nMinus - 나갈 돈 
// nPlus  - 들어올 돈 
BOOL CheckTradeGold( CMover* pMover, int nMinus, int nPlus )
{
	if( nMinus >= 0 )
	{
		if( 
#ifdef __GAMEGUARD
			pMover &&
#endif //__GAMEGUARD
			pMover->GetGold() >= nMinus )
		{
			int nGold = pMover->GetGold() - nMinus;
			int nResult = nGold + nPlus;
			if( nPlus >= 0 )
			{
				if( nResult >= nGold )	// overflow가 아니면?
					return TRUE;
			}
			else
			{
				if( nResult >= 0 )		// underflow가 아니면?
					return TRUE;
			}

		}
	}

	return FALSE;
}
//raiders.2006.11.28  계산과정 변경 
//로그와 체크를 때어내면 클라와 같다. 통합해서 refactoring하자.
TRADE_CONFIRM_TYPE CVTInfo::TradeLastConfirm( CAr & ar )
{
	TRADE_CONFIRM_TYPE		result = TRADE_CONFIRM_ERROR;
	CMover*					pTrader	= GetOther();

	// gold_step1. GetGold(), AddGold() 함수가 원하는데로 동작되도록 보관 후에 clear해둔다.
	int nTraderGold        = pTrader->m_vtInfo.TradeGetGold();
	int nUserGold          = TradeGetGold();
	
	// gold_step2. 줄돈과 뺄돈을 구해둔다.
	int nThisAdd   = nTraderGold;
	int nTraderAdd = nUserGold;

	// gold_step3. overflow를 검사 
#ifdef __VENDING_NPC
	if (CheckTradeGold(m_pVendor, 0, nTraderGold) == FALSE ||
#else // __VENDING_NPC
	if (CheckTradeGold(m_pOwner, 0, nTraderGold) == FALSE ||
#endif // __VENDING_NPC
		CheckTradeGold( pTrader, 0, nUserGold ) == FALSE )
	{
		TradeClear();
		pTrader->m_vtInfo.TradeClear();
		return result;
	}

	// 교환 할 만큼 양쪽의 인벤토리 슬롯이 여유가 있는 지를 검사한다. 
	int nPlayers = 0;
	int nTraders = 0;

	for( int i = 0; i < MAX_TRADE; i++ )
	{
		if( GetItem( i ) )
			nPlayers++;

		if( pTrader->m_vtInfo.GetItem( i ) )
			nTraders++;
	}

	if( ( pTrader->m_Inventory.GetSize() - pTrader->m_Inventory.GetCount() ) < nPlayers )
		result = TRADE_CONFIRM_ERROR;
#ifdef __VENDING_NPC
	else if ((m_pVendor->m_Inventory.GetSize() - m_pVendor->m_Inventory.GetCount()) < nTraders)
#else //__VENDING_NPC
	else if ((m_pOwner->m_Inventory.GetSize() - m_pOwner->m_Inventory.GetCount()) < nTraders)
#endif // __VENDING_NPC
		result = TRADE_CONFIRM_ERROR;
	else
		result = TRADE_CONFIRM_OK;

	if( result == TRADE_CONFIRM_OK )
	{
		CItemContainer<CItemElem> u;
		u.SetItemContainer( ITYPE_ITEM, MAX_TRADE );

		// gold_step4. 돈을 더한다.
#ifdef __VENDING_NPC
		m_pVendor->AddGold(nThisAdd, FALSE);
#else // __VENDING_NPC
		m_pOwner->AddGold(nThisAdd, FALSE);
#endif // __VENDING_NPC						
		pTrader->AddGold( nTraderAdd, FALSE );
		// TradeClear에서 원복이 안되기 위해서 
		pTrader->m_vtInfo.TradeSetGold( 0 );
		TradeSetGold( 0 );

		ar.WriteString( "T" );
		ar.WriteString( pTrader->GetName() );

#ifdef __VENDING_NPC
		ar.WriteString(m_pVendor->GetName());
		ar << m_pVendor->GetWorld()->GetID();
		ar << nTraderGold << nUserGold;
		ar << pTrader->GetGold() << m_pVendor->GetGold();
		ar << m_pVendor->m_idPlayer << m_pVendor->GetLevel() << m_pVendor->GetJob();
		ar << pTrader->m_idPlayer << pTrader->GetLevel() << pTrader->GetJob();
#ifdef __WORLDSERVER
		ar.WriteString(((CUser*)m_pVendor)->m_playAccount.lpAddr);
		ar.WriteString(((CUser*)pTrader)->m_playAccount.lpAddr);
#endif	// __WORLDSERVER
#else // __VENDING_NPC
		ar.WriteString(m_pOwner->GetName());
		ar << m_pOwner->GetWorld()->GetID();
		ar << nTraderGold << nUserGold;
		ar << pTrader->GetGold() << m_pOwner->GetGold();
		ar << m_pOwner->m_idPlayer << m_pOwner->GetLevel() << m_pOwner->GetJob();
		ar << pTrader->m_idPlayer << pTrader->GetLevel() << pTrader->GetJob();
#ifdef __WORLDSERVER
		ar.WriteString(((CUser*)m_pOwner)->m_playAccount.lpAddr);
		ar.WriteString(((CUser*)pTrader)->m_playAccount.lpAddr);
#endif	// __WORLDSERVER
#endif // __VENDING_NPC

		u_long uSize1	= 0;
		u_long uOffset1	= ar.GetOffset();
		ar << (DWORD)0;
		u_long uSize2	= 0;
		u_long uOffset2	= ar.GetOffset();
		ar << (DWORD)0;

		// item_step1. m_pOwner->임시 
		CItemBase* pItemBase;
		for( int i = 0; i < MAX_TRADE; i++ )
		{
			pItemBase = m_apItem_VT[i];
			if( pItemBase == NULL )
				continue;

			m_apItem_VT[i] = NULL;
			CItemElem* pItemElem = ( CItemElem* )pItemBase;
			if( pItemElem->GetProp()->dwPackMax > 1 )
			{
				short nTradeNum = pItemElem->m_nItemNum - pItemBase->GetExtra();
				pItemElem->m_nItemNum = pItemBase->GetExtra();
				u.Add( pItemElem );
				pItemElem->m_nItemNum = nTradeNum;
				pItemElem->SetExtra( 0 );
				if( nTradeNum == 0 )
#ifdef __VENDING_NPC
					m_pVendor->m_Inventory.RemoveAtId(pItemBase->m_dwObjId);
#else // __VENDING_NPC
					m_pOwner->m_Inventory.RemoveAtId(pItemBase->m_dwObjId);	// 제거
#endif // __VENDING_NPC
			}
			else
			{
				u.Add( pItemElem );	// 임시 버퍼에 추가
#ifdef __VENDING_NPC
				m_pVendor->m_Inventory.RemoveAtId(pItemBase->m_dwObjId);	// 제거
#else // __VENDING_NPC
				m_pOwner->m_Inventory.RemoveAtId(pItemBase->m_dwObjId);	// 제거
#endif // __VENDING_NPC
			}
		}
		
		// item_step2. pTrader -> m_pOwner
		for( int i = 0; i < MAX_TRADE; i++ )
		{
			pItemBase = pTrader->m_vtInfo.GetItem( i );
			if( pItemBase == NULL )
				continue;

			pTrader->m_vtInfo.SetItem( i, NULL );
			uSize1++;
			ar << pItemBase->m_dwItemId;
			ar << pItemBase->GetSerialNumber();
			//ar.WriteString( pItemBase->GetProp()->szName );
			char szItemId[32] = {0, };
			_stprintf( szItemId, "%d", pItemBase->GetProp()->dwID );
			ar.WriteString( szItemId );

			CItemElem* pItemElem = ( CItemElem* )pItemBase;
			if( pItemElem->GetProp()->dwPackMax > 1 )
			{
				int nTradeNum = pItemElem->m_nItemNum - pItemBase->GetExtra();
				pItemElem->m_nItemNum = pItemBase->GetExtra();
#ifdef __VENDING_NPC
				m_pVendor->m_Inventory.Add(pItemElem);
#else // __VENDING_NPC
				m_pOwner->m_Inventory.Add(pItemElem);
#endif // __VENDING_NPC
				pItemElem->m_nItemNum = (short)nTradeNum;

				TradeLog( ar, pItemBase, pItemBase->GetExtra() );
				
				pItemElem->SetExtra( 0 );
				if( nTradeNum == 0 )
					pTrader->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );	
			}
			else
			{
				TradeLog( ar, pItemBase, 1 );
				
#ifdef __VENDING_NPC
				m_pVendor->m_Inventory.Add(pItemElem);
#else // __VENDING_NPC		
				m_pOwner->m_Inventory.Add(pItemElem);	// pUser에 pTrader가 준 아이템을 추가
#endif // __VENDING_NPC
				pTrader->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );	
			}
		}

		// item_step3. 임시 -> pTrader
		nPlayers = u.GetCount();	// 합침을 고려해서 구해둔다.
		for( int i = 0; i < nPlayers; i++ )
		{
			pItemBase = u.GetAtId( i );
			pTrader->m_Inventory.Add( (CItemElem*)pItemBase );
			uSize2++;
			ar << pItemBase->m_dwItemId;
			ar << pItemBase->GetSerialNumber();
			//ar.WriteString( pItemBase->GetProp()->szName );
			char szItemId[32] = {0, };
			_stprintf( szItemId, "%d", pItemBase->GetProp()->dwID );
			ar.WriteString( szItemId );
			TradeLog( ar, pItemBase, ((CItemElem*)pItemBase)->m_nItemNum );
		}

//		GETBLOCK( ar, lpBlock, nBlockSize );
		int nBufSize;	
		LPBYTE lpBlock	= ar.GetBuffer( &nBufSize );

		*(UNALIGNED u_long*)( lpBlock + uOffset1 )	= uSize1;
		*(UNALIGNED u_long*)( lpBlock + uOffset2 )	= uSize2;	
	}


	TradeClear();
	pTrader->m_vtInfo.TradeClear();
	return result;
}

TRADE_STATE CVTInfo::TradeGetState()
{
	return m_state;
}

void CVTInfo::TradeSetState( TRADE_STATE state )
{
	m_state = state;
}

///////////////////////////////////////////////////////////////////////////////
// 개인상점 
///////////////////////////////////////////////////////////////////////////////

//void CDPClient::OnUnregisterPVendorItem( OBJID objid, CAr & ar )
BOOL CVTInfo::VendorClearItem( BYTE i )
{
	CItemBase* pItemBase = m_apItem_VT[i];
	if( pItemBase )
	{
#ifdef __LEEAEG_SHOP_FINDER
#ifdef __WORLDSERVER
		CAegonShopFinderMng::GetInstance()->RemoveItem((CItemElem*)m_apItem_VT[i]);
#endif // __WORLDSERVER
#endif // __LEEAEG_SHOP_FINDER
		pItemBase->SetExtra( 0 );
		pItemBase->m_nCost = 0;
		m_apItem_VT[i]     = NULL;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// void CDPClient::OnRegisterPVendorItem( OBJID objid, CAr & ar )
#ifdef __CHIP_PLAYERSHOP
void CVTInfo::VendorSetItem(BYTE nId, BYTE i, short nNum, int nCost, BYTE nBuyMode)
#else // __CHIP_PLAYERSHOP
void CVTInfo::VendorSetItem(BYTE nId, BYTE i, short nNum, int nCost)
#endif // __CHIP_PLAYERSHOP
{
#ifdef __VENDING_NPC
	CItemBase* pItemBase = m_pVendor->GetItemId(nId);
#else // __VENDING_NPC
	CItemBase* pItemBase = m_pOwner->GetItemId(nId);
#endif // __VENDING_NPC
	if( pItemBase )
	{
		m_apItem_VT[i] = pItemBase;
		pItemBase->SetExtra( nNum );
		pItemBase->m_nCost = nCost;
#ifdef __CHIP_PLAYERSHOP
		pItemBase->m_nBuyMode = nBuyMode;
#endif // __CHIP_PLAYERSHOP
	}
}

#ifdef __VENDING_NPC
void CVTInfo::VendorExchange()
{
#ifdef __WORLDSERVER
	if (m_idOwner == NULL)
		return;

	CMover* pOwner = (CMover*)g_UserMng.GetUserByPlayerID(m_idOwner);
	if (!pOwner)
		return;

	if (!m_pVendor->m_Inventory.m_apItem)
		m_pVendor->m_Inventory.SetItemContainer(ITYPE_ITEM, MAX_VENDITEM);

	/*pOwner->m_vtInfo.*/
	pOwner->m_vtInfo.VendorGetItems(m_apItem_VT);
	for (int i = 0; i < MAX_VENDITEM; i++)
	{

		if (m_apItem_VT[i])
		{
			CItemBase* pItemBase = m_apItem_VT[i];
			CItemElem* pItemElem = (CItemElem*)pItemBase;
			m_apItem_VT[i] = NULL;

			CItemElem itemElem;
			itemElem = *pItemElem;

			if (itemElem.m_pPet && itemElem.m_pPet == pItemElem->m_pPet)
			{
				Error("itemElem.m_pPet && itemElem.m_pPet == pItemElem->m_pPet");
				SAFE_DELETE(itemElem.m_pPet);
				itemElem.m_pPet = new CPet;
				*(itemElem.m_pPet) = *(pItemElem->m_pPet);
			}

			itemElem.m_nItemNum = pItemBase->GetExtra();

			m_pVendor->m_Inventory.m_apItem[i] = itemElem;
#ifdef __CHIP_PLAYERSHOP
			VendorSetItem((BYTE)i, (BYTE)i, pItemBase->GetExtra(), pItemBase->m_nCost, pItemBase->m_nBuyMode);
#else //__CHIP_PLAYERSHOP
			VendorSetItem((BYTE)i, (BYTE)i, pItemBase->GetExtra(), pItemBase->m_nCost);
#endif //__CHIP_PLAYERSHOP
			pOwner->RemoveItem((BYTE)pItemBase->m_dwObjId, pItemBase->GetExtra());
		}
	}
#endif
}

void CVTInfo::VendorGetItems(CItemBase** ppItems)
{
	for (int i = 0; i < MAX_VENDITEM; i++)
	{
		ppItems[i] = m_apItem_VT[i];
	}

}
#endif // __VENDOR_NPC

//void CDPClient::OnPVendorItemNum( OBJID objid, CAr & ar )
// nNum - 남은 갯수 
void CVTInfo::VendorItemNum( BYTE i, short nNum )
{
	CItemBase* pItemBase = m_apItem_VT[i];
	if( pItemBase )
	{
		pItemBase->SetExtra( nNum );
		if( nNum == 0 )
		{
		#ifdef __CLIENT
#ifdef __VENDING_NPC
			if (m_pVendor->IsActiveMover() == FALSE)
#else // __VENDING_NPC
			if (m_pOwner->IsActiveMover() == FALSE)
#endif // __VENDING_NPC
				SAFE_DELETE( m_apItem_VT[i] );
		#endif
			m_apItem_VT[i] = NULL;
		}
	}
}

// 데이타 카피를 해서 보관?
// void CDPClient::OnPVendorItem( OBJID objid, CAr & ar )
void CVTInfo::VendorCopyItems( CItemBase** ppItemVd )
{
	memcpy( (void*)m_apItem_VT, ppItemVd, sizeof(m_apItem_VT) );
}


#ifdef __VENDING_NPC
void CVTInfo::VendorClose(BOOL bClearTitle, BOOL bClearShop)
#else // __VENDING_NPC
void CVTInfo::VendorClose(BOOL bClearTitle)
#endif // __VENDING_NPC
{

#ifdef __LEEAEG_SHOP_FINDER
#ifdef __WORLDSERVER
	for (int i = 0; i < MAX_VENDITEM; i++)
	{
		if (m_apItem_VT[i])
		{
			CAegonShopFinderMng::GetInstance()->RemoveItem((CItemElem*)m_apItem_VT[i]);
		}
	}
#endif // __WORLDSERVER
#endif // __LEEAEG_SHOP_FINDER

#ifdef __VENDING_NPC
	if (bClearShop)
	{
#endif // __VENDING_NPC
		for( int i = 0; i < MAX_VENDITEM; i++ )
		{
			if( m_apItem_VT[i] )
			{
				m_apItem_VT[i]->SetExtra( 0 );
				m_apItem_VT[i]->m_nCost	= 0;
#ifdef __CLIENT
#ifdef __VENDING_NPC
				if (FALSE == m_pVendor->IsActiveMover())
#else // __VENDING_NPC
				if (FALSE == m_pOwner->IsActiveMover())
#endif // __VENDING_NPC
					SAFE_DELETE( m_apItem_VT[i] );
#endif	// __CLIENT
				m_apItem_VT[i]	= NULL;
			}
		}
#ifdef __VENDING_NPC
	}
#endif // __VENDING_NPC

#ifdef __VENDING_NPC
	if (bClearTitle)
	{
		m_strTitle = "";
		m_tmOpened = 0;
	}
#else // __VENDING_NPC
	if (bClearTitle)
		m_strTitle = "";
#endif // __VENDING_NPC

	SetOther( NULL );	
}


// 나는 판매자 인가? 
BOOL CVTInfo::VendorIsVendor() 
{
	for( int i=0; i<MAX_VENDITEM; ++i )
	{
		if( m_apItem_VT[i] )	// 등록한 아이템이 있는가?
			return TRUE;
	}

	return FALSE;
}

BOOL CVTInfo::IsTrading( CItemElem* pItemElem )
{
	for( int i = 0; i < MAX_VENDITEM; i++ )
	{
		if( m_apItem_VT[i] == pItemElem )
			return TRUE;
	}
	return FALSE;
}

//CDPSrvr::OnBuyPVendorItem
#ifdef __WORLDSERVER
BOOL CVTInfo::VendorSellItem( CMover* pBuyer, BYTE i, DWORD dwItemId, short nNum, VENDOR_SELL_RESULT& result )
{
	result.nRemain    = 0;
	result.nErrorCode = 0;

	if( IsVendorOpen() == FALSE )
		return FALSE;

	CItemBase* pItemBase = m_apItem_VT[i];
	if( IsUsingItem( pItemBase ) == FALSE || pItemBase->m_dwItemId != dwItemId )
		return FALSE;

#ifdef __VENDING_NPC
	if (m_idOwner != NULL && pBuyer->m_idPlayer == m_idOwner)
	{
		if (pBuyer->IsPlayer())
			((CUser*)pBuyer)->AddText("You can't buy your own items.");
		return FALSE;
	}
#endif // __VENDING_NPC

	if( nNum < 1 )
		nNum = 1;
	if( nNum > pItemBase->GetExtra() )
		nNum = (short)pItemBase->GetExtra();

//	06.10.26

#ifdef __CHIP_PLAYERSHOP
	if (pItemBase->m_nBuyMode == 1)
	{
		if (pItemBase->m_nCost > 0 && pBuyer->GetGold() < nNum * pItemBase->m_nCost)
		{
			result.nErrorCode = TID_GAME_LACKMONEY;
			return FALSE;
		}
	}
	else if (pItemBase->m_nBuyMode == 2)
	{
		if (pItemBase->m_nCost > 0 && pBuyer->GetPerinNum() < nNum * pItemBase->m_nCost)
		{
			result.nErrorCode = TID_GAME_LACKMONEY;
			return FALSE;
		}
	}
	else if (pItemBase->m_nBuyMode == 3)
	{
		if (pItemBase->m_nCost > 0 && pBuyer->GetChipNum() < nNum * pItemBase->m_nCost)
		{
			result.nErrorCode = TID_GAME_LACKMONEY;
			return FALSE;
		}
	}
	else if (pItemBase->m_nBuyMode == 4)
	{
		if (pItemBase->m_nCost > 0 && pBuyer->GetChipRPNum() < nNum * pItemBase->m_nCost)
		{
			result.nErrorCode = TID_GAME_LACKMONEY;
			return FALSE;
		}
	}
	else
	{
		result.nErrorCode = TID_GAME_LACKMONEY;
		return FALSE;
	}
#else // __CHIP_PLAYERSHOP
	if( pItemBase->m_nCost > 0 && (float)pBuyer->GetGold() < (float)nNum * (float)pItemBase->m_nCost )
	{
#if __VER >= 8 // __S8_VENDOR_REVISION
		result.nErrorCode = TID_GAME_LACKMONEY;
		return FALSE;
#else // __VER >= 8 // __S8_VENDOR_REVISION
		nNum = (short)( pBuyer->GetGold() / pItemBase->m_nCost );
#endif // __VER >= 8 // __S8_VENDOR_REVISION
	}
#endif //__CHIP_PLAYERSHOP

	if( nNum == 0 )
	{
		result.nErrorCode = TID_GAME_LACKMONEY;
		return FALSE;
	}

	CItemElem* pItemElem = (CItemElem*)pItemBase;
	CItemElem itemElem;
	itemElem	= *pItemElem;
	itemElem.m_nItemNum	 = nNum;

	if( pBuyer->CreateItem( &itemElem ) == FALSE )
	{
		result.nErrorCode = TID_GAME_LACKSPACE;
		return FALSE;
	}

	// CItemElem의 복사 연산자에 m_nCost는 제외되어 있다.
	int nCost	= pItemBase->m_nCost;

#ifdef __VENDING_NPC
	char szText[512];
	sprintf_s<512>(szText, "[%s] purchased : [%d %s]!", pBuyer->GetName(), nNum, pItemBase->GetProp()->szName);

	CMover* pOwner = (CMover*)g_UserMng.GetUserByPlayerID(m_idOwner);

	DWORD dwBuyItem = NULL_ID;
	if (pItemBase->m_nBuyMode == 2) {
		dwBuyItem = II_SYS_SYS_SCR_PERIN;
	}
	else if (pItemBase->m_nBuyMode == 3) {
		dwBuyItem = II_CHP_RED;
	}
	else if (pItemBase->m_nBuyMode == 4) {
		dwBuyItem = II_SYS_REDPERIN;
	}

	int nPrice = pItemBase->m_nCost * nNum;
	if (nPrice <= 0) {
		return FALSE;
	}
	else if (nPrice > INT_MAX) {
		return FALSE;
	}

	CItemElem cBuyItem;
	cBuyItem.m_dwItemId = dwBuyItem;
	if (pItemBase->m_nBuyMode != 1 && dwBuyItem != NULL_ID)
	{
		while (nPrice > SHRT_MAX)
		{
			pBuyer->RemoveItemA(dwBuyItem, SHRT_MAX);
			cBuyItem.m_nItemNum = SHRT_MAX;
			if (m_idOwner == NULL)
			{
				if (m_pVendor->CreateItem(&cBuyItem) == FALSE)
					g_dpDBClient.SendQueryPostMail(m_pVendor->m_idPlayer, 0, cBuyItem, 0, "Vendor NPC", szText);
			}
			else
			{
				if (IsValidObj(pOwner))
				{
					if (pOwner->CreateItem(&cBuyItem) == FALSE)
						g_dpDBClient.SendQueryPostMail(pOwner->m_idPlayer, 0, cBuyItem, 0, "Vendor NPC", szText);
				}
				else
				{
					g_dpDBClient.SendQueryPostMail(m_idOwner, 0, cBuyItem, 0, "Vendor NPC", szText);
			}
		}
			nPrice -= SHRT_MAX;
	}

		pBuyer->RemoveItemA(dwBuyItem, (short)nPrice);
		cBuyItem.m_nItemNum = (short)nPrice;
		if (m_idOwner == NULL)
		{
			if (m_pVendor->CreateItem(&cBuyItem) == FALSE)
				g_dpDBClient.SendQueryPostMail(m_pVendor->m_idPlayer, 0, cBuyItem, 0, "Vendor NPC", szText);
		}
		else
		{
			if (IsValidObj(pOwner))
			{
				if (pOwner->CreateItem(&cBuyItem) == FALSE)
					g_dpDBClient.SendQueryPostMail(pOwner->m_idPlayer, 0, cBuyItem, 0, "Vendor NPC", szText);
			}
			else
			{
				g_dpDBClient.SendQueryPostMail(m_idOwner, 0, cBuyItem, 0, "Vendor NPC", szText);
			}
		}

		if (IsValidObj(pOwner))
		{
			((CUser*)pOwner)->AddText(szText);
		}
	}
	else if (pItemBase->m_nBuyMode == 1)
	{
		pBuyer->AddGold(-(pItemBase->m_nCost * nNum));
		if (m_idOwner == NULL)
		{
			int nGold = m_pVendor->GetGold();
			int nPrice = pItemBase->m_nCost * nNum;
			if ((nGold + nPrice) > nGold)
			{
				m_pVendor->AddGold(nPrice);
			}
			else
			{
				g_dpDBClient.SendQueryPostMail(m_pVendor->m_idPlayer, 0, CItemElem(), nPrice, "Vendor NPC", szText);
			}
		}
		else
		{
			if (IsValidObj(pOwner))
			{
				int nGold = pOwner->GetGold();
				if ((nGold + nPrice) > nGold)
				{
					pOwner->AddGold(nPrice);

				}
				else
				{
					g_dpDBClient.SendQueryPostMail(m_idOwner, 0, CItemElem(), nPrice, "Vendor NPC", szText);
				}
			}
			else
			{
				g_dpDBClient.SendQueryPostMail(m_idOwner, 0, CItemElem(), nPrice, "Vendor NPC", szText);
			}
		}
	}
#else // __VENDING_NPC
#ifdef __CHIP_PLAYERSHOP
	DWORD dwBuyItem = NULL_ID;
	if (pItemBase->m_nBuyMode == 2) {
		dwBuyItem = II_SYS_SYS_SCR_PERIN;
	}
	else if (pItemBase->m_nBuyMode == 3) {
		dwBuyItem = II_CHP_RED;
	}
	else if (pItemBase->m_nBuyMode == 4) {
		dwBuyItem = II_SYS_REDPERIN;
	}

	int nPrice = pItemBase->m_nCost * nNum;
	if (nPrice <= 0) {
		return FALSE;
	}
	else if (nPrice > INT_MAX) {
		return FALSE;
	}

	CItemElem cBuyItem;
	cBuyItem.m_dwItemId = dwBuyItem;
	if (pItemBase->m_nBuyMode != 1 && dwBuyItem != NULL_ID)
	{
		while (nPrice > SHRT_MAX)
		{
			pBuyer->RemoveItemA(dwBuyItem, SHRT_MAX);
			cBuyItem.m_nItemNum = SHRT_MAX;
			if (!m_pOwner->CreateItem(&cBuyItem))
				g_dpDBClient.SendQueryPostMail(m_pOwner->m_idPlayer, NULL, cBuyItem, NULL, "Shop Item", "Your items.");

			nPrice -= SHRT_MAX;
		}

		pBuyer->RemoveItemA(dwBuyItem, (short)nPrice);
		cBuyItem.m_nItemNum = (short)nPrice;
		if (!m_pOwner->CreateItem(&cBuyItem))
			g_dpDBClient.SendQueryPostMail(m_pOwner->m_idPlayer, NULL, cBuyItem, NULL, "Shop Item", "Your items.");
	}
	else if (pItemBase->m_nBuyMode == 1)
	{
		pBuyer->AddGold(-(pItemBase->m_nCost * nNum));
		m_pOwner->AddGold(pItemBase->m_nCost * nNum);
	}
#else // __CHIP_PLAYERSHOP	
	pBuyer->AddGold(-(pItemBase->m_nCost * nNum));
	m_pOwner->AddGold(pItemBase->m_nCost * nNum);
#endif // __CHIP_PLAYERSHOP
#endif // __VENDING_NPC

	pItemBase->SetExtra( pItemBase->GetExtra() - nNum );
	int nRemain = pItemBase->GetExtra();
	if( nRemain <= 0 )
		m_apItem_VT[i] = NULL;


#ifdef __LEEAEG_SHOP_FINDER
#ifdef __WORLDSERVER
	CAegonShopFinderMng::GetInstance()->UpdateItem((CItemElem*)pItemBase, nRemain);
#endif // __WORLDSERVER
#endif // __LEEAEG_SHOP_FINDER

#if __VER >= 11 // __MOD_VENDOR
#ifdef __WORLDSERVER
#ifdef __VENDING_NPC
	g_UserMng.AddPVendorItemNum((CUser*)m_pVendor, i, nRemain, pBuyer->GetName());
#else // __VENDING_NPC
	g_UserMng.AddPVendorItemNum((CUser*)m_pOwner, i, nRemain, pBuyer->GetName());
#endif // __VENDING_NPC
#endif	// __WORLDSERVER
#endif	// __MOD_VENDOR

#ifdef __VENDING_NPC
	m_pVendor->RemoveItem((BYTE)pItemBase->m_dwObjId, nNum);
#else // __VENDING_NPC
	m_pOwner->RemoveItem((BYTE)pItemBase->m_dwObjId, nNum);
#endif // __VENDING_NPC

	result.item = itemElem;
	result.item.m_nCost	= nCost;
	result.nRemain = nRemain;



#ifdef __VENDING_NPC
	if (m_idOwner > 0)
	{
#ifdef __AEGON_ACHIEVEMENT_SYSTEM
		CUser* pOwnerTMP = (CUser*)prj.GetUserByID(m_idOwner);
		if (IsValidObj(pOwnerTMP))
		{
			CAegonAchievementMng::GetInstance()->AddAchievementProgressForType(pOwnerTMP, AEGACH_VENDSELLS, 1);
		}
		else 
		{
			IncreaseAchievementSellCount();
		}
#endif // __AEGON_ACHIEVEMENT_SYSTEM

		for (int j = 0; j < MAX_VENDITEM; j++)
		{
			if (m_apItem_VT[j])
			{
				g_dpDBClient.SendUpdateNPCVendor(m_pVendor);
				return TRUE;
			}
		}

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
		CAegonAchievementMng::GetInstance()->CollectOfflineVendorSellCount(m_pVendor);
#endif // __AEGON_ACHIEVEMENT_SYSTEM

		CUser* pOwner = g_UserMng.GetUserByPlayerID(m_idOwner);
		ItemProp* pItemProp = prj.GetItemProp(m_pVendor->m_dwVendorItemID);
		if (IsValidObj(pOwner))
		{
			pOwner->AddText("All items from your offline vendor were sold!");

			g_UserMng.AddCreateSfxObj(m_pVendor, XI_GEN_MONSTER_SPAWN01);

			//if (pItemProp && pItemProp->dwAbilityMax == 3)
			{
				CItemElem itemVendor;
				itemVendor.m_dwItemId = m_pVendor->m_dwVendorItemID;
				itemVendor.m_nItemNum = 1;
				itemVendor.SetSerialNumber();
				itemVendor.m_dwKeepTime = m_pVendor->m_dwVendorTime - time(0);

				if (pOwner && pOwner->CreateItem(&itemVendor) == FALSE)
					g_dpDBClient.SendQueryPostMail(pOwner->m_idPlayer, 0, itemVendor, 0, pItemProp->szName, "Your vendor returned!");

				pOwner->AddText("Your vendor returned!");
			}
		}
		else
		{
			//if (pItemProp && pItemProp->dwAbilityMax == 3)
			{
				CItemElem itemVendor;
				itemVendor.m_dwItemId = m_pVendor->m_dwVendorItemID;
				itemVendor.m_nItemNum = 1;
				itemVendor.SetSerialNumber();
				itemVendor.m_dwKeepTime = m_pVendor->m_dwVendorTime - time(0);

				g_dpDBClient.SendQueryPostMail(m_idOwner, 0, itemVendor, 0, pItemProp->szName, "Your vendor returned!");
			}
		}

		((CUser*)pBuyer)->m_vtInfo.SetOther(NULL);
		((CUser*)pBuyer)->AddPVendorRemove(m_pVendor->GetId());

		prj.RemoveVendorNpcFromGlobal(m_pVendor->m_idVendorNPC);
		g_dpDBClient.SendRemoveNPCVendor(m_pVendor);
		g_DPCoreClient.SendRemoveNPCVendor(m_pVendor);

		if (pOwner)
			pOwner->AddNpcVendorUnlink(m_pVendor->m_dwVendorItemID);

		m_pVendor->Delete();
	}
#endif //__VENDING_NPC
#ifdef __AEGON_ACHIEVEMENT_SYSTEM
	else 
	{
		if(m_pVendor->IsPlayer())
			CAegonAchievementMng::GetInstance()->AddAchievementProgressForType((CUser*)m_pVendor, AEGACH_VENDSELLS, 1);
	
	}
#endif // __AEGON_ACHIEVEMENT_SYSTEM

	return TRUE;
}
#endif // __WORLDSERVER