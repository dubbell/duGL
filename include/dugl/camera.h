#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <format>

#include "keyboard_observer.h"


class Camera
{
private:
    float maxSpeed;
    float yaw, pitch;

    glm::vec3 position;
    glm::vec3 velocity;

    float fov, aspectRatio, near, far;  // projection matrix parameters

    bool viewInvalidated, projectionInvalidated;
    glm::mat4 cachedViewMatrix, cachedProjectionMatrix;

    glm::mat3 getLocalBasis();

    glm::vec3 toGlobal(glm::vec3 local);

public:
    Camera();

    void setPosition(glm::vec3 &newPosition);
    void updatePosition();

    void setVelocity(glm::vec3 &newVelocity);
    void setLocalVelocity(glm::vec3 &newLocalVelocity);
    
    void accelerate(glm::vec3 &acceleration);
    void accelerateLocally(glm::vec3 &acceleration);

    void turnRight(float deltaYaw);
    void turnUp(float deltaPitch);

    glm::vec3& getPosition();

    void invalidateCachedViewMatrix();
    glm::mat4& getViewMatrix();

    void invalidateCachedProjectionMatrix();
    glm::mat4& getProjectionMatrix();

    void setFov(float fov);
    void setAspectRatio(float aspectRatio);
    void setRenderDistance(float near, float far);
};

#endif