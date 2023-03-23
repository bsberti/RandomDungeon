#pragma once

#include "iShape.h"
#include "Math.h"

namespace physics
{
	class CylinderShape : public iShape
	{
	public:
		CylinderShape(const Vector3& halfExtents);
		virtual ~CylinderShape();

		const Vector3& GetHalfExtents() const;

		static CylinderShape* Cast(iShape* shape);

	protected:
		CylinderShape(ShapeType shapeType)
			: iShape(shapeType) {}

	private:
		Vector3 m_HalfExtents;
		CylinderShape(const CylinderShape&) : iShape(ShapeType::Cylinder) {}
		CylinderShape& operator=(const CylinderShape&) {
			return *this;
		}
	};
}
