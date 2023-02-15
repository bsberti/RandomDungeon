#pragma once

#include <engine/ecs/Component.h>

struct PositionComponent : public Component
{
	float x, y, z;
};