#include "dugl/modelling/renderable_builder.h"


Renderable RenderableBuilder::build()
{
    meshes.clear();
    generateMeshes();
    return Renderable(std::move(meshes));
}
