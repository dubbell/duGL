#pragma once

#include "dugl/shading/shader.h"
#include "dugl/modelling/renderable.h"


// An Entity is the world space representation of a Renderable. While
// each Renderable is singular, multiple Entity objects with the same
// Renderable can exist and be rendered on the screen simultaneously.
class Entity
{
protected:
    Renderable* renderable;
    glm::vec3 position;

public:
    Entity(Renderable* model);
    Entity(Renderable* model, glm::vec3 position);
    virtual ~Entity() = default;

    virtual void render(Shader* shader);

    void setPosition(glm::vec3 position);
    glm::vec3& getPosition();

    glm::mat4 getModelTransform();
};