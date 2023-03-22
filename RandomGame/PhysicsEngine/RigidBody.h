#pragma once

#include<glm/gtc/quaternion.hpp>

#include "iRigidBody.h"
#include "RigidBodyDesc.h"
#include "iShape.h"
#include "Vector3.h"

namespace physics
{
	class RigidBody : public iRigidBody
	{
	public:
		RigidBody(const RigidBodyDesc& desc, iShape* shape);
		~RigidBody();

		static RigidBody* Cast(iCollisionBody* body);

		iShape* GetShape();

		bool IsStatic();
		virtual void GetPosition(Vector3& position) override;
		virtual void SetPosition(const Vector3& position) override;

		virtual void GetVelocity(Vector3& velocity) override;
		virtual void GetRestitution(float& restitution) override;
		virtual void GetMass(float& mass) override;

		virtual void GetRotation(glm::quat& rotation) override;
		virtual void SetRotation(const glm::quat& rotation) override;

		virtual void ApplyForce(const Vector3& force) override;
		virtual void ApplyImpulse(const Vector3& impulse) override;

		virtual void ApplyForceAtPoint(const Vector3& force, const Vector3& relativePoint) override;
		virtual void ApplyImpulseAtPoint(const Vector3& impulse, const Vector3& relativePoint) override;

		virtual void ApplyTorque(const Vector3& torque) override;
		virtual void ApplyTorqueImpulse(const Vector3& torqueImpulse) override;

		void SetGravityAcceleration(const Vector3& gravity);
		void UpdateAcceleration();

		void VerletStep1(float dt);
		void VerletStep2(float dt);
		void VerletStep3(float dt);

		void KillForces();

		void ApplyDamping(float dt);

		// Lock
		void SetRenderPosition(Vector3* position) {
			m_RenderPosition = position;
		}

		void Update(float v) {
			if (m_RenderPosition != nullptr)
				m_RenderPosition->x += v;
		}

		friend class CollisionHandler;
	private:
		float m_Mass;
		float m_InvMass;
		float m_Friction;
		float m_Restitution;
		float m_LinearDamping;
		float m_AngularDamping;
		bool m_IsStatic;

		Vector3 m_Position;
		Vector3 m_PreviousPosition;
		Vector3 m_LinearVelocity;
		Vector3 m_LinearAcceleration;

		Vector3 m_Force;
		Vector3 m_Torque;
		Vector3 m_Gravity;

		glm::quat m_Rotation;
		Vector3 m_AngularVelocity;
		Vector3 m_AngularAcceleration;

		iShape* m_Shape;

		Vector3* m_RenderPosition;

		RigidBody(const RigidBody&) { }
		RigidBody& operator=(const RigidBody&) {
			return *this;
		}
	};
}
