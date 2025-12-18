#ifndef MOUSE_H
#define MOUSE_H


#include <GLFW/glfw3.h>
#include <memory>
#include <set>

#include "controllers/interfaces/mouse_controllable.h"


class MouseController
{
private:
    std::set<std::shared_ptr<MouseControllable>> observers;

    float lastX, lastY;
    float sensitivity;
    bool firstMouse;

public:
    MouseController();

    void registerObserver(std::shared_ptr<MouseControllable> observer);
    void unregisterObserver(std::shared_ptr<MouseControllable> observer);

    void cursorPosCallback(float xpos, float ypos);
};


#endif