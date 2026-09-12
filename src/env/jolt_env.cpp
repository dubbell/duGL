#include "dugl/env/jolt_env.h"
#include "dugl/utils/jolt_adapter.h"
#include "dugl/utils/time.h"
#include "dugl/modelling/primitive_builder.h"

#include <chrono>
#include <memory>

static constexpr uint32_t MAX_BODIES              = 1024;
static constexpr uint32_t MAX_BODY_PAIRS          = 1024;
static constexpr uint32_t MAX_CONTACT_CONSTRAINTS = 1024;
static constexpr uint32_t MAX_JOBS                = 1024;
static constexpr uint32_t MAX_BARRIERS            = 8;
static constexpr uint32_t TEMP_ALLOCATOR_SIZE     = 10 * 1024 * 1024;  // 10 MB

static constexpr dugl::uint DEFAULT_ENTITY_GROUP_ID = 0;

static std::unique_ptr<Shader> createShader(const char* vertexPath, const char* fragmentPath)
{
    return std::make_unique<Shader>(
        ("assets/shaders/" + std::string(vertexPath)).c_str(),
        ("assets/shaders/" + std::string(fragmentPath)).c_str());
}

static std::unique_ptr<Renderable> createRenderable(RenderableBuilder& builder)
{
    return std::make_unique<Renderable>(builder.build());
}

static std::unique_ptr<Entity> createEntity(Renderable* renderable)
{
    return std::make_unique<Entity>(renderable);
}

static std::unique_ptr<StandardEntityGroup> createStandardEntityGroup(Shader* shader)
{
    return std::make_unique<StandardEntityGroup>(shader);
}

JoltedEnvironment::JoltedEnvironment()
    : tempAllocator(TEMP_ALLOCATOR_SIZE), 
      dispatcher(MAX_JOBS, MAX_BARRIERS)
{
    initJolt();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    physicsSystem.Init(
        MAX_BODIES,
        0,  // numBodyMutexes: 0 = auto
        MAX_BODY_PAIRS,
        MAX_CONTACT_CONSTRAINTS,
        broadPhaseMapper,
        objectBroadPhaseFilter,
        objectCollisionFilter
    );

    Shader* defaultShader = scene.addShader(createShader("basic_texture.vert", "basic_texture.frag"));
    StandardEntityGroup* defaultGroup = scene.addGroup(DEFAULT_ENTITY_GROUP_ID, createStandardEntityGroup(defaultShader));
    BoxBuilder boxBuilder(glm::vec3(1.0f));
    Renderable* boxRenderable = scene.addRenderable(createRenderable(boxBuilder));
    Entity* boxEntity = scene.addEntity(createEntity(boxRenderable));
    defaultGroup->addEntity(boxEntity);
}

void JoltedEnvironment::start()
{
    Stopwatch stopwatch;

    while (!glfwWindowShouldClose(window))
    {
        float dt = stopwatch.tick();        // time since last frame

        // Process inputs.
        glfwPollEvents();                   // viewport resizing and GUI interaction
        mouseController.processInput();     // user mouse input
        keyboardController.processInput();  // user keyboard input

        // Simulate physics.
        physicsSystem.Update(dt, 1, &tempAllocator, &dispatcher.getJobSystem());

        update(dt);
        
        // Start frame render.
        render();

        glfwSwapBuffers(window);
    }
}

void JoltedEnvironment::frameBufferResizeCallback(int width, int height)
{
    activeCamera->setAspectRatio((float)width / (float)height);
}
