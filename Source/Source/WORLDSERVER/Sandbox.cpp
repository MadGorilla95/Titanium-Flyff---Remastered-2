#include "stdafx.h"

#ifdef __SYS_SANDBOX
#include "WorldMng.h"
#include "LinkMap.h"
extern	CWorldMng	g_WorldMng;

CSandbox::CSandbox()
{
	m_vecSpawn = D3DXVECTOR3(558.763F, 100.481F, 570.177F);
}
CSandbox::~CSandbox()
{
}

CSandbox* CSandbox::GetInstance(void)
{
	static CSandbox s_Sandbox;
	return &s_Sandbox;
}

SANDBOX* CSandbox::GetLayerMap(int nLayer)
{
	auto layer = m_aLayermap.find(nLayer);
	if (layer != m_aLayermap.end())
		return &layer->second;

	return nullptr;
}

void CSandbox::EnterSandbox(CMover* pMover, D3DXVECTOR3 vecPos)
{
	if (!IsValidObj(pMover))
		return;

	CWorld* pWorld = g_WorldMng.GetWorld( WORLD_SANDBOX );
	if (!pWorld)
		return;
	int nLayer = (int)pMover->m_idPlayer;
	if (pMover->m_idparty > 0)
		nLayer = INT_MAX - (int)pMover->m_idparty;

	InsertQueue(nLayer, pMover);
	if (pMover->m_idparty > 0)
	{
		if (GetLayerMap(nLayer))
		{
			pMover->Replace(g_uIdofMulti, WORLD_SANDBOX, vecPos, REPLACE_NORMAL, nLayer);
			InsertMover(nLayer, pMover);
		}
		else
		{
			if (CreateLayer(nLayer))
			{
				pMover->Replace(g_uIdofMulti, WORLD_SANDBOX, vecPos, REPLACE_NORMAL, nLayer);
				InsertMover(nLayer, pMover);
			}
		}
	}
	else
	{
		if (GetLayerMap(nLayer) || CreateLayer(nLayer))
		{
			pMover->Replace(g_uIdofMulti, WORLD_SANDBOX, vecPos, REPLACE_NORMAL, nLayer);
			InsertMover(nLayer, pMover);
		}
	}
}

void CSandbox::InsertQueue(int nLayer, CMover* pMover)
{
	if (!IsValidObj(pMover))
		return;
	BOOL bNew = TRUE;
	for (auto queue = m_vecQueue.begin(); queue != m_vecQueue.end(); queue++)
	{
		if (queue->m_nLayer == nLayer)
		{
			queue->m_vecPlayer.push_back(pMover->m_idPlayer);
			bNew = FALSE;
		}
	}
	if (bNew)
	{
		QUEUELIST queuelist = QUEUELIST();
		queuelist.m_vecPlayer.push_back(pMover->m_idPlayer);
		queuelist.m_dwWaitTime = GetTickCount();
		queuelist.m_nLayer = nLayer;
		m_vecQueue.push_back(queuelist);
	}
}

void CSandbox::DeleteQueue(int nLayer, CMover* pMover)
{
	if (!IsValidObj(pMover))
		return;

	auto queue = m_vecQueue.begin();
	for (; queue != m_vecQueue.end(); queue++)
	{
		if (queue->m_nLayer == nLayer)
			if (std::find(queue->m_vecPlayer.begin(), queue->m_vecPlayer.end(), pMover->m_idPlayer) != queue->m_vecPlayer.end())
				break;
	}
	m_vecQueue.erase(queue);
}

void CSandbox::InsertMover(int nLayer, CMover* pMover)
{
	if (!IsValidObj(pMover))
		return;
	SANDBOX* pSandbox = GetLayerMap(nLayer);
	if (pSandbox)
	{
		pSandbox->m_vecPlayer.push_back(pMover->m_idPlayer);
	}
}

void CSandbox::DeleteMover(int nLayer, CMover* pMover)
{
	if (!IsValidObj(pMover))
		return;
	SANDBOX* pSandbox = GetLayerMap(nLayer);
	if (pSandbox)
	{
		if (pMover->IsPlayer())
		{
			auto player = std::find(pSandbox->m_vecPlayer.begin(), pSandbox->m_vecPlayer.end(), pMover->m_idPlayer);
			if (player != pSandbox->m_vecPlayer.end())
			{
				pSandbox->m_vecPlayer.erase(player);
			}
		}
		else if (pMover == pSandbox->pMonster)
		{
			pSandbox->pMonster = nullptr;
		}
	}
}

BOOL CSandbox::CreateLayer(int nLayer)
{
	CWorld* pWorld = g_WorldMng.GetWorld(WORLD_SANDBOX);
	if (!pWorld)
		return FALSE;
	if (GetLayerMap(nLayer))
		return FALSE;
	if ((pWorld->m_linkMap.GetLinkMap(nLayer) && !GetLayerMap(nLayer)) || pWorld->CreateLayer(nLayer))
	{
		SANDBOX sandbox = SANDBOX();
		m_aLayermap.insert(make_pair(nLayer, sandbox));
		return TRUE;
	}
	return FALSE;
}

void CSandbox::DeleteLayer(int nLayer)
{
	CWorld* pWorld = g_WorldMng.GetWorld(WORLD_SANDBOX);
	if (pWorld)
	{
		ClearMonster(nLayer);
		CLinkMap* pLink = pWorld->m_linkMap.GetLinkMap(nLayer);
		if (pLink)
		{
			pLink->Invalidate(TRUE);
			//pWorld->m_linkMap.DeleteLinkMap(nLayer);
		}
	}
}

void CSandbox::SpawnMonster(int nLayer, DWORD dwIndex, BOOL bDamage)
{
	CWorld* pWorld = g_WorldMng.GetWorld(WORLD_SANDBOX);
	SANDBOX* pSandbox = GetLayerMap(nLayer);
	if (pSandbox && !pSandbox->pMonster)
	{
		MoverProp* pMoverProp = nullptr;
		pMoverProp = prj.GetMoverPropEx(dwIndex);
		CSandboxMover* pObj = new CSandboxMover;

		pObj->m_dwType = OT_MOVER;

		if (pObj->SetIndex(D3DDEVICE, pMoverProp->dwID, TRUE) == TRUE)
		{
			pObj->SetMotion(MTI_STAND);
			MoverProp* pProp = pObj->GetProp();

			pObj->m_pActMover->m_fSpeed = pProp->fSpeed;
#if __VER >= 9	//__AI_0509
			if (pProp->dwAI == AII_MONSTER)
			{
				pObj->m_pActMover->m_fSpeed /= 2.0F;
			}
#endif	// __AI_0509

			pObj->SetPos(m_vecSpawn);
			pObj->InitMotion(MTI_STAND);
			pObj->UpdateLocalMatrix();
			pObj->m_bDamage = !bDamage;
			pWorld->ADDOBJ(pObj, TRUE, nLayer);
			pSandbox->pMonster = pObj;
		}
		else
		{
			SAFE_DELETE(pObj);
		}
	}
}

void CSandbox::ClearMonster(int nLayer)
{
	SANDBOX* pSandbox = GetLayerMap(nLayer);
	if (pSandbox)
	{
		if (pSandbox->pMonster)
		{
			//pSandbox->pMonster->SendAIMsg(AIMSG_INIT_MOVERDIE); // Unknown, comment works?
			pSandbox->pMonster->Delete();
			pSandbox->pMonster = nullptr;
		}
	}
}

BOOL CSandbox::ProcessQueueList(QUEUELIST* queue)
{
	int nUser = 0;
	for (auto user = queue->m_vecPlayer.begin(); user != queue->m_vecPlayer.end();)
	{
		CMover* pMover = prj.GetUserByID(*user);
		if (!IsValidObj(pMover) || pMover->IsWorldSandBox())
		{
			nUser++;
			user = queue->m_vecPlayer.erase(user);
		}
		else
			user++;

	}
	return nUser;
}

BOOL CSandbox::Process()
{
	vector<int> ignoreList;
	vector<int> DeleteList;
	for (auto queue = m_vecQueue.begin(); queue != m_vecQueue.end();)
	{
		if (queue->m_dwWaitTime + SEC(5) > GetTickCount() && !ProcessQueueList(&*queue))
		{
			ignoreList.push_back(queue->m_nLayer);
			queue++;
		}
		else
		{
			queue = m_vecQueue.erase(queue);
		}
	}
	for (auto layer = m_aLayermap.begin(); layer != m_aLayermap.end(); layer++)
	{
		if (std::find(ignoreList.begin(), ignoreList.end(), layer->first) != ignoreList.end())
			continue;


		SANDBOX* pSandbox = &layer->second;
		if (pSandbox)
		{
			if (pSandbox->m_vecPlayer.size() > 0)
			{
				if (pSandbox->pMonster && pSandbox->pMonster->IsDie())
				{
					pSandbox->pMonster = nullptr;
				}

				int nUser = 0;
				for (auto user = pSandbox->m_vecPlayer.begin(); user != pSandbox->m_vecPlayer.end(); user++)
				{
					CMover* pMover = prj.GetUserByID(*user);
					if (IsValidObj(pMover) && pMover->IsWorldSandBox() && pMover->GetLayer() == layer->first)
						nUser++;
				}
				if (!nUser)
				{
					if (std::find(DeleteList.begin(), DeleteList.end(), layer->first) == DeleteList.end())
						DeleteList.push_back(layer->first);
					//DeleteLayer(layer->first);
					//layer = m_aLayermap.erase(layer);
				}
			}
			else if (pSandbox->m_vecPlayer.size() == 0)
			{
				if (std::find(DeleteList.begin(), DeleteList.end(), layer->first) == DeleteList.end())
					DeleteList.push_back(layer->first);
			}
		}
	}

	for (auto layer = DeleteList.begin(); layer != DeleteList.end(); layer++)
	{
		DeleteLayer(*layer);
		m_aLayermap.erase(*layer);
	}
	return TRUE;
}
#endif //__SYS_SANDBOX