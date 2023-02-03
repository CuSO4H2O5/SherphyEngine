#include <EASTL/bonus/ring_buffer.h>
#include <string>

namespace SherphyEngine(Miracle){
    // TODO Sherphy RingBuffer
    //template<typename T>
    //class RingBuffer 
    //{
    //    eastl::ring_buffer
    //};
    
    typedef eastl::ring_buffer<std::string> RingBufferString;
    typedef eastl::ring_buffer<int> RingBufferInt;
}