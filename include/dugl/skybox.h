#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>

#include <glad/gl.h>

#include "stb_image.h"
#include "shader.h"


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

#endif