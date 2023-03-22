#include "SphereShape.h"

namespace physics
{
	SphereShape::SphereShape(float radius)
		: iShape(ShapeType::Sphere)
		, m_Radius(radius)
	{ }

	SphereShape::~SphereShape()
	{ }

	SphereShape* SphereShape::Cast(iShape* shape)
	{
		return dynamic_cast<SphereShape*>(shape);
	}

	float SphereShape::GetRadius() const
	{
		return m_Radius;
	}
}