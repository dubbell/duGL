#include "camera.h"


Camera::Camera(glm::vec3 &startingPosition, float maxSpeed) : velocity(0.0f, 0.0f, 0.0f)
{
    position = startingPosition;
    maxSpeed = maxSpeed;
    pitch = 0.0f;
    yaw = 0.0f;
}

Basis Camera::getLocalBasis()
{
    glm::vec3 front = glm::normalize(glm::vec3(
        cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch))));

    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

    glm::vec3 up = glm::cross(right, front);

    return Basis(front, right, up);
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
    Basis basis = getLocalBasis();
    velocity = localVelocity.x * basis.right + localVelocity.y * basis.up - localVelocity.z * basis.front;
}

void Camera::accelerate(glm::vec3 &acceleration)
{
    velocity += acceleration;
}

void Camera::accelerateLocally(glm::vec3 &acceleration)
{
    Basis basis = getLocalBasis();
    velocity += acceleration.x * basis.right + acceleration.y * basis.up - acceleration.z * basis.front;
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



glm::mat4 Camera::getView() const
{
    glm::vec3 front = glm::normalize(glm::vec3(
        cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch))));

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    return glm::lookAt(position, position + front, up);
}

