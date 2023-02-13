#include "EntityLoaderManager.h"

#ifdef _DEBUG
#define DEBUG_LOG_ENABLED
#endif
#ifdef DEBUG_LOG_ENABLED
#define DEBUG_PRINT(x, ...) printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#endif

EntityLoaderManager* EntityLoaderManager::m_EntityLoaderManager = nullptr;

EntityLoaderManager::EntityLoaderManager() {
	DEBUG_PRINT("EntityLoaderManager::EntityLoaderManager()\n");
}

EntityLoaderManager::~EntityLoaderManager() {
	DEBUG_PRINT("EntityLoaderManager::~EntityLoaderManager()\n");
}

EntityLoaderManager* EntityLoaderManager::GetInstance() {
	DEBUG_PRINT("EntityLoaderManager::GetInstance()\n");
	if (m_EntityLoaderManager == nullptr) {
		m_EntityLoaderManager = new EntityLoaderManager();
	}
	return m_EntityLoaderManager;
}

bool EntityLoaderManager::LoadGameObject(const std::string path, GameObject& gameObject, std::string& errorMsg) {
	DEBUG_PRINT("EntityLoaderManager::LoadGameObject(%s, %s, %s)\n", path.c_str(), "gameObject", errorMsg.c_str());
	return true;
}
