#pragma once
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
            virtual void _memcpy(void* _dst, void const* _src, size_t _size) = 0;
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

        virtual void _memcpy(void* _dst,void const* _src, size_t _size) override
        {
            memcpy(_dst, _src, _size);
        }
    };
    SherphyAllocatorCallBack* SherphyGetAllocatorCallback();
}


