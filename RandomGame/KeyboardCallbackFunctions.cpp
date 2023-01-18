#include "globalOpenGL.h"
#include "globalThings.h"   // For the light manager, etc.
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include "GraphicScene.h"
#include <iostream>
#include "Ball.h"
#include "SimulationView.h"

// Extern is so the compiler knows what TYPE this thing is
// The LINKER needs the ACTUAL declaration 
// These are defined in theMainFunction.cpp
extern glm::vec3 g_cameraEye;// = glm::vec3(0.0, 0.0, -25.0f);
extern glm::vec3 g_cameraTarget;// = glm::vec3(0.0f, 0.0f, 0.0f);
extern GraphicScene g_GraphicScene;
extern SimulationView* simView;
extern std::map< std::string, cMeshObject*>::iterator itBeholdsToFollow;

enum eEditMode
{
    MOVING_CAMERA,
    MOVING_LIGHT,
    MOVING_SELECTED_OBJECT
};

eEditMode theEditMode = MOVING_CAMERA;
unsigned int selectedLightIndex = 0;
unsigned int selectedObjectIndex = 0;

bool bEnableDebugLightingObjects = true;

unsigned int currentObjectID = 0;

void key_callback(GLFWwindow* window,
    int key, int scancode,
    int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        theEditMode = MOVING_CAMERA;
    }
    else if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        theEditMode = MOVING_LIGHT;
    }
    else if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        theEditMode = MOVING_SELECTED_OBJECT;
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

    switch (theEditMode)
    {
    case MOVING_CAMERA:
    {
        // Move the camera
        // AWSD   AD - left and right
        //        WS - forward and back
        g_GraphicScene.cameraFollowing = false;
        g_GraphicScene.cameraTransitioning = false;

        const float CAMERA_MOVE_SPEED = 5.0f;
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

    }//case MOVING_LIGHT:
    break;

    case MOVING_SELECTED_OBJECT:
    {
        g_GraphicScene.cameraTransitioning = true;

        if (key == GLFW_KEY_F6 && action == GLFW_PRESS) {
            g_GraphicScene.cameraFollowing = false;
            g_GraphicScene.cameraTransitioning = true;
            itBeholdsToFollow++;
            if (itBeholdsToFollow == g_GraphicScene.map_beholds->end()) {
                itBeholdsToFollow = g_GraphicScene.map_beholds->begin();
            }
        }

    }
    break;
    }//switch (theEditMode)

    return;
}
