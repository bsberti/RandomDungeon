#pragma once
#include <string>
#include "GameObject.h"

/// <summary>
/// This Singleton Class is responsible to Load Entities from a single JSON file
/// </summary>
class EntityLoaderManager {
protected:
	// Default Constructor - Protected
	EntityLoaderManager();
	// Instance of the Class
	static EntityLoaderManager* m_EntityLoaderManager;

public:
	// Destructor
	~EntityLoaderManager();
	// Singleton Copy Constructor
	EntityLoaderManager(EntityLoaderManager& other) = delete;
	// Singleton Assignment Operator Overwrite
	void operator=(const EntityLoaderManager&) = delete;

	/// <summary>
	/// Singleton Method for Getting an Instance of this class
	/// </summary>
	/// <returns>Pointer of EntityLoaderManager</returns>
	static EntityLoaderManager* GetInstance();

	/// <summary>
	/// This method reads the JSON defined at path variable
	/// And tries to load all information inside the variable gameObject
	/// In case of any error, the string errorMsg will return the issue
	/// </summary>
	/// <param name="path">Path to the JSON file</param>
	/// <param name="gameObject">Address to the GameObject instance</param>
	/// <param name="errorMsg">Address to a string which will contain error msg if needed</param>
	/// <returns>True if gameObject loaded correctly, else false and returns a errorMsg</returns>
	static bool LoadMainCharacter(const std::string path, GameObject& gameObject, std::string& errorMsg);
};

