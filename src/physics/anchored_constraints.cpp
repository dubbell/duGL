#include "dugl/physics/anchored_constraints.h"
#include "dugl/physics/common.h"
#include "dugl/utils/jolt_adapter.h"

#include <Jolt/Physics/Constraints/DistanceConstraint.h>
#include <Jolt/Physics/Constraints/SixDOFConstraint.h>

#include <stdexcept>


AnchoredConstraints::AnchoredConstraints(JPH::PhysicsSystem& physicsSystem)
    : physicsSystem(physicsSystem), bodyInterface(physicsSystem.GetBodyInterface()) {}

AnchoredConstraints::~AnchoredConstraints()
{
    clear();
}

dugl::uint AnchoredConstraints::addDistanceConstraint(JPH::BodyID bodyId, const DistanceConstraintParams& params)
{
    JPH::DistanceConstraintSettings settings;
    settings.mPoint1 = toJolt(params.anchorInWorldSpace);
    settings.mPoint2 = toJolt(toWorldSpace(bodyId, params.pointInEntitySpace));
    settings.mMinDistance = params.minDistance;
    settings.mMaxDistance = params.maxDistance;
    settings.mLimitsSpringSettings.mFrequency = params.frequency;
    settings.mLimitsSpringSettings.mDamping = params.damping;

    return add(bodyId, params.anchorInWorldSpace, settings);
}

dugl::uint AnchoredConstraints::addPointConstraint(JPH::BodyID bodyId, const PointConstraintParams& params)
{
    using EAxis = JPH::SixDOFConstraintSettings::EAxis;

    JPH::SixDOFConstraintSettings settings;
    settings.mPosition1 = toJolt(params.anchorInWorldSpace);
    settings.mPosition2 = toJolt(toWorldSpace(bodyId, params.pointInEntitySpace));

    for (EAxis axis : { EAxis::TranslationX, EAxis::TranslationY, EAxis::TranslationZ }) {
        settings.SetLimitedAxis(axis, 0.0f, 0.0f);
        settings.mLimitsSpringSettings[axis].mFrequency = params.frequency;
        settings.mLimitsSpringSettings[axis].mDamping = params.damping;
    }
    for (EAxis axis : { EAxis::RotationX, EAxis::RotationY, EAxis::RotationZ }) {
        settings.MakeFreeAxis(axis);
    }

    return add(bodyId, params.anchorInWorldSpace, settings);
}

void AnchoredConstraints::setAnchor(dugl::uint constraintId, glm::vec3 anchorInWorldSpace)
{
    auto it = constraints.find(constraintId);
    if (it == constraints.end()) {
        return;
    }

    bodyInterface.SetPosition(it->second.anchorId, toJolt(anchorInWorldSpace), JPH::EActivation::DontActivate);
    bodyInterface.ActivateBody(it->second.bodyId);
}

void AnchoredConstraints::remove(dugl::uint constraintId)
{
    auto it = constraints.find(constraintId);
    if (it == constraints.end()) {
        return;
    }

    physicsSystem.RemoveConstraint(it->second.constraint);
    bodyInterface.DestroyBody(it->second.anchorId);
    bodyInterface.ActivateBody(it->second.bodyId);
    constraints.erase(it);
}

void AnchoredConstraints::clear()
{
    while (!constraints.empty()) {
        remove(constraints.begin()->first);
    }
}

bool AnchoredConstraints::has(dugl::uint constraintId) const
{
    return constraints.contains(constraintId);
}

glm::vec3 AnchoredConstraints::toWorldSpace(JPH::BodyID bodyId, glm::vec3 pointInBodySpace) const
{
    return toGlm(bodyInterface.GetPosition(bodyId)) + toGlm(bodyInterface.GetRotation(bodyId)) * pointInBodySpace;
}

dugl::uint AnchoredConstraints::add(JPH::BodyID bodyId, glm::vec3 anchorInWorldSpace, JPH::TwoBodyConstraintSettings& settings)
{
    JPH::Body* anchor = bodyInterface.CreateBody(JPH::BodyCreationSettings(
        new JPH::SphereShapeSettings(0.01f),
        toJolt(anchorInWorldSpace),
        JPH::Quat::sIdentity(),
        JPH::EMotionType::Static,
        Layers::NON_MOVING));
    if (anchor == nullptr) {
        throw std::runtime_error("Failed to create constraint anchor: out of bodies.");
    }

    JPH::Ref<JPH::Constraint> constraint = bodyInterface.CreateConstraint(&settings, anchor->GetID(), bodyId);
    physicsSystem.AddConstraint(constraint);
    bodyInterface.ActivateBody(bodyId);

    dugl::uint constraintId = nextConstraintId++;
    constraints[constraintId] = { bodyId, anchor->GetID(), constraint };
    return constraintId;
}
