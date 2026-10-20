#include "dugl/physics/draggable_entity.h"


DraggableEntity::DraggableEntity(Renderable* renderable, Shader* shader, JPH::PhysicsSystem& physicsSystem, const PrimitiveBuilder& builder, const PhysicsMaterial& material)
    : RigidEntity(renderable, shader, physicsSystem, builder, material) {}

void DraggableEntity::update(float dt)
{
    bool held = hasFlag(EntityFlag::Held);
    if (holdConstraint && (!held || isFrozen())) {
        removeConstraint(*holdConstraint);
        holdConstraint.reset();
    }

    if (!held) {
        return;
    }

    glm::vec3 heldInEntitySpace = getProperty(EntityProperty::HeldInEntitySpace, glm::vec3(0.0f));
    glm::vec3 heldInWorldSpace = getProperty(EntityProperty::HeldInWorldSpace, position);

    if (isFrozen()) {
        setPosition(heldInWorldSpace - rotation * heldInEntitySpace);
    }
    else if (!holdConstraint) {
        holdConstraint = addPointConstraint({
            .pointInEntitySpace = heldInEntitySpace,
            .anchorInWorldSpace = heldInWorldSpace,
            .frequency = HOLD_FREQUENCY,
            .damping = HOLD_DAMPING });
    }
    else {
        setConstraintAnchor(*holdConstraint, heldInWorldSpace);
    }
}
