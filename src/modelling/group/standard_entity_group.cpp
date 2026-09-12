#include "dugl/modelling/standard_entity_group.h"
#include "dugl/common.h"

#include <algorithm>


StandardEntityGroup::StandardEntityGroup(Shader* shader) : shader(dugl::requireNonNull(shader)) {}

void StandardEntityGroup::render()
{
    shader->use();
    for (Entity* entity : entities)
    {
        entity->render(shader);
    }
}

void StandardEntityGroup::addEntity(Entity* entity)
{
    dugl::requireNonNull(entity);
    entities.push_back(entity);
}

void StandardEntityGroup::removeEntity(Entity* entity)
{
    entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
}

void StandardEntityGroup::clear()
{
    entities.clear();
}

void StandardEntityGroup::setShader(Shader* shader)
{
    this->shader = dugl::requireNonNull(shader);
}

Shader* StandardEntityGroup::getShader() const
{
    return shader;
}
