#pragma once
#include <vulkan/vulkan.h>

#pragma once
namespace Sherphy 
{
	struct VulkanBuffer 
	{
		VkDevice device;
		VkDeviceAddress buffer_device_address;
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		void* mapped = nullptr;

		void destroy();
		VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		void unmap();
		void copyTo(void* data, VkDeviceSize size);
		VkResult bind(VkDeviceSize offset = 0);
		VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	};
}