#include <app.h>


Application::Application(int width, int height)
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
    
    camera.setAspectRatio(16.0f / 9.0f);

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
    // shader for ground and cubes
    Shader* shader = shaders.emplace_back(std::make_unique<Shader>(
        "assets/shaders/basic_texture.vert", 
        "assets/shaders/basic_texture.frag")).get();

    // textures for ground and cubes
    std::string containerTexturePath = "assets/textures/container.jpg";
    std::string wallTexturePath = "assets/textures/wall.jpg";
    
    // create cube renderable
    // std::vector<std::vector<float>> cubeColors = {{ 0.5f, 0.5f, 0.5f }};
    Renderable* cubeRenderable = renderables.emplace_back(RenderableFactory::cubeBuilder(&vertexManager)
        ->addTexture(containerTexturePath)
        ->setShader(shader)
        ->build()).get();

    unsigned int VAO = cubeRenderable->getVAO(), VBO = cubeRenderable->getVBO();

    // create ground renderable
    Renderable* groundRenderable = renderables.emplace_back(RenderableFactory::groundBuilder(&vertexManager)
        ->addTexture(wallTexturePath)
        ->setShader(shader)
        ->setVAO(VAO)
        ->setVBO(VBO)
        ->build()).get();

    // load attributes and vertices into GPU
    vertexManager.loadAttributes(VAO);
    vertexManager.loadVertexData(VBO);

    glm::vec3 groundPosition(0.0f, -1.0f, 0.0f);
    entityMap[VAO][VBO].emplace_back(Entity(groundRenderable, groundPosition));
    
    for (float x : {-5.0f, 5.0f})
    {
        for (float y : {0.0f, 5.0f})
        {
            for (float z : {-5.0f, 5.0f})
            {
                glm::vec3 cubePosition(x, y, z);
                entityMap[VAO][VBO].emplace_back(Entity(cubeRenderable, cubePosition));
            }
        }
    }
}


void Application::startMainLoop()
{
    std::vector<unsigned int> activeTextures(32);

    while (!glfwWindowShouldClose(window))
    {
        // process input
        keyboardController.processKeyboardInput();

        // clear buffers
        glClearColor(0.7f, 0.8f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // perspective transformations
        glm::mat4 viewMatrix = camera.getViewMatrix();
        glm::mat4 projectionMatrix = camera.getProjectionMatrix();

        unsigned int prevVAO = 0, prevVBO = 0;
        Shader* prevShader = nullptr;

        // loop through every VAO group
        for (auto& [VAO, VBOmap] : entityMap)
        {
            if (VAO != prevVAO) 
            {
                glBindVertexArray(VAO);
                prevVAO = VAO;
            }

            // loop through every VBO group
            for (auto& [VBO, entities] : VBOmap)
            {
                if (VBO != prevVBO) 
                {
                    glBindBuffer(GL_ARRAY_BUFFER, VBO);
                    prevVBO = VBO;
                }

                // loop through all entities in group
                for (auto entity : entities)
                {
                    // set shader
                    Shader* shader = entity.getShader();
                    if (shader != prevShader)
                    {
                        shader->use();
                        prevShader = shader;
                    }

                    // set perspective transforms
                    shader->setMatrix("view", viewMatrix);
                    shader->setMatrix("projection", projectionMatrix);

                    // bind textures
                    auto textures = entity.getTextures();
                    for (size_t t_i = 0; t_i < textures.size(); t_i++)
                    {
                        if (textures[t_i] != activeTextures[t_i])
                        {
                            activeTextures[t_i] = textures[t_i];
                            glActiveTexture(GL_TEXTURE0 + static_cast<GLint>(t_i));
                            glBindTexture(GL_TEXTURE_2D, textures.at(t_i));
                        }
                    }

                    // draw call
                    entity.render();
                }
            }
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
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    application->camera.setAspectRatio(float(width) / float(height));
}

void Application::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    application->mouseController.cursorPosCallback((float)xpos, (float)ypos);
}