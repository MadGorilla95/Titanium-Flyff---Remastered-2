#pragma once
#ifndef __AEGSHTYP__H
#define __AEGSHTYP__H

#include <atltypes.h>

#ifdef __AEGON_MINIMIZE_WINDOWS
#define __AEG_MINIMIZE_MODE_NONE	0
#define __AEG_MINIMIZE_MODE_REG		1
#define __AEG_MINIMIZE_MODE_TEX		2
#endif

#ifdef __AEGON_F_TOOL
#define MIN_F_TOOL_INTERVALL 0.5
#define MAX_F_TOOL_INTERVALL 86400 //24 hours
#define MAX_F_TOOL_UNITS	100
typedef struct __AEGON_F_TOOL_UNIT
{
	int m_nPage;
	int m_nFKey;
	float m_fIntervall;
	BOOL m_bActive;
	DWORD m_dwNextUse;

	__AEGON_F_TOOL_UNIT()
	{
		Init();
	}
	void Init()
	{
		m_nPage = 0;
		m_nFKey = 0;
		m_fIntervall = 0.0;
		m_bActive = FALSE;
		m_dwNextUse = 0;
	}
	void TimeCheck() 
	{
		if (m_fIntervall < MIN_F_TOOL_INTERVALL)
			m_fIntervall = MIN_F_TOOL_INTERVALL;

		if (m_fIntervall > MAX_F_TOOL_INTERVALL)
			m_fIntervall = MAX_F_TOOL_INTERVALL;
	}
	void Write(CAr& ar)
	{
		TimeCheck();

		ar << m_nPage;
		ar << m_nFKey;
		ar << m_fIntervall;
	}
	void Read(CAr& ar)
	{
		ar >> m_nPage;
		ar >> m_nFKey;
		ar >> m_fIntervall;

		TimeCheck();
	}
	BOOL IsValid()
	{
		if (m_nPage < 0 || m_nPage > 7)
			return FALSE;

		if (m_nFKey < 0 || m_nFKey > 8)
			return FALSE;

		if (m_fIntervall < MIN_F_TOOL_INTERVALL)
			return FALSE;

		if (m_fIntervall > MAX_F_TOOL_INTERVALL)
			return FALSE;

		return TRUE;
	}


}__AEGON_F_TOOL_UNIT;
#endif

typedef struct __AEGON_2D_TRIANGLE
{
	CPoint x;
	CPoint y;
	CPoint z;

	__AEGON_2D_TRIANGLE::__AEGON_2D_TRIANGLE()
	{
		x = CPoint(0, 0);
		y = CPoint(0, 0);
		z = CPoint(0, 0);
	}

	__AEGON_2D_TRIANGLE::__AEGON_2D_TRIANGLE(CPoint a, CPoint b, CPoint c)
	{
		x = a;
		y = b;
		z = c;
	}

	int __AEGON_2D_TRIANGLE::sign(CPoint p1, CPoint p2, CPoint p3)
	{
		return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	}

	BOOL __AEGON_2D_TRIANGLE::IsInTriangle(CPoint pt)
	{
		int d1, d2, d3;
		BOOL has_neg, has_pos;

		d1 = sign(pt, x, y);
		d2 = sign(pt, y, z);
		d3 = sign(pt, z, x);

		has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
		has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

		return !(has_neg && has_pos);
	}

}__AEGON_2D_TRIANGLE;

typedef struct __AEGON_PET_FILTER_LIST_UNIT
{
	vector<DWORD> m_vdwItemList;
	__AEGON_PET_FILTER_LIST_UNIT() 
	{
		Init();
	}
	void Init() 
	{
		m_vdwItemList.clear();
	}

	void Write(CAr& ar)
	{
		ar << static_cast<unsigned long>(m_vdwItemList.size());
		for (int i = 0; i < m_vdwItemList.size(); i++)
		{
			ar << m_vdwItemList[i];
		}
	}
	void Read(CAr& ar)
	{
		unsigned int nSize;
		ar >> nSize;

		m_vdwItemList.clear();
		for (int i = 0; i < nSize; i++)
		{
			DWORD dwListItem;
			ar >> dwListItem;
			if(i < 200)
				m_vdwItemList.push_back(dwListItem);
		}
	}

	void WriteToString(char* szString) 
	{
		char szTmp[32] = { 0, };
		sprintf(szTmp, "");
		unsigned int nLimit = m_vdwItemList.size();
		if (nLimit > 200)
			nLimit = 200;
		for (int i = 0; i < nLimit; i++)
		{
			if (i == nLimit - 1)
			{
				sprintf(szTmp, "%d", m_vdwItemList[i]);
			}
			else 
			{
				sprintf(szTmp, "%d,", m_vdwItemList[i]);
			}
			strcat(szString, szTmp);
		}
		strcat(szString, "/");
	}

	void ReadFromString(char* szString, int* nPos) 
	{
		m_vdwItemList.clear();
		int nCount = 0;
		while (szString[*nPos] != '/' && szString[*nPos] != 0)
		{
			DWORD dwListItem = GetIntPaFromStr(szString, nPos);
			if (dwListItem > 0 && nCount < 200)
			{
				m_vdwItemList.push_back(dwListItem);
				nCount++;
			}
		}
	}

}__AEGON_PET_FILTER_LIST_UNIT;

typedef struct __AEGON_PET_FILTER_LISTS
{
	__AEGON_PET_FILTER_LIST_UNIT sWhitelist;
	__AEGON_PET_FILTER_LIST_UNIT sBlacklist;

	__AEGON_PET_FILTER_LISTS() 
	{
		Init();
	}

	void Init() 
	{
		sWhitelist.Init();
		sBlacklist.Init();
	}

	void Write(CAr& ar)
	{
		sWhitelist.Write(ar);
		sBlacklist.Write(ar);

	}
	void Read(CAr& ar)
	{
		sWhitelist.Read(ar);
		sBlacklist.Read(ar);
	}

	void WriteToString(char* szString) 
	{
		sWhitelist.WriteToString(szString);
		sBlacklist.WriteToString(szString);
		strcat(szString, "$");
	}

	void ReadFromString(char* szString)
	{
		int nPos = 0;
		if (szString[nPos] != '$' && szString[nPos] != 0)
		{
			sWhitelist.ReadFromString(szString, &nPos);
			nPos++;
			sBlacklist.ReadFromString(szString, &nPos);
			nPos++;
		}
	}
	

}__AEGON_PET_FILTER_LISTS;

#endif // __AEGSHTYP__H