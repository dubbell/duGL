#pragma once

#include "dugl/common.h"

#include <glm/glm.hpp>

#include <variant>
#include <vector>


struct BoxCollider
{
    glm::vec3 halfExtents;
    glm::vec3 offset{ 0.0f };
};

struct SphereCollider
{
    float radius;
};

struct ConvexHullCollider
{
    std::vector<glm::vec3> points;
};

struct HeightFieldCollider
{
    std::vector<float> heights;
    dugl::uint rows, columns;
    glm::vec2 size;
};

/* Parameters of the collision geometry for primitives. */
using Collider = std::variant<BoxCollider, SphereCollider, ConvexHullCollider, HeightFieldCollider>;
