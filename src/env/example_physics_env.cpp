#include "dugl/env/example_physics_env.h"
#include "dugl/modelling/primitive_builder.h"

#include <Jolt/Physics/Collision/Shape/PlaneShape.h>

static constexpr dugl::uint DEFAULT_ENTITY_GROUP_ID = 0;

void ExamplePhysicsEnvironment::init()
{
    // disable cursor initially
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glm::vec3 cameraPosition(-8.0f, 2.0f, 0.0f);
    activeCamera->setPosition(cameraPosition);
    activeCamera->turnUp(-10.0f);

    Shader* defaultShader = createShader("basic_texture.vert", "basic_texture.frag");
    StandardEntityGroup* defaultGroup = createEntityGroup(DEFAULT_ENTITY_GROUP_ID, defaultShader);
    JPH::BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();

    PlaneBuilder planeBuilder(20.0f, 20.0f);
    JPH::BodyCreationSettings planeSettings(
        new JPH::PlaneShapeSettings(JPH::Plane(JPH::Vec3::sAxisY(), 0.0f), nullptr, 10.0f),
        JPH::RVec3::sZero(),
        JPH::Quat::sIdentity(),
        JPH::EMotionType::Static,
        Layers::NON_MOVING);
    defaultGroup->addEntity(createEntity<PhysicsEntity>(createRenderable(planeBuilder), bodyInterface, planeSettings));

    BoxBuilder boxBuilder(glm::vec3(1.0f));
    JPH::BodyCreationSettings boxSettings(
        new JPH::BoxShapeSettings(JPH::Vec3(0.5f, 0.5f, 0.5f)),
        JPH::RVec3(0.0f, 5.0f, 0.0f),
        JPH::Quat::sRotation(JPH::Vec3(1.0f, 0.0f, 1.0f).Normalized(), 0.4f),
        JPH::EMotionType::Dynamic,
        Layers::MOVING);
    defaultGroup->addEntity(createEntity<PhysicsEntity>(createRenderable(boxBuilder), bodyInterface, boxSettings));

    physicsSystem.OptimizeBroadPhase();
}
