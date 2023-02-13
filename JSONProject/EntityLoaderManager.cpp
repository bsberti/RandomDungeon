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

bool EntityLoaderManager::LoadMainCharacter(const std::string path, GameObject& gameObject, std::string& errorMsg) {
	DEBUG_PRINT("EntityLoaderManager::LoadGameObject(%s, %s, %s)\n", path.c_str(), "gameObject", errorMsg.c_str());

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
	if (std::strcmp(className.GetString(), "MainCharacter") != 0) {
		errorMsg = "JSON File doesn't contain data for Main Character";
		return false;
	}
	// Gets all Class attributes
	const rapidjson::Value& attributes = doc["attributes"];

	// Reads Friendly Name
	const rapidjson::Value& attribute0 = attributes[0];
	std::string attrName = attribute0["name"].GetString();
	// Checks if JSON File attribute name matches the desired variable
	if (attrName != "friendlyName") {
		errorMsg = "JSON File first attribute isn't friendly name";
		return false;
	}
	// Saves the Friendly Name
	gameObject.m_friendlyName = attribute0["value"].GetString();

	// Reads Position
	const rapidjson::Value& attribute1 = attributes[1];
	attrName = attribute1["name"].GetString();
	// Checks if JSON File attribute name matches the desired variable
	if (attrName != "position") {
		errorMsg = "JSON File second attribute isn't position";
		return false;
	}
	// Saves the position
	const rapidjson::Value& values = attribute1["value"];
	gameObject.m_position[0] = values[0].GetFloat();
	gameObject.m_position[1] = values[1].GetFloat();
	gameObject.m_position[2] = values[2].GetFloat();

	// Reads Mesh File Path
	const rapidjson::Value& attribute2 = attributes[2];
	attrName = attribute2["name"].GetString();
	// Checks if JSON File attribute name matches the desired variable
	if (attrName != "meshFilePath") {
		errorMsg = "JSON File third attribute isn't mesh file path";
		return false;
	}
	// Saves the Mesh File Path
	gameObject.m_meshFilePath = attribute2["value"].GetString();

	return true;
}
