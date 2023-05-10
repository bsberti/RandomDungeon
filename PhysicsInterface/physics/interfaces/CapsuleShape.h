#pragma once

#include "iShape.h"

namespace physics
{
	class CapsuleShape : public iConvexShape
	{
	public:
		CapsuleShape(float radius, float height);
		virtual ~CapsuleShape();

		float GetRadius() const;
		float GetHeight() const;

		static CapsuleShape* Cast(iShape* shape);

	protected:
		CapsuleShape(ShapeType shapeType)
			: iConvexShape(shapeType) {}

	private:
		float m_Radius;
		float m_Height;

		CapsuleShape(const CapsuleShape&) : iConvexShape(ShapeType::Capsule) {}
		CapsuleShape& operator=(const CapsuleShape&) {
			return *this;
		}
	};
}
