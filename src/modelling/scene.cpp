#include "dugl/modelling/scene.h"

#include <iostream>


void Scene::render()
{
    Shader* objectShader = getShader(ShaderType::ObjectShader);

    if (objectShader != nullptr)
    {
        objectShader->use();

        objectShader->setDirectionalLight(directionalLight);
        objectShader->setPointLights(pointLights);

        for (auto& entity : entities)
        {
            entity->render(objectShader);
        }
    }

    skybox.draw();
}


Shader* Scene::setShader(ShaderType type, const char* vertexPath, const char* fragmentPath)
{
    return setShader(type, std::make_unique<Shader>(vertexPath, fragmentPath));
}


Shader* Scene::setShader(ShaderType type, std::unique_ptr<Shader> shader)
{
    Shader* ptr = shader.get();
    shaders[type] = std::move(shader);
    return ptr;
}


Shader* Scene::getShader(ShaderType type) const
{
    auto it = shaders.find(type);
    return it == shaders.end() ? nullptr : it->second.get();
}


void Scene::setSkybox(const char* path, Shader* shader)
{
    skybox.init(path, shader);
}


void Scene::setSkybox(const char* path)
{
    Shader* cubeMapShader = getShader(ShaderType::CubeMapShader);

    if (cubeMapShader == nullptr)
    {
        std::cerr << "Warning: no cube map shader set; the skybox '" << path
            << "' will not be drawn." << std::endl;
        return;
    }

    setSkybox(path, cubeMapShader);
}


Renderable* Scene::createRenderable(RenderableBuilder& builder)
{
    return addRenderable(std::make_unique<Renderable>(builder.build()));
}


Renderable* Scene::addRenderable(std::unique_ptr<Renderable> renderable)
{
    return renderables.emplace_back(std::move(renderable)).get();
}


Entity* Scene::createEntity(Renderable* renderable)
{
    return addEntity(std::make_unique<Entity>(renderable));
}


Entity* Scene::createEntity(Renderable* renderable, glm::vec3 position)
{
    return addEntity(std::make_unique<Entity>(renderable, position));
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
