#pragma once

#include "dugl/modelling/entity.h"
#include "dugl/modelling/primitive_builder.h"
#include "dugl/physics/anchored_constraints.h"
#include "dugl/physics/ragdoll_builder.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Ragdoll/Ragdoll.h>

#include <atomic>
#include <optional>
#include <vector>


/* An entity made of several physics bodies (parts) connected by joints. */
class RagdollEntity : public Entity
{
protected:
    JPH::PhysicsSystem& physicsSystem;
    JPH::BodyInterface& bodyInterface;
    JPH::Ref<JPH::Ragdoll> ragdoll;
    std::vector<glm::mat4> partTransforms;

private:
    static inline std::atomic<JPH::CollisionGroup::GroupID> nextCollisionGroup = 0;

    std::vector<glm::mat4> inverseRestTransforms;
    std::vector<glm::mat4> boneMatrices;
    std::vector<LocalRayHitTest> localRayHitTests;
    AnchoredConstraints constraints;
    std::optional<dugl::uint> holdConstraint;
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

    void setPosition(glm::vec3 position) override;
    void setRotation(glm::quat rotation) override;
    void setVelocity(glm::vec3 velocity) override;
    void setAngularVelocity(glm::vec3 angularVelocity) override;

    void applyImpulse(glm::vec3 impulse);

protected:
    void update(float dt) override;
    std::optional<float> intersectRay(const Ray& ray) const override;

private:
    void preUpdate(float dt) override final;
    void syncTransforms();
    void moveRigidly(const glm::mat4& targetRootTransform);
};
