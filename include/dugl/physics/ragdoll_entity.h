#pragma once

#include "dugl/modelling/primitive_builder.h"
#include "dugl/physics/anchored_constraints.h"
#include "dugl/physics/physics_entity.h"
#include "dugl/physics/ragdoll_builder.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Constraints/HingeConstraint.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Ragdoll/Ragdoll.h>

#include <atomic>
#include <optional>
#include <vector>


/* An entity made of several physics bodies (parts) connected by joints. */
class RagdollEntity : public PhysicsEntity
{
protected:
    JPH::Ref<JPH::Ragdoll> ragdoll;
    std::vector<glm::mat4> partTransforms;

private:
    static inline std::atomic<JPH::CollisionGroup::GroupID> nextCollisionGroup = 0;

    std::vector<glm::mat4> inverseRestTransforms;
    std::vector<glm::mat4> boneMatrices;
    std::vector<LocalRayHitTest> localRayHitTests;
    mutable std::optional<dugl::uint> hitPart;

public:
    RagdollEntity(Renderable* renderable, Shader* shader, JPH::PhysicsSystem& physicsSystem, RagdollBuilder& builder);
    ~RagdollEntity();

    RagdollEntity(const RagdollEntity&) = delete;
    RagdollEntity& operator=(const RagdollEntity&) = delete;

    void render(Shader* shader) override;

    /* Get the number of parts. */
    dugl::uint getPartCount() const;
    /* Get the ID of a part's JoltPhysics body. */
    JPH::BodyID getPartBodyId(dugl::uint part) const;
    /* Get a part's transform in world space. */
    glm::mat4 getPartTransform(dugl::uint part) const;

    /* Keeps a point on a part within a min/max distance of a world space anchor. */
    dugl::uint addDistanceConstraint(dugl::uint part, const DistanceConstraintParams& params);
    /* Springs a point on a part to a world space anchor. Rotation remains free. */
    dugl::uint addPointConstraint(dugl::uint part, const PointConstraintParams& params);

    void setJointTargetAngle(dugl::uint part, float angle);
    void releaseJoint(dugl::uint part);
    void applyJointTorque(dugl::uint part, float torque);
    float getJointAngle(dugl::uint part) const;

    void setPosition(glm::vec3 position) override;
    void setRotation(glm::quat rotation) override;
    void setVelocity(glm::vec3 velocity) override;
    void setAngularVelocity(glm::vec3 angularVelocity) override;

    void applyImpulse(glm::vec3 impulse);

protected:
    /* Get the part hit by the most recent ray hit test, if any. */
    std::optional<dugl::uint> getHitPart() const;

    void syncWithPhysics() override;
    std::optional<float> intersectRay(const Ray& ray) const override;

private:
    void moveRigidly(const glm::mat4& targetRootTransform);
    JPH::HingeConstraint* getHingeJoint(dugl::uint part) const;
};
