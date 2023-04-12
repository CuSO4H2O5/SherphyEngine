#pragma once
#include "JadeBreaker/RHI/VulkanBuffer.h"

namespace Sherphy 
{
	class RayTracing 
	{
		PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;
		PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
		PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;
		PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;
		PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
		PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;
		PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR;
		PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;
		PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR;
		PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;

		VkPhysicalDeviceRayTracingPipelinePropertiesKHR  rayTracingPipelineProperties{};
		VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures{};

		VkPhysicalDeviceBufferDeviceAddressFeatures enabledBufferDeviceAddresFeatures{};
		VkPhysicalDeviceRayTracingPipelineFeaturesKHR enabledRayTracingPipelineFeatures{};
		VkPhysicalDeviceAccelerationStructureFeaturesKHR enabledAccelerationStructureFeatures{};

		AccelerationStructure bottomLevelAS{};
		AccelerationStructure topLevelAS{};

		std::vector<VkRayTracingShaderGroupCreateInfoKHR> shaderGroups{};
		VulkanBuffer raygenShaderBindingTable;
		VulkanBuffer missShaderBindingTable;
		VulkanBuffer hitShaderBindingTable;
	};
}