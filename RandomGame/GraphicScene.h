#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include <sstream>

#include "cMeshObject.h"
#include "cVAOManager/sModelDrawInfo.h"
#include "cShaderManager.h"
#include "cVAOManager/cVAOManager.h"
#include "cLightHelper.h"
#include "cVAOManager/c3DModelFileLoader.h"
#include "cBasicTextureManager/cBasicTextureManager.h"
#include "cFBO.h"
#include "CharacterAnimator.h"

/* Link with Win32 shared freeglut lib */
#           if FREEGLUT_LIB_PRAGMAS
#               ifdef NDEBUG
#                   pragma comment (lib, "freeglut.lib")
#               else
#                   pragma comment (lib, "freeglutd.lib")
#               endif
#           endif

class cCharacter;

class GraphicScene {
public:
	friend class cRandomUI;
	GraphicScene();
	~GraphicScene();

	void Initialize();

	void PositioningMe(cMeshObject* pMeWhoIsAsking);
	void LoadTextures();
	int PrepareScene();
	void DrawScene(GLFWwindow* window, glm::vec3 g_cameraEye, glm::vec3 g_cameraTarget);
	void DrawMapView(GLFWwindow* window, glm::vec3 g_cameraEye, glm::vec3 g_cameraTarget);
	void CalculateSceneExtension(glm::vec3 g_cameraEye, glm::vec3 g_cameraTarget);
	void Shutdown();

	void cleanMazeView();

	bool SATIntersectionTest(cMeshObject* mesh, glm::mat4 frustum);
	void ExtractFrustumPlanes(const glm::mat4& frustumMatrix,
		std::vector<glm::vec4>& frustumPlanes);
	void ProjectVerticesOntoAxis(const std::vector<glm::vec3>& vertices,
		const glm::vec3& axis, float& min, float& max);
	glm::vec4 GetRow(const glm::mat4& matrix, int row);

	sModelDrawInfo returnDrawInformation(std::string objectName);
	GLuint returnShaderID(std::string shaderName);

	cMeshObject* CreateGameObjectByType(const std::string& type, glm::vec3 position, sModelDrawInfo& drawInfo);
	cCharacter* CreateAnimatedCharacter(const char* filename,
		const std::vector<std::string>& animations, sModelDrawInfo drawInfo);
	cMeshObject* GetObjectByName(std::string name, bool bSearchChildren);
	cMeshObject* GetObjectByGridPosition(int i, int j);

	cMeshObject selectedObject;
	std::vector<cMeshObject*> vec_torchFlames;
	std::vector<cMeshObject*> vec_pMeshObjects;
	std::vector<cMeshObject*> vec_pMeshFullMaze;
	std::vector<cMeshObject*> vec_pMeshCurrentMaze;
	std::vector<physics::iRigidBody*> vec_pWalls;
	int drawFog;

	//std::map<std::string, cMeshObject*>* map_beholds;
	std::vector<cMeshObject*> map_beholds;

	std::vector<glm::vec3> trianglesCenter;

	bool cameraFollowing;
	bool cameraTransitioning;

	GLuint vertex_buffer = 0;
	GLuint shaderID = 0;

	cShaderManager* pTheShaderManager;
	cVAOManager* pVAOManager;
	cBasicTextureManager* g_pTextureManager;

	GLint mvp_location;
	GLint mModel_location;
	GLint mView_location;
	GLint mProjection_location;
	GLint mModelInverseTransform_location;

	glm::mat4x4 matProjection;
	glm::mat4x4 matView;
	glm::mat4 frustumMatrix;
	glm::vec3 minExt;
	glm::vec3 maxExt;
	float ratio;
	int width, height;
	int mapWidth, mapHeigth;

	cMeshObject* pSkyBox;

	// Frame Buffer Object
	// (This is global so the windows resize callback can "see" it)
	cFBO* g_pFBO_01 = NULL;

	std::string mAnimationName;


	// Animation Stuffs
	std::vector<Model*> gModelVec;

	bool loggedIn;

	CharacterAnimator* animator = nullptr;
private:
	float deltaTime, lastFrame;
	
};