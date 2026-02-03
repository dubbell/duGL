#include "dugl/renderable.h"


Renderable::Renderable(std::string directory, std::vector<Mesh> meshes)
    : directory(directory), meshes(std::move(meshes))
{}

void Renderable::render(Shader* shader)
{
    for (Mesh mesh : meshes) mesh.render(shader, true);
}