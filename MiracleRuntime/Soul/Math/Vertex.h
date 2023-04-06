#pragma once

#include "Vector.h"

namespace Sherphy 
{
	struct Vertex 
	{
		Vec3 pos;
		Vec3 color;
		Vec2 tex_coord;

		bool operator==(const Vertex& other) const {
			return pos == other.pos && color == other.color && tex_coord == other.tex_coord;
		}
	};
}