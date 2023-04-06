#include "SceneLoader.h"
#include "World/Scene.h"
#include "World/WorldDataBase.h"
#include "Resource/FileSystem.h"
#include "Soul/GlobalContext/GlobalContext.h"

namespace Sherphy 
{
	void SceneLoader::LoadScene(NormalScene* data_base)
	{
		data_base->clear();
		LoadATestScene(data_base);
		return;
	}

	void SceneLoader::LoadATestScene(NormalScene* data_base) 
	{
			SOBJ_ID obj_id = data_base->addOneObject({ ComponentType::position, ComponentType::rotation, ComponentType::rendermesh });
			data_base->addComponent<RenderMeshComponent>(ComponentType::rendermesh, obj_id);
			data_base->addComponent<PositionComponent>(ComponentType::position, obj_id);
			data_base->addComponent<RotationComponent>(ComponentType::rotation, obj_id);

			{
				RenderMeshComponent* ren_comp = 
					Function::GetObjectComponent<RenderMeshComponent>(obj_id, data_base->getComponentDataBase(ComponentType::rendermesh));

				g_miracle_global_context.m_file_system->loadObjFile(ren_comp->m_vertices, ren_comp->m_indices, "I:\\SherphyEngine\\resource\\model\\viking_room.obj");
				PositionComponent* pos_comp = 
					Function::GetObjectComponent<PositionComponent>(obj_id, data_base->getComponentDataBase(ComponentType::position));

				pos_comp->pos = { 0, 0, 0 };
			}
	}
}