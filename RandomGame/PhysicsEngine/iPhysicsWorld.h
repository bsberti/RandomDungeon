#pragma once

#include "Math.h"
#include "iCollisionBody.h"
#include "Vector3.h"

namespace physics
{
	class iCollisionListener
	{
	public:
		virtual ~iCollisionListener() {}

		virtual void NotifyCollision(iCollisionBody* bodyA, iCollisionBody* bodyB) = 0;
		virtual bool AccountForCollision() = 0;

	protected:
		iCollisionListener() {}
	};

	class iPhysicsWorld
	{
	public:
		virtual ~iPhysicsWorld() {}

		virtual void SetGravity(const Vector3& gravity) = 0;

		virtual void AddBody(iCollisionBody* body) = 0;
		virtual void RemoveBody(iCollisionBody* body) = 0;

		virtual void TimeStep(float dt) = 0;

		virtual void DebugDraw() { }
		virtual void RegisterCollisionListener(iCollisionListener* listener) = 0;

	protected:
		iPhysicsWorld() {}

	private:
		iPhysicsWorld(const iPhysicsWorld&) {}
		iPhysicsWorld& operator=(const iPhysicsWorld&) {
			return *this;
		}
	};
}