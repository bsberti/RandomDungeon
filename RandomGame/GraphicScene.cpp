#include "GraphicScene.h"
#include <iostream>

GraphicScene::GraphicScene() {
	cameraFollowing = false;
	cameraTransitioning = false;
}

GraphicScene::~GraphicScene() {
}

void GraphicScene::Initialize() {

}

void DrawObject(cMeshObject* pCurrentMeshObject,
    glm::mat4x4 mat_PARENT_Model,               // The "parent's" model matrix
    GLuint shaderID,                            // ID for the current shader
    cBasicTextureManager* pTextureManager,
    cVAOManager* pVAOManager,
    GLint mModel_location,                      // Uniform location of mModel matrix
    GLint mModelInverseTransform_location);      // Uniform location of mView location



// From here: https://stackoverflow.com/questions/5289613/generate-random-float-between-two-floats/5289624
float RandomFloatGraphic(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

// ------------------------ Load Model into VAO ------------------------
bool LoadModelTypesIntoVAO(std::string fileTypesToLoadName,
    cVAOManager* pVAOManager,
    GLuint shaderID)
{
    std::ifstream modelTypeFile(fileTypesToLoadName.c_str());
    if (!modelTypeFile.is_open()) {
        // Can't find that file
        return false;
    }

    // At this point, the file is open and ready for reading
    std::string PLYFileNameToLoad;      // example = "assets/models/MOTO/Blender (load from OBJ export) - only Moto_xyz_n_rgba_uv.ply";
    std::string friendlyName;           // example = "MOTO";

    bool bKeepReadingFile = true;

    const unsigned int BUFFER_SIZE = 1000;  // 1000 characters
    char textBuffer[BUFFER_SIZE];           // Allocate AND clear (that's the {0} part)
    // Clear that array to all zeros
    memset(textBuffer, 0, BUFFER_SIZE);

    while (bKeepReadingFile) {
        // Reads the entire line into the buffer (including any white space)
        modelTypeFile.getline(textBuffer, BUFFER_SIZE);

        PLYFileNameToLoad.clear();  // Erase whatever is already there (from before)
        PLYFileNameToLoad.append(textBuffer);

        // Is this the end of the file (have I read "EOF" yet?)?
        if (PLYFileNameToLoad == "EOF") {
            bKeepReadingFile = false;
            continue;
        }

        // Load the "friendly name" line also
        memset(textBuffer, 0, BUFFER_SIZE);
        modelTypeFile.getline(textBuffer, BUFFER_SIZE);
        friendlyName.clear();
        friendlyName.append(textBuffer);

        sModelDrawInfo drawInfo;
        c3DModelFileLoader fileLoader;

        std::string errorText = "";
        if (fileLoader.LoadPLYFile_Format_XYZ_N_RGBA_UV(PLYFileNameToLoad, drawInfo, errorText)) {
            std::cout << "Loaded the file OK" << std::endl;
        }
        else {
            std::cout << errorText;
        }

        if (pVAOManager->LoadModelIntoVAO(friendlyName, drawInfo, shaderID)) {
            std::cout << "Loaded the " << friendlyName << " model" << std::endl;
        }
    }

    return true;
}
// ------------------------ Load Model into VAO ------------------------


void GraphicScene::PositioningMe(cMeshObject* pMeWhoIsAsking) {
	cMeshObject* currObj = pMeWhoIsAsking;

	// Set a new X and Z
	//currObj->position = glm::vec3(RandomFloatGraphic(-128, 128), -46.5f, RandomFloatGraphic(-128, 128));

	float minDistance = 1000.f;
	int triangleMinDistance = 0;
	for (int j = 0; j < trianglesCenter.size(); j++) {
		float currentDistance = glm::distance(trianglesCenter[j], currObj->position);
		if (minDistance > currentDistance) {
			minDistance = currentDistance;
			triangleMinDistance = j;
		}
	}

	currObj->position = trianglesCenter[triangleMinDistance];
}

int GraphicScene::PrepareScene() {
    pTheShaderManager = new cShaderManager();

    cShaderManager::cShader vertexShader01;
    cShaderManager::cShader fragmentShader01;

    vertexShader01.fileName = "assets/shaders/vertexShader01.glsl";
    fragmentShader01.fileName = "assets/shaders/fragmentShader01.glsl";

    if (!pTheShaderManager->createProgramFromFile("Shader_1", vertexShader01, fragmentShader01)) {
        std::cout << "Didn't compile shaders" << std::endl;
        std::string theLastError = pTheShaderManager->getLastError();
        std::cout << "Because: " << theLastError << std::endl;
        return -1;
    }
    else {
        std::cout << "Compiled shader OK." << std::endl;
    }

    pTheShaderManager->useShaderProgram("Shader_1");
    shaderID = pTheShaderManager->getIDFromFriendlyName("Shader_1");
    glUseProgram(shaderID);

    pVAOManager = new cVAOManager();
    if (!LoadModelTypesIntoVAO("assets/PLYFilesToLoadIntoVAO.txt", pVAOManager, shaderID)) {
        std::cout << "Error: Unable to load list of models to load into VAO file" << std::endl;
        // Do we exit here? 
        // TO-DO
    }

    mvp_location = glGetUniformLocation(shaderID, "MVP");
    mModel_location = glGetUniformLocation(shaderID, "mModel");
    mView_location = glGetUniformLocation(shaderID, "mView");
    mProjection_location = glGetUniformLocation(shaderID, "mProjection");
    // Need this for lighting
    mModelInverseTransform_location = glGetUniformLocation(shaderID, "mModelInverseTranspose");

    pSkyBox = new cMeshObject();
    pSkyBox->meshName = "Skybox_Sphere";
    pSkyBox->friendlyName = "skybox";
}

void GraphicScene::DrawScene(GLFWwindow* window, glm::vec3 g_cameraEye, glm::vec3 g_cameraTarget) {
    // Making the fire impostor "move"
    for (int i = 0; i < vec_torchFlames.size(); i++) {
        cMeshObject* torchFire = vec_torchFlames[i];
        if (torchFire) {
            torchFire->scaleXYZ.y += RandomFloatGraphic(-0.1f, 0.1f);
        }
    }

    float ratio;
    int width, height;

    glm::mat4x4 matProjection;
    glm::mat4x4 matView;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;

    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

    matView = glm::lookAt(g_cameraEye,
        g_cameraTarget,
        upVector);

    // Pass eye location to the shader
    GLint eyeLocation_UniLoc = glGetUniformLocation(shaderID, "eyeLocation");

    glUniform4f(eyeLocation_UniLoc,
        g_cameraEye.x, g_cameraEye.y, g_cameraEye.z, 1.0f);

    matProjection = glm::perspective(
        0.6f,           // Field of view (in degress, more or less 180)
        ratio,
        0.1f,           // Near plane (make this as LARGE as possible)
        10000.0f);      // Far plane (make this as SMALL as possible)
    // 6-8 digits of precision

    glUniformMatrix4fv(mView_location, 1, GL_FALSE, glm::value_ptr(matView));
    glUniformMatrix4fv(mProjection_location, 1, GL_FALSE, glm::value_ptr(matProjection));

    //    ____  _             _            __                           
    //   / ___|| |_ __ _ _ __| |_    ___  / _|  ___  ___ ___ _ __   ___ 
    //   \___ \| __/ _` | '__| __|  / _ \| |_  / __|/ __/ _ \ '_ \ / _ \
    //    ___) | || (_| | |  | |_  | (_) |  _| \__ \ (_|  __/ | | |  __/
    //   |____/ \__\__,_|_|   \__|  \___/|_|   |___/\___\___|_| |_|\___|
    //                                                                  
    // We draw everything in our "scene"
    // In other words, go throug the vec_pMeshObjects container
    //  and draw each one of the objects 
    for (std::vector< cMeshObject* >::iterator itCurrentMesh = vec_pMeshObjects.begin();
        itCurrentMesh != vec_pMeshObjects.end();
        itCurrentMesh++)
    {
        cMeshObject* pCurrentMeshObject = *itCurrentMesh;

        if (!pCurrentMeshObject->bIsVisible)
            continue;

        // The parent's model matrix is set to the identity
        glm::mat4x4 matModel = glm::mat4x4(1.0f);

        DrawObject(pCurrentMeshObject,
            matModel,
            shaderID, g_pTextureManager,
            pVAOManager, mModel_location, mModelInverseTransform_location);
    }

    // --------------- Draw the skybox -----------------------------
    {
        GLint bIsSkyboxObject_UL = glGetUniformLocation(shaderID, "bIsSkyboxObject");
        glUniform1f(bIsSkyboxObject_UL, (GLfloat)GL_TRUE);

        glm::mat4x4 matModel = glm::mat4x4(1.0f);

        pSkyBox->position = g_cameraEye;
        pSkyBox->SetUniformScale(7500.0f);

        DrawObject(pSkyBox,
            matModel,
            shaderID, g_pTextureManager,
            pVAOManager, mModel_location, mModelInverseTransform_location);

        glUniform1f(bIsSkyboxObject_UL, (GLfloat)GL_FALSE);
    }
    // --------------- Draw the skybox -----------------------------

    //    _____           _          __                           
    //   | ____|_ __   __| |   ___  / _|  ___  ___ ___ _ __   ___ 
    //   |  _| | '_ \ / _` |  / _ \| |_  / __|/ __/ _ \ '_ \ / _ \
    //   | |___| | | | (_| | | (_) |  _| \__ \ (_|  __/ | | |  __/
    //   |_____|_| |_|\__,_|  \___/|_|   |___/\___\___|_| |_|\___|

}

void GraphicScene::LoadTextures() {
    // ---------------- LOADING TEXTURES ----------------------------------------------
    g_pTextureManager = new cBasicTextureManager();
    g_pTextureManager->SetBasePath("assets/textures");

    if (!g_pTextureManager->Create2DTextureFromBMPFile("Dungeons_2_Texture_01_A.bmp")) {
        std::cout << "Didn't load texture" << std::endl;
    }
    else {
        std::cout << "texture loaded" << std::endl;
    }

    if (!g_pTextureManager->Create2DTextureFromBMPFile("lroc_color_poles_4k.bmp")) {
        std::cout << "Didn't load texture" << std::endl;
    }
    else {
        std::cout << "texture loaded" << std::endl;
    }

    if (!g_pTextureManager->Create2DTextureFromBMPFile("fire-torch-texture.bmp")) {
        std::cout << "Didn't load texture" << std::endl;
    }
    else {
        std::cout << "texture loaded" << std::endl;
    }

    if (!g_pTextureManager->Create2DTextureFromBMPFile("Beholder_Base_color.bmp")) {
        std::cout << "Didn't load texture" << std::endl;
    }
    else {
        std::cout << "texture loaded" << std::endl;
    }

    // Load a skybox
    // Here's an example of the various sides: http://www.3dcpptutorials.sk/obrazky/cube_map.jpg
    std::string errorString = "";
    if (g_pTextureManager->CreateCubeTextureFromBMPFiles("SpaceBox",
        "SpaceBox_right1_posX.bmp", /* positive X */
        "SpaceBox_left2_negX_1.bmp",  /* negative X */
        "SpaceBox_top3_posY.bmp",    /* positive Y */
        "SpaceBox_bottom4_negY.bmp",  /* negative Y */
        "SpaceBox_front5_posZ_1.bmp",  /* positive Z */
        "SpaceBox_back6_negZ.bmp", /* negative Z */
        true, errorString)) {
        std::cout << "Loaded the tropical sunny day cube map OK" << std::endl;
    }
    else {
        std::cout << "ERROR: Didn't load the tropical sunny day cube map. How sad." << std::endl;
        std::cout << "(because: " << errorString << std::endl;
    }
}

void GraphicScene::Shutdown() {
    delete pTheShaderManager;
}

sModelDrawInfo GraphicScene::returnDrawInformation(std::string objectName)
{
    sModelDrawInfo drawInfo;
    pVAOManager->FindDrawInfoByModelName(objectName, drawInfo);
    return drawInfo;
}

GLuint GraphicScene::returnShaderID(std::string shaderName)
{
    GLuint shaderID;
    shaderID = pTheShaderManager->getIDFromFriendlyName(shaderName);
    return shaderID;
}

void GraphicScene::CreateGameObjectByType(const std::string& type, glm::vec3 position, sModelDrawInfo& drawInfo) {
	cMeshObject* go = new cMeshObject();
	go->meshName = type;
	go->friendlyName = type;
	go->position = position;
	go->bUse_RGBA_colour = false;
	go->RGBA_colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); 
	go->specular_colour_and_power = glm::vec4(0.95f, 0.85f, 0.29f, 0.3f);
	go->isWireframe = false;
	go->soundPlayed = false;
	go->numberOfTriangles = drawInfo.numberOfTriangles;
	go->meshTriangles = drawInfo.modelTriangles;

	vec_pMeshObjects.push_back(go);
}

cMeshObject* GraphicScene::GetObjectByName(std::string name, bool bSearchChildren) {
	cMeshObject* returnObject;
	for (int i = 0; i < vec_pMeshObjects.size(); i++) {
		returnObject = vec_pMeshObjects[i];
		if (vec_pMeshObjects[i]->friendlyName == name) {
			return returnObject;
		}

		// Search children too? 
		cMeshObject* pChildMesh = returnObject->findObjectByFriendlyName(name, bSearchChildren);

		if (pChildMesh) { /* NULL = 0 = false) */
			return pChildMesh;
		}
	}

	return nullptr;
}
