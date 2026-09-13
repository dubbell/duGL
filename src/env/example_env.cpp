#include "dugl/env/example_env.h"
#include "dugl/utils/imgui_adapter.h"
#include "dugl/modelling/model_builder.h"
#include "dugl/modelling/primitive_builder.h"
#include "dugl/shading/ubo_templates.h"

#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <format>
#include <algorithm>


static constexpr dugl::uint DEFAULT_ENTITY_GROUP_ID = 0;
static constexpr dugl::uint OUTLINE_ENTITY_GROUP_ID = 1;


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

    scene.setDirectionalLight({
        0.0f, -60.0f,
        { 0.3f,  0.3f,  0.3f},
        { 0.5f,  0.5f,  0.5f},
        { 1.0f,  1.0f,  1.0f}});

    // create objects to render
    ModelBuilder backpackBuilder("assets/models/backpack/backpack.obj");
    Renderable* backpackRenderable = createRenderable(backpackBuilder);
    
    // create shader programs
    Shader* objectShader = createShader("basic_texture.vert", "basic_texture.frag");
    Shader* outlineShader = createShader("outline.vert", "outline.frag");
    Shader* cubeMapShader = createShader("skybox.vert", "skybox.frag");

    // create entity groups
    StandardEntityGroup* standardGroup = createEntityGroup(DEFAULT_ENTITY_GROUP_ID, objectShader);
    OutlinedEntityGroup* outlinedGroup = createEntityGroup<OutlinedEntityGroup>(OUTLINE_ENTITY_GROUP_ID, objectShader, outlineShader);

    // create entities
    HoverableEntity* backpack1 = createEntity<HoverableEntity>(backpackRenderable, &mouseController);
    backpack1->setPosition(glm::vec3(1.0f, 1.0f, 6.0f));
    HoverableEntity* backpack2 = createEntity<HoverableEntity>(backpackRenderable, &mouseController);
    backpack2->setPosition(glm::vec3(-2.0f, 1.0f, 1.0f));

    outlinedGroup->addEntity(backpack1);
    outlinedGroup->addEntity(backpack2);

    createPrimitives(standardGroup);

    // create skybox
    scene.setSkybox("assets/skyboxes/sea", cubeMapShader);
}

void ExampleEnvironment::createPrimitives(StandardEntityGroup* entityGroup)
{
    const float rowX = 8.0f;      // in front of the camera's starting position
    const float groundY = -2.0f;  // the plane everything else stands on

    PlaneBuilder groundBuilder(24.0f, 24.0f, 12, 12);
    groundBuilder.setMaterial({
        .diffuseColor = { 0.35f, 0.40f, 0.35f },
        .specularColor = glm::vec3(0.05f),
        .shininess = 4.0f });
    Entity* ground = createEntity(createRenderable(groundBuilder));
    ground->setPosition({ rowX, groundY, 0.0f });
    entityGroup->addEntity(ground);

    // matte, to contrast with the sphere
    BoxBuilder boxBuilder({ 1.5f, 1.5f, 1.5f });
    boxBuilder.setMaterial({
        .diffuseColor = { 0.85f, 0.25f, 0.20f },
        .specularColor = glm::vec3(0.08f),
        .shininess = 8.0f });
    Entity* box = createEntity(createRenderable(boxBuilder));
    box->setPosition({ rowX, groundY + 0.75f, -6.0f });
    entityGroup->addEntity(box);

    // equal radii, so this is a plain sphere; glossy
    EllipsoidBuilder sphereBuilder(glm::vec3(1.0f), 32, 16);
    sphereBuilder.setMaterial({
        .diffuseColor = { 0.20f, 0.45f, 0.85f },
        .specularColor = glm::vec3(0.9f),
        .shininess = 128.0f });
    Entity* sphere = createEntity(createRenderable(sphereBuilder));
    sphere->setPosition({ rowX, groundY + 1.0f, -3.0f });
    entityGroup->addEntity(sphere);

    // unequal radii: the interesting case for the normals
    EllipsoidBuilder ellipsoidBuilder({ 0.6f, 1.5f, 0.6f }, 32, 16);
    ellipsoidBuilder.setMaterial({
        .diffuseColor = { 0.30f, 0.75f, 0.35f },
        .specularColor = glm::vec3(0.5f),
        .shininess = 48.0f });
    Entity* ellipsoid = createEntity(createRenderable(ellipsoidBuilder));
    ellipsoid->setPosition({ rowX, groundY + 1.5f, 0.0f });
    entityGroup->addEntity(ellipsoid);

    // smoothly shaded around its axis
    ConeBuilder coneBuilder(1.0f, 2.5f, 32);
    coneBuilder.setMaterial({
        .diffuseColor = { 0.95f, 0.65f, 0.15f },
        .specularColor = glm::vec3(0.4f),
        .shininess = 32.0f });
    Entity* cone = createEntity(createRenderable(coneBuilder));
    cone->setPosition({ rowX, groundY, 3.0f });
    entityGroup->addEntity(cone);

    // faceted, unlike the cone: its four sides each carry a flat normal
    PyramidBuilder pyramidBuilder(2.0f, 2.0f, 2.0f);
    pyramidBuilder.setMaterial({
        .diffuseColor = { 0.60f, 0.35f, 0.80f },
        .specularColor = glm::vec3(0.25f),
        .shininess = 16.0f });
    Entity* pyramid = createEntity(createRenderable(pyramidBuilder));
    pyramid->setPosition({ rowX, groundY, 6.0f });
    entityGroup->addEntity(pyramid);
}

void ExampleEnvironment::update(float dt)
{
    createImGuiFrame();
}

void ExampleEnvironment::postRender()
{
    drawImGui();
}

void ExampleEnvironment::createImGuiFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SeparatorText("Directional Light");

    ImGui::SliderFloat("Yaw", &scene.getDirectionalLight().yaw, -180.0f, 180.0f);
    ImGui::SliderFloat("Pitch", &scene.getDirectionalLight().pitch, -89.0f, 89.0f);
    ImGui::SliderFloat3("Ambient", glm::value_ptr(scene.getDirectionalLight().ambient), 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse", glm::value_ptr(scene.getDirectionalLight().diffuse), 0.0f, 1.0f);
    ImGui::SliderFloat3("Specular", glm::value_ptr(scene.getDirectionalLight().specular), 0.0f, 1.0f);

    ImGui::SeparatorText("Entities");

    int entityCount = 1;
    for (auto& entity : scene.getEntities())
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