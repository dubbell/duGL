#include "dugl/camera.h"


Camera::Camera()
    : position(0.0f, 0.0f, 0.0f), 
      velocity(0.0f, 0.0f, 0.0f), 
      maxSpeed(1.0f), 
      pitch(0.0f), 
      yaw(0.0f),
      fov(90.0f),
      aspectRatio(4 / 3),
      near(0.1f),
      far(500.0f),
      viewInvalidated(true),
      projectionInvalidated(true)
{};

glm::mat3 Camera::getLocalBasis()
{
    glm::vec3 front = glm::normalize(glm::vec3(
        cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch))));

    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

    glm::vec3 up = glm::cross(right, front);

    return glm::mat3(right, up, -front);
}

glm::vec3 Camera::toGlobal(glm::vec3 local)
{
    glm::mat3 globalToLocal = getLocalBasis();
    return globalToLocal * local;
}

void Camera::setPosition(glm::vec3 &newPosition)
{
    position = newPosition;
    invalidateCachedViewMatrix();
}

void Camera::updatePosition()
{
    position += velocity;
    invalidateCachedViewMatrix();
}

void Camera::setVelocity(glm::vec3 &newVelocity)
{
    velocity = newVelocity;
    invalidateCachedViewMatrix();
}

void Camera::setLocalVelocity(glm::vec3 &localVelocity)
{
    velocity = toGlobal(localVelocity);
    invalidateCachedViewMatrix();
}

void Camera::accelerate(glm::vec3 &acceleration)
{
    velocity += acceleration;
    invalidateCachedViewMatrix();
}

void Camera::accelerateLocally(glm::vec3 &localAcceleration)
{
    velocity += toGlobal(localAcceleration);
    invalidateCachedViewMatrix();
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


glm::vec3& Camera::getPosition()
{
    return position;
}

glm::mat4& Camera::getViewMatrix()
{
    if (viewInvalidated)
    {
        glm::vec3 front = glm::normalize(glm::vec3(
            cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
            sin(glm::radians(pitch)),
            sin(glm::radians(yaw)) * cos(glm::radians(pitch))));

        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        cachedViewMatrix = glm::lookAt(position, position + front, up);
        viewInvalidated = false;
    }

    return cachedViewMatrix;
}

void Camera::invalidateCachedViewMatrix()
{
    viewInvalidated = true;
}

glm::mat4& Camera::getProjectionMatrix()
{
    if (projectionInvalidated)
    {
        cachedProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 500.0f);
        projectionInvalidated = false;
    }

    return cachedProjectionMatrix;
}

void Camera::invalidateCachedProjectionMatrix()
{
    projectionInvalidated = true;
}

void Camera::setFov(float fov)
{
    this->fov = fov;

    invalidateCachedProjectionMatrix();
}

void Camera::setAspectRatio(float aspectRatio)
{
    this->aspectRatio = aspectRatio;

    invalidateCachedProjectionMatrix();
}

void Camera::setRenderDistance(float near, float far)
{
    this->near = near;
    this->far = far;

    invalidateCachedProjectionMatrix();
}