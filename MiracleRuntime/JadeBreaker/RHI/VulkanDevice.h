#pragma once
#include "VulkanBuffer.h"
#include <vulkan/vulkan.h>

#include <vector>
#include <optional>

namespace Sherphy 
{
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphics_family;
		std::optional<uint32_t> present_family;

		bool isComplete() {
			return graphics_family.has_value() && present_family.has_value();
		}
	};

	struct VulkanDevice 
	{
		//picked physical device
		VkPhysicalDevice m_physical_device;
		VkDevice m_logical_device;
		//------------------ Command Buffer ----------------------------------
		VkCommandPool m_command_pool;
		std::vector<VkCommandBuffer> m_command_buffers;
		//queue properties
		float m_queue_priority = 1.0f;
		QueueFamilyIndices m_queue_family_indices;
		std::vector<VkQueueFamilyProperties> m_device_queue_families;


		// physical device have this properties
		VkPhysicalDeviceProperties m_physical_device_properties;
		VkPhysicalDeviceFeatures m_physical_device_features;
		VkPhysicalDeviceMemoryProperties m_physical_device_memory_properties;
		// logical device want this properties
		VkPhysicalDeviceFeatures m_enabled_features;//m_device_features


		// device complete functions
		void getPhysicalDeviceProperties();
		void createLogicalDevice(VkSurfaceKHR surface, 
								 VkPhysicalDeviceFeatures enabled_features,
								 const std::vector<const char*>& enabled_extensions,
								 void* pNext_chain, bool use_swap_chain = true);
		VkCommandBuffer createCommandBuffer(VkCommandBufferLevel level, bool begin = false, VkCommandBufferUsageFlags flags = 0);
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue queue);

		void createCommandBuffers(uint32_t frame_count);
		VkCommandPool createCommandPool();
		uint32_t findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice& physical_device, VkSurfaceKHR surface);
		VkResult createBuffer(VkDeviceSize size,
						  VkBufferUsageFlags usage,
						  VkMemoryPropertyFlags properties,
						  VulkanBuffer& buffer, void* data = nullptr);
		uint64_t getBufferDeviceAddress(VkBuffer buffer);
		uint64_t getBufferDeviceAddress(VulkanBuffer buffer);
	};
}