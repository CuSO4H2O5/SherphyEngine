#include "GameEngine.h"
#include "World/WorldDataBase.h"
#include "JadeBreaker/RHI/vulkan_rhi.h"
#include "Resource/SceneLoader.h"
#include "Soul/GlobalContext/GlobalContext.h"

namespace Sherphy 
{
	GameEngine::GameEngine() {
		m_rendering_device = new VulkanRHI;
		m_world_data = new WorldDataBase;
	}


	void GameEngine::init() 
	{
		m_world_data->addOne();
		SceneLoader::LoadScene(m_world_data->getSceneAt(0));
		g_miracle_global_context.startSystem();
		m_rendering_device->initWindow();
		m_rendering_device->initVulkan();
		return;
	}

	void GameEngine::start() 
	{
		while (m_rendering_device->shouldClose())
		{
			swapData();
			glfwPollEvents();
			m_rendering_device->drawFrame();
		}
	}

	void GameEngine::swapData() {
		auto data_base = m_world_data->getSceneAt(0);
		Function::GetObjectComponent<RenderMeshComponent>(0, data_base->getComponentDataBase(ComponentType::rendermesh));
	}

	void GameEngine::shutdown() 
	{
		m_rendering_device->cleanUp();
	}
}