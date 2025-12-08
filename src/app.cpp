#include <app.h>


Application::Application(int width, int height)
    : camera(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f), mouseController(), keyboardController()
{
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
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    keyboardController.setWindow(window);

    auto flightController = std::make_shared<FlightController>(window, &camera);

    mouseController.registerObserver(flightController);
    keyboardController.registerObserver(flightController);
    for (int key : flightController->getActiveKeys())
        keyboardController.registerKey(key);

    glfwSetFramebufferSizeCallback(window, Application::frameBufferSizeCallback);
    glfwSetCursorPosCallback(window, Application::cursorPosCallback);
    glfwSetWindowUserPointer(window, this);
}

void Application::stop()
{
    glfwDestroyWindow(window);
    glfwTerminate();
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

        glm::mat4 viewMatrix = camera.getViewMatrix();
        glm::mat4 projectionMatrix = camera.getProjectionMatrix();

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