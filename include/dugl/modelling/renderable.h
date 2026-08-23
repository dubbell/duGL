#pragma once

#include <vector>

#include "dugl/shading/shader.h"
#include "mesh.h"


class Renderable
{
private:
    std::vector<Mesh> meshes;

public:
    Renderable(std::vector<Mesh> meshes);

    void render(Shader* shader);
};
