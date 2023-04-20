#include "vulkan_rhi.h"
#include "Soul/Object.h"
#include "Resource/FileSystem.h"
#include "JadeBreaker/Display/GLFWDisplay.h"
#include "Soul/GlobalContext/GlobalContext.h"

#include <string.h>
#include <algorithm>
#include <chrono>
#include <set>

const int MAX_FRAMES_IN_FLIGHT = 2;

namespace Sherphy{
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    void VulkanRHI::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debug_messenger, pAllocator);
        }
    }

    void VulkanRHI::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info) {
        create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        create_info.pfnUserCallback = debugCallback;
    }

    void VulkanRHI::createInstance()
    {
        if (m_enable_validation_layer)
        {
            SHERPHY_EXCEPTION_IF_FALSE(checkValidationLayerSupport(), "validation layers requested, but not avaliable.");
        }

        // Optional
        VkApplicationInfo vk_app_info{};
        vk_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        vk_app_info.pApplicationName = "Hello World";
        vk_app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        vk_app_info.pEngineName = "No Engine";
        vk_app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        vk_app_info.apiVersion = VK_API_VERSION_1_0;

        // Not Optional
        VkInstanceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &vk_app_info;

        std::vector<const char*> extensions = getRequiredExtensions();
        create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        create_info.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if(m_enable_validation_layer)
        {
            create_info.enabledLayerCount = static_cast<uint32_t>(m_validation_layers.size());
            create_info.ppEnabledLayerNames = m_validation_layers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else
        {
            create_info.enabledLayerCount = 0;
            create_info.pNext = nullptr;
        }

        
// VkResult result = ;
        SHERPHY_EXCEPTION_IF_FALSE(vkCreateInstance(&create_info, nullptr, &m_instance) == VK_SUCCESS, "create vk instance failed!\n");

#ifdef SHERPHY_DEBUG
        std::cout << "available extensions:\n";

        for (const char* extension : extensions) {
            std::cout << '\t' << extension << '\n';
        }
#endif

    }

    VkResult VulkanRHI::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void VulkanRHI::setupDebugMessenger() {
        SHERPHY_RETURN_IF_FALSE(m_enable_validation_layer, "No Validation Layer Debug Info");

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        SHERPHY_EXCEPTION_IF_FALSE(CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debug_messenger) == VK_SUCCESS, "failed to set up debug messenger!");
    }

    std::vector<const char*> VulkanRHI::getRequiredExtensions()
    {
        uint32_t display_extensions_count = 0;
        const char** displayVkExtensions = g_miracle_global_context.m_display_system->getVkExtensions(display_extensions_count);;

        std::vector<const char*> extensions(displayVkExtensions, displayVkExtensions + display_extensions_count);

        if(m_enable_validation_layer)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool VulkanRHI::checkValidationLayerSupport()
    {
        uint32_t layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        std::vector<VkLayerProperties> avaliable_layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, avaliable_layers.data());

        for (const char* layer_name : m_validation_layers) {
            bool layer_found = false;

            for (const auto& layerProperties : avaliable_layers) {
                if (strcmp(layer_name, layerProperties.layerName) == 0) {
                    layer_found = true;
                    break;
                }
            }

            if (!layer_found) {
                return false;
            }
        }

        return true;
    }

    void VulkanRHI::initVulkan()
    {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        SHERPHY_EXCEPTION_IF_FALSE(m_physical_device, "Did not detected Proper Physical Device");
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createRenderPass(RenderPassType::Normal);
        createDescriptorSetLayout();
        //std::vector<char> vertex_shader = FileSystem::readBinaryFile("I:/SherphyEngine/resource/public/SherphyShaderLib/SPV/Normal/SimpleTestTriangle_vert.spv");
        //std::vector<char> fragment_shader = FileSystem::readBinaryFile("I:/SherphyEngine/resource/public/SherphyShaderLib/SPV/Normal/SimpleTestTriangle_frag.spv");
        //createGraphicsPipeline(PipeLineType::TriangleTest, vertex_shader, fragment_shader);
        std::vector<char> vertex_shader = g_miracle_global_context.m_file_system->readBinaryFile("I:/SherphyEngine/resource/public/SherphyShaderLib/SPV/Normal/NormalShader_vert.spv");
        std::vector<char> fragment_shader = g_miracle_global_context.m_file_system->readBinaryFile("I:/SherphyEngine/resource/public/SherphyShaderLib/SPV/Normal/NormalColorOutput_frag.spv");
        createGraphicsPipeline(PipeLineType::Normal, vertex_shader, fragment_shader);
        createCommandPool();
        createDepthResources();
        createFrameBuffers();
        createTextureImage();
        createTextureImageView();
        createTextureSampler();
        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
        createCommandBuffer();
        createSyncObjects();
    }

    std::vector<VkVertex>& VulkanRHI::getVerticesWrite()
    {
        return m_vertices;
    }
    std::vector<uint32_t>& VulkanRHI::getIndicesWrite() 
    {
        return m_indices;
    }

    void VulkanRHI::createDescriptorSets() 
    {
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_descriptor_set_layout);
        VkDescriptorSetAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = m_descriptor_pool;
        alloc_info.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        alloc_info.pSetLayouts = layouts.data();

        m_descriptor_sets.resize(MAX_FRAMES_IN_FLIGHT);
        SHERPHY_EXCEPTION_IF_FALSE(vkAllocateDescriptorSets(m_device, &alloc_info, m_descriptor_sets.data()) == VK_SUCCESS, "failed to allocate descriptor sets!");

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            VkDescriptorBufferInfo buffer_info{};
            buffer_info.buffer = m_uniform_buffers[i];
            buffer_info.offset = 0;
            buffer_info.range = sizeof(VkUniformBufferObject);

            VkDescriptorImageInfo image_info{};
            image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            image_info.imageView = m_texture_image_view;
            image_info.sampler = m_sampler;

            std::array<VkWriteDescriptorSet, 2> descriptor_write{};
            descriptor_write[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_write[0].dstSet = m_descriptor_sets[i];
            descriptor_write[0].dstBinding = 0;
            descriptor_write[0].dstArrayElement = 0;
            descriptor_write[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptor_write[0].descriptorCount = 1;
            descriptor_write[0].pBufferInfo = &buffer_info;

            descriptor_write[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_write[1].dstSet = m_descriptor_sets[i];
            descriptor_write[1].dstBinding = 1;
            descriptor_write[1].dstArrayElement = 0;
            descriptor_write[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptor_write[1].descriptorCount = 1;
            descriptor_write[1].pImageInfo = &image_info;

            vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(descriptor_write.size()), descriptor_write.data(), 0, nullptr);
        }
    }

    void VulkanRHI::createDescriptorPool() 
    {
        std::array<VkDescriptorPoolSize, 2> pool_size{};
        pool_size[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_size[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        pool_size[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        pool_size[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = static_cast<uint32_t>(pool_size.size());
        pool_info.pPoolSizes = pool_size.data();
        pool_info.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        SHERPHY_EXCEPTION_IF_FALSE(vkCreateDescriptorPool(m_device, &pool_info, nullptr, &m_descriptor_pool) == VK_SUCCESS, "failed to create descriptor pool!");
    }

    void VulkanRHI::createUniformBuffers()
    {
        VkDeviceSize buffer_size = sizeof(VkUniformBufferObject);

        m_uniform_buffers.resize(MAX_FRAMES_IN_FLIGHT);
        m_uniform_buffers_memory.resize(MAX_FRAMES_IN_FLIGHT);
        m_uniform_buffers_mapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            createBuffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                m_uniform_buffers[i], m_uniform_buffers_memory[i]);

            vkMapMemory(m_device, m_uniform_buffers_memory[i], 0, buffer_size, 0, &m_uniform_buffers_mapped[i]);
        }
    }


    //TODO be control
    void VulkanRHI::updateUniformBuffer(uint32_t current_image)
    {
        static auto start_time = std::chrono::high_resolution_clock::now();

        auto current_time = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

        Vec3 camera_pos = {};

        VkUniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), m_extent.width / (float)m_extent.height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;

        memcpy(m_uniform_buffers_mapped[current_image], &ubo, sizeof(ubo));
    }

    void VulkanRHI::createDescriptorSetLayout() 
    {
        VkDescriptorSetLayoutBinding ubo_layout_binding{};
        ubo_layout_binding.binding = 0;
        ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        ubo_layout_binding.descriptorCount = 1;
        ubo_layout_binding.pImmutableSamplers = nullptr;
        ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding sampler_layout_binding{};
        sampler_layout_binding.binding = 1;
        sampler_layout_binding.descriptorCount = 1;
        sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        sampler_layout_binding.pImmutableSamplers = nullptr;
        sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = { ubo_layout_binding, sampler_layout_binding };
        VkDescriptorSetLayoutCreateInfo layout_info{};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = bindings.size();
        layout_info.pBindings = bindings.data();

        SHERPHY_EXCEPTION_IF_FALSE(vkCreateDescriptorSetLayout(m_device, &layout_info, nullptr, &m_descriptor_set_layout) == VK_SUCCESS,
            "failed to create descriptor set layout!");
    }

    void VulkanRHI::createBuffer(VkDeviceSize size, 
                                           VkBufferUsageFlags usage, 
                                           VkMemoryPropertyFlags properties, 
                                           VkBuffer& buffer, 
                                           VkDeviceMemory& buffer_memory)
    {
        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = size;
        buffer_info.usage = usage;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        SHERPHY_EXCEPTION_IF_FALSE(vkCreateBuffer(m_device, &buffer_info, nullptr, &buffer) == VK_SUCCESS, "failed to create vertex buffer!");

        VkMemoryRequirements mem_requirements;
        vkGetBufferMemoryRequirements(m_device, buffer, &mem_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = findMemoryType(mem_requirements.memoryTypeBits, properties);

        SHERPHY_EXCEPTION_IF_FALSE(vkAllocateMemory(m_device, &alloc_info, nullptr, &buffer_memory) == VK_SUCCESS, "failed to allocate vertex buffer memory!");
        vkBindBufferMemory(m_device, buffer, buffer_memory, 0);
    }


    void VulkanRHI::createVertexBuffer() 
    {
        SHERPHY_EXCEPTION_IF_FALSE(m_vertices.size() != 0, "no vertices input\n");
        VkDeviceSize buffer_size = sizeof(m_vertices[0]) * m_vertices.size();

        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        createBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     staging_buffer, staging_buffer_memory);

        void* data;
        vkMapMemory(m_device, staging_buffer_memory, 0, buffer_size, 0, &data);
        SHERPHY_MEMCPY(data, m_vertices.data(), static_cast<size_t>(buffer_size));
        vkUnmapMemory(m_device, staging_buffer_memory);

        createBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     m_vertex_buffer, m_vertex_buffer_memory);

        copyBufferImmediate(staging_buffer, m_vertex_buffer, buffer_size);

        vkDestroyBuffer(m_device, staging_buffer, nullptr);
        vkFreeMemory(m_device, staging_buffer_memory, nullptr);
    }

    void VulkanRHI::createIndexBuffer() 
    {
        VkDeviceSize buffer_size = sizeof(m_indices[0]) * m_indices.size();

        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        createBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

        void* data;
        vkMapMemory(m_device, staging_buffer_memory, 0, buffer_size, 0, &data);
        memcpy(data, m_indices.data(), (size_t)buffer_size);
        vkUnmapMemory(m_device, staging_buffer_memory);

        createBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_index_buffer, m_index_buffer_memory);

        copyBufferImmediate(staging_buffer, m_index_buffer, buffer_size);

        vkDestroyBuffer(m_device, staging_buffer, nullptr);
        vkFreeMemory(m_device, staging_buffer_memory, nullptr);
    }

    void VulkanRHI::copyBufferImmediate(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
    {
        VkCommandBuffer command_buffer = beginSingleTimeCommands();
        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copyRegion);
        endSingleTimeCommands(command_buffer);
    }

    uint32_t VulkanRHI::findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties mem_properties;
        vkGetPhysicalDeviceMemoryProperties(m_physical_device, &mem_properties);

        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
            if (type_filter & (1 << i) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        SHERPHY_EXCEPTION_IF_FALSE(false, "failed to find suitable memory type!");
    }

    void VulkanRHI::createRenderPass(RenderPassType type) 
    {
        switch (type)
        {
        case Sherphy::RenderPassType::Normal:
            createRenderPassNormal();
            break;
        case Sherphy::RenderPassType::RayTracing:
            //createRenderPassRayTracing();
            break;
        default:
            createRenderPassNormal();
            break;
        }

        return;
    }

    void VulkanRHI::createCommandBuffer() 
    {
        m_command_buffers.resize(MAX_FRAMES_IN_FLIGHT);
        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = m_command_pool;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = static_cast<uint32_t>(m_command_buffers.size());

        SHERPHY_EXCEPTION_IF_FALSE(vkAllocateCommandBuffers(m_device, &alloc_info, m_command_buffers.data()) == VK_SUCCESS, "failed to allocate command buffers!");
    }


    VkCommandBuffer VulkanRHI::beginSingleTimeCommands() 
    {
        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool = m_command_pool;
        alloc_info.commandBufferCount = 1;

        VkCommandBuffer command_buffer;
        vkAllocateCommandBuffers(m_device, &alloc_info, &command_buffer);

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(command_buffer, &begin_info);

        return command_buffer;
    }

    void VulkanRHI::endSingleTimeCommands(VkCommandBuffer command_buffer) 
    {
        vkEndCommandBuffer(command_buffer);

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer;

        vkQueueSubmit(m_graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_graphics_queue);

        vkFreeCommandBuffers(m_device, m_command_pool, 1, &command_buffer);
    }

    void VulkanRHI::recordCommandBuffer(VkCommandBuffer command_buffer, uint32_t image_index) {
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        SHERPHY_EXCEPTION_IF_FALSE(vkBeginCommandBuffer(command_buffer, &begin_info) == VK_SUCCESS, "failed to begin recording command buffer!");

        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = m_render_pass;
        render_pass_info.framebuffer = m_swap_chain_frame_buffers[image_index];
        render_pass_info.renderArea.offset = { 0, 0 };
        render_pass_info.renderArea.extent = m_extent;


        std::array<VkClearValue, 2> clear_values{};
        clear_values[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
        clear_values[1].depthStencil = { 1.0f, 0 };

        //VkClearValue clear_color = { {{1.0f, 1.0f, 1.0f, 1.0f}} };
        render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
        render_pass_info.pClearValues = clear_values.data();
        vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline);
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_extent.width);
        viewport.height = static_cast<float>(m_extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(command_buffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = m_extent;
        vkCmdSetScissor(command_buffer, 0, 1, &scissor);

        VkDeviceSize offsets[] = { 0 };
        //vkCmdBindVertexBuffers(command_buffer, 0, static_cast<uint32_t>(m_vertex_buffers.size()), m_vertex_buffers.data(), offsets);
        vkCmdBindVertexBuffers(command_buffer, 0, 1, &m_vertex_buffer, offsets);
        vkCmdBindIndexBuffer(command_buffer, m_index_buffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, 
            m_pipeline_layout, 0, 1, &m_descriptor_sets[m_current_frame], 0, nullptr);

        vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);
        //vkCmdDraw(command_buffer, static_cast<uint32_t>(m_vertices.size()), 1, 0, 0);//TODO abstract command
        vkCmdEndRenderPass(command_buffer);
        SHERPHY_EXCEPTION_IF_FALSE(vkEndCommandBuffer(command_buffer) == VK_SUCCESS, "failed to record command buffer!");
    }

    void VulkanRHI::createCommandPool() 
    {
        QueueFamilyIndices queue_family_indices = findQueueFamilies(m_physical_device);

        VkCommandPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();

        SHERPHY_EXCEPTION_IF_FALSE(vkCreateCommandPool(m_device, &pool_info, nullptr, &m_command_pool) == VK_SUCCESS, "failed to create command pool!");

    }

    void VulkanRHI::createDepthResources()
    {
        VkFormat depth_format = findDepthFormat();

        createImage(m_extent.width, m_extent.height, depth_format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depth_image, m_depth_image_memory);
        m_depth_image_view = createImageView(m_depth_image, depth_format, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    VkFormat VulkanRHI::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(m_physical_device, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        SHERPHY_EXCEPTION_IF_FALSE(false, "failed to find supported format!");
    }

    VkFormat VulkanRHI::findDepthFormat() {
        return findSupportedFormat(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }

    void VulkanRHI::createTextureImage() 
    {
        int tex_width, tex_height, tex_channels;
        unsigned char* pixels = g_miracle_global_context.m_file_system->readImageFile("I:\\SherphyEngine\\resource\\texture\\viking_room.png", tex_width, tex_height, tex_channels);
        VkDeviceSize image_size = tex_width * tex_height * 4;

        SHERPHY_EXCEPTION_IF_FALSE(pixels, "failed to load texture image!");

        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        createBuffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

        void* data;
        vkMapMemory(m_device, staging_buffer_memory, 0, image_size, 0, &data);
            memcpy(data, pixels, static_cast<size_t>(image_size));
        vkUnmapMemory(m_device, staging_buffer_memory);

        g_miracle_global_context.m_file_system->releaseImageAsset(pixels);

        createImage(tex_width, tex_height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_texture_image, m_texture_image_memory);

        transitionImageLayout(m_texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            copyBufferToImage(staging_buffer, m_texture_image, static_cast<uint32_t>(tex_width), static_cast<uint32_t>(tex_height));
        transitionImageLayout(m_texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(m_device, staging_buffer, nullptr);
        vkFreeMemory(m_device, staging_buffer_memory, nullptr);
    }


    VkImageView VulkanRHI::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags)
    {
        VkImageViewCreateInfo view_info{};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = image;
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = format;
        view_info.subresourceRange.aspectMask = aspect_flags;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        VkImageView image_view;
        SHERPHY_EXCEPTION_IF_FALSE(vkCreateImageView(m_device, &view_info, nullptr, &image_view) == VK_SUCCESS, "failed to create texture image view!");

        return image_view;
    }

    void VulkanRHI::createTextureImageView()
    {
        m_texture_image_view = createImageView(m_texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    void VulkanRHI::createTextureSampler()
    {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(m_physical_device, &properties);

        VkSamplerCreateInfo sampler_info{};
        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter = VK_FILTER_LINEAR;
        sampler_info.minFilter = VK_FILTER_LINEAR;
        sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.anisotropyEnable = VK_TRUE;
        sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_info.unnormalizedCoordinates = VK_FALSE;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        SHERPHY_EXCEPTION_IF_FALSE(vkCreateSampler(m_device, &sampler_info, nullptr, &m_sampler) == VK_SUCCESS, "failed to create texture sampler!");
    }

    void VulkanRHI::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
        VkCommandBuffer command_buffer = beginSingleTimeCommands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = {
            width,
            height,
            1
        };

        vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        endSingleTimeCommands(command_buffer);
    }

    void VulkanRHI::createImage(uint32_t width, 
                                uint32_t height, 
                                VkFormat format, 
                                VkImageTiling tiling, 
                                VkImageUsageFlags usage, 
                                VkMemoryPropertyFlags properties, 
                                VkImage& image, 
                                VkDeviceMemory& image_memory) 
    {
        VkImageCreateInfo image_info{};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D;
        image_info.extent.width = width;
        image_info.extent.height = height;
        image_info.extent.depth = 1;
        image_info.mipLevels = 1;
        image_info.arrayLayers = 1;
        image_info.format = format;
        image_info.tiling = tiling;
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage = usage;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        SHERPHY_EXCEPTION_IF_FALSE(vkCreateImage(m_device, &image_info, nullptr, &image) == VK_SUCCESS, "failed to create image!");

        VkMemoryRequirements mem_requirements;
        vkGetImageMemoryRequirements(m_device, image, &mem_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = findMemoryType(mem_requirements.memoryTypeBits, properties);

        SHERPHY_EXCEPTION_IF_FALSE(vkAllocateMemory(m_device, &alloc_info, nullptr, &image_memory) == VK_SUCCESS, "failed to allocate image memory!")

        vkBindImageMemory(m_device, image, image_memory, 0);
    }


    void VulkanRHI::transitionImageLayout(VkImage image, 
                                          VkFormat format, 
                                          VkImageLayout old_layout, 
                                          VkImageLayout new_layout)
    {
        VkCommandBuffer command_buffer = beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = old_layout;
        barrier.newLayout = new_layout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags source_stage;
        VkPipelineStageFlags destination_stage;

        if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            command_buffer,
            source_stage, destination_stage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        endSingleTimeCommands(command_buffer);
    }

    void VulkanRHI::createFrameBuffers() 
    {
        m_swap_chain_frame_buffers.resize(m_swap_chain_image_views.size());

        for (size_t i = 0; i < m_swap_chain_image_views.size(); i++) {
            std::array<VkImageView, 2> attachments = {
                m_swap_chain_image_views[i],
                m_depth_image_view
            };

            VkFramebufferCreateInfo frame_buffer_info{};
            frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            frame_buffer_info.renderPass = m_render_pass;
            frame_buffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
            frame_buffer_info.pAttachments = attachments.data();
            frame_buffer_info.width = m_extent.width;
            frame_buffer_info.height = m_extent.height;
            frame_buffer_info.layers = 1;
            SHERPHY_EXCEPTION_IF_FALSE(vkCreateFramebuffer(m_device, &frame_buffer_info, nullptr, &m_swap_chain_frame_buffers[i]) == VK_SUCCESS, "failed to create framebuffer!");
        }
    }

    void VulkanRHI::createSurface() {
        g_miracle_global_context.m_display_system->createWindowSurface(m_instance, nullptr, &m_surface);
    }

    void VulkanRHI::pickPhysicalDevice()
    {
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);
        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());

        SHERPHY_EXCEPTION_IF_FALSE(device_count > 0, "Failed to Find Graphic Device with Vulkan Support\n");

        for (VkPhysicalDevice& device : devices)
        {
            SHERPHY_CONTINUE_WITH_LOG(device, "WTF");
            if (isDeviceSuitable(device))
            {
                VkPhysicalDeviceProperties prop;
                vkGetPhysicalDeviceProperties(device, &prop);
                SHERPHY_LOG(prop.deviceName);
                m_physical_device = device;
                break;
            }
        }
    }

    void VulkanRHI::createLogicalDevice()
    {
        QueueFamilyIndices indices = findQueueFamilies(m_physical_device);

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(), indices.present_family.value()};

        for (uint32_t queue_family : unique_queue_families) 
        {
            VkDeviceQueueCreateInfo queue_create_info{};
            queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = indices.graphics_family.value();
            queue_create_info.queueCount = 1;
            queue_create_info.pQueuePriorities = &m_queue_prioity;
            queue_create_infos.push_back(queue_create_info);
        }

        m_device_features.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
        create_info.pQueueCreateInfos = queue_create_infos.data();
        create_info.pEnabledFeatures = &m_device_features;
        create_info.enabledExtensionCount = static_cast<uint32_t>(m_device_extensions.size());
        create_info.ppEnabledExtensionNames = m_device_extensions.data();
        if (m_enable_validation_layer) 
        {
            create_info.enabledLayerCount = static_cast<uint32_t>(m_validation_layers.size());
            create_info.ppEnabledLayerNames = m_validation_layers.data();
        }
        else 
        {
            create_info.enabledLayerCount = 0;
        }
        SHERPHY_EXCEPTION_IF_FALSE((vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device) == VK_SUCCESS), "faild to create logical device")
        vkGetDeviceQueue(m_device, indices.graphics_family.value(), 0, &m_graphics_queue);
        vkGetDeviceQueue(m_device, indices.present_family.value(), 0, &m_present_queue);
    }

    void VulkanRHI::createSwapChain() 
    {
        SwapChainSupportDetails swap_chain_support = querySwapChainSupport(m_physical_device);

        VkSurfaceFormatKHR surface_format = chooseSwapSurfaceFormat(swap_chain_support.formats);
        VkPresentModeKHR present_mode = chooseSwapPresentMode(swap_chain_support.present_modes);
        VkExtent2D extent = chooseSwapExtent(swap_chain_support.capabilities);

        uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
        if (swap_chain_support.capabilities.maxImageCount > 0)
        {
            image_count = std::clamp(image_count, (uint32_t)1, swap_chain_support.capabilities.maxImageCount);
        }

        VkSwapchainCreateInfoKHR create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface = m_surface;

        create_info.minImageCount = image_count;
        create_info.imageFormat = surface_format.format;
        create_info.imageColorSpace = surface_format.colorSpace;
        create_info.imageExtent = extent;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = findQueueFamilies(m_physical_device);
        uint32_t queueFamilyIndices[] = { indices.graphics_family.value(), indices.present_family.value() };

        if (indices.graphics_family != indices.present_family) {
            create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }
        create_info.preTransform = swap_chain_support.capabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.oldSwapchain = VK_NULL_HANDLE;
        create_info.presentMode = present_mode;
        create_info.clipped = VK_TRUE;
        
        SHERPHY_EXCEPTION_IF_FALSE(vkCreateSwapchainKHR(m_device, &create_info, nullptr, &m_swap_chain) == VK_SUCCESS, "failed to create swap chain!");

        vkGetSwapchainImagesKHR(m_device, m_swap_chain, &image_count, nullptr);
        m_swap_chain_images.resize(image_count);
        vkGetSwapchainImagesKHR(m_device, m_swap_chain, &image_count, m_swap_chain_images.data());

        m_swap_chain_image_format = surface_format.format;
        m_present_mode = present_mode;
        m_extent = extent;

        return;
    }

    void VulkanRHI::createImageViews() 
    {
        SHERPHY_EXCEPTION_IF_FALSE((m_swap_chain_images.size()>0), "swap chain image is empty");
        m_swap_chain_image_views.resize(m_swap_chain_images.size());

        for (size_t i = 0; i < m_swap_chain_images.size(); i++) {
            m_swap_chain_image_views[i] = createImageView(m_swap_chain_images[i], m_swap_chain_image_format, VK_IMAGE_ASPECT_COLOR_BIT);
        }
    }

    VkShaderModule VulkanRHI::createShaderModule(const std::vector<char>& code) 
    {
        VkShaderModuleCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = code.size();
        create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shader_module;
        SHERPHY_EXCEPTION_IF_FALSE(vkCreateShaderModule(m_device, &create_info, nullptr, &shader_module) == VK_SUCCESS, "failed to create shader module!");

        return shader_module;
    }

    void VulkanRHI::createRenderPassNormal() 
    {
        VkAttachmentDescription color_attachment{};
        color_attachment.format = m_swap_chain_image_format;
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        //LOWTODO stencil test
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


        VkAttachmentReference color_attachment_ref{};
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depth_attachment_ref{};
        depth_attachment_ref.attachment = 1;
        depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_ref;
        subpass.pDepthStencilAttachment = &depth_attachment_ref;

        VkAttachmentDescription depth_attachment{};
        depth_attachment.format = findDepthFormat();
        depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 2> attachments = { color_attachment, depth_attachment };
        VkRenderPassCreateInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount = attachments.size();
        render_pass_info.pAttachments = attachments.data();
        render_pass_info.subpassCount = 1;
        render_pass_info.dependencyCount = 1;
        render_pass_info.pDependencies = &dependency;
        render_pass_info.pSubpasses = &subpass;

        SHERPHY_EXCEPTION_IF_FALSE(vkCreateRenderPass(m_device, &render_pass_info, nullptr, &m_render_pass) == VK_SUCCESS, "failed to create render pass!");
        return;
    }
    
    void VulkanRHI::createGraphicsPipeline(PipeLineType type, std::vector<char>& vertex_shader, std::vector<char>& fragment_shader) 
    {
        switch (type)
        {
        case PipeLineType::TriangleTest:
            createGraphicsPipelineTriangleTest(vertex_shader, fragment_shader);
            break;
        case PipeLineType::Normal:
            createGraphicsPipelineNormal(vertex_shader, fragment_shader);
            break;
        case PipeLineType::Uniform:
            createGraphicsPipelineUniform(vertex_shader, fragment_shader);
            break;
        case PipeLineType::RayTracing:
            break;
        default:
            createGraphicsPipelineNormal(vertex_shader, fragment_shader);
            break;
        }
    }

    //TODO Uniform Type
    void VulkanRHI::createGraphicsPipelineUniform(std::vector<char>& vertex_shader, std::vector<char>& fragment_shader)
    {
        auto uniform_shader = g_miracle_global_context.m_file_system->readBinaryFile("I:/SherphyEngine/resource/public/SherphyShaderLib/SPV/Normal/SimpleTestTriangle_uniform.spv");

        VkShaderModule uniform_shader_module = createShaderModule(uniform_shader);

        VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
        vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vert_shader_stage_info.module = uniform_shader_module;
        vert_shader_stage_info.pName = "vert";

        VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
        frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        frag_shader_stage_info.module = uniform_shader_module;
        frag_shader_stage_info.pName = "frag";

        VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_info, frag_shader_stage_info };

        VkPipelineDynamicStateCreateInfo dynamic_state{};
        dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = static_cast<uint32_t>(m_dynamic_states.size());
        dynamic_state.pDynamicStates = m_dynamic_states.data();

        VkPipelineVertexInputStateCreateInfo vertex_input_info{};
        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexBindingDescriptionCount = 0;
        vertex_input_info.pVertexBindingDescriptions = nullptr; // Optional
        vertex_input_info.vertexAttributeDescriptionCount = 0;
        vertex_input_info.pVertexAttributeDescriptions = nullptr; // Optional

        VkPipelineInputAssemblyStateCreateInfo input_assembly{};
        input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)m_extent.width;
        viewport.height = (float)m_extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = m_extent;

        VkPipelineViewportStateCreateInfo viewport_state{};
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.pViewports = &viewport;
        viewport_state.scissorCount = 1;
        viewport_state.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional


        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional


        VkPipelineColorBlendAttachmentState color_blend_attachment{};
        color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable = VK_FALSE;
        color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

        VkPipelineColorBlendStateCreateInfo color_blending{};
        color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.logicOpEnable = VK_FALSE;
        color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
        color_blending.attachmentCount = 1;
        color_blending.pAttachments = &color_blend_attachment;
        color_blending.blendConstants[0] = 0.0f; // Optional
        color_blending.blendConstants[1] = 0.0f; // Optional
        color_blending.blendConstants[2] = 0.0f; // Optional
        color_blending.blendConstants[3] = 0.0f; // Optional

        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = 0; // Optional
        pipeline_layout_info.pSetLayouts = nullptr; // Optional
        pipeline_layout_info.pushConstantRangeCount = 0; // Optional
        pipeline_layout_info.pPushConstantRanges = nullptr; // Optional

        SHERPHY_EXCEPTION_IF_FALSE(vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &m_pipeline_layout) == VK_SUCCESS, "");

        VkGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.stageCount = 2;
        pipeline_info.pStages = shader_stages;
        pipeline_info.pVertexInputState = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pViewportState = &viewport_state;
        pipeline_info.pRasterizationState = &rasterizer;
        pipeline_info.pMultisampleState = &multisampling;
        pipeline_info.pDepthStencilState = nullptr; // Optional
        pipeline_info.pColorBlendState = &color_blending;
        pipeline_info.pDynamicState = &dynamic_state;
        pipeline_info.layout = m_pipeline_layout;
        pipeline_info.renderPass = m_render_pass;
        pipeline_info.subpass = 0;
        pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipeline_info.basePipelineIndex = -1; // Optional

        SHERPHY_EXCEPTION_IF_FALSE(vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_graphics_pipeline) == VK_SUCCESS, "failed to create graphics pipeline!");

        vkDestroyShaderModule(m_device, uniform_shader_module, nullptr);
        return;
    }

    void VulkanRHI::createGraphicsPipelineTriangleTest(std::vector<char>& vertex_shader, std::vector<char>& fragment_shader)
    {
        VkShaderModule vert_shader_module = createShaderModule(vertex_shader);
        VkShaderModule frag_shader_module = createShaderModule(fragment_shader);

        VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
        vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vert_shader_stage_info.module = vert_shader_module;
        vert_shader_stage_info.pName = "main";

        VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
        frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        frag_shader_stage_info.module = frag_shader_module;
        frag_shader_stage_info.pName = "main";

        VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_info, frag_shader_stage_info };

        VkPipelineDynamicStateCreateInfo dynamic_state{};
        dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = static_cast<uint32_t>(m_dynamic_states.size());
        dynamic_state.pDynamicStates = m_dynamic_states.data();

        VkPipelineVertexInputStateCreateInfo vertex_input_info{};
        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexBindingDescriptionCount = 0;
        vertex_input_info.pVertexBindingDescriptions = nullptr; // Optional
        vertex_input_info.vertexAttributeDescriptionCount = 0;
        vertex_input_info.pVertexAttributeDescriptions = nullptr; // Optional

        VkPipelineInputAssemblyStateCreateInfo input_assembly{};
        input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)m_extent.width;
        viewport.height = (float)m_extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = m_extent;

        VkPipelineViewportStateCreateInfo viewport_state{};
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.pViewports = &viewport;
        viewport_state.scissorCount = 1;
        viewport_state.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional


        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional


        VkPipelineColorBlendAttachmentState color_blend_attachment{};
        color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable = VK_FALSE;
        color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

        VkPipelineColorBlendStateCreateInfo color_blending{};
        color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.logicOpEnable = VK_FALSE;
        color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
        color_blending.attachmentCount = 1;
        color_blending.pAttachments = &color_blend_attachment;
        color_blending.blendConstants[0] = 0.0f; // Optional
        color_blending.blendConstants[1] = 0.0f; // Optional
        color_blending.blendConstants[2] = 0.0f; // Optional
        color_blending.blendConstants[3] = 0.0f; // Optional

        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = 0; // Optional
        pipeline_layout_info.pSetLayouts = nullptr; // Optional
        pipeline_layout_info.pushConstantRangeCount = 0; // Optional
        pipeline_layout_info.pPushConstantRanges = nullptr; // Optional

        SHERPHY_EXCEPTION_IF_FALSE(vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &m_pipeline_layout) == VK_SUCCESS, "");

        VkGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.stageCount = 2;
        pipeline_info.pStages = shader_stages;
        pipeline_info.pVertexInputState = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pViewportState = &viewport_state;
        pipeline_info.pRasterizationState = &rasterizer;
        pipeline_info.pMultisampleState = &multisampling;
        pipeline_info.pDepthStencilState = nullptr; // Optional
        pipeline_info.pColorBlendState = &color_blending;
        pipeline_info.pDynamicState = &dynamic_state;
        pipeline_info.layout = m_pipeline_layout;
        pipeline_info.renderPass = m_render_pass;
        pipeline_info.subpass = 0;
        pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipeline_info.basePipelineIndex = -1; // Optional

        SHERPHY_EXCEPTION_IF_FALSE(vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_graphics_pipeline) == VK_SUCCESS, "failed to create graphics pipeline!");

        vkDestroyShaderModule(m_device, vert_shader_module, nullptr);
        vkDestroyShaderModule(m_device, frag_shader_module, nullptr);
        return;
    }

    void VulkanRHI::createGraphicsPipelineNormal(std::vector<char>& vertex_shader, std::vector<char>& fragment_shader)
    {
        VkShaderModule vert_shader_module = createShaderModule(vertex_shader);
        VkShaderModule frag_shader_module = createShaderModule(fragment_shader);

        VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
        vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vert_shader_stage_info.module = vert_shader_module;
        vert_shader_stage_info.pName = "main";

        VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
        frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        frag_shader_stage_info.module = frag_shader_module;
        frag_shader_stage_info.pName = "main";

        VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_info, frag_shader_stage_info };

        VkPipelineDynamicStateCreateInfo dynamic_state{};
        dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = static_cast<uint32_t>(m_dynamic_states.size());
        dynamic_state.pDynamicStates = m_dynamic_states.data();

        auto bindingDescription = VkVertex::getBindingDescription();
        auto attributeDescriptions = VkVertex::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertex_input_info{};
        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexBindingDescriptionCount = 1;
        vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertex_input_info.pVertexBindingDescriptions = &bindingDescription;
        vertex_input_info.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo input_assembly{};
        input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;

        //VkViewport viewport{};
        //viewport.x = 0.0f;
        //viewport.y = 0.0f;
        //viewport.width = (float)m_extent.width;
        //viewport.height = (float)m_extent.height;
        //viewport.minDepth = 0.0f;
        //viewport.maxDepth = 1.0f;

        //VkRect2D scissor{};
        //scissor.offset = { 0, 0 };
        //scissor.extent = m_extent;

        VkPipelineViewportStateCreateInfo viewport_state{};
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        //viewport_state.pViewports = &viewport;
        viewport_state.scissorCount = 1;
        //viewport_state.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional


        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional

        VkPipelineDepthStencilStateCreateInfo depth_stencil{};
        depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth_stencil.depthTestEnable = VK_TRUE;
        depth_stencil.depthWriteEnable = VK_TRUE;
        depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depth_stencil.depthBoundsTestEnable = VK_FALSE;
        depth_stencil.stencilTestEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState color_blend_attachment{};
        color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable = VK_FALSE;
        color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

        VkPipelineColorBlendStateCreateInfo color_blending{};
        color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.logicOpEnable = VK_FALSE;
        color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
        color_blending.attachmentCount = 1;
        color_blending.pAttachments = &color_blend_attachment;
        color_blending.blendConstants[0] = 0.0f; // Optional
        color_blending.blendConstants[1] = 0.0f; // Optional
        color_blending.blendConstants[2] = 0.0f; // Optional
        color_blending.blendConstants[3] = 0.0f; // Optional

        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = 1;
        pipeline_layout_info.pSetLayouts = &m_descriptor_set_layout;
        pipeline_layout_info.pushConstantRangeCount = 0; // Optional
        pipeline_layout_info.pPushConstantRanges = nullptr; // Optional

        SHERPHY_EXCEPTION_IF_FALSE(vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &m_pipeline_layout) == VK_SUCCESS, "");

        VkGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.stageCount = 2;
        pipeline_info.pStages = shader_stages;
        pipeline_info.pVertexInputState = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pViewportState = &viewport_state;
        pipeline_info.pRasterizationState = &rasterizer;
        pipeline_info.pMultisampleState = &multisampling;
        pipeline_info.pDepthStencilState = &depth_stencil;
        pipeline_info.pColorBlendState = &color_blending;
        pipeline_info.pDynamicState = &dynamic_state;
        pipeline_info.layout = m_pipeline_layout;
        pipeline_info.renderPass = m_render_pass;
        pipeline_info.subpass = 0;
        pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipeline_info.basePipelineIndex = -1; // Optional

        SHERPHY_EXCEPTION_IF_FALSE(vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_graphics_pipeline) == VK_SUCCESS, "failed to create graphics pipeline!");

        vkDestroyShaderModule(m_device, vert_shader_module, nullptr);
        vkDestroyShaderModule(m_device, frag_shader_module, nullptr);
        return;
    }

    // TODO test if device suitable
    bool VulkanRHI::isDeviceSuitable(VkPhysicalDevice& device) 
    {
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extension_supported = checkDeviceExtensionSupport(device);

        bool swap_chain_adequate = false;
        if (extension_supported) {
            SwapChainSupportDetails swap_chain_support = querySwapChainSupport(device);
            swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
        }

        bool nv_device = false;
        VkPhysicalDeviceProperties prop;
        vkGetPhysicalDeviceProperties(device, &prop);
        if ((static_cast<std::string>(prop.deviceName).find_first_of("Intel") == std::string::npos) == false) 
        {
            SHERPHY_LOG("YES");
            nv_device = true;
        }

        VkPhysicalDeviceFeatures supported_features;
        vkGetPhysicalDeviceFeatures(device, &supported_features);

        return indices.isComplete() && extension_supported && swap_chain_adequate && nv_device && supported_features.samplerAnisotropy;
    }

    VkSurfaceFormatKHR VulkanRHI::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats) {
        for (VkSurfaceFormatKHR available_format : available_formats) {
            if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return available_format;
            }
        }
        return available_formats[0];
    }

    VkPresentModeKHR VulkanRHI::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanRHI::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        else {
            int width, height;
            g_miracle_global_context.m_display_system->getFramebufferSize(width, height);

            VkExtent2D actual_extent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actual_extent;
        }
    }

    SwapChainSupportDetails VulkanRHI::querySwapChainSupport(VkPhysicalDevice& device) 
    {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);
        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &format_count, nullptr);

        if (format_count != 0) {
            details.formats.resize(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &format_count, details.formats.data());
        }

        uint32_t present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &present_mode_count, nullptr);

        if (present_mode_count != 0) {
            details.present_modes.resize(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &present_mode_count, details.present_modes.data());
        }

        return details;
    }

    bool VulkanRHI::checkDeviceExtensionSupport(VkPhysicalDevice& device) 
    {
        uint32_t extension_count;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

        std::vector<VkExtensionProperties> available_extensions(extension_count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

        std::set<std::string> required_extensions(m_device_extensions.begin(), m_device_extensions.end());

        for (const auto& extesion : available_extensions) 
        {
            required_extensions.erase(extesion.extensionName);
        }

        return required_extensions.empty();
    }

    QueueFamilyIndices VulkanRHI::findQueueFamilies(VkPhysicalDevice& device) 
    {
        QueueFamilyIndices indices;
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
        std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());


        VkBool32 present_support = false;
        for (size_t i = 0; i < queue_families.size(); i++) {
            if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) 
            {
                indices.graphics_family = i;
            }

            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &present_support);
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

    void VulkanRHI::createSyncObjects() 
    {
        m_image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            SHERPHY_EXCEPTION_IF_FALSE(vkCreateSemaphore(m_device, &semaphore_info, nullptr, &m_image_available_semaphores[i]) == VK_SUCCESS, "failed to create image semaphore for a frame!");
            SHERPHY_EXCEPTION_IF_FALSE(vkCreateSemaphore(m_device, &semaphore_info, nullptr, &m_render_finished_semaphores[i]) == VK_SUCCESS, "failed to create render finish semaphore for a frame!");
            SHERPHY_EXCEPTION_IF_FALSE(vkCreateFence(m_device, &fence_info, nullptr, &m_in_flight_fences[i]) == VK_SUCCESS, "failed to create fence for a frame!");
        }

        return;
    }

    void VulkanRHI::recreateSwapChain() {
        int width = 0, height = 0;
        while (width == 0 || height == 0) {
            g_miracle_global_context.m_display_system->getFramebufferSize(width, height);
            g_miracle_global_context.m_display_system->waitEvents();
        }

        vkDeviceWaitIdle(m_device);

        cleanupSwapChain();

        createSwapChain();
        createImageViews();
        createDepthResources();
        createFrameBuffers();
    }

    void VulkanRHI::drawFrame() 
    {
        vkWaitForFences(m_device, 1, &m_in_flight_fences[m_current_frame], VK_TRUE, UINT64_MAX);

        uint32_t image_index;
        VkResult result = vkAcquireNextImageKHR(m_device, m_swap_chain, UINT64_MAX, m_image_available_semaphores[m_current_frame], VK_NULL_HANDLE, &image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }
        else {
            SHERPHY_EXCEPTION_IF_FALSE(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "failed to acquire swap chain image!");
        }

        updateUniformBuffer(m_current_frame);

        vkResetFences(m_device, 1, &m_in_flight_fences[m_current_frame]);

        vkResetCommandBuffer(m_command_buffers[m_current_frame], /*VkCommandBufferResetFlagBits*/ 0);
        recordCommandBuffer(m_command_buffers[m_current_frame], image_index);

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore wait_semaphores[] = { m_image_available_semaphores[m_current_frame] };
        VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;

        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &m_command_buffers[m_current_frame];

        VkSemaphore signal_semaphores[] = { m_render_finished_semaphores[m_current_frame] };
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;

        SHERPHY_EXCEPTION_IF_FALSE(vkQueueSubmit(m_graphics_queue, 1, &submit_info, m_in_flight_fences[m_current_frame]) == VK_SUCCESS, "failed to submit draw command buffer!");

        VkPresentInfoKHR present_info{};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = signal_semaphores;

        VkSwapchainKHR swap_chains[] = { m_swap_chain };
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swap_chains;

        present_info.pImageIndices = &image_index;

        result = vkQueuePresentKHR(m_present_queue, &present_info);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_frame_buffer_resized) 
        {
            m_frame_buffer_resized = false;
            recreateSwapChain();
        }
        else
        {
            SHERPHY_EXCEPTION_IF_FALSE(result == VK_SUCCESS, "failed to present swap chain image!");
        }

        m_current_frame = (m_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void VulkanRHI::cleanupSwapChain() {
        for (auto image_view : m_swap_chain_image_views) {
            vkDestroyImageView(m_device, image_view, nullptr);
        }

        vkDestroyImageView(m_device, m_depth_image_view, nullptr);
        vkDestroyImage(m_device, m_depth_image, nullptr);
        vkFreeMemory(m_device, m_depth_image_memory, nullptr);

        for (auto frame_buffer : m_swap_chain_frame_buffers) {
            vkDestroyFramebuffer(m_device, frame_buffer, nullptr);
        }
        vkDestroySwapchainKHR(m_device, m_swap_chain, nullptr);
        return;
    }

    void VulkanRHI::cleanUp()
    {
        vkDeviceWaitIdle(m_device);
        cleanupSwapChain();

        vkDestroyPipeline(m_device, m_graphics_pipeline, nullptr);
        vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
        vkDestroyRenderPass(m_device, m_render_pass, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroyBuffer(m_device, m_uniform_buffers[i], nullptr);
            vkFreeMemory(m_device, m_uniform_buffers_memory[i], nullptr);
        }
        vkDestroyDescriptorPool(m_device, m_descriptor_pool, nullptr);

        vkDestroySampler(m_device, m_sampler, nullptr);
        vkDestroyImage(m_device, m_texture_image, nullptr);
        vkFreeMemory(m_device, m_texture_image_memory, nullptr);

        vkDestroyDescriptorSetLayout(m_device, m_descriptor_set_layout, nullptr);
        
        vkDestroyBuffer(m_device, m_vertex_buffer, nullptr);
        vkFreeMemory(m_device, m_vertex_buffer_memory, nullptr);
        vkDestroyBuffer(m_device, m_index_buffer, nullptr);
        vkFreeMemory(m_device, m_index_buffer_memory, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(m_device, m_render_finished_semaphores[i], nullptr);
            vkDestroySemaphore(m_device, m_image_available_semaphores[i], nullptr);
            vkDestroyFence(m_device, m_in_flight_fences[i], nullptr);
        }
        vkDestroyCommandPool(m_device, m_command_pool, nullptr);

        vkDestroyDevice(m_device, nullptr);
        if (m_enable_validation_layer) {
            DestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
        }

        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        vkDestroyInstance(m_instance, nullptr);
    }

    void VulkanRHI::run()
    {
        initVulkan();
        cleanUp();
    }
}

