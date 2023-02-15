#include <physics/bodies/rigidbody.h>

#include <physics/utils/PhysicsUtils.h>
#include <physics/PhysicsWorld.h>

RigidBody::RigidBody()
	: m_Shape(nullptr)
{
}

RigidBody::~RigidBody()
{
}

void RigidBody::AddToWorld()
{
	PhysicsWorld* world = PhysicsUtils::GetActiveWorld();
	if (world != nullptr)
	{
		world->AddToWorld(this);
	}
}

void RigidBody::RemoveFromWorld()
{
	PhysicsWorld* world = PhysicsUtils::GetActiveWorld();
	if (world != nullptr)
	{
		world->RemoveFromWorld(this);
	}
}