#include "WindowManager.h"
#include "WindowProperties.h"

WindowManager::WindowManager() {
}

WindowManager::~WindowManager() {
}

unsigned int WindowManager::CreateWindowEntity(const unsigned int entity, const char* title, int width, int height) {
	//WindowProperties* wp = g_Engine.AddComponent<WindowProperties>(entity);
	//rbc->SetRigidBody();
	//rbc->SetPhysicsMaterial();
	// Some way to customize the rbc
	// You want to assign the shape yourself, and collider info


	return 0;
}
