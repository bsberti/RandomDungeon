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

		iShape(const iShape&) = delete;
		iShape& operator=(const iShape&) {}
	};

	class iConvexShape : public iShape
	{
	public:
		virtual ~iConvexShape() {}

	protected:
		iConvexShape(ShapeType shapeType)
			: iShape(shapeType)
		{ }

	private:
		iConvexShape(const iConvexShape&) = delete;
		iConvexShape& operator=(const iConvexShape&) {}
	};

	class iConcaveShape : public iShape
	{
	public:
		virtual ~iConcaveShape() {}

	protected:
		iConcaveShape(ShapeType shapeType)
			: iShape(shapeType)
		{ }

	private:
		iConcaveShape(const iConcaveShape&) = delete;
		iConcaveShape& operator=(const iConcaveShape&) {}
	};
}
