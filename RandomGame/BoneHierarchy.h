#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

struct BoneNode
{
	void AddChild(BoneNode* child)
	{
		child->parent = this;
		children.push_back(child);
	}

	int boneId;
	std::string name;
	BoneNode* parent;
	glm::mat4 transformation;
	glm::mat4 finalTransformation;
	std::vector<BoneNode*> children;
};

typedef BoneNode AnimNode;


struct BoneHierarchy
{
	BoneNode* root;
	glm::mat4 globalInverseTransform;
};