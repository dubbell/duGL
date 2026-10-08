#pragma once

#include <glm/glm.hpp>


class PhysicsEntity;

enum class CollisionPhase
{
    Begin,
    End
};

struct CollisionEvent
{
    CollisionPhase phase;
    PhysicsEntity* first;
    PhysicsEntity* second;
    glm::vec3 point{ 0.0f };
    glm::vec3 normal{ 0.0f };
    float penetrationDepth = 0.0f;
    glm::vec3 relativeVelocity{ 0.0f };
};
