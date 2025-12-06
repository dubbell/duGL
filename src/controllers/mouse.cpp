#include "mouse.h"


MouseController::MouseController()
    : lastX(0.0f), lastY(0.0f), sensitivity(0.1f), firstMouse(true) {}

MouseController::MouseController(Camera* camera) 
    : camera(camera), lastX(0.0f), lastY(0.0f), sensitivity(0.1f), firstMouse(true) {}


    
void MouseController::setCamera(Camera* camera)
{
    this->camera = camera;
}

void MouseController::cursorPosCallback(float xpos, float ypos)
{
    if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

    float xoffset = (xpos - lastX) * sensitivity;
    float yoffset = (lastY - ypos) * sensitivity; 
    lastX = xpos;
    lastY = ypos;
    
    (*camera).turnRight(xoffset);
    (*camera).turnUp(yoffset);
}
