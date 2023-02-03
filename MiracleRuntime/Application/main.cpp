#include "JadeBreaker/RHI/vulkan_rhi.h"


int main()
{
    SherphyMiracle::TriangleApplication app;
        
    try
    {
        app.run();
    } catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
