#include "renderable.h"


Renderable::Renderable(std::string directory, std::vector<Mesh> meshes)
    : directory(directory), meshes(std::move(meshes))
{}

void Renderable::draw(Shader* shader)
{
    for (Mesh mesh : meshes) mesh.draw(shader);
}