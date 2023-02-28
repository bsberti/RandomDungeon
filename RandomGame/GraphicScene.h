#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <math.h>
#include "cMeshObject.h"
#include "cVAOManager/sModelDrawInfo.h"
#include "globalOpenGL.h"
#include "globalThings.h"
#include "cShaderManager.h"
#include "cVAOManager/cVAOManager.h"
#include "cLightHelper.h"
#include "cVAOManager/c3DModelFileLoader.h"
#include "cBasicTextureManager/cBasicTextureManager.h"
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 

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

	void Shutdown();

	sModelDrawInfo returnDrawInformation(std::string objectName);
	GLuint returnShaderID(std::string shaderName);

	void CreateGameObjectByType(const std::string& type, glm::vec3 position, sModelDrawInfo& drawInfo);

	cMeshObject* GetObjectByName(std::string name, bool bSearchChildren);

	cMeshObject selectedObject;
	std::vector<cMeshObject*> vec_torchFlames;
	std::vector<cMeshObject*> vec_pMeshObjects;
	std::map<std::string, cMeshObject*>* map_beholds;

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

	cMeshObject* pSkyBox;
private:
	
};