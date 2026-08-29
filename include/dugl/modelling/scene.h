#pragma once

#include <vector>
#include <map>
#include <memory>
#include <type_traits>

#include "renderable.h"
#include "entity.h"
#include "skybox.h"
#include "dugl/shading/shader.h"
#include "dugl/shading/material.h"
#include "dugl/modelling/renderable_builder.h"


enum class ShaderType
{
    ObjectShader,
    CubeMapShader
};

class Scene
{
private:
    Skybox skybox;

    std::vector<std::unique_ptr<Renderable>> renderables;
    std::vector<std::unique_ptr<Entity>> entities;
    std::map<ShaderType, std::unique_ptr<Shader>> shaders;

    // a neutral overhead light, so a Scene renders sensibly before any lighting is set
    DirectionalLight directionalLight = {
        0.0f, -60.0f,
        glm::vec3(0.3f),
        glm::vec3(0.5f),
        glm::vec3(1.0f)
    };
    std::vector<PointLight> pointLights;

public:
    void render();

    Shader* setShader(ShaderType type, const char* vertexPath, const char* fragmentPath);
    Shader* setShader(ShaderType type, std::unique_ptr<Shader> shader);

    Shader* getShader(ShaderType type) const;

    void setSkybox(const char* path, Shader* shader);
    void setSkybox(const char* path);

    Renderable* createRenderable(RenderableBuilder& builder);
    Renderable* createRenderable(RenderableBuilder&& builder) { return createRenderable(builder); }

    Renderable* addRenderable(std::unique_ptr<Renderable> renderable);

    Entity* createEntity(Renderable* renderable);
    Entity* createEntity(Renderable* renderable, glm::vec3 position);

    template <class T>
    T* addEntity(std::unique_ptr<T> entity)
    {
        static_assert(std::is_base_of_v<Entity, T>, "addEntity() requires an Entity subclass");

        T* ptr = entity.get();
        entities.push_back(std::move(entity));
        return ptr;
    }

    void setDirectionalLight(const DirectionalLight& directionalLight);
    DirectionalLight& getDirectionalLight() { return directionalLight; }

    void setPointLights(std::vector<PointLight> pointLights);
    void addPointLight(const PointLight& pointLight);
    std::vector<PointLight>& getPointLights() { return pointLights; }
};
