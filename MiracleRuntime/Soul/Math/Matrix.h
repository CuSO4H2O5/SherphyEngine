#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <matrix.hpp>


namespace Sherphy 
{
	template<glm::length_t _col, glm::length_t _row, typename _type = float, glm::qualifier Q = glm::defaultp>
	using mat = glm::mat<_col, _row, _type, Q>;

	typedef mat<4, 4> Mat4x4;
	typedef mat<3, 3> Mat3x3;
	typedef mat<2, 2> Mat2x2;
}