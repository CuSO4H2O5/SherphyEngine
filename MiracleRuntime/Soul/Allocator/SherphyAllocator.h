#pragma once
namespace Sherphy
{
    class SpAllocator
    {
        public:
            SpAllocator(const char* name = nullptr);

            virtual ~SpAllocator();

            void* Allocate(size_t size, size_t alignment);

    };
} // namespace SherphyEngine(Miracle)
