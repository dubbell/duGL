#pragma once

#include "dugl/env/physics_env.h"
#include "dugl/controllers/flight_controller.h"


class HeightFieldEnvironment : public PhysicsEnvironment
{
private:
    FlightController flightController;

    RigidEntity* sphere = nullptr;
    bool resetHeld = false;

public:
    HeightFieldEnvironment() : flightController(this, &keyboardController, &mouseController) {}

protected:
    void init() override;
    void update(float dt) override;

private:
    void resetSphere();
};
