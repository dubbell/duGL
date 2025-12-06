#ifndef MOUSE_H
#define MOUSE_H


#include <GLFW/glfw3.h>

#include "camera.h"


class MouseController
{
private:
    Camera* camera;
    float lastX, lastY;
    float sensitivity;
    bool firstMouse;

public:
    MouseController(Camera* camera);

    void mouseCallback(GLFWwindow* window, double xpos, double ypos);
};


#endif