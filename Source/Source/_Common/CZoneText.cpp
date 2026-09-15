#include "stdafx.h"
#include "CZoneText.h"
#include <iostream>
#include "defineSound.h"
extern CZoneText g_ZoneText;

#ifdef __ZONE_AFFICHAGE
CZoneText::CZoneText()
{	
	m_bEnd = false;
	m_bDraw = false;
	m_ptX = 0;
	sizeText = 0;
	lpMessage = "";
	m_nDuration = SEC(1);
	m_nDurationEnd = SEC(9);
	m_ptXText = 0;
	m_bEndTime = FALSE;
	m_nAlpha = 225;
	m_nAlphaText = 255;
}

CZoneText::~CZoneText()
{
}

void CZoneText::Process()
{
 	if ( !g_pPlayer )
	{
		m_bEnd = false;
		m_bDraw = false;
		m_ptX = 0;
		sizeText = 0;
		m_ptXText = 0;
		m_bEndTime = FALSE;
		m_nAlpha = 225;
		m_nAlphaText = 255;
		vZoneText.clear();
	}
	
	
	if ( !m_bDraw && vZoneText.size() > 0 )
	{
		m_tShow.Set( (float)m_nDuration );
		m_bDraw = true;
	}
		
	
	if( m_bDraw )
	{
		if ( m_bEnd )
		{
			m_bEnd = false;
			m_bDraw = false;
			m_ptX = 0;
			m_ptXText = 0;
			sizeText = 0;
			m_bEndTime = FALSE;
			m_nAlpha = 225;
			m_nAlphaText = 255;	
			vZoneText.clear();
		}
		else
		{
			if(m_ptXText < vZoneText[0].TextSize.cx + 25)
			{
				m_ptX += vZoneText[0].TextSize.cx / 100;
				
				if(vZoneText[0].TextSize.cx / 100 <= 0)
					m_ptX++;
			}		
			
			int nStartAtTime = m_nDuration - SEC(1);
			if ( m_tShow.GetLeftTime() >= nStartAtTime && m_ptXText < vZoneText[0].TextSize.cx + 25)
			{
				m_ptXText += vZoneText[0].TextSize.cx / 100 ;
				
				if(vZoneText[0].TextSize.cx / 100 <= 0)
					m_ptXText++;
			}		
				
			if(m_ptXText >= vZoneText[0].TextSize.cx && m_ptX >= vZoneText[0].TextSize.cx && !m_bEndTime)
			{
				m_bEndTime = TRUE;
				m_tShowEnd.Set( (float)m_nDurationEnd );				
			}	
			
			if(m_bEndTime)
			{	
				if(m_nAlpha > 1)
					m_nAlpha--;
				
				if(m_nAlphaText > 2)
					m_nAlphaText -= 2;
				
				if(m_nAlpha <= 0)
					m_nAlphaText = 0;
				
				int nEndAtTime = m_nDurationEnd - SEC(1);
				if ( m_tShowEnd.GetLeftTime() >= nEndAtTime )
					m_bEnd = TRUE;
			}
				
		}
	}
}

void CZoneText::Render( C2DRender* p2DRender )
{
	if(!m_bDraw)
		return;
	
	p2DRender->RenderFillZone( CPoint( 0, 145 ), CPoint( m_ptX + 50, 145 ),	CPoint( 0 , 210 ),	CPoint( m_ptX + 10, 210), D3DCOLOR_TEMP(m_nAlpha, 30, 30, 30));
						
	if( lpMessage.GetLength() )
    {
		CWndBase::m_Theme.m_pFontZoneText->DrawText((float)(m_ptXText - vZoneText[0].TextSize.cx), 160, 1.0f, 1.0f, D3DCOLOR_ARGB(m_nAlphaText, 255, 255, 255), lpMessage);
	}
	
}

void CZoneText::Add( CString szText )
{
	//if(vZoneText.size() > 0)
	//	return;
	
	vZoneText.clear();
	
	CSize	TextSize;
	CWndBase::m_Theme.m_pFontZoneText->GetTextExtent(szText, &TextSize);
	
	ZONE_TEXT pText;
	pText.strText = szText;
	pText.TextSize = TextSize;
	sizeText = szText.GetLength();
	vZoneText.push_back(pText);
	lpMessage.Format("%s", vZoneText[0].strText );
}

CZoneDesc::CZoneDesc()
{	
	m_bEnd = false;
	m_bDraw = false;
	m_ptX = 0;
	sizeText = 0;
	lpMessage = "";
	m_nDuration = SEC(1);
	m_nDurationEnd = SEC(8);
	m_ptXText = 0;
	m_bEndTime = FALSE;
	m_nAlpha = 225;
	m_nAlphaText = 255;
}

CZoneDesc::~CZoneDesc()
{
}

void CZoneDesc::Process()
{
 	if ( !g_pPlayer )
	{
		m_bEnd = false;
		m_bDraw = false;
		m_ptX = 0;
		sizeText = 0;
		m_ptXText = 0;
		m_bEndTime = FALSE;
		m_nAlpha = 225;
		m_nAlphaText = 255;
		vZoneText.clear();
	}
	
	if ( !m_bDraw && vZoneText.size() > 0 )
	{
		m_tShow.Set( (float)m_nDuration );
		m_bDraw = true;
	}
	
	if( m_bDraw )
	{
		
		if ( m_bEnd )
		{
			m_bEnd = false;
			m_bDraw = false;
			m_ptX = 0;
			m_ptXText = 0;
			sizeText = 0;
			m_bEndTime = FALSE;
			m_nAlpha = 225;
			m_nAlphaText = 255;	
			vZoneText.clear();
		}
		else
		{
			if(m_ptXText < vZoneText[0].TextSize.cx + 25)
			{
				m_ptX += vZoneText[0].TextSize.cx / 100;
				
				if(vZoneText[0].TextSize.cx / 100 <= 0)
					m_ptX++;
			}
				
			
			int nStartAtTime = m_nDuration - SEC(1);
			if ( m_tShow.GetLeftTime() >= nStartAtTime && m_ptXText < vZoneText[0].TextSize.cx + 25)
			{
				m_ptXText += vZoneText[0].TextSize.cx / 100 ;
				
				if(vZoneText[0].TextSize.cx / 100 <= 0)
					m_ptXText++;
			}
					
				
			if(m_ptXText >= vZoneText[0].TextSize.cx && m_ptX >= vZoneText[0].TextSize.cx && !m_bEndTime)
			{
				m_bEndTime = TRUE;
				m_tShowEnd.Set( (float)m_nDurationEnd );				
			}
				
			
			if(m_bEndTime)
			{	
				if(m_nAlpha > 1)
					m_nAlpha--;
				
				if(m_nAlphaText > 2)
					m_nAlphaText -= 2;
				
				if(m_nAlpha <= 0)
					m_nAlphaText = 0;
				
				int nEndAtTime = m_nDurationEnd - SEC(1);
				if ( m_tShowEnd.GetLeftTime() >= nEndAtTime )
					m_bEnd = TRUE;
			}
				
		}
	}
}

void CZoneDesc::Render( C2DRender* p2DRender )
{
	if(!m_bDraw)
		return;
	
	p2DRender->RenderFillZone( CPoint( 0, 215 ), CPoint( m_ptX + 50, 215), CPoint( 0 , 250 ), CPoint( m_ptX + 10, 250), D3DCOLOR_TEMP(m_nAlpha, 30, 30, 30));
						
	if( lpMessage.GetLength() )
    {
		CWndBase::m_Theme.m_pFontZoneTextSub->DrawText((float)(m_ptXText - vZoneText[0].TextSize.cx), 220, 1.0f, 1.0f, D3DCOLOR_ARGB(m_nAlphaText, 255, 255, 255), lpMessage);
	}
}

void CZoneDesc::Add( CString szText )
{
	vZoneText.clear();
	
	CSize	TextSize;
	CWndBase::m_Theme.m_pFontZoneText->GetTextExtent(szText, &TextSize);
	
	ZONE_DESC pText;
	pText.strText = szText;
	pText.TextSize = TextSize;
	sizeText = szText.GetLength();
	vZoneText.push_back(pText);
	lpMessage.Format("%s", vZoneText[0].strText );
}

CZoneText g_ZoneText;
CZoneDesc g_ZoneDesc;
#endif // __ZONE_AFFICHAGE