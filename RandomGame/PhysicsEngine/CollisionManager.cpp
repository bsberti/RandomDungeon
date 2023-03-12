#include "CollisionManager.h"
#include "PhysicsMath.h"
#include "Shapes.h"

void CollisionManager::addModelToAABBEnviroment(PhysicsSystem* physSys, sModelDrawInfo* model, const glm::vec3 position) {
	for (int i = 0; i < model->numberOfTriangles; i += 3) {
		;
		Point a = Point(glm::vec3(model->pVertices[ model->pIndices[i] ].x,
								  model->pVertices[ model->pIndices[i] ].y,
								  model->pVertices[ model->pIndices[i] ].z)   + position);
		Point b = Point(glm::vec3(model->pVertices[ model->pIndices[i+1] ].x,
								  model->pVertices[ model->pIndices[i+1] ].y,
								  model->pVertices[ model->pIndices[i+1] ].z) + position);
		Point c = Point(glm::vec3(model->pVertices[ model->pIndices[i+2] ].x,
								  model->pVertices[ model->pIndices[i+2] ].y,
								  model->pVertices[ model->pIndices[i+2] ].z) + position);

		int hashA = CalculateHashValue(a);
		int hashB = CalculateHashValue(b);
		int hashC = CalculateHashValue(c);

		Triangle* newTriangle = new Triangle(a, b, c);

		physSys->AddTriangleToAABBCollisionCheck(hashA, newTriangle);

		if (hashA != hashB)
			physSys->AddTriangleToAABBCollisionCheck(hashB, newTriangle);

		if (hashC != hashB && hashC != hashA)
			physSys->AddTriangleToAABBCollisionCheck(hashC, newTriangle);

	}
}

int CollisionManager::CalculateHashValue(const Vector3& v) {
	return CalculateHashValue(v.x, v.y, v.z);
}

int CollisionManager::CalculateHashValue(float x, float y, float z) {
	int hashValue = 0;

	assert(x + 128 > 0);
	assert(y + 300 > 0);
	assert(z + 128 > 0);

	hashValue += floor(x + 128) / 100 * 10000;
	hashValue += floor(y + 300) / 100 * 100;
	hashValue += floor(z + 128) / 100;
	return hashValue;
}
