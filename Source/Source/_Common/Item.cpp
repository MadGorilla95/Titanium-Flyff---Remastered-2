#include "stdafx.h"
#include "Item.h"
#include "defineObj.h"
#include "CreateObj.h"

#if __VER >= 11 // __SYS_IDENTIFY
#include "randomoption.h"
#ifdef __WORLDSERVER
#include "ticket.h"
#endif	// __WORLDSERVER
#endif	// __SYS_IDENTIFY

#include "serialnumber.h"
#if __VER >= 9
#include "definetext.h"
#endif	// 

BOOL IsUsableItem( CItemBase* pItem )
{
	if( pItem == NULL )
		return FALSE;
	
#ifdef __GAMEGUARD
	if (!pItem->GetProp())
		return FALSE;
#endif // __GAMEGUARD

	if( pItem->GetExtra() == 0 )
		return TRUE;
	else
		return FALSE;	// 거래중이거나 개인 상점에 올린 아이템은 사용할 수 없다.
}

BOOL IsUsingItem( CItemBase* pItem )
{
	if( pItem == NULL )
		return FALSE;
	
	if( pItem->GetExtra() != 0 )
		return TRUE;
	else
		return FALSE;	//        .
}
//////////////////////////////////////////////////////////////////////
// CItemBase
//////////////////////////////////////////////////////////////////////
CItemBase::CItemBase() 
{ 
	m_szItemText[0]  = '\0';
	m_dwItemId	     = 0;
	m_dwObjId	     = NULL_ID;
	m_dwObjIndex	 = NULL_ID;
	m_nExtra	     = 0;
	m_nCost		     = 0;
	m_liSerialNumber	= 0;
	SetTexture( NULL );	

#ifdef __CHIP_PLAYERSHOP
	m_nBuyMode = 0;
#endif // __CHIP_PLAYERSHOP

#ifdef __LEEAEG_SHOP_FINDER
	m_shopItemIndex = 0;
	b_shopFinderItem = FALSE;
#endif // __LEEAEG_SHOP_FINDER

}

void CItemBase::Empty()
{
	m_szItemText[0] = '\0';
	m_dwItemId = 0;
	m_nExtra   = 0;
	SetTexture( NULL );
}

ItemProp* CItemBase::GetProp() 
{ 
	return	prj.GetItemProp( m_dwItemId );	
}

CItemBase& CItemBase::operator = ( CItemBase& ib )
{
	_tcscpy( m_szItemText, ib.m_szItemText );
	m_dwItemId	      = ib.m_dwItemId  ;
	m_liSerialNumber	= ib.m_liSerialNumber;

#ifdef __CLIENT
	m_pTexture        = ib.m_pTexture;
#endif

#ifdef __LEEAEG_SHOP_FINDER
	m_shopItemIndex = ib.m_shopItemIndex;
	b_shopFinderItem = ib.b_shopFinderItem;
#endif // __LEEAEG_SHOP_FINDER

	return *this;
}

void CItemBase::SetTexture()
{
#ifdef __CLIENT
	ItemProp* pProp =GetProp();
	if (!pProp)
	{
		LPCTSTR szErr = Error("CItemBase::SetTexture GetProp() NULL Return %d", m_dwItemId);
		ADDERRORMSG( szErr );
#ifdef __SECURITY_FIXES
		return;
#endif // __SECURITY_FIXES
	}

#if __VER >= 9	// __PET_0410
	CString strIcon	= pProp->szIcon;
	if( pProp->dwItemKind3 == IK3_EGG )
	{
		BYTE nLevel	= 0;
		if( ( (CItemElem*)this )->m_pPet )
			nLevel	= ( (CItemElem*)this )->m_pPet->GetLevel();
		switch( nLevel )
		{
			case PL_D:
			case PL_C:
				strIcon.Replace( ".", "_00." );
				break;
			case PL_B:
			case PL_A:
				strIcon.Replace( ".", "_01." );
				break;
			case PL_S:
				strIcon.Replace( ".", "_02." );
				break;
		}
	}
	m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, strIcon ), COLOR_ITEM_BG );
#else	// __PET_0410
	m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, GetProp()->szIcon), COLOR_ITEM_BG );
#endif	// __PET_0410
#endif
}

//
// 아이템 가격을 계산해서 리턴한다.
// -1을 리턴하면 처리해선 안된다.
//
int	CItemBase::GetCost( void )
{
	ItemProp *pProp = GetProp();
	if( pProp == NULL )		
		return -1;
	if( pProp->dwCost == 0xFFFFFFFF )	
		return -1;
		
	CItemElem *pItemElem = (CItemElem *)this;

	int nCost;
	if( pItemElem->GetAbilityOption() )
	{		
		//INT((아이템가격+아이템가격*(0.1+(아이템강화레벨*아이템강화레벨)/18))
		nCost = (int)( pProp->dwCost + pProp->dwCost * ( 0.1f + ( pItemElem->GetAbilityOption() * pItemElem->GetAbilityOption() ) / 18.0f ) );
	}
	else
	{
		nCost = (int)pProp->dwCost;
	}
	
	return nCost;
}

#if __VER >= 11 // __GUILDCOMBATCHIP
#ifdef __SQK_MULTIALTERNATEPRICE
int CItemBase::GetCurrencyCost(DWORD currency) {
	ItemProp* itemProp = GetProp();
	if (!itemProp)
		return -1;

	return itemProp->alternatePrices.GetPrice(currency);
}
#else //__SQK_MULTIALTERNATEPRICE
DWORD CItemBase::GetChipCost()
{
	ItemProp *pProp = GetProp();
	if( pProp == NULL )		
		return -1;
	if( pProp->dwReferValue1 == 0xFFFFFFFF )	
		return -1;

	return pProp->dwReferValue1;
}
#endif //__SQK_MULTIALTERNATEPRICE
#endif // __GUILDCOMBATCHIP

// 퀘스트 아이템인가?
BOOL CItemBase::IsQuest()
{
	ItemProp* p = GetProp();
	if(p && p->dwItemKind3 == IK3_QUEST )
	{
		if( ::GetLanguage() == LANG_JAP )
		{
			if( p->dwID == II_SYS_SYS_QUE_REDSOCKS )	// 예외 
				return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CItemElem - 아이템 하나하나의 요소를 말한다.
//////////////////////////////////////////////////////////////////////
/*
	1 ~ 99    
	100 ~ 유저 지정 로고

	아이템에 로고 번호.

*/

CItemElem::CItemElem()
{
	m_idGuild = 0;
	m_nItemNum = 1;
	m_nAbilityOption = 0;
	m_nRepairNumber	= 0;
	m_nHitPoint		= -1;
	m_nRepair	= 0;
	m_byFlag	= 0;
#ifdef __CLIENT
	m_bRepair	= FALSE;
#endif	// __CLIENT

    m_bItemResist = SAI79::NO_PROP;
	m_nResistAbilityOption = 0;	
	m_nResistSMItemId = 0;

#ifdef __WORLDSERVER
	m_bQuery	= FALSE;
#endif	// __WORLDSERVER

//	memset( &m_piercingInfo, 0, sizeof(m_piercingInfo) );
	m_bCharged	= FALSE;
	m_dwKeepTime	= 0;
#if __VER >= 11 // __SYS_IDENTIFY
	m_iRandomOptItemId	= 0;
#else	// __SYS_IDENTIFY
	m_nRandomOptItemId	= 0;
#endif	// __SYS_IDENTIFY


	//	mulcom	BEGIN100405	각성 보호의 두루마리
	m_n64NewRandomOption	= 0;
	//	mulcom	END100405	각성 보호의 두루마리


/*
#ifdef __XPET2
	m_nMaxLevel = 0;	
	m_nLevel = 0;		
	m_dwHungry = 0;		
	m_dwFeeling = 0;	
#endif // xPet2
*/
	/*
#ifdef __GIFTBOX0213
#ifdef __WORLDSERVER
	m_nQueryGiftbox	= 0;
#endif	// __WORLDSERVER
#endif	// __GIFTBOX0213
	*/
#if __VER >= 9	// __PET_0410
	m_pPet	= NULL;
#endif	// __PET_0410
#if __VER >= 15 // __PETVIS
	m_bTranformVisPet = FALSE;
#endif // __PETVIS

#ifdef __LOOKCHANGE
	m_dwLookId = NULL_ID;
#endif // __LOOKCHANGE

#ifdef __SYS_LOCK_ITEM
	SetItemLock(false);
#endif // __SYS_LOCK_ITEM

#ifdef __AEGON_RETURN_AWAKE
	m_iOldRandomOptItemId = 0;
#endif // __AEGON_RETURN_AWAKE

#ifdef __VENDING_NPC
	m_nCost = 0;
#endif //__VENDING_NPC

#ifdef __WEAPON_RARITY
	m_nWeaponRarity = 0;
#endif // __WEAPON_RARITY

#ifdef __UPGRADE_SUCCESS
	m_nUpgradeTries = 0;
#endif // __UPGRADE_SUCCESS

#ifdef __WEAPON_MERGE
	m_nMergedWeaponATK = 0;
	m_iMergedRandomOptItemId = 0;
#endif //__WEAPON_MERGE

#ifdef __BUYBACK
	m_nBuyBackPrice = 0;
	m_nBuyBackTaxes = 0;
	m_nBuyBackCont = 0;
#endif // __BUYBACK

#ifdef __PET_GLOW
	m_nPetGlow = 0;
#endif // __PET_GLOW

#ifdef __CRYSTAL
	m_dwCrystal = NULL_ID;
#endif // __CRYSTAL
}

CItemElem::~CItemElem()
{
#if __VER >= 9	// __PET_0410
#ifdef __LEEAEG_SHOP_FINDER
	if(!b_shopFinderItem)
		SAFE_DELETE(m_pPet);
#else // __LEEAEG_SHOP_FINDER
	SAFE_DELETE( m_pPet );
#endif // __LEEAEG_SHOP_FINDER
#endif	// __PET_0410
}

void	CItemElem::Empty( void )
{
	CItemBase::Empty();
#if __VER >= 9
	SAFE_DELETE( m_pPet );
#endif	// __PET_0410
	m_piercing.Clear();
}

// 돈의 양을 얻는다. 
int	CItemElem::GetGold()
{
	ASSERT( GetProp()->dwItemKind1 == IK1_GOLD );
	return m_nHitPoint;		// int형으로 돈을 버리게 하기 위해서 이 변수를 사용 
}

void CItemElem::UseItem()
{
#ifdef __GAMEGUARD
	if (GetProp() && GetProp()->bPermanence != TRUE)
#else // __GAMEGUARD
	if (GetProp()->bPermanence != TRUE)
#endif // __GAMEGUARD
	{
		m_nItemNum--;
	}
}

CItemElem& CItemElem::operator =( CItemElem & ie )
{
	CItemBase::operator =( ie );

	SetTexture( ie.GetTexture() );

	m_nItemNum             = ie.m_nItemNum;
	m_nRepairNumber        = ie.m_nRepairNumber;
	m_nHitPoint            = ie.m_nHitPoint;
	m_nRepair				= ie.m_nRepair;
	m_byFlag               = ie.m_byFlag;
	m_nAbilityOption       = ie.m_nAbilityOption;
	m_idGuild              = ie.m_idGuild;
	m_bItemResist          = ie.m_bItemResist;
	m_nResistAbilityOption = ie.m_nResistAbilityOption;
	m_nResistSMItemId      = ie.m_nResistSMItemId;
	m_dwKeepTime           = ie.m_dwKeepTime;
	m_piercing	= ie.m_piercing;
	m_bCharged	           = ie.m_bCharged;
#if __VER >= 11 // __SYS_IDENTIFY
	m_iRandomOptItemId     = ie.GetRandomOptItemId();
#else	// __SYS_IDENTIFY
	m_nRandomOptItemId     = ie.m_nRandomOptItemId;
#endif	// __SYS_IDENTIFY

	//	mulcom	BEGIN100405	각성 보호의 두루마리
	m_n64NewRandomOption	= ie.GetNewRandomOption();
	//	mulcom	END100405	각성 보호의 두루마리

#if __VER >= 9	// __PET_0410
	SAFE_DELETE( m_pPet );
	if( ie.m_pPet )
	{
		m_pPet	= new CPet;
		*m_pPet	= *ie.m_pPet;
	}
#endif	// __PET_0410
#if __VER >= 15 // __PETVIS
	m_bTranformVisPet		= ie.m_bTranformVisPet;
#endif // __PETVIS

#ifdef __LOOKCHANGE
	m_dwLookId				= ie.m_dwLookId;
#endif //__LOOKCHANGE

#ifdef __SYS_LOCK_ITEM
	SetItemLock(ie.IsItemLocked());
#endif // __SYS_LOCK_ITEM

#ifdef __AEGON_RETURN_AWAKE
	m_iOldRandomOptItemId = ie.GetOldRandomOptItemId();
#endif // __AEGON_RETURN_AWAKE

#ifdef __WEAPON_RARITY
	m_nWeaponRarity = ie.GetWeaponRarity();
#endif // __WEAPON_RARITY

#ifdef __UPGRADE_SUCCESS
	m_nUpgradeTries = ie.m_nUpgradeTries;
#endif // __UPGRADE_SUCCESS

#ifdef __WEAPON_MERGE
	m_nMergedWeaponATK = ie.GetMergedWeaponATK();
	m_iMergedRandomOptItemId = ie.GetMergedRandomOptItemId();
#endif //__WEAPON_MERGE

#ifdef __BUYBACK
	m_nBuyBackPrice = ie.m_nBuyBackPrice;
	m_nBuyBackTaxes = ie.m_nBuyBackTaxes;
	m_nBuyBackCont = ie.m_nBuyBackCont;
#endif // __BUYBACK

#ifdef __PET_GLOW
	m_nPetGlow = ie.m_nPetGlow;
#endif // __PET_GLOW

#ifdef __CRYSTAL
	m_dwCrystal = ie.GetCrystal();
#endif // __CRYSTAL

	return *this;
}

// 유료아이템인가?
BOOL CItemElem::IsCharged()
{
	if( m_bCharged == 1 || m_nResistSMItemId != 0 )
		return TRUE;

	return FALSE;
}

void CItemElem::GetPiercingAvail( PPIERCINGAVAIL pPiercingAvail )
{
	for( int i = 0; i < GetPiercingSize(); i++ )
	{
		PPIERCINGAVAIL ptr	= CPiercingAvail::GetInstance()->GetPiercingAvail( GetPiercingItem( i ) );
		if( ptr )
		{
			for( int j = 0; j < ptr->nSize; j++ )
			{
				int nFind	= -1;
				for( int k = 0; k < pPiercingAvail->nSize; k++ )
				{
					if(  pPiercingAvail->anDstParam[k] == ptr->anDstParam[j] )
					{
						nFind	= k;
						break;
					}
				}
				if( nFind < 0 )
					nFind	= pPiercingAvail->nSize++;
				pPiercingAvail->anDstParam[nFind]		= ptr->anDstParam[j];
				pPiercingAvail->anAdjParam[nFind]		+= ptr->anAdjParam[j];
			}
		}
	}
}

#ifdef __WEAPON_MERGE
void CItemElem::GetMergedPiercingAvail(PPIERCINGAVAIL pPiercingAvail)
{
	for (int i = 0; i < GetMergedPiercingSize(); i++)
	{
		PPIERCINGAVAIL ptr = CPiercingAvail::GetInstance()->GetPiercingAvail(GetMergedPiercingItem(i));
		if (ptr)
		{
			for (int j = 0; j < ptr->nSize; j++)
			{
				int nFind = -1;
				for (int k = 0; k < pPiercingAvail->nSize; k++)
				{
					if (pPiercingAvail->anDstParam[k] == ptr->anDstParam[j])
					{
						nFind = k;
						break;
					}
				}
				if (nFind < 0)
					nFind = pPiercingAvail->nSize++;
				pPiercingAvail->anDstParam[nFind] = ptr->anDstParam[j];
				pPiercingAvail->anAdjParam[nFind] += ptr->anAdjParam[j];
			}
		}
	}
}
#endif //__WEAPON_MERGE

// 주사위로 제련 가능한 아이템 종류
BOOL CItemElem::IsDiceRefineryAble( ItemProp* pProp )
{
	if( !pProp )
		return FALSE;

#ifdef __NEW_ITEM_VARUNA
	if( pProp->IsBaruna() )
		return FALSE;
#endif // __NEW_ITEM_VARUNA

	if( pProp->dwItemKind2 == IK2_ARMOR ||
		pProp->dwItemKind2 == IK2_WEAPON_MAGIC ||
		pProp->dwItemKind2 == IK2_WEAPON_DIRECT ||
		pProp->dwItemKind2 == IK2_ARMORETC )
		return TRUE;
					
	return FALSE;
}

// 카드로 제련 가능한 아이템 종류
BOOL CItemElem::IsEleRefineryAble( ItemProp* pProp )
{
	if( !pProp )
		return FALSE;

#ifdef __NEW_ITEM_VARUNA
	if( pProp->IsBaruna() )
		return FALSE;
#endif // __NEW_ITEM_VARUNA

	if( pProp->dwItemKind3 == IK3_SUIT ||
		pProp->dwItemKind2 == IK2_WEAPON_MAGIC ||
		pProp->dwItemKind2 == IK2_WEAPON_DIRECT )
		return TRUE;
	
	return FALSE;
}

#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
BOOL CItemElem::IsElementalCard( const DWORD dwItemID )
{
	switch( dwItemID )
	{
	case II_GEN_MAT_ELE_FLAME: case II_GEN_MAT_ELE_RIVER: case II_GEN_MAT_ELE_GENERATOR: case II_GEN_MAT_ELE_DESERT: case II_GEN_MAT_ELE_CYCLON:
		return TRUE;
	default:
		return FALSE;
	}
}
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY

BOOL CItemElem::IsBinds( void )
{
	ItemProp* pProperty = GetProp();

#ifdef __GAMEGUARD
	if (!pProperty)
		return FALSE;
#endif // __GAMEGUARD

	if( m_dwKeepTime && pProperty->dwItemKind2 != IK2_WARP )
		return TRUE;

	if( (pProperty->dwFlag & IP_FLAG_BINDS) == IP_FLAG_BINDS )
		return TRUE;

	if( IsFlag( CItemElem::binds ) )
		return TRUE;

#if __VER >= 11 // __SYS_IDENTIFY
#ifndef __BLESSINGS_TRADABLE
	if (g_xRandomOptionProperty->GetRandomOptionSize(GetRandomOptItemId()) > 0
		&& (g_xRandomOptionProperty->GetRandomOptionKind(this) == CRandomOptionProperty::eBlessing || g_xRandomOptionProperty->GetRandomOptionKind(this) == CRandomOptionProperty::eEatPet))
		return TRUE;
#endif //__BLESSINGS_TRADABLE
	if( GetLevelDown() < 0 )
		return TRUE;
#endif	// __SYS_IDENTIFY

	return FALSE;
}

BOOL CItemElem::IsUndestructable( void )
{
	ItemProp* pProperty = GetProp();
#ifdef __GAMEGUARD
	if (pProperty && (pProperty->dwFlag & IP_FLAG_UNDESTRUCTABLE) == IP_FLAG_UNDESTRUCTABLE)
#else // __GAMEGUARD
	if ((pProperty->dwFlag & IP_FLAG_UNDESTRUCTABLE) == IP_FLAG_UNDESTRUCTABLE)
#endif // __GAMEGUARD
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CItemElem::IsLogable( void )
{
#ifdef __GAMEGUARD
	if (GetProp() && (GetProp()->nLog != -1 || GetRandomOptItemId() != 0))
#else // __GAMEGUARD
	if (GetProp()->nLog != -1 || GetRandomOptItemId() != 0)
#endif // __GAMEGUARD
	{
		return TRUE;
	}
	return FALSE;
}

#ifndef __VM_0820
#ifndef __MEM_TRACE
#ifdef __WORLDSERVER
	#ifdef __VM_0819
	CItemPool*	CItem::m_pPool	= new CItemPool( 512, "CItem" );
	#else	// __VM_0819
	CItemPool*	CItem::m_pPool	= new CItemPool( 512 );
	#endif	// __VM_0819
#else	// __WORLDSERVER
	#ifdef __VM_0819
	CItemPool*	CItem::m_pPool	= new CItemPool( 128, "CItem" );
	#else	// __VM_0819
	CItemPool*	CItem::m_pPool	= new CItemPool( 128 );
	#endif	// __VM_0819
#endif	// __WORLDSERVER
#endif	// __MEM_TRACE
#endif	// __VM_0820

//////////////////////////////////////////////////////////////////////
// CItem - 이것은 게임 월드 상에 실제로 등장하는 오브젝트 아이템이다. 
//////////////////////////////////////////////////////////////////////
CItem::CItem()
{
	m_dwType = OT_ITEM;
	m_pItemBase = NULL;
	m_idHolder	= 0;
	m_idOwn = NULL_ID;
	m_dwDropTime = 0;
	m_bDropMob = 0;
	m_dwDropTime = timeGetTime();
#ifdef __CLIENT
	m_fGroundY = 0;
	m_vDelta.x = m_vDelta.y = m_vDelta.z = 0;
#endif //__CLIENT
#ifdef __EVENT_MONSTER
	m_IdEventMonster = NULL_ID;
#endif // __EVENT_MONSTER
}

void CItem::SetOwner( OBJID id )
{
	m_idOwn = id;	// 이 아이템의 소유가 pAttacker(어태커)꺼란걸 표시.
	m_dwDropTime = ::timeGetTime();	// 드랍 했을당시의 시간을 기록함.
	m_bDropMob = TRUE;		// 몹이 죽어서 떨군 돈은 표시를 해둠.
}


CItem::~CItem()
{
	SAFE_DELETE( m_pItemBase );
	if( GetWorld() )
	{
		#if !defined(__CLIENT)
		#ifdef __LAYER_1021
			GetWorld()->m_respawner.Increment( GetRespawn(), m_nRespawnType, FALSE, GetLayer() );
		#else	// __LAYER_1021
			GetWorld()->m_respawner.Increment( GetRespawn(), m_nRespawnType, FALSE );
		#endif	// __LAYER_1021
		#endif
	}
}

BOOL CItem::Read( CFileIO* pFile )
{
	CObj::Read( pFile );
	return TRUE;
}

#ifdef __CLIENT
// 아이템을 생성시킬땐 이것을 불러줘야 중력에 의해 떨어진다.
void CItem::SetDelta( float fGroundY, D3DXVECTOR3 &vDelta )
{
	m_fGroundY = fGroundY;
	m_vDelta = vDelta;
}
#endif // __CLIENT

void CItem::Process()
{
	CCtrl::Process();
#ifdef __CLIENT
	AddAngle( 0.5f );
	D3DXVECTOR3 vPos = GetPos();

	//--- 비행몹에게 떨어진 좌표는 이처리를 해선 안된다. 
	if( m_fGroundY )	// 이게 0이면 중력처리를 해선 안된다.
	{
		if( vPos.y > m_fGroundY )		// 공중에 떠있느냐?
		{
			m_vDelta.y -= 0.0075f;		// 이동벡터에 중력 벡터 더함.
		} 
		else
		{
			vPos.y = m_fGroundY;		// 바닥에 닿은 상태면 지면좌표와 동일하게 맞춤.
			m_vDelta.x = m_vDelta.y = m_vDelta.z = 0;	// 이동벡터는 없어짐. 튀기게 하려면 이렇게 하면 안된다.
			SetPos( vPos );				// 최종 좌표 세팅.
		}

		// 이동벡터가 없으면 더해줄필요 없다.
		if( m_vDelta.x == 0 && m_vDelta.y == 0 && m_vDelta.z == 0 )
		{
			// 현재 좌표가 변경될 필요 없다.
		} 
		else
		{
			m_vDelta.x = m_vDelta.z = 0;		// 여기에 값 넣지말것.
			vPos += m_vDelta;			// 이동 벡터를 더함.
			SetPos( vPos );
		}
	}
#endif // __CLIENT

#ifdef __WORLDSERVER
	if( g_eLocal.GetState( EVE_SCHOOL ) )
		return;
	if( (int)g_tmCurrent - (int)m_dwDropTime > MIN( 3 ) )
		Delete();
#endif	// __WORLDSERVER
}

void CItem::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
#ifndef __WORLDSERVER
	ItemProp *pItemProp = GetProp();
	if( pItemProp && pItemProp->nReflect > 0 )
	{
		((CModelObject*)m_pModel)->SetEffect( 0, XE_REFLECT );	
	}

	CObj::Render( pd3dDevice );

#ifdef __WEAPON_RARITY	
	// Weapon Rarity > Floor Item SFX
	CItemElem* const itemElem = dynamic_cast<CItemElem*>(GetItemBase());
	WeaponRarity* nWeaponRarity = prj.m_nWeaponRarity.GetAt(itemElem->GetWeaponRarity());
	if (nWeaponRarity && itemElem && itemElem->GetWeaponRarity() > 0)
	{
		if (itemElem->GetWeaponRarity() == 0)  // Common 
		{
			CreateSfx(pd3dDevice, XI_GEN_ITEM_RARITY_COMMON, GetPos());
		}
		else if (itemElem->GetWeaponRarity() == 1) // Precious
		{
			CreateSfx(pd3dDevice, XI_GEN_ITEM_RARITY_PRECIOUS, GetPos());
		}
		else if (itemElem->GetWeaponRarity() == 2) // Rare
		{
			CreateSfx(pd3dDevice, XI_GEN_ITEM_RARITY_RARE, GetPos());
		}
		else if (itemElem->GetWeaponRarity() == 3) // Epic
		{
			CreateSfx(pd3dDevice, XI_GEN_ITEM_RARITY_EPIC, GetPos());
		}
		else if (itemElem->GetWeaponRarity() == 4) // Legendary
		{
			CreateSfx(pd3dDevice, XI_GEN_ITEM_RARITY_LEGENDARY, GetPos());
		}
		else if (itemElem->GetWeaponRarity() == 5) // Mystique
		{
			CreateSfx(pd3dDevice, XI_GEN_ITEM_RARITY_MYSTIQUE, GetPos());
		}
	}
	else
#endif //__WEAPON_RARITY
	{
		if (xRandom(50) == 1)
			CreateSfx(pd3dDevice, XI_GEN_ITEM_SHINE01, GetPos());
	}
#endif
}

void CItem::RenderName(LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor)
{
#ifndef __WORLDSERVER
#ifdef __GAMEGUARD
	if (!GetProp())
		return;
#endif // __GAMEGUARD

#ifdef __SYS_GROUND_ITEM_TEXT
	const ItemProp* const  iProp = GetProp();
	CItemElem* const itemElem = dynamic_cast<CItemElem*>(GetItemBase());
	std::string tempString;

	if (iProp && itemElem)
	{
#ifdef __WEAPON_RARITY
		WeaponRarity* nWeaponRarity = prj.m_nWeaponRarity.GetAt(itemElem->GetWeaponRarity());
		if (nWeaponRarity && itemElem->GetWeaponRarity() > 0)
		{
			dwColor = nWeaponRarity->m_dwColor;
		}
		else
#endif //__WEAPON_RARITY
		{
			dwColor = D3DCOLOR_XRGB(255, 255, 255);

			if (iProp->dwItemKind1 == IK1_WEAPON)
			{
				if (iProp->dwReferStat1 == WEAPON_UNIQUE)
					dwColor = D3DCOLOR_XRGB(0, 93, 0);
				else if (iProp->dwReferStat1 == WEAPON_ULTIMATE)
					dwColor = D3DCOLOR_XRGB(127, 0, 255);
				else
					dwColor = D3DCOLOR_XRGB(46, 112, 169);
			}
			if (iProp->dwItemKind1 == IK1_ARMOR && (iProp->dwItemKind2 == IK2_ARMOR || iProp->dwItemKind2 == IK2_ARMORETC))
			{
				dwColor = D3DCOLOR_XRGB(0, 204, 102);
			}
			if (iProp->dwItemKind2 == IK2_JEWELRY)
			{
				dwColor = D3DCOLOR_XRGB(51, 204, 255);
			}
			if (iProp->dwItemKind2 == IK2_CLOTH || iProp->dwItemKind2 == IK2_CLOTHETC)
			{
				dwColor = D3DCOLOR_XRGB(153, 84, 32);
			}
			if (iProp->dwItemKind3 == IK3_SOCKETCARD2 || iProp->dwItemKind3 == IK3_SOCKETCARD)
			{
				dwColor = D3DCOLOR_XRGB(255, 149, 79);
			}
			if (iProp->dwItemKind3 == IK3_VIS)
			{
				dwColor = D3DCOLOR_XRGB(255, 175, 229);
			}
			if (iProp->dwItemKind2 == IK2_GEM || iProp->dwItemKind3 == IK3_GEM)
			{
				dwColor = D3DCOLOR_XRGB(107, 102, 255);
			}
			if (iProp->dwItemKind2 == IK2_FOOD || iProp->dwItemKind2 == IK2_POTION || iProp->dwItemKind2 == IK2_REFRESHER || iProp->dwItemKind3 == IK3_DRINK || iProp->dwItemKind3 == IK3_COOKING || iProp->dwItemKind3 == IK3_REFRESHER)
			{
				dwColor = D3DCOLOR_XRGB(159, 255, 81);
			}
			if (iProp->dwID == II_GEN_MAT_ORICHALCUM01 || iProp->dwID == II_GEN_MAT_ORICHALCUM02 || iProp->dwID == II_GEN_MAT_ORICHALCUM01_1 || iProp->dwID == II_SYS_SYS_SCR_SCRAPORICHALCUM ||
				iProp->dwID == II_GEN_MAT_MOONSTONE || iProp->dwID == II_GEN_MAT_MOONSTONE_1 || iProp->dwID == II_SYS_SYS_SCR_SCRAPMOONSTONE)
			{
				dwColor = D3DCOLOR_XRGB(192, 165, 255);
			}
		}

		if (iProp->dwParts != static_cast<DWORD>(-1))
		{
			PRANDOMOPTITEM pRandomOptItem = CRandomOptItemGen::GetInstance()->GetRandomOptItem(itemElem->GetRandomOpt());
			if (pRandomOptItem)
				tempString = pRandomOptItem->pszString;
		}

#ifdef __WEAPON_RARITY
		CString strRarity;

		if (nWeaponRarity && itemElem->GetWeaponRarity() > 0)
			strRarity.Format("[%s] %s", nWeaponRarity->m_szName, iProp->szName);
		else
			strRarity.Format("%s", iProp->szName);

		tempString += strRarity;
#else //__WEAPON_RARITY
		tempString += iProp->szName;
#endif //__WEAPON_RARITY

		if (itemElem->m_nItemNum > 1)
		{
			tempString += " (";
			tempString += std::to_string(itemElem->m_nItemNum);
			tempString += ")";
		}

	}
	if (tempString.empty())
		return;
#endif //__SYS_GROUND_ITEM_TEXT

	D3DXVECTOR3 vOut, vPos = GetPos(), vPosHeight;
	D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();
	pd3dDevice->GetViewport(&vp);
	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matRotation);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);

	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;

#ifdef __SYS_GROUND_ITEM_TEXT
	vPosHeight.y += 0.3f;
#endif //__SYS_GROUND_ITEM_TEXT

	D3DXVec3Project(&vOut, &vPosHeight, &vp, &GetWorld()->m_matProj, &GetWorld()->m_pCamera->m_matView, &matWorld);
	vOut.x -= pFont->GetTextExtent(m_pItemBase->GetProp()->szName).cx / 2;

#ifndef __SYS_GROUND_ITEM_TEXT
	pFont->DrawText(vOut.x + 1, vOut.y + 1, 0.9f, 0.9f, 0xff000000, tempString.c_str());
	pFont->DrawText(vOut.x, vOut.y, 0.9f, 0.9f, dwColor, tempString.c_str());
#else //__SYS_GROUND_ITEM_TEXT
	if (g_Option.groundItemText == TRUE)
	{
		pFont->DrawText(vOut.x, vOut.y, dwColor, tempString.c_str());
	}
#endif //__SYS_GROUND_ITEM_TEXT

	return;
#endif //__WORLDSERVER
}

void	CItemBase::SetSerialNumber( void )
{
	m_liSerialNumber	= CSerialNumber::GetInstance()->Get();
}

CString CItemElem::GetName( void )
{
	ItemProp* pProp	= GetProp();
#ifdef __GAMEGUARD
	if (!pProp)
	{
		Error("Invalid item name : Item ID = %d", m_dwItemId);
		return _T("CItemElem::GetName");
	}
#endif // __GAMEGUARD
	CString strName		= pProp->szName;
#if __VER >= 9
	if( pProp->dwItemKind3 == IK3_EGG && m_pPet /*&& m_pPet->GetLevel() > PL_EGG*/ )
	{
		MoverProp* pMoverProp	= prj.GetMoverProp( m_pPet->GetIndex() );
		if( pMoverProp )
		{
#ifdef __PET_1024
			if( m_pPet->HasName() )
				strName.Format( prj.GetText( TID_GAME_CAGE_STRING ), m_pPet->GetName() );
			else
#endif	// __PET_1024
				strName.Format( prj.GetText( TID_GAME_CAGE_STRING ), pMoverProp->szName );
		}
	}
/*
	else if( pProp->IsUltimate() )
	{
		int	nFind	= strName.Find( "@", 0 );
		if( nFind > -1 )
			strName.Delete( nFind - 1, 2 );
	}
*/
#endif	// 
	return strName;
}

#ifdef __WEAPON_RARITY
int CItemElem::GetTotalDropLuck(int nLv)
{
	if (nLv > prj.m_nHighWeaponRarityLevel)
	{
		Error("nLv > prj.m_nHighWeaponRarityLevel");
		return 0;
	}

	int nTotal = 0;
	for (int i = 0; i <= nLv; i++)
	{
		nTotal += prj.m_nWeaponRarity.GetAt(i)->m_nDropLuck;
	}
	return nTotal - 1;
}

void CItemElem::SetRandomWeaponRarity()
{
	ItemProp* pProp = GetProp();
	if (IsValidItem(pProp->dwItemKind3))
	{
#ifdef __LEESE_RARITY_1000
		for (int i = 0, nRandom = xRandom(1000); i <= prj.m_nHighWeaponRarityLevel; i++)
#else //__LEESE_RARITY_1000
		for (int i = 0, nRandom = xRandom(100); i <= prj.m_nHighWeaponRarityLevel; i++)
#endif //__LEESE_RARITY_1000
		{
			int nTotal = GetTotalDropLuck(i);
			if (nRandom >= (nTotal - (prj.m_nWeaponRarity.GetAt(i)->m_nDropLuck - 1)) && nRandom <= nTotal)
			{
				SetWeaponRarity(i);
				return;
			}
		}
	}
}
#endif // __WEAPON_RARITY

enum	{	eNoLevelDown, e5LevelDown, e10LevelDown,	};
	int		GetLevelDown( void );	// 64|63
	void	SetLevelDown( int i );


#if __VER >= 11 // __SYS_IDENTIFY
int	CItemElem::GetLevelDown( void )
{
	if( m_iRandomOptItemId & 0x8000000000000000 )
		return -10;
	else if( m_iRandomOptItemId & 0x4000000000000000 )
		return -5;
	return 0;
}

void CItemElem::SetLevelDown( int i )
{
	//	0x8000000000000000
	//	0x4000000000000000
	m_iRandomOptItemId	&= ~0xC000000000000000;
	if( i == e5LevelDown )
		m_iRandomOptItemId	|= 0x4000000000000000;
	else if( i == e10LevelDown )
		m_iRandomOptItemId	|= 0x8000000000000000;
}

DWORD CItemElem::GetLimitLevel( void )
{
#ifdef __GAMEGUARD
	if (!GetProp())
		return 0;
#endif // __GAMEGUARD
	if( GetProp()->dwLimitLevel1 == 0xFFFFFFFF ) 
		return 0xFFFFFFFF;
	int nLimitLevel	= static_cast<int>( GetProp()->dwLimitLevel1 ) + GetLevelDown();
	if( nLimitLevel < 0 )
		nLimitLevel		= 0;

	return (DWORD)nLimitLevel;
}
#endif	// __SYS_IDENTIFY

#if __VER >= 14 // __NEW_ITEM_LIMIT_LEVEL
BOOL CItemElem::IsLimitLevel( CMover* pMover )
{
#ifdef __GAMEGUARD
	if (pMover == NULL)
		return FALSE;
#endif // __GAMEGUARD

	if( pMover->GetJobType() >= JTYPE_MASTER && pMover->GetJobType() > pMover->GetJobType( GetProp()->dwItemJob ) )
		return FALSE;

	if( (DWORD)( pMover->GetLevel() ) >= GetLimitLevel() )
		return FALSE;

	return TRUE;
}
#endif // __NEW_ITEM_LIMIT_LEVEL


#if __VER >= 12 // __EXT_PIERCING
// bSize는 피어싱 사이즈를 늘릴 수 있는지 검사할 때 TRUE값을 setting 한다.
// bSize를 TRUE로 할 경우 dwTagetItemKind3는 NULL_ID로 한다.
BOOL CItemElem::IsPierceAble( DWORD dwTargetItemKind3, BOOL bSize )
{
	if( !GetProp() )
		return FALSE;

#ifdef __NEW_ITEM_VARUNA
	if( GetProp()->IsBaruna() )
		return FALSE;
#endif // __NEW_ITEM_VARUNA

	int nPiercedSize = GetPiercingSize();
	if( bSize ) // 피어싱 사이즈를 늘리는 경우
		nPiercedSize++;
	
	if( GetProp()->dwItemKind3 == IK3_SUIT )
	{
		if( nPiercedSize <= MAX_PIERCING_SUIT )
		{
			if( dwTargetItemKind3 == NULL_ID )
				return TRUE;
			else if( dwTargetItemKind3 == IK3_SOCKETCARD )
					return TRUE;
		}
	}
	
	else if( GetProp()->dwItemKind3 == IK3_SHIELD
		|| GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT
		|| GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC
		)
	{
		if( nPiercedSize <= MAX_PIERCING_WEAPON )
		{
			if( dwTargetItemKind3 == NULL_ID )
				return TRUE;
			else if( dwTargetItemKind3 == IK3_SOCKETCARD2 )
				return TRUE;
		}
	}

#if __VER >= 15 // __PETVIS
	else if( IsVisPet() )
	{
		if( nPiercedSize <= MAX_VIS )
		{
			if( dwTargetItemKind3 == NULL_ID )
				return TRUE;
			else if( dwTargetItemKind3 == IK3_VIS )
				return TRUE;
		}
	}
#endif // __PETVIS

	return FALSE;
}
#endif // __EXT_PIERCING

#if __VER >= 11
#ifdef __WORLDSERVER
BOOL CItemElem::IsActiveTicket( DWORD dwItemId )
{
#ifdef __SECURITY_FIXES
	if (GetProp() == NULL)
		return FALSE;
#endif // __SECURITY_FIXES
	if( !IsFlag( expired ) && GetProp()->dwItemKind3 == IK3_TICKET && m_dwKeepTime > 0 )
	{
#ifdef __V18TICKET
		if (!CTicketProperty::GetInstance()->isMultiWorldTicket(m_dwItemId))
		{
			const TicketProp* pProp1 = CTicketProperty::GetInstance()->getTicketProp(m_dwItemId);
			const TicketProp* pProp2 = CTicketProperty::GetInstance()->getTicketProp(dwItemId);
			return pProp1->dwWorldId == pProp2->dwWorldId;
		}
		if (CTicketProperty::GetInstance()->isMultiWorldTicket(dwItemId))
			return true;
#else //__V18TICKET
		TicketProp* pProp1 = CTicketProperty::GetInstance()->GetTicketProp(m_dwItemId);
		TicketProp* pProp2 = CTicketProperty::GetInstance()->GetTicketProp(dwItemId);
		if (!pProp1 || !pProp2)
			return FALSE;
		return (pProp1->dwWorldId == pProp2->dwWorldId);
#endif //__V18TICKET
	}
	return FALSE;
}
#endif	// __WORLDSERVER
BOOL	IsNeedTarget( ItemProp* pProp )
{	// 아이템을 사용하기 위해 더블 클릭 했을 때
	// 커서가 바뀌면서 인벤토리 내 다른 아이템 클릭이 필요한 아이템인가?
#if __VER >= 12 // __PET_0519
	// 아이템 식별자 추가가 번거로와 속성 확인으로 변경
	return( pProp->dwExeTarget == EXT_ITEM );
#else	// __PET_0519
	switch( pProp->dwID )
	{
		case II_SYS_SYS_SCR_AWAKE:
		case II_SYS_SYS_SCR_BLESSEDNESS:
		case II_SYS_SYS_SCR_BLESSEDNESS02:
		case II_SYS_SYS_SCR_LEVELDOWN01:
		case II_SYS_SYS_SCR_LEVELDOWN02:
		case II_SYS_SYS_SCR_AWAKECANCEL:
		case II_SYS_SYS_SCR_AWAKECANCEL02:
		case II_SYS_SYS_QUE_PETRESURRECTION02_S:
		case II_SYS_SYS_QUE_PETRESURRECTION02_A:
		case II_SYS_SYS_QUE_PETRESURRECTION02_B:
//		case II_SYS_SYS_SCR_PETAWAKE:
//		case II_SYS_SYS_SCR_PETAWAKECANCEL:
			return TRUE;
		default:
			return FALSE;
	}
#endif	// __PET_0519
}
#endif	// __VER

#ifdef __WEAPON_RARITY
BOOL IsValidItem(DWORD dwId)
{
	return dwId == IK3_SWD
		|| dwId == IK3_AXE
		|| dwId == IK3_CHEERSTICK
		|| dwId == IK3_KNUCKLEHAMMER
		|| dwId == IK3_WAND
		|| dwId == IK3_STAFF
		|| dwId == IK3_THSWD
		|| dwId == IK3_THAXE
		|| dwId == IK3_YOYO
		|| dwId == IK3_BOW
#ifdef __CROSSBOW
		|| dwId == IK3_CROSSBOW
#endif // __CROSSBOW
#ifdef __NEW_ITEM_VARUNA
		|| dwId == IK3_MAGICBARUNA
		|| dwId == IK3_ZEMBARUNA
#endif // __NEW_ITEM_VARUNA
		|| dwId == IK3_HELMET
		|| dwId == IK3_SUIT
		|| dwId == IK3_GAUNTLET
		|| dwId == IK3_BOOTS
		|| dwId == IK3_SHIELD;
}
#endif // __WEAPON_RARITY

BOOL CItemElem::IsEgg()
{
	if( !IsPet() )
		return FALSE;
	return ( !m_pPet || m_pPet->GetLevel() == PL_EGG );
}

#if __VER >= 15 // __PETVIS
void  CItemElem::SetSwapVisItem( int nPos1, int nPos2 )
{
	int nSize = GetPiercingSize();
	if( nPos1 >= nSize || nPos2 >= nSize )
		return;

	DWORD dwTempId = GetPiercingItem( nPos1 );
	time_t tmTemp = GetVisKeepTime( nPos1 );

	SetPiercingItem( nPos1, GetPiercingItem( nPos2 ) );
	SetVisKeepTime( nPos1, GetVisKeepTime( nPos2 ) );
	SetPiercingItem( nPos2, dwTempId );
	SetVisKeepTime( nPos2, tmTemp );
}

DWORD CItemElem::GetVisPetSfxId()
{
	int nLevel = 0;
	for( int i=0; i<GetPiercingSize(); i++ )
	{
		ItemProp* pProp = prj.GetItemProp( GetPiercingItem( i ) );
		if( pProp && pProp->dwAbilityMax > (DWORD)( nLevel ) )
			nLevel = pProp->dwAbilityMax;
	}

	switch( nLevel )
	{
		case 1:	return XI_BUFFPET_GRADE1;
		case 2: return XI_BUFFPET_GRADE2;
		case 3: return XI_BUFFPET_GRADE3;
	}

	return NULL_ID;
}
#endif // __PETVIS

#ifdef __PET_GLOW
DWORD CItemElem::GetPetGlowChange()
{
	switch (m_nPetGlow)
	{
		case 1: return XI_GEN_ITEM_SETITEM34;
		case 2: return XI_GEN_ITEM_SETITEM34;
		case 3: return XI_GEN_ITEM_SETITEM34;
		case 60: return XI_GEN_ITEM_BUFF_PETGLOW01; break;
		case 61: return XI_GEN_ITEM_BUFF_PETGLOW02; break;
		case 62: return XI_GEN_ITEM_BUFF_PETGLOW03; break;
		case 63: return XI_GEN_ITEM_BUFF_PETGLOW04; break;
		case 64: return XI_GEN_ITEM_BUFF_PETGLOW05; break;
		case 65: return XI_GEN_ITEM_BUFF_PETGLOW06; break;
		case 66: return XI_GEN_ITEM_BUFF_PETGLOW07; break;
		case 67: return XI_GEN_ITEM_BUFF_PETGLOW08; break;
		case 68: return XI_GEN_ITEM_BUFF_PETGLOW09; break;
		case 69: return XI_GEN_ITEM_BUFF_PETGLOW10; break;
		case 70: return XI_GEN_ITEM_BUFF_PETGLOW11; break;
		case 71: return XI_GEN_ITEM_BUFF_PETGLOW12; break;
	}
	return NULL_ID;
}
#endif // __PET_GLOW

#ifdef __CRYSTAL
BOOL IsValidCrystal(CItemElem* pItemElem)
{
	if (pItemElem->GetProp()->IsUltimate() == FALSE)
		return TRUE;

	if (pItemElem->GetAbilityOption() < 10)
		return TRUE;

	if (pItemElem->GetCrystal() != NULL)
		return TRUE;

	return TRUE;
}
#endif // __CRYSTAL

//	mulcom	BEGIN100405	각성 보호의 두루마리
__int64		CItemElem::GetNewRandomOption()
{
	return m_n64NewRandomOption;
}

__int64*	CItemElem::GetNewRandomOptionPtr()
{
	return &m_n64NewRandomOption;
}

void		CItemElem::ResetNewRandomOption()
{
	m_n64NewRandomOption = 0;

	return;
}

void		CItemElem::SelectNewRandomOption()
{
	m_iRandomOptItemId	= (( m_iRandomOptItemId & 0xC0000000000000FF ) | m_n64NewRandomOption );

	return;
}

bool		CItemElem::SelectRandomOption( BYTE bySelectFlag )
{
	bool	bRetValue	= true;

	if( bySelectFlag == _AWAKE_OLD_VALUE )
	{
		ResetNewRandomOption();
	}
	else if( bySelectFlag == _AWAKE_NEW_VALUE )
	{
		SelectNewRandomOption();
		ResetNewRandomOption();
	}
	else
	{
		WriteLog( "bySelectFlag is invalid value. bySelectFlag : [%d]", (int)( bySelectFlag ) );

		bRetValue	= false;
	}

	return	bRetValue;
}

//	mulcom	END100405	각성 보호의 두루마리




////////////////////////////////////////////////////////////////////////////////