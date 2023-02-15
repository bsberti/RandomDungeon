#pragma once

#include <physics/Physicsworld.h>
//class PhysicsWorld;
class PhysicsUtils
{
public:
	static PhysicsWorld* GetActiveWorld();
	static void SetActiveWorld(PhysicsWorld* world);

private:
	//TODO: Uncomment this and solve the linker error
	//static PhysicsWorld* s_ActivePhysicsWorld;
};