#ifndef FLIGHT_CONTROLLER_H
#define FLIGHT_CONTROLLER_H

#include <memory>
#include <set>

#include "controllers/interfaces/keyboard_controllable.h"
#include "controllers/interfaces/mouse_controllable.h"
#include "view/camera.h"


class FlightController : public KeyboardControllable, public MouseControllable
{
private:
    GLFWwindow* window;
    Camera* camera;

    float speed;
    bool freeCursor;

public:
    FlightController(GLFWwindow* window, Camera* camera);

    std::set<int> getActiveKeys() override;
    std::set<int> getActiveTogglableKeys() override;
    std::set<int> getActiveSingleTriggerKeys() override;

    void processKeyboardInput(KeyboardState& keyboardState) override;
    void cursorPosCallback(float xOffset, float yOffset) override;
};

#endif