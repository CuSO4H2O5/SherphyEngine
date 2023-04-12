#pragma once

namespace Sherphy 
{
	class VulkanRHI;
	struct WorldDataBase;
	class GameEngine 
	{
	public:
		GameEngine();
		void init();
		void start();
		void shutdown();
	private:
		void swapData();
		bool m_is_new_world { true };
		WorldDataBase* m_world_data;
	};

}