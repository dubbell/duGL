#pragma once

#include "rigid_entity.h"

#include <optional>


class DraggableEntity : public RigidEntity
{
private:
    std::optional<dugl::uint> holdConstraint;

public:
    DraggableEntity(Renderable* renderable, Shader* shader, JPH::PhysicsSystem& physicsSystem, const PrimitiveBuilder& builder, const PhysicsMaterial& material);

protected:
    void update(float dt) override;
};
