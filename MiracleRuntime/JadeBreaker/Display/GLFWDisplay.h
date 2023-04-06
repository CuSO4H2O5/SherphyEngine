#pragma once
#include "Soul/PreCompile/SoulGlobal.h"

#if defined(VulkanBackEnd)
#define GLFW_INCLUDE_VULKAN
#else 
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

namespace Sherphy 
{
	enum class BackEnd 
	{
		Vulkan,
		DirectX12
	};


	class GLFWDisplay
	{
		public :
			void init(uint32_t width, uint32_t height);
			void distroy();
			void tick();
		private:
			GLFWwindow* m_window;
#if defined(DirectX12BackEnd)
			BackEnd m_backend{ BackEnd::DirectX12 };
#else
			BackEnd m_backend{ BackEnd::Vulkan };
#endif
			
	};
}