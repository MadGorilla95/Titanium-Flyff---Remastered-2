///////////
// This file is a part of the ATools project
// Some parts of code are the property of Microsoft, Qt or Aeonsoft
// The rest is released without license and without any warranty
///////////

#include "stdafx.h"
#include "MainFrame.h"
#include "ModelViewer.h"
#include <AnimatedMesh.h>
#include <Object3D.h>

void CMainFrame::ChangeCW()
{
	if (!m_mesh)
		return;

	QVector<GMObject*> objs;

	CObject3D* obj3D;
	int i, j, k, bufferSize;
	void* data;

	for (i = 0; i < MAX_ANIMATED_ELEMENTS; i++)
	{
		if (m_mesh->m_elements[i].obj)
		{
			obj3D = m_mesh->m_elements[i].obj;
			if (obj3D)
			{
				for (j = 0; j < (obj3D->m_LOD ? MAX_GROUP : 1); j++)
					for (k = 0; k < obj3D->m_groups[j].objectCount; k++)
						objs.push_back(&obj3D->m_groups[j].objects[k]);

				if (obj3D->m_collObj.type != GMT_ERROR)
					objs.push_back(&obj3D->m_collObj);
			}
		}
	}

	GMObject* obj;
	for (i = 0; i < objs.size(); i++)
	{
		obj = objs[i];

		for (j = 0; j < obj->indexCount / 3; j++)
			std::swap(obj->indices[j * 3], obj->indices[j * 3 + 2]);

		if (obj->IB)
		{
			bufferSize = sizeof(ushort) * obj->indexCount;
			if (SUCCEEDED(obj->IB->Lock(0, bufferSize, &data, 0)))
			{
				memcpy(data, obj->indices, bufferSize);
				obj->IB->Unlock();
			}
		}
	}

	if (!m_modelViewer->IsAutoRefresh())
		m_modelViewer->RenderEnvironment();
}

void CMainFrame::SetOverallEffect(int effect)
{
	if (!m_mesh)
		return;

	CObject3D* obj3D;

	for (int m = 0; m < MAX_ANIMATED_ELEMENTS; m++)
	{
		if (m_mesh->m_elements[m].obj)
		{
			obj3D = m_mesh->m_elements[m].obj;
			if (obj3D)
			{
				for (int i = 0; i < (obj3D->m_LOD ? MAX_GROUP : 1); i++)
				{
					for (int j = 0; j < obj3D->m_groups[i].objectCount; j++)
					{
						for (int k = 0; k < obj3D->m_groups[i].objects[j].materialBlockCount; k++)
						{
							MaterialBlock* editing = &obj3D->m_groups[i].objects[j].materialBlocks[k];
							if (effect == XE_OPACITY)
								editing->amount = 255;
							editing->effect |= effect;
						}
					}
				}
			}
		}
	}

	if (!m_modelViewer->IsAutoRefresh())
		m_modelViewer->RenderEnvironment();
}