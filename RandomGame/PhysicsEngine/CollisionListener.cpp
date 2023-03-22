#include "CollisionListener.h"

CollisionListener::CollisionListener() {
}

CollisionListener::~CollisionListener() {
}

void CollisionListener::NotifyCollision(physics::iCollisionBody* bodyA, physics::iCollisionBody* bodyB) {
	sCollisionNotification newNotification;
	newNotification.bodyA = bodyA;
	newNotification.bodyB = bodyB;
	this->m_CollisionArray.push_back(newNotification);
}

bool CollisionListener::AccountForCollision() {
	if (m_CollisionArray.size() > 0) {
		m_CollisionArray.pop_back();
		return true;
	}
	return false;
}
