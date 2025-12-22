#ifndef OUTLINED_ENTITY_H
#define OUTLINED_ENTITY_H

#include "entity.h"


class OutlinedEntity : public Entity
{
public:
    OutlinedEntity(Model* model, Shader* outlineShader);
    OutlinedEntity(Model* model, glm::vec3 position, Shader* outlineShader);

    virtual void render() override;

private:
    bool enableOutline;
    Shader* outlineShader;
};

#endif