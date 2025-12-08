#include "camera.h"


Camera::Camera() 
    : position(0.0f, 0.0f, 0.0f), 
      velocity(0.0f, 0.0f, 0.0f), 
      maxSpeed(0.0f), 
      pitch(0.0f), 
      yaw(0.0f) 
{
    projectionMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
};

Camera::Camera(glm::vec3 startingPosition, float maxSpeed) 
    : position(startingPosition),
      velocity(0.0f, 0.0f, 0.0f),
      maxSpeed(maxSpeed), 
      pitch(0.0f), 
      yaw(0.0f)
{
    projectionMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
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

void Camera::updatePosition()
{
    position += velocity;
}


void Camera::setVelocity(glm::vec3 &newVelocity)
{
    velocity = newVelocity;
}

void Camera::setLocalVelocity(glm::vec3 &localVelocity)
{
    velocity = toGlobal(localVelocity);
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




glm::mat4 Camera::getViewMatrix() const
{
    glm::vec3 front = glm::normalize(glm::vec3(
        cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch))));

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix() const
{
    return projectionMatrix;
}
