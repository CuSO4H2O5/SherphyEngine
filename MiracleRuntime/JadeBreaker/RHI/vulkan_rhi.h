#pragma once

#include "rendering_math.h"
#include "World/Scene.h"

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <optional>

//#define GLFW_INCLUDE_VULKAN
//#include <GLFW/glfw3.h>

namespace Sherphy{
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;

        bool isComplete() {
            return graphics_family.has_value() && present_family.has_value();
        }
    };

    enum class PipeLineType {
        TriangleTest,
        Normal,
        Uniform,
        RayTracing
    };

    enum class RenderPassType {
        Normal,
        RayTracing
    };

    struct SwapChainSupportDetails 
    {
        VkSurfaceCapabilitiesKHR capabilities{};
        std::vector<VkSurfaceFormatKHR> formats{};
        std::vector<VkPresentModeKHR> present_modes{};
    };

    class VulkanRHI
    {
    private:
        //----------------- Vk Device Part ---------------------------
        VkInstance m_instance;
        //std::vector<VkExtensionProperties> m_extensions;
        //uint32_t m_extensions_count;

        VkDevice m_device;
        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;

        std::vector<const char*> m_validation_layers = {
            "VK_LAYER_KHRONOS_validation"
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
        const std::vector<const char*> m_device_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
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

        //------------------ Global Buffers Pipeline ----------------------------------
        VkDescriptorSetLayout m_descriptor_set_layout;
        VkDescriptorPool m_descriptor_pool;
        std::vector<VkDescriptorSet> m_descriptor_sets;

        //------------------ Command Buffer ----------------------------------
        VkCommandPool m_command_pool;
        std::vector<VkCommandBuffer> m_command_buffers;

        //------------------ Rendering Buffers -------------------------------
        VkBuffer m_vertex_buffer;
        //std::vector<VkBuffer> m_vertex_buffers;
        VkDeviceMemory m_vertex_buffer_memory;
        VkBuffer m_index_buffer;
        VkDeviceMemory m_index_buffer_memory;
        std::vector<VkBuffer> m_uniform_buffers;
        std::vector<VkDeviceMemory> m_uniform_buffers_memory;
        std::vector<void*> m_uniform_buffers_mapped;

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
#else 
        std::vector<VkVertex> m_vertices;
        std::vector<uint32_t> m_indices;
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

        //------------------ GLFW Interface ----------------------------------
        GLFWwindow* m_window;

        //------------------ Debug -------------------------------------------
        VkDebugUtilsMessengerEXT m_debug_messenger;

    public:
        void run();
        void initWindow();
        void initVulkan();
        void drawFrame();
        bool shouldClose();
        void cleanUp();
    private:
        //------------------ System Initialization ----------------------------
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();

        void createSwapChain();
        void recreateSwapChain();
        void cleanupSwapChain();

        void createImageViews();
        void createRenderPass(RenderPassType type);
        void createDescriptorSetLayout();
        void createDescriptorPool();
        void createDescriptorSets();
        void createGraphicsPipeline(PipeLineType type, 
                                    std::vector<char>& vertex_shader, 
                                    std::vector<char>& fragment_shader);

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
        void createBuffer(VkDeviceSize size, 
                          VkBufferUsageFlags usage, 
                          VkMemoryPropertyFlags properties, 
                          VkBuffer& buffer, 
                          VkDeviceMemory& bufferMemory);
        void createVertexBuffer();
        void createIndexBuffer();
        void createDepthResources();
        VkFormat findDepthFormat();
        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        void createUniformBuffers();
        void updateUniformBuffer(uint32_t current_image);

        void copyBufferImmediate(VkBuffer src_buffer,
                                VkBuffer dst_buffer,
                                VkDeviceSize size);

        void createCommandPool();
        void createCommandBuffer();
        void recordCommandBuffer(VkCommandBuffer command_buffer,
                                 uint32_t image_index);

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);

        void createSyncObjects();

        void createRenderPassNormal();
        void createGraphicsPipelineNormal(std::vector<char>& vertex_shader,
                                          std::vector<char>& fragment_shader);
        void createGraphicsPipelineTriangleTest(std::vector<char>& vertex_shader, 
                                                std::vector<char>& fragment_shader);
        void createGraphicsPipelineUniform(std::vector<char>& vertex_shader, 
                                           std::vector<char>& fragment_shader);
        VkShaderModule createShaderModule(const std::vector<char>& code);


        //------------------- Check Pick --------------------------------------
        bool isDeviceSuitable(VkPhysicalDevice& device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice& device);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice& device);
        uint32_t findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info);
        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
                                              const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
                                              const VkAllocationCallbacks* pAllocator, 
                                              VkDebugUtilsMessengerEXT* pDebugMessenger);
        void setupDebugMessenger();
        void DestroyDebugUtilsMessengerEXT(VkInstance instance, 
                                           VkDebugUtilsMessengerEXT debugMessenger, 
                                           const VkAllocationCallbacks* pAllocator);

        
        void createInstance();
        std::vector<const char*> getRequiredExtensions();
        bool checkValidationLayerSupport();
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice& device);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    };
}