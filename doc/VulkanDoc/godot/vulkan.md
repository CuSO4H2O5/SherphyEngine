

# platform control



platform层主要修改window_create

<drivers/vulkan/vulkan_context.cpp>
VulkanContext::initialize()



platform层判断选用哪种渲染语言

<servers/display_server.h>
register_create_function -> push_back server_create_functions[MAX_SERVERS] :: MAX_SERVERS = 64


<platform/windows/display_server_windows.h>
create context_vulkan
create rendering_driver_vulkan to initialize

#if defined(VULKAN_ENABLED)

	if (rendering_driver == "vulkan") {
		rendering_device_vulkan = memnew(RenderingDeviceVulkan);
		rendering_device_vulkan->initialize(context_vulkan);

		RendererCompositorRD::make_current();
	}
#endif
<platform/windows/vulkan_context_win.h>
VulkanContextWindows : public VulkanContext
