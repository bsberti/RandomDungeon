#include "rigidbodycomponent.h"

void RigidBodyComponent::Enable()
{
	if (rigidBody != nullptr)
	{
		rigidBody->AddToWorld();
	}
}

void RigidBodyComponent::Disable()
{
	if (rigidBody != nullptr)
	{
		rigidBody->RemoveFromWorld();
	}
}