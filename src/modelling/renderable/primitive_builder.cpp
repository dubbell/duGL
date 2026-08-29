#include "dugl/modelling/primitive_builder.h"

#include <algorithm>
#include <cmath>
#include <numbers>


static const float PI = std::numbers::pi_v<float>;


PrimitiveBuilder::PrimitiveBuilder(Material material) : material(material)
{}

void PrimitiveBuilder::setMaterial(Material material)
{
    this->material = material;
}

void PrimitiveBuilder::addMesh(std::vector<StaticVertex> vertices, std::vector<unsigned int> indices)
{
    meshes.push_back(Mesh(std::move(vertices), std::move(indices), material));
}

static void addQuad(
    std::vector<StaticVertex>& vertices,
    std::vector<unsigned int>& indices,
    glm::vec3 center,
    glm::vec3 u,
    glm::vec3 v,
    float halfU,
    float halfV)
{
    glm::vec3 normal = glm::cross(u, v);
    unsigned int corner = (unsigned int)vertices.size();

    vertices.push_back({ center - u * halfU - v * halfV, normal, { 0.0f, 1.0f } });
    vertices.push_back({ center + u * halfU - v * halfV, normal, { 1.0f, 1.0f } });
    vertices.push_back({ center + u * halfU + v * halfV, normal, { 1.0f, 0.0f } });
    vertices.push_back({ center - u * halfU + v * halfV, normal, { 0.0f, 0.0f } });

    indices.insert(indices.end(), {
        corner, corner + 1, corner + 2,
        corner, corner + 2, corner + 3 });
}


PlaneBuilder::PlaneBuilder(float width, float depth, dugl::uint widthSegments, dugl::uint depthSegments)
    : width(width), depth(depth),
      widthSegments(std::max(widthSegments, 1u)), depthSegments(std::max(depthSegments, 1u))
{}

void PlaneBuilder::generateMeshes()
{
    std::vector<StaticVertex> vertices;
    std::vector<unsigned int> indices;

    for (dugl::uint row = 0; row <= depthSegments; row++)
    {
        for (dugl::uint column = 0; column <= widthSegments; column++)
        {
            float u = (float)column / widthSegments;
            float v = (float)row / depthSegments;

            vertices.push_back({
                { -width * 0.5f + u * width, 0.0f, -depth * 0.5f + v * depth },
                { 0.0f, 1.0f, 0.0f },
                { u, v }});
        }
    }

    dugl::uint rowStride = widthSegments + 1;

    for (dugl::uint row = 0; row < depthSegments; row++)
    {
        for (dugl::uint column = 0; column < widthSegments; column++)
        {
            unsigned int corner = row * rowStride + column;

            indices.insert(indices.end(), {
                corner, corner + rowStride, corner + 1,
                corner + 1, corner + rowStride, corner + rowStride + 1 });
        }
    }

    addMesh(std::move(vertices), std::move(indices));
}


BoxBuilder::BoxBuilder(glm::vec3 dimensions) : dimensions(dimensions)
{}

void BoxBuilder::generateMeshes()
{
    // the in-plane axes of each face, ordered so that u x v points out of the box
    static const glm::vec3 faceAxes[6][2] = {
        {{  0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f,  0.0f }},  // +X
        {{  0.0f, 0.0f,  1.0f }, { 0.0f, 1.0f,  0.0f }},  // -X
        {{  1.0f, 0.0f,  0.0f }, { 0.0f, 0.0f, -1.0f }},  // +Y
        {{  1.0f, 0.0f,  0.0f }, { 0.0f, 0.0f,  1.0f }},  // -Y
        {{  1.0f, 0.0f,  0.0f }, { 0.0f, 1.0f,  0.0f }},  // +Z
        {{ -1.0f, 0.0f,  0.0f }, { 0.0f, 1.0f,  0.0f }}}; // -Z

    std::vector<StaticVertex> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 halfExtents = dimensions * 0.5f;

    for (const glm::vec3* axes : faceAxes)
    {
        glm::vec3 u = axes[0], v = axes[1];
        glm::vec3 normal = glm::cross(u, v);

        // each axis is a signed unit axis, so projecting the half extents onto it gives the
        // face's extent along that axis, and its distance from the centre along the normal
        addQuad(vertices, indices,
            normal * glm::dot(glm::abs(normal), halfExtents),
            u, v,
            glm::dot(glm::abs(u), halfExtents),
            glm::dot(glm::abs(v), halfExtents));
    }

    addMesh(std::move(vertices), std::move(indices));
}


EllipsoidBuilder::EllipsoidBuilder(glm::vec3 radii, dugl::uint segments, dugl::uint rings)
    : radii(radii), segments(std::max(segments, 3u)), rings(std::max(rings, 2u))
{}

void EllipsoidBuilder::generateMeshes()
{
    std::vector<StaticVertex> vertices;
    std::vector<unsigned int> indices;

    for (dugl::uint ring = 0; ring <= rings; ring++)
    {
        float v = (float)ring / rings;
        float phi = PI * v;  // 0 at the north pole, PI at the south pole
        float sinPhi = std::sin(phi), cosPhi = std::cos(phi);

        for (dugl::uint segment = 0; segment <= segments; segment++)
        {
            float u = (float)segment / segments;
            float theta = 2.0f * PI * u;

            glm::vec3 position = {
                radii.x * sinPhi * std::cos(theta),
                radii.y * cosPhi,
                radii.z * sinPhi * std::sin(theta)};

            // Unless the radii are equal, an ellipsoid's normal is not the direction of its
            // position: it's the gradient of (x/a)^2 + (y/b)^2 + (z/c)^2, which divides the
            // position by the squared radii. For a sphere this reduces to the position.
            glm::vec3 normal = glm::normalize(position / (radii * radii));

            // the seam vertex is duplicated at u = 0 and u = 1 so the texture wraps around
            // rather than being squeezed back across the whole ellipsoid
            vertices.push_back({ position, normal, { u, v }});
        }
    }

    dugl::uint rowStride = segments + 1;

    for (dugl::uint ring = 0; ring < rings; ring++)
    {
        for (dugl::uint segment = 0; segment < segments; segment++)
        {
            unsigned int corner = ring * rowStride + segment;

            // every vertex of the top ring sits on the north pole, so the triangle that
            // would use two of them has no area; likewise at the south pole
            if (ring != 0)
            {
                indices.insert(indices.end(), { corner, corner + 1, corner + rowStride });
            }
            if (ring != rings - 1)
            {
                indices.insert(indices.end(), { corner + 1, corner + rowStride + 1, corner + rowStride });
            }
        }
    }

    addMesh(std::move(vertices), std::move(indices));
}


ConeBuilder::ConeBuilder(float radius, float height, dugl::uint segments)
    : radius(radius), height(height), segments(std::max(segments, 3u))
{}

void ConeBuilder::generateMeshes()
{
    std::vector<StaticVertex> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 apex = { 0.0f, height, 0.0f };

    auto rimPosition = [this](float theta) {
        return glm::vec3(radius * std::cos(theta), 0.0f, radius * std::sin(theta));
    };

    // the slanted surface leans outwards by the base radius and upwards by the height, so
    // its normal leans the same way with those two swapped
    auto slopeNormal = [this](float theta) {
        return glm::normalize(glm::vec3(height * std::cos(theta), radius, height * std::sin(theta)));
    };

    // The apex is a singularity: the normal there depends on which side you approach from.
    // Each segment therefore gets its own apex vertex, carrying the normal of that segment's
    // midpoint, which keeps the shading smooth all the way around.
    for (dugl::uint segment = 0; segment < segments; segment++)
    {
        float theta = 2.0f * PI * segment / segments;
        float nextTheta = 2.0f * PI * (segment + 1) / segments;

        unsigned int corner = (unsigned int)vertices.size();

        vertices.push_back({ rimPosition(theta), slopeNormal(theta),
            { (float)segment / segments, 1.0f }});
        vertices.push_back({ apex, slopeNormal((theta + nextTheta) * 0.5f),
            { ((float)segment + 0.5f) / segments, 0.0f }});
        vertices.push_back({ rimPosition(nextTheta), slopeNormal(nextTheta),
            { (float)(segment + 1) / segments, 1.0f }});

        indices.insert(indices.end(), { corner, corner + 1, corner + 2 });
    }

    // base cap, wound counter-clockwise as seen from below
    unsigned int center = (unsigned int)vertices.size();
    vertices.push_back({ { 0.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.5f, 0.5f }});

    for (dugl::uint segment = 0; segment <= segments; segment++)
    {
        float theta = 2.0f * PI * segment / segments;

        vertices.push_back({ rimPosition(theta), { 0.0f, -1.0f, 0.0f },
            { 0.5f + 0.5f * std::cos(theta), 0.5f + 0.5f * std::sin(theta) }});
    }

    for (dugl::uint segment = 0; segment < segments; segment++)
    {
        indices.insert(indices.end(), { center, center + 1 + segment, center + 2 + segment });
    }

    addMesh(std::move(vertices), std::move(indices));
}


PyramidBuilder::PyramidBuilder(float baseWidth, float baseDepth, float height)
    : baseWidth(baseWidth), baseDepth(baseDepth), height(height)
{}

void PyramidBuilder::generateMeshes()
{
    std::vector<StaticVertex> vertices;
    std::vector<unsigned int> indices;

    float halfWidth = baseWidth * 0.5f, halfDepth = baseDepth * 0.5f;

    glm::vec3 corners[4] = {
        { -halfWidth, 0.0f,  halfDepth },
        {  halfWidth, 0.0f,  halfDepth },
        {  halfWidth, 0.0f, -halfDepth },
        { -halfWidth, 0.0f, -halfDepth }};
    glm::vec3 apex = { 0.0f, height, 0.0f };

    // base, wound counter-clockwise as seen from below
    for (const glm::vec3& corner : corners)
    {
        vertices.push_back({ corner, { 0.0f, -1.0f, 0.0f },
            { corner.x / baseWidth + 0.5f, corner.z / baseDepth + 0.5f }});
    }
    indices.insert(indices.end(), { 0, 3, 2, 0, 2, 1 });

    // the four sides, each with its own copy of its corners so it can carry a flat normal
    for (dugl::uint side = 0; side < 4; side++)
    {
        glm::vec3 left = corners[side], right = corners[(side + 1) % 4];
        glm::vec3 normal = glm::normalize(glm::cross(right - left, apex - left));

        unsigned int corner = (unsigned int)vertices.size();

        vertices.push_back({ left,  normal, { 0.0f, 1.0f }});
        vertices.push_back({ right, normal, { 1.0f, 1.0f }});
        vertices.push_back({ apex,  normal, { 0.5f, 0.0f }});

        indices.insert(indices.end(), { corner, corner + 1, corner + 2 });
    }

    addMesh(std::move(vertices), std::move(indices));
}
