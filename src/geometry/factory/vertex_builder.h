#ifndef VERTEX_BUILDER_H
#define VERTEX_BUILDER_H

#include <vertex_builder.h>
#include <stb_image.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <vertex_manager.h>
#include <shader.h>
#include <renderable.h>

#include <iostream>
#include <map>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <array>
#include <string>




class VertexBuilder
{
protected:
    VertexManager* _vertexManager;
    unsigned int _VAO, _VBO;
    GLenum _usage;

    int vertexCount;
    bool useTextures, useColors, useNormals;

    std::vector<std::array<float, 3>> colors;
    std::vector<unsigned int> textures;
    Shader* _shader;

    VertexBuilder();
    VertexBuilder(VertexManager* vertexManager, int numVertices);

    virtual std::vector<std::array<float, 3>> getVertexPositions() = 0;
    virtual std::vector<std::array<float, 2>> getVertexTextureCoords() = 0;
    virtual std::array<float, 3> getVertexColor(std::array<float, 3> position) = 0;
    std::array<float, 3> getNormal(std::array<std::array<float, 3>, 3> positions);

    std::vector<unsigned char> getData();

public:
    VertexBuilder* addTexture(std::string &texturePath);
    VertexBuilder* addColors(std::vector<std::array<float, 3>> &colors);
    VertexBuilder* enableNormals();
    VertexBuilder* setVAO(unsigned int VAO);
    VertexBuilder* setVBO(unsigned int VBO);
    VertexBuilder* setUsage(GLenum usage);
    VertexBuilder* setShader(Shader* shader);

    std::unique_ptr<Renderable> build();
};

#endif