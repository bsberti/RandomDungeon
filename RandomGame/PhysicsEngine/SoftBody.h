#pragma once

#include "iSoftBody.h"
#include "SoftBodyDesc.h"
#include "RigidBody.h"

namespace physics
{
	class SoftBodyNode;

	class SoftBodySpring
	{
	public:
		SoftBodySpring(SoftBodyNode* nodeA, SoftBodyNode* nodeB)
			: nodeA(nodeA)
			, nodeB(nodeB)
		{ }

		SoftBodyNode* nodeA;
		SoftBodyNode* nodeB;

		float length;
		float restingLength;

	private:
		SoftBodySpring(const SoftBodySpring&) { }
		SoftBodySpring& operator=(const SoftBodySpring&) {
			return *this;
		}
	};

	class SoftBodyNode : public RigidBody
	{
	public:
		SoftBodyNode(const RigidBodyDesc& desc, iShape* shape);

		bool IsNeighbour(SoftBodyNode* n);

		std::vector<SoftBodySpring*> springs;
		std::vector<SoftBodyNode*> neighbours;

		friend class CollisionHandler;
	private:
		SoftBodyNode(const SoftBodyNode&) : RigidBody(RigidBodyDesc(), nullptr) { }
		SoftBodyNode& operator=(const SoftBodyNode&) {
			return *this;
		}
	};

	class SoftBody : public iSoftBody
	{
	public:
		SoftBody(const SoftBodyDesc& desc);
		virtual ~SoftBody();

		static SoftBody* Cast(iCollisionBody* body);

		virtual unsigned int GetNumNodes();
		virtual void GetNodePosition(unsigned int nodeIndex, glm::vec3& nodePosition);

		void SetGravityAcceleration(const glm::vec3& gravity);
		void UpdateAcceleration();

		void KillForces();

		void ApplyDamping(float damping);

		void VerletStep1(float dt);
		void VerletStep2(float dt);
		void VerletStep3(float dt);

		void UpdateBoundaries();
		const glm::vec3& GetMaxBounds();
		const glm::vec3& GetMinBounds();

		friend class CollisionHandler;
	private:
		void AddSpring(unsigned int idxA, unsigned int idxB);

		float m_SpringConstant;

		glm::vec3 m_MaxBounds;
		glm::vec3 m_MinBounds;

		std::vector<SoftBodyNode*> m_Nodes;
		std::vector<SoftBodySpring*> m_Springs;

		SoftBody(const SoftBody& other) { }
		SoftBody& operator=(const SoftBody& other0) {
			return *this;
		}
	};
}