#ifndef APP_H
#define APP_H

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <GLFW/glfw3.h>
#include <string>

#include <camera.h>
#include <keyboard.h>
#include <mouse.h>
#include <shader.h>
#include <render_collection.h>
#include <vertex_factory.h>


class Application
{
private:
    GLFWwindow* window;
    Camera camera;
    glm::mat4 projectionMatrix;

    std::vector<RenderCollection> renderCollections;

    KeyboardController keyboardController;
    MouseController mouseController;

public:
    Application(GLFWwindow* window);

    void setupCubeScene();
    void startMainLoop();

    static void frameBufferSizeCallback(GLFWwindow*, int width, int height);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
};


#endif