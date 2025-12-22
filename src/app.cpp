#include <app.h>


Application::Application(int width, int height) 
    : clearColor(0.7f, 0.8f, 1.0f, 1.0f), screenWidth(width), screenHeight(height)
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

    // enable depth testing
    glEnable(GL_DEPTH_TEST);

    // enable face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // disable cursor initially
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // set size of view volume
    fov = 90.0f;
    aspectRatio = (float)width / (float)height;
    projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 500.0f);

    // necessary keyboard controller initialization step
    keyboardController.setWindow(window);

    // player
    auto flightController = std::make_shared<FlightController>(window, &camera);

    // register user input observers
    mouseController.registerObserver(flightController);
    keyboardController.registerObserver(flightController);
    
    glfwSetFramebufferSizeCallback(window, Application::frameBufferSizeCallback);
    glfwSetCursorPosCallback(window, Application::cursorPosCallback);
    glfwSetWindowUserPointer(window, this);

    // create objects to render
    Renderable* backpack_renderable = renderables.emplace_back
        (std::make_unique<Renderable>(ModelBuilder("assets/models/backpack/backpack.obj").build())).get();
    
    entities.emplace_back(std::make_unique<Entity>(backpack_renderable, glm::vec3(1.0f, 1.0f, 6.0f)));
    // entities.emplace_back(std::make_unique<Entity>(backpack_renderable, glm::vec3(-2.0f, 1.0f, 1.0f)));

    // shader for object renderables
    Shader* objectShader = shaders.emplace(ShaderType::ObjectShader, 
        std::make_unique<Shader>("assets/shaders/basic_texture.vert", "assets/shaders/basic_texture.frag")).first->second.get();
    
    backpack_renderable->setDefaultShader(objectShader);

    // shader for cube map skybox
    Shader* cubeMapShader = shaders.emplace(ShaderType::CubeMapShader,
        std::make_unique<Shader>("assets/shaders/skybox.vert", "assets/shaders/skybox.frag")).first->second.get();
    
    // create skybox
    skybox.setShader(shaders[ShaderType::CubeMapShader].get());
    skybox.loadSkybox("assets/skyboxes/sea");

    // create uniform buffer object
    uboPerspective.create("Perspective", { objectShader, cubeMapShader }, sizeof(PerspectiveData), GL_DYNAMIC_DRAW);
}

void Application::clearBuffers()
{
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

glm::vec3 Application::castRay(float screenX, float screenY, glm::mat4 viewMatrix)
{
    glm::vec4 viewport(0.0f, 0.0f, (float)screenWidth, (float)screenHeight);

    screenY = screenHeight - screenY;
    
    glm::vec3 nearPlanePoint = glm::unProject(
        glm::vec3(screenX, screenY, 0.0f),
        viewMatrix,
        projectionMatrix,
        viewport);
    
    glm::vec3 farPlanePoint = glm::unProject(
        glm::vec3(screenX, screenY, 1.0f),
        viewMatrix,
        projectionMatrix,
        viewport);

    glm::vec3 direction =  glm::normalize(farPlanePoint - nearPlanePoint);

    return direction;
}

bool checkRayIntersection(glm::vec3 target, float distance, glm::vec3 origin, glm::vec3 direction)
{
    glm::vec3 toTarget = target - origin;

    float t = glm::dot(toTarget, direction);

    // ray is cast in wrong direction
    if (t < 0.0f) return false;

    glm::vec3 closestPoint = origin + t * direction;

    return glm::length2(closestPoint - target) <= (distance * distance);
}

void Application::startMainLoop()
{
    Shader* objectShader = shaders[ShaderType::ObjectShader].get();

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
        
        glfwPollEvents();  // user cursor input
        keyboardController.processKeyboardInput();  // user keyboard input

        // write to perspective UBO, shared among shaders
        PerspectiveData perspectiveData = { camera.getViewMatrix(), projectionMatrix };
        uboPerspective.writeData(perspectiveData);
        
        // for rendering objects
        objectShader->use();

        // camera pos only used by object shader
        objectShader->setVec3("viewPos", camera.getPosition());

        // lighting
        objectShader->setDirectionalLight(directionalLight);
        objectShader->setPointLights(pointLights);
        
        // render all entities
        for (const auto& entity : entities)
        {
            objectShader->setMat4("model", entity->getModelTransform());
            entity->render();
        }

        // draw sky box
        skybox.draw();

        glfwSwapBuffers(window);
    }
}

void Application::stop()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}


void Application::frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    application->aspectRatio = (float)width / (float)height;
}

void Application::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    application->mouseController.cursorPosCallback((float)xpos, (float)ypos);
}