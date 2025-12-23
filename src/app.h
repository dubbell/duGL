#ifndef APP_H
#define APP_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "controllers/mouse.h"
#include "controllers/keyboard.h"
#include "app_state.h"


class Application
{
public:
    Application(int width, int height) : applicationState({ width, height })
    {
        // window initialization
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(width, height, "duGL", NULL, NULL);
        if (window == NULL) {
            std::cout << "Failed to create GLFW window." << std::endl;
        }
        glfwMakeContextCurrent(window);
        if (!gladLoadGL(glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD." << std::endl;
        }
        glViewport(0, 0, width, height);

        keyboardController.setWindow(window);

        glfwSetFramebufferSizeCallback(window, Application::frameBufferResizeCallback_);
        glfwSetCursorPosCallback(window, Application::cursorPosCallback_);
        glfwSetWindowUserPointer(window, this);
    }

    void stop()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

protected:
    GLFWwindow* window;
    
    ApplicationState applicationState;
    
    KeyboardController keyboardController;
    MouseController mouseController;

    virtual void frameBufferResizeCallback(int width, int height) {}
    virtual void cursorPosCallback(float xpos, float ypos) {}

private:
    static void frameBufferResizeCallback_(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
        Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
        application->applicationState.screenWidth = width;
        application->applicationState.screenHeight = height;
        application->frameBufferResizeCallback(width, height);
    }

    static void cursorPosCallback_(GLFWwindow* window, double xpos, double ypos)
    {
        Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
        application->mouseController.cursorPosCallback((float)xpos, (float)ypos);
        application->cursorPosCallback((float)xpos, (float)ypos);
    }
};

#endif