#pragma once

#include "iCollisionBody.h"

#include <glm/vec3.hpp>

namespace physics
{
	class iSoftBody : public iCollisionBody
	{
	public:
		virtual ~iSoftBody() {}

		virtual unsigned int GetNumNodes() = 0;
		virtual void GetNodePosition(unsigned int nodeIndex, glm::vec3& nodePosition) = 0;

	protected:
		iSoftBody() : iCollisionBody(BodyType::SoftBody) { }

	private:
		iSoftBody(const iSoftBody&) : iCollisionBody(BodyType::SoftBody) { }
		iSoftBody& operator=(const iSoftBody&) {
			return *this;
		}
	};
}