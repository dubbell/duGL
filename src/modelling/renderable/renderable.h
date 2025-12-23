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
    
    void render(Shader* shader);

private:
    std::vector<Mesh> meshes;
    std::string directory;
};

#endif