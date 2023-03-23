#include "BoxShape.h"

namespace physics
{
	BoxShape::BoxShape(const Vector3& halfExtents)
		: iShape(ShapeType::Box)
		, m_HalfExtents(halfExtents)
	{ }

	BoxShape::~BoxShape()
	{ }

	const Vector3& BoxShape::GetHalfExtents() const
	{
		return m_HalfExtents;
	}

	BoxShape* BoxShape::Cast(iShape* shape)
	{
		return dynamic_cast<BoxShape*>(shape);
	}
}
