#include "dugl/env/jolt_env.h"
#include "dugl/utils/jolt_adapter.h"
#include "dugl/utils/time.h"

#include <chrono>


static constexpr uint32_t MAX_BODIES              = 1024;
static constexpr uint32_t MAX_BODY_PAIRS          = 1024;
static constexpr uint32_t MAX_CONTACT_CONSTRAINTS = 1024;
static constexpr uint32_t MAX_JOBS                = 1024;
static constexpr uint32_t MAX_BARRIERS            = 8;
static constexpr uint32_t TEMP_ALLOCATOR_SIZE     = 10 * 1024 * 1024;  // 10 MB


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
        physicsSystem.Update(dt, 1, &tempAllocator, &dispatcher);

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
