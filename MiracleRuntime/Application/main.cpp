#include "JadeBreaker/RHI/VulkanRHI.h"
#include "JadeBreaker/Display/GLFWDisplay.h"
#include "GameEngine.h"
#define VulkanBackEnd

//namespace Sherphy{
//    class Application 
//    {
//        void run() 
//        {
//            
//            display.init();
//
//        }
//        SceneLoader loader;
//        GLFWDisplay display;
//    };
//}
int main()
{
    Sherphy::GameEngine engine;
    //Sherphy::GLFWDisplay display;
        
    try
    {
        engine.init();
        engine.start();
        engine.shutdown();
    } catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
