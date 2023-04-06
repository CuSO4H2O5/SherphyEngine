#include "WorldDataBase.h"


namespace Sherphy 
{
	namespace Function 
	{
		template<typename T>
		T* GetObjectComponent(SOBJ_ID obj_id, std::unordered_map<SOBJ_ID, Component*>* data_base)
		{
			return static_cast<T*>((*data_base)[obj_id]);
		}

		//TODO complete
		void GetWorldAllVertex(WorldDataBase& database) 
		{
			SceneID scene_count = database.countScene();
			for (SceneID id = 0; id < scene_count; id++)
			{
				NormalScene* scene = database.getSceneAt(id);
				auto& ref_scene = *scene;
				auto views = ref_scene.view(ComponentType::position, ComponentType::rotation, ComponentType::rendermesh);
				const auto& objects = std::get<0>(views);
				const auto& components  = std::get<1>(views);

				for (auto object_id : objects) 
				{
					auto* obj_pos = GetObjectComponent<PositionComponent>(object_id, components[0]);
					auto* obj_rot = GetObjectComponent<RotationComponent>(object_id, components[1]);
					auto* obj_ren = GetObjectComponent<RenderMeshComponent>(object_id, components[2]);
				}
			}
			return ;
		}

		Camera* GetMainCamera(WorldDataBase& database) 
		{
			Camera* camera;
			SceneID scene_count = database.countScene();
			for (SceneID id = 0; id < scene_count; id++) 
			{
				NormalScene* scene = database.getSceneAt(id);
				camera = (* scene).pickMainCamera();
				if ((camera == nullptr) == false)
				{
					break;
				}
			}
			SHERPHY_EXCEPTION_IF_FALSE((camera == nullptr) == false, "No Main Camera");
			return camera;
		}


	}

}