#include "stdafx.h"
#include "Badge.h"
#include "../XML/XmlWrapperBadge.h"

#ifdef __BADGE

CBadge::CBadge()
{
	m_Badges.clear();
}

CBadge::~CBadge()
{
}

bool CBadge::LoadBadge()
{
	CXmlWrapper xDoc;
	bool Success = false;
	if (xDoc.Load("Badge.xml"))
	{
		bool found;
		CXmlNodeWrapper xRoot = xDoc.AsNode();
		CXmlNodeWrapper xBadgeData = xRoot.SelectNode("BadgeProp", found);
		if (found)
		{
			Success = true;
			vector<CXmlNodeWrapper> vecBadge = xBadgeData.SelectNodes("Badge");
			for (auto xBadge : vecBadge)
			{
				BadgeData badge;
				badge.ID = xBadge.GetIntVal("ID");
				badge.Req = xBadge.GetUIntVal("Req");
				badge.Condition = xBadge.GetUIntVal("Condition");
				badge.Amount = xBadge.GetIntVal("Amount");
#ifdef __CLIENT
				badge.Icon = xBadge.GetStringVal("Icon", "", false).c_str();
#ifdef __AEGON_APNG
				badge.Animated = xBadge.GetUIntVal("Animated");
#endif //__AEGON_APNG
				badge.Name = xBadge.GetStringVal("Name").c_str();
#endif //__CLIENT
				m_Badges.insert(make_pair(badge.ID, badge));
			}
		}
	}
	return Success;
}

const bool CBadge::IsOwned(CMover* pMover, int ID) const
{
	return std::find(pMover->GetOwnedBadges().begin(), pMover->GetOwnedBadges().end(), ID) != pMover->GetOwnedBadges().end();
}

const void CBadge::AddOwned(CMover* pMover, int ID)
{
	auto it = m_Badges.find(ID);
	if (it != m_Badges.end())
		pMover->GetOwnedBadges().push_back(it->first);
	else
		Error("CBadge::AddOwned");
}

const bool CBadge::RemoveOwned(CMover* pMover, int ID)
{
	for (auto it = pMover->GetOwnedBadges().begin(); it != pMover->GetOwnedBadges().end(); ++it)
	{
		if (*it == ID)
		{
			pMover->GetOwnedBadges().erase(it);
			return true;
		}
	}
	return false;
}

const BadgeData* CBadge::GetBadge(int ID)
{
	for (auto it = m_Badges.begin(); it != m_Badges.end(); ++it)
		if (it->first == ID)
			return &it->second;
	return nullptr;
}

#ifdef __CLIENT
CString CBadge::GetName(int ID)
{
	for (auto it = m_Badges.begin(); it != m_Badges.end(); ++it)
		if (it->second.ID == ID)
			return it->second.Name;
	return "";
}

CString CBadge::GetIcon(int ID)
{
	for (auto it = m_Badges.begin(); it != m_Badges.end(); ++it)
		if (it->second.ID == ID)
			return it->second.Icon;
	return "";
}
#endif //__CLIENT

#endif //__BADGE
