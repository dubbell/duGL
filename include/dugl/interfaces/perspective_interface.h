#pragma once

#include "dugl/view/camera.h"


// Abstract interface for accessing an object's Camera.
class PerspectiveInterface
{
public:
    virtual ~PerspectiveInterface() = default;
    virtual Camera* getActiveCamera() = 0;
};