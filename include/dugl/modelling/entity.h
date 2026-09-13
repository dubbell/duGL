#pragma once

#include "dugl/shading/shader.h"
#include "dugl/modelling/renderable.h"

#include <glm/gtc/quaternion.hpp>

// An Entity is the world space representation of a Renderable. While
// each Renderable is singular, multiple Entity objects with the same
// Renderable can exist and be rendered on the screen simultaneously.
class Entity
{
protected:
    Renderable* renderable;
    glm::vec3 position;
    glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
    glm::vec3 velocity{ 0.0f };
    glm::vec3 angularVelocity{ 0.0f };

public:
    Entity(Renderable* model);
    Entity(Renderable* model, glm::vec3 position);
    virtual ~Entity() = default;

    virtual void update(float dt) {}
    virtual void render(Shader* shader);

    void setPosition(glm::vec3 position);
    glm::vec3& getPosition();

    void setRotation(glm::quat rotation);
    glm::quat& getRotation();

    void setVelocity(glm::vec3 velocity);
    glm::vec3& getVelocity();

    void setAngularVelocity(glm::vec3 angularVelocity);
    glm::vec3& getAngularVelocity();

    glm::mat4 getModelTransform();
};