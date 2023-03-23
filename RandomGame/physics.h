#pragma once

#include <physics\interfaces\ShapeType.h>
#include <physics\interfaces\iShape.h>
#include <physics\interfaces\iRigidBody.h>
#include <physics\interfaces\iSoftBody.h>
#include <physics\interfaces\iPhysicsWorld.h>
#include <physics\interfaces\iPhysicsFactory.h>
#include <physics\interfaces\SphereShape.h>
#include <physics\interfaces\PlaneShape.h>
#include <physics\interfaces\BoxShape.h>

#define Bullet_Physics

#if defined GDP_Physics
#pragma comment(lib, "GDPPhysics.lib")
#include <physics\gdp\PhysicsFactory.h>
#elif defined Bullet_Physics
#pragma comment(lib, "BulletSource.lib")
#pragma comment(lib, "BulletPhysics.lib")
#include <physics\bullet\PhysicsFactory.h>
#elif defined PhysX_Physics
#pragma comment(lib, "PhysXPhysics.lib")
#pragma comment(lib, "PhysXSource.lib")
#include <physics\physx\PhysicsFactory.h>
#endif

typedef physics::PhysicsFactory PhysicsFactoryType;