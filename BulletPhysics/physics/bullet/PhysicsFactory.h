#ifndef PhysicsFactory_H
#define PhysicsFactory_H

#include <physics/interfaces/iPhysicsFactory.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>

namespace physics
{
	class PhysicsFactory : public iPhysicsFactory
	{
	public:
		PhysicsFactory();
		virtual ~PhysicsFactory();

		virtual iPhysicsWorld* CreateWorld() override;
		virtual iRigidBody* CreateRigidBody(const RigidBodyDesc& desc, iShape* shape) override;
		virtual iSoftBody* CreateSoftBody(const SoftBodyDesc& desc) override;
		virtual iCharacterController* CreateCharacterController(iConvexShape* shape, float stepHeight, const Vector3& up) override;
	private:

		//btDiscreteDynamicsWorld* m_DynamicsWorld;
	};
}

#endif
