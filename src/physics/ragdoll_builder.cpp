#include "dugl/physics/ragdoll_builder.h"

#include <glm/gtc/matrix_transform.hpp>

#include <format>
#include <stdexcept>


dugl::uint RagdollBuilder::addRoot(PrimitiveBuilder& builder, const PhysicsMaterial& material, glm::vec3 position, glm::quat rotation)
{
    if (!parts.empty()) {
        throw std::invalid_argument("A ragdoll can only have one root part.");
    }

    parts.push_back({ &builder, material, position, rotation, -1, std::nullopt });
    renderable = nullptr;
    return 0;
}

dugl::uint RagdollBuilder::addPart(PrimitiveBuilder& builder, const PhysicsMaterial& material, glm::vec3 position, glm::quat rotation, dugl::uint parent, RagdollJoint joint)
{
    if (parts.empty()) {
        throw std::invalid_argument("A ragdoll needs a root part before other parts are added.");
    }
    if (parent >= parts.size()) {
        throw std::invalid_argument(std::format("Ragdoll part parent {} does not exist.", parent));
    }
    if (parts.size() >= MAX_BONES) {
        throw std::invalid_argument(std::format("A ragdoll can have at most {} parts.", MAX_BONES));
    }

    parts.push_back({ &builder, material, position, rotation, (int)parent, joint });
    renderable = nullptr;
    return (dugl::uint)parts.size() - 1;
}

dugl::uint RagdollBuilder::getPartCount() const
{
    return (dugl::uint)parts.size();
}

const std::vector<RagdollPart>& RagdollBuilder::getParts() const
{
    return parts;
}

std::vector<glm::mat4> RagdollBuilder::getRestTransforms() const
{
    std::vector<glm::mat4> transforms;
    transforms.reserve(parts.size());
    for (const RagdollPart& part : parts) {
        transforms.push_back(glm::translate(glm::mat4(1.0f), part.position) * glm::mat4_cast(part.rotation));
    }
    return transforms;
}

std::vector<LocalRayHitTest> RagdollBuilder::createLocalRayHitTests() const
{
    std::vector<LocalRayHitTest> hitTests;
    hitTests.reserve(parts.size());
    for (const RagdollPart& part : parts) {
        hitTests.push_back(part.builder->createLocalRayHitTest());
    }
    return hitTests;
}

void RagdollBuilder::generateMeshes()
{
    std::vector<glm::mat4> restTransforms = getRestTransforms();

    for (size_t i = 0; i < parts.size(); i++)
    {
        const RagdollPart& part = parts[i];
        glm::mat3 normalTransform = glm::mat3_cast(part.rotation);

        for (const MeshGeometry& geometry : part.builder->createGeometry())
        {
            std::vector<SkinnedVertex> vertices;
            vertices.reserve(geometry.vertices.size());
            for (const StaticVertex& vertex : geometry.vertices)
            {
                vertices.push_back({
                    glm::vec3(restTransforms[i] * glm::vec4(vertex.position, 1.0f)),
                    normalTransform * vertex.normal,
                    vertex.texCoord,
                    glm::uvec4((dugl::uint)i, 0, 0, 0),
                    glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) });
            }
            meshes.push_back(Mesh(vertices, geometry.indices, part.builder->getMaterial()));
        }
    }
}
