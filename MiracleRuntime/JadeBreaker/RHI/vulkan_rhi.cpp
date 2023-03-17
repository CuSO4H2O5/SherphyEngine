#include <string.h>
#include <algorithm>
#include <set>

#include "vulkan_rhi.h"
#include "shader_resource_manger.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

namespace Sherphy{

    void TriangleApplication::initWindow()
    {   
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_window = glfwCreateWindow(WIDTH, HEIGHT, "Sherphy_Engine", nullptr, nullptr);
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    void TriangleApplication::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debug_messenger, pAllocator);
        }
    }

    void TriangleApplication::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info) {
        create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        create_info.pfnUserCallback = debugCallback;
    }

    void TriangleApplication::createInstance()
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

        //// update extensions properties
        //vkEnumerateInstanceExtensionProperties(nullptr, &m_extensions_count, nullptr);
        //m_extensions.clear();
        //m_extensions.resize(m_extensions_count);
        //vkEnumerateInstanceExtensionProperties(nullptr, &m_extensions_count, m_extensions.data());

#ifdef SHERPHY_DEBUG
        std::cout << "available extensions:\n";

        for (const char* extension : extensions) {
            std::cout << '\t' << extension << '\n';
        }
#endif

    }

    VkResult TriangleApplication::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void TriangleApplication::setupDebugMessenger() {
        SHERPHY_RETURN_IF_FALSE(m_enable_validation_layer, "No Validation Layer Debug Info");

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        SHERPHY_EXCEPTION_IF_FALSE(CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debug_messenger) == VK_SUCCESS, "failed to set up debug messenger!");
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
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        SHERPHY_EXCEPTION_IF_FALSE(m_physical_device, "Did not detected Proper Physical Device");
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createRenderPass(RenderPassType::Normal);
        createGraphicsPipeline(PipeLineType::Normal);
        createFrameBuffers();
        createCommandPool();
        createCommandBuffer();
        createSyncObjects();
    }

    void TriangleApplication::createRenderPass(RenderPassType type) 
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

    void TriangleApplication::createCommandBuffer() {
        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = m_command_pool;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = 1;

        SHERPHY_EXCEPTION_IF_FALSE(vkAllocateCommandBuffers(m_device, &alloc_info, &m_command_buffer) == VK_SUCCESS, "failed to allocate command buffers!");
    }

    void TriangleApplication::recordCommandBuffer(VkCommandBuffer command_buffer, uint32_t image_index) {
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        SHERPHY_EXCEPTION_IF_FALSE(vkBeginCommandBuffer(command_buffer, &begin_info) == VK_SUCCESS, "failed to begin recording command buffer!");

        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = m_render_pass;
        render_pass_info.framebuffer = m_swap_chain_frame_buffers[image_index];
        render_pass_info.renderArea.offset = { 0, 0 };
        render_pass_info.renderArea.extent = m_extent;
        VkClearValue clear_color = { {{1.0f, 1.0f, 1.0f, 1.0f}} };
        render_pass_info.clearValueCount = 1;
        render_pass_info.pClearValues = &clear_color;
        vkCmdBeginRenderPass(m_command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

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
        vkCmdDraw(command_buffer, 3, 1, 0, 0);//TODO abstract command
        vkCmdEndRenderPass(command_buffer);
        SHERPHY_EXCEPTION_IF_FALSE(vkEndCommandBuffer(command_buffer) == VK_SUCCESS, "failed to record command buffer!");
    }

    void TriangleApplication::createCommandPool() 
    {
        QueueFamilyIndices queue_family_indices = findQueueFamilies(m_physical_device);

        VkCommandPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();

        SHERPHY_EXCEPTION_IF_FALSE(vkCreateCommandPool(m_device, &pool_info, nullptr, &m_command_pool) == VK_SUCCESS, "failed to create command pool!");

    }

    void TriangleApplication::createFrameBuffers() 
    {
        m_swap_chain_frame_buffers.resize(m_swap_chain_image_views.size());

        for (size_t i = 0; i < m_swap_chain_image_views.size(); i++) {
            VkImageView attachments[] = {
                m_swap_chain_image_views[i]
            };

            VkFramebufferCreateInfo frame_buffer_info{};
            frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            frame_buffer_info.renderPass = m_render_pass;
            frame_buffer_info.attachmentCount = 1;
            frame_buffer_info.pAttachments = attachments;
            frame_buffer_info.width = m_extent.width;
            frame_buffer_info.height = m_extent.height;
            frame_buffer_info.layers = 1;
            SHERPHY_EXCEPTION_IF_FALSE(vkCreateFramebuffer(m_device, &frame_buffer_info, nullptr, &m_swap_chain_frame_buffers[i]) == VK_SUCCESS, "failed to create framebuffer!");
        }
    }

    void TriangleApplication::createSurface() {
        SHERPHY_EXCEPTION_IF_FALSE(glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) == VK_SUCCESS, "failed to create window surface!");
    }

    void TriangleApplication::pickPhysicalDevice()
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

    void TriangleApplication::createLogicalDevice()
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

    void TriangleApplication::createSwapChain() 
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

    void TriangleApplication::createImageViews() 
    {
        SHERPHY_EXCEPTION_IF_FALSE((m_swap_chain_images.size()>0), "swap chain image is empty");
        m_swap_chain_image_views.resize(m_swap_chain_images.size());

        for (size_t i = 0; i < m_swap_chain_images.size(); i++) {
            VkImageViewCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            create_info.image = m_swap_chain_images[i];
            create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            create_info.format = m_swap_chain_image_format;
            create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            create_info.subresourceRange.baseMipLevel = 0;
            create_info.subresourceRange.levelCount = 1;
            create_info.subresourceRange.baseArrayLayer = 0;
            create_info.subresourceRange.layerCount = 1;

            SHERPHY_EXCEPTION_IF_FALSE(vkCreateImageView(m_device, &create_info, nullptr, &m_swap_chain_image_views[i]) == VK_SUCCESS, "failed to create image views!");
        }
    }

    VkShaderModule TriangleApplication::createShaderModule(const std::vector<char>& code) 
    {
        VkShaderModuleCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = code.size();
        create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shader_module;
        SHERPHY_EXCEPTION_IF_FALSE(vkCreateShaderModule(m_device, &create_info, nullptr, &shader_module) == VK_SUCCESS, "failed to create shader module!");

        return shader_module;
    }

    void TriangleApplication::createRenderPassNormal() 
    {
        VkAttachmentDescription color_attachment{};
        color_attachment.format = m_swap_chain_image_format;
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        //LOWTODO stencil test
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


        VkAttachmentReference color_attachment_ref{};
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_ref;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount = 1;
        render_pass_info.pAttachments = &color_attachment;
        render_pass_info.subpassCount = 1;
        render_pass_info.dependencyCount = 1;
        render_pass_info.pDependencies = &dependency;
        render_pass_info.pSubpasses = &subpass;

        SHERPHY_EXCEPTION_IF_FALSE(vkCreateRenderPass(m_device, &render_pass_info, nullptr, &m_render_pass) == VK_SUCCESS, "failed to create render pass!");
        return;
    }
    
    void TriangleApplication::createGraphicsPipeline(PipeLineType type) {
        switch (type)
        {
        case PipeLineType::Normal:
            createGraphicsPipelineNormal();
            break;
        case PipeLineType::RayTracing:
            break;
        default:
            createGraphicsPipelineNormal();
            break;
        }
    }

    void TriangleApplication::createGraphicsPipelineNormal()
    {
        auto vert_shader_code = SPVReader::readFile("I:/SherphyEngine/resource/public/SherphyShaderLib/SPV/Normal/SimpleTestTriangle_vert.spv");
        auto frag_shader_code = SPVReader::readFile("I:/SherphyEngine/resource/public/SherphyShaderLib/SPV/Normal/SimpleTestTriangle_frag.spv");

        VkShaderModule vert_shader_module = createShaderModule(vert_shader_code);
        VkShaderModule frag_shader_module = createShaderModule(frag_shader_code);

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

            VkPipelineColorBlendStateCreateInfo colorBlending{};
            colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &color_blend_attachment;
            colorBlending.blendConstants[0] = 0.0f; // Optional
            colorBlending.blendConstants[1] = 0.0f; // Optional
            colorBlending.blendConstants[2] = 0.0f; // Optional
            colorBlending.blendConstants[3] = 0.0f; // Optional

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
            pipeline_info.pColorBlendState = &colorBlending;
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
    bool TriangleApplication::isDeviceSuitable(VkPhysicalDevice& device) 
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

        return indices.isComplete() && extension_supported && swap_chain_adequate && nv_device;
    }

    VkSurfaceFormatKHR TriangleApplication::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats) {
        for (VkSurfaceFormatKHR available_format : available_formats) {
            if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return available_format;
            }
        }
        return available_formats[0];
    }

    VkPresentModeKHR TriangleApplication::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D TriangleApplication::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        else {
            int width, height;
            glfwGetFramebufferSize(m_window, &width, &height);

            VkExtent2D actual_extent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actual_extent;
        }
    }

    SwapChainSupportDetails TriangleApplication::querySwapChainSupport(VkPhysicalDevice& device) 
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

    bool TriangleApplication::checkDeviceExtensionSupport(VkPhysicalDevice& device) 
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

    QueueFamilyIndices TriangleApplication::findQueueFamilies(VkPhysicalDevice& device) 
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

    void TriangleApplication::mainLoop()
    {
        while (!glfwWindowShouldClose(m_window))
        {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(m_device);
    }

    void TriangleApplication::createSyncObjects() 
    {
        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        SHERPHY_EXCEPTION_IF_FALSE(vkCreateSemaphore(m_device, &semaphore_info, nullptr, &m_image_available_semaphore) == VK_SUCCESS, "failed to create image semaphore for a frame!");
        SHERPHY_EXCEPTION_IF_FALSE(vkCreateSemaphore(m_device, &semaphore_info, nullptr, &m_render_finished_semaphore) == VK_SUCCESS, "failed to create render finish semaphore for a frame!");
        SHERPHY_EXCEPTION_IF_FALSE(vkCreateFence(m_device, &fence_info, nullptr, &m_in_flight_fence) == VK_SUCCESS, "failed to create fence for a frame!");
        return;
    }

    void TriangleApplication::drawFrame() 
    {
        vkWaitForFences(m_device, 1, &m_in_flight_fence, VK_TRUE, UINT64_MAX);
        vkResetFences(m_device, 1, &m_in_flight_fence);

        uint32_t image_index;
        vkAcquireNextImageKHR(m_device, m_swap_chain, UINT64_MAX, m_image_available_semaphore, VK_NULL_HANDLE, &image_index);

        vkResetCommandBuffer(m_command_buffer, /*VkCommandBufferResetFlagBits*/ 0);
        recordCommandBuffer(m_command_buffer, image_index);

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore wait_semaphores[] = { m_image_available_semaphore };
        VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;

        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &m_command_buffer;

        VkSemaphore signal_semaphores[] = { m_render_finished_semaphore };
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;

        SHERPHY_EXCEPTION_IF_FALSE(vkQueueSubmit(m_graphics_queue, 1, &submit_info, m_in_flight_fence) == VK_SUCCESS, "failed to submit draw command buffer!");

        VkPresentInfoKHR present_info{};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = signal_semaphores;

        VkSwapchainKHR swap_chains[] = { m_swap_chain };
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swap_chains;

        present_info.pImageIndices = &image_index;

        vkQueuePresentKHR(m_present_queue, &present_info);
    }

    void TriangleApplication::cleanUp()
    {
        vkDestroyCommandPool(m_device, m_command_pool, nullptr);
        for (auto frame_buffer : m_swap_chain_frame_buffers) {
            vkDestroyFramebuffer(m_device, frame_buffer, nullptr);
        }
        vkDestroyPipeline(m_device, m_graphics_pipeline, nullptr);
        vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
        vkDestroyRenderPass(m_device, m_render_pass, nullptr);
        for(auto image_view : m_swap_chain_image_views) {
            vkDestroyImageView(m_device, image_view, nullptr);
        }
        vkDestroySwapchainKHR(m_device, m_swap_chain, nullptr);
        vkDestroyDevice(m_device, nullptr);
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

        if (m_enable_validation_layer) {
            DestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
        }

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

