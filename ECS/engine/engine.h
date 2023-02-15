#pragma once

#include <vector>

#include <animation/animationmanager.h>

#include <audio/audiomanager.h>

#include <engine/ecs/System.h>
#include <engine/entity/entitymanager.h>
#include <engine/InputManager.h>

#include <graphics/camera/cameramanager.h>
#include <graphics/material/materialmanager.h>
#include <graphics/mesh/meshmanager.h>
#include <graphics/rendersystem.h>
#include <graphics/shader/shadermanager.h>
#include <graphics/texture/texturemanager.h>

#include <world/worldmanager.h>


class Engine
{
public:
	Engine();
	~Engine();

	void Initialize();
	void Destroy();

	void Update(float dt);
	void Render();

	unsigned int CreateEntity();

	template <class T>
	T* AddComponent(unsigned int entityId)
	{
		return m_EntityManager.AddComponent<T>(entityId);
	}

	//template <class T>
	//T* AddComponentSlow(unsigned int entityId)
	//{
	//	// Add this to a job queue
	//	return m_EntityManager.AddComponent<T>(entityId);
	//}

	template <class T>
	T* RemoveComponent(unsigned int entityId)
	{
		return m_EntityManager.RemoveComponent<T>(entityId);
	}

	void AddSystem(System* newSystem);
	void RemoveSystem(System* removeSystem);

private:
	AnimationManager m_AnimationManager;
	AudioManager m_AudioManager;
	CameraManager m_CameraManager;
	InputManager m_InputManager;
	MaterialManager m_MaterialManager;
	MeshManager m_MeshManager;
	ShaderManager m_ShaderManager;
	TextureManager m_TextureManager;
	WorldManager m_WorldManager;

	EntityManager m_EntityManager;

	//JobQueue m_JobQueue;
	RenderSystem* m_RenderSystem;

	std::vector<System*> m_Systems;
};