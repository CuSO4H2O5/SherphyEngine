#pragma once

#include "RenderingMath.h"
#include "VulkanBuffer.h"
#include "VulkanDevice.h"
#include "World/Scene.h"

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>

namespace Sherphy{
    enum class PipeLineType {
        TriangleTest,
        Normal,
        Uniform,
        RayTracing
    };

    struct SwapChainSupportDetails 
    {
        VkSurfaceCapabilitiesKHR capabilities{};
        std::vector<VkSurfaceFormatKHR> formats{};
        std::vector<VkPresentModeKHR> present_modes{};
    };

    struct AccelerationStructure {
        VkAccelerationStructureKHR handle;
        uint64_t device_address = 0;
        VkDeviceMemory memory;
        VkBuffer buffer;
    };

    class VulkanRHI
    {
    public:
        void initVulkan(PipeLineType type);
        std::vector<VkVertex>& getVerticesWrite();
        std::vector<uint32_t>& getIndicesWrite();
        void drawFrame();
        void cleanUp();
    private:
        void initBasic(PipeLineType type);
        void createRenderingStructure(PipeLineType type);
        void allocRenderingMemory(PipeLineType type);
    private:
        //------------------ System Initialization ----------------------------
        void createSurface();
        void pickPhysicalDevice();
        void setupRequiredDeviceExtensions(PipeLineType type);
        void getEnabledFeatures(PipeLineType type);
        void getEnabledFeaturesRayTracing();

        void createSwapChain(VkPhysicalDevice deivce);
        void recreateSwapChain();
        void cleanupSwapChain();

        void createImageViews();
        void createRenderPass();
        void createDescriptorSetLayout(PipeLineType type);
        void createDescriptorSetLayoutNormal();
        void createDescriptorSetLayoutRayTracing();
        void createDescriptorPool(PipeLineType type);
        void createDescriptorPoolNormal();
        void createDescriptorPoolRayTracing();
        void createDescriptorSets(PipeLineType type);
        void createDescriptorSetsNormal();
        void createDescriptorSetsRayTracing();
        void createGraphicsPipeline(PipeLineType type, 
                                    const std::vector<char>& vertex_shader, 
                                    const std::vector<char>& fragment_shader,
                                    const std::vector<char>& closet_hit_shader = {});
        void createGraphicsPipelineNormal(const std::vector<char>& vertex_shader,
                                          const std::vector<char>& fragment_shader);
        void createGraphicsPipelineTriangleTest(const std::vector<char>& vertex_shader,
                                                const std::vector<char>& fragment_shader);
        void createGraphicsPipelineUniform(const std::vector<char>& vertex_shader,
                                           const std::vector<char>& fragment_shader);
        void createGraphicsPipelineRayTracing(const std::vector<char>& raygen_shader,
                                              const std::vector<char>& raymiss_shader,
                                              const std::vector<char>& closest_hit_shader);
        VkPipelineShaderStageCreateInfo loadShader(const std::vector<char>& shader, VkShaderStageFlagBits stage);
        VkShaderModule createShaderModule(const std::vector<char>& code);
        void cleanShader();

        void createTextureImage();
        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags);
        void createTextureImageView();
        void createTextureSampler();
        void createImage(uint32_t width, 
                         uint32_t height, 
                         VkFormat format, 
                         VkImageTiling tiling, 
                         VkImageUsageFlags usage, 
                         VkMemoryPropertyFlags properties, 
                         VkImage& image, 
                         VkDeviceMemory& image_memory);
        void copyBufferToImage(VkBuffer buffer, 
                               VkImage image, 
                               uint32_t width, 
                               uint32_t height);
        void transitionImageLayout(VkImage image, 
                                   VkFormat format, 
                                   VkImageLayout old_layout, 
                                   VkImageLayout new_layout);
        void createFrameBuffers();
        void createVertexBuffer(PipeLineType type);
        void createIndexBuffer(PipeLineType type);
        void createTransformBuffer(PipeLineType type);
        void createDepthResources();
        VkFormat findDepthFormat();
        VkFormat findSupportedFormat(VkPhysicalDevice device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        void createUniformBuffers();
        void updateUniformBuffer(uint32_t current_image);

        void copyBufferImmediate(VulkanBuffer src_buffer,
                                 VulkanBuffer dst_buffer,
                                 VkDeviceSize size);

        void recordCommandBuffer(VkCommandBuffer command_buffer,
                                 uint32_t image_index);

        void createSyncObjects();

        void createRenderPassNormal();
        //------------------- Acceleration Structure --------------------------
        void createAccelerationStructureBuffer(AccelerationStructure& acceleration_structure, VkAccelerationStructureBuildSizesInfoKHR build_size_info);
        void createBottomLevelAccelerationStructure();
        void createTopLevelAccelerationStructure();

        //------------------- Check Pick --------------------------------------
        bool isDeviceSuitable(VkPhysicalDevice& device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice& device);
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info);
        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
                                              const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
                                              const VkAllocationCallbacks* pAllocator, 
                                              VkDebugUtilsMessengerEXT* pDebugMessenger);
        void setupDebugMessenger();

        
        void createInstance();
        std::vector<const char*> getRequiredExtensions();
        bool checkValidationLayerSupport();
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice& device);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    private:
        //----------------- Vk Device Part ---------------------------
        VulkanDevice m_device;
        VkInstance m_instance;

        //physical device pool
        std::vector<VkPhysicalDevice> m_physical_devices;
        uint32_t m_pick_physical_device_id = std::numeric_limits<uint32_t>::max();

        void* m_logical_device_create_pNext_chain = nullptr;
        std::vector<const char*> m_validation_layers = {
            "VK_LAYER_KHRONOS_validation"
        };

        std::vector<const char*> m_device_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

#ifdef SHERPHY_DEBUG
        const bool m_enable_validation_layer = true;
#else
        const bool m_enable_validation_layer = false;
#endif

        float m_queue_prioity = 1.0f;
        VkPhysicalDeviceFeatures m_device_features{};
        VkQueue m_graphics_queue;


        //------------------ Vk Platform Surface -----------------------------
        VkSurfaceKHR m_surface;
        VkQueue m_present_queue;

        //------------------ Vk Swap Chain -----------------------------------
        VkSwapchainKHR m_swap_chain;
        std::vector<VkImage> m_swap_chain_images;

        VkFormat m_swap_chain_image_format;
        VkPresentModeKHR m_present_mode;
        VkExtent2D m_extent;

        std::vector<VkImageView> m_swap_chain_image_views;
        std::vector<VkFramebuffer> m_swap_chain_frame_buffers;

        //------------------ Graphics PipeLine -------------------------------
        std::vector<VkDynamicState> m_dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkRenderPass m_render_pass;
        VkPipelineLayout m_pipeline_layout;
        VkPipeline m_graphics_pipeline;

        //------------------ Shader Asset -------------------------------------
        std::vector<VkShaderModule> m_managed_shader_modules;

        //------------------ Global Buffers Pipeline ----------------------------------
        VkDescriptorSetLayout m_descriptor_set_layout;
        VkDescriptorPool m_descriptor_pool;
        std::vector<VkDescriptorSet> m_descriptor_sets;

        //------------------ Rendering Buffers -------------------------------
        VulkanBuffer m_vertex_buffer;
        //VkBuffer m_vertex_buffer;
        //VkDeviceMemory m_vertex_buffer_memory;
        VulkanBuffer m_index_buffer;
        //VkBuffer m_index_buffer;
        //VkDeviceMemory m_index_buffer_memory;
        VulkanBuffer m_transform_buffer;
        std::vector<VulkanBuffer> m_uniform_buffers;
        //std::vector<VkBuffer> m_uniform_buffers;
        //std::vector<VkDeviceMemory> m_uniform_buffers_memory;
        //std::vector<void*> m_uniform_buffers_mapped;

        VkImage m_depth_image;
        VkDeviceMemory m_depth_image_memory;
        VkImageView m_depth_image_view;

        //------------------ Submit data -------------------------------------
#if defined(SHERPHY_DEUBG_RAW)
        const std::vector<VkVertex> m_vertices = {
            {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

            {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
        };
        const std::vector<uint16_t> m_indices = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4
        };
        const VkTransformMatrixKHR transform_matrix = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f
        };
#else 
        std::vector<VkVertex> m_vertices;
        std::vector<uint32_t> m_indices;
        VkTransformMatrixKHR m_transform_matrix = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f
        };
#endif
        VkImage m_texture_image;
        VkImageView m_texture_image_view;
        VkSampler m_sampler;
        VkDeviceMemory m_texture_image_memory;

        //------------------ Draw Frame --------------------------------------
        std::vector<VkSemaphore> m_image_available_semaphores;
        std::vector<VkSemaphore> m_render_finished_semaphores;
        std::vector<VkFence> m_in_flight_fences;
        uint32_t m_current_frame = 0;
        bool m_frame_buffer_resized = false;

        //------------------ Debug -------------------------------------------
        VkDebugUtilsMessengerEXT m_debug_messenger;


        //------------------ RayTracingLine ----------------------------------
        AccelerationStructure m_bottom_level_AS{};
        AccelerationStructure m_top_level_AS{};

        VkPhysicalDeviceBufferDeviceAddressFeatures m_enabled_buffer_device_addres_features{};
        VkPhysicalDeviceRayTracingPipelineFeaturesKHR m_enabled_ray_tracing_pipeline_features{};
        VkPhysicalDeviceAccelerationStructureFeaturesKHR m_enabled_acceleration_structure_features{};
    };
}