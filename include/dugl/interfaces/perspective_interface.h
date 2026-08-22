#pragma once

#include "dugl/view/camera.h"


class PerspectiveInterface
{
public:
    virtual ~PerspectiveInterface() = default;
    virtual Camera* getActiveCamera() = 0;
};