#include "stdafx.h"
#include "resData.h"
#include "WndTicketsDetails.h"
#include "ticket.h"

#ifdef __TICKETS_DETAILS
CWndTicketsDetails::CWndTicketsDetails()
{
    size = 0;
    layer = 0;
}
CWndTicketsDetails::~CWndTicketsDetails()
{
    size = 0;
    layer = 0;
}
void CWndTicketsDetails::OnLayersDetail(CAr& ar)
{
    int nLayer;
    int nSize;

    ar >> nLayer >> nSize;
    size = nSize;
    layer = nLayer;
}
void CWndTicketsDetails::DrawTicketsDetails(C2DRender* p2DRender)
{
    if (!g_pPlayer)
        return;

    if (g_pPlayer->GetWorld()->GetID() != WI_WORLD_KEBARAS
        && g_pPlayer->GetWorld()->GetID() != WI_WORLD_CISLAND
        && g_pPlayer->GetWorld()->GetID() != WI_WORLD_RARTESIA
        && g_pPlayer->GetWorld()->GetID() != WI_WORLD_DARKRARTESIA)
        return;

    CString world;

    if (g_pPlayer->GetWorld()->GetID() == WI_WORLD_KEBARAS)
        world.Format("Azria");
    if (g_pPlayer->GetWorld()->GetID() == WI_WORLD_CISLAND)
        world.Format("Coral Island");
    if (g_pPlayer->GetWorld()->GetID() == WI_WORLD_RARTESIA)
        world.Format("Traseia");
    if (g_pPlayer->GetWorld()->GetID() == WI_WORLD_DARKRARTESIA)
        world.Format("Dark Traseia");

    int PosY = 120;
    char szText[255];
    CD3DFont* pOldFont = p2DRender->GetFont();
    p2DRender->SetFont(CWndBase::m_Theme.m_pFontSRGiuld);
    DWORD dwRenderColor = D3DCOLOR_TEMP(130, 30, 30, 30);

    p2DRender->RenderFillRect(CRect(5, PosY, 185, PosY + 50), dwRenderColor, dwRenderColor, dwRenderColor, dwRenderColor);

    sprintf(szText, "%s [Channel %d]", world, layer - (layer * 2) + 1);
    p2DRender->TextOut(10, PosY + 6, szText, 0xFFf5f5f5);
    sprintf(szText, "Amount of players(s): %d", size);
    p2DRender->TextOut(10, PosY + 28, szText, 0xFFf5f5f5);

    p2DRender->SetFont(pOldFont);
}

CWndTicketsDetails g_LayersDetail;
#endif // __TICKETS_DETAILS