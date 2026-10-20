#pragma once

#include "dugl/common.h"

#include <glm/glm.hpp>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Constraints/TwoBodyConstraint.h>

#include <unordered_map>


static constexpr float HOLD_FREQUENCY = 4.0f;
static constexpr float HOLD_DAMPING = 1.0f;

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
    JPH::BodyID bodyId;
    JPH::BodyID anchorId;
    JPH::Ref<JPH::Constraint> constraint;
};

/* Constraints between bodies and static, movable anchors in world space. */
class AnchoredConstraints
{
private:
    JPH::PhysicsSystem& physicsSystem;
    JPH::BodyInterface& bodyInterface;
    std::unordered_map<dugl::uint, AnchoredConstraint> constraints;
    dugl::uint nextConstraintId = 0;

public:
    AnchoredConstraints(JPH::PhysicsSystem& physicsSystem);
    ~AnchoredConstraints();

    AnchoredConstraints(const AnchoredConstraints&) = delete;
    AnchoredConstraints& operator=(const AnchoredConstraints&) = delete;

    /* Keeps a point on the body within a min/max distance of a world space anchor. */
    dugl::uint addDistanceConstraint(JPH::BodyID bodyId, const DistanceConstraintParams& params);
    /* Springs a point on the body to a world space anchor. Rotation remains free. */
    dugl::uint addPointConstraint(JPH::BodyID bodyId, const PointConstraintParams& params);
    /* Set the anchor point for an existing constraint. */
    void setAnchor(dugl::uint constraintId, glm::vec3 anchorInWorldSpace);
    /* Remove a constraint. */
    void remove(dugl::uint constraintId);
    /* Remove all constraints. */
    void clear();
    /* True if a constraint with the given ID exists. */
    bool has(dugl::uint constraintId) const;

private:
    glm::vec3 toWorldSpace(JPH::BodyID bodyId, glm::vec3 pointInBodySpace) const;
    dugl::uint add(JPH::BodyID bodyId, glm::vec3 anchorInWorldSpace, JPH::TwoBodyConstraintSettings& settings);
};
