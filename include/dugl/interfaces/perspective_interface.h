#pragma once

#include "window_interface.h"

#include "dugl/view/camera.h"


// Abstract interface for accessing window properties and the perspective
// that is displayed in the window.
class PerspectiveInterface : public WindowInterface
{
public:
    virtual Camera* getActiveCamera() = 0;
};
