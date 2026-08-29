#pragma once

#include <set>

#include "mouse_observer.h"
#include "dugl/interfaces/perspective_interface.h"


class MouseController
{
private:
    PerspectiveInterface* perspective;

    std::set<ScreenRayObserver*> screenRayObservers;
    std::set<MouseOffsetObserver*> mouseOffsetObservers;

    float lastX, lastY;
    float sensitivity;
    bool firstMouse;

    void handleCursorPosition(float xPos, float yPos);
    void handleScreenRay(float xPos, float yPos);

public:
    MouseController(PerspectiveInterface* perspective);

    void registerOffsetObserver(MouseOffsetObserver* observer);
    void unregisterOffsetObserver(MouseOffsetObserver* observer);

    void registerScreenRayObserver(ScreenRayObserver* observer);
    void unregisterScreenRayObserver(ScreenRayObserver* observer);

    void processInput();
};