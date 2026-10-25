#include "dugl/example/height_field_env.h"
#include "dugl/modelling/primitive_builder.h"
#include "dugl/modelling/outline_pass.h"
#include "dugl/modelling/terrain_noise.h"
#include "dugl/example/draggable_entity.h"

#include <vector>

static constexpr dugl::uint TERRAIN_ROWS = 48;
static constexpr dugl::uint TERRAIN_COLUMNS = 64;
static constexpr float TERRAIN_WIDTH = 100.0f;
static constexpr float TERRAIN_LENGTH = 100.0f;
static constexpr float BOWL_DEPTH = 4.0f;
static constexpr int TERRAIN_SEED = 1337;
static constexpr float NOISE_FREQUENCY = 0.15f;
static constexpr float NOISE_AMPLITUDE = 0.8f;

static const glm::vec3 SPHERE_START(-13.0f, BOWL_DEPTH + NOISE_AMPLITUDE + 1.0f, 0.0f);

static std::vector<std::vector<float>> createTerrainHeights()
{
    std::vector<std::vector<float>> heights(TERRAIN_ROWS, std::vector<float>(TERRAIN_COLUMNS));
    std::vector<std::vector<float>> noise = generateTerrainNoise(TERRAIN_ROWS, TERRAIN_COLUMNS, TERRAIN_WIDTH, TERRAIN_LENGTH, {
        .seed = TERRAIN_SEED,
        .frequency = NOISE_FREQUENCY,
        .amplitude = NOISE_AMPLITUDE });

    for (dugl::uint row = 0; row < TERRAIN_ROWS; row++)
    {
        for (dugl::uint column = 0; column < TERRAIN_COLUMNS; column++)
        {
            float x = TERRAIN_WIDTH * ((float)column / (TERRAIN_COLUMNS - 1) - 0.5f);
            float z = TERRAIN_LENGTH * ((float)row / (TERRAIN_ROWS - 1) - 0.5f);

            float u = 2.0f * x / TERRAIN_WIDTH;
            float v = 2.0f * z / TERRAIN_LENGTH;
            heights[row][column] = BOWL_DEPTH * (u * u + v * v) + noise[row][column];
        }
    }

    return heights;
}

void HeightFieldEnvironment::init()
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glm::vec3 cameraPosition(-20.0f, 8.0f, 0.0f);
    activeCamera->setPosition(cameraPosition);
    activeCamera->turnUp(-15.0f);

    Shader* defaultShader = createShader("basic_texture.vert", "basic_texture.frag");

    HeightFieldBuilder terrainBuilder(createTerrainHeights(), TERRAIN_WIDTH, TERRAIN_LENGTH);
    terrainBuilder.setMaterial({
        .diffuseColor = { 0.75f, 0.70f, 0.75f },
        .specularColor = glm::vec3(0.05f),
        .shininess = 4.0f });
    RigidEntity* terrain = createEntity<RigidEntity>(terrainBuilder, defaultShader, { .motionType = MotionType::Static, .friction = 0.8f });

    EllipsoidBuilder sphereBuilder(glm::vec3(0.5f));
    sphereBuilder.setMaterial({
        .diffuseColor = { 0.20f, 0.45f, 0.85f },
        .specularColor = glm::vec3(0.9f),
        .shininess = 128.0f });
    sphere = createEntity<DraggableEntity>(sphereBuilder, defaultShader, { .friction = 0.5f });
    sphere->setFeature(EntityFeature::Hoverable, true);
    sphere->setFeature(EntityFeature::Selectable, true);
    sphere->setFeature(EntityFeature::Holdable, true);
    resetSphere();

    Shader* outlineMaskShader = createShader("outline_mask.vert", "outline_mask.frag");
    Shader* outlineShader = createShader("outline.vert", "outline.frag");
    createRenderPass<OutlinePass>(outlineMaskShader, outlineShader,
        flagMask(EntityFlag::Hovered, EntityFlag::Selected), flagMask(EntityFlag::Held));
    OutlinePass* heldPass = createRenderPass<OutlinePass>(outlineMaskShader, outlineShader, flagMask(EntityFlag::Held));
    heldPass->setOutlineColor(glm::vec3(0.2f, 0.9f, 0.3f));

    physicsSystem.OptimizeBroadPhase();
}

void HeightFieldEnvironment::update(float dt)
{
    bool reset = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
    if (reset && !resetHeld) {
        resetSphere();
    }
    resetHeld = reset;

    physicsUpdate(dt);
}

void HeightFieldEnvironment::resetSphere()
{
    sphere->setPosition(SPHERE_START);
    sphere->setVelocity(glm::vec3(0.0f));
    sphere->setAngularVelocity(glm::vec3(0.0f));
}
