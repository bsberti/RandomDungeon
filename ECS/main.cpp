#include <engine/engine.h>
#include <systems/motionsystem.h>
#include <physics/PhysicsSystem.h>
#include <physics/components/rigidbodycomponent.h>
#include <components/positioncomponent.h>
#include <components/velocitycomponent.h>

Engine g_Engine;

unsigned int CreateAnEntity() {
	unsigned int entity = g_Engine.CreateEntity();
	RigidBodyComponent* rbc = g_Engine.AddComponent<RigidBodyComponent>(entity);
	//rbc->SetRigidBody();
	//rbc->SetPhysicsMaterial();
	// Some way to customize the rbc
	// You want to assign the shape yourself, and collider info


	return entity;
}

struct PositionVelocityEntityType {
	PositionComponent position;
	VelocityComponent velocity;
};

std::vector<PositionVelocityEntityType> position_velocity_types;


int main(int argc, char** argv) {

	g_Engine.Initialize();

	MotionSystem motion;
	PhysicsSystem physics;

	g_Engine.CreateWindow("Multiverse Cursed Village", 800, 600);

	//g_Engine.AddSystem(&motion);
	//g_Engine.AddSystem(&physics);
	//
	//g_Engine.Update(0.1f);
	//g_Engine.Update(0.1f);
	//g_Engine.Update(0.1f);
	//
	//unsigned int entity = CreateAnEntity();
	//
	//g_Engine.Update(0.1f);
	//g_Engine.Update(0.1f);
	//g_Engine.Update(0.1f);


	//VelocityComponent* velCmp = g_Engine.AddComponent<VelocityComponent>(entity);
	//velCmp->vx = 10;
	//velCmp->vy = 0;
	//velCmp->vz = 0;

	//g_Engine.Update(0.1f);
	//g_Engine.Update(0.1f);
	//g_Engine.Update(0.1f);
	//
	g_Engine.RemoveSystem(&motion);
	g_Engine.RemoveSystem(&physics);

	return 0;
}
