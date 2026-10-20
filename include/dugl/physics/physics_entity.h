#pragma once

#include "dugl/modelling/entity.h"
#include "dugl/physics/collision.h"
#include "dugl/physics/anchored_constraints.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include <vector>


class PhysicsEnvironment;

/* Base for entities simulated by one or more JoltPhysics bodies. */
class PhysicsEntity : public Entity
{
    friend class PhysicsEnvironment;

protected:
    JPH::PhysicsSystem& physicsSystem;
    JPH::BodyInterface& bodyInterface;
    AnchoredConstraints constraints;

private:
    struct PendingForce
    {
        JPH::BodyID bodyId;
        JPH::Vec3 force = JPH::Vec3::sZero();
        JPH::Vec3 torque = JPH::Vec3::sZero();
    };

    std::vector<CollisionEvent> pendingCollisions;
    std::vector<PendingForce> pendingForces;

public:
    PhysicsEntity(Renderable* renderable, Shader* shader, JPH::PhysicsSystem& physicsSystem);
    virtual ~PhysicsEntity() = default;

    PhysicsEntity(const PhysicsEntity&) = delete;
    PhysicsEntity& operator=(const PhysicsEntity&) = delete;

    /* Set the anchor point for an existing constraint on the entity. */
    void setConstraintAnchor(dugl::uint constraintId, glm::vec3 anchorInWorldSpace);
    /* Remove a constraint from the entity. */
    void removeConstraint(dugl::uint constraintId);
    /* True if the entity has a constraint with the given ID. */
    bool hasConstraint(dugl::uint constraintId) const;

protected:
    /* Make collisions with the body report to this entity. */
    void registerBody(JPH::BodyID bodyId);
    /* Copy the state of the entity's bodies into the entity. */
    virtual void syncWithPhysics() = 0;
    /* Called when something collides with the entity. */
    virtual void onCollision(const CollisionEvent& event, PhysicsEntity* other) {}

    void addForce(JPH::BodyID bodyId, JPH::Vec3 force);
    void addForce(JPH::BodyID bodyId, JPH::Vec3 force, JPH::RVec3 point);
    void addTorque(JPH::BodyID bodyId, JPH::Vec3 torque);

private:
    void preUpdate(float dt) override final;
    PendingForce& getPendingForce(JPH::BodyID bodyId);
    void applyPendingForces();
    void clearPendingForces();
};
