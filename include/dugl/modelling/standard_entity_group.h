#pragma once

#include "entity_group.h"
#include "dugl/shading/shader.h"


// Renders its entities in a single pass with one shader.
class StandardEntityGroup : public EntityGroup
{
private:
    Shader* shader;
    std::vector<Entity*> entities;

public:
    StandardEntityGroup(Shader* shader);

    void render() override;

    void addEntity(Entity* entity);
    void removeEntity(Entity* entity);
    void clear();

    void setShader(Shader* shader);
    Shader* getShader() const;
};
