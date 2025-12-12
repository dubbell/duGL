#ifndef CUBE_TEMPLATE_H
#define CUBE_TEMPLATE_H

#include <vertex_builder.h>
#include <stb_image.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <vertex_manager.h>
#include <shader.h>

#include <iostream>
#include <map>
#include <stdexcept>




class CubeBuilder : public VertexBuilder
{
private:
    VertexManager* _vertexManager;
    unsigned int _VAO, _VBO;
    GLenum _usage;

    int vertexCount;
    bool enableTextures, enableColors;

    std::vector<std::vector<float>> colors;
    std::vector<unsigned int> textures;
    Shader* _shader;
    
    int getVertexCount() override;
    std::vector<unsigned char> getData() override;
    
public:
    CubeBuilder(VertexManager* vertexManager);

    VertexBuilder* addTexture(std::string &texturePath) override;
    VertexBuilder* addColors(std::vector<std::vector<float>> &colors) override;
    VertexBuilder* setVAO(unsigned int VAO) override;
    VertexBuilder* setVBO(unsigned int VBO) override;
    VertexBuilder* setUsage(GLenum usage) override;
    VertexBuilder* setShader(Shader* shader) override;

    std::unique_ptr<Renderable> build() override;
};


#endif