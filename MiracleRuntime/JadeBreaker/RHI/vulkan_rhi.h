#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <optional>

// #include <vulkan/vulkan.h>
//#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
//#define GLFW_EXPOSE_NATIVE_WIN32
//#include <GLFW/glfw3native.h>

namespace Sherphy{
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;

        bool isComplete() {
            return graphics_family.has_value() && present_family.has_value();
        }
    };

    struct SwapChainSupportDetails 
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    class TriangleApplication
    {
    private:
        //----------------- Vk Device Part ---------------------------
        VkInstance m_instance;
        std::vector<VkExtensionProperties> m_extensions;
        uint32_t m_extensions_count;

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

        //------------------ Graphics PipeLine -------------------------------
        std::vector<VkDynamicState> m_dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkRenderPass m_render_pass;
        VkPipelineLayout m_pipeline_layout;
        VkPipeline m_graphics_pipeline;


        //------------------ GLFW Interface ----------------------------------
        GLFWwindow* m_window;

    public:
        void run();

    private:
        //------------------ System Initialization ----------------------------
        void initWindow();
        void initVulkan();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSwapChain();
        void createImageViews();

        void createRenderPassNormal();
        void createGraphicsPipelineNormal();
        VkShaderModule createShaderModule(const std::vector<char>& code);


        //------------------- Check Pick --------------------------------------
        bool isDeviceSuitable(VkPhysicalDevice& device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice& device);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice& device);


        void mainLoop();
        void createInstance();
        void cleanUp();
        std::vector<const char*> getRequiredExtensions();
        bool checkValidationLayerSupport();
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice& device);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        //TODO DEBUG CALL BACK
    };
}