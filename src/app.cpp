#include <app.h>


Application::Application(GLFWwindow* window)
    : window(window),
      keyboardController(window)
{
    camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);

    auto flightController = std::make_shared<FlightController>(camera);

    mouseController.registerObserver(flightController);
    keyboardController.registerObserver(flightController);

    keyboardController.registerKey(GLFW_KEY_W);
    keyboardController.registerKey(GLFW_KEY_A);
    keyboardController.registerKey(GLFW_KEY_S);
    keyboardController.registerKey(GLFW_KEY_D);

    glfwSetFramebufferSizeCallback(window, Application::frameBufferSizeCallback);
    glfwSetCursorPosCallback(window, Application::cursorPosCallback);
    glfwSetWindowUserPointer(window, this);
}

void Application::setupCubeScene()
{
    std::vector<float> cubeVertexData = VertexFactory::getColoredCube();
    auto shaderPtr = std::make_shared<Shader>("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    auto renderCollection = std::make_shared<RenderCollection>(shaderPtr);
    renderCollection->addVertexAttribute(3, GL_FLOAT, GL_FALSE);
    renderCollection->addVertexAttribute(3, GL_FLOAT, GL_FALSE);

    renderCollection->createRenderable("cube", cubeVertexData, 36);

    for (float x : {-5.0f, 5.0f})
    {
        for (float y : {-5.0f, 5.0f})
        {
            for (float z : {-5.0f, 5.0f})
            {
                glm::vec3 position(x, y, z);
                renderCollection->createEntity("cube", position);
            }
        }
    }
    
    renderCollection->loadRemote();
    renderCollections.push_back(renderCollection);
}

void Application::startMainLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        keyboardController.processKeyboardInput();

        glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 viewMatrix = camera->getViewMatrix();
        glm::mat4 projectionMatrix = camera->getProjectionMatrix();

        for (const auto& renderCollection : renderCollections)
            renderCollection->renderEntities(viewMatrix, projectionMatrix);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
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