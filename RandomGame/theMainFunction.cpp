
#include "globalOpenGL.h"
#include "globalThings.h"

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>

#include "GraphicScene.h"

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "cRandomUI.h"
#include "FModManager.h"
#include "BlocksLoader.h"
#include "cLuaBrain.h"
#include "EntityLoaderManager.h"

#include "cMazeMaker_W2023.h"
#include <chrono>

#include "threads.h"

// Physics Includes

#include "PhysicsEngine/Shapes.h"
#include "PhysicsEngine/PhysicsObject.h"
#include "PhysicsEngine/PhysicsSystem.h"

PhysicsSystem* g_PhysicsSystem;

glm::vec3 g_cameraEye = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_MapCameraEye = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_MapCameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

GraphicScene g_GraphicScene;
//ParticleSystem g_ParticleSystem;
cLuaBrain* pBrain;
cMazeMaker_W2023 theMM;
BlocksLoader* m_blocksLoader;

cRandomUI gameUi;
FModManager* fmod_manager;
FMOD::Channel* _channel;
constexpr int max_channels = 255;
//SimulationView* simView = new SimulationView();
int animationType;
float animationSpeed;

GLFWwindow* window;

float NinetyDegrees = 1.575f;
std::map< std::string, cMeshObject*>::iterator itBeholdsToFollow;

#define NUMBER_OF_TAGS 10
#define MAP_WIDTH 100
#define MAP_HEIGHT 100
#define BEHOLDERS_NUMBER 100
#define GLOBAL_MAP_OFFSET 50
#define SMALLEST_DISTANCE 0.1
#define CAMERA_OFFSET 50.0
#define MAIN_CHAR_JSON_INFO "MainCharacter.json"

RotationKeyFrame nextRKF;
RotationKeyFrame currRKF;

cMeshObject* currentBehold;

PositionKeyFrame nextPKF;
PositionKeyFrame currPKF;

#define NUM_THREADS 64
#define NUM_ELEMENTS_TO_INIT 10000
CRITICAL_SECTION dataMutex; 
HANDLE dataSemaphore;
//HANDLE ahThread[NUM_THREADS];
HANDLE ahThread;

// Main Character Objects
cMeshObject* mainChar;
cMeshObject* planeFloor;
cCharacter playabledCharacter;

// Entity Loader using JSON
EntityLoaderManager* entityLoaderManager = EntityLoaderManager::GetInstance();

// String used for error feedback for methods call
std::string errorMessage;

// Call back signatures here
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

typedef struct _MAZE_TILE_INFO
{
    int i;
    int j;
} _MAZE_TILE_INFO;

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

// From here: https://stackoverflow.com/questions/5289613/generate-random-float-between-two-floats/5289624
float RandomFloat(float a, float b) {
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

bool SaveTheVAOModelTypesToFile(std::string fileTypesToLoadName,
    cVAOManager* pVAOManager);

void DrawConcentricDebugLightObjects(int currentLight);

void updateCurrentMazeView(int newI, int newJ);

// HACK: These are the light spheres we will use for debug lighting
cMeshObject* pDebugSphere_1 = NULL; // = new cMeshObject();
cMeshObject* pDebugSphere_2 = NULL; // = new cMeshObject();
cMeshObject* pDebugSphere_3 = NULL; // = new cMeshObject();
cMeshObject* pDebugSphere_4 = NULL; // = new cMeshObject();
cMeshObject* pDebugSphere_5 = NULL; // = new cMeshObject();

// Creation and positioning of the lights
void lightning(GLuint shaderID) {
    ::g_pTheLightManager = new cLightManager();
    cLightHelper* pLightHelper = new cLightHelper();

    ::g_pTheLightManager->CreateBasicDirecLight(shaderID, glm::vec4(250.0f, 900.0f, 250.0f, 0.0f));
}

void rotateMinMax(glm::vec3& minPoint, glm::vec3& maxPoint) {

    // Calculate the center point of the AABB object
    glm::vec3 centerPoint = (minPoint + maxPoint) / 2.0f;

    // Calculate the rotation matrix for a 90-degree rotation around the y-axis
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Apply the rotation matrix to each corner point of the AABB object
    glm::vec4 rotatedMinPoint = rotationMatrix * glm::vec4(minPoint - centerPoint, 1.0f);
    glm::vec4 rotatedMaxPoint = rotationMatrix * glm::vec4(maxPoint - centerPoint, 1.0f);

    // Calculate the new minimum and maximum x, y, and z values of the rotated AABB
    float rotatedMinX = min(rotatedMinPoint.x, rotatedMaxPoint.x) + centerPoint.x;
    float rotatedMaxX = max(rotatedMinPoint.x, rotatedMaxPoint.x) + centerPoint.x;
    float rotatedMinY = min(rotatedMinPoint.y, rotatedMaxPoint.y) + centerPoint.y;
    float rotatedMaxY = max(rotatedMinPoint.y, rotatedMaxPoint.y) + centerPoint.y;
    float rotatedMinZ = min(rotatedMinPoint.z, rotatedMaxPoint.z) + centerPoint.z;
    float rotatedMaxZ = max(rotatedMinPoint.z, rotatedMaxPoint.z) + centerPoint.z;

    // Print the new minimum and maximum points of the rotated AABB object
    //std::cout << "Rotated AABB minimum point: (" << rotatedMinX << ", " << rotatedMinY << ", " << rotatedMinZ << ")" << std::endl;
    //std::cout << "Rotated AABB maximum point: (" << rotatedMaxX << ", " << rotatedMaxY << ", " << rotatedMaxZ << ")" << std::endl;

    minPoint.x = rotatedMinX;
    minPoint.y = rotatedMinY;
    minPoint.z = rotatedMinZ;
    
    maxPoint.x = rotatedMaxX;
    maxPoint.y = rotatedMaxY;
    maxPoint.z = rotatedMaxZ;
}

void setStaticPhysObjectAABB(cMeshObject* mesh, bool rotated) {
    //cMeshObject* pmesh;
    iShape* paabb = nullptr;
    PhysicsObject* pphysObj = nullptr;

    // Cone AABB
    //pmesh = g_GraphicScene.GetObjectByName(friendlyName, false);
    sModelDrawInfo drawingInfo = g_GraphicScene.returnDrawInformation(mesh->meshName);

    // Creates the AABB structure for the Terrain
    float min[3] = { mesh->position.x + drawingInfo.minX,
                     mesh->position.y + drawingInfo.minY,
                     mesh->position.z + drawingInfo.minZ };
    float max[3] = { mesh->position.x + drawingInfo.maxX,
                     mesh->position.y + drawingInfo.maxY,
                     mesh->position.z + drawingInfo.maxZ };

    glm::vec3 minPoint;
    minPoint.x = min[0];
    minPoint.y = min[1];
    minPoint.z = min[2];

    glm::vec3 maxPoint;
    maxPoint.x = max[0];
    maxPoint.y = max[1];
    maxPoint.z = max[2];

    if (rotated) {
        rotateMinMax(minPoint, maxPoint);

        min[0] = minPoint.x;
        min[1] = minPoint.y;
        min[2] = minPoint.z;

        max[0] = maxPoint.x;
        max[1] = maxPoint.y;
        max[2] = maxPoint.z;
    }

    paabb = new AABB(min, max);

    Vector3 position;
    position.x = mesh->position.x;
    position.y = mesh->position.y;
    position.z = mesh->position.z;

    // Adds the AABB to the Physics System
    pphysObj = g_PhysicsSystem->CreatePhysicsObject(mesh->meshName, position, paabb);
    pphysObj->SetMass(-1.0f);
}

// Function called inside creatingModels for the wall object creation
void createWall(unsigned int line, unsigned int column, float x, float z, bool horizontal, std::string orientation) {
    sModelDrawInfo drawingInformation;
    //pVAOManager->FindDrawInfoByModelName("Wall", drawingInformation);
    drawingInformation = g_GraphicScene.returnDrawInformation("Wall");
    cMeshObject* wall = g_GraphicScene.CreateGameObjectByType("Wall", glm::vec3(x, 0.0f, z), drawingInformation);
    //wall = g_GraphicScene.GetObjectByName("Wall", false);
    wall->friendlyName = "Wall" + std::to_string(line) + "_" + std::to_string(column) + orientation;
    wall->textures[0] = "Dungeons_2_Texture_01_A.bmp";
    wall->textureRatios[0] = 1.0f;

    wall->currentI = line;
    wall->currentJ = column;

    if (!horizontal) {
        //wall->setRotationFromEuler(glm::vec3(0.0f, 1.575f, 0.0f));
        wall->rotation.y = 1.575f;
    }

    setStaticPhysObjectAABB(wall, !horizontal);

    g_GraphicScene.vec_pMeshCurrentMaze.push_back(wall);
}

// Function called inside creatingModels for the Torch object creation
void createTorch(unsigned int line, unsigned int column, glm::vec3 pos, glm::vec3 rotation, std::string orientation) {
    sModelDrawInfo drawingInformation;
    //pVAOManager->FindDrawInfoByModelName("Torch", drawingInformation);
    drawingInformation = g_GraphicScene.returnDrawInformation("Torch");
    cMeshObject* torch = g_GraphicScene.CreateGameObjectByType("Torch", pos, drawingInformation);
    //torch = g_GraphicScene.GetObjectByName("Torch", false);
    torch->friendlyName = "Torch" + std::to_string(line) + "_" + std::to_string(column) + orientation;
    torch->textures[0] = "Dungeons_2_Texture_01_A.bmp";
    torch->textureRatios[0] = 1.0f;
    torch->setRotationFromEuler(rotation);

    cMeshObject* flame = new cMeshObject();
    flame->meshName = "Imposter_Base";
    flame->friendlyName = "Flame" + std::to_string(line) + "_" + std::to_string(column) + orientation;
    flame->textures[0] = "fire-torch-texture.bmp";
    flame->textureRatios[0] = 1.0f;

    //flame->setRotationFromEuler(glm::vec3(0.0f));
    flame->SetUniformScale(3.0f);
    //flame->position = pos;
    //flame->position.y += 10;
    //flame->position.x += 10;

    torch->vecChildMeshes.push_back(flame);
    g_GraphicScene.vec_torchFlames.push_back(flame);
}

// Function called inside creatingModels for the Dead Body object creation
void createDeadbody(unsigned int line, unsigned int column, glm::vec3 pos, glm::vec3 rotation, std::string orientation) {
    sModelDrawInfo drawingInformation;
    //pVAOManager->FindDrawInfoByModelName("Deadbody", drawingInformation);
    drawingInformation = g_GraphicScene.returnDrawInformation("Deadbody");
    cMeshObject* deadbody = g_GraphicScene.CreateGameObjectByType("Deadbody", pos, drawingInformation);
    //deadbody = g_GraphicScene.GetObjectByName("Deadbody", false);
    deadbody->friendlyName = "Deadbody" + std::to_string(line) + "_" + std::to_string(column) + orientation;
    deadbody->textures[0] = "Dungeons_2_Texture_01_A.bmp";
    deadbody->textureRatios[0] = 1.0f;
    deadbody->setRotationFromEuler(rotation);
    deadbody->SetUniformScale(0.2f);
}

//DWORD WINAPI createMazeTile(PVOID pvParam) {
//    _MAZE_TILE_INFO* pInfo = (_MAZE_TILE_INFO*)pvParam;
//
//    sModelDrawInfo drawingInformation;
//    std::string northString = "";
//    std::string westString = "";
//    std::string eastString = "";
//    std::string southString = "";
//
//    int i = pInfo->i;
//    int j = pInfo->j;
//
//    if (i > 0) northString = m_blocksLoader->g_blockMap->at(i - 1).at(j);
//    if (j > 0) westString = m_blocksLoader->g_blockMap->at(i).at(j - 1);
//    if (j < m_blocksLoader->g_blockMap->at(i).size()) eastString = m_blocksLoader->g_blockMap->at(i).at(j + 1);
//    if (i < m_blocksLoader->g_blockMap->size()) southString = m_blocksLoader->g_blockMap->at(i + 1).at(j);
//
//    float x = (j * GLOBAL_MAP_OFFSET);
//    float z = (i * GLOBAL_MAP_OFFSET);
//    //pVAOManager->FindDrawInfoByModelName("Floor", drawingInformation);
//    drawingInformation = g_GraphicScene.returnDrawInformation("Floor");
//    cMeshObject* currentObject = g_GraphicScene.CreateGameObjectByType("Floor", glm::vec3(x, 0.0f, z), drawingInformation);
//    //currentObject = g_GraphicScene.GetObjectByName("Floor", false);
//    std::string floorName = "Floor" + std::to_string(i) + "_" + std::to_string(j);
//    currentObject->currentI = i;
//    currentObject->currentJ = j;
//    currentObject->friendlyName = floorName;
//    currentObject->textures[0] = "Dungeons_2_Texture_01_A.bmp";
//    currentObject->textureRatios[0] = 1.0f;
//
//    // Creating some Walls! Depending on the adjacents tiles
//    if (i == 0) {
//        if (j == 0) {
//            float x1 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
//            float z1 = (GLOBAL_MAP_OFFSET) * (j - 1);
//            createWall(i, j, x1, z1, true, "N");
//
//            float x2 = (GLOBAL_MAP_OFFSET) * (i - 1);
//            float z2 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
//            createWall(i, j, x2, z2, false, "W");
//
//            // Check if there is another Floor - East
//            if (eastString == ".") {
//                float x3 = (GLOBAL_MAP_OFFSET) * (i);
//                float z3 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
//                createWall(i, j, x3, z3, false, "E");
//            }
//
//            // Check if there is another Floor - South
//            if (southString == ".") {
//                float x4 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
//                float z4 = (GLOBAL_MAP_OFFSET / 2) * (i);
//                createWall(i, j, x4, z4, true, "S");
//            }
//
//        }
//        else {
//            float x1 = (GLOBAL_MAP_OFFSET / 2) + GLOBAL_MAP_OFFSET * (j - 1);
//            float z1 = (GLOBAL_MAP_OFFSET) * (i - 1);
//            createWall(i, j, x1, z1, true, "N");
//
//            // Check if there is another Floor - West
//            if (westString == ".") {
//                float x2 = (GLOBAL_MAP_OFFSET) * (i);
//                float z2 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
//                createWall(i, j, x2, z2, false, "W");
//            }
//
//            // Check if there is another Floor - East
//            if (eastString == ".") {
//                float x3 = (GLOBAL_MAP_OFFSET) * (j);
//                float z3 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
//                createWall(i, j, x3, z3, false, "E");
//            }
//
//            // Check if there is another Floor - South
//            if (southString == ".") {
//                float x4 = (GLOBAL_MAP_OFFSET / 2) + GLOBAL_MAP_OFFSET * (j - 1);
//                float z4 = GLOBAL_MAP_OFFSET * i;
//                createWall(i, j, x4, z4, true, "S");
//            }
//        }
//    }
//    else {
//        if (j == 0) {
//            // Check if there is another Floor - North
//            if (northString == ".") {
//                float x1 = (GLOBAL_MAP_OFFSET / 2) * (j - 1);
//                float z1 = (GLOBAL_MAP_OFFSET) * (i - 1);
//                createWall(i, j, x1, z1, true, "N");
//            }
//
//            float x2 = (GLOBAL_MAP_OFFSET) * (j - 1);
//            float z2 = (GLOBAL_MAP_OFFSET / 2) + GLOBAL_MAP_OFFSET * (i - 1);
//            createWall(i, j, x2, z2, false, "W");
//
//            // Check if there is another Floor - East
//            if (eastString == ".") {
//                float x3 = (GLOBAL_MAP_OFFSET) * (j);
//                float z3 = (GLOBAL_MAP_OFFSET / 2) + GLOBAL_MAP_OFFSET * (i - 1);
//                createWall(i, j, x3, z3, false, "E");
//            }
//
//            // Check if there is another Floor - South
//            if (southString == ".") {
//                float x4 = (GLOBAL_MAP_OFFSET / 2) * (j - 1);
//                float z4 = (GLOBAL_MAP_OFFSET) * (i);
//                createWall(i, j, x4, z4, true, "S");
//            }
//        }
//        else {
//            // Check if there is another Floor - North
//            if (northString == ".") {
//                float x1 = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (j - 1);
//                float z1 = (GLOBAL_MAP_OFFSET) * (i - 1);
//                createWall(i, j, x1, z1, true, "N");
//            }
//
//            // Check if there is another Floor - West
//            if (westString == ".") {
//                float x2 = (GLOBAL_MAP_OFFSET) * (j - 1);
//                float z2 = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (i - 1);
//                createWall(i, j, x2, z2, false, "W");
//            }
//
//            // Check if there is another Floor - East
//            if (eastString == ".") {
//                float x3 = (GLOBAL_MAP_OFFSET) * (j);
//                float z3 = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (i - 1);
//                createWall(i, j, x3, z3, false, "E");
//            }
//
//            // Check if there is another Floor - South
//            if (southString == ".") {
//                float x4 = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (j - 1);
//                float z4 = (GLOBAL_MAP_OFFSET) * (i);
//                createWall(i, j, x4, z4, true, "S");
//            }
//        }
//    }
//
//    // Lock the mutex to prevent other threads from accessing the vector
//    EnterCriticalSection(&dataMutex);
//    // Add a new value to the vector
//    g_GraphicScene.vec_pMeshCurrentMaze.push_back(currentObject);
//    // Unlock the mutex to allow other threads to access the vector
//    LeaveCriticalSection(&dataMutex);
//    // Signal the semaphore to indicate that a thread has finished generating data
//    ReleaseSemaphore(dataSemaphore, 1, NULL);
//
//    return 0;
//}

void createMazeTile(int i, int j) {
    sModelDrawInfo drawingInformation;
    std::string northString = "";
    std::string westString = "";
    std::string eastString = "";
    std::string southString = "";

    if (i > 0) northString = m_blocksLoader->g_blockMap->at(i - 1).at(j);
    if (j > 0) westString = m_blocksLoader->g_blockMap->at(i).at(j - 1);
    if (j < m_blocksLoader->g_blockMap->at(i).size()) eastString = m_blocksLoader->g_blockMap->at(i).at(j + 1);
    if (i < m_blocksLoader->g_blockMap->size()) southString = m_blocksLoader->g_blockMap->at(i + 1).at(j);

    float x = (j * GLOBAL_MAP_OFFSET);
    float z = (i * GLOBAL_MAP_OFFSET);
    //pVAOManager->FindDrawInfoByModelName("Floor", drawingInformation);
    drawingInformation = g_GraphicScene.returnDrawInformation("Floor");
    cMeshObject* currentObject = g_GraphicScene.CreateGameObjectByType("Floor", glm::vec3(x, 0.0f, z), drawingInformation);
    //currentObject = g_GraphicScene.GetObjectByName("Floor", false);
    std::string floorName = "Floor" + std::to_string(i) + "_" + std::to_string(j);
    currentObject->currentI = i;
    currentObject->currentJ = j;
    currentObject->friendlyName = floorName;
    currentObject->textures[0] = "Dungeons_2_Texture_01_A.bmp";
    currentObject->textureRatios[0] = 1.0f;

    // Creating some Walls! Depending on the adjacents tiles
    if (i == 0) {
        if (j == 0) {
            float x1 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
            float z1 = (GLOBAL_MAP_OFFSET) * (j - 1);
            createWall(i, j, x1, z1, true, "N");

            float x2 = (GLOBAL_MAP_OFFSET) * (i - 1);
            float z2 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
            createWall(i, j, x2, z2, false, "W");

            // Check if there is another Floor - East
            if (eastString == ".") {
                float x3 = (GLOBAL_MAP_OFFSET) * (i);
                float z3 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
                createWall(i, j, x3, z3, false, "E");
            }

            // Check if there is another Floor - South
            if (southString == ".") {
                float x4 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
                float z4 = (GLOBAL_MAP_OFFSET / 2) * (i);
                createWall(i, j, x4, z4, true, "S");
            }

        }
        else {
            float x1 = (GLOBAL_MAP_OFFSET / 2) + GLOBAL_MAP_OFFSET * (j - 1);
            float z1 = (GLOBAL_MAP_OFFSET) * (i - 1);
            createWall(i, j, x1, z1, true, "N");

            // Check if there is another Floor - West
            if (westString == ".") {
                float x2 = (GLOBAL_MAP_OFFSET) * (i);
                float z2 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
                createWall(i, j, x2, z2, false, "W");
            }

            // Check if there is another Floor - East
            if (eastString == ".") {
                float x3 = (GLOBAL_MAP_OFFSET) * (j);
                float z3 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
                createWall(i, j, x3, z3, false, "E");
            }

            // Check if there is another Floor - South
            if (southString == ".") {
                float x4 = (GLOBAL_MAP_OFFSET / 2) + GLOBAL_MAP_OFFSET * (j - 1);
                float z4 = GLOBAL_MAP_OFFSET * i;
                createWall(i, j, x4, z4, true, "S");
            }
        }
    }
    else {
        if (j == 0) {
            // Check if there is another Floor - North
            if (northString == ".") {
                float x1 = (GLOBAL_MAP_OFFSET / 2) * (j - 1);
                float z1 = (GLOBAL_MAP_OFFSET) * (i - 1);
                createWall(i, j, x1, z1, true, "N");
            }

            float x2 = (GLOBAL_MAP_OFFSET) * (j - 1);
            float z2 = (GLOBAL_MAP_OFFSET / 2) + GLOBAL_MAP_OFFSET * (i - 1);
            createWall(i, j, x2, z2, false, "W");

            // Check if there is another Floor - East
            if (eastString == ".") {
                float x3 = (GLOBAL_MAP_OFFSET) * (j);
                float z3 = (GLOBAL_MAP_OFFSET / 2) + GLOBAL_MAP_OFFSET * (i - 1);
                createWall(i, j, x3, z3, false, "E");
            }

            // Check if there is another Floor - South
            if (southString == ".") {
                float x4 = (GLOBAL_MAP_OFFSET / 2) * (j - 1);
                float z4 = (GLOBAL_MAP_OFFSET) * (i);
                createWall(i, j, x4, z4, true, "S");
            }
        }
        else {
            // Check if there is another Floor - North
            if (northString == ".") {
                float x1 = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (j - 1);
                float z1 = (GLOBAL_MAP_OFFSET) * (i - 1);
                createWall(i, j, x1, z1, true, "N");
            }

            // Check if there is another Floor - West
            if (westString == ".") {
                float x2 = (GLOBAL_MAP_OFFSET) * (j - 1);
                float z2 = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (i - 1);
                createWall(i, j, x2, z2, false, "W");
            }

            // Check if there is another Floor - East
            if (eastString == ".") {
                float x3 = (GLOBAL_MAP_OFFSET) * (j);
                float z3 = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (i - 1);
                createWall(i, j, x3, z3, false, "E");
            }

            // Check if there is another Floor - South
            if (southString == ".") {
                float x4 = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (j - 1);
                float z4 = (GLOBAL_MAP_OFFSET) * (i);
                createWall(i, j, x4, z4, true, "S");
            }
        }
    }

    g_GraphicScene.vec_pMeshCurrentMaze.push_back(currentObject);
}

/* Function that create all the objects for the game.
        The function reads the blockmap and create objects based on what information it has in each pointer
        Examples:
        . -> empty tile
        X -> floor empty tile 
        B -> Beholder on the floor
        C -> Crystal on the floor
        D -> Deadbody on the floor

        The function also creates the walls in all four directions depending on what are the adjacents tiles.
        Example: If the north tile is empty (.), creates a wall object in the north direction of the tile with the right rotation. */
void creatingModels() {
    sModelDrawInfo drawingInformation; 
    //pVAOManager->FindDrawInfoByModelName("Moon", drawingInformation);
    drawingInformation = g_GraphicScene.returnDrawInformation("Moon");
    g_GraphicScene.CreateGameObjectByType("Moon", glm::vec3(0.0f, 100.0f, 0.0f), drawingInformation);
    cMeshObject* moon;
    moon = g_GraphicScene.GetObjectByName("Moon", false);
    moon->bDoNotLight = true;
    moon->SetUniformScale(100.f);
    moon->textures[0] = "lroc_color_poles_4k.bmp";
    moon->textureRatios[0] = 1.0f;

    for (int i = 0; i < m_blocksLoader->g_blockMap->size(); i++) {
        for (int j = 0; j < m_blocksLoader->g_blockMap->at(i).size(); j++) {
            std::string currentString = m_blocksLoader->g_blockMap->at(i).at(j);
            if (currentString != "") {
                if (currentString != ".") {
                    std::string northString = "";
                    std::string westString = "";
                    std::string eastString = "";
                    std::string southString = "";

                    if (i > 0) northString = m_blocksLoader->g_blockMap->at(i - 1).at(j);
                    if (j > 0) westString = m_blocksLoader->g_blockMap->at(i).at(j - 1);
                    if (j < m_blocksLoader->g_blockMap->at(i).size()) eastString = m_blocksLoader->g_blockMap->at(i).at(j + 1);
                    if (i < m_blocksLoader->g_blockMap->size()) southString = m_blocksLoader->g_blockMap->at(i + 1).at(j);
                    
                    float x = (j * GLOBAL_MAP_OFFSET);
                    float z = (i * GLOBAL_MAP_OFFSET);
                    //pVAOManager->FindDrawInfoByModelName("Floor", drawingInformation);
                    drawingInformation = g_GraphicScene.returnDrawInformation("Floor");
                    g_GraphicScene.CreateGameObjectByType("Floor", glm::vec3(x, 0.0f, z), drawingInformation);
                    cMeshObject* currentObject;
                    currentObject = g_GraphicScene.GetObjectByName("Floor", false);
                    std::string floorName = "Floor" + std::to_string(i) + "_" + std::to_string(j);
                    currentObject->currentI = i;
                    currentObject->currentJ = j;
                    currentObject->friendlyName = floorName;
                    currentObject->textures[0] = "Dungeons_2_Texture_01_A.bmp";
                    currentObject->textureRatios[0] = 1.0f;

                    // Creating some Torches!
                    if (currentString == "T") {
                        glm::vec3 torchN = glm::vec3(0.0f);
                        glm::vec3 torchW = glm::vec3(0.0f);
                        glm::vec3 torchE = glm::vec3(0.0f);
                        glm::vec3 torchS = glm::vec3(0.0f);

                        // Check if there is another Floor - North
                        if (northString == ".") {
                            torchN.x = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (j - 1);
                            torchN.y = 25.0f;
                            torchN.z = (GLOBAL_MAP_OFFSET) * (i - 1);
                        }

                        // Check if there is another Floor - West
                        if (westString == ".") {
                            torchW.x = (GLOBAL_MAP_OFFSET) * (j - 1);
                            torchW.y = 25.0f;
                            torchW.z = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (i - 1);
                        }

                        // Check if there is another Floor - East
                        if (eastString == ".") {
                            torchE.x = (GLOBAL_MAP_OFFSET) * (j);
                            torchE.y = 25.0f;
                            torchE.z = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (i - 1);
                        }

                        // Check if there is another Floor - South
                        if (southString == ".") {
                            torchS.x = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (j - 1);
                            torchS.y = 25.0f;
                            torchS.z = (GLOBAL_MAP_OFFSET) * (i);
                        }

                        bool validTorch = false;
                        GLuint shaderID = 0;
                        //shaderID = pTheShaderManager->getIDFromFriendlyName("Shader_1");
                        shaderID = g_GraphicScene.returnShaderID("Shader_1");
                        do {
                            unsigned int randTorch = rand() % 4;
                            switch (randTorch) {
                            case 0:
                                // North
                                if (torchN != glm::vec3(0.0f)) {
                                    createTorch(i, j, torchN, glm::vec3(0.0f, (NinetyDegrees * 2), 0.0f), "N");
                                    // Creating the spot light too?
                                    ::g_pTheLightManager->CreateBasicPointLight(shaderID, glm::vec4(torchN.x, torchN.y + 10, torchN.z + 10, 0.0f));
                                    validTorch = true;
                                }
                                break;
                            case 1:
                                // West
                                if (torchW != glm::vec3(0.0f)) {
                                    createTorch(i, j, torchW, glm::vec3(0.0f, (NinetyDegrees * -1), 0.0f), "W");
                                    ::g_pTheLightManager->CreateBasicPointLight(shaderID, glm::vec4(torchW.x + 10, torchW.y + 10, torchW.z, 0.0f));
                                    validTorch = true;
                                }
                                break;
                            case 2:
                                // East
                                if (torchE != glm::vec3(0.0f)) {
                                    createTorch(i, j, torchE, glm::vec3(0.0f, (NinetyDegrees), 0.0f), "E");
                                    ::g_pTheLightManager->CreateBasicPointLight(shaderID, glm::vec4(torchE.x - 10, torchE.y + 10, torchE.z, 0.0f));
                                    validTorch = true;
                                }
                                break;
                            case 3:
                                // South
                                if (torchS != glm::vec3(0.0f)) {
                                    createTorch(i, j, torchS, glm::vec3(0.0f), "S");
                                    ::g_pTheLightManager->CreateBasicPointLight(shaderID, glm::vec4(torchS.x, torchS.y + 10, torchS.z - 10, 0.0f));
                                    validTorch = true;
                                }
                                break;
                            }

                        } while (!validTorch);

                        
                    }

                    // Creating some DeadBodies =0
                    if (currentString == "D") {
                        glm::vec3 deadbodyN = glm::vec3(0.0f);
                        glm::vec3 deadbodyW = glm::vec3(0.0f);
                        glm::vec3 deadbodyE = glm::vec3(0.0f);
                        glm::vec3 deadbodyS = glm::vec3(0.0f);

                        // Check if there is another Floor - North
                        if (northString == ".") {
                            deadbodyN.x = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (j - 1);
                            deadbodyN.y = 0.0f;
                            deadbodyN.z = (GLOBAL_MAP_OFFSET) * (i - 1) + 10;
                        }

                        // Check if there is another Floor - West
                        if (westString == ".") {
                            deadbodyW.x = (GLOBAL_MAP_OFFSET) * (j - 1) + 10;
                            deadbodyW.y = 0.0f;
                            deadbodyW.z = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (i - 1);
                        }

                        // Check if there is another Floor - East
                        if (eastString == ".") {
                            deadbodyE.x = (GLOBAL_MAP_OFFSET) * (j) - 10;
                            deadbodyE.y = 0.0f;
                            deadbodyE.z = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (i - 1);
                        }

                        // Check if there is another Floor - South
                        if (southString == ".") {
                            deadbodyS.x = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (j - 1);
                            deadbodyS.y = 0.0f;
                            deadbodyS.z = (GLOBAL_MAP_OFFSET) * (i) - 10;
                        }

                        bool validDeadbody = false;
                        GLuint shaderID = 0;
                        //shaderID = pTheShaderManager->getIDFromFriendlyName("Shader_1");
                        shaderID = g_GraphicScene.returnShaderID("Shader_1");
                        do {
                            unsigned int randTorch = rand() % 4;
                            switch (randTorch) {
                            case 0:
                                // North
                                if (deadbodyN != glm::vec3(0.0f)) {
                                    createDeadbody(i, j, deadbodyN, glm::vec3(0.0f, (NinetyDegrees), 0.0f), "N");
                                    // Creating the spot light too?
                                    validDeadbody = true;
                                }
                                break;
                            case 1:
                                // West
                                if (deadbodyW != glm::vec3(0.0f)) {
                                    createDeadbody(i, j, deadbodyW, glm::vec3(0.0f, (NinetyDegrees * 2), 0.0f), "W");
                                    validDeadbody = true;
                                }
                                break;
                            case 2:
                                // East
                                if (deadbodyE != glm::vec3(0.0f)) {
                                    createDeadbody(i, j, deadbodyE, glm::vec3(0.0f), "E");
                                    validDeadbody = true;
                                }
                                break;
                            case 3:
                                // South
                                if (deadbodyS != glm::vec3(0.0f)) {
                                    createDeadbody(i, j, deadbodyS, glm::vec3(0.0f, (NinetyDegrees * -1), 0.0f), "S");
                                    validDeadbody = true;
                                }
                                break;
                            }

                        } while (!validDeadbody);
                    }

                    // Creating Crystals Clusters (center of tile)
                    if (currentString == "C") {
                        //pVAOManager->FindDrawInfoByModelName("Crystal_Cluster1", drawingInformation);
                        drawingInformation = g_GraphicScene.returnDrawInformation("Crystal_Cluster1");
                        g_GraphicScene.CreateGameObjectByType("Crystal_Cluster1", glm::vec3(x - (GLOBAL_MAP_OFFSET / 2), 10.0f, z - (GLOBAL_MAP_OFFSET / 2)), drawingInformation);
                        cMeshObject* crystalC;
                        crystalC = g_GraphicScene.GetObjectByName("Crystal_Cluster1", false);
                        std::string crystalCName = "Crystal_Cluster1" + std::to_string(i) + "_" + std::to_string(j);
                        crystalC->friendlyName = crystalCName;
                        crystalC->bUse_RGBA_colour = true;
                        crystalC->RGBA_colour = glm::vec4(0.4f, 1.0f, 0.8f, 0.75f);
                        crystalC->specular_colour_and_power = glm::vec4(0.4f, 1.0f, 0.8f, 0.75f);
                        crystalC->SetUniformScale(RandomFloat(0.05f, 0.10f));
                        crystalC->setRotationFromEuler(glm::vec3(0.0f, rand() % 180, 0.0f));
                    }

                    // Creating Crystals Clusters (center of tile)
                    if (currentString == "c") {
                        //pVAOManager->FindDrawInfoByModelName("Crystal_Cluster2", drawingInformation);
                        drawingInformation = g_GraphicScene.returnDrawInformation("Crystal_Cluster2");
                        g_GraphicScene.CreateGameObjectByType("Crystal_Cluster2", glm::vec3(x - (GLOBAL_MAP_OFFSET / 2), 10.0f, z - (GLOBAL_MAP_OFFSET / 2)), drawingInformation);
                        cMeshObject* crystalC;
                        crystalC = g_GraphicScene.GetObjectByName("Crystal_Cluster2", false);
                        std::string crystalCName = "Crystal_Cluster2" + std::to_string(i) + "_" + std::to_string(j);
                        crystalC->friendlyName = crystalCName;
                        crystalC->bUse_RGBA_colour = true;
                        crystalC->RGBA_colour = glm::vec4(0.5f, 0.1f, 1.0f, 0.8f);
                        crystalC->specular_colour_and_power = glm::vec4(0.5f, 0.1f, 1.0f, 0.3f);
                        crystalC->SetUniformScale(RandomFloat(0.10f, 0.15f));
                        crystalC->setRotationFromEuler(glm::vec3(0.0f, rand() % 180, 0.0f));
                    }

                    // Creating Crystals Clusters (center of tile)
                    if (currentString == "Y") {
                        //pVAOManager->FindDrawInfoByModelName("Crystal_Cluster3", drawingInformation);
                        drawingInformation = g_GraphicScene.returnDrawInformation("Crystal_Cluster3");
                        g_GraphicScene.CreateGameObjectByType("Crystal_Cluster3", glm::vec3(x - (GLOBAL_MAP_OFFSET / 2), 10.0f, z - (GLOBAL_MAP_OFFSET / 2)), drawingInformation);
                        cMeshObject* crystalC;
                        crystalC = g_GraphicScene.GetObjectByName("Crystal_Cluster3", false);
                        std::string crystalCName = "Crystal_Cluster3" + std::to_string(i) + "_" + std::to_string(j);
                        crystalC->friendlyName = crystalCName;
                        crystalC->bUse_RGBA_colour = true;
                        crystalC->RGBA_colour = glm::vec4(0.0f, 0.2f, 1.0f, 0.65f);
                        crystalC->specular_colour_and_power = glm::vec4(0.0f, 0.2f, 1.0f, 0.65f);
                        crystalC->SetUniformScale(RandomFloat(0.10f, 0.15f));
                        crystalC->setRotationFromEuler(glm::vec3(0.0f, rand() % 180, 0.0f));
                    }

                    // Creating the mighty BEHOLDER!
                    if (currentString == "b") {
                        //pVAOManager->FindDrawInfoByModelName("Beholder", drawingInformation);
                        drawingInformation = g_GraphicScene.returnDrawInformation("Beholder");
                        g_GraphicScene.CreateGameObjectByType("Beholder", glm::vec3(x - (GLOBAL_MAP_OFFSET / 2), 25.0f, z - (GLOBAL_MAP_OFFSET / 2)), drawingInformation);
                        cMeshObject* beholder;
                        beholder = g_GraphicScene.GetObjectByName("Beholder", false);
                        std::string beholderName = "B" + std::to_string(i) + "_" + std::to_string(j);
                        beholder->friendlyName = beholderName;
                        beholder->textures[0] = "Beholder_Base_color.bmp";
                        beholder->textureRatios[0] = 1.0f;
                        beholder->SetUniformScale(10.0f);

                        cMeshObject* cone = new cMeshObject();
                        cone->meshName = "Beholder_Vision";
                        cone->friendlyName = "Beholder_Vision" + std::to_string(i) + "_" + std::to_string(j);
                        cone->bUse_RGBA_colour = true;
                        cone->RGBA_colour = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
                        cone->bDoNotLight = true;
                        beholder->vecChildMeshes.push_back(cone);

                        beholder->currentI = i;
                        beholder->currentJ = j;

                        g_GraphicScene.map_beholds->try_emplace(beholderName, beholder);
                    }

                    // Creating some Walls! Depending on the adjacents tiles
                    if (i == 0) {
                        if (j == 0) {
                            float x1 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
                            float z1 = (GLOBAL_MAP_OFFSET) * (j - 1);
                            createWall(i, j, x1, z1, true, "N");

                            float x2 = (GLOBAL_MAP_OFFSET) * (i - 1);
                            float z2 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
                            createWall(i, j, x2, z2, false, "W");

                            // Check if there is another Floor - East
                            if (eastString == ".") {
                                float x3 = (GLOBAL_MAP_OFFSET) * (i);
                                float z3 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
                                createWall(i, j, x3, z3, false, "E");
                            }

                            // Check if there is another Floor - South
                            if (southString == ".") {
                                float x4 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
                                float z4 = (GLOBAL_MAP_OFFSET / 2) * (i);
                                createWall(i, j, x4, z4, true, "S");
                            }

                        }
                        else {
                            float x1 = (GLOBAL_MAP_OFFSET / 2) + GLOBAL_MAP_OFFSET * (j - 1);
                            float z1 = (GLOBAL_MAP_OFFSET) * (i - 1);
                            createWall(i, j, x1, z1, true, "N");

                            // Check if there is another Floor - West
                            if (westString == ".") {
                                float x2 = (GLOBAL_MAP_OFFSET) * (i);
                                float z2 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
                                createWall(i, j, x2, z2, false, "W");
                            }

                            // Check if there is another Floor - East
                            if (eastString == ".") {
                                float x3 = (GLOBAL_MAP_OFFSET) * (j);
                                float z3 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
                                createWall(i, j, x3, z3, false, "E");
                            }

                            // Check if there is another Floor - South
                            if (southString == ".") {
                                float x4 = (GLOBAL_MAP_OFFSET / 2) + GLOBAL_MAP_OFFSET * (j - 1);
                                float z4 = GLOBAL_MAP_OFFSET * i;
                                createWall(i, j, x4, z4, true, "S");
                            }
                        }
                    }
                    else {
                        if (j == 0) {
                            // Check if there is another Floor - North
                            if (northString == ".") {
                                float x1 = (GLOBAL_MAP_OFFSET / 2) * (j - 1);
                                float z1 = (GLOBAL_MAP_OFFSET) * (i - 1);
                                createWall(i, j, x1, z1, true, "N");
                            }

                            float x2 = (GLOBAL_MAP_OFFSET) * (j - 1);
                            float z2 = (GLOBAL_MAP_OFFSET / 2) + GLOBAL_MAP_OFFSET * (i - 1);
                            createWall(i, j, x2, z2, false, "W");

                            // Check if there is another Floor - East
                            if (eastString == ".") {
                                float x3 = (GLOBAL_MAP_OFFSET) * (j);
                                float z3 = (GLOBAL_MAP_OFFSET / 2) + GLOBAL_MAP_OFFSET * (i - 1);
                                createWall(i, j, x3, z3, false, "E");
                            }

                            // Check if there is another Floor - South
                            if (southString == ".") {
                                float x4 = (GLOBAL_MAP_OFFSET / 2) * (j - 1);
                                float z4 = (GLOBAL_MAP_OFFSET) * (i);
                                createWall(i, j, x4, z4, true, "S");
                            }
                        }
                        else {
                            // Check if there is another Floor - North
                            if (northString == ".") {
                                float x1 = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (j - 1);
                                float z1 = (GLOBAL_MAP_OFFSET) * (i - 1);
                                createWall(i, j, x1, z1, true, "N");
                            }

                            // Check if there is another Floor - West
                            if (westString == ".") {
                                float x2 = (GLOBAL_MAP_OFFSET) * (j - 1);
                                float z2 = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (i - 1);
                                createWall(i, j, x2, z2, false, "W");
                            }

                            // Check if there is another Floor - East
                            if (eastString == ".") {
                                float x3 = (GLOBAL_MAP_OFFSET) * (j);
                                float z3 = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (i - 1);
                                createWall(i, j, x3, z3, false, "E");
                            }

                            // Check if there is another Floor - South
                            if (southString == ".") {
                                float x4 = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (j - 1);
                                float z4 = (GLOBAL_MAP_OFFSET) * (i);
                                createWall(i, j, x4, z4, true, "S");
                            }
                        }
                    }
                }
            }
        }
    } 

}

// Functions that creates the debug objets for the lights
void debugLightSpheres() {
    pDebugSphere_1 = new cMeshObject();
    pDebugSphere_1->meshName = "ISO_Sphere_1";
    pDebugSphere_1->friendlyName = "Debug Sphere 1";
    pDebugSphere_1->bUse_RGBA_colour = true;
    pDebugSphere_1->RGBA_colour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    pDebugSphere_1->isWireframe = true;
    pDebugSphere_1->scale = 1.0f;
    pDebugSphere_1->bDoNotLight = true;
    g_GraphicScene.vec_pMeshObjects.push_back(pDebugSphere_1);

    pDebugSphere_2 = new cMeshObject();
    pDebugSphere_2->meshName = "ISO_Sphere_1";
    pDebugSphere_2->friendlyName = "Debug Sphere 2";
    pDebugSphere_2->bUse_RGBA_colour = true;
    pDebugSphere_2->RGBA_colour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    pDebugSphere_2->isWireframe = true;
    pDebugSphere_2->scale = 1.0f;
    pDebugSphere_2->bDoNotLight = true;
    g_GraphicScene.vec_pMeshObjects.push_back(pDebugSphere_2);

    pDebugSphere_3 = new cMeshObject();
    pDebugSphere_3->meshName = "ISO_Sphere_1";
    pDebugSphere_3->friendlyName = "Debug Sphere 3";
    pDebugSphere_3->bUse_RGBA_colour = true;
    pDebugSphere_3->RGBA_colour = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    pDebugSphere_3->isWireframe = true;
    pDebugSphere_3->scale = 1.0f;
    pDebugSphere_3->bDoNotLight = true;
    g_GraphicScene.vec_pMeshObjects.push_back(pDebugSphere_3);

    pDebugSphere_4 = new cMeshObject();
    pDebugSphere_4->meshName = "ISO_Sphere_1";
    pDebugSphere_4->friendlyName = "Debug Sphere 4";
    pDebugSphere_4->bUse_RGBA_colour = true;
    pDebugSphere_4->RGBA_colour = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
    pDebugSphere_4->isWireframe = true;
    pDebugSphere_4->scale = 1.0f;
    pDebugSphere_4->bDoNotLight = true;
    g_GraphicScene.vec_pMeshObjects.push_back(pDebugSphere_4);

    pDebugSphere_5 = new cMeshObject();
    pDebugSphere_5->meshName = "ISO_Sphere_1";
    pDebugSphere_5->friendlyName = "Debug Sphere 5";
    pDebugSphere_5->bUse_RGBA_colour = true;
    pDebugSphere_5->RGBA_colour = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
    pDebugSphere_5->isWireframe = true;
    pDebugSphere_5->scale = 1.0f;
    pDebugSphere_5->bDoNotLight = true;
    g_GraphicScene.vec_pMeshObjects.push_back(pDebugSphere_5);
}

// Calculate triangles center of the cMeshObject in parameter
void calculateTrianglesCenter(cMeshObject* obj) {
    g_GraphicScene.trianglesCenter.reserve(obj->numberOfTriangles);
    sModelDrawInfo drawingInformation;
    //pVAOManager->FindDrawInfoByModelName(obj->meshName, drawingInformation);
    drawingInformation = g_GraphicScene.returnDrawInformation(obj->meshName);

    for (int i = 0; i < obj->numberOfTriangles; i++) {        
        glm::vec3 triangleCenter1;
        glm::vec3 triangleCenter2;
        /*if ((drawingInformation.pVertices[(int)obj->meshTriangles[i].x].y ==
            drawingInformation.pVertices[(int)obj->meshTriangles[i].y].y) &&
            (drawingInformation.pVertices[(int)obj->meshTriangles[i].x].y ==
            drawingInformation.pVertices[(int)obj->meshTriangles[i].z].y)) {*/
            
            triangleCenter1.x = (
                drawingInformation.pVertices[(int)obj->meshTriangles[i].x].x +
                drawingInformation.pVertices[(int)obj->meshTriangles[i].y].x +
                drawingInformation.pVertices[(int)obj->meshTriangles[i].z].x) / 3;

            //triangleCenter1.y = drawingInformation.pVertices[(int)obj->meshTriangles[i].x].y;

            triangleCenter1.y = (
                drawingInformation.pVertices[(int)obj->meshTriangles[i].x].y +
                drawingInformation.pVertices[(int)obj->meshTriangles[i].y].y +
                drawingInformation.pVertices[(int)obj->meshTriangles[i].z].y) / 3;

            triangleCenter1.z = (
                drawingInformation.pVertices[(int)obj->meshTriangles[i].x].z +
                drawingInformation.pVertices[(int)obj->meshTriangles[i].y].z +
                drawingInformation.pVertices[(int)obj->meshTriangles[i].z].z) / 3;
        /*}*/

        g_GraphicScene.trianglesCenter.push_back(triangleCenter1);
        obj->trianglesCenter.push_back(triangleCenter1);
    }
}

bool checkNorth(float& newPosition, int& newI, int& newJ, int& numTiles) {
    int i = newI;
    int j = newJ;

    std::string northString;
    if (i > 0) {
        northString = m_blocksLoader->g_blockMap->at(i - 1).at(j);
    }
    else {
        return false;
    }

    if (northString == "X" || northString == "D" || northString == "T" ||
        northString == "b" || northString == "B") {
        newI = i - 1;
        newPosition -= 50;
        numTiles++;
        checkNorth(newPosition, newI, newJ, numTiles);
        return true;
    }
    else {
        return false;
    }
}

bool checkWest(float& newPosition, int& newI, int& newJ, int& numTiles) {
    int i = newI;
    int j = newJ;

    std::string westString;
    if (j > 0) {
        westString = m_blocksLoader->g_blockMap->at(i).at(j - 1);
    }
    else {
        return false;
    }

    if (westString == "X" || westString == "D" || westString == "T" ||
        westString == "b" || westString == "B") {
        newJ = j - 1;
        newPosition -= 50;
        numTiles++;
        checkWest(newPosition, newI, newJ, numTiles);
        return true;
    }
    else {
        return false;
    }
}

bool checkEast(float& newPosition, int& newI, int& newJ, int& numTiles) {
    int i = newI;
    int j = newJ;

    std::string eastString;
    if (j < m_blocksLoader->g_blockMap->at(i).size()) {
        eastString = m_blocksLoader->g_blockMap->at(i).at(j + 1);
    }
    else {
        return false;
    }

    if (eastString == "X" || eastString == "D" || eastString == "T" ||
        eastString == "b" || eastString == "B") {
        newJ = j + 1;
        newPosition += 50;
        numTiles++;
        checkEast(newPosition, newI, newJ, numTiles);
        return true;
    }
    else {
        return false;
    }
}

bool checkSouth(float& newPosition, int& newI, int& newJ, int& numTiles) {
    int i = newI;
    int j = newJ;

    std::string southString;
    if (i < m_blocksLoader->g_blockMap->size()) {
        southString = m_blocksLoader->g_blockMap->at(i + 1).at(j);
    }
    else {
        return false;
    }

    if (southString == "X" || southString == "D" || southString == "T" ||
        southString == "b" || southString == "B") {
        newI = i + 1;
        newPosition += 50;
        numTiles++;
        checkSouth(newPosition, newI, newJ, numTiles);
        return true;
    }
    else {
        return false;
    }
}

/* Function that calculat next posible position for the beholder based on the block map (map of tiles of the dungeon). 
Beholder can only move in 4 directions (north, west, east and south) and the function returns true if the nextPosition is valid 
returns false if next position or the current position is also a next or current position of other beholder. */
bool calculateNextPosition(cMeshObject* currentBehold, glm::vec3& nextPosition, int& numTiles) {

    std::string northString = "";
    std::string westString = "";
    std::string eastString = "";
    std::string southString = "";

    unsigned int i = currentBehold->currentI;
    unsigned int j = currentBehold->currentJ;

    if (i > 0) northString = m_blocksLoader->g_blockMap->at(i - 1).at(j);
    if (j > 0) westString = m_blocksLoader->g_blockMap->at(i).at(j - 1);
    if (j < m_blocksLoader->g_blockMap->at(i).size()) eastString = m_blocksLoader->g_blockMap->at(i).at(j + 1);
    if (i < m_blocksLoader->g_blockMap->size()) southString = m_blocksLoader->g_blockMap->at(i + 1).at(j);

    glm::vec3 returnVec;
    returnVec = currentBehold->position;

    for (std::map< std::string, cMeshObject*>::iterator itBeholds =
        g_GraphicScene.map_beholds->begin(); itBeholds != g_GraphicScene.map_beholds->end();
        itBeholds++) {
        cMeshObject* cmBehold = itBeholds->second;
        unsigned int cmBeholdID = cmBehold->getID();

        if (cmBeholdID == currentBehold->getID()) continue;

        // N = i - 1 and j
        // NE = i - 1 and j + 1
        // E = i and j + 1
        // SE = i + 1 and j + 1
        // S = i + 1 and j
        // SW = j + 1 and j - 1
        // W = i and j - 1
        // NW = i - 1 and j - 1
        std::string cmBeholdIJString = std::to_string(cmBehold->currentI) + "0" + std::to_string(cmBehold->currentJ);

        std::string N = std::to_string(i - 1) + "0" + std::to_string(j);
        std::string NE = std::to_string(i - 1) + "0" + std::to_string(j + 1);
        std::string E = std::to_string(i) + "0" + std::to_string(j + 1);
        std::string SE = std::to_string(i + 1) + "0" + std::to_string(j + 1);
        std::string S = std::to_string(i + 1) + "0" + std::to_string(j);
        std::string SW = std::to_string(i + 1) + "0" + std::to_string(j - 1);
        std::string W = std::to_string(i) + "0" + std::to_string(j - 1);
        std::string NW = std::to_string(i - 1) + "0" + std::to_string(j - 1);

        if (cmBeholdIJString == N || cmBeholdIJString == NE || cmBeholdIJString == E || cmBeholdIJString == SE ||
            cmBeholdIJString == S || cmBeholdIJString == SW || cmBeholdIJString == W || cmBeholdIJString == NW) {
            // ATTACK!
            return false;
        }
    }

    bool validPath = false;
    int newI = i;
    int newJ = j;
    float newPosition;
    do {
        unsigned int randPath = rand() % 4;
        switch (randPath) {
        case 0:
            // North
            //if (northString == "X" || northString == "D" || northString == "T" ||
            //    northString == "b" || northString == "B") {
            //    returnVec.z -= 50.0f;
            //    currentBehold->currentI -= 1;
            //    validPath = true;
            //}
            newPosition = returnVec.z;
            if (checkNorth(newPosition, newI, newJ, numTiles)) {
                returnVec.z = newPosition;
                currentBehold->currentI = newI;
                validPath = true;
            }
            break;
        case 1:
            // West
            //if (westString == "X" || westString == "D" || westString == "T" ||
            //    westString == "b" || westString == "B") {
            //    returnVec.x -= 50.0f;
            //    currentBehold->currentJ -= 1;
            //    validPath = true;
            //}
            newPosition = returnVec.x;
            if (checkWest(newPosition, newI, newJ, numTiles)) {
                returnVec.x = newPosition;
                currentBehold->currentJ = newJ;
                validPath = true;
            }
            break;
        case 2:
            // East
            //if (eastString == "X" || eastString == "D" || eastString == "T" ||
            //    eastString == "b" || eastString == "B") {
            //    returnVec.x += 50.0f;
            //    currentBehold->currentJ += 1;
            //    validPath = true;
            //}
            newPosition = returnVec.x;
            if (checkEast(newPosition, newI, newJ, numTiles)) {
                returnVec.x = newPosition;
                currentBehold->currentJ = newJ;
                validPath = true;
            }
            break;
        case 3:
            // South
            //if (southString == "X" || southString == "D" || southString == "T" ||
            //    southString == "b" || southString == "B") {
            //    returnVec.z += 50.0f;
            //    currentBehold->currentI += 1;
            //    validPath = true;
            //}
            newPosition = returnVec.z;
            if (checkSouth(newPosition, newI, newJ, numTiles)) {
                returnVec.z = newPosition;
                currentBehold->currentI = newI;
                validPath = true;
            }
            break;
        }

    } while (!validPath);

    nextPosition = returnVec;
    return true;
}

DWORD WINAPI animationUpdate(PVOID pvParam) {
    for (std::map< std::string, cMeshObject*>::iterator itBeholds =
        g_GraphicScene.map_beholds->begin(); itBeholds != g_GraphicScene.map_beholds->end();
        itBeholds++)
    {
        currentBehold = itBeholds->second;
        unsigned int currentBeholdID = currentBehold->getID();
        int numTilesNext = 0;

        if (currentBehold->moving == 0 && !currentBehold->dead) {
            glm::vec3 nextPosition = glm::vec3(0.0f);
            if (!calculateNextPosition(currentBehold, nextPosition, numTilesNext)) {
                // Spin and reduce;
                currentBehold->adjustRoationAngleFromEuler(glm::vec3(0.0f, 0.2f, 0.0f));
                currentBehold->reduceFromScale(0.99f);
                if (currentBehold->scaleXYZ.x <= 0.01) {
                    currentBehold->dead = true;
                    std::cout << currentBehold->friendlyName << " is Dead :(" << std::endl;
                }
                continue;
            }

            currentBehold->moving = 1;
            currentBehold->rotating = 1;
            currentBehold->PositionKeyFrames.clear();
            currentBehold->RotationKeyFrames.clear();

            //std::string currentBeholdNextTileXScript =
            //    "nextTileX" + std::to_string(currentBeholdID) + " = " +
            //    std::to_string(nextPosition.x);
            //std::string currentBeholdNextTileYScript =
            //    "nextTileY" + std::to_string(currentBeholdID) + " = " +
            //    std::to_string(nextPosition.y);
            //std::string currentBeholdNextTileZScript =
            //    "nextTileZ" + std::to_string(currentBeholdID) + "  = " +
            //    std::to_string(nextPosition.z);

            currRKF.value = currentBehold->rotation;
            currRKF.time = 0;
            currRKF.useSlerp = false;

            // Lock the mutex to prevent other threads from accessing the vector
            //EnterCriticalSection(&dataMutex);

            currentBehold->RotationKeyFrames.push_back(currRKF);
            
            // Unlock the mutex to allow other threads to access the vector
            //LeaveCriticalSection(&dataMutex);

            nextRKF.time = 1;
            nextRKF.useSlerp = false;

            // Setting rotation
            if (nextPosition.x < currentBehold->position.x) {// Going WEST
                //currentBehold->setRotationFromEuler(glm::vec3(0.0f, NinetyDegrees, 0.0f));
                nextRKF.value = glm::vec3(0.0f, NinetyDegrees, 0.0f);
            }

            if (nextPosition.x > currentBehold->position.x) {// Going EAST
                //currentBehold->setRotationFromEuler(glm::vec3(0.0f, -NinetyDegrees, 0.0f));
                nextRKF.value = glm::vec3(0.0f, -NinetyDegrees, 0.0f);
            }

            if (nextPosition.z < currentBehold->position.z) {// Going NORTH
                //currentBehold->setRotationFromEuler(glm::vec3(0.0f, 0.0, 0.0f));
                nextRKF.value = glm::vec3(0.0f, 0.0, 0.0f);
            }

            if (nextPosition.z > currentBehold->position.z) {// Going SOUTH
                //currentBehold->setRotationFromEuler(glm::vec3(0.0f, NinetyDegrees * 2, 0.0f));
                nextRKF.value = glm::vec3(0.0f, NinetyDegrees * 2, 0.0f);
            }

            // Lock the mutex to prevent other threads from accessing the vector
            //EnterCriticalSection(&dataMutex);

            currentBehold->RotationKeyFrames.push_back(nextRKF);

            // Unlock the mutex to allow other threads to access the vector
            //LeaveCriticalSection(&dataMutex);

            //std::cout << " ------------------------------------------------------ " << std::endl;
            //std::cout << currentBehold->friendlyName << " CurrentPosition: (" <<
            //    currentBehold->position.x << ", " <<
            //    currentBehold->position.y << ", " <<
            //    currentBehold->position.z << ")" << std::endl;

            //std::cout << currentBehold->friendlyName << " I -> " <<
            //    currentBehold->currentI << ", J -> " <<
            //    currentBehold->currentJ << std::endl;

            //std::cout << currentBehold->friendlyName << " NextPosition: (" <<
            //    nextPosition.x << ", " <<
            //    nextPosition.y << ", " <<
            //    nextPosition.z << ")" << std::endl;

            //std::cout << "nextTileX" << currentBeholdID << " changed" << std::endl;
            //std::cout << "nextTileY" << currentBeholdID << " changed" << std::endl;
            //std::cout << "nextTileZ" << currentBeholdID << " changed" << std::endl;

            currPKF.value = glm::vec3(currentBehold->position.x, currentBehold->position.y, currentBehold->position.z);
            currPKF.time = 0;

            // Lock the mutex to prevent other threads from accessing the vector
            //EnterCriticalSection(&dataMutex);

            currentBehold->PositionKeyFrames.push_back(currPKF);

            // Unlock the mutex to allow other threads to access the vector
            //LeaveCriticalSection(&dataMutex);

            nextPKF.value = glm::vec3(nextPosition.x, nextPosition.y, nextPosition.z);
            nextPKF.time = 1;

            // Lock the mutex to prevent other threads from accessing the vector
            //EnterCriticalSection(&dataMutex);

            currentBehold->PositionKeyFrames.push_back(nextPKF);

            // Unlock the mutex to allow other threads to access the vector
            //LeaveCriticalSection(&dataMutex);

            currentBehold->CurrentTime = 0.f;
            currentBehold->IsLooping = false;
            currentBehold->IsPlaying = true;
        }
        else if (currentBehold->moving == 2 && !currentBehold->dead && currentBehold->rotating == 0) {
            // Check if the animation is finished?
            currentBehold->UpdateAnimation(1);
            currentBehold->Speed = animationSpeed;

            glm::vec3 newPosition = currentBehold->GetAnimationPosition(currentBehold->CurrentTime, animationType);
            currentBehold->position.x = newPosition.x;
            currentBehold->position.z = newPosition.z;
        }
        else if (currentBehold->moving == 1 && !currentBehold->dead && currentBehold->rotating == 1) {
            currentBehold->UpdateAnimation(1);
            currentBehold->Speed = animationSpeed;

            glm::vec3 newRotation = currentBehold->GetAnimationRotation(currentBehold->CurrentTime, animationType);
            currentBehold->rotation = newRotation;
        }
    }
    return 0;
}

void updateCurrentMazeView(int newI, int newJ) {
    
    g_GraphicScene.cleanMazeView();
    g_GraphicScene.vec_pMeshCurrentMaze.push_back(mainChar);
    g_GraphicScene.vec_pMeshCurrentMaze.push_back(planeFloor);

    mainChar->currentI = newI;
    mainChar->currentJ = newJ;

    m_blocksLoader->cleanPairs();

    g_PhysicsSystem->RemoveWalls();

    _MAZE_TILE_INFO* pMazeTileInfo = new _MAZE_TILE_INFO[NUM_THREADS];
    DWORD dw;

    int iLoop = 0;
    // Initialize the mutex that will be used to protect the vector
    //InitializeCriticalSection(&dataMutex); 
    //dataSemaphore = CreateSemaphore(NULL, 0, NUM_THREADS, NULL);

    for (int a = 0; a < g_GraphicScene.drawFog; a++) {
        for (int b = 0; b < g_GraphicScene.drawFog; b++) {

            int iPlusA = newI + a;
            int jPlusB = newJ + b;
            int iMinusA = newI - a;
            int jMinusB = newJ - b;

            pMazeTileInfo[iLoop].i = iPlusA;
            pMazeTileInfo[iLoop].j = jPlusB;

            if (m_blocksLoader->checkValidPosition(iPlusA, jPlusB)) {
                createMazeTile(iPlusA, jPlusB);
                //ahThread[iLoop] = chBEGINTHREADEX(NULL, 0, createMazeTile,
                //    (PVOID)(&pMazeTileInfo[iLoop]),
                //    0, &dw);

                iLoop++;
                if (iLoop >= NUM_THREADS) iLoop = 0;
            }

            pMazeTileInfo[iLoop].i = iPlusA;
            pMazeTileInfo[iLoop].j = jMinusB;

            if (m_blocksLoader->checkValidPosition(iPlusA, jMinusB)) {
                createMazeTile(iPlusA, jMinusB);
                //ahThread[iLoop] = chBEGINTHREADEX(NULL, 0, createMazeTile,
                //    (PVOID)(&pMazeTileInfo[iLoop]),
                //    0, &dw);

                iLoop++;
                if (iLoop >= NUM_THREADS) iLoop = 0;
            }

            pMazeTileInfo[iLoop].i = iMinusA;
            pMazeTileInfo[iLoop].j = jMinusB;

            if (m_blocksLoader->checkValidPosition(iMinusA, jMinusB)) {
                createMazeTile(iMinusA, jMinusB);
                //ahThread[iLoop] = chBEGINTHREADEX(NULL, 0, createMazeTile,
                //    (PVOID)(&pMazeTileInfo[iLoop]),
                //    0, &dw);

                iLoop++;
                if (iLoop >= NUM_THREADS) iLoop = 0;
            }

            pMazeTileInfo[iLoop].i = iMinusA;
            pMazeTileInfo[iLoop].j = jPlusB;

            if (m_blocksLoader->checkValidPosition(iMinusA, jPlusB)) {
                createMazeTile(iMinusA, jPlusB);
                //ahThread[iLoop] = chBEGINTHREADEX(NULL, 0, createMazeTile,
                //    (PVOID)(&pMazeTileInfo[iLoop]),
                //    0, &dw);

                iLoop++;
                if (iLoop >= NUM_THREADS) iLoop = 0;
            }
        }
    }

    // Only waits for 64 of them, not matter how many you call.
    //WaitForMultipleObjects(NUM_THREADS, ahThread, TRUE, INFINITE);

    // Wait for all threads to finish executing
    //for (int i = 0; i < NUM_THREADS; ++i) {
    //    WaitForSingleObject(dataSemaphore, INFINITE);
    //}

    m_blocksLoader->cleanPairs();

    float mainCharX = (newJ * GLOBAL_MAP_OFFSET) - (GLOBAL_MAP_OFFSET / 2);
    float mainCharZ = (newI * GLOBAL_MAP_OFFSET) - (GLOBAL_MAP_OFFSET / 2);
    mainChar->position = glm::vec3(mainCharX, 25.f, mainCharZ);

    planeFloor->position.x = mainChar->position.x;
    planeFloor->position.y = -5.f;
    planeFloor->position.z = mainChar->position.z;

    g_cameraTarget = mainChar->position;
    g_cameraEye = glm::vec3(mainChar->position.x, 500.f, mainChar->position.z + 50.f);

    g_MapCameraTarget = mainChar->position;
    g_MapCameraEye = glm::vec3(mainChar->position.x, (g_GraphicScene.drawFog * 160.f), mainChar->position.z + 2.f);
}

void setStaticPlane() {
    cMeshObject* pmesh = new cMeshObject();
    pmesh->meshName = "Plane_Floor";
    iShape* paabb = nullptr;
    PhysicsObject* pphysObj = nullptr;

    // Creates the AABB structure for the Terrain
    float min[3] = { 0.f,
                     0.f,
                     0.f };
    float max[3] = { 10000.f,
                     10.f,
                     10000.f };
    paabb = new AABB(min, max);

    Vector3 position;
    position.x = 0.f;
    position.y = 0.f;
    position.z = 0.f;

    // Adds the AABB to the Physics System
    pphysObj = g_PhysicsSystem->CreatePhysicsObject(pmesh->meshName, position, paabb);
    pphysObj->SetMass(-1.0f);
}

int main(int argc, char* argv[]) {
    std::cout << "starting up..." << std::endl;

    GLint vpos_location = 0;
    GLint vcol_location = 0;

    animationType = 0;
    animationSpeed = 0.01;

    g_PhysicsSystem = new PhysicsSystem();

    // ---------------------------- Creating the Maze -----------------------------
    { 
        srand(time(NULL));

        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

        cMazeMaker_W2023::sProcessMemoryCounters memInfoStart;

        theMM.getMemoryUse(memInfoStart);
        theMM.GenerateMaze(MAP_HEIGHT, MAP_WIDTH);

        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

        std::cout << "It took me " << time_span.count() << " seconds.";

        cMazeMaker_W2023::sProcessMemoryCounters memInfoEnd;
        theMM.getMemoryUse(memInfoEnd);

        unsigned long long deltaMemoryUsed = memInfoEnd.WorkingSetSize - memInfoStart.WorkingSetSize;
        std::cout << "Delta memory:" << std::endl;
        std::cout << "\t" << deltaMemoryUsed << " bytes" << std::endl;
        std::cout << "\t" << deltaMemoryUsed / 1024 << " K" << std::endl;
        std::cout << "\t" << deltaMemoryUsed / (1024 * 1024) << " M" << std::endl;
        std::cout << "\t" << deltaMemoryUsed / (1024 * 1024 * 1024) << " G" << std::endl;

        theMM.PrintMaze();
    } // ---------------------------- Creating the Maze -----------------------------

    int updateCount = 0;
    pBrain = new cLuaBrain();

    g_GraphicScene.map_beholds = new std::map<std::string, cMeshObject*>();

    m_blocksLoader = new BlocksLoader(MAP_HEIGHT, MAP_WIDTH);

    g_cameraTarget = glm::vec3(1000.f, 0.0, 1000.f);
    g_cameraEye = glm::vec3(1000.f, 2000.f, 1010.f);

    g_MapCameraTarget = glm::vec3(1000.f, 0.0, 1000.f);
    g_MapCameraEye = glm::vec3(1000.f, 2000.f, 1010.f);

    // ------------------ FMOD INITIALIZATION ------------------------------------
    {
    //initialize fmod with max channels
        fmod_manager = new FModManager();
        if (!fmod_manager->Initialize(max_channels, FMOD_INIT_NORMAL)) {
            std::cout << "Failed to initialize FMod" << std::endl;
            return -1;
        }

        // 1 - MP3 Format
        // 2 - WAV Format
        fmod_manager->choosenAudio = 1;
        // Load XML soundList and create sounds
        if (fmod_manager->LoadSounds() != 0) {
            std::cout << "Failed to load sounds in sounds/sounds.xml" << std::endl;
            return -5;
        }

        //create channel groups
        if (
            !fmod_manager->create_channel_group("master") ||
            !fmod_manager->create_channel_group("music") ||
            !fmod_manager->create_channel_group("fx") ||
            !fmod_manager->create_channel_group("radio")
            )
        {
            return -2;
        }

        //set parents for channel groups
        if (!fmod_manager->set_channel_group_parent("music", "master") ||
            !fmod_manager->set_channel_group_parent("fx", "master") ||
            !fmod_manager->set_channel_group_parent("radio", "master")
            )
            return -4;
    }

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(1024, 768, "Random Game", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    std::cout << "Window created." << std::endl;

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    gameUi.fmod_manager_ = fmod_manager;
    gameUi.iniciatingUI();

    g_GraphicScene.PrepareScene();

    // Setting the lights
    lightning(g_GraphicScene.shaderID);

    // Creating Beholders
    for (int i = 0; i < BEHOLDERS_NUMBER; i++) {
        std::string randomPos;
        randomPos = m_blocksLoader->getRandomValidPosition();
        int pos = randomPos.find('.');
        int randomI = stoi(randomPos.substr(0, pos));
        int randomJ = stoi(randomPos.substr(pos + 1));

        float x = (randomJ * GLOBAL_MAP_OFFSET);
        float z = (randomI * GLOBAL_MAP_OFFSET);

        sModelDrawInfo drawingInformation;
        drawingInformation = g_GraphicScene.returnDrawInformation("Beholder");
        cMeshObject* beholder = g_GraphicScene.CreateGameObjectByType("Beholder", 
            glm::vec3(x - (GLOBAL_MAP_OFFSET / 2), 25.0f, z - (GLOBAL_MAP_OFFSET / 2)), drawingInformation);
        //beholder = g_GraphicScene.GetObjectByName("Beholder", false);
        std::string beholderName = "B" + std::to_string(randomI) + "_" + std::to_string(randomJ);
        beholder->friendlyName = beholderName;
        beholder->textures[0] = "Beholder_Base_color.bmp";
        beholder->textureRatios[0] = 1.0f;
        beholder->SetUniformScale(10.0f);

        cMeshObject* cone = new cMeshObject();
        cone->meshName = "Beholder_Vision";
        cone->friendlyName = "Beholder_Vision" + std::to_string(randomI) + "_" + std::to_string(randomJ);
        cone->bUse_RGBA_colour = true;
        cone->RGBA_colour = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
        cone->bDoNotLight = true;
        beholder->vecChildMeshes.push_back(cone);

        beholder->currentI = randomI;
        beholder->currentJ = randomJ;

        g_GraphicScene.map_beholds->try_emplace(beholderName, beholder);
    }

    itBeholdsToFollow = g_GraphicScene.map_beholds->begin();

    debugLightSpheres();

    // ---------------- LOADING TEXTURES ----------------------------------------------
    g_GraphicScene.LoadTextures();

    // ---------------- LUA  ----------------------------------------------
    { 
        std::string moveScriptTowardsDestinationFUNCTION =
            "function moveObjectTowardsDestination( objectID, xDest, yDest, zDest )										    \n"	\
            "	isValidObj, xObj, yObj, zObj, vxObj, vyObj, vzObj, moving = getObjectState( objectID )		\n"	\
            "	if isValidObj and xDest ~= nil and yDest ~= nil and zDest ~= nil and moving == 1 then					\n"	\
            "		xDirection = xDest - xObj																		    \n"	\
            "		yDirection = yDest - yObj																		    \n"	\
            "		zDirection = zDest - zObj																			\n"	\
            "		magnitude = (xDirection * xDirection + yDirection * yDirection + zDirection * zDirection)^0.5		\n"	\
            "		if magnitude > " + std::to_string(SMALLEST_DISTANCE) + " then										\n"	\
            "			xStep = xDirection / magnitude / 2																\n"	\
            "			yStep = yDirection / magnitude / 2																\n"	\
            "			zStep = zDirection / magnitude / 2         													    \n"	\
            "																                                    \n"	\
            "			xObj = xObj + xStep																					\n"	\
            "			yObj = yObj + yStep																				    \n"	\
            "			zObj = zObj + zStep																					\n"	\
            "																								\n"	\
            "			setObjectState( objectID, xObj, yObj, zObj, vyObj, vzObj, vzObj, 1 )							\n"	\
            "			return true																						\n"	\
            "		else																								\n"	\
            "		    print(\"Current Object: \", objectID ) 															\n"	\
            "		    print(\"Setting this position: \", xObj, yObj, zObj ) 															\n"	\
            "			setObjectState( objectID, xObj, yObj, zObj, vx, vy, vz, 0 )							\n"	\
            "			return false																					\n"	\
            "		end																									\n"	\
            "	end																										\n"	\
            "end";
        pBrain->RunScriptImmediately(moveScriptTowardsDestinationFUNCTION);


        pBrain->SetObjectVector(g_GraphicScene.map_beholds);

        for (std::map< std::string, cMeshObject*>::iterator itBeholds =
            g_GraphicScene.map_beholds->begin(); itBeholds != g_GraphicScene.map_beholds->end();
            itBeholds++)
        {
            cMeshObject* currentBehold = itBeholds->second;
            unsigned int currentBeholdID = currentBehold->getID();

            std::string currentBeholdNextTileXScript = "nextTileX" + std::to_string(currentBeholdID) + " = nil";
            std::string currentBeholdNextTileYScript = "nextTileY" + std::to_string(currentBeholdID) + " = nil";
            std::string currentBeholdNextTileZScript = "nextTileZ" + std::to_string(currentBeholdID) + " = nil";
            pBrain->RunScriptImmediately(currentBeholdNextTileXScript);
            pBrain->RunScriptImmediately(currentBeholdNextTileYScript);
            pBrain->RunScriptImmediately(currentBeholdNextTileZScript);

            std::string myScriptText = "setObjectState(" + std::to_string(currentBeholdID) + ", " +
                std::to_string(currentBehold->position.x) + ", " +
                std::to_string(currentBehold->position.y) + ", " +
                std::to_string(currentBehold->position.z) + ", " +
                " 0, 0, 0 " +
                ")";
            pBrain->RunScriptImmediately(myScriptText);

            pBrain->LoadScript("behold" + std::to_string(currentBeholdID) + "BasicMove", "moveObjectTowardsDestination(" +
                std::to_string(currentBeholdID) + ", " +
                "nextTileX" + std::to_string(currentBeholdID) + ", " +
                "nextTileY" + std::to_string(currentBeholdID) + ", " +
                "nextTileZ" + std::to_string(currentBeholdID) + ")");
        }
    } // ---------------- LUA  ----------------------------------------------

    // ---------------- MAIN CHAR CREATION ------------------
    {
        bool loaderReturn = entityLoaderManager->LoadCharacter(MAIN_CHAR_JSON_INFO, playabledCharacter, errorMessage);

        if (!loaderReturn) {
            std::cout << "Error loading playable character: " << errorMessage << std::endl;
        }

        sModelDrawInfo drawingInformation;
        drawingInformation = g_GraphicScene.returnDrawInformation(playabledCharacter.mFriendlyName);
        glm::vec3 mainCharacterPosition(playabledCharacter.mPosition[0], playabledCharacter.mPosition[1], playabledCharacter.mPosition[2]);
        mainChar = g_GraphicScene.CreateGameObjectByType(playabledCharacter.mFriendlyName, mainCharacterPosition, drawingInformation);
        //mainChar = g_GraphicScene.GetObjectByName(playabledCharacter.mFriendlyName, false);
        mainChar->friendlyName = "MainChar";
        mainChar->bUse_RGBA_colour = true;
        mainChar->RGBA_colour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        playabledCharacter.mFriendlyName = mainChar->friendlyName;
        mainChar->SetUniformScale(10.0f);

        mainChar->currentI = mainChar->position.z / GLOBAL_MAP_OFFSET;
        mainChar->currentJ = mainChar->position.x / GLOBAL_MAP_OFFSET;
    }

    // ------------------ CREATING FLOOR --------------------
    {
        sModelDrawInfo drawingInformation;
        drawingInformation = g_GraphicScene.returnDrawInformation("Plane_Floor");
        planeFloor = g_GraphicScene.CreateGameObjectByType("Plane_Floor", glm::vec3(0.f), drawingInformation);
        planeFloor->bUse_RGBA_colour = true;
        planeFloor->RGBA_colour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        planeFloor->position = mainChar->position;
        planeFloor->SetUniformScale(10.0f);

        setStaticPlane();
    }

    std::string randomPos;
    randomPos = m_blocksLoader->getRandomValidPosition();
    int pos = randomPos.find('.');
    int randomI = stoi(randomPos.substr(0, pos));
    int randomJ = stoi(randomPos.substr(pos + 1));

    updateCurrentMazeView(randomI, randomJ);

    iShape* ball;
    PhysicsObject* physObj;
    ball = new Sphere(Point(0.0f, 0.0f, 0.0f), 2.f);

    // Adds the sphere to the Physics System
    Vector3 position;
    position.x = mainChar->position.x;
    position.y = mainChar->position.y;
    position.z = mainChar->position.z;
    physObj = g_PhysicsSystem->CreatePhysicsObject(mainChar->friendlyName, position, ball);
    mainChar->physObj = physObj;
    //std::cout << mainChar->position.x << ", " << mainChar->position.y << ", " << mainChar->position.z << std::endl;

    // ---------------- GAME LOOP START -----------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        ::g_pTheLightManager->CopyLightInformationToShader(g_GraphicScene.shaderID);

        DrawConcentricDebugLightObjects(gameUi.listbox_lights_current);        

        g_GraphicScene.DrawMapView(window, ::g_MapCameraEye, ::g_MapCameraTarget);

        g_GraphicScene.DrawScene(window, ::g_cameraEye, ::g_cameraTarget);

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        gameUi.render(g_GraphicScene, fmod_manager, g_pTheLightManager->vecTheLights);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        std::stringstream ssTitle;
        ssTitle << "Multiverse Cursed Village";
        std::string theText = ssTitle.str();

        // Physics Update
        g_PhysicsSystem->UpdateStep(1.f);

        for (int i = 0; i < g_PhysicsSystem->m_PhysicsObjects.size(); i++) {
            if (g_PhysicsSystem->m_PhysicsObjects[i]->meshName == "MainChar")
                mainChar->position = g_PhysicsSystem->m_PhysicsObjects[i]->GetPosition().GetGLM();
        }

        // Animation Update
        //for (std::map< std::string, cMeshObject*>::iterator itBeholds =
        //    g_GraphicScene.map_beholds->begin(); itBeholds != g_GraphicScene.map_beholds->end();
        //    itBeholds++) 
        //{
        //    cMeshObject* currentBehold = itBeholds->second;
        //    unsigned int currentBeholdID = currentBehold->getID();
        //    int numTilesNext = 0;
        //
        //    if (currentBehold->moving == 0 && !currentBehold->dead) {
        //        glm::vec3 nextPosition = glm::vec3(0.0f);
        //        if (!calculateNextPosition(currentBehold, nextPosition, numTilesNext)) {
        //            // Spin and reduce;
        //            currentBehold->adjustRoationAngleFromEuler(glm::vec3(0.0f, 0.2f, 0.0f));
        //            currentBehold->reduceFromScale(0.99f);
        //            if (currentBehold->scaleXYZ.x <= 0.01) {
        //                currentBehold->dead = true;
        //                std::cout << currentBehold->friendlyName << " is Dead :(" << std::endl;
        //            }
        //            continue;
        //        }
        //
        //        currentBehold->moving = 1;
        //        currentBehold->rotating = 1;
        //        currentBehold->PositionKeyFrames.clear();
        //        currentBehold->RotationKeyFrames.clear();
        //
        //        std::string currentBeholdNextTileXScript = 
        //            "nextTileX" + std::to_string(currentBeholdID) + " = " + 
        //            std::to_string(nextPosition.x);
        //        std::string currentBeholdNextTileYScript =
        //            "nextTileY" + std::to_string(currentBeholdID) + " = " +
        //            std::to_string(nextPosition.y);
        //        std::string currentBeholdNextTileZScript = 
        //            "nextTileZ" + std::to_string(currentBeholdID) + "  = " +
        //            std::to_string(nextPosition.z);
        //
        //        RotationKeyFrame currRKF;        
        //        currRKF.value = currentBehold->rotation;
        //        currRKF.time = 0;
        //        currRKF.useSlerp = false;
        //        currentBehold->RotationKeyFrames.push_back(currRKF);
        //
        //        RotationKeyFrame nextRKF;
        //        nextRKF.time = 1;
        //        nextRKF.useSlerp = false;
        //
        //        // Setting rotation
        //        if (nextPosition.x < currentBehold->position.x) {// Going WEST
        //            //currentBehold->setRotationFromEuler(glm::vec3(0.0f, NinetyDegrees, 0.0f));
        //            nextRKF.value = glm::vec3(0.0f, NinetyDegrees, 0.0f);
        //        }
        //
        //        if (nextPosition.x > currentBehold->position.x) {// Going EAST
        //            //currentBehold->setRotationFromEuler(glm::vec3(0.0f, -NinetyDegrees, 0.0f));
        //            nextRKF.value = glm::vec3(0.0f, -NinetyDegrees, 0.0f);
        //        }
        //
        //        if (nextPosition.z < currentBehold->position.z) {// Going NORTH
        //            //currentBehold->setRotationFromEuler(glm::vec3(0.0f, 0.0, 0.0f));
        //            nextRKF.value = glm::vec3(0.0f, 0.0, 0.0f);
        //        }
        //
        //        if (nextPosition.z > currentBehold->position.z) {// Going SOUTH
        //            //currentBehold->setRotationFromEuler(glm::vec3(0.0f, NinetyDegrees * 2, 0.0f));
        //            nextRKF.value = glm::vec3(0.0f, NinetyDegrees * 2, 0.0f);
        //        }
        //
        //        currentBehold->RotationKeyFrames.push_back(nextRKF);
        //
        //        std::cout << " ------------------------------------------------------ " << std::endl;
        //        std::cout << currentBehold->friendlyName << " CurrentPosition: (" <<
        //            currentBehold->position.x << ", " <<
        //            currentBehold->position.y << ", " <<
        //            currentBehold->position.z << ")" << std::endl;
        //
        //        std::cout << currentBehold->friendlyName << " I -> " <<
        //            currentBehold->currentI << ", J -> " <<
        //            currentBehold->currentJ << std::endl;
        //
        //        std::cout << currentBehold->friendlyName << " NextPosition: (" <<
        //            nextPosition.x << ", " <<
        //            nextPosition.y << ", " <<
        //            nextPosition.z << ")" << std::endl;
        //
        //        std::cout << "nextTileX" << currentBeholdID << " changed" << std::endl;
        //        std::cout << "nextTileY" << currentBeholdID << " changed" << std::endl;
        //        std::cout << "nextTileZ" << currentBeholdID << " changed" << std::endl;
        //
        //        PositionKeyFrame currPKF;
        //        currPKF.value = glm::vec3(currentBehold->position.x, currentBehold->position.y, currentBehold->position.z);
        //        currPKF.time = 0;
        //        currentBehold->PositionKeyFrames.push_back(currPKF);
        //
        //        PositionKeyFrame nextPKF;
        //        nextPKF.value = glm::vec3(nextPosition.x, nextPosition.y, nextPosition.z);
        //        nextPKF.time = 1;
        //        //nextPKF.time = numTilesNext;
        //        currentBehold->PositionKeyFrames.push_back(nextPKF);
        //
        //        currentBehold->CurrentTime = 0.f;
        //        currentBehold->IsLooping = false;
        //        currentBehold->IsPlaying = true;
        //    }
        //    else if (currentBehold->moving == 2 && !currentBehold->dead && currentBehold->rotating == 0) {
        //        // Check if the animation is finished?
        //        currentBehold->UpdateAnimation(1);
        //        currentBehold->Speed = animationSpeed;
        //
        //        glm::vec3 newPosition = currentBehold->GetAnimationPosition(currentBehold->CurrentTime, animationType);
        //        currentBehold->position.x = newPosition.x;
        //        currentBehold->position.z = newPosition.z;
        //    }
        //    else if (currentBehold->moving == 1 && !currentBehold->dead && currentBehold->rotating == 1) {
        //        currentBehold->UpdateAnimation(1);
        //        currentBehold->Speed = animationSpeed;
        //
        //        glm::vec3 newRotation = currentBehold->GetAnimationRotation(currentBehold->CurrentTime, animationType);
        //        currentBehold->rotation = newRotation;
        //    }
        //}
        //
        
        { // THREAD BEHOLD UPDATE
            DWORD dw;
            int iLoop = 0;
            _MAZE_TILE_INFO* pMazeTileInfo = new _MAZE_TILE_INFO;

            ahThread = chBEGINTHREADEX(NULL, 0, animationUpdate,
                (PVOID)(&pMazeTileInfo),
                0, &dw);

            //iLoop++;
            //if (iLoop >= NUM_THREADS) iLoop = 0;

            // Only waits for 64 of them, not matter how many you call.
            //WaitForMultipleObjects(NUM_THREADS, ahThread, TRUE, INFINITE);
            WaitForSingleObject(ahThread, INFINITE);
        }

        // Update will run any Lua script sitting in the "brain"
        pBrain->Update(1);

        if (g_GraphicScene.cameraFollowing && !g_GraphicScene.cameraTransitioning) {
            cMeshObject* currentBehold = itBeholdsToFollow->second;
            g_cameraEye.x = currentBehold->position.x + CAMERA_OFFSET;
            g_cameraEye.y = currentBehold->position.y + CAMERA_OFFSET;
            g_cameraEye.y = currentBehold->position.z + CAMERA_OFFSET;

            g_cameraTarget = currentBehold->position;
        } else if (g_GraphicScene.cameraTransitioning) {
            cMeshObject* currentBehold = itBeholdsToFollow->second;
            float distanceCameraBehold = glm::distance(g_cameraEye, currentBehold->position);
            
            float xDir = currentBehold->position.x - g_cameraEye.x;
            float yDir = currentBehold->position.y - g_cameraEye.y;
            float zDir = currentBehold->position.z - g_cameraEye.z;

            g_cameraTarget = currentBehold->position;

            float mag = pow((xDir * xDir + yDir * yDir + zDir * zDir), 0.5);

            if (distanceCameraBehold > CAMERA_OFFSET * 5) {
                
                float xStep = xDir / mag;
                float yStep = yDir / mag;
                float zStep = zDir / mag;

                g_cameraEye.x += xStep * 15;
                g_cameraEye.y += yStep * 15;
                g_cameraEye.z += zStep * 15;
            }
            else {
                g_GraphicScene.cameraFollowing = true;
                g_GraphicScene.cameraTransitioning = false;
            }
        }
        else {
            //g_cameraTarget = glm::vec3(500.f, 0.0, 500.f);
            //g_cameraEye = glm::vec3(500.f, 2000.f, 510.f);
        }

        glfwSetWindowTitle(window, ssTitle.str().c_str());
    }

    // FINISHING
    // Clean up the mutex before exiting the program
    DeleteCriticalSection(&dataMutex);
    CloseHandle(dataSemaphore);
    CloseHandle(ahThread);
    //for (int iLoop = 0; iLoop < NUM_THREADS; iLoop++)
    //    CloseHandle(ahThread[iLoop]);

    g_GraphicScene.Shutdown();
    delete ::g_pTheLightManager;
    delete entityLoaderManager;

    glfwDestroyWindow(window);
    fmod_manager->Shutdown();
    delete fmod_manager;

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void DrawConcentricDebugLightObjects(int currentLight) {
    extern bool bEnableDebugLightingObjects;

    if (!bEnableDebugLightingObjects)
    {
        pDebugSphere_1->bIsVisible = false;
        pDebugSphere_2->bIsVisible = false;
        pDebugSphere_3->bIsVisible = false;
        pDebugSphere_4->bIsVisible = false;
        pDebugSphere_5->bIsVisible = false;
        return;
    }

    pDebugSphere_1->bIsVisible = true;
    pDebugSphere_2->bIsVisible = true;
    pDebugSphere_3->bIsVisible = true;
    pDebugSphere_4->bIsVisible = true;
    pDebugSphere_5->bIsVisible = true;

    cLightHelper theLightHelper;

    // Move the debug sphere to where the light #0 is
    pDebugSphere_1->position = glm::vec3(::g_pTheLightManager->vecTheLights[currentLight].position);
    pDebugSphere_2->position = glm::vec3(::g_pTheLightManager->vecTheLights[currentLight].position);
    pDebugSphere_3->position = glm::vec3(::g_pTheLightManager->vecTheLights[currentLight].position);
    pDebugSphere_4->position = glm::vec3(::g_pTheLightManager->vecTheLights[currentLight].position);
    pDebugSphere_5->position = glm::vec3(::g_pTheLightManager->vecTheLights[currentLight].position);

    {
        // Draw a bunch of concentric spheres at various "brightnesses" 
        float distance75percent = theLightHelper.calcApproxDistFromAtten(
            0.75f,  // 75%
            0.001f,
            100000.0f,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.x,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.y,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.z);

        pDebugSphere_2->scale = distance75percent;
        pDebugSphere_2->position = glm::vec3(::g_pTheLightManager->vecTheLights[currentLight].position);
    }

    {
        // Draw a bunch of concentric spheres at various "brightnesses" 
        float distance50percent = theLightHelper.calcApproxDistFromAtten(
            0.50f,  // 75%
            0.001f,
            100000.0f,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.x,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.y,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.z);

        pDebugSphere_3->scale = distance50percent;
        pDebugSphere_3->position = glm::vec3(::g_pTheLightManager->vecTheLights[currentLight].position);
    }

    {
        // Draw a bunch of concentric spheres at various "brightnesses" 
        float distance25percent = theLightHelper.calcApproxDistFromAtten(
            0.25f,  // 75%
            0.001f,
            100000.0f,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.x,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.y,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.z);

        pDebugSphere_4->scale = distance25percent;
        pDebugSphere_4->position = glm::vec3(::g_pTheLightManager->vecTheLights[currentLight].position);
    }

    {
        // Draw a bunch of concentric spheres at various "brightnesses" 
        float distance5percent = theLightHelper.calcApproxDistFromAtten(
            0.05f,  // 75%
            0.001f,
            100000.0f,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.x,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.y,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.z);

        pDebugSphere_5->scale = distance5percent;
        pDebugSphere_5->position = glm::vec3(::g_pTheLightManager->vecTheLights[currentLight].position);
    }

    return;
}