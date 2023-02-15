#pragma once

#include <engine/ecs/Component.h>

class AnimationSequence;

struct AnimationComponent : public Component
{
	AnimationSequence* animation;
	float currentTime;
};