#ifndef APP_H
#define APP_H

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <GLFW/glfw3.h>
#include <string>
#include <memory>
#include <map>

#include <camera.h>
#include <keyboard.h>
#include <mouse.h>
#include <shader.h>
#include <vertex_factory.h>
#include <flight_controller.h>
#include <entity.h>
#include <vertex_manager.h>


struct RenderTarget
{
public:
    unsigned int VAO, VBO;
    Shader* shader;
    std::vector<Entity> entities;

    RenderTarget(unsigned int vao, unsigned int vbo, Shader* shdr, std::vector<Entity> ents)
        : VAO(vao), VBO(vbo), shader(shdr), entities(std::move(ents)) {};
};

class Application
{
private:
    GLFWwindow* window;

    Camera camera;
    glm::mat4 projectionMatrix;

    std::vector<std::unique_ptr<Shader>> shaders;
    VertexManager vertexManager;

    std::vector<RenderTarget> renderTargets;

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