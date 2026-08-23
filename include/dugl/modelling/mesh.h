#pragma once

#include <vector>
#include <string>

#include "dugl/shading/shader.h"
#include "dugl/common.h"


struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
private:
    dugl::uint VAO, VBO, EBO;

public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
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
};