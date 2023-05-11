//#include "globalOpenGL.h"
#include "globalThings.h"   // For the light manager, etc.
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include "GraphicScene.h"
#include <iostream>

#include "cMazeMaker_W2023.h"
#include "BlocksLoader.h"

#include "cCharacter.h"

// Extern is so the compiler knows what TYPE this thing is
// The LINKER needs the ACTUAL declaration 
// These are defined in theMainFunction.cpp
extern glm::vec3 g_cameraEye;// = glm::vec3(0.0, 0.0, -25.0f);
extern glm::vec3 g_cameraTarget;// = glm::vec3(0.0f, 0.0f, 0.0f);

extern glm::vec3 g_MapCameraEye;
extern glm::vec3 g_MapCameraTarget;

extern physics::iCharacterController* g_CharController;
extern GraphicScene g_GraphicScene;
extern std::map< std::string, cMeshObject*>::iterator itBeholdsToFollow;

extern cCharacter* mainChar;
extern cMazeMaker_W2023 theMM;
extern BlocksLoader* m_blocksLoader;

const float CAMERA_MOVE_SPEED = 5.0f;
const float MOVE_SPEED = 0.1f;

#define GLOBAL_MAP_OFFSET 50

enum eEditMode
{
    ANIMATION,
    MOVING_CAMERA,
    MOVING_LIGHT,
    MOVING_MAINCHAR,
    MOVING_MAZE
};

eEditMode theEditMode = MOVING_MAZE;
unsigned int selectedLightIndex = 0;
unsigned int selectedObjectIndex = 0;

bool bEnableDebugLightingObjects = true;

unsigned int currentObjectID = 0;

extern void updateCurrentMazeView(int newI, int newJ);;

glm::vec3 direction(0.f);
float force = 3.f;
float directionX = 0.f;
float directionZ = 0.f;
bool isMovingForward = false;

void moveCharacter()
{
    directionX = sin(mainChar->rotation.y);
    directionZ = cos(mainChar->rotation.y);
    float directionMagnitude = sqrt(directionX * directionX + directionZ * directionZ);
    directionX /= directionMagnitude;
    directionZ /= directionMagnitude;

    direction = glm::vec3(directionX * force, 0.f, directionZ * force);
    g_CharController->SetWalkDirection(direction);
}

void stopCharacter()
{
    direction = glm::vec3(0.f);
    g_CharController->SetWalkDirection(direction);
}

void key_callback(GLFWwindow* window,
    int key, int scancode,
    int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        theEditMode = ANIMATION;
    }
    else if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        theEditMode = MOVING_CAMERA;
    }
    else if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        theEditMode = MOVING_LIGHT;
    }
    else if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        theEditMode = MOVING_MAINCHAR;
    }
    else if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        theEditMode = MOVING_MAZE;
    }

    switch (theEditMode)
    {
    case MOVING_CAMERA:
    {
        // Move the camera
        // AWSD   AD - left and right
        //        WS - forward and back
        g_GraphicScene.cameraFollowing = false;
        g_GraphicScene.cameraTransitioning = false;

        if (key == GLFW_KEY_A)     // Left
        {
            ::g_cameraEye.x -= CAMERA_MOVE_SPEED;
        }
        if (key == GLFW_KEY_D)     // Right
        {
            ::g_cameraEye.x += CAMERA_MOVE_SPEED;
        }
        if (key == GLFW_KEY_W)     // Forward
        {
            ::g_cameraEye.z += CAMERA_MOVE_SPEED;
        }
        if (key == GLFW_KEY_S)     // Backwards
        {
            ::g_cameraEye.z -= CAMERA_MOVE_SPEED;
        }
        if (key == GLFW_KEY_Q)     // Down
        {
            ::g_cameraEye.y -= CAMERA_MOVE_SPEED;
        }
        if (key == GLFW_KEY_E)     // Up
        {
            ::g_cameraEye.y += CAMERA_MOVE_SPEED;
        }

        if (key == GLFW_KEY_1)
        {
            ::g_cameraEye = glm::vec3(-5.5f, -3.4f, 15.0f);
        }
    }//case MOVING_CAMERA:
    break;

    case ANIMATION:
    {

    }
    break;

    case MOVING_LIGHT:
    {
        const float LIGHT_MOVE_SPEED = 1.0f;
        if (key == GLFW_KEY_A)     // Left
        {
            ::g_pTheLightManager->vecTheLights[selectedLightIndex].position.x -= LIGHT_MOVE_SPEED;
        }
        if (key == GLFW_KEY_D)     // Right
        {
            ::g_pTheLightManager->vecTheLights[selectedLightIndex].position.x += LIGHT_MOVE_SPEED;
        }
        if (key == GLFW_KEY_W)     // Forward
        {
            ::g_pTheLightManager->vecTheLights[selectedLightIndex].position.z += LIGHT_MOVE_SPEED;
        }
        if (key == GLFW_KEY_S)     // Backwards
        {
            ::g_pTheLightManager->vecTheLights[selectedLightIndex].position.z -= LIGHT_MOVE_SPEED;
        }
        if (key == GLFW_KEY_Q)     // Down
        {
            ::g_pTheLightManager->vecTheLights[selectedLightIndex].position.y -= LIGHT_MOVE_SPEED;
        }
        if (key == GLFW_KEY_E)     // Up
        {
            ::g_pTheLightManager->vecTheLights[selectedLightIndex].position.y += LIGHT_MOVE_SPEED;
        }

        if (key == GLFW_KEY_PAGE_DOWN)
        {
            // Select previous light
            if (selectedLightIndex > 0)
            {
                selectedLightIndex--;
            }
        }
        if (key == GLFW_KEY_PAGE_UP)
        {
            if (selectedLightIndex < (::g_pTheLightManager->vecTheLights.size() - 1))
            {
                // Select previous light
                selectedLightIndex++;
            }
        }

        // Change attenuation
        // Linear is ==> "how bright the light is"
        // Quadratic is ==> "how far does the light go or 'throw' into the scene?"
        if (key == GLFW_KEY_1)
        {
            // Linear Decrease by 1% 
            ::g_pTheLightManager->vecTheLights[selectedLightIndex].atten.y *= 0.99f;
        }
        if (key == GLFW_KEY_2)
        {
            // Linear Increase by 1%
            ::g_pTheLightManager->vecTheLights[selectedLightIndex].atten.y *= 1.01f;
        }
        if (key == GLFW_KEY_3)
        {
            if (mods == GLFW_MOD_SHIFT)
            {   // ONLY shift modifier is down
                // Quadratic Decrease by 0.1% 
                ::g_pTheLightManager->vecTheLights[selectedLightIndex].atten.z *= 0.99f;
            }
            else
            {
                // Quadratic Decrease by 0.01% 
                ::g_pTheLightManager->vecTheLights[selectedLightIndex].atten.z *= 0.999f;
            }
        }
        if (key == GLFW_KEY_4)
        {
            if (mods == GLFW_MOD_SHIFT)
            {   // ONLY shift modifier is down
                // Quadratic Increase by 0.1% 
                ::g_pTheLightManager->vecTheLights[selectedLightIndex].atten.z *= 1.01f;
            }
            else
            {
                // Quadratic Decrease by 0.01% 
                ::g_pTheLightManager->vecTheLights[selectedLightIndex].atten.z *= 1.001f;
            }
        }

        if (key == GLFW_KEY_9 && action == GLFW_PRESS)
        {
            // Check for the mods to turn the spheres on or off
            bEnableDebugLightingObjects = false;
        }

        if (key == GLFW_KEY_0 && action == GLFW_PRESS)
        {
            // Check for the mods to turn the spheres on or off
            bEnableDebugLightingObjects = true;
        }


    }//case MOVING_LIGHT:
    break;

    case MOVING_MAINCHAR:
    {

    }
    break;

    case MOVING_MAZE:
    {
        if (g_GraphicScene.loggedIn) {
            if (key == GLFW_KEY_W)
            {
                if (action == GLFW_PRESS)
                {
                    mainChar->isMovingForward = true;
                }
                else if (action == GLFW_RELEASE)
                {
                    mainChar->isMovingForward = false;
                }
            }
            else if (key == GLFW_KEY_A)
            {
                if (action == GLFW_PRESS)
                {
                    mainChar->isTurningLeft = true;
                }
                else if (action == GLFW_RELEASE)
                {
                    mainChar->isTurningLeft = false;
                }
            }
            else if (key == GLFW_KEY_D)
            {
                if (action == GLFW_PRESS)
                {
                    mainChar->isTurningRight = true;
                }
                else if (action == GLFW_RELEASE)
                {
                    mainChar->isTurningRight = false;
                }
            }
        }
    }
    break;
    } //switch (theEditMode)
}