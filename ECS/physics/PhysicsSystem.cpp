#include <physics/PhysicsSystem.h>

#include <physics/utils/PhysicsUtils.h>
#include <physics/physicsworld.h>

// Uses the Physics World's simulate to update all of the
// RigidBodies that are in the physics world

void PhysicsSystem::Process(const std::vector<Entity*>& entities, float dt)
{
	PhysicsWorld* world = PhysicsUtils::GetActiveWorld();
	if (world != nullptr)
	{
		world->SimulateStep(dt);
	}
}