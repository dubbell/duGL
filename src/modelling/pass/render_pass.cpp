#include "dugl/modelling/render_pass.h"

#include <algorithm>


void RenderPass::renderSurfaces(std::span<Entity* const> entities)
{
    sortedEntities.assign(entities.begin(), entities.end());
    std::stable_sort(sortedEntities.begin(), sortedEntities.end(), [](const Entity* a, const Entity* b) {
        return a->getShader()->ID < b->getShader()->ID;
    });

    Shader* activeShader = nullptr;
    for (Entity* entity : sortedEntities)
    {
        Shader* shader = entity->getShader();
        if (shader != activeShader) {
            shader->use();
            activeShader = shader;
        }
        entity->render(shader);
    }

    sortedEntities.clear();
}
