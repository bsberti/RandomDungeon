#pragma once

#include "BodyType.h"

namespace physics
{
	class iCollisionBody
	{
	public:
		virtual ~iCollisionBody() {}

		BodyType GetBodyType() const {
			return m_BodyType;
		}

	protected:
		iCollisionBody(BodyType type)
			: m_BodyType(type)
		{ }

	private:
		BodyType m_BodyType;
		iCollisionBody(const iCollisionBody&) {}
		iCollisionBody& operator=(const iCollisionBody&) {
			return *this;
		}
	};
}
