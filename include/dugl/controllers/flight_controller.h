#pragma once

#include <memory>
#include <set>

#include "keyboard_observer.h"
#include "mouse_observer.h"
#include "dugl/interfaces/player_interface.h"

class KeyboardController;
class MouseController;

class FlightController : public KeyboardObserver, public MouseOffsetObserver
{
private:
    PlayerInterface* playerInterface;

    KeyboardController* keyboardController;
    MouseController* mouseController;

    float speed;

public:
    FlightController(PlayerInterface* playerInterface, KeyboardController* keyboardController, MouseController* mouseController);
    ~FlightController();

    // Registration with the keyboard/mouse controllers is tied to this object's lifetime; disable
    // copy/move so a duplicate/relocated instance can't end up registered twice or unregister
    // the wrong instance.
    FlightController(const FlightController&) = delete;
    FlightController& operator=(const FlightController&) = delete;
    FlightController(FlightController&&) = delete;
    FlightController& operator=(FlightController&&) = delete;

    std::set<int> getActiveKeys() override;
    std::set<int> getActiveTogglableKeys() override;
    std::set<int> getActiveSingleTriggerKeys() override;

    void processKeyboardInput(KeyboardState& keyboardState) override;
    void cursorOffsetCallback(float xOffset, float yOffset) override;
};