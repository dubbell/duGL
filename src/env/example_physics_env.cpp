#include "dugl/env/example_physics_env.h"
#include "dugl/modelling/primitive_builder.h"
#include "dugl/modelling/outline_pass.h"
#include "dugl/physics/draggable_entity.h"

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

    auto makeInteractive = [](Entity* entity) {
        entity->setFeature(EntityFeature::Hoverable, true);
        entity->setFeature(EntityFeature::Selectable, true);
        entity->setFeature(EntityFeature::Holdable, true);
    };

    PlaneBuilder planeBuilder(20.0f, 20.0f);
    createEntity<PhysicsEntity>(planeBuilder, defaultShader, { .motionType = MotionType::Static });

    PhysicsMaterial dynamicMaterial;
    glm::quat boxRotation = glm::angleAxis(0.4f, glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));

    BoxBuilder boxBuilder(glm::vec3(1.0f));
    box = createEntity<DraggableEntity>(boxBuilder, defaultShader, dynamicMaterial);
    box->setPosition(glm::vec3(0.0f, 5.0f, 0.0f));
    box->setRotation(boxRotation);
    makeInteractive(box);
    box->setFrozen(true);

    DraggableEntity* secondBox = createEntity<DraggableEntity>(boxBuilder, defaultShader, dynamicMaterial);
    secondBox->setPosition(glm::vec3(0.0f, 9.0f, 0.0f));
    secondBox->setRotation(boxRotation);
    makeInteractive(secondBox);

    EllipsoidBuilder sphereBuilder(glm::vec3(0.75f));
    sphereBuilder.setMaterial({ .diffuseColor = { 0.20f, 0.45f, 0.85f } });
    DraggableEntity* sphere = createEntity<DraggableEntity>(sphereBuilder, defaultShader, dynamicMaterial);
    sphere->setPosition(glm::vec3(0.0f, 3.0f, -6.0f));
    makeInteractive(sphere);

    EllipsoidBuilder ellipsoidBuilder(glm::vec3(0.5f, 1.0f, 0.5f));
    ellipsoidBuilder.setMaterial({ .diffuseColor = { 0.30f, 0.75f, 0.35f } });
    DraggableEntity* ellipsoid = createEntity<DraggableEntity>(ellipsoidBuilder, defaultShader, dynamicMaterial);
    ellipsoid->setPosition(glm::vec3(0.0f, 3.0f, -3.0f));
    makeInteractive(ellipsoid);

    ConeBuilder coneBuilder(0.75f, 1.5f);
    coneBuilder.setMaterial({ .diffuseColor = { 0.95f, 0.65f, 0.15f } });
    DraggableEntity* cone = createEntity<DraggableEntity>(coneBuilder, defaultShader, dynamicMaterial);
    cone->setPosition(glm::vec3(0.0f, 3.0f, 3.0f));
    makeInteractive(cone);

    PyramidBuilder pyramidBuilder(1.5f, 1.5f, 1.25f);
    pyramidBuilder.setMaterial({ .diffuseColor = { 0.60f, 0.35f, 0.80f } });
    DraggableEntity* pyramid = createEntity<DraggableEntity>(pyramidBuilder, defaultShader, dynamicMaterial);
    pyramid->setPosition(glm::vec3(0.0f, 3.0f, 6.0f));
    makeInteractive(pyramid);

    Shader* outlineMaskShader = createShader("outline_mask.vert", "outline_mask.frag");
    Shader* outlineShader = createShader("outline.vert", "outline.frag");
    createRenderPass<OutlinePass>(outlineMaskShader, outlineShader, flagMask(EntityFlag::Hovered, EntityFlag::Selected));
    OutlinePass* heldPass = createRenderPass<OutlinePass>(outlineMaskShader, outlineShader, flagMask(EntityFlag::Held));
    heldPass->setOutlineColor(glm::vec3(0.2f, 0.9f, 0.3f));

    physicsSystem.OptimizeBroadPhase();
}

void ExamplePhysicsEnvironment::update(float dt)
{
    bool kick = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;
    if (kick && !kickHeld) {
        box->applyImpulse(glm::vec3(0.0f, 0.0f, KICK_IMPULSE));
    }
    kickHeld = kick;

    bool freeze = glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS;
    if (freeze && !freezeHeld) {
        box->setFrozen(!box->isFrozen());
    }
    freezeHeld = freeze;

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
