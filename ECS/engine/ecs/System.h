#pragma once

#include <vector>

#include <engine/ecs/Entity.h>

class System
{
public:
	virtual void Process(const std::vector<Entity*>& entities, float dt) = 0;
};