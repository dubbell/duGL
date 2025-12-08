#ifndef APP_H
#define APP_H

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <GLFW/glfw3.h>
#include <string>
#include <memory>

#include <camera.h>
#include <keyboard.h>
#include <mouse.h>
#include <shader.h>
#include <render_collection.h>
#include <vertex_factory.h>
#include <flight_controller.h>


class Application
{
private:
    GLFWwindow* window;

    std::shared_ptr<Camera> camera;
    glm::mat4 projectionMatrix;

    std::vector<std::shared_ptr<RenderCollection>> renderCollections;

    std::shared_ptr<KeyboardController> keyboardController;
    std::shared_ptr<MouseController> mouseController;

public:
    Application(int width, int height);
    void stop();

    void setupCubeScene();
    void startMainLoop();

    static void frameBufferSizeCallback(GLFWwindow*, int width, int height);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
};


#endif