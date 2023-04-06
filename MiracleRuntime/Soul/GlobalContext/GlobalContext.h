#pragma once
#include <memory>

namespace Sherphy 
{
	class FileSystem;

	class MiracleGlobalContext 
	{
	public:
		void startSystem();
		void shutdownSystem();
	public:
		std::shared_ptr<FileSystem> m_file_system;
	};
	extern MiracleGlobalContext g_miracle_global_context;
}