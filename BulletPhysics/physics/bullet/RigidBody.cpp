#include "RigidBody.h"
//#include "Conversion.h"
#include <math.h>

//#include <bullet/btBulletDynamicsCommon.h>

namespace physics
{
	RigidBody::RigidBody(const RigidBodyDesc& desc, iShape* shape)
		: iRigidBody()
	{
		//btQuaternion rotation;
		//btVector3 position;

		//CastBulletQuaternion(desc.rotation, &rotation);
		//CastBulletVector3(desc.position, &position);

		//btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(rotation, position));
		//btVector3 inertia(0, 0, 0);
		//btCollisionShape* bulletShape = CastBulletShape(shape);

		//if (desc.mass != 0.0f)
		//{
		//	bulletShape->calculateLocalInertia(desc.mass, inertia);
		//}

		//btRigidBody::btRigidBodyConstructionInfo bodyCI(desc.mass, motionState, bulletShape, inertia);

		//m_BulletRigidBody = new btRigidBody(bodyCI);

		//btVector3 btAngularFactor;
		//btVector3 btLinearFactor;

		//CastBulletVector3(desc.angularFactor, &btAngularFactor);
		//CastBulletVector3(desc.linearFactor, &btLinearFactor);

		//m_BulletRigidBody->setAngularFactor(btAngularFactor);
		//m_BulletRigidBody->setLinearFactor(btLinearFactor);
	}

	RigidBody::~RigidBody()
	{ }

	RigidBody* RigidBody::Cast(iCollisionBody* body)
	{
		return dynamic_cast<RigidBody*>(body);
	}

	void RigidBody::GetPosition(Vector3& position)
	{
		//CastGLMVec3(m_BulletRigidBody->getCenterOfMassPosition(), &position);
	}

	void RigidBody::SetPosition(const Vector3& position)
	{
		// No.
	}

	void RigidBody::GetRotation(Quaternion& rotation)
	{
		//CastGLMQuat(m_BulletRigidBody->getOrientation(), &rotation);
	}

	void RigidBody::SetRotation(const Quaternion& rotation)
	{
		// No.
	}

	void RigidBody::ApplyForce(const Vector3& force)
	{
		//btVector3 btForce;
		//CastBulletVector3(force, &btForce);
		//m_BulletRigidBody->applyCentralForce(btForce);
	}

	void RigidBody::ApplyForceAtPoint(const Vector3& force, const Vector3& relativePoint)
	{
		//btVector3 btForce;
		//btVector3 btForceAtPoint;

		//CastBulletVector3(force, &btForce);
		//CastBulletVector3(relativePoint, &btForceAtPoint);

		//m_BulletRigidBody->applyForce(btForce, btForceAtPoint);
	}

	void RigidBody::ApplyImpulse(const Vector3& impulse)
	{
		//btVector3 btImpulse;
		//CastBulletVector3(impulse, &btImpulse);
		//m_BulletRigidBody->applyCentralImpulse(btImpulse);
	}

	void RigidBody::ApplyImpulseAtPoint(const Vector3& impulse, const Vector3& relativePoint)
	{
		//btVector3 btImpulse;
		//btVector3 btImpulseAtPoint;

		//CastBulletVector3(impulse, &btImpulse);
		//CastBulletVector3(relativePoint, &btImpulseAtPoint);

		//m_BulletRigidBody->applyImpulse(btImpulse, btImpulseAtPoint);
	}

	void RigidBody::ApplyTorque(const Vector3& torque)
	{
		//btVector3 btTorque;

		//CastBulletVector3(torque, &btTorque);

		//m_BulletRigidBody->applyTorque(btTorque);
	}

	void RigidBody::ApplyTorqueImpulse(const Vector3& torqueImpulse)
	{
		//btVector3 btTorqueImpulse;

		//CastBulletVector3(torqueImpulse, &btTorqueImpulse);

		//m_BulletRigidBody->applyTorqueImpulse(btTorqueImpulse);
	}
}