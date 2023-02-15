#pragma once

class Component
{
public:
	virtual ~Component() {}

	virtual void Disable() {}
	virtual void Enable() {}

protected:
	Component() {}
};