#pragma once

#include <physics/bodies/collisionbody.h>

#include <physics/shapes/shape.h>

class RigidBody : public CollisionBody
{
public:
	RigidBody();
	virtual ~RigidBody();

	void AddToWorld();
	void RemoveFromWorld();

	Shape* m_Shape;
};