



#ifdef SHERPHY_DEBUG
#define SherphyAssert(_Expression) ((void) 0)
#endif


#define SHERPHY_EXCEPTION_IF_FALSE(x, log) \
    if(!x) \
    {   \
        throw std::runtime_error(log); \
    } \

