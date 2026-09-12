#pragma once

#include "entity.h"
#include "dugl/controllers/mouse_observer.h"

class MouseController;

// An Entity that tracks whether the cursor is currently hovering over it.
class HoverableEntity : public Entity, public ScreenRayObserver
{
protected:
    bool hovered;
    float hoverRadius;

    MouseController* mouseController;

public:
    HoverableEntity(Renderable* renderable, MouseController* mouseController);
    HoverableEntity(Renderable* renderable, glm::vec3 position, MouseController* mouseController);
    ~HoverableEntity();

    // registration with the mouse controller is tied to this object's lifetime; disable
    // copy/move so a duplicate/relocated instance can't end up registered twice or unregister
    // the wrong instance.
    HoverableEntity(const HoverableEntity&) = delete;
    HoverableEntity& operator=(const HoverableEntity&) = delete;
    HoverableEntity(HoverableEntity&&) = delete;
    HoverableEntity& operator=(HoverableEntity&&) = delete;

    void observeRay(glm::vec3 rayOrigin, glm::vec3 rayDirection) override;

    bool isHovered() const;

    void setHoverRadius(float hoverRadius);
    float getHoverRadius() const;
};
