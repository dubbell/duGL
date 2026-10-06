#pragma once

#include <vector>
#include <map>
#include <memory>
#include <type_traits>

#include "renderable.h"
#include "entity.h"
#include "render_pass.h"
#include "skybox.h"
#include "dugl/shading/shader.h"
#include "dugl/shading/material.h"
#include "dugl/modelling/renderable_builder.h"


class Scene
{
private:
    Skybox skybox;

    std::vector<std::unique_ptr<Renderable>> renderables;
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<RenderPass>> renderPasses;
    std::vector<std::vector<Entity*>> passEntities;
    std::map<dugl::uint, std::unique_ptr<Shader>> shaders;

    // a neutral overhead light, so a Scene renders sensibly before any lighting is set
    DirectionalLight directionalLight = {
        0.0f, -60.0f,
        glm::vec3(0.3f),
        glm::vec3(0.5f),
        glm::vec3(1.0f)
    };
    std::vector<PointLight> pointLights;

public:
    Scene();

    void render();

    Shader* addShader(std::unique_ptr<Shader> shader);
    Shader* getShader(dugl::uint);
    std::vector<Shader*> getShaders();

    void setSkybox(const char* path, Shader* shader);

    Renderable* addRenderable(std::unique_ptr<Renderable> renderable);

    // Add an entity to the scene. The scene takes ownership of the entity's lifecycle.
    template <class T> requires std::is_base_of_v<Entity, T>
    T* addEntity(std::unique_ptr<T> entity) {
        T* ptr = entity.get();
        entities.push_back(std::move(entity));
        return ptr;
    }

    template <class T> requires std::is_base_of_v<RenderPass, T>
    T* addRenderPass(std::unique_ptr<T> renderPass) {
        T* ptr = renderPass.get();
        renderPasses.push_back(std::move(renderPass));
        passEntities.emplace_back();
        return ptr;
    }

    std::vector<Entity*> getEntities();
    void clearEntities();

    void setDirectionalLight(const DirectionalLight& directionalLight);
    DirectionalLight& getDirectionalLight() { return directionalLight; }

    void setPointLights(std::vector<PointLight> pointLights);
    void addPointLight(const PointLight& pointLight);
    std::vector<PointLight>& getPointLights() { return pointLights; }
};
