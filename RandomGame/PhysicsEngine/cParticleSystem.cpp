#include "cParticleSystem.h"
#include <iostream>
#include <iomanip>

//#define DEBUG_LOG_ENABLED
#ifdef DEBUG_LOG_ENABLED
#define DEBUG_PRINT(x, ...) printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#endif

// Default Constructor
cParticleSystem::cParticleSystem() {
	DEBUG_PRINT("cParticleSystem::cParticleSystem();\n");
	cParticleSystem(glm::vec3(0.0f), 10);
}

// Overloaded Constructor 
// Accepts a position and the number of particles to be generated
cParticleSystem::cParticleSystem(const glm::vec3 position, unsigned int numParticles) {
	DEBUG_PRINT("cParticleSystem::cParticleSystem(const glm::vec3 position, unsigned int numParticles);\n");
	this->position = position;
	this->numParticles = numParticles;
	this->numParticlesAvail = numParticles;
	this->indexAvailParticle = 0;
	this->particles.resize(numParticles);
	for (int i = 0; i < numParticles; i++) {
		particles[i].setPosition(position);
	}
}

// Destructor
cParticleSystem::~cParticleSystem() {
	DEBUG_PRINT("cParticleSystem::~cParticleSystem();\n");
	for (int i = 0; i < numParticles; i++) {
		particles[i].~cParticle();
	}
}

// Method that grabs a dead Particlle from the pool and make it alive
// accepts Up Vector
void cParticleSystem::AllocateParticle(glm::vec3 upVector) {
	DEBUG_PRINT("cParticleSystem::AllocateParticle(munition type);\n");
	// Checks if there are particles available
	if (numParticlesAvail == 0) {
		DEBUG_PRINT("cParticleSystem::AllocateParticle - Error - All particles are alive, can't create a new particle!\n");
		return;
	}
	cParticle& p = particles[indexAvailParticle];
	if (p.getAge() >= 0) {
		DEBUG_PRINT("cParticleSystem::AllocateParticle - Error - Oldest particle still alive, can't create a new particle!\n");
		return;
	}

	// Transform the dead particle into a living one with static age of 5s
	p.setVelocity(upVector);
	p.setAge(5.0f);

	// Particle allocated, one less particle available
	this->numParticlesAvail--;
	// Checks if index is at the end of the vector, if so index = 0, else index++
	indexAvailParticle == (numParticles - 1) ? indexAvailParticle = 0 : indexAvailParticle++;
	return;
}

// Method that grabs a dead Particlle from the pool and make it alive
// accepts Position Up Vector, Acceleration, Age, Damping and Mass
cParticle* cParticleSystem::AllocateParticle(glm::vec3 position, glm::vec3 upVector, glm::vec3 acceleration, float age, float damping, float mass) {
	DEBUG_PRINT("cParticleSystem::AllocateParticle(munition type);\n");
	// Checks if there are particles available
	if (numParticlesAvail == 0) {
		DEBUG_PRINT("cParticleSystem::AllocateParticle - Error - All particles are alive, can't create a new particle!\n");
		return nullptr;
	}
	// Cheks if particle available at index is alive
	cParticle& p = particles[indexAvailParticle];
	if (p.getAge() >= 0) {
		DEBUG_PRINT("cParticleSystem::AllocateParticle - Error - Oldest particle still alive, can't create a new particle!\n");
		return nullptr;
	}

	// Transform the dead particle into a living one
	p.setPosition(position);
	p.setVelocity(upVector);
	p.setAcceleration(acceleration);
	p.setAge(age);
	p.setDamping(damping);
	p.setMass(mass);

	// Particle allocated, one less particle available
	this->numParticlesAvail--;
	// Checks if index is at the end of the vector, if so index = 0, else index++
	indexAvailParticle == (numParticles - 1) ? indexAvailParticle = 0 : indexAvailParticle++;
	return &p;
}

// Method that Integrate all living particles
// Accepts float deltaType
void cParticleSystem::Integrate(float dt) {
	for (int i = 0; i < numParticles; i++) {
		cParticle& p = particles[i];
		// Checks if the particle is alive
		if (p.getAge() > 0) {
			p.Integrate(dt);
			// Checks if the position of the particle isn't the ground
			if (p.getPosition().y <= 0.0f) {
				// Reset the particle if its on the ground
				p.setAge(-1);
				p.setPosition(this->position);
				numParticlesAvail++;
				DEBUG_PRINT("ParticleSystem::Integrate Particle Reseted\n");
			}
		} else if (p.getAge() < 0 && p.getAge() != -1) {
			// Reset the particle if its dead
			p.setAge(-1);
			p.setPosition(this->position);
			numParticlesAvail++;
			DEBUG_PRINT("ParticleSystem::Integrate Particle Reseted\n");
		}
	}
}

// Method that cheks distance between two vectors
float cParticleSystem::getDistanceTwoVectors(glm::vec3 v1, glm::vec3 v2) {
	float xdist = v1.x - v2.x;
	xdist *= xdist;

	float ydist = v1.y - v2.y;
	ydist *= ydist;

	float zdist = v1.z - v2.z;
	zdist *= zdist;

	return sqrt(xdist + ydist + zdist);
}

// Method that resets all particles
void cParticleSystem::killAllParticles() {
	for (int i = 0; i < numParticles; i++) {
		particles[i].setAge(-1);
	}
	this->numParticlesAvail = this->numParticles;
}

// Getters and Setters
void cParticleSystem::setPosition(glm::vec3 position) {
	this->position = position;
	// Also set position of all dead particles to the new position
	for (int i = 0; i < numParticles; i++) {
		cParticle& p = particles[i];
		if (p.getAge() < 0)
			p.setPosition(this->position);
	}
}

cParticle* cParticleSystem::getParticle(unsigned int index) {
	return &this->particles[index];
}

unsigned int cParticleSystem::getNumParticles() {
	return this->numParticles;
}

unsigned int cParticleSystem::getNumParticlesAvail() {
	return this->numParticlesAvail;
}