#pragma once

#include <glm/glm.hpp>


class MouseOffsetObserver
{
public:
    virtual void cursorOffsetCallback(float xOffset, float yOffset) = 0;
};

class ScreenRayObserver
{
public:
    virtual void observeRay(glm::vec3 rayOrigin, glm::vec3 rayDirection) = 0;
};