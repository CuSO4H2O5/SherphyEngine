#pragma once
#include "Soul/PreCompile/SoulGlobal.h"

#include "Soul/Math/MathPack.h"
#include "Component.h"

#include <vector>
#include <map>
#include <unordered_set>
#include <type_traits>
#include <typeinfo>


namespace Sherphy 
{
	struct Object {};
	using SOBJ_ID = size_t;

	//struct SceneObject : public Object 
	//{
	//	std::unordered_set<ComponentType> m_components;
	//};

	struct Camera 
	{
		Vec3 pos;
		Vec3 target;
		Vec3 up;
		float fov;
		float aspect;
		float znear;
		float zfar;
	};

	class PlaneCamera : public Object
	{
		uint32_t width = 800, height = 600;
	};
}