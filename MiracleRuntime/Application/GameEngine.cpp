#include "GameEngine.h"
#include "World/WorldDataBase.h"
#include "JadeBreaker/RHI/vulkan_rhi.h"
#include "Resource/SceneLoader.h"
#include "JadeBreaker/RHI/vulkan_rhi.h"
#include "Soul/GlobalContext/GlobalContext.h"

namespace Sherphy 
{
	GameEngine::GameEngine() {
		m_world_data = new WorldDataBase;
	}


	void GameEngine::init() 
	{
		m_world_data->addOne();
		SceneLoader::LoadScene(m_world_data->getSceneAt(0));
		g_miracle_global_context.startSystem();
		g_miracle_global_context.m_rendering_system->initWindow();
		swapData();
		g_miracle_global_context.m_rendering_system->initVulkan();
		return;
	}

	void GameEngine::start() 
	{
		std::shared_ptr<VulkanRHI> renderning_system = g_miracle_global_context.m_rendering_system;
		while (renderning_system->shouldClose())
		{
			swapData();
			glfwPollEvents();
			renderning_system->drawFrame();
		}
	}

	void GameEngine::swapData() 
	{
		if (m_is_new_world) 
		{
			SceneID id = 0;
			auto data_base = m_world_data->getSceneAt(id);
			auto* ren_comp = Function::GetObjectComponent<RenderMeshComponent>(id, data_base->getComponentDataBase(ComponentType::rendermesh));

			std::vector<VkVertex>& vertices_buffer = g_miracle_global_context.m_rendering_system->getVerticesWrite();
			std::vector<uint32_t>& indices_buffer = g_miracle_global_context.m_rendering_system->getIndicesWrite();
			vertices_buffer.clear();
			for (Vertex vert : ren_comp->m_vertices) 
			{
				vertices_buffer.emplace_back(static_cast<VkVertex>(vert));
			}
			indices_buffer.clear();
			for (uint32_t index : ren_comp->m_indices) {
				indices_buffer.emplace_back(index);
			}
			m_is_new_world = false;
		}
		return;
	}

	void GameEngine::shutdown() 
	{
		delete m_world_data;
	}
}