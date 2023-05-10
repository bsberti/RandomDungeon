#include "PhysicsWorld.h"
#include "Conversion.h"
#include "DebugDrawer.h"

#include <BulletDynamics/Character/btCharacterControllerInterface.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "CharacterController.h"

namespace physics
{
	PhysicsWorld::PhysicsWorld()
		: iPhysicsWorld()
	{
		m_CollisionConfiguration = new btDefaultCollisionConfiguration();
		m_Dispatcher = new btCollisionDispatcher(m_CollisionConfiguration);
		m_OverlappingPairCache = new btDbvtBroadphase();
		m_Solver = new btSequentialImpulseConstraintSolver;
		m_DynamicsWorld = new btDiscreteDynamicsWorld(m_Dispatcher, 
			m_OverlappingPairCache, m_Solver, m_CollisionConfiguration);

		//m_DebugDrawer = new DebugDrawer(7);
		//m_DynamicsWorld->setDebugDrawer(m_DebugDrawer);
		//int debugMode = 1;
		//m_DynamicsWorld->getDebugDrawer()->setDebugMode(debugMode);
	}

	PhysicsWorld::~PhysicsWorld()
	{
		delete m_DynamicsWorld;
		delete m_CollisionConfiguration;
		delete m_Dispatcher;
		delete m_OverlappingPairCache;
		delete m_Solver;
	}

	void PhysicsWorld::SetGravity(const Vector3& gravity)
	{
		btVector3 btGravity;
		CastBulletVector3(gravity, &btGravity);
		m_DynamicsWorld->setGravity(btGravity);
	}

	void PhysicsWorld::AddBody(iCollisionBody* body)
	{
		btRigidBody* bulletBody = CastBulletRigidBody(body);
		m_DynamicsWorld->addRigidBody(bulletBody);
	}

	void PhysicsWorld::RemoveBody(iCollisionBody* body)
	{
		btRigidBody* bulletBody = CastBulletRigidBody(body);
		m_DynamicsWorld->removeRigidBody(bulletBody);
	}

	void PhysicsWorld::AddCharacterController(iCharacterController* characterController)
	{
		btCharacterControllerInterface* btCharacterController =
			CastBulletCharacterController(characterController);
		CharacterController* cc = dynamic_cast<CharacterController*>(characterController);

		btPairCachingGhostObject* btGhostObject = cc->GetGhostObject();
		m_DynamicsWorld->addCollisionObject(btGhostObject,
			btBroadphaseProxy::CharacterFilter,
			btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);

		m_DynamicsWorld->addAction(btCharacterController);
	}

	void PhysicsWorld::RemoveCharacterController(iCharacterController* characterController)
	{
		CharacterController* cc = dynamic_cast<CharacterController*>(characterController);
		btPairCachingGhostObject* btGhostObject = cc->GetGhostObject();
		btCharacterControllerInterface* btCharacterController =
			CastBulletCharacterController(characterController);
		m_DynamicsWorld->removeAction(btCharacterController);
		m_DynamicsWorld->removeCollisionObject(btGhostObject);

	}

	void PhysicsWorld::TimeStep(float dt)
	{
		m_DynamicsWorld->stepSimulation(dt);
	}

	void PhysicsWorld::DebugDraw()
	{
		m_DynamicsWorld->debugDrawWorld();
	}
}
