#pragma once

#include <vector>

#include "dugl/shading/shader.h"
#include "mesh.h"


// An object that can be rendered on the screen, constructed from a collection
// of Meshes. Like its Meshes, the Renderable is still in object space. Renderables
// are instantiated in world space with Entity objects.
class Renderable
{
private:
    std::vector<Mesh> meshes;

public:
    Renderable(std::vector<Mesh> meshes);
    void render(Shader* shader);
};
