#include "SoftBody.h"

#include "SphereShape.h"

namespace physics
{
	SoftBodyNode::SoftBodyNode(const RigidBodyDesc& desc, iShape* shape)
		: RigidBody(desc, shape)
	{ }

	bool SoftBodyNode::IsNeighbour(SoftBodyNode* n)
	{
		for (std::vector<SoftBodyNode*>::iterator it = neighbours.begin();
			it != neighbours.end(); ++it)
		{
			if (*it == n)
			{
				return true;
			}
		}

		return false;
	}

	SoftBody::SoftBody(const SoftBodyDesc& desc)
	{
		RigidBodyDesc rigidBodyDesc;
		rigidBodyDesc.mass = 0.4f;
		rigidBodyDesc.linearDamping = 0.3f;
		SphereShape* sphereShape = nullptr;
		sphereShape = new SphereShape(0.1f);

		m_SpringConstant = 15.f;

		m_Nodes.resize(desc.NodePositions.size());
		unsigned int numNodes = desc.NodePositions.size();

		for (unsigned int idxNode = 0; idxNode < numNodes; idxNode++)
		{
			rigidBodyDesc.position = desc.NodePositions[idxNode];
			rigidBodyDesc.isStatic = idxNode == 1 || idxNode == 11;
			m_Nodes[idxNode] = new SoftBodyNode(rigidBodyDesc, sphereShape);
		}

		unsigned int numTriangles = desc.TriangulatedIndices.size();
		for (unsigned int idxTriangle = 0; idxTriangle < numTriangles; idxTriangle += 3)
		{
			unsigned int idxA = desc.TriangulatedIndices[idxTriangle];
			unsigned int idxB = desc.TriangulatedIndices[idxTriangle + 1];
			unsigned int idxC = desc.TriangulatedIndices[idxTriangle + 2];

			AddSpring(idxA, idxB);
			AddSpring(idxA, idxC);
			AddSpring(idxB, idxC);
		}
	}

	SoftBody::~SoftBody()
	{
		for (unsigned int idx = 0; idx < m_Nodes.size(); idx++)
		{
			delete m_Nodes[idx];
		}
		for (unsigned int idx = 0; idx < m_Springs.size(); idx++)
		{
			delete m_Springs[idx];
		}
	}

	void SoftBody::AddSpring(unsigned int idxA, unsigned int idxB)
	{
		SoftBodyNode* nodeA = m_Nodes[idxA];
		SoftBodyNode* nodeB = m_Nodes[idxB];
		SoftBodySpring* spring = nullptr;

		if (std::find(nodeA->neighbours.begin(), nodeA->neighbours.end(), nodeB) == nodeA->neighbours.end())
		{
			spring = new SoftBodySpring(nodeA, nodeB);

			nodeA->springs.push_back(spring);
			nodeB->springs.push_back(spring);

			nodeA->neighbours.push_back(nodeB);
			nodeB->neighbours.push_back(nodeA);
		}

		if (std::find(nodeB->neighbours.begin(), nodeB->neighbours.end(), nodeA) == nodeB->neighbours.end())
		{
			spring = new SoftBodySpring(nodeA, nodeB);

			nodeA->springs.push_back(spring);
			nodeB->springs.push_back(spring);

			nodeA->neighbours.push_back(nodeB);
			nodeB->neighbours.push_back(nodeA);
		}

		if (spring)
		{
			m_Springs.push_back(spring);

			Vector3 nodeAPos;
			Vector3 nodeBPos;

			nodeA->GetPosition(nodeAPos);
			nodeB->GetPosition(nodeBPos);

			spring->restingLength = glm::length(nodeAPos.GetGLM() - nodeBPos.GetGLM());
		}
	}

	SoftBody* SoftBody::Cast(iCollisionBody* body)
	{
		return dynamic_cast<SoftBody*>(body);
	}

	unsigned int SoftBody::GetNumNodes()
	{
		return m_Nodes.size();
	}

	void SoftBody::GetNodePosition(unsigned int nodeIndex, glm::vec3& nodePosition)
	{
		Vector3 pos;
		m_Nodes[nodeIndex]->GetPosition(pos);
		nodePosition = pos.GetGLM();
	}

	void SoftBody::SetGravityAcceleration(const glm::vec3& gravity)
	{
		for (unsigned int i = 0; i < m_Nodes.size(); i++)
		{
			m_Nodes[i]->SetGravityAcceleration(gravity);
		}
	}

	void SoftBody::UpdateAcceleration()
	{
		for (int i = 0; i < m_Springs.size(); i++)
		{
			Vector3 posA;
			Vector3 posB;

			SoftBodySpring* spring = m_Springs[i];

			spring->nodeA->GetPosition(posA);
			spring->nodeB->GetPosition(posB);

			glm::vec3 ab = posA.GetGLM() - posB.GetGLM();

			spring->length = glm::length(ab);

			float delta = spring->length - spring->restingLength;

			float force = -m_SpringConstant * delta / spring->restingLength;

			spring->nodeA->ApplyForce(force * ab);
			spring->nodeB->ApplyForce(-force * ab);
		}

		for (int i = 0; i < m_Nodes.size(); i++)
		{
			m_Nodes[i]->UpdateAcceleration();
		}
	}

	void SoftBody::KillForces()
	{
		for (unsigned int i = 0; i < m_Nodes.size(); i++)
		{
			m_Nodes[i]->KillForces();
		}
	}

	void SoftBody::ApplyDamping(float damping)
	{
		for (unsigned int i = 0; i < m_Nodes.size(); i++)
		{
			m_Nodes[i]->ApplyDamping(damping);
		}
	}

	void SoftBody::VerletStep1(float dt)
	{
		for (unsigned int i = 0; i < m_Nodes.size(); i++)
		{
			m_Nodes[i]->VerletStep1(dt);
		}
	}

	void SoftBody::VerletStep2(float dt)
	{
		for (unsigned int i = 0; i < m_Nodes.size(); i++)
		{
			m_Nodes[i]->VerletStep2(dt);
		}
	}

	void SoftBody::VerletStep3(float dt)
	{
		for (unsigned int i = 0; i < m_Nodes.size(); i++)
		{
			m_Nodes[i]->VerletStep3(dt);
		}
	}

	void SoftBody::UpdateBoundaries()
	{
		m_MaxBounds = glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		m_MinBounds = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);

		for (int i = 0; i < m_Nodes.size(); i++)
		{
			SoftBodyNode* node = m_Nodes[i];

			Vector3 pos;
			node->GetPosition(pos);

			float radius = SphereShape::Cast(node->GetShape())->GetRadius();

			m_MaxBounds = glm::max(glm::vec3(pos.x + radius, pos.y + radius, pos.z + radius), m_MaxBounds);
			m_MaxBounds = glm::min(glm::vec3(pos.x - radius, pos.y - radius, pos.z - radius), m_MinBounds);
		}
	}

	const glm::vec3& SoftBody::GetMaxBounds()
	{
		return m_MaxBounds;
	}

	const glm::vec3& SoftBody::GetMinBounds()
	{
		return m_MinBounds;
	}
}
