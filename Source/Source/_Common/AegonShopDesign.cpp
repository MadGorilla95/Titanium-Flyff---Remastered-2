#include "stdafx.h"
#include "AegonShopDesign.h"
#include "defineObj.h"
#include "defineItem.h"
#include "ResData.h"
#include "d3d9types.h"

#ifdef __CLIENT
#include "DPClient.h"
#include "AppDefine.h"
#include "WndManager.h"
#endif

#ifdef __WORLDSERVER
#include "User.h"
#include "dpdatabaseclient.h"
extern	CDPDatabaseClient	g_dpDBClient;
extern	CUserMng			g_UserMng;
#endif


#ifdef __AEGON_IMPROVED_SHOPDESIGN
CAegonShopDesignMng::CAegonShopDesignMng()
{
	m_bScriptLoaded = LoadScript();
	if (!m_bScriptLoaded)
	{
		m_mapShopDesigns.clear();
		Error("Could not load File: %s", m_scriptFile);
	}
	else
	{
	}
}

CAegonShopDesignMng::~CAegonShopDesignMng()
{
}

CAegonShopDesignMng* CAegonShopDesignMng::GetInstance(void)
{
	static CAegonShopDesignMng sAegonShopDesignMng;
	return &sAegonShopDesignMng;
}

BOOL CAegonShopDesignMng::LoadScript()
{
	CScript s;
	if (!s.Load(m_scriptFile))
		return FALSE;

	DWORD dwItem;
	dwItem = s.GetNumber();
	while (s.tok != FINISHED)
	{
		__AEGON_SHOP_DESIGN tmpDesignUnit;
#ifdef __CLIENT
		s.GetToken();
		memcpy(tmpDesignUnit.szNameTexLeft, s.Token.GetString(), 127);
		s.GetToken();
		memcpy(tmpDesignUnit.szNameTexCenter, s.Token.GetString(), 127);
		s.GetToken();
		memcpy(tmpDesignUnit.szNameTexRight, s.Token.GetString(), 127);
#else 
		s.GetToken();	
		s.GetToken();
		s.GetToken();
#endif
		

		m_mapShopDesigns.insert(make_pair(dwItem, tmpDesignUnit));
		dwItem = s.GetNumber();
	}

	if (m_mapShopDesigns.size() <= 0)
		return FALSE;

	return TRUE;
}

#ifdef __WORLDSERVER
void CAegonShopDesignMng::OnPlayerShopDesignBuffChange(CUser* pUser, BOOL bForceRemove)
{
	if (!IsValidObj(pUser) || !pUser->IsPlayer())
		return;

	if (pUser->m_idVendorNPC == 0)
		return;

	
	CMover* pVendor = prj.GetVendorNpcByID(pUser->m_idVendorNPC);
	if (!pVendor)
		return;

	DWORD dwOld = pVendor->m_dwShopDesign;

	BOOL bDesignFound = FALSE;
	map<int, __AEGON_SHOP_DESIGN>::iterator it = CAegonShopDesignMng::GetInstance()->m_mapShopDesigns.begin();
	for (; it != CAegonShopDesignMng::GetInstance()->m_mapShopDesigns.end(); it++)
	{
		if (pUser->HasBuff(BUFF_ITEM2, it->first)) 
		{
			pVendor->m_dwShopDesign = it->first;
			bDesignFound = TRUE;
			break;
		}
	}

	if(!bDesignFound || bForceRemove)
		pVendor->m_dwShopDesign = 0;


	if (bForceRemove || dwOld != pVendor->m_dwShopDesign) 
	{
		g_dpDBClient.SendUpdateNPCVendor(pVendor);		
		g_UserMng.AddVendorShopDesign(pVendor);
	}
}
#endif

#endif //__AEGON_IMPROVED_SHOPDESIGN