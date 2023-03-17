#include "SherphyAllocatorCallBack.h"

namespace Sherphy 
{
    static DefaultAllocatorCallback g_defaultAllocatorCallback;

    //////// Global API implementation ////////
    SherphyAllocatorCallBack* SherphyGetAllocatorCallback() {
        return &g_defaultAllocatorCallback;
    }
}