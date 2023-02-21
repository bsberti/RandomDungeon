#include "Engine.h"

Engine::Engine()
{
}

Engine::~Engine()
{

}

void Engine::Initialize()
{
	m_RenderSystem = new RenderSystem();
}

void Engine::Destroy()
{
	delete m_RenderSystem;
}

void Engine::Update(float dt)
{
	for (int i = 0; i < m_Systems.size(); i++)
	{
		m_Systems[i]->Process(m_EntityManager.GetEntities(), dt);
	}
}

void Engine::Render()
{
	m_RenderSystem->Process(m_EntityManager.GetEntities(), 0.f);
}

unsigned int Engine::CreateEntity()
{
	return m_EntityManager.CreateEntity();
}

void Engine::AddSystem(System* system)
{
	std::vector<System*>::iterator itFind = 
		std::find(m_Systems.begin(), m_Systems.end(), system);

	if (itFind != m_Systems.end())
	{
		printf("System already added!\n");
		return;
	}

	m_Systems.push_back(system);
}

void Engine::RemoveSystem(System* system)
{
	std::vector<System*>::iterator itFind =
		std::find(m_Systems.begin(), m_Systems.end(), system);

	if (itFind == m_Systems.end())
	{
		printf("System is not added!\n");
		return;
	}

	m_Systems.erase(itFind);
}

void Engine::CreateWindow(const char* title, int width, int height, const char* glslVersion) {
	unsigned int entity = CreateEntity();
	m_WindowManager.CreateWindowEntity(m_EntityManager, entity, title, width, height, glslVersion);
	
}

