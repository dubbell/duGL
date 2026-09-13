#include "dugl/modelling/entity.h"

#include <glm/gtc/matrix_transform.hpp>


Entity::Entity(Renderable* renderable) : renderable(renderable), position(0.0f, 0.0f, 0.0f) {}


Entity::Entity(Renderable* renderable, glm::vec3 position) : renderable(renderable), position(position) {}


void Entity::render(Shader* shader)
{
    shader->setMat4("model", getModelTransform());
    renderable->render(shader);
}

void Entity::setPosition(glm::vec3 position)
{
    this->position = position;
}


glm::vec3& Entity::getPosition()
{
    return position;
}

void Entity::setRotation(glm::quat rotation)
{
    this->rotation = rotation;
}

glm::quat& Entity::getRotation()
{
    return rotation;
}

void Entity::setVelocity(glm::vec3 velocity)
{
    this->velocity = velocity;
}

glm::vec3& Entity::getVelocity()
{
    return velocity;
}

void Entity::setAngularVelocity(glm::vec3 angularVelocity)
{
    this->angularVelocity = angularVelocity;
}

glm::vec3& Entity::getAngularVelocity()
{
    return angularVelocity;
}

glm::mat4 Entity::getModelTransform()
{
    return glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotation);
}
