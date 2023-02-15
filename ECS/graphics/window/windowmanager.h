#pragma once

class WindowManager {
public:
	WindowManager();
	~WindowManager();

	unsigned int CreateWindowEntity(const unsigned int entity, const char* title, int width, int height);
};