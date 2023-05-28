#include "GameEngine.h"
#include "World/WorldDataBase.h"
#include "JadeBreaker/RHI/VulkanRHI.h"
#include "Resource/SceneLoader.h"
#include "JadeBreaker/Display/GLFWDisplay.h"
#include "Soul/GlobalContext/GlobalContext.h"

namespace Sherphy 
{
	GameEngine::GameEngine() {
		m_world_data = new WorldDataBase;
	}

	const uint32_t WIDTH = 800, HEIGHT = 600;
	void GameEngine::init() 
	{
		m_world_data->addOne();
		SceneLoader::LoadScene(m_world_data->getSceneAt(0));
		g_miracle_global_context.startSystem();
		g_miracle_global_context.m_display_system->init(WIDTH, HEIGHT);
		swapData();
		g_miracle_global_context.m_rendering_system->initVulkan(PipeLineType::Normal);
		return;
	}

	void GameEngine::start() 
	{
		std::shared_ptr<GLFWDisplay> display_system = g_miracle_global_context.m_display_system;
		std::shared_ptr<VulkanRHI> renderning_system = g_miracle_global_context.m_rendering_system;
		while (display_system->shouldClose())
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
		g_miracle_global_context.shutdownSystem();
		delete m_world_data;
	}
}