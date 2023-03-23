#pragma once

#include "iShape.h"
#include "Math.h"

namespace physics
{
	class BoxShape : public iShape
	{
	public:
		BoxShape(const Vector3& halfExtents);
		virtual ~BoxShape();

		const Vector3& GetHalfExtents() const;

		static BoxShape* Cast(iShape* shape);

	protected:
		BoxShape(ShapeType shapeType)
			: iShape(shapeType) {}

	private:
		Vector3 m_HalfExtents;
		BoxShape(const BoxShape&) : iShape(ShapeType::Box) {}
		BoxShape& operator=(const BoxShape&) {
			return *this;
		}
	};
}