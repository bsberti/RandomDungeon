#include "cMeshObject.h"

unsigned int cMeshObject::nextID = cMeshObject::STARTING_ID;

cMeshObject::cMeshObject() {
	this->position = glm::vec3(0.0f);

	//this->rotation = glm::vec3(0.0f);		// Euler angles
	// Set rotation to 0 degrees around each axis
	this->qRotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

	// RGB and "alpha" (A) 0.0f = transparent
	this->SetUniformScale(1.0f);
	this->isWireframe = false;

	// This will mainly be used for non-lit debug objects, eventually
	this->RGBA_colour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->bUse_RGBA_colour = false;

	// Make this white unless it's plastic or metal
	// Specualar (w) of 1 is "not" shiny
	this->specular_colour_and_power = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->bDoNotLight = false;
	this->bIsVisible = true;

	this->textureRatios[0] = 0.0f;
	this->textureRatios[1] = 0.0f;
	this->textureRatios[2] = 0.0f;
	this->textureRatios[3] = 0.0f;
	this->textureRatios[4] = 0.0f;
	this->textureRatios[5] = 0.0f;
	this->textureRatios[6] = 0.0f;
	this->textureRatios[7] = 0.0f;

	this->m_myID = cMeshObject::nextID;
	cMeshObject::nextID += 1;

	this->currentI = 0;
	this->currentJ = 0;
	this->moving = 0;
	this->dead = false;
}

int cMeshObject::FindPositionKeyFrameIndex(float time)
{
	for (int i = 0; i < PositionKeyFrames.size() - 1; i++)
	{
		if (PositionKeyFrames[i].time < time)
			return i;
	}

	// [1, 2, 3, 4, 5] 5 - 2, 3;
	return PositionKeyFrames.size() - 1;
}

int cMeshObject::FindScaleKeyFrameIndex(float time)
{
	for (int i = 0; i < ScaleKeyFrames.size(); i++)
	{
		if (ScaleKeyFrames[i].time > time)
			return i - 1;
	}

	// [1, 2, 3, 4, 5] 5 - 2, 3;
	return ScaleKeyFrames.size() - 1;
}

int cMeshObject::FindRotationKeyFrameIndex(float time)
{
	for (int i = 0; i < RotationKeyFrames.size(); i++)
	{
		if (RotationKeyFrames[i].time > time)
			return i - 1;
	}

	return RotationKeyFrames.size() - 1;
}

glm::vec3 cMeshObject::GetAnimationPosition(float time, int type)
{
	// Assert animation.PositionKeyFrames.size() > 0

	//if (PositionKeyFrames.size() == time)
	//	return glm::vec3(0.f);

	if (PositionKeyFrames.size() == 1)
		return PositionKeyFrames[0].value;

	int positionKeyFrameIndex = FindPositionKeyFrameIndex(time);

	if (PositionKeyFrames.size() - 1 == positionKeyFrameIndex)
		return PositionKeyFrames[positionKeyFrameIndex].value;

	int nextPositionKeyFrameIndex = positionKeyFrameIndex + 1;
	PositionKeyFrame positionKeyFrame = PositionKeyFrames[positionKeyFrameIndex];
	PositionKeyFrame nextPositionKeyFrame = PositionKeyFrames[nextPositionKeyFrameIndex];
	float difference = nextPositionKeyFrame.time - positionKeyFrame.time;
	float ratio = (time - positionKeyFrame.time) / difference;

	switch (type)
	{
	case 1:
		ratio = glm::sineEaseIn(ratio);
		bUse_RGBA_colour = true;
		RGBA_colour = glm::vec4(1.f, 0.f, 0.f, 1.f); //RED
		break;

	case 2:
		ratio = glm::sineEaseOut(ratio);
		bUse_RGBA_colour = true;
		RGBA_colour = glm::vec4(0.f, 0.f, 1.f, 1.f); //BLUE
		break;

	case 3:
		ratio = glm::sineEaseInOut(ratio);
		bUse_RGBA_colour = true;
		RGBA_colour = glm::vec4(0.f, 1.f, 0.f, 1.f); //GREEN
		break;

	case 0:
		bUse_RGBA_colour = false;
	default:
		break;
	}

	return glm::mix(positionKeyFrame.value, nextPositionKeyFrame.value, ratio);
}

glm::vec3 cMeshObject::GetAnimationScale(float time, int type)
{
	// Assert animation.ScaleKeyFrames.size() > 0

	if (ScaleKeyFrames.size() == 1)
		return ScaleKeyFrames[0].value;

	int scaleKeyFrameIndex = FindScaleKeyFrameIndex(time);

	if (ScaleKeyFrames.size() - 1 == scaleKeyFrameIndex)
		return ScaleKeyFrames[scaleKeyFrameIndex].value;

	int nextScaleKeyFrameIndex = scaleKeyFrameIndex + 1;
	ScaleKeyFrame scaleKeyFrame = ScaleKeyFrames[scaleKeyFrameIndex];
	ScaleKeyFrame nextScaleKeyFrame = ScaleKeyFrames[nextScaleKeyFrameIndex];
	float difference = nextScaleKeyFrame.time - scaleKeyFrame.time;
	float ratio = (time - scaleKeyFrame.time) / difference;

	switch (type)
	{
	case 1:
		ratio = glm::sineEaseIn(ratio);
		bUse_RGBA_colour = true;
		RGBA_colour = glm::vec4(1.f, 0.f, 0.f, 1.f); //RED
		break;

	case 2:
		ratio = glm::sineEaseOut(ratio);
		bUse_RGBA_colour = true;
		RGBA_colour = glm::vec4(0.f, 0.f, 1.f, 1.f); //BLUE
		break;

	case 3:
		ratio = glm::sineEaseInOut(ratio);
		bUse_RGBA_colour = true;
		RGBA_colour = glm::vec4(0.f, 1.f, 0.f, 1.f); //GREEN
		break;

	case 0:
		bUse_RGBA_colour = false;
	default:
		break;
	}

	glm::vec3 result = glm::mix(scaleKeyFrame.value, nextScaleKeyFrame.value, ratio);

	return result;
}

glm::quat cMeshObject::GetAnimationRotation(float time, int type)
{
	if (RotationKeyFrames.size() == 1)
		return RotationKeyFrames[0].value;

	int rotationKeyFrameIndex = FindRotationKeyFrameIndex(time);

	if (RotationKeyFrames.size() - 1 == rotationKeyFrameIndex)
		return RotationKeyFrames[rotationKeyFrameIndex].value;

	int nextRotationKeyFrameIndex = rotationKeyFrameIndex + 1;
	RotationKeyFrame rotationKeyFrame = RotationKeyFrames[rotationKeyFrameIndex];
	RotationKeyFrame nextRotationKeyFrame = RotationKeyFrames[nextRotationKeyFrameIndex];
	float difference = nextRotationKeyFrame.time - rotationKeyFrame.time;
	float ratio = (time - rotationKeyFrame.time) / difference;

	switch (type)
	{
	case 1:
		ratio = glm::sineEaseIn(ratio);
		bUse_RGBA_colour = true;
		RGBA_colour = glm::vec4(1.f, 0.f, 0.f, 1.f); //RED
		break;

	case 2:
		ratio = glm::sineEaseOut(ratio);
		bUse_RGBA_colour = true;
		RGBA_colour = glm::vec4(0.f, 0.f, 1.f, 1.f); //BLUE
		break;

	case 3:
		ratio = glm::sineEaseInOut(ratio);
		bUse_RGBA_colour = true;
		RGBA_colour = glm::vec4(0.f, 1.f, 0.f, 1.f); //GREEN
		break;

	case 0:
		bUse_RGBA_colour = false;
	default:
		break;
	}

	glm::quat result;

	//if (rotationKeyFrame.useSlerp) {
	//	//result = glm::slerp(rotationKeyFrame.value, nextRotationKeyFrame.value, ratio);
	//}
	//else {
	result = glm::mix(rotationKeyFrame.value, nextRotationKeyFrame.value, ratio);
	//}

	return result;
}

void cMeshObject::UpdateAnimation(float deltaTime)
{
	CurrentTime += deltaTime * Speed;

	if (IsLooping)
	{
		if (CurrentTime < 0 && Speed < 0)
		{
			CurrentTime = 1.f;
		}
		else if (CurrentTime > 1 && Speed > 0)
		{
			CurrentTime = 0;
		}
	}
	else
	{
		// We may check the playback direction, depending on the behaviour
		// - If the animation is < 0 and playback is forward what should we do?
		// - If the animation is > 1 and playback is reversed what should we do?
		// - How would that case happen?
		// - Is the CurrentTime able to be set by the user? It should be clamped when set

		if (CurrentTime < 0)
		{
			CurrentTime = 0;
			IsPlaying = false;
		}
		else if (CurrentTime > 1)
		{
			CurrentTime = 1;

			if (rotating == 1 && moving == 1) {
				CurrentTime = 0;
				moving = 2;
				rotating = 0;
			}
			else if (rotating == 0 && moving == 2) {
				moving = 0;
			}

			IsPlaying = true;
		}
	}
}

cMeshObject* cMeshObject::findObjectByFriendlyName(std::string nameToFind, bool bSearchChildren /*=true*/) {
	for (std::vector< cMeshObject* >::iterator itCurrentMesh = this->vecChildMeshes.begin();
		itCurrentMesh != this->vecChildMeshes.end();
		itCurrentMesh++)
	{
		cMeshObject* pCurrentMesh = *itCurrentMesh;

		if (pCurrentMesh->friendlyName == nameToFind) {
			return pCurrentMesh;
		}

		// Search children too? 
		cMeshObject* pChildMesh = pCurrentMesh->findObjectByFriendlyName(nameToFind, bSearchChildren);

		if (pChildMesh) { /* NULL = 0 = false) */
			return pChildMesh;
		}
	}

	return NULL;
}