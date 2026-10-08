#include "dugl/env/example_physics_env.h"
#include "dugl/modelling/primitive_builder.h"
#include "dugl/modelling/outline_pass.h"

#include <Jolt/Physics/Collision/Shape/MeshShape.h>

#include <iostream>

static constexpr float THRUST_FORCE = 20000.0f;
static constexpr float KICK_IMPULSE = 1000.0f;

void ExamplePhysicsEnvironment::init()
{
    // disable cursor initially
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glm::vec3 cameraPosition(-8.0f, 2.0f, 0.0f);
    activeCamera->setPosition(cameraPosition);
    activeCamera->turnUp(-10.0f);

    Shader* defaultShader = createShader("basic_texture.vert", "basic_texture.frag");
    JPH::BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();

    PlaneBuilder planeBuilder(20.0f, 20.0f);
    JPH::TriangleList planeTriangles;
    planeTriangles.push_back(JPH::Triangle(JPH::Float3(-10.0f, 0.0f, -10.0f), JPH::Float3(-10.0f, 0.0f, 10.0f), JPH::Float3(10.0f, 0.0f, 10.0f)));
    planeTriangles.push_back(JPH::Triangle(JPH::Float3(-10.0f, 0.0f, -10.0f), JPH::Float3(10.0f, 0.0f, 10.0f), JPH::Float3(10.0f, 0.0f, -10.0f)));
    JPH::BodyCreationSettings planeSettings(
        new JPH::MeshShapeSettings(planeTriangles),
        JPH::RVec3::sZero(),
        JPH::Quat::sIdentity(),
        JPH::EMotionType::Static,
        Layers::NON_MOVING);
    createEntity<PhysicsEntity>(createRenderable(planeBuilder), defaultShader, bodyInterface, planeSettings);

    BoxBuilder boxBuilder(glm::vec3(1.0f));
    JPH::BodyCreationSettings boxSettings(
        new JPH::BoxShapeSettings(JPH::Vec3(0.5f, 0.5f, 0.5f)),
        JPH::RVec3(0.0f, 5.0f, 0.0f),
        JPH::Quat::sRotation(JPH::Vec3(1.0f, 0.0f, 1.0f).Normalized(), 0.4f),
        JPH::EMotionType::Dynamic,
        Layers::MOVING);
    box = createEntity<PhysicsEntity>(createRenderable(boxBuilder), defaultShader, bodyInterface, boxSettings);
    box->setFlag(EntityFlag::Hoverable, true);

    Shader* outlineMaskShader = createShader("outline_mask.vert", "outline_mask.frag");
    Shader* outlineShader = createShader("outline.vert", "outline.frag");
    createRenderPass<OutlinePass>(outlineMaskShader, outlineShader, flagMask(EntityFlag::Hovered));

    physicsSystem.OptimizeBroadPhase();
}

void ExamplePhysicsEnvironment::update(float dt)
{
    bool kick = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;
    if (kick && !kickHeld) {
        box->applyImpulse(glm::vec3(0.0f, 0.0f, KICK_IMPULSE));
    }
    kickHeld = kick;

    physicsUpdate(dt);

    for (const CollisionEvent& event : getCollisionEvents())
    {
        bool begin = event.phase == CollisionPhase::Begin;
        std::cout << (begin ? "Collision begin: " : "Collision end: ")
                  << (event.first == box ? "box" : "other") << " / "
                  << (event.second == box ? "box" : "other");
        if (begin) {
            std::cout << ", normal (" << event.normal.x << ", " << event.normal.y << ", " << event.normal.z << ")"
                      << ", relative speed " << glm::length(event.relativeVelocity);
        }
        std::cout << std::endl;
    }
}

void ExamplePhysicsEnvironment::prePhysicsStep(float stepDt)
{
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        box->applyForce(glm::vec3(0.0f, THRUST_FORCE, 0.0f));
    }
}
