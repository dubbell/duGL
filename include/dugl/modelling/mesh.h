#pragma once

#include <vector>

#include "dugl/shading/shader.h"
#include "dugl/shading/material.h"
#include "dugl/common.h"
#include "vertex_layout.h"


// A collection of triangles in the GPU that can be rendered on the screen, 
// constructed from vertices and indices in object space. Its appearance is 
// determined by its Material properties. Accessed through OpenGL GPU handles.
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