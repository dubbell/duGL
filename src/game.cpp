#include "game.h"


ExampleGame::ExampleGame()
    : clearColor(0.7f, 0.8f, 1.0f, 1.0f), flightController(this)
{
    // enable depth testing
    glEnable(GL_DEPTH_TEST);

    // enable face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // disable cursor initially
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // register user input observers
    mouseController.registerOffsetObserver(&flightController);
    keyboardController.registerObserver(&flightController);

    // create objects to render
    Renderable* backpack_renderable = createRenderable("assets/models/backpack/backpack.obj");
    
    // create shader programs
    Shader* objectShader = createShader(
        "assets/shaders/basic_texture.vert", "assets/shaders/basic_texture.frag", 
        ShaderType::ObjectShader);
    Shader* outlineShader = createShader(
        "assets/shaders/outline.vert", "assets/shaders/outline.frag", 
        ShaderType::OutlineShader);
    Shader* cubeMapShader = createShader(
        "assets/shaders/skybox.vert", "assets/shaders/skybox.frag", 
        ShaderType::CubeMapShader);

    // create entities
    OutlinedEntity* entity1 = createOutlinedEntity(backpack_renderable, glm::vec3(1.0f, 1.0f, 6.0f), outlineShader);
    OutlinedEntity* entity2 = createOutlinedEntity(backpack_renderable, glm::vec3(-2.0f, 1.0f, 1.0f), outlineShader);

    mouseController.registerScreenRayObserver(entity1);
    mouseController.registerScreenRayObserver(entity2);

    // create skybox
    skybox.setShader(shaders[ShaderType::CubeMapShader].get());
    skybox.loadSkybox("assets/skyboxes/sea");

    // create uniform buffer object for perspective transforms
    uboPerspective.create("Perspective", { objectShader, cubeMapShader }, sizeof(PerspectiveData), GL_DYNAMIC_DRAW);
}

void ExampleGame::clearBuffers()
{
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

Renderable* ExampleGame::createRenderable(const char* renderablePath)
{
    return renderables.emplace_back(std::make_unique<Renderable>(ModelBuilder(renderablePath).build())).get();
}

Entity* ExampleGame::createEntity(Renderable* renderable, glm::vec3 position)
{
    return entities.emplace_back(std::make_unique<Entity>(renderable, glm::vec3(1.0f, 1.0f, 6.0f))).get();
}

OutlinedEntity* ExampleGame::createOutlinedEntity(Renderable* renderable, glm::vec3 position, Shader* outlineShader)
{
    auto outlinedEntity = std::make_unique<OutlinedEntity>(renderable, position, outlineShader);
    OutlinedEntity* ptr = outlinedEntity.get();
    entities.push_back(std::move(outlinedEntity));
    return ptr;
}

Shader* ExampleGame::createShader(const char* vertexShaderPath, const char* fragmentShaderPath, ShaderType shaderType)
{
    return shaders.emplace(shaderType, std::make_unique<Shader>(vertexShaderPath, fragmentShaderPath)).first->second.get();
}

void ExampleGame::startMainLoop()
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

        // write to perspective UBO, shared among shaders
        PerspectiveData perspectiveData = { activeCamera->getViewMatrix(), activeCamera->getProjectionMatrix() }; 
        uboPerspective.writeData(perspectiveData);
        
        glfwPollEvents();  // user cursor input
        keyboardController.processKeyboardInput();  // user keyboard input
        
        // for rendering objects
        objectShader->use();

        // camera pos only used by object shader
        objectShader->setVec3("viewPos", activeCamera->getPosition());

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


void ExampleGame::frameBufferResizeCallback(int width, int height)
{
    activeCamera->setAspectRatio((float)width / (float)height);
    viewportWidth = width;
    viewportHeight = height;
}