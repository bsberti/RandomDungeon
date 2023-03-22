#pragma once
#include "iPhysicsWorld.h"
#include "iCollisionBody.h"
#include <vector>

class CollisionListener : public physics::iCollisionListener {
public:
	struct sCollisionNotification {
		physics::iCollisionBody* bodyA;
		physics::iCollisionBody* bodyB;
	};

	CollisionListener();
	~CollisionListener();

	/// <summary>
	/// Adds a Collision to the m_CollisionArray
	/// </summary>
	/// <param name="bodyA">First Body Colliding</param>
	/// <param name="bodyB">Second Body Colliding</param>
	void NotifyCollision(physics::iCollisionBody* bodyA, physics::iCollisionBody* bodyB);
	/// <summary>
	/// Checks the m_CollisionArray for a Sphere Sphere Collision
	/// After that if existing account for it and removes from the m_CollisionArray
	/// </summary>
	/// <returns>If found returns true</returns>
	bool AccountForCollision();

protected:
	std::vector<sCollisionNotification> m_CollisionArray;
};

