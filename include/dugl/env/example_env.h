#pragma once

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include "env.h"
#include "dugl/controllers/flight_controller.h"
#include "dugl/shading/uniform_buffer.h"
#include "dugl/modelling/entity.h"
#include "dugl/modelling/hoverable_entity.h"
#include "dugl/modelling/entity_group.h"
#include "dugl/modelling/standard_entity_group.h"
#include "dugl/modelling/outlined_entity_group.h"
#include "dugl/modelling/renderable.h"
#include "dugl/modelling/renderable_builder.h"
#include "dugl/modelling/skybox.h"


enum class ExampleShaderType
{
    ObjectShader,
    OutlineShader,
    CubeMapShader
};

class ExampleEnvironment : public Environment
{
private:
    FlightController flightController;

    UniformBuffer perspectiveUbo;

    Skybox skybox;

    std::vector<std::unique_ptr<Renderable>> renderables;
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<EntityGroup>> entityGroups;
    std::map<ExampleShaderType, std::unique_ptr<Shader>> shaders;

    StandardEntityGroup* standardGroup = nullptr;
    OutlinedEntityGroup* outlinedGroup = nullptr;

    DirectionalLight directionalLight;
    std::vector<PointLight> pointLights;

    glm::vec4 clearColor;

public:
    ExampleEnvironment();
    void start() override;

private:
    void createPrimitives();

    Renderable* createRenderable(RenderableBuilder& builder);
    Renderable* createRenderable(RenderableBuilder&& builder) { return createRenderable(builder); }

    Entity* createEntity(Renderable* renderable, glm::vec3 position);
    HoverableEntity* createHoverableEntity(Renderable* renderable, glm::vec3 position);

    template <class T>
    T* addGroup(std::unique_ptr<T> group)
    {
        T* ptr = group.get();
        entityGroups.push_back(std::move(group));
        return ptr;
    }

    Shader* createShader(const char* vertexShaderPath, const char* fragmentShaderPath, ExampleShaderType type);

    void createImGuiFrame();
    void drawImGui();
    void clearBuffers();

    virtual void frameBufferResizeCallback(int width, int height) override;
};