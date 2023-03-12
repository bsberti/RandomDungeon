#pragma once
#include <glm/vec3.hpp>

class cParticle {

public:
	// Default Constructor
	cParticle();
	// Overloaded Constructor 
	// Accepts a particle
	cParticle(const cParticle& particle);
	// Destructor
	~cParticle();

	// Overloaded Copy Operator
	cParticle operator=(const cParticle& particle);

	// Method to Integrate the Particle on a specified Delta Time
	void Integrate(float dt);

	// Getters and Setters
	void setPosition(glm::vec3 position);
	void setVelocity(glm::vec3 velocity);
	void setAcceleration(glm::vec3 acceleration);
	void setAge(float age);
	void setDamping(float damping);
	void setMass(float mass);
	// ---
	glm::vec3 getPosition();
	glm::vec3 getVelocity();
	glm::vec3 getAcceleration();
	float getAge();
	float getDamping();
	float getMass();

protected:
	// Variables of the Particle
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	float	 age;		// ttl
	float	 damping;
	float	 mass;		// f = ma
	
};

