#include "dugl/physics/body_settings.h"
#include "dugl/physics/common.h"
#include "dugl/utils/jolt_adapter.h"

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include <algorithm>
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
    JPH::Ref<JPH::ShapeSettings> shapeSettings = std::visit([](const auto& collider) {
        return createShapeSettings(collider);
    }, builder.createCollider());

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
