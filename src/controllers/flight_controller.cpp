#include "flight_controller.h"


FlightController::FlightController(PlayerInterface* playerInterface) 
    : speed(0.1f), playerInterface(playerInterface) {}


std::set<int> FlightController::getActiveKeys()
{
    return {
        GLFW_KEY_W,
        GLFW_KEY_A,
        GLFW_KEY_S,
        GLFW_KEY_D,
        GLFW_KEY_SPACE,
        GLFW_KEY_LEFT_CONTROL,
        GLFW_KEY_LEFT_SHIFT
    };
}

std::set<int> FlightController::getActiveTogglableKeys()
{
    return {};
}

std::set<int> FlightController::getActiveSingleTriggerKeys()
{
    return {
        GLFW_KEY_ESCAPE
    };
}

void FlightController::processKeyboardInput(KeyboardState& keyboardState)
{
    glm::vec3 localVelocity(0.0f, 0.0f, 0.0f);

    // backwards/forwards
    if (keyboardState.keys[GLFW_KEY_W] == GLFW_PRESS)
    {
        if (keyboardState.keys[GLFW_KEY_S] == GLFW_RELEASE)
            localVelocity += glm::vec3(0.0f, 0.0f, -1.0);
    }
    else if (keyboardState.keys[GLFW_KEY_S] == GLFW_PRESS)
        localVelocity += glm::vec3(0.0f, 0.0f, 1.0);

    // left/right
    if (keyboardState.keys[GLFW_KEY_A] == GLFW_PRESS)
    {
        if (keyboardState.keys[GLFW_KEY_D] == GLFW_RELEASE)
            localVelocity += glm::vec3(-1.0, 0.0f, 0.0f);
    }
    else if (keyboardState.keys[GLFW_KEY_D] == GLFW_PRESS)
        localVelocity += glm::vec3(1.0, 0.0f, 0.0f);
    
    // up/down
    if (keyboardState.keys[GLFW_KEY_SPACE] == GLFW_PRESS)
    {
        if (keyboardState.keys[GLFW_KEY_LEFT_CONTROL] == GLFW_RELEASE)
            localVelocity += glm::vec3(0.0f, 1.0, 0.0f);
    }
    else if (keyboardState.keys[GLFW_KEY_LEFT_CONTROL] == GLFW_PRESS)
        localVelocity += glm::vec3(0.0f, -1.0, 0.0f);

    // normalize velocity vector and set to movement speed, if velocity is nonzero
    if (glm::length(localVelocity) > 0.0001f)
        localVelocity = glm::normalize(localVelocity) * (keyboardState.keys[GLFW_KEY_LEFT_SHIFT] == GLFW_RELEASE ? speed : speed * 2);
    
    Camera* camera = playerInterface->getActiveCamera();
    GLFWwindow* window = playerInterface->getWindow();

    // set velocity in local basis and update position based on velocity
    camera->setLocalVelocity(localVelocity);
    camera->updatePosition();

    // cursor capture
    if (keyboardState.singleTriggerKeys[GLFW_KEY_ESCAPE] == SINGLE_TRIGGER_KEY_PRESS)
    {
        bool freeCursor = playerInterface->getFreeCursor();
        glfwSetInputMode(window, GLFW_CURSOR, freeCursor ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        playerInterface->setFreeCursor(!freeCursor);
    }
}

void FlightController::cursorOffsetCallback(float xOffset, float yOffset)
{
    if (!playerInterface->getFreeCursor())
    {
        Camera* camera = playerInterface->getActiveCamera();
        camera->turnRight(xOffset);
        camera->turnUp(yOffset);
    }
}