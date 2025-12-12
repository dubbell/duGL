#ifndef VERTEX_BUILDER_H
#define VERTEX_BUILDER_H

#include <tuple>
#include <vector>
#include <string>

#include <renderable.h>
#include <vertex_manager.h>


class VertexBuilder
{
protected:
    VertexManager* _vertexManager;
    unsigned int _VAO, _VBO;
    GLenum _usage;

    int vertexCount;
    bool enableTextures, enableColors;

    std::vector<std::vector<float>> colors;
    std::vector<unsigned int> textures;

    virtual int getVertexCount();
    virtual std::vector<unsigned char> getData();

public:
    virtual VertexBuilder* addTexture(std::string &texturePath) = 0;
    virtual VertexBuilder* addColors(std::vector<std::vector<float>> &colors) = 0;
    virtual VertexBuilder* setVAO(unsigned int VAO) = 0;
    virtual VertexBuilder* setVBO(unsigned int VBO) = 0;
    virtual VertexBuilder* setUsage(GLenum usage) = 0;

    virtual std::unique_ptr<Renderable> build();
};

#endif