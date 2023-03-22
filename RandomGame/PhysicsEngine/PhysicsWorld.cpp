#include "PhysicsWorld.h"

namespace physics
{
	PhysicsWorld::PhysicsWorld()
		: iPhysicsWorld()
		, m_CollisionHandler(nullptr)
	{
		m_CollisionHandler = new CollisionHandler();
	}

	PhysicsWorld::~PhysicsWorld()
	{
		// TODO:
		// Clear rigid body vector
		// Delete everything
	}

	void PhysicsWorld::SetGravity(const Vector3& gravity)
	{
		m_Gravity = gravity;
	}

	void PhysicsWorld::AddBody(iCollisionBody* body)
	{
		if (body == nullptr)
			return;

		// TODO: Check if body is already in the bodies vector
		m_Bodies.push_back(body);

		// You could use a switch here, or get the body type once..
		if (body->GetBodyType() == BodyType::RigidBody)
		{
			RigidBody* rigidBody = RigidBody::Cast(body);

			if (std::find(m_RigidBodies.begin(), m_RigidBodies.end(), rigidBody) == m_RigidBodies.end())
			{
				m_RigidBodies.push_back(rigidBody);
			}
		}
		else if (body->GetBodyType() == BodyType::SoftBody)
		{
			SoftBody* rigidBody = SoftBody::Cast(body);

			if (std::find(m_SoftBodies.begin(), m_SoftBodies.end(), rigidBody) == m_SoftBodies.end())
			{
				m_SoftBodies.push_back(rigidBody);
			}
		}
	}

	void PhysicsWorld::RemoveBody(iCollisionBody* body)
	{
		// TODO:
	}

	void PhysicsWorld::TimeStep(float dt)
	{
		int bodyCount = m_Bodies.size();
		int softBodyCount = m_SoftBodies.size();
		int rigidBodyCount = m_RigidBodies.size();

		for (int i = 0; i < m_RigidBodies.size(); i++) {
			m_RigidBodies[i]->Update(dt);
		}

		// Velocity Verlet steps
		// Step #0 Update everything
		for (int i = 0; i < rigidBodyCount; i++) {
			if (!m_RigidBodies[i]->IsStatic())
			{
				m_RigidBodies[i]->SetGravityAcceleration(m_Gravity);
				m_RigidBodies[i]->UpdateAcceleration();
			}
		}

		for (int i = 0; i < softBodyCount; i++) {
			m_SoftBodies[i]->SetGravityAcceleration(m_Gravity.GetGLM());
			m_SoftBodies[i]->UpdateAcceleration();
		}

		// Step #3 : Verlet
		// velocity += acceleration * (dt/2)
		for (int i = 0; i < rigidBodyCount; i++) {
			if (!m_RigidBodies[i]->IsStatic())
			{
				m_RigidBodies[i]->VerletStep3(dt);
				m_RigidBodies[i]->ApplyDamping(dt / 2.f);
			}
		}

		for (int i = 0; i < softBodyCount; i++) {
			m_SoftBodies[i]->VerletStep3(dt);
			m_SoftBodies[i]->ApplyDamping(dt / 2.f);
		}

		// Step #1 : Verlet
		// position += ( velocity+acceleration * (dt/2) ) * dt
		for (int i = 0; i < rigidBodyCount; i++) {
			if (!m_RigidBodies[i]->IsStatic())
			{
				m_RigidBodies[i]->VerletStep1(dt);
			}
		}

		for (int i = 0; i < softBodyCount; i++) {
			m_SoftBodies[i]->VerletStep1(dt);
		}

		// Handle collisions here
		std::vector<CollidingBodies> collisions;
		m_CollisionHandler->Collide(dt, m_Bodies, collisions);

		for (int i = 0; i < softBodyCount; i++)
		{
			m_CollisionHandler->CollideInternalSoftBody(dt, m_SoftBodies[i]);
		}

		// A list of collisions..
		// Here we can collision callbacks.
		for (int i = 0; i < collisions.size(); i++)
		{
			CollidingBodies& collision  = collisions[i];
			// Gets CollisionBody ShapeA
			iShape* shapeA = RigidBody::Cast(collision.bodyA)->GetShape();
			// Gets COllisionBody ShapeB
			iShape* shapeB = RigidBody::Cast(collision.bodyB)->GetShape();
			if (shapeA->GetShapeType() == ShapeType::Sphere && shapeB->GetShapeType() == ShapeType::Sphere) {
				printf("Sphere Shpere Collision! -> Handler\n");
				m_CollisionListener->NotifyCollision(collision.bodyA, collision.bodyB);
			}

			if (shapeA->GetShapeType() == ShapeType::Sphere && shapeB->GetShapeType() == ShapeType::AABB) {
				// Does Nothing
			}

			if (shapeA->GetShapeType() == ShapeType::AABB && shapeB->GetShapeType() == ShapeType::AABB) {
				printf("AABB AABB Collision! -> Handler\n");
				m_CollisionListener->NotifyCollision(collision.bodyA, collision.bodyB);
			}
		}

		// Step #2 : Verlet
		// velocity += acceleration * (dt/2)
		for (int i = 0; i < rigidBodyCount; i++) {
			m_RigidBodies[i]->VerletStep2(dt);
			m_RigidBodies[i]->ApplyDamping(dt / 2.f);
			m_RigidBodies[i]->KillForces();

			// Update all listeners for rigid body positions
		}

		for (int i = 0; i < softBodyCount; i++) {
			m_SoftBodies[i]->VerletStep2(dt);
			m_SoftBodies[i]->ApplyDamping(dt / 2.f);
			m_SoftBodies[i]->KillForces();
		}
	}
}
