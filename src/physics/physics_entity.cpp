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

void PhysicsEntity::addForce(JPH::BodyID bodyId, JPH::Vec3 force)
{
    getPendingForce(bodyId).force += force;
}

void PhysicsEntity::addForce(JPH::BodyID bodyId, JPH::Vec3 force, JPH::RVec3 point)
{
    PendingForce& pending = getPendingForce(bodyId);
    pending.force += force;
    pending.torque += JPH::Vec3(point - bodyInterface.GetCenterOfMassPosition(bodyId)).Cross(force);
}

void PhysicsEntity::addTorque(JPH::BodyID bodyId, JPH::Vec3 torque)
{
    getPendingForce(bodyId).torque += torque;
}

PhysicsEntity::PendingForce& PhysicsEntity::getPendingForce(JPH::BodyID bodyId)
{
    for (PendingForce& pending : pendingForces) {
        if (pending.bodyId == bodyId) {
            return pending;
        }
    }
    return pendingForces.emplace_back(PendingForce{ bodyId });
}

void PhysicsEntity::applyPendingForces()
{
    for (const PendingForce& pending : pendingForces) {
        bodyInterface.AddForceAndTorque(pending.bodyId, pending.force, pending.torque);
    }
}

void PhysicsEntity::clearPendingForces()
{
    pendingForces.clear();
}

void PhysicsEntity::preUpdate(float dt)
{
    syncWithPhysics();

    for (const CollisionEvent& event : pendingCollisions) {
        onCollision(event, event.first == this ? event.second : event.first);
    }
    pendingCollisions.clear();
}
