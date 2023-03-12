#include "PhysicsSystem.h"
#include "PhysicsUtils.h"

PhysicsSystem::PhysicsSystem() {
}

PhysicsSystem::~PhysicsSystem() {
	for (int i = 0; i < m_PhysicsObjects.size(); i++) {
		delete m_PhysicsObjects[i];
	}
	m_PhysicsObjects.clear();

	// delete[] m_Particles[0];		Can check if this works (Not sure)
	//  -> edit: Nope, use this for array of pointers
}

PhysicsObject* PhysicsSystem::CreatePhysicsObject(std::string meshName, Vector3 pos, iShape* shape) {
	PhysicsObject* physicsObject = new PhysicsObject(pos);
	physicsObject->meshName = meshName;
	physicsObject->pShape = shape;
	m_PhysicsObjects.push_back(physicsObject);
	return physicsObject;
}

void PhysicsSystem::AddSpring(Spring* spring) {
	m_Springs.push_back(spring);
}

void PhysicsSystem::UpdateStep(float duration) {
	size_t numPhysicsObjects = m_PhysicsObjects.size();

	// Get Camera Position
	// Perform 1 iteration of bubble sort

	if (numPhysicsObjects == 0)
		return;

	for (int i = 0; i < numPhysicsObjects; i++) {
		/* Gravity */
		m_PhysicsObjects[i]->ApplyForce(Vector3(0, -0.981f, 0));
	}

	for (int i = 0; i < m_Springs.size(); i++) {
		m_Springs[i]->ApplyForces(duration);
	}

	for (int i = 0; i < numPhysicsObjects; i++) {
		m_PhysicsObjects[i]->Integrate(duration);
	}

	// Detect collisions
	PhysicsObject *physObjA, *physObjB;

	iShape *shapeA, *shapeB;

	bool collision;

	for (int i = 0; i < numPhysicsObjects - 1; i++) {
		physObjA = m_PhysicsObjects[i];
		shapeA = physObjA->pShape;

		for (int j = i + 1; j < numPhysicsObjects; j++) {
			physObjB = m_PhysicsObjects[j];
			shapeB = physObjB->pShape;

			collision = false;
			if(shapeA->GetType() != SHAPE_TYPE_AABB || shapeB->GetType() != SHAPE_TYPE_AABB)
				collision = CollisionTest(physObjA->position, shapeA, physObjB->position, shapeB);

			if (collision) {
				if (physObjA->m_IsStatic == false)
				{
					if (physObjB->meshName.compare("Plane_Floor") == 0) {
						physObjA->position.y = physObjA->prevPosition.y;
						physObjA->velocity.y = 0.0f;
					} else {
						physObjA->position.x = physObjA->prevPosition.x;
						physObjA->position.z = physObjA->prevPosition.z;
						physObjA->KillAllForces();
						physObjA->SetVelocity(Vector3(0.0f));
					}
				}

				if (physObjB->m_IsStatic == false)
				{
					if (physObjA->meshName.compare("Plane_Floor") == 0) {
						physObjB->position.y = physObjB->prevPosition.y;
						physObjB->velocity.y = 0.0f;
					} else {
						physObjB->position.x = physObjB->prevPosition.x;
						physObjB->position.z = physObjB->prevPosition.z;
						physObjB->KillAllForces();
						physObjB->SetVelocity(Vector3(0.0f));
					}
				}
			}
		}
	}

	for (int i = 0; i < numPhysicsObjects; i++) {
		m_PhysicsObjects[i]->KillAllForces();
	}
}

void PhysicsSystem::AddTriangleToAABBCollisionCheck(int hash, Triangle* triangle)
{
	m_AABBStructure[hash].push_back(triangle);
}

void PhysicsSystem::RemoveWalls()
{
	for (int i = m_PhysicsObjects.size() - 1; i >= 0; i--) {
		if (m_PhysicsObjects[i]->meshName == "Wall") {
			m_PhysicsObjects.erase(m_PhysicsObjects.begin() + i);
		}
	}
}

bool PhysicsSystem::RayCastClosest(Ray ray, PhysicsObject** hitObject, unsigned char flags)
{
	PhysicsObject* closestPhysicsObject = nullptr;
	float closestDistance = FLT_MAX;

	// An alternate method to optimize this would be to sort the m_PhysicsObjects
	// by the distance of the object from the eye.
	// Sort objects should be done once per frame, not each call
	// This can be a "bubble sort", as objects will not be needing to be 100%
	// sorted every frame, and you are not jumping around the world really fast.

	for (int i = 0; i < m_PhysicsObjects.size(); i++) {
		PhysicsObject* physicsObject = m_PhysicsObjects[i];
		if (physicsObject->pShape->GetType() == SHAPE_TYPE_SPHERE)
		{
			Sphere* pSphere = dynamic_cast<Sphere*>(physicsObject->pShape);
			if (physicsObject->type & flags)
			{
				if (TestRaySphere(ray.origin, ray.direction, pSphere->Center + physicsObject->position, pSphere->Radius))
				{
					float distance = Vector3::Distance(ray.origin, physicsObject->position);
					if (distance < closestDistance) {
						closestPhysicsObject = physicsObject;
						closestDistance = distance;
					}
				}
			}
		}
	}
	*hitObject = closestPhysicsObject;
	return closestPhysicsObject != nullptr;
}

bool PhysicsSystem::RayCastFirstFound(Ray ray, PhysicsObject** hitObject)
{
	for (int i = 0; i < m_PhysicsObjects.size(); i++) {
		PhysicsObject* physicsObject = m_PhysicsObjects[i];
		if (physicsObject->pShape->GetType() == SHAPE_TYPE_SPHERE)
		{
			Sphere* pSphere = dynamic_cast<Sphere*>(physicsObject->pShape);
			if (TestRaySphere(ray.origin, ray.direction, pSphere->Center + physicsObject->position, pSphere->Radius))
			{
				*hitObject = physicsObject;
				return true;
			}
		}
	}
	return false;
}

std::vector<PhysicsObject*> PhysicsSystem::RayCastAll(Ray ray)
{
	std::vector<PhysicsObject*> hitList;
	for (int i = 0; i < m_PhysicsObjects.size(); i++) {
		PhysicsObject* physicsObject = m_PhysicsObjects[i];
		if (physicsObject->pShape->GetType() == SHAPE_TYPE_SPHERE)
		{
			Sphere* pSphere = dynamic_cast<Sphere*>(physicsObject->pShape);
			if (TestRaySphere(ray.origin, ray.direction, pSphere->Center + physicsObject->position, pSphere->Radius))
			{
				hitList.push_back(physicsObject);
			}
		}
	}
	return hitList;
}

bool PhysicsSystem::CollisionTest(const Vector3& posA, iShape* shapeA, const Vector3& posB, iShape* shapeB)
{
	bool detectedCollision = false;

	// ShapeA AABB Collision test 
	if (shapeA->GetType() == SHAPE_TYPE_AABB)
	{
		if (shapeB->GetType() == SHAPE_TYPE_AABB) {}

		else if (shapeB->GetType() == SHAPE_TYPE_SPHERE) {
			detectedCollision = CollisionTest(posB, dynamic_cast<Sphere*>(shapeB), posA, dynamic_cast<AABB*>(shapeA));
		}

		else if (shapeB->GetType() == SHAPE_TYPE_TRIANGLE) {}
	}


	// ShapeA Sphere collision tests
	else if (shapeA->GetType() == SHAPE_TYPE_SPHERE)
	{
		if (shapeB->GetType() == SHAPE_TYPE_AABB) {
			detectedCollision = CollisionTest(posA, dynamic_cast<Sphere*>(shapeA), posB, dynamic_cast<AABB*>(shapeB));
		}

		else if (shapeB->GetType() == SHAPE_TYPE_SPHERE) {
			detectedCollision = CollisionTest(posA, dynamic_cast<Sphere*>(shapeA), posB, dynamic_cast<Sphere*>(shapeB));
		}

		else if (shapeB->GetType() == SHAPE_TYPE_TRIANGLE) {
			detectedCollision = CollisionTest(posA, dynamic_cast<Sphere*>(shapeA), posB, dynamic_cast<Triangle*>(shapeB));
		}
	}


	// ShapeA Triangle collision tests
	else if (shapeA->GetType() == SHAPE_TYPE_TRIANGLE)
	{
		if (shapeB->GetType() == SHAPE_TYPE_AABB) {}

		else if (shapeB->GetType() == SHAPE_TYPE_SPHERE) {
			detectedCollision = CollisionTest(posB, dynamic_cast<Sphere*>(shapeB), posA, dynamic_cast<Triangle*>(shapeA));
		}

		else if (shapeB->GetType() == SHAPE_TYPE_TRIANGLE) {}
	}


	// ShapeA is unknown shape...
	else
	{
		// what is this!?!?!
	}

	return detectedCollision;
}

bool PhysicsSystem::CollisionTest(const Vector3& posA, Sphere* a, const Vector3& posB, Sphere* b)
{
	return TestSphereSphere(posA + a->Center, a->Radius, posB + b->Center, b->Radius);
}

bool PhysicsSystem::CollisionTest(const Vector3 &posA, Sphere* a, const Vector3& posB, AABB* b)
{
	return TestSphereAABB(posA + a->Center, a->Radius, *b);
}

bool PhysicsSystem::CollisionTest(const Vector3 &posA, Sphere* a, const Vector3& posB, Triangle* b)
{
	Point unused;
	return TestSphereTriangle(posA + a->Center, a->Radius, posB + (*b).A, posB + (*b).B, posB + (*b).C, unused);
}