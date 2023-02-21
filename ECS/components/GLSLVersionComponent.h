#pragma once

#include <engine/ecs/Component.h>

struct GLSLVersionComponent : public Component {
	const char* glslVersion;
};