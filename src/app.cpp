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
    unsigned int VAO = vertexManager.createAttributeObject();
    unsigned int VBO = vertexManager.createBufferObject(VAO);

    vertexManager.addAttribute(VAO, 3, GL_FLOAT, GL_FALSE);
    vertexManager.addAttribute(VAO, 3, GL_FLOAT, GL_FALSE);
    
    auto [cubeVertexCount, cubeData] = VertexFactory::getColoredCube();
    
    Shader* cubeShader = shaders.emplace_back(std::make_unique<Shader>("assets/shaders/basic.vert", "assets/shaders/basic.frag")).get();
    Renderable* renderable = vertexManager.createRenderable(VBO, cubeData, cubeVertexCount, GL_STATIC_DRAW, cubeShader);

    vertexManager.loadAttributes(VAO);
    vertexManager.loadVertexData(VBO);

    RenderTarget& renderTarget = renderTargets.emplace_back(VAO, VBO, cubeShader, std::vector<Entity>{});

    for (float x : {-5.0f, 5.0f})
    {
        for (float y : {-5.0f, 5.0f})
        {
            for (float z : {-5.0f, 5.0f})
            {
                glm::vec3 position(x, y, z);
                renderTarget.entities.emplace_back(Entity(renderable, position));
            }
        }
    }
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

        for (auto& renderTarget : renderTargets)
        {
            glBindVertexArray(renderTarget.VAO);
            glBindBuffer(GL_ARRAY_BUFFER, renderTarget.VBO);

            renderTarget.shader->use();
            renderTarget.shader->setMatrix("view", viewMatrix);
            renderTarget.shader->setMatrix("projection", projectionMatrix);

            for (auto& entity : renderTarget.entities) entity.render();
        }

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
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