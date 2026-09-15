#ifndef __BADGE_H
#define __BADGE_H

#ifdef __BADGE
struct BadgeData
{
	int ID;
	int Req;
	int Condition;
	int Amount;
	CString Icon;
#ifdef __AEGON_APNG
	int Animated;
#endif //__AEGON_APNG
	CString Name;
};

class CBadge
{
public:
	CBadge();
	virtual ~CBadge();
	static CBadge* GetInstance()
	{
		static CBadge b;
		return &b;
	}
	bool LoadBadge();

#ifdef __CLIENT
	CString GetName(int ID);
	CString GetIcon(int ID);
#endif //__CLIENT

	const bool IsOwned(CMover* pMover, int ID) const;
	const void AddOwned(CMover* pMover, int ID);
	const void AddOwned(CMover* pMover, vector<BadgeData> v) const;
	const void AddOwned(CMover* pMover, BadgeData data);
	const bool RemoveOwned(CMover* pMover, int ID);

	const BadgeData* GetBadge(int ID);
	const bool GetBadgeData(int ID, int* Req, int* Cond, int* Am, CString* Name) const;

	inline map<int, BadgeData>& GetBadgesMap() { return m_Badges; }

protected:
	map<int, BadgeData> m_Badges;

};

#endif //__BADGE

#endif //__BADGE_H
