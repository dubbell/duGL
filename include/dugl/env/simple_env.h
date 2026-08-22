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
    template<typename T> using vector = std::vector<T>;
    template<typename T> using unique_ptr = std::unique_ptr<T>;
    template<typename T, typename U> using map = std::map<T, U>;

private:
    FlightController flightController;

    UniformBuffer uboPerspective;

    Skybox skybox;

    vector<unique_ptr<Renderable>> renderables;
    vector<unique_ptr<Entity>> entities;
    map<ShaderType, unique_ptr<Shader>> shaders;

    DirectionalLight directionalLight;
    vector<PointLight> pointLights;

    glm::vec4 clearColor;

public:
    ExampleEnvironment();
    void start() override;

private:
    Renderable* createRenderable(const char* renderablePath);
    Entity* createEntity(Renderable* renderable, glm::vec3 position);
    OutlinedEntity* createOutlinedEntity(Renderable* renderable, glm::vec3 position, Shader* outlineShader);

    Shader* createShader(const char* vertexShaderPath, const char* fragmentShaderPath, ShaderType type);

    void createImGuiFrame();
    void drawImGui();
    void clearBuffers();

    virtual void frameBufferResizeCallback(int width, int height) override;
};