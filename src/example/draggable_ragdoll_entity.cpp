#include "dugl/example/draggable_ragdoll_entity.h"


DraggableRagdollEntity::DraggableRagdollEntity(Renderable* renderable, Shader* shader, JPH::PhysicsSystem& physicsSystem, RagdollBuilder& builder)
    : RagdollEntity(renderable, shader, physicsSystem, builder) {}

void DraggableRagdollEntity::update(float dt)
{
    bool held = hasFlag(EntityFlag::Held);
    if (holdConstraint && !held) {
        removeConstraint(*holdConstraint);
        holdConstraint.reset();
    }

    if (!held) {
        return;
    }

    glm::vec3 heldInWorldSpace = getProperty(EntityProperty::HeldInWorldSpace, position);
    std::optional<dugl::uint> part = getHitPart();

    if (holdConstraint) {
        setConstraintAnchor(*holdConstraint, heldInWorldSpace);
    }
    else if (part)
    {
        glm::vec3 grabPoint = position + rotation * getProperty(EntityProperty::HeldInEntitySpace, glm::vec3(0.0f));
        glm::vec3 grabPointInPartSpace = glm::vec3(glm::inverse(getPartTransform(*part)) * glm::vec4(grabPoint, 1.0f));
        holdConstraint = addPointConstraint(*part, {
            .pointInEntitySpace = grabPointInPartSpace,
            .anchorInWorldSpace = heldInWorldSpace,
            .frequency = HOLD_FREQUENCY,
            .damping = HOLD_DAMPING });
    }
}
