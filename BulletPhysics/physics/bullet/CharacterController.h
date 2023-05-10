#pragma once

#include <physics/interfaces/iCharacterController.h>

#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <physics/interfaces/Math.h>
#include <physics/interfaces/iShape.h>

namespace physics
{
	class CharacterController : public iCharacterController
	{
	public:
		CharacterController(btPairCachingGhostObject* btGhostObject, btConvexShape* shape, btScalar stepHeight, const btVector3& up);
		virtual ~CharacterController();

		virtual void SetWalkDirection(const Vector3& walkDirection);
		virtual void SetVelocityForTimeInterval(const Vector3& velocity, float timeInterval);
		virtual void Warp(const Vector3& origin);
		virtual bool CanJump() const;
		virtual void Jump(const Vector3& dir = Vector3(0, 0, 0));
		virtual bool OnGround() const;

		btPairCachingGhostObject* GetGhostObject() {
			return m_GhostObject;
		}

		btCharacterControllerInterface* GetBulletCharacterController() {
			return m_CharacterController;
		}

		glm::vec3 position;
		glm::quat orientation;

		// Add these functions to get the position and orientation of the character controller
		virtual Vector3 GetPosition();
		virtual Quaternion GetOrientation();

		virtual void SetPosition(Vector3 newPosition);
		virtual void SetOrientation(Quaternion newOrientation);

		//void Reset(btCollisionWorld* collisionWorld);
		//void preStep(btCollisionWorld* collisionWorld);
		//void playerStep(btCollisionWorld* collisionWorld, float dt);
		//void setUpInterpolate(bool value);
	private:
		CharacterController() { }

		btPairCachingGhostObject* m_GhostObject;
		btKinematicCharacterController* m_CharacterController;

		CharacterController(const CharacterController& other) { }
		CharacterController& operator=(const CharacterController& other0) {
			return *this;
		}
	};
}