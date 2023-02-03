#include <string.h>

#include "vulkan_rhi.h"

#include "MiracleRuntime/Soul/macro.h"

#define WIDTH 800
#define HEIGHT 600

namespace SherphyEngine(Miracle){

    void TriangleApplication::initWindow()
    {   
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_window = glfwCreateWindow(WIDTH, HEIGHT, "Sherphy_Engine", nullptr, nullptr);
    }

    void TriangleApplication::createInstance()
    {
        if(m_enable_validation_layer && !checkValidationLayerSupport())
        {
            throw std::runtime_error("validation layers requested, but not avaliable.");
        }


        // Optional
        VkApplicationInfo vk_app_info{};
        vk_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        vk_app_info.pApplicationName = "Hello World";
        vk_app_info.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
        vk_app_info.pEngineName = "No Engine";
        vk_app_info.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
        vk_app_info.apiVersion = VK_API_VERSION_1_0;
        vk_app_info.pNext = nullptr;

        // Not Optional
        // TODO add a debugCallBack to create_info.pNext to debug and validate
        // SEE  https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Validation_layers
        VkInstanceCreateInfo create_info{};

        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &vk_app_info;

        // uint32_t glfwExtensionCount = 0;
        // const char** glfwExtensions;
        // glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        auto extensions = getRequiredExtensions();
        create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        create_info.ppEnabledExtensionNames = extensions.data();

        if(m_enable_validation_layer)
        {
            create_info.enabledLayerCount = static_cast<uint32_t>(m_validation_layers.size());
            create_info.ppEnabledLayerNames = m_validation_layers.data();
        }
        else
        {
            create_info.enabledLayerCount = 0;
        }

        
// VkResult result = ;
        SHERPHY_EXCEPTION_IF_FALSE(vkCreateInstance(&create_info, nullptr, &m_instance) == VK_SUCCESS, "create vk instance failed!\n");

        // update extensions properties
        vkEnumerateInstanceExtensionProperties(nullptr, &m_extensions_count, nullptr);
        m_extensions.clear();
        m_extensions.resize(m_extensions_count);
        vkEnumerateInstanceExtensionProperties(nullptr, &m_extensions_count, m_extensions.data());

#ifdef SHERPHY_DEBUG
        std::cout << "available extensions:\n";

        for (const auto& extension : extensions) {
            std::cout << '\t' << extension.extensionName << '\n';
        }
#endif

    }

    std::vector<const char*> TriangleApplication::getRequiredExtensions()
    {
        uint32_t glfwExtensionsCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);

        if(m_enable_validation_layer)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool TriangleApplication::checkValidationLayerSupport()
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

    void TriangleApplication::initVulkan()
    {
        createInstance();
        //setupDebugMessenger();
        
    }

    void TriangleApplication::pickPhysicalDevice()
    {
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);
        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());

        if (device_count == 0)
        {
            throw std::runtime_error("Failed to Find Graphic Device with Vulkan Support");
        }

        for (VkPhysicalDevice& device : devices)
        {
            if (isDeviceSuitable(device)) 
            {
                m_physicalDevice = device;
                break;
            }
        }
    }

    // TODO test if device suitable
    bool TriangleApplication::isDeviceSuitable(VkPhysicalDevice& device) {
        QueueFamilyIndices indices = findQueueFamilies(device);

        return indices.isComplete();
    }

    QueueFamilyIndices TriangleApplication::findQueueFamilies(VkPhysicalDevice& device) 
    {
        QueueFamilyIndices indices;
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

        std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

        for (size_t i = 0; i < queue_families.size(); i++) {
            if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) 
            {
                indices.graphics_family = i;
            }

            if (indices.isComplete()) 
            {
                break;
            }
        }

        return indices;
    }

    void TriangleApplication::mainLoop()
    {
        while (!glfwWindowShouldClose(m_window))
        {
            glfwPollEvents();
        }
    }

    void TriangleApplication::cleanUp()
    {
        vkDestroyInstance(m_instance, nullptr);

        glfwDestroyWindow(m_window);

        glfwTerminate();
    }

    void TriangleApplication::run()
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanUp();
    }
}

