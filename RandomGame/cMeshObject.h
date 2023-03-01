#ifndef _cMeshObject_HG_
#define _cMeshObject_HG_

#include <string>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <iostream>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/easing.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>

struct PositionKeyFrame
{
	glm::vec3 value;
	float time;
};

struct ScaleKeyFrame
{
	glm::vec3 value;
	float time;
};

struct RotationKeyFrame
{
	glm::vec3 value;
	float time;
	bool useSlerp;
};

struct ColorKeyFrame
{
	glm::vec3 value;
	float time;
};

class cMeshObject {
public:
	cMeshObject();
	std::string meshName;

	// Human friendly name for this particular instance of the mesh
	std::string friendlyName;

	glm::vec3 position;     // 0,0,0 (origin)
	// Euler angles
	glm::vec3 rotation;     // 0,0,0 ration around each Euler axis

	glm::quat qRotation;	// This ISN'T in Euler angles
	// This OVERWRITES the current angle
	void setRotationFromEuler(glm::vec3 newEulerAngleXYZ) {
		this->qRotation = glm::quat(newEulerAngleXYZ);
	}

	// This one updates ("adds") to the current angle
	void adjustRoationAngleFromEuler(glm::vec3 EulerAngleXYZ_Adjust) {
		// To combine quaternion values, you multiply them together
		// Make a quaternion that represents that CHANGE in angle
		glm::quat qChange = glm::quat(EulerAngleXYZ_Adjust);
		// Multiply them together to get the change
		this->qRotation *= qChange;
	}

	float scale;
	glm::vec3 scaleXYZ;
	void SetUniformScale(float newScale) {
		this->scaleXYZ = glm::vec3(newScale, newScale, newScale);
	}

	void reduceFromScale(float scalar) {
		this->scaleXYZ.x *= scalar;
		this->scaleXYZ.y *= scalar;
		this->scaleXYZ.z *= scalar;
	}

	bool isWireframe;

	// This is the "diffuse" colour
	glm::vec4 RGBA_colour;		// RGA & Alpha, 0,0,0,1 (black, with transparency of 1.0)

	// When true, it will overwrite the vertex colours
	bool bUse_RGBA_colour;

	glm::vec4 specular_colour_and_power;
	// RGB is the specular highlight colour
	// w is the power

	bool bDoNotLight;
	bool bIsVisible;

	// Physics Collision properties
	unsigned int numberOfTriangles;
	std::vector<glm::vec3> meshTriangles;
	std::vector<glm::vec3> meshVertices;
	std::vector<glm::vec3> trianglesCenter;

	// Music properties
	bool soundPlayed;

	std::string textures[8];
	float textureRatios[8];

	// Behold properties
	bool dead;
	unsigned int currentI;
	unsigned int currentJ;
	unsigned int moving;
	unsigned int rotating;

	// Animation properties
	std::vector<PositionKeyFrame> PositionKeyFrames;
	std::vector<ScaleKeyFrame> ScaleKeyFrames;
	std::vector<RotationKeyFrame> RotationKeyFrames;
	std::vector<ColorKeyFrame> ColorKeyFrames;
	float CurrentTime;
	// TO-DO CurrentTime set
	float Speed;
	bool IsLooping;
	bool IsPlaying;

	glm::vec3 GetAnimationPosition(float time, int type);
	glm::vec3 GetAnimationScale(float time, int type);
	glm::vec3 GetAnimationRotation(float time, int type);

	int FindPositionKeyFrameIndex(float time);
	int FindScaleKeyFrameIndex(float time);
	int FindRotationKeyFrameIndex(float time);

	void UpdateAnimation(float deltaTime);

	// Child meshes - move with the parent mesh
	std::vector< cMeshObject* > vecChildMeshes;

	cMeshObject* findObjectByFriendlyName(std::string name, bool bSearchChildren = true);

	unsigned int getID(void) { return this->m_myID; }

	unsigned int m_myID;
	static unsigned int nextID;
	static const unsigned int STARTING_ID = 1;
};

#endif // _cMeshObject_HG_
