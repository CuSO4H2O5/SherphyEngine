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
    #define ERROR_MARK(log) static_cast<std::string>(__FILE__) + "in line"+ std::to_string(__LINE__) + log

    #define SHERPHY_ASSERT(x, checked, log) \
    {\
        if(x != checked)\
        {\
            LogMessage(ERROR_MARK(log), WarningStage::Fatal);\
            assert(x == checked);\
        }\
    }\

    #define SHERPHY_EXCEPTION_IF_FALSE(x, log) \
        if(!x) \
        {   \
            throw std::runtime_error(ERROR_MARK(log)); \
        } \

    #define SHERPHY_RETURN_NULLPTR_IF_FALSE_WITH_LOG_ERROR(x, log) \
        if(!x) \
        { \
            LogMessage(ERROR_MARK(log), WarningStage::Medium); \
            return nullptr; \
        } \

    #define SHERPHY_RETURN_FALSE_IF_NULL(x, log) \
        if(x == nullptr) \
        {   \
            LogMessage(ERROR_MARK(log), WarningStage::Medium); \
            return false; \
        } \

    #define SHERPHY_RETURN_IF_FALSE(x, log) \
        if(!x) \
        {   \
            LogMessage(ERROR_MARK(log), WarningStage::Medium); \
            return; \
        } \

    #define SHERPHY_CONTINUE_WITH_LOG(x, log) \
        if(x == NULL) \
        {   \
            LogMessage(ERROR_MARK(log), WarningStage::Low); \
            continue; \
        } \
    
    #define SHERPHY_RENDERING_LOG(log) LogMessage("validation layer: " + static_cast<std::string>(log), WarningStage::Normal);

    #define SHERPHY_LOG(log) LogMessage(ERROR_MARK(log), WarningStage::Normal);

    #define SHERPHY_ALLOC(_Size) SherphyGetAllocatorCallback()->allocate(_Size, nullptr, __FILE__, __LINE__);
    #define SHERPHY_DEALLOC(_Ptr) SherphyGetAllocatorCallback()->deallocate(_Ptr);
    #define SHERPHY_MEMCPY(_Dst, _Src, _Size) SherphyGetAllocatorCallback()->_memcpy(_Dst, _Src, _Size);
}