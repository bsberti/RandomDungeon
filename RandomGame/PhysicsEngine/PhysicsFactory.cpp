#include "PhysicsFactory.h"

#include "PhysicsWorld.h"
#include "CollisionListener.h"
#include "RigidBody.h"
#include "SoftBody.h"

namespace physics
{
	PhysicsFactory::PhysicsFactory()
		: iPhysicsFactory()
	{
		printf("Created GDP Physics Factory!\n");
	}

	PhysicsFactory::~PhysicsFactory()
	{ }

	iPhysicsWorld* PhysicsFactory::CreateWorld()
	{
		return new PhysicsWorld();
	}

	iRigidBody* PhysicsFactory::CreateRigidBody(const RigidBodyDesc& desc, iShape* shape)
	{
		return new RigidBody(desc, shape);
	}

	iSoftBody* PhysicsFactory::CreateSoftBody(const SoftBodyDesc& desc)
	{
		return new SoftBody(desc);
	}
	iCollisionListener* PhysicsFactory::CreateCollisionListener() {
		return new CollisionListener();
	}
}