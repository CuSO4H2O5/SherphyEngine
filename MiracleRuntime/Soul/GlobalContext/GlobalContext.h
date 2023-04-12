#pragma once
#include <memory>

namespace Sherphy 
{
	class FileSystem;
	class VulkanRHI;

	class MiracleGlobalContext 
	{
	public:
		void startSystem();
		void shutdownSystem();
	public:
		std::shared_ptr<FileSystem> m_file_system;
		std::shared_ptr<VulkanRHI> m_rendering_system;
	};
	extern MiracleGlobalContext g_miracle_global_context;
}