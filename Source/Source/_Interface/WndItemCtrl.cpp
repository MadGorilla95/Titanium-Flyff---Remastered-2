// WndBase.cpp: implementation of the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AppDefine.h"
#include "DefineText.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;

#include "WndManager.h"
#include "WndText.h"

#ifdef __MODEL_VIEW
#include "WndModelView.h"
#endif //__MODEL_VIEW

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndItemCtrl
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTextureMng CWndItemCtrl::m_textureMng;

CWndItemCtrl::CWndItemCtrl() 
{
	m_nWndColor    = D3DCOLOR_TEMP( 255,  0x5 << 3,  0x5 << 3,  0x5 << 3 );
	m_nFontColor   = D3DCOLOR_TEMP( 255, 255, 255, 255 );
	m_nSelectColor = D3DCOLOR_TEMP( 255, 255, 255, 0   );
	m_nCurSelect = -1;
	m_pFocusItem = NULL;
	m_pItemContainer = NULL;
	m_bDrag = FALSE;
	m_nOnSelect = -1;

	memset( m_pArrayItemElem, 0, sizeof(CItemElem*)*100 );
	m_nArrayCount = 0;
	m_bVisibleCount = TRUE;
#if __VER >= 8 // __CSC_VER8_5
	m_useDieFlag = FALSE;
#endif // __CSC_VER8_5
#ifdef __SYS_LOCK_ITEM
	m_pLockItem = nullptr;
#endif // __SYS_LOCK_ITEM
}
CWndItemCtrl::~CWndItemCtrl()
{
//	ResetContent();
}
void CWndItemCtrl::Create( DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	CWndBase::Create( WBS_CHILD | dwListCtrlStyle, rect, pParentWnd, nID );
}
void CWndItemCtrl::InitItem( CItemContainer<CItemElem>* pItemContainer, DWORD SendToId )
{
	m_pItemContainer = pItemContainer;
	m_pFocusItem	= NULL;
	m_dwFrame = SendToId;
}

void CWndItemCtrl::OnInitialUpdate()
{
#ifdef __SYS_LOCK_ITEM
	m_wndLockMenu.CreateMenu(this);
	m_pLockTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "lock.png"), COLOR_ITEM_BG);
#endif // __SYS_LOCK_ITEM
	CRect rect = GetWindowRect();
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );//,m_pSprPack,-1);


#ifdef __AEGON_VIEW_INV
	m_wndViewInvMenu.CreateMenu(this);
#endif // __AEGON_VIEW_INV
}

#if __VER >= 9 // __CSC_VER9_1
void CWndItemCtrl::UpdateTooltip( void )
{
	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 32;
	int nHeight = rect.Height() / 32;

	CPoint point= GetMousePoint();
//	ClientToScreen( &point );

	// ½ºÅ©·Ñ¹Ù °ü·Ã   
	CPoint pt;
	pt.y = 0;
	pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

	for( int i = pt.y; i < m_pItemContainer->GetSize(); i++ ) 
	{
		if( i < 0 )
			continue;

		int x = ( i - pt.y ) % nWidth;
		int y = ( i - pt.y ) / nWidth;

		CItemElem* pItemElem = GetItem( i );
		if( pItemElem == NULL )
			continue;

#ifdef __AEGON_THEME_SWITCHER
		CPoint ptIconRender = CPoint(x * 32, y * 32);
		ptIconRender.y += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetY(y);
		ptIconRender.x += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetX(x);

		CRect rectHittest = CRect(ptIconRender.x, ptIconRender.y, ptIconRender.x + 32 - 2, ptIconRender.y + 32 - 2);
#else // __AEGON_THEME_SWITCHER
		CRect rectHittest = CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 );
#endif // __AEGON_THEME_SWITCHER

		if( rectHittest.PtInRect( point ) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &rectHittest );
			
			if( m_dwFrame == APP_SHOP_ )		// »óÁ¡
				g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_SHOP_ );
			else if( m_dwFrame == APP_INVENTORY || m_dwFrame == APP_BANK 
#ifdef __AEGON_VIEW_INV
				|| m_dwFrame == APP_VIEW_INVENTORY_CTRL
				|| m_dwFrame == APP_VIEW_BAG
#endif // __AEGON_VIEW_INV
				)	// ³ªÀÇ ÀÎº¥Åä¸®
			{
				if( g_WndMng.GetWndBase( APP_REPAIR ) )
				{
					g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_REPAIR );		
				}
#ifdef __AEGON_VIEW_INV
				else if (g_WndMng.GetWndBase(APP_VIEW_INVENTORY_CTRL))
				{
					g_WndMng.PutToolTipItemAndSubToolTip(pItemElem, point2, &rectHittest, APP_VIEW_INVENTORY_CTRL);
				}
				else if (g_WndMng.GetWndBase(APP_VIEW_BAG))
				{
					g_WndMng.PutToolTipItemAndSubToolTip(pItemElem, point2, &rectHittest, APP_VIEW_BAG);
				}
#endif // __AEGON_VIEW_INV
				else
				{
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
					g_WndMng.PutToolTipItemAndSubToolTip( pItemElem, point2, &rectHittest, APP_INVENTORY );
#else // __IMPROVE_SYSTEM_VER15
					g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_INVENTORY );		
#endif // __IMPROVE_SYSTEM_VER15
				}
			}
		}
	}
}

#endif	// __CSC_VER9_1

#ifdef __FIX_TOOLTIP_INVENTORY
void CWndItemCtrl::DrawToolTip(CPoint point)
{
	if (!m_pItemContainer)
		return;

	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 32;
	int nHeight = rect.Height() / 32;

	// ½ºÅ©·Ñ¹Ù °ü·Ã   
	CPoint pt;
	pt.y = 0;
	pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

	CWndBase* pWndBase = this->GetParentWnd();

	int nParent;
	if (pWndBase)
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if (pWndBaseTemp)
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
	{
		return;
	}

	// Test !?
	if (rect.PtInRect(point) == false)
		return;

	if (nParent == APP_INVENTORY
#ifdef __AEGON_VIEW_INV
		|| nParent == APP_VIEW_INVENTORY_CTRL
		|| nParent == APP_VIEW_BAG
#endif // __AEGON_VIEW_INV
		)
	{
		for (int i = pt.y; i < m_pItemContainer->GetSize(); i++)
		{
			if (i < 0) continue;
			int x = (i - pt.y) % nWidth;
			int y = (i - pt.y) / nWidth;
			CItemElem* pItemElem = GetItem(i);
			if (pItemElem == NULL) continue;
#ifdef __AEGON_THEME_SWITCHER
			CPoint ptIconRender = CPoint(x * 32, y * 32);
			ptIconRender.y += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetY(y);
			ptIconRender.x += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetX(x);

			CRect rectHittest = CRect(ptIconRender.x, ptIconRender.y, ptIconRender.x + 32 - 2, ptIconRender.y + 32 - 2);
#else // __AEGON_THEME_SWITCHER
			CRect rectHittest = CRect(x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2);
#endif // __AEGON_THEME_SWITCHER

			if (rectHittest.PtInRect(point))
			{
				CPoint point2 = point;
				ClientToScreen(&point2);
				ClientToScreen(&rectHittest);

				if (m_dwFrame == APP_SHOP_)		// »óÁ¡
					g_WndMng.PutToolTip_Item(pItemElem, point2, &rectHittest, APP_SHOP_);
				else
					if (m_dwFrame == APP_INVENTORY || m_dwFrame == APP_BANK
#ifdef __AEGON_VIEW_INV
						|| m_dwFrame == APP_VIEW_INVENTORY_CTRL
						|| m_dwFrame == APP_VIEW_BAG
#endif // __AEGON_VIEW_INV
						)	// ³ªÀÇ ÀÎº¥Åä¸®
					{
						if (g_WndMng.GetWndBase(APP_REPAIR))
						{
							g_WndMng.PutToolTip_Item(pItemElem, point2, &rectHittest, APP_REPAIR);
						}
#ifdef __AEGON_VIEW_INV
						else if (g_WndMng.GetWndBase(APP_VIEW_INVENTORY_CTRL))
						{
							g_WndMng.PutToolTipItemAndSubToolTip(pItemElem, point2, &rectHittest, APP_VIEW_INVENTORY_CTRL);
						}
						else if (g_WndMng.GetWndBase(APP_VIEW_BAG))
						{
							g_WndMng.PutToolTipItemAndSubToolTip(pItemElem, point2, &rectHittest, APP_VIEW_BAG);
						}
#endif // __AEGON_VIEW_INV
						else
						{
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
							g_WndMng.PutToolTipItemAndSubToolTip(pItemElem, point2, &rectHittest, APP_INVENTORY);
#else // __IMPROVE_SYSTEM_VER15
							g_WndMng.PutToolTip_Item(pItemElem, point2, &rectHittest, APP_INVENTORY);
#endif // __IMPROVE_SYSTEM_VER15
						}
					}

				m_nOnSelect = i;
			}
		}
	}
#if __VER >= 11 // __SYS_POCKET
	if (nParent == APP_BAG_EX)
	{
		for (int i = pt.y; i < m_pItemContainer->GetSize(); i++)
		{
			if (i < 0) continue;
			int x = (i - pt.y) % nWidth;
			int y = (i - pt.y) / nWidth;
			CItemElem* pItemElem = GetItem(i);
			if (pItemElem == NULL) continue;

#ifdef __AEGON_THEME_SWITCHER
			CPoint ptIconRender = CPoint(x * 32, y * 32);
			ptIconRender.y += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetY(y);
			ptIconRender.x += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetX(x);

			CRect rectHittest = CRect(ptIconRender.x, ptIconRender.y, ptIconRender.x + 32 - 2, ptIconRender.y + 32 - 2);
#else // __AEGON_THEME_SWITCHER
			CRect rectHittest = CRect(x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2);
#endif // __AEGON_THEME_SWITCHER

			if (rectHittest.PtInRect(point))
			{
				CPoint point2 = point;
				ClientToScreen(&point2);
				ClientToScreen(&rectHittest);

				g_WndMng.PutToolTip_Item(pItemElem, point2, &rectHittest, APP_INVENTORY);
				m_nOnSelect = i;
			}
		}
	}
#endif
	else
	{
		for (int i = 0; i < m_nArrayCount; i++)
		{
			if (i < 0) continue;
			int x = (i - pt.y) % nWidth;
			int y = (i - pt.y) / nWidth;
			CItemElem* pItemElem = m_pArrayItemElem[i];
			if (pItemElem == NULL) continue;

#ifdef __AEGON_THEME_SWITCHER
			CPoint ptIconRender = CPoint(x * 32, y * 32);
			ptIconRender.y += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetY(y);
			ptIconRender.x += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetX(x);

			CRect rectHittest = CRect(ptIconRender.x, ptIconRender.y, ptIconRender.x + 32 - 2, ptIconRender.y + 32 - 2);
#else // __AEGON_THEME_SWITCHER
			CRect rectHittest = CRect(x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2);
#endif // __AEGON_THEME_SWITCHER

			if (rectHittest.PtInRect(point))
			{
				CPoint point2 = point;
				ClientToScreen(&point2);
				ClientToScreen(&rectHittest);

				if (m_dwFrame == APP_SHOP_)		// »óÁ¡
				{
					g_WndMng.PutToolTip_Item(pItemElem, point2, &rectHittest, APP_SHOP_);
				}
				else
					if (m_dwFrame == APP_INVENTORY || m_dwFrame == APP_BANK
						|| m_dwFrame == APP_GUILD_BANK
						|| m_dwFrame == APP_COMMON_BANK
#ifdef __AEGON_VIEW_INV
						|| m_dwFrame == APP_VIEW_INVENTORY_CTRL
						|| m_dwFrame == APP_VIEW_BAG
#endif // __AEGON_VIEW_INV
						)	// ³ªÀÇ ÀÎº¥Åä¸®
					{
						if (g_WndMng.GetWndBase(APP_REPAIR))
						{
							g_WndMng.PutToolTip_Item(pItemElem, point2, &rectHittest, APP_REPAIR);
						}
#ifdef __AEGON_VIEW_INV
						else if (g_WndMng.GetWndBase(APP_VIEW_INVENTORY_CTRL))
						{
							g_WndMng.PutToolTip_Item(pItemElem, point2, &rectHittest, APP_VIEW_INVENTORY_CTRL);
						}
						else if (g_WndMng.GetWndBase(APP_VIEW_BAG))
						{
							g_WndMng.PutToolTip_Item(pItemElem, point2, &rectHittest, APP_VIEW_BAG);
						}
#endif // __AEGON_VIEW_INV
						else
						{
							g_WndMng.PutToolTip_Item(pItemElem, point2, &rectHittest, APP_INVENTORY);
						}
					}

				m_nOnSelect = i;
			}
		}
	}
}
#else //__FIX_TOOLTIP_INVENTORY
void CWndItemCtrl::OnMouseWndSurface( CPoint point )
{
	if( !m_pItemContainer )
		return;

	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 32;
	int nHeight = rect.Height() / 32;

	// ½ºÅ©·Ñ¹Ù °ü·Ã   
	CPoint pt;
	pt.y = 0;
	pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

	CWndBase* pWndBase = this->GetParentWnd();
	
	int nParent;
	if( pWndBase )
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if( pWndBaseTemp )
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
	{
		return;
	}

	if( nParent == APP_INVENTORY 
#ifdef __AEGON_VIEW_INV
		|| nParent == APP_VIEW_INVENTORY_CTRL
#endif // __AEGON_VIEW_INV
		)
	{
		for( int i = pt.y; i < m_pItemContainer->GetSize(); i++ ) 
		{
			if( i < 0 ) continue;
			int x = ( i - pt.y ) % nWidth;
			int y = ( i - pt.y ) / nWidth;
			CItemElem* pItemElem = GetItem( i );
			if( pItemElem == NULL ) continue;
			CRect rectHittest = CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 );

			if( rectHittest.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &rectHittest );
				
				if( m_dwFrame == APP_SHOP_ )		// »óÁ¡
					g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_SHOP_ );
				else 
				if( m_dwFrame == APP_INVENTORY || m_dwFrame == APP_BANK 
#ifdef __AEGON_VIEW_INV
					|| m_dwFrame == APP_VIEW_INVENTORY_CTRL
#endif // __AEGON_VIEW_INV
					)	// ³ªÀÇ ÀÎº¥Åä¸®
				{
					if( g_WndMng.GetWndBase( APP_REPAIR ) )
					{
						g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_REPAIR );		
					}
#ifdef __AEGON_VIEW_INV
					else if (g_WndMng.GetWndBase(APP_VIEW_INVENTORY_CTRL))
					{
						g_WndMng.PutToolTipItemAndSubToolTip(pItemElem, point2, &rectHittest, APP_VIEW_INVENTORY_CTRL);
					}
#endif // __AEGON_VIEW_INV
					else
					{
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
						g_WndMng.PutToolTipItemAndSubToolTip( pItemElem, point2, &rectHittest, APP_INVENTORY );
#else // __IMPROVE_SYSTEM_VER15
						g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_INVENTORY );		
#endif // __IMPROVE_SYSTEM_VER15
					}
				}

				m_nOnSelect = i;
			}
		}
	}
#if __VER >= 11 // __SYS_POCKET
	if( nParent == APP_BAG_EX )
	{
		for( int i = pt.y; i < m_pItemContainer->GetSize(); i++ ) 
		{
			if( i < 0 ) continue;
			int x = ( i - pt.y ) % nWidth;
			int y = ( i - pt.y ) / nWidth;
			CItemElem* pItemElem = GetItem( i );
			if( pItemElem == NULL ) continue;
			CRect rectHittest = CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 );

			if( rectHittest.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &rectHittest );
					
				g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_INVENTORY );		
				m_nOnSelect = i;
			}
		}
	}
#endif
	else
	{
		for( int i = 0; i < m_nArrayCount; i++ ) 
		{
			if( i < 0 ) continue;
			int x = ( i - pt.y ) % nWidth;
			int y = ( i - pt.y ) / nWidth;
			CItemElem* pItemElem = m_pArrayItemElem[i];
			if( pItemElem == NULL ) continue;
			CRect rectHittest = CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 );

			if( rectHittest.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &rectHittest );
				
				if( m_dwFrame == APP_SHOP_ )		// »óÁ¡
					g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_SHOP_ );
				else 
				if( m_dwFrame == APP_INVENTORY || m_dwFrame == APP_BANK
				 || m_dwFrame == APP_GUILD_BANK
				 || m_dwFrame == APP_COMMON_BANK
#ifdef __AEGON_VIEW_INV
					|| m_dwFrame == APP_VIEW_INVENTORY_CTRL
#endif // __AEGON_VIEW_INV
					)	// ³ªÀÇ ÀÎº¥Åä¸®
				{
					if( g_WndMng.GetWndBase( APP_REPAIR ) )
					{
						g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_REPAIR );		
					}
#ifdef __AEGON_VIEW_INV
					else if (g_WndMng.GetWndBase(APP_VIEW_INVENTORY_CTRL))
					{
						g_WndMng.PutToolTip_Item(pItemElem, point2, &rectHittest, APP_VIEW_INVENTORY_CTRL);
					}
#endif // __AEGON_VIEW_INV
					else
					{
						g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_INVENTORY );		
					}
				}

				m_nOnSelect = i;
			}
		}
	}



}
#endif //__FIX_TOOLTIP_INVENTORY

void CWndItemCtrl::RanderIcon( C2DRender* p2DRender, CItemElem* pItemElem, int x, int y, int nParent, int nalpha ) 
{
	int bRander = 0;

	if( nParent == APP_SHOP_ )
	{
		ItemProp* pItemProp		= pItemElem->GetProp();
		if( pItemProp == NULL ) return;

		switch( pItemProp->dwItemKind2 )
		{
			case IK2_WEAPON_DIRECT:
//			case IK2_WEAPON_RANGE:
			case IK2_WEAPON_MAGIC:
//			case IK2_WEAPON_DOLL:
//			case IK2_WEAPON_GUN:
			case IK2_ARMOR:
			case IK2_ARMORETC:
			case IK2_CLOTH:
			case IK2_CLOTHETC:
			{
					
				// ¹«±â : Á÷¾÷, ´É·ÂÄ¡ ½ºÅÝ, °¡°Ý
				if( pItemProp->dwItemSex != 0xffffffff && pItemProp->dwItemSex != g_pPlayer->GetSex() )
				{
					bRander = 1;
					break;
				}

				if( JOB_VAGRANT != pItemProp->dwItemJob )
				{
					if( pItemProp->dwItemJob != 0xffffffff && JOB_VAGRANT < pItemProp->dwItemJob && !(g_pPlayer->IsInteriorityJob( pItemProp->dwItemJob ) ) )
					{
						bRander = 1;
						break;
					}
				}
#if __VER >= 14 // __NEW_ITEM_LIMIT_LEVEL
				if( pItemProp->dwLimitLevel1 != 0xffffffff && pItemElem->IsLimitLevel( g_pPlayer ) )
#else // __NEW_ITEM_LIMIT_LEVEL
#if __VER >= 11 // __SYS_IDENTIFY
				if( pItemProp->dwLimitLevel1 != 0xffffffff && g_pPlayer->GetLevel() < pItemElem->GetLimitLevel() )
#else	// __SYS_IDENTIFY
				if( pItemProp->dwLimitLevel1 != 0xffffffff && g_pPlayer->GetLevel() < pItemProp->dwLimitLevel1 )
#endif	// __SYS_IDENTIFY
#endif // __NEW_ITEM_LIMIT_LEVEL
				{
					bRander = 1;
					break;
				}
				break;
			}
			case IK2_RIDING:
//			case IK2_BOARDING:
//			case IK2_HATCH:
			{
				// ºñÇà : ºñÇà·¹º§
				if( g_pPlayer->GetFlightLv() < (int)( pItemProp->dwFlightLimit ) )
				{
					bRander = 1;
					break;
				}
				break;
			}
		}
 
		// ¸ðµç¾ÆÀÌÅÛ °¡°Ý ºñ±³
#if __VER >= 11 // __CSC_VER11_3
		if( nParent == APP_SHOP_ )
		{
			CWndShop* pWndBase = (CWndShop*)g_WndMng.GetWndBase( APP_SHOP_ );
			LPCHARACTER lpCharacter = pWndBase->m_pMover->GetCharacter();
#ifdef __VENDOR_UPDATE
			if (lpCharacter && lpCharacter->m_nVenderType != 0)
			{
#ifdef __SQK_MULTIALTERNATEPRICE
				int cost = pItemElem->GetCurrencyCost(lpCharacter->m_nVenderType);
				if (bRander == 0 && g_pPlayer->m_Inventory.GetAtItemNum(lpCharacter->m_nVenderType) < cost)
#else //__SQK_MULTIALTERNATEPRICE
				if (bRander == 0 && g_pPlayer->m_Inventory.GetAtItemNum(lpCharacter->m_nVenderType) < (int)pItemElem->GetChipCost())
#endif //__SQK_MULTIALTERNATEPRICE
					bRander = 1;
			}
#else // __VENDOR_UPDATE
			if (lpCharacter && lpCharacter->m_nVenderType == 1)
			{
				if (bRander == 0 && g_pPlayer->m_Inventory.GetAtItemNum(II_CHP_RED) < (int)pItemElem->GetChipCost())
					bRander = 1;
			}
#endif // __VENDOR_UPDATE
		}
		else
		{
			if( bRander == 0 && g_pPlayer->GetGold() < (int)pItemProp->dwCost )
				bRander = 1;
		}
#else //__CSC_VER11_3
		if( bRander == 0 && g_pPlayer->GetGold() < (int)pItemProp->dwCost )
			bRander = 1;
#endif //__CSC_VER11_3
	}

#ifdef __VENDING_NPC
	if (pItemElem->IsFlag(CItemElem::expired) && pItemElem->GetPropA() && pItemElem->GetPropA()->dwItemKind3 != IK3_VENDING)
#else // __VENDING_NPC
	if (pItemElem->IsFlag(CItemElem::expired))
#endif // __VENDING_NPC
	{
		bRander = 1;
	}

#ifdef	__GUILD_BANK_SEARCH
	CWndBase* pWndBase = this->GetParentWnd();

	if (pWndBase)
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if (pWndBaseTemp)
			nParent = pWndBaseTemp->GetWndId();

		if (nParent == APP_GUILD_BANK) // Bank Search
		{
			CWndEdit* pSearchBox2 = (CWndEdit*)pWndBaseTemp->GetDlgItem(WIDC_EDIT2);
			if (pSearchBox2)
			{
				CString strFilter = pSearchBox2->GetString();
				if (strFilter.GetLength())
				{
					CString strName = pItemElem->GetName();
					if (!strstr(strName.MakeLower(), strFilter.MakeLower()))
						nalpha = 70;
				}
			}
		}

#endif // __GUILD_BANK_SEARCH

#ifdef __SHOP_SEARCH
		if (nParent == APP_SHOPEX) // Shop Search
		{
			CWndEdit* pSearchBox5 = (CWndEdit*)pWndBaseTemp->GetDlgItem(WIDC_EDIT);
			if (pSearchBox5)
			{
				CString strFilter = pSearchBox5->GetString();
				if (strFilter.GetLength())
				{
					CString strName = pItemElem->GetName();
					if (!strstr(strName.MakeLower(), strFilter.MakeLower()))
						nalpha = 70;
				}
			}
		}
#endif //__SHOP_SEARCH

	if( !g_eLocal.GetState(EVE_SCHOOL) )
	{
		if( bRander == 0 )
		{
			pItemElem->GetTexture()->Render( p2DRender, CPoint( x, y ), nalpha );
		}
		else
		if( bRander == 1 )	// »ç¿ëÇÒ¼ö ¾ø´Â°Í
		{
			pItemElem->GetTexture()->Render2( p2DRender, CPoint( x, y ), D3DCOLOR_ARGB( nalpha, 255, 100, 100 ) );
		}
	}
	else
		pItemElem->GetTexture()->Render( p2DRender, CPoint( x, y ), nalpha );
#ifdef __GUILD_BANK_SEARCH
	}
#endif //__GUILD_BANK_SEARCH
}

void CWndItemCtrl::OnDraw(C2DRender* p2DRender)
{
#ifdef __SYS_TICKET
	CMover* pPlayer = g_pPlayer;
	if (!pPlayer)
		return;
	CWorld* pWorld = pPlayer->GetWorld();
	if (!pWorld)
		return;
#endif	// __SYS_TICKET

	if (!m_pItemContainer)
		return;

#ifdef __ACTIVE_ITEM_ICON_V17
	CTexture* CheckTexture;
	CTexture* CheckVendTexture;
#endif //__ACTIVE_ITEM_ICON_V17

	DWORD dwCur = GetTickCount();
	CPoint pt(3, 3);

	CWndBase* pWndBase = this->GetParentWnd();

	int nParent;
	if (pWndBase)
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if (pWndBaseTemp)
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
	{
		return;
	}

	if (0) //m_dwListCtrlStyle == WLVS_REPORT )
	{
		m_nFontHeight = GetFontHeight();
		pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
		for (int i = 0; i < (int)(m_pItemContainer->m_dwIndexNum); i++)
		{
			DWORD dwColor = 0xffffffff;
			if (i == m_nCurSelect)
				dwColor = 0xff00ff00;
			CItemElem* pItemElem = &m_pItemContainer->m_apItem[i];
			if (pItemElem->m_dwItemId)
			{
				for (int i2 = 0, x = 0; i2 < m_aColumns.GetSize(); i2++)
				{
					switch (i2)
					{
					case 0:
						p2DRender->TextOut(x + 2, pt.y, pItemElem->GetName(), dwColor);
						break;
					case 1:
						break;
					case 2:
						break;
					}
					LVCOLUMN* pColumn = (LVCOLUMN*)m_aColumns.GetAt(i2);
					x += pColumn->cx + 7;
				}
			}
			pt.y += m_nFontHeight + 3;
		}
	}

	if (1) //m_dwListCtrlStyle == WLVS_ICON )
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 32;
		int nHeight = rect.Height() / 32;

		int nPage = nHeight;
		int nRange = m_pItemContainer->m_dwIndexNum / nWidth;// - nPage;

		if (m_pItemContainer->m_dwIndexNum % nWidth)
			nRange++;

		m_wndScrollBar.SetScrollRange(0, nRange);
		m_wndScrollBar.SetScrollPage(nPage);

		pt.y = 0;
		pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
#if __VER >= 11 // __SYS_POCKET
		if (nParent == APP_INVENTORY || nParent == APP_BAG_EX
#ifdef __AEGON_VIEW_INV
			|| nParent == APP_VIEW_INVENTORY_CTRL
			|| nParent == APP_VIEW_BAG
#endif // __AEGON_VIEW_INV
			)
#else
		if (nParent == APP_INVENTORY)
#endif
		{
			for (int i = pt.y; i < m_pItemContainer->GetSize(); i++)
			{
				if (i < 0) continue;
				int x = (i - pt.y) % nWidth;
				int y = (i - pt.y) / nWidth;
				CItemElem* pItemElem = GetItem(i);

#ifdef __AEGON_THEME_SWITCHER
				CPoint ptIconRender = CPoint(x * 32, y * 32);
				ptIconRender.y += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetY(y);
				ptIconRender.x += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetX(x);
#endif // __AEGON_THEME_SWITCHER

				if (pItemElem == NULL) continue;

				if (pItemElem->GetTexture())
				{
#if __VER >= 8 //__CSC_VER8_5
					if (
						(pItemElem->m_nItemNum == pItemElem->GetExtra() || !m_useDieFlag)
						&& (IsUsingItem(pItemElem) || pItemElem->m_bRepair
#if __VER >= 9	// __PET_0410
							|| (nParent == APP_INVENTORY && g_pPlayer->GetPetId() == pItemElem->m_dwObjId)
#endif	// __PET_0410
#ifdef __SYS_TICKET
							|| pPlayer->GetTicket() == pItemElem
#endif	// __SYS_TICKET
#ifdef __AEGON_FIXES
							|| pPlayer->GetEatPetItem() == pItemElem
							|| pPlayer->GetVisPetItem() == pItemElem
#endif
							)
						)
#else
					if (IsUsingItem(pItemElem) || pItemElem->m_bRepair)
#endif	//__CSC_VER8_5
					{

#ifdef __AEGON_THEME_SWITCHER
						RanderIcon(p2DRender, pItemElem, ptIconRender.x, ptIconRender.y, nParent, 60);
						CheckTexture = m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_THEME, "WndItemCheck.bmp"), COLOR_ITEM_BG);
						CheckVendTexture = m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_THEME, "WndVendCheck.bmp"), COLOR_ITEM_BG);
						if (g_pPlayer->m_vtInfo.IsTrading(pItemElem))
						{
							if (CheckVendTexture != NULL)
							{
								CheckVendTexture->Render(p2DRender, CPoint(ptIconRender.x, ptIconRender.y + 1), 255);
							}
						}
						else
						{
							if (CheckTexture != NULL)
							{
								CheckTexture->Render(p2DRender, CPoint(ptIconRender.x, ptIconRender.y + 1), 255);
							}
						}
#else // __AEGON_THEME_SWITCHER
						RanderIcon(p2DRender, pItemElem, x * 32, y * 32, nParent, 60);
#ifdef __ACTIVE_ITEM_ICON_V17
						CheckTexture = m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_THEME, "WndItemCheck.bmp"), COLOR_ITEM_BG);
						if (CheckTexture != NULL)
						{
							CheckTexture->Render(p2DRender, CPoint(x * 32, (y * 32) + 1), 255);
						}
#endif // __ACTIVE_ITEM_ICON_V17
#endif // __AEGON_THEME_SWITCHER
					}
#ifndef __INVENTORY_SEARCH
					else if (m_nOnSelect == i)
					{
						pItemElem->GetTexture()->m_size.cx += 4;
						pItemElem->GetTexture()->m_size.cy += 4;
#if __VER >= 9 // __CSC_VER9_1
						if ((pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_FEED_POCKET && pItemElem->m_dwKeepTime <= 0)
#ifdef __SYS_TICKET
							|| pPlayer->GetTicket() == pItemElem
#endif	// __SYS_TICKET
							)
							RanderIcon(p2DRender, pItemElem, x * 32 - 2, y * 32 - 2, nParent, 100);
						else
							RanderIcon(p2DRender, pItemElem, x * 32 - 2, y * 32 - 2, nParent);
#else //__CSC_VER9_1
						RanderIcon(p2DRender, pItemElem, x * 32 - 2, y * 32 - 2, nParent);
#endif //__CSC_VER9_1
						pItemElem->GetTexture()->m_size.cx -= 4;
						pItemElem->GetTexture()->m_size.cy -= 4;
					}
#endif // __INVENTORY_SEARCH
					else
					{
#if __VER >= 9 // __CSC_VER9_1
#ifdef __AEGON_THEME_SWITCHER
						if (pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_FEED_POCKET && pItemElem->m_dwKeepTime <= 0) //¸ÔÀÌ ÁÖ¸Ó´Ï ¾ÆÀÌÅÛÀÌ »ç¿ë¾ÈµÈ »óÅÂÀÎ°¡?
							RanderIcon(p2DRender, pItemElem, ptIconRender.x, ptIconRender.y, nParent, 100);
#else // __AEGON_THEME_SWITCHER
						if (pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_FEED_POCKET && pItemElem->m_dwKeepTime <= 0) //¸ÔÀÌ ÁÖ¸Ó´Ï ¾ÆÀÌÅÛÀÌ »ç¿ë¾ÈµÈ »óÅÂÀÎ°¡?
							RanderIcon(p2DRender, pItemElem, x * 32, y * 32, nParent, 100);
#endif // __AEGON_THEME_SWITCHER
						else
						{
#ifdef __INVENTORY_SEARCH
							int nAlpha = 255;
							if (nParent == APP_INVENTORY)
							{
								CWndInventory* pWndInventory = dynamic_cast<CWndInventory*>(GetWndBase(APP_INVENTORY));
								if (pWndInventory)
								{
									CWndEdit* pWndSearch = pWndInventory->GetWndSearch();
									if (pWndSearch)
									{
										CString strFilter = pWndSearch->GetString();
										ItemProp* pItemProp = pItemElem->GetProp();
										if (pItemProp && strFilter.GetLength())
										{
											if (!strstr(pItemProp->szName, "?")
												&& strlen(pItemProp->szName))
											{
												CString strName = pItemProp->szName;
												if (!strstr(strName.MakeLower(), strFilter.MakeLower()))
													nAlpha = 70;
											}
										}
									}
								}
							}
#ifdef __AEGON_THEME_SWITCHER
							RanderIcon(p2DRender, pItemElem, ptIconRender.x, ptIconRender.y, nParent, nAlpha);
#else // __AEGON_THEME_SWITCHER
							RanderIcon(p2DRender, pItemElem, x * 32, y * 32, nParent, nAlpha);
#endif // __AEGON_THEME_SWITCHER
#else // __INVENTORY_SEARCH
							RanderIcon(p2DRender, pItemElem, x * 32, y * 32, nParent);
#endif // __INVENTORY_SEARCH
						}
#else //__CSC_VER9_1
						RanderIcon(p2DRender, pItemElem, x * 32, y * 32, nParent);
#endif //__CSC_VER9_1
#ifdef __DEV
						if (pItemElem->GetAbilityOption() > 0)
						{
							TCHAR szTemp[32];
							_stprintf(szTemp, "+%d", pItemElem->GetAbilityOption());
							CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
							m_p2DRender->TextOut(x * 32 + 32 - size.cx, y * 32 + 3, szTemp, D3DCOLOR_ARGB(255, 64, 64, 64));
							m_p2DRender->TextOut(x * 32 + 31 - size.cx, y * 32 + 2, szTemp, D3DCOLOR_ARGB(255, 176, 176, 176));
						}
#endif //__DEV
					}

					if (pItemElem->GetProp()->dwPackMax > 1)
					{
						short m_nItemNum = pItemElem->m_nItemNum;
						if (pItemElem->GetExtra() > 0)
							m_nItemNum -= (short)pItemElem->GetExtra();
						TCHAR szTemp[32];
						_stprintf(szTemp, "%d", m_nItemNum);

#ifdef __LEESE_RENDER_ITEMCOUNT
						CD3DFont* pOldFont = p2DRender->GetFont();
						p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemCount);
#endif //__LEESE_RENDER_ITEMCOUNT
						CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
#ifdef __LEESE_RENDER_ITEMCOUNT
#ifdef __AEGON_THEME_SWITCHER
						m_p2DRender->TextOut(ptIconRender.x + 32 - size.cx, ptIconRender.y + 33 - size.cy, szTemp, 0xFF000000);
						m_p2DRender->TextOut(ptIconRender.x + 31 - size.cx, ptIconRender.y + 32 - size.cy, szTemp, 0xFFffb912);
#else //__AEGON_THEME_SWITCHER
						m_p2DRender->TextOut(x * 32 + 32 - size.cx, y * 32 + 32 - size.cy, szTemp, 0xFF000000);
						m_p2DRender->TextOut(x * 32 + 31 - size.cx, y * 32 + 31 - size.cy, szTemp, 0xFFffb912);
#endif //__AEGON_THEME_SWITCHER
#else //__LEESE_RENDER_ITEMCOUNT
#ifdef __AEGON_THEME_SWITCHER
						m_p2DRender->TextOut(ptIconRender.x + 32 - size.cx, ptIconRender.y + 33 - size.cy, szTemp, 0xff0000ff);
						m_p2DRender->TextOut(ptIconRender.x + 31 - size.cx, ptIconRender.y + 32 - size.cy, szTemp, 0xffb0b0f0);
#else //__AEGON_THEME_SWITCHER
						m_p2DRender->TextOut(x * 32 + 32 - size.cx, y * 32 + 32 - size.cy, szTemp, 0xff0000ff);
						m_p2DRender->TextOut(x * 32 + 31 - size.cx, y * 32 + 31 - size.cy, szTemp, 0xffb0b0f0);
#endif //__AEGON_THEME_SWITCHER
#endif //__LEESE_RENDER_ITEMCOUNT
#ifdef __LEESE_RENDER_ITEMCOUNT
						p2DRender->SetFont(pOldFont);
#endif //__LEESE_RENDER_ITEMCOUNT
					}

					DWORD dwGroup = g_pPlayer->m_cooltimeMgr.GetGroup(pItemElem->GetProp());
					if (dwGroup)
					{
						DWORD dwEnd = g_pPlayer->m_cooltimeMgr.GetTime(dwGroup);		// 이벤트 종료 시각 
						if (dwEnd != 0 && dwEnd > dwCur)
						{
							DWORD dwBase = g_pPlayer->m_cooltimeMgr.GetBase(dwGroup);	// 이벤트 시작 시각 
#ifdef __AEGON_THEME_SWITCHER
							CPoint pt(ptIconRender.x, ptIconRender.y);
#else // __AEGON_THEME_SWITCHER
							CPoint pt(x * 32, y * 32);
#endif// __AEGON_THEME_SWITCHER
							RenderRadar(p2DRender, pt, dwCur - dwBase, dwEnd - dwBase);
						}
					}

					CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);
					if (pWndInventory)
					{
						if (pWndInventory->m_pUpgradeItem && pItemElem)
						{
							if (pWndInventory->m_pUpgradeItem == pItemElem)
							{
								if (pWndInventory->m_dwEnchantWaitTime != 0xffffffff)
								{
#ifdef __AEGON_THEME_SWITCHER
									CPoint pt(ptIconRender.x, ptIconRender.y);
#else // __AEGON_THEME_SWITCHER
									CPoint pt(x * 32, y * 32);
#endif // __AEGON_THEME_SWITCHER
									RenderEnchant(p2DRender, pt);
								}
							}
						}
					}
				}

#ifdef __SYS_LOCK_ITEM
				if (pItemElem->IsItemLocked())
				{
#ifdef __AEGON_THEME_SWITCHER
					p2DRender->RenderTexture2(CPoint(ptIconRender.x + 17, ptIconRender.y + 1), m_pLockTexture, 0.95f, 0.95f, D3DCOLOR_XRGB(255, 255, 255));
#else // __AEGON_THEME_SWITCHER
					p2DRender->RenderTexture2(CPoint((x * 33) + 17, (y * 32) + 1), m_pLockTexture, 0.95f, 0.95f, D3DCOLOR_XRGB(255, 255, 255));
#endif // __AEGON_THEME_SWITCHER
				}
#endif // __SYS_LOCK_ITEM

#ifdef __WEAPON_RARITY
				WeaponRarity* nWeaponRarity = prj.m_nWeaponRarity.GetAt(pItemElem->GetWeaponRarity());
				CTexture* RarityTexture = NULL;
				if (nWeaponRarity && pItemElem->GetWeaponRarity() > 0)
				{
					if (pItemElem->GetWeaponRarity() == 0) // Common 
					{
						RarityTexture = m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_ITEM, "Rare-Item-Icons_0000s_0005_1.png"), COLOR_ITEM_BG);
					}
					else if (pItemElem->GetWeaponRarity() == 1) // Precious
					{
						RarityTexture = m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_ITEM, "Rare-Item-Icons_0000s_0004_2.png"), COLOR_ITEM_BG);
					}
					else if (pItemElem->GetWeaponRarity() == 2) // Rare
					{
						RarityTexture = m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_ITEM, "Rare-Item-Icons_0000s_0002_4.png"), COLOR_ITEM_BG);
					}
					else if (pItemElem->GetWeaponRarity() == 3) // Epic
					{
						RarityTexture = m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_ITEM, "Rare-Item-Icons_0000s_0000_6.png"), COLOR_ITEM_BG);
					}
					else if (pItemElem->GetWeaponRarity() == 4) // Legendary
					{
						RarityTexture = m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_ITEM, "Rare-Item-Icons_0000s_0003_3.png"), COLOR_ITEM_BG);
					}
					else if (pItemElem->GetWeaponRarity() == 5) // Mystique
					{
						RarityTexture = m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_ITEM, "Rare-Item-Icons_0000s_0001_5.png"), COLOR_ITEM_BG);
					}

					if (RarityTexture)
					{
#ifdef __AEGON_THEME_SWITCHER
						RarityTexture->Render(p2DRender, CPoint(ptIconRender.x, ptIconRender.y + 1), 255);
#else // __AEGON_THEME_SWITCHER
						RarityTexture->Render(p2DRender, CPoint((x * 32) + 2, (y * 32)), 255);
#endif // __AEGON_THEME_SWITCHER
					}
				}
			
#endif // __WEAPON_RARITY
#ifdef __AEGON_THEME_SWITCHER
				CRect rectHittest = CRect(ptIconRender.x, ptIconRender.y, ptIconRender.x + 32 - 2, ptIconRender.y + 32 - 2);
#else // __AEGON_THEME_SWITCHER
				CRect rectHittest = CRect(x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2);
#endif // __AEGON_THEME_SWITCHER
			}
		}
		else
		{
			m_nArrayCount = 0;

			if (nParent == APP_SHOP_)
			{
				CWndShop* pWndBase = (CWndShop*)g_WndMng.GetWndBase(APP_SHOP_);
				for (int i = 0; i < m_pItemContainer->GetSize(); i++)
				{
					CItemElem* pItemElem = GetItem(i);
					if (pItemElem == NULL)
						continue;

					ItemProp* pItemProp = pItemElem->GetProp();
					if (pItemProp == NULL)
						continue;

					if (pWndBase)
					{
						if (pWndBase->m_bSexSort && pWndBase->m_bLevelSort)
						{
							if (pItemProp->dwItemSex != 0xffffffff && pItemProp->dwLimitLevel1 != 0xffffffff)
							{
#if __VER >= 14 // __NEW_ITEM_LIMIT_LEVEL
								if (pItemProp->dwItemSex == g_pPlayer->GetSex() && !pItemElem->IsLimitLevel(g_pPlayer))
#else // __NEW_ITEM_LIMIT_LEVEL
#if __VER >= 11 // __SYS_IDENTIFY
								if (pItemProp->dwItemSex == g_pPlayer->GetSex() && pItemElem->GetLimitLevel() <= g_pPlayer->m_nLevel)
#else	// __SYS_IDENTIFY
								if (pItemProp->dwItemSex == g_pPlayer->GetSex() && pItemProp->dwLimitLevel1 <= g_pPlayer->m_nLevel)
#endif	// __SYS_IDENTIFY
#endif // __NEW_ITEM_LIMIT_LEVEL
									m_pArrayItemElem[m_nArrayCount++] = pItemElem;
							}
							else
								if (pItemProp->dwItemSex != 0xffffffff)
								{
									if (pItemProp->dwItemSex == g_pPlayer->GetSex())
										m_pArrayItemElem[m_nArrayCount++] = pItemElem;
								}
								else
									if (pItemProp->dwLimitLevel1 != 0xffffffff)
									{
#if __VER >= 14 // __NEW_ITEM_LIMIT_LEVEL
										if (!pItemElem->IsLimitLevel(g_pPlayer))
#else // __NEW_ITEM_LIMIT_LEVEL
#if __VER >= 11 // __SYS_IDENTIFY
										if (pItemElem->GetLimitLevel() <= g_pPlayer->m_nLevel)
#else	// __SYS_IDENTIFY
										if (pItemProp->dwLimitLevel1 <= g_pPlayer->m_nLevel)
#endif	// __SYS_IDENTIFY
#endif // __NEW_ITEM_LIMIT_LEVEL
											m_pArrayItemElem[m_nArrayCount++] = pItemElem;
									}
									else
										m_pArrayItemElem[m_nArrayCount++] = pItemElem;
						}
						else
							if (pWndBase->m_bSexSort)
							{
								if (pItemProp->dwItemSex != 0xffffffff)
								{
									if (pItemProp->dwItemSex == g_pPlayer->GetSex())
										m_pArrayItemElem[m_nArrayCount++] = pItemElem;
								}
								else
									m_pArrayItemElem[m_nArrayCount++] = pItemElem;
							}
							else
								if (pWndBase->m_bLevelSort)
								{
									if (pItemProp->dwLimitLevel1 != 0xffffffff)
									{
#if __VER >= 14 // __NEW_ITEM_LIMIT_LEVEL
										if (!pItemElem->IsLimitLevel(g_pPlayer))
#else // __NEW_ITEM_LIMIT_LEVEL
#if __VER >= 11 // __SYS_IDENTIFY
										if (pItemElem->GetLimitLevel() <= g_pPlayer->m_nLevel)
#else	// __SYS_IDENTIFY
										if (pItemProp->dwLimitLevel1 <= g_pPlayer->m_nLevel)
#endif	// __SYS_IDENTIFY
#endif // __NEW_ITEM_LIMIT_LEVEL
											m_pArrayItemElem[m_nArrayCount++] = pItemElem;
									}
									else
										m_pArrayItemElem[m_nArrayCount++] = pItemElem;
								}
								else
									m_pArrayItemElem[m_nArrayCount++] = pItemElem;
					}
				}
			}
			else
			{
				for (int i = 0; i < m_pItemContainer->GetSize(); i++)
				{
					CItemElem* pItemElem = GetItem(i);
					if (pItemElem == NULL)
						continue;

					ItemProp* pItemProp = pItemElem->GetProp();
					if (pItemProp == NULL)
						continue;

					m_pArrayItemElem[m_nArrayCount++] = pItemElem;
				}
			}

			for (int i = 0; i < m_nArrayCount; i++)
			{
				int x = (i - pt.y) % nWidth;
				int y = (i - pt.y) / nWidth;
				CItemElem* pItemElem = m_pArrayItemElem[i];
				if (pItemElem == NULL)
					continue;

#ifdef __AEGON_THEME_SWITCHER
				CPoint ptIconRender = CPoint(x * 32, y * 32);
				ptIconRender.y += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetY(y);
				ptIconRender.x += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetX(x);
#endif // __AEGON_THEME_SWITCHER

				if (pItemElem->m_pTexture)
				{
#ifdef __AEGON_THEME_SWITCHER
					if (IsUsingItem(pItemElem) || pItemElem->m_bRepair)
						RanderIcon(p2DRender, pItemElem, ptIconRender.x, ptIconRender.y, nParent, 60);
#else // __AEGON_THEME_SWITCHER
					if (IsUsingItem(pItemElem) || pItemElem->m_bRepair)
						RanderIcon(p2DRender, pItemElem, x * 32, y * 32, nParent, 60);
#endif // __AEGON_THEME_SWITCHER
					else
						if (m_nOnSelect == i)
						{
							pItemElem->GetTexture()->m_size.cx += 4;
							pItemElem->GetTexture()->m_size.cy += 4;
#ifdef __AEGON_THEME_SWITCHER
							RanderIcon(p2DRender, pItemElem, ptIconRender.x - 2, ptIconRender.y - 2, nParent);
#else // __AEGON_THEME_SWITCHER
							RanderIcon(p2DRender, pItemElem, x * 32 - 2, y * 32 - 2, nParent);
#endif // __AEGON_THEME_SWITCHER
							pItemElem->GetTexture()->m_size.cx -= 4;
							pItemElem->GetTexture()->m_size.cy -= 4;
						}
						else
#ifdef __AEGON_THEME_SWITCHER
							RanderIcon(p2DRender, pItemElem, ptIconRender.x, ptIconRender.y, nParent);
#else // __AEGON_THEME_SWITCHER
							RanderIcon(p2DRender, pItemElem, x * 32, y * 32, nParent);
#endif // __AEGON_THEME_SWITCHER
					if (pItemElem->GetProp()->dwPackMax > 1)
					{
						short m_nItemNum = pItemElem->m_nItemNum;
						if (pItemElem->GetExtra() > 0)
							m_nItemNum -= pItemElem->GetExtra();
						if (m_bVisibleCount)
						{
							TCHAR szTemp[32];
							_stprintf(szTemp, "%d", m_nItemNum);

#ifdef __LEESE_RENDER_ITEMCOUNT
							CD3DFont* pOldFont = p2DRender->GetFont();
							p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemCount);
#endif //__LEESE_RENDER_ITEMCOUNT
							CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
#ifdef __LEESE_RENDER_ITEMCOUNT
#ifdef __AEGON_THEME_SWITCHER
							m_p2DRender->TextOut(ptIconRender.x + 32 - size.cx, ptIconRender.y + 33 - size.cy, szTemp, 0xFF000000);
							m_p2DRender->TextOut(ptIconRender.x + 31 - size.cx, ptIconRender.y + 32 - size.cy, szTemp, 0xFFffb912);
#else // __AEGON_THEME_SWITCHER
							m_p2DRender->TextOut(x * 32 + 32 - size.cx, y * 32 + 32 - size.cy, szTemp, 0xFF000000);
							m_p2DRender->TextOut(x * 32 + 31 - size.cx, y * 32 + 31 - size.cy, szTemp, 0xFFffb912);
#endif // __AEGON_THEME_SWITCHER
#else //__LEESE_RENDER_ITEMCOUNT
#ifdef __AEGON_THEME_SWITCHER
							m_p2DRender->TextOut(ptIconRender.x + 32 - size.cx, ptIconRender.y + 32 - size.cy, szTemp, 0xff0000ff);
							m_p2DRender->TextOut(ptIconRender.x + 31 - size.cx, ptIconRender.y + 31 - size.cy, szTemp, 0xffb0b0f0);
#else // __AEGON_THEME_SWITCHER
							m_p2DRender->TextOut(x * 32 + 32 - size.cx, y * 32 + 32 - size.cy, szTemp, 0xff0000ff);
							m_p2DRender->TextOut(x * 32 + 31 - size.cx, y * 32 + 31 - size.cy, szTemp, 0xffb0b0f0);
#endif // __AEGON_THEME_SWITCHER
#endif //__LEESE_RENDER_ITEMCOUNT
#ifdef __LEESE_RENDER_ITEMCOUNT
							p2DRender->SetFont(pOldFont);
#endif //__LEESE_RENDER_ITEMCOUNT
						}
					}
				}
			}
		}
	}
#ifdef __FIX_TOOLTIP_INVENTORY
	DrawToolTip(m_ptMouse);
#endif //__FIX_TOOLTIP_INVENTORY
}

#ifdef __ITEMLINK
DWORD CWndItemCtrl::GetColor( CItemElem* pItemElem )
{
	ItemProp* pItemProp = pItemElem->GetProp();
	if( pItemProp == NULL )
		return 0xffffffff;

#ifdef __WEAPON_RARITY
	WeaponRarity* nWeaponRarity = prj.m_nWeaponRarity.GetAt(pItemElem->GetWeaponRarity());
	if (nWeaponRarity && pItemElem->GetWeaponRarity() > 0)
	{
		return nWeaponRarity->m_dwColor;
	}
#endif // __WEAPON_RARITY

	DWORD dwColorbuf = 0xffffffff;
	switch( pItemProp->dwItemKind1 )
	{
	case IK1_ARMOR:
	case IK1_WEAPON:
		if( pItemProp->dwReferStat1 == WEAPON_UNIQUE || pItemProp->dwReferStat1 == WEAPON_ULTIMATE || pItemProp->dwReferStat1 == ARMOR_SET 
#ifdef __NEW_ITEM_VARUNA
			|| pItemProp->dwReferStat1 == BARUNA_D 
#endif //__NEW_ITEM_VARUNA
			)
		{
			if( pItemElem->IsFlag( CItemElem::binds ))
				dwColorbuf = COLOR_ERROR;
			else
			{
				if( pItemProp->IsUltimate() )
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwName3;
				else
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwName1;
			}
		}
		else
			dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwName0;
		break;
	case IK1_SYSTEM:
	{
#ifdef __ANARCHY_SYSTEM
		if (pItemProp->dwItemKind3 == IK3_ANARCHY)
		{
			dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwName7;
		}
#endif //__ANARCHY_SYSTEM
#ifdef __SYS_MAP_BUFFS
		else if (pItemProp->dwItemKind3 == IK3_MAPBUFF)
		{
			dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwName7;
		}
#endif //__SYS_MAP_BUFFS
#ifdef __SYS_GLYPHS
		else if (pItemProp->dwID == II_SYS_GLYPH_SHARD)
		{
			dwColorbuf = 0xff452aa3; // Purple
		}
#endif //__SYS_GLYPHS
		else
		{
			dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwName0;
		}
		break;
	}
	case IK1_CHARGED:
	{
#ifdef __PREMIUM
		if (pItemProp->dwItemKind3 == IK3_PREMIUM)
		{
			dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwName6;
		}
#endif //__PREMIUM
		break;
	}
#ifdef __SYS_GLYPHS
	case IK1_GENERAL:
	{
		if (pItemProp->dwItemKind3 == IK3_ARROW)
		{
			dwColorbuf = 0xff452aa3; // Purple
		}
		break;
	}
#endif //__SYS_GLYPHS
	default:
		dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwName0;
		break;
	}

	return dwColorbuf;
}
#endif //__ITEMLINK

void CWndItemCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	if( !g_pPlayer )
		return;

	if( !m_pItemContainer )
		return;

	CPoint pt( 3, 3 );
	CRect rect;

#if __VER < 16 //__ITEMLINK
	if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 )
		return;
#endif //__ITEMLINK

	if(	CWndBase::m_GlobalShortcut.IsEmpty() == FALSE )
		return;
	//SetCapture();
	int nItem = HitTest( point );

	if( nItem == -1 )	return;

	CWndBase* pWndBase = this->GetParentWnd();
	
	int nParent;
	if( pWndBase )
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if( pWndBaseTemp )
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
	{
		return;
	}

	CItemElem* pItemElem = NULL;

	CWndInventory* pWndInventory	= (CWndInventory*)GetWndBase( APP_INVENTORY );

#ifdef __INVENTORY_SEARCH
	if (g_WndMng.GetWndBase(APP_INVENTORY))
	{
		CWndEdit* pSearchBox = (CWndEdit*)pWndInventory->GetDlgItem(WIDC_EDIT);
		pWndInventory->KillFocus(pSearchBox, NULL);
		pSearchBox->Empty();
	}
#endif //__INVENTORY_SEARCH

#ifdef __GUILD_BANK_SEARCH
	if (g_WndMng.GetWndBase(APP_GUILD_BANK))
	{
		CWndGuildBank* pWndGuildBank = (CWndGuildBank*)GetWndBase(APP_GUILD_BANK);
		CWndEdit* pSearchBox2 = (CWndEdit*)pWndGuildBank->GetDlgItem(WIDC_EDIT2);
		pWndGuildBank->KillFocus(pSearchBox2, NULL);
		pSearchBox2->Empty();
	}
#endif //__GUILD_BANK_SEARCH

#ifdef __SHOP_SEARCH
	if (g_WndMng.GetWndBase(APP_SHOPEX))
	{
		CWndShop* pWndShopex = (CWndShop*)GetWndBase(APP_SHOPEX);
		CWndEdit* pSearchBox5 = (CWndEdit*)pWndShopex->GetDlgItem(WIDC_EDIT);
		pWndShopex->KillFocus(pSearchBox5, NULL);
		pSearchBox5->Empty();
	}
#endif //__SHOP_SEARCH

#if __VER >= 11 // __SYS_POCKET
	if( nParent == APP_INVENTORY || nParent == APP_BAG_EX)
	{
		if(!(nParent == APP_BAG_EX)) 
#else
	if( nParent == APP_INVENTORY )
	{
#endif

#if __VER >= 14 // __SMELT_SAFETY
		if( pWndInventory->m_dwEnchantWaitTime != 0xffffffff || GetWndBase(APP_SMELT_SAFETY_CONFIRM) != NULL )
#else //__SMELT_SAFETY
		if( pWndInventory->m_dwEnchantWaitTime != 0xffffffff )
#endif //__SMELT_SAFETY
		{
			g_WndMng.PutString( prj.GetText(TID_MMI_NOTUPGRADE), NULL, prj.GetTextColor(TID_MMI_NOTUPGRADE) );			
			m_pFocusItem = NULL;
			return;
		}

#if __VER >= 14 // __EQUIP_BIND
		if( GetWndBase(APP_EQUIP_BIND_CONFIRM) != NULL )
		{
			g_WndMng.PutString( prj.GetText(TID_TOOLTIP_EQUIPBIND_ERROR01), NULL, prj.GetTextColor(TID_TOOLTIP_EQUIPBIND_ERROR01) );
			m_pFocusItem = NULL;
			return;
		}
#endif // __EQUIP_BIND

#if __VER >= 14 // __DROP_CONFIRM_BUG
		if( GetWndBase(APP_COMMITEM_DIALOG) != NULL )
		{
			g_WndMng.PutString( prj.GetText(TID_TOOLTIP_ITEM_USING_ERROR), NULL, prj.GetTextColor(TID_TOOLTIP_ITEM_USING_ERROR) );
			m_pFocusItem = NULL;
			return;
		}
#endif // __DROP_CONFIRM_BUG

		pItemElem = GetItem( nItem );
	}
	else
	{
		pItemElem = m_pArrayItemElem[nItem];
	}
	
	if( pItemElem )
	{
#ifdef __ITEMLINK
		if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 )
		{
			if( nParent == APP_INVENTORY )
			{
				CWndChat* pWndChat = (CWndChat*) g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );
				if( pWndChat )
				{
					CString strItem;
#ifdef __WEAPON_RARITY
					WeaponRarity* nWeaponRarity = prj.m_nWeaponRarity.GetAt(pItemElem->GetWeaponRarity());
					if (nWeaponRarity && pItemElem->GetWeaponRarity() > 0)
					{
						strItem += "[";
						strItem += nWeaponRarity->m_szName;
						strItem += "] ";
					}
#endif // __WEAPON_RARITY
					strItem.Format( "%s", pItemElem->GetProp()->szName );
					pWndChat->SetFocus();
					CWndEditChat* pWndEdit = &pWndChat->m_wndEdit;
					pWndEdit->AddString( strItem, GetColor( pItemElem ), ESSTY_UNDERLINE & ESSTY_BOLD, g_pPlayer->m_idPlayer, pItemElem->m_dwObjId );
					pWndEdit->AddString( " " );
					pWndEdit->SetFocus();
				}
			}
			return;
		}
#endif
#if __VER >= 8 //__CSC_VER8_5
		if( (pItemElem->m_nItemNum != pItemElem->GetExtra() && m_useDieFlag) || 
			( IsUsingItem( pItemElem ) == FALSE && pItemElem->m_bRepair == FALSE) )
		{
			if( m_useDieFlag || IsSelectedItem( nItem ) == FALSE )
#else
		if( IsUsingItem( pItemElem ) == FALSE && pItemElem->m_bRepair == FALSE )
		{
			if( IsSelectedItem( nItem ) == FALSE )
#endif //__CSC_VER8_5
			{
				m_dwSelectAry.RemoveAll();
				m_dwSelectAry.Add( nItem );
				m_nCurSelect = nItem;
				m_pFocusItem = pItemElem;
			}
			m_bDrag = TRUE;
		}

		// ÀÎÃ¾Æ®ÇÒ ¾ÆÀÌÅÛÀ» ¼±ÅÃÇÏ¿´´Ù¸é ½ÇÇàÇÑ´Ù.
		// Å¬¶ó¿¡¼­ 4ÃÊÁ¤µµ ÀÌÆåÆ® Ãâ·ÂÈÄ ÆÐÅ¶À» ³¯¸°´Ù..
		if( nParent == APP_INVENTORY && pWndInventory )
		{
			CPoint pt( 3, 3 );

			CRect rect = GetClientRect();
			int nWidth = rect.Width() / 32;
			int nHeight = rect.Height() / 32;

			// ÀÎÃ¾, ÇÇ¾î½Ì°ü·Ã ÀÌÆåÆ® Ãâ·ÂÀ§Ä¡   
			int nPage = nHeight;
			int nRange = m_pItemContainer->m_dwIndexNum / nWidth;
			if( m_pItemContainer->m_dwIndexNum % nWidth )
				nRange++;
			m_wndScrollBar.SetScrollRange( 0, nRange );
			m_wndScrollBar.SetScrollPage( nPage );

			pt.y = 0;
			pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

			int x = ( nItem - pt.y ) % nWidth;
			int y = ( nItem - pt.y ) / nWidth;

#if __VER >= 14 // __SMELT_SAFETY
			if(pWndInventory->m_bIsUpgradeMode && pWndInventory->m_pUpgradeMaterialItem != NULL && m_pFocusItem != NULL)
			{
				if(!g_pPlayer->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT) && 
					( pWndInventory->m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_ORICHALCUM01 || pWndInventory->m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_ORICHALCUM01_1 ) && 
					m_pFocusItem->GetAbilityOption() >= 3 && CItemElem::IsDiceRefineryAble(m_pFocusItem->GetProp()))
				{
					if(g_WndMng.m_pWndSmeltSafetyConfirm)
						SAFE_DELETE(g_WndMng.m_pWndSmeltSafetyConfirm)

					g_WndMng.m_pWndSmeltSafetyConfirm = new CWndSmeltSafetyConfirm(CWndSmeltSafetyConfirm::WND_ERROR1);

					if(g_WndMng.m_pWndSmeltSafetyConfirm)
					{
						CWndBase::m_GlobalShortcut.Empty();
						m_bDrag = FALSE;
						g_WndMng.m_pWndSmeltSafetyConfirm->SetWndMode(m_pFocusItem);
						g_WndMng.m_pWndSmeltSafetyConfirm->Initialize(NULL);
					}
				}
				else if(!g_pPlayer->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT3) && pWndInventory->m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_ORICHALCUM02 &&
						m_pFocusItem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE)
				{
					if(g_WndMng.m_pWndSmeltSafetyConfirm)
						SAFE_DELETE(g_WndMng.m_pWndSmeltSafetyConfirm)

					g_WndMng.m_pWndSmeltSafetyConfirm = new CWndSmeltSafetyConfirm(CWndSmeltSafetyConfirm::WND_ERROR2);

					if(g_WndMng.m_pWndSmeltSafetyConfirm)
					{
						CWndBase::m_GlobalShortcut.Empty();
						m_bDrag = FALSE;
						g_WndMng.m_pWndSmeltSafetyConfirm->SetWndMode(m_pFocusItem);
						g_WndMng.m_pWndSmeltSafetyConfirm->Initialize(NULL);
					}
				}
				else if(!g_pPlayer->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT4) && m_pFocusItem->GetAbilityOption() >= 3 && 
						(pWndInventory->m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_MOONSTONE || pWndInventory->m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_MOONSTONE_1) &&
						m_pFocusItem->GetProp()->dwItemKind2 == IK2_JEWELRY)
				{
					if(g_WndMng.m_pWndSmeltSafetyConfirm)
						SAFE_DELETE(g_WndMng.m_pWndSmeltSafetyConfirm)

					g_WndMng.m_pWndSmeltSafetyConfirm = new CWndSmeltSafetyConfirm(CWndSmeltSafetyConfirm::WND_ERROR3);

					if(g_WndMng.m_pWndSmeltSafetyConfirm)
					{
						CWndBase::m_GlobalShortcut.Empty();
						m_bDrag = FALSE;
						g_WndMng.m_pWndSmeltSafetyConfirm->SetWndMode(m_pFocusItem);
						g_WndMng.m_pWndSmeltSafetyConfirm->Initialize(NULL);
					}
				}
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
				else if( g_pPlayer->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT ) == FALSE && 
						 m_pFocusItem->GetResistAbilityOption() >= 3 && 
						 CItemElem::IsElementalCard( pWndInventory->m_pUpgradeMaterialItem->m_dwItemId ) == TRUE && 
						 CItemElem::IsEleRefineryAble( m_pFocusItem->GetProp() ) == TRUE )
				{
					if( g_WndMng.m_pWndSmeltSafetyConfirm )
						SAFE_DELETE( g_WndMng.m_pWndSmeltSafetyConfirm )

					g_WndMng.m_pWndSmeltSafetyConfirm = new CWndSmeltSafetyConfirm( CWndSmeltSafetyConfirm::WND_ERROR1 );
					if( g_WndMng.m_pWndSmeltSafetyConfirm )
					{
						CWndBase::m_GlobalShortcut.Empty();
						m_bDrag = FALSE;
						g_WndMng.m_pWndSmeltSafetyConfirm->SetWndMode( m_pFocusItem );
						g_WndMng.m_pWndSmeltSafetyConfirm->Initialize( NULL );
					}
				}
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
				else
					pWndInventory->RunUpgrade(m_pFocusItem);
			}
#else //__SMELT_SAFETY									
			pWndInventory->RunUpgrade(m_pFocusItem);	
#endif //__SMELT_SAFETY
		}
	}
	else
	{
		m_dwSelectAry.RemoveAll();
		m_nCurSelect	= nItem;
		m_pFocusItem	= NULL;
//		m_bDrag		= FALSE;
	}
	//SetCapture();
}

void CWndItemCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
	if( !m_pItemContainer )
		return;

	//ReleaseCapture();
	m_bDrag = FALSE;
	//if(	CWndBase::m_GlobalShortcut.IsEmpty() == FALSE )
	//	return;
	int nItem = HitTest( point );
	if( nItem == -1 ) return;

	CWndBase* pWndBase = this->GetParentWnd();
	
	int nParent;
	if( pWndBase )
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if( pWndBaseTemp )
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
	{
		return;
	}

#ifdef __INVENTORY_SEARCH
	if (nParent == APP_INVENTORY)
	{
		CWndInventory* pWndInventory = dynamic_cast<CWndInventory*>(GetWndBase(APP_INVENTORY));
		if (pWndInventory)
		{
			CWndEdit* pWndEdit = pWndInventory->GetWndSearch();
			if (pWndEdit)
			{
				pWndEdit->OnKillFocus(nullptr);
			}
		}
	}
#endif //__INVENTORY_SEARCH

	CItemElem* pItemElem = NULL;

#if __VER >= 11 // __SYS_POCKET
	if( nParent == APP_INVENTORY || nParent == APP_BAG_EX)
#else
	if( nParent == APP_INVENTORY )
#endif
		pItemElem = GetItem( nItem );
	else
		pItemElem = m_pArrayItemElem[nItem];
	
	if( pItemElem )
	{
		if( (GetAsyncKeyState( VK_LCONTROL ) & 0x8000 ) )
		{
			int j = NULL;
			for( ; j < m_dwSelectAry.GetSize(); j++ )
			{
				if( m_dwSelectAry.GetAt( j ) == nItem )
					break;
			}
			if( j == m_dwSelectAry.GetSize() )
				m_dwSelectAry.Add( nItem );
			else
			if( m_nCurSelect != nItem )
				m_dwSelectAry.RemoveAt( j );
			m_nCurSelect = nItem;
			m_pFocusItem = pItemElem;
		//	m_rect = rect;
		}
		else
		if( IsSelectedItem( nItem ) )
		{
			m_dwSelectAry.RemoveAll();
			m_dwSelectAry.Add( nItem );
			m_nCurSelect = nItem;
			m_pFocusItem = pItemElem;
		}
	}
}
void CWndItemCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if( !m_pItemContainer )
		return;

	if( m_bDrag == FALSE )
		return;
	//TRACE( " hello \n" );
	m_bDrag = FALSE;
	CPoint pt( 3, 3 );
	CRect rect;
	//if( m_rect.PtInRect( point ) )
	//{
	CWndBase* pWndBase = this->GetParentWnd();
	
	int nParent;
	if( pWndBase )
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if( pWndBaseTemp )
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
	{
		return;
	}

	CItemElem* pItemElem = NULL;

#if __VER >= 11 // __SYS_POCKET
	if( nParent == APP_INVENTORY || nParent == APP_BAG_EX
#ifdef __AEGON_VIEW_INV
		|| nParent == APP_VIEW_INVENTORY_CTRL
		|| nParent == APP_VIEW_BAG
#endif // __AEGON_VIEW_INV
		)
#else
	if( nParent == APP_INVENTORY )
#endif
		pItemElem = GetItem( m_nCurSelect ); //
	else
		pItemElem = m_pArrayItemElem[m_nCurSelect];
	
#if __VER >= 8 //__CSC_VER8_5
#if __VER >= 9 // __CSC_VER9_1
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
#if __VER >= 11 // __SYS_POCKET
	if(!pWndInventory && nParent == APP_BAG_EX && pItemElem != NULL)
	{
		if(IsUsableItem( pItemElem ))
		{
			m_GlobalShortcut.m_pFromWnd   = this;
			m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
			m_GlobalShortcut.m_dwIndex    = m_nCurSelect;// pItemElem->m_dwItemId; //m_nCurSelect;// m_nCurSelect;//(DWORD)pItemElem;//->m_dwItemId;
			m_GlobalShortcut.m_dwType     = 0;// m_nCurSelect;//(DWORD)pItemElem;//->m_dwItemId;
			m_GlobalShortcut.m_dwId       = pItemElem->m_dwObjId;//(DWORD)pItemElem;
			m_GlobalShortcut.m_pTexture   = pItemElem->GetTexture();
#ifdef _WIN64
			m_GlobalShortcut.m_dwData = (ULONG_PTR)pItemElem;
#else
			m_GlobalShortcut.m_dwData     = (DWORD) pItemElem;
#endif
			
			if(pItemElem->GetProp() == NULL)
			{
				CString strErr;
				strErr.Format( "CWndItemCtrl::OnMouseMove¿¡¼­ GetProp°¡ NULL\nItemType : %d, ItemId : %d ", 0, pItemElem->m_dwItemId );
				ADDERRORMSG(strErr);
			}
			_tcscpy( m_GlobalShortcut.m_szString, pItemElem->GetName() );
		}
		return;
	}
#endif
	if( ((pItemElem != NULL && m_useDieFlag) || IsUsableItem( pItemElem )) && !pWndInventory->m_bRemoveJewel)
#else
	if( (pItemElem != NULL && m_useDieFlag) || IsUsableItem( pItemElem ) )
#endif //__CSC_VER9_1
#else
	if( IsUsableItem( pItemElem ) )
#endif //__CSC_VER8_5
	{
		m_GlobalShortcut.m_pFromWnd   = this;
		m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
		m_GlobalShortcut.m_dwIndex    = m_nCurSelect;// pItemElem->m_dwItemId; //m_nCurSelect;// m_nCurSelect;//(DWORD)pItemElem;//->m_dwItemId;
		m_GlobalShortcut.m_dwType     = 0;// m_nCurSelect;//(DWORD)pItemElem;//->m_dwItemId;
		m_GlobalShortcut.m_dwId       = pItemElem->m_dwObjId;//(DWORD)pItemElem;
		m_GlobalShortcut.m_pTexture   = pItemElem->GetTexture();
#ifdef _WIN64
		m_GlobalShortcut.m_dwData = (ULONG_PTR)pItemElem;
#else
		m_GlobalShortcut.m_dwData     = (DWORD) pItemElem;
#endif
		
		if(pItemElem->GetProp() == NULL)
		{
			CString strErr;
			strErr.Format( "CWndItemCtrl::OnMouseMove¿¡¼­ GetProp°¡ NULL\nItemType : %d, ItemId : %d ", 0, pItemElem->m_dwItemId );
			ADDERRORMSG(strErr);
		}
		_tcscpy( m_GlobalShortcut.m_szString, pItemElem->GetName() );
	}
}
CItemElem* CWndItemCtrl::GetItem( DWORD dwIndex )
{
	return m_pItemContainer->GetAt( dwIndex );
}

CItemElem* CWndItemCtrl::GetItemFromArr( DWORD dwIndex )
{
	ASSERT(dwIndex < 100);
	return m_pArrayItemElem[dwIndex];
}

int CWndItemCtrl::HitTest( CPoint point )
{
	if( !m_pItemContainer )
		return -1;

	int nDstId = -1;
	CPoint pt( 3, 3 );
	CRect rect;
	//
	// ¸®Æ÷Æ®  
	//
	if( 0 ) //m_dwListCtrlStyle == WLVS_REPORT )
	{
		pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
		for( int i = 0; i < (int)( m_pItemContainer->m_dwIndexNum ); i++ ) 
		{
			CItemElem* pItemElem = &m_pItemContainer->m_apItem[ m_pItemContainer->m_apIndex[ i ] ] ;
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
			if( rect.PtInRect( point ) )
			{
				nDstId = i;
				break;
			}
			pt.y += m_nFontHeight + 3;
		}
	}
	//
	// ¾ÆÀÌÄÜ 
	//
	if( 1 ) //m_dwListCtrlStyle == WLVS_ICON )
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 32;
		int nHeight = rect.Height() / 32;
		CPoint pt( 3, 3 );

		pt.y = 0;
		pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

		CWndBase* pWndBase = this->GetParentWnd();
		
		int nParent;
		if( pWndBase )
		{
			CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
			if( pWndBaseTemp )
				nParent = pWndBaseTemp->GetWndId();
			else
				return nDstId;
		}
		else
		{
			return nDstId;
		}

	#if __VER >= 11 // __SYS_POCKET
		if( nParent == APP_INVENTORY || nParent == APP_BAG_EX
#ifdef __AEGON_VIEW_INV
			|| nParent == APP_VIEW_INVENTORY_CTRL
			|| nParent == APP_VIEW_BAG
#endif // __AEGON_VIEW_INV
			)
	#else
		if( nParent == APP_INVENTORY )
	#endif
		{
			for( int i = pt.y; i < (int)( m_pItemContainer->m_dwIndexNum ); i++ ) 
			{
				int x = ( i - pt.y ) % nWidth;
				int y = ( i - pt.y ) / nWidth;

#ifdef __AEGON_THEME_SWITCHER
				CPoint ptIconRender = CPoint(x * 32, y * 32);
				ptIconRender.y += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetY(y);
				ptIconRender.x += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetX(x);

				rect.SetRect(ptIconRender.x, ptIconRender.y, ptIconRender.x + 32, ptIconRender.y + 32);
#else // __AEGON_THEME_SWITCHER
				rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
#endif // __AEGON_THEME_SWITCHER
				if( rect.PtInRect( point ) )
				{
					nDstId = i;
					break;
				}
			}
		}
		else
		{
			for( int i = pt.y; i < m_nArrayCount; i++ ) 
			{
				int x = ( i - pt.y ) % nWidth;
				int y = ( i - pt.y ) / nWidth;
#ifdef __AEGON_THEME_SWITCHER
				CPoint ptIconRender = CPoint(x * 32, y * 32);
				ptIconRender.y += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetY(y);
				ptIconRender.x += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetX(x);

				rect.SetRect(ptIconRender.x, ptIconRender.y, ptIconRender.x + 32, ptIconRender.y + 32);
#else // __AEGON_THEME_SWITCHER
				rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
#endif // __AEGON_THEME_SWITCHER
				if( rect.PtInRect( point ) )
				{
					nDstId = i;
					break;
				}
			}
		}
	}
	return nDstId;
}
BOOL CWndItemCtrl::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	if( !m_pItemContainer )
		return FALSE;

	if( pShortcut->m_dwShortcut == SHORTCUT_ITEM )
	{
		int nDstIndex = HitTest( point );//
		if( nDstIndex == -1 ) nDstIndex = m_pItemContainer->m_dwIndexNum;
//			return FALSE;

		if( pShortcut->m_pFromWnd == this )
		{

			CWndBase* pWndFrame =  pShortcut->m_pFromWnd->GetFrameWnd();
			if( pWndFrame->GetWndId() == APP_BANK ) // 42 // Bank
			{
				g_DPlay.SendMoveBankItem( (BYTE)( pShortcut->m_dwIndex ), (BYTE)( (DWORD)nDstIndex ) );
			}
			else if( pWndFrame->GetWndId() == APP_INVENTORY )	// inventory -> inventory
			{
				CItemElem* pItemElem	= m_pItemContainer->GetAt( pShortcut->m_dwIndex );
				if( IsUsingItem( pItemElem ) == FALSE )
					g_DPlay.SendMoveItem( 0, (BYTE)( pShortcut->m_dwIndex ), (BYTE)( (DWORD)nDstIndex ) );
			}
#ifdef __AEGON_BAG_FREE_MOVE
			else if (pWndFrame->GetWndId() == APP_BAG_EX)
			{
				CItemElem* pItemElem = m_pItemContainer->GetAt(pShortcut->m_dwIndex);
				if (IsUsingItem(pItemElem) == FALSE) 
				{
					int nPocket = GetWndId() - 11;
					if (IsValidObj(g_pPlayer) && g_pPlayer->m_Pocket.IsAvailable(nPocket, TRUE))
					{
						g_DPlay.SendMovePocketItem(nPocket, (int)pShortcut->m_dwIndex, nDstIndex);
					}
				}
			}
#endif
		}
		else
		{
#ifdef __AEGON_VIEW_INV
			CWndBase* pWndBase = GetParentWnd()->GetParentWnd();
			if (pWndBase->GetWndId() == APP_VIEW_INVENTORY_CTRL)
			{
				pWndBase->OnDropIcon(pShortcut, CPoint(0, 0));
				return TRUE;
			}
			else if (pWndBase->GetWndId() == APP_VIEW_BAG)
			{
				int nContainerField = GetWndId() - 11;
				return ((CWndViewBag*)pWndBase)->OnDropIconOnItemCtrl(pShortcut, nContainerField);
			}
#endif // __AEGON_VIEW_INV
			CWndBase* pParent = (CWndBase*)GetParentWnd();
			pParent->OnChildNotify( WIN_ITEMDROP, m_nIdWnd, (LRESULT*)pShortcut ); 
		}
		return TRUE;
	}
	return FALSE;
}
/*
void CWndItemCtrl::OnDropIcon( DWORD dwShorCutType, DWORD dwShortCutIndex, CPoint point )
{
	if( dwShorCutType == SHORTCUT_ITEM )
	{
		if( m_dwListCtrlStyle == WLVS_ICON )
		{
			CRect rect = GetClientRect();
			int nWidth = rect.Width() / 32;
			int nHeight = rect.Height() / 32;
	CPoint pt( 3, 3 );

			pt.y = 0;
			pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
			for( int i = pt.y; i < m_pItemContainer->m_dwIndexNum; i++ ) 
			{
				int x = ( i - pt.y ) % nWidth;
				int y = ( i - pt.y ) / nWidth;
				CItemElem* pItemElem = &m_pItemContainer->m_apItem[ m_pItemContainer->m_apIndex[ i ] ] ;
				rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
				if( rect.PtInRect( point ) )
				{
					for( int j = m_pItemContainer->m_dwIndexNum; j > i; j-- )
						m_pItemContainer->m_apIndex[ j ] = m_pItemContainer->m_apIndex[ j - 1 ];
					m_pItemContainer->m_apIndex[ j ] = dwItemOld;
					(m_pItemContainer->m_dwIndexNum)++;
					return;
				}
			}
			m_pItemContainer->m_apIndex[ m_pItemContainer->m_dwIndexNum ] = dwItemOld;
			(m_pItemContainer->m_dwIndexNum)++;
		}



		_pItemContainer->Swap( CWndBase::m_GlobalShortcut.m_dwData, 
		
		CItemElem* pItemOld = &m_apItem[ CWndBase::m_GlobalShortcut.m_dwData ];
		for( int i = 0; m_pItemContainer->m_dwIndexNum; i++ )
		{
			CItemElem* pItemElem = &m_apItem[ i ];
			if( pItemElem->m_dwItemId )
			{
				*pItemElem = *pItemOld;
				pItemOld->m_dwItemId = 0;
			}
		}
		
	}
}*/
BOOL CWndItemCtrl::IsSelectedItem( int iItem )
{
	for( int i = 0; i < m_dwSelectAry.GetSize(); i++ )
		if( m_dwSelectAry.GetAt( i ) == iItem )
			return TRUE;
	return FALSE;
}
int CWndItemCtrl::GetSelectedItem( int i )
{
	return m_dwSelectAry.GetAt( i );
}
UINT CWndItemCtrl::GetSelectedCount()
{
	return m_dwSelectAry.GetSize();
}

void CWndItemCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{	
	CWndBase* pWndBase = this->GetParentWnd();

	int nParent;
	if (pWndBase)
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if (pWndBaseTemp)
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
	{
		return;
	}

#ifdef __SYS_LOCK_ITEM
	if (nParent == APP_INVENTORY)
	{
#ifdef __QUICK_MOVE_ITEMS
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
#endif //__QUICK_MOVE_ITEMS
			if (!m_pItemContainer)
				return;

			CRect rectCtrl = GetClientRect();

			int nWidth = rectCtrl.Width() / 32;
			int nHeight = rectCtrl.Height() / 32;

			CPoint pt;
			pt.y = 0;
			pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

			for (int i = pt.y; i < m_pItemContainer->GetSize(); i++)
			{
				if (i < 0) continue;
				int x = (i - pt.y) % nWidth;
				int y = (i - pt.y) / nWidth;

				CItemElem* pItemElem = GetItem(i);
				if (pItemElem == NULL) continue;


#ifdef __AEGON_THEME_SWITCHER
				CPoint ptIconRender = CPoint(x * 32, y * 32);
				ptIconRender.y += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetY(y);
				ptIconRender.x += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetX(x);

				CRect rectHittest = CRect(ptIconRender.x, ptIconRender.y, ptIconRender.x + 32 - 2, ptIconRender.y + 32 - 2);
#else // __AEGON_THEME_SWITCHER
				CRect rectHittest = CRect(x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2);
#endif // __AEGON_THEME_SWITCHER

				if (rectHittest.PtInRect(point))
				{
					CPoint point2 = point;
					ClientToScreen(&point2);
					ClientToScreen(&rectHittest);
					ItemProp* pItemProp = pItemElem->GetProp();

					if (pItemElem == NULL)
						continue;

					m_wndLockMenu.DeleteAllMenu();

					CString dwText = "Lock";
					if (pItemElem->IsItemLocked())
						dwText = "Unlock";

					m_wndLockMenu.AppendMenu(0, MGI_APPELL_UP, dwText);

					m_wndLockMenu.Move(CPoint(GetWndRect().left + rectHittest.left, GetWndRect().top + rectHittest.bottom));
					m_wndLockMenu.SetVisible(TRUE);
					m_wndLockMenu.SetFocus();

					m_pLockItem = pItemElem;
				}
			}
#ifdef __QUICK_MOVE_ITEMS
		}
#endif //__QUICK_MOVE_ITEMS
	}
#endif // __SYS_LOCK_ITEM

	int nItem = HitTest(point);
	if (nItem == -1) return;

	CItemElem* pItemElem = NULL;
#ifdef __AEGON_VIEW_INV
	if (nParent == APP_VIEW_INVENTORY_CTRL || nParent == APP_VIEW_BAG)
	{
		pItemElem = GetItem(nItem);
		if(pItemElem)
		{
			if (!m_pItemContainer)
				return;

			CRect rectCtrl = GetClientRect();

			int nWidth = rectCtrl.Width() / 32;
			int nHeight = rectCtrl.Height() / 32;

			CPoint pt;
			pt.y = 0;
			pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

			for (int i = pt.y; i < m_pItemContainer->GetSize(); i++)
			{
				if (i < 0) continue;
				int x = (i - pt.y) % nWidth;
				int y = (i - pt.y) / nWidth;

				CItemElem* pItemElem = GetItem(i);
				if (pItemElem == NULL) continue;

#ifdef __AEGON_THEME_SWITCHER
				CPoint ptIconRender = CPoint(x * 32, y * 32);
				ptIconRender.y += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetY(y);
				ptIconRender.x += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetX(x);

				CRect rectHittest = CRect(ptIconRender.x, ptIconRender.y, ptIconRender.x + 32 - 2, ptIconRender.y + 32 - 2);
#else // __AEGON_THEME_SWITCHER
				CRect rectHittest = CRect(x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2);
#endif // __AEGON_THEME_SWITCHER
				if (rectHittest.PtInRect(point))
				{
					CPoint point2 = point;
					ClientToScreen(&point2);
					ClientToScreen(&rectHittest);
					ItemProp* pItemProp = pItemElem->GetProp();

					if (pItemElem == NULL)
						continue;

					m_wndViewInvMenu.DeleteAllMenu();

					CString dwText = "Copy";
					m_wndViewInvMenu.AppendMenu(0, MTI_VIEWINV_COPY, dwText);

					dwText = "Delete";
					m_wndViewInvMenu.AppendMenu(1, MTI_VIEWINV_DELETE, dwText);

					dwText = "Move";
					m_wndViewInvMenu.AppendMenu(2, MTI_VIEWINV_MOVE, dwText);

					m_wndViewInvMenu.Move(CPoint(GetWndRect().left + rectHittest.left, GetWndRect().top + rectHittest.bottom));
					m_wndViewInvMenu.SetVisible(TRUE);
					m_wndViewInvMenu.SetFocus();

					m_pViewInvItem = pItemElem;

				}
			}
		}
	}
		

#endif // __AEGON_VIEW_INV

#ifdef __QUICK_MOVE_ITEMS
	int index = HitTest(point);
	if (index != -1)
	{
		CItemElem* pItem = GetItem(index);
		if (IsUsableItem(pItem) || (m_useDieFlag && pItem != nullptr))
		{
			if (IsSelectedItem(index) == FALSE)
			{
				m_dwSelectAry.RemoveAll();
				m_dwSelectAry.Add(index);
				m_nCurSelect = index;
				m_pFocusItem = pItem;
			}

			CWndBase* pParent = (CWndBase*)GetParentWnd();
			if (pParent)
				pParent->OnChildNotify(WNM_ITEMCLICK, m_nIdWnd, (LRESULT*)m_pFocusItem);
		}
	}
#endif //__QUICK_MOVE_ITEMS
}

#if defined(__SYS_LOCK_ITEM) || defined(__AEGON_VIEW_INV)
BOOL CWndItemCtrl::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	if (pWndBase == &m_wndLockMenu)
	{
		switch (nID)
		{
		case MGI_APPELL_UP:
		{
			if (m_pLockItem)
			{
				g_DPlay.SendLockItem(m_pLockItem->m_dwObjId, !m_pLockItem->IsItemLocked());
			}
			m_pLockItem = NULL;
		}
		break;
		}
		m_wndLockMenu.SetVisible(FALSE);
	}
#ifdef __AEGON_VIEW_INV
	else if (pWndBase == &m_wndViewInvMenu)
	{
		CString tmpOut;

		int nContainer = 0;
		int nContainerField = -1;
		CWndBase* pParent = m_wndViewInvMenu.GetParentWnd()->GetParentWnd()->GetParentWnd();
		if (pParent)
		{
			if (pParent->GetWndId() == APP_VIEW_INVENTORY_CTRL) 
			{
				nContainer = 0;
			}
			else if (pParent->GetWndId() == APP_VIEW_BAG)
			{
				nContainer = 1;
				nContainerField = GetWndId() - 11;
			}
			
		}

		switch (nID)
		{
		case MTI_VIEWINV_COPY:
		{
			if (m_pViewInvItem)
			{
				g_DPlay.SendViewInventoryCommand(m_pViewInvItem, 0, nContainer, nContainerField);
			}
		}
		break;
		case MTI_VIEWINV_DELETE:
		{
			if (m_pViewInvItem)
			{
				g_DPlay.SendViewInventoryCommand(m_pViewInvItem, 1, nContainer, nContainerField);
			}
		}
		break;
		case MTI_VIEWINV_MOVE:
		{
			if (m_pViewInvItem)
			{
				g_DPlay.SendViewInventoryCommand(m_pViewInvItem, 2, nContainer, nContainerField);
			}
		}
		break;
		}
		m_wndViewInvMenu.SetVisible(FALSE);
		g_WndMng.PutString(tmpOut, NULL, COLOR_ERROR);
	}
#endif // __AEGON_VIEW_INV
	return CWndBase::OnCommand(nID, dwMessage, pWndBase);
}
#endif // __SYS_LOCK_ITEM // __AEGON_VIEW_INV

void CWndItemCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{
	// ÀÎº¥Ã¢ÀÌ ¿­·ÁÀÖ°í ÀÎÃ¾Æ® ¸ðµåÀÌ¸é Ä¿¼­¸ð¾ç º¯°æ
	CWndInventory* pWndInventory	= (CWndInventory*)GetWndBase( APP_INVENTORY );

	if( pWndInventory && pWndInventory->m_dwEnchantWaitTime == 0xffffffff )
	{
		pWndInventory->BaseMouseCursor();
#if __VER >= 14 // __SMELT_SAFETY
		if(g_WndMng.m_pWndSmeltSafetyConfirm != NULL)
			SAFE_DELETE(g_WndMng.m_pWndSmeltSafetyConfirm)
#endif // __SMELT_SAFETY
	}
	
	if( !m_pItemContainer )
		return;

	CRect rectCtrl = GetClientRect();

	int nWidth = rectCtrl.Width() / 32;
	int nHeight = rectCtrl.Height() / 32;

	CPoint pt;
	pt.y = 0;
	pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

	for( int i=pt.y ; i < m_pItemContainer->GetSize() ; i++ )
	{
		if( i < 0 ) continue;
		int x = ( i - pt.y ) % nWidth;
		int y = ( i - pt.y ) / nWidth;

		CItemElem* pItemElem = GetItem( i );
		if( pItemElem == NULL ) continue;
		

#ifdef __AEGON_THEME_SWITCHER
		CPoint ptIconRender = CPoint(x * 32, y * 32);
		ptIconRender.y += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetY(y);
		ptIconRender.x += CAegonThemeSwitcherMng::GetInstance()->GetInventoryCtrlOffsetX(x);

		CRect rectHittest = CRect(ptIconRender.x, ptIconRender.y, ptIconRender.x + 32 - 2, ptIconRender.y + 32 - 2);
#else // __AEGON_THEME_SWITCHER
		CRect rectHittest = CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 );
#endif // __AEGON_THEME_SWITCHER
		if( rectHittest.PtInRect( point ) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &rectHittest );
			ItemProp* pItemProp = pItemElem->GetProp();

			if (pItemElem == NULL )
				continue;
			if( pItemProp->dwItemKind2 == IK2_TEXT ) 
			{
				CString strText = pItemProp->szTextFileName;
				//strText = strText.Left( 5 );
				strText.MakeLower();
				if( strText.Find( "book_" ) != -1 )
				{
					if ( !g_WndMng.m_pWndTextBook || g_WndMng.m_pWndTextBook->IsVisible() == FALSE)
						g_WndMng.OpenTextBook(this, pItemElem) ;
					else
						g_WndMng.ChangeTextBook(pItemElem) ;
				}
				else
				if( strText.Find( "scroll_" ) != -1  )
				{
					if ( !g_WndMng.m_pWndTextScroll || g_WndMng.m_pWndTextScroll->IsVisible() == FALSE)
						g_WndMng.OpenTextScroll(this, pItemElem) ;
					else
						g_WndMng.ChangeTextScroll(pItemElem) ;
				}
				else
				if( strText.Find( "letter_" ) != -1 )
				{
					if ( !g_WndMng.m_pWndTextLetter || g_WndMng.m_pWndTextLetter->IsVisible() == FALSE)
						g_WndMng.OpenTextLetter(this, pItemElem) ;
					else
						g_WndMng.ChangeTextLetter(pItemElem) ;
				}

			}
			else
			if( pItemProp->dwItemKind3 == IK3_QUEST )
			{
				if ( !g_WndMng.m_pQuestItemInfo || g_WndMng.m_pQuestItemInfo->IsVisible() == FALSE)
					g_WndMng.OpenQuestItemInfo(this, pItemElem) ;
				else
					g_WndMng.ChangeQuestItemInfo(pItemElem) ;
			}
		}
	}
}
BOOL CWndItemCtrl::OnSetCursor ( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	// ÀÎº¥Ã¢ÀÌ ¿­·ÁÀÖ°í ÀÎÃ¾Æ® ¸ðµåÀÌ¸é Ä¿¼­¸ð¾ç º¯°æ
	CWndInventory* pWndInventory	= (CWndInventory*)GetWndBase( APP_INVENTORY );	
	if( pWndInventory )
		pWndInventory->SetEnchantCursor();
	
	return TRUE;
}

void CWndItemCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if( !g_pPlayer )
		return;

	if( !m_pItemContainer )
		return;

	if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 )
		return;

#ifdef __MODEL_VIEW
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000 && !g_WndMng.GetWndBase(APP_SMELT_SAFETY))
	{
		int nItem = HitTest(point);
		if (nItem == -1) return;
		CItemElem* pItem = GetItem(nItem);
		PPACKITEMELEM pPackItemElem = CPackItem::GetInstance()->Open(pItem->m_dwItemId);
		if (pPackItemElem)
		{
			for (int i = 0; i < pPackItemElem->nSize; i++)
			{
				CWndModelView* pWndViewer = (CWndModelView*)g_WndMng.GetApplet(APP_MODEL_VIEW);
				CItemElem* itemElem = new CItemElem;
				itemElem->m_dwItemId = pPackItemElem->adwItem[i];
				pWndViewer = (CWndModelView*)g_WndMng.CreateApplet(APP_MODEL_VIEW);
				if (!pWndViewer)
				{
					if (pWndViewer)
					{
						if (!pWndViewer->SetPart(pItem))
							pWndViewer->Destroy();
					}
				}
				else
					pWndViewer->SetPart(pItem);
			}
		}
		else if (pItem && pItem->GetProp())
		{
			CWndModelView* pWndViewer = (CWndModelView*)g_WndMng.GetApplet(APP_MODEL_VIEW);
			if (!pWndViewer)
			{
				pWndViewer = (CWndModelView*)g_WndMng.CreateApplet(APP_MODEL_VIEW);
				if (pWndViewer)
				{
					if (!pWndViewer->SetPart(pItem))
						pWndViewer->Destroy();
				}
			}
			else
				pWndViewer->SetPart(pItem);

#ifdef __GIFTBOX_LIST_ITEMS
			g_DPlay.SendQueryGBPreview(pItem->m_dwItemId);
#endif //__GIFTBOX_LIST_ITEMS

		}
		return;
	}
#endif //__MODEL_VIEW

	if(	CWndBase::m_GlobalShortcut.IsEmpty() == FALSE )
		return;

	int nItem = HitTest( point );

	if( nItem == -1 )	return;

	CItemElem* pItemElem = NULL;
	pItemElem = GetItem( nItem );

#if __VER >= 8 //__CSC_VER8_5
	if( IsUsableItem( pItemElem ) || (m_useDieFlag && pItemElem != NULL))
#else
	if( IsUsableItem( pItemElem ) )
#endif //__CSC_VER8_5
	{
		if( IsSelectedItem( nItem ) == FALSE )
		{
			m_dwSelectAry.RemoveAll();
			m_dwSelectAry.Add( nItem );
			m_nCurSelect = nItem;
			m_pFocusItem = pItemElem;
		}

		CWndBase* pParent = (CWndBase*)GetParentWnd();
		pParent->OnChildNotify( WIN_DBLCLK,m_nIdWnd,(LRESULT*)m_pFocusItem); 
	}
}
void CWndItemCtrl::OnRButtonDblClk( UINT nFlags, CPoint point)
{
}
void CWndItemCtrl::OnSize( UINT nType, int cx, int cy )
{
//	m_string.Reset( g_2DRender.m_pFont, &GetClientRect() );

	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;
	m_wndScrollBar.SetWndRect( rect );

	CWndBase::OnSize( nType, cx, cy);
}
void CWndItemCtrl::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 4, 0 );

	if( m_pItemContainer ) 
	{
		if( 0 ) //m_dwListCtrlStyle == WLVS_REPORT )
		{
			int nPage = GetClientRect().Height() / (m_nFontHeight + 3);
			int nRange = m_pItemContainer->m_dwIndexNum;// - nPage;
			if( !( nRange - nPage <= 0 ) )
				m_rectClient.right -= 15; // ½ºÅ©·Ñ ¹Ù°¡ º¸ÀÌ¸é 
			m_rectClient.top += 15; // ¸®Æ÷Æ® Ä®·³ 
		}
		if( 1 ) //m_dwListCtrlStyle == WLVS_ICON )
		{
			CRect rect = GetClientRect();
			int nWidth = rect.Width() / 32;
			int nHeight = rect.Height() / 32;
			int nPage = nHeight;
			int nRange = m_pItemContainer->m_dwIndexNum / nWidth;// - nPage;
			//if( !( nRange - nPage <= 0 ) )
				m_rectClient.right -= 15; // ½ºÅ©·Ñ ¹Ù°¡ º¸ÀÌ¸é 
		}
	}
	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}
void CWndItemCtrl::PaintFrame( C2DRender* p2DRender )
{
	return;
	CRect rect = GetWindowRect();
	//m_pTheme->RenderWndTextFrame( p2DRender, &rect );
	/*
	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 180, 240, 240,  240 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor3 );
	*/
	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor3 );

	int nPage, nRange;
	if( 0 ) //m_dwListCtrlStyle == WLVS_REPORT )
	{
		nPage = GetClientRect().Height() / ( GetFontHeight( ) + 3 );
		nRange = m_pItemContainer->m_dwIndexNum;// - nPage;
	}
	if( 1 ) //m_dwListCtrlStyle == WLVS_ICON )
	{
		CRect rect = GetClientRect();
		nPage = rect.Height() / 32;
		nRange = m_pItemContainer->m_dwIndexNum / ( rect.Width() / 32 );// - nPage;
	}
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );

	if( 0 ) //m_dwListCtrlStyle == WLVS_REPORT )
	{
		// ÄÃ·³ Ãâ·Â 
		int i, x;
		for( i, x = 0; i < m_aColumns.GetSize(); i++ )
		{
			LVCOLUMN* pColumn = (LVCOLUMN*)m_aColumns.GetAt( i );
			p2DRender->TextOut( x + 4, 4, pColumn->pszText ); 
			m_pTheme->GradationRect( p2DRender, &CRect( x, 0, x + pColumn->cx + 5, 20 ), 
				D3DCOLOR_ARGB( 50, 100, 100, 100 ),
				D3DCOLOR_ARGB( 90, 255, 255, 255 ),
				D3DCOLOR_ARGB( 50, 000, 000, 000 ), 20 );
			x += pColumn->cx + 6;
		}
		m_pTheme->GradationRect( p2DRender, &CRect( x, 0, rect.Width(), 20 ), 
			D3DCOLOR_ARGB( 50, 100, 100, 100 ),
			D3DCOLOR_ARGB( 90, 255, 255, 255 ),
			D3DCOLOR_ARGB( 50, 000, 000, 000 ), 20 );
	}
}
/*
typedef struct tagLVCOLUMNA
{
    UINT mask;
    int fmt;
    int cx;
    LPSTR pszText;
    int cchTextMax;
    int iSubItem;
#if (_WIN32_IE >= 0x0300)
    int iImage;
    int iOrder;
#endif
} LVCOLUMNA, FAR* LPLVCOLUMNA;
typedef struct tagLVITEMA
{
    UINT mask;
    int iItem;
    int iSubItem;
    UINT state;
    UINT stateMask;
    LPSTR pszText;
    int cchTextMax;
    int iImage;
    LPARAM lParam;
#if (_WIN32_IE >= 0x0300)
    int iIndent;
#endif
} LVITEMA, FAR* LPLVITEMA;
*/
BOOL CWndItemCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}
BOOL CWndItemCtrl::SetItem( const LVITEM* pItem )
{
	if( pItem->iItem < m_aItems.GetSize() && m_aItems.GetAt( pItem->iItem ) == NULL ) 
		return FALSE; // Á¸ÀçÇÏÁö ¾Ê´Â´Ù.
	LVITEM* pItems = (LVITEM*)m_aItems.GetAt( pItem->iItem );
	memcpy( &pItems[ pItem->iSubItem ], pItem, sizeof( LVITEM ) );
	pItems[ pItem->iSubItem ].pszText = new _TCHAR[ _tcslen( pItem->pszText ) + sizeof( _TCHAR ) ];
	_tcscpy( pItems[ pItem->iSubItem ].pszText, pItem->pszText );
	return TRUE;
}
/*
int CWndItemCtrl::InsertItem( const LVITEM* pItem )
{
	if( pItem->iItem < m_aItems.GetSize() && m_aItems.GetAt( pItem->iItem ) ) 
		return -1; // ÀÌ¹Ì Á¸ÀçÇÑ´Ù.
	LVITEM* pNewItems = new LVITEM[ 3 ]; // m_aColumns.GetSize()
	memcpy( &pNewItems[ pItem->iSubItem ], pItem, sizeof( LVITEM ) );
	pNewItems[ pItem->iSubItem ].pszText = new _TCHAR[ _tcslen( pItem->pszText ) + sizeof( _TCHAR ) ];
	_tcscpy( pNewItems[ pItem->iSubItem ].pszText, pItem->pszText );
	m_aItems.SetAtGrow( pItem->iItem, (void*)pNewItems );
	return pItem->iItem;
}
*/
int CWndItemCtrl::GetItemCount() const
{
	return m_aItems.GetSize();
}
int CWndItemCtrl::InsertColumn( int nCol, const LVCOLUMN* pColumn )
{
	LVCOLUMN* pNewColumn = new LVCOLUMN;
	memcpy( pNewColumn, pColumn, sizeof( LVCOLUMN ) );
	pNewColumn->pszText = new _TCHAR[ _tcslen( pColumn->pszText ) ];
	_tcscpy( pNewColumn->pszText, pColumn->pszText );
	if( nCol < m_aColumns.GetSize() && m_aColumns.GetAt( nCol ) ) 
		return -1; // ÀÌ¹Ì Á¸ÀçÇÑ´Ù.
	m_aColumns.SetAtGrow( nCol, (void*)pNewColumn );
	// ÄÃ·³ ¹öÆ° »ý¼º 
	/*
	CPtrArray m_aWndColumns;
	CRect rect;// = GetWindowRect();
	CWndButton* pWndButton = new CWndButton;
	pWndButton->AddWndStyle( WBS_DOCKING );
	rect = CRect( 0, 0, pColumn->cx, 20 );
	if( nCol )
	{
		LVCOLUMN* pNewColumn = (LVCOLUMN*)m_aColumns.GetAt( nCol - 1 );
		if( 
		i
	}
	pWndButton->Create( pColumn->pszText, 0, rect , this, nCol );
	m_aWndColumns.SetAtGrow( nCol, pWndButton );
	*/
	return nCol;
}
BOOL CWndItemCtrl::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if( zDelta < 0 )
	{
		if( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()+1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() );
	}
	else
	{
		if( m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()-1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMinScrollPos() );
	}
	
	return TRUE;
}

#if __VER >= 8 //__CSC_VER8_5
void CWndItemCtrl::SetDieFlag(BOOL flag)
{
	this->m_useDieFlag = flag;
}
#endif //__CSC_VER8_5
