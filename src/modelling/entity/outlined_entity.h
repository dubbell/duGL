#ifndef OUTLINED_ENTITY_H
#define OUTLINED_ENTITY_H


#include "common.h"
#include "entity.h"
#include "observers/mouse_observer.h"


class OutlinedEntity : public Entity, public ScreenRayObserver
{
public:
    OutlinedEntity(Renderable* renderable, Shader* outlineShader);
    OutlinedEntity(Renderable* renderable, glm::vec3 position, Shader* outlineShader);

    void render(Shader* shader) override;
    void observeRay(glm::vec3 rayOrigin, glm::vec3 rayDirection) override;

private:
    bool enableOutline;
    float outlineThickness;
    Shader* outlineShader;
};

#endif