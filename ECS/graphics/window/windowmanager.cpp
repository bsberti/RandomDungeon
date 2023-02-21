#include "WindowManager.h"
#include <components/TitleComponent.h>
#include <components/PositionComponent.h>
#include <components/SizeComponent.h>
#include <components/WindowComponent.h>


WindowManager::WindowManager() {
}

WindowManager::~WindowManager() {
}

bool WindowManager::CreateWindowEntity(EntityManager& entityManager, const unsigned int entity, const char* title, int width, int height) {
	TitleComponent*		titleComponent		= entityManager.AddComponent<TitleComponent>(entity);
	PositionComponent*	positionComponent	= entityManager.AddComponent<PositionComponent>(entity);
	SizeComponent*		sizeComponent		= entityManager.AddComponent<SizeComponent>(entity);
	WindowComponent*	windowComponent		= entityManager.AddComponent<WindowComponent>(entity);

	titleComponent->title = title;
	positionComponent->x = 0;
	positionComponent->y = 0;
	positionComponent->z = 0;
	sizeComponent->width = width;
	sizeComponent->height = height;
	windowComponent->window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!windowComponent->window) {
		return false;
	}

	return true;
}
