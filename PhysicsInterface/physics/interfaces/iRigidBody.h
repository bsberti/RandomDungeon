#pragma once

#include "iCollisionBody.h"

#include "Math.h"

namespace physics
{
	class iRigidBody : public iCollisionBody
	{
	public:
		virtual ~iRigidBody() {}

		virtual void GetPosition(Vector3& position) = 0;
		virtual void SetPosition(const Vector3& position) = 0;

		virtual void GetRotation(Quaternion& rotation) = 0;
		virtual void SetRotation(const Quaternion& rotation) = 0;

		virtual void ApplyForce(const Vector3& force) = 0;
		virtual void ApplyForceAtPoint(const Vector3& force, const Vector3& relativePoint) = 0;

		virtual void ApplyImpulse(const Vector3& impulse) = 0;
		virtual void ApplyImpulseAtPoint(const Vector3& impulse, const Vector3& relativePoint) = 0;

		virtual void ApplyTorque(const Vector3& torque) = 0;
		virtual void ApplyTorqueImpulse(const Vector3& torqueImpulse) = 0;

	protected:
		iRigidBody() : iCollisionBody(BodyType::RigidBody) {}

	private:
		iRigidBody(const iRigidBody&) : iCollisionBody(BodyType::RigidBody) { }
		iRigidBody& operator=(const iRigidBody&) {
			return *this;
		}
	};
}
