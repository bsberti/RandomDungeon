#pragma once

#include "Vector3.h"
#include "BoundingBox.h"
#include "Shapes.h"
#include <string>

class PhysicsObject {
	// Private area
public:
	PhysicsObject();
	PhysicsObject(const Vector3& position);
	~PhysicsObject();
	PhysicsObject(const PhysicsObject& particle);
	PhysicsObject operator=(const PhysicsObject& particle);

	void SetMass(float mass) {
		// If the mass is 0 or less we treat it as 
		// a static 
		if (mass <= 0) {
			m_IsStatic = true;
			invMass = -1.f;
		} else {
			m_IsStatic = false;
			invMass = 1.f / mass;
		}
	}

	void Integrate(float dt);
	void ApplyForce(const Vector3& direction);
	void KillAllForces();
	void SetPosition(const Vector3& pos);
	void SetVelocity(const Vector3& velocity);
	void SetAcceleration(const Vector3& acceleration);

	inline const Vector3& GetPosition() const {	return position; }
	inline const Vector3& GetVelocity() const {	return velocity; }
	inline const Vector3& GetAcceleration() const {	return acceleration; }
	inline const Vector3& GetForce() const { return force; }

	BoundingBox*	pBoundingBox;
	iShape*			pShape;
	unsigned char	type;
	std::string		meshName;

private:
	friend class PhysicsSystem;

	Vector3 position;
	Vector3 prevPosition;
	Vector3 velocity;
	Vector3 acceleration;

	Vector3 force;
	float damping;

	bool m_IsStatic;
	float invMass;

	void PrintInfo();

};