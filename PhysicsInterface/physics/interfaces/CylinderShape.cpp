#include "CylinderShape.h"

namespace physics
{
	CylinderShape::CylinderShape(const Vector3& halfExtents)
		: iShape(ShapeType::Cylinder)
		, m_HalfExtents(halfExtents)
	{ }

	CylinderShape::~CylinderShape()
	{ }

	const Vector3& CylinderShape::GetHalfExtents() const
	{
		return m_HalfExtents;
	}

	CylinderShape* CylinderShape::Cast(iShape* shape)
	{
		return dynamic_cast<CylinderShape*>(shape);
	}
}