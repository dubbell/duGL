#pragma once

#include "entity.h"
#include "dugl/controllers/mouse_observer.h"

class MouseController;

class OutlinedEntity : public Entity, public ScreenRayObserver
{
private:
    bool enableOutline;
    float outlineThickness;
    Shader* outlineShader;

    MouseController* mouseController;

public:
    OutlinedEntity(Renderable* renderable, Shader* outlineShader, MouseController* mouseController);
    OutlinedEntity(Renderable* renderable, glm::vec3 position, Shader* outlineShader, MouseController* mouseController);
    ~OutlinedEntity();

    // registration with the mouse controller is tied to this object's lifetime; disable
    // copy/move so a duplicate/relocated instance can't end up registered twice or unregister
    // the wrong instance.
    OutlinedEntity(const OutlinedEntity&) = delete;
    OutlinedEntity& operator=(const OutlinedEntity&) = delete;
    OutlinedEntity(OutlinedEntity&&) = delete;
    OutlinedEntity& operator=(OutlinedEntity&&) = delete;

    void render(Shader* shader) override;
    void observeRay(glm::vec3 rayOrigin, glm::vec3 rayDirection) override;
};