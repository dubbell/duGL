#pragma once

#include <optional>
#include <set>

#include "mouse_observer.h"
#include "dugl/interfaces/perspective_interface.h"
#include "dugl/utils/common.h"


class MouseController
{
private:
    PerspectiveInterface* perspective;

    std::set<MouseOffsetObserver*> mouseOffsetObservers;
    std::optional<Ray> screenRay;

    float lastX, lastY;
    float sensitivity;
    bool firstMouse;

    void handleCursorPosition(float xPos, float yPos);
    Ray computeScreenRay(float xPos, float yPos);

public:
    MouseController(PerspectiveInterface* perspective);

    void registerOffsetObserver(MouseOffsetObserver* observer);
    void unregisterOffsetObserver(MouseOffsetObserver* observer);

    void processInput();

    std::optional<Ray> getScreenRay() const;
};