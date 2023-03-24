#pragma once

#include <physics/interfaces/iPhysicsWorld.h>

#include "RigidBody.h"
#include "SoftBody.h"

#include <vector>

#include <btBulletDynamicsCommon.h>

namespace physics
{
	class PhysicsWorld : public iPhysicsWorld
	{
	public:
		PhysicsWorld();
		virtual ~PhysicsWorld();

		virtual void SetGravity(const Vector3& gravity) override;

		virtual void AddBody(iCollisionBody* body) override;
		virtual void RemoveBody(iCollisionBody* body) override;

		virtual void TimeStep(float dt) override;

		virtual void DebugDraw() override;
		virtual void RegisterCollisionListener(iCollisionListener* listener) override {}

	private:
		btDefaultCollisionConfiguration* m_CollisionConfiguration;
		btCollisionDispatcher* m_Dispatcher;
		btBroadphaseInterface* m_OverlappingPairCache;
		btSequentialImpulseConstraintSolver* m_Solver;
		btDiscreteDynamicsWorld* m_DynamicsWorld;

		//btIDebugDraw* m_DebugDrawer;

		PhysicsWorld(const PhysicsWorld&) {}
		PhysicsWorld& operator=(const PhysicsWorld&) {
			return *this;
		}
	};
}
