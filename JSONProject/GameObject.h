#pragma once
#include <string>

class GameObject {
public:
	// Game Object Friendly Name
	std::string m_friendlyName;
	// Position of the Mesh in the World
	float		m_position[3];
	// Game Object Mesh file path
	std::string m_meshFilePath;

	// Default Constructor
	GameObject();
	// Overloaded Constructor
	GameObject(std::string friendlyName, float position[3], std::string meshFilePath);
	// Destructor
	~GameObject();
};

