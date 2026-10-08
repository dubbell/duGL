#include "dugl/env/physics_env.h"
#include "dugl/utils/jolt_adapter.h"

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
    physicsSystem.SetContactListener(&contactRecorder);
}

PhysicsEnvironment::~PhysicsEnvironment()
{
    scene.clearEntities();
}

void PhysicsEnvironment::physicsUpdate(float dt)
{
    collisionEvents.clear();

    accumulator = std::min(accumulator + dt, MAX_ACCUMULATED_TIME);
    while (accumulator >= PHYSICS_STEP)
    {
        prePhysicsStep(PHYSICS_STEP);
        physicsSystem.Update(PHYSICS_STEP, 1, &tempAllocator, dispatcher.getJobSystem());
        collectCollisionEvents();
        accumulator -= PHYSICS_STEP;
    }
}

static bool isAsleep(const JPH::BodyInterface& bodyInterface, JPH::BodyID body1, JPH::BodyID body2)
{
    return bodyInterface.IsAdded(body1) && bodyInterface.IsAdded(body2)
        && !bodyInterface.IsActive(body1) && !bodyInterface.IsActive(body2);
}

void PhysicsEnvironment::collectCollisionEvents()
{
    const JPH::BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();

    for (const RecordedContact& contact : contactRecorder.drain())
    {
        std::pair bodies(contact.body1, contact.body2);
        if (contact.phase == CollisionPhase::Begin)
        {
            if (sleepingContacts.erase(bodies) == 0) {
                addCollisionEvent(contact);
            }
        }
        else if (isAsleep(bodyInterface, contact.body1, contact.body2)) {
            sleepingContacts[bodies] = false;
        }
        else {
            addCollisionEvent(contact);
        }
    }

    // A sleeping contact ends once its bodies have been awake for a whole step without touching again.
    for (auto it = sleepingContacts.begin(); it != sleepingContacts.end();)
    {
        bool awake = !isAsleep(bodyInterface, it->first.first, it->first.second);
        if (awake && it->second)
        {
            addCollisionEvent({ CollisionPhase::End, it->first.first, it->first.second });
            it = sleepingContacts.erase(it);
        }
        else
        {
            it->second = awake;
            ++it;
        }
    }
}

void PhysicsEnvironment::addCollisionEvent(const RecordedContact& contact)
{
    const JPH::BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();

    PhysicsEntity* first = reinterpret_cast<PhysicsEntity*>(bodyInterface.GetUserData(contact.body1));
    PhysicsEntity* second = reinterpret_cast<PhysicsEntity*>(bodyInterface.GetUserData(contact.body2));
    if (first == nullptr || second == nullptr) {
        return;
    }

    const CollisionEvent& event = collisionEvents.emplace_back(CollisionEvent{
        contact.phase,
        first,
        second,
        toGlm(contact.point),
        toGlm(contact.normal),
        contact.penetrationDepth,
        toGlm(contact.relativeVelocity) });

    first->pendingCollisions.push_back(event);
    second->pendingCollisions.push_back(event);
}
