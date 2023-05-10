#include "CharacterController.h"
#include "Conversion.h"

#include <BulletCollision/CollisionDispatch/btGhostObject.h>

namespace physics
{
	CharacterController::CharacterController(
		btPairCachingGhostObject* btGhostObject,
		btConvexShape* shape,
		btScalar stepHeight,
		const btVector3& up)
		: m_GhostObject(btGhostObject) {
		m_CharacterController = new btKinematicCharacterController(
			m_GhostObject, shape, stepHeight, up);
		m_CharacterController->setUseGhostSweepTest(false);

		m_CharacterController->setMaxPenetrationDepth(0);

		// Store the initial position and orientation of the character controller
		position = glm::vec3(0, 0, 0);
		orientation = glm::quat(0, 0, 0, 1.0);
	}

	CharacterController::~CharacterController() {

	}

	Vector3 CharacterController::GetPosition() {
		return position;
	}

	Quaternion CharacterController::GetOrientation() {
		return orientation;
	}

	void CharacterController::SetPosition(Vector3 newPosition) {
		position = newPosition;
	}

	void CharacterController::SetOrientation(Quaternion newOrientation) {
		orientation = glm::quat(newOrientation.x, newOrientation.y, newOrientation.z, newOrientation.w);
	}

	void CharacterController::SetWalkDirection(const Vector3& walkDirection) {
		btVector3 btWalkDirection;
		CastBulletVector3(walkDirection, &btWalkDirection);
		m_CharacterController->setWalkDirection(btWalkDirection);

		// Update the position of the character controller
		btTransform transform = m_CharacterController->getGhostObject()->getWorldTransform();
		position = glm::vec3(transform.getOrigin().getX(),
			transform.getOrigin().getY(),
			transform.getOrigin().getZ());
	}

	void CharacterController::SetVelocityForTimeInterval(const Vector3& velocity, float timeInterval) {
		btVector3 btVelocity;
		CastBulletVector3(velocity, &btVelocity);
		m_CharacterController->setVelocityForTimeInterval(btVelocity, btScalar(timeInterval));
	}

	void CharacterController::Warp(const Vector3& origin) {
		btVector3 btOrigin;
		CastBulletVector3(origin, &btOrigin);
		m_CharacterController->warp(btOrigin);
	}

	bool CharacterController::CanJump() const {
		return m_CharacterController->canJump();
	}

	void CharacterController::Jump(const Vector3& dir/*=Vector3(0, 0, 0)*/) {
		btVector3 btDir;
		CastBulletVector3(dir, &btDir);
		m_CharacterController->jump(btDir);
	}

	bool CharacterController::OnGround() const {
		return m_CharacterController->onGround();
	}



}