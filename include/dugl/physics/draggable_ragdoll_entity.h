#pragma once

#include "ragdoll_entity.h"

#include <optional>


class DraggableRagdollEntity : public RagdollEntity
{
private:
    std::optional<dugl::uint> holdConstraint;

public:
    DraggableRagdollEntity(Renderable* renderable, Shader* shader, JPH::PhysicsSystem& physicsSystem, RagdollBuilder& builder);

protected:
    void update(float dt) override;
};
