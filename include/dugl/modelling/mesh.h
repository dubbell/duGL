#pragma once

#include <vector>

#include "dugl/shading/shader.h"
#include "dugl/shading/material.h"
#include "dugl/common.h"
#include "vertex_layout.h"


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

// A Mesh represents an object that can be rendered on the screen. Accessed through OpenGL GPU handles.
// The object geometry consists of triangles, which are represented by collections of vertices and
// indices. The object appearance is defined by its Material properties.
class Mesh
{
private:
    // OpenGL GPU handles
    dugl::uint VAO, VBO, EBO;

    // the attribute locations this mesh's vertex format supplies
    VertexAttributeMask attributes;

    std::vector<StaticVertex> vertices;
    std::vector<unsigned int> indices;
    Material material;

public:

    Mesh(std::vector<StaticVertex> vertices, std::vector<unsigned int> indices, Material material);
    ~Mesh();

    // enable move functionality
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    // delete copy functionality
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void render(Shader* shader, bool bindTextures);

private:
    void setupMesh();
    void checkShaderCompatibility(Shader* shader) const;
};