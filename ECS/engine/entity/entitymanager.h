#pragma once

#include <assert.h>
#include <vector>
#include <map>

#include <engine/ecs/Entity.h>


class EntityManager
{
public:
	EntityManager();
	~EntityManager();

	unsigned int CreateEntity();

	template <class T>
	T* AddComponent(unsigned int entityId)
	{
		assert(entityId < m_Entities.size());
		Entity* entity = m_Entities[entityId];
		return entity->AddComponent<T>();
	}

	template <class T>
	void RemoveComponent(unsigned int entityId)
	{
		assert(entityId < m_Entities.size());
		Entity* entity = m_Entities[entityId];
		entity->RemoveComponent<T>();
	}

	const std::vector<Entity*>& GetEntities();

private:
	std::vector<Entity*> m_Entities;
	std::map<unsigned int, std::vector<Component*>> m_EntityTypes;
};