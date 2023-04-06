#pragma once
//#include <detail/qualifier.hpp>
#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"


namespace Sherphy 
{
	template<glm::length_t _val, typename _type = float, glm::qualifier Q = glm::defaultp>
	using vec = glm::vec<_val, _type, Q>;

	typedef vec<4> Vec4;
	typedef vec<3> Vec3;
	typedef vec<2> Vec2;
}