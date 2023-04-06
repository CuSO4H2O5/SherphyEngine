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
		VulkanRHI* m_rendering_device;
		WorldDataBase* m_world_data;
	};

}