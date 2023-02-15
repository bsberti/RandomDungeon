#pragma once

#include <ecs/Component.h>

class AnimationSequence;

struct AnimationComponent : public Component
{
	AnimationSequence* animation;
	float currentTime;
};