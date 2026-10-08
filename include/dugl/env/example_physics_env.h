#pragma once

#include "physics_env.h"
#include "dugl/controllers/flight_controller.h"


class ExamplePhysicsEnvironment : public PhysicsEnvironment
{
private:
    FlightController flightController;

    PhysicsEntity* box = nullptr;
    bool kickHeld = false;

public:
    ExamplePhysicsEnvironment() : flightController(this, &keyboardController, &mouseController) {}

protected:
    void init() override;
    void update(float dt) override;
    void prePhysicsStep(float stepDt) override;
};
