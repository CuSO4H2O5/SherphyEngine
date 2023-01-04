#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>

// #include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Miracle{
    class TriangleApplication
    {
    private:
        GLFWwindow* m_window;
        VkInstance m_instance;
        std::vector<VkExtensionProperties> m_extensions;
        uint32_t m_extensions_count;

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
        void mainLoop();
        void createInstance();
        void cleanUp();
        std::vector<const char*> getRequiredExtensions();
        bool checkValidationLayerSupport();
    };
}