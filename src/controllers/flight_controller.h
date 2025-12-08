#ifndef FLIGHT_CONTROLLER_H
#define FLIGHT_CONTROLLER_H

#include <memory>

#include <keyboard_controllable.h>
#include <mouse_controllable.h>
#include <camera.h>


class FlightController : public KeyboardControllable, public MouseControllable
{
private:
    float speed;
    std::shared_ptr<Camera> camera;

public:
    FlightController(std::shared_ptr<Camera> &camera);

    void processKeyboardInput(std::map<int, int> &keyboardState);
    void cursorPosCallback(float xOffset, float yOffset);
};

#endif