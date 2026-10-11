#pragma once

#include "dugl/modelling/entity.h"
#include "dugl/physics/collision.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include <vector>


class PhysicsEnvironment;

class PhysicsEntity : public Entity
{
    friend class PhysicsEnvironment;

protected:
    JPH::BodyInterface& bodyInterface;
    JPH::BodyID bodyId;

private:
    JPH::EMotionType motionType;
    bool frozen = false;
    std::vector<CollisionEvent> pendingCollisions;

public:
    PhysicsEntity(Renderable* renderable, Shader* shader, JPH::BodyInterface& bodyInterface, const JPH::BodyCreationSettings& settings);
    ~PhysicsEntity();

    PhysicsEntity(const PhysicsEntity&) = delete;
    PhysicsEntity& operator=(const PhysicsEntity&) = delete;

    JPH::BodyID getBodyId() const;

    void applyForce(glm::vec3 force);
    void applyForce(glm::vec3 force, glm::vec3 point);
    void applyTorque(glm::vec3 torque);

    void applyImpulse(glm::vec3 impulse);
    void applyImpulse(glm::vec3 impulse, glm::vec3 point);
    void applyAngularImpulse(glm::vec3 angularImpulse);

    void setFrozen(bool frozen);
    bool isFrozen() const;

protected:
    virtual void onCollision(const CollisionEvent& event, PhysicsEntity* other) {}

private:
    void preUpdate(float dt) override final;
};
