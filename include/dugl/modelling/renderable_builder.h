#pragma once

#include <vector>

#include "renderable.h"
#include "mesh.h"


// Base for anything that produces a Renderable, regardless of where the geometry comes
// from (an imported model file, a procedurally generated primitive, ...). Subclasses only
// have to fill 'meshes' in generateMeshes(); build() takes care of the rest.
class RenderableBuilder
{
protected:
    std::vector<Mesh> meshes;

public:
    virtual ~RenderableBuilder() = default;

    // builders own OpenGL resources through their meshes; keep them non-copyable and let
    // the meshes move as a unit
    RenderableBuilder(const RenderableBuilder&) = delete;
    RenderableBuilder& operator=(const RenderableBuilder&) = delete;

    Renderable build();

protected:
    RenderableBuilder() = default;

    virtual void generateMeshes() = 0;
};
