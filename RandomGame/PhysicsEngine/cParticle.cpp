#include "cParticle.h"
#include <iostream>

#ifdef _DEBUG
#define DEBUG_LOG_ENABLED
#endif
#ifdef DEBUG_LOG_ENABLED
#define DEBUG_PRINT(x, ...) printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#endif

// Default Constructor
cParticle::cParticle() : position(0.0f)
, velocity(0.0f)
, acceleration(0.0f, -9.8f, 0.0f) // Gravity
, damping(1.0f)
, mass(1.0f)
, age(-1.0f) {
	DEBUG_PRINT("Particle::Particle();\n");
}

// Destructor
cParticle::~cParticle() {
	DEBUG_PRINT("cParticle::~cParticle();\n");
}

// Overloaded Constructor
cParticle::cParticle(const cParticle& particle) {
	DEBUG_PRINT("cParticle::cParticle(const cParticle& particle);\n");
	this->position = particle.position;
	this->velocity = particle.velocity;
	this->acceleration = particle.acceleration;
	this->damping = particle.damping;
	this->mass = particle.mass;
	this->age = particle.age;
}

// Overloaded Copy Operator
cParticle cParticle::operator=(const cParticle& particle) {
	DEBUG_PRINT("cParticle::operator=();\n");

	return cParticle(particle);
}

// Method to Integrate the Particle on a specified Delta Time
void cParticle::Integrate(float dt) {
	//velocity = velocity + acceleration * dt;
	//velocity *= damping;
	//position = position + velocity * dt;
	velocity = velocity + (acceleration * dt);
	velocity *= damping;
	position = position + (velocity * dt);

	// Aging the Particle
	age -= dt;
}

// Getters and Setters
void cParticle::setPosition(glm::vec3 position) {
	this->position = position;
}

void cParticle::setVelocity(glm::vec3 velocity) {
	this->velocity = velocity;
}

void cParticle::setAcceleration(glm::vec3 acceleration) {
	this->acceleration = acceleration;
}

void cParticle::setAge(float age) {
	this->age = age;
}

void cParticle::setDamping(float damping) {
	this->damping = damping;
}

void cParticle::setMass(float mass) {
	this->mass = mass;
}

glm::vec3 cParticle::getPosition() {
	return this->position;
}

glm::vec3 cParticle::getVelocity() {
	return this->velocity;
}

glm::vec3 cParticle::getAcceleration() {
	return this->acceleration;
}

float cParticle::getAge() {
	return this->age;
}

float cParticle::getDamping() {
	return this->damping;
}

float cParticle::getMass() {
	return this->mass;
}