#include "EntityLoaderManager.h"
#include <iostream>

int main(int argc, char* argv[]) {

	EntityLoaderManager* entityLoaderManager = EntityLoaderManager::GetInstance();
	cCharacter mainCharacter;
	std::string pathJSONFile = "MainCharacter.json";
	std::string errorMessage;
	bool loaderReturn = entityLoaderManager->LoadCharacter(pathJSONFile, mainCharacter, errorMessage);

	if (loaderReturn) {
		std::cout << "friendlyName: " << mainCharacter.mFriendlyName << std::endl;
		std::cout << "position(x,y,z): (" << mainCharacter.mPosition[0] << ", " << mainCharacter.mPosition[1] << ", " << mainCharacter.mPosition[2] << ")" << std::endl;
		std::cout << "meshFilePath: " << mainCharacter.mMeshFilePath << std::endl;
		std::cout << "Char Name: " << mainCharacter.mName << std::endl;
		std::cout << "Level: " << mainCharacter.mLevel << std::endl;
		std::cout << "Max Health: " << mainCharacter.mMaxHealth << std::endl;
		std::cout << "Current Health: " << mainCharacter.mCurrentHealth << std::endl;
		std::cout << "Max Mana: " << mainCharacter.mMaxMana << std::endl;
		std::cout << "Current Mana: " << mainCharacter.mCurrentMana << std::endl;
	} else {
		std::cout << "Error: " << errorMessage << std::endl;
	}

	delete entityLoaderManager;
	return 0;
}