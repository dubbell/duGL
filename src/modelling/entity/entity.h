#ifndef ENTITY_H
#define ENTITY_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shading/shader.h"
#include "modelling/renderable/renderable.h"


class Entity
{
public:
    Entity(Renderable* model);
    Entity(Renderable* model, glm::vec3 position);

    virtual void render();

    void setPosition(glm::vec3 position);
    glm::vec3 getPosition() const;

    glm::mat4 getModelTransform();

protected:
    Renderable* renderable;
    glm::vec3 position;
};


#endif