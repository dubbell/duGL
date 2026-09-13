#include "dugl/env/physics_env.h"

#include <algorithm>

static constexpr uint32_t MAX_BODIES              = 1024;
static constexpr uint32_t MAX_BODY_PAIRS          = 1024;
static constexpr uint32_t MAX_CONTACT_CONSTRAINTS = 1024;
static constexpr uint32_t TEMP_ALLOCATOR_SIZE     = 10 * 1024 * 1024;  // 10 MB

static constexpr float PHYSICS_STEP         = 1.0f / 60.0f;
static constexpr float MAX_ACCUMULATED_TIME = 0.25f;

PhysicsEnvironment::PhysicsEnvironment()
    : tempAllocator(TEMP_ALLOCATOR_SIZE)
{
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

PhysicsEnvironment::~PhysicsEnvironment()
{
    scene.clearEntities();
}

void PhysicsEnvironment::update(float dt)
{
    accumulator = std::min(accumulator + dt, MAX_ACCUMULATED_TIME);
    while (accumulator >= PHYSICS_STEP)
    {
        physicsSystem.Update(PHYSICS_STEP, 1, &tempAllocator, dispatcher.getJobSystem());
        accumulator -= PHYSICS_STEP;
    }
}

void PhysicsEnvironment::frameBufferResizeCallback(int width, int height)
{
    activeCamera->setAspectRatio((float)width / (float)height);
}
