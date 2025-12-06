#include "camera.h"


Camera::Camera(glm::vec3 &startingPosition, float maxSpeed) : velocity(0.0f, 0.0f, 0.0f)
{
    position = startingPosition;
    maxSpeed = maxSpeed;
    pitch = 0.0f;
    yaw = 0.0f;
}

glm::mat3 Camera::getLocalBasis()
{
    glm::vec3 front = glm::normalize(glm::vec3(
        cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch))));

    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

    glm::vec3 up = glm::cross(right, front);

    return glm::mat3(front, right, up);
}

glm::vec3 Camera::toGlobal(glm::vec3 local)
{
    glm::mat3 globalToLocal = getLocalBasis();
    return glm::transpose(globalToLocal) * local;
}




void Camera::setPosition(glm::vec3 &newPosition)
{
    position = newPosition;
}

void Camera::setVelocity(glm::vec3 &newVelocity)
{
    velocity = newVelocity;
}

void Camera::setLocalVelocity(glm::vec3 &localVelocity)
{
    glm::mat3 localToGlobal = getLocalBasis();
    velocity = glm::transpose(localToGlobal) * localVelocity;
}

void Camera::accelerate(glm::vec3 &acceleration)
{
    velocity += acceleration;
}

void Camera::accelerateLocally(glm::vec3 &localAcceleration)
{
    velocity += toGlobal(localAcceleration);
}

void Camera::turnRight(float deltaYaw)
{
    yaw += deltaYaw;
    if (yaw < -180.0f) yaw += 360.0f;
    else if (yaw > 180.0f) yaw -= 360.0f;
}

void Camera::turnUp(float deltaPitch)
{
    pitch += deltaPitch;
    if (pitch < -89.0f) pitch = -89.0f;
    else if (pitch > 89.0f) pitch = 89.0f;
}


void Camera::processKeyboardInput(std::map<int, int> &keyboardState)
{
    float speed = 2.0f;
    glm::vec3 globalVelocity(0.0f, 0.0f, 0.0f);

    if (keyboardState[GLFW_KEY_W] == GLFW_PRESS)
    {
        if (keyboardState[GLFW_KEY_S] == GLFW_RELEASE)
            globalVelocity += toGlobal(glm::vec3(0.0f, 0.0f, -speed));
    }
    else if (keyboardState[GLFW_KEY_S] == GLFW_PRESS)
        globalVelocity += toGlobal(glm::vec3(0.0f, 0.0f, speed));

    if (keyboardState[GLFW_KEY_A] == GLFW_PRESS)
    {
        if (keyboardState[GLFW_KEY_D] == GLFW_RELEASE)
            globalVelocity += toGlobal(glm::vec3(-speed, 0.0f, 0.0f));
    }
    else if (keyboardState[GLFW_KEY_D] == GLFW_PRESS)
        globalVelocity += toGlobal(glm::vec3(speed, 0.0f, 0.0f));
}



glm::mat4 Camera::getView() const
{
    glm::vec3 front = glm::normalize(glm::vec3(
        cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch))));

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    return glm::lookAt(position, position + front, up);
}

