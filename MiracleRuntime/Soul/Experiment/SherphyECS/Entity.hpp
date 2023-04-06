#pragma once
#ifndef SHERPHY_ID_TYPE
#include <cstdint>
#define SHERPHY_ID_TYPE uint32_t;
#endif // SHERPHY_ID_TYPE

#include <type_traits>

namespace Sherphy 
{
	using id_type = SHERPHY_ID_TYPE;
}