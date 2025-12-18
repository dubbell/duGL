#ifndef FLIGHT_CONTROLLER_H
#define FLIGHT_CONTROLLER_H

#include <memory>
#include <vector>

#include "controllers/interfaces/keyboard_controllable.h"
#include "controllers/interfaces/mouse_controllable.h"
#include "view/camera.h"


class FlightController : public KeyboardControllable, public MouseControllable
{
private:
    GLFWwindow* window;
    Camera* camera;
    std::vector<int> activeKeys;

    float speed;    
    bool escapeHold, freeCursor;

public:
    FlightController(GLFWwindow* window, Camera* camera);

    std::vector<int> getActiveKeys();

    void processKeyboardInput(std::map<int, int> &keyboardState);
    void cursorPosCallback(float xOffset, float yOffset);
};

#endif