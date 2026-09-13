#include "dugl/physics/physics_entity.h"
#include "dugl/utils/jolt_adapter.h"


PhysicsEntity::PhysicsEntity(Renderable* renderable, JPH::BodyInterface& bodyInterface, const JPH::BodyCreationSettings& settings)
    : Entity(renderable),
      bodyInterface(bodyInterface),
      bodyId(bodyInterface.CreateAndAddBody(settings, settings.mMotionType == JPH::EMotionType::Static
          ? JPH::EActivation::DontActivate
          : JPH::EActivation::Activate))
{}

PhysicsEntity::~PhysicsEntity()
{
    bodyInterface.RemoveBody(bodyId);
    bodyInterface.DestroyBody(bodyId);
}

void PhysicsEntity::update(float dt)
{
    position = toGlm(bodyInterface.GetPosition(bodyId));
    rotation = toGlm(bodyInterface.GetRotation(bodyId));
    velocity = toGlm(bodyInterface.GetLinearVelocity(bodyId));
    angularVelocity = toGlm(bodyInterface.GetAngularVelocity(bodyId));
}

JPH::BodyID PhysicsEntity::getBodyId() const
{
    return bodyId;
}
