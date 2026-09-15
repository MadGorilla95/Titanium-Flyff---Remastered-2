#include "stdafx.h"
#include "WndNameColor.h"
#include "ResData.h"
#include "DPClient.h"
extern CDPClient g_DPlay;

#ifdef __SYS_NAMECOLOR
CWndNameColor::CWndNameColor()
{
	nChoiceBar = -1;
	m_bLBClick = FALSE;
}
CWndNameColor::~CWndNameColor()
{
	nChoiceBar = -1;
	m_bLBClick = FALSE;
}
void CWndNameColor::OnDraw(C2DRender* p2DRender)
{
	for (int i = 0; i < 3; i++)
	{
		CPoint pt = CPoint(m_ColorScrollBar[i].x - (m_Texture.m_size.cx / 2), m_ColorScrollBar[i].y);

		m_Texture.Render(p2DRender, pt);
	}

	for (int j = 0; j < 3; j++)
	{
		if (m_ColorScrollBar[j].x != m_OriginalColorScrollBar[j].x)
			m_Texture.Render(p2DRender, CPoint(m_OriginalColorScrollBar[j].x - (m_Texture.m_size.cx / 2), m_OriginalColorScrollBar[j].y), 160);
	}

	CString pStaticNC = "Preview: ";
	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	if (g_pPlayer)
	{
		pStatic->SetTitle(pStaticNC + g_pPlayer->GetName());
	}
	else
		pStatic->SetTitle(pStaticNC + "Player Name");

	BYTE color_R = (BYTE)(m_fColor[0]);
	BYTE color_G = (BYTE)(m_fColor[1]);
	BYTE color_B = (BYTE)(m_fColor[2]);
	pStatic->m_dwColor = D3DCOLOR_XRGB(color_R, color_G, color_B);
}
BOOL CWndNameColor::Initialize(CWndBase* pWndParent, DWORD nType)
{
	m_fColor[0] = (FLOAT)g_pPlayer->m_ncR;
	m_fColor[1] = (FLOAT)g_pPlayer->m_ncG;
	m_fColor[2] = (FLOAT)g_pPlayer->m_ncB;
	for (int i = 0; i < 3; i++)
		if (m_fColor[i] < 0)
			m_fColor[i] = 0;

	m_Texture.LoadTexture(g_Neuz.GetDevice(), MakePath(DIR_THEME, "yellowbuttten.tga"), COLOR_ITEM_BG, TRUE);
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_NAME_COLORCHANGE, 0, CPoint(0, 0), pWndParent);
}
void CWndNameColor::OnLButtonDown(UINT nFlags, CPoint point)
{
	int i;
	for (i = 0; i < 3; i++)
	{
		CRect DrawRect = m_ColorRect[i];
		if (DrawRect.PtInRect(point))
			nChoiceBar = i;
	}
	m_bLBClick = true;
}
void CWndNameColor::OnMouseWndSurface(CPoint point)
{
	for (int i = 0; i < 3; i++)
	{
		CRect DrawRect = m_ColorRect[i];

		DrawRect.top += 4;
		DrawRect.bottom += 4;

		if (DrawRect.PtInRect(point) && m_bLBClick)
		{
			point.x = (point.x > DrawRect.right) ? DrawRect.right : point.x;

			LONG Width = DrawRect.right - DrawRect.left;
			LONG Pos = point.x - DrawRect.left;

			FLOAT p = ((FLOAT)((FLOAT)Pos / (FLOAT)Width));

			D3DXVECTOR2 vec1 = D3DXVECTOR2(0.0f, 1.0f);
			D3DXVECTOR2 vec2 = D3DXVECTOR2(1.0f, 1.0f);
			D3DXVECTOR2 vec3;

			D3DXVec2Lerp(&vec3, &vec1, &vec2, p);

			m_fColor[i] = (vec3.x * 255);

			m_ColorScrollBar[i].x = point.x;
		}
	}
}
void CWndNameColor::OnLButtonUp(UINT nFlags, CPoint point)
{
	nChoiceBar = -1;
	m_bLBClick = false;
}
void CWndNameColor::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rect = GetWindowRect(TRUE);

	LPWNDCTRL lpWndCtrl;
	lpWndCtrl = GetWndCtrl(WIDC_CUSTOM_R);
	rect = lpWndCtrl->rect;
	m_ColorRect[0].left = rect.left;
	m_ColorRect[0].top = rect.top + 25;
	m_ColorRect[0].right = rect.right;
	m_ColorRect[0].bottom = rect.bottom + 25;
	m_ColorRect[0] = rect;

	lpWndCtrl = GetWndCtrl(WIDC_CUSTOM_G);
	rect = lpWndCtrl->rect;
	m_ColorRect[1].left = rect.left;
	m_ColorRect[1].top = rect.top + 25;
	m_ColorRect[1].right = rect.right;
	m_ColorRect[1].bottom = rect.bottom + 25;
	m_ColorRect[1] = rect;

	lpWndCtrl = GetWndCtrl(WIDC_CUSTOM_B);
	rect = lpWndCtrl->rect;
	m_ColorRect[2].left = rect.left;
	m_ColorRect[2].top = rect.top + 25;
	m_ColorRect[2].right = rect.right;
	m_ColorRect[2].bottom = rect.bottom + 25;
	m_ColorRect[2] = rect;

	ReSetBar(m_fColor[0], m_fColor[1], m_fColor[2]);

	m_OriginalColorScrollBar[0] = m_ColorScrollBar[0];
	m_OriginalColorScrollBar[1] = m_ColorScrollBar[1];
	m_OriginalColorScrollBar[2] = m_ColorScrollBar[2];

	MoveParentCenter();
}
BOOL CWndNameColor::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (g_pPlayer == NULL)
		return FALSE;

	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case WIDC_OK:
		{
			g_DPlay.SendSetNameColor((int)(m_fColor[0]), (int)(m_fColor[1]), (int)(m_fColor[2]));
			Destroy();
		}
		break;
		case 10000:
			Destroy();
			break;
		}
	}
	return TRUE;
}
void CWndNameColor::ReSetBar(FLOAT r, FLOAT g, FLOAT b)
{
	FLOAT fR = (FLOAT)(r / 255);
	FLOAT fG = (FLOAT)(g / 255);
	FLOAT fB = (FLOAT)(b / 255);

	m_ColorScrollBar[0].x = (LONG)((((m_ColorRect[0].right - m_ColorRect[0].left) * fR)) + m_ColorRect[0].left);
	m_ColorScrollBar[0].y = m_ColorRect[0].top;
	m_ColorScrollBar[1].x = (LONG)((((m_ColorRect[1].right - m_ColorRect[1].left) * fG)) + m_ColorRect[1].left);
	m_ColorScrollBar[1].y = m_ColorRect[1].top;
	m_ColorScrollBar[2].x = (LONG)((((m_ColorRect[2].right - m_ColorRect[2].left) * fB)) + m_ColorRect[2].left);
	m_ColorScrollBar[2].y = m_ColorRect[2].top;
}
#endif //__SYS_NAMECOLOR