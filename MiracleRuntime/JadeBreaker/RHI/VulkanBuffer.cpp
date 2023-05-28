#include "VulkanBuffer.h"
#include "Soul/PreCompile/SoulGlobal.h"

#include <volk.h>

namespace Sherphy
{

	void VulkanBuffer::destroy()
	{
		if (buffer != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(device, buffer, nullptr);
		}
		if (memory != VK_NULL_HANDLE)
		{
			vkFreeMemory(device, memory, nullptr);
		}
	}

	// this function make to sync with device and instance
	VkResult VulkanBuffer::flush(VkDeviceSize size, VkDeviceSize offset)
	{
		VkMappedMemoryRange mapped_range = {};
		mapped_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mapped_range.memory = memory;
		mapped_range.offset = offset;
		mapped_range.size = size;
		return vkFlushMappedMemoryRanges(device, 1, &mapped_range);
	}

	VkResult VulkanBuffer::map(VkDeviceSize size, VkDeviceSize offset)
	{
		return vkMapMemory(device, memory, offset, size, 0, &mapped);
	}

	void VulkanBuffer::unmap()
	{
		if (mapped)
		{
			vkUnmapMemory(device, memory);
			mapped = nullptr;
		}
	}

	VkResult VulkanBuffer::bind(VkDeviceSize offset)
	{
		return vkBindBufferMemory(device, buffer, memory, offset);
	}

	void VulkanBuffer::copyTo(void* data, VkDeviceSize size)
	{
		assert(mapped);
		memcpy(mapped, data, size);
	}
}