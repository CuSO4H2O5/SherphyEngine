#pragma once
#include <vulkan/vulkan.h>
#define VK_FLAGS_NONE 0

namespace Sherphy 
{
	namespace vki 
	{
		inline VkCommandBufferAllocateInfo commandBufferAllocateInfo(
											VkCommandPool commandPool,
											VkCommandBufferLevel level,
											uint32_t bufferCount)
		{
			VkCommandBufferAllocateInfo command_buffer_allocate_info{};
			command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			command_buffer_allocate_info.commandPool = commandPool;
			command_buffer_allocate_info.level = level;
			command_buffer_allocate_info.commandBufferCount = bufferCount;
			return command_buffer_allocate_info;
		}

		inline VkSubmitInfo submitInfo()
		{
			VkSubmitInfo submit_info{};
			submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			return submit_info;
		}

		inline VkFenceCreateInfo fenceCreateInfo(VkFenceCreateFlags flags = 0)
		{
			VkFenceCreateInfo fence_create_info{};
			fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fence_create_info.flags = flags;
			return fence_create_info;
		}

		inline VkMemoryAllocateInfo memoryAllocateInfo()
		{
			VkMemoryAllocateInfo memAllocInfo{};
			memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			return memAllocInfo;
		}

		inline VkCommandBufferBeginInfo commandBufferBeginInfo()
		{
			VkCommandBufferBeginInfo cmdBufferBeginInfo{};
			cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			return cmdBufferBeginInfo;
		}
	}
}