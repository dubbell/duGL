#include <flight_controller.h>


FlightController::FlightController(GLFWwindow* window, std::shared_ptr<Camera> camera) 
    : window(window), camera(camera), speed(0.1f), escapeHold(false), freeCursor(true) 
{
    activeKeys = {
        GLFW_KEY_W,
        GLFW_KEY_A,
        GLFW_KEY_S,
        GLFW_KEY_D,
        GLFW_KEY_ESCAPE,
        GLFW_KEY_SPACE,
        GLFW_KEY_LEFT_CONTROL
    };
};

std::vector<int> FlightController::getActiveKeys()
{
    return activeKeys;
}

void FlightController::processKeyboardInput(std::map<int, int> &keyboardState)
{
    glm::vec3 localVelocity(0.0f, 0.0f, 0.0f);

    // backwards/forwards
    if (keyboardState[GLFW_KEY_W] == GLFW_PRESS)
    {
        if (keyboardState[GLFW_KEY_S] == GLFW_RELEASE)
            localVelocity += glm::vec3(0.0f, 0.0f, -speed);
    }
    else if (keyboardState[GLFW_KEY_S] == GLFW_PRESS)
        localVelocity += glm::vec3(0.0f, 0.0f, speed);

    // left/right
    if (keyboardState[GLFW_KEY_A] == GLFW_PRESS)
    {
        if (keyboardState[GLFW_KEY_D] == GLFW_RELEASE)
            localVelocity += glm::vec3(-speed, 0.0f, 0.0f);
    }
    else if (keyboardState[GLFW_KEY_D] == GLFW_PRESS)
        localVelocity += glm::vec3(speed, 0.0f, 0.0f);
    
    // up/down
    if (keyboardState[GLFW_KEY_SPACE] == GLFW_PRESS)
    {
        if (keyboardState[GLFW_KEY_LEFT_CONTROL] == GLFW_RELEASE)
            localVelocity += glm::vec3(0.0f, speed, 0.0f);
    }
    else if (keyboardState[GLFW_KEY_LEFT_CONTROL] == GLFW_PRESS)
        localVelocity += glm::vec3(0.0f, -speed, 0.0f);

    // set velocity in local basis and update position based on velocity
    camera->setLocalVelocity(localVelocity);
    camera->updatePosition();

    if (!escapeHold && keyboardState[GLFW_KEY_ESCAPE] == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, freeCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        escapeHold = true;
        freeCursor = !freeCursor;
    }
    else if (keyboardState[GLFW_KEY_ESCAPE] == GLFW_RELEASE)
    {
        escapeHold = false;
    }
}

void FlightController::cursorPosCallback(float xOffset, float yOffset)
{
    if (freeCursor)
    {
        camera->turnRight(xOffset);
        camera->turnUp(yOffset);
    }
}

