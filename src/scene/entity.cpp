#include <entity.h>



Entity::Entity(glm::vec3 &position) : position(position) {};

glm::vec3 Entity::getPosition()
{
    return position;
}