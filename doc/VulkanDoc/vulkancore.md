
``` c++
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)									
{																									
	for (int32_t i = 0; i < __argc; i++) { VulkanExample::args.push_back(__argv[i]); };  			
	vulkanExample = new VulkanExample();															
	vulkanExample->initVulkan();																	
	vulkanExample->setupWindow(hInstance, WndProc);													
	vulkanExample->prepare();																		
	vulkanExample->renderLoop();																	
	delete(vulkanExample);																			
	return 0;																						
}
```