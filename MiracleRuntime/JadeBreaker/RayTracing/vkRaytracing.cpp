#include "vkRaytracing.h"
#include "JadeBreaker/RHI/vulkan_rhi.h"

namespace Sherphy 
{
	struct RayTracingScratchBuffer
	{
		uint64_t deviceAddress = 0;
		VkBuffer handle = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
	};

	// Ray tracing acceleration structure
	struct AccelerationStructure {
		VkAccelerationStructureKHR handle;
		uint64_t deviceAddress = 0;
		VkDeviceMemory memory;
		VkBuffer buffer;
	};


	class RayTracing 
	{


	};
}