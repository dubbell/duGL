#include "dugl/physics/physics_entity.h"


PhysicsEntity::PhysicsEntity(Renderable* renderable, Shader* shader, JPH::PhysicsSystem& physicsSystem)
    : Entity(renderable, shader),
      physicsSystem(physicsSystem),
      bodyInterface(physicsSystem.GetBodyInterface()),
      constraints(physicsSystem) {}

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

void PhysicsEntity::registerBody(JPH::BodyID bodyId)
{
    bodyInterface.SetUserData(bodyId, reinterpret_cast<JPH::uint64>(this));
}

void PhysicsEntity::preUpdate(float dt)
{
    syncWithPhysics();

    for (const CollisionEvent& event : pendingCollisions) {
        onCollision(event, event.first == this ? event.second : event.first);
    }
    pendingCollisions.clear();
}
