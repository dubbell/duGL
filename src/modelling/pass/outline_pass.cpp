#include "dugl/modelling/outline_pass.h"
#include "dugl/utils/glad_include.h"
#include "dugl/common.h"


OutlinePass::OutlinePass(Shader* outlineShader, std::uint64_t acceptedFlags)
    : outlineShader(dugl::requireNonNull(outlineShader)), acceptedFlags(acceptedFlags), outlineThickness(0.15f) {}


bool OutlinePass::accepts(const Entity& entity) const
{
    return entity.hasAnyFlag(acceptedFlags);
}

void OutlinePass::render(std::span<Entity* const> entities)
{
    if (entities.empty()) {
        return;
    }

    glEnable(GL_STENCIL_TEST);
    glStencilMask(0xFF);
    glClear(GL_STENCIL_BUFFER_BIT);

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);

    renderSurfaces(entities);

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);

    outlineShader->use();
    outlineShader->setFloat("thickness", outlineThickness);
    for (Entity* entity : entities) {
        entity->render(outlineShader);
    }

    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glDisable(GL_STENCIL_TEST);
}

void OutlinePass::setOutlineShader(Shader* outlineShader)
{
    this->outlineShader = dugl::requireNonNull(outlineShader);
}

void OutlinePass::setOutlineThickness(float outlineThickness)
{
    this->outlineThickness = outlineThickness;
}

float OutlinePass::getOutlineThickness() const
{
    return outlineThickness;
}
