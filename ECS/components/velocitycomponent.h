#pragma once

#include <engine/ecs/Component.h>

struct VelocityComponent : public Component
{
	float vx, vy, vz;
};
