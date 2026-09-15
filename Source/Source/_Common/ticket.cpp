#include "stdafx.h"
#include "ticket.h"

CTicketProperty::CTicketProperty()
{
}

CTicketProperty::~CTicketProperty()
{
#ifndef __V18TICKET
	m_mapTicketProp.clear();
#endif //__V18TICKET
}

#ifdef __V18TICKET
bool CTicketProperty::isInTicketedWorld(const unsigned long dwWorldId) const
{
	for (std::map<unsigned long, TicketProp>::const_iterator it = indexToTicketProp.cbegin(); it != indexToTicketProp.cend(); ++it)
	{
		if (dwWorldId == it->second.dwWorldId)
			return true;
	}
	return false;
}
bool CTicketProperty::isMultiWorldTicket(const unsigned long itemId) const
{
	return itemToIndex.count(itemId) > 1;
}

std::vector<const TicketProp*> CTicketProperty::getMultiworldTicketProp(const unsigned long itemId) const
{
	std::vector<const TicketProp*> tempVector;
	//const std::multimap<unsigned long, unsigned long>::_Paircc result = itemToIndex.equal_range(itemId);
	auto result = itemToIndex.equal_range(itemId);
	for (std::multimap<unsigned long, unsigned long>::const_iterator it = result.first; it != result.second; ++it)
	{
		const std::map<unsigned long, TicketProp>::const_iterator it2 = indexToTicketProp.find(it->second);
		if (it2 != indexToTicketProp.end())
			tempVector.push_back(&it2->second);
		else
			tempVector.push_back(nullptr);
	}
	return tempVector;
}

const TicketProp* CTicketProperty::getTicketProp(const unsigned long dwItemId) const
{
	const std::multimap<unsigned long, unsigned long>::const_iterator it = itemToIndex.find(dwItemId);
	if (it != itemToIndex.end())
	{
		const std::map<unsigned long, TicketProp>::const_iterator it2 = indexToTicketProp.find(it->second);
		if (it2 != indexToTicketProp.end())
			return &it2->second;
	}
	return nullptr;
}

int CTicketProperty::getExpandedLayer(const unsigned long dwWorldId) const
{
	const std::map<unsigned long, int>::const_iterator it = expandedLayers.find(dwWorldId);
	if (it != expandedLayers.end())
		return it->second;
	return 0;
}
#else //__V18TICKET
BOOL	CTicketProperty::IsTarget( DWORD dwWorldId )
{
	for( map<DWORD, TicketProp>::iterator i = m_mapTicketProp.begin(); i != m_mapTicketProp.end(); ++i )
	{
		if( i->second.dwWorldId == dwWorldId )
			return TRUE;
	}
	return FALSE;
}

TicketProp*	CTicketProperty::GetTicketProp( DWORD dwItemId )
{
	map<DWORD, TicketProp>::iterator i	= m_mapTicketProp.find( dwItemId );
	if( i != m_mapTicketProp.end() )
		return &i->second;
	return NULL;
}
#endif //__V18TICKET

CTicketProperty*	CTicketProperty::GetInstance( void )
{
	static	CTicketProperty sTicketProperty;
	return &sTicketProperty;
}

#ifdef __V18TICKET
bool CTicketProperty::LoadScript()
#else //__V18TICKET
BOOL	CTicketProperty::LoadScript()
#endif //__V18TICKET
{
	CScript s;

#ifdef __V18TICKET
	if (!s.Load("PaidWorldSet.txt"))
		return false;

	unsigned long dwIndex = s.GetNumber();
	while (s.tok != FINISHED)
	{
		TicketProp tp;
		tp.dwWorldId = s.GetNumber();
		tp.vPos.x = s.GetFloat();
		tp.vPos.y = s.GetFloat();
		tp.vPos.z = s.GetFloat();
		s.GetToken(); // Grab the string from the world that is stored in memory already zzz
		indexToTicketProp.insert(std::map<unsigned long, TicketProp>::value_type(dwIndex, tp));
		dwIndex = s.GetNumber();
	}

	if (!s.Load("PaidWorldTicket.txt"))
		return false;

	dwIndex = s.GetNumber();
	unsigned long dwWorldId = 0;
	while (s.tok != FINISHED)
	{
		dwWorldId = s.GetNumber();
		itemToIndex.insert(std::multimap<unsigned long, unsigned long>::value_type(dwIndex, dwWorldId));
		dwIndex = s.GetNumber();
	}

#ifdef __AZRIA_1023
	if (!s.Load("layer.inc"))
		return false;

	dwWorldId = s.GetNumber();
	while (s.tok != FINISHED)
	{
		const int nExpand = s.GetNumber();
		expandedLayers.insert(std::make_pair(dwWorldId, nExpand));
		dwWorldId = s.GetNumber();
	}
	return true;
#endif //__AZRIA_1023
#else //__V18TICKET
	if( s.Load( "ticket.inc" ) == FALSE )
		return FALSE;

	DWORD dwItemId	= s.GetNumber();
	while( s.tok != FINISHED )
	{
		TicketProp	tp;
		tp.dwWorldId	= s.GetNumber();
		tp.vPos.x	= s.GetFloat();
		tp.vPos.y	= s.GetFloat();
		tp.vPos.z	= s.GetFloat();
		bool b	= m_mapTicketProp.insert( map<DWORD, TicketProp>::value_type( dwItemId, tp ) ).second;
		ASSERT( b );
		dwItemId	= s.GetNumber();
	}
#ifdef __AZRIA_1023
	return m_lp.LoadScript();
#endif	// __AZRIA_1023
#endif //__V18TICKET
}

#ifndef __V18TICKET
#ifdef __AZRIA_1023
CLayerProperty::CLayerProperty()
{
}

CLayerProperty::~CLayerProperty()
{
}

BOOL CLayerProperty::LoadScript()
{
	CScript s;
	if( s.Load( "layer.inc" ) == FALSE )
		return FALSE;
	DWORD dwWorldId	= s.GetNumber();
	while( s.tok != FINISHED )
	{
		LayerStruct ls;
		ls.dwWorldId	= dwWorldId;
		ls.nExpand	= s.GetNumber();
		m_vLayers.push_back( ls );
		dwWorldId	= s.GetNumber();
	}
	return TRUE;
}

int CLayerProperty::GetExpanedLayer( DWORD dwWorldId )
{
	for( VLS::iterator i = m_vLayers.begin(); i != m_vLayers.end(); ++i )
		if( ( *i ).dwWorldId == dwWorldId )
			return ( *i ).nExpand;
	return 0;
}
#endif	// __AZRIA_1023
#endif //__V18TICKET