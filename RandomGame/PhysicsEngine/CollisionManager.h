#pragma once
#include <glm/vec3.hpp>
#include "PhysicsSystem.h"

#include "../cVAOManager/sModelDrawInfo.h"

class CollisionManager {
public:
	/// <summary>
	/// Receives a model and maps all of its triangles into the Physics System
	/// </summary>
	/// <param name="model">Receives a cModel Class</param>
	/// <param name="position">Receives the Mesh Location</param>
	void addModelToAABBEnviroment(PhysicsSystem* physSys, sModelDrawInfo* model, const glm::vec3 position);
	/// <summary>
	/// Receives a Vector3 and calculate its HashValue
	/// </summary>
	/// <param name="v">Receives a Vector3</param>
	/// <returns>Returns int - Hash ID</returns>
	int CalculateHashValue(const Vector3& v);
	int CalculateHashValue(float x, float y, float z);
};

