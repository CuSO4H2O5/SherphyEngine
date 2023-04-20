#pragma once
#include <memory>

namespace Sherphy 
{
	class FileSystem;
	class VulkanRHI;
	class GLFWDisplay;

	class MiracleGlobalContext 
	{
	public:
		void startSystem();
		void shutdownSystem();
	public:
		std::shared_ptr<FileSystem> m_file_system;
		std::shared_ptr<GLFWDisplay> m_display_system;
		std::shared_ptr<VulkanRHI> m_rendering_system;
	};
	extern MiracleGlobalContext g_miracle_global_context;
}