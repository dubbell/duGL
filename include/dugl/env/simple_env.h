#pragma once

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include "env.h"
#include "dugl/controllers/flight_controller.h"
#include "dugl/shading/uniform_buffer.h"
#include "dugl/modelling/entity.h"
#include "dugl/modelling/outlined_entity.h"
#include "dugl/modelling/renderable.h"
#include "dugl/modelling/skybox.h"


enum class ShaderType
{
    ObjectShader,
    OutlineShader,
    CubeMapShader
};

class ExampleEnvironment : public Environment
{
private:
    FlightController flightController;

    UniformBuffer uboPerspective;

    Skybox skybox;

    std::vector<std::unique_ptr<Renderable>> renderables;
    std::vector<std::unique_ptr<Entity>> entities;
    std::map<ShaderType, std::unique_ptr<Shader>> shaders;

    DirectionalLight directionalLight;
    std::vector<PointLight> pointLights;

    glm::vec4 clearColor;
    void clearBuffers();

    Renderable* createRenderable(const char* renderablePath);

    Entity* createEntity(Renderable* renderable, glm::vec3 position);
    OutlinedEntity* createOutlinedEntity(Renderable* renderable, glm::vec3 position, Shader* outlineShader);

    Shader* createShader(const char* vertexShaderPath, const char* fragmentShaderPath, ShaderType type);

    virtual void frameBufferResizeCallback(int width, int height) override;

    void createImGuiFrame();
    void drawImGui();

public:
    ExampleEnvironment();

    void start() override;
};