#pragma once

#include <glm/glm.hpp>

#include "dugl/common.h"


// Global registry of vertex attribute semantics.
//
// A location number belongs to one semantic permanently. Vertex formats pick a subset of
// these locations; they never renumber. That keeps mesh/shader compatibility a simple
// subset test: a shader can render any mesh whose format provides at least the attributes
// the shader reads, no matter which format that is.
//
// Shaders declare the same numbers through 'layout (location = N)' qualifiers. The two
// sides are checked against each other at draw time in debug builds, see
// Mesh::checkShaderCompatibility().
enum class VertexAttribute : dugl::uint
{
    Position  = 0,
    Normal    = 1,
    TexCoord  = 2,
    Tangent   = 3,  // vec4: w carries the bitangent handedness sign
    Color     = 4,
    TexCoord1 = 5,
    BoneIds   = 6,  // reserved: skinned formats
    Weights   = 7,  // reserved: skinned formats
    // 8..11 are reserved for a per-instance model matrix; a mat4 vertex attribute occupies
    // four consecutive locations
};

// a set of attribute locations, one bit per location
using VertexAttributeMask = dugl::uint32;

constexpr dugl::uint attributeLocation(VertexAttribute attribute)
{
    return static_cast<dugl::uint>(attribute);
}

constexpr VertexAttributeMask attributeMask(VertexAttribute attribute)
{
    return VertexAttributeMask{1} << attributeLocation(attribute);
}

// name of whichever semantic owns a location, for diagnostics
inline const char* attributeName(dugl::uint location)
{
    switch (location)
    {
        case attributeLocation(VertexAttribute::Position):  return "position";
        case attributeLocation(VertexAttribute::Normal):    return "normal";
        case attributeLocation(VertexAttribute::TexCoord):  return "texCoord";
        case attributeLocation(VertexAttribute::Tangent):   return "tangent";
        case attributeLocation(VertexAttribute::Color):     return "color";
        case attributeLocation(VertexAttribute::TexCoord1): return "texCoord1";
        case attributeLocation(VertexAttribute::BoneIds):   return "boneIds";
        case attributeLocation(VertexAttribute::Weights):   return "weights";
        default:                                            return "unassigned";
    }
}


// The vertex format for non-deformable geometries.
struct StaticVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

// The vertex attribute mask for non-deformable geometries.
constexpr VertexAttributeMask STATIC_VERTEX_ATTRIBUTES =
    attributeMask(VertexAttribute::Position) |
    attributeMask(VertexAttribute::Normal) |
    attributeMask(VertexAttribute::TexCoord);
