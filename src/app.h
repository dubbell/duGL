#ifndef APP_H
#define APP_H

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <GLFW/glfw3.h>
#include <string>
#include <memory>
#include <map>
#include <string>
#include <format>
#include <algorithm>

#include "view/camera.h"
#include "controllers/keyboard.h"
#include "controllers/mouse.h"
#include "controllers/flight_controller.h"
#include "shading/shader.h"
#include "shading/lighting.h"
#include "model/renderable.h"
#include "model/entity.h"
#include "model/skybox.h"


enum class ShaderType
{
    ObjectShader,
    CubeMapShader
};


class Application
{
private:
    GLFWwindow* window;

    Camera camera;

    Skybox skybox;

    std::vector<std::unique_ptr<Renderable>> renderables;
    std::vector<std::unique_ptr<Entity>> entities;

    std::map<ShaderType, std::unique_ptr<Shader>> shaders;

    KeyboardController keyboardController;
    MouseController mouseController;

    glm::vec4 clearColor;
    void clearBuffers();

public:
    Application(int width, int height);
    
    void startMainLoop();
    void stop();

    static void frameBufferSizeCallback(GLFWwindow*, int width, int height);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
};


#endif