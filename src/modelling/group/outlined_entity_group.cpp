#include "dugl/modelling/outlined_entity_group.h"
#include "dugl/utils/glad_include.h"
#include "dugl/common.h"

#include <algorithm>


OutlinedEntityGroup::OutlinedEntityGroup(Shader* shader, Shader* outlineShader)
    : shader(dugl::requireNonNull(shader)), outlineShader(dugl::requireNonNull(outlineShader)), outlineThickness(0.15f) {}


void OutlinedEntityGroup::render()
{
    for (HoverableEntity* entity : hoverableEntities)
    {
        if (!entity->isHovered()) 
        {
            shader->use();
            entity->render(shader);
        }
        else 
        {
            glEnable(GL_STENCIL_TEST);
            glClear(GL_STENCIL_BUFFER_BIT);

            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);        // keep value in stencil buffer or replace it on test result, for when
                                                              // stencil test fails, stencil test passes but depth test fails, and finally when both succeed
            glStencilFunc(GL_ALWAYS, 1, 0xFF);                // always write 1 to stencil where fragments are rendered
            glStencilMask(0xFF);                              // enable writing to the stencil buffer

            shader->use();
            entity->render(shader);                           // render objects with the normal object shader while also writing to stencil buffer

            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);              // stencil test passes if stencil buffer value not 1, i.e. not in position drawn previously
            glStencilMask(0x00);                              // disable writing to the stencil buffer

            outlineShader->use();
            outlineShader->setFloat("thickness", outlineThickness);
            entity->render(outlineShader);                    // draw outline (upscaled renderable)

            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glDisable(GL_STENCIL_TEST);
        }
    }
}

void OutlinedEntityGroup::addEntity(HoverableEntity* entity)
{
    dugl::requireNonNull(entity);
    hoverableEntities.push_back(entity);
}

void OutlinedEntityGroup::removeEntity(HoverableEntity* entity)
{
    hoverableEntities.erase(std::remove(hoverableEntities.begin(), hoverableEntities.end(), entity),
                            hoverableEntities.end());
}

void OutlinedEntityGroup::clear()
{
    hoverableEntities.clear();
}

const std::vector<HoverableEntity*>& OutlinedEntityGroup::getHoverableEntities() const
{
    return hoverableEntities;
}

void OutlinedEntityGroup::setShader(Shader* shader)
{
    this->shader = dugl::requireNonNull(shader);
}

void OutlinedEntityGroup::setOutlineShader(Shader* outlineShader)
{
    this->outlineShader = dugl::requireNonNull(outlineShader);
}

void OutlinedEntityGroup::setOutlineThickness(float outlineThickness)
{
    this->outlineThickness = outlineThickness;
}

float OutlinedEntityGroup::getOutlineThickness() const
{
    return outlineThickness;
}
