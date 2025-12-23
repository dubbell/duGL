#include "game.h"


Game::Game(int width, int height)
    : Application(width, height), clearColor(0.7f, 0.8f, 1.0f, 1.0f), gameState({ 90.f, false })
{
    // enable depth testing
    glEnable(GL_DEPTH_TEST);

    // enable face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // disable cursor initially
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // set size of view volume
    projectionMatrix = glm::perspective(glm::radians(gameState.fov), (float)width / (float)height, 0.1f, 500.0f);

    // player
    auto flightController = std::make_shared<FlightController>(window, &camera, &gameState);

    // register user input observers
    mouseController.registerObserver(flightController);
    keyboardController.registerObserver(flightController);

    // create objects to render
    Renderable* backpack_renderable = createRenderable("assets/models/backpack/backpack.obj");
    
    // create shader programs
    Shader* objectShader = createShader(
        "assets/shaders/basic_texture.vert", "assets/shaders/basic_texture.frag", ShaderType::ObjectShader);
    Shader* outlineShader = createShader(
        "assets/shaders/outline.vert", "assets/shaders/outline.frag", ShaderType::OutlineShader);
    Shader* cubeMapShader = createShader(
        "assets/shaders/skybox.vert", "assets/shaders/skybox.frag", ShaderType::CubeMapShader);

    // create entities
    createOutlinedEntity(backpack_renderable, glm::vec3(1.0f, 1.0f, 6.0f), outlineShader);
    createOutlinedEntity(backpack_renderable, glm::vec3(-2.0f, 1.0f, 1.0f), outlineShader);

    // create skybox
    skybox.setShader(shaders[ShaderType::CubeMapShader].get());
    skybox.loadSkybox("assets/skyboxes/sea");

    // create uniform buffer object for perspective transforms
    uboPerspective.create("Perspective", { objectShader, cubeMapShader }, sizeof(PerspectiveData), GL_DYNAMIC_DRAW);
}

void Game::clearBuffers()
{
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

Renderable* Game::createRenderable(const char* renderablePath)
{
    return renderables.emplace_back(std::make_unique<Renderable>(ModelBuilder(renderablePath).build())).get();
}

Entity* Game::createEntity(Renderable* renderable, glm::vec3 position)
{
    return entities.emplace_back(std::make_unique<Entity>(renderable, glm::vec3(1.0f, 1.0f, 6.0f))).get();
}

Entity* Game::createOutlinedEntity(Renderable* renderable, glm::vec3 position, Shader* outlineShader)
{
    return entities.emplace_back(std::make_unique<OutlinedEntity>(renderable, position, outlineShader)).get();
}

Shader* Game::createShader(const char* vertexShaderPath, const char* fragmentShaderPath, ShaderType shaderType)
{
    return shaders.emplace(shaderType, std::make_unique<Shader>(vertexShaderPath, fragmentShaderPath)).first->second.get();
}

glm::vec3 Game::castRay(float screenX, float screenY, glm::mat4 viewMatrix)
{
    glm::vec4 viewport(0.0f, 0.0f, (float)applicationState.screenWidth, (float)applicationState.screenHeight);

    screenY = applicationState.screenHeight - screenY;
    
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

void Game::startMainLoop()
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
            entity->render(objectShader);
        }

        // draw sky box
        skybox.draw();

        glfwSwapBuffers(window);
    }
}


void Game::frameBufferResizeCallback(int width, int height)
{
    projectionMatrix = glm::perspective(glm::radians(gameState.fov), (float)width / (float)height, 0.1f, 500.0f);
}