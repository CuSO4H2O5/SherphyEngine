#pragma once 
#include "Soul/Math/Vector.h"
#include "Soul/Math/Quaternion.h"
#include <vector>

namespace Sherphy 
{
	enum class ComponentType
	{
		normal,
		position,
		rotation,
		rendermesh,
		light,
		camera
	};

	struct Component 
	{
	public:
		ComponentType getTypeid() 
		{
			return m_id;
		};
		Component(ComponentType _type) : m_id(_type) {};
	protected:
		ComponentType m_id{ ComponentType::normal };
	};

	struct PositionComponent : public Component
	{
		PositionComponent() : Component(ComponentType::position)
		{
			x = 0;
			y = 0;
			z = 0;
		}
		PositionComponent(float _x, float _y, float _z): x(x), y(y), z(z),Component(ComponentType::position)
		{
		}
		union {
			Vec3 pos;
			struct { float x, y, z; };
		};
	};

	struct RotationComponent : public Component
	{
		RotationComponent(): Component(ComponentType::rotation)
		{
			qua.data = { 1, 0, 0, 0 };
		};
		RotationComponent(float _w, float _x, float _y, float _z) : w(_w), x(_x), y(_y), z(_z), Component(ComponentType::rotation)
		{
		};
		union {
			Quaternion qua;
			struct { float w, x, y, z; };
		};
	};

	struct LightComponent : public Component 
	{
		LightComponent() : Component(ComponentType::light){
		}
	};

	struct RenderMeshComponent : Component 
	{
		RenderMeshComponent() : Component(ComponentType::rendermesh) 
		{
		}
		std::vector<Vertex> m_vertices{};
		std::vector<uint32_t> m_indices{};
	};

	struct PhysicalComponent : Component 
	{

	};

	struct SimpleObjectComponent : public Component
	{
		enum struct SimpleObjectType
		{
			Box,
			Sphere,
			Plane
		};

		struct SimpleObject {};

		struct Sphere : public SimpleObject
		{
			union { float r, range; };
		};

		struct Box : public SimpleObject
		{
			union { struct { float w, h; }; Vec2 data; };
		};
	public:
		SimpleObjectType m_type;
		SimpleObject m_object;
	};
}