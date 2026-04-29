#pragma once

#include <vector>
#include <string>

#include "dugl/shading/shader.h"


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
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void render(Shader* shader, bool bindTextures);
    
private:
    unsigned int VAO, VBO, EBO;
    void setupMesh();
};