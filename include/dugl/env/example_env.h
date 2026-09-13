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

    glm::vec4 clearColor;

public:
    ExampleEnvironment();

protected:
    void update(float dt) override;
    void postRender() override;

private:
    void createPrimitives(StandardEntityGroup* entityGroup);

    void createImGuiFrame();
    void drawImGui();

    virtual void frameBufferResizeCallback(int width, int height) override;
};