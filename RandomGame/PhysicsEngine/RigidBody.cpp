#include "RigidBody.h"

#include <math.h>

namespace physics
{
	RigidBody::RigidBody(const RigidBodyDesc& desc, iShape* shape)
		: iRigidBody()
		, m_Shape(shape)
		, m_Position(desc.position)
		, m_Rotation(desc.rotation)
		, m_IsStatic(desc.isStatic)
		, m_LinearVelocity(desc.linearVelocity)
		, m_AngularVelocity(desc.angularVelocity)
		, m_Restitution(desc.restitution)
		, m_Friction(desc.friction)
		, m_LinearDamping(desc.linearDamping)
		, m_AngularDamping(desc.angularDamping)
	{
		if (m_IsStatic || desc.mass <= 0.f) {
			//m_Mass = 0.f;
			//m_InvMass = 0.f;
			m_Mass = desc.mass;
			m_InvMass = 1.f / m_Mass;
			m_IsStatic = true;
		}
		else
		{
			m_Mass = desc.mass;
			m_InvMass = 1.f / m_Mass;
		}
	}

	RigidBody::~RigidBody()
	{ }

	RigidBody* RigidBody::Cast(iCollisionBody* body)
	{
		return dynamic_cast<RigidBody*>(body);
	}

	iShape* RigidBody::GetShape()
	{
		return m_Shape;
	}

	bool RigidBody::IsStatic()
	{
		return m_IsStatic;
	}

	void RigidBody::GetPosition(Vector3& position)
	{
		position = m_Position;
	}

	void RigidBody::SetPosition(const Vector3& position)
	{
		m_Position = position;
	}

	void RigidBody::GetVelocity(Vector3& velocity)
	{
		velocity = m_LinearVelocity;
	}

	void RigidBody::GetRestitution(float& restitution)
	{
		restitution = m_Restitution;
	}

	void RigidBody::GetMass(float& mass)
	{
		mass = m_Mass;
	}

	void RigidBody::GetRotation(glm::quat& rotation)
	{
		rotation = m_Rotation;
	}

	void RigidBody::SetRotation(const glm::quat& rotation)
	{
		m_Rotation = rotation;
	}

	void RigidBody::ApplyForce(const Vector3& force)
	{
		m_Force += force;
	}

	void RigidBody::ApplyForceAtPoint(const Vector3& force, const Vector3& relativePoint)
	{
		ApplyForce(force);
		ApplyTorque(glm::cross(relativePoint.GetGLM(), force.GetGLM()));
	}

	void RigidBody::ApplyImpulse(const Vector3& impulse)
	{
		m_LinearVelocity += impulse * m_InvMass;
	}

	void RigidBody::ApplyImpulseAtPoint(const Vector3& impulse, const Vector3& relativePoint)
	{
		ApplyTorqueImpulse(glm::cross(relativePoint.GetGLM(), impulse.GetGLM()));
	}

	void RigidBody::ApplyTorque(const Vector3& torque)
	{
		m_Torque += torque;
	}

	void RigidBody::ApplyTorqueImpulse(const Vector3& torqueImpulse)
	{
		m_AngularVelocity += torqueImpulse;
	}

	void RigidBody::SetGravityAcceleration(const Vector3& gravity)
	{
		m_Gravity = gravity;
	}

	void RigidBody::UpdateAcceleration()
	{
		if (m_IsStatic)
			return;

		m_LinearAcceleration = m_Force * m_InvMass + m_Gravity;
		m_AngularAcceleration = m_Torque;
	}

	void RigidBody::VerletStep1(float dt)
	{
		if (m_IsStatic)
			return;

		m_PreviousPosition = m_Position;
		m_Position += (m_LinearVelocity + m_LinearAcceleration * (dt * 0.5f)) * dt;

		// Calculate the axis we will rotate around
		glm::vec3 axis = (m_AngularVelocity.GetGLM() + m_AngularAcceleration.GetGLM() * (dt * 0.5f)) * dt;

		// Calculate the angle we will rotate
		float angle = glm::length(axis);
		axis = glm::normalize(axis);
		if (angle != 0.f)
		{
			glm::quat rot = glm::angleAxis(angle, axis);
			m_Rotation *= rot;
		}
	}

	void RigidBody::VerletStep2(float dt)
	{
		if (m_IsStatic)
			return;

		m_LinearVelocity += m_LinearAcceleration * (dt * 0.5f);
		m_AngularVelocity += m_AngularAcceleration * (dt * 0.5f);
	}

	void RigidBody::VerletStep3(float dt)
	{
		VerletStep2(dt);
	}

	void RigidBody::KillForces()
	{
		m_Force = glm::vec3(0.f);
		m_Torque = glm::vec3(0.f);
	}

	void RigidBody::ApplyDamping(float dt)
	{
		m_LinearVelocity *= pow(1.f - m_LinearDamping, dt);
		m_AngularVelocity *= pow(1.f - m_AngularDamping, dt);

		// We can kill very small velocities here
		if (glm::length(m_LinearVelocity.GetGLM()) < 0.001f)
			m_LinearVelocity = glm::vec3(0.f);

		if (glm::length(m_AngularVelocity.GetGLM()) < 0.001f)
			m_AngularVelocity = glm::vec3(0.f);
	}
}