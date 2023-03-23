#include "SoftBody.h"

#include <physics/interfaces/SphereShape.h>

namespace physics
{
	SoftBody::SoftBody(const SoftBodyDesc& desc)
	{
	}

	SoftBody::~SoftBody()
	{
	}

	SoftBody* SoftBody::Cast(iCollisionBody* body)
	{
		return dynamic_cast<SoftBody*>(body);
	}

	unsigned int SoftBody::GetNumNodes()
	{
		return 0;
	}

	void SoftBody::GetNodePosition(unsigned int nodeIndex, glm::vec3& nodePosition)
	{
		nodePosition = glm::vec3(0.f);
	}

}
