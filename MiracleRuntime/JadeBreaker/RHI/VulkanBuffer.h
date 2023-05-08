#pragma once
#include <vulkan/vulkan.h>

namespace Sherphy 
{
	class VulkanBuffer 
	{
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
	};
}