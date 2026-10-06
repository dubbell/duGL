#pragma once

#include <span>
#include <vector>

#include "entity.h"


class RenderPass
{
private:
    std::vector<Entity*> sortedEntities;

public:
    virtual ~RenderPass() = default;

    virtual bool accepts(const Entity& entity) const = 0;
    virtual void render(std::span<Entity* const> entities) = 0;

protected:
    void renderSurfaces(std::span<Entity* const> entities);
};
