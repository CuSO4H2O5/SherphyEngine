#include "VulkanDevice.h"
#include "VulkanInitializer.h"

#include <volk.h>
#include <set>

namespace Sherphy 
{
    void VulkanDevice::createLogicalDevice(VkSurfaceKHR surface, VkPhysicalDeviceFeatures enabled_features, const std::vector<const char*>& enabled_extensions, void* pNext_chain, bool use_swap_chain)
    {
        m_queue_family_indices = findQueueFamilies(m_physical_device, surface);

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        std::set<uint32_t> unique_queue_families = { m_queue_family_indices.graphics_family.value(), m_queue_family_indices.present_family.value() };

        for (uint32_t queue_family : unique_queue_families)
        {
            VkDeviceQueueCreateInfo queue_create_info{};
            queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = m_queue_family_indices.graphics_family.value();
            queue_create_info.queueCount = 1;
            queue_create_info.pQueuePriorities = &m_queue_priority;
            queue_create_infos.push_back(queue_create_info);
        }

        std::vector<const char*> device_extensions(enabled_extensions);
        if (use_swap_chain)
        {
            device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        }

        enabled_features.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
        create_info.pQueueCreateInfos = queue_create_infos.data();
        create_info.pEnabledFeatures = &enabled_features;
        create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
        create_info.ppEnabledExtensionNames = device_extensions.data();

        VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{};
        if (pNext_chain) {
            physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            physicalDeviceFeatures2.features = enabled_features;
            physicalDeviceFeatures2.pNext = pNext_chain;
            create_info.pEnabledFeatures = nullptr;//must set to nullptr when set feature2
            create_info.pNext = &physicalDeviceFeatures2;
        }


        SHERPHY_EXCEPTION_IF_FALSE((vkCreateDevice(m_physical_device, &create_info, nullptr, &m_logical_device) == VK_SUCCESS), "faild to create logical device");
        m_command_pool = createCommandPool();
    }


    QueueFamilyIndices VulkanDevice::findQueueFamilies(VkPhysicalDevice& device, VkSurfaceKHR surface)
    {
        QueueFamilyIndices indices;
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
        SHERPHY_ASSERT(queue_family_count > 0, true, "Device Queue Family Properties is null");
        m_device_queue_families.resize(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, m_device_queue_families.data());

        VkBool32 present_support = false;
        for (size_t i = 0; i < m_device_queue_families.size(); i++)
        {
            if (m_device_queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphics_family = i;
            }

            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
            if (present_support)
            {
                indices.present_family = i;
            }

            if (indices.isComplete())
            {
                break;
            }
        }

        return indices;
    }

    VkResult VulkanDevice::createBuffer(VkDeviceSize size,
                                    VkBufferUsageFlags usage,
                                    VkMemoryPropertyFlags properties,
                                    VulkanBuffer& buffer, void* data)
    {
        buffer.device = m_logical_device;
        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = size;
        buffer_info.usage = usage;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        SHERPHY_EXCEPTION_IF_FALSE(vkCreateBuffer(m_logical_device, &buffer_info, nullptr, &buffer.buffer) == VK_SUCCESS, "failed to create vertex buffer!");

        VkMemoryRequirements mem_requirements;
        vkGetBufferMemoryRequirements(m_logical_device, buffer.buffer, &mem_requirements);

        VkMemoryAllocateInfo alloc_info = vki::memoryAllocateInfo();
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = findMemoryType(mem_requirements.memoryTypeBits, properties);
        // If the buffer has VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT set we also need to enable the appropriate flag during allocation
        VkMemoryAllocateFlagsInfoKHR alloc_flags_info{};
        if (usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
            alloc_flags_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO_KHR;
            alloc_flags_info.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
            alloc_info.pNext = &alloc_flags_info;
        }

        SHERPHY_EXCEPTION_IF_FALSE(vkAllocateMemory(m_logical_device, &alloc_info, nullptr, &buffer.memory) == VK_SUCCESS, "failed to allocate vertex buffer memory!");

        if (data != nullptr)
        {
            SHERPHY_ASSERT(buffer.map(), VK_SUCCESS, "");
            memcpy(buffer.mapped, data, size);
            if ((properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
                buffer.flush();

            buffer.unmap();
        }
        return buffer.bind();
    }

    VkCommandBuffer VulkanDevice::createCommandBuffer(VkCommandBufferLevel level, bool begin, VkCommandBufferUsageFlags flags)
    {
        VkCommandBufferAllocateInfo alloc_info = vki::commandBufferAllocateInfo(m_command_pool, level, 1);

        VkCommandBuffer command_buffer;
        vkAllocateCommandBuffers(m_logical_device, &alloc_info, &command_buffer);

        if (begin) 
        {
            VkCommandBufferBeginInfo begin_info = vki::commandBufferBeginInfo();
            begin_info.flags = flags;
            vkBeginCommandBuffer(command_buffer, &begin_info);
        }

        return command_buffer;
    }

    void VulkanDevice::createCommandBuffers(uint32_t frame_count)
    {
        m_command_buffers.resize(frame_count);
        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = m_command_pool;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = static_cast<uint32_t>(m_command_buffers.size());

        SHERPHY_EXCEPTION_IF_FALSE(vkAllocateCommandBuffers(m_logical_device, &alloc_info, m_command_buffers.data()) == VK_SUCCESS, "failed to allocate command buffers!");
    }

    VkCommandPool VulkanDevice::createCommandPool()
    {
        VkCommandPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_info.queueFamilyIndex = m_queue_family_indices.graphics_family.value();

        VkCommandPool pool;
        SHERPHY_EXCEPTION_IF_FALSE(vkCreateCommandPool(m_logical_device, &pool_info, nullptr, &pool) == VK_SUCCESS, "failed to create command pool!");
        return pool;

    }

    uint32_t VulkanDevice::findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties)
    {
        for (uint32_t i = 0; i < m_physical_device_memory_properties.memoryTypeCount; i++) {
            if (type_filter & (1 << i) && (m_physical_device_memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        SHERPHY_EXCEPTION_IF_FALSE(false, "failed to find suitable memory type!");
    }

    void VulkanDevice::getPhysicalDeviceProperties()
    {
        SHERPHY_EXCEPTION_IF_FALSE(m_physical_device, "Did not detected Proper Physical Device");
        vkGetPhysicalDeviceProperties(m_physical_device, &m_physical_device_properties);
        vkGetPhysicalDeviceFeatures(m_physical_device, &m_physical_device_features);
        vkGetPhysicalDeviceMemoryProperties(m_physical_device, &m_physical_device_memory_properties);
        return;
    }

    VkCommandBuffer VulkanDevice::beginSingleTimeCommands()
    {
        return createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, true);
    }

    void VulkanDevice::endSingleTimeCommands(VkCommandBuffer command_buffer, VkQueue queue)
    {
        vkEndCommandBuffer(command_buffer);

        VkSubmitInfo submit_info = vki::submitInfo();
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer;

        VkFenceCreateInfo fence_info = vki::fenceCreateInfo(VK_FLAGS_NONE);
        VkFence fence;
        SHERPHY_ASSERT(vkCreateFence(m_logical_device, &fence_info, nullptr, &fence), VK_SUCCESS, "Vk Single Time Command Fence Creation Failed");
        SHERPHY_ASSERT(vkQueueSubmit(queue, 1, &submit_info, fence), VK_SUCCESS, "Queue Submit Failure");
        SHERPHY_ASSERT(vkWaitForFences(m_logical_device, 1, &fence, VK_TRUE, UINT64_MAX), VK_SUCCESS, "Fence Wait Time Out");

        vkDestroyFence(m_logical_device, fence, nullptr);

        vkFreeCommandBuffers(m_logical_device, m_command_pool, 1, &command_buffer);
    }

    uint64_t VulkanDevice::getBufferDeviceAddress(VulkanBuffer buffer)
    {
        buffer.buffer_device_address = getBufferDeviceAddress(buffer.buffer);
        return buffer.buffer_device_address;
    }

    uint64_t VulkanDevice::getBufferDeviceAddress(VkBuffer buffer)
    {
        VkBufferDeviceAddressInfoKHR buffer_device_AI{};
        buffer_device_AI.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        buffer_device_AI.buffer = buffer;
        return vkGetBufferDeviceAddressKHR(m_logical_device, &buffer_device_AI);
    }
}