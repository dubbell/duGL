#include "dugl/env/example_env.h"
#include "dugl/utils/imgui_adapter.h"
#include "dugl/modelling/model_builder.h"
#include "dugl/modelling/primitive_builder.h"
#include "dugl/shading/ubo_templates.h"

#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <format>
#include <algorithm>


ExampleEnvironment::ExampleEnvironment() : clearColor(0.7f, 0.8f, 1.0f, 1.0f), flightController(this, &keyboardController, &mouseController)
{
    initImGui(window);

    // enable depth testing
    glEnable(GL_DEPTH_TEST);

    // enable face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // disable cursor initially
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    directionalLight = {
        0.0f, -60.0f,
        { 0.3f,  0.3f,  0.3f},
        { 0.5f,  0.5f,  0.5f},
        { 1.0f,  1.0f,  1.0f}
    };

    pointLights = {{
        { 0.0f, 0.0f, 0.0f },
        { 0.7f, 0.2f, 0.0f },
        { 0.1f, 0.7f, 0.7f },
        { 1.0f, 1.0f, 1.0f },
        1.0f, 0.09f, 0.032f}};

    // create objects to render
    Renderable* backpack_renderable = createRenderable(ModelBuilder("assets/models/backpack/backpack.obj"));
    
    // create shader programs
    Shader* objectShader = createShader(
        "assets/shaders/basic_texture.vert", "assets/shaders/basic_texture.frag", 
        ExampleShaderType::ObjectShader);
    Shader* outlineShader = createShader(
        "assets/shaders/outline.vert", "assets/shaders/outline.frag", 
        ExampleShaderType::OutlineShader);
    Shader* cubeMapShader = createShader(
        "assets/shaders/skybox.vert", "assets/shaders/skybox.frag", 
        ExampleShaderType::CubeMapShader);

    // create entity groups, one per kind of render pass
    outlinedGroup = addGroup(std::make_unique<OutlinedEntityGroup>(objectShader, outlineShader));
    standardGroup = addGroup(std::make_unique<StandardEntityGroup>(objectShader));

    // create entities
    outlinedGroup->addEntity(createHoverableEntity(backpack_renderable, glm::vec3(1.0f, 1.0f, 6.0f)));
    outlinedGroup->addEntity(createHoverableEntity(backpack_renderable, glm::vec3(-2.0f, 1.0f, 1.0f)));

    createPrimitives();

    // create skybox
    skybox.init("assets/skyboxes/sea", shaders[ExampleShaderType::CubeMapShader].get());

    // create uniform buffer object for perspective transforms
    perspectiveUbo.create("Perspective", { objectShader, cubeMapShader }, sizeof(PerspectiveData), GL_DYNAMIC_DRAW);
}

void ExampleEnvironment::clearBuffers()
{
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void ExampleEnvironment::createPrimitives()
{
    const float rowX = 8.0f;      // in front of the camera's starting position
    const float groundY = -2.0f;  // the plane everything else stands on

    PlaneBuilder groundBuilder(24.0f, 24.0f, 12, 12);
    groundBuilder.setMaterial({
        .diffuseColor = { 0.35f, 0.40f, 0.35f },
        .specularColor = glm::vec3(0.05f),
        .shininess = 4.0f });
    standardGroup->addEntity(createEntity(createRenderable(groundBuilder), { rowX, groundY, 0.0f }));

    // matte, to contrast with the sphere
    BoxBuilder boxBuilder({ 1.5f, 1.5f, 1.5f });
    boxBuilder.setMaterial({
        .diffuseColor = { 0.85f, 0.25f, 0.20f },
        .specularColor = glm::vec3(0.08f),
        .shininess = 8.0f });
    standardGroup->addEntity(createEntity(createRenderable(boxBuilder), { rowX, groundY + 0.75f, -6.0f }));

    // equal radii, so this is a plain sphere; glossy
    EllipsoidBuilder sphereBuilder(glm::vec3(1.0f), 32, 16);
    sphereBuilder.setMaterial({
        .diffuseColor = { 0.20f, 0.45f, 0.85f },
        .specularColor = glm::vec3(0.9f),
        .shininess = 128.0f });
    standardGroup->addEntity(createEntity(createRenderable(sphereBuilder), { rowX, groundY + 1.0f, -3.0f }));

    // unequal radii: the interesting case for the normals
    EllipsoidBuilder ellipsoidBuilder({ 0.6f, 1.5f, 0.6f }, 32, 16);
    ellipsoidBuilder.setMaterial({
        .diffuseColor = { 0.30f, 0.75f, 0.35f },
        .specularColor = glm::vec3(0.5f),
        .shininess = 48.0f });
    standardGroup->addEntity(createEntity(createRenderable(ellipsoidBuilder), { rowX, groundY + 1.5f, 0.0f }));

    // smoothly shaded around its axis
    ConeBuilder coneBuilder(1.0f, 2.5f, 32);
    coneBuilder.setMaterial({
        .diffuseColor = { 0.95f, 0.65f, 0.15f },
        .specularColor = glm::vec3(0.4f),
        .shininess = 32.0f });
    standardGroup->addEntity(createEntity(createRenderable(coneBuilder), { rowX, groundY, 3.0f }));

    // faceted, unlike the cone: its four sides each carry a flat normal
    PyramidBuilder pyramidBuilder(2.0f, 2.0f, 2.0f);
    pyramidBuilder.setMaterial({
        .diffuseColor = { 0.60f, 0.35f, 0.80f },
        .specularColor = glm::vec3(0.25f),
        .shininess = 16.0f });
    standardGroup->addEntity(createEntity(createRenderable(pyramidBuilder), { rowX, groundY, 6.0f }));
}

Renderable* ExampleEnvironment::createRenderable(RenderableBuilder& builder)
{
    return renderables.emplace_back(std::make_unique<Renderable>(builder.build())).get();
}

Entity* ExampleEnvironment::createEntity(Renderable* renderable, glm::vec3 position)
{
    auto entity = std::make_unique<Entity>(renderable, position);
    Entity* ptr = entity.get();
    entities.push_back(std::move(entity));
    return ptr;
}

HoverableEntity* ExampleEnvironment::createHoverableEntity(Renderable* renderable, glm::vec3 position)
{
    auto entity = std::make_unique<HoverableEntity>(renderable, position, &mouseController);
    HoverableEntity* ptr = entity.get();
    entities.push_back(std::move(entity));
    return ptr;
}

Shader* ExampleEnvironment::createShader(const char* vertexShaderPath, const char* fragmentShaderPath, ExampleShaderType shaderType)
{
    return shaders.emplace(shaderType, std::make_unique<Shader>(vertexShaderPath, fragmentShaderPath)).first->second.get();
}

void ExampleEnvironment::start()
{
    Shader* objectShader = shaders[ExampleShaderType::ObjectShader].get();

    while (!glfwWindowShouldClose(window))
    {
        clearBuffers();  // clear color and depth buffer

        // write to perspective UBO, shared among shaders
        PerspectiveData perspectiveData = { activeCamera->getViewMatrix(), activeCamera->getProjectionMatrix() }; 
        perspectiveUbo.writeData(perspectiveData);
        
        glfwPollEvents();  // viewport resizing and GUI interaction

        mouseController.processInput();     // user mouse input
        keyboardController.processInput();  // user keyboard input

        // user interface
        createImGuiFrame();
        
        // for rendering objects
        objectShader->use();

        // camera pos only used by object shader
        objectShader->setVec3("viewPos", activeCamera->getPosition());

        // lighting
        objectShader->setDirectionalLight(directionalLight);
        objectShader->setPointLights(pointLights);
        
        // render all entity groups
        for (const auto& group : entityGroups)
        {
            group->render();
        }

        // draw sky box
        skybox.draw();

        // draw gui
        drawImGui();

        // swap buffers
        glfwSwapBuffers(window);
    }
}

void ExampleEnvironment::createImGuiFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SeparatorText("Directional Light");

    ImGui::SliderFloat("Yaw", &directionalLight.yaw, -180.0f, 180.0f);
    ImGui::SliderFloat("Pitch", &directionalLight.pitch, -89.0f, 89.0f);
    ImGui::SliderFloat3("Ambient", glm::value_ptr(directionalLight.ambient), 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse", glm::value_ptr(directionalLight.diffuse), 0.0f, 1.0f);
    ImGui::SliderFloat3("Specular", glm::value_ptr(directionalLight.specular), 0.0f, 1.0f);


    ImGui::SeparatorText("Entities");

    int entityCount = 1;
    for (auto& entity : entities)
    {
        ImGui::DragFloat3(std::format("Entity {}", entityCount).c_str(), glm::value_ptr(entity->getPosition()), 0.1f);
        entityCount++;
    }
}

void ExampleEnvironment::drawImGui()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ExampleEnvironment::frameBufferResizeCallback(int width, int height)
{
    activeCamera->setAspectRatio((float)width / (float)height);
    viewportWidth = width;
    viewportHeight = height;
}