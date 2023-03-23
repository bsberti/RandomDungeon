#pragma once

#include <physics/interfaces/iRigidBody.h>
#include <physics/interfaces/RigidBodyDesc.h>
#include <physics/interfaces/iShape.h>

//#include <bullet/btBulletDynamicsCommon.h>

namespace physics
{
	class RigidBody : public iRigidBody
	{
	public:
		RigidBody(const RigidBodyDesc& desc, iShape* shape);
		~RigidBody();

		static RigidBody* Cast(iCollisionBody* body);

		virtual void GetPosition(Vector3& position) override;
		virtual void SetPosition(const Vector3& position) override;

		virtual void GetRotation(Quaternion& rotation) override;
		virtual void SetRotation(const Quaternion& rotation) override;

		virtual void ApplyForce(const Vector3& force) override;
		virtual void ApplyImpulse(const Vector3& impulse) override;

		virtual void ApplyForceAtPoint(const Vector3& force, const Vector3& relativePoint) override;
		virtual void ApplyImpulseAtPoint(const Vector3& impulse, const Vector3& relativePoint) override;

		virtual void ApplyTorque(const Vector3& torque) override;
		virtual void ApplyTorqueImpulse(const Vector3& torqueImpulse) override;

		//btRigidBody* GetBulletBody(void) { return m_BulletRigidBody; }
	private:

		//btRigidBody* m_BulletRigidBody;

		RigidBody(const RigidBody&) { }
		RigidBody& operator=(const RigidBody&) {
			return *this;
		}
	};
}
