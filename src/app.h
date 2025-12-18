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
#include "pipeline/shader.h"
#include "model/renderable.h"


class Application
{
private:
    GLFWwindow* window;

    Camera camera;
    glm::mat4 projectionMatrix;

    std::vector<Renderable> renderables;

    std::vector<std::unique_ptr<Shader>> shaders;
    // VertexManager vertexManager;

    // std::vector<std::unique_ptr<Renderable>> renderables;
    // std::map<unsigned int, std::map<unsigned int, std::vector<Entity>>> entityMap;

    KeyboardController keyboardController;
    MouseController mouseController;

public:
    Application(int width, int height);
    void stop();

    void setupCubeScene();
    void startMainLoop();

    static void frameBufferSizeCallback(GLFWwindow*, int width, int height);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
};


#endif