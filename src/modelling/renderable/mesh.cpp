#include "dugl/modelling/mesh.h"
#include "dugl/shading/shader.h"
#include "dugl/utils/glad_include.h"

#include <format>
#include <stdexcept>
#include <string>


Mesh::Mesh(const std::vector<StaticVertex>& vertices, const std::vector<unsigned int>& indices, Material material)
    : attributes(STATIC_VERTEX_ATTRIBUTES), indexCount((dugl::uint)indices.size()), material(material)
{
    uploadBuffers(vertices, indices);
    setupSurfaceAttributes<StaticVertex>();
    glBindVertexArray(0);
}

Mesh::Mesh(const std::vector<SkinnedVertex>& vertices, const std::vector<unsigned int>& indices, Material material)
    : attributes(SKINNED_VERTEX_ATTRIBUTES), indexCount((dugl::uint)indices.size()), material(material)
{
    uploadBuffers(vertices, indices);
    setupSurfaceAttributes<SkinnedVertex>();
    setupSkinningAttributes();
    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

Mesh::Mesh(Mesh&& other) noexcept
    : VAO(other.VAO), VBO(other.VBO), EBO(other.EBO), attributes(other.attributes),
      indexCount(other.indexCount), material(other.material)
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
        indexCount = other.indexCount;
        material = other.material;

        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
    }
    return *this;
}

template <typename Vertex>
void Mesh::uploadBuffers(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}

template <typename Vertex>
void Mesh::setupSurfaceAttributes()
{
    // vertex positions
    glEnableVertexAttribArray(attributeLocation(VertexAttribute::Position));
    glVertexAttribPointer(attributeLocation(VertexAttribute::Position),
        3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    // vertex normals
    glEnableVertexAttribArray(attributeLocation(VertexAttribute::Normal));
    glVertexAttribPointer(attributeLocation(VertexAttribute::Normal),
        3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(attributeLocation(VertexAttribute::TexCoord));
    glVertexAttribPointer(attributeLocation(VertexAttribute::TexCoord),
        2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
}

void Mesh::setupSkinningAttributes()
{
    glEnableVertexAttribArray(attributeLocation(VertexAttribute::BoneIds));
    glVertexAttribIPointer(attributeLocation(VertexAttribute::BoneIds),
        4, GL_UNSIGNED_INT, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, boneIds));

    glEnableVertexAttribArray(attributeLocation(VertexAttribute::Weights));
    glVertexAttribPointer(attributeLocation(VertexAttribute::Weights),
        4, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, weights));
}

void Mesh::checkShaderCompatibility(Shader* shader) const
{
    VertexAttributeMask missing = shader->getRequiredAttributes() & ~attributes;
    if (missing == 0) {
        return;
    }

    std::string missingNames;
    for (dugl::uint location = 0; location < 8 * sizeof(VertexAttributeMask); location++)
    {
        if ((missing >> location & 1) == 0) {
            continue;
        }

        if (!missingNames.empty()) {
            missingNames += ", ";
        }
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
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}