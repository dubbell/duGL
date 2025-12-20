#include "mouse.h"


MouseController::MouseController()
    : lastX(0.0f), lastY(0.0f), sensitivity(0.05f), firstMouse(true) 
{}


void MouseController::registerObserver(std::shared_ptr<MouseControllable> observer)
{
    observers.insert(observer);
}

void MouseController::unregisterObserver(std::shared_ptr<MouseControllable> observer)
{
    observers.erase(observer);
}

void MouseController::cursorPosCallback(float xpos, float ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = (xpos - lastX) * sensitivity;
    float yOffset = (lastY - ypos) * sensitivity; 
    lastX = xpos;
    lastY = ypos;

    for (auto& observer : observers)
        observer->cursorPosCallback(xOffset, yOffset);
}
