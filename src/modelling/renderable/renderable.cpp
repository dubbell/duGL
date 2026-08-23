#include "dugl/modelling/renderable.h"


Renderable::Renderable(std::vector<Mesh> meshes) : meshes(std::move(meshes))
{}

void Renderable::render(Shader* shader)
{
    for (Mesh& mesh : meshes) {
        mesh.render(shader, true);
    }
}
