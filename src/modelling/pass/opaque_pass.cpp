#include "dugl/modelling/opaque_pass.h"


bool OpaquePass::accepts(const Entity& entity) const
{
    return true;
}

void OpaquePass::render(std::span<Entity* const> entities)
{
    renderSurfaces(entities);
}
