#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <vector>
#include <string>
#include <iostream>

#include "utils/stb_image.h"
#include "shading/shader.h"
#include "mesh.h"


class Renderable
{
public:
    Renderable(std::string directory, std::vector<Mesh> meshes);
    
    void render();
    void render(Shader* shader);

    void setDefaultShader(Shader* shader);

private:
    Shader* defaultShader;
    std::vector<Mesh> meshes;
    std::string directory;
};

#endif