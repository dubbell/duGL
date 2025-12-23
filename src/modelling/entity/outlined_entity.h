#ifndef OUTLINED_ENTITY_H
#define OUTLINED_ENTITY_H

#include "entity.h"


class OutlinedEntity : public Entity
{
public:
    OutlinedEntity(Renderable* renderable, Shader* outlineShader);
    OutlinedEntity(Renderable* renderable, glm::vec3 position, Shader* outlineShader);

    virtual void render(Shader* shader) override;

private:
    bool enableOutline;
    Shader* outlineShader;
};

#endif