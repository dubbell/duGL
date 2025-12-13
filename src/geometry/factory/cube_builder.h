#ifndef CUBE_BUILDER_H
#define CUBE_BUILDER_H

#include <vertex_builder.h>
#include <stb_image.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <vertex_manager.h>
#include <shader.h>

#include <iostream>
#include <map>
#include <stdexcept>
#include <array>




class CubeBuilder : public VertexBuilder
{
protected:
    std::vector<std::array<float, 3>> getVertexPositions() override;
    std::vector<std::array<float, 2>> getVertexTextureCoords() override;
    std::array<float, 3> getVertexColor(std::array<float, 3> position) override;
    Material getMaterial() override;

public:
    CubeBuilder(VertexManager* vertexManager);
};


#endif