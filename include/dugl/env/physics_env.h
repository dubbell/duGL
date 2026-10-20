#pragma once

#include "env.h"
#include "dugl/async/dispatcher.h"
#include "dugl/physics/common.h"
#include "dugl/physics/collision.h"
#include "dugl/physics/physics_entity.h"
#include "dugl/physics/ragdoll_builder.h"
#include "dugl/physics/ragdoll_entity.h"
#include "dugl/physics/body_settings.h"
#include "dugl/modelling/scene.h"

#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include <map>
#include <utility>
#include <vector>


// An Environment with Jolt physics enabled.
class PhysicsEnvironment : public Environment
{
protected:
    JPH::TempAllocatorImpl tempAllocator;

    ObjectToBroadPhaseMapper broadPhaseMapper;
    ObjectCollisionFilter objectCollisionFilter;
    ObjectBroadPhaseFilter objectBroadPhaseFilter;
    ContactRecorder contactRecorder;

    JPH::PhysicsSystem physicsSystem;

    float accumulator = 0.0f;

private:
    std::vector<CollisionEvent> collisionEvents;
    std::map<std::pair<JPH::BodyID, JPH::BodyID>, bool> sleepingContacts;

public:
    PhysicsEnvironment();
    virtual ~PhysicsEnvironment();

protected:
    using Environment::createEntity;

    template<typename T = PhysicsEntity> requires std::is_base_of_v<PhysicsEntity, T>
    T* createEntity(PrimitiveBuilder& builder, Shader* shader, const PhysicsMaterial& material) {
        return Environment::createEntity<T>(builder, shader, physicsSystem, createBodySettings(builder, material));
    }

    template<typename T = RagdollEntity> requires std::is_base_of_v<RagdollEntity, T>
    T* createRagdoll(RagdollBuilder& builder, Shader* shader) {
        return Environment::createEntity<T>(createRenderable(builder), shader, physicsSystem, builder);
    }

    void physicsUpdate(float dt);
    virtual void prePhysicsStep(float stepDt) {}

    const std::vector<CollisionEvent>& getCollisionEvents() const { return collisionEvents; }

private:
    void collectCollisionEvents();
    void addCollisionEvent(const RecordedContact& contact);
};
