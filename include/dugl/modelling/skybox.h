#pragma once

#include "dugl/shading/shader.h"
#include "dugl/common.h"


// For rendering a background image, in all directions.
class Skybox
{
private:
    dugl::uint VAO = 0;
    dugl::uint VBO = 0;
    dugl::uint textureId = 0;
    Shader* shader;

public:
    Skybox() = default;
    ~Skybox();

    // delete copy functionality
    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;

    void init(const char* path, Shader* shader);
    void draw();
};