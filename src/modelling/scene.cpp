#include "dugl/modelling/scene.h"

#include <iostream>


void Scene::render()
{
    for (auto& [groupId, group] : entityGroups)
    {
        group->render();
    }
    skybox.draw();
}

Shader* Scene::addShader(std::unique_ptr<Shader> shader)
{
    Shader* shader_ptr = shader.get();
    shaders.insert_or_assign(shader->ID, std::move(shader));
    return shader_ptr;
}

Shader* Scene::getShader(dugl::uint shaderId) const
{
    auto it = shaders.find(shaderId);
    return it == shaders.end() ? nullptr : it->second.get();
}

void Scene::setSkybox(const char* path, Shader* shader)
{
    skybox.init(path, shader);
}

Renderable* Scene::addRenderable(std::unique_ptr<Renderable> renderable)
{
    return renderables.emplace_back(std::move(renderable)).get();
}

void Scene::setDirectionalLight(const DirectionalLight& directionalLight)
{
    this->directionalLight = directionalLight;
}

void Scene::setPointLights(std::vector<PointLight> pointLights)
{
    this->pointLights = std::move(pointLights);
}

void Scene::addPointLight(const PointLight& pointLight)
{
    pointLights.push_back(pointLight);
}
