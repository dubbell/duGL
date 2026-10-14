#pragma once

#include "dugl/modelling/entity.h"
#include "dugl/physics/collision.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Constraints/TwoBodyConstraint.h>

#include <unordered_map>
#include <vector>


class PhysicsEnvironment;

struct DistanceConstraintParams
{
    glm::vec3 pointInEntitySpace{ 0.0f };
    glm::vec3 anchorInWorldSpace{ 0.0f };
    float minDistance = 0.0f;
    float maxDistance = 0.0f;
    float frequency = 0.0f;
    float damping = 0.0f;
};

struct PointConstraintParams
{
    glm::vec3 pointInEntitySpace{ 0.0f };
    glm::vec3 anchorInWorldSpace{ 0.0f };
    float frequency = 0.0f;
    float damping = 0.0f;
};

struct AnchoredConstraint
{
    JPH::BodyID anchorId;
    JPH::Ref<JPH::Constraint> constraint;
};

class PhysicsEntity : public Entity
{
    friend class PhysicsEnvironment;

protected:
    JPH::PhysicsSystem& physicsSystem;
    JPH::BodyInterface& bodyInterface;
    JPH::BodyID bodyId;

private:
    JPH::EMotionType motionType;
    bool frozen = false;
    std::vector<CollisionEvent> pendingCollisions;
    std::unordered_map<dugl::uint, AnchoredConstraint> constraints;
    dugl::uint nextConstraintId = 0;

public:
    PhysicsEntity(Renderable* renderable, Shader* shader, JPH::PhysicsSystem& physicsSystem, const JPH::BodyCreationSettings& settings);
    ~PhysicsEntity();

    PhysicsEntity(const PhysicsEntity&) = delete;
    PhysicsEntity& operator=(const PhysicsEntity&) = delete;

    /* Get the ID of the entity's JoltPhysics body. */
    JPH::BodyID getBodyId() const;

    /* Freeze or unfreeze entity. Freezing makes the entity static. */
    void setFrozen(bool frozen);
    /* True if the entity is frozen. */
    bool isFrozen() const;

    /* Keeps a point on the entity within a min/max distance of a world space anchor. */
    dugl::uint addDistanceConstraint(const DistanceConstraintParams& params);
    /* Springs a point on the entity to a world space anchor. Rotation remains free. */
    dugl::uint addPointConstraint(const PointConstraintParams& params);
    /* Set the anchor point for an existing constraint on the entity. */
    void setConstraintAnchor(dugl::uint constraintId, glm::vec3 anchorInWorldSpace);
    /* Remove a constraint from the entity. */
    void removeConstraint(dugl::uint constraintId);
    /* True if the entity has a constraint with the given ID. */
    bool hasConstraint(dugl::uint constraintId) const;

    void setPosition(glm::vec3 position) override;
    void setRotation(glm::quat rotation) override;
    void setVelocity(glm::vec3 velocity) override;
    void setAngularVelocity(glm::vec3 angularVelocity) override;

    void applyForce(glm::vec3 force);
    void applyForce(glm::vec3 force, glm::vec3 point);
    void applyTorque(glm::vec3 torque);

    void applyImpulse(glm::vec3 impulse);
    void applyImpulse(glm::vec3 impulse, glm::vec3 point);
    void applyAngularImpulse(glm::vec3 angularImpulse);

protected:
    /* Called when something collides with the entity. */
    virtual void onCollision(const CollisionEvent& event, PhysicsEntity* other) {}

private:
    glm::vec3 toWorldSpace(glm::vec3 pointInEntitySpace) const;
    dugl::uint addAnchoredConstraint(glm::vec3 anchorInWorldSpace, JPH::TwoBodyConstraintSettings& settings);
    void preUpdate(float dt) override final;
};
