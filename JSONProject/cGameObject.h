#pragma once
#include <string>

class cGameObject {
public:
	// Game Object Friendly Name
	std::string mFriendlyName;
	// Position of the Mesh in the World
	float		mPosition[3];
	// Game Object Mesh file path
	std::string mMeshFilePath;

	// Default Constructor
	cGameObject();
	// Overloaded Constructor
	cGameObject(std::string friendlyName, float position[3], std::string meshFilePath);
	// Destructor
	~cGameObject();
};

