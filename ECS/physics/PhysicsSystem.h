#pragma once

#include <engine/ecs/System.h>

class PhysicsWorld;
class PhysicsSystem : public System
{
public:
	PhysicsSystem() { }

	virtual ~PhysicsSystem() { }

	virtual void Process(const std::vector<Entity*>& entities, float dt) override;
};