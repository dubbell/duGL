#include "dugl/physics/physics_entity.h"
#include "dugl/utils/jolt_adapter.h"


PhysicsEntity::PhysicsEntity(Renderable* renderable, Shader* shader, JPH::BodyInterface& bodyInterface, const JPH::BodyCreationSettings& settings)
    : Entity(renderable, shader),
      bodyInterface(bodyInterface),
      bodyId(bodyInterface.CreateAndAddBody(settings, settings.mMotionType == JPH::EMotionType::Static
          ? JPH::EActivation::DontActivate
          : JPH::EActivation::Activate))
{
    bodyInterface.SetUserData(bodyId, reinterpret_cast<JPH::uint64>(this));
}

PhysicsEntity::~PhysicsEntity()
{
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
