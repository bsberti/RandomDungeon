#include "CapsuleShape.h"

namespace physics
{
	CapsuleShape::CapsuleShape(float radius, float height)
		: iConvexShape(ShapeType::Capsule)
		, m_Radius(radius)
		, m_Height(height)
	{ }

	CapsuleShape::~CapsuleShape()
	{ }

	CapsuleShape* CapsuleShape::Cast(iShape* shape)
	{
		return dynamic_cast<CapsuleShape*>(shape);
	}

	float CapsuleShape::GetRadius() const
	{
		return m_Radius;
	}

	float CapsuleShape::GetHeight() const
	{
		return m_Height;
	}
}