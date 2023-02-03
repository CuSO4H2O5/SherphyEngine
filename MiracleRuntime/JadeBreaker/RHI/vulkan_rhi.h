#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <optional>

// #include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace SherphyEngine(Miracle){
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphics_family;

        bool isComplete() {
            return graphics_family.has_value();
        }
    };


    class TriangleApplication
    {
    private:
        GLFWwindow* m_window;
        VkInstance m_instance;
        std::vector<VkExtensionProperties> m_extensions;
        uint32_t m_extensions_count;

        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

        std::vector<const char*> m_validation_layers = {
            "VK_LAYER_KHRONOS_validation"
        };

#ifdef SHERPHY_DEBUG
        const bool m_enable_validation_layer = true;
#else
        const bool m_enable_validation_layer = false;
#endif

    public:
        void run();

    private:
        void initWindow();
        void initVulkan();
        void pickPhysicalDevice();
        bool isDeviceSuitable(VkPhysicalDevice& device);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice& device);


        void mainLoop();
        void createInstance();
        void cleanUp();
        std::vector<const char*> getRequiredExtensions();
        bool checkValidationLayerSupport();

        //static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        //    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        //    VkDebugUtilsMessageTypeFlagsEXT messageType,
        //    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        //    void* pUserData) {
        //    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        //        // Message is important enough to show
        //    }
        //    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        //    return VK_FALSE;
        //}
    };
}