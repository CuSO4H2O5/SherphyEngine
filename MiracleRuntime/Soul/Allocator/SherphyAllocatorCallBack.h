#include <cstdio>
#include <malloc.h>

namespace Sherphy
{
    class SherphyAllocatorCallBack
    {
        public:
            virtual ~SherphyAllocatorCallBack() {};
            virtual void* allocate(size_t size, const char* typeName, const char* filename, int line) = 0;
            virtual void deallocate(void* ptr) = 0;
    };

    class DefaultAllocatorCallback : public SherphyAllocatorCallBack
    {
    public:
        virtual void* allocate(size_t size, [[unused]] const char* typeName, [[unused]] const char* filename, [[unused]] int line) override
        {
            return _aligned_malloc(size, 16);
        }

        virtual void deallocate(void* ptr) override
        {
            _aligned_free(ptr);
        }
    };
    DefaultAllocatorCallback g_defaultAllocatorCallback;

    SherphyAllocatorCallBack* g_SherphyAllocatorCallBack = &g_defaultAllocatorCallback;
}

//////// Global API implementation ////////
Sherphy::SherphyAllocatorCallBack* SherphyGetAllocatorCallback(){
    return Sherphy::g_SherphyAllocatorCallBack;
}
