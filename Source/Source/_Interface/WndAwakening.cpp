#include "stdafx.h"
#include "resData.h"
#include "WndAwakening.h"
#include "DPClient.h"
#include "WndManager.h"
#include "defineText.h"

#if __VER >= 11 // __SYS_IDENTIFY
#include "randomoption.h"
#endif	// __SYS_IDENTIFY

extern CDPClient g_DPlay;

#ifdef __AEGON_THEME_SWITCHER
#define WND_MAX_AWAKE_COLOR CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_RED)
#else
#define WND_MAX_AWAKE_COLOR D3DCOLOR_ARGB(255, 255, 0, 0)
#endif

#ifdef __AEGON_THEME_SWITCHER
#define WND_REG_AWAKE_COLOR CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE)
#else //__AEGON_THEME_SWITCHER
#define WND_REG_AWAKE_COLOR D3DCOLOR_ARGB(255, 0, 0, 255)
#endif //__AEGON_THEME_SWITCHER

/****************************************************
  WndId : APP_AWAKENING - ¾ÆÀÌÅÛ °¢¼º Ã¢
  CtrlId : WIDC_TEXT1 - 
  CtrlId : WIDC_STATIC1 - 
  CtrlId : WIDC_BUTTON1 - Button
  CtrlId : WIDC_BUTTON2 - Cancel Button
  CtrlId : WIDC_STATIC2 - First Line
  CtrlId : WIDC_STATIC3 - Second Line
  CtrlId : WIDC_STATIC4 - Third Line
****************************************************/
#if __VER >= 11 // __SYS_IDENTIFY

// gmpbigsun ( 10_04_05 ) : CWndAwakening class´Â ÇöÀç ¾²ÀÌÁö ¾ÊÀ½
#ifdef __AEGON_AWAKEWINDOW
CWndAwakening::CWndAwakening()
{
	m_pItemElem = NULL;
	m_pEItemProp = NULL;
	m_pTexture = NULL;

#ifdef __AWAKE_NEEDSTONE
	m_pMatElem = NULL;
	m_pEMatProp = NULL;
	m_pTextureMat = NULL;
#endif // __AWAKE_NEEDSTONE


	m_pLineStr[0] = _T("");
	m_pLineStr[1] = _T("");
	m_pLineStr[2] = _T("");
}
CWndAwakening::~CWndAwakening()
{
}
void CWndAwakening::OnDraw(C2DRender* p2DRender)
{
	if (!g_pPlayer) 
	{
		m_pItemElem = NULL;
		m_pEItemProp = NULL;
		m_pTexture = NULL;

		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		if (pButton)
			pButton->EnableWindow(FALSE);
		return;
	}
	//item
	LPWNDCTRL wndCtrlItem = GetWndCtrl(WIDC_STATIC1);
	CRect rectItem = wndCtrlItem->rect;

#ifdef __AWAKE_NEEDSTONE
	//awakestone
	LPWNDCTRL wndCtrlMat = GetWndCtrl(WIDC_STATIC5);
	CRect rectMat = wndCtrlMat->rect;
	DWORD dwAlphaMat = 255;


	if (!m_pEMatProp)
		m_pEMatProp = prj.GetItemProp(II_SYS_SYS_STONE_AWAKE);

	if (!m_pTextureMat && m_pEMatProp)
		m_pTextureMat = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, m_pEMatProp->szIcon), COLOR_ITEM_BG);

	if (m_pMatElem)
	{
		dwAlphaMat = 255;
	}
	else
		dwAlphaMat = 100;
#endif // __AWAKE_NEEDSTONE

	


	//render item
	if (m_pTexture)
		m_pTexture->Render(p2DRender, CPoint(wndCtrlItem->rect.left, wndCtrlItem->rect.top));

#ifdef __AWAKE_NEEDSTONE
	//render mat
	if (m_pTextureMat && m_pItemElem)
		m_pTextureMat->Render(p2DRender, CPoint(wndCtrlMat->rect.left, wndCtrlMat->rect.top), dwAlphaMat);

#endif // __AWAKE_NEEDSTONE
	// Draw Tooltip
	CPoint pointMouse = GetMousePoint();
	if (rectItem.PtInRect(pointMouse))
	{
		if (m_pItemElem != NULL)
		{
			ClientToScreen(&pointMouse);
			ClientToScreen(&rectItem);
			g_WndMng.PutToolTip_Item((CItemBase*)m_pItemElem, pointMouse, &rectItem);
		}
		else
		{
			ClientToScreen(&pointMouse);
			ClientToScreen(&rectItem);
			CString strEmptyTooltip;
			strEmptyTooltip.Format("Please place the item you would like to awake.");
#if _WIN64
			g_toolTip.PutToolTip(reinterpret_cast<ULONG_PTR>(this), strEmptyTooltip, rectItem, pointMouse);
#else
			g_toolTip.PutToolTip(reinterpret_cast<DWORD>(this), strEmptyTooltip, rectItem, pointMouse);
#endif
		}
	}
#ifdef __AWAKE_NEEDSTONE
	else if (rectMat.PtInRect(pointMouse))
	{
		if (m_pMatElem != NULL)
		{
			ClientToScreen(&pointMouse);
			ClientToScreen(&rectMat);
			g_WndMng.PutToolTip_Item((CItemBase*)m_pMatElem, pointMouse, &rectMat);
		}
		else
		{
			ClientToScreen(&pointMouse);
			ClientToScreen(&rectMat);
			CString strEmptyTooltip;
			strEmptyTooltip.Format("Place the awakestone here");
			g_toolTip.PutToolTip(reinterpret_cast<DWORD>(this), strEmptyTooltip, rectMat, pointMouse);
		}
	}
#endif // __AWAKE_NEEDSTONE



	if (m_pItemElem != NULL) {
		SetAwakeLines();
	}
	else
		SetEmptyLines(3);
}
BOOL CWndAwakening::process()
{
	/*ItemProp* pItemProp;

	if(m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
		if(pItemProp != NULL)
		{
			m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), COLOR_ITEM_BG );
		}
	} */
	return TRUE;
}
void CWndAwakening::SetEmptyLines(int num)
{
#ifdef __AEGON_THEME_SWITCHER
	DWORD dwColor = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
#else //__AEGON_THEME_SWITCHER
	DWORD dwColor = D3DCOLOR_ARGB(255, 46, 112, 169);
#endif //__AEGON_THEME_SWITCHER
	CString  str = CString(_T("- Empty Line -"));
	num = 3 - num;

	for (num; num < 3; num++)
	{
		m_pLine[num]->m_dwColor = dwColor;
		m_pLineStr[num] = str;
		m_pLine[num]->SetTitle(m_pLineStr[num]);
	}
}
extern BOOL IsDst_Rate(int nDstParam);
extern char* FindDstString(int nDstParam);
void CWndAwakening::SetAwakeLines()
{
	if (m_pItemElem == NULL) {

		SetEmptyLines(3);
		return;
	}
	__int64 n64Options = 0;
	int nSize = 0;
	int nDst = 0, nAdj = 0;
	CString str;

	nSize = g_xRandomOptionProperty->GetRandomOptionSize(m_pItemElem->GetRandomOptItemId());
	n64Options = m_pItemElem->GetRandomOptItemId();
	int nRandomOptionKind = g_xRandomOptionProperty->GetRandomOptionKind(m_pItemElem);


	for (int i = 0; i < nSize; i++)
	{
		if (!g_xRandomOptionProperty->GetParam(n64Options, i, &nDst, &nAdj))
			continue;

		if (IsDst_Rate(nDst))
		{
			if (nDst == DST_ATTACKSPEED)
				str.Format("%s %c %d%c", FindDstString(nDst), (nAdj > 0 ? '+' : '-'), ::abs(nAdj / 2 / 10), '%');
			else
				str.Format("%s %c %d%c", FindDstString(nDst), (nAdj > 0 ? '+' : '-'), ::abs(nAdj), '%');
		}
		else
		{
			str.Format("%s %c %d", FindDstString(nDst), (nAdj > 0 ? '+' : '-'), ::abs(nAdj));
		}
#ifdef __AEGON_AWAKE_COLOR
		BOOL isMaxAdjParamary = g_xRandomOptionProperty->IsMaxAdjParamary(nRandomOptionKind, nDst, nAdj);
#else // __AEGON_AWAKE_COLOR
		BOOL isMaxAdjParamary = FALSE;
#endif // __AEGON_AWAKE_COLOR
		dwLineColor[i] = (isMaxAdjParamary ? WND_MAX_AWAKE_COLOR : WND_REG_AWAKE_COLOR);

		m_pLineStr[i] = str;
		m_pLine[i]->SetTitle(m_pLineStr[i]);
		m_pLine[i]->m_dwColor = dwLineColor[i];
	}
	switch (nSize)
	{
	case 0:
		SetEmptyLines(3);
		break;
	case 1:
		SetEmptyLines(2);
		break;
	case 2:
		SetEmptyLines(1);
		break;
	}
}
void CWndAwakening::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	// 여기에 코딩하세요
	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	m_pLine[0] = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	m_pLine[1] = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
	m_pLine[2] = (CWndStatic*)GetDlgItem(WIDC_STATIC4);

	pButton->EnableWindow(FALSE);

	// Awake Information
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);
	CString strTitle;
	if (pWndText)
	{
		strTitle.Format("Each awakening will cost you 200k!");
		pWndText->SetString(strTitle);
	}

#ifdef __AWAKE_NEEDSTONE
	//mat init
	m_pEMatProp = prj.GetItemProp(II_SYS_SYS_STONE_AWAKE);

	if (m_pEMatProp)
		m_pTextureMat = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, m_pEMatProp->szIcon), COLOR_ITEM_BG);
#endif // __AWAKE_NEEDSTONE


	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndAwakening::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_AWAKENING, 0, CPoint(0, 0), pWndParent);
}
/*
  직접 윈도를 열때 사용
BOOL CWnd::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 50 ,50, 300, 300 );
	SetTitle( _T( "title" ) );
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId );
}
*/

void CWndAwakening::OnDestroy()
{
	if (m_pItemElem != NULL)
	{
		if (!g_pPlayer->m_vtInfo.IsTrading(m_pItemElem))
			m_pItemElem->SetExtra(0);
	}
#ifdef __AWAKE_NEEDSTONE
	if (m_pMatElem != NULL)
	{
		if (!g_pPlayer->m_vtInfo.IsTrading(m_pMatElem))
			m_pMatElem->SetExtra(0);
	}
#endif // __AWAKE_NEEDSTONE
}

BOOL CWndAwakening::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndAwakening::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndAwakening::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndAwakening::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndAwakening::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{


	if (nID == WIDC_BUTTON1)
	{

		if (m_pItemElem 
#ifdef __AWAKE_NEEDSTONE
			&& m_pMatElem
#endif // __AWAKE_NEEDSTONE
			)
		{
#ifdef __AWAKE_NEEDSTONE
			BOOL lastStone = FALSE;
			if (m_pMatElem->m_nItemNum == 1)
				lastStone = TRUE;
			g_DPlay.SendAwakening(m_pItemElem->m_dwObjId, m_pMatElem->m_dwObjId);
			if (lastStone)
				SetMat(NULL);
#else // __AWAKE_NEEDSTONE
			g_DPlay.SendAwakening(m_pItemElem->m_dwObjId);
#endif // __AWAKE_NEEDSTONE
		}

	}
	else if (nID == WIDC_BUTTON2)
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndAwakening::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!m_pItemElem) return;


	CRect rectItem;
	LPWNDCTRL wndCtrlItem = GetWndCtrl(WIDC_STATIC1);
	rectItem = wndCtrlItem->rect;

#ifdef __AWAKE_NEEDSTONE
	CRect rectMat;
	LPWNDCTRL wndCtrlMat = GetWndCtrl(WIDC_STATIC5);
	rectMat = wndCtrlMat->rect;
#endif // __AWAKE_NEEDSTONE

	if (rectItem.PtInRect(point))
	{
		SetAwakeItem(NULL);

#ifdef __AWAKE_NEEDSTONE
		if (m_pMatElem)
		{
			SetMat(NULL);
		}
#endif // __AWAKE_NEEDSTONE


		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->EnableWindow(FALSE);
		SetAwakeLines();
	}
#ifdef __AWAKE_NEEDSTONE
	else if (rectMat.PtInRect(point))
	{
		if (m_pMatElem)
		{
			SetMat(NULL);

			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
			pButton->EnableWindow(FALSE);
		}
	}
#endif // __AWAKE_NEEDSTONE
}

BOOL CWndAwakening::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CItemElem* pTempElem;
	pTempElem = (CItemElem*)g_pPlayer->GetItemId(pShortcut->m_dwId);

	LPWNDCTRL wndCtrlItem = GetWndCtrl(WIDC_STATIC1);
	CRect rectItem = wndCtrlItem->rect;

#ifdef __AWAKE_NEEDSTONE
	LPWNDCTRL wndCtrlMat = GetWndCtrl(WIDC_STATIC5);
	CRect rectMat = wndCtrlMat->rect;
#endif // __AWAKE_NEEDSTONE

	if (rectItem.PtInRect(point))
	{
		SetAwakeItem(pTempElem);

	}
#ifdef __AWAKE_NEEDSTONE
	else if (rectMat.PtInRect(point))
	{
		if (pTempElem != NULL)
		{
			SetMat(pTempElem);
		}

	}
#endif // __AWAKE_NEEDSTONE
#ifdef __AWAKE_NEEDSTONE
	if (m_pItemElem && m_pMatElem)
#else // __AWAKE_NEEDSTONE
	if (m_pItemElem)
#endif // __AWAKE_NEEDSTONE
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->EnableWindow(TRUE);
	}
	return TRUE;
}
void CWndAwakening::SetAwakeItem(CItemElem* pItemElem)
{
	if (!pItemElem || pItemElem->m_nItemNum <= 0)
	{
		m_pItemElem->SetExtra(0);
		m_pItemElem = NULL;
		m_pEItemProp = NULL;
		m_pTexture = NULL;

		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->EnableWindow(FALSE);

		return;
	}

	int nRandomOptionKind = g_xRandomOptionProperty->GetRandomOptionKind(pItemElem);

	if (nRandomOptionKind != CRandomOptionProperty::eAwakening					//eAwakening -> Sets / Weapons
#ifdef __AWAKENING_EXTENDED
		&& nRandomOptionKind != CRandomOptionProperty::eWeapon
#endif //__AWAKENING_EXTENDED
		//&& nRandomOptionKind != CRandomOptionProperty::eAwakeningExtension	//?
		//&& nRandomOptionKind != CRandomOptionProperty::eMaxRandomOptionKind	//?
		&& nRandomOptionKind != CRandomOptionProperty::eBlessing				//eBlessing -> Fashion
		&& nRandomOptionKind != CRandomOptionProperty::eEatPet					//eEatPet -> buffpet
		&& nRandomOptionKind != CRandomOptionProperty::eSystemPet)			    //eSystemPet -> normal pet 			
	{
		DWORD dwColorRed = D3DCOLOR_ARGB(255, 255, 0, 0);
		LPCTSTR wrongItem = _T("This item cant be awakened!");
		g_WndMng.PutString(wrongItem, NULL, dwColorRed);

		return;
	}
	if (pItemElem != NULL)
	{
		if (m_pItemElem)
			m_pItemElem->SetExtra(0);

		m_pItemElem = pItemElem;
		m_pEItemProp = m_pItemElem->GetProp();
		m_pItemElem->SetExtra(m_pItemElem->GetExtra() + 1);

		LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_STATIC1);
		if (m_pEItemProp != NULL)
		{
			m_pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, m_pEItemProp->szIcon), COLOR_ITEM_BG);
		}

		SetAwakeLines();
	}
#ifdef __AWAKE_NEEDSTONE
	if (m_pItemElem && m_pMatElem)
#else // __AWAKE_NEEDSTONE
	if (m_pItemElem)
#endif // __AWAKE_NEEDSTONE
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->EnableWindow(TRUE);
	}
}
#ifdef __AWAKE_NEEDSTONE
void CWndAwakening::SetMat(CItemElem* pItemElem)
{
	if (!pItemElem || pItemElem->m_nItemNum <= 0)
	{
		if (m_pMatElem)
		{
			m_pMatElem->SetExtra(0);
			m_pMatElem = NULL;

			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
			pButton->EnableWindow(FALSE);
		}
		return;
	}

	if (pItemElem->m_dwItemId && pItemElem->m_dwItemId == II_SYS_SYS_STONE_AWAKE)
	{
		if (!m_pMatElem)
		{
			if (m_pMatElem)
				m_pMatElem->SetExtra(0);

			m_pMatElem = pItemElem;
			m_pEMatProp = m_pMatElem->GetProp();
			m_pMatElem->SetExtra(m_pMatElem->GetExtra() + 1);
		}
	}
	if (m_pItemElem && m_pMatElem)
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->EnableWindow(TRUE);
	}
}
#endif // __AWAKE_NEEDSTONE

void CWndAwakening::SetItem(CItemElem* pItemElem)
{
	if (pItemElem->m_nItemNum <= 0)
		return;

#ifdef __AWAKE_NEEDSTONE
	if (pItemElem->m_dwItemId == II_SYS_SYS_STONE_AWAKE)
	{
		SetMat(pItemElem);
	}
	else
#endif // __AWAKE_NEEDSTONE
	{
		SetAwakeItem(pItemElem);
	}
}
#endif // __AEGON_AWAKEWINDOW
#endif

#ifdef __PROTECT_AWAKE
CWndSelectAwakeCase::CWndSelectAwakeCase( )
: m_dwDeltaTime( 0 ),
m_dwItemIndex( 0 ),
m_pTexture( NULL ),
m_pVertexBufferGauge(NULL), 
m_pTexGuage( NULL )
{
	m_dwOldTime = g_tmCurrent;

	m_byObjID = 0;
	m_dwSerialNum = 0;
	m_n64NewOption = 0;
}

CWndSelectAwakeCase::~CWndSelectAwakeCase( )
{
	DeleteDeviceObjects();
}
// Ã³À½ ÀÌ ÇÔ¼ö¸¦ ºÎ¸£¸é À©µµ°¡ ¿­¸°´Ù.
BOOL CWndSelectAwakeCase::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy¿¡¼­ ¼³Á¤ÇÑ ¸®¼Ò½º·Î À©µµ¸¦ ¿¬´Ù.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_AWAKE_SELECTCASE, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndSelectAwakeCase::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	// ¿©±â¿¡ ÄÚµùÇÏ¼¼¿ä

	// À©µµ¸¦ Áß¾ÓÀ¸·Î ¿Å±â´Â ºÎºÐ.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	ItemProp* pProp = (ItemProp*)prj.GetItemProp( m_dwItemIndex );
	if( pProp )
		m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pProp->szIcon ), COLOR_ITEM_BG );
	
	m_pTexGuage = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "Wndguage.tga"   ), COLOR_ITEM_BG );

	if( !m_pTexGuage )
		Error( "CWndSelectAwakeCase::OnInitialUpdate m_pTexGuage(Wndguage.tga) is NULL" );

	AddWndStyle( WBS_MODAL );

	RestoreDeviceObjects( );
} 


BOOL CWndSelectAwakeCase::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 )
	{
		g_DPlay.SendSelectedAwakeningValue( m_byObjID, m_dwSerialNum, _AWAKE_OLD_VALUE );
		g_WndMng.PutString( GETTEXT( TID_GAME_REGARDLESS_USE03 ) );	//¼±ÅÃµÊ
		Destroy();
	}
	else if( nID == WIDC_BUTTON2 )
	{
		g_DPlay.SendSelectedAwakeningValue( m_byObjID, m_dwSerialNum, _AWAKE_NEW_VALUE );
		g_WndMng.PutString( GETTEXT( TID_GAME_REGARDLESS_USE03 ) );	//¼±ÅÃµÊ
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

const static DWORD AWAKE_KEEP_TIME = 60000;	//60ÃÊ

BOOL CWndSelectAwakeCase::process()
{
	return TRUE;
}

void CWndSelectAwakeCase::OnDraw( C2DRender* p2DRender ) 
{ 
	if( !g_pPlayer )
		return;

	CItemElem* pItemElem = (CItemElem*)g_pPlayer->GetItemId( m_byObjID );
	if( !pItemElem )
	{
		assert( 0 );
		return;
	}

	m_dwDeltaTime += (g_tmCurrent - m_dwOldTime);
	m_dwOldTime = g_tmCurrent;

	if( m_dwDeltaTime > AWAKE_KEEP_TIME )
	{
		//Á¦ÇÑ½Ã°£ÀÌ Áö³ª¸é ÇöÀç¿É¼ÇÀ¸·Î ¿äÃ» 
		g_DPlay.SendSelectedAwakeningValue( m_byObjID, m_dwSerialNum, _AWAKE_OLD_VALUE );

		g_WndMng.PutString( GETTEXT( TID_GAME_REGARDLESS_USE02 ) );	//¼±ÅÃ½Ã°£ ÃÊ°ú
		
		m_dwDeltaTime = 0;
		Destroy();
	}

	//old
	OutputOptionString( p2DRender, pItemElem );

	//new
	OutputOptionString( p2DRender, pItemElem, TRUE );


	CRect rect = GetWindowRect();
	rect.left += 14;
	rect.top += 174;

	// ½Ã°£ / 100 * 360(ÃÑ±æÀÌ) / 600 (ÃÑ½Ã°£)
	rect.right = LONG(( rect.left + (AWAKE_KEEP_TIME - m_dwDeltaTime) / 100 ) * 0.6f) ;		//±ÍÂúÀ¸´Ï °Á ¹ÙÀÇ ±æÀÌ¸¦ ÃÖ´ëÃÊ·Î...
	rect.bottom = rect.top + 20;

//	if( m_pTexGuage )
//		p2DRender->RenderFillRect( rect, COLOR_ERROR, m_pTexGuage->m_pTexture  );

	m_pTheme->RenderGauge(p2DRender, &rect, 0xffffffff, m_pVertexBufferGauge, m_pTexGuage);

	// draw icon
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	assert( wndCtrl );
			
	if(m_pTexture)
		m_pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
} 

extern BOOL IsDst_Rate( int nDstParam );
extern char *FindDstString( int nDstParam );
void CWndSelectAwakeCase::OutputOptionString( C2DRender* p2DRender, CItemElem* pItemElem, BOOL bNew )
{
	// ¿É¼Ç ¹®ÀÚ¿­ ±×¸®±â
	if( !pItemElem )
		return;

    int nSize = 0;
	if( !bNew )
		nSize = g_xRandomOptionProperty->GetRandomOptionSize( pItemElem->GetRandomOptItemId() );
	else nSize = g_xRandomOptionProperty->GetViewRandomOptionSize( m_n64NewOption );

	__int64 n64Options = 0;
	n64Options = ( bNew ? m_n64NewOption : pItemElem->GetRandomOptItemId() );

	CString str;

	// option
	for( int i = 0; i < nSize; i++ )
	{
		int nDst = 0, nAdj = 0;
		if( !g_xRandomOptionProperty->GetParam( n64Options, i, &nDst, &nAdj ) )
			continue;

		if( IsDst_Rate( nDst ) )
		{
			if( nDst == DST_ATTACKSPEED )
				str.Format( "\n%s %c%d%% ", FindDstString( nDst ), ( nAdj > 0? '+': '-' ), ::abs( nAdj / 2 / 10 ) );
			else
				str.Format( "\n%s %c%d%%", FindDstString( nDst ), ( nAdj > 0? '+': '-' ), ::abs( nAdj ) );
		}
		else
		{
			str.Format( "\n%s %c%d", FindDstString( nDst ), ( nAdj > 0? '+': '-' ), ::abs( nAdj ) );
		}

		//°Á ÇÑÁÙÇÑÁÙ¾¿ ÂïÀÚ
		if( bNew )
			p2DRender->TextOut( 70, 92 + i * 20, str, 0xff000000 );
		else p2DRender->TextOut( 70, 20 + i * 20, str, 0xff000000 );
	}
}

void CWndSelectAwakeCase::SetData( BYTE byObjID, DWORD dwSerialNum, __int64 n64NewOption ) 
{ 
	//server·Î Àü¼ÛÇÒ µ¥ÀÌÅÍ¸¦ À¯ÁöÇÏ°í, ¾ÆÀÌÅÛ Index¸¦ »Ì¾Æ¼­ ¾ÆÀÌÄÜÀ» ±×·ÁÁÙ ÁØºñ
	m_byObjID = byObjID;
	m_dwSerialNum = dwSerialNum;
	m_n64NewOption = n64NewOption; 

	if( !g_pPlayer )
	{
		assert( 0 );
		return;
	}

	CItemBase* pItemBase = g_pPlayer->GetItemId( m_byObjID );
	if( pItemBase )
	{
		ItemProp* pProp = pItemBase->GetProp( );
		if(pProp)
			m_dwItemIndex = pProp->dwID;
	}
}

HRESULT CWndSelectAwakeCase::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVertexBufferGauge == NULL )
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, 
												 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
												 D3DFVF_TEXTUREVERTEX2, 
												 D3DPOOL_DEFAULT, 
												 &m_pVertexBufferGauge, 
												 NULL);
	assert(m_pVertexBufferGauge != NULL);
	
	return S_OK;
}
HRESULT CWndSelectAwakeCase::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVertexBufferGauge );
	return S_OK;
}
HRESULT CWndSelectAwakeCase::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVertexBufferGauge );
	return S_OK;
}

#endif	//__PROTECT_AWAKE

#ifdef __AEGON_RETURN_AWAKE
CWndReturnAwakening::CWndReturnAwakening()
{
	m_pItemElem = NULL;
	m_pEItemProp = NULL;
	m_pItemTexture = NULL;

	m_pMaterialElem = NULL;
	m_pEMaterialProp = NULL;
	m_pMaterialTexture = NULL;

	m_pCurrentLineStr[0] = _T("");
	m_pCurrentLineStr[1] = _T("");
	m_pCurrentLineStr[2] = _T("");

	m_pOldLineStr[0] = _T("");
	m_pOldLineStr[1] = _T("");
	m_pOldLineStr[2] = _T("");
}
CWndReturnAwakening::~CWndReturnAwakening()
{
}
void CWndReturnAwakening::OnDraw(C2DRender* p2DRender)
{
	LPWNDCTRL wndCtrlItem = GetWndCtrl(WIDC_STATIC1);
	LPWNDCTRL wndCtrlMaterial = GetWndCtrl(WIDC_STATIC5);
	DWORD dwAlphaMat = 255;

	if (m_pItemElem != NULL) {
		SetAwakeLines(0);
		SetAwakeLines(1);

		if (!m_pMaterialElem)
		{
			ItemProp* materialProp = prj.GetItemProp(II_SYS_SYS_SCR_AEGONREVERT);
			if (materialProp)
				m_pMaterialTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, materialProp->szIcon), COLOR_ITEM_BG);
			dwAlphaMat = 100;

		}
	}
	else
		m_pMaterialTexture = NULL;

	if (m_pItemTexture)
		m_pItemTexture->Render(p2DRender, CPoint(wndCtrlItem->rect.left, wndCtrlItem->rect.top));

	if (m_pMaterialTexture)
		m_pMaterialTexture->Render(p2DRender, CPoint(wndCtrlMaterial->rect.left, wndCtrlMaterial->rect.top), dwAlphaMat);


}
BOOL CWndReturnAwakening::process()
{
	/*ItemProp* pItemProp;

	if(m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
		if(pItemProp != NULL)
		{
			m_pItemTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), COLOR_ITEM_BG );
		}
	} */
	return TRUE;
}
void CWndReturnAwakening::SetEmptyLines(int nMode, int num)
{
#ifdef __AEGON_THEME_SWITCHER
	DWORD dwColor = CAegonThemeSwitcherMng::GetInstance()->GetTextColor();
#else //__AEGON_THEME_SWITCHER
	DWORD dwColor = D3DCOLOR_ARGB(255, 46, 112, 169);
#endif //__AEGON_THEME_SWITCHER
	CString  str = CString(_T("- Empty Line -"));
	num = 3 - num;

	switch (nMode)
	{
	case 0:
		for (num; num < 3; num++)
		{
			m_pCurrentLine[num]->m_dwColor = dwColor;
			m_pCurrentLineStr[num] = str;
			m_pCurrentLine[num]->SetTitle(m_pCurrentLineStr[num]);
		}
		break;
	case 1:
		for (num; num < 3; num++)
		{
			m_pOldLine[num]->m_dwColor = dwColor;
			m_pOldLineStr[num] = str;
			m_pOldLine[num]->SetTitle(m_pOldLineStr[num]);
		}
		break;
	default:
		break;
	}

}
extern BOOL IsDst_Rate(int nDstParam);
extern char* FindDstString(int nDstParam);
void CWndReturnAwakening::SetAwakeLines(int nMode)
{
	if (m_pItemElem == NULL) {

		SetEmptyLines(0, 3);
		SetEmptyLines(1, 3);
		return;
	}
	__int64 n64Options = 0;
	int nSize = 0;
	int nDst = 0, nAdj = 0;
	CString str;

	int nRandomOptionKind = g_xRandomOptionProperty->GetRandomOptionKind(m_pItemElem);

	switch (nMode)
	{
	case 0:
		nSize = g_xRandomOptionProperty->GetRandomOptionSize(m_pItemElem->GetRandomOptItemId());
		n64Options = m_pItemElem->GetRandomOptItemId();
		break;
	case 1:
		nSize = g_xRandomOptionProperty->GetRandomOptionSize(m_pItemElem->GetOldRandomOptItemId());
		n64Options = m_pItemElem->GetOldRandomOptItemId();
		break;
	default:
		break;
	}



	for (int i = 0; i < nSize; i++)
	{
		if (!g_xRandomOptionProperty->GetParam(n64Options, i, &nDst, &nAdj))
			continue;

		if (IsDst_Rate(nDst))
		{
			if (nDst == DST_ATTACKSPEED)
				str.Format("%s %c %d%c", FindDstString(nDst), (nAdj > 0 ? '+' : '-'), ::abs(nAdj / 2 / 10), '%');
			else
				str.Format("%s %c %d%c", FindDstString(nDst), (nAdj > 0 ? '+' : '-'), ::abs(nAdj), '%');
		}
		else
		{
			str.Format("%s %c %d", FindDstString(nDst), (nAdj > 0 ? '+' : '-'), ::abs(nAdj));
		}

		BOOL isMaxAdjParamary = g_xRandomOptionProperty->IsMaxAdjParamary(nRandomOptionKind, nDst, nAdj);



		switch (nMode)
		{
		case 0:
			dwCurrentLineColor[i] = (isMaxAdjParamary ? WND_MAX_AWAKE_COLOR : WND_REG_AWAKE_COLOR);
			m_pCurrentLineStr[i] = str;
			m_pCurrentLine[i]->SetTitle(m_pCurrentLineStr[i]);
			m_pCurrentLine[i]->m_dwColor = dwCurrentLineColor[i];
			break;
		case 1:
			dwOldLineColor[i] = (isMaxAdjParamary ? WND_MAX_AWAKE_COLOR : WND_REG_AWAKE_COLOR);
			m_pOldLineStr[i] = str;
			m_pOldLine[i]->SetTitle(m_pOldLineStr[i]);
			m_pOldLine[i]->m_dwColor = dwOldLineColor[i];
			break;
		default:
			break;
		}

	}
	switch (nSize)
	{
	case 0:
		SetEmptyLines(nMode, 3);
		break;
	case 1:
		SetEmptyLines(nMode, 2);
		break;
	case 2:
		SetEmptyLines(nMode, 1);
		break;
	}
}
void CWndReturnAwakening::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	// 여기에 코딩하세요
	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	m_pCurrentLine[0] = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	m_pCurrentLine[1] = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
	m_pCurrentLine[2] = (CWndStatic*)GetDlgItem(WIDC_STATIC4);

	m_pOldLine[0] = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
	m_pOldLine[1] = (CWndStatic*)GetDlgItem(WIDC_STATIC7);
	m_pOldLine[2] = (CWndStatic*)GetDlgItem(WIDC_STATIC8);

	CWndStatic* currentAwake = (CWndStatic*)GetDlgItem(WIDC_STATIC9);
	CWndStatic* oldAwake = (CWndStatic*)GetDlgItem(WIDC_STATIC10);

	CWndStatic* itemLevel = (CWndStatic*)GetDlgItem(WIDC_STATIC11);
	CWndStatic* revertFee = (CWndStatic*)GetDlgItem(WIDC_STATIC12);

	currentAwake->m_dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	oldAwake->m_dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);

	itemLevel->m_dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	revertFee->m_dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);

	pButton->EnableWindow(FALSE);

	SetEmptyLines(0, 3);
	SetEmptyLines(1, 3);

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndReturnAwakening::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_RETURN_AWAKENING, 0, CPoint(0, 0), pWndParent);
}
/*
  직접 윈도를 열때 사용
BOOL CWnd::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 50 ,50, 300, 300 );
	SetTitle( _T( "title" ) );
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId );
}
*/

void CWndReturnAwakening::OnDestroy()
{
	if (m_pItemElem != NULL)
	{
		if (!g_pPlayer->m_vtInfo.IsTrading(m_pItemElem))
			m_pItemElem->SetExtra(0);
	}

	if (m_pMaterialElem != NULL)
	{
		if (!g_pPlayer->m_vtInfo.IsTrading(m_pMaterialElem))
			m_pMaterialElem->SetExtra(0);
	}
}

BOOL CWndReturnAwakening::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndReturnAwakening::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndReturnAwakening::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndReturnAwakening::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndReturnAwakening::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	LPWNDCTRL wndCtrlMaterial = GetWndCtrl(WIDC_STATIC5);

	if (nID == WIDC_BUTTON1)
	{
		if (m_pItemElem != NULL)
		{
			if (m_pItemElem && m_pMaterialElem)
			{
				g_DPlay.SendReturnAwakening(m_pItemElem->m_dwObjId, m_pMaterialElem->m_dwObjId);

				OnLButtonDblClk(0, wndCtrlMaterial->rect.CenterPoint());
			}
		}
	}
	else if (nID == WIDC_BUTTON2)
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndReturnAwakening::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!m_pItemElem) return;

	CRect rectItem, rectMaterial;
	LPWNDCTRL wndCtrlItem = GetWndCtrl(WIDC_STATIC1);
	LPWNDCTRL wndCtrlMaterial = GetWndCtrl(WIDC_STATIC5);

	CWndStatic* itemLevel = (CWndStatic*)GetDlgItem(WIDC_STATIC11);
	CWndStatic* revertFee = (CWndStatic*)GetDlgItem(WIDC_STATIC12);

	rectItem = wndCtrlItem->rect;
	rectMaterial = wndCtrlMaterial->rect;

	if (rectItem.PtInRect(point))
	{
		//remove item
		m_pItemElem->SetExtra(0);
		m_pItemElem = NULL;
		m_pEItemProp = NULL;
		m_pItemTexture = NULL;

		CString strItemLevel;
		CString strRevertFee;

		strItemLevel.Format("Item Level:");
		itemLevel->SetTitle(strItemLevel);

		strRevertFee.Format("Revert Fee:");
		revertFee->SetTitle(strRevertFee);

		if (m_pMaterialElem)
		{
			//remove material
			m_pMaterialElem->SetExtra(0);
			m_pMaterialElem = NULL;
			m_pEMaterialProp = NULL;
			m_pMaterialTexture = NULL;
		}
		//disable button
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->EnableWindow(FALSE);
		SetAwakeLines(0);
		SetAwakeLines(1);
	}
	else if (rectMaterial.PtInRect(point) && m_pMaterialElem)
	{
		//remove material
		m_pMaterialElem->SetExtra(0);
		m_pMaterialElem = NULL;
		m_pEMaterialProp = NULL;
		m_pMaterialTexture = NULL;

		//disable button
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->EnableWindow(FALSE);
		SetAwakeLines(0);
		SetAwakeLines(1);
	}
}

BOOL CWndReturnAwakening::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CItemElem* pTempElem;
	pTempElem = (CItemElem*)g_pPlayer->GetItemId(pShortcut->m_dwId);

	CRect rect, rect2;
	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_STATIC1);
	LPWNDCTRL wndCtrl2 = GetWndCtrl(WIDC_STATIC5);

	CWndStatic* itemLevel = (CWndStatic*)GetDlgItem(WIDC_STATIC11);
	CWndStatic* revertFee = (CWndStatic*)GetDlgItem(WIDC_STATIC12);

	rect = wndCtrl->rect;
	rect2 = wndCtrl2->rect;
	if (rect.PtInRect(point))
	{
		int nRandomOptionKind = g_xRandomOptionProperty->GetRandomOptionKind(pTempElem);

		if (nRandomOptionKind != CRandomOptionProperty::eAwakening					//eAwakening -> Sets / Weapons
#ifdef __AWAKENING_EXTENDED
			&& nRandomOptionKind != CRandomOptionProperty::eWeapon
#endif //__AWAKENING_EXTENDED
			//&& nRandomOptionKind != CRandomOptionProperty::eAwakeningExtension	//?
			//&& nRandomOptionKind != CRandomOptionProperty::eMaxRandomOptionKind	//?
			&& nRandomOptionKind != CRandomOptionProperty::eBlessing				//eBlessing -> Fashion
			&& nRandomOptionKind != CRandomOptionProperty::eEatPet					//eEatPet -> buffpet
			&& nRandomOptionKind != CRandomOptionProperty::eSystemPet)			    //eSystemPet -> normal pet 			
		{
			DWORD dwColorRed = D3DCOLOR_ARGB(255, 255, 0, 0);
			LPCTSTR wrongItem = _T("Invalid Item!");
			g_WndMng.PutString(wrongItem, NULL, dwColorRed);

			return FALSE;
		}
		if (pTempElem != NULL)
		{
			if (m_pItemElem) m_pItemElem->SetExtra(0);
			m_pItemElem = pTempElem;
			m_pEItemProp = m_pItemElem->GetProp();
			m_pItemElem->SetExtra(m_pItemElem->GetExtra() + 1);

			CString strItemLevel;
			CString strRevertFee;

			strItemLevel.Format("Item Level: %s", GetRevertItemLevelStr(m_pEItemProp));
			itemLevel->SetTitle(strItemLevel);

			strRevertFee.Format("Revert Fee: %s", GetRevertPriceStr(m_pEItemProp));
			revertFee->SetTitle(strRevertFee);



			if (m_pEItemProp != NULL)
			{
				m_pItemTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, m_pEItemProp->szIcon), COLOR_ITEM_BG);
			}
		}

	}
	else if (rect2.PtInRect(point))
	{
		if (!m_pEItemProp)
		{
			DWORD dwColorRed = D3DCOLOR_ARGB(255, 255, 0, 0);
			LPCTSTR wrongItem = _T("Please put your item first!");
			g_WndMng.PutString(wrongItem, NULL, dwColorRed);
			return FALSE;
		}
		if (pTempElem != NULL)
		{
			if (pTempElem->GetPropA()->dwID != II_SYS_SYS_SCR_AEGONREVERT)
			{
				DWORD dwColorRed = D3DCOLOR_ARGB(255, 255, 0, 0);
				LPCTSTR wrongItem = _T("Invalid Material!");
				g_WndMng.PutString(wrongItem, NULL, dwColorRed);

				return FALSE;
			}
			if (m_pMaterialElem) m_pMaterialElem->SetExtra(0);
			m_pMaterialElem = pTempElem;
			m_pEMaterialProp = m_pMaterialElem->GetProp();
			m_pMaterialElem->SetExtra(m_pMaterialElem->GetExtra() + 1);

			if (m_pEMaterialProp != NULL)
			{
				m_pMaterialTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, m_pEMaterialProp->szIcon), COLOR_ITEM_BG);
			}
		}



	}

	if (m_pItemElem != NULL && m_pMaterialElem != NULL)
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->EnableWindow(TRUE);
	}

	return TRUE;
}
CString CWndReturnAwakening::GetRevertPriceStr(ItemProp* pItemProp)
{
	DWORD nItemLevel, nCost;
	CString strOut;

	nItemLevel = pItemProp->dwItemLV;
	if (nItemLevel > 1 && nItemLevel <= 60)
	{
		nCost = 25000000; //25M
	}
	else if (nItemLevel > 60 && nItemLevel <= 90)
	{
		nCost = 50000000; //50M
	}
	else if (nItemLevel > 90 && nItemLevel <= 125)
	{
		nCost = 100000000; //1P
	}
	else if (nItemLevel > 125 && nItemLevel <= 155)
	{
		nCost = 500000000; //5P
	}
	else if (nItemLevel > 155 && nItemLevel <= 175)
	{
		nCost = 1000000000; //10P
	}
	else
	{
		//level 0-1
		if (pItemProp->dwItemKind3 == IK3_PET)
		{
			//pick up pet
			nCost = 100000000; //1P
		}
		else
		{
			//fashion
			nCost = 100000000; //1P
		}
		//real level 1 weapons / parts
		if (pItemProp->dwID == II_WEA_SWO_WOODEN || pItemProp->dwID == II_WEA_SWO_LONG
			|| pItemProp->dwID == II_ARM_M_VAG_SUIT01 || pItemProp->dwID == II_ARM_F_VAG_SUIT01
			|| pItemProp->dwID == II_ARM_M_VAG_GAUNTLET01 || pItemProp->dwID == II_ARM_F_VAG_GAUNTLET01
			|| pItemProp->dwID == II_ARM_M_VAG_BOOTS01 || pItemProp->dwID == II_ARM_F_VAG_BOOTS01
			)
		{
			nCost = 25000000; //25M
		}
		if (nItemLevel == -1)
		{
			//raised pet
			nCost = 500000000; //5P
		}
	}

	if (nCost < 100000000)
	{
		strOut.Format("%dM Penya", nCost / 1000000);
	}
	else
	{
		strOut.Format("%d Perin", nCost / 100000000);
	}

	return strOut;

}

CString CWndReturnAwakening::GetRevertItemLevelStr(ItemProp* pItemProp)
{
	DWORD nItemLevel;
	CString strOut;

	nItemLevel = pItemProp->dwItemLV;

	if (nItemLevel >= 1 && nItemLevel <= 175)
	{
		if (nItemLevel == 1 && pItemProp->dwID != II_WEA_SWO_WOODEN && pItemProp->dwID != II_WEA_SWO_LONG
			&& pItemProp->dwID != II_ARM_M_VAG_SUIT01 && pItemProp->dwID != II_ARM_F_VAG_SUIT01
			&& pItemProp->dwID != II_ARM_M_VAG_GAUNTLET01 && pItemProp->dwID != II_ARM_F_VAG_GAUNTLET01
			&& pItemProp->dwID != II_ARM_M_VAG_BOOTS01 && pItemProp->dwID != II_ARM_F_VAG_BOOTS01
			)
		{

			if (pItemProp->dwItemKind3 == IK3_PET)
			{
				strOut.Format("Pick-Up Pet");

			}
			else
				strOut.Format("Fashion");
		}
		else
			strOut.Format("%d", nItemLevel);
	}
	else if (nItemLevel == -1)
		strOut.Format("Pet");
	else
		strOut.Format("Unknown");

	return strOut;

}
#endif // __AEGON_RETURN_AWAKE