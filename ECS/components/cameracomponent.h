#pragma once

#include <engine/ecs/component.h>
#include <graphics/camera/camera.h>

class CameraComponent : public Component
{
public:
	Camera* m_Camera;
};