#pragma once

#include <engine/ecs/Component.h>
#include <physics/bodies/rigidbody.h>

struct RigidBodyComponent : public Component
{
	RigidBody* rigidBody;

	void SetRigidBody(RigidBody* body) { rigidBody = body; }
	virtual void Enable() override;
	virtual void Disable() override;
};