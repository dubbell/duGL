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

private:
    std::vector<CollisionEvent> pendingCollisions;

    void preUpdate(float dt) override final;

protected:
    JPH::BodyInterface& bodyInterface;
    JPH::BodyID bodyId;

    virtual void onCollision(const CollisionEvent& event, PhysicsEntity* other) {}

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
};
