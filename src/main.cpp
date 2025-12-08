#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <app.h>
#include <init_utils.h>


int main()
{
    GLFWwindow* window;
    initialize(&window);

    Application* application = new Application(window);

    application->setupCubeScene();
    application->startMainLoop();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}