#pragma once

#include <vector>
#include <map>
#include <memory>
#include <type_traits>

#include "renderable.h"
#include "entity.h"
#include "entity_group.h"
#include "standard_entity_group.h"
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
    std::map<dugl::uint, std::unique_ptr<EntityGroup>> entityGroups;
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
    void render();

    Shader* addShader(std::unique_ptr<Shader> shader);
    Shader* getShader(dugl::uint) const;

    void setSkybox(const char* path, Shader* shader);

    Renderable* addRenderable(std::unique_ptr<Renderable> renderable);

    // Add an entity to the scene. The scene takes ownership of the entity's lifecycle.
    template <class T>
    T* addEntity(std::unique_ptr<T> entity)
    {
        static_assert(std::is_base_of_v<Entity, T>, "addEntity() requires an Entity subclass");

        T* ptr = entity.get();
        entities.push_back(std::move(entity));
        return ptr;
    }

    // Add an entity group to the scene. The scene takes ownership of the entity group's lifecycle.
    template <class T>
    T* addGroup(dugl::uint groupId, std::unique_ptr<T> group)
    {
        static_assert(std::is_base_of_v<EntityGroup, T>, "addGroup() requires an EntityGroup subclass");

        T* ptr = group.get();
        entityGroups.insert_or_assign(groupId, std::move(group));
        return ptr;
    }

    template <class T>
    T* getGroup(dugl::uint groupId) const
    {
        auto it = entityGroups.find(groupId);
        return it == entityGroups.end() ? nullptr : dynamic_cast<T*>(it->second.get());
    }

    void setDirectionalLight(const DirectionalLight& directionalLight);
    DirectionalLight& getDirectionalLight() { return directionalLight; }

    void setPointLights(std::vector<PointLight> pointLights);
    void addPointLight(const PointLight& pointLight);
    std::vector<PointLight>& getPointLights() { return pointLights; }
};
