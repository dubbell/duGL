#pragma once

#include "physics_entity.h"

#include <optional>


class DraggableEntity : public PhysicsEntity
{
private:
    std::optional<dugl::uint> holdConstraint;

public:
    DraggableEntity(Renderable* renderable, Shader* shader, JPH::PhysicsSystem& physicsSystem, const JPH::BodyCreationSettings& settings);

protected:
    void update(float dt) override;
};
