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
    MouseController();
    MouseController(Camera* camera);

    void setCamera(Camera* camera);

    void cursorPosCallback(float xpos, float ypos);
};


#endif