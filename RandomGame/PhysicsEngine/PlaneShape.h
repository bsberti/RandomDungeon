#pragma once

#include "iShape.h"
#include "Math.h"
#include "Vector3.h"

namespace physics
{
	class PlaneShape : public iShape
	{
	public:
		PlaneShape(float dotProduct, const Vector3& normal);
		//PlaneShape(const Vector3& a, const Vector3& b, const Vector3& c);
		virtual ~PlaneShape();

		float GetDotProduct() const;
		const Vector3& GetNormal() const;

		static PlaneShape* Cast(iShape* shape);

	protected:
		PlaneShape(ShapeType shapeType)
			: iShape(shapeType) {}

	private:
		Vector3 m_Normal;
		float m_DotProduct;

		PlaneShape(const PlaneShape&) : iShape(ShapeType::Plane) {}
		PlaneShape& operator=(const PlaneShape&) {
			return *this;
		}
	};
}
