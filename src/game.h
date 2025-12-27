#ifndef GAME_H
#define GAME_H

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <string>
#include <memory>
#include <map>
#include <string>
#include <format>
#include <algorithm>

#include "app.h"
#include "view/camera.h"
#include "controllers/keyboard.h"
#include "controllers/mouse.h"
#include "controllers/flight_controller.h"
#include "shading/shader.h"
#include "shading/lighting.h"
#include "shading/uniform_buffer.h"
#include "shading/ubo_templates.h"
#include "modelling/renderable/renderable.h"
#include "modelling/renderable/model_builder.h"
#include "modelling/entity/entity.h"
#include "modelling/entity/outlined_entity.h"
#include "modelling/skybox.h"
#include "gui/utils.h"


enum class ShaderType
{
    ObjectShader,
    OutlineShader,
    CubeMapShader
};

class ExampleGame : public Game
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
    ExampleGame();
    void startMainLoop();
};


#endif