#include <iostream>
#include <fstream>
#include <sstream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
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

bool EntityLoaderManager::LoadGameObject(const std::string path, cGameObject& gameObject, std::string& errorMsg) {
	DEBUG_PRINT("EntityLoaderManager::LoadGameObject(%s, %s)\n", path.c_str(), "cGameObject");

	// JSON File
	std::ifstream file(path);
	// JSON File String Buffer
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content = buffer.str();

	// RapidJSON Document
	rapidjson::Document doc;
	// Parsing the string File to the Document
	doc.Parse(content.c_str());

	// Gets the Name of the Class
	const rapidjson::Value& className = doc["className"];
	// Checks if JSON File information matches the desired Game Object
	if (std::strcmp(className.GetString(), "GameObject") != 0) {
		errorMsg = "JSON File doesn't contain data for cGameObject";
		return false;
	}
	// Gets all Class attributes
	const rapidjson::Value& attributes = doc["attributes"];

	// Reads Friendly Name
	std::string attrName = attributes[0]["name"].GetString();
	// Checks if JSON File attribute name matches the desired variable
	if (attrName != "friendlyName") {
		errorMsg = "JSON File first attribute isn't friendly name";
		return false;
	}
	// Saves the Friendly Name
	gameObject.mFriendlyName = attributes[0]["value"].GetString();

	// Reads Position
	attrName = attributes[1]["name"].GetString();
	// Checks if JSON File attribute name matches the desired variable
	if (attrName != "position") {
		errorMsg = "JSON File second attribute isn't position";
		return false;
	}
	// Saves the position
	const rapidjson::Value& values = attributes[1]["value"];
	gameObject.mPosition[0] = values[0].GetFloat();
	gameObject.mPosition[1] = values[1].GetFloat();
	gameObject.mPosition[2] = values[2].GetFloat();

	// Reads Mesh File Path
	attrName = attributes[2]["name"].GetString();
	// Checks if JSON File attribute name matches the desired variable
	if (attrName != "meshFilePath") {
		errorMsg = "JSON File third attribute isn't mesh file path";
		return false;
	}
	// Saves the Mesh File Path
	gameObject.mMeshFilePath = attributes[2]["value"].GetString();

	return true;
}

bool EntityLoaderManager::LoadCharacter(const std::string path, cCharacter& character, std::string& errorMsg) {
	DEBUG_PRINT("EntityLoaderManager::LoadCharacter(%s, %s)\n", path.c_str(), "cCharacter");

	// JSON File
	std::ifstream file(path);
	// Checks if the file is null
	if (!file.is_open()) {
		std::cout << "Error loading JSON file: " << path << std::endl;
	}
	// JSON File String Buffer
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content = buffer.str();

	// RapidJSON Document
	rapidjson::Document doc;
	// Parsing the string File to the Document
	doc.Parse(content.c_str());

	// Gets the Name of the Class
	const rapidjson::Value& className = doc["className"];
	// Checks if JSON File information matches the desired Game Object
	if (std::strcmp(className.GetString(), "Character") != 0) {
		errorMsg = "JSON File doesn't contain data for cCharacter";
		return false;
	}
	// Gets all Class attributes
	const rapidjson::Value& attributes = doc["attributes"];

	// Reads Mesh Friendly Name
	std::string attrName = attributes[0]["name"].GetString();
	// Checks if JSON File attribute name matches the desired variable
	if (attrName != "friendlyName") {
		errorMsg = "JSON File first attribute isn't friendly name";
		return false;
	}
	// Saves the Friendly Name
	character.friendlyName = attributes[0]["value"].GetString();

	// Reads Position
	attrName = attributes[1]["name"].GetString();
	// Checks if JSON File attribute name matches the desired variable
	if (attrName != "position") {
		errorMsg = "JSON File second attribute isn't position";
		return false;
	}
	// Saves the position
	const rapidjson::Value& values = attributes[1]["value"];
	character.position[0] = values[0].GetFloat();
	character.position[1] = values[1].GetFloat();
	character.position[2] = values[2].GetFloat();

	// Reads Mesh File Path
	attrName = attributes[2]["name"].GetString();
	// Checks if JSON File attribute name matches the desired variable
	if (attrName != "meshFilePath") {
		errorMsg = "JSON File third attribute isn't mesh file path";
		return false;
	}
	// Saves the Mesh File Path
	//character.mMeshFilePath = attributes[2]["value"].GetString();

	// Reads Character Name
	attrName = attributes[3]["name"].GetString();
	// Checks if JSON File attribute name matches the desired variable
	if (attrName != "name") {
		errorMsg = "JSON File fourth attribute isn't character name";
		return false;
	}
	// Saves the Mesh File Path
	character.mName = attributes[3]["value"].GetString();

	// Reads Character Level
	attrName = attributes[4]["name"].GetString();
	// Checks if JSON File attribute name matches the desired variable
	if (attrName != "level") {
		errorMsg = "JSON File fifth attribute isn't character level";
		return false;
	}
	// Saves the Mesh File Path
	character.mLevel = attributes[4]["value"].GetUint();

	// Reads Character Max Health
	attrName = attributes[5]["name"].GetString();
	// Checks if JSON File attribute name matches the desired variable
	if (attrName != "maxHealth") {
		errorMsg = "JSON File sixth attribute isn't character max health";
		return false;
	}
	// Saves the Mesh File Path
	character.mMaxHealth = attributes[5]["value"].GetFloat();

	// Reads Character Current Health
	attrName = attributes[6]["name"].GetString();
	// Checks if JSON File attribute name matches the desired variable
	if (attrName != "currentHealth") {
		errorMsg = "JSON File seventh attribute isn't character current health";
		return false;
	}
	// Saves the Mesh File Path
	character.mCurrentHealth = attributes[6]["value"].GetFloat();

	// Reads Character Max Mana
	attrName = attributes[7]["name"].GetString();
	// Checks if JSON File attribute name matches the desired variable
	if (attrName != "maxMana") {
		errorMsg = "JSON File eight attribute isn't character max mana";
		return false;
	}
	// Saves the Mesh File Path
	character.mMaxMana = attributes[7]["value"].GetFloat();

	// Reads Character Current Health
	attrName = attributes[8]["name"].GetString();
	// Checks if JSON File attribute name matches the desired variable
	if (attrName != "currentMana") {
		errorMsg = "JSON File nineth attribute isn't character current mana";
		return false;
	}
	// Saves the Mesh File Path
	character.mCurrentMana = attributes[8]["value"].GetFloat();

	return true;
}
