#ifndef __TICKETSDETAILS__H__
#define __TICKETSDETAILS__H__
#ifdef __TICKETS_DETAILS

class CWndTicketsDetails : public CWndNeuz
{
public:
	CWndTicketsDetails();
	~CWndTicketsDetails();

	void OnLayersDetail(CAr& ar);
	void DrawTicketsDetails(C2DRender* p2DRender);

private:
	int size;
	int layer;
};
#endif // __TICKETS_DETAILS
#endif // __TICKETSDETAILS__H__