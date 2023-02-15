#include "PhysicsUtils.h"

#include <physics/physicsworld.h>

PhysicsWorld* PhysicsUtils::GetActiveWorld()
{
	return s_ActivePhysicsWorld;
}

void PhysicsUtils::SetActiveWorld(PhysicsWorld* world)
{
	s_ActivePhysicsWorld = world;
}