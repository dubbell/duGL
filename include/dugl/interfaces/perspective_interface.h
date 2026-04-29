#pragma once

#include "dugl/view/camera.h"


class PerspectiveInterface
{
public:
    virtual Camera* getActiveCamera() = 0;

    virtual ~PerspectiveInterface() = default;
};