#include "dugl/modelling/hoverable_entity.h"
#include "dugl/utils/common.h"
#include "dugl/controllers/mouse.h"


HoverableEntity::HoverableEntity(Renderable* renderable, MouseController* mouseController)
    : Entity(renderable), hovered(false), hoverRadius(1.0f), mouseController(mouseController)
{
    mouseController->registerScreenRayObserver(this);
}

HoverableEntity::HoverableEntity(Renderable* renderable, glm::vec3 position, MouseController* mouseController)
    : Entity(renderable, position), hovered(false), hoverRadius(1.0f), mouseController(mouseController)
{
    mouseController->registerScreenRayObserver(this);
}

HoverableEntity::~HoverableEntity()
{
    mouseController->unregisterScreenRayObserver(this);
}

void HoverableEntity::observeRay(glm::vec3 rayOrigin, glm::vec3 rayDirection)
{
    hovered = checkRayIntersection(position, hoverRadius, rayOrigin, rayDirection);
}

bool HoverableEntity::isHovered() const
{
    return hovered;
}

void HoverableEntity::setHoverRadius(float hoverRadius)
{
    this->hoverRadius = hoverRadius;
}

float HoverableEntity::getHoverRadius() const
{
    return hoverRadius;
}
