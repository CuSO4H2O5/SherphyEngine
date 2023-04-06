#pragma once
#include "Soul/PreCompile/SoulGlobal.h"
#include "Soul/Object.h"

#include <unordered_map>

namespace Sherphy 
{
	class Scene{};

#define NO_MAIN_CAMERA -1
	class NormalScene : public Scene
	{
	public:
		Camera* pickMainCamera() 
		{
			if (m_main_camera_id == NO_MAIN_CAMERA || m_main_camera_id >= m_cameras.size())
			{
				return nullptr;
			}
			return &m_cameras[m_main_camera_id];
		}

		template <typename... Args>
		std::tuple<std::vector<SOBJ_ID>, std::vector<std::unordered_map<SOBJ_ID, Component*>*>>
			view(ComponentType _typeid, Args... arg)
		{
			std::vector<SOBJ_ID> type_list;
			for(auto& obj : m_scene_objects)
			{
				if ((obj.second.contains(arg)&&...)) 
				{
					type_list.push_back(obj.first);
				}
			}
			std::vector<std::unordered_map<SOBJ_ID, Component*>*> view_data{ &m_data_base[_typeid], &m_data_base[arg]...};
			return std::make_tuple(type_list, view_data);
		}

		std::unordered_map<SOBJ_ID, Component*>* getComponentDataBase(ComponentType _typeid)
		{
			return &m_data_base[_typeid];
		}

		SOBJ_ID addOneObject(const std::unordered_set<ComponentType>& components_type)
		{
			m_scene_objects.insert({ object_conter, components_type});
			return object_conter++;
		}

		template<typename Comp>
		void addComponent(ComponentType component_type, SOBJ_ID id)
		{
			Comp* component = new Comp;
			m_data_base[component_type][id] = component;
			return;
		}

		void clear();
	private:
		std::map<SOBJ_ID, std::unordered_set<ComponentType>> m_scene_objects;
		std::unordered_map<ComponentType, std::unordered_map<SOBJ_ID, Component*>> m_data_base;
		std::vector<Camera> m_cameras;
		int m_main_camera_id{ NO_MAIN_CAMERA };
		SOBJ_ID object_conter = 0;
	};
}