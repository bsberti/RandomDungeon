#include "EntityLoaderManager.h"
#include <iostream>

int main(int argc, char* argv[]) {

	EntityLoaderManager* entityLoaderManager = EntityLoaderManager::GetInstance();
	GameObject gameObject;
	std::string pathJSONFile = "entityLoader.json";
	std::string errorMessage;
	bool loaderReturn = entityLoaderManager->LoadMainCharacter(pathJSONFile, gameObject, errorMessage);

	if (loaderReturn) {
		std::cout << "friendlyName: " << gameObject.m_friendlyName << std::endl;
		std::cout << "position(x,y,z): (" << gameObject.m_position[0] << ", " << gameObject.m_position[1] << ", " << gameObject.m_position[2] << ")" << std::endl;
		std::cout << "meshFilePath: " << gameObject.m_meshFilePath << std::endl;
	} else {
		std::cout << "Error: " << errorMessage << std::endl;
	}

	delete entityLoaderManager;
	return 0;
}