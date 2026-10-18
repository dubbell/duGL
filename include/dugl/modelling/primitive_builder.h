#pragma once

#include <vector>

#include "dugl/shading/material.h"
#include "renderable_builder.h"
#include "vertex_layout.h"
#include "collider.h"
#include "entity.h"


// Base for builders that generate their geometry procedurally instead of loading it.
// A primitive defaults to a plain white material, so giving one a solid colour is just
// setting a material whose diffuseColor is that colour and which has no maps.
class PrimitiveBuilder : public RenderableBuilder
{
protected:
    Material material;

public:
    void setMaterial(Material material);

    /* Get the parameters of the collision geometry. */
    virtual Collider createCollider() const = 0;
    /* Get the ray hit detection function. */
    virtual RayHitTest createRayHitTest() const = 0;

protected:
    PrimitiveBuilder() = default;
    PrimitiveBuilder(Material material);

    // appends one mesh made of the given geometry, shaded with this builder's material
    void addMesh(std::vector<StaticVertex> vertices, std::vector<unsigned int> indices);
};


// A flat rectangle in the XZ plane, centred on the origin, optionally subdivided.
class PlaneBuilder : public PrimitiveBuilder
{
private:
    float width, depth;
    dugl::uint widthSegments, depthSegments;

public:
    PlaneBuilder(float width, float depth, dugl::uint widthSegments = 1, dugl::uint depthSegments = 1);

    Collider createCollider() const override;
    RayHitTest createRayHitTest() const override;

private:
    void generateMeshes() override;
};


// A rectangular block (cuboid) centred on the origin.
class BoxBuilder : public PrimitiveBuilder
{
private:
    glm::vec3 dimensions;

public:
    BoxBuilder(glm::vec3 dimensions);

    Collider createCollider() const override;
    RayHitTest createRayHitTest() const override;

private:
    void generateMeshes() override;
};


// An ellipsoid centred on the origin; equal radii give a sphere. Tessellated as a UV
// sphere: 'segments' subdivisions around the vertical axis, 'rings' from pole to pole.
class EllipsoidBuilder : public PrimitiveBuilder
{
private:
    glm::vec3 radii;
    dugl::uint segments, rings;

public:
    EllipsoidBuilder(glm::vec3 radii, dugl::uint segments = 32, dugl::uint rings = 16);

    Collider createCollider() const override;
    RayHitTest createRayHitTest() const override;

private:
    void generateMeshes() override;
};


// A cone with its base in the XZ plane and its apex on the positive Y axis.
class ConeBuilder : public PrimitiveBuilder
{
private:
    float radius, height;
    dugl::uint segments;

public:
    ConeBuilder(float radius, float height, dugl::uint segments = 32);

    Collider createCollider() const override;
    RayHitTest createRayHitTest() const override;

private:
    void generateMeshes() override;
};


// A pyramid with a rectangular base in the XZ plane and its apex on the positive Y axis.
class PyramidBuilder : public PrimitiveBuilder
{
private:
    float baseWidth, baseDepth, height;

public:
    PyramidBuilder(float baseWidth, float baseDepth, float height);

    Collider createCollider() const override;
    RayHitTest createRayHitTest() const override;

private:
    void generateMeshes() override;
};
