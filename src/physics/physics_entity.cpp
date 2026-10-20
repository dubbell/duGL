#include "dugl/physics/physics_entity.h"
#include "dugl/utils/jolt_adapter.h"


PhysicsEntity::PhysicsEntity(Renderable* renderable, Shader* shader, JPH::PhysicsSystem& physicsSystem, const JPH::BodyCreationSettings& settings)
    : Entity(renderable, shader),
      physicsSystem(physicsSystem),
      bodyInterface(physicsSystem.GetBodyInterface()),
      bodyId(bodyInterface.CreateAndAddBody(settings, settings.mMotionType == JPH::EMotionType::Static
          ? JPH::EActivation::DontActivate
          : JPH::EActivation::Activate)),
      motionType(settings.mMotionType),
      constraints(physicsSystem)
{
    bodyInterface.SetUserData(bodyId, reinterpret_cast<JPH::uint64>(this));
}

PhysicsEntity::~PhysicsEntity()
{
    constraints.clear();

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
    return constraints.addDistanceConstraint(bodyId, params);
}

dugl::uint PhysicsEntity::addPointConstraint(const PointConstraintParams& params)
{
    return constraints.addPointConstraint(bodyId, params);
}

void PhysicsEntity::setConstraintAnchor(dugl::uint constraintId, glm::vec3 anchorInWorldSpace)
{
    constraints.setAnchor(constraintId, anchorInWorldSpace);
}

void PhysicsEntity::removeConstraint(dugl::uint constraintId)
{
    constraints.remove(constraintId);
}

bool PhysicsEntity::hasConstraint(dugl::uint constraintId) const
{
    return constraints.has(constraintId);
}
