#include "entity.h"



Entity::Entity(Renderable* renderable) : renderable(renderable), position(0.0f, 0.0f, 0.0f) {}


Entity::Entity(Renderable* renderable, glm::vec3 position) : renderable(renderable), position(position) {}


void Entity::render()
{
    renderable->render();
}


void Entity::setPosition(glm::vec3 position)
{
    this->position = position;
}


glm::vec3 Entity::getPosition() const
{
    return position;
}


glm::mat4 Entity::getModelTransform()
{
    return glm::translate(glm::mat4(1.0f), position);
}