#include "GlobalContext.h"
#include "Resource/FileSystem.h"

namespace Sherphy 
{
	MiracleGlobalContext g_miracle_global_context;
	void MiracleGlobalContext::startSystem() 
	{
		m_file_system = std::make_shared<FileSystem>();
	}

	void MiracleGlobalContext::shutdownSystem() 
	{
		m_file_system.reset();
	}
}