#include "globalOpenGL.h"
#include "globalThings.h"   // For the light manager, etc.
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include "GraphicScene.h"
#include <iostream>
#include "Ball.h"
#include "SimulationView.h"
#include "cMazeMaker_W2023.h"
#include "BlocksLoader.h"

// Extern is so the compiler knows what TYPE this thing is
// The LINKER needs the ACTUAL declaration 
// These are defined in theMainFunction.cpp
extern glm::vec3 g_cameraEye;// = glm::vec3(0.0, 0.0, -25.0f);
extern glm::vec3 g_cameraTarget;// = glm::vec3(0.0f, 0.0f, 0.0f);
extern GraphicScene g_GraphicScene;
extern SimulationView* simView;
extern std::map< std::string, cMeshObject*>::iterator itBeholdsToFollow;
extern int animationType;
extern float animationSpeed;
extern cMeshObject* mainChar;
extern cMazeMaker_W2023 theMM;
extern BlocksLoader* m_blocksLoader;

const float CAMERA_MOVE_SPEED = 5.0f;
const float MOVE_SPEED = 0.1f;

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
        if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        {
            if (animationType < 3) {
                animationType++;
            }
            else {
                animationType = 0;
            }
        }

        if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        {
            if (animationType > 0) {
                animationType--;
            }
            else {
                animationType = 3;
            }
        }

        if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        {
            animationSpeed = 0.01;
        }

        if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        {
            animationSpeed = 0.02;
        }

        if (key == GLFW_KEY_3 && action == GLFW_PRESS)
        {
            animationSpeed = 0.03;
        }

        if (key == GLFW_KEY_4 && action == GLFW_PRESS)
        {
            animationSpeed = 0.04;
        }

        if (key == GLFW_KEY_5 && action == GLFW_PRESS)
        {
            animationSpeed = 0.05;
        }
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
        if (key == GLFW_KEY_A)     // Tile LEFT
        {
            mainChar->rotation.y += MOVE_SPEED;
        }
        if (key == GLFW_KEY_D)     // Tile RIGHT
        {
            mainChar->rotation.y -= MOVE_SPEED;
        }
        if (key == GLFW_KEY_W)     // Tile UP
        {
            mainChar->position.x += sin(mainChar->rotation.y);
            mainChar->position.z += cos(mainChar->rotation.y);
        }
        if (key == GLFW_KEY_S)     // Tile DOWN
        {
            mainChar->position.x -= sin(mainChar->rotation.y);
            mainChar->position.z -= cos(mainChar->rotation.y);
        }
    }
    break;

    case MOVING_MAZE:
    {
        int nextTileI;
        int nextTileJ;

        if (key == GLFW_KEY_A && action == GLFW_PRESS)     // Tile LEFT
        {
            //TO-DO
            //MainChar tries to move to left tile
            nextTileI = mainChar->currentI;
            nextTileJ = mainChar->currentJ - 1;
            if (m_blocksLoader->checkValidPosition(nextTileI, nextTileJ)) {
                updateCurrentMazeView(nextTileI, nextTileJ);
            }
        }
        if (key == GLFW_KEY_D && action == GLFW_PRESS)     // Tile RIGHT
        {
            nextTileI = mainChar->currentI;
            nextTileJ = mainChar->currentJ + 1;
            if (m_blocksLoader->checkValidPosition(nextTileI, nextTileJ)) {
                updateCurrentMazeView(nextTileI, nextTileJ);
            }
        }
        if (key == GLFW_KEY_W && action == GLFW_PRESS)     // Tile UP
        {
            nextTileI = mainChar->currentI - 1;
            nextTileJ = mainChar->currentJ;
            if (m_blocksLoader->checkValidPosition(nextTileI, nextTileJ)) {
                updateCurrentMazeView(nextTileI, nextTileJ);
            }
        }
        if (key == GLFW_KEY_S && action == GLFW_PRESS)     // Tile DOWN
        {
            nextTileI = mainChar->currentI + 1;
            nextTileJ = mainChar->currentJ;
            if (m_blocksLoader->checkValidPosition(nextTileI, nextTileJ)) {
                updateCurrentMazeView(nextTileI, nextTileJ);
            }
        }
    }
    break;
    } //switch (theEditMode)

    return;
}
