#ifndef ENTITY_H
#define ENTITY_H

#include <renderable.h>


class Entity
{
private:
    glm::vec3 position;

public:
    Entity(glm::vec3 &position);

    glm::vec3 getPosition();
};


#endif