#include "CollisionHandler.h"

#include "RigidBody.h"

#include <glm/gtx/projection.hpp>


namespace physics
{
	// Page 224 Chapter 5 Basic Primitive Tests
	// Christer Ericson - Real-time collision detection
	bool TestMovingSphereSphere(
		const Vector3& s0Center, const float s0Radius,
		const Vector3& s1Center, const float s1Radius,
		const Vector3& v0, const Vector3& v1, float& t)
	{
		Vector3 s = s1Center - s0Center;	// Vector between sphere centers
		Vector3 v = v1 - v0;				// Relative motion of s1 with respect to stationary s0
		float r = s1Radius + s0Radius;		// Sum of sphere radii
		float c = glm::dot(s.GetGLM(), s.GetGLM()) - r * r;

		if (c < 0.f) {
			// Already intersecting, not moving towards a collision
			t = 0.f;
			return true;
		}

		float a = glm::dot(v.GetGLM(), v.GetGLM());
		if (a < std::numeric_limits<float>::epsilon())
			return false;		// Spheres not moving relative to eachother

		float b = glm::dot(v.GetGLM(), s.GetGLM());
		if (b >= 0.f)
			return false;		// Spheres not moving towards eachother

		float d = (b * b) - (a * c);
		if (d < 0.f)
			return false;		// No real-valued root, spheres do not intersect

		t = (-b - sqrt(d)) / a;
		return true;
	}

	// Page #? Chapter 5 Basic Primitive Tests
	// Christer Ericson - Real-time collision detection
	bool TestMovingSpherePlane(const Vector3& prevPosition, const Vector3& currPosition, float radius, const Vector3& norm, float dotProduct)
	{
		// Get distance for both a and b from the plane
		float adist = glm::dot(prevPosition.GetGLM(), norm.GetGLM()) - dotProduct;
		float bdist = glm::dot(currPosition.GetGLM(), norm.GetGLM()) - dotProduct;

		// Intersects if on different sides of a plane (distance have different signs)
		if (adist * bdist < 0.f)
			return true;

		// Intersects if start or end position within radius from plane
		if (abs(adist) < radius || abs(bdist) <= radius)
			return true;

		// No intersection
		return false;
	}

	CollisionHandler::CollisionHandler()
	{

	}

	CollisionHandler::~CollisionHandler()
	{

	}

	// From Christer Ericson book
	glm::vec3 CollisionHandler::ClosestPtPointPlane(const glm::vec3& pt, const glm::vec3& planeNormal, float planeDotProduct)
	{
		float t = glm::dot(planeNormal, pt) - planeDotProduct;
		return pt - t * planeNormal;
	}

	glm::vec3 closestPointToAABB(const glm::vec3& point, const glm::vec3 minBounds, const glm::vec3 maxBounds)
	{
		// Find the closest point to the AABB by clamping the point's coordinates
		glm::vec3 closestPoint = point;
		closestPoint.x = std::max(minBounds.x, std::min(point.x, maxBounds.x));
		closestPoint.y = std::max(minBounds.y, std::min(point.y, maxBounds.y));
		closestPoint.z = std::max(minBounds.z, std::min(point.z, maxBounds.z));
		//closestPoint.x = glm::clamp(point.x, minBounds.x, maxBounds.x);
		//closestPoint.y = glm::clamp(point.y, minBounds.y, maxBounds.y);
		//closestPoint.z = glm::clamp(point.z, minBounds.z, maxBounds.z);
		return closestPoint;
	}

	bool CollisionHandler::CollideSphereSphere(float dt, RigidBody* bodyA, SphereShape* sphereA,
		RigidBody* bodyB, SphereShape* sphereB)
	{
		if (bodyA->IsStatic() && bodyB->IsStatic())
			return false;

		// TestMovingSphereSphere
		// if There is no a collision detected we will return false
		Vector3 v0 = bodyA->m_Position - bodyA->m_PreviousPosition;
		Vector3 v1 = bodyB->m_Position - bodyB->m_PreviousPosition;
		float t = 0.f;

		if (!TestMovingSphereSphere(
			bodyA->m_PreviousPosition, // We want PreviousPosition
			sphereA->GetRadius(),
			bodyB->m_PreviousPosition, // We want PreviousPosition
			sphereB->GetRadius(),
			v0, v1, t))
		{
			// There is no collision or future collision
			return false;
		}

		if (t > dt) {
			// Collision will not happen this time step
			return false;
		}

		// Otherwise, we will handle the collision here

		// calculate what percent of DT to reverse the Verlet Step

		// This might be wrong...
		float fractDT = t / (glm::length(v0.GetGLM()) + glm::length(v1.GetGLM()));
		float revDT = (1.0f - fractDT) * dt;
		bodyA->VerletStep1(-revDT);
		bodyB->VerletStep1(-revDT);

		// p = mv
		// p: momentum
		// m: mass
		// v: velocity

		// Total Mass & Momentum calculation
		float totalMass = bodyB->m_Mass + bodyA->m_Mass;
		float aFactor = bodyB->m_Mass / totalMass;
		float bFactor = bodyA->m_Mass / totalMass;

		Vector3 aMomentum = bodyA->m_LinearVelocity * bodyA->m_Mass;
		Vector3 bMomentum = bodyB->m_LinearVelocity * bodyB->m_Mass;
		Vector3 totalMomentum = aMomentum + bMomentum;

		aMomentum = totalMomentum * aFactor;
		bMomentum = totalMomentum * bFactor;

		// Check if any part of the objects are still overlapping.
		// We need to nudge the objects back more if they are still overlapping.
		Vector3 vec = bodyB->m_Position - bodyA->m_Position;
		float vecLength = glm::length(vec.GetGLM());
		float overlap = vecLength - (sphereA->GetRadius() + sphereB->GetRadius());

		if (overlap < std::numeric_limits<float>::epsilon())
		{
			Vector3 overlapVec = vec;
			overlapVec = glm::normalize(overlapVec.GetGLM());
			overlapVec *= -overlap;

			// Nudge out of the collision a bit more
			if (!bodyA->m_IsStatic) bodyA->m_Position -= overlapVec * aFactor;
			if (!bodyB->m_IsStatic) bodyB->m_Position += overlapVec * bFactor;

			vec = bodyB->m_Position - bodyA->m_Position;
			vecLength = glm::length(vec.GetGLM());
		}
		vec /= vecLength;

		// Calculate elastic and inelastic momentum for both bodyA and bodyB
		float elasticity = 0.4f;

		Vector3 aElasticMomentum = vec * (glm::length(aMomentum.GetGLM()) * elasticity);
		Vector3 aInelasticMomentum = vec * glm::length(aMomentum.GetGLM()) * (1.f - elasticity);

		Vector3 bElasticMomentum = vec * (glm::length(bMomentum.GetGLM()) * elasticity) * -1.f;
		Vector3 bInelasticMomentum = vec * glm::length(bMomentum.GetGLM()) * (1.f - elasticity);

		// Finally use our elastic & inelastic momentums to calculate a new velocity
		bodyA->m_LinearVelocity -= (aElasticMomentum + aInelasticMomentum) * bodyA->m_InvMass * bodyA->m_Restitution;
		bodyB->m_LinearVelocity += (bElasticMomentum + bInelasticMomentum) * bodyB->m_InvMass * bodyB->m_Restitution;

		// Verlet integrate
		bodyA->VerletStep1(revDT);
		bodyB->VerletStep1(revDT);

		return true;
	}

	bool CollisionHandler::CollideSphereAABB(float dt, RigidBody* sphere, SphereShape* sphereShape
		, RigidBody* bodyAABB, AABBShape* shapeAABBShape)
	{
		Vector3 spherePosition;
		sphere->GetPosition(spherePosition);

		glm::vec3 aabbMin = glm::vec3(shapeAABBShape->Min[0], shapeAABBShape->Min[1], shapeAABBShape->Min[2]);
		glm::vec3 aabbMax = glm::vec3(shapeAABBShape->Max[0], shapeAABBShape->Max[1], shapeAABBShape->Max[2]);
		glm::vec3 aabbSphereClosestPoint = closestPointToAABB(spherePosition.GetGLM(), aabbMin, aabbMax);

		glm::vec3 overlapVector = aabbSphereClosestPoint - sphere->m_Position.GetGLM();
		float overlapLength = glm::length(overlapVector);
		float linearVelocityLength = glm::length(sphere->m_LinearVelocity.GetGLM());
		float angularVelocityLength = glm::length(sphere->m_AngularVelocity.GetGLM());

		if (linearVelocityLength > 0.f || angularVelocityLength > 0.f)
		{
			float velocity = glm::length(sphere->m_LinearVelocity.GetGLM());
			float fractDt = 0.f;
			if (velocity != 0.0f)
			{
				fractDt = sphereShape->GetRadius() * ((sphereShape->GetRadius() / overlapLength) - 1.0f) / velocity;
			}
			float partialDt = (1.f - fractDt) * dt;

			// Reverse the sphere out of the plane
			sphere->VerletStep1(-partialDt);

			// calculate the reflection (Bounce) off the plane
			glm::vec3 prevVelocity = sphere->m_LinearVelocity.GetGLM();
			glm::vec3 reflect = glm::reflect(sphere->m_LinearVelocity.GetGLM(), shapeAABBShape->GetNormal().GetGLM());
			sphere->m_LinearVelocity = reflect;

			// calculate impact info
			glm::vec3 impactComponent = glm::proj(sphere->m_LinearVelocity.GetGLM(), shapeAABBShape->GetNormal().GetGLM());
			glm::vec3 impactTangent = sphere->m_LinearVelocity.GetGLM() - impactComponent;

			glm::vec3 relativePoint = glm::normalize(aabbSphereClosestPoint - sphere->m_Position.GetGLM()) * sphereShape->GetRadius();
			float surfaceVelocity = sphereShape->GetRadius() * glm::length(sphere->m_AngularVelocity.GetGLM());
			glm::vec3 rotationDirection = glm::normalize(glm::cross(relativePoint - sphere->m_Position.GetGLM(), sphere->m_AngularVelocity.GetGLM()));

			// Detect if we are bouncing off the plane, or "moving" along it.
			if (glm::dot(impactTangent, shapeAABBShape->GetNormal().GetGLM()) > 0.f)
			{
				sphere->ApplyImpulseAtPoint(-2.f * impactComponent * sphere->m_Mass, relativePoint);
				//sphere->ApplyImpulseAtPoint( , relativePoint);
			}
			else
			{
				glm::vec3 impactForce = impactTangent * -1.f * sphere->m_Mass * bodyAABB->m_Friction;
				sphere->ApplyForceAtPoint(impactForce, relativePoint);
			}

			if (glm::dot(sphere->m_LinearVelocity.GetGLM(), shapeAABBShape->GetNormal().GetGLM()) == 0.0f)
			{
				glm::vec3 force = surfaceVelocity * rotationDirection * sphere->m_Mass * bodyAABB->m_Friction;
				sphere->ApplyForce(force);
			}

			sphere->UpdateAcceleration();

			// Move the sphere into the new direction
			sphere->VerletStep1(partialDt);

			//if (glm::length(impactTangent) > 0.001f)
			//{
			//	sphere->m_LinearVelocity += impactTangent * 0.1f;
			//}

			// Here we ensure we are on the right side of the plane
			//closestPoint = ClosestPtPointPlane(sphere->m_Position.GetGLM(), planeShape->GetNormal().GetGLM(), planeShape->GetDotProduct());
			aabbSphereClosestPoint = closestPointToAABB(sphere->m_Position.GetGLM(), aabbMin, aabbMax);
			overlapVector = aabbSphereClosestPoint - sphere->m_Position.GetGLM();
			overlapLength = glm::length(overlapVector);
			//if (aabbSphereClosestPoint > glm::vec3(0.f));
			if (overlapLength < sphereShape->GetRadius())
			{
				// we are still colliding!!!

				sphere->m_Position += shapeAABBShape->GetNormal() * (sphereShape->GetRadius() - overlapLength);

				float velocityDotNormal = glm::dot(sphere->m_LinearVelocity.GetGLM(), shapeAABBShape->GetNormal().GetGLM());

				if (velocityDotNormal < 0.f)
				{
					sphere->m_LinearVelocity -= shapeAABBShape->GetNormal() * velocityDotNormal;
				}
			}
			else
			{
				sphere->m_LinearVelocity *= sphere->m_Restitution;
			}

		}
		else
		{
			return false;
		}

		return true;
	}

	bool CollisionHandler::CollideSpherePlane(float dt, RigidBody* sphere, SphereShape* sphereShape,
		RigidBody* plane, PlaneShape* planeShape)
	{
		// TestMovingSpherePlane
		if (!TestMovingSpherePlane(sphere->m_PreviousPosition, sphere->m_Position, sphereShape->GetRadius(),
			planeShape->GetNormal(), planeShape->GetDotProduct()))
		{
			return false;
		}

		glm::vec3 closestPoint = ClosestPtPointPlane(sphere->m_Position.GetGLM(), planeShape->GetNormal().GetGLM(), planeShape->GetDotProduct());
		glm::vec3 overlapVector = closestPoint - sphere->m_Position.GetGLM();
		float overlapLength = glm::length(overlapVector);
		float linearVelocityLength = glm::length(sphere->m_LinearVelocity.GetGLM());
		float angularVelocityLength = glm::length(sphere->m_AngularVelocity.GetGLM());

		if (linearVelocityLength > 0.f || angularVelocityLength > 0.f)
		{
			float velocity = glm::length(sphere->m_LinearVelocity.GetGLM());
			float fractDt = 0.f;
			if (velocity != 0.0f)
			{
				fractDt = sphereShape->GetRadius() * ((sphereShape->GetRadius() / overlapLength) - 1.0f) / velocity;
			}
			float partialDt = (1.f - fractDt) * dt;

			// Reverse the sphere out of the plane
			sphere->VerletStep1(-partialDt);

			// calculate the reflection (Bounce) off the plane
			glm::vec3 prevVelocity = sphere->m_LinearVelocity.GetGLM();
			glm::vec3 reflect = glm::reflect(sphere->m_LinearVelocity.GetGLM(), planeShape->GetNormal().GetGLM());
			sphere->m_LinearVelocity = reflect;

			// calculate impact info
			glm::vec3 impactComponent = glm::proj(sphere->m_LinearVelocity.GetGLM(), planeShape->GetNormal().GetGLM());
			glm::vec3 impactTangent = sphere->m_LinearVelocity.GetGLM() - impactComponent;


			glm::vec3 relativePoint = glm::normalize(closestPoint - sphere->m_Position.GetGLM()) * sphereShape->GetRadius();
			float surfaceVelocity = sphereShape->GetRadius() * glm::length(sphere->m_AngularVelocity.GetGLM());
			glm::vec3 rotationDirection = glm::normalize(glm::cross(relativePoint - sphere->m_Position.GetGLM(), sphere->m_AngularVelocity.GetGLM()));



			// Detect if we are bouncing off the plane, or "moving" along it.
			if (glm::dot(impactTangent, planeShape->GetNormal().GetGLM()) > 0.f)
			{
				sphere->ApplyImpulseAtPoint(-2.f * impactComponent * sphere->m_Mass, relativePoint);
				//sphere->ApplyImpulseAtPoint( , relativePoint);
			}
			else
			{
				glm::vec3 impactForce = impactTangent * -1.f * sphere->m_Mass * plane->m_Friction;
				sphere->ApplyForceAtPoint(impactForce, relativePoint);
			}

			if (glm::dot(sphere->m_LinearVelocity.GetGLM(), planeShape->GetNormal().GetGLM()) == 0.0f)
			{
				glm::vec3 force = surfaceVelocity * rotationDirection * sphere->m_Mass * plane->m_Friction;
				sphere->ApplyForce(force);
			}

			sphere->UpdateAcceleration();

			// Move the sphere into the new direction
			sphere->VerletStep1(partialDt);

			//if (glm::length(impactTangent) > 0.001f)
			//{
			//	sphere->m_Velocity += impactTangent * 0.1f;
			//}

			// Here we ensure we are on the right side of the plane
			closestPoint = ClosestPtPointPlane(sphere->m_Position.GetGLM(), planeShape->GetNormal().GetGLM(), planeShape->GetDotProduct());
			overlapVector = closestPoint - sphere->m_Position.GetGLM();
			overlapLength = glm::length(overlapVector);
			if (overlapLength < sphereShape->GetRadius())
			{
				// we are still colliding!!!

				sphere->m_Position += planeShape->GetNormal() * (sphereShape->GetRadius() - overlapLength);

				float velocityDotNormal = glm::dot(sphere->m_LinearVelocity.GetGLM(), planeShape->GetNormal().GetGLM());

				if (velocityDotNormal < 0.f)
				{
					sphere->m_LinearVelocity -= planeShape->GetNormal() * velocityDotNormal;
				}
			}
			else
			{
				sphere->m_LinearVelocity *= sphere->m_Restitution;
			}

		}
		else
		{
			return false;
		}

		return true;
	}


	// Calculate the collision normal between two AABBs
	glm::vec3 getCollisionNormal(glm::vec3 aabb1Center, glm::vec3 aabb2Center)
	{
		// Calculate the displacement vector between the centers of the two AABBs		
		glm::vec3 displacement = aabb2Center - aabb1Center;

		// Calculate the absolute value of the displacement vector
		glm::vec3 absDisplacement = abs(displacement);

		// Determine which axis has the largest displacement
		if (absDisplacement.x > absDisplacement.y && absDisplacement.x > absDisplacement.z)
		{
			// Collision is along the x-axis
			return displacement.x > 0 ? glm::vec3(1, 0, 0) : glm::vec3(-1, 0, 0);
		}
		else if (absDisplacement.y > absDisplacement.x && absDisplacement.y > absDisplacement.z)
		{
			// Collision is along the y-axis
			return displacement.y > 0 ? glm::vec3(0, 1, 0) : glm::vec3(0, -1, 0);
		}
		else
		{
			// Collision is along the z-axis
			return displacement.z > 0 ? glm::vec3(0, 0, 1) : glm::vec3(0, 0, -1);
		}
	}

	// Calculate the impulse to apply to a dynamic AABB in response to a collision
	glm::vec3 computeImpulse(RigidBody* dynamicBody, RigidBody* staticBody, const glm::vec3 & collisionNormal)
	{
		// Calculate the relative velocity between the two AABBs
		Vector3 dynamicBodyVel;
		Vector3 staticBodyVel;

		dynamicBody->GetVelocity(dynamicBodyVel);
		staticBody->GetVelocity(staticBodyVel);

		glm::vec3 relativeVelocity = dynamicBodyVel.GetGLM() - staticBodyVel.GetGLM();

		// Calculate the relative velocity along the collision normal
		float velocityAlongNormal = dot(relativeVelocity, collisionNormal);

		// Calculate the restitution coefficient for the collision
		float dynamicBodyRestitution;
		float staticBodyRestitution;

		dynamicBody->GetRestitution(dynamicBodyRestitution);
		staticBody->GetRestitution(staticBodyRestitution);

		float dynamicBodyMass;
		float staticBodyMass;

		dynamicBody->GetMass(dynamicBodyMass);
		staticBody->GetMass(staticBodyMass);

		float e = (dynamicBodyRestitution + staticBodyRestitution) / 2.0f;

		// Calculate the impulse scalar
		float j = -(1 + e) * velocityAlongNormal;
		j /= 1 / dynamicBodyMass + 1 / staticBodyMass;

		// Return the impulse vector
		return j * collisionNormal;
	}

	bool CollisionHandler::CollideAABBxAABB(float dt, RigidBody* rigidA, AABBShape* shapeA, RigidBody* rigidB, AABBShape* shapeB) {
		//printf("Should be colliding\n");
		bool collide = true;

		Vector3 shapeAPos;
		rigidA->GetPosition(shapeAPos);

		Vector3 shapeBPos;
		rigidB->GetPosition(shapeBPos);

		glm::vec3 shapeAMin = glm::vec3(shapeA->Min[0] + shapeAPos.x,
			shapeA->Min[1] + shapeAPos.y,
			shapeA->Min[2] + shapeAPos.z);
		glm::vec3 shapeAMax = glm::vec3(shapeA->Max[0] + shapeAPos.x,
			shapeA->Max[1] + shapeAPos.y,
			shapeA->Max[2] + shapeAPos.z);

		glm::vec3 shapeBMin = glm::vec3(shapeB->Min[0] + shapeBPos.x,
			shapeB->Min[1] + shapeBPos.y,
			shapeB->Min[2] + shapeBPos.z);
		glm::vec3 shapeBMax = glm::vec3(shapeB->Max[0] + shapeBPos.x,
			shapeB->Max[1] + shapeBPos.y,
			shapeB->Max[2] + shapeBPos.z);


		if (shapeAMax.x < shapeBMin.x || shapeAMin.x > shapeBMax.x) {
			printf("(shapeAMax.x < shapeBMin.x || shapeAMin.x > shapeBMax.x)\n");
			//return false;
			collide = false;
		}

		if (shapeAMax.y < shapeBMin.y || shapeAMin.y > shapeBMax.y) {
			printf("(shapeAMax.y < shapeBMin.y || shapeAMin.y > shapeBMax.y)\n");
			//return false;
			collide = false;
		}

		if (shapeAMax.z < shapeBMin.z || shapeAMin.z > shapeBMax.z) {
			printf("(shapeAMax.z < shapeBMin.z || shapeAMin.z > shapeBMax.z)\n");
			//return false;
			collide = false;
		}

		if (!collide) {
			// Apply an impulse to the first AABB in the direction of the collision normal
			glm::vec3 shapeACenter = (shapeAMin + shapeAMax) / 2.0f;
			glm::vec3 shapeBCenter = (shapeBMin + shapeBMax) / 2.0f;
			glm::vec3 collisionNormal = getCollisionNormal(shapeACenter, shapeBCenter);
			
			if (rigidA->IsStatic()) {
				glm::vec3 impulse = computeImpulse(rigidB, rigidA, collisionNormal);
				rigidB->ApplyImpulse(impulse);
			}
			else {
				glm::vec3 impulse = computeImpulse(rigidA, rigidB, collisionNormal);
				rigidA->ApplyImpulse(impulse);
			}
			
			//glm::vec3 impulse = computeImpulse(rigidBody[i], rigidBody[j], collisionNormal);
			//rigidBody[i].applyImpulse(impulse);
			return collide;
		}

		return collide;
	}

	void CollisionHandler::Collide(float dt, std::vector<iCollisionBody*>& bodies, std::vector<CollidingBodies>& collisions)
	{
		int bodyCount = bodies.size();
		bool collision = false;
		for (int idxA = 0; idxA < bodyCount - 1; idxA++)
		{
			iCollisionBody* bodyA = bodies[idxA];

			for (int idxB = idxA + 1; idxB < bodyCount; idxB++)
			{
				iCollisionBody* bodyB = bodies[idxB];

				collision = false;

				// CHECK WHICH BODY TYPES...
				if (bodyA->GetBodyType() == BodyType::RigidBody)
				{
					if (bodyB->GetBodyType() == BodyType::RigidBody)
					{
						collision = CollideRigidRigid(dt, RigidBody::Cast(bodyA), RigidBody::Cast(bodyB));
					}
					else if (bodyB->GetBodyType() == BodyType::SoftBody)
					{
						collision = CollideRigidSoft(dt, RigidBody::Cast(bodyA), SoftBody::Cast(bodyB));
					}
					else
					{
						// We don't know this type of body
					}
				}
				else if (bodyA->GetBodyType() == BodyType::SoftBody)
				{
					if (bodyB->GetBodyType() == BodyType::RigidBody)
					{
						collision = CollideRigidSoft(dt, RigidBody::Cast(bodyB), SoftBody::Cast(bodyA));
					}
					else if (bodyB->GetBodyType() == BodyType::SoftBody)
					{
						collision = CollideSoftSoft(dt, SoftBody::Cast(bodyA), SoftBody::Cast(bodyB));
					}
					else
					{
						// We don't know this type of body
					}
				}
				else
				{
					// We don't know this type of body
				}


				if (collision)
				{
					collisions.push_back(CollidingBodies(bodyA, bodyB));
				}
			}
		}
	}

	bool CollisionHandler::CollideRigidRigid(float dt, RigidBody* rigidA, RigidBody* rigidB)
	{
		iShape* shapeA = rigidA->GetShape();
		iShape* shapeB = rigidB->GetShape();

		bool collision = false;

		if (shapeA->GetShapeType() == ShapeType::Sphere)
		{
			if (shapeB->GetShapeType() == ShapeType::Sphere)
			{
				collision = CollideSphereSphere(dt, rigidA, SphereShape::Cast(shapeA), rigidB, SphereShape::Cast(shapeB));
			}
			else if (shapeB->GetShapeType() == ShapeType::Plane)
			{
				collision = CollideSpherePlane(dt, rigidA, SphereShape::Cast(shapeA), rigidB, PlaneShape::Cast(shapeB));
			}
			else if (shapeB->GetShapeType() == ShapeType::Box)
			{
				// CollideSphereBox(dt, bodyA, SphereShape::Cast(shapeA), bodyB, BoxShape::Cast(shapeB));
			}
			else if (shapeB->GetShapeType() == ShapeType::AABB)
			{
				collision = CollideSphereAABB(dt, rigidA, SphereShape::Cast(shapeA), rigidB, AABBShape::Cast(shapeB));
			}
			else
			{
				// We don't have this handled at the moment.
			}
		}
		else if (shapeA->GetShapeType() == ShapeType::Plane)
		{
			if (shapeB->GetShapeType() == ShapeType::Sphere)
			{
				collision = CollideSpherePlane(dt, rigidB, SphereShape::Cast(shapeB), rigidA, PlaneShape::Cast(shapeA));
			}
			else if (shapeB->GetShapeType() == ShapeType::Plane)
			{
				// nope...
			}
			else
			{
				// We don't have this handled at the moment.
			}
		}
		else if (shapeA->GetShapeType() == ShapeType::AABB)
		{
			if (shapeB->GetShapeType() == ShapeType::AABB)
			{
				collision = CollideAABBxAABB(dt, rigidA, AABBShape::Cast(shapeA), rigidB, AABBShape::Cast(shapeB));
			}
		}
		else
		{
			// We don't have this handled at the moment.
		}

		return collision;
	}

	bool CollisionHandler::CollideRigidSoft(float dt, RigidBody* rigidA, SoftBody* softB)
	{
		iShape* shapeA = rigidA->m_Shape;

		bool collision = false;

		if (shapeA->GetShapeType() == ShapeType::Sphere)
		{
			unsigned int numNodes = softB->GetNumNodes();
			for (unsigned int i = 0; i < numNodes; i++)
			{
				if (CollideSphereSphere(dt, rigidA, SphereShape::Cast(shapeA),
					softB->m_Nodes[i], SphereShape::Cast(softB->m_Nodes[i]->m_Shape)))
				{
					collision = true;
				}
			}
		}
		else if (shapeA->GetShapeType() == ShapeType::Plane)
		{
			unsigned int numNodes = softB->GetNumNodes();
			for (unsigned int i = 0; i < numNodes; i++)
			{
				if (CollideSpherePlane(dt,
					softB->m_Nodes[i], SphereShape::Cast(softB->m_Nodes[i]->m_Shape),
					rigidA, PlaneShape::Cast(shapeA)))
				{
					collision = true;
				}
			}
		}
		else
		{
			// Implement more shapes!
		}

		return collision;
	}

	bool CollisionHandler::CollideSoftSoft(float dt, SoftBody* softA, SoftBody* softB)
	{
		glm::vec3 minBoundsA = softA->GetMinBounds();
		glm::vec3 maxBoundsA = softA->GetMaxBounds();
		glm::vec3 minBoundsB = softB->GetMinBounds();
		glm::vec3 maxBoundsB = softB->GetMaxBounds();

		if (maxBoundsA.x < minBoundsB.x) return false;
		if (maxBoundsA.y < minBoundsB.y) return false;
		if (maxBoundsA.z < minBoundsB.z) return false;

		if (minBoundsA.x > maxBoundsB.x) return false;
		if (minBoundsA.y > maxBoundsB.y) return false;
		if (minBoundsA.z > maxBoundsB.z) return false;

		return true;
	}

	void CollisionHandler::CollideInternalSoftBody(float dt, SoftBody* softBody)
	{
		unsigned int numNodes = softBody->m_Nodes.size();

		for (unsigned int idxA = 0; idxA < numNodes - 1; idxA++)
		{
			SoftBodyNode* nodeA = softBody->m_Nodes[idxA];
			for (unsigned int idxB = idxA + 1; idxB < numNodes; idxB++)
			{
				SoftBodyNode* nodeB = softBody->m_Nodes[idxB];

				if (!nodeA->IsNeighbour(nodeB))
				{
					CollideSphereSphere(dt, nodeA, SphereShape::Cast(nodeA->GetShape()), nodeB, SphereShape::Cast(nodeB->GetShape()));
				}
			}
		}
		softBody->UpdateBoundaries();
	}

	// Distance From a Sphere to AABB
	float CollisionHandler::SqDistPointAABB(Vector3 p, physics::AABBShape* b) {
		float sqDist = 0.0f;

		// x, y, z
		// 0, 1, 2

		//for (int i = 0; i < 3; i++) {
		//	// For each axis count any excess distance outside box extents
		//	float v = p[i];
		//	if (v < b.Min[i]) sqDist += (b.Min[i] - v) * (b.Min[i] - v);
		//	if (v > b.Max[i]) sqDist += (v - b.Max[i]) * (v - b.Max[i]);
		//}
		float v;
		v = p.x;
		if (v < b->Min[0]) sqDist += (b->Min[0] - v) * (b->Min[0] - v);
		if (v > b->Max[0]) sqDist += (v - b->Max[0]) * (v - b->Max[0]);

		v = p.y;
		if (v < b->Min[1]) sqDist += (b->Min[1] - v) * (b->Min[1] - v);
		if (v > b->Max[1]) sqDist += (v - b->Max[1]) * (v - b->Max[1]);

		v = p.z;
		if (v < b->Min[2]) sqDist += (b->Min[2] - v) * (b->Min[2] - v);
		if (v > b->Max[2]) sqDist += (v - b->Max[2]) * (v - b->Max[2]);

		return sqDist;
	}
}
