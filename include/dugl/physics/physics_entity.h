#pragma once

#include "dugl/modelling/entity.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>


class PhysicsEntity : public Entity
{
protected:
    JPH::BodyInterface& bodyInterface;
    JPH::BodyID bodyId;

    void update(float dt) override;

public:
    PhysicsEntity(Renderable* renderable, Shader* shader, JPH::BodyInterface& bodyInterface, const JPH::BodyCreationSettings& settings);
    ~PhysicsEntity();

    PhysicsEntity(const PhysicsEntity&) = delete;
    PhysicsEntity& operator=(const PhysicsEntity&) = delete;

    JPH::BodyID getBodyId() const;
};
