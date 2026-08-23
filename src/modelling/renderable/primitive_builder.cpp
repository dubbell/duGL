#include "dugl/modelling/primitive_builder.h"


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


PlaneBuilder::PlaneBuilder(float width, float depth, dugl::uint widthSegments, dugl::uint depthSegments)
    : width(width), depth(depth), widthSegments(widthSegments), depthSegments(depthSegments)
{}

void PlaneBuilder::generateMeshes()
{
    // TODO: generate the plane's vertices/indices and hand them to addMesh()
}


BoxBuilder::BoxBuilder(glm::vec3 dimensions) : dimensions(dimensions)
{}

void BoxBuilder::generateMeshes()
{
    // TODO: generate the box's vertices/indices and hand them to addMesh()
}


EllipsoidBuilder::EllipsoidBuilder(glm::vec3 radii, dugl::uint segments, dugl::uint rings)
    : radii(radii), segments(segments), rings(rings)
{}

void EllipsoidBuilder::generateMeshes()
{
    // TODO: generate the ellipsoid's vertices/indices and hand them to addMesh()
}


ConeBuilder::ConeBuilder(float radius, float height, dugl::uint segments)
    : radius(radius), height(height), segments(segments)
{}

void ConeBuilder::generateMeshes()
{
    // TODO: generate the cone's vertices/indices and hand them to addMesh()
}


PyramidBuilder::PyramidBuilder(float baseWidth, float baseDepth, float height)
    : baseWidth(baseWidth), baseDepth(baseDepth), height(height)
{}

void PyramidBuilder::generateMeshes()
{
    // TODO: generate the pyramid's vertices/indices and hand them to addMesh()
}
