#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Basis
{
public:
    const glm::vec3 front, right, up;
    Basis(glm::vec3 front, glm::vec3 right, glm::vec3 up) : front(front), right(right), up(up) {};
};


class Camera
{
private:
    float maxSpeed;
    float yaw, pitch;

    glm::vec3 position;
    glm::vec3 velocity;

    Basis getLocalBasis();

public:
    Camera(glm::vec3 &startingPosition, float maxSpeed);

    void setPosition(glm::vec3 &newPosition);

    void setVelocity(glm::vec3 &newVelocity);
    void setLocalVelocity(glm::vec3 &newLocalVelocity);
    
    void accelerate(glm::vec3 &acceleration);
    void accelerateLocally(glm::vec3 &acceleration);

    void turnRight(float deltaYaw);
    void turnUp(float deltaPitch);

    glm::mat4 getView() const;
};

#endif