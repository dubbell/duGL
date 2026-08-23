#include "dugl/modelling/mesh.h"
#include "dugl/shading/shader.h"
#include "dugl/utils/glad_include.h"

#include <format>
#include <stdexcept>
#include <string>


Mesh::Mesh(std::vector<StaticVertex> vertices, std::vector<unsigned int> indices, Material material)
    : attributes(STATIC_VERTEX_ATTRIBUTES),
      vertices(std::move(vertices)), indices(std::move(indices)), material(material)
{
    setupMesh();
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

Mesh::Mesh(Mesh&& other) noexcept
    : VAO(other.VAO), VBO(other.VBO), EBO(other.EBO), attributes(other.attributes),
      vertices(std::move(other.vertices)), indices(std::move(other.indices)), material(other.material)
{
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if (this != &other)
    {
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);

        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;
        attributes = other.attributes;
        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        material = other.material;

        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
    }
    return *this;
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(StaticVertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(attributeLocation(VertexAttribute::Position));
    glVertexAttribPointer(attributeLocation(VertexAttribute::Position),
        3, GL_FLOAT, GL_FALSE, sizeof(StaticVertex), (void*)offsetof(StaticVertex, position));
    // vertex normals
    glEnableVertexAttribArray(attributeLocation(VertexAttribute::Normal));
    glVertexAttribPointer(attributeLocation(VertexAttribute::Normal),
        3, GL_FLOAT, GL_FALSE, sizeof(StaticVertex), (void*)offsetof(StaticVertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(attributeLocation(VertexAttribute::TexCoord));
    glVertexAttribPointer(attributeLocation(VertexAttribute::TexCoord),
        2, GL_FLOAT, GL_FALSE, sizeof(StaticVertex), (void*)offsetof(StaticVertex, texCoord));

    glBindVertexArray(0);
}

void Mesh::checkShaderCompatibility(Shader* shader) const
{
    VertexAttributeMask missing = shader->getRequiredAttributes() & ~attributes;
    if (missing == 0) return;

    std::string missingNames;
    for (dugl::uint location = 0; location < 8 * sizeof(VertexAttributeMask); location++)
    {
        if ((missing >> location & 1) == 0) continue;

        if (!missingNames.empty()) missingNames += ", ";
        missingNames += std::format("{} (location {})", attributeName(location), location);
    }

    throw std::runtime_error(std::format(
        "Shader reads vertex attributes this mesh does not provide: {}", missingNames));
}

void Mesh::render(Shader* shader, bool bindTextures)
{
#ifndef NDEBUG
    checkShaderCompatibility(shader);
#endif

    if (bindTextures)
    {
        shader->setMaterial(material);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}