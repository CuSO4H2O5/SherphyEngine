#include "LogMessager.h"
//typedef bool SBool;
//
//const SBool k_false = false;
//const SBool k_true = true;
namespace SherphyEngine(Miracle) {
    #ifdef SHERPHY_DEBUG
    #define SherphyAssert(_Expression) ((void) 0)
    #else
    #define SherphyAssert(_Expression)
    #endif

    #define SHERPHY_EXCEPTION_IF_FALSE(x, log) \
        if(!x) \
        {   \
            throw std::runtime_error(log); \
        } \


    #define SHERPHY_RETURN_FALSE_IF_NULL(x, log) \
        if(x == nullptr) \
        {   \
            return false; \
        } \

}