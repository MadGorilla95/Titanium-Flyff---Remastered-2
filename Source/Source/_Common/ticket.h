#ifndef __TICKET_H__
#define	__TICKET_H__

typedef	struct	_TicketProp
{
	DWORD	dwWorldId;
	D3DXVECTOR3	vPos;
}	TicketProp, *PTicketProp;

#ifdef __AZRIA_1023
#ifndef __V18TICKET
typedef struct	_LayerStruct
{
	DWORD dwWorldId;
	int		nExpand;
}	LayerStruct;
typedef vector<LayerStruct>	VLS;
class CLayerProperty
{
public:
	CLayerProperty();
	virtual ~CLayerProperty();
	BOOL	LoadScript();
	int	GetExpanedLayer(DWORD dwWorldId);
private:
	VLS	m_vLayers;
};
#endif //__V18TICKET
#endif //__AZRIA_1023

class CTicketProperty
{
#ifdef __V18TICKET
public:
	std::multimap<unsigned long, unsigned long> itemToIndex;
	std::map<unsigned long, TicketProp> indexToTicketProp;
#ifdef __AZRIA_1023
	std::map<unsigned long, int> expandedLayers;
#endif //__AZRIA_1023
#endif //__V18TICKET

public:
	CTicketProperty();
	virtual	~CTicketProperty();
	static	CTicketProperty*	GetInstance();

#ifdef __V18TICKET
	bool isMultiWorldTicket(const unsigned long itemId) const;
	std::vector<const TicketProp*> getMultiworldTicketProp(const unsigned long itemId) const;
	bool isInTicketedWorld(const unsigned long dwWorldId) const;
	bool LoadScript();
	const TicketProp* getTicketProp(const unsigned long dwItemId) const;
	int getExpandedLayer(const unsigned long dwWorldId) const;
#else //__V18TICKET
	TicketProp*	GetTicketProp( DWORD dwItemId );
	BOOL	IsTarget( DWORD dwWorldId );
	BOOL	LoadScript();
#ifdef __AZRIA_1023
	int		GetExpanedLayer( DWORD dwWorldId )	{	return m_lp.GetExpanedLayer( dwWorldId );	}
#endif	// __AZRIA_1023
private:
	map<DWORD, TicketProp>	m_mapTicketProp;
#ifdef __AZRIA_1023
	CLayerProperty	m_lp;
#endif // __AZRIA_1023
#endif //__V18TICKET
};
#endif	// __TICKET_H__
