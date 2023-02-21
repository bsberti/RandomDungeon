#pragma once
#include <engine/entity/Entitymanager.h>

class WindowManager {
public:
	WindowManager();
	~WindowManager();

	bool CreateWindowEntity(EntityManager& entityManager, const unsigned int entity, const char* title
							, int width, int height, const char* glslVersion);
};