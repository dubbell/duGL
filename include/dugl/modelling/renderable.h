#pragma once

#include <vector>
#include <string>

#include "dugl/shading/shader.h"
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