#pragma once

#include <engine/ecs/System.h>

class GraphicsSystem : public System {
	
	GraphicsSystem() {}

	virtual ~GraphicsSystem() {}

	virtual void Process(const std::vector<Entity*>& entities, float dt) override;

};

