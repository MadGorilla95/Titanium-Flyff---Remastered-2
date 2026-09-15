#include "stdafx.h"
#include <iostream>

#ifdef __BUFF_1107

#include "buff.h"
#include "defineskill.h"
#include "defineobj.h"

#ifdef __WORLDSERVER
#include "user.h"
#include "dpdatabaseclient.h"
extern	CUserMng	g_UserMng;
extern	CDPDatabaseClient	g_dpDBClient;
#ifdef __BUFF_TOGIFT
#include "DPSrvr.h"
extern	CDPSrvr		g_DPSrvr;
#endif // __BUFF_TOGIFT
#endif	// __WORLDSERVER

////////////////////////////////////////////////////////////////////////////////
IBuff::IBuff()
:
m_wType( 0 )
#ifdef __INCREASE_SKILL_ID_VALUE
, m_dwId( 0 )
#else
, m_wId(0)
#endif
, m_dwLevel( 0 )
, m_tmTotal( 0 )
, m_tmInst( 0 )
, m_oiAttacker( NULL_ID )
#ifdef __CLIENT
, m_bSFX( FALSE )
#endif	// __CLIENT
, m_bRemove( FALSE )
{
}

#ifdef __ANARCHY_SYSTEM
void IBuff::SetTotal(DWORD tmTotal, bool bOverwrite)
#else // __ANARCHY_SYSTEM
void IBuff::SetTotal(DWORD tmTotal)
#endif // __ANARCHY_SYSTEM
{
#ifdef __ANARCHY_SYSTEM
	if (bOverwrite)
	{
		m_tmTotal = tmTotal;
	}
	else
#endif // __ANARCHY_SYSTEM
	{
		m_tmTotal = max(tmTotal, GetRemain());
	}
}

#ifndef __DBSERVER
BOOL IBuff::IsAddable( CMover* pMover )
{
	int nTotal	= static_cast<int>( GetTotal() );
	ASSERT( nTotal >= 0 );
	if( nTotal < 0 )
		return FALSE;
	return TRUE;
}

void IBuff::AddTotal( DWORD tmTotal )
{
	SetTotal( tmTotal );
	SetInst( ::timeGetTime() );		// +
}

#endif	// __DBSERVER

DWORD IBuff::GetRemain()
{
	if( GetInst() == 0 )
		return 0;
	
	DWORD dwElapsedTime = ::timeGetTime() - GetInst();
	if( dwElapsedTime > GetTotal() )	// 남은 시간은 총 버프 시간보다 클 수 없다.
		return 0;						// 실제로 버프 제거는 시간 만료 후 다음 프로세스에서 하기 때문에 발생 가능하다.

	return GetTotal() - dwElapsedTime;
}

IBuff& IBuff::operator =( const IBuff & buff )
{
	m_wType	= buff.m_wType;
#ifdef __INCREASE_SKILL_ID_VALUE
	m_dwId = buff.m_dwId;
#else
	m_wId = buff.m_wId;
#endif
	m_dwLevel	= buff.m_dwLevel;
	m_tmTotal	= buff.m_tmTotal;
	m_tmInst	= buff.m_tmInst;
	m_oiAttacker	= buff.m_oiAttacker;
#ifdef __CLIENT
	m_bSFX	= buff.m_bSFX;
#endif	// __CLIENT
	return *this;
}

void IBuff::SerializeLevel( CAr & ar )
{
	if( ar.IsStoring() )
	{
#ifdef __DBSERVER
		if( GetType() == BUFF_ITEM2 )
		{

			__int64 t	= static_cast<__int64>( GetLevel() ) - static_cast<__int64>(time_null());
			ar << t;
		}
		else
		{
			ar << m_dwLevel;
		}
#else	// __DBSERVER
		ar << m_dwLevel;
#endif	// __DBSERVER
	}
	else
	{
#ifdef __DBSERVER
		if( GetType() == BUFF_ITEM2 )
		{
			__int64 t;
			ar >> t;
			SetLevel(static_cast<DWORD>(static_cast<__int64>(time_null()) + t) );
		}
		else
		{
			ar >> m_dwLevel;
		}
#else	// __DBSERVER
		ar >> m_dwLevel;
#endif	// __DBSERVER
	}
}

void IBuff::Serialize( CAr & ar, CMover* pMover )
{
	if( ar.IsStoring() )
	{
		ar << m_wType;
#ifdef __INCREASE_SKILL_ID_VALUE
		ar << m_dwId;
#else
		ar << m_wId;
#endif
		SerializeLevel( ar );
#ifdef __WORLDSERVER
		DWORD tmTotal	= 0;
		if( GetTotal() > 0 )
		{
			if( GetInst() > 0 )
				tmTotal	= GetTotal() - ( g_tmCurrent - GetInst() );
			else
				tmTotal	= GetTotal();
		}
		if( static_cast<int>( tmTotal ) < 0 )
			tmTotal	= 0;
		ar << tmTotal;
#else	// __WORLDSERVER
		ar << m_tmTotal;
#endif	// __WORLDSERVER
	}
	else
	{
#ifdef __INCREASE_SKILL_ID_VALUE
		ar >> m_dwId;
#else
		ar >> m_wId;
#endif
		SerializeLevel( ar );
		DWORD tmTotal;
		ar >> tmTotal;
#ifndef __FIX_RESET_BUFFTIME_ON_RECONNECT
#ifdef __WORLDSERVER
		if (IsCommon() && pMover && tmTotal >= MIN(60))
			tmTotal = MIN(60);
#endif	// __WORLDSERVER
#endif //__FIX_RESET_BUFFTIME_ON_RECONNECT
		SetTotal( tmTotal );
	}
}

#ifndef __DBSERVER
BOOL IBuff::Timeover( CMover* pMover, DWORD tmCurrent )
{
	return tmCurrent - GetInst() >= GetTotal();
}

BOOL IBuff::Expire( CMover* pMover, DWORD tmCurrent )
{
	if( IsRemovable( pMover ) && Timeover( pMover, tmCurrent ) )
	{
#ifdef __WORLDSERVER
		OnExpire( pMover );
#endif	// __WORLDSERVER
		return TRUE;
	}
	return FALSE;
}

BOOL IBuff::Process( CMover* pMover, DWORD tmCurrent )
{
	if( GetRemove() )
		return TRUE;
	if( IsInvalid( pMover ) )
		return TRUE;
	if( pMover )
	{
#ifdef __CLIENT
		CreateSFX( pMover );
#endif	// __CLIENT
		if( GetInst() == 0 )
		{
			Apply( pMover );
			SetInst( tmCurrent );
		}
	}
	return Expire( pMover, tmCurrent );
}
#endif	// __DBSERVER

#ifdef __CLIENT
void IBuff::CreateSFX( CMover* pMover )
{
	ItemProp* pProp	= GetProp();
	if( pProp && !HasSFX() )
	{
		FLOAT fSkillTime	= (float)GetTotal() / 1000.0F;	// 초단위
		SetSFX();
		// 지속 효과: dwSfxObj4
		if( pProp->dwSfxObj4 != NULL_ID )
		{
			if( GetTotal() == 0 )
				CreateSfx( g_Neuz.m_pd3dDevice, pProp->dwSfxObj4, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3( 0, 0, 0), pMover->GetId(), 0 );
			else
				CreateSfx( g_Neuz.m_pd3dDevice, pProp->dwSfxObj4, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3( 0, 0, 0), pMover->GetId(), (int)( fSkillTime ) );
		}
	}
}
#endif	// __CLIENT

#ifndef __DBSERVER
////////////////////////////////////////////////////////////////////////////////

void IBuffItemBase::Release( CBuffMgr* pBuffMgr )
{
#ifdef __WORLDSERVER
	ItemProp* pProp	= GetProp();
	if( pProp )
	{
		pBuffMgr->GetMover()->ResetDestParam( pProp->dwDestParam1, pProp->nAdjParamVal1, TRUE );
		pBuffMgr->GetMover()->ResetDestParam( pProp->dwDestParam2, pProp->nAdjParamVal2, TRUE );
#ifdef __V19_FORMATS
		pBuffMgr->GetMover()->ResetDestParam( pProp->dwDestParam3, pProp->nAdjParamVal3, TRUE );
		pBuffMgr->GetMover()->ResetDestParam( pProp->dwDestParam4, pProp->nAdjParamVal4, TRUE );
		pBuffMgr->GetMover()->ResetDestParam( pProp->dwDestParam5, pProp->nAdjParamVal5, TRUE );
		pBuffMgr->GetMover()->ResetDestParam( pProp->dwDestParam6, pProp->nAdjParamVal6, TRUE );
#endif //__V19_FORMATS
		
		if( strlen( pProp->szTextFileName ) > 0 && IK3_ANGEL_BUFF != pProp->dwItemKind3 
#if __VER >= 13 // __HOUSING
			&& IK2_PAPERING != pProp->dwItemKind2
#endif // )__HOUSING
			)
		{
#ifdef __INCREASE_SKILL_ID_VALUE
			if (NULL_ID != pProp->dwActiveSkill)
				pBuffMgr->RemoveBuff(BUFF_SKILL, pProp->dwActiveSkill);
#else
			if (NULL_ID != pProp->dwActiveSkill)
				pBuffMgr->RemoveBuff(BUFF_SKILL, (WORD)(pProp->dwActiveSkill));
#endif

			char szCommand[100]	= { 0,};
			CString strCommand	= pProp->szTextFileName;
			strCommand.Replace( '(', '\"' );
			strCommand.Replace( ')', '\"' ); 
			int n	= strCommand.Find( "/", 1 );
			if( n >= 0 )
			{
				strncpy( szCommand, &pProp->szTextFileName[n], strCommand.GetLength() );
				ParsingCommand( szCommand, pBuffMgr->GetMover(), TRUE );
			}
		}
#ifdef __BUFF_TOGIFT
		if( pProp->dwItemKind2 == IK2_BUFF_TOGIFT )
		{
			CUser* pUser = static_cast<CUser*>( pBuffMgr->GetMover() );
			ItemProp* pItemProp = prj.GetItemProp( pProp->dwReferTarget1 );
			if( IsValidObj( pUser ) && pItemProp )
			{
				CItemElem itemElem;
				itemElem.m_dwItemId = pItemProp->dwID;
				itemElem.m_bCharged = pItemProp->bCharged;
				itemElem.m_nItemNum = 1;
				itemElem.SetSerialNumber();
				if( pUser->CreateItem( &itemElem ) )
					g_DPSrvr.PutItemLog( pUser, "g", "BuffToGift", &itemElem, 1 );
				else
				{
					g_dpDBClient.SendQueryPostMail( pUser->m_idPlayer, 0, itemElem, 0, pItemProp->szName, "" );
					g_DPSrvr.PutItemLog( pUser, "g", "BuffToGift_Post", &itemElem, 1 );
				}
			}
		}
#endif // __BUFF_TOGIFT
	}
#endif	// __WORLDSERVER
}

ItemProp* IBuffItemBase::GetProp()
{
	return prj.GetItemProp( GetId() );
}

BOOL IBuffItemBase::IsCommon()
{
	ItemProp* pProp	= GetProp();
	return !( pProp && pProp->bCharged );
}

BOOL IBuffItemBase::HasChrState( CMover*, DWORD dwChrState )
{
	ItemProp* pProp	= GetProp();
	if( pProp )
	{
		for( int i = 0; i < 2; i++ )
			if( pProp->dwDestParam[i] == DST_CHRSTATE && pProp->nAdjParamVal[i] & dwChrState )
				return TRUE;
	}
	return FALSE;
}

BOOL IBuffItemBase::IsDebuf( CMover* pMover )
{
	ItemProp* pProp	= GetProp();
	return pProp && pProp->nEvildoing < 0;
}

#ifdef __CLIENT
BOOL IBuffItemBase::IsCommercial()
{
	ItemProp* pProp	= GetProp();
	return pProp && pProp->bCharged;	// ?	// pProp->dwID != II_PET_EGG
}
#endif	// __CLIENT

////////////////////////////////////////////////////////////////////////////////
BOOL CBuffItem::IsIk1( DWORD dwIk1 )
{
	ItemProp* pProp	= GetProp();
	return pProp && pProp->dwItemKind1 == dwIk1;
}

BOOL CBuffItem::IsIk3( DWORD dwIk3 )
{
	ItemProp* pProp	= GetProp();
	return pProp && pProp->dwItemKind3 == dwIk3;
}

#ifdef __SYS_MAP_BUFFS
BOOL CBuffEquip::IsIk3(DWORD dwIk3)
{
	ItemProp* pProp = prj.GetItemProp(GetId());
	return pProp && pProp->dwItemKind3 == dwIk3;
}
#endif //__SYS_MAP_BUFFS

void CBuffItem::Apply( CMover* pMover )
{
	ItemProp* pProp	= GetProp();
	if( pProp )
		pMover->ApplyParam( pMover, pProp, NULL, FALSE, 0 );
}

BOOL CBuffItem::IsRemovable( CMover* pMover )
{
	ItemProp* pProp	= GetProp();
	return !( pProp && 999999999 == pProp->dwSkillTime );
}

void CBuffItem::AddTotal( DWORD tmTotal )
{
	switch( GetId() )
	{
		case II_SYS_SYS_SCR_PET_FEED_POCKET02:
		case II_SYS_SYS_SCR_PET_TONIC_A:
		case II_SYS_SYS_SCR_PET_TONIC_B:
			tmTotal	+= GetRemain();
			break;
		default:
			break;
	}
	IBuff::AddTotal( tmTotal );
}

DWORD CBuffItem::GetDisguise()
{
	char szCommand[100]	= { 0,};
	ItemProp* pProp	= GetProp();
	if( pProp && pProp->dwItemKind3 == IK3_TEXT_DISGUISE )
	{
		char* ptr	= ::strstr( pProp->szTextFileName, "/dis" );
		if( ptr )
		{
			int n	= atoi( ptr + 5 );	// "/dis "
			if( n > 0 )
			{
				MoverProp* pMoverProp	= prj.GetMoverProp( n );
				if( pMoverProp )
					return pMoverProp->dwID;
			}
		}
	}
	return NULL_ID;
}

#ifdef __WORLDSERVER
void CBuffItem::OnExpire( CMover* pMover )
{
	if( GetId() == II_SYS_SYS_SCR_PET_FEED_POCKET02 )
	{
		ItemProp* pProp	= GetProp();
		if( pProp && pMover->IsPlayer() )
			g_dpDBClient.SendLogSMItemUse( "2", static_cast<CUser*>( pMover ), NULL, pProp );
	}
}
#endif	// __WORLDSERVER

////////////////////////////////////////////////////////////////////////////////
BOOL CBuffSkill::IsInvalid( CMover* pMover )
{
#ifdef __WORLDSERVER
	switch( GetId() )
	{
		case SI_JST_SUP_POISON:
		case SI_JST_SUP_BLEEDING:
		case SI_JST_SUP_ABSORB:
		case SI_ACR_SUP_YOYOMASTER:
		case SI_JST_YOYO_CRITICALSWING:
			if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_MELEE_YOYO )
				return TRUE;
			break;
		case SI_RAG_SUP_FASTATTACK:
		case SI_ACR_SUP_BOWMASTER:
			if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_RANGE_BOW )
				return TRUE;
			break;
		case SI_BLD_MASTER_ONEHANDMASTER:
			if( !pMover->IsDualWeapon() )
				return TRUE;
			break;
		case SI_KNT_MASTER_TWOHANDMASTER:
			if( pMover->GetActiveHandItemProp()->dwHanded != HD_TWO )
				return TRUE;
			break;
		case SI_MER_SUP_IMPOWERWEAPON:		// 
			{
				BOOL bRelease	= FALSE;
				CItemElem* pRight	= pMover->GetWeaponItem();
				CItemElem* pLeft	= pMover->GetLWeaponItem();
				if( pRight && pLeft )
					bRelease	= pRight->m_bItemResist == SAI79::NO_PROP
												&& pLeft->m_bItemResist == SAI79::NO_PROP;
				else if( pRight && !pLeft )
					bRelease	= pRight->m_bItemResist == SAI79::NO_PROP;
				else if( !pRight && !pLeft )
					bRelease	= TRUE;
				if( bRelease )
					return TRUE;
				break;
			}
		case SI_MER_SUP_BLAZINGSWORD:
		case SI_MER_SUP_SWORDMASTER:
			if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_MELEE_SWD )
				return TRUE;
			break;
		case SI_MER_SUP_SMITEAXE:
		case SI_MER_SUP_AXEMASTER:
			if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_MELEE_AXE )
				return TRUE;
			break;
		case SI_BIL_PST_ASMODEUS:
			if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_MELEE_KNUCKLE )
				return TRUE;
			break;
		case SI_MER_SHIELD_PROTECTION:
		case SI_MER_SHIELD_PANBARRIER:
			if( pMover->GetActiveHandItemProp( PARTS_SHIELD )->dwItemKind3 != IK3_SHIELD )
				return TRUE;
			break;
#ifdef __SLEEPING_FIX
		case SI_ELE_STF_SLIPPING:
			if ((pMover->GetAdjParam(DST_CHRSTATE) & CHS_STUN) == 0)
				return TRUE;
			break;
#endif //__SLEEPING_FIX
	}
#endif	// __WORLDSERVER
	return FALSE;
}

void CBuffSkill::Apply( CMover* pMover )
{
#ifdef __CLIENT
	if( !pMover )	return;
#endif	// __CLIENT
	ItemProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	BOOL bResult	= pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.Apply" );
	if( bResult )
		pMover->ApplyParam( pMover, pSkillProp, pAddSkillProp, FALSE, 0 );
}

#ifdef __SYS_MAP_BUFFS
void CBuffEquip::Apply(CMover* pMover)
{
	ItemProp* pProp = prj.GetItemProp(GetId());
	if (pProp)
		pMover->ApplyParam(pMover, pProp, NULL, FALSE, 0);
}
#endif //__SYS_MAP_BUFFS

void CBuffSkill::Release( CBuffMgr* pBuffMgr )
{
#ifdef __CLIENT
	CMover* pMover	= pBuffMgr->GetMover();
	if( pMover )
		pMover->OnEndSkillState( GetId(), GetLevel() );
#else	// __CLIENT
	pBuffMgr->GetMover()->OnEndSkillState( GetId(), GetLevel() );
#endif	// __CLIENT
}

#ifdef __SYS_MAP_BUFFS
void CBuffEquip::Release(CBuffMgr* pBuffMgr)
{
	ItemProp* pProp = prj.GetItemProp(GetId());
	if (pProp)
		for (int i = 0; i < MAX_SLOTS; i++)
			if (pProp->dwDestParam[i] != NULL_ID)
				pBuffMgr->GetMover()->ResetDestParam(i, pProp, FALSE);
}
#endif //__SYS_MAP_BUFFS

BOOL CBuffSkill::IsRemovable( CMover* pMover )
{
#ifdef __CLIENT
	if( !pMover )	return TRUE;
#endif	// __CLIENT
	ItemProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.IsRemovable" );
	return pAddSkillProp && pAddSkillProp->dwSkillTime > 0;
}

ItemProp* CBuffSkill::GetProp()
{
	return prj.GetSkillProp( GetId() );
}

BOOL CBuffSkill::IsAddable( CMover* pMover )
{
#ifdef __CLIENT
	ItemProp* pProp	= GetProp();
	return IBuff::IsAddable( pMover )
		&& !( pMover && pMover->IsPlayer() && pMover->m_nDead > 0 && pProp && pProp->nEvildoing < 0 );
#else	// __CLIENT
	ItemProp* pProp	= GetProp();
	return IBuff::IsAddable( pMover )
					&& !( pMover->IsPlayer() && pMover->m_nDead > 0 && pProp && pProp->nEvildoing < 0 );
#endif	// __CLIENT
}

BOOL CBuffSkill::IsGood( CMover* pMover )
{
	ASSERT( pMover );
	ItemProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.IsGood" );
	return pSkillProp && pSkillProp->nEvildoing >= 0;
}

BOOL CBuffSkill::HasChrState( CMover* pMover, DWORD dwChrState )
{
	ASSERT( pMover );
	ItemProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.HasChrState" );
	for( int i = 0; i < 2; i++ )
		if( pAddSkillProp->dwDestParam[i] == DST_CHRSTATE && pAddSkillProp->nAdjParamVal[i] & dwChrState )
			return TRUE;
	return FALSE;
}

BOOL CBuffSkill::HasDstParam( CMover* pMover, DWORD dwDstParam )
{
	ASSERT( pMover );
	ItemProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.HasDstParam" );
	for( int i = 0; i < 2; i++ )
		if( pSkillProp->nEvildoing < 0 && pAddSkillProp->dwDestParam[i] == dwDstParam )
			return TRUE;
	return FALSE;
}

BOOL CBuffSkill::IsDebuf( CMover* pMover )
{
	ASSERT( pMover );
	ItemProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.IsDebuf" );
	return pSkillProp && pSkillProp->nEvildoing < 0;
}

BOOL CBuffSkill::IsAttacker( CMover* pMover, OBJID oiAttacker )
{
	ASSERT( pMover );
	ItemProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.IsAttacker" );
	if( pSkillProp && pSkillProp->nEvildoing < 0 && GetAttacker() == oiAttacker )
		return TRUE;
	return FALSE;
}

#ifdef __WORLDSERVER
void CBuffSkill::OnExpire( CMover* pMover )
{
	if( GetId() == SI_GEN_BURN )
	{
		ItemProp* pSkillProp	= prj.GetSkillProp( SI_GEN_BURN );
#ifdef __GAMEGUARD
		if (!pSkillProp)
			return;
#endif //__GAMEGUARD
		int nMin	= pSkillProp->dwAbilityMin;
		int nMax	= pSkillProp->dwAbilityMax;
		int nDamage	= xRandom( nMin, nMax );
		pMover->SendDamage( AF_FORCE, GetAttacker(), nDamage );
		g_UserMng.AddCreateSfxObj( pMover, XI_SKILL_ELE_FIRE_BURINGFIELD02 );
	}
}
#endif	// __WORLDSERVER

////////////////////////////////////////////////////////////////////////////////
void CBuffPet::Apply( CMover* pMover )
{
#ifdef __GAMEGUARD
	if (pMover)
#endif //__GAMEGUARD
	pMover->SetDestParam( HIWORD( GetLevel() ), LOWORD( GetLevel() ), NULL_CHGPARAM, FALSE );
}

void CBuffPet::Release( CBuffMgr* pBuffMgr )
{
#ifdef __WORLDSERVER
#ifdef __GAMEGUARD
	if (pBuffMgr)
#endif //__GAMEGUARD
	pBuffMgr->GetMover()->ResetDestParam( HIWORD( GetLevel() ), LOWORD( GetLevel() ), TRUE );
#endif	// __WORLDSERVER
}

#ifdef __ANARCHY_SYSTEM
BOOL CBuffItem3::Timeover(CMover* pMover, DWORD tmCurrnet)
{
	__time32_t	t = time_null();
	if ((DWORD)(t) >= GetLevel())
	{
#ifdef __WORLDSERVER
		ItemProp* pProp = GetProp();
		if (pProp && pMover->IsPlayer())
			g_dpDBClient.SendLogSMItemUse("2", static_cast<CUser*>(pMover), NULL, pProp);
#endif	// __WORLDSERVER
		return TRUE;
	}
	return FALSE;
}


#ifdef __WORLDSERVER
void CBuffItem3::OnExpire(CMover* pMover)
{
	if (GetId() == SI_GEN_BURN)
	{
		ItemProp* pSkillProp = prj.GetSkillProp(SI_GEN_BURN);
		int nMin = pSkillProp->dwAbilityMin;
		int nMax = pSkillProp->dwAbilityMax;
		int nDamage = xRandom(nMin, nMax);
		pMover->SendDamage(AF_FORCE, GetAttacker(), nDamage);
		g_UserMng.AddCreateSfxObj(pMover, XI_SKILL_ELE_FIRE_BURINGFIELD02);
	}
}
#endif	// __WORLDSERVER

void CBuffItem3::SerializeLevel(CAr& ar)
{
	if (ar.IsStoring())
	{
		__time32_t t = static_cast<__time32_t>(GetLevel()) - time_null();
		ar << t;
	}
	else
	{
		__time32_t t;
		ar >> t;
		SetLevel(time_null() + t);
	}
}
BOOL CBuffItem3::IsIk1(DWORD dwIk1)
{
	ItemProp* pProp = GetProp();
	return pProp && pProp->dwItemKind1 == dwIk1;
}

BOOL CBuffItem3::IsIk3(DWORD dwIk3)
{
	ItemProp* pProp = GetProp();
	return pProp && pProp->dwItemKind3 == dwIk3;
}


void CBuffItem3::Apply(CMover* pMover)
{
	ItemProp* pProp = GetProp();
	if (pProp)
		pMover->ApplyParam(pMover, pProp, NULL, TRUE, 0);
}

BOOL CBuffItem3::IsRemovable(CMover* pMover)
{
	ItemProp* pProp = GetProp();
	return !(pProp && 999999999 == pProp->dwSkillTime);
}
#endif // __ANARCHY_SYSTEM

////////////////////////////////////////////////////////////////////////////////
BOOL CBuffItem2::Timeover( CMover* pMover, DWORD tmCurrnet )
{
	time_t	t	= time_null();
	if( (DWORD)( t ) >= GetLevel() )
	{
#ifdef __WORLDSERVER
		ItemProp *pProp	= GetProp();
#ifdef __GAMEGUARD
		if (pProp && pMover && pMover->IsPlayer())
#else //__GAMEGUARD
		if (pProp && pMover->IsPlayer())
#endif //__GAMEGUARD
			g_dpDBClient.SendLogSMItemUse( "2", static_cast<CUser*>( pMover ), NULL, pProp );
#endif	// __WORLDSERVER
		return TRUE;
	}
	return FALSE;
}

void CBuffItem2::SerializeLevel( CAr & ar )
{
	if( ar.IsStoring() )
	{
		time_t t	= static_cast<time_t>( GetLevel() ) - time_null();
		ar << t;
	}
	else
	{
		time_t t;
		ar >> t;
		SetLevel( time_null() + t );
	}
}
#ifdef __AEGON_IMPROVED_SHOPDESIGN
BOOL CBuffItem2::IsIk3(DWORD ik3) {
	ItemProp* pProp = GetProp();
	return pProp && pProp->dwItemKind3 == ik3;
}
#endif
#endif	// __DBSERVER

CBuffMgr::CBuffMgr( CMover* pMover )
:
m_pMover( pMover )
{
}

CBuffMgr::~CBuffMgr()
{
	Clear();
}

void CBuffMgr::Clear()
{
	for( MAPBUFF::iterator i = m_mapBuffs.begin(); i != m_mapBuffs.end(); ++i )
		SAFE_DELETE( i->second );
	m_mapBuffs.clear();
}

IBuff* CBuffMgr::CreateBuff( WORD wType )
{
	IBuff* ptr	= NULL;
#ifdef __DBSERVER
	ptr	= new IBuff;
#else	// __DBSERVER
	switch( wType )
	{
		case BUFF_SKILL:	ptr	= new CBuffSkill;	break;
		case BUFF_ITEM:		ptr	= new CBuffItem;	break;
		case BUFF_ITEM2:	ptr	= new CBuffItem2;	break;
#ifdef __ANARCHY_SYSTEM
		case BUFF_ITEM3:	ptr = new CBuffItem3;	break;
#endif // __ANARCHY_SYSTEM
		case BUFF_PET:		ptr	= new CBuffPet;		break;
		case BUFF_EQUIP:	ptr	= new CBuffEquip;	break;
		default:	return NULL;
	}
#endif	// __DBSERVER
	ptr->SetType( wType );
	return ptr;
}

#ifndef __DBSERVER
BOOL CBuffMgr::Overwrite( IBuff* pBuff )
{
	IBuff* pOld	= GetBuff( pBuff->GetType(), pBuff->GetId() );
	if( pOld )
	{
		if( pOld->GetLevel() == pBuff->GetLevel() )
		{
			pOld->AddTotal( pBuff->GetTotal() );
			return TRUE;
		}
		else if( pOld->GetLevel() < pBuff->GetLevel() )
			RemoveBuff( pOld, FALSE );
		else	// if( pOld->GetLevel() > pBuff->GetLevel() )	// ignore
			return TRUE;
	}
	return FALSE;
}
#ifdef __INCREASE_SKILL_ID_VALUE
BOOL CBuffMgr::HasBuff(WORD wType, DWORD dwId)
{
	return GetBuff(wType, dwId) != NULL;
}
#else
BOOL CBuffMgr::HasBuff(WORD wType, WORD wId)
{
	return GetBuff(wType, wId) != NULL;
}
#endif

#ifdef __INCREASE_SKILL_ID_VALUE
IBuff* CBuffMgr::GetBuff(WORD wType, DWORD dwId)
{
	MAPBUFF::iterator i = m_mapBuffs.find(MAKELONG(dwId, wType));
	if (i != m_mapBuffs.end())
		return i->second;
	return NULL;
}
#else
IBuff* CBuffMgr::GetBuff(WORD wType, WORD wId)
{
	MAPBUFF::iterator i = m_mapBuffs.find(MAKELONG(wId, wType));
	if (i != m_mapBuffs.end())
		return i->second;
	return NULL;
}
#endif


IBuff* CBuffMgr::GetBuffByIk3( DWORD dwIk3 )
{
	for( MAPBUFF::iterator i = m_mapBuffs.begin(); i != m_mapBuffs.end(); ++i )
	{
		IBuff* pBuff	= i->second;
#ifdef __GAMEGUARD
		if (!pBuff) return NULL;
#endif // //__GAMEGUARD
		if( pBuff->IsIk3( dwIk3 ) )
			return pBuff;
	}
	return NULL;
}

BOOL CBuffMgr::HasBuffByIk3( DWORD dwIk3 )
{
	return GetBuffByIk3( dwIk3 ) != NULL;
}

#ifdef __AEGON_PERMANENT_BUFF
int CBuffMgr::CountBuffByIk3(DWORD dwIk3)
{
	int nCount = 0;
	for (MAPBUFF::iterator i = m_mapBuffs.begin(); i != m_mapBuffs.end(); ++i)
	{
		IBuff* pBuff = i->second;
		if (pBuff && pBuff->IsIk3(dwIk3))
			nCount++;
	}
	return nCount;
}
#endif // __AEGON_PERMANENT_BUFF

#ifdef __ANARCHY_SYSTEM
#ifdef __INCREASE_SKILL_ID_VALUE
BOOL CBuffMgr::AddBuff(WORD wType, DWORD dwId, DWORD dwLevel, DWORD tmTotal, OBJID oiAttacker, bool bOverwrite)
{
	IBuff* pBuff = CreateBuff(wType);
	pBuff->SetId(dwId);
	pBuff->SetLevel(dwLevel);
#ifdef __ANARCHY_SYSTEM
	pBuff->SetTotal(tmTotal, bOverwrite);
#else // __ANARCHY_SYSTEM
	pBuff->SetTotal(tmTotal);
#endif // __ANARCHY_SYSTEM
	pBuff->SetInst(::timeGetTime());		// +
	pBuff->SetAttacker(oiAttacker);
#ifdef __ANARCHY_SYSTEM
	BOOL bNew = AddBuff(pBuff, bOverwrite);
#else // __ANARCHY_SYSTEM
	BOOL bNew = AddBuff(pBuff);
#endif // __ANARCHY_SYSTEM
	if (!bNew)
		SAFE_DELETE(pBuff);
	return bNew;
}
#else
BOOL CBuffMgr::AddBuff(WORD wType, WORD wId, DWORD dwLevel, DWORD tmTotal, OBJID oiAttacker, bool bOverwrite)
{
	IBuff* pBuff = CreateBuff(wType);
	pBuff->SetId(wId);
	pBuff->SetLevel(dwLevel);
#ifdef __ANARCHY_SYSTEM
	pBuff->SetTotal(tmTotal, bOverwrite);
#else // __ANARCHY_SYSTEM
	pBuff->SetTotal(tmTotal);
#endif // __ANARCHY_SYSTEM
	pBuff->SetInst(::timeGetTime());		// +
	pBuff->SetAttacker(oiAttacker);
#ifdef __ANARCHY_SYSTEM
	BOOL bNew = AddBuff(pBuff, bOverwrite);
#else // __ANARCHY_SYSTEM
	BOOL bNew = AddBuff(pBuff);
#endif // __ANARCHY_SYSTEM
	if (!bNew)
		SAFE_DELETE(pBuff);
	return bNew;
}
#endif
#else // __ANARCHY_SYSTEM
BOOL CBuffMgr::AddBuff(WORD wType, WORD wId, DWORD dwLevel, DWORD tmTotal, OBJID oiAttacker)
{
	IBuff* pBuff = CreateBuff(wType);
	pBuff->SetId(wId);
	pBuff->SetLevel(dwLevel);
	pBuff->SetTotal(tmTotal);
	pBuff->SetInst(::timeGetTime());		// +
	pBuff->SetAttacker(oiAttacker);
	BOOL bNew = AddBuff(pBuff);
	if (!bNew)
		SAFE_DELETE(pBuff);
	return bNew;
}
#endif // __ANARCHY_SYSTEM




#ifdef __ANARCHY_SYSTEM
BOOL CBuffMgr::AddBuff(IBuff* pBuff, bool bOverwrite)
#else // __ANARCHY_SYSTEM
BOOL CBuffMgr::AddBuff(IBuff* pBuff)
#endif // __ANARCHY_SYSTEM
{
	if( !pBuff->IsAddable( GetMover() ) )
		return FALSE;

#ifdef __SYS_MAP_BUFFS
	if (pBuff->GetType() == BUFF_EQUIP)
	{
		if (((CBuffEquip*)pBuff)->IsIk3(IK3_MAPBUFF))
		{
			((CBuffEquip*)pBuff)->Apply(GetMover());
		}
	}
#endif //__SYS_MAP_BUFFS

#ifdef __ANARCHY_SYSTEM
	if (Overwrite(pBuff) || bOverwrite)
#else // __ANARCHY_SYSTEM
	if (Overwrite(pBuff))
#endif // __ANARCHY_SYSTEM
	{
#ifdef __ANARCHY_SYSTEM
#ifdef __CLIENT
		IBuff* buffOld = GetBuff(pBuff->GetType(), pBuff->GetId());
		if (buffOld && pBuff->GetLevel() >= buffOld->GetLevel())
		{
			buffOld->SetTotal(pBuff->GetTotal(), true);
		}
#endif // __CLIENT
#endif // __ANARCHY_SYSTEM

#ifdef __WORLDSERVER
#ifdef __ANARCHY_SYSTEM
#ifdef __AEGON_FIXES
		IBuff* pOld = GetBuff(pBuff->GetType(), pBuff->GetId());
		if (pOld) 
		{
			if (pBuff->GetLevel() < pOld->GetLevel())
				return FALSE;

			g_UserMng.AddSetSkillState(GetMover(), GetMover(), pOld->GetType(), pOld->GetId(), pOld->GetLevel(), pOld->GetTotal(), bOverwrite);
		}
		else
			g_UserMng.AddSetSkillState(GetMover(), GetMover(), pBuff->GetType(), pBuff->GetId(), pBuff->GetLevel(), pBuff->GetTotal(), bOverwrite);
#else
		g_UserMng.AddSetSkillState(GetMover(), GetMover(), pBuff->GetType(), pBuff->GetId(), pBuff->GetLevel(), pBuff->GetTotal(), bOverwrite);
#endif
#else // __ANARCHY_SYSTEM
		g_UserMng.AddSetSkillState(GetMover(), GetMover(), pBuff->GetType(), pBuff->GetId(), pBuff->GetLevel(), pBuff->GetTotal());
#endif // __ANARCHY_SYSTEM
#endif	// __WORLDSERVER

		return FALSE;
	}
	PrepareBS( pBuff );
	bool bResult	= Add( pBuff );	
#ifdef __WORLDSERVER
	if( bResult == true )
		g_UserMng.AddSetSkillState( GetMover(), GetMover(), pBuff->GetType(), pBuff->GetId(), pBuff->GetLevel(), pBuff->GetTotal() );
#endif	// __WORLDSERVER
	return static_cast<BOOL>( bResult );
}

void CBuffMgr::RemoveBuff( IBuff* pBuff, BOOL bFake )
{
	ASSERT( pBuff );
	if( !pBuff->GetRemove() )	// pBuff에 대하여 RemoveBuff의 최초 호출 시 Release 호출
		pBuff->Release( this );
#ifdef __WORLDSERVER
	if( bFake )
	{
		pBuff->SetRemove();
		return;
	}
	g_UserMng.AddRemoveSkillInfluence( GetMover(), pBuff->GetType(), pBuff->GetId() );
#endif	// __WORLDSERVER
	int nResult	= m_mapBuffs.erase( MAKELONG( pBuff->GetId(), pBuff->GetType() ) );
	if( !nResult )
	{
		Error( "CBuffMgr.RemoveBuff: could not erase(type: %d, id: %d)", pBuff->GetType(), pBuff->GetId() );
		return;
	}
	SAFE_DELETE( pBuff );
}

#ifdef __INCREASE_SKILL_ID_VALUE
void CBuffMgr::RemoveBuff(WORD wType, DWORD dwId, BOOL bFake)
{
	IBuff* pBuff = GetBuff(wType, dwId);
	if (pBuff)
		RemoveBuff(pBuff, bFake);
}
#else
void CBuffMgr::RemoveBuff(WORD wType, WORD wId, BOOL bFake)
{
	IBuff* pBuff = GetBuff(wType, wId);
	if (pBuff)
		RemoveBuff(pBuff, bFake);
}
#endif


#ifdef __SYS_MAP_BUFFS
void CBuffMgr::RemoveMapBuff()
{
	for (auto buff : m_mapBuffs) 
	{
		if (buff.second->GetType() == BUFF_EQUIP)
			if (((CBuffEquip*)buff.second)->IsIk3(IK3_MAPBUFF))
				RemoveBuff(buff.second, FALSE);
	}
}
#endif //__SYS_MAP_BUFFS

void CBuffMgr::PrepareBS( IBuff* pBuff )
{
	if( pBuff->GetType() == BUFF_SKILL && IsBSFull() )
	{
		IBuff* pFirst	= GetFirstBS();
		//ItemProp* pProp	= pBuff->GetProp();
		//if( pProp && pProp->nEvildoing >= 0 )
		if( pFirst )
			RemoveBuff( pFirst, FALSE );
	}
}

int CBuffMgr::IsBSFull()
{
	int nNum	= 0;
	for( MAPBUFF::iterator i = m_mapBuffs.begin(); i != m_mapBuffs.end(); ++i )
	{
		IBuff* pBuff	= i->second;
		if( pBuff->GetType() == BUFF_SKILL )
			nNum++;
	}
	return nNum >= MAX_SKILLBUFF_COUNT;
}

IBuff* CBuffMgr::GetFirstBS()
{
	for( MAPBUFF::iterator i = m_mapBuffs.begin(); i != m_mapBuffs.end(); ++i )
	{
		if( i->second->GetType() == BUFF_SKILL )
		{
			ItemProp* pProp = i->second->GetProp();
			if( pProp && pProp->nEvildoing >= 0 )
				return i->second;
		}
	}

	return NULL;
}

void CBuffMgr::Process()
{
	vector<LONG> vTemp;
	for( MAPBUFF::iterator i = m_mapBuffs.begin(); i != m_mapBuffs.end(); ++i )
	{
		IBuff* pBuff	= i->second;
		BOOL bExpired	= pBuff->Process( GetMover(), this->GetCurrentTime() );
		if( bExpired )
			vTemp.push_back( MAKELONG( pBuff->GetId(), pBuff->GetType() ) );
	}
	while( !vTemp.empty() )
	{
		LONG l	= vTemp.back();
		RemoveBuff( HIWORD( l ), LOWORD( l ), FALSE );
		vTemp.pop_back();
	}
}

#ifdef __ANARCHY_SYSTEM
void CBuffMgr::RemoveBuffs(DWORD dwFlags, DWORD dwParam, bool bForceRemove)
#else // __ANARCHY_SYSTEM
void CBuffMgr::RemoveBuffs(DWORD dwFlags, DWORD dwParam)
#endif // __ANARCHY_SYSTEM
{
	vector<LONG> vTemp;
	for( MAPBUFF::iterator i = m_mapBuffs.begin(); i != m_mapBuffs.end(); ++i )
	{
		IBuff* pBuff	= i->second;
		if(
			( ( dwFlags & RBF_UNCONDITIONAL )
#ifndef __MIKSIK_EQUALIZED_MODE
#if __VER >= 13 // __HOUSING
				&& !pBuff->IsIk1( IK1_HOUSING )
#endif // __HOUSING	
#endif	
			)
			|| ( ( dwFlags & RBF_COMMON ) && GetMover() && pBuff->IsCommon() )
			|| ( ( dwFlags & RBF_IK3 ) && pBuff->IsIk3( dwParam ) )
			|| ( ( dwFlags & RBF_GOODSKILL ) && pBuff->IsGood( GetMover() ) )
			|| ( ( dwFlags & RBF_CHRSTATE ) && pBuff->HasChrState( GetMover(), dwParam ) )
			|| ( ( dwFlags & RBF_DSTPARAM ) && pBuff->HasDstParam( GetMover(), dwParam ) )
			|| ( ( dwFlags & RBF_DEBUF ) && pBuff->IsDebuf( GetMover() ) )
			|| ( ( dwFlags & RBF_ATTACKER ) && pBuff->IsAttacker( GetMover(), dwParam ) )
		)
		{
			vTemp.push_back( MAKELONG( pBuff->GetId(), pBuff->GetType() ) );
			if( dwFlags & RBF_ONCE )
				break;
		}
	}
	while( !vTemp.empty() )
	{
		LONG l	= vTemp.back();
#ifdef __ANARCHY_SYSTEM
		RemoveBuff(HIWORD(l), LOWORD(l), !bForceRemove);
#else // __ANARCHY_SYSTEM
		RemoveBuff(HIWORD(l), LOWORD(l));
#endif // __ANARCHY_SYSTEM
		vTemp.pop_back();
	}
}

DWORD CBuffMgr::GetDisguise()
{
	for( MAPBUFF::iterator i = m_mapBuffs.begin(); i != m_mapBuffs.end(); ++i )
	{
		DWORD dwIndex	= i->second->GetDisguise();
		if( dwIndex != NULL_ID )
			return dwIndex;
	}
	return NULL_ID;
}
#endif	// __DBSERVER

#ifdef __WORLDSERVER
IBuff* CBuffMgr::GetBuffPet()
{
	for( MAPBUFF::iterator i = m_mapBuffs.begin(); i != m_mapBuffs.end(); ++i )
	{
		IBuff* pBuff	= i->second;
		if( pBuff->GetType() == BUFF_PET )
			return pBuff;
	}
	return NULL;
}

void CBuffMgr::RemoveBuffPet( IBuff* pBuff )
{
	ASSERT( pBuff );
	RemoveBuff( pBuff, FALSE );
	CItemElem* pItem	= GetMover()->GetPetItem();
	if( pItem )
		GetMover()->ResetDestParamRandomOptExtension( pItem );
}

void CBuffMgr::RemoveBuffPet()
{
	IBuff* pBuff	= GetBuffPet();
	if( pBuff )
		RemoveBuffPet( pBuff );
}
#endif	// __WORLDSERVER

void CBuffMgr::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		// ar << m_mapBuffs.size();
		// chipi_090217
#ifdef _WIN64
		ar << static_cast<unsigned long>(m_mapBuffs.size() - GetRemoveBuffSize());
#else
		ar << m_mapBuffs.size() - GetRemoveBuffSize();
#endif
		for( MAPBUFF::iterator i = m_mapBuffs.begin(); i != m_mapBuffs.end(); ++i )
		{
			if( !i->second->GetRemove() )	// chipi_090217
				i->second->Serialize( ar, GetMover() );
		}
	}
	else
	{
		Clear();
#if _WIN64
		unsigned long t;
#else
		size_t t;
#endif
		ar >> t;
		for( size_t i = 0; i < t; i++ )
		{
			WORD wType;
			ar >> wType;
			IBuff* pBuff	
				= CreateBuff( wType );
			pBuff->Serialize( ar, GetMover() );
			bool bResult	= Add( pBuff );
			ASSERT( bResult );
		}
#ifndef __DBSERVER
		Process();
#endif // __DBSERVER
	}
}

CBuffMgr& CBuffMgr::operator =( CBuffMgr & bm )
{
	Clear();
	for( MAPBUFF::iterator i = bm.m_mapBuffs.begin(); i != bm.m_mapBuffs.end(); ++i )
	{
		IBuff* pSource	= i->second;
		IBuff* pDest	= CreateBuff( pSource->GetType() );
		*pDest	= *pSource;
		bool bResult	= Add( pDest );
		ASSERT( bResult );
	}
	return *this;
}


#ifdef __CLIENT
int CBuffMgr::GetCommercialCount()
{
	int nCount	= 0;
	for( MAPBUFF::iterator i = m_mapBuffs.begin(); i != m_mapBuffs.end(); ++i )
	{
		if( i->second->IsCommercial() )
			nCount++;
	}
	return nCount;
}

void CBuffMgr::ClearInst()
{
	for( MAPBUFF::iterator i = m_mapBuffs.begin(); i != m_mapBuffs.end(); ++i )
		i->second->SetInst( 0 );
}
#endif	// __CLIENT

#ifdef __DBSERVER
void CBuffMgr::ToString( char* szString )
{
	char szBuff[256]	= { 0,};
	for( MAPBUFF::iterator i = m_mapBuffs.begin(); i != m_mapBuffs.end(); ++i )
	{
		IBuff* pBuff	= i->second;
		if( pBuff->GetType() == BUFF_EQUIP )
			continue;
#if __VER >= 13 // __HOUSING
		if( pBuff->GetType() == BUFF_ITEM )
		{
			ItemProp* pProp	= prj.GetItemProp( pBuff->GetId() );
			if( pProp && pProp->dwItemKind1 == IK1_HOUSING )
				continue;
		}
#endif	// __HOUSING
		sprintf( szBuff, "%d,%d,%d,%d/", 
			pBuff->GetType(), pBuff->GetId(), pBuff->GetLevel(), pBuff->GetTotal() );
		strcat( szString, szBuff );
	}
	strcat( szString, NullStr );
}

#endif	// DBSERVER

// chipi_090217
size_t CBuffMgr::GetRemoveBuffSize()
{
	size_t nCount = 0;
	for( MAPBUFF::iterator it=m_mapBuffs.begin(); it!=m_mapBuffs.end(); it++ )
	{
		if( it->second->GetRemove() )
			nCount++;
	}

	return nCount;
}

#endif	// __BUFF_1107