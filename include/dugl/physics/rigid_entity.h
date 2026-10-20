#pragma once

#include "dugl/modelling/primitive_builder.h"
#include "dugl/physics/physics_entity.h"
#include "dugl/physics/physics_material.h"
#include "dugl/physics/anchored_constraints.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>


/* An entity simulated by a single rigid JoltPhysics body. */
class RigidEntity : public PhysicsEntity
{
protected:
    JPH::BodyID bodyId;

private:
    JPH::EMotionType motionType;
    bool frozen = false;

public:
    RigidEntity(Renderable* renderable, Shader* shader, JPH::PhysicsSystem& physicsSystem, const PrimitiveBuilder& builder, const PhysicsMaterial& material);
    ~RigidEntity();

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
    void syncWithPhysics() override;
};
