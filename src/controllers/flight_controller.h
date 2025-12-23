#ifndef FLIGHT_CONTROLLER_H
#define FLIGHT_CONTROLLER_H

#include <memory>
#include <set>

#include "app_state.h"
#include "controllers/interfaces/keyboard_controllable.h"
#include "controllers/interfaces/mouse_controllable.h"
#include "view/camera.h"


class FlightController : public KeyboardControllable, public MouseControllable
{
private:
    GLFWwindow* window;
    Camera* camera;

    float speed;
    ApplicationState* applicationState;

public:
    FlightController(GLFWwindow* window, Camera* camera, ApplicationState* applicationState);

    std::set<int> getActiveKeys() override;
    std::set<int> getActiveTogglableKeys() override;
    std::set<int> getActiveSingleTriggerKeys() override;

    void processKeyboardInput(KeyboardState& keyboardState) override;
    void cursorPosCallback(float xOffset, float yOffset) override;
};

#endif