#ifndef APP_H
#define APP_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <utility>

#include "controllers/mouse.h"
#include "controllers/keyboard.h"
#include "interfaces/player_interface.h"



class Game : public PlayerInterface
{
public:
    Game() : viewportWidth(1920), viewportHeight(1080), freeCursor(false), mouseController(this)
    {
        // window initialization
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // anti-aliasing MSAA (tell GLFW to create larger buffer)
        glfwWindowHint(GLFW_SAMPLES, 4);

        window = glfwCreateWindow(viewportWidth, viewportHeight, "duGL", NULL, NULL);
        if (window == NULL) {
            std::cout << "Failed to create GLFW window." << std::endl;
        }
        glfwMakeContextCurrent(window);
        if (!gladLoadGL(glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD." << std::endl;
        }
        glViewport(0, 0, viewportWidth, viewportHeight);

        keyboardController.setWindow(window);

        glfwSetFramebufferSizeCallback(window, Game::frameBufferResizeCallback_);
        glfwSetWindowUserPointer(window, this);

        // enable MSAA in OpenGL
        glEnable(GL_MULTISAMPLE);

        activeCamera = cameras.emplace_back(std::make_unique<Camera>()).get();
        activeCamera->setAspectRatio((float)16 / 9);
    }

    void stop()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    GLFWwindow* getWindow() override
    {
        return window;
    }

    bool getFreeCursor() override
    {
        return freeCursor;
    }

    void setFreeCursor(bool freeCursor) override
    {
        this->freeCursor = freeCursor;
    }

    std::pair<int, int> getViewportSize() override
    {
        return { viewportWidth, viewportHeight };
    }

    Camera* getActiveCamera() override
    {
        return activeCamera;
    }
    
protected:
    GLFWwindow* window;
    int viewportWidth, viewportHeight;

    std::vector<std::unique_ptr<Camera>> cameras;
    Camera* activeCamera;

    bool freeCursor;
    
    KeyboardController keyboardController;
    MouseController mouseController;

    virtual void frameBufferResizeCallback(int width, int height) {}

private:
    static void frameBufferResizeCallback_(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
        Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
        game->viewportWidth = width;
        game->viewportHeight = height;
        game->frameBufferResizeCallback(width, height);
    }
};

#endif