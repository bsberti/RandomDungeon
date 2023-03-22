#pragma once

#include "ShapeType.h"

namespace physics
{
	class iShape
	{
	public:
		virtual ~iShape() {}

		ShapeType GetShapeType() const {
			return m_ShapeType;
		}

	protected:
		iShape(ShapeType shapeType)
			: m_ShapeType(shapeType)
		{ }

	private:
		ShapeType m_ShapeType;

		iShape(const iShape&) {}
		iShape& operator=(const iShape&) {}
	};
}
