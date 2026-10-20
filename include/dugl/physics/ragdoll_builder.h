#pragma once

#include "dugl/modelling/primitive_builder.h"
#include "dugl/modelling/renderable_builder.h"
#include "dugl/physics/physics_material.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <numbers>
#include <optional>
#include <variant>
#include <vector>


struct HingeJoint
{
    glm::vec3 pivot{ 0.0f };
    glm::vec3 axis{ 0.0f, 0.0f, 1.0f };
    glm::vec3 normal{ 1.0f, 0.0f, 0.0f };
    float minAngle = -std::numbers::pi_v<float>;
    float maxAngle = std::numbers::pi_v<float>;
};

struct FixedJoint
{
    glm::vec3 pivot{ 0.0f };
};

/* Joint connecting a ragdoll part to its parent, defined in ragdoll space in the rest pose. */
using RagdollJoint = std::variant<HingeJoint, FixedJoint>;

struct RagdollPart
{
    PrimitiveBuilder* builder;
    PhysicsMaterial material;
    glm::vec3 position;
    glm::quat rotation;
    int parent;
    std::optional<RagdollJoint> joint;
};

/* Builds a ragdoll from primitive parts connected by joints. The part builders must outlive the builder. */
class RagdollBuilder : public RenderableBuilder
{
private:
    std::vector<RagdollPart> parts;

public:
    RagdollBuilder() = default;

    /* Add the root part, positioned in ragdoll space. */
    dugl::uint addRoot(PrimitiveBuilder& builder, const PhysicsMaterial& material, glm::vec3 position, glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    /* Add a part connected to an earlier part with a joint. */
    dugl::uint addPart(PrimitiveBuilder& builder, const PhysicsMaterial& material, glm::vec3 position, glm::quat rotation, dugl::uint parent, RagdollJoint joint);

    dugl::uint getPartCount() const;
    const std::vector<RagdollPart>& getParts() const;

    /* Get the transforms of the parts in ragdoll space in the rest pose. */
    std::vector<glm::mat4> getRestTransforms() const;
    /* Get the ray hit detection functions of the parts in their own spaces. */
    std::vector<LocalRayHitTest> createLocalRayHitTests() const;

private:
    void generateMeshes() override;
};
