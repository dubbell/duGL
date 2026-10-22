#include "dugl/physics/body_settings.h"
#include "dugl/physics/common.h"
#include "dugl/utils/jolt_adapter.h"

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/HeightFieldShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Constraints/FixedConstraint.h>
#include <Jolt/Physics/Constraints/HingeConstraint.h>
#include <Jolt/Skeleton/Skeleton.h>

#include <algorithm>
#include <format>
#include <stdexcept>
#include <string>
#include <variant>


static JPH::Ref<JPH::ShapeSettings> createShapeSettings(const BoxCollider& collider)
{
    float minHalfExtent = std::min({ collider.halfExtents.x, collider.halfExtents.y, collider.halfExtents.z });
    JPH::Ref<JPH::ShapeSettings> box = new JPH::BoxShapeSettings(
        toJolt(collider.halfExtents), std::min(JPH::cDefaultConvexRadius, minHalfExtent));

    if (collider.offset == glm::vec3(0.0f)) {
        return box;
    }
    return new JPH::RotatedTranslatedShapeSettings(toJolt(collider.offset), JPH::Quat::sIdentity(), box);
}

static JPH::Ref<JPH::ShapeSettings> createShapeSettings(const SphereCollider& collider)
{
    return new JPH::SphereShapeSettings(collider.radius);
}

static JPH::Ref<JPH::ShapeSettings> createShapeSettings(const ConvexHullCollider& collider)
{
    JPH::Array<JPH::Vec3> points;
    points.reserve(collider.points.size());
    for (const glm::vec3& point : collider.points) {
        points.push_back(toJolt(point));
    }
    return new JPH::ConvexHullShapeSettings(points);
}

static JPH::Ref<JPH::ShapeSettings> createShapeSettings(const HeightFieldCollider& collider)
{
    JPH::Ref<JPH::HeightFieldShapeSettings> settings = new JPH::HeightFieldShapeSettings();

    // Jolt needs a square grid sized in whole blocks, so pad the samples with holes.
    dugl::uint blockSize = settings->mBlockSize;
    dugl::uint sampleCount = std::max(collider.rows, collider.columns);
    sampleCount = std::max((sampleCount + blockSize - 1) / blockSize, 2u) * blockSize;

    settings->mSampleCount = sampleCount;
    settings->mHeightSamples.resize(sampleCount * sampleCount, JPH::HeightFieldShapeConstants::cNoCollisionValue);
    for (dugl::uint row = 0; row < collider.rows; row++) {
        for (dugl::uint column = 0; column < collider.columns; column++) {
            settings->mHeightSamples[row * sampleCount + column] = collider.heights[row * collider.columns + column];
        }
    }

    settings->mOffset = JPH::Vec3(-collider.size.x * 0.5f, 0.0f, -collider.size.y * 0.5f);
    settings->mScale = JPH::Vec3(collider.size.x / (collider.columns - 1), 1.0f, collider.size.y / (collider.rows - 1));

    return settings.GetPtr();
}

static JPH::EMotionType toJolt(MotionType motionType)
{
    switch (motionType)
    {
        case MotionType::Static:
            return JPH::EMotionType::Static;
        case MotionType::Kinematic:
            return JPH::EMotionType::Kinematic;
        default:
            return JPH::EMotionType::Dynamic;
    }
}

JPH::BodyCreationSettings createBodySettings(const PrimitiveBuilder& builder, const PhysicsMaterial& material)
{
    Collider collider = builder.createCollider();
    if (std::holds_alternative<HeightFieldCollider>(collider) && material.motionType != MotionType::Static) {
        throw std::logic_error("A height field can only be simulated as a static body.");
    }

    JPH::Ref<JPH::ShapeSettings> shapeSettings = std::visit([](const auto& collider) {
        return createShapeSettings(collider);
    }, collider);

    JPH::ShapeSettings::ShapeResult shape = shapeSettings->Create();
    if (shape.HasError()) {
        throw std::runtime_error("Failed to create collision shape: " + std::string(shape.GetError().c_str()));
    }

    JPH::BodyCreationSettings settings(
        shape.Get(),
        JPH::RVec3::sZero(),
        JPH::Quat::sIdentity(),
        toJolt(material.motionType),
        material.motionType == MotionType::Static ? Layers::NON_MOVING : Layers::MOVING);
    settings.mFriction = material.friction;
    settings.mRestitution = material.restitution;

    return settings;
}

static JPH::Ref<JPH::TwoBodyConstraintSettings> createJointSettings(const HingeJoint& joint)
{
    JPH::Ref<JPH::HingeConstraintSettings> settings = new JPH::HingeConstraintSettings();
    settings->mSpace = JPH::EConstraintSpace::WorldSpace;
    settings->mPoint1 = settings->mPoint2 = toJolt(joint.pivot);
    settings->mHingeAxis1 = settings->mHingeAxis2 = toJolt(glm::normalize(joint.axis));
    settings->mNormalAxis1 = settings->mNormalAxis2 = toJolt(glm::normalize(joint.normal));
    settings->mLimitsMin = joint.minAngle;
    settings->mLimitsMax = joint.maxAngle;
    settings->mMotorSettings.mSpringSettings.mFrequency = joint.motorFrequency;
    settings->mMotorSettings.mSpringSettings.mDamping = joint.motorDamping;
    settings->mMotorSettings.SetTorqueLimit(joint.maxMotorTorque);
    settings->mMaxFrictionTorque = joint.maxFrictionTorque;
    return settings.GetPtr();
}

static JPH::Ref<JPH::TwoBodyConstraintSettings> createJointSettings(const FixedJoint& joint)
{
    JPH::Ref<JPH::FixedConstraintSettings> settings = new JPH::FixedConstraintSettings();
    settings->mSpace = JPH::EConstraintSpace::WorldSpace;
    settings->mPoint1 = settings->mPoint2 = toJolt(joint.pivot);
    return settings.GetPtr();
}

JPH::Ref<JPH::RagdollSettings> createRagdollSettings(const RagdollBuilder& builder)
{
    const std::vector<RagdollPart>& parts = builder.getParts();
    if (parts.empty()) {
        throw std::logic_error("A ragdoll needs at least a root part.");
    }

    JPH::Ref<JPH::Skeleton> skeleton = new JPH::Skeleton();
    JPH::Ref<JPH::RagdollSettings> ragdollSettings = new JPH::RagdollSettings();
    ragdollSettings->mParts.resize(parts.size());

    for (size_t i = 0; i < parts.size(); i++)
    {
        const RagdollPart& part = parts[i];
        skeleton->AddJoint(std::format("part{}", i), part.parent);

        JPH::RagdollSettings::Part& settings = ragdollSettings->mParts[i];
        static_cast<JPH::BodyCreationSettings&>(settings) = createBodySettings(*part.builder, part.material);
        settings.mPosition = toJolt(part.position);
        settings.mRotation = toJolt(part.rotation);

        if (part.joint) {
            settings.mToParent = std::visit([](const auto& joint) { return createJointSettings(joint); }, *part.joint);
        }
    }

    ragdollSettings->mSkeleton = skeleton;
    ragdollSettings->Stabilize();
    ragdollSettings->DisableParentChildCollisions();
    ragdollSettings->CalculateBodyIndexToConstraintIndex();
    ragdollSettings->CalculateConstraintIndexToBodyIdxPair();

    return ragdollSettings;
}
