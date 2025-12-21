#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <format>

#include "controllers/interfaces/keyboard_controllable.h"


class Camera
{
private:
    float maxSpeed;
    float yaw, pitch;

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

    glm::vec3 getPosition() const;

    glm::mat4 getViewMatrix() const;
};

#endif