#pragma once

#include "env.h"
#include "dugl/async/dispatcher.h"
#include "dugl/physics/common.h"
#include "dugl/physics/physics_entity.h"
#include "dugl/modelling/scene.h"

#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>


// An Environment with Jolt physics enabled.
class PhysicsEnvironment : public Environment
{
protected:
    JPH::TempAllocatorImpl tempAllocator;

    ObjectToBroadPhaseMapper broadPhaseMapper;
    ObjectCollisionFilter objectCollisionFilter;
    ObjectBroadPhaseFilter objectBroadPhaseFilter;

    JPH::PhysicsSystem physicsSystem;

    float accumulator = 0.0f;

public:
    PhysicsEnvironment();
    virtual ~PhysicsEnvironment();

protected:
    void update(float dt) override;

    void frameBufferResizeCallback(int width, int height) override;
};
