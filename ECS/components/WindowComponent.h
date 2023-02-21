#pragma once

#include <engine/ecs/Component.h>
#include <GLFW/glfw3.h>

struct WindowComponent : public Component {
	GLFWwindow* window;
};
