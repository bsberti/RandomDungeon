#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "WindowManager.h"
#include <components/TitleComponent.h>
#include <components/PositionComponent.h>
#include <components/SizeComponent.h>
#include <components/WindowComponent.h>
#include <components/GLSLVersionComponent.h>


WindowManager::WindowManager() {
}

WindowManager::~WindowManager() {
}

bool WindowManager::CreateWindowEntity(EntityManager& entityManager, const unsigned int entity, const char* title
	, int width, int height, const char* glslVersion) {
	TitleComponent*			titleComponent		 = entityManager.AddComponent<TitleComponent>(entity);
	PositionComponent*		positionComponent	 = entityManager.AddComponent<PositionComponent>(entity);
	SizeComponent*			sizeComponent		 = entityManager.AddComponent<SizeComponent>(entity);
	WindowComponent*		windowComponent		 = entityManager.AddComponent<WindowComponent>(entity);
	GLSLVersionComponent*	glslVersionComponent = entityManager.AddComponent<GLSLVersionComponent>(entity);

	titleComponent->title = title;
	positionComponent->x = 0;
	positionComponent->y = 0;
	positionComponent->z = 0;
	sizeComponent->width = width;
	sizeComponent->height = height;
	glslVersionComponent->glslVersion = glslVersion;

	if (!glfwInit())
		return false;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	windowComponent->window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!windowComponent->window) {
		return false;
	}

	return true;
}
