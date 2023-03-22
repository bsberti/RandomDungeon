#pragma once

#include "iShape.h"
#include "Math.h"
#include "Vector3.h"

namespace physics {
	class AABBShape : public iShape {
	public:
		float Min[3];
		float Max[3];

		const Vector3& GetNormal() const;

		AABBShape(float min[3], float max[3], const Vector3& normal);
		virtual ~AABBShape();

		static AABBShape* Cast(iShape* shape);

	protected:
		AABBShape(ShapeType shapeType)
			: iShape(shapeType) {
		}

	private:
		Vector3 m_Normal;

		AABBShape(const AABBShape&) : iShape(ShapeType::AABB) {}
		AABBShape& operator=(const AABBShape&) {
			return *this;
		}
	};
}
