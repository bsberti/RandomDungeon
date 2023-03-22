#include "AABBShape.h"

#include <memory>

namespace physics {
	const Vector3& AABBShape::GetNormal() const
	{
		return m_Normal;
	}

	AABBShape::AABBShape(float min[3], float max[3], const Vector3& normal)
		: iShape(ShapeType::AABB) {
		memcpy(&(Min[0]), &(min[0]), 3 * sizeof(float));
		memcpy(&(Max[0]), &(max[0]), 3 * sizeof(float));
		m_Normal = normal;
	}

	AABBShape::~AABBShape() {
	}

	AABBShape* AABBShape::Cast(iShape* shape) {
		return dynamic_cast<AABBShape*>(shape);
	}
}