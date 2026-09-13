#pragma once

#include "physics_env.h"
#include "dugl/controllers/flight_controller.h"


class ExamplePhysicsEnvironment : public PhysicsEnvironment
{
private:
    FlightController flightController;

public:
    ExamplePhysicsEnvironment() : flightController(this, &keyboardController, &mouseController) {}

protected:
    void init() override;
};
