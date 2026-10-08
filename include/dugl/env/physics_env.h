#pragma once

#include "env.h"
#include "dugl/async/dispatcher.h"
#include "dugl/physics/common.h"
#include "dugl/physics/collision.h"
#include "dugl/physics/physics_entity.h"
#include "dugl/modelling/scene.h"

#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include <map>
#include <utility>
#include <vector>


// An Environment with Jolt physics enabled.
class PhysicsEnvironment : public Environment
{
private:
    std::vector<CollisionEvent> collisionEvents;
    std::map<std::pair<JPH::BodyID, JPH::BodyID>, bool> sleepingContacts;

protected:
    JPH::TempAllocatorImpl tempAllocator;

    ObjectToBroadPhaseMapper broadPhaseMapper;
    ObjectCollisionFilter objectCollisionFilter;
    ObjectBroadPhaseFilter objectBroadPhaseFilter;
    ContactRecorder contactRecorder;

    JPH::PhysicsSystem physicsSystem;

    float accumulator = 0.0f;

public:
    PhysicsEnvironment();
    virtual ~PhysicsEnvironment();

protected:
    void physicsUpdate(float dt);
    virtual void prePhysicsStep(float stepDt) {}

    const std::vector<CollisionEvent>& getCollisionEvents() const { return collisionEvents; }

private:
    void collectCollisionEvents();
    void addCollisionEvent(const RecordedContact& contact);
};
