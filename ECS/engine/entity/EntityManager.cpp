#include <engine/entity/Entitymanager.h>

#include <assert.h>

EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
}

unsigned int EntityManager::CreateEntity()
{
	Entity* newEntity = new Entity();
	m_Entities.push_back(newEntity);
	return m_Entities.size() - 1;
}

const std::vector<Entity*>& EntityManager::GetEntities()
{
	return m_Entities;
}
