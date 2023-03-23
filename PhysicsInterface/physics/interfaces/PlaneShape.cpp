#include "PlaneShape.h"

namespace physics
{
	PlaneShape::PlaneShape(float dotProduct, const Vector3& normal)
		: iShape(ShapeType::Plane)
		, m_DotProduct(dotProduct)
		, m_Normal(normal)
	{ }

	//PlaneShape::PlaneShape(const Vector3& a, const Vector3& b, const Vector3& c)
	//	: iShape(ShapeTypePlane)
	//{ 
	//	// TODO glm not included at the moment
	//}

	PlaneShape::~PlaneShape()
	{ }

	const Vector3& PlaneShape::GetNormal() const
	{
		return m_Normal;
	}
	float PlaneShape::GetDotProduct() const
	{
		return m_DotProduct;
	}

	PlaneShape* PlaneShape::Cast(iShape* shape)
	{
		return dynamic_cast<PlaneShape*>(shape);
	}
}