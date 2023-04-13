#include "AnimationManager.h"

#include <glm/gtx/easing.hpp>
#include <iostream>

#include <glm/gtx/matrix_decompose.hpp>

AnimationManager::AnimationManager() {

}

AnimationManager::~AnimationManager() {

}

void AnimationManager::Update(const std::vector<cMeshObject*>& gameObjects, float dt) {
	for (int i = 0; i < gameObjects.size(); i++)
	{
		cMeshObject* go = gameObjects[i];

		if (!go->Enabled)
			continue;

		if (go->Animation.IsCharacterAnimation)
		{
			Animation& animation = go->Animation;
			std::map<std::string, CharacterAnimationData>::iterator itFind = m_CharacterAnimations.find(go->Animation.AnimationType);
			CharacterAnimationData& animationData = itFind->second;

			if (itFind != m_CharacterAnimations.end())
			{
				if (animation.IsPlaying && animation.Speed != 0.0f)
				{
					animation.AnimationTime += dt * animation.Speed * animationData.TicksPerSecond;

					double clipDuration = animationData.Duration;

					if (animation.AnimationTime > clipDuration)
					{
						if (animation.IsLooping)
						{
							if (animation.Speed > 0)
							{
								animation.AnimationTime = 0.0f;
							}
							else
							{
								animation.AnimationTime = clipDuration;
							}
						}
						else
						{
							animation.AnimationTime = clipDuration;
							animation.IsPlaying = false;
						}

					}
					else if (animation.AnimationTime < 0.f)
					{
						if (animation.IsLooping)
						{
							if (animation.Speed < 0)
							{
								animation.AnimationTime = animationData.Duration;
							}
							else
							{
								animation.AnimationTime = 0.f;
							}
						}
						else
						{
							animation.AnimationTime = 0.f;
							animation.IsPlaying = false;
						}
					}

					m_GlobalInverseTransform = animationData.BoneHierarchy->globalInverseTransform;

					int keyFrameTime = (int)((animation.AnimationTime / clipDuration) * animationData.Duration);


					animation.AnimationTime = 0;
					glm::mat4 identity(1.f);

					//printf("--------------------\n");
					//printf("Time: %.4f %d/%d\n", animation.AnimationTime, keyFrameTime, (int)animationData.Duration);
					UpdateBoneHierarchy(animationData.BoneHierarchy->root, animationData, identity, animation.AnimationTime);

					//Model* model = GDP_GetModel(go->Renderer.MeshId);

					SetGameObjectBoneModelMatrices(go, animationData.BoneHierarchy->root, animationData);
				}

			}
		}
		else if (go->IsBonedObject)
		{
			// for each bone, there is an animation channel that controls it's transformation

		}
		else if (go->Animation.AnimationType.length() != 0)
		{
			Animation& animation = go->Animation;
			std::map<std::string, AnimationData>::iterator itFind = m_Animations.find(go->Animation.AnimationType);
			const AnimationData& animationData = itFind->second;

			if (itFind != m_Animations.end())
			{
				if (animation.IsPlaying && animation.Speed != 0.0f)
				{
					animation.AnimationTime += dt * animation.Speed;
					if (animation.AnimationTime > animationData.Duration)
					{
						if (animation.IsLooping)
						{
							if (animation.Speed > 0)
							{
								animation.AnimationTime = 0.0f;
							}
							else
							{
								animation.AnimationTime = animationData.Duration;
							}
						}
						else
						{
							animation.AnimationTime = animationData.Duration;
							animation.IsPlaying = false;
						}

					}
					else if (animation.AnimationTime < 0.f)
					{
						if (animation.IsLooping)
						{
							if (animation.Speed < 0)
							{
								animation.AnimationTime = animationData.Duration;
							}
							else
							{
								animation.AnimationTime = 0.f;
							}
						}
						else
						{
							animation.AnimationTime = 0.f;
							animation.IsPlaying = false;
						}
					}
				}

				// Controlling one "node"/"bone".
				go->position = GetAnimationPosition(itFind->second, animation.AnimationTime);
				go->scaleXYZ = GetAnimationScale(itFind->second, animation.AnimationTime);
				go->qRotation = GetAnimationRotation(itFind->second, animation.AnimationTime);
#ifdef PRINT_DEBUG_INFO
				//printf("Position: %.2f, %.2f, %.2f\n", go->Position.x, go->Position.y, go->Position.z);
				//printf("Scale: %.2f, %.2f, %.2f\n", go->Scale.x, go->Scale.y, go->Scale.z);
#endif
			}
		}
	}
}

AnimationData* AnimationManager::FindAnimationData(const std::string& nodeName, const CharacterAnimationData& data)
{
	for (int i = 0; i < data.Channels.size(); i++)
	{
		if (nodeName == data.Channels[i]->Name)
			return data.Channels[i];
	}
	return nullptr;
}

void AnimationManager::SetGameObjectBoneModelMatrices(cMeshObject* gameObject, BoneNode* node, const CharacterAnimationData& data)
{
	gameObject->BoneModelMatrices.resize(data.boneInfoVec.size());
	for (int i = 0; i < data.boneInfoVec.size(); i++)
	{
		gameObject->BoneModelMatrices[i] = data.boneInfoVec[i].finalTransformation;
	}

	//std::map<std::string, int>::const_iterator findIt = data.boneNameToIdMap.find(node->name);
	//if (findIt != data.boneNameToIdMap.end())
	//{
	//	gameObject->BoneModelMatrices[findIt->second] = data.boneInfoVec[findIt->second].finalTransformation;
	//}

	//for (int i = 0; i < node->children.size(); i++)
	//{
	//	SetGameObjectBoneModelMatrices(gameObject, node->children[i], data);
	//}
}

aiNode* GetNodeFromName(aiNode* node, const std::string& name)
{
	if (std::string(node->mName.C_Str()) == name)
		return node;

	for (int i = 0; i < node->mNumChildren; i++)
	{
		return GetNodeFromName(node->mChildren[i], name);
	}

	return nullptr;
}

void AnimationManager::UpdateBoneHierarchy(BoneNode* node, CharacterAnimationData& data, const glm::mat4& parentTransformationMatrix, float keyFrameTime)
{
	if (node == nullptr)
		return;

	std::string nodeName(node->name);
	glm::mat4 transformationMatrix = node->transformation;

	AnimationData* animNode = FindAnimationData(nodeName, data);
	//printf(" %s\n", nodeName.c_str());

	if (animNode != nullptr)
	{
		// Calculate the position of this node.
		glm::vec3 position = GetAnimationPosition(*animNode, keyFrameTime);
		glm::vec3 scale = GetAnimationScale(*animNode, keyFrameTime);
		glm::quat rotation = GetAnimationRotation(*animNode, keyFrameTime);

		// Calculate our transformation matrix
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.f), position);
		glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

		//transformationMatrix = translationMatrix * rotationMatrix * scaleMatrix;

		//printf("  Local Transformation:\n");
		//printf("    Position: (%.4f, %.4f, %.4f)\n", position.x, position.y, position.z);
		//printf("    Scale: (%.4f, %.4f, %.4f)\n", scale.x, scale.y, scale.z);
		//printf("    Rotation: (%.4f, %.4f, %.4f, %.4f)\n", rotation.x, rotation.y, rotation.z, rotation.w);
	}

	glm::mat4 globalTransformation = parentTransformationMatrix * transformationMatrix;

	// Store the transformation matrix in the Animation component info.
	//Check if this is a bone, and get the bone offset
	std::map<std::string, int>::iterator boneIt = data.boneNameToIdMap.find(nodeName);
	if (boneIt != data.boneNameToIdMap.end())
	{
		//printf("%d : %s\n", boneIt->second, nodeName.c_str());
		int boneIdx = boneIt->second;
		BoneInfo& bi = data.boneInfoVec[boneIdx];
		bi.finalTransformation = m_GlobalInverseTransform * globalTransformation * bi.boneOffset; // m_GlobalInverseTransform* globalTransformation* bi.boneOffset;
	}

	glm::vec3 globalScale, globalTranslation, ignore3;
	glm::vec4 ignore4;
	glm::quat globalOrientation;

	bool success = glm::decompose(globalTransformation, globalScale, globalOrientation, globalTranslation, ignore3, ignore4);

	//printf("  Global Transformation:\n");
	//printf("    Position: (%.4f, %.4f, %.4f)\n", globalTranslation.x, globalTranslation.y, globalTranslation.z);
	//printf("    Scale: (%.4f, %.4f, %.4f)\n", globalScale.x, globalScale.y, globalScale.z);
	//printf("    Rotation: (%.4f, %.4f, %.4f, %.4f)\n", globalOrientation.x, globalOrientation.y, globalOrientation.z, globalOrientation.w);
	//printf("\n");

	for (int i = 0; i < node->children.size(); i++)
	{
		UpdateBoneHierarchy(node->children[i], data, globalTransformation, keyFrameTime);
	}
}

bool AnimationManager::LoadAnimation(const std::string& name, AnimationData animation)
{
	std::map<std::string, AnimationData>::iterator itFind = m_Animations.find(name);
	if (itFind != m_Animations.end())
	{
#ifdef PRINT_DEBUG_INFO
		printf("Animation is already added with this name!\n");
#endif
		return false;
	}

	m_Animations.insert(std::pair<std::string, AnimationData>(name, animation));

	return true;
}

bool AnimationManager::LoadCharacterAnimation(const std::string& name, CharacterAnimationData animation)
{
	std::map<std::string, CharacterAnimationData>::iterator itFind = m_CharacterAnimations.find(name);
	if (itFind != m_CharacterAnimations.end())
	{
#ifdef PRINT_DEBUG_INFO
		printf("Animation is already added with this name!\n");
#endif
		return false;
	}

	m_CharacterAnimations.insert(std::pair<std::string, CharacterAnimationData>(name, animation));

	return true;
}

int AnimationManager::FindPositionKeyFrameIndex(const AnimationData& animation, float keyFrameTime)
{
	for (int i = 0; i < animation.PositionKeyFrames.size() - 1; i++)
	{
		if (animation.PositionKeyFrames[i + 1].time > keyFrameTime)
			return i;
	}

	// [1, 2, 3, 4, 5] 5 - 2, 3;
	return 0;
}

int AnimationManager::FindScaleKeyFrameIndex(const AnimationData& animation, float keyFrameTime)
{
	for (int i = 0; i < animation.ScaleKeyFrames.size() - 1; i++)
	{
		if (animation.ScaleKeyFrames[i + 1].time > keyFrameTime)
			return i;
	}

	// [1, 2, 3, 4, 5] 5 - 2, 3;
	return 0;
}

int AnimationManager::FindRotationKeyFrameIndex(const AnimationData& animation, float keyFrameTime)
{
	for (int i = 0; i < animation.RotationKeyFrames.size() - 1; i++)
	{
		if (animation.RotationKeyFrames[i + 1].time > keyFrameTime)
			return i;
	}

	return 0;
}

glm::vec3 AnimationManager::GetAnimationPosition(const AnimationData& animation, float keyFrameTime)
{
	// Assert animation.PositionKeyFrames.size() > 0

	if (animation.PositionKeyFrames.size() == 1)
		return animation.PositionKeyFrames[0].value;

	int positionKeyFrameIndex = FindPositionKeyFrameIndex(animation, keyFrameTime);
	int nextPositionKeyFrameIndex = positionKeyFrameIndex + 1;
	PositionKeyFrame positionKeyFrame = animation.PositionKeyFrames[positionKeyFrameIndex];
	PositionKeyFrame nextPositionKeyFrame = animation.PositionKeyFrames[nextPositionKeyFrameIndex];
	float difference = nextPositionKeyFrame.time - positionKeyFrame.time;
	float ratio = (keyFrameTime - positionKeyFrame.time) / difference;

	if (ratio < 0.0f) ratio = 0.0f;
	if (ratio > 1.0f) ratio = 1.0f;

	//glm::vec3 result = glm::mix(positionKeyFrame.value, nextPositionKeyFrame.value, ratio);
	glm::vec3 result = (nextPositionKeyFrame.value - positionKeyFrame.value) * ratio + positionKeyFrame.value;

#ifdef PRINT_DEBUG_INFO
	printf("[%.2f : %.2f%%] (%.2f, %.2f) -> (%.2f, %.2f) = (%.2f, %.2f)\n", time, ratio,
		positionKeyFrame.value.x, positionKeyFrame.value.y,
		nextPositionKeyFrame.value.x, nextPositionKeyFrame.value.y,
		result.x, result.y);
#endif

	return result;
}

glm::vec3 AnimationManager::GetAnimationScale(const AnimationData& animation, float keyFrameTime)
{
	// Assert animation.ScaleKeyFrames.size() > 0

	if (animation.ScaleKeyFrames.size() == 1)
		return animation.ScaleKeyFrames[0].value;

	int scaleKeyFrameIndex = FindScaleKeyFrameIndex(animation, keyFrameTime);
	int nextScaleKeyFrameIndex = scaleKeyFrameIndex + 1;
	ScaleKeyFrame scaleKeyFrame = animation.ScaleKeyFrames[scaleKeyFrameIndex];
	ScaleKeyFrame nextScaleKeyFrame = animation.ScaleKeyFrames[nextScaleKeyFrameIndex];
	float difference = nextScaleKeyFrame.time - scaleKeyFrame.time;
	float ratio = (keyFrameTime - scaleKeyFrame.time) / difference;

	if (ratio < 0.0f) ratio = 0.0f;
	if (ratio > 1.0f) ratio = 1.0f;

	//glm::vec3 result = glm::mix(scaleKeyFrame.value, nextScaleKeyFrame.value, ratio);
	glm::vec3 result = (nextScaleKeyFrame.value - scaleKeyFrame.value) * ratio + scaleKeyFrame.value;

#ifdef PRINT_DEBUG_INFO
	//printf("[%.2f : %.2f%%] (%.2f, %.2f) -> (%.2f, %.2f) = (%.2f, %.2f)\n", time, ratio,
		//scaleKeyFrame.value.x, scaleKeyFrame.value.y,
		//nextScaleKeyFrame.value.x, nextScaleKeyFrame.value.y,
		//result.x, result.y);
#endif

	return result;
}

glm::quat AnimationManager::GetAnimationRotation(const AnimationData& animation, float keyFrameTime)
{
	if (animation.RotationKeyFrames.size() == 1)
		return animation.RotationKeyFrames[0].value;

	int rotationKeyFrameIndex = FindRotationKeyFrameIndex(animation, keyFrameTime);
	int nextRotationKeyFrameIndex = rotationKeyFrameIndex + 1;
	RotationKeyFrame rotationKeyFrame = animation.RotationKeyFrames[rotationKeyFrameIndex];
	RotationKeyFrame nextRotationKeyFrame = animation.RotationKeyFrames[nextRotationKeyFrameIndex];
	float difference = nextRotationKeyFrame.time - rotationKeyFrame.time;
	float ratio = (keyFrameTime - rotationKeyFrame.time) / difference;

	if (ratio < 0.0f) ratio = 0.0f;
	if (ratio > 1.0f) ratio = 1.0f;
		
	glm::quat result;
	result = glm::slerp(rotationKeyFrame.value, nextRotationKeyFrame.value, ratio);
	result = glm::normalize(result);

#ifdef PRINT_DEBUG_INFO
	//printf("[%.2f : %.2f%%] (%.2f, %.2f) -> (%.2f, %.2f) = (%.2f, %.2f)\n", time, ratio,
		//scaleKeyFrame.value.x, scaleKeyFrame.value.y,
		//nextScaleKeyFrame.value.x, nextScaleKeyFrame.value.y,
		//result.x, result.y);
#endif

	return result;
}
