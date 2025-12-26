#ifndef MOUSE_CONTROLLABLE_H
#define MOUSE_CONTROLLABLE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


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

#endif