#pragma once

#include "dugl/shading/shader.h"
#include "dugl/common.h"


class Skybox
{
private:
    uint VAO = 0; 
    uint VBO = 0;
    uint textureId = 0;
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