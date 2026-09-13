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

    Shader* defaultShader = createShader("basic_texture.vert", "basic_texture.frag");
    StandardEntityGroup* defaultGroup = createEntityGroup(DEFAULT_ENTITY_GROUP_ID, defaultShader);
    BoxBuilder boxBuilder(glm::vec3(1.0f));
    Renderable* boxRenderable = createRenderable(boxBuilder);
    Entity* boxEntity = createEntity(boxRenderable);
    defaultGroup->addEntity(boxEntity);
}

void JoltedEnvironment::frameBufferResizeCallback(int width, int height)
{
    activeCamera->setAspectRatio((float)width / (float)height);
}
