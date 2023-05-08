#pragma once
#include "Soul/LogMessager.h"
#include "Soul/Math/MathPack.h"
#include "Soul/Allocator/SherphyAllocatorCallBack.h"
#include <stdexcept>
typedef void (*func_ptr)();
//
//const SBool k_false = false;
//const SBool k_true = true;
namespace Sherphy{
    #ifdef SHERPHY_DEBUG
    #define SherphyAssert(_Expression) if(_Expression) ((void) 0)
    #else
    #define SherphyAssert(_Expression) assert(_Expression);
    #endif

    #define SHERPHY_EXCEPTION_IF_FALSE(x, log) \
        if(!x) \
        {   \
            throw std::runtime_error(log); \
        } \

    #define SHERPHY_RETURN_NULLPTR_IF_FALSE_WITH_LOG_ERROR(x, log) \
        if(!x) \
        { \
            LogMessage(log, WarningStage::Medium); \
            return nullptr; \
        } \

    #define SHERPHY_RETURN_FALSE_IF_NULL(x, log) \
        if(x == nullptr) \
        {   \
            LogMessage(log, WarningStage::Medium); \
            return false; \
        } \

    #define SHERPHY_RETURN_IF_FALSE(x, log) \
        if(!x) \
        {   \
            LogMessage(log, WarningStage::Medium); \
            return; \
        } \

    #define SHERPHY_CONTINUE_WITH_LOG(x, log) \
        if(x == NULL) \
        {   \
            LogMessage(log, WarningStage::Low); \
            continue; \
        } \
    
    #define SHERPHY_RENDERING_LOG(log) LogMessage("validation layer: " + static_cast<std::string>(log), WarningStage::Normal);

    #define SHERPHY_LOG(log) LogMessage(static_cast<std::string>(log), WarningStage::Normal);

    #define SHERPHY_ALLOC(_Size) SherphyGetAllocatorCallback()->allocate(_Size, nullptr, __FILE__, __LINE__);
    #define SHERPHY_DEALLOC(_Ptr) SherphyGetAllocatorCallback()->deallocate(_Ptr);
    #define SHERPHY_MEMCPY(_Dst, _Src, _Size) SherphyGetAllocatorCallback()->_memcpy(_Dst, _Src, _Size);
}