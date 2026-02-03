#ifndef MOUSE_H
#define MOUSE_H


#include <GLFW/glfw3.h>
#include <memory>
#include <set>

#include "mouse_observer.h"
#include "player_interface.h"
#include "common.h"


class MouseController
{
private:
    PlayerInterface* screenCastInterface;

    std::set<ScreenRayObserver*> screenRayObservers;
    std::set<MouseOffsetObserver*> mouseOffsetObservers;

    float lastX, lastY;
    float sensitivity;
    bool firstMouse;

    void handleCursorPosition(float xPos, float yPos);
    void handleScreenRay(float xPos, float yPos);

public:
    MouseController(PlayerInterface* screenCastInterface);

    void registerOffsetObserver(MouseOffsetObserver* observer);
    void unregisterOffsetObserver(MouseOffsetObserver* observer);

    void registerScreenRayObserver(ScreenRayObserver* observer);
    void unregisterScreenRayObserver(ScreenRayObserver* observer);

    void processMouseInput();
};


#endif