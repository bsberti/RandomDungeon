#include <systems/motionsystem.h>

#include <components/positioncomponent.h>
#include <components/velocitycomponent.h>

//PositionComponent* FindPositionComponent(const std::vector<Component*>& components)
//{
//	PositionComponent* cmp = nullptr;
//	for (int cmpIdx = 0; cmpIdx < components.size(); cmpIdx++)
//	{
//		cmp = dynamic_cast<PositionComponent*>(components[cmpIdx]);
//		if (cmp != nullptr)
//			return cmp;
//	}
//	return nullptr;
//}
//
//
//VelocityComponent* FindVelocityComponent(const std::vector<Component*>& components)
//{
//	VelocityComponent* cmp = nullptr;
//	for (int cmpIdx = 0; cmpIdx < components.size(); cmpIdx++)
//	{
//		cmp = dynamic_cast<VelocityComponent*>(components[cmpIdx]);
//		if (cmp != nullptr)
//			return cmp;
//	}
//	return nullptr;
//}

void MotionSystem::Process(const std::vector<Entity*>& entities, float dt)
{
	printf("MotionSystem::Process...\n");
	PositionComponent* positionCmp = nullptr;
	VelocityComponent* velocityCmp = nullptr;

	for (int entityIdx = 0, numEntities = entities.size(); entityIdx < numEntities; ++entityIdx)
	{
		Entity* entity = entities[entityIdx];

		positionCmp = entity->GetComponentByType<PositionComponent>();
		velocityCmp = entity->GetComponentByType<VelocityComponent>();

		if (positionCmp != nullptr && velocityCmp != nullptr)
		{
			printf("  Processing entity...\n");
			positionCmp->x += velocityCmp->vx * dt;
			positionCmp->y += velocityCmp->vy * dt;
			positionCmp->z += velocityCmp->vz * dt;
		}
	}
}