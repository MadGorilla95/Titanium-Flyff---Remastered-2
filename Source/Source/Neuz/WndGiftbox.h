#pragma once
#ifdef __GIFTBOX_LIST_ITEMS
class CWndGiftbox : public CWndNeuz
{
public:
	CWndGiftbox();
	~CWndGiftbox();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);

	bool Serialize(CAr& ar);

	vector<pair<DWORD, pair<DWORD, FLOAT>>> Get();

private:
	template<typename T, class U>
	struct less_second {
		typedef pair<T, U> t;
		bool operator()(t const& a, t const& b)
		{
			return a.second.second > b.second.second;
		}
	};

	vector<pair<DWORD, pair<DWORD, FLOAT>>> m_mapItems;
};
#endif //__GIFTBOX_LIST_ITEMS