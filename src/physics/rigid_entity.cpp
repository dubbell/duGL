#include "dugl/physics/rigid_entity.h"
#include "dugl/physics/body_settings.h"
#include "dugl/utils/jolt_adapter.h"


RigidEntity::RigidEntity(Renderable* renderable, Shader* shader, JPH::PhysicsSystem& physicsSystem, const PrimitiveBuilder& builder, const PhysicsMaterial& material)
    : PhysicsEntity(renderable, shader, physicsSystem),
      bodyId(bodyInterface.CreateAndAddBody(createBodySettings(builder, material), material.motionType == MotionType::Static
          ? JPH::EActivation::DontActivate
          : JPH::EActivation::Activate)),
      motionType(bodyInterface.GetMotionType(bodyId))
{
    registerBody(bodyId);
}

RigidEntity::~RigidEntity()
{
    constraints.clear();

    bodyInterface.RemoveBody(bodyId);
    bodyInterface.DestroyBody(bodyId);
}

void RigidEntity::syncWithPhysics()
{
    position = toGlm(bodyInterface.GetPosition(bodyId));
    rotation = toGlm(bodyInterface.GetRotation(bodyId));
    velocity = toGlm(bodyInterface.GetLinearVelocity(bodyId));
    angularVelocity = toGlm(bodyInterface.GetAngularVelocity(bodyId));
}

JPH::BodyID RigidEntity::getBodyId() const
{
    return bodyId;
}

void RigidEntity::applyForce(glm::vec3 force)
{
    bodyInterface.AddForce(bodyId, toJolt(force));
}

void RigidEntity::applyForce(glm::vec3 force, glm::vec3 point)
{
    bodyInterface.AddForce(bodyId, toJolt(force), toJolt(point));
}

void RigidEntity::applyTorque(glm::vec3 torque)
{
    bodyInterface.AddTorque(bodyId, toJolt(torque));
}

void RigidEntity::applyImpulse(glm::vec3 impulse)
{
    bodyInterface.AddImpulse(bodyId, toJolt(impulse));
}

void RigidEntity::applyImpulse(glm::vec3 impulse, glm::vec3 point)
{
    bodyInterface.AddImpulse(bodyId, toJolt(impulse), toJolt(point));
}

void RigidEntity::applyAngularImpulse(glm::vec3 angularImpulse)
{
    bodyInterface.AddAngularImpulse(bodyId, toJolt(angularImpulse));
}

void RigidEntity::setFrozen(bool frozen)
{
    if (this->frozen == frozen) {
        return;
    }
    this->frozen = frozen;
    bodyInterface.SetMotionType(bodyId, frozen ? JPH::EMotionType::Static : motionType, JPH::EActivation::Activate);
}

bool RigidEntity::isFrozen() const
{
    return frozen;
}

void RigidEntity::setPosition(glm::vec3 position)
{
    Entity::setPosition(position);
    bodyInterface.SetPosition(bodyId, toJolt(position), JPH::EActivation::Activate);
}

void RigidEntity::setRotation(glm::quat rotation)
{
    Entity::setRotation(rotation);
    bodyInterface.SetRotation(bodyId, toJolt(rotation), JPH::EActivation::Activate);
}

void RigidEntity::setVelocity(glm::vec3 velocity)
{
    Entity::setVelocity(velocity);
    bodyInterface.SetLinearVelocity(bodyId, toJolt(velocity));
}

void RigidEntity::setAngularVelocity(glm::vec3 angularVelocity)
{
    Entity::setAngularVelocity(angularVelocity);
    bodyInterface.SetAngularVelocity(bodyId, toJolt(angularVelocity));
}

dugl::uint RigidEntity::addDistanceConstraint(const DistanceConstraintParams& params)
{
    return constraints.addDistanceConstraint(bodyId, params);
}

dugl::uint RigidEntity::addPointConstraint(const PointConstraintParams& params)
{
    return constraints.addPointConstraint(bodyId, params);
}
