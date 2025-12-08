#ifndef VERTEX_MANAGER_H
#define VERTEX_MANAGER_H

#include <glad/gl.h>

#include <set>
#include <map>
#include <vector>
#include <iostream>
#include <memory>

#include <renderable.h>
#include <vmanager_interface.h>


struct VertexAttribute
{
    GLuint size;
    GLenum type;
    GLboolean normalized;
};

struct VertexConfiguration
{
    bool loaded;
    std::vector<VertexAttribute> attributes;
    std::vector<GLuint> activePointers;
};

struct VertexContent
{
    bool loaded;
    GLenum usage;
    std::vector<unsigned char> data;
    std::vector<std::unique_ptr<Renderable>> renderables;
};

class VertexManager : public IVertexManager
{
private:
    std::map<unsigned int, std::set<unsigned int>> vaoToVbos;
    std::map<unsigned int, unsigned int> vboToVao;

    // VAO configurations
    std::map<unsigned int, VertexConfiguration> configs;
    // VBO contents
    std::map<unsigned int, VertexContent> contents;

public:
    VertexManager();

    unsigned int createAttributeObject();

    unsigned int createBufferObject();
    unsigned int createBufferObject(unsigned int VAO);

    void addAttribute(unsigned int VAO, GLuint size, GLenum type, GLboolean normalized);
    void loadAttributes(unsigned int VAO);
    void loadAttributes(unsigned int VAO, unsigned int VBO);

    Renderable* createRenderable(unsigned int VBO, std::vector<unsigned char> data, unsigned int vertexCount, GLenum usage, Shader* shader);
    void loadVertexData(unsigned int VBO);

    bool isLoaded(unsigned int VAO, unsigned int VBO);
};


#endif