#pragma once

#include <engine/ecs/Component.h>
#include <GLFW/glfw3.h>

class WindowProperties : public Component {
	GLFWwindow* window;
	int			width;
	int			height;
	const char* title;
};