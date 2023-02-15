#pragma once

#include <vector>

#include <engine/ecs/Component.h>

struct Entity
{
	unsigned int id;
	std::vector<Component*> components;

	void Disable()
	{
		for (int i = 0; i < components.size(); i++)
		{
			components[i]->Disable();
		}
	}
	void Enable()
	{
		for (int i = 0; i < components.size(); i++)
		{
			components[i]->Enable();
		}
	}

	template <class T>
	T* GetComponentByType()
	{
		for (int i = 0; i < components.size(); i++)
		{
			T* component = dynamic_cast<T*>(components[i]);
			if (component != nullptr)
				return component;
		}

		return nullptr;
	}

	template <class T>
	bool HasComponent()
	{
		for (int i = 0; i < components.size(); i++)
		{
			T* component = dynamic_cast<T*>(components[i]);
			if (component != nullptr)
				return true;
		}

		return false;
	}

	template <class T>
	T* AddComponent()
	{
		// TODO: Ensure T is type of Component
		if (HasComponent<T>())
		{
			return;
		}

		T* newComponent = new T();
		newComponent->Enable();
		components.push_back(newComponent);
		return newComponent;
	}

	template <class T>
	T* RemoveComponent()
	{
		for (std::vector<Component*>::iterator it = components.begin();
			it != components.end(); it++)
		{
			T* component = dynamic_cast<T*>(*it);
			if (component != nullptr)
			{
				newComponent->Disable();
				components.erase(it);
				return *it;
			}
		}

		return nullptr;
	}
};
