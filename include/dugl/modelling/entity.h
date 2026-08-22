#pragma once

#include "dugl/shading/shader.h"
#include "dugl/modelling/renderable.h"


class Entity
{
protected:
    Renderable* renderable;
    glm::vec3 position;

public:
    Entity(Renderable* model);
    Entity(Renderable* model, glm::vec3 position);

    virtual void render(Shader* shader);

    void setPosition(glm::vec3 position);
    glm::vec3& getPosition();

    glm::mat4 getModelTransform();
};