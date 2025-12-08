#include <flight_controller.h>


FlightController::FlightController(std::shared_ptr<Camera> &camera) : camera(camera), speed(0.2f) {};

void FlightController::processKeyboardInput(std::map<int, int> &keyboardState)
{
    glm::vec3 localVelocity(0.0f, 0.0f, 0.0f);

    if (keyboardState[GLFW_KEY_W] == GLFW_PRESS)
    {
        if (keyboardState[GLFW_KEY_S] == GLFW_RELEASE)
            localVelocity += glm::vec3(0.0f, 0.0f, -speed);
    }
    else if (keyboardState[GLFW_KEY_S] == GLFW_PRESS)
        localVelocity += glm::vec3(0.0f, 0.0f, speed);
    if (keyboardState[GLFW_KEY_A] == GLFW_PRESS)
    {
        if (keyboardState[GLFW_KEY_D] == GLFW_RELEASE)
            localVelocity += glm::vec3(-speed, 0.0f, 0.0f);
    }
    else if (keyboardState[GLFW_KEY_D] == GLFW_PRESS)
        localVelocity += glm::vec3(speed, 0.0f, 0.0f);

    camera->setLocalVelocity(localVelocity);
    camera->updatePosition();
}

void FlightController::cursorPosCallback(float xOffset, float yOffset)
{
    camera->turnRight(xOffset);
    camera->turnUp(yOffset);
}

