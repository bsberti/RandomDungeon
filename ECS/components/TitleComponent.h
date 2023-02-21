#pragma once

#include <engine/ecs/Component.h>

struct TitleComponent : public Component {
	const char* title;
};
