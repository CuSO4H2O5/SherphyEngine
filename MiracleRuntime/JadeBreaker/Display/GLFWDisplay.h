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
			GLFWwindow* getWindow() { return m_window; }
			void getFramebufferSize(int& width, int& height);
			bool shouldClose() { return !glfwWindowShouldClose(m_window);}
			const char** getVkExtensions(uint32_t& count) { return glfwGetRequiredInstanceExtensions(&count);}
			void waitEvents() { glfwWaitEvents(); return; }
			void createWindowSurface(VkInstance instance,
									const VkAllocationCallbacks* allocator,
									VkSurfaceKHR* surface);
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