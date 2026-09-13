#include "dugl/modelling/scene.h"
#include "dugl/utils/glad_include.h"

#include <iostream>


void Scene::render()
{
    for (auto& [shaderId, shader] : shaders) {
        if (shader->hasActiveUniform("directionalLight.direction")) {
            shader->setDirectionalLight(directionalLight);
        }
    }
    for (auto& [groupId, group] : entityGroups) {
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

Shader* Scene::getShader(dugl::uint shaderId)
{
    auto it = shaders.find(shaderId);
    return it == shaders.end() ? nullptr : it->second.get();
}

std::vector<Shader*> Scene::getShaders()
{
    std::vector<Shader*> rawShaders;
    rawShaders.reserve(shaders.size());
    for (auto& [shaderId, shader] : shaders) {
        rawShaders.push_back(shader.get());
    }
    return rawShaders;
}

void Scene::setSkybox(const char* path, Shader* shader)
{
    skybox.init(path, shader);
}

Renderable* Scene::addRenderable(std::unique_ptr<Renderable> renderable)
{
    return renderables.emplace_back(std::move(renderable)).get();
}


std::vector<Entity*> Scene::getEntities()
{
    std::vector<Entity*> rawEntities;
    rawEntities.reserve(entities.size());
    for (auto& entity : entities) {
        rawEntities.push_back(entity.get());
    }
    return rawEntities;
}

void Scene::clearEntities()
{
    entityGroups.clear();
    entities.clear();
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
