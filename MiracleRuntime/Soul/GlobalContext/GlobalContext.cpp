#include "GlobalContext.h"
#include "Resource/FileSystem.h"
#include "JadeBreaker/Display/GLFWDisplay.h"
#include "JadeBreaker/RHI/VulkanRHI.h"

namespace Sherphy 
{
	MiracleGlobalContext g_miracle_global_context;
	void MiracleGlobalContext::startSystem() 
	{
		m_file_system = std::make_shared<FileSystem>();
		m_display_system = std::make_shared<GLFWDisplay>();
		m_rendering_system = std::make_shared<VulkanRHI>();
	}

	void MiracleGlobalContext::shutdownSystem() 
	{
		m_file_system.reset();
		m_rendering_system->cleanUp();
		m_rendering_system.reset();
		m_display_system->distroy();
		m_display_system.reset();
	}
}