#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>

#include <glad/gl.h>

#include "utils/stb_image.h"
#include "shading/shader.h"


class Skybox
{
public:
    Skybox();
    void setShader(Shader* shader);
    void loadSkybox(const char* path);
    void draw(glm::mat4 cameraView, glm::mat4 cameraProjection);

private:
    unsigned int VAO, VBO, textureId;
    Shader* shader;
};

#endif