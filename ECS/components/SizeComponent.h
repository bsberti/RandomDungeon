#pragma once

#include <engine/ecs/Component.h>

struct SizeComponent : public Component {
	float width, height;
};