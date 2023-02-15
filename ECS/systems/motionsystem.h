#pragma once

#include <engine/ecs/System.h>

class MotionSystem : public System
{
public:
	virtual void Process(const std::vector<Entity*>& entities, float dt) override;
};
