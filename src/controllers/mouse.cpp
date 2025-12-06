#include "mouse.h"


MouseController::MouseController(Camera* camera)
{
    lastX, lastY = 0.0f, 0.0f;
    sensitivity = 0.1f;
    firstMouse = true;
}

void MouseController::mouseCallback(GLFWwindow* window, double xpos, double ypos)
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
