#ifndef __ZONETEXT_H__
#define __ZONETEXT_H__
#ifdef __ZONE_AFFICHAGE
struct ZONE_TEXT
{
	CString	strText;
	CSize	TextSize;
};

struct ZONE_DESC
{
	CString	strText;
	CSize	TextSize;
};

class CZoneText
{
	private:
	public:
		CZoneText();
		virtual ~CZoneText();
		void Process();
		void Render( C2DRender* p2DRender );
		void Add( CString szText );
		vector<ZONE_TEXT>	vZoneText;
		bool m_bEnd;
		bool m_bDraw;
		int m_ptX;
		int m_ptXText;
		int sizeText;
		CString lpMessage;
		CTimer	m_tShow;
		CTimer	m_tShowEnd;
		bool 	m_bEndTime;
		int		m_nDuration;
		int		m_nDurationEnd;
		int		m_nAlpha;
		int		m_nAlphaText;
};

class CZoneDesc
{
	private:
	public:
		CZoneDesc();
		virtual ~CZoneDesc();
		void Process();
		void Render( C2DRender* p2DRender );
		void Add( CString szText );
		vector<ZONE_DESC>	vZoneText;
		bool m_bEnd;
		bool m_bDraw;
		int m_ptX;
		int m_ptXText;
		int sizeText;
		CString lpMessage;
		CTimer	m_tShow;
		CTimer	m_tShowEnd;
		bool 	m_bEndTime;
		int		m_nDuration;
		int		m_nDurationEnd;
		int		m_nAlpha;
		int		m_nAlphaText;
};

#endif // __ZONE_AFFICHAGE
#endif // __ZONETEXT_H__