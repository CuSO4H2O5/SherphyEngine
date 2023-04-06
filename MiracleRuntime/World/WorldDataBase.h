#pragma once

#include "Scene.h"

namespace Sherphy 
{
	using SceneID = size_t;
	class WorldDataBase
	{
	public:
		void addOne() {
			NormalScene* scene = new NormalScene;
			m_scenes.push_back(scene);
			return;
		}
		NormalScene* getSceneAt(SceneID id) {
			return m_scenes[id];
		}
		SceneID countScene() 
		{
			return static_cast<SceneID>(m_scenes.size());
		}
	private:
		std::vector<NormalScene*> m_scenes;
	};
	namespace Function {
		template<typename T>
		T* GetObjectComponent(SOBJ_ID obj_id, std::unordered_map<SOBJ_ID, Component*>* data_base);
		Camera* GetMainCamera(WorldDataBase& database);
		void GetWorldAllVertex(WorldDataBase& database);
	}
}