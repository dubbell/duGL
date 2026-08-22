#pragma once

#include "dugl/shading/shader.h"


class Skybox
{
private:
    unsigned int VAO, VBO, textureId;
    Shader* shader;

public:
    Skybox();
    void setShader(Shader* shader);
    void loadSkybox(const char* path);
    void draw();
};