#ifndef FLIGHT_CONTROLLER_H
#define FLIGHT_CONTROLLER_H

#include <memory>
#include <set>

#include "keyboard_observer.h"
#include "mouse_observer.h"
#include "camera.h"
#include "player_interface.h"


class FlightController : public KeyboardObserver, public MouseOffsetObserver
{
private:
    PlayerInterface* playerInterface;

    float speed;

public:
    FlightController(PlayerInterface* playerInterface);

    std::set<int> getActiveKeys() override;
    std::set<int> getActiveTogglableKeys() override;
    std::set<int> getActiveSingleTriggerKeys() override;

    void processKeyboardInput(KeyboardState& keyboardState) override;
    void cursorOffsetCallback(float xOffset, float yOffset) override;
};

#endif