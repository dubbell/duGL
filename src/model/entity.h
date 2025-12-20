#ifndef ENTITY_H
#define ENTITY_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shading/shader.h"
#include "model/renderable.h"


class Entity
{
public:
    Entity(Renderable* renderable);
    Entity(Renderable* renderable, glm::vec3 position);

    void draw(Shader* shader);

    void setPosition(glm::vec3 position);

    glm::mat4 getModelTransform();

private:
    Renderable* renderable;
    glm::vec3 position;
};

#endif