#include "renderable.h"


Renderable::Renderable(std::string directory, std::vector<Mesh> meshes)
    : defaultShader(nullptr), directory(directory), meshes(std::move(meshes))
{}

void Renderable::render()
{
    if (defaultShader == nullptr)
        std::cout << "Model rendering error: model rendered without shader" << std::endl;
    
    render(defaultShader);
}

void Renderable::setDefaultShader(Shader* shader)
{
    this->defaultShader = shader;
}

void Renderable::render(Shader* shader)
{
    for (Mesh mesh : meshes) mesh.render(shader, true);
}