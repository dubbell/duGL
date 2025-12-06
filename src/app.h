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


class Application
{
private:
    GLFWwindow* window;
    Camera camera;
    Shader shader;



    KeyboardController keyboardController;
    MouseController mouseController;

public:
    Application(GLFWwindow* window);

    void createArrayEntity(std::string &name, float vertices[], int vertexCount);
    void createElementArrayEntity(std::string &name, float vertices[], float indices[], int vertexCount, int indexCount);

    void startMainLoop();

    static void frameBufferSizeCallback(GLFWwindow*, int width, int height);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
};


#endif