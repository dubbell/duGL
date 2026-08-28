#pragma once

#include "env.h"
#include "dugl/async/dispatcher.h"
#include "dugl/physics/common.h"

#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>


// And Environment with Jolt physics enabled.
class JoltedEnvironment : public Environment
{
protected:
    JPH::TempAllocatorImpl tempAllocator;
    dugl::AsyncDispatcher dispatcher;

    ObjectToBroadPhaseMapper broadPhaseMapper;
    ObjectCollisionFilter objectCollisionFilter;
    ObjectBroadPhaseFilter objectBroadPhaseFilter;

    JPH::PhysicsSystem physicsSystem;

public:
    JoltedEnvironment();
    virtual ~JoltedEnvironment() = default;

    void start() override;

protected:
    virtual void update(float dt) {}
    virtual void render() {}

    void frameBufferResizeCallback(int width, int height) override;
};
