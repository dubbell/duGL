#include "dugl/physics/physics_entity.h"
#include "dugl/physics/common.h"
#include "dugl/utils/jolt_adapter.h"

#include <Jolt/Physics/Constraints/DistanceConstraint.h>
#include <Jolt/Physics/Constraints/SixDOFConstraint.h>

#include <stdexcept>


PhysicsEntity::PhysicsEntity(Renderable* renderable, Shader* shader, JPH::PhysicsSystem& physicsSystem, const JPH::BodyCreationSettings& settings)
    : Entity(renderable, shader),
      physicsSystem(physicsSystem),
      bodyInterface(physicsSystem.GetBodyInterface()),
      bodyId(bodyInterface.CreateAndAddBody(settings, settings.mMotionType == JPH::EMotionType::Static
          ? JPH::EActivation::DontActivate
          : JPH::EActivation::Activate)),
      motionType(settings.mMotionType)
{
    bodyInterface.SetUserData(bodyId, reinterpret_cast<JPH::uint64>(this));
}

PhysicsEntity::~PhysicsEntity()
{
    while (!constraints.empty()) {
        removeConstraint(constraints.begin()->first);
    }

    bodyInterface.RemoveBody(bodyId);
    bodyInterface.DestroyBody(bodyId);
}

void PhysicsEntity::preUpdate(float dt)
{
    position = toGlm(bodyInterface.GetPosition(bodyId));
    rotation = toGlm(bodyInterface.GetRotation(bodyId));
    velocity = toGlm(bodyInterface.GetLinearVelocity(bodyId));
    angularVelocity = toGlm(bodyInterface.GetAngularVelocity(bodyId));

    for (const CollisionEvent& event : pendingCollisions) {
        onCollision(event, event.first == this ? event.second : event.first);
    }
    pendingCollisions.clear();
}

JPH::BodyID PhysicsEntity::getBodyId() const
{
    return bodyId;
}

void PhysicsEntity::applyForce(glm::vec3 force)
{
    bodyInterface.AddForce(bodyId, toJolt(force));
}

void PhysicsEntity::applyForce(glm::vec3 force, glm::vec3 point)
{
    bodyInterface.AddForce(bodyId, toJolt(force), toJolt(point));
}

void PhysicsEntity::applyTorque(glm::vec3 torque)
{
    bodyInterface.AddTorque(bodyId, toJolt(torque));
}

void PhysicsEntity::applyImpulse(glm::vec3 impulse)
{
    bodyInterface.AddImpulse(bodyId, toJolt(impulse));
}

void PhysicsEntity::applyImpulse(glm::vec3 impulse, glm::vec3 point)
{
    bodyInterface.AddImpulse(bodyId, toJolt(impulse), toJolt(point));
}

void PhysicsEntity::applyAngularImpulse(glm::vec3 angularImpulse)
{
    bodyInterface.AddAngularImpulse(bodyId, toJolt(angularImpulse));
}

void PhysicsEntity::setFrozen(bool frozen)
{
    if (this->frozen == frozen) {
        return;
    }
    this->frozen = frozen;
    bodyInterface.SetMotionType(bodyId, frozen ? JPH::EMotionType::Static : motionType, JPH::EActivation::Activate);
}

bool PhysicsEntity::isFrozen() const
{
    return frozen;
}

void PhysicsEntity::setPosition(glm::vec3 position)
{
    Entity::setPosition(position);
    bodyInterface.SetPosition(bodyId, toJolt(position), JPH::EActivation::Activate);
}

void PhysicsEntity::setRotation(glm::quat rotation)
{
    Entity::setRotation(rotation);
    bodyInterface.SetRotation(bodyId, toJolt(rotation), JPH::EActivation::Activate);
}

void PhysicsEntity::setVelocity(glm::vec3 velocity)
{
    Entity::setVelocity(velocity);
    bodyInterface.SetLinearVelocity(bodyId, toJolt(velocity));
}

void PhysicsEntity::setAngularVelocity(glm::vec3 angularVelocity)
{
    Entity::setAngularVelocity(angularVelocity);
    bodyInterface.SetAngularVelocity(bodyId, toJolt(angularVelocity));
}

dugl::uint PhysicsEntity::addDistanceConstraint(const DistanceConstraintParams& params)
{
    JPH::DistanceConstraintSettings settings;
    settings.mPoint1 = toJolt(params.anchorInWorldSpace);
    settings.mPoint2 = toJolt(toWorldSpace(params.pointInEntitySpace));
    settings.mMinDistance = params.minDistance;
    settings.mMaxDistance = params.maxDistance;
    settings.mLimitsSpringSettings.mFrequency = params.frequency;
    settings.mLimitsSpringSettings.mDamping = params.damping;

    return addAnchoredConstraint(params.anchorInWorldSpace, settings);
}

dugl::uint PhysicsEntity::addPointConstraint(const PointConstraintParams& params)
{
    using EAxis = JPH::SixDOFConstraintSettings::EAxis;

    JPH::SixDOFConstraintSettings settings;
    settings.mPosition1 = toJolt(params.anchorInWorldSpace);
    settings.mPosition2 = toJolt(toWorldSpace(params.pointInEntitySpace));

    for (EAxis axis : { EAxis::TranslationX, EAxis::TranslationY, EAxis::TranslationZ }) {
        settings.SetLimitedAxis(axis, 0.0f, 0.0f);
        settings.mLimitsSpringSettings[axis].mFrequency = params.frequency;
        settings.mLimitsSpringSettings[axis].mDamping = params.damping;
    }
    for (EAxis axis : { EAxis::RotationX, EAxis::RotationY, EAxis::RotationZ }) {
        settings.MakeFreeAxis(axis);
    }

    return addAnchoredConstraint(params.anchorInWorldSpace, settings);
}

void PhysicsEntity::setConstraintAnchor(dugl::uint constraintId, glm::vec3 anchorInWorldSpace)
{
    auto it = constraints.find(constraintId);
    if (it == constraints.end()) {
        return;
    }

    bodyInterface.SetPosition(it->second.anchorId, toJolt(anchorInWorldSpace), JPH::EActivation::DontActivate);
    bodyInterface.ActivateBody(bodyId);
}

void PhysicsEntity::removeConstraint(dugl::uint constraintId)
{
    auto it = constraints.find(constraintId);
    if (it == constraints.end()) {
        return;
    }

    physicsSystem.RemoveConstraint(it->second.constraint);
    bodyInterface.DestroyBody(it->second.anchorId);
    constraints.erase(it);
    bodyInterface.ActivateBody(bodyId);
}

bool PhysicsEntity::hasConstraint(dugl::uint constraintId) const
{
    return constraints.contains(constraintId);
}

glm::vec3 PhysicsEntity::toWorldSpace(glm::vec3 pointInEntitySpace) const
{
    return toGlm(bodyInterface.GetPosition(bodyId)) + toGlm(bodyInterface.GetRotation(bodyId)) * pointInEntitySpace;
}

dugl::uint PhysicsEntity::addAnchoredConstraint(glm::vec3 anchorInWorldSpace, JPH::TwoBodyConstraintSettings& settings)
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
    constraints[constraintId] = { anchor->GetID(), constraint };
    return constraintId;
}
