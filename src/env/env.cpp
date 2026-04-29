#include "dugl/env/env.h"
#include "dugl/utils/glfw_include.h"

#include <iostream>


Environment::Environment() : viewportWidth(1920), viewportHeight(1080), freeCursor(false), mouseController(this)
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

    glfwSetFramebufferSizeCallback(window, Environment::glfwFrameBufferResizeCallback);
    glfwSetWindowUserPointer(window, this);

    // enable MSAA in OpenGL
    glEnable(GL_MULTISAMPLE);

    activeCamera = cameras.emplace_back(std::make_unique<Camera>()).get();
    activeCamera->setAspectRatio((float)16 / 9);
}

void Environment::stop()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

GLFWwindow* Environment::getWindow()
{
    return window;
}

std::pair<int, int> Environment::getViewportSize()
{
    return { viewportWidth, viewportHeight };
}

Camera* Environment::getActiveCamera()
{
    return activeCamera;
}

bool Environment::getFreeCursor()
{
    return freeCursor;
}

void Environment::setFreeCursor(bool freeCursor)
{
    this->freeCursor = freeCursor;
}

void Environment::glfwFrameBufferResizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    Environment* env = static_cast<Environment*>(glfwGetWindowUserPointer(window));
    env->viewportWidth = width;
    env->viewportHeight = height;
    env->frameBufferResizeCallback(width, height);
}