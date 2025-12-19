#include <app.h>


Application::Application(int width, int height) : clearColor(0.7f, 0.8f, 1.0f, 1.0f)
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
    
    camera.setAspectRatio((float)width / (float)height);

    keyboardController.setWindow(window);

    auto flightController = std::make_shared<FlightController>(window, &camera);

    mouseController.registerObserver(flightController);
    keyboardController.registerObserver(flightController);
    for (int key : flightController->getActiveKeys())
        keyboardController.registerKey(key);

    glfwSetFramebufferSizeCallback(window, Application::frameBufferSizeCallback);
    glfwSetCursorPosCallback(window, Application::cursorPosCallback);
    glfwSetWindowUserPointer(window, this);

    Renderable* backpack_renderable = renderables.emplace_back(std::make_unique<Renderable>("assets/models/backpack/backpack.obj")).get();
    
    entities.emplace_back(std::make_unique<Entity>(backpack_renderable, glm::vec3(1.0f, 1.0f, 6.0f)));
    entities.emplace_back(std::make_unique<Entity>(backpack_renderable, glm::vec3(-2.0f, 1.0f, 1.0f)));
}

void Application::clearBuffers()
{
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Application::stop()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::startMainLoop()
{
    Shader shader = Shader("assets/shaders/basic_texture.vert", "assets/shaders/basic_texture.frag");

    DirectionalLight directionalLight = {
        {-0.2f, -1.0f, -0.3f},
        { 0.0f,  0.0f,  0.0f},
        { 0.5f,  0.5f,  0.5f},
        { 1.0f,  1.0f,  1.0f}
    };

    std::vector<PointLight> pointLights = {{
        { 0.0f, 0.0f, 0.0f },
        { 0.4f, 0.2f, 0.0f },
        { 0.7f, 0.3f, 0.3f },
        { 1.0f, 0.0f, 0.0f },
        1.0f, 0.09f, 0.032f}};

    while (!glfwWindowShouldClose(window))
    {
        clearBuffers();  // clear color and depth buffer
        
        keyboardController.processKeyboardInput();  // user keyboard input

        shader.use();
        shader.setCamera(camera);
        shader.setDirectionalLight(directionalLight);
        shader.setPointLights(pointLights);
        
        for (const auto& entity : entities)
        {
            shader.setMat4("model", entity->getModelTransform());
            entity->draw(shader);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}


void Application::frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    application->camera.setAspectRatio(float(width) / float(height));
}

void Application::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    application->mouseController.cursorPosCallback((float)xpos, (float)ypos);
}