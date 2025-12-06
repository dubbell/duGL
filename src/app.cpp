#include <app.h>


Application::Application(GLFWwindow* window)
    : window(window),
      camera(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f),
      shader("assets/shaders/basic.vert", "assets/shaders/basic.frag"),
      keyboardController(window)
{
    mouseController.setCamera(&camera);

    keyboardController = KeyboardController(window);

    keyboardController.registerKey(GLFW_KEY_W);
    keyboardController.registerKey(GLFW_KEY_A);
    keyboardController.registerKey(GLFW_KEY_S);
    keyboardController.registerKey(GLFW_KEY_D);

    keyboardController.registerKeyboardObserver(&camera);

    glfwSetFramebufferSizeCallback(window, Application::frameBufferSizeCallback);
    glfwSetCursorPosCallback(window, Application::cursorPosCallback);

    glfwSetWindowUserPointer(window, this);
}



void Application::createArrayObject(std::string &name, float vertices[], int size)
{
    
}



void Application::frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Application::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    application->mouseController.cursorPosCallback((float)xpos, (float)ypos);
}