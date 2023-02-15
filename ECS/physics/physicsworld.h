#pragma once

#include <physics/bodies/rigidbody.h>

class PhysicsWorld
{
public:
	void AddToWorld(RigidBody* body);
	void RemoveFromWorld(RigidBody* body);
	void SimulateStep(float dt) { }
};