#pragma once

#include <engine/ecs/Component.h>

struct MeshRenderer : public Component
{
	unsigned int shaderId;
	unsigned int vbo;
	unsigned int numTriangles;
};