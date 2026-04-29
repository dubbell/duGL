#pragma once

#include "dugl/shading/shader.h"


class Skybox
{
public:
    Skybox();
    void setShader(Shader* shader);
    void loadSkybox(const char* path);
    void draw();

private:
    unsigned int VAO, VBO, textureId;
    Shader* shader;
};