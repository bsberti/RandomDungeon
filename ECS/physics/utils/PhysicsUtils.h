#pragma once

class PhysicsWorld;
class PhysicsUtils
{
public:
	static PhysicsWorld* GetActiveWorld();
	static void SetActiveWorld(PhysicsWorld* world);

private:
	static PhysicsWorld* s_ActivePhysicsWorld;
};