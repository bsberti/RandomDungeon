#include "GraphicScene.h"
#include "cCharacter.h"
#include <glm/gtx/intersect.hpp>

GraphicScene::GraphicScene() {
	cameraFollowing = false;
	cameraTransitioning = false;
    drawFog = 10;
    pVAOManager = new cVAOManager();
    loggedIn = false;
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

bool LoadFBXFileToModelDrawInfo(Model* model, sModelDrawInfo& modelDrawInfo, std::string& errorText)
{
    modelDrawInfo.numberOfVertices = model->Vertices.size();
    modelDrawInfo.numberOfTriangles = model->NumTriangles;
    modelDrawInfo.numberOfIndices = model->NumTriangles * 3;

    // This is now different because the vertex layout in the shader is different
    modelDrawInfo.pVertices = new sVertex_RGBA_XYZ_N_UV_T_BiN_Bones[modelDrawInfo.numberOfVertices];
    //modelDrawInfo.pVertices = new sVertex[modelDrawInfo.numberOfVertices];

    // Now copy the information from the PLY infomation to the model draw info structure
    for (unsigned int index = 0; index != modelDrawInfo.numberOfVertices; index++)
    {
        // To The Shader                        From the file
        modelDrawInfo.pVertices[index].x = model->Vertices[index].x;
        modelDrawInfo.pVertices[index].y = model->Vertices[index].y;
        modelDrawInfo.pVertices[index].z = model->Vertices[index].z;
    }

    return true;
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

    cShaderManager::cShader boneShaderVertex;
    cShaderManager::cShader boneShaderFragment;

    vertexShader01.fileName = "assets/shaders/vertexShader01.glsl";
    fragmentShader01.fileName = "assets/shaders/fragmentShader01.glsl";

    boneShaderVertex.fileName = "assets/shaders/BoneShader.vertex.glsl";
    boneShaderFragment.fileName = "assets/shaders/BoneShader.fragment.glsl";

    if (!pTheShaderManager->createProgramFromFile("Shader_1", vertexShader01, fragmentShader01)) {
        std::cout << "Didn't compile shaders" << std::endl;
        std::string theLastError = pTheShaderManager->getLastError();
        std::cout << "Because: " << theLastError << std::endl;
        return -1;
    }
    else {
        std::cout << "Compiled Shader_1 OK." << std::endl;
    }

    if (!pTheShaderManager->createProgramFromFile("BoneShader", boneShaderVertex, boneShaderFragment)) {
        std::cout << "Didn't compile shaders" << std::endl;
        std::string theLastError = pTheShaderManager->getLastError();
        std::cout << "Because: " << theLastError << std::endl;
        return -1;
    }
    else {
        std::cout << "Compiled BoneShader OK." << std::endl;
    }

    pTheShaderManager->useShaderProgram("Shader_1");
    shaderID = pTheShaderManager->getIDFromFriendlyName("Shader_1");
    glUseProgram(shaderID);

    if (!LoadModelTypesIntoVAO("assets/PLYFilesToLoadIntoVAO.txt", pVAOManager, shaderID)) {
        std::cout << "Error: Unable to load list of models to load into VAO file" << std::endl;
        // Do we exit here? 
        // TO-DO
        return -1;
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

    cShaderManager::cShaderProgram* shader = pTheShaderManager->pGetShaderProgramFromFriendlyName("BoneShader");
    printf("gBoneShaderId id: %d\n", shader->ID);
    //glUseProgram(shader->ID);
    //BoneShaderUpdate(shader->ID);

    sModelDrawInfo drawInfo;
    std::string errorText = "";

    pVAOManager->shaderID = shaderID;
    pVAOManager->loadModel("assets/models/animation/Mutant.fbx");
    pVAOManager->loadModel("assets/models/animation/MutantIdle.fbx");
    pVAOManager->loadModel("assets/models/animation/MutantWalking.fbx");

    pVAOManager->loadModel("assets/models/animation/MainChar.fbx");
    pVAOManager->loadModel("assets/models/animation/Ninja.fbx");
    pVAOManager->loadModel("assets/models/animation/Warrior.fbx");
    pVAOManager->loadModel("assets/models/animation/Archer.fbx");
    pVAOManager->loadModel("assets/models/animation/Monk.fbx");

}


bool IntersectPlanes(const glm::vec4& plane1, const glm::vec4& plane2, const glm::vec4& plane3, glm::vec3& point) {
    // Calculate the cross product of the first two planes' normals
    glm::vec3 n1(plane1);
    glm::vec3 n2(plane2);
    glm::vec3 n3(plane3);
    glm::vec3 n1xn2 = glm::cross(n1, n2);

    // Calculate the determinant of the matrix formed by the three plane normals
    float det = glm::dot(n1xn2, n3);

    // If the determinant is zero, the planes are parallel and do not intersect
    if (det == 0.0f) {
        return false;
    }

    // Calculate the point where the three planes intersect
    glm::vec3 p1(plane1.w * n2.y * n3.z + n1.y * plane2.w * n3.z + n1.z * n2.y * plane3.w - n1.z * plane2.w * n3.y - n1.y * n2.z * plane3.w + plane1.w * n2.z * n3.y,
        n1.x * plane2.w * n3.z + plane1.w * n2.x * n3.z + n1.z * n2.x * plane3.w - n1.z * plane2.w * n3.x - plane1.w * n2.z * n3.x + n1.x * n2.z * plane3.w,
        n1.x * n2.y * plane3.w + n1.y * n2.x * plane3.w + plane1.w * n2.x * n3.y - n1.x * plane2.w * n3.y - n1.y * plane2.w * n3.x - plane1.w * n2.y * n3.x);
    point = p1 / det;

    return true;
}

void GraphicScene::CalculateSceneExtension(glm::vec3 g_cameraEye, glm::vec3 g_cameraTarget) {
    // Set up projection matrix
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //gluPerspective(fov, ratio, nearPlane, farPlane);
    //glGetFloatv(GL_PROJECTION_MATRIX, &matProjection[0][0]);

    // Set up view matrix
    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //gluLookAt(eyeX, eyeY, eyeZ, targetX, targetY, targetZ, upX, upY, upZ);
    //glGetFloatv(GL_MODELVIEW_MATRIX, &matView[0][0]);
    
    // Calculate frustum planes
    frustumMatrix = matProjection * matView;
    glm::vec4 leftPlane = frustumMatrix[3] + frustumMatrix[0];
    glm::vec4 rightPlane = frustumMatrix[3] - frustumMatrix[0];
    glm::vec4 bottomPlane = frustumMatrix[3] + frustumMatrix[1];
    glm::vec4 topPlane = frustumMatrix[3] - frustumMatrix[1];
    glm::vec4 nearPlane = frustumMatrix[3] + frustumMatrix[2];
    glm::vec4 farPlane = frustumMatrix[3] - frustumMatrix[2];

    // Calculate frustum vertices
    std::vector<glm::vec3> frustumVertices(8);
    IntersectPlanes(leftPlane, bottomPlane, nearPlane, frustumVertices[0]);
    IntersectPlanes(leftPlane, topPlane, nearPlane, frustumVertices[1]);
    IntersectPlanes(rightPlane, topPlane, nearPlane, frustumVertices[2]);
    IntersectPlanes(rightPlane, bottomPlane, nearPlane, frustumVertices[3]);
    IntersectPlanes(leftPlane, bottomPlane, farPlane, frustumVertices[4]);
    IntersectPlanes(leftPlane, topPlane, farPlane, frustumVertices[5]);
    IntersectPlanes(rightPlane, topPlane, farPlane, frustumVertices[6]);
    IntersectPlanes(rightPlane, bottomPlane, farPlane, frustumVertices[7]);

    // Calculate scene extension
    minExt = frustumVertices[0];
    maxExt = frustumVertices[0];
    for (int i = 1; i < 8; i++) {
        glm::vec3 vertex = frustumVertices[i];
        minExt = glm::min(minExt, vertex);
        maxExt = glm::max(maxExt, vertex);
    }
}

void GraphicScene::cleanMazeView() {
    vec_pMeshCurrentMaze.clear();
}

void GraphicScene::DrawScene(GLFWwindow* window, glm::vec3 g_cameraEye, glm::vec3 g_cameraTarget) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;
    glViewport(0, 0, width, height);

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

    glUniformMatrix4fv(mView_location, 1, GL_FALSE, glm::value_ptr(matView));
    glUniformMatrix4fv(mProjection_location, 1, GL_FALSE, glm::value_ptr(matProjection));

    //    ____  _             _            __                           
    //   / ___|| |_ __ _ _ __| |_    ___  / _|  ___  ___ ___ _ __   ___ 
    //   \___ \| __/ _` | '__| __|  / _ \| |_  / __|/ __/ _ \ '_ \ / _ \
    //    ___) | || (_| | |  | |_  | (_) |  _| \__ \ (_|  __/ | | |  __/
    //   |____/ \__\__,_|_|   \__|  \___/|_|   |___/\___\___|_| |_|\___|
                                                                      
    // We draw everything in our "vec_pMeshCurrentMaze"
    // In other words, go throug the vec_pMeshObjects container
    //  and draw each one of the objects

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    if (animator != nullptr)
        animator->UpdateAnimation(deltaTime);

    for (std::vector< cMeshObject* >::iterator itCurrentMesh = vec_pMeshCurrentMaze.begin();
        itCurrentMesh != vec_pMeshCurrentMaze.end();
        itCurrentMesh++)
    {
        cMeshObject* pCurrentMeshObject = *itCurrentMesh;
        auto transforms = animator->GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i)
        {
            GLint boneMatLoc = glGetUniformLocation(shaderID, std::string("BoneMatrices[" + std::to_string(i) + "]").c_str());
            glUniformMatrix4fv(boneMatLoc, 1, GL_FALSE, glm::value_ptr(transforms[i]));
        }

        if (!pCurrentMeshObject->bIsVisible)
            continue;

        if (pCurrentMeshObject->friendlyName == "Plane_Floor")
            continue;

        // The parent's model matrix is set to the identity
        glm::mat4x4 matModel = glm::mat4x4(1.0f);

        DrawObject(pCurrentMeshObject,
            matModel,
            shaderID, g_pTextureManager,
            pVAOManager, mModel_location, mModelInverseTransform_location);
    }

    for (std::vector<cMeshObject*>::iterator itBeholds =
        map_beholds.begin(); itBeholds != map_beholds.end();
        itBeholds++) 
    {
        cMeshObject* pCurrentMeshObject = *itBeholds;

        if (!pCurrentMeshObject->bIsVisible)
            continue;

        // The parent's model matrix is set to the identity
        glm::mat4x4 matModel = glm::mat4x4(1.0f);

        float distance = glm::distance(pCurrentMeshObject->position, g_cameraTarget);
        float range = drawFog * 50.f; //GLOBAL_MAP_OFFSET
        if (distance < range) {
            DrawObject(pCurrentMeshObject,
                matModel,
                shaderID, g_pTextureManager,
                pVAOManager, mModel_location, mModelInverseTransform_location);
        }
    }

    // --------------- Draw the skybox -----------------------------
    //{
    //    GLint bIsSkyboxObject_UL = glGetUniformLocation(shaderID, "bIsSkyboxObject");
    //    glUniform1f(bIsSkyboxObject_UL, (GLfloat)GL_TRUE);

    //    glm::mat4x4 matModel = glm::mat4x4(1.0f);

    //    pSkyBox->position = g_cameraEye;
    //    pSkyBox->SetUniformScale(7500.0f);

    //    DrawObject(pSkyBox,
    //        matModel,
    //        shaderID, g_pTextureManager,
    //        pVAOManager, mModel_location, mModelInverseTransform_location);

    //    glUniform1f(bIsSkyboxObject_UL, (GLfloat)GL_FALSE);
    //}

    //    _____           _          __                           
    //   | ____|_ __   __| |   ___  / _|  ___  ___ ___ _ __   ___ 
    //   |  _| | '_ \ / _` |  / _ \| |_  / __|/ __/ _ \ '_ \ / _ \
    //   | |___| | | | (_| | | (_) |  _| \__ \ (_|  __/ | | |  __/
    //   |_____|_| |_|\__,_|  \___/|_|   |___/\___\___|_| |_|\___|
    
    //glDisable(GL_SCISSOR_TEST);
    //glfwSwapBuffers(window);

    // Disable blending
    //glDisable(GL_BLEND);
}

void GraphicScene::DrawMapView(GLFWwindow* window, glm::vec3 g_cameraEye, glm::vec3 g_cameraTarget) {

    // Enable blending
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // Clear color and depth buffer
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    //glLoadIdentity();

    mapWidth = 200;
    mapHeigth = 200;
    
    ratio = mapWidth / (float)mapHeigth;
    glViewport(0, 0, mapWidth, mapHeigth);

    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, mapWidth, mapHeigth);

    // Draw the first part of the scene
    // Make sure to set the depth value to a low value
    // to ensure that it is drawn in front of the second part of the scene
    //glClearDepth(1.0f);
    //glDepthFunc(GL_GREATER);

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

    // We draw everything in our "vec_pMeshCurrentMaze"
    // In other words, go throug the vec_pMeshObjects container
    //  and draw each one of the objects
    for (std::vector< cMeshObject* >::iterator itCurrentMesh = vec_pMeshCurrentMaze.begin();
        itCurrentMesh != vec_pMeshCurrentMaze.end();
        itCurrentMesh++)
    {
        cMeshObject* pCurrentMeshObject = *itCurrentMesh;

        if (!pCurrentMeshObject->bIsVisible)
            continue;

        if (pCurrentMeshObject->meshName == "assets/models/animation/UnarmedIdleLooking.fbx") {
            int breakpoint = 5;
        }

        // The parent's model matrix is set to the identity
        glm::mat4x4 matModel = glm::mat4x4(1.0f);

        DrawObject(pCurrentMeshObject,
            matModel,
            shaderID, g_pTextureManager,
            pVAOManager, mModel_location, mModelInverseTransform_location);
    }

    for (std::vector<cMeshObject*>::iterator itBeholds =
        map_beholds.begin(); itBeholds != map_beholds.end();
        itBeholds++)
    {
        cMeshObject* pCurrentMeshObject = *itBeholds;

        if (!pCurrentMeshObject->bIsVisible)
            continue;

        // The parent's model matrix is set to the identity
        glm::mat4x4 matModel = glm::mat4x4(1.0f);

        float distance = glm::distance(pCurrentMeshObject->position, g_cameraTarget);
        float range = drawFog * 50.f; //GLOBAL_MAP_OFFSET
        if (distance < range) {
            DrawObject(pCurrentMeshObject,
                matModel,
                shaderID, g_pTextureManager,
                pVAOManager, mModel_location, mModelInverseTransform_location);
        }
    }

    //    _____           _          __                           
    //   | ____|_ __   __| |   ___  / _|  ___  ___ ___ _ __   ___ 
    //   |  _| | '_ \ / _` |  / _ \| |_  / __|/ __/ _ \ '_ \ / _ \
    //   | |___| | | | (_| | | (_) |  _| \__ \ (_|  __/ | | |  __/
    //   |_____|_| |_|\__,_|  \___/|_|   |___/\___\___|_| |_|\___|
    
    glDisable(GL_SCISSOR_TEST);
    glEnable(GL_DEPTH_TEST);
    // Set the blending function for the second part of the scene
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

bool GraphicScene::SATIntersectionTest(cMeshObject* mesh, glm::mat4 frustum) {
    // Extract the six planes of the frustum
    std::vector<glm::vec4> frustumPlanes;
    ExtractFrustumPlanes(frustum, frustumPlanes);

    // Iterate over all potential separating axes
    for (const glm::vec4& plane : frustumPlanes) {
        // Extract the normal of the plane and normalize it
        glm::vec3 planeNormal(plane.x, plane.y, plane.z);
        planeNormal = glm::normalize(planeNormal);

        // Project all vertices of the mesh onto the axis defined by the plane normal
        //float min = INFINITY, max = -INFINITY;
        //ProjectVerticesOntoAxis(mesh->meshVertices, planeNormal, min, max);

        float projection = glm::dot(mesh->position, planeNormal);
        float min = std::min(min, projection);
        float max = std::max(max, projection);

        // Check for overlap between the projections
        if (max < -plane.w || min > plane.w) {
            // The projections do not overlap, so the mesh is not visible in the frustum
            return false;
        }
    }

    // If we get here, then no separating axis was found, so the mesh is visible in the frustum
    return true;
}

void GraphicScene::ExtractFrustumPlanes(const glm::mat4& frustumMatrix,
    std::vector<glm::vec4>& frustumPlanes) {
    // Extract the six planes of a frustum matrix
    frustumPlanes.resize(6);
    frustumPlanes[0] = GetRow(frustumMatrix, 3) + GetRow(frustumMatrix, 0);  // Left plane
    frustumPlanes[1] = GetRow(frustumMatrix, 3) - GetRow(frustumMatrix, 0);  // Right plane
    frustumPlanes[2] = GetRow(frustumMatrix, 3) + GetRow(frustumMatrix, 1);  // Bottom plane
    frustumPlanes[3] = GetRow(frustumMatrix, 3) - GetRow(frustumMatrix, 1);  // Top plane
    frustumPlanes[4] = GetRow(frustumMatrix, 3) + GetRow(frustumMatrix, 2);  // Near plane
    frustumPlanes[5] = GetRow(frustumMatrix, 3) - GetRow(frustumMatrix, 2);  // Far plane
    for (glm::vec4& plane : frustumPlanes) {
        plane /= glm::length(glm::vec3(plane));
    }
}

glm::vec4 GraphicScene::GetRow(const glm::mat4& matrix, int row) {
    return glm::vec4(matrix[row][0], matrix[row][1], matrix[row][2], matrix[row][3]);
}

void GraphicScene::ProjectVerticesOntoAxis(const std::vector<glm::vec3>& vertices,
    const glm::vec3& axis, float& min, float& max) {
    // Project a set of vertices onto an axis and find their min/max projections
    min = INFINITY;
    max = -INFINITY;
    for (const glm::vec3& vertex : vertices) {
        float projection = glm::dot(vertex, axis);
        min = std::min(min, projection);
        max = std::max(max, projection);
    }
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

    if (!g_pTextureManager->Create2DTextureFromBMPFile("Mutant_diffuse.bmp")) {
        std::cout << "Didn't load texture" << std::endl;
    }
    else {
        std::cout << "texture loaded" << std::endl;
    }
    if (!g_pTextureManager->Create2DTextureFromBMPFile("Mutant_normal.bmp")) {
        std::cout << "Didn't load texture" << std::endl;
    }
    else {
        std::cout << "texture loaded" << std::endl;
    }

    if (!g_pTextureManager->Create2DTextureFromBMPFile("Ninja_diffuse.bmp")) {
        std::cout << "Didn't load texture" << std::endl;
    }
    else {
        std::cout << "texture loaded" << std::endl;
    }
    if (!g_pTextureManager->Create2DTextureFromBMPFile("Ninja_normal.bmp")) {
        std::cout << "Didn't load texture" << std::endl;
    }
    else {
        std::cout << "texture loaded" << std::endl;
    }

    if (!g_pTextureManager->Create2DTextureFromBMPFile("Archer_diffuse.bmp")) {
        std::cout << "Didn't load texture" << std::endl;
    }
    else {
        std::cout << "texture loaded" << std::endl;
    }
    if (!g_pTextureManager->Create2DTextureFromBMPFile("Archer_normal.bmp")) {
        std::cout << "Didn't load texture" << std::endl;
    }
    else {
        std::cout << "texture loaded" << std::endl;
    }

    if (!g_pTextureManager->Create2DTextureFromBMPFile("Warrior_diffuse.bmp")) {
        std::cout << "Didn't load texture" << std::endl;
    }
    else {
        std::cout << "texture loaded" << std::endl;
    }
    if (!g_pTextureManager->Create2DTextureFromBMPFile("Warrior_normal.bmp")) {
        std::cout << "Didn't load texture" << std::endl;
    }
    else {
        std::cout << "texture loaded" << std::endl;
    }

    if (!g_pTextureManager->Create2DTextureFromBMPFile("Monk_diffuse.bmp")) {
        std::cout << "Didn't load texture" << std::endl;
    }
    else {
        std::cout << "texture loaded" << std::endl;
    }
    if (!g_pTextureManager->Create2DTextureFromBMPFile("Monk_normal.bmp")) {
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

cMeshObject* GraphicScene::CreateGameObjectByType(const std::string& type, glm::vec3 position, sModelDrawInfo& drawInfo) {
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

    //for (unsigned int index = 0; index != drawInfo.numberOfVertices; index++) {
    //    glm::vec3 newVertice = glm::vec3(
    //        drawInfo.pVertices[index].x, 
    //        drawInfo.pVertices[index].y, 
    //        drawInfo.pVertices[index].z);
    //    go->meshVertices.push_back(newVertice);
    //}

    // Instead of adding to the vector right now
    // gonna just return so the one adding to vector can be handle by critical section
    //vec_pMeshObjects.push_back(go);
    return go;
}

cCharacter* GraphicScene::CreateAnimatedCharacter(const char* filename,
    const std::vector<std::string>& animations, sModelDrawInfo drawInfo) {
    cCharacter* tempChar = new cCharacter();
    tempChar->position = glm::vec3(0.0f);
    tempChar->scale = 1.0f;
    tempChar->qRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    tempChar->meshName = filename;
    tempChar->bUse_RGBA_colour = false;
    tempChar->numberOfTriangles = drawInfo.numberOfTriangles;
    tempChar->meshTriangles = drawInfo.modelTriangles;
    tempChar->Animation.IsCharacterAnimation = true;
    tempChar->Animation.AnimationTime = 0.f;
    tempChar->Animation.IsLooping = true;
    tempChar->Animation.IsPlaying = true;
    tempChar->Animation.Speed = 1.f;
    tempChar->HasBones = true;
    tempChar->Enabled = true;

    int numAnimations = animations.size();
    for (int i = 0; i < numAnimations; i++)
    {
        // Load animation
        tempChar->LoadAnimationFromAssimp(animations[i].c_str());
    }

    return tempChar;
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

cMeshObject* GraphicScene::GetObjectByGridPosition(int i, int j) {

    cMeshObject* returnObject;
    for (int x = 0; x < vec_pMeshCurrentMaze.size(); x++) {
        returnObject = vec_pMeshCurrentMaze[x];

        size_t index = returnObject->friendlyName.find("Floor");
        if (index == std::string::npos) continue;

        if (vec_pMeshCurrentMaze[x]->currentI == i &&
            vec_pMeshCurrentMaze[x]->currentJ == j) {
            return returnObject;
        }
    }

    return nullptr;
}