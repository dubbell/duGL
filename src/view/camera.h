#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include <keyboard_controllable.h>


class Camera
{
private:
    float maxSpeed;
    float yaw, pitch;
    float fov, aspectRatio;

    glm::mat4 projectionMatrix;

    glm::vec3 position;
    glm::vec3 velocity;

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

    void setFov(float FOV);
    void setAspectRatio(float AR);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
};

#endif